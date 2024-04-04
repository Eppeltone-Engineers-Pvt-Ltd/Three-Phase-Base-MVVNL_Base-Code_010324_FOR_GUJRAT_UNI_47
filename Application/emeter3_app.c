// emeter3_app.c
#include <stdio.h>

#include	<string.h>

/* Driver layer */
#include "r_cg_macrodriver.h"	// located in driver/mcu
#include "typedef.h"            // located right at top of tree below iodefine.h
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "em_type.h"

/* Application layer */
//#include "platform.h"
#include "emeter3_structs.h"
#include "emeter3_app.h"
#include "memoryOps.h"
//#include "r_lcd_display.h"      /* LCD Display Application Layer */
//#include "r_lcd_config.h"
//#include "em_display.h"         /* EM Display Application Layer */
//#include "event.h"
#include "powermgmt.h"
#include "pravakComm.h"
#include "utilities.h"
#include "survey.h"
#include "tampers.h"						// contains description of macro ABS(x)
#include "p_timer.h"
#include "tod.h"


/* Display tamper condition */
//#include "format.h"
//#include "eeprom.h"
#include "em_core.h"		// located in middleware/headers
#include "em_type.h"
#include "r_dlms_obis_ic.h"


extern void restoreRTC_Date_Time(void);

uint8_t checkForVoltageHarmonics(EM_LINE line);
uint8_t checkForCurrentHarmonics(EM_LINE line);
void checkForBilling(void);
void computeNextBillDate(void);
void performModeChangeActions(uint8_t i);
void restoreMeterDisplay(void);
void energy_incrementation(void);	// this function increments the energy registers
void CommunicationTimeoutCheck(void);
uint8_t do_md_stuff(void);
//void do_md_stuff(void);



extern time_t g_Class22_billtime;
extern date_t g_Class22_billdate;
extern date_time_t g_Class03_BillingDate;
extern uint32_t g_Class07_Blockload_MaxEntries;
extern float32_t operatingPF;


//uint8_t total_active_energy_pulse_remaining_time; // total_energy_pulse_remaining_time used in wrp_em_adc.c instead
int32_t total_active_power;
//int32_t total_active_power_counter;	// not used anywhere
int32_t total_consumed_active_energy;  //uint32_t
#ifdef SPLIT_ENERGIES_REQUIRED
int32_t total_consumed_active_energy_lag;
int32_t total_consumed_active_energy_lead;
#endif

#ifdef BIDIR_METER
int32_t total_consumed_active_energy_export;
#endif
int32_t total_consumed_fundamental_active_energy;

int32_t total_energy_lastSaved;  //uint32_t
//int32_t zone_kwh_last_saved;
//int32_t zone_kvah_last_saved;
//int32_t zone_kvarh_lag_last_saved;
//int32_t zone_kvarh_lead_last_saved;

#ifdef SPLIT_ENERGIES_REQUIRED
int32_t active_energy_lag_lastSaved;
int32_t	active_energy_lead_lastSaved;
int32_t	apparent_energy_lag_lastSaved;
int32_t	apparent_energy_lead_lastSaved;
#endif

#ifdef BIDIR_METER
int32_t total_energy_lastSaved_export;
//int32_t zone_kwh_last_saved_export;
//int32_t zone_kvah_last_saved_export;
//int32_t zone_kvarh_lag_last_saved_export;
//int32_t zone_kvarh_lead_last_saved_export;
#endif

//uint8_t total_reactive_energy_pulse_remaining_time;	// new variable to be defined in wrp_em_adc.c
int32_t total_reactive_power; // this is the sum of the two below
int32_t total_reactive_power_lag;
int32_t total_reactive_power_lead;
//int32_t total_reactive_power_counter; // not used anywhere
int32_t total_consumed_reactive_energy_lag;//uint32_t
#ifdef BIDIR_METER
int32_t total_consumed_reactive_energy_lag_export;
#endif

int32_t total_consumed_reactive_energy_lead;//uint32_t
#ifdef BIDIR_METER
int32_t total_consumed_reactive_energy_lead_export;
#endif
int32_t reactive_energy_lead_lastSaved;//uint32_t
int32_t reactive_energy_lag_lastSaved;//uint32_t

#ifdef BIDIR_METER
int32_t reactive_energy_lead_lastSaved_export;  //uint32_t
int32_t reactive_energy_lag_lastSaved_export;   // uint32_t
#endif


//uint8_t total_reactive_energy_pulse_remaining_time; // we will not be pulsing the apparent power
int32_t total_apparent_power;
//int32_t total_apparent_power_cotounter;
int32_t total_consumed_apparent_energy;//uint32_t
#ifdef SPLIT_ENERGIES_REQUIRED
int32_t total_consumed_apparent_energy_lag;
int32_t total_consumed_apparent_energy_lead;
#endif


#ifdef BIDIR_METER
int32_t total_consumed_apparent_energy_export;
#endif
int32_t apparent_energy_lastSaved;
int32_t gujaratSecondsCounter;	// this counter is used to power off the display in battery_mode

#ifdef BIDIR_METER
int32_t apparent_energy_lastSaved_export;
#endif


int32_t total_active_power_pulsing;	// this is in deciwatts
int32_t total_apparent_power_pulsing;
int32_t total_reactive_power_pulsing;


toddata_t mytoddata;
// zoneDefs_t zoneDefines;	// active zone definitions - now a local variable
//zoneDefs_t zoneDefinesPassive;	// passive zone definitions

energydata_t myenergydata;
mddata_t mymddata;
ctldata_t ctldata;
commondata_t Common_Data;
tmprdata_t mytmprdata;


ext_rtc_t rtc;	// this is the rtc used in our app

uint16_t genflags;
uint16_t e2write_flags;

#ifdef SURVEY_USES_AVERAGE_DATA
uint16_t no_load_time;
uint16_t average_Vr;
uint16_t average_Vy;
uint16_t average_Vb;
int16_t average_Ir;
int16_t average_Iy;
int16_t average_Ib;
uint16_t averagePF_r;
uint16_t averagePF_y;
uint16_t averagePF_b;
uint16_t nCount;
#endif

uint8_t class20_status;
uint8_t class22_status;

uint8_t presentZone;

uint8_t rtctimer;
#define RTC_READ_INTERVAL 30

uint8_t twoWireOperation;
uint8_t failCount;	// this represents a fail during write_energies

#ifdef TESTING
int32_t load;
#endif

void update_energies_stored(void);

#ifdef SURVEY_USES_AVERAGE_DATA
void perform_VI_averaging(void);
#endif

#ifdef ND_POWER_ON_ACTUALS
	float32_t getTNEB_ND_Power(EM_LINE line);
	float32_t calculateNDPower(float32_t v_line, float32_t i_line, float32_t i_neutral);
#endif
	float32_t computeTwoWirePower(EM_LINE line);

uint8_t isBillToBeMade(uint8_t mth);


float32_t computeTwoWirePower(EM_LINE line)
{
	float32_t retPower = 0;
	float32_t volt;
	float32_t curr;
	float32_t pf;
	uint8_t j;

	if(line==LINE_TOTAL)
	{
		for(j=0;j<3;j++)
		{
			volt = EM_GetVoltageRMS((EM_LINE)j)/0.866;
			curr = EM_EXT_GetIRMS((EM_LINE)j);
			if(volt>140.0)
			{
				retPower += ABS((volt*curr*operatingPF));
			}
		}
	}
	else
	{
		if(line==LINE_PHASE_R)
		{
			volt = EM_GetVoltageRMS(LINE_PHASE_R)/0.866;
			curr = EM_EXT_GetIRMS(LINE_PHASE_R);
			if(volt>140.0)
			{
				retPower += ABS((volt*curr*operatingPF));
			}
		}
		else
		{
			if(line==LINE_PHASE_Y)
			{
				volt = EM_GetVoltageRMS(LINE_PHASE_Y)/0.866;
				curr = EM_EXT_GetIRMS(LINE_PHASE_Y);
				if(volt>140.0)
				{
					retPower += ABS((volt*curr*operatingPF));
				}
			}
			else
			{
				if(line==LINE_PHASE_B)
				{
					volt = EM_GetVoltageRMS(LINE_PHASE_B)/0.866;
					curr = EM_EXT_GetIRMS(LINE_PHASE_B);
					if(volt>140.0)
					{
						retPower += ABS((volt*curr*operatingPF));
					}
				}
			}
		}
	}
	
	return retPower;
}

//#ifdef TN3P_BOARD
#ifdef ND_POWER_ON_ACTUALS
float32_t calculateNDPower(float32_t v_line, float32_t i_line, float32_t i_neutral)
{// i_line and in_neutral are +ve
	float32_t retPower;
	float32_t used_current;
//	if(v_line>350.0)
	if(v_line>300.0)	// using a lower threshold
	{
		retPower = v_line*i_line;
	}
	else
	{// here power will be either 240.0 or 0.0 - could be 242 also
// it is possible that the tamper conditions have been restored - 
// however it will take three	minutes for the actual to be restored
// when the tamper condition is restored, the line current will now flow and neutral current will become zero
// hence we must use the higher of the two

// the code is modified - for the phase whose measured voltage is < 240, we must add the i_line and i_neutral
//		if(ABS(i_line)>=ABS(i_neutral))
//			used_current = i_line;
//		else
//			used_current = i_neutral;
		used_current = i_line + i_neutral;
		
		if(v_line<=240.0)
		{
			retPower = 240.0*used_current;
		}
		else
		{// if voltage being fed is say 248V or so
			retPower = v_line*used_current;
		}
	}
	return retPower;
}

float32_t getTNEB_ND_Power(EM_LINE line)
{// this fellow computes power when neutral disturbance has been created
	// in case 1, twoPhasesGT350, one phase = 240, Two phases actual current, One phase current 0, Neutral current=actual current
	// in case 2, twoPhasesGT350, one phase = 240, Two phases actual current, One phase current 0, Neutral current=actual current
	//  in case 3, twoPhasesGT350, one phase =0V, Two phases actual current, One phase current 0, Neutral current=actual current
	//if voltage measured is > Vref, use higher voltage
	// If voltage measured is < Vref, use 240
	float32_t computedPower=0;
  float32_t i[EM_NUM_OF_LINE];
//  float32_t v[EM_NUM_OF_PHASE];

//At the time of registering the value the vR, vY and vB values have been saved
// and are available in vR_saved, vY_saved, vB_saved
	
//	v[LINE_PHASE_R] = EM_GetVoltageRMS(LINE_PHASE_R);
//	v[LINE_PHASE_Y] = EM_GetVoltageRMS(LINE_PHASE_Y);
//	v[LINE_PHASE_B] = EM_GetVoltageRMS(LINE_PHASE_B);
	
  i[LINE_PHASE_R] = EM_EXT_GetIRMS(LINE_PHASE_R);
  i[LINE_PHASE_Y] = EM_EXT_GetIRMS(LINE_PHASE_Y);
  i[LINE_PHASE_B] = EM_EXT_GetIRMS(LINE_PHASE_B);
  i[LINE_NEUTRAL] = EM_GetCurrentRMS(LINE_NEUTRAL);
	
	if(i[LINE_PHASE_R]<0)
		i[LINE_PHASE_R]*=-1;

	if(i[LINE_PHASE_Y]<0)
		i[LINE_PHASE_Y]*=-1;
		
	if(i[LINE_PHASE_B]<0)
		i[LINE_PHASE_B]*=-1;
		
	if(i[LINE_NEUTRAL]<0)
		i[LINE_NEUTRAL]*=-1;
		
	if(line==LINE_TOTAL)
	{
		computedPower+=calculateNDPower((((float32_t)vR_saved)/10.0),i[LINE_PHASE_R],i[LINE_NEUTRAL]);
		computedPower+=calculateNDPower((((float32_t)vY_saved)/10.0),i[LINE_PHASE_Y],i[LINE_NEUTRAL]);
		computedPower+=calculateNDPower((((float32_t)vB_saved)/10.0),i[LINE_PHASE_B],i[LINE_NEUTRAL]);
	}
	else
	{
		if(line==LINE_PHASE_R)
		{
			computedPower=calculateNDPower((((float32_t)vR_saved)/10.0),i[LINE_PHASE_R],i[LINE_NEUTRAL]);
		}
		else
		{
			if(line==LINE_PHASE_Y)
			{
				computedPower=calculateNDPower((((float32_t)vY_saved)/10.0),i[LINE_PHASE_Y],i[LINE_NEUTRAL]);
			}
			else
			{
				if(line==LINE_PHASE_B)
				{
					computedPower=calculateNDPower((((float32_t)vB_saved)/10.0),i[LINE_PHASE_B],i[LINE_NEUTRAL]);
				}
			}
		}
	}
	
	return computedPower;
}
#endif

float32_t getInstantaneousParams(uint8_t param, EM_LINE line)
{
// parameters are voltage(0), current(1), pf(2), frequency(3), actpower(4), apppower(5), reactpowerlead(6), reactpowerlag(7)
  powermgmt_mode_t power_mode;
	float32_t tempFloat=0;	
	float32_t tempFloat1 =0;
	int32_t tempLong1;
	int32_t tempLong2;
	uint8_t	tbuf[8];
//	float32_t tempFloat2 =0;
//	float32_t absTempFloat = 0;
	
	power_mode = POWERMGMT_GetMode();
	if (power_mode != POWERMGMT_MODE3)
	{
		if(((mytmprdata.tamperEvent_image&maskTable[MAGNET])==0)&&((mytmprdata.tamperEvent_image&maskTable[AC_MAGNET])==0))
		{// no magnet tamper here
			if((fault_code&_BV(9))==0)
			{// normal case or 2 wire
				switch(param)
				{
					case	IVOLTS:
						tempFloat=EM_GetVoltageRMS(line);
						break;
				
					case	ICURRENTS:
						tempFloat=EM_GetCurrentRMS(line);
/*						
	#ifdef CLASS1_METER
						if(tempFloat<=0.0098)
							tempFloat=0;
	#else
						if(tempFloat<=0.003)
							tempFloat=0;
	#endif
*/	
						tempLong1 = (int32_t)(EM_GetActivePower(line));
						tempLong2 = (int32_t)(EM_GetReactivePower(line));

						if((ABS(tempLong1)<3)&&(ABS(tempLong2)<6))
							tempFloat=0;	// return current = 0 when there is zero power in the power (active or reactive)		
						else
						{// here there is some power
							tempFloat1 = EM_GetPowerFactor(line);
							if(tempFloat1<0)
								tempFloat1*=-1;	// pf is forced to be positive
							
							if(tempFloat1 <=0.05)
							{// we are operating close to 90 (87 to 93) or 270 (267 to 273)
								if(tempLong2>0)
								{// reactive power is +ve : we are around 90 - hence force current to be positive  
									if(tempFloat<0)
										tempFloat*=-1;	// current is forced to be positive
								}
								else
								{// reactive power is -ve
									if(tempFloat>0)
										tempFloat*=-1;	// current is forced to be negative
								}
							}
						}
						break;
				
					case	IPFS:
						if(line==LINE_TOTAL)
						{// in this case the sign is to be the product of the sign of total_active and total_reactive_power
							tempFloat = (float32_t)systemPF/(float32_t)1000.0;	// systemPF can be positive or negative
						}
						else
						{
							tempFloat=EM_GetPowerFactor(line);
							if(tempFloat < 0)
								tempFloat*=-1;
							// tempFloat is now always positive
							tempLong1 = (int32_t)(EM_GetActivePower(line));
							tempLong2 = (int32_t)(EM_GetReactivePower(line));
/*							
							sprintf((char*)tbuf,"%ld ",tempLong1);
							asendbare(tbuf);
							
							sprintf((char*)tbuf,"%ld ",tempLong2);
							asendbare(tbuf);
							asendcrlf();
*/							
							if((ABS(tempLong1)<3)&&(ABS(tempLong2)<6))
								tempFloat=1.0;	// return pf = 1 when there is zero power in the power (active or reactive)		
							else
							{
								if(tempFloat > 0.05)	
								{// here magnitude of pf is greater than 0.05 - need to determine its sign
									if(tempFloat>0.985)
										tempFloat = 1.000001;
									else
									{
										if(isSameSign(tempLong1,tempLong2)==0)
										{// here the signs are different - pf is capacitive
											tempFloat*=-1;	// make the pf value -ve
										}
									}
								}
								else
								{// when pf is < 0.05 as is likely in the case of 90 and 270 degrees
									// treat the pf as +ve always - this will fix the problem of 89,91 and 269,271
									// do nothing
										tempFloat = 0.0;
								}
							}
							
/*
// EARLIER IMPLEMENTATION HAS BEEN COMMENTED - 16/SEP/2022
							absTempFloat = tempFloat;
							if(absTempFloat<0)
								absTempFloat*=-1;
								
							if(absTempFloat<0.085)
							{
								tempFloat=0.0;
							}
							else
							{
								if(absTempFloat<0.995)
								{	
									tempFloat1=(float32_t)EM_GetPowerFactorSign(line);
									if(tempFloat1<0)
										tempFloat = tempFloat1*tempFloat;
								}
								else
									tempFloat=1.0;
							}
*/							
						}
						break;
				
					case	IFREQS:
						tempFloat=EM_GetLineFrequency(line);
						break;
				
					case	ACT_POWER:
						if(twoWireOperation!=0)
						{
							tempFloat = computeTwoWirePower(line);
						}
						else
						{
							if(line==LINE_TOTAL)
							{
								tempFloat=(float32_t)total_active_power;
							}
							else
							{// we need to account for the cases of 90 and 270 degrees
							// close to 90 and 270 when pf is less than 0.05 we always send the sign as lagging
							// if reactive power is high(+ve) and active power is small, and pf<0.05 then it is safe to assume 90 deg operation
							// if reactive power is high(-ve) and active power is small, and pf<0.05 then it is safe to assume 270 deg operation
							// for 90 deg operation, active power will be forced to be +ve and this will lead to Import Lag
							// for 270 deg operation, active power will be forced to be -ve and this will lead to Export Lag
								tempFloat = EM_GetActivePower(line);
								
								tempLong2 = (int32_t)(EM_GetReactivePower(line)*1000);
								tempFloat1 = EM_GetPowerFactor(line);
								if(tempFloat1<0)
									tempFloat1*=-1;	// pf is forced to be positive
								
								if(tempFloat1 <=0.05)
								{// we are operating close to 90 (87 to 93) or 270 (267 to 273)
									if(tempLong2>0)
									{// reactive power is +ve : we are around 90 - hence force active power to be positive  
										if(tempFloat<0)
											tempFloat*=-1;	// active power is forced to be positive
									}
									else
									{// reactive power is -ve
										if(tempFloat>0)
											tempFloat*=-1;	// active power is forced to be negative
									}
								}
							}
						}
						break;
				
					case	APP_POWER:
						if(twoWireOperation!=0)
							tempFloat=computeTwoWirePower(line);
						else
						{
							if(line==LINE_TOTAL)
								tempFloat=(float32_t)total_apparent_power;
							else
								tempFloat = EM_GetApparentPower(line);
						}
						break;
				
					case	REACT_POWER_LEAD:
						if(twoWireOperation!=0)
							tempFloat=0;
						else
						{
							tempFloat=(float32_t)total_reactive_power_lead;
						}
						break;
				
					case	REACT_POWER_LAG:
						if(twoWireOperation!=0)
							tempFloat=0;
						else
							tempFloat=(float32_t)total_reactive_power_lag;
						break;
				
					case	PHASE_CURRENT_VECT:
						tempFloat=EM_GetPhaseCurrentSumVector();
						break;
				
					case	PFSIGNVAL:
						if(line==LINE_TOTAL)
						{// in this case the sign is to be the product of the sign of total_active and total_reactive_power
							if(systemPF==0)
								tempFloat=1.0;	// PF_SIGN_LAG
							else
							{
								if(ABS(systemPF)>985)
									tempFloat=0.0;	// 0 means PF_SIGN_UNITY
								else
								{
									if(systemPF<0)
										tempFloat =  -1.0;	// PF_SIGN_LEAD
									else
										tempFloat = 1.0;	// PF_SIGN_LAG
								}
							}
						}
						else
						{// this is for individual lines
							tempFloat=EM_GetPowerFactor(line);	// tempFloat contains pf
							if(tempFloat < 0)
								tempFloat*=-1;	// tempFloat is always +ve

								// tempFloat is now always positive
							tempLong1 = (int32_t)(EM_GetActivePower(line)*1000);
							tempLong2 = (int32_t)(EM_GetReactivePower(line)*1000);
							if((ABS(tempLong1)<3)&&(ABS(tempLong2)<6))
								tempFloat=0.0;	// pf sign is UNITY for zero power		
							else
							{
								if(tempFloat > 0.05)	
								{// here magnitude of pf is greater than 0.05 - need to determine its sign
									if(tempFloat>0.985)
										tempFloat = 0.00000;	// PF SIGN is UPF
									else
									{
										if(isSameSign(tempLong1,tempLong2)==0)
										{// here the signs are different - pf is capacitive
											tempFloat=-1.0;	// PF SIGN is LEAD
										}
										else
											tempFloat=1.0;	// PF SIGN is LAG
									}
								}
								else
								{// here pf is less than 0.05
									tempFloat = 1.0000;	// PF SIGN is LAG
								}
							}
						}
						break;
						
					case REACT_POWER_SIGNED:
						if(twoWireOperation!=0)
							tempFloat=0;
						else
						{
							tempFloat=(float32_t)EM_GetReactivePower(line);
						}
						break;					
				}
			}
			else
			{// Here it is neutral disturbance
				switch(param)
				{
					case	IVOLTS:
//#ifdef TN3P_BOARD						
						tempFloat = EM_GetVoltageRMS(line);
//#else
//						tempFloat=240.0;
//#endif
						break;
				
					case	ICURRENTS:
						tempFloat=EM_GetCurrentRMS(line);
						if(tempFloat<0.003)
							tempFloat=0;
						break;
				
					case	IPFS:
#ifdef TN3P_BOARD					
						tempFloat=EM_GetPowerFactor(line);
						if(tempFloat<0.95)
						{
							tempFloat1=(float32_t)EM_GetPowerFactorSign(line);
							if(tempFloat1<0)
								tempFloat = tempFloat1*tempFloat;
						}
#else
						tempFloat=1.0;
#endif						
						break;
				
					case	IFREQS:
						tempFloat=50.0;
						break;
				
					case	ACT_POWER:
#ifdef ND_POWER_ON_ACTUALS
						tempFloat = getTNEB_ND_Power(line);
#else
							if(line==LINE_TOTAL)
							{
								tempFloat=ABS(240*EM_GetCurrentRMS(LINE_PHASE_R));	// is the ABS necessary
								tempFloat+=ABS(240*EM_GetCurrentRMS(LINE_PHASE_Y));
								tempFloat+=ABS(240*EM_GetCurrentRMS(LINE_PHASE_B));
							}
							else
							{
								tempFloat=ABS(240*EM_GetCurrentRMS(line));
							}
#endif
						break;
				
					case	APP_POWER:
#ifdef ND_POWER_ON_ACTUALS
						tempFloat = getTNEB_ND_Power(line);
#else
						tempFloat=240*EM_GetCurrentRMS(LINE_PHASE_R)+240*EM_GetCurrentRMS(LINE_PHASE_Y)+240*EM_GetCurrentRMS(LINE_PHASE_B);
#endif
						break;
				
					case	REACT_POWER_LEAD:
						tempFloat=0.0;
						break;
				
					case	REACT_POWER_LAG:
						tempFloat=0.0;
						break;
				
					case	PHASE_CURRENT_VECT:
						tempFloat=EM_GetPhaseCurrentSumVector();
						break;
				
					case	PFSIGNVAL:
						tempFloat=0.0;
						break;
						
					case REACT_POWER_SIGNED:
						tempFloat=0.0;
						break;					
				}					
			}
		}
		else
		{// here iit is magnet case
			switch(param)
			{
				case	IVOLTS:
					tempFloat=240.0;
					break;
			
				case	ICURRENTS:
					tempFloat=MAGNET_IMAX_CURRENT;
					break;
			
				case	IPFS:
					tempFloat=1.0;
					break;
			
				case	IFREQS:
					tempFloat=50.0;
					break;
			
				case	ACT_POWER:
					if(line==LINE_TOTAL)
						tempFloat=(MAGNET_IMAX_POWER*3.0);
					else
						tempFloat=MAGNET_IMAX_POWER;
					break;
			
				case	APP_POWER:
					if(line==LINE_TOTAL)
						tempFloat=(MAGNET_IMAX_POWER*3.0);
					else
						tempFloat=MAGNET_IMAX_POWER;
					break;
			
				case	REACT_POWER_LEAD:
					tempFloat=0.0;
					break;
			
				case	REACT_POWER_LAG:
					tempFloat=0.0;
					break;
			
				case	PHASE_CURRENT_VECT:
					tempFloat=0.0;
					break;
			
				case	PFSIGNVAL:
					tempFloat=0.0;
					break;
					
				case REACT_POWER_SIGNED:
					tempFloat=0.0;
					break;					
			}					
		}
	}
	else
	{// here is Mode 3
//		exportMode=0;
		tempFloat=0.0;
	}
	return tempFloat;
}

void update_saved_energies(void)
{
	total_energy_lastSaved=myenergydata.kwh;
	apparent_energy_lastSaved = myenergydata.kvah;
	reactive_energy_lag_lastSaved = myenergydata.kvarh_lag;
	reactive_energy_lead_lastSaved= myenergydata.kvarh_lead;
//	zone_kwh_last_saved=myenergydata.zone_kwh;
//	zone_kvah_last_saved=myenergydata.zone_kvah;

#ifdef SPLIT_ENERGIES_REQUIRED
	active_energy_lag_lastSaved=myenergydata.kwh_lag;
	active_energy_lead_lastSaved=myenergydata.kwh_lead;
	apparent_energy_lag_lastSaved=myenergydata.kvah_lag;
	apparent_energy_lead_lastSaved=myenergydata.kvah_lead;
#endif
/*
#ifdef TOD_HAS_REACTIVE_ENERGIES	
//	zone_kvarh_lag_last_saved=myenergydata.zone_kvarh_lag;
//	zone_kvarh_lead_last_saved=myenergydata.zone_kvarh_lead;
#endif
*/
#ifdef BIDIR_METER
	total_energy_lastSaved_export=myenergydata.kwh_export;
	apparent_energy_lastSaved_export = myenergydata.kvah_export;
	reactive_energy_lag_lastSaved_export = myenergydata.kvarh_lag_export;
	reactive_energy_lead_lastSaved_export= myenergydata.kvarh_lead_export;
#endif		
}

void update_energies_stored(void)
{// it is possible that kwh rolls over but kvah doesn't - 
// will this affect computation of consumption - it is likely it will and must be accounted for
// what about rollover of other energies rolling over including those of export
  if(myenergydata.kwh>999999999)  //999999.999*1000
    myenergydata.kwh = myenergydata.kwh - 999999999;

  if(myenergydata.kvah>999999999)  //999999.999*1000
    myenergydata.kvah = myenergydata.kvah - 999999999;

#ifdef BIDIR_METER
  if(myenergydata.kwh_export>999999999)  //999999.999*1000
    myenergydata.kwh_export = myenergydata.kwh_export - 999999999;

  if(myenergydata.kvah_export>999999999)  //999999.999*1000
    myenergydata.kvah_export = myenergydata.kvah_export - 999999999;
#endif

  myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);
//	R_WDT_Restart();
  if(0!=write_energies()) // important to compute checksum of enrgydata before calling write energies      
  {
		asendbare((uint8_t *)"EnergyWriteError\r\n");  	
  }
	update_saved_energies();
}

uint8_t checkForVoltageHarmonics(EM_LINE line)
{
	uint8_t retval=0;	// no harmonics
	
	if(((mytmprdata.tamperEvent_image&maskTable[MAGNET])==0)&&((mytmprdata.tamperEvent_image&maskTable[AC_MAGNET])==0))
	{// do this check only when MAGNET and AC_MAGNET is NOT THERE 
		if(EM_GetVoltageTHD(line)>0.065)
			retval=1;
	}
	return retval;
}

uint8_t checkForCurrentHarmonics(EM_LINE line)
{
	uint8_t retval=0;	// no harmonics
	if(((mytmprdata.tamperEvent_image&maskTable[MAGNET])==0)&&((mytmprdata.tamperEvent_image&maskTable[AC_MAGNET])==0))
	{// do this check only when MAGNET and AC_MAGNET is NOT THERE 
		if(EM_GetCurrentTHD(line)>0.35)
			retval=1;
	}
	return retval;
}

// following options to be used when resolution required is 2+5
uint8_t	high_resolution_kwh_value;
uint8_t	high_resolution_kvah_value;
uint8_t	high_resolution_kvarh_lag_value;
uint8_t	high_resolution_kvarh_lead_value;
#ifdef BIDIR_METER
uint8_t high_resolution_kwh_value_export;
#else
uint8_t high_resolution_kwh_fundamental_value;
#endif

void makeZeroSurveyRecordValues(void);


void makeZeroSurveyRecordValues(void)
{
	#ifdef SURVEY_USES_AVERAGE_DATA	
  average_Ir=0;	
  average_Iy=0;	
  average_Ib=0;

	average_Vr=0;
	average_Vy=0;
	average_Vb=0;
	#endif

	#ifdef SURVEY_USES_AVERAGE_PF
	averagePF_r=0;		
	averagePF_y=0;		
	averagePF_b=0;		
	#endif

	ctldata.last_saved_kwh=total_energy_lastSaved;
	ctldata.last_saved_kvah=apparent_energy_lastSaved;
		
	#ifdef SURVEY_USES_REACTIVE_DATA	
	ctldata.last_saved_kvarh_lag = reactive_energy_lag_lastSaved;
	ctldata.last_saved_kvarh_lead = reactive_energy_lead_lastSaved; 	
	#endif

// this is included in the BIDIR_METER define	
/*	
	#ifdef SURVEY_USES_EXPORT_DATA
	ctldata.last_saved_kwh_export = total_energy_lastSaved_export;
	#endif
*/
	#ifdef BIDIR_METER	
  ctldata.last_saved_kwh_export=total_energy_lastSaved_export;
  ctldata.last_saved_kvah_export=apparent_energy_lastSaved_export;
  ctldata.last_saved_kvarh_lag_export=reactive_energy_lag_lastSaved_export;
  ctldata.last_saved_kvarh_lead_export=reactive_energy_lead_lastSaved_export;
	#endif	
					
//	#ifdef SURVEY_USES_FREQ_DATA		
//	surveyData->frequency = getInstantaneousParams(IFREQS,LINE_TOTAL)*10;
//	#endif

	#ifdef SURVEY_USES_DEMAND_DATA
	ctldata.last_saved_kvah = apparent_energy_lastSaved;
	#endif

	#ifdef SURVEY_USES_AVERAGE_DATA
	nCount=0;
	#endif	
}

void per_second_activity(void)
{// this function is called only when power is there
	uint8_t previous_rtc_second;
	uint8_t previous_rtc_minute;
	
	powermgmt_mode_t power_mode;

	if(perSecondFlag!=0)
	{
		perSecondFlag=0;
		
		power_mode = POWERMGMT_GetMode();	// actual function to be used
		if (power_mode == POWERMGMT_MODE1)
		{// following block is only to be execute in MODE1
			
			rtctimer++;
			if(0==(rtctimer%RTC_READ_INTERVAL))
			{	
				previous_rtc_second = rtc.second;
				previous_rtc_minute = rtc.minute;
				
				readRTC();

				rtctimer=rtc.second;
				
				if(rtc.minute!=ctldata.minute)
				{
					genflags|=MINUTES_FLAG; // required to perform minutes activitiy
				}
				
				if((previous_rtc_second==rtc.second)&&(previous_rtc_minute==rtc.minute))
					genflags|=RTC_IS_STOPPED;
			}	// end of if(0==(rtctimer%RTC_READ_INTERVAL))

			performTamperProcessing();	// tamper processing called every second now
			
			if((tamperTimer%30)==0)
			{// do this every 30 seconds
				if(class22_status==0)
				{
					checkForBilling();	// this is highly asynchrous, but now billing could be done at any time
				}
			}	
			
			CommunicationTimeoutCheck();
			
			if((genflags&SURVEY_INTERVAL_CHANGED)!=0)
			{// if this flag is set then erase previous survey and create fresh one - some energy consumption will be lost
			// previous survey_interval and new_survey_interval can be the same
					Common_Data.survey_interval = Common_Data.new_survey_interval;
					write_common_data();

					ctldata.survey_wrt_ptr=SURVEY_BASE_ADDRESS;
					ctldata.overflow&=~SUR_OVERFLOW;
					// since survey interval has been changed we need to clear the survey data 
		      ctldata.year = rtc.year;
		      ctldata.month = rtc.month;
		      ctldata.date = rtc.date;

		      ctldata.hour = 0;
		      ctldata.minute = 0;
					
	// also update the energies etc so that only zero values will be written				
					makeZeroSurveyRecordValues();
					
			  	do_survey_update_small();
		      ctldata.hour = rtc.hour;
		      ctldata.minute = rtc.minute;
				  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
				  e2write_flags|=E2_W_CTLDATA;

					computeLoadSurveyEntries(rtc.hour,rtc.minute);
					
	//				computeDailyLogEntries();
					write_ctl_data();  	// this function creates 4 copies and clears the E2_W_CTLDATA
					genflags&=~SURVEY_INTERVAL_CHANGED;
			}
		}
	}
}




//void do_md_stuff(void)
uint8_t do_md_stuff(void)
{// this function now returns a value to indicate if update_energies_stored function has been called or not
//md is being computed for tod also  
  int32_t temp_md;  // making this int32 will not cause overflows and -ve values will get registered
	int32_t temp_power;
	int32_t tempLong;
  uint8_t tempchar;
	uint8_t retval;
  
// the if is being check outside in per_minute_activity	
//  if(((int32_t)rtc.minute%(int32_t)Common_Data.mdinterval) == 0) 
//  {// here time is a multiple of Common Data.mdinterval


	retval = 0;

// this is a better implementation

#ifdef BIDIR_METER
	if((total_active_power<3)&&(total_reactive_power<6))
	{// here there is no power (less than 3 watts) - active or reactive
		if((myenergydata.kwh!=total_energy_lastSaved)||(myenergydata.kwh_export!=total_energy_lastSaved_export))
		{
			update_energies_stored();
			retval=1;
		}
	}
	else
	{// here there is some load on the meter - could be active or reactive - use the larger power
		if(total_active_power>total_reactive_power)
		{// active power is greater
			if(((myenergydata.kwh-total_energy_lastSaved)>=KWH_INCREMENT)||((myenergydata.kwh_export-total_energy_lastSaved_export)>=KWH_INCREMENT))
			{
				update_energies_stored();
				retval=1;
			}
		}
		else
		{// reactive power is greater
		// since this is the end of the function the two variables are being used to store temp values
			temp_power = myenergydata.kvarh_lag-reactive_energy_lag_lastSaved;
			tempLong = myenergydata.kvarh_lead - reactive_energy_lead_lastSaved;
		
			if(temp_power>tempLong)
			{// lag consumption is greater
				if(((myenergydata.kvarh_lag-reactive_energy_lag_lastSaved)>=KWH_INCREMENT)||((myenergydata.kvarh_lag_export-reactive_energy_lag_lastSaved_export)>=KWH_INCREMENT))
				{
					update_energies_stored();
					retval=1;
				}
			}
			else
			{// lead consumption is greater
				if(((myenergydata.kvarh_lead-reactive_energy_lead_lastSaved)>=KWH_INCREMENT)||((myenergydata.kvarh_lead_export-reactive_energy_lead_lastSaved_export)>=KWH_INCREMENT))
				{
					update_energies_stored();
					retval=1;
				}
			}
		}
	}
#else
	if((total_active_power<3)&&(total_reactive_power<6))
	{// here there is no power (less than 3 watts) - active or reactive
		if(myenergydata.kwh!=total_energy_lastSaved)
		{
			update_energies_stored();
			retval=1;
		}
	}
	else
	{// here there is some load on the meter - could be active or reactive - use the larger power
		if(total_active_power>total_reactive_power)
		{// active power is greater
			if((myenergydata.kwh-total_energy_lastSaved)>=KWH_INCREMENT)
			{
				update_energies_stored();
				retval=1;
			}
		}
		else
		{// reactive power is greater
		// since this is the end of the function the two variables are being used to store temp values
			temp_power = myenergydata.kvarh_lag-reactive_energy_lag_lastSaved;
			tempLong = myenergydata.kvarh_lead - reactive_energy_lead_lastSaved;
		
			if(temp_power>tempLong)
			{// lag consumption is greater
				if((myenergydata.kvarh_lag-reactive_energy_lag_lastSaved)>=KWH_INCREMENT)
				{
					update_energies_stored();
					retval=1;
				}
			}
			else
			{// lead consumption is greater
				if((myenergydata.kvarh_lead-reactive_energy_lead_lastSaved)>=KWH_INCREMENT)
				{
					update_energies_stored();
					retval=1;
				}
			}
		}
	}
#endif

// this is the earlier implementation
/*
	if(total_active_power>0)
	{	
	  if(myenergydata.kwh!=total_energy_lastSaved)
		{
	    update_energies_stored();
			retval=1;
		}
	}
#ifdef BIDIR_METER	
	else
	{
	  if(myenergydata.kwh_export!=total_energy_lastSaved_export)
		{
	    update_energies_stored();
			retval=1;
		}
	}
#endif    
*/

  tempchar=60/(int32_t)Common_Data.mdinterval;

//----------------------md kw-----------------------------------------------	
  temp_md = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);

  if(temp_md<0)
    temp_md=0;  // negative values should be ignored
		
  if(temp_md>72000)
		temp_md=0;
    
  if(mymddata.mdkw < temp_md)
  {	// store max demand and its time.
    mymddata.mdkw = temp_md;
		mymddata.mdkw_datetime = rtc;
#ifndef BIDIR_METER
		mymddata.mdkw_datetime.second = 0;
#endif
    e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
  }

  if(mytoddata.mdkw < temp_md)
  {	// store max demand and its time.
    mytoddata.mdkw = temp_md;
    mytoddata.mdkw_datetime = rtc; 
#ifndef BIDIR_METER
		mytoddata.mdkw_datetime.second = 0;
#endif
    e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
  }
    
//----------------------md kva-----------------------------------------------	
  temp_md = ((myenergydata.kvah-ctldata.kvah_last_saved)*(int32_t)tempchar);

  if(temp_md<0)
    temp_md=0;  // negative values should be ignored

  if(temp_md>72000)
		temp_md=0;
		
  if(mymddata.mdkva < temp_md)
  {	// store max demand and its time.
    mymddata.mdkva = temp_md;
    mymddata.mdkva_datetime = rtc; 
#ifndef BIDIR_METER
		mymddata.mdkva_datetime.second = 0;
#endif
    e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
  }
    
  if(mytoddata.mdkva < temp_md)
  {	// store max demand and its time.
    mytoddata.mdkva = temp_md;
    mytoddata.mdkva_datetime = rtc; 
#ifndef BIDIR_METER
		mytoddata.mdkva_datetime.second = 0;
#endif
    e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
  }
  
#ifdef BIDIR_METER	
//----------------------md kw export-----------------------------------------------	
  temp_md = ((myenergydata.kwh_export-ctldata.kwh_last_saved_export)*(int32_t)tempchar);

  if(temp_md<0)
    temp_md=0;  // negative values should be ignored
    
  if(temp_md>72000)
		temp_md=0;
		
  if(mymddata.mdkw_export < temp_md)
  {	// store max demand and its time.
    mymddata.mdkw_export = temp_md;
		mymddata.mdkw_datetime_export = rtc;
    e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
  }

  if(mytoddata.mdkw_export < temp_md)
  {	// store max demand and its time.
    mytoddata.mdkw_export = temp_md;
    mytoddata.mdkw_datetime_export = rtc; 
    e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
  }
    
//----------------------md kva export-----------------------------------------------	
  temp_md = ((myenergydata.kvah_export-ctldata.kvah_last_saved_export)*(int32_t)tempchar);

  if(temp_md<0)
    temp_md=0;  // negative values should be ignored
    
  if(temp_md>72000)
		temp_md=0;
		
  if(mymddata.mdkva_export < temp_md)
  {	// store max demand and its time.
    mymddata.mdkva_export = temp_md;
    mymddata.mdkva_datetime_export = rtc; 
    e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
  }
    
//  if(mytoddata.mdkva < temp_md)	// here is the bug it should be mytoddata.mdkva_export
  if(mytoddata.mdkva_export < temp_md)	// fixed
  {	// store max demand and its time.
    mytoddata.mdkva_export = temp_md;
    mytoddata.mdkva_datetime_export = rtc; 
    e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
  }
#endif	
  ctldata.kwh_last_saved=myenergydata.kwh;
  ctldata.kvah_last_saved=myenergydata.kvah;
#ifdef BIDIR_METER	
  ctldata.kwh_last_saved_export=myenergydata.kwh_export;
  ctldata.kvah_last_saved_export=myenergydata.kvah_export;
#endif	
  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
  e2write_flags|=E2_W_CTLDATA;
	return retval;
}


#ifdef SURVEY_USES_AVERAGE_DATA
void perform_VI_averaging(void)
{
	int32_t accumulator;
	int32_t tempint;

//	tempint = (uint32_t)(EM_GetVoltageRMS(LINE_PHASE_R)*10);
  tempint = (int32_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_R)*10.0);	
	accumulator = ((int32_t)average_Vr*(int32_t)nCount + tempint);
	average_Vr = accumulator/((int32_t)(nCount+1));
	
//	tempint = (uint32_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*10);
  tempint = (int32_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*10.0);	
	accumulator = ((int32_t)average_Vy*(int32_t)nCount + tempint);
	average_Vy = accumulator/((int32_t)(nCount+1));
	
//	tempint = (uint32_t)(EM_GetVoltageRMS(LINE_PHASE_B)*10);
  tempint = (int32_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_B)*10.0);	
	accumulator = ((int32_t)average_Vy*(int32_t)nCount + tempint);
	average_Vb = accumulator/((int32_t)(nCount+1));
	
//	tempint = (uint32_t)(EM_GetCurrentRMS(LINE_PHASE_R)*100);
	tempint = (int32_t)(getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*100.0);
	accumulator = ((int32_t)average_Ir*(int32_t)nCount + tempint);
	average_Ir = accumulator/((int32_t)(nCount+1));
	
//	tempint = (uint32_t)(EM_GetCurrentRMS(LINE_PHASE_Y)*100);
	tempint = (int32_t)(getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*100.0);
	accumulator = ((int32_t)average_Iy*(int32_t)nCount + tempint);
	average_Iy = accumulator/((int32_t)(nCount+1));
	
//	tempint = (uint32_t)(EM_GetCurrentRMS(LINE_PHASE_B)*100);
	tempint = (int32_t)(getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*100.0);
	accumulator = ((int32_t)average_Ib*(int32_t)nCount + tempint);
	average_Ib = accumulator/((int32_t)(nCount+1));

//	tempint = EM_GetPowerFactor(LINE_PHASE_R)*100;	// 2 decimals
  tempint = (int32_t)(getInstantaneousParams(IPFS,LINE_PHASE_R)*100.0);
	if(tempint<0)
		tempint*=-1;
	accumulator = ((int32_t)averagePF_r*(int32_t)nCount + tempint);
	averagePF_r = accumulator/((int32_t)(nCount+1));
	
//	tempint = EM_GetPowerFactor(LINE_PHASE_Y)*100;	// 2 decimals
  tempint = (int32_t)(getInstantaneousParams(IPFS,LINE_PHASE_Y)*100.0);
	if(tempint<0)
		tempint*=-1;
	accumulator = ((int32_t)averagePF_y*(int32_t)nCount + tempint);
	averagePF_y = accumulator/((int32_t)(nCount+1));
	
//	tempint = EM_GetPowerFactor(LINE_PHASE_B)*100;	// 2 decimals
  tempint = (int32_t)(getInstantaneousParams(IPFS,LINE_PHASE_B)*100.0);
	if(tempint<0)
		tempint*=-1;
	accumulator = ((int32_t)averagePF_b*(int32_t)nCount + tempint);
	averagePF_b = accumulator/((int32_t)(nCount+1));

//	sprintf(tbuf, "%d ",tempint);
//	asendbare(tbuf);
	
	nCount++;
}
#endif

void updateBothPresentZoneTodEnergies(void)
{
// The myenergydata now stores the cumulative energies - hence no addition
// is required to be made to mytoddata energies
// instead the myenergydata zone energies are to be directly copied
	  mytoddata.kwh=myenergydata.zone_kwh;
	  mytoddata.kvah=myenergydata.zone_kvah;

#ifdef TOD_HAS_KVARH_LAG
	mytoddata.kvarh_lag=myenergydata.zone_kvarh_lag;
#endif
		
#ifdef BIDIR_METER
	  mytoddata.kwh_export=myenergydata.zone_kwh_export;
	  mytoddata.kvah_export=myenergydata.zone_kvah_export;
#endif		
}

void updateEnergyDataZoneEnergies(void)
{// we have just read the new TOD zone energies into mytodata - we must update the myenergydata
// this function is now a revised version of the earlier one

// The mytoddata  now stores the cumulative energies - hence no addition/subtraction
// is required to be made to myenergydata energies
// instead the mytoddata zone energies are directly copied to the myenergydata zone energies
	  myenergydata.zone_kwh=mytoddata.kwh;
	  myenergydata.zone_kvah=mytoddata.kvah;

#ifdef TOD_HAS_KVARH_LAG
	myenergydata.zone_kvarh_lag=mytoddata.kvarh_lag;
#endif
		
#ifdef BIDIR_METER
	myenergydata.zone_kwh_export=mytoddata.kwh_export;
	myenergydata.zone_kvah_export=mytoddata.kvah_export;
#endif		

}
/*
void updateEnergyDataZoneEnergies(void)
{		
//	myenergydata.zone_kwh-=zone_kwh_last_saved;
//  myenergydata.zone_kvah-=zone_kvah_last_saved;
//#ifdef TOD_HAS_REACTIVE_ENERGIES	
//  myenergydata.zone_kvarh_lag-=zone_kvarh_lag_last_saved;
//  myenergydata.zone_kvarh_lead-=zone_kvarh_lead_last_saved;
//#endif

//#ifdef BIDIR_METER	
//  myenergydata.zone_kwh_export-=zone_kwh_last_saved_export;
//  myenergydata.zone_kvah_export-=zone_kvah_last_saved_export;
//  myenergydata.zone_kvarh_lag_export-=zone_kvarh_lag_last_saved_export;
//  myenergydata.zone_kvarh_lead_export-=zone_kvarh_lead_last_saved_export;
//#endif	
}

void clearZoneLastSavedEnergies(void)
{
  zone_kwh_last_saved=0;
  zone_kvah_last_saved=0;
  zone_kvarh_lag_last_saved=0;
  zone_kvarh_lead_last_saved=0;
#ifdef BIDIR_METER	
  zone_kwh_last_saved_export=0;
  zone_kvah_last_saved_export=0;
  zone_kvarh_lag_last_saved_export=0;
  zone_kvarh_lead_last_saved_export=0;
#endif	
}
*/

void per_minute_activity(void)
{// all minute related tasks will be done here

  int32_t templong;
  uint8_t tempzone;
	uint8_t energiesUpdatedFlag;

#ifdef CTL_FAULT_DEBUG
	if(0!=(ctldata.overflow&FAULT_FLAG))
		return;
#endif
	
  ctldata.minute = rtc.minute;
  myenergydata.powerOnminutes_eb++;	// from now on this will store the cumulative value 04/02/2024
  ctldata.powerOnminutes_eb++;	// ctldata.powerOnminutes_eb - for the month 12/02/2024

#ifdef DAILY_POWER_ON_LOGGING	
	myenergydata.dailyPowerOnMinutes++;
#endif

  myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);	

// we can perform the averages of voltages and currents here
#ifdef SURVEY_USES_AVERAGE_DATA
	perform_VI_averaging();
#endif  

	energiesUpdatedFlag=0;

  if(((int32_t)rtc.minute%(int32_t)Common_Data.mdinterval) == 0) 
  {// here time is a multiple of Common Data.mdinterval
  	energiesUpdatedFlag=do_md_stuff();  // including the tod md also - this function sets the E2_W_CTLDATA flag
	}
	
  if(e2write_flags&E2_W_ZONEDATA)
  {// this data needs to be written in the appropriate zone location
    write_alternately((TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (uint8_t*)&mytoddata, sizeof(mytoddata));
    e2write_flags&=~E2_W_ZONEDATA;
  }
  
  if(((int32_t)rtc.minute%(int32_t)Common_Data.survey_interval) == 0) 
	{	
		if(energiesUpdatedFlag==0)
	    update_energies_stored();	// this is to record the latest energy	
		
		do_survey_stuff_small();
		computeLoadSurveyEntries(rtc.hour,rtc.minute);	// don't use ctldata.hour - it is updated below
	  e2write_flags|=E2_W_CTLDATA;
	} 

// check to see if mdinterval or survey interval need to be changed - only to be done when minutes are 00 or 30

  if(((int32_t)rtc.minute%(int32_t)30) == 0)
	{
		if(Common_Data.mdinterval!=Common_Data.new_mdinterval)
		{
			Common_Data.mdinterval = Common_Data.new_mdinterval;
			e2write_flags|=E2_W_IMPDATA;
		}
// survey interval change will be implemented as soon as it has been command
// all survey ptrs will be initialised - and survey data will be written afresh
// the action now takes place in per_second_activity
//		if(Common_Data.survey_interval!=Common_Data.new_survey_interval)
//		{
//			Common_Data.survey_interval = Common_Data.new_survey_interval;
//			e2write_flags|=E2_W_IMPDATA;
//		}
	}

  if(e2write_flags&E2_W_IMPDATA)
  {// this data needs to be written in the appropriate zone location
		write_common_data();	// this function clears the IMPDATA flag
  }
	
  tempzone=findZone(&rtc); // there is only one presentzone
  if(presentZone!=tempzone)
  {// here the zone has changed
		update_energies_stored();	// nothing happens to the zone energies
		updateBothPresentZoneTodEnergies();	// zone energies in energydata transferred to mytoddata
		verifyTOD_energies(presentZone); 
    write_alternately((TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (uint8_t*)&mytoddata, sizeof(mytoddata));
//		updateEnergyDataZoneEnergies();
//		clearZoneLastSavedEnergies();
		
    presentZone=tempzone; // now present zone is the new zone
    read_alternately(TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC, (uint8_t*)&mytoddata, sizeof(mytoddata));
		updateEnergyDataZoneEnergies();	// copy the tod energy data into the myenergydata zone energies

    myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);
    write_energies(); // new zone has started - make a copy of the energy data      
	}
  
  ctldata.hour = rtc.hour;
  ctldata.date = rtc.date;
	
  if(e2write_flags&E2_W_CURRENTDATA)	// save current meter data if flags are set
  {
    write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
    e2write_flags&=~E2_W_CURRENTDATA;
  }

  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);	// line added on 27/Dec/2021

  if(ctldata.minute != 0)
	{
	  if(e2write_flags&E2_W_CTLDATA)
	  {
			write_ctl_data();
	  }
    return;
	}
// this is the hours area and it is synchronous with rtc.hours

  ctldata.month = rtc.month;
  ctldata.year = rtc.year;
	ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
	
  if(rtc.hour==0)
  {// this is a new day
    writeLogData(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second);
	  e2write_flags|=E2_W_CTLDATA;
    computeDailyLogEntries();
		write_ctl_backup_data(0);	// this function will save the important values of ctl_data at 00:00 hours
  }

		write_ctl_data();	// this is perfectly fine - we reach this place every hour
}

void generate_bill(uint8_t dat, uint8_t mth, uint8_t yr, uint8_t hrs, uint8_t mnts, uint8_t secs, uint8_t bill_type)
{
	ext_rtc_t bdate;
	uint16_t wcount;
	
	uint16_t int_year;
	
	int_year = 2000+yr;
	wcount = CalcDayNumFromDate(int_year,mth,dat);
	
	bdate.second = secs;
	bdate.minute = mnts;
	bdate.hour = hrs;
	bdate.day = (uint8_t)wcount;
	bdate.date = dat;
	bdate.month = mth;
	bdate.year = yr;

	ctldata.cumulative_md+=(uint32_t)mymddata.mdkw;
	ctldata.cumulative_md_kva+=(uint32_t)mymddata.mdkva;
  write_history_data(bdate);	// write the composite billdata - both import and export
	performEndJobsForBilling();
  clearMymeterData(); // this will also clear the current TOD_registers in E2rom
/* 29/Feb/2024	
// it is important to ensure that once bill is made, it is to be treated that after bill generation, even if the 
// present tariff zone continues, as may happen during md resets, then any earlier information about md generation
// should be nulled to prevent a new md being created immediately thereafter.
// This implies that the following action will ensure that new mds are made from this point onwards
// what are the things that happen after a bill has been made
// depends on whether is was made
	a) at 00:00 hours on the first day of the new month
	b) any other arbitrary time
	In both these cases - it is possible that the tariff zone remains the same before and after the bill is made

*/


	ctldata.kwh_last_saved=myenergydata.kwh;
	ctldata.kvah_last_saved=myenergydata.kvah;
	
#if BIDIR_METER
	ctldata.kwh_last_saved_export=myenergydata.kwh_export;
	ctldata.kvah_last_saved_export=myenergydata.kvah_export;
#endif
// this should solve the issue Shubham is reporting

// we have not saved ctldata yet

  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
  e2write_flags|=E2_W_CTLDATA;
	write_ctl_data();
	
	computeBillingEntries();
	
// this must not be done always ie for every bill that is made
// rather it should only be done for automatic bills
	if(bill_type==BT_AUTO)
	{
		computeNextBillDate();
		class22_status=0;	// no single action is to be performed
		write_single_action_activation_data(class22_status);
	}
// do one last thing before leaving
	update_ctl_backup_data();	// this function will update the cumulative md values in the ctldata_backup
	
}

void do_initial_things(void)
{
// we need to load the earlier (prior to power failure) todata for the then zone,
// then determine if zone has changed and if so then perform all the zone change related activities here
  
  
// last action includes 
//  - load_survey update(half hour or more)
//  - daily log update(one day or more)
//  - md update(md interval or more)
//  - tod update(md interval or more)
//  - billing update(one month or more)
// the objective is to determine if some prescribed time interval has been crossed during the
// power failure period and complete the action pertaining to that period.
  timeinfo_t lastTime;  // minutes,hours,day,date,month,year - use this so that existing single phase incSurveyDate may be used.
  ext_rtc_t rtcLast;
	
  int32_t temp_md;
//  unsigned int temp_md_kva;
  uint8_t tempchar;
  uint8_t doMD = 0;
  
  uint8_t this_interval;
  uint8_t prev_interval;
  	
// make a copy of ctldata time stamp
  lastTime.minute=ctldata.minute;
  lastTime.hour=ctldata.hour;
  lastTime.date=ctldata.date;
  lastTime.month=ctldata.month;
  lastTime.year=ctldata.year;
	
// also copy in rtcLast
	rtcLast.second=0;
	rtcLast.minute=ctldata.minute;
	rtcLast.hour=ctldata.hour;
	rtcLast.date=ctldata.date;
	rtcLast.month=ctldata.month;
	rtcLast.year=ctldata.year;
	rtcLast.day=(uint8_t)CalcDayNumFromDate((2000+rtcLast.year), rtcLast.month, rtcLast.date);	// this is correct
	

// get the toddata corresponding to last time
  
// now read the tod data also
  presentZone=findZone(&rtcLast);
	
  if(read_alternately(TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC, (uint8_t*)&mytoddata, sizeof(mytoddata))==0)
	{// none of the locations had valid data
    clear_structure((uint8_t*)&mytoddata,sizeof(mytoddata)); // this function fills zero bytes in the structure
	}
// we now have the zonedata at the time of power failure - what about the zone energies?
  
  this_interval = rtc.minute/(int32_t)Common_Data.mdinterval;
  prev_interval = lastTime.minute/(int32_t)Common_Data.mdinterval;

  if((lastTime.year!=rtc.year)||(lastTime.month!=rtc.month)||(lastTime.date!=rtc.date)||(lastTime.hour!=rtc.hour)||(this_interval!=prev_interval))
  {// here we know that 30 minute boundary has been crossed in no power mode.
    doMD = 1;
    lastTime.minute=prev_interval*Common_Data.mdinterval;
  }

//----------------- do the md bit ---------------------------------------------
  if(doMD == 1)
  {// here we have no further use of doMD so we can use it in place of tempchar
    tempchar=60/(int32_t)Common_Data.mdinterval;

//----------------- mdkw ---------------------------
		
    temp_md = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);
    if(temp_md<0)
      temp_md=0;  // negative values should be ignored
			
    if(temp_md>72000)
      temp_md=0;  // >max values are to be zeroed
			
    
// increment the date time of the lastTime to align it with the actual md interval for which the data is being stored    
    incDate((uint8_t*)&lastTime.minute,Common_Data.mdinterval);
    
    if(mymddata.mdkw < temp_md)
    {	// store max demand and its time.
// we now need to compute the end interval time
      mymddata.mdkw = temp_md;
      mymddata.mdkw_datetime.date = lastTime.date; 
      mymddata.mdkw_datetime.month = lastTime.month;
      mymddata.mdkw_datetime.year = lastTime.year;
      mymddata.mdkw_datetime.hour = lastTime.hour;
      mymddata.mdkw_datetime.minute = lastTime.minute;
      mymddata.mdkw_datetime.second = 0;
      mymddata.mdkw_datetime.day = 0;
			
      e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
    }

    if(mytoddata.mdkw < temp_md)
    {	// store max demand and its time.
      mytoddata.mdkw = temp_md;
      mytoddata.mdkw_datetime.date = lastTime.date; 
      mytoddata.mdkw_datetime.month = lastTime.month;
      mytoddata.mdkw_datetime.year = lastTime.year;
      mytoddata.mdkw_datetime.hour = lastTime.hour;
      mytoddata.mdkw_datetime.minute = lastTime.minute;
      mytoddata.mdkw_datetime.second = 0;
      mytoddata.mdkw_datetime.day = 0;
			
      e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
    }
    ctldata.kwh_last_saved=myenergydata.kwh;  // the saving is done below

//----------------- mdkva ---------------------------
// now do the same for md kva - use the variable temp_md itself
    temp_md = ((myenergydata.kvah-ctldata.kvah_last_saved)*(int32_t)tempchar);
    if(temp_md<0)
      temp_md=0;  // negative values should be ignored
    
    if(temp_md>72000)
      temp_md=0;  // >max values are to be zeroed
			
    if(mymddata.mdkva < temp_md)
    {	// store max demand and its time.
// we now need to compute the end interval time
      mymddata.mdkva = temp_md;
// the date time has already been incremented above      
      mymddata.mdkva_datetime.date = lastTime.date; 
      mymddata.mdkva_datetime.month = lastTime.month;
      mymddata.mdkva_datetime.year = lastTime.year;
      mymddata.mdkva_datetime.hour = lastTime.hour;
      mymddata.mdkva_datetime.minute = lastTime.minute;
      mymddata.mdkva_datetime.second = 0;
      mymddata.mdkva_datetime.day = 0;
      e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
    }

    if(mytoddata.mdkva < temp_md)
    {	// store max demand and its time.
      mytoddata.mdkva = temp_md;
      mytoddata.mdkva_datetime.date = lastTime.date; 
      mytoddata.mdkva_datetime.month = lastTime.month;
      mytoddata.mdkva_datetime.year = lastTime.year;
      mytoddata.mdkva_datetime.hour = lastTime.hour;
      mytoddata.mdkva_datetime.minute = lastTime.minute;
      mytoddata.mdkva_datetime.second = 0;
      mytoddata.mdkva_datetime.day = 0;
      e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
    }
    ctldata.kvah_last_saved=myenergydata.kvah;  // the saving is done below

#ifdef BIDIR_METER		
//----------------- mdkw export ---------------------------
    temp_md = ((myenergydata.kwh_export-ctldata.kwh_last_saved_export)*(int32_t)tempchar);
    if(temp_md<0)
      temp_md=0;  // negative values should be ignored
    
    if(temp_md>72000)
      temp_md=0;  // >max values are to be zeroed
			
    if(mymddata.mdkw_export < temp_md)
    {	// store max demand and its time.
// we now need to compute the end interval time
      mymddata.mdkw_export = temp_md;
      mymddata.mdkw_datetime_export.date = lastTime.date; 
      mymddata.mdkw_datetime_export.month = lastTime.month;
      mymddata.mdkw_datetime_export.year = lastTime.year;
      mymddata.mdkw_datetime_export.hour = lastTime.hour;
      mymddata.mdkw_datetime_export.minute = lastTime.minute;
      mymddata.mdkw_datetime_export.second = 0;
      mymddata.mdkw_datetime_export.day = 0;
			
      e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
    }

    if(mytoddata.mdkw_export < temp_md)
    {	// store max demand and its time.
      mytoddata.mdkw_export = temp_md;
      mytoddata.mdkw_datetime_export.date = lastTime.date; 
      mytoddata.mdkw_datetime_export.month = lastTime.month;
      mytoddata.mdkw_datetime_export.year = lastTime.year;
      mytoddata.mdkw_datetime_export.hour = lastTime.hour;
      mytoddata.mdkw_datetime_export.minute = lastTime.minute;
      mytoddata.mdkw_datetime_export.second = 0;
      mytoddata.mdkw_datetime_export.day = 0;
			
      e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
    }
    ctldata.kwh_last_saved_export=myenergydata.kwh_export;  // the saving is done below		
		
//----------------- mdkva export ---------------------------
    temp_md = ((myenergydata.kvah_export-ctldata.kvah_last_saved_export)*(int32_t)tempchar);
    if(temp_md<0)
      temp_md=0;  // negative values should be ignored

    if(temp_md>72000)
      temp_md=0;  // >max values are to be zeroed
			
    if(mymddata.mdkva_export < temp_md)
    {	// store max demand and its time.
// we now need to compute the end interval time
      mymddata.mdkva_export = temp_md;
// the date time has already been incremented above      
      mymddata.mdkva_datetime_export.date = lastTime.date; 
      mymddata.mdkva_datetime_export.month = lastTime.month;
      mymddata.mdkva_datetime_export.year = lastTime.year;
      mymddata.mdkva_datetime_export.hour = lastTime.hour;
      mymddata.mdkva_datetime_export.minute = lastTime.minute;
      mymddata.mdkva_datetime_export.second = 0;
      mymddata.mdkva_datetime_export.day = 0;
      e2write_flags|=E2_W_CURRENTDATA;  // set flag to save current meter data
    }

    if(mytoddata.mdkva_export < temp_md)
    {	// store max demand and its time.
      mytoddata.mdkva_export = temp_md;
      mytoddata.mdkva_datetime_export.date = lastTime.date; 
      mytoddata.mdkva_datetime_export.month = lastTime.month;
      mytoddata.mdkva_datetime_export.year = lastTime.year;
      mytoddata.mdkva_datetime_export.hour = lastTime.hour;
      mytoddata.mdkva_datetime_export.minute = lastTime.minute;
      mytoddata.mdkva_datetime_export.second = 0;
      mytoddata.mdkva_datetime_export.day = 0;
      e2write_flags|=E2_W_ZONEDATA;  // set flag to save current zone data
    }
		
    ctldata.kvah_last_saved_export=myenergydata.kvah_export;  // the saving is done below
#endif
  }
	else
	{// here doMD !=1
	// this implies that ctldata would not have got updated
	// it is likely that ctldata.kwh_last_saved and other fellows may be > myenerydata.kwh
	// this will lead to problems while computing rising demand - Hence following check is required
		if(myenergydata.kwh<ctldata.kwh_last_saved)
			ctldata.kwh_last_saved=myenergydata.kwh;
			
		if(myenergydata.kvah<ctldata.kvah_last_saved)
    	ctldata.kvah_last_saved=myenergydata.kvah;

#ifdef BIDIR_METER		
		if(myenergydata.kwh_export<ctldata.kwh_last_saved_export)
			ctldata.kwh_last_saved_export=myenergydata.kwh_export;

		if(myenergydata.kvah_export<ctldata.kvah_last_saved_export)
    	ctldata.kvah_last_saved_export=myenergydata.kvah_export;
#endif
	}

  if(e2write_flags&E2_W_ZONEDATA)
  {// this data needs to be written in the appropriate zone location - tod energies are still the old values
    write_alternately((TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (uint8_t*)&mytoddata, sizeof(mytoddata));
    e2write_flags&=~E2_W_ZONEDATA;
  }
  
// see if the zone has changed
  tempchar=findZone(&rtc); // there is only one presentzone
	
  if(presentZone!=tempchar)
  {// here the zone has changed
		update_energies_stored();
		updateBothPresentZoneTodEnergies(); // energydata zone energies are copied to mytoddata energies 
		verifyTOD_energies(presentZone);
    write_alternately((TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC), (uint8_t*)&mytoddata, sizeof(mytoddata));
//		updateEnergyDataZoneEnergies();
//    clearZoneLastSavedEnergies();
		
//    myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);
//    write_energies(); // important to compute checksum of enrgydata before calling write energies      
    
    presentZone=tempchar; // now present zone is the new zone
    read_alternately(TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+presentZone*ZONE_PTR_INC, (uint8_t*)&mytoddata, sizeof(mytoddata));
		updateEnergyDataZoneEnergies();	// copy the tod energy data into the myenergydata zone energies

    myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);
    write_energies(); // new zone has started - make a copy of the energy data      
	}
  
  if(e2write_flags&E2_W_CURRENTDATA)
  {
    write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
    e2write_flags&=~E2_W_CURRENTDATA;
  }
//-----------------------------------------------------------------------------  
// now do the billing bit
// we should ideally be generating only one bill
// this is the code for arbitrary bill_date  
	checkForBilling();	// This function replaces all that is written below by using a common approach
// end of billing stuff
//-----------------------------------------------------------------------------  

  if(e2write_flags&E2_W_CURRENTDATA)
  {
	  write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
	  e2write_flags&=~E2_W_CURRENTDATA;
	}

	do_survey_update_small();

	do_log_update();	// must be called only for regular meters
	
// update the ctldata record    
  ctldata.year=rtc.year;
  ctldata.month=rtc.month;
  ctldata.date=rtc.date;
  ctldata.hour=rtc.hour;
  ctldata.minute=rtc.minute;
	
	
// we need to compute the CurrentEntry and EntriesInUse
	computeLoadSurveyEntries(rtc.hour,rtc.minute);
	computeDailyLogEntries();
	computeBillingEntries();

  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
  e2write_flags|=E2_W_CTLDATA;
	
	write_ctl_data();	// this function creates 2 copies and clears the E2_W_CTLDATA
// also make a backup of ctldata(?)
// we always come to do initial things after init ctl data has been read hence there is
// no necessity to perform the following operation
//	write_ctl_backup_data();
}

// this function can be called anytime to check if bill is required to be made
// the next date on which the bill is to be made is stored in g_Class22_billdate
// this value will also be available by default
void checkForBilling(void)
{
	uint8_t year;
	uint8_t makeBill = 0;
	if(class22_status==0)
	{// if current rtc is less than the value provided in g_Class22_billdate and g_Class22_billtime do nothing
		// else generate a bill
		year = (uint8_t)((g_Class22_billdate.year_high*(int16_t)256 + g_Class22_billdate.year_low) - (int16_t)2000);
		
		if(rtc.year>year)
			makeBill=1;
		else
		{// here rtc.year could be equal or less
			if(rtc.year==year)
			{
				if(rtc.month>g_Class22_billdate.month)
					makeBill=1;
				else
				{// here rtc month could be equal or less
					if(rtc.month==g_Class22_billdate.month)
					{
						if(rtc.date>g_Class22_billdate.day_of_month)
							makeBill=1;
						else
						{// here rtc.date could be equal or less
							if(rtc.date==g_Class22_billdate.day_of_month)
							{// now we need to check the time
								if(rtc.hour>g_Class22_billtime.hour)
									makeBill=1;
								else
								{// here rtc.hour could be equal or less
									if(rtc.hour==g_Class22_billtime.hour)
									{
										if(rtc.minute>g_Class22_billtime.minute)
											makeBill=1;
										else
										{// here rtc.minutes could be equal or less
											if(rtc.minute==g_Class22_billtime.minute)
											{
												if(rtc.second>=g_Class22_billtime.second)
													makeBill=1;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if(makeBill!=0)
		{
			generate_bill(g_Class22_billdate.day_of_month, g_Class22_billdate.month, year, g_Class22_billtime.hour, g_Class22_billtime.minute, g_Class22_billtime.second, BT_AUTO);
		}
	}
//	else
//	{// do nothing - it will get done while handling single action schedule
//	}
}

// this function creates the next bill date
void computeNextBillDate(void)
{
	int32_t secondsRTC;
	int32_t secondsOther;

	int16_t yr;
	
	readRTC();
	yr = 2000+rtc.year;
	
	if(class22_status==255)
	{// this is only for the first time
		g_Class22_billdate.day_of_month=1;
		if(Common_Data.billing_month==0)
		{
			g_Class22_billdate.month=rtc.month+1;	// monthly billing
		}
		else
		{// bimonthly billing
			g_Class22_billdate.month=rtc.month+2;
		}
		
		if(g_Class22_billdate.month>12)
		{
			g_Class22_billdate.month-=12;
			yr++;
		}
		
		g_Class22_billdate.year_high = yr>>8;
		g_Class22_billdate.year_low = yr&0xff;

		g_Class22_billdate.day_of_week = 0xFF; //NOT_SPECIFIED
		
		g_Class22_billtime.hour=0;
		g_Class22_billtime.minute=0;
		g_Class22_billtime.second=0;
		g_Class22_billtime.hundredths = 0xFF;	//NOT_SPECIFIED
		class22_status=0;
	}
	else
	{// here class22_status could be 0 or 1
		switch(Common_Data.billing_month)
		{// 0 - monthly, 1-odd, 2-even
			case 0:	// monthly billing - could be this month or next
				// for monthly cases also bill could be made in this month
				if(rtc.date>g_Class22_billdate.day_of_month)
				{// bill must definitely made next month
					g_Class22_billdate.month=rtc.month+1;
					if(g_Class22_billdate.month>12)
					{
						g_Class22_billdate.month-=12;
						yr++;
					}
				}
				else
				{
					if(rtc.date<g_Class22_billdate.day_of_month)
					{// bill can be made in this month itself
						g_Class22_billdate.month=rtc.month;
					}
					else
					{// here rtc.date == g_Class22_billdate.day_of_month
						secondsRTC = (((int32_t)rtc.hour*(int32_t)60)+(int32_t)rtc.minute)*(int32_t)60 + (int32_t)rtc.second;
						secondsOther = (((int32_t)g_Class22_billtime.hour*(int32_t)60)+ (int32_t)g_Class22_billtime.minute)*(int32_t)60 +(int32_t)g_Class22_billtime.second;
						if(secondsRTC>secondsOther)
						{// time has past - bill to be made after one month
							g_Class22_billdate.month=rtc.month+1;
							if(g_Class22_billdate.month>12)
							{
								g_Class22_billdate.month-=12;
								yr++;
							}
						}
						else
						{// bill can be made in this month itself
							g_Class22_billdate.month=rtc.month;
						}
					}
				}
				break;
		
			case 1:	// odd month billing
				if((rtc.month%2)==0)
				{// currently the month is even - hence bill to be made next month
					g_Class22_billdate.month=rtc.month+1;
					if(g_Class22_billdate.month>12)
					{
						g_Class22_billdate.month-=12;
						yr++;
					}
				}
				else
				{// currently the month is odd - hence bill to be made after 2 months or in this month itself
					if(rtc.date>g_Class22_billdate.day_of_month)
					{// billing date is already crossed for this month hence it must be done 2 months later
						g_Class22_billdate.month=rtc.month+2;
						if(g_Class22_billdate.month>12)
						{
							g_Class22_billdate.month-=12;
							yr++;
						}
					}
					else
					{// month is odd
						if(rtc.date<g_Class22_billdate.day_of_month)
						{// odd month billing is required and rtc.date is less than g_Class22_billdate.day_of_month
						// bill could be made in this month itself
							g_Class22_billdate.month=rtc.month;
						}
						else
						{// here rtc.date==g_Class22_billdate.day_of_month
							secondsRTC = (((int32_t)rtc.hour*(int32_t)60)+(int32_t)rtc.minute)*(int32_t)60 + (int32_t)rtc.second;
							secondsOther = (((int32_t)g_Class22_billtime.hour*(int32_t)60)+ (int32_t)g_Class22_billtime.minute)*(int32_t)60 +(int32_t)g_Class22_billtime.second;
							if(secondsRTC>secondsOther)
							{// time has past - bill to be made after two months
								g_Class22_billdate.month=rtc.month+2;
								if(g_Class22_billdate.month>12)
								{
									g_Class22_billdate.month-=12;
									yr++;
								}
							}
							else
							{// bill can be made in this month itself
								g_Class22_billdate.month=rtc.month;
							}
						}
					}
				}
				break;
				
			case 2:	// even month billing
				if((rtc.month%2)!=0)
				{// currently the month is odd - hence bill to be made next month
					g_Class22_billdate.month=rtc.month+1;
					if(g_Class22_billdate.month>12)
					{
						g_Class22_billdate.month-=12;
						yr++;
					}
				}
				else
				{// currently the month is even - hence bill to be made after 2 months or his month itself
					if(rtc.date>g_Class22_billdate.day_of_month)
					{// billing date is crossed hence it must be done 2 months later
						g_Class22_billdate.month=rtc.month+2;
						if(g_Class22_billdate.month>12)
						{
							g_Class22_billdate.month-=12;
							yr++;
						}
					}
					else
					{// month is even
						if(rtc.date<g_Class22_billdate.day_of_month)
						{// even month billing reqd & rtc.date < g_Class22_billdate.day_of_month
						// bill could be made in this month itself
							g_Class22_billdate.month=rtc.month;
						}
						else
						{// here rtc.date==g_Class22_billdate.day_of_month
							secondsRTC = (((int32_t)rtc.hour*(int32_t)60)+(int32_t)rtc.minute)*(int32_t)60 + (int32_t)rtc.second;
							secondsOther = (((int32_t)g_Class22_billtime.hour*(int32_t)60)+ (int32_t)g_Class22_billtime.minute)*(int32_t)60 +(int32_t)g_Class22_billtime.second;
							if(secondsRTC>secondsOther)
							{// time has past - bill to be made after two months
								g_Class22_billdate.month=rtc.month+2;
								if(g_Class22_billdate.month>12)
								{
									g_Class22_billdate.month-=12;
									yr++;
								}
							}
							else
							{// bill can be made in this month itself
								g_Class22_billdate.month=rtc.month;
							}
						}
					}
				}
				break;
				
			default:
				break;
		}
		
		g_Class22_billdate.year_high = yr>>8;
		g_Class22_billdate.year_low = yr&0xff;
		g_Class22_billdate.day_of_week = 0xFF; //NOT_SPECIFIED
// the following time variables must not be changed - since they are not affected by monthly/odd/even billing		
//		g_Class22_billtime.hour=0;
//		g_Class22_billtime.minute=0;
//		g_Class22_billtime.second=0;
//		g_Class22_billtime.hundredths = 0xFF;	//NOT_SPECIFIED
	}
}





