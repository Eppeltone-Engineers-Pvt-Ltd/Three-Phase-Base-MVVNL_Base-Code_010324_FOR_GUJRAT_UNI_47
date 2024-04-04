// tampers.c
/*
// This file has different peristence times for different tampers
*/

#include "r_cg_macrodriver.h"

#include <stdio.h>

#include "typedef.h"            // located right at top of tree below iodefine.h
#include "r_cg_wdt.h"
#include "em_core.h"
#include "em_adapter.h"
#include "r_lcd_display.h"      /* LCD display header file */
#include "em_display.h"  
#include "emeter3_structs.h"
#include "memoryOps.h"
#include "emeter3_app.h"
#include "eeprom.h"
#include "e2rom.h"	// pravak implementation
#include "pravakComm.h"
#include "utilities.h"
#include "tampers.h"
#include "wrp_mcu.h"
#include "trigFunctions.h"
#include "FrequencyDisturbance.h"

#define RECORD_DELAY 15

#define	MISSING_POT_PERSISTENCE_ON	900
#define	CT_OPEN_PERSISTENCE_ON	900
#define	VOLT_UNBALANCE_PERSISTENCE_ON	900
#define	CURRENT_UNBALANCE_PERSISTENCE_ON	900


/*
#define HALLSENSOR1	(P12.5)	
#define HALLSENSOR2	(P5.4)
#define HALLSENSOR3	(P4.3)	
#define HALLSENSOR4	(P5.0)
*/ 
#ifdef ND_POWER_ON_ACTUALS
int16_t vR_saved;
int16_t vY_saved;
int16_t vB_saved;
#endif

int16_t seedValue_R=45;
int16_t seedValue_Y=45;
int16_t seedValue_B=45;
float32_t operatingPF;

// persistence counters for all phases
uint16_t missingPotentialCtr[3];
uint16_t ctReverseCtr[3];
uint16_t ctOpenCtr[3];

uint16_t fault_code;	// redefined on 22/June/2020 for TNEB - use for GUJARAT too

// common counters across phases
uint16_t overVoltageCtr;
uint16_t lowVoltageCtr;
uint16_t voltageUnbalanceCtr;
uint16_t currentUnbalanceCtr;
uint16_t ctBypassCtr;
//uint16_t overCurrentCtr_R;
//uint16_t overCurrentCtr_Y;
//uint16_t overCurrentCtr_B;
uint16_t magnetCtr;
uint16_t acMagnetCtr;
uint16_t neutralDisturbanceCtr;
//uint16_t lowPFCtr;
int16_t coverOpenCtr;	// we are allowing this to go negative also to account for debounce

uint8_t g_TamperStatus;
uint8_t acMagnetSecondsCtr;	// created on 02/Sep/2020

int8_t freqDisturbed;

// powerfail, transactions, do not need counters

//int16_t findMax(int16_t i1, int16_t i2, int16_t i3);
//int16_t findMin(int16_t i1, int16_t i2, int16_t i3);

uint8_t countVoltageLT50(int16_t vr, int16_t vy, int16_t vb);
uint8_t countVoltageGT300(int16_t vr, int16_t vy, int16_t vb);
// The following 2 functions are replaced by the third one
//uint8_t countVoltageGT170(float32_t vr, float32_t vy, float32_t vb);
//uint8_t countVoltageLT300(float32_t vr, float32_t vy, float32_t vb);
uint8_t determineTwoWireTamper(int16_t iR, int16_t iY, int16_t iB);
int8_t checkTwoWireTamper(int16_t pf1,int16_t pf2);

void clearPersistenceCounters(void)
{
	missingPotentialCtr[0]=0;
	missingPotentialCtr[1]=0;
	missingPotentialCtr[2]=0;
	
	ctReverseCtr[0]=0;
	ctReverseCtr[1]=0;
	ctReverseCtr[2]=0;
	
	ctOpenCtr[0]=0;
	ctOpenCtr[1]=0;
	ctOpenCtr[2]=0;

	overVoltageCtr=0;
	lowVoltageCtr=0;
	voltageUnbalanceCtr=0;
	currentUnbalanceCtr=0;
	ctBypassCtr=0;
	magnetCtr=0;
	acMagnetCtr=0;
	neutralDisturbanceCtr=0;
//	coverOpenCtr=0;
}


void record_transaction_event(uint32_t flagmask)
{
	readRTC();
	mytmprdata.tamperEvent_flags |= flagmask;
	mytmprdata.tamperEvent_image |= flagmask;
	mytmprdata.tamperEvent_on|=flagmask;
	e2write_flags |= E2_W_EVENTDATA;
	wre2rom(); // write the tamper here itself
}

int8_t record_power_fail_event(void)
{// return value : 0 - failure, 1 - success
// before recording the power fail event it is important to ensure that power has returned properly
// and the values from the energy meter of at least one voltage is > 50
// If this does not happen within a few seconds , we must abort the recording of the power fail event

	int16_t VR;
	int16_t VY;
	int16_t VB;
	
	uint8_t counter=0;
  do
	{
    R_WDT_Restart();
  	MCU_Delay(5000);	// this function generates delay of 5 ms
		counter++;
	}
	while(counter<20);
	
  VR = (int16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_R)*10.0);
  VY = (int16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*10.0);
  VB = (int16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_B)*10.0);
	
	if((VR<500)&&(VY<500)&&(VB<500))
		return 0;	// failed

// THIS is the proper one
// Our treatment is not correct - if power failure is the event then the occurrence
// represents the power fail event and the restoration represents the return of power
// hence the image and on flags must be 1 for the power fail event and 0 for the restoration

// this is the power fail event
  mytmprdata.tamperEvent_flags |= POWER_EVENT_MASK;
  mytmprdata.tamperEvent_image &= ~POWER_EVENT_MASK;
  mytmprdata.tamperEvent_on&=~POWER_EVENT_MASK;
  e2write_flags |= E2_W_EVENTDATA;
	do
	{
  	wre2rom(); // write the tamper here itself
    R_WDT_Restart();
	}
	while((e2write_flags&E2_W_EVENTDATA)!=0);

// this is the power restored event	
//	readRTC();	// read the current time
	mytmprdata.tamperEvent_flags |= POWER_EVENT_MASK;
  mytmprdata.tamperEvent_image |= POWER_EVENT_MASK;
  mytmprdata.tamperEvent_on|=POWER_EVENT_MASK;
  e2write_flags |= E2_W_EVENTDATA;
//  wre2rom(); // write the tamper here itself
//  LCD_DisplayDigit(2, 2);
	do
	{
  	wre2rom(); // write the tamper here itself
    R_WDT_Restart();
	}
	while((e2write_flags&E2_W_EVENTDATA)!=0);



	Common_Data.noOfPowerFailures++;
  e2write_flags |= E2_W_IMPDATA;	// save the number of Power failure count
  wre2rom();
	return 1;	// success
}

void set_tamperEvent_image(uint32_t flagmask)
{
#ifdef LAST_TAMPER_DETAILS_REQUIRED		
	tmprdetails_t earlierTamperDetails;	
#endif	
  readRTC();  // reading the RTC now is important so that the correct time gets recorded - 09/Apr/2011
  mytmprdata.tamperEvent_flags |= flagmask; // this is to be done for all
  mytmprdata.tamperEvent_image |= flagmask;
	
  if((flagmask==RTC_TRANSACTION_MASK)||(flagmask==MD_INTERVAL_TRANSACTION_MASK)||(flagmask==SURVEY_INTERVAL_TRANSACTION_MASK)||(flagmask==SINGLE_ACTION_TRANSACTION_MASK)||(flagmask==ACTIVITY_CALENDAR_TRANSACTION_MASK)||(flagmask==POWER_EVENT_MASK)||(flagmask==MODE_UNI_MASK)||(flagmask==MODE_BIDIR_MASK))
  {// do nothing
  }
  else
  {// tampers to be counted only if event is not settings changed or event is not power event
    mytmprdata.tamperCount++;
    mymddata.tamperCount++;
    mytmprdata.tamperEvent_on |= flagmask;
    mytmprdata.tamperEvent_once |= flagmask; // this will only store the bit position
		
#ifdef LAST_TAMPER_DETAILS_REQUIRED		
// in the general case before recording the latest tamper, we should saved the data
// shift the latest tamper to 2nd latest
#if defined(JVVNL_UNI_DIR_METER)
	if((flagmask==computeMask(MAGNET))||(flagmask==computeMask(AC_MAGNET)))
	{
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
			EPR_Write(SECOND_LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
			
	    mytmprdata.latestTamperCode = findWhichBitIsOne(flagmask);
			mytmprdata.latestTamperDateTime=rtc;

	// now write the latest tamper details		
			earlierTamperDetails.tamperTime = mytmprdata.latestTamperDateTime;
			earlierTamperDetails.tamperCode = mytmprdata.latestTamperCode; 
			EPR_Write(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
	}
	#else
		EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
		EPR_Write(SECOND_LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
		
    mytmprdata.latestTamperCode = findWhichBitIsOne(flagmask);
		mytmprdata.latestTamperDateTime=rtc;

// now write the latest tamper details		
		earlierTamperDetails.tamperTime = mytmprdata.latestTamperDateTime;
		earlierTamperDetails.tamperCode = mytmprdata.latestTamperCode; 
		EPR_Write(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
	#endif		
#else		
    mytmprdata.latestTamperCode = findWhichBitIsOne(flagmask);
		mytmprdata.latestTamperDateTime=rtc;
#endif		
// Count the individual tampers - start

		switch(mytmprdata.latestTamperCode)
		{
			case 0:
				break;
				
			case 1:	//	MISSING_POTENTIAL_R
				mytmprdata.voltageFailureCount_R++;
				break;
			
			case 2:	//	MISSING_POTENTIAL_Y
				mytmprdata.voltageFailureCount_Y++;
				break;
				
			case 3:	//	MISSING_POTENTIAL_B
				mytmprdata.voltageFailureCount_B++;
				break;
				
			case 4:	//	OVER_VOLTAGE
//#ifdef BIDIR_METER			
				mytmprdata.highVoltageCount++;
//#endif				
				break;
			
			case 5:	//	UNDER_VOLTAGE
//#ifdef BIDIR_METER			
				mytmprdata.lowVoltageCount++;
//#endif				
				break;
			
			case 6:	//	VOLTAGE_UNBALANCED
				mytmprdata.voltageUnbalanceCount++;
				break;

#ifdef BIDIR_METER
			case 7:	//	CT_REVERSED_R
			case 8:	//	CT_REVERSED_Y
			case 9:	//	CT_REVERSED_B
				break;
#else
			case 7:	//	CT_REVERSED_R
				mytmprdata.currentReversalCount_R++;	// these must be individual counts
				break;
				
			case 8:	//	CT_REVERSED_Y
				mytmprdata.currentReversalCount_Y++;	// these must be individual counts
				break;
				
			case 9:	//	CT_REVERSED_B
				mytmprdata.currentReversalCount_B++;	// these must be individual counts
				break;
#endif			
			case 10:	//	CT_OPEN_R
				mytmprdata.currentFailureCount_R++;
				break;
			case 11:	//	CT_OPEN_Y
				mytmprdata.currentFailureCount_Y++;
				break;
			case 12:	//	CT_OPEN_B
				mytmprdata.currentFailureCount_B++;
				break;

			case 13:	//	CURRENT_UNBALANCED
				mytmprdata.currentUnbalanceCount++;
				break;
				
			case 14:	//	CT_BYPASS
//#ifndef BIDIR_METER			
				mytmprdata.currentBypassCount++;
//#endif				
				break;
				
			case 15:	//	OVER_CURRENT
			case 16:	//	POWER_EVENT
			case 17:	//	RTC_TRANSACTION
			case 18:	//	MD_INTERVAL_TRANSACTION
			case 19:	//	SURVEY_INTERVAL_TRANSACTION
			case 20:	//	SINGLE_ACTION_TRANSACTION
			case 21:	//	ACTIVITY_CALENDAR_TRANSACTION
			case 25:	//	LOW_PF
			case 26:	//	COVER_OPEN
			case 27:	//	MODE_UNIDIRECTIONAL_SET
			case 28:	//	MODE_BIDIRECTIONAL_SET
				break;

			case 22:	//	MAGNET
			case 23:	//	AC_MAGNET
				mytmprdata.magnetTamperCount++;
				break;
			case 24:	//	NEUTRAL_DISTURBANCE
				mytmprdata.neutralDisturbanceCount++;
				break;
			
			case 29:	//	OVER_CURRENT_Y
			case 30:	//	OVER_CURRENT_B
			
			default:
				break;
		}
// Count the individual tampers - end
  }
	
  e2write_flags |= E2_W_EVENTDATA;
//  while((moreflags&MS50FLG)==0);
  wre2rom(); 

// why is MD data being written here?
// This is because mymddata.tamperCount has been incremented above and needs to be saved
  write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
  e2write_flags&=~E2_W_CURRENTDATA;
}

void clear_tamperEvent_image(uint32_t flagmask)
{
	uint8_t eventCodeBit; 
	uint8_t recordEvent;
	
  readRTC();  // reading the RTC now is important so that the correct time gets recorded - 09/Apr/2011
  mytmprdata.tamperEvent_image &= ~flagmask;
  mytmprdata.tamperEvent_flags |= flagmask;
  mytmprdata.tamperEvent_on &= ~flagmask;

// For Tamil Nadu, the last tamper than has been restored is required	
	eventCodeBit = findWhichBitIsOne(flagmask);
	
#ifdef TN3P_BOARD	
	recordEvent=0;
	switch(eventCodeBit)
	{
		case 0:
			recordEvent=0;
			break;
			
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			recordEvent=1;
			break;
			
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
			recordEvent=0;
			break;
			
		case 22:
		case 23:
		case 24:
			recordEvent=1;
			break;
		
		case 25:
		case 26:
		case 27:
		case 28:
			recordEvent=0;
			break;

		case 29:
		case 30:
			recordEvent=1;
			break;

		default:
			recordEvent=0;
			break;
	}
	
	if(recordEvent!=0)
	{// only restored tamper event is to be recorded - not others such as transaction, power fail etc.
	  mytmprdata.latestTamperCode = eventCodeBit;
		mytmprdata.latestTamperDateTime=rtc;
	}
#endif		
	
#ifdef ND_POWER_ON_ACTUALS
	if(eventCodeBit==24)
	{// when the neutral disturbance tamper is restored then the peak values used for power calculations must be reset
		vR_saved=0;
		vY_saved=0;
		vB_saved=0;
	}
#endif

  e2write_flags |= E2_W_EVENTDATA;
//  while((moreflags&MS50FLG)==0);
  wre2rom();
}

// TNEB wants the indicators to be turned on, 15 seconds after the onset of tamper and turned off when tamper is restored
void check_setTamper(uint16_t *ptr, uint32_t flagmask , uint16_t persistence_value, uint8_t flagBit, uint8_t onDelay)
{
  if((mytmprdata.tamperEvent_image&flagmask) == 0)
  {//event occurence has not yet registered
    *ptr=*ptr+1;
		
		if(*ptr==2)
		{
			recordInstantaneousDateTimeEnergy(flagmask);
		}
		
		if(*ptr == onDelay)
		{// here exact interval has been reached
			recordInstantaneousData(flagmask);
		}
		
		if(flagBit<16)
			fault_code|=_BV(flagBit);
		
    if(*ptr >= persistence_value)
    {// record the tamper
      *ptr = 0;
      set_tamperEvent_image(flagmask);
    }
  }
  else
    *ptr = 0;
}

void check_clearTamper(uint16_t *ptr, uint32_t flagmask, uint16_t persistence_value, uint8_t flagBit, uint8_t onDelay)
{
  if((mytmprdata.tamperEvent_image&flagmask)!= 0)
  {//event has gone
    *ptr=*ptr+1;
		
		if(*ptr==2)
		{
			recordInstantaneousDateTimeEnergy(flagmask);
		}

		if(*ptr == onDelay)
		{// here exact interval has been reached
			recordInstantaneousData(flagmask);
		}
		
    if(*ptr >= persistence_value)
    {// record the tamper
      *ptr = 0;
      clear_tamperEvent_image(flagmask);
			if(flagBit<16)
				fault_code&=~_BV(flagBit);
    }
  }
  else
	{
    *ptr = 0;
		if(flagBit<16)
			fault_code&=~_BV(flagBit);
	}
}

int16_t findMax(int16_t i1, int16_t i2, int16_t i3)
{
	int16_t result;
	
	if(i1>=i2)
	{
		if(i1>=i3)
			result=i1;
		else
			result=i3;	
	}
	else
	{
		if(i2>=i3)
			result=i2;
		else
			result=i3;	
	}
	return result;
}

int16_t findMin(int16_t i1, int16_t i2, int16_t i3)
{
	int16_t result;
	
	if(i1<=i2)
	{
		if(i1<=i3)
			result=i1;
		else
			result=i3;	
	}
	else
	{
		if(i2<=i3)
			result=i2;
		else
			result=i3;	
	}
	return result;
}

uint8_t countCurrentGT95(int16_t ir, int16_t iy, int16_t ib)
{
	uint8_t retval=0;	
	
	if(ir>95)
	{
		retval++;
	}
		
	if(iy>95)
	{
		retval++;
	}

	if(ib>95)
	{
		retval++;
	}
	return retval;
}

uint8_t countVoltageGT276LT300(int16_t vr, int16_t vy, int16_t vb)
{
	uint8_t retval=0;	
	
	if((vr<3000)&&(vr>2760))
	{
		retval++;
	}
		
	if((vy<3000)&&(vy>2760))
	{
		retval++;
	}

	if((vb<3000)&&(vb>2760))
	{
		retval++;
	}
	return retval;
}

uint8_t countVoltageGT180LT276(int16_t vr, int16_t vy, int16_t vb)
{// returns the count of number of phases < 264 and > 180
	uint8_t retval=0;	
	
	if((vr<2760)&&(vr>1790))
	{
		retval++;
	}
		
	if((vy<2760)&&(vy>1790))
	{
		retval++;
	}

	if((vb<2760)&&(vb>1790))
	{
		retval++;
	}
	return retval;
// retval contains the count of the number of phases which are between 170 and 300	
}

uint8_t countVoltageGT144LT276(int16_t vr, int16_t vy, int16_t vb)
{// returns the count of number of phases < 264 and > 180
	uint8_t retval=0;	
	
	if((vr<2760)&&(vr>1430))
	{
		retval++;
	}
		
	if((vy<2760)&&(vy>1430))
	{
		retval++;
	}

	if((vb<2760)&&(vb>1430))
	{
		retval++;
	}
	return retval;
// retval contains the count of the number of phases which are between 170 and 300	
}

uint8_t countVoltageLT50(int16_t vr, int16_t vy, int16_t vb)
{// returns the count of number of phases < 50
	uint8_t retval=0;	
	
	if(vr<500)
	{
		retval++;
	}
		
	if(vy<500)
	{
		retval++;
	}

	if(vb<500)
	{
		retval++;
	}
	return retval;
// retval contains the count of the number of phases which are less than 50 Volts	
}

uint8_t countVoltageGT300(int16_t vr, int16_t vy, int16_t vb)
{// returns count of number of phases GT 300
	uint8_t retval=0;	// false

	if(vr>3000)
	{
		retval++;
	}
	
	if(vy>3000)
	{
		retval++;
	}
	
	if(vb>3000)
	{
		retval++;
	}
	
	return retval;
//	if(retval>=2)
//		return 1;	
//	else
//		return 0;
}

void performTamperProcessing(void){
  int16_t i[EM_NUM_OF_LINE];	// two decimals 
  int16_t v[EM_NUM_OF_PHASE];	// one decimal
  int16_t ivector;
	int16_t	ineutralDiff;
	int16_t maxValue;
	int16_t minValue;
  EM_LINE         phase_line;
	int32_t	temp_power;
//	float32_t tempFloat;

	uint8_t	misspotoccurencectr;
	uint8_t	currentreversectr;
	uint8_t	ct_open_ctr;
	uint8_t phasesGT300; // this variable holds 1 if true and 0 if false
	uint8_t phasesLT50;	// this variable returns the count of number of phases < 50 V 
	uint8_t phasesGT180LT276;
	uint8_t phasesGT276LT300;
	uint8_t currentsGT95;
	uint8_t phasesGT144LT276;
	
//	int8_t freqDisturbed;
	
  i[LINE_NEUTRAL] = (int16_t)(EM_GetCurrentRMS(LINE_NEUTRAL)*100);
  ivector = (int16_t)(EM_GetPhaseCurrentSumVector()*100);

	ineutralDiff = ABS(ivector) - ABS(i[LINE_NEUTRAL]);
	
	misspotoccurencectr=0;
	currentreversectr=0;
	ct_open_ctr=0;

//-----------------------------------------------------------------------------------------	
  // Check missing potential condition 
	v[LINE_PHASE_R] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*10);
	v[LINE_PHASE_Y] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*10);
	v[LINE_PHASE_B] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*10);
	
  i[LINE_PHASE_R] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_R)*100);
	if(i[LINE_PHASE_R]<0)
		i[LINE_PHASE_R]*=-1;
		
  i[LINE_PHASE_Y] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_Y)*100);
	if(i[LINE_PHASE_Y]<0)
		i[LINE_PHASE_Y]*=-1;
		
  i[LINE_PHASE_B] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_B)*100);
	if(i[LINE_PHASE_B]<0)
		i[LINE_PHASE_B]*=-1;
		
//	fault_code=0;	// not be made 0 for TNEB
	maxValue = findMax(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);
	minValue = findMin(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);

	phasesGT300 = countVoltageGT300(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);
	phasesLT50 = countVoltageLT50(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);	// returns the count of phases < 50
	phasesGT180LT276 = countVoltageGT180LT276(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);	// returns phases between 170 and 300
	phasesGT276LT300 = countVoltageGT276LT300(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);
	currentsGT95 = countCurrentGT95(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
	phasesGT144LT276 = countVoltageGT144LT276(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);
	
//-----------------------------------------------------------------------------------------	
// Voltage Missing - to be checked first
	if(phasesGT300<2)
	{/* no two voltages are greater than 350 - one might be but not two of them*/
		if ((v[LINE_PHASE_R] < 960)&&(i[LINE_PHASE_R]>95))
		{
	    check_setTamper(&missingPotentialCtr[LINE_PHASE_R], computeMask(MISSING_POTENTIAL_R),900, 0, RECORD_DELAY);
			misspotoccurencectr++;
		}
		
		if (v[LINE_PHASE_R] > 1790)
		{
	    check_clearTamper(&missingPotentialCtr[LINE_PHASE_R], computeMask(MISSING_POTENTIAL_R),TAMPER_PERSISTENCE_OFF, 0, RECORD_DELAY);
		}

		if ((v[LINE_PHASE_Y] < 960)&&(i[LINE_PHASE_Y]>95))
		{
	    check_setTamper(&missingPotentialCtr[LINE_PHASE_Y], computeMask(MISSING_POTENTIAL_Y),900, 1, RECORD_DELAY);
			misspotoccurencectr++;
		}
		
		if (v[LINE_PHASE_Y] > 1790)
		{
	    check_clearTamper(&missingPotentialCtr[LINE_PHASE_Y], computeMask(MISSING_POTENTIAL_Y),TAMPER_PERSISTENCE_OFF, 1, RECORD_DELAY);
		}
		
		if ((v[LINE_PHASE_B] < 960)&&(i[LINE_PHASE_B]>95))
		{
	    check_setTamper(&missingPotentialCtr[LINE_PHASE_B], computeMask(MISSING_POTENTIAL_B),900,2,RECORD_DELAY);
			misspotoccurencectr++;
		}
		
		if (v[LINE_PHASE_B] > 1790)
		{
	    check_clearTamper(&missingPotentialCtr[LINE_PHASE_B], computeMask(MISSING_POTENTIAL_B),TAMPER_PERSISTENCE_OFF,2, RECORD_DELAY);
		}
		
		if(misspotoccurencectr==0)
			g_TamperStatus&=~MISSPOT_MASK;
		else
			g_TamperStatus|=MISSPOT_MASK;

	}
	else
	{
		missingPotentialCtr[LINE_PHASE_R]=0;
		missingPotentialCtr[LINE_PHASE_Y]=0;
		missingPotentialCtr[LINE_PHASE_B]=0;
		misspotoccurencectr=0;
		g_TamperStatus&=~MISSPOT_MASK;
	}
//-----------------------------------------------------------------------------------------	
	
// to determine chopped ac injection in neutral call the following function	
#ifdef FREQ_DIST_CHECK	

	if((phasesGT300==2)&&(currentsGT95==3))
		freqDisturbed = checkFrequencyDisturbance();
	else
	{
		freqDisturbed=0;
		clearFrequencyArray();
	}
#else
	freqDisturbed=0;
	clearFrequencyArray();
#endif

// after pot missing - check neutral disturbance - fault_code = 9	
	if(((phasesGT300==2)&&(phasesLT50==1)&&(currentsGT95>=2))||(freqDisturbed==1))
	{
    check_setTamper(&neutralDisturbanceCtr, computeMask(NEUTRAL_DISTURBANCE),TAMPER_PERSISTENCE_ON,9,RECORD_DELAY);
#ifdef ND_POWER_ON_ACTUALS		
		if(vR_saved<v[LINE_PHASE_R])
			vR_saved=v[LINE_PHASE_R];
		if(vY_saved<v[LINE_PHASE_Y])
			vY_saved=v[LINE_PHASE_Y];
		if(vB_saved<v[LINE_PHASE_B])
			vB_saved=v[LINE_PHASE_B];
#endif			
	}
	else
	{// here phasesGT300 can be 0,1, or 3(Really?)
		if(phasesGT180LT276==3)
		{
    	check_clearTamper(&neutralDisturbanceCtr, computeMask(NEUTRAL_DISTURBANCE),TAMPER_PERSISTENCE_OFF,9, RECORD_DELAY);
			twoWireOperation=0;	// this needs to be uncommented - in the code of 25/Jun/2020 this line was not there
		}
		else
		{// here phasesGT180LT276 can be 0(Nah), 1, or 2
			if((phasesGT180LT276==2)&&(phasesLT50==1))
			{// it may be potential missing or two wire tamper
				if((g_TamperStatus&MISSPOT_MASK)==0)
					twoWireOperation = determineTwoWireTamper(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);	// function returns 0(no tamper) or 1(tamper)
				else
					twoWireOperation=0;
			}
		}
	}
	
//-----------------------------------------------------------------------------------------	

// Over Voltage & Low voltage & Voltage Unbalance
	if((fault_code&_BV(9))==0)
	{// here there is no Neutral Disturbance
		if((g_TamperStatus&MISSPOT_MASK)==0)
		{	// Low Voltage in any phase must be check only if missing potential is NOT THERE
//			if((((v[LINE_PHASE_R]<1800)&&(v[LINE_PHASE_R]>960))||((v[LINE_PHASE_Y]<1800)&&(v[LINE_PHASE_Y]>960))||((v[LINE_PHASE_B]<1800)&&(v[LINE_PHASE_B]>960)))&&((i[LINE_PHASE_R]>95)||(i[LINE_PHASE_Y]>95)||(i[LINE_PHASE_B]>95)))
			if((((v[LINE_PHASE_R]<1800)&&(v[LINE_PHASE_R]>960))||((v[LINE_PHASE_Y]<1800)&&(v[LINE_PHASE_Y]>960))||((v[LINE_PHASE_B]<1800)&&(v[LINE_PHASE_B]>960)))&&(currentsGT95>=1))
			{
		    check_setTamper(&lowVoltageCtr, computeMask(UNDER_VOLTAGE),TAMPER_PERSISTENCE_ON,99,RECORD_DELAY);	// 99 means no fault code
				g_TamperStatus|=UNDERVOLT_MASK;
			}
			
			if((v[LINE_PHASE_R]>1799)&&(v[LINE_PHASE_Y]>1799)&&(v[LINE_PHASE_B]>1799))
			{
		    check_clearTamper(&lowVoltageCtr, computeMask(UNDER_VOLTAGE),TAMPER_PERSISTENCE_OFF,99,RECORD_DELAY);
				g_TamperStatus&=~UNDERVOLT_MASK;
			}
		}
		else
		{
			lowVoltageCtr=0;
			g_TamperStatus&=~UNDERVOLT_MASK;
		}
		
		// Check overvoltage in any phase - Only requirement is ND must NOT BE THERE
		if((phasesGT276LT300>=1)&&(currentsGT95>=1))
		{
	    check_setTamper(&overVoltageCtr, computeMask(OVER_VOLTAGE),TAMPER_PERSISTENCE_ON,99,RECORD_DELAY);
			g_TamperStatus|=OVERVOLT_MASK;
		}
		
		if(phasesGT180LT276==3)
		{
	    check_clearTamper(&overVoltageCtr, computeMask(OVER_VOLTAGE),TAMPER_PERSISTENCE_OFF,99,RECORD_DELAY);
			g_TamperStatus&=~OVERVOLT_MASK;
		}
		
	  // Check  voltage unbalance condition - No Neutral disturbance, No High Voltage and No Low voltage, No missing potential 
		if((phasesGT144LT276==3)&&((g_TamperStatus&MISSPOT_MASK)==0)&&((g_TamperStatus&UNDERVOLT_MASK)==0)&&((g_TamperStatus&OVERVOLT_MASK)==0))
		{// all three phases are normal
			if(currentsGT95>=1)
			{
				if((maxValue-minValue)>(maxValue/10))
				{
			    check_setTamper(&voltageUnbalanceCtr, computeMask(VOLTAGE_UNBALANCED),900,3,RECORD_DELAY);
				}
			}
			
			if((maxValue-minValue)<(maxValue/10))
		    check_clearTamper(&voltageUnbalanceCtr, computeMask(VOLTAGE_UNBALANCED),TAMPER_PERSISTENCE_OFF,3,RECORD_DELAY);
		}
		else
		{
			fault_code&=~_BV(3);	// clear the icon
			voltageUnbalanceCtr=0;
		}	
	}// end of ND if
	else
	{// here there is Neutral disturbance
		lowVoltageCtr=0;
		overVoltageCtr=0;
		voltageUnbalanceCtr=0;
		g_TamperStatus&=~UNDERVOLT_MASK;
		g_TamperStatus&=~OVERVOLT_MASK;
		fault_code&=~_BV(3);	// clear the icon
	}
//-----------------------------------------------------------------------------------------	
//-----------------------------------------------------------------------------------------	
// Check reverse current condition 
	if((v[LINE_PHASE_R] > 1799)&&(i[LINE_PHASE_R]>95))
	{
		temp_power = (int32_t)(EM_GetActivePower(LINE_PHASE_R));
#ifdef BIDIR_METER		
	  if (temp_power < 0){
			currentreversectr++;}
	}
#else
	  if (temp_power < 0){
	    check_setTamper(&ctReverseCtr[LINE_PHASE_R], computeMask(CT_REVERSED_R),TAMPER_PERSISTENCE_ON,6,RECORD_DELAY);
			currentreversectr++;}
	  else
	    check_clearTamper(&ctReverseCtr[LINE_PHASE_R], computeMask(CT_REVERSED_R),TAMPER_PERSISTENCE_OFF,6,RECORD_DELAY);
	}
#endif

	if((v[LINE_PHASE_Y] > 1799)&&(i[LINE_PHASE_Y]>95))
	{
		temp_power = (int32_t)(EM_GetActivePower(LINE_PHASE_Y));
#ifdef BIDIR_METER		
	  if (temp_power < 0){
			currentreversectr++;}
#else
	  if (temp_power < 0){
	    check_setTamper(&ctReverseCtr[LINE_PHASE_Y], computeMask(CT_REVERSED_Y),TAMPER_PERSISTENCE_ON,7,RECORD_DELAY);
			currentreversectr++;}
	  else
	    check_clearTamper(&ctReverseCtr[LINE_PHASE_Y], computeMask(CT_REVERSED_Y),TAMPER_PERSISTENCE_OFF,7,RECORD_DELAY);
#endif
	}
	
	if((v[LINE_PHASE_B] > 1799)&&(i[LINE_PHASE_B]>95))
	{
		temp_power = (int32_t)(EM_GetActivePower(LINE_PHASE_B));
#ifdef BIDIR_METER		
	  if (temp_power < 0){
			currentreversectr++;}
#else
	  if (temp_power < 0){
	    check_setTamper(&ctReverseCtr[LINE_PHASE_B], computeMask(CT_REVERSED_B),TAMPER_PERSISTENCE_ON,8,RECORD_DELAY);
			currentreversectr++;}
		else		
	    check_clearTamper(&ctReverseCtr[LINE_PHASE_B], computeMask(CT_REVERSED_B),TAMPER_PERSISTENCE_OFF,8,RECORD_DELAY);
#endif
	}
	
	if(currentreversectr==0)		
		g_TamperStatus&=~REVERSE_MASK;
	else
	{
		g_TamperStatus|=REVERSE_MASK;
	}
//-----------------------------------------------------------------------------------------	
// if there is a reverse condition in the meter then current tampers will provide false results - hence skip
// Check open current condition 
// if all three active energies are of the same sign - only then should we proceed

	if((phasesGT180LT276==3)&&(mixedPowers==0)&&((g_TamperStatus&HALLSENS_MASK)==0)&&((g_TamperStatus&ACMAG_MASK)==0))
	{
		if(currentsGT95>=1)
		{
			maxValue = findMax(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
			
		  if(i[LINE_PHASE_R] < (maxValue/50))
			{
			  check_setTamper(&ctOpenCtr[LINE_PHASE_R], computeMask(CT_OPEN_R),900,11,RECORD_DELAY);
				ct_open_ctr++;
			}
		  else
		    check_clearTamper(&ctOpenCtr[LINE_PHASE_R], computeMask(CT_OPEN_R),TAMPER_PERSISTENCE_OFF,11,RECORD_DELAY);
				
		  if(i[LINE_PHASE_Y] < (maxValue/50))
			{
		    check_setTamper(&ctOpenCtr[LINE_PHASE_Y], computeMask(CT_OPEN_Y),900,12,RECORD_DELAY);
				ct_open_ctr++;
			}
		  else
		    check_clearTamper(&ctOpenCtr[LINE_PHASE_Y], computeMask(CT_OPEN_Y),TAMPER_PERSISTENCE_OFF,12,RECORD_DELAY);
				
		  if(i[LINE_PHASE_B] < (maxValue/50))
			{
				check_setTamper(&ctOpenCtr[LINE_PHASE_B], computeMask(CT_OPEN_B),900,13,RECORD_DELAY);
				ct_open_ctr++;
			}
		  else
		    check_clearTamper(&ctOpenCtr[LINE_PHASE_B], computeMask(CT_OPEN_B),TAMPER_PERSISTENCE_OFF,13,RECORD_DELAY);
		}
		else
		{
			ctOpenCtr[LINE_PHASE_R]=0;
			ctOpenCtr[LINE_PHASE_Y]=0;
			ctOpenCtr[LINE_PHASE_B]=0;
		}
				
		if(ct_open_ctr==0)
			g_TamperStatus&=~CTOPEN_MASK;
		else
			g_TamperStatus|=CTOPEN_MASK;
		
		if((g_TamperStatus&CTOPEN_MASK)==0)
		{		
		  if((ineutralDiff > 90)&&(i[LINE_PHASE_R]>18)&&(i[LINE_PHASE_Y]>18)&&(i[LINE_PHASE_B]>18)&&(1))
			{
		    check_setTamper(&ctBypassCtr, computeMask(CT_BYPASS),TAMPER_PERSISTENCE_ON,4,RECORD_DELAY);
				g_TamperStatus|=BYPASS_MASK;
			}
			else
			{
		    check_clearTamper(&ctBypassCtr, computeMask(CT_BYPASS),TAMPER_PERSISTENCE_OFF,4,RECORD_DELAY);
				g_TamperStatus&=~BYPASS_MASK;
			}	
		}
		else
		{// this is required, because if CT open condition occurs after ctbypass then ct bypass icon remains permanently on
			g_TamperStatus&=~BYPASS_MASK;
			ctBypassCtr=0;
		}
		
	
//		if(((g_TamperStatus&CTOPEN_MASK)==0)&&((g_TamperStatus&BYPASS_MASK)==0)&&((g_TamperStatus&MISSPOT_MASK)==0)&&(currentsGT95==3))
		if(((g_TamperStatus&CTOPEN_MASK)==0)&&((g_TamperStatus&BYPASS_MASK)==0)&&(currentsGT95==3))
		{// Check current unbalance condition			if((i[LINE_PHASE_R]>95)&&(i[LINE_PHASE_Y]>95)&&(i[LINE_PHASE_B]>95))
			{
				maxValue = findMax(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
				minValue = findMin(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
				
				if((maxValue-minValue)>(((int32_t)maxValue*(int32_t)3)/(int32_t)10))
				{
			    check_setTamper(&currentUnbalanceCtr, computeMask(CURRENT_UNBALANCED),900,5,RECORD_DELAY);
				}
				else
			    check_clearTamper(&currentUnbalanceCtr, computeMask(CURRENT_UNBALANCED),TAMPER_PERSISTENCE_OFF,5,RECORD_DELAY);
			}
		}
		else
		{
			fault_code&=~_BV(5);	// clear the icon
			currentUnbalanceCtr=0;
		}	
		
	
/*	
	  // Check over current condition
		if((i[LINE_PHASE_R]>OVER_CURRENT_THRESHOLD)||(i[LINE_PHASE_Y]>OVER_CURRENT_THRESHOLD)||(i[LINE_PHASE_B]>OVER_CURRENT_THRESHOLD))
		{
	    check_setTamper(&overCurrentCtr, computeMask(OVER_CURRENT),TAMPER_PERSISTENCE_ON);
			fault_code=7;	// generic fault
		}
		else
	    check_clearTamper(&overCurrentCtr, computeMask(OVER_CURRENT),TAMPER_PERSISTENCE_OFF);
*/			
	}
	else
	{
		ctOpenCtr[LINE_PHASE_R]=0;
		ctOpenCtr[LINE_PHASE_Y]=0;
		ctOpenCtr[LINE_PHASE_B]=0;
	
		g_TamperStatus&=~BYPASS_MASK;
		g_TamperStatus&=~CTOPEN_MASK;
		ctBypassCtr=0;		
		fault_code&=~_BV(5);	// clear the icon
		currentUnbalanceCtr=0;
	}
	

  // Check hall sensor condition
#ifdef PCB_2_5
	if(DC_MAGNET_SENSOR==0)
#else
	if((HALLSENSOR1==0)||(HALLSENSOR2==0)||(HALLSENSOR3==0)||(HALLSENSOR4==0))
#endif	
	{
    check_setTamper(&magnetCtr, computeMask(MAGNET),MAGNET_TAMPER_PERSISTENCE_ON,10,RECORD_DELAY);
		g_TamperStatus|=HALLSENS_MASK;
		
// if dc magnet is sensed then we should disable reverse current detection
		ctReverseCtr[0]=0;
		ctReverseCtr[1]=0;
		ctReverseCtr[2]=0;
		g_TamperStatus&=~REVERSE_MASK;
	}
	else
	{
    check_clearTamper(&magnetCtr, computeMask(MAGNET),MAGNET_TAMPER_PERSISTENCE_OFF,10,RECORD_DELAY);
		g_TamperStatus&=~HALLSENS_MASK;
	}

	
	
// we cannot use the same mask for AC magnet, since if magnet is there but ac magnet is not there
// then it will restore the occured tamper - Hence for ac magnet we must use 
// computeMask(AC_MAGNET)
// check every second

/*
	if(acMagnetSense!=0)
	{
		acMagnetSenseCtr++;
		if(acMagnetSenseCtr>15)
		{
			acMagnetSense=0;
		}	
	}
*/
	acMagnetSecondsCtr++;
	if(acMagnetSecondsCtr==58)
	{// it is going to be one minute soon
    DI();
		acMagnetSecondsCtr=0;	// get ready for the next 58 seconds
		if(acMagnetPulseCtr<4)
		{
			acMagnetSense=0;	// it can't be an acmagnet situation
			pulsesStartedComingFlag=0;	// this flag MUST BE CLEARED TOO else the MAG Icon may remain LIT
			acMagnetPulseCtr=0;
		}
		else
		{// here 4 or more pulses have been received in a minute
			acMagnetPulseCtr=0;	// prepare for the next minute
		}
    EI();
	}

	if(acMagnetSense!=0)
	{// ac magnet has been sensed
  	check_setTamper(&acMagnetCtr, computeMask(AC_MAGNET),MAGNET_TAMPER_PERSISTENCE_ON,14,RECORD_DELAY);
		g_TamperStatus|=ACMAG_MASK;
		
// if ac magnet is sensed then we should disable dc magnet detection
		magnetCtr=0;
		g_TamperStatus&=~HALLSENS_MASK;	// clear the display flag too
		
// if ac magnet is sensed then we should disable reverse current detection
		ctReverseCtr[0]=0;
		ctReverseCtr[1]=0;
		ctReverseCtr[2]=0;
		g_TamperStatus&=~REVERSE_MASK;
	}
	else
	{// here the acMagnet sense has disappeared - this means ac magnet pulses have not come for 15 seconds
    check_clearTamper(&acMagnetCtr, computeMask(AC_MAGNET),MAGNET_TAMPER_PERSISTENCE_OFF,14,RECORD_DELAY);
		g_TamperStatus&=~ACMAG_MASK;
	}	
	
	
		

	
	
	
/*	
	tempFloat = ABS(getInstantaneousParams(IPFS,LINE_TOTAL));
		
	if((tempFloat<0.2)&&((i[LINE_PHASE_R]>0.95)||(i[LINE_PHASE_Y]>0.95)||(i[LINE_PHASE_B]>0.95)))
	{
    check_setTamper(&lowPFCtr, computeMask(LOW_PF),TAMPER_PERSISTENCE_ON);
		g_TamperStatus|=LOW_PF_MASK;
	}
	else
	{
    check_clearTamper(&lowPFCtr, computeMask(LOW_PF),TAMPER_PERSISTENCE_OFF);
		g_TamperStatus&=~LOW_PF_MASK;
	}
*/	
	
}

uint8_t determineTwoWireTamper(int16_t iR, int16_t iY, int16_t iB)
{// the currents have two decimals - i.e 0.01 amps
	int16_t	pf_R;
	int16_t	pf_Y;
	int16_t	pf_B;
	
	int16_t angleR;
	int16_t angleY;
	int16_t angleB;
	
	uint8_t currentInPhaseCount;
	uint8_t zeroPFcount;
	uint8_t retval;
	
	pf_R = (int16_t)(getInstantaneousParams(IPFS,LINE_PHASE_R)*10000);	// these are signed values
	pf_Y = (int16_t)(getInstantaneousParams(IPFS,LINE_PHASE_Y)*10000);	// these are signed values
	pf_B = (int16_t)(getInstantaneousParams(IPFS,LINE_PHASE_B)*10000);	// these are signed values

	currentInPhaseCount=0;
	zeroPFcount=0;
	
	
/*
// This code was used earlier, before the changes made of reporting pf as 1 
// when both active power and reactive power were 0 
	if(ABS(pf_R)<200)
		zeroPFcount++;
	if(ABS(pf_Y)<200)
		zeroPFcount++;
	if(ABS(pf_B)<200)
		zeroPFcount++;
*/
		

// under the modifications made in getInstantaneousParams for pf values we need to use the following expressions
// this is because when there is no power then the pf is reported as 1.000

	if(ABS(pf_R)>=9998)	// 10000 for twowiretest
		zeroPFcount++;
	if(ABS(pf_Y)>=9998)	// 10000 for twowiretest
		zeroPFcount++;
	if(ABS(pf_B)>=9998)	// 10000 for twowiretest
		zeroPFcount++;

	if(zeroPFcount==0)
	{
		retval=0;	//no tamper
	}
	else
	{
		if(zeroPFcount==3)
			retval=0;	//no tamper
		else
		{// here zeroPFcount = 1 or 2 - possibility of a tamper
			if(iR>50)
			{
				angleR = findArcCos(pf_R, seedValue_R);	// this function only returns angles between 1 and 90
				seedValue_R=angleR;	// for next time
				currentInPhaseCount++;
			}
			else
				angleR=999;	// this means do not consider this phase for current less than 0.05 amps i.e. 50 mA
			
			if(iY>50)
			{
				angleY = findArcCos(pf_Y, seedValue_Y);	// this function only returns angles between 1 and 90
				seedValue_Y=angleY;	// for next time
				currentInPhaseCount++;
			}
			else
				angleY=999;	// this means do not consider this phase for current less than 0.05 amps i.e. 50 mA

			if(iB>50)
			{
				angleB = findArcCos(pf_B, seedValue_B);	// this function only returns angles between 1 and 90
				seedValue_B=angleB;	// for next time
				currentInPhaseCount++;
			}
			else
				angleB=999;	// this means do not consider this phase for current less than 0.05 amps i.e. 50 mA
			
			if(currentInPhaseCount==2)
			{// two phases must have current
				if(angleR==999)
				{// we must consider phases Y and B
					if(checkTwoWireTamper(pf_Y,pf_B)==0)
						retval=0;
					else
					{// here a two wire tamper condition exists
						retval=1;
						if(angleY>=angleB)
						{// angleY is higher or equal
							operatingPF = ((float32_t)findCos(angleY-30))/10000.0;
						}
						else
							operatingPF = ((float32_t)findCos(angleB-30))/10000.0;
					}
				}
				else
				{
					if(angleY==999)
					{// we must consider phases R and B
						if(checkTwoWireTamper(pf_R,pf_B)==0)
							retval=0;
						else
						{// here a two wire tamper condition exists
							retval=1;
							if(angleR>=angleB)
							{// angleR is higher or equal
								operatingPF = ((float32_t)findCos(angleR-30))/10000.0;
							}
							else
								operatingPF = ((float32_t)findCos(angleB-30))/10000.0;
						}
					}
					else
					{
						if(angleB==999)
						{// we must consider phases R and Y
							if(checkTwoWireTamper(pf_R,pf_Y)==0)
								retval=0;
							else
							{// here a two wire tamper condition exists
								retval=1;
								if(angleR>=angleY)
								{// angleR is higher or equal
									operatingPF = ((float32_t)findCos(angleR-30))/10000.0;
								}
								else
									operatingPF = ((float32_t)findCos(angleY-30))/10000.0;
							}
						}
					}
				}
			}
			else
				retval=0;	// no tamper if only 0 or 1 or 3 phases have current
		}
	}
	return retval;	
}

int8_t checkTwoWireTamper(int16_t pf1,int16_t pf2)
{// pf1 and pf2 are signed integers between -10000 and 10000 
// if the two pfs are same in magnitude and sign - then there is no tamper
// if magnitudes are different - then there is tamper
// if signs are different - then there is tamper

	int16_t diff;
	
	diff = ABS(pf1-pf2);
	
	if(diff>2000)	// diff is expect to be > 5700 - just being safe
		return 1;
	else
		return 0;
}


#ifdef TN3P_BOARD
// this function is called whenever rtc is backforce - all occured tampers are to be restored
// this function is used specifically for TNEB
extern uint32_t gTamperEvent;

void restoreUnrestoredTampers(void)
{
	uint32_t whichevent;

// the following variable is being set to begin with since transaction events and power events are not to be restored	
	gTamperEvent = mytmprdata.tamperEvent_image&TAMPER_DISPLAY_MASK; // make all those bits not to be displayed as 0
// only those tampers which are unrestored will be 1 in gTamperEvent
	do
	{
		whichevent = gTamperEvent & ~(gTamperEvent-1);
		
//		clear_tamperEvent_image(whichevent);

	  readRTC();  // reading the RTC now is important so that the correct time gets recorded - 09/Apr/2011
	  mytmprdata.tamperEvent_image &= ~whichevent;
	  mytmprdata.tamperEvent_flags |= whichevent;
	  mytmprdata.tamperEvent_on &= ~whichevent;
		
	  e2write_flags |= E2_W_EVENTDATA;
	  wre2rom();
		
		gTamperEvent&=~whichevent;	// job complete for this fellow
	}
	while(gTamperEvent!=0);
	
}

#endif








