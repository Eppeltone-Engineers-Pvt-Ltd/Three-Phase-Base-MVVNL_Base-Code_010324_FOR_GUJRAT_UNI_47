// em_display.c
// Driver layer 
#include "r_cg_macrodriver.h"
#include "typedef.h"            /* GCSE Standard definitions */
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "emeter3_structs.h"

// Application layer 
#include "platform.h"
#include "r_lcd_display.h"      /* LCD Display Application Layer */
#include "r_lcd_config.h"
#include "em_display.h"         /* EM Display Application Layer */
#include "event.h"
#include "emeter3_app.h"
#include "memoryOps.h"
#include "e2rom.h"
#include "tampers.h"
#include "utilities.h"
#include "r_dlms_obis_ic.h"
#include "em_display_TN3P_Helper.h"
#include "powermgmt.h"

// Display tamper condition
#include "format.h"
#include "eeprom.h"
#include "em_core.h"


extern float32_t operatingPF;

extern date_time_t g_Class03_BillingDate;
extern display_int_flag_t g_display_flag;

#define MDKW 0
#define MDKVA 1

#define MDDATE 0
#define MDTIME 1

// for selecting KVARH lag/lead or total
#define D_LAG 0
#define D_LEAD 1
#define D_TOTAL 2


const uint8_t msg_nil_tamper[8] = {LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CHAR_N, LCD_CHAR_I, LCD_CHAR_L, LCD_CLEAR_DIGIT};
const uint8_t msg_hv_tamper[8] = {LCD_CHAR_H, LCD_CHAR_V, LCD_MINUS_SIGN, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t msg_lv_tamper[8] = {LCD_CHAR_L, LCD_CHAR_V, LCD_MINUS_SIGN, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t voltage_fail_tamper[8] = {LCD_CLEAR_DIGIT, LCD_CHAR_V, LCD_CHAR_F, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};	// posn 0 to be replaced with r,y,b
const uint8_t voltage_unbalance_tamper[8] = {LCD_CHAR_P, LCD_CHAR_T, LCD_CHAR_U, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t current_fail_tamper[8] = {LCD_CLEAR_DIGIT, LCD_CHAR_C, LCD_CHAR_F, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};	// posn 0 to be replaced with r,y,b
const uint8_t current_unbalance_tamper[8] = {LCD_CHAR_C, LCD_CHAR_T, LCD_CHAR_U, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t current_bypass_tamper[8] = {LCD_CHAR_B, LCD_CHAR_Y, LCD_CHAR_P, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t magnet_indication_tamper[8]={LCD_CHAR_M, LCD_CHAR_G, LCD_CHAR_T, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t magnet_tamper_nil[8] = {LCD_CHAR_M, LCD_CHAR_G, LCD_CHAR_T, LCD_MINUS_SIGN, LCD_CHAR_N, LCD_CHAR_I, LCD_CHAR_L, LCD_CLEAR_DIGIT};
const uint8_t magnet_tamper_yes[8] = {LCD_CHAR_M, LCD_CHAR_G, LCD_CHAR_T, LCD_MINUS_SIGN, LCD_CHAR_Y, LCD_CHAR_E, 5, LCD_CLEAR_DIGIT};
const uint8_t neutral_disturbance_tamper[8] = {LCD_CHAR_N, LCD_CHAR_D, LCD_MINUS_SIGN, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};
const uint8_t current_reverse_tamper[8] = {LCD_CLEAR_DIGIT, LCD_CHAR_C, LCD_CHAR_R, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};	// posn 0 to be replaced with r,y,b
const uint8_t over_current_tamper[8] = {LCD_CLEAR_DIGIT, 0, LCD_CHAR_C, LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CLEAR_DIGIT};	// posn 0 to be replaced with r,y,b

// event_code_bit=findWhichBitIsOne(whichevent);	// bit numbers 1 to 15, 22, 23, 24, 29 and 30  - bit 0 is not used, others masked out

#ifdef TN3P_BOARD
uint8_t getEventGroupInfo(eventGroupInfo_t* eventGroupInfoPtr, uint8_t eventCodeBit);
#endif
void display_unrestored_tamper_time(uint8_t event_code_bit);
void display_unrestored_tamper_date(uint8_t event_code_bit);

#ifdef TN3P_BOARD
uint8_t getEventGroupInfo(eventGroupInfo_t* eventGroupInfoPtr, uint8_t eventCodeBit)
{
	uint8_t retval;
	switch(eventCodeBit)
	{
		case 0:
			retval=0;	// error
			break;
			
		case 1:	//	MISSING_POTENTIAL_R
		case 2:	//	MISSING_POTENTIAL_Y
		case 3:	//	MISSING_POTENTIAL_B
		case 4:	//	OVER_VOLTAGE
		case 5:	//	UNDER_VOLTAGE
		case 6:	//	VOLTAGE_UNBALANCED
			eventGroupInfoPtr->event_wrt_ptr=mytmprdata.tamper_events0_wrt_ptr;
			eventGroupInfoPtr->base_address=EVENTS0_BASE_ADDRESS;
			eventGroupInfoPtr->end_address=EVENTS0_END_ADDRESS;
			eventGroupInfoPtr->whichGroup=0;
			retval=1;	// ok
			break;
			
		case 7:	//	CT_REVERSED_R
		case 8:	//	CT_REVERSED_Y
		case 9:	//	CT_REVERSED_B
		case 10:	//	CT_OPEN_R
		case 11:	//	CT_OPEN_Y
		case 12:	//	CT_OPEN_B
		case 13:	//	CURRENT_UNBALANCED
		case 14:	//	CT_BYPASS
		case 15:	//	OVER_CURRENT
		case 29:	//	OVER_CURRENT_Y
		case 30:	//	OVER_CURRENT_B
			eventGroupInfoPtr->event_wrt_ptr=mytmprdata.tamper_events1_wrt_ptr;
			eventGroupInfoPtr->base_address=EVENTS1_BASE_ADDRESS;
			eventGroupInfoPtr->end_address=EVENTS1_END_ADDRESS;
			eventGroupInfoPtr->whichGroup=1;
			retval=1;	// ok
			break;
			
		case 16:	//	POWER_EVENT
			eventGroupInfoPtr->event_wrt_ptr=mytmprdata.tamper_events2_wrt_ptr;
			eventGroupInfoPtr->base_address=EVENTS2_BASE_ADDRESS;
			eventGroupInfoPtr->end_address=EVENTS2_END_ADDRESS;
			eventGroupInfoPtr->whichGroup=2;
			retval=0;	// error
			break;

		case 17:	//	RTC_TRANSACTION
		case 18:	//	MD_INTERVAL_TRANSACTION
		case 19:	//	SURVEY_INTERVAL_TRANSACTION
		case 20:	//	SINGLE_ACTION_TRANSACTION
		case 21:	//	ACTIVITY_CALENDAR_TRANSACTION
		case 27:	//	UNIDIRECTIONAL MODE SET
		case 28:	//	BIDIRECTIONAL MODE SET
			eventGroupInfoPtr->event_wrt_ptr=mytmprdata.tamper_events3_wrt_ptr;
			eventGroupInfoPtr->base_address=EVENTS3_BASE_ADDRESS;
			eventGroupInfoPtr->end_address=EVENTS3_END_ADDRESS;
			eventGroupInfoPtr->whichGroup=3;
			retval=0;	// error
			break;

		case 22:	//	MAGNET
		case 23:	//	AC_MAGNET
		case 24:	//	NEUTRAL_DISTURBANCE
		case 25:	//	LOW_PF
			eventGroupInfoPtr->event_wrt_ptr=mytmprdata.tamper_events4_wrt_ptr;
			eventGroupInfoPtr->base_address=EVENTS4_BASE_ADDRESS;
			eventGroupInfoPtr->end_address=EVENTS4_END_ADDRESS;
			eventGroupInfoPtr->whichGroup=4;
			retval=1;	// ok
			break;
			
		case 26:	//	COVER_OPEN
			eventGroupInfoPtr->event_wrt_ptr=mytmprdata.tamper_events5_wrt_ptr;
			eventGroupInfoPtr->base_address=EVENTS5_BASE_ADDRESS;
			eventGroupInfoPtr->end_address=EVENTS5_END_ADDRESS;
			eventGroupInfoPtr->whichGroup=5;
			retval=0;	// error
			break;
	}
	return retval;
}

// The function display_unrestored_tamper_date will read the event records of the group
// and once it identifies the unrestored tamper record matching the event_code_bit, it will
// retrieve the date and time of that record and display it
void display_unrestored_tamper_date(uint8_t event_code_bit)
{
	eventGroupInfo_t eventGroupInfo;
	ext_rtc_t recordDtTm;
	uint32_t address;
	uint32_t terminalAddress;
	uint8_t retval;
	uint8_t eventCodeBit;
	uint8_t done;
	uint8_t success;
	
	retval = getEventGroupInfo(&eventGroupInfo, event_code_bit);
	if(retval==0)
	{// this is an error condition - probably a wrong event code bit has been sent
	
	}
	else
	{// here we are good to go
	// the  objective is to find the unrestored occurence record of this event_code_bit
	// if no overflow has occured, we must search backwards till the base_addr
	// if overflow has occured, we must stop search once we read the startAddress which is the wrt_ptr
		if((mytmprdata.tamperEvent_overflow&_BV(eventGroupInfo.whichGroup))==0)
			terminalAddress=eventGroupInfo.base_address;
		else
			terminalAddress = eventGroupInfo.event_wrt_ptr;

		done = 0;
		success = 0;
		address = eventGroupInfo.event_wrt_ptr;
		decptr_maskless(&address, eventGroupInfo.base_address, eventGroupInfo.end_address, EVENT_PTR_INC); // begin with the previous record which is the newest record
		do
		{// we are looking for the unrestored occurence record which has the same event_code_bit as this restoration case
			EPR_Read((address+(uint32_t)EVENT_CODE_OFFSET), &eventCodeBit, sizeof(eventCodeBit));

			if(event_code_bit==eventCodeBit)
			{// we got the record which has the event code bit we are looking for
				// we should check if this record is restored or not - since this is the earliest encountered
				// occurence record, it must be the one which has not been restored
				// if for some reason it has been restored then it may be an error - let us be safe
				EPR_Read((address+(uint32_t)(EVENT_PTR_INC/2)), (uint8_t*)&recordDtTm, sizeof(recordDtTm));
				if((recordDtTm.date==0)&&(recordDtTm.month==0)&&(recordDtTm.year==0))
				{// this record is has not been restored - this is the one we wanted
				// now read the occurence datetime
					EPR_Read(address, (uint8_t*)&recordDtTm, sizeof(recordDtTm));
					done=1;// job is done
					success = 1;
				}
			}
// here either event_code does not match or the record is already restored or done is 1					 
			if(done==0)
			{
				if(address==terminalAddress)
				{
					done = 1;	// we were unsuccesful in finding a matching record
					success = 0;
				}
				else
				{// continue the search in the ulta direction
					decptr_maskless(&address, eventGroupInfo.base_address, eventGroupInfo.end_address, EVENT_PTR_INC);
				}
			}
		}
		while(done == 0);
		
		if(success==0)
		{// we could not find an unrestored record - highly unusual - some mistake
		
		}
		else
		{
			lcd_display_date(recordDtTm.date, recordDtTm.month, recordDtTm.year);
		}
	}
}


void display_unrestored_tamper_time(uint8_t event_code_bit)
{
	eventGroupInfo_t eventGroupInfo;
	ext_rtc_t recordDtTm;
	uint32_t address;
	uint32_t terminalAddress;
	uint8_t retval;
	uint8_t eventCodeBit;
	uint8_t done;
	uint8_t success;

	retval = getEventGroupInfo(&eventGroupInfo, event_code_bit);
	if(retval==0)
	{// this is an error condition - probably a wrong event code bit has been sent
	
	}
	else
	{// here we are good to go
	// the  objective is to find the unrestored occurence record of this event_code_bit
	// if no overflow has occured, we must search backwards till the base_addr
	// if overflow has occured, we must stop search once we read the startAddress which is the wrt_ptr
		if((mytmprdata.tamperEvent_overflow&_BV(eventGroupInfo.whichGroup))==0)
			terminalAddress=eventGroupInfo.base_address;
		else
			terminalAddress = eventGroupInfo.event_wrt_ptr;

		done = 0;
		address = eventGroupInfo.event_wrt_ptr;
		decptr_maskless(&address, eventGroupInfo.base_address, eventGroupInfo.end_address, EVENT_PTR_INC); // begin with the previous record which is the newest record
		do
		{// we are looking for the unrestored occurence record which has the same event_code_bit as this restoration case
			EPR_Read((address+(uint32_t)EVENT_CODE_OFFSET), &eventCodeBit, sizeof(eventCodeBit));

			if(event_code_bit==eventCodeBit)
			{// we got the record which has the event code bit we are looking for
				// we should check if this record is restored or not - since this is the earliest encountered
				// occurence record, it must be the one which has not been restored
				// if for some reason it has been restored then it may be an error - let us be safe
				EPR_Read((address+(uint32_t)(EVENT_PTR_INC/2)), (uint8_t*)&recordDtTm, sizeof(recordDtTm));
				if((recordDtTm.date==0)&&(recordDtTm.month==0)&&(recordDtTm.year==0))
				{// this record is has not been restored - this is the one we wanted
				// now read the occurence datetime
					EPR_Read(address, (uint8_t*)&recordDtTm, sizeof(recordDtTm));
					done=1;// job is done
					success = 1;
				}
			}
// here either event_code does not match or the record is already restored or done is 1					 
			if(done==0)
			{
				if(address==terminalAddress)
				{
					done = 1;	// we were unsuccesful in finding a matching record
					success = 0;
				}
				else
				{// continue the search in the ulta direction
					decptr_maskless(&address, eventGroupInfo.base_address, eventGroupInfo.end_address, EVENT_PTR_INC);
				}
			}
		}
		while(done == 0);
		
		if(success==0)
		{// we could not find an unrestored record - highly unusual - some mistake
		
		}
		else
		{
			lcd_display_time(recordDtTm.hour, recordDtTm.minute, recordDtTm.second);
		}
	}
}
#endif

void display_specific_tamper(uint8_t eventcodebit)
{
	switch(eventcodebit)
	{
		case 0:	//	No Tamper
			lcd_msg((const uint8_t*)&msg_nil_tamper);
			break;
		
		case 1:	//	MISSING_POTENTIAL_R
			lcd_msg((const uint8_t*)&voltage_fail_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			break;
			
		case 2:	//	MISSING_POTENTIAL_Y
			lcd_msg((const uint8_t*)&voltage_fail_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			break;
			
		case 3:	//	MISSING_POTENTIAL_B
			lcd_msg((const uint8_t*)&voltage_fail_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			break;
			
		case 4:	//	OVER_VOLTAGE
			lcd_msg((const uint8_t*)&msg_hv_tamper);
			break;
		
		case 5:	//	UNDER_VOLTAGE
			lcd_msg((const uint8_t*)&msg_lv_tamper);
			break;
			
		case 6:	//	VOLTAGE_UNBALANCED
			lcd_msg((const uint8_t*)&voltage_unbalance_tamper);
			break;
			
		case 7:	//	CT_REVERSED_R
			lcd_msg((const uint8_t*)&current_reverse_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			break;
		
		case 8:	//	CT_REVERSED_Y
			lcd_msg((const uint8_t*)&current_reverse_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			break;
		
		case 9:	//	CT_REVERSED_B
			lcd_msg((const uint8_t*)&current_reverse_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			break;
		
		case 10:	//	CT_OPEN_R
			lcd_msg((const uint8_t*)&current_fail_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			break;

		case 11:	//	CT_OPEN_Y
			lcd_msg((const uint8_t*)&current_fail_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			break;
		
		case 12:	//	CT_OPEN_B
			lcd_msg((const uint8_t*)&current_fail_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			break;
		
		case 13:	//	CURRENT_UNBALANCED
			lcd_msg((const uint8_t*)&current_unbalance_tamper);
			break;
		
		case 14:	//	CT_BYPASS
			lcd_msg((const uint8_t*)&current_bypass_tamper);
			break;
		
		case 15:	//	OVER_CURRENT_R
			lcd_msg((const uint8_t*)&over_current_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			break;

		case 22:	//	MAGNET
		case 23:	//	AC_MAGNET
			lcd_msg((const uint8_t*)&magnet_indication_tamper);
			break;

		case 24:	//	NEUTRAL_DISTURBANCE
			lcd_msg((const uint8_t*)&neutral_disturbance_tamper);
			break;
		
		case 29:	//	OVER_CURRENT_Y
			lcd_msg((const uint8_t*)&over_current_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			break;
		
		case 30:	//	OVER_CURRENT_B
			lcd_msg((const uint8_t*)&over_current_tamper);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			break;
				
		default:
			break;
	}
}

/*
	once d_Tampers function is called - it will prevent disp_state from being incremented till
	all steps have been completed - Pressing the forward key/multiple calls will cause the steps to be executed
	turn by turn till the last step is reached
	Pressing the Back key will cause exit from the function altogether
*/
uint32_t gTamperEvent;
uint8_t g_tamper_display_state;
uint8_t g_cover_display_state;
uint8_t cover_disp_timer;

//	g_tamper_display_state = 0;
// This variable must be made zero whenever we enter Mode I

// the function d_Tampers only displays unrestored tampers
void d_Tampers(void)
{
	uint32_t whichevent;
	uint8_t event_code_bit;
	uint8_t whichGroup;

	if(g_display_flag.holdState==0)
	{
		gTamperEvent = mytmprdata.tamperEvent_image&TAMPER_DISPLAY_MASK; // make all those bits not to be displayed as 0
		g_display_flag.holdState = 1;
		g_tamper_display_state = 0;	// safety
	}
	
	switch(g_tamper_display_state)
	{
		case 0:
			if(gTamperEvent==0)
			{// hereg_display_flag there are no tampers
				display_specific_tamper(0);	// 0 is to display the no tamper message
			}
			else
			{// here there is some tamper present i.e. some bits are 1
			// only those bits which are to be displayed have not been masked, transaction events and cover open etc have been masked out
		    whichevent = gTamperEvent & ~(gTamperEvent-1);
				event_code_bit=findWhichBitIsOne(whichevent);	// bit numbers 1 to 15, 22, 23, 24, 29 and 30  - bit 0 is not used, others masked out
				
				display_specific_tamper(event_code_bit);
			}
//			g_tamper_display_state++;
			break;

		case 1:
			if(gTamperEvent==0)
			{// hereg_display_flag there are no tampers
				lcd_display_date(0,0,0);		// this will display 00:00:00
			}
			else
			{// here there is some tamper present
		    whichevent = gTamperEvent & ~(gTamperEvent-1);
				event_code_bit=findWhichBitIsOne(whichevent);	// bit number 1 to 30 (31 max) - bit 0 is not used
				display_unrestored_tamper_date(event_code_bit);
			}
//			g_tamper_display_state++;
			break;

		case 2:
			if(gTamperEvent==0)
			{// hereg_display_flag there are no tampers
				lcd_display_time(0,0,0);		// this will display 00:00:00
			}
			else
			{// here there is some tamper present
		    whichevent = gTamperEvent & ~(gTamperEvent-1);
				event_code_bit=findWhichBitIsOne(whichevent);	// bit number 1 to 30 (31 max) - bit 0 is not used
				display_unrestored_tamper_time(event_code_bit);
				gTamperEvent&=~whichevent;	// job complete for this fellow
			}
			
			if(gTamperEvent==0)
			{// everything has been completed
				g_display_flag.holdState=0; 	// allow disp_state to be incremented
			}
			g_tamper_display_state = 0;	// get ready for next time around
			break;
	}
}

void d_Last_Restored_Tamper(void)
{
// this is also a three state function, and will set the holdState flag
// releasing it once the time has been displayed
	if(g_display_flag.holdState==0)
	{
		g_display_flag.holdState = 1;
		g_tamper_display_state = 0;	// safety
	}
	
	switch(g_tamper_display_state)
	{
		case 0:
			display_specific_tamper(mytmprdata.latestTamperCode);	// latestTamperCode contains the permitted eventCodeBit
//			g_tamper_display_state++;
			break;

		case 1:
			lcd_display_date(mytmprdata.latestTamperDateTime.date, mytmprdata.latestTamperDateTime.month, mytmprdata.latestTamperDateTime.year);
//			g_tamper_display_state++;
			break;

		case 2:
			lcd_display_time(mytmprdata.latestTamperDateTime.hour, mytmprdata.latestTamperDateTime.minute, mytmprdata.latestTamperDateTime.second);
//			g_tamper_display_state++;
			g_display_flag.holdState=0; 	// allow disp_state to be incremented
			g_tamper_display_state = 0;	// get ready for next time around
			break;
	}
}

void d_Tamper_Count(void)
{// display the cumulative tamper count
	LCD_DisplayIntWithPos(mytmprdata.tamperCount, LCD_LAST_POS_DIGIT);
	LCD_DisplayDigit(9, LCD_CHAR_T);
	LCD_DisplayDigit(8, LCD_CHAR_C);
  LCD_DisplaySpSign(S_T8);	// CUM Sign
}

void d_Magnet_Tamper_Indication(void)
{
//	if((g_TamperStatus&HALLSENS_MASK)==0)
	if((mytmprdata.tamperEvent_once&MAGNET_MASK)==0)
	{// there was never any magnet tamper
		lcd_msg((const uint8_t*)&magnet_tamper_nil);
	}
	else
	{// there is magnet tamper at least once
		lcd_msg((const uint8_t*)&magnet_tamper_yes);
	}
}


void displayEnergy(int32_t energy, uint8_t whichEnergy)
{
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
	display_long_one_dp(energy/100);
	
	switch(whichEnergy)
	{
		case BILL_KWH_ENERGY:
			LCD_DisplaykWhSign();
			break;
			
		case BILL_KVAH_ENERGY:
			LCD_DisplaykVAhSign();
			break;
			
		case BILL_KVARH_LAG_ENERGY:
		case BILL_KVARH_LEAD_ENERGY:
		case BILL_KVARH_TOTAL_ENERGY:
			LCD_DisplaykVArhSign();
			break;
			
	}
}

int32_t getCumMD_uptillNow(uint8_t imp_ex_type, uint8_t mdType)
{
	uint32_t baseAddr;
	uint32_t addr;
	uint32_t offset;
	int32_t cumMD;
	uint8_t tmonth;
	
	if(imp_ex_type==IMPORT)
	{
		baseAddr = BILLDATA_BASE_ADDRESS;
		if(mdType==BILL_MD_KW)
		{
			offset = 404;
		}
		else
		{
			offset = 408;
		}
	}
	else
	{
		baseAddr = BILLDATA_EXPORT_BASE_ADDRESS;
		if(mdType==BILL_MD_KW)
		{
			offset = 412;
		}
		else
		{
			offset = 416;
		}
	}

	if(Common_Data.bpcount==0)
	{
		cumMD = 0;
	}
	else
	{// some bills have been made
		tmonth = decrementMonth(1);
		addr=baseAddr + (tmonth-1)*BILL_PTR_INC + offset;
		EPR_Read(addr, (uint8_t*)&cumMD, 4);	// read 4 bytes of appropriate CumMD
	}
	return cumMD;
}

int32_t getCurrentTOD_KWH(uint8_t imp_ex_type, uint8_t zone)
{
// zone goes from 0,1,2,3,4,5 - these are our registers
	int32_t tempTodKwh;
	int32_t currentZoneValue;
	uint32_t offset;
		
	if(imp_ex_type==IMPORT)
	{
		offset = 0;
		currentZoneValue = myenergydata.zone_kwh;
	}
	else
	{
		offset = 36;
		currentZoneValue = myenergydata.zone_kwh_export;
	}
	
	EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+offset, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
	
	if(zone==presentZone)
	{
		tempTodKwh+=currentZoneValue;
	}
	return tempTodKwh;
}

int32_t getBillingTOD_KWH(uint8_t imp_ex_type, uint8_t whichBill, uint8_t zone)
{
// zone goes from 0,1,2,3,4,5 - these are our registers
	int32_t tempTodKwh;
	uint32_t addr;
	uint32_t offset;
	uint8_t tmonth;
	
	if(whichBill>Common_Data.bpcount)
		return 0;
		
	tmonth = decrementMonth(whichBill);
	
	if(imp_ex_type==IMPORT)
	{
		addr = BILLDATA_BASE_ADDRESS;
		offset = 12;
	}
	else
	{
		addr = BILLDATA_EXPORT_BASE_ADDRESS;
		offset = 12;
	}
	
  tmonth = decrementMonth(whichBill);
  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + offset + (zone*4),(uint8_t*)&tempTodKwh,sizeof(tempTodKwh));
	
	return tempTodKwh;
}

void display_CumKWH_TOD(uint8_t imp_ex_type, uint8_t index)
{// this displays current TOD
// index for TN is 1,2,3,4,5
// pvk zone definitions are 0,1,2,3,4,5
// relation between index and zone is as follows
/*
Index			Zone
	1					1
	2					3
	3					4
	4					0 + 2
	5					5
*/

	int32_t todEnergy;
	
	switch(index)
	{
		case 1:
			todEnergy = getCurrentTOD_KWH(imp_ex_type,1);
			break;
			
		case 2:
			todEnergy = getCurrentTOD_KWH(imp_ex_type,3);
			break;
	
		case 3:
			todEnergy = getCurrentTOD_KWH(imp_ex_type,4);
			break;
			
		case 4:
			todEnergy = getCurrentTOD_KWH(imp_ex_type,0);
			todEnergy+= getCurrentTOD_KWH(imp_ex_type,2);
			break;
			
		case 5:
			todEnergy = getCurrentTOD_KWH(imp_ex_type,5);
			break;

		default:
			todEnergy=0;
			break;
			
	}

	displayEnergy(todEnergy, BILL_KWH_ENERGY);
  LCD_DisplaySpSign(S_TOD);
  LCD_DisplaySpSign(S_T8);	// Cum
	
	if(imp_ex_type==EXPORT)
		LCD_DisplaySpSign(S_EXPORT);
	else
		LCD_DisplaySpSign(S_IMPORT);	
		
	LCD_DisplayDigit(8, index);
}

void displayBabyDigits_IE(uint8_t imp_ex_type)
{// now we have IMPORT and EXPORT icons
	if(imp_ex_type==EXPORT)
  	LCD_DisplaySpSign(S_EXPORT);
	else
  	LCD_DisplaySpSign(S_IMPORT);
}

void display_CumKWH(uint8_t imp_ex_type)
{
	int32_t energy;
	if(imp_ex_type==IMPORT)
		energy = myenergydata.kwh;
	else
		energy = myenergydata.kwh_export;
	
	displayEnergy(energy, BILL_KWH_ENERGY);
	
  LCD_DisplaySpSign(S_T8);	// CUM Sign
	
	displayBabyDigits_IE(imp_ex_type);
	disp_autoscroll_interval=20;
}

void display_CumKVAH(uint8_t imp_ex_type)
{
	int32_t energy;
	if(imp_ex_type==IMPORT)
		energy = myenergydata.kvah;
	else
		energy = myenergydata.kvah_export;
	
	displayEnergy(energy, BILL_KVAH_ENERGY);
  LCD_DisplaySpSign(S_T8);	// CUM Sign
	displayBabyDigits_IE(imp_ex_type);
}

void display_CumKVARH(uint8_t imp_ex_type, uint8_t lgld)
{
	int32_t energy;
	uint8_t lgldType;
	if(imp_ex_type==IMPORT)
	{
		if(lgld==D_LAG)
		{
			energy = myenergydata.kvarh_lag;
			lgldType = BILL_KVARH_LAG_ENERGY;
		}
		else if(lgld==D_LEAD)
		{
			energy = myenergydata.kvarh_lead;
			lgldType = BILL_KVARH_LEAD_ENERGY;
		}
		else
		{
			energy = myenergydata.kvarh_lag+myenergydata.kvarh_lead;
			lgldType = BILL_KVARH_TOTAL_ENERGY;
		}
	}
	else
	{
		if(lgld==D_LAG)
		{
			energy = myenergydata.kvarh_lag_export;
			lgldType = BILL_KVARH_LAG_ENERGY;
		}
		else if(lgld==D_LEAD)
		{
			energy = myenergydata.kvarh_lead_export;
			lgldType = BILL_KVARH_LEAD_ENERGY;
		}
		else
		{
			energy = myenergydata.kvarh_lag_export+myenergydata.kvarh_lead_export;
			lgldType = BILL_KVARH_TOTAL_ENERGY;
		}
	}

	displayEnergy(energy, lgldType);
	
  LCD_DisplaySpSign(S_T8);	// CUM Sign
// what about Lag / Lead differentiation	

	if(imp_ex_type==EXPORT)
  	LCD_DisplaySpSign(S_EXPORT);
	else
  	LCD_DisplaySpSign(S_IMPORT);
		
	if(lgld==D_LAG)
  	LCD_DisplaySpSign(S_LAG);
	else
  	LCD_DisplaySpSign(S_LEAD);
}

void display_Current_AVG_PF(uint8_t imp_ex_type)
{
	int32_t tempLong;
	int32_t tempLong1;
	int32_t tempLong2;
	int32_t currentKWH;
	int32_t currentKVAH;
	
	if(imp_ex_type==IMPORT)
	{
		currentKWH=myenergydata.kwh;
		currentKVAH=myenergydata.kvah;
	}
	else
	{
		currentKWH=myenergydata.kwh_export;
		currentKVAH=myenergydata.kvah_export;
	}
	
	if(Common_Data.bpcount==0)
	{// no bills have been made
		tempLong = (currentKWH*1000)/currentKVAH;
	}
	else
	{//read last months kwh and kvah
		tempLong1 = get_Energy_Billing(imp_ex_type, 1, BILL_KWH_ENERGY);
		tempLong2 = get_Energy_Billing(imp_ex_type, 1, BILL_KVAH_ENERGY);
		if(currentKVAH>tempLong2)
			tempLong = ((currentKWH-tempLong1)*1000)/(currentKVAH - tempLong2);
		else
			tempLong=0;
	}
	display_long_three_dp(tempLong);
	lcd_display_A();	

	LCD_DisplayDigit(1, LCD_CHAR_P);
	LCD_DisplayDigit(2, LCD_CHAR_F);

	if(imp_ex_type==EXPORT)
  	LCD_DisplaySpSign(S_EXPORT);
	else
  	LCD_DisplaySpSign(S_IMPORT);
}


void display_Billing_AVG_PF(uint8_t imp_ex_type, uint8_t whichBill)
{
//now the bills store the correct value for average pf - hence we need to only display the value
	uint32_t addr;
	uint32_t offset;
	int32_t bill_avgPF;
	uint8_t tmonth;

	if(imp_ex_type==IMPORT)
		addr = BILLDATA_BASE_ADDRESS;
	else
		addr = BILLDATA_EXPORT_BASE_ADDRESS;

	offset = 296;	// this is the offset for average pf (power_factor)
		
	if((whichBill>Common_Data.bpcount)||(whichBill==255))
	{// whichBill can be 1 to 12
		bill_avgPF=0;	//this bill not yet made
	}
	else	
	{
	  tmonth = decrementMonth(whichBill);
	  EPR_Read(addr+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&bill_avgPF,sizeof(bill_avgPF));
	}

	display_long_three_dp(bill_avgPF);
	lcd_display_A();
	LCD_DisplayDigit(9, whichBill);
  LCD_DisplayDigit(1, LCD_CHAR_P);
  LCD_DisplayDigit(2, LCD_CHAR_F);
  LCD_DisplaySpSign(S_BILL);
	
	if(imp_ex_type==EXPORT)
  	LCD_DisplaySpSign(S_EXPORT);
	else
  	LCD_DisplaySpSign(S_IMPORT);	
}

void displayMD(uint16_t md, uint8_t mdtype, uint8_t imp_ex_type, uint8_t whichBill)
{
	display_long_three_dp(md);
	if(mdtype==0)
		LCD_DisplaykWSign();
	else
		LCD_DisplaykVASign();
	
  LCD_DisplaySpSign(S_MD);
	
	if(whichBill!=0)
		LCD_DisplayDigit(9, whichBill);
	
	if(imp_ex_type!=IMPORT)
		LCD_DisplaySpSign(S_EXPORT);
	else
		LCD_DisplaySpSign(S_IMPORT);
}

void display_MD_Current(uint8_t imp_ex_type)
{
	uint16_t md;
	if(imp_ex_type==IMPORT)
	{
		if(Common_Data.mdType==MDKW)
			md = mymddata.mdkw;
		else
			md = mymddata.mdkva;
	}
	else
	{
		if(Common_Data.mdType==MDKW)
			md = mymddata.mdkw_export;
		else
			md = mymddata.mdkva_export;
	}
	displayMD(md, Common_Data.mdType, imp_ex_type,0);
}

uint16_t get_MD_Billing(uint8_t imp_ex_type, uint8_t whichBill)
{// for TNEB there is only MD - KW or KVA depends on Common_Data.mdtype
	uint32_t addr;
	uint32_t offset;
	uint16_t tempInt;
	uint8_t tmonth;

	if(imp_ex_type==IMPORT)
		addr = BILLDATA_BASE_ADDRESS;
	else
		addr = BILLDATA_EXPORT_BASE_ADDRESS;
	
	if(Common_Data.mdType==MDKW)
		offset=80;
	else
		offset=188;
		
	if(whichBill>Common_Data.bpcount)
		tempInt=0;	// bill not made yet
	else
	{	
	  tmonth = decrementMonth(whichBill);
	  EPR_Read(addr+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&tempInt,2);
	}
	
	return tempInt;
}

ext_rtc_t get_MD_BillingDateTime(uint8_t imp_ex_type, uint8_t whichBill, uint8_t mdtype)
{// whichBill = 1 (L1) in this case
	ext_rtc_t tdate;
	uint32_t addr;
	uint32_t offset;
	uint8_t tmonth;

	if(imp_ex_type==IMPORT)
		addr = BILLDATA_BASE_ADDRESS;
	else
		addr = BILLDATA_EXPORT_BASE_ADDRESS;
	
	if(mdtype==MDKW)
		offset=84;
	else
		offset=192;
		
	if(whichBill>Common_Data.bpcount)
	{
		tdate.date=0;	// bill not made yet
		tdate.month=0;
		tdate.year=0;
		tdate.hour=0;
		tdate.minute=0;
		tdate.second=0;
	}
	else
	{	
	  tmonth = decrementMonth(whichBill);
	  EPR_Read(addr+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&tdate,sizeof(tdate));
	}
	return tdate;
}

void display_MD_Billing(uint8_t imp_ex_type, uint8_t whichBill)
{// whichBill = 1 (L1), 2(L2) etc upto 12(L12)
	uint16_t md;
	md=get_MD_Billing(imp_ex_type, whichBill);
	displayMD(md, Common_Data.mdType, imp_ex_type, whichBill);
	LCD_DisplaySpSign(S_BILL);	
}

void display_MD_BillingDateTime_L1(uint8_t imp_ex_type, uint8_t dtOrTm)
{
// date/time of current MD or L1 MD whichever is higher is to be shown 
	ext_rtc_t tdate;
	uint16_t mdBill;
	uint16_t mdCurrent;
	
	if(Common_Data.mdType==MDKW)
	{
		if(imp_ex_type==IMPORT)
			mdCurrent=mymddata.mdkw;
		else
			mdCurrent=mymddata.mdkw_export;
	}
	else
	{
		if(imp_ex_type==IMPORT)
			mdCurrent=mymddata.mdkva;
		else
			mdCurrent=mymddata.mdkva_export;
	}
	
	mdBill=get_MD_Billing(imp_ex_type, 1);	// this will return mdkw or mdva - L1 assumed
	
	if(mdBill>mdCurrent)
	{
		tdate = get_MD_BillingDateTime(imp_ex_type, 1,Common_Data.mdType); // - L1 assumed
  	LCD_DisplaySpSign(S_BILL);	
// gaurav		
	}
	else
	{
		if(Common_Data.mdType==MDKW)
		{
			if(imp_ex_type==IMPORT)
				tdate = mymddata.mdkw_datetime;
			else
				tdate = mymddata.mdkw_datetime_export;
		}
		else
		{
			if(imp_ex_type==IMPORT)
				tdate = mymddata.mdkva_datetime;
			else
				tdate = mymddata.mdkva_datetime_export;
		}
	}
	if(dtOrTm==MDDATE)
		lcd_display_date(tdate.date,tdate.month,tdate.year);
	else
		lcd_display_time(tdate.hour,tdate.minute,tdate.second);

  LCD_DisplaySpSign(S_MD);
	
	if(Common_Data.mdType==MDKW)
		LCD_DisplaykWSign();
	else
		LCD_DisplaykVASign();
		
	if(imp_ex_type==EXPORT)
		LCD_DisplaySpSign(S_EXPORT);
	else
		LCD_DisplaySpSign(S_IMPORT);	
}

void d_LastBillDate(void)
{
	int16_t year;
	if (Common_Data.bpcount==0)
		year = 0;
	else
		year = ((g_Class03_BillingDate.year_high*256) + g_Class03_BillingDate.year_low) - 2000;
	lcd_display_date(g_Class03_BillingDate.day_of_month,g_Class03_BillingDate.month,year);
}

void d_LastBillTime(void)
{
	lcd_display_time(g_Class03_BillingDate.hour,g_Class03_BillingDate.minute,g_Class03_BillingDate.second);
}

int32_t get_Energy_Billing(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichEnergy)
{
// this function gets energy from bills stored in e2rom
	uint32_t addr;
	uint32_t offset;
	int32_t tempLong;
	uint8_t tmonth;

	if(imp_ex_type==IMPORT)
		addr = BILLDATA_BASE_ADDRESS;
	else
		addr = BILLDATA_EXPORT_BASE_ADDRESS;
	
	switch(whichEnergy)
	{
		case BILL_KWH_ENERGY:
			offset = 8;
			break;
			
		case BILL_KVAH_ENERGY:
			offset = 44;
			break;
			
		case BILL_KVARH_LAG_ENERGY:
			offset = 332;
			break;
			
		case BILL_KVARH_LEAD_ENERGY:
			offset = 368;
			break;
			
		default:
			whichBill = 255;	// error
			break;
	}
		
		
	if((whichBill>Common_Data.bpcount)||(whichBill==255))
	{// whichBill can be 1 to 12
		tempLong=0;	//this bill not yet made
	}
	else	
	{
	  tmonth = decrementMonth(whichBill);
	  EPR_Read(addr+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&tempLong,sizeof(tempLong));
	}
	return tempLong;
}


void display_Energy_Billing(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichEnergy)
{
	int32_t energy;
	energy = get_Energy_Billing(imp_ex_type, whichBill, whichEnergy);
	
	displayEnergy(energy, whichEnergy);
	
  LCD_DisplaySpSign(S_BILL);
  LCD_DisplaySpSign(S_T8);	// Cum
	
// display LwhichBill, L1,L2 etc
//	LCD_DisplayDigit(9, LCD_CHAR_L);
	LCD_DisplayDigit(9, whichBill);

// display Import/Export
	if(imp_ex_type==EXPORT)
		LCD_DisplaySpSign(S_EXPORT);
	else
		LCD_DisplaySpSign(S_IMPORT);	
}

int32_t get_Energy_Billing_TOD(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichZone, uint8_t whichEnergy)
{
// this function gets zone energies from the bills stored in e2rom
//whichZone goes from 0,1,2,3,4,5,6,7
	uint32_t addr;
	uint32_t offset;
	int32_t tempLong;
	uint8_t tmonth;

	if(imp_ex_type==IMPORT)
		addr = BILLDATA_BASE_ADDRESS;
	else
		addr = BILLDATA_EXPORT_BASE_ADDRESS;
	
	switch(whichEnergy)
	{
		case BILL_KWH_ENERGY:
			offset = 12+whichZone*4;
			break;
			
		case BILL_KVAH_ENERGY:
			offset = 48+whichZone*4;
			break;
			
		case BILL_KVARH_LAG_ENERGY:
			offset = 336+whichZone*4;
			break;
			
		case BILL_KVARH_LEAD_ENERGY:
			offset = 372+whichZone*4;
			break;
			
		default:
			whichBill = 255;	// error
			break;
	}
		
		
	if((whichBill>Common_Data.bpcount)||(whichBill==255))
	{// whichBill can be 1 to 12
		tempLong=0;	//this bill not yet made
	}
	else	
	{
	  tmonth = decrementMonth(whichBill);
	  EPR_Read(addr+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&tempLong,sizeof(tempLong));
	}
	return tempLong;

}

void display_Energy_Billing_TOD(uint8_t imp_ex_type, uint8_t whichBill, uint8_t whichIndex, uint8_t whichEnergy)
{
// index for TNEB goes from 1,2,3,4,5
// we need to convert them to zones
	int32_t todEnergy;
	
	switch(whichIndex)
	{
		case 1:
			todEnergy = get_Energy_Billing_TOD(imp_ex_type,whichBill,1,whichEnergy);
			break;
			
		case 2:
			todEnergy = get_Energy_Billing_TOD(imp_ex_type,whichBill,3,whichEnergy);
			break;
	
		case 3:
			todEnergy = get_Energy_Billing_TOD(imp_ex_type,whichBill,4,whichEnergy);
			break;
			
		case 4:
			todEnergy = get_Energy_Billing_TOD(imp_ex_type,whichBill,0,whichEnergy);
			todEnergy+= get_Energy_Billing_TOD(imp_ex_type,whichBill,2,whichEnergy);
			break;
			
		case 5:
			todEnergy = get_Energy_Billing_TOD(imp_ex_type,whichBill,5,whichEnergy);
			break;

		default:
			todEnergy=0;
			break;
			
	}
	displayEnergy(todEnergy, whichEnergy);
  LCD_DisplaySpSign(S_BILL);
  LCD_DisplaySpSign(S_TOD);
  LCD_DisplaySpSign(S_T8);	// Cum
// now display the index
	LCD_DisplayDigit(9, whichBill);
	LCD_DisplayDigit(8, whichIndex);
	
	if(imp_ex_type==EXPORT)
		LCD_DisplaySpSign(S_EXPORT);
	else
		LCD_DisplaySpSign(S_IMPORT);	
}

void display_CumMD(uint8_t imp_ex_type)
{
	uint32_t baseAddr;
	uint32_t addr;
	uint32_t offset;
	int32_t cumMD;
	int32_t currentMD;
	uint8_t tmonth;
	
	if(imp_ex_type==IMPORT)
	{
		baseAddr = BILLDATA_BASE_ADDRESS;
		if(Common_Data.mdType==MDKW)
		{
			currentMD = mymddata.mdkw;
			offset = 404;
		}
		else
		{
			currentMD = mymddata.mdkva;
			offset = 408;
		}
	}
	else
	{
		baseAddr = BILLDATA_EXPORT_BASE_ADDRESS;
		if(Common_Data.mdType==MDKW)
		{
			currentMD = mymddata.mdkw_export;
			offset = 412;
		}
		else
		{
			currentMD = mymddata.mdkva_export;
			offset = 416;
		}
	}

// currentMD has already been obtained

	if(Common_Data.bpcount==0)
	{
		cumMD = 0;
	}
	else
	{// some bills have been made
		tmonth = decrementMonth(1);
		addr=baseAddr + (tmonth-1)*BILL_PTR_INC + offset;
		EPR_Read(addr, (uint8_t*)&cumMD, 4);	// read 4 bytes of appropriate CumMD
	}
	cumMD+=currentMD; // This is the final figure
//	displayMD(cumMD,Common_Data.mdType, imp_ex_type, 0);
	
	display_long_three_dp(cumMD);
	if(Common_Data.mdType==0)
		LCD_DisplaykWSign();
	else
		LCD_DisplaykVASign();
	
  LCD_DisplaySpSign(S_MD);
  LCD_DisplaySpSign(S_T8);	// CUM Sign	
	
	if(imp_ex_type==EXPORT)
		LCD_DisplaySpSign(S_EXPORT);
	else
		LCD_DisplaySpSign(S_IMPORT);	
}

void display_Vrms(EM_LINE line)
{
	int32_t tempLong;
	tempLong = getInstantaneousParams(IVOLTS,line)*100;
	display_long_two_dp(tempLong);
  LCD_DisplaySpSign(S_T2);	// V
	g_display_flag.fast_display_flag=1;
}

void display_Irms(EM_LINE line)
{
	int32_t tempLong;
	tempLong = getInstantaneousParams(ICURRENTS,line)*1000;
	display_long_three_dp(tempLong);
	if(exportMode!=0)
		LCD_DisplayDigit(1, LCD_MINUS_SIGN);
	lcd_display_A();
	g_display_flag.fast_display_flag=1;
}

// gaurav
void displayRisingDemand(uint8_t imp_ex_type)
{
	int32_t tempLong;
	uint8_t tempchar;
	
  tempchar=60/(int32_t)Common_Data.mdinterval;
	if(imp_ex_type==IMPORT)
		tempLong = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);
	else
		tempLong = ((myenergydata.kwh_export-ctldata.kwh_last_saved_export)*(int32_t)tempchar);
	
	if(tempLong<0)
		tempLong=0;
	display_long_three_dp(tempLong);	// rising MD
	LCD_DisplaykWSign();
  LCD_DisplaySpSign(S_MD);
  LCD_DisplaySpSign(S_TIME);
	
// we can also display the minutes elapsed
	tempchar = rtc.minute - ((rtc.minute/Common_Data.mdinterval)*Common_Data.mdinterval); 
	LCD_DisplayDigit(9, tempchar/10);
	LCD_DisplayDigit(8, tempchar%10);
	
	if(imp_ex_type==EXPORT)
  	LCD_DisplaySpSign(S_EXPORT);
	else
  	LCD_DisplaySpSign(S_IMPORT);	
		
	LCD_DisplayDigit(1, LCD_CHAR_R);
	LCD_DisplayDigit(2, LCD_CHAR_D);
	g_display_flag.fast_display_flag=1;
}

void displayActivePower(EM_LINE line)
{
	int32_t tempLong;
  powermgmt_mode_t power_mode;
	power_mode = POWERMGMT_GetMode();
	if (power_mode != POWERMGMT_MODE3)
		tempLong = (int32_t)(EM_GetActivePower(line));
	else
		tempLong=0;
	display_long_three_dp(tempLong);
//	if(exportMode!=0)
//		LCD_DisplayDigit(1, LCD_MINUS_SIGN);
	LCD_DisplaykWSign();
	g_display_flag.fast_display_flag=1;
}

void displayApparentPower(EM_LINE line)
{
	int32_t tempLong;
  powermgmt_mode_t power_mode;
	power_mode = POWERMGMT_GetMode();
	if (power_mode != POWERMGMT_MODE3)
		tempLong = (int32_t)(EM_GetApparentPower(line));
	else
		tempLong=0;
	display_long_three_dp(tempLong);
	LCD_DisplaykVASign();
	g_display_flag.fast_display_flag=1;
}

void display_Billing_PowerOff(uint8_t whichBill)
{
// this is the value for a particular month - both power on hours and power off hours are now being computed for each bill
// data at offset 420 contains the power on minutes for that month
// data at offset 432 contains the power off minutes for that month
// we need to know the bill date so that we can figure out the number of
// minutes in that month
	int32_t powerOffMinutes;
	int32_t totalMinutes;

// get the powerOnminutes from bills stored in e2rom
	ext_rtc_t bdate;
	uint32_t addr;
	uint32_t offset;
	int32_t tempLong;
	uint8_t tmonth;

	addr = BILLDATA_BASE_ADDRESS;
	offset = 432;	
		
	if(whichBill>Common_Data.bpcount)
	{// whichBill can be 1 to 12
		powerOffMinutes=-1;	// no data for this bill
	}
	else	
	{
	  tmonth = decrementMonth(whichBill);
	  EPR_Read(addr+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&powerOffMinutes,sizeof(powerOffMinutes));
	}
	
	LCD_DisplayIntWithPos((powerOffMinutes/60), LCD_LAST_POS_DIGIT);	// we need to display the hours
	LCD_DisplayDigit(1, 0);
	LCD_DisplayDigit(2, LCD_CHAR_F);
}

void display_PowerOff(uint8_t currOrCum)
{// poweroff values for current month can be calculated by calling computePowerOffDurn(bdate)
// 0 - current, 1 is Cumulative
// power off is shown in hours
	int32_t temp;
	
	temp = computePowerOffDurn(rtc);	// use the rtc for current value

	if(currOrCum!=0)
	{
		temp += Common_Data.cumPowerFailDuration;
	}
	
	LCD_DisplayIntWithPos((temp/60), LCD_LAST_POS_DIGIT);	// power off to be shown in hours
	LCD_DisplayDigit(1, LCD_CHAR_P);
	LCD_DisplayDigit(2, 0);
	LCD_DisplayDigit(3, LCD_CHAR_F);
  LCD_DisplaySpSign(S_T7); // h
}

//--------------------------------------------------------------------------------------
// - final top level functions these are to be called from the display.c file
// all these functions do not need any parameter to be passed


// this function automatically gets called
void d_COPEN(void)
{
	ext_rtc_t tamperTime;
	if(g_display_flag.holdState==0)
	{
		g_display_flag.holdState=1;
		g_cover_display_state=0;
	}
	
	disp_timer = disp_autoscroll_interval; // we want this function to be called every second
	cover_disp_timer++;
	
	if(g_display_flag.tglbit==0)
	{
	  LCD_ClearAll();		// this will cause blinking every other second
	}
	else
	{	
		switch(g_cover_display_state)
		{
			case 0:
				LCD_DisplayDigit(2, LCD_CHAR_C);
				LCD_DisplayDigit(3, LCD_MINUS_SIGN);
				LCD_DisplayDigit(4, 0);
				LCD_DisplayDigit(5, LCD_CHAR_P);
				LCD_DisplayDigit(6, LCD_CHAR_E);
				LCD_DisplayDigit(7, LCD_CHAR_N);
				break;

			case 1: /*display date*/
				EPR_Read(EVENTS5_BASE_ADDRESS, (uint8_t*)&tamperTime, sizeof(tamperTime));
				lcd_display_date(tamperTime.date,tamperTime.month,tamperTime.year);
				break;

			case 2:	/*display time*/
				EPR_Read(EVENTS5_BASE_ADDRESS, (uint8_t*)&tamperTime, sizeof(tamperTime));
				lcd_display_time(tamperTime.hour,tamperTime.minute,tamperTime.second);
				break;
		}
	}
	
	if(cover_disp_timer>=6)
	{
		cover_disp_timer=0;
		g_cover_display_state++;
		if(g_cover_display_state>2)
			g_cover_display_state=0;
	}
}

void d_COMM_ON(void)
{
	LCD_ClearAll();	

	LCD_DisplayDigit(1, LCD_CHAR_C);
	LCD_DisplayDigit(2, 0);
	LCD_DisplayDigit(3, LCD_CHAR_M);
	LCD_DisplayDigit(4, LCD_CHAR_M);
	LCD_DisplayDigit(5, LCD_MINUS_SIGN);
	LCD_DisplayDigit(6, 0);
	LCD_DisplayDigit(7, LCD_CHAR_N);	
}

void d_DNLD(void)
{
	LCD_ClearAll();	

	LCD_DisplayDigit(2, LCD_CHAR_D);
	LCD_DisplayDigit(3, LCD_CHAR_N);
	LCD_DisplayDigit(4, LCD_CHAR_L);
	LCD_DisplayDigit(5, LCD_CHAR_D);
}

void d_PowerOff_Current(void)
{
	display_PowerOff(0);
}
void d_PowerOff_Cum(void)
{
	display_PowerOff(1);
  LCD_DisplaySpSign(S_T8);	// CUM Sign
}

void d_PowerOff_Bill_L2(void)
{
	display_Billing_PowerOff(2);	
  LCD_DisplaySpSign(S_T8);	// CUM Sign
  LCD_DisplaySpSign(S_BILL);
//	LCD_DisplayDigit(9, LCD_CHAR_L);
	LCD_DisplayDigit(8, 2);
}

void d_resetCount(void)
{
	LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT);
	LCD_DisplayDigit(9, LCD_CHAR_P);
  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
	LCD_DisplayDigit(8, LCD_CHAR_C);
  LCD_DisplaySpSign(S_T8);	// CUM Sign
}

// Bill energies Import
void d_CumKWH_L1_Import(void)
{
	display_Energy_Billing(IMPORT, 1, BILL_KWH_ENERGY); 
}
void d_CumKVAH_L1_Import(void)
{
	display_Energy_Billing(IMPORT, 1, BILL_KVAH_ENERGY);	
}
void d_AVGPF_L1_Import(void)
{
	display_Billing_AVG_PF(IMPORT, 1);	
}

void d_CumKWH_L2_Import(void)
{
	display_Energy_Billing(IMPORT, 2, BILL_KWH_ENERGY); 
//	LCD_DisplayDigit(8, LCD_CHAR_E);
	
}
void d_CumKVAH_L2_Import(void)
{
	display_Energy_Billing(IMPORT, 2, BILL_KVAH_ENERGY); 
}
void d_AVGPF_L2_Import(void)
{
	display_Billing_AVG_PF(IMPORT, 2);	
}

	
	
//Bill TOD energies Import
void d_CumKWH_TOD1_L1_Import(void)
{
	display_Energy_Billing_TOD(IMPORT,1, 1, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD2_L1_Import(void)
{
	display_Energy_Billing_TOD(IMPORT,1, 2, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD3_L1_Import(void)
{
	display_Energy_Billing_TOD(IMPORT,1, 3, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD4_L1_Import(void)
{
	display_Energy_Billing_TOD(IMPORT,1, 4, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD5_L1_Import(void)
{
	display_Energy_Billing_TOD(IMPORT,1, 5, BILL_KWH_ENERGY); 
}

// Bill L1 energies Export
void d_CumKWH_L1_Export(void)
{
	display_Energy_Billing(EXPORT, 1, BILL_KWH_ENERGY); 
}
void d_CumKVAH_L1_Export(void)
{
	display_Energy_Billing(EXPORT, 1, BILL_KVAH_ENERGY); 
}
void d_AVGPF_L1_Export(void)
{
	display_Billing_AVG_PF(EXPORT, 1);
//	LCD_DisplayDigit(8, LCD_CHAR_E);
}
void d_CumKWH_L2_Export(void)
{
	display_Energy_Billing(EXPORT, 2, BILL_KWH_ENERGY);
//	LCD_DisplayDigit(8, LCD_CHAR_E);

}
void d_CumKVAH_L2_Export(void)
{
	display_Energy_Billing(EXPORT, 2, BILL_KVAH_ENERGY); 
}
void d_AVGPF_L2_Export(void)
{
	display_Billing_AVG_PF(EXPORT, 2);
//	LCD_DisplayDigit(8, LCD_CHAR_E);
}

//Bill TOD energies Export
void d_CumKWH_TOD1_L1_Export(void)
{
	display_Energy_Billing_TOD(EXPORT,1, 1, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD2_L1_Export(void)
{
	display_Energy_Billing_TOD(EXPORT,1, 2, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD3_L1_Export(void)
{
	display_Energy_Billing_TOD(EXPORT,1, 3, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD4_L1_Export(void)
{
	display_Energy_Billing_TOD(EXPORT,1, 4, BILL_KWH_ENERGY); 
}
void d_CumKWH_TOD5_L1_Export(void)
{
	display_Energy_Billing_TOD(EXPORT,1, 5, BILL_KWH_ENERGY); 
}


// Current TOD Energies Import
void d_CumKWH_TOD1_Import(void)
{
	display_CumKWH_TOD(IMPORT,1);	
}
void d_CumKWH_TOD2_Import(void)
{
	display_CumKWH_TOD(IMPORT,2);	
}
void d_CumKWH_TOD3_Import(void)
{
	display_CumKWH_TOD(IMPORT,3);	
}
void d_CumKWH_TOD4_Import(void)
{
	display_CumKWH_TOD(IMPORT,4);	
}
void d_CumKWH_TOD5_Import(void)
{
	display_CumKWH_TOD(IMPORT,5);	
}
// Current TOD Energies Export
void d_CumKWH_TOD1_Export(void)
{
	display_CumKWH_TOD(EXPORT,1);	
}
void d_CumKWH_TOD2_Export(void)
{
	display_CumKWH_TOD(EXPORT,2);	
}
void d_CumKWH_TOD3_Export(void)
{
	display_CumKWH_TOD(EXPORT,3);	
}
void d_CumKWH_TOD4_Export(void)
{
	display_CumKWH_TOD(EXPORT,4);	
}
void d_CumKWH_TOD5_Export(void)
{
	display_CumKWH_TOD(EXPORT,5);	
}


void d_netCumKWH(void)
{
	int32_t diff;
	diff = (myenergydata.kwh -myenergydata.kwh_export);
	if(diff<0)
		diff=diff*-1;
		
	displayEnergy(diff, BILL_KWH_ENERGY);
  LCD_DisplaySpSign(S_T8);	// CUM Sign
// what about the Net symbol	
	LCD_DisplayDigit(9, LCD_CHAR_N);
	LCD_DisplayDigit(8, LCD_CHAR_E);
	
	if(myenergydata.kwh<myenergydata.kwh_export)
		LCD_DisplayDigit(1, LCD_MINUS_SIGN);
}

// Current Energies & Average pF Import
void d_CumKWH_Import(void)
{
	display_CumKWH(IMPORT);
}
void d_CumKVAH_Import(void)
{
	display_CumKVAH(IMPORT);
}
void d_CumKVARH_Lag_Import(void)
{
	display_CumKVARH(IMPORT,D_LAG);
}
void d_CumKVARH_Lead_Import(void)
{
	display_CumKVARH(IMPORT,D_LEAD);
}
void d_AVGPF_Current_Import(void)
{
	display_Current_AVG_PF(IMPORT);
}

// Current Energies & Average pF Export
void d_CumKWH_Export(void)
{
	display_CumKWH(EXPORT);
}
void d_CumKVAH_Export(void)
{
	display_CumKVAH(EXPORT);
}
void d_CumKVARH_Lag_Export(void)
{
	display_CumKVARH(EXPORT,D_LAG);
}
void d_CumKVARH_Lead_Export(void)
{
	display_CumKVARH(EXPORT,D_LEAD);
}
void d_AVGPF_Current_Export(void)
{
	display_Current_AVG_PF(EXPORT);
}



void d_MD_Current_Import(void)
{
	display_MD_Current(IMPORT);
}
void d_MD_L1_Import(void)
{// for TN MD is kw by default and can be changed to KVA - but only one is to be shown
// hence KW or KVA can be avoided
	display_MD_Billing(IMPORT,1);
}

void d_MD_L2_Import(void)
{
	display_MD_Billing(IMPORT,2);
}

void d_MD_BillDate_Import(void)
{
	display_MD_BillingDateTime_L1(IMPORT, MDDATE);
}
void d_MD_BillTime_Import(void)
{
	display_MD_BillingDateTime_L1(IMPORT, MDTIME);
}
void d_CumMD_Import(void)
{
	display_CumMD(IMPORT);
}



void d_MD_Current_Export(void)
{
	display_MD_Current(EXPORT);
}
void d_MD_L1_Export(void)
{
	display_MD_Billing(EXPORT,1);
}
void d_MD_L2_Export(void)
{
	display_MD_Billing(EXPORT,2);
}
void d_MD_BillDate_Export(void)
{
	display_MD_BillingDateTime_L1(EXPORT, MDDATE);
}
void d_MD_BillTime_Export(void)
{
	display_MD_BillingDateTime_L1(EXPORT, MDTIME);
}
void d_CumMD_Export(void)
{
	display_CumMD(EXPORT);
}



void d_Vrms_R(void)
{
	display_Vrms(LINE_PHASE_R);
	LCD_DisplayDigit(9, LCD_CHAR_R);	
}
void d_Vrms_Y(void)
{
	display_Vrms(LINE_PHASE_Y);
	LCD_DisplayDigit(9, LCD_CHAR_Y);	
}
void d_Vrms_B(void)
{
	display_Vrms(LINE_PHASE_B);
	LCD_DisplayDigit(9, LCD_CHAR_B);	
}
void d_Irms_R(void)
{
	display_Irms(LINE_PHASE_R);
	LCD_DisplayDigit(9, LCD_CHAR_R);	
}
void d_Irms_Y(void)
{
	display_Irms(LINE_PHASE_Y);
	LCD_DisplayDigit(9, LCD_CHAR_Y);	
}
void d_Irms_B(void)
{
	display_Irms(LINE_PHASE_B);
	LCD_DisplayDigit(9, LCD_CHAR_B);	
}
void d_Irms_Neutral(void)
{
	int32_t tempLong;
	tempLong = getInstantaneousParams(ICURRENTS,LINE_NEUTRAL)*1000;
	display_long_three_dp(tempLong);
	lcd_display_A();
	LCD_DisplayDigit(9, LCD_CHAR_N);	
	g_display_flag.fast_display_flag=1;
}


void displaySignedPF(EM_LINE line)
{
	int32_t tempLong;
	EM_PF_SIGN pf_sign;    
	
	pf_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,line);
	
//	if((line==LINE_TOTAL)&&(twoWireOperation!=0))
//		tempLong=operatingPF*1000;
//	else
	tempLong = getInstantaneousParams(IPFS,line)*1000;
	display_long_three_dp(tempLong);
	LCD_DisplayDigit(1, LCD_CHAR_P);	
	LCD_DisplayDigit(2, LCD_CHAR_F);	
	
/*	
	if(exportMode==0)
	{// this is the regular case - import
		if (pf_sign == PF_SIGN_LEAD_C)
		    LCD_DisplayDigit(1, LCD_CHAR_C);
		else if (pf_sign == PF_SIGN_LAG_L)
		    LCD_DisplayDigit(1, LCD_CHAR_L);
		else if (pf_sign == PF_SIGN_UNITY)
		    LCD_ClearDigit(1);
	}
	else
	{
		if (pf_sign == PF_SIGN_LEAD_C)
		    LCD_DisplayDigit(1, LCD_CHAR_L);	// show the ulta
		else if (pf_sign == PF_SIGN_LAG_L)
		    LCD_DisplayDigit(1, LCD_CHAR_C);	// show the ulta
		else if (pf_sign == PF_SIGN_UNITY)
		    LCD_ClearDigit(1);
	}
*/	
}

void d_SystemSignedPF(void)
{
	displaySignedPF(LINE_TOTAL);
//	LCD_DisplayDigit(9, LCD_CHAR_P);
//	LCD_DisplayDigit(8, LCD_CHAR_F);
	LCD_DisplayDigit(9, LCD_CHAR_T);
}
void d_SignedPF_R(void)
{
	displaySignedPF(LINE_PHASE_R);
//	LCD_DisplayDigit(9, LCD_CHAR_P);
	LCD_DisplayDigit(9, LCD_CHAR_R);
}
void d_SignedPF_Y(void)
{
	displaySignedPF(LINE_PHASE_Y);
//	LCD_DisplayDigit(9, LCD_CHAR_P);
	LCD_DisplayDigit(9, LCD_CHAR_Y);
}
void d_SignedPF_B(void)
{
	displaySignedPF(LINE_PHASE_B);
//	LCD_DisplayDigit(9, LCD_CHAR_P);
	LCD_DisplayDigit(9, LCD_CHAR_B);
}
void d_LCDcheck(void)
{
  LCD_DisplayAll();
	gLcdCheck=1;	// global variable to indicate that icon updates must not be carried out
}
void d_MeterSlNo(void)
{
	LCD_DisplayIntWithPos(Common_Data.meterNumber, LCD_LAST_POS_DIGIT);
	lcd_display_id();
}
void d_CurrentDate(void)
{
	EM_DisplayDate();
}
void d_CurrentTime(void)
{
	EM_DisplayInstantTime();
	g_display_flag.fast_display_flag=1;
}
void d_PhaseSequence(void)
{
  int16_t v[EM_NUM_OF_PHASE];	
  powermgmt_mode_t power_mode;
	uint8_t phasesGT170LT300;

	power_mode = POWERMGMT_GetMode();
	if (power_mode != POWERMGMT_MODE3)
	{
		v[LINE_PHASE_R] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*10);
		v[LINE_PHASE_Y] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*10);
		v[LINE_PHASE_B] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*10);

		phasesGT170LT300 = countVoltageGT170LT300(v[LINE_PHASE_R],v[LINE_PHASE_Y],v[LINE_PHASE_B]);	// returns phases between 170 and 300
		
		if(phasesGT170LT300==3)
		{
			if(gPhaseSequence==0)
			{
				LCD_DisplayDigit(3, LCD_CHAR_R);
				LCD_DisplayDigit(4, LCD_CHAR_Y);
				LCD_DisplayDigit(5, LCD_CHAR_B);
			}
			else
			{
				LCD_DisplayDigit(3, LCD_CHAR_R);
				LCD_DisplayDigit(4, LCD_CHAR_B);
				LCD_DisplayDigit(5, LCD_CHAR_Y);
			}
		}
		else
		{
			LCD_DisplayDigit(3, LCD_MINUS_SIGN);
			LCD_DisplayDigit(4, LCD_MINUS_SIGN);
			LCD_DisplayDigit(5, LCD_MINUS_SIGN);
		}
	}
	else
	{
		LCD_DisplayDigit(3, LCD_MINUS_SIGN);
		LCD_DisplayDigit(4, LCD_MINUS_SIGN);
		LCD_DisplayDigit(5, LCD_MINUS_SIGN);
	}
  LCD_DisplayDigit(9, LCD_CHAR_P);
  LCD_DisplayDigit(8, 5);
}
void d_Frequency(void)
{
	int32_t tempLong;
	tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
	display_long_two_dp(tempLong);
  LCD_DisplayDigit(1, LCD_CHAR_F);
  LCD_DisplayDigit(9, LCD_CHAR_H);
  LCD_DisplayDigit(8, 2);
}
void d_ActivePower_R(void)
{
	displayActivePower(LINE_PHASE_R);
	LCD_DisplayDigit(9, LCD_CHAR_R);
}
void d_ActivePower_Y(void)
{
	displayActivePower(LINE_PHASE_Y);
	LCD_DisplayDigit(9, LCD_CHAR_Y);
}
void d_ActivePower_B(void)
{
	displayActivePower(LINE_PHASE_B);
	LCD_DisplayDigit(9, LCD_CHAR_B);
}
void d_ActivePower_Total(void)
{
	displayActivePower(LINE_TOTAL);
	LCD_DisplayDigit(9, LCD_CHAR_T);
}
void d_ApparentPower_Total(void)
{
	displayApparentPower(LINE_TOTAL);
	LCD_DisplayDigit(9, LCD_CHAR_T);
}
void d_RisingDemand_Import(void)
{
	displayRisingDemand(IMPORT);
}
void d_RisingDemand_Export(void)
{
	displayRisingDemand(EXPORT);
}


/*
NOTE:
If we had an eight digit display with a decimal point at 6 then the following code could be used

#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kwh%1000000)*1000)+(int32_t)(high_resolution_kwh_value);
	display_long_six_dp(temp_high_res_energy);
#else
	temp_high_res_energy = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
	display_long_five_dp(temp_high_res_energy);
#endif
*/



void d_HighRes_KWH_Import(void)
{// see NOTE above
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kwh%100000)*100)+(int32_t)(high_resolution_kwh_value/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kwh_value%10);
#else
	temp_high_res_energy = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykWhSign();
	LCD_DisplaySpSign(S_IMPORT);
	g_display_flag.fast_display_flag=1;
}

void d_HighRes_KVAH_Import(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kvah%100000)*100)+(int32_t)(high_resolution_kvah_value/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kvah_value%10);
#else
	temp_high_res_energy = ((myenergydata.kvah%100000)*100)+(int32_t)high_resolution_kvah_value;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykVAhSign();
	LCD_DisplaySpSign(S_IMPORT);
	g_display_flag.fast_display_flag=1;
}
			
void d_HighRes_KVARH_LAG_Import(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kvarh_lag%100000)*100)+(int32_t)(high_resolution_kvarh_lag_value/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kvarh_lag_value%10);
#else
	temp_high_res_energy = ((myenergydata.kvarh_lag%100000)*100)+(int32_t)high_resolution_kvarh_lag_value;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykVArhSign();
//	LCD_DisplayDigit(9, LCD_CHAR_L);
//	LCD_DisplayDigit(8, LCD_CHAR_G);
	LCD_DisplaySpSign(S_IMPORT);
	LCD_DisplaySpSign(S_LAG);
	g_display_flag.fast_display_flag=1;
}
			
void d_HighRes_KVARH_LEAD_Import(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kvarh_lead%100000)*100)+(int32_t)(high_resolution_kvarh_lead_value/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kvarh_lead_value%10);
#else
	temp_high_res_energy = ((myenergydata.kvarh_lead%100000)*100)+(int32_t)high_resolution_kvarh_lead_value;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykVArhSign();
//	LCD_DisplayDigit(9, LCD_CHAR_L);
//	LCD_DisplayDigit(8, LCD_CHAR_D);
	LCD_DisplaySpSign(S_IMPORT);
	LCD_DisplaySpSign(S_LEAD);
	g_display_flag.fast_display_flag=1;
}

void d_HighRes_KWH_Export(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kwh_export%100000)*100)+(int32_t)(high_resolution_kwh_value_export/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kwh_value_export%10);
#else
	temp_high_res_energy = ((myenergydata.kwh_export%100000)*100)+(int32_t)high_resolution_kwh_value_export;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykWhSign();
//	LCD_DisplayDigit(9, LCD_CHAR_E);
//	LCD_DisplayDigit(8, LCD_CHAR_E);
	LCD_DisplaySpSign(S_EXPORT);
	g_display_flag.fast_display_flag=1;
}
			
void d_HighRes_KVAH_Export(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kvah_export%100000)*100)+(int32_t)(high_resolution_kvah_value_export/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kvah_value_export%10);
#else
	temp_high_res_energy = ((myenergydata.kvah_export%100000)*100)+(int32_t)high_resolution_kvah_value_export;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykVAhSign();
//	LCD_DisplayDigit(8, LCD_CHAR_E);
	LCD_DisplaySpSign(S_EXPORT);
	g_display_flag.fast_display_flag=1;
}
			
void d_HighRes_KVARH_LAG_Export(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kvarh_lag_export%100000)*100)+(int32_t)(high_resolution_kvarh_lag_value_export/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kvarh_lag_value_export%10);
#else
	temp_high_res_energy = ((myenergydata.kvarh_lag_export%100000)*100)+(int32_t)high_resolution_kvarh_lag_value_export;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykVArhSign();
//	LCD_DisplayDigit(9, LCD_CHAR_L);
//	LCD_DisplayDigit(8, LCD_CHAR_G);
	LCD_DisplaySpSign(S_EXPORT);
	LCD_DisplaySpSign(S_LAG);
	g_display_flag.fast_display_flag=1;
}
			
void d_HighRes_KVARH_LEAD_Export(void)
{
	int32_t temp_high_res_energy;
#ifdef SIX_DECIMALS	
	temp_high_res_energy = ((myenergydata.kvarh_lead_export%100000)*100)+(int32_t)(high_resolution_kvarh_lead_value_export/10);
	display_long_five_dp(temp_high_res_energy);
	LCD_DisplayDigit(9, high_resolution_kvarh_lead_value_export%10);
#else
	temp_high_res_energy = ((myenergydata.kvarh_lead_export%100000)*100)+(int32_t)high_resolution_kvarh_lead_value_export;
	display_long_five_dp(temp_high_res_energy);
#endif
	LCD_DisplaykVArhSign();
//	LCD_DisplayDigit(9, LCD_CHAR_L);
//	LCD_DisplayDigit(8, LCD_CHAR_D);
	LCD_DisplaySpSign(S_EXPORT);
	LCD_DisplaySpSign(S_LEAD);
	g_display_flag.fast_display_flag=1;
}
						































