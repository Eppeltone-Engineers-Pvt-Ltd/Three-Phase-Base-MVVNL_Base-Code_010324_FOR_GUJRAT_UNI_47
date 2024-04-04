// File Name    : r_dlms_user_interface.c
#include <stdio.h>

// Drivers  
#include "r_cg_macrodriver.h"			// Macro Driver Definitions  
#include "r_cg_rtc.h"					// RTC Driver  
#include "r_cg_wdt.h"					// CG WDT Driver  

// DLMS  
#include "r_dlms_typedef.h"				// DLMS Type Definitions  
#include "r_dlms_data.h"				// DLMS Data Definitions  
#include "r_dlms_obis.h"				// DLMS OBIS Definitions  
#include "r_dlms_obis_ic.h"				// DLMS OBIS IC Definitions  
#include "DLMS_User.h"					// DLMS User Definitions  
#include "r_dlms_user_interface.h"		// DLMS User Interface Definitions  
#include "r_dlms_data_meter.h"

#include "emeter3_structs.h"
#include "emeter3_app.h"
#include "tampers.h"
#include "survey.h"
#include "e2rom.h"
#include "utilities.h"
#include "memoryOps.h"


// Standard Lib  
#include <string.h>						// String Standard Lib  

// EM Core Component  
#include "format.h"			// EEPROM Format  
//#include "event_DLMS.h"		// Event Header File  
#include "em_core.h"

//Macro definitions

//Typedef definitions

//Imported global variables and functions (from other files)

void do_initial_things(void);
void readRTC(void);




// Class ID = 07 - Profile generic class  
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
extern Unsigned8					g_Class07_Obj0_Capture_Enable;
extern Unsigned32				g_Class07_Blockload_CapturePeriod;
extern Unsigned32					g_Class07_Blockload_EntriesInUse;
extern Unsigned16					g_Class07_Blockload_CurrentEntry;
extern const Unsigned32					g_Class07_Blockload_MaxEntries;
//extern class07_blockload_entry_t	g_Class07_BlockLoadBuffer[];

extern const Unsigned32				g_Class07_Dailyload_CapturePeriod;
extern Unsigned32					g_Class07_Dailyload_EntriesInUse;
extern Unsigned16					g_Class07_Dailyload_CurrentEntry;
extern const Unsigned32					g_Class07_Dailyload_MaxEntries;
//extern class07_dailyload_entry_t	g_Class07_DailyLoadBuffer[];

extern Unsigned32					g_Class07_Billing_EntriesInUse;
extern Unsigned16					g_Class07_Billing_CurrentEntry;
extern const Unsigned32					g_Class07_Billing_MaxEntries;
//extern class07_billing_entry_t	g_Class07_BillingBuffer[];

extern Unsigned32					g_Class07_Event_EntriesInUse[];
extern Unsigned16					g_Class07_Event_CurrentEntry[];
extern Unsigned32					g_Class07_Event_MaxEntries[];
//extern class07_event_entry_t		g_Class07_EventBuffer[CLASS07_EVENT_MAX_PROFILE][CLASS07_EVENT_MAX_ENTRY];
//extern STORAGE_EEPROM_HEADER 		g_storage_header;								// Storage Header  
#endif

//USER VARIABLE
/*
unsigned int avg_pf;
unsigned long int kwmd_date;//,kvamd_date;
unsigned long int kvamd_date;
date_time_t tmp_time_string;
unsigned int kwmd_val,kvamd_val;
unsigned  int kwmd_time;
unsigned long int kvamd_time;
unsigned long int demand_volt;
unsigned char billing_day;
unsigned int event_cnt[TOT_EVENT_TYPE];
extern class07_billing_entry_t g_Class07_BillingBuffer;
*/

extern Unsigned8 secret1[9];//="ABCDEFGH";
extern Unsigned8 secret2[17];//="RENESAS_P6wRJ21F";
/*
//unsigned long int meter_no;
void convert_data_string(unsigned long int tmp_long, unsigned char size);	// not needed - 25/Jul/2020
unsigned char string1[8];	// not needed - 25/Jul/2020
//Exported global variables and functions (to be accessed by other files)
*/
//Private global variables and functions
/*
void convert_data_string(unsigned long int tmp_long, unsigned char size)
{
	unsigned char i;
	
	for(i=0;i<size;i++)
	{
		string1[size-i-2]=tmp_long%10+0x30;
		tmp_long=tmp_long/10;
	}
}
*/

// RL78/G13 Day Of Week Driver  
const Unsigned8 g_DayOfWeek[7] =
{	
	DATE_WEEK_SUN,		// 0 is Sunday  
	DATE_WEEK_MON,		// 1 is Monday  
	DATE_WEEK_TUE,		// 2 is Tuesday  
	DATE_WEEK_WED,		// 3 is Wednesday  
	DATE_WEEK_THU,		// 4 is Thusday  
	DATE_WEEK_FRI,		// 5 is Friday  
	DATE_WEEK_SAT,		// 6 is Saturday  
	
};

void R_USER_Set_RTC_timezone(void)
{
	//g_Class08_RTC_TimeZone
	
	
}

void R_USER_Convert2date_time(date_time_t *p_date_time, RTC_DATE_TIME *p_rtctime)
{
}

void R_USER_GetRTCTime(date_time_t *p_date_time)
{
	uint8_t  status;	// Status of operation  
	uint16_t timeout;		
	rtc_calendarcounter_value_t rtctime; // RTC value for Driver IF  

	// Check parameters
	if (p_date_time == NULL)
		return;

	timeout = 100;
	R_RTC_Get_CalendarCounterValue(&rtctime);
	status = MD_OK;

	if (status != MD_OK)
		return;

	// Convert all to decimal
	_BCD2DEC(rtctime.rseccnt);
	_BCD2DEC(rtctime.rmincnt);
	_BCD2DEC(rtctime.rhrcnt);
	_BCD2DEC(rtctime.rdaycnt);
	_BCD2DEC(rtctime.rwkcnt);
	_BCD2DEC(rtctime.rmoncnt);
	_BCD2DEC(rtctime.ryrcnt);

	// Convert to date_time_t structure
	// HIGH of Year
	timeout = 2000 + rtctime.ryrcnt;
	p_date_time->year_high         = (Unsigned8)(timeout >> 8);
	// LOW  of Year
	p_date_time->year_low          = (Unsigned8)timeout;
	p_date_time->month             = (Unsigned8)rtctime.rmoncnt;					// Month
	p_date_time->day_of_month      = (Unsigned8)rtctime.rdaycnt;					// Day
	p_date_time->day_of_week       = (Unsigned8)g_DayOfWeek[rtctime.rwkcnt];		// Friday
	p_date_time->hour              = (Unsigned8)rtctime.rhrcnt;					// Hour
	p_date_time->minute            = (Unsigned8)rtctime.rmincnt;					// Minute
	p_date_time->second            = (Unsigned8)rtctime.rseccnt;					// Second
	// Not support
	p_date_time->hundredths        = (Unsigned8)TIME_HUNDREDTHS_NOT_SPECIFIED;
	// Time zone in min 
	p_date_time->deviation_high    = 0x80;//(Unsigned8)(((Unsigned16)7 * 60) >> 8);
 	p_date_time->deviation_low     = 0x00;//(Unsigned8)(((Unsigned16)7 * 60));
	// Daylight saving active 
	p_date_time->clock_status      = (Unsigned8)NOT_SPECIFIED;
}

void R_USER_SetRTCTime(date_time_t date_time)
{
	Unsigned8  i;					// Counter  
	Unsigned8  status;				// Status of operation  
	Unsigned16 timeout;				// Timeout counter to ensure the system not crash  
	rtc_calendarcounter_value_t rtctime;		// RTC value for Driver IF  
	date_time_t original_date_time;
	Unsigned8  *p_u8;

	// Get original date_time first  
	R_USER_GetRTCTime(&original_date_time);

	// Checking the correction before setting or keep as original  
	if (date_time.year_high != 0xFF && date_time.year_low != 0xFF)
	{
		original_date_time.year_high = date_time.year_high;
		original_date_time.year_low = date_time.year_low;
	}
	if (date_time.month < 13 && date_time.month > 0)
	{
		original_date_time.month = date_time.month;
	}
	if (date_time.day_of_month < 32 && date_time.day_of_month > 0)
	{
		original_date_time.day_of_month = date_time.day_of_month;
	}
	if (date_time.hour < 24)
	{
		original_date_time.hour = date_time.hour;
	}
	if (date_time.minute < 60)
	{
		original_date_time.minute = date_time.minute;
	}
	if (date_time.second < 60)
	{
		original_date_time.second = date_time.second;
	}
	if(date_time.deviation_high != 0x80 && date_time.deviation_low != 0x00) // 0x8000 = not specified
	{
		original_date_time.deviation_high = date_time.deviation_high;
		original_date_time.deviation_low = date_time.deviation_low;
	}

	// Recalculate year  
	timeout         = ((Unsigned16)original_date_time.year_high << 8);
	timeout        += (Unsigned16)original_date_time.year_low;

	// Recalculate day of week  
	original_date_time.day_of_week = 0xFF;// (Unsigned8)R_OBIS_wday((Integer16)timeout, (Integer16)original_date_time.month, (Integer16)original_date_time.day_of_month);

	// Get the RTC value from date_time  
	rtctime.rdaycnt   = date_time.day_of_month;
	rtctime.rmoncnt   = date_time.month;
	rtctime.ryrcnt    = (Unsigned8)((timeout % 100));
	rtctime.rhrcnt    = date_time.hour;
	rtctime.rmincnt   = date_time.minute;
	rtctime.rseccnt   = date_time.second;
	for (i = 0; i < 7; i++)
	{
		if (g_DayOfWeek[i] == original_date_time.day_of_week)
		{
			break;
		}
	}
	if (i == 7)
	{
		i = 0;
	}
	rtctime.rwkcnt    = i;

	// Convert to BCD  
	_DEC2BCD(rtctime.rdaycnt);
	_DEC2BCD(rtctime.rmoncnt);
	_DEC2BCD(rtctime.ryrcnt);
	_DEC2BCD(rtctime.rhrcnt);
	_DEC2BCD(rtctime.rmincnt);
	_DEC2BCD(rtctime.rseccnt);
	_DEC2BCD(rtctime.rwkcnt);

	// Set to by RTC driver IF  
	timeout = 100;
	R_RTC_Set_CalendarCounterValue(rtctime);
	status = MD_OK;
	
	readRTC();	// this is to get the values in our data structure

 
	do_initial_things();
	
	Common_Data.pgm_count++;
	e2write_flags|=E2_W_IMPDATA;

	
// what do the following three lines do?	
//	//p_u8 = (Unsigned8 *)&330;
//	*(p_u8 + 1) = 0x80;//original_date_time.deviation_high;
//	*(p_u8) = 0x00;//original_date_time.deviation_low;
}

Unsigned8 R_USER_GetSpecificTime(Unsigned16 time_id, date_time_t *p_date_time)
{
	Unsigned8 result = 1;
	Unsigned8 support_log_class7 = 0;

	switch(time_id)
	{
		case CURRENT_TIME:
			R_USER_GetRTCTime(p_date_time);
			result = 0;
			break;
			
		case LASTBILLING_TIME:
			{
				if (support_log_class7 == 1)
				{
//					memcpy((Unsigned8 *)p_date_time,(Unsigned8 *)&energy_log.bill_date, 12);
				}
			}
			result = 0;
			break;
		case LAST_ACTIVE_MD_TIME:
			{
				if (support_log_class7 == 1)
				{
					//memcpy((Unsigned8 *)p_date_time,(Unsigned8 *)&energy_log.active_MD_time, 12); // NEED TO IMPLEMENT
				}
			}
			result = 0;
			break;
		case LAST_APPARENT_MD_TIME:
			{
				if (support_log_class7 == 1)
				{
					//memcpy((Unsigned8 *)p_date_time,(Unsigned8 *)&energy_log.apparent_MD_time, 12); // NEED TO IMPLEMENT
				}
			}
			result = 0;
			break;
		// And more …  
		default:
			return 1;
	}

	return result;
}

Unsigned8 R_USER_SetSpecificTime(Unsigned16 time_id, date_time_t date_time)
{
	Unsigned8 result = 1;

	switch(time_id)
	{
		case CURRENT_TIME:
			R_USER_SetRTCTime(date_time);
			result = 0;
			break;
		case LASTBILLING_TIME:
		case LAST_ACTIVE_MD_TIME:
		case LAST_APPARENT_MD_TIME:
			result = 1; // Not allow to change
			break;
		// And more …  
		default:
			return 1;
	}

	return result;
}

void R_USER_WDT_Restart(void)
{
	R_WDT_Restart();
}

const unsigned char manfact_name[30]="Eppeltone Engineers Pvt Ltd\0";
//const unsigned char manfact_name[40]="United Electrical Industries Limited\0";

const unsigned char ldn_name[16]="EEOEPL\0";
//const unsigned char ldn_name[16]="UEIL\0";

//unsigned char manfact_name[25]="EPPELTONE\0"; // not used
#if defined(METER_CLASS) && (METER_CLASS==5)
const unsigned char meter_rating[10]="5-10 A\0";
#else
const unsigned char meter_rating[10]="10-60 A\0";
#endif


Integer16 R_USER_GetNamePlate(Unsigned16 nameplate_id, Unsigned8* p_data)
{
	Integer16 result_len = -1;
	Unsigned16	u16;
	Unsigned8 firmware_version[]="V1.01\0";

#if (defined(METER_CLASS) && METER_CLASS == 5)
	Unsigned8 category_name[]="C1\0";
#else	
	Unsigned8 category_name[]="C2\0";
#endif

//	Unsigned8 category_name[]="C2\0";

	Unsigned8 tbuf[16];
	
	if(p_data == NULL)
	{
		return -1;
	}

	switch(nameplate_id)
	{
		case NAME_PLATE_LOGICAL_NAME:
			result_len = strlen((void*)ldn_name);
			memcpy((Unsigned8 *)p_data,(Unsigned8 *)ldn_name, result_len);
			break;
		case NAME_PLATE_METER_SERIAL:
//			convert_data_string(Common_Data.meterNumber,8); 				
//			result_len = strlen((void*)string1);
//			memcpy((Unsigned8 *)p_data,(Unsigned8 *)string1, result_len);
			getFullMeterNumber((uint8_t*)&tbuf[0]);
//			sprintf((char*)tbuf,"%ld",Common_Data.meterNumber); 				
			result_len = strlen((char*)tbuf);
			memcpy((Unsigned8 *)p_data, tbuf, result_len);

			break;
		case NAME_PLATE_MANUFACT_NAME:
			result_len = strlen((void*)manfact_name);
			memcpy((Unsigned8 *)p_data,(Unsigned8 *)manfact_name, result_len);
			break;
		case NAME_PLATE_FIRMWARE_NAME:
			result_len = strlen((void*)firmware_version);
			memcpy((Unsigned8 *)p_data,(Unsigned8 *)firmware_version, result_len);
			break;
		case NAME_PLATE_METER_TYPE:
			//ATTR_TYPE_LONG_UNSIGNED
			// (2 bytea) a value of 5 indicates 1P-2W 
#if defined(METER_CLASS) && (METER_CLASS==5)
			*p_data++ = 3;
#else
			*p_data++ = 4;
#endif			
			*p_data++ = 0;
			result_len = 2;
			break;
		case NAME_PLATE_CATEGORY_NAME:
			result_len = strlen((void*)category_name);
			memcpy((Unsigned8 *)p_data,(Unsigned8 *)category_name, result_len);
			break;
		case NAME_PLATE_CURRRATE_NAME:
			result_len = strlen((void*)meter_rating);
			memcpy((Unsigned8 *)p_data,(Unsigned8 *)meter_rating, result_len);
			break;
			
		case NAME_PLATE_YEAR_OF_MANUFACT:
			//ATTR_TYPE_LONG_UNSIGNED
			u16 = 2000 + Common_Data.batchyear;
//			u16 = 2018;
			*p_data++ = (Unsigned8)(u16 &0xFF);
			*p_data++ = (Unsigned8)((u16>>8) &0xFF);
			result_len = 2;
			break;

		case NAME_PLATE_CT_RATIO:
			*p_data++ = 1;
			*p_data++ = 0;
			result_len = 2;
			break;

		case NAME_PLATE_PT_RATIO:
			*p_data++ = 1;
			*p_data++ = 0;
			result_len = 2;
			break;
			
			
		// And more …  
		default:
			return -1;
	}
	
	return result_len;
}

//#define CURRENT_BLOCK_ENERGY	// comment this if previous block energy is required
//#define CURRENT_MONTH_DURATION	// option not available at present



Integer32 R_USER_GetEMData(Unsigned16 em_data)
{
	Integer32 result = 0;
	Integer32 temp = 0;
	uint16_t utemp16=0;

#ifndef CURRENT_BLOCK_ENERGY
	uint32_t addr;
#endif

	switch(em_data)
	{
		case I_R:
			result = (getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*100.0);
			break;
		
		case I_Y:
			result = (getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*100.0);
			break;
		
		case I_B:
			result = (getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*100.0);
			break;
		
		case V_R:
			result = (getInstantaneousParams(IVOLTS,LINE_PHASE_R)*10.0);
			break;
		
		case V_Y:
			result = (getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*10.0);
			break;
		
		case V_B:
			result = (getInstantaneousParams(IVOLTS,LINE_PHASE_B)*10.0);
			break;

		case PF_R:
			result = (int32_t)(getInstantaneousParams(IPFS,LINE_PHASE_R)*1000.0);
			break;
		
		case PF_Y:
			result = (int32_t)(getInstantaneousParams(IPFS,LINE_PHASE_Y)*1000.0);
			break;
		
		case PF_B:
			result = (int32_t)(getInstantaneousParams(IPFS,LINE_PHASE_B)*1000.0);
			break;

		case PF_TOTAL:	// average pf - instantaneous
				result = (int32_t)(getInstantaneousParams(IPFS,LINE_TOTAL)*1000.0);
			break;

		case APPARENT_POWER:	// kVA
			result = getInstantaneousParams(APP_POWER,LINE_TOTAL);
			break;

		case ACTIVE_POWER:	// KW signed
			result = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			break;

		case REACTIVE_POWER:	// kVAR - signed +Lag -Lead
			result = getInstantaneousParams(REACT_POWER_SIGNED, LINE_TOTAL);	// power already comes in watts
			break;

		case CUM_ACTIVE_ENERGY:	// KWH
//			result = myenergydata.kwh;
			result = total_energy_lastSaved;
			break;

		case CUM_REACTIVE_LAG_ENERGY:	// Kvarh_lag
//			result = myenergydata.kvarh_lag;
			result = reactive_energy_lag_lastSaved;
			break;

		case CUM_REACTIVE_LEAD_ENERGY:	// Kvarh_lead
//			result = myenergydata.kvarh_lead;
			result = reactive_energy_lead_lastSaved;
			break;

		case CUM_APPARENT_ENERGY:	// Kvah
//			result = myenergydata.kvah;
			result = apparent_energy_lastSaved;
			break;

		case CUM_POWER_FAILURE_COUNT:	//
			result = (Unsigned32)Common_Data.noOfPowerFailures;
			break;

		case CUM_POWER_FAILURE_DURN:	//
			temp = computePowerOffDurn(rtc);	// use the rtc for current value and returns power fail duration for the month
			temp += Common_Data.cumPowerFailDuration;	// now it is cumulative
			result = temp;

			if(result<0)
				result = 0;
			break;
			
		case CUM_TAMPER_CNT:	//
			result = (Unsigned32)mytmprdata.tamperCount;
			break;
			
		case CUM_BILLING_CNT:	//
			result = (Unsigned32)Common_Data.bpcount;
			break;

		case CUM_PROGAM_CNT:	//
			result = (Unsigned32)Common_Data.pgm_count;
			break;

		case BILLING_DATE:	//
			result = (Unsigned32)Common_Data.billing_date;
			break;
			
		case FREQUENCY:	// frequency
//			result = EM_GetLineFrequency(LINE_TOTAL)*10.0;
			result = getInstantaneousParams(IFREQS, LINE_TOTAL)*10;// this will return 0 in battery mode
			break;
			
		case EM_ACTIVE_MD:			
// now being handled in R_OBIS_DistributeXRegisterClass 		
			result = mymddata.mdkw;
			break;
			
		case EM_APPARENT_MD:
// now being handled in R_OBIS_DistributeXRegisterClass 		
			result = mymddata.mdkva;
			break;
			
		case EM_REACTIVE_MD:	// current mdkvar - this value is not there
			result = 0;
			break;
			
		case I_N:	// neutral current
			result = (getInstantaneousParams(ICURRENTS,LINE_NEUTRAL)*100.0);
			break;

		case DEMAND_INTEGRATION_PERIOD:
			// Exchange from minutes to seconds
//			result = (Common_Data.mdinterval*60);
			result = (Common_Data.new_mdinterval*60);
			break;
			
		case PROFILE_CAPTURE_PERIOD:
			// Exchange from minutes to seconds
			result = (Common_Data.survey_interval*60); //R ENESAS, TO BE MODIFY
			g_Class07_Blockload_CapturePeriod = result;
			break;
			
		case AVL_BILLING_PERIOD:
			result = g_Class07_Billing_EntriesInUse-1;
			break;

		case EM_BLOCK_ENERGY_KWH:
			if(total_energy_lastSaved>ctldata.last_saved_kwh)
		  	result=(total_energy_lastSaved-ctldata.last_saved_kwh);
				
//			result = (int32_t)utemp16;
			break;

		case EM_BLOCK_ENERGY_KVARH_LAG:
		#ifdef SURVEY_USES_REACTIVE_DATA	
			if(reactive_energy_lag_lastSaved>ctldata.last_saved_kvarh_lag)
			  result=(reactive_energy_lag_lastSaved-ctldata.last_saved_kvarh_lag);
			
//			result = (int32_t)utemp16;
		#else
			result = 0;
		#endif	
			break;
			
		case EM_BLOCK_ENERGY_KVARH_LEAD:
		#ifdef SURVEY_USES_REACTIVE_DATA
			if(reactive_energy_lead_lastSaved>ctldata.last_saved_kvarh_lead)
			  result=(reactive_energy_lead_lastSaved-ctldata.last_saved_kvarh_lead);
			
//			result = (int32_t)utemp16;
		#else
			result = 0;
		#endif	
			break;
			
		case EM_BLOCK_ENERGY_KVAH:
			if(apparent_energy_lastSaved>ctldata.last_saved_kvah)
			  result=(apparent_energy_lastSaved-ctldata.last_saved_kvah);
				
//			result = (int32_t)utemp16;
			break;
			
		case POWER_ON_DURATION:
			result = ctldata.powerOnminutes_eb;	// since 12/02/2024
//			result = (int32_t)utemp16;
			break;

#ifdef BIDIR_METER			
		case I_R_AVG:
			result = average_Ir;
			break;
		
		case I_Y_AVG:
			result = average_Iy;
			break;
		
		case I_B_AVG:
			result = average_Ib;
			break;
		
		case V_R_AVG:
			result = average_Vr;
			break;
		
		case V_Y_AVG:
			result = average_Vy;
			break;
		
		case V_B_AVG:
			result = average_Vb;
			break;
			
		case CUM_ACTIVE_ENERGY_EXPORT:	// KWH
//			result = myenergydata.kwh_export;
			result = total_energy_lastSaved_export;
			break;

		case CUM_REACTIVE_LAG_ENERGY_EXPORT:
//			result = myenergydata.kvarh_lag_export;
			result = reactive_energy_lag_lastSaved_export;
			break;

		case CUM_REACTIVE_LEAD_ENERGY_EXPORT:
//			result = myenergydata.kvarh_lead_export;
			result = reactive_energy_lead_lastSaved_export;
			break;

		case CUM_APPARENT_ENERGY_EXPORT:
//			result = myenergydata.kvah_export;
			result = apparent_energy_lastSaved_export;
			break;

		case EM_ACTIVE_MD_EXPORT:		
// now being handled in R_OBIS_DistributeXRegisterClass 		
//			ConvertRTCDate(&g_Class04_mdCaptureTime, &mymddata.mdkw_datetime_export);
			result = mymddata.mdkw_export;
			break;
			
		case EM_APPARENT_MD_EXPORT:
// now being handled in R_OBIS_DistributeXRegisterClass 		
//			ConvertRTCDate(&g_Class04_mdCaptureTime, &mymddata.mdkva_datetime_export);
			result = mymddata.mdkva_export;
			break;
#endif

		default:
			break;
	}

	return result;
}

Integer8 R_USER_SetEMData(Unsigned16 em_data, Unsigned8 *p_em_value)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
	Unsigned16 tmp_long=0;
	date_t * tt;
//	EM_CONFIG 		config;
//	Integer8 result = -1;
//	Unsigned16 data;

	tmp_long= (Unsigned16 *)p_em_value;

	switch(em_data)
	{
		case DEMAND_INTEGRATION_PERIOD: //md interval to be in minutes only
			if((tmp_long==900)||(tmp_long==1800)||(tmp_long==3600))
			{
				Common_Data.new_mdinterval = (tmp_long/60);	// this value will be transferred at next 00 or 30 minutes
				Common_Data.pgm_count++;
				e2write_flags|=E2_W_IMPDATA;
				record_transaction_event(MD_INTERVAL_TRANSACTION_MASK);
				response_result = DATA_ACCESS_RESULT_SUCCESS;
			}
			break;
			
		case PROFILE_CAPTURE_PERIOD: //survey_interval in minutes only
			if((tmp_long==900)||(tmp_long==1800)||(tmp_long==3600))
			{
				Common_Data.new_survey_interval = (tmp_long/60);	// this value will be transferred at next 00 or 30 minutes
				g_Class07_Blockload_CapturePeriod=tmp_long;
				Common_Data.pgm_count++;
				e2write_flags|=E2_W_IMPDATA;
				record_transaction_event(SURVEY_INTERVAL_TRANSACTION_MASK);
				response_result = DATA_ACCESS_RESULT_SUCCESS;
				genflags|=SURVEY_INTERVAL_CHANGED;	// set this flag to indicate that survey interval has been changed
			}
			break;
			
		case BILLING_DATE: //USER CODE
			tt=(date_t *)p_em_value;
			if ((tt->day_of_month>=1)&&(tt->day_of_month<=31))
			{
				Common_Data.billing_date=tt->day_of_month;
				Common_Data.pgm_count++;
				e2write_flags|=E2_W_IMPDATA;
				record_transaction_event(SINGLE_ACTION_TRANSACTION_MASK);
				response_result = DATA_ACCESS_RESULT_SUCCESS;
			}
			break;
			
//		case EM_CUCUMLARIVE_TAMPER_CNT:
//			{
//				//result = (Float32)g_storage_header.num_of_tamper_log; //RENESAS_USER TODO
////				g_storage_header.num_of_tamper_log = *((Unsigned16*)p_em_value);//RENESAS_USER TODO
//			}
//			break;
//		case EM_CUCUMLARIVE_BILLING_CNT:
//			{
////				g_Class07_Billing_EntriesInUse = *((Unsigned16*)p_em_value);//RENESAS_USER TODO
////				g_storage_header.num_of_energy_log = (Unsigned16)g_Class07_Billing_EntriesInUse; //RENESAS_USER TODO
//			}
//			break;
//		case EM_CUCUMLARIVE_PROGAM_CNT:
//			{
//			
////				data = *((Unsigned16*)p_em_value);//RENESAS_USER TODO
////				g_storage_header.program_cnt = data;//RENESAS_USER TODO
//			}
//			break;
		default:
			break;
	}

	return response_result;
}

#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
// Class09 - Script table  
Unsigned8 R_USER_Class09_ScriptExecute(Unsigned16 ID)
{
	switch(ID)
	{
		// Option: User code START  
		case SCRIPT_ACTION_TZ1:
		case SCRIPT_ACTION_TZ2:
		case SCRIPT_ACTION_TZ3:
		case SCRIPT_ACTION_TZ4:
			break;
		// Option: User code END  
		default:
			// Do nothing 
			break;
	}
	return 0;
}
#endif // #if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)  

#if (defined(USED_CLASS_15) && USED_CLASS_15 == 1)
// Class15 -  Association LN  
Unsigned8 R_USER_Class15_Secret_Backup(
	Unsigned8             *p_secret,
	Unsigned16            length,
	Unsigned8             mechanism_id
)
{
	// Check params  
	if (p_secret == NULL || length == 0 || mechanism_id == MECHANISM_ID0_LOWEST_SECURITY)
	{
		return 1;
	}
	if (mechanism_id == MECHANISM_ID1_LOW_SECURITY)
	{
		//CLK> Pls do some check to ensure the LLS length is within EEPROM storage expected space
		if (length>8)
		{
			return 1;
		}
		
//		memset(secret1, 0, 9);	// this should make the entire buffer 0s
//		memcpy(secret1, p_secret, length);
		
		write_alternately(SECRET1_ADDRESS,ALTERNATE_SECRET1_ADDRESS,(uint8_t*)&secret1,9);	// the null character is being written into memory
// upon returning from above function secret1[length] has been corrupted
// it should be made NULL
		secret1[length]=0x00;	// secret1 is now again a string
		
		Set_PP(0,15);
		return 0; //Return 0 for OK 
	}
	else // HLS
	{
		//CLK> Pls do some check to ensure the HLS length is within EEPROM storage expected space
		if (length>16)
		{
			return 1;
		}

//		memset(secret1, 0, 17);	// this should make the entire buffer 0s
//		memcpy(secret2, p_secret, length);
		
//		memset(secret2, 0, 17); // is this necessary or writing 17 bytes is sufficient
		write_alternately(SECRET2_ADDRESS,ALTERNATE_SECRET2_ADDRESS,(uint8_t*)&secret2,17);	// the null character is being written into memory
// upon returning from above function secret1[length] has been corrupted
// it should be made NULL
		secret2[length]=0x00;	// secret2 is now again a string

		Set_PP(0,16);
		return 0; //Return 0 for OK 
	}
	
//	return 0;
}

Unsigned8 R_USER_Class15_Secret_Restore(
	Unsigned8             *p_secret,
	Unsigned16            length,
	Unsigned8             mechanism_id
)
{
	// Check params  
	if (p_secret == NULL || length == 0 || mechanism_id == MECHANISM_ID0_LOWEST_SECURITY)
	{
		return 1;
	}
	if (mechanism_id == MECHANISM_ID1_LOW_SECURITY)
	{
		//CLK> Pls do some check to ensure the LLS length is within EEPROM storage expected space
//		if (DLMS_LLS_SECRET_SIZE >= length)
//		{
//			return 1;
//		}
		p_secret =(uint8_t *) secret1;  //CLK> Should read from EEPOM
		return 0; //Return 0 for OK 
//		// Check EEPROM setting  
//		if (DLMS_LLS_SECRET_ADDR == DLMS_ADDR_NOTSPECIFIC || DLMS_LLS_SECRET_SIZE != length)
//		{
//			return 1;
//		}
//		if (E2PR_READ(
//			DLMS_LLS_SECRET_ADDR,
//			(uint8_t *)p_secret,
//			DLMS_LLS_SECRET_SIZE
//			) != E2PR_OK)
//		{
//			return 1;
//		}
	}
	else // HLS
	{
		//CLK> Pls do some check to ensure the HLS length is within EEPROM storage expected space
//		if (DLMS_HLS_SECRET_SIZE != length)
//		{
//			return 1;
//		}

		p_secret =(uint8_t *) secret2; //CLK> Should read from EEPOM
		return 0; //Return 0 for OK 
//		// Check EEPROM setting  
//		if (DLMS_HLS_SECRET_ADDR == DLMS_ADDR_NOTSPECIFIC || DLMS_HLS_SECRET_SIZE != length)
//		{
//			return 1;
//		}
//		if (E2PR_READ(
//			DLMS_HLS_SECRET_ADDR,
//			(uint8_t *)p_secret,
//			DLMS_HLS_SECRET_SIZE
//			) != E2PR_OK)
//		{
//			return 1;
//		}
	}
	
//	return 0;
}
#endif // #if (defined(USED_CLASS_15) && USED_CLASS_15 == 1)  


#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
// Class 20 - Activity Calendar  


Unsigned8 R_USER_Class20_GetTariffSeasonTable(
	season_profile_t    *buf,
	Unsigned16          index,
	Unsigned16          is_active
)
{
	// Option: User code START  
	// Follow EM SDK config to update for DLMS info
//	* Now just keep default setting

	// Option: User code END  
	return 0;
}

Unsigned8 R_USER_Class20_GetTariffWeekTable(
	week_profile_t      *buf,
	Unsigned16          index,
	Unsigned16          is_active
)
{
	// Option: User code START  
	// Follow EM SDK config to update for DLMS info
//	* Now just keep default setting

	// Option: User code END  
	return 0;
}

Unsigned8 R_USER_Class20_GetTariffDayTable(
	day_profile_t       *buf,
	Unsigned16          index,
	Unsigned16          is_active
)
{

// fill the buf with entries of 	
//	buf->day_schedule.TZ_start_time0[0]=(time_t)variable which contains the zone settings in e2rom 
	
	
	// Option: User code START  
	// Follow EM SDK config to update for DLMS info
//	* Now just keep default setting

	// Option: User code END  
	return 0;
}

Unsigned8 R_USER_Class20_Activate_PassiveCalendar(class20_child_record_t *p_child_record_20)
{
	// Option: User code START  
	// Callback function  to User who can get this information to update tariff.
	
	// Option: User code END  
	return 0;
}

	#if (defined(USED_CLASS_20_SEASON_PROFILE) && USED_CLASS_20_SEASON_PROFILE == 1)
void R_USER_Class20_ChangSeason(date_time_t current_time)
{
	// Option: User code START  
	// Option: User code END  
	return;
}
	#endif // #if (defined(USED_CLASS_20_SEASON_PROFILE) && USED_CLASS_20_SEASON_PROFILE == 1)
	
	#if (defined(USED_CLASS_20_WEEK_PROFILE) && USED_CLASS_20_WEEK_PROFILE == 1)

void R_USER_Class20_ChangeWeekDay(date_time_t current_time)
{
	// Option: User code START  
	// Option: User code END  
	return;
}
	#endif // #if (defined(USED_CLASS_20_WEEK_PROFILE) && USED_CLASS_20_WEEK_PROFILE == 1)  

Unsigned8 R_USER_Class20_Active_Passive_Time(void)
{	
	//R_OBIS_Class20_Activate_PassiveCalendar();
	Set_PP(0,5);
	return 0;
	
}
#endif // #if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)  
void Set_PP(unsigned int i, unsigned char j)
{
}