/******************************************************************************
* File Name    : r_dlms_data_meter.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   :
* H/W Platform : Unified Energy Meter Platform
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 18.06.2013
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Drivers */

#include "r_cg_macrodriver.h"			/* Macro Driver Definitions */
#include "r_cg_rtc.h"					/* RTC Driver */
#include "r_cg_wdt.h"					/* CG WDT Driver */

/* DLMS */
#include "r_dlms_typedef.h"				/* DLMS Type Definitions */
#include "r_dlms_data.h"				/* DLMS Data Definitions */
#include "r_dlms_obis.h"				/* DLMS OBIS Definitions */
#include "r_dlms_obis_ic.h"				/* DLMS OBIS IC Definitions */
#include "r_dlms_data_ic.h"
#include "DLMS_User.h"					/* DLMS User Definitions */
#include "r_dlms_user_interface.h"		/* DLMS User Interface Definitions */
#include "emeter3_structs.h"
#include "utilities.h"
#include "r_lcd_display.h"
#include "tod.h"
/* Standard Lib */
#include <string.h>						/* String Standard Lib */


///* EM Core Component */
//#include "em_operation.h"	/* EM Core Operation APIs */
//#include "em_calibration.h"	/* EM Calibration APIs */
//#include "em_measurement.h"	/* EM Measurement APIs */

//#include "emeter_structs.h" - this is included in r_dlms-data.h
/******************************************************************************
Macro definitions
******************************************************************************/
//USER VARIABLE
extern unsigned int vrms_reg3, irms_reg3, irms1_reg3, irms2_reg3, kw_reg3, kva_reg3, temp_reg3,freq_reg3;
extern int pf_reg4;
extern unsigned long int all_tamper_cnt;//meter_no,  //USER
extern unsigned long int load_val[2], last_hr_load_val[2];
extern unsigned long int power_on_time,cum_on_time;
extern unsigned int avg_pf;
//extern unsigned char string[10]; //USER
extern unsigned int md_intgr_val, md_reset_cnt,survey_intgr_val; //USER
extern unsigned long int kwmd_date,kvamd_date;
extern date_time_t tmp_time_string;
extern unsigned int kwmd_val,kvamd_val;
extern unsigned int kwmd_time,kvamd_time;
extern unsigned long int demand_volt;
extern unsigned char billing_day;
//unsigned char md_reset_b;	// this variable has no use in our scheme of things


extern time_t TZ_start_time0[];	// 20/Jul/2019
extern time_t TZ_start_time1[]; // 20/Jul/2019

extern day_profile_action_t	day_action_active0[];
extern day_profile_action_t	day_action_passive0[];


void generate_bill(uint8_t dat, uint8_t mth, uint8_t yr, uint8_t hrs, uint8_t mnts, uint8_t secs, uint8_t bill_type);
void computeNextBillDate(void);


/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern Unsigned32		g_Class01_Blockload_interval;// = CLASS07_BLOCKLOAD_INTERVAL;
/*
 * Imported from r_dlms_data_ic.c
 * For data access
 *
 * All of following variables are linked into each specified-child-table.
 * Refer to each child-table in r_dlms_data_ic.c
 */
/* Class ID = 01 - Data Class */
 void R_USER_GetRTCTime(date_time_t *p_date_time);

/* Class ID = 03 - Register Class */
#if (defined(USED_CLASS_03) && USED_CLASS_03 == 1)
/* Block load profile parameter */
/* Billing profile parameter */
extern date_time_t		g_Class03_BillingDate;
#endif

/* Class ID = 04 - Extended Register Class */
#if (defined(USED_CLASS_04) && USED_CLASS_04 == 1)
/* Block load profile parameter */
extern date_time_t			g_Class04_mdCaptureTime;	// one single common variable - needs to be populated everytime before use
#endif

/* Class ID = 07 - Profile generic class */
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
extern Unsigned8					g_Class07_Obj0_Capture_Enable;
extern Unsigned32					g_Class07_Blockload_CapturePeriod;
extern Unsigned32					g_Class07_Blockload_EntriesInUse;
extern Unsigned16					g_Class07_Blockload_CurrentEntry;
extern class07_blockload_entry_t			g_Class07_BlockLoadBuffer;

extern const Unsigned32					g_Class07_Dailyload_CapturePeriod;
extern Unsigned32					g_Class07_Dailyload_EntriesInUse;
extern Unsigned16					g_Class07_Dailyload_CurrentEntry;
extern class07_dailyload_entry_t			g_Class07_DailyLoadBuffer;

extern Unsigned32					g_Class07_Billing_EntriesInUse;
extern Unsigned16					g_Class07_Billing_CurrentEntry;
extern class07_billing_entry_t				g_Class07_BillingBuffer;

extern class07_event_entry_t				g_Class07_EventBuffer;//[CLASS07_EVENT_MAX_PROFILE];//[CLASS07_EVENT_MAX_ENTRY];
extern Unsigned32					g_Class07_Event_EntriesInUse[];
extern Unsigned16					g_Class07_Event_CurrentEntry[];

#endif

/* Class ID = 15, Association LN Child-table */
#if (defined(USED_CLASS_15) && USED_CLASS_15 == 1)
extern const class15_child_record_t g_ChildTableClass15[];  /* Association LN List      */
extern const Unsigned16 g_ChildTableLengthClass15;  		/* Association LN Length    */
#endif

/* Class ID = 64 - Security setup class */
#if (defined(USED_CLASS_64) && USED_CLASS_64 == 1)
extern const class64_child_record_t g_ChildTableClass64[];  /* Security setup reference list   */
extern const Unsigned16 g_ChildTableLengthClass64;  		/* Security setup reference length */
#endif
/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* RCT current time */
static date_time_t current_RTCtime;

/* Stick counter in second */
static Unsigned32 rtc_stick = 0;

/******************************************************************************
* Function Name : R_OBIS_IsValidDateTime
* Interface     : Unsigned8 R_OBIS_IsValidDateTime(
*               :     Unsigned8 *p_date_time,
*               : );
* Description   : Check valid date time or not
* Arguments     : Unsigned8 *p_date_time
* Function Calls: None
* Return Value  : Unsigned8, 0: invalid, 1 : valid
******************************************************************************/
Unsigned8 R_OBIS_IsValidDateTime(Unsigned8 *p_date_time)
{
	Unsigned8 temp;

	if(*p_date_time++ != ATTR_TYPE_OCTET_STRING)
	{
		return 0;
	}
	if(*p_date_time++ != sizeof(date_time_t))
	{
		return 0;
	}
	p_date_time++; // Ignore year highbyte
	p_date_time++; // Ignore year lowbyte

	temp = *p_date_time++;
	if(temp > 12 && temp < DATE_MONTH_DAYLIGHT_SAVINGS_END) // Month
	{				
		return 0;
	}
	temp = *p_date_time++;
	if(temp > 31 && temp < DATE_DAY_2ND_LAST_OF_MONTH) // day of month
	{
		return 0;
	}
	temp = *p_date_time++;
	if(temp > 7 && temp < NOT_SPECIFIED) // day of week
	{
		return 0;
	}
	temp = *p_date_time++;
	if(temp > 23 && temp < NOT_SPECIFIED) // hour
	{
		return 0;
	}
	temp = *p_date_time++;
	if(temp > 59 && temp < NOT_SPECIFIED) // minute
	{
		return 0;
	}
	temp = *p_date_time++;
	if(temp > 59 && temp < NOT_SPECIFIED) // second
	{
		return 0;
	}
	temp = *p_date_time++;
	if(temp > 99 && temp < NOT_SPECIFIED) // hundred
	{
		return 0;
	}
	return 1;
}

/******************************************************************************
* Function Name : R_OBIS_IsValidDate
* Interface     : Unsigned8 R_OBIS_IsValidDate(
*               :     Unsigned8 *p_date,
*               : );
* Description   : Check valid date time or not
* Arguments     : Unsigned8 *p_date
* Function Calls: None
* Return Value  : Unsigned8, 0: invalid, 1 : valid
******************************************************************************/
Unsigned8 R_OBIS_IsValidDate(Unsigned8 *p_date)
{
	Unsigned8 temp;

	if(*p_date++ != ATTR_TYPE_OCTET_STRING)
	{
		return 0;
	}
	if(*p_date++ != 5)
	{
		return 0;
	}
	p_date++; // Ignore year highbyte
	p_date++; // Ignore year lowbyte

	temp = *p_date++;
	if((temp > 12 && temp < DATE_MONTH_DAYLIGHT_SAVINGS_END)||(temp == 0)) // Month
	{
		return 0;
	}
	temp = *p_date++;
	if((temp > 31 && temp < DATE_DAY_2ND_LAST_OF_MONTH)|(temp == 0)) // day of month
	{
		return 0;
	}
	temp = *p_date++;
	if((temp > 7 && temp < NOT_SPECIFIED)||(temp == 0)) // day of week
	{
		return 0;
	}

	return 1;
}

/******************************************************************************
* Function Name : R_OBIS_IsValidTime
* Interface     : Unsigned8 R_OBIS_IsValidTime(
*               :     Unsigned8 *p_time,
*               : );
* Description   : Check valid date time or not
* Arguments     : Unsigned8 *p_time
* Function Calls: None
* Return Value  : Unsigned8, 0: invalid, 1 : valid
******************************************************************************/
Unsigned8 R_OBIS_IsValidTime(Unsigned8 *p_time)
{
	Unsigned8 temp;

	if(*p_time++ != ATTR_TYPE_OCTET_STRING)
	{
		return 0;
	}
	if(*p_time++ != 4)
	{
		return 0;
	}

	temp = *p_time++;
	if(temp > 23 && temp < NOT_SPECIFIED) // hour
	{
		return 0;
	}
	temp = *p_time++;
	if(temp > 59 && temp < NOT_SPECIFIED) // minute
	{
		return 0;
	}
	temp = *p_time++;
	if(temp > 59 && temp < NOT_SPECIFIED) // second
	{
		return 0;
	}
	temp = *p_time++;
	if(temp > 99 && temp < NOT_SPECIFIED) // hundred
	{
		return 0;
	}
	return 1;
}

Integer8 R_OBIS_CompareTime(Unsigned8 *p_src_time,	Unsigned8 *p_des_time)
{// both times are in time_t*
	Unsigned8	i;
	Integer8	result = 1;

	/*
	 * Checking following: - only three entries
	 * hour, minute, second
	 */
	for (i = 0; i < 3; i++)
	{
		if (p_src_time[i] != NOT_SPECIFIED &&
			p_des_time[i] != NOT_SPECIFIED)
		{
			if (p_src_time[i] < p_des_time[i])
			{
				return -1;	// source time has gone past
			}
			else if (p_src_time[i] > p_des_time[i])
			{
				return 1;	// source time is in future - still to come
			}
			result = 0;	// source time is now
		}
	}
	return result;
}


Integer8 R_OBIS_CompareDate(Unsigned8 *p_src_date,	Unsigned8 *p_des_date)
{// Both entries are date_t
	Unsigned8	i;
	Integer8	result = 1;

	/*
	 * Checking following: - only four entries
	 * year_high, year_low, month, day_of_month
	 */
	for (i = 0; i < 4; i++)
	{
		if (p_src_date[i] != NOT_SPECIFIED &&
			p_des_date[i] != NOT_SPECIFIED)
		{
			if (p_src_date[i] < p_des_date[i])
			{
				return -1;	// source time has gone past
			}
			else if (p_src_date[i] > p_des_date[i])
			{
				return 1;	// source time is in future - still to come
			}
			result = 0;	// source time is now
		}
	}
	return result;
}


/******************************************************************************
* Function Name : R_OBIS_CompareDateTime
* Interface     : Integer8 R_OBIS_CompareDateTime(
*               :     Unsigned8 *p_src_date_time,
*               :     Unsigned8 *p_des_date_time
*               : );
* Description   : Compare date time
* Arguments     : Unsigned8 * p_src_date_time: Src date
*               : Unsigned8 * p_des_date_time: Des date
* Function Calls: None
* Return Value  : Integer8, -1: src < des, 1: src > des, 0: src = des
******************************************************************************/
Integer8 R_OBIS_CompareDateTime(
	Unsigned8 *p_src_date_time,
	Unsigned8 *p_des_date_time
)
{
	Unsigned8	i;
	Integer8	result = 1;

	/*
	 * Checking following:
	 * year_high, year_low, month, day_of_month
	 * hour, minute, second, hundredths of second
	 */
	for (i = 0; i < 9; i++)
	{
		/* skip day_of_week */
		if (i == 4)
		{
			continue;
		}

		if (p_src_date_time[i] != NOT_SPECIFIED &&
			p_des_date_time[i] != NOT_SPECIFIED)
		{
			if (p_src_date_time[i] < p_des_date_time[i])
			{
				return -1;	// source time has gone past
			}
			else if (p_src_date_time[i] > p_des_date_time[i])
			{
				return 1;	// source time is in future - still to come
			}
			result = 0;	// source time is now
		}
	}

	/* all equal */
	return result;
}



#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
Unsigned16 R_OBIS_Class07_FilterOneScalerEntry (buffer_t *buf, Unsigned16 index, Unsigned16 scaler_nr, Unsigned8 *p_out_buf, Unsigned16 *p_out_len)
{
	Unsigned16 org_len, i;
	Unsigned16 length;
	scaler_unit_t *p_scaler_unit = NULL;

	org_len = *p_out_len;
	p_scaler_unit = (scaler_unit_t *)buf->p_buffer;
	/* Encode each element of entry */
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;

	for (i = 0; i < scaler_nr; i++){
		if (buf->from_value <= (i+1) && (i+1) <= buf->to_value){
			length = R_OBIS_EncodeScalerUnit(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,&p_scaler_unit[i]);

			*p_out_len += length;
			p_out_buf  += length;
		}
	}

	return (*p_out_len - org_len);
}

Unsigned16 R_OBIS_Class07_FilterOneInstEntry (buffer_t *buf, Unsigned16 index, Unsigned8 *p_out_buf, Unsigned16 *p_out_len){
	Unsigned16 org_len;
	Unsigned16 length;
	Integer32 temp;
	Unsigned32 temp_f32;
	Unsigned16 temp_u16;

	org_len = *p_out_len;
	/* Encode each element of entry */
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;
	{
		/* 1. Clock : octet-string(12) */
		if (buf->from_value <= 1 && 1 <= buf->to_value){
			R_USER_GetRTCTime(&current_RTCtime);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(current_RTCtime),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 2. Current, IR: Integer32(4) */
		if (buf->from_value <= 2 && 2 <= buf->to_value){
			temp = R_USER_GetEMData(I_R);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		/* 3. Current, IY:: Integer32(4) */
		if (buf->from_value <= 3 && 3 <= buf->to_value){
			temp = R_USER_GetEMData(I_Y);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 4. Current, IB: Integer32(4) */
		if (buf->from_value <= 4 && 4 <= buf->to_value){
			temp = R_USER_GetEMData(I_B);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		/* 5. Voltage VR: Integer32(4) */
		if (buf->from_value <= 5 && 5 <= buf->to_value){
			temp = R_USER_GetEMData(V_R);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		/* 6. Voltage VY: Integer32(4) */
		if (buf->from_value <= 6 && 6 <= buf->to_value){
			temp = R_USER_GetEMData(V_Y);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 7. Voltage VB: Integer32(4) */
		if (buf->from_value <= 7 && 7 <= buf->to_value){
			temp = R_USER_GetEMData(V_B);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 8. Power factor: Integer32(4) */
		if (buf->from_value <= 8 && 8 <= buf->to_value){
			temp = R_USER_GetEMData(PF_R);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		/* 9. Power factor: Integer32(4) */
		if (buf->from_value <= 9 && 9 <= buf->to_value){
			temp = R_USER_GetEMData(PF_Y);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 10. Power factor: Integer32(4) */
		if (buf->from_value <= 10 && 10 <= buf->to_value){
			temp = R_USER_GetEMData(PF_B);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		/* 11. Power factor: Integer32(4) */
		if (buf->from_value <= 11 && 11 <= buf->to_value){
			temp = R_USER_GetEMData(PF_TOTAL);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		/* 12. Frequency: Integer32(4) */
		if (buf->from_value <= 12 && 12 <= buf->to_value){
			temp = R_USER_GetEMData(FREQUENCY);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 13. Apparent Power : Integer32(4) */
		if (buf->from_value <= 13 &&  13 <= buf->to_value){
			temp = R_USER_GetEMData(APPARENT_POWER);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		/* 14. Active Power: Integer32(4) */
		if (buf->from_value <= 14 && 14 <= buf->to_value){
			temp = R_USER_GetEMData(ACTIVE_POWER);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 15. Reactive Power: Integer32(4)
		if (buf->from_value <= 15 && 15 <= buf->to_value){
			temp = R_USER_GetEMData(REACTIVE_POWER);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 16. Number of power — failures: Unsigned16(2)
		if (buf->from_value <= 16 && 16 <= buf->to_value){
			temp_u16 = (Unsigned16)R_USER_GetEMData(CUM_POWER_FAILURE_COUNT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)&temp_u16,2);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 17. Cumulative power FAIL duration in minutes : Integer32(4)
		if (buf->from_value <= 17 && 17 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_POWER_FAILURE_DURN);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 18. Cumulative tamper count: Unsigned16(2)
		if (buf->from_value <= 18 && 18 <= buf->to_value){
			temp_u16 = (Unsigned16)R_USER_GetEMData(CUM_TAMPER_CNT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)&temp_u16,2);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 19. Cumulative billing count: Unsigned16(2)
		if (buf->from_value <= 19 && 19 <= buf->to_value){
			temp_u16 = (Unsigned16)R_USER_GetEMData(CUM_BILLING_CNT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)&temp_u16,2);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 20. Cumulative programming count: Unsigned16(2)
		if (buf->from_value <= 20 && 20 <= buf->to_value){
			temp_u16 = (Unsigned16)R_USER_GetEMData(CUM_PROGAM_CNT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)&temp_u16,2);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 21. Clock : octet-string(12) - this is actually the Billing Date - must be changed
		if (buf->from_value <= 21 && 21 <= buf->to_value){
//			R_USER_GetRTCTime(&current_RTCtime);
//			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(current_RTCtime),12);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(g_Class03_BillingDate),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 22. Cumulative Energy - kWh: Integer32(4)
		if (buf->from_value <= 22 && 22 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_ACTIVE_ENERGY);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 23. Cumulative Energy - kWh: Integer32(4)
		if (buf->from_value <= 23 && 23 <= buf->to_value)	{
			temp = R_USER_GetEMData(CUM_REACTIVE_LAG_ENERGY);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 24. Cumulative Energy - kVAh: Integer32(4)
		if (buf->from_value <= 24 && 24 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_REACTIVE_LEAD_ENERGY);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 25. Cumulative Energy - kVAh: Integer32(4)
		if (buf->from_value <= 25 && 25 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_APPARENT_ENERGY);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 26. Maximum Demand - kW: Unsigned32(4)
		// float value maybe replaced with uint16 and scaler
		if (buf->from_value <= 26 && 26 <= buf->to_value){
			temp_f32=(Unsigned32)R_USER_GetEMData(EM_ACTIVE_MD);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG_UNSIGNED,(Unsigned8 *)&temp_f32,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 27. Maximum Demand Date/Time - kWDT: DateTime
		// The g_Class04_ActiveMDCaptureTime needs to be done away with and data to be picked up from mymddata
		if (buf->from_value <= 27 && 27 <= buf->to_value){
			ConvertRTCDate(&g_Class04_mdCaptureTime, &mymddata.mdkw_datetime);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(g_Class04_mdCaptureTime),12);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 28. Maximum Demand - kAV: Unsigned32(4)
		// float value maybe replaced with uint16 and scaler
		if (buf->from_value <= 28 && 28 <= buf->to_value){
			temp_f32= (Unsigned32)R_USER_GetEMData(EM_APPARENT_MD);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG_UNSIGNED,(Unsigned8 *)&temp_f32,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 29. Maximum Demand Date/Time - kVA DT: DateTime
		// The g_Class04_ApparentMDCaptureTime needs to be done away with and data to be picked up from mymddata
		if (buf->from_value <= 29 && 29 <= buf->to_value){
			ConvertRTCDate(&g_Class04_mdCaptureTime, &mymddata.mdkva_datetime);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(g_Class04_mdCaptureTime),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
#ifdef BIDIR_METER	
		// 30. Cumulative Energy - kWh export: Integer32(4)
		if (buf->from_value <= 30 && 30 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_ACTIVE_ENERGY_EXPORT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
#endif		
/*		
		// 31. Cumulative Energy Reactive Lag export: Integer32(4)
		if (buf->from_value <= 31 && 31 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_REACTIVE_LAG_ENERGY_EXPORT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 32. Cumulative Energy Reactive Lead export: Integer32(4)
		if (buf->from_value <= 32 && 32 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_REACTIVE_LEAD_ENERGY_EXPORT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 33. Cumulative Energy Reactive Lead export: Integer32(4)
		if (buf->from_value <= 33 && 33 <= buf->to_value){
			temp = R_USER_GetEMData(CUM_APPARENT_ENERGY_EXPORT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&temp,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 34. Maximum Demand - kW export: Unsigned32(4)
		if (buf->from_value <= 34 && 34 <= buf->to_value){
			temp_f32=(Unsigned32)R_USER_GetEMData(EM_ACTIVE_MD_EXPORT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG_UNSIGNED,(Unsigned8 *)&temp_f32,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 35. Maximum Demand Date/Time - kWDT: DateTime
		if (buf->from_value <= 35 && 35 <= buf->to_value){
			ConvertRTCDate(&g_Class04_mdCaptureTime, &mymddata.mdkw_datetime_export);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(g_Class04_mdCaptureTime),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 36. Maximum Demand - kVA export: Unsigned32(4)
		if (buf->from_value <= 36 && 36 <= buf->to_value){
			temp_f32= (Unsigned32)R_USER_GetEMData(EM_APPARENT_MD_EXPORT);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG_UNSIGNED,(Unsigned8 *)&temp_f32,4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 37. Maximum Demand Date/Time - kVA DT export: DateTime
		if (buf->from_value <= 29 && 29 <= buf->to_value){
			ConvertRTCDate(&g_Class04_mdCaptureTime, &mymddata.mdkva_datetime_export);
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(g_Class04_mdCaptureTime),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
*/		
	}

	return (*p_out_len - org_len);
}

void R_OBIS_Class07_BlockLoadUpdate_Pause(void){// Mark to start async service, pause the capturing
	g_Class07_Obj0_Capture_Enable = 0;
}

void R_OBIS_Class07_BlockLoadUpdate_Start(void){// Reset counter of async service for Block Load Profile (object 0)
	g_Class07_Obj0_Capture_Enable = 1;
}

// This function has been modified - The variables are now uint16_t instead of int32_t
// The get_lp_data will read from e2rom and will transfer the data suitably into the g_Class07_BlockLoadBuffer
// the purpose of this function is to fill data into structure pointed to be p_buf_entry
// the data corresponding to the index value must be read from the e2rom and stored in the class07_blockload_entry_t structure 
Unsigned16 R_OBIS_Class07_FilterOneBlockLoadEntry(buffer_t *buf, Unsigned16 index, Unsigned8 *p_out_buf, Unsigned16 *p_out_len){
	Integer32 temp;
	Unsigned16 org_len;
	Unsigned16 length;
	uint16_t addr;
	int16_t counter;
	
	class07_blockload_entry_t *p_buf_entry;
//	p_out_buf points to g_ServiceBuffer
	org_len = *p_out_len;	// Point to entry

	get_lp_data(index);	// This function will read the data from e2rom and fill the g_Class07_BlockLoadBuffer
	p_buf_entry = &g_Class07_BlockLoadBuffer;
	
	// Encode each element of entry 
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;
	{
		counter =1;
		// 1. Clock : octet-string(12)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(uint8_t*)&(p_buf_entry->clock_value),12);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		
#ifdef SURVEY_USES_AVERAGE_DATA
		// 2. Current IR : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->current_value_IR),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 3. Current IY  : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->current_value_IY),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 4. Current IB : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->current_value_IB),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 5. Voltage VR : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->voltage_value_VR),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 6. Voltage VY : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->voltage_value_VY),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 7. Voltage VB : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->voltage_value_VB),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
#endif

		// 8. kWh Consumption Value : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->kWh_value),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		
#ifdef SURVEY_USES_REACTIVE_DATA		
		// 9. kvarh_lag_value Consumption Value : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->kvarh_lag_value),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 10. kvarh_lead_value Consumption Value : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->kvarh_lead_value),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
#endif

		// 11. kVAh_value Consumption Value : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->kVAh_value),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		
#ifdef SURVEY_USES_EXPORT_DATA	
		// 12. kWh_value Export Consumption Value : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->kWh_value_export),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
#endif

#ifdef SURVEY_USES_AVERAGE_PF
		// 13. pf_R : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->avgPF_r),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 14. pf_Y : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->avgPF_y),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		// 15. pf_B : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->avgPF_b),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
#endif

#ifdef SURVEY_USES_FREQ_DATA
		// 15. pf_B : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->frequency),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
#endif

#ifdef SURVEY_USES_DEMAND_DATA
		// 16. apparent demand : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->apparentDemand),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}

		// 17. active demand : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(uint8_t*)&(p_buf_entry->activeDemand),4);
			*p_out_len += length;
			p_out_buf  += length;
			counter++;
		}
		
#endif
	}
	return (*p_out_len - org_len);
}


Unsigned16 R_OBIS_Class07_FilterOneDailyLoadEntry(buffer_t *buf, Unsigned16 index, Unsigned8 *p_out_buf, Unsigned16 *p_out_len){
	Unsigned16 org_len;
	Unsigned16 length;
	class07_dailyload_entry_t *p_buf_entry;

	org_len = *p_out_len;
	// Point to entry
	get_dlp_data(index); // this function reads data from e2rom and fills it into the g_Class07_DailyLoadBuffer
	p_buf_entry = &g_Class07_DailyLoadBuffer;

	// Encode each element of entry
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;
#ifdef SMALL_DAILY_LOG
	{
		// 1. Clock : octet-string(12)
		if (buf->from_value <= 1 && 1 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->clock_value),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		// 2. Cumulative Energy – kWh : Integer32(4)
		if (buf->from_value <= 2 && 2 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kWh_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		// 3. Cumulative Energy – kVAh : Integer32(4)
		if (buf->from_value <= 3 && 3 <= buf->to_value)
		{
	#ifdef BIDIR_METER
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kWh_value_export),4);
	#else
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kVAh_value),4);
	#endif
			*p_out_len += length;
			p_out_buf  += length;
		}
	}
#else
	{// In this case Clock, kwh, kvah, kvarh_lag and kvarh_lead are to be sent
		// 1. Clock : octet-string(12)
		if (buf->from_value <= 1 && 1 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->clock_value),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		// 2. Cumulative Energy – kWh : Integer32(4)
		if (buf->from_value <= 2 && 2 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kWh_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		// 3. Cumulative Energy – kVAh : Integer32(4)
		if (buf->from_value <= 3 && 3 <= buf->to_value)
		{
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kVAh_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
	#ifdef DAILY_POWER_ON_LOGGING		
		// 4. Daily Power On Minutes : Integer32(4)
		if (buf->from_value <= 4 && 4 <= buf->to_value)
		{
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->dailyPonMinutes),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
	#else
		// 4. Cumulative Energy – kVArh_lag : Integer32(4)
		if (buf->from_value <= 4 && 4 <= buf->to_value)
		{
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kVArh_lag_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		// 5. Cumulative Energy – kVArh_lead : Integer32(4)
		if (buf->from_value <= 5 && 5 <= buf->to_value)
		{
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kVArh_lead_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
	#endif		
	}
#endif	
	return (*p_out_len - org_len);
}

Unsigned16 R_OBIS_Class07_FilterOneBillingEntry (buffer_t *buf, Unsigned16 index, Unsigned8 *p_out_buf, Unsigned16 *p_out_len){
	Unsigned16 org_len, i;
	Unsigned16 length;
	class07_billing_entry_t *p_buf_entry;	
	int16_t counter;

	org_len = *p_out_len;
	// Point to entry 
	get_bill_data(index);	// FILL the values into g_Class07_BillingBuffer
	p_buf_entry = &g_Class07_BillingBuffer;

	// Encode each element of entry 
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;
	{
		counter = 1;
		// 1. bill_date : octet-string(12) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->bill_date),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 2

		// 2. power_factor : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->power_factor),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 3

		// 3. active_energy : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energy),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 4
		
		// 4~11. active_energyTZ : Integer32(4) - for all zones
		for (i = 0; i < TARIFFS; i++)	{
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energyTZ[i]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			counter++;	// 12
		}
		
		// 12. kvarh_lag : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kvarh_lag_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 13
		
		// 13. kvarh_lead : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kvarh_lead_value),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 14

		// 14. apparent_energy : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energy),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 15
		
		// 15~22. apparent_energyTZ : Integer32(4)
		for (i = 0; i < TARIFFS; i++){
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energyTZ[i]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			counter++;	// 23
		}
		
		// 23. active_MD : Unsigned32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_MD),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 24
		
		// 24. active_MD Date Time:octet-string(12) ) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->active_MD_DT),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	//25
		
		// 25~40. active_MD_TZ[i] & active_MD_DT_TZ[i] : Integer32(4)
		for (i = 0; i < 2*TARIFFS;){
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_MD_TZ[i/2]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			if (buf->from_value <= (counter+1) && (counter+1) <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->active_MD_DT_TZ[i/2]),12);
				*p_out_len += length;
				p_out_buf  += length;
			}
			i=i+2;
			counter=counter+2;	// 41
		}

		// 41. apparent_MD : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_MD),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	//42
		
		// 42. apparent_MD Date Time:octet-string(12) ) - in the BIDIR code of 10/Aug/2020 the number 42 below was incorrect - should have been 34
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->apparent_MD_DT),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 43	

		// 43~58. apparent_MD_TZ[i] & apparent_MD_DT_TZ[i] : Integer32(4)
		for (i = 0; i < 2*TARIFFS;){
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_MD_TZ[i/2]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			if (buf->from_value <= (counter+1) && (counter+1) <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->apparent_MD_DT_TZ[i/2]),12);
				*p_out_len += length;
				p_out_buf  += length;
			}
			i=i+2;
			counter=counter+2;	// 59
		}
		
		// 59. Billing power ON duration in minutes (During Billing period) : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(
				p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->total_power),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++; // 60

#ifdef BILLING_HAS_TAMPER_COUNT
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(
				p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->tamper_count),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++; // 60
#endif
		
		
// the following is a requirement of GUJARAT LT CT meter which is a UNIDIR meter
// the SPLIT_ENERGIES_REQUIRED must not be defined as indices may get upset
// will need to determine what happens for the GUJARAT LT CT BiDirectional Meter
#ifdef TOD_HAS_KVARH_LAG
		// 60~67. reactive_energy_lagTZ : Integer32(4) - for all zones
		for (i = 0; i < TARIFFS; i++)	{
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->reactive_energy_lagTZ[i]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			counter++;	// 68
		}
#endif		
		
// now for the export data		
#ifdef BIDIR_METER
		// 68. active_energy : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energy_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 69
		
		// 69~73. active_energyTZ : Integer32(4) - bidirs will have 5 tariff zones for now for all zones
		for (i = 0; i < TARIFFS; i++)	{
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energyTZ_export[i]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			counter++;	// 74
		}
		
		// 74. kvarh_lag : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kvarh_lag_value_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 75
		
		// 75. kvarh_lead : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kvarh_lead_value_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 76
		
		// 76. apparent_energy : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energy_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 77
		
		// 77~81. apparent_energyTZ : Integer32(4)
		for (i = 0; i < TARIFFS; i++){
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energyTZ_export[i]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			counter++;	// 82
		}
		
		// 82. active_MD : Unsigned32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_MD_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 83
		
		// 83. active_MD Date Time:octet-string(12) ) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->active_MD_DT_export),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 84
		
		// 84~93. active_MD_TZ[i] & active_MD_DT_TZ[i] : Integer32(4)
		for (i = 0; i < 2*TARIFFS;){
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_MD_TZ_export[i/2]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			if (buf->from_value <= (counter+1) && (counter+1) <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->active_MD_DT_TZ_export[i/2]),12);
				*p_out_len += length;
				p_out_buf  += length;
			}
			i=i+2;
			counter=counter+2;	// 94
		}

		// 94. apparent_MD : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_MD_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 95
		
		// 95. active_MD Date Time:octet-string(12) )
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->apparent_MD_DT_export),12);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 96

		// 96~105. apparent_MD_TZ[i] & apparent_MD_DT_TZ[i] : Integer32(4)
		for (i = 0; i < 2*TARIFFS;){
			if (buf->from_value <= counter && counter <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_MD_TZ_export[i/2]),4);
				*p_out_len += length;
				p_out_buf  += length;
			}
			if (buf->from_value <= (counter+1) && (counter+1) <= buf->to_value){
				length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->apparent_MD_DT_TZ_export[i/2]),12);
				*p_out_len += length;
				p_out_buf  += length;
			}
			i=i+2;
			counter=counter+2;	// 106
		}

		// 106. kwh_nonSolar_export : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kwh_nonSolar_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 107
		
		// 107. kwh_Solar_import : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kwh_Solar_import),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 108
		
		// 108. kwh_Solar_export : Integer32(4)
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->kwh_Solar_export),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 109
		
#endif	

// the following is a requirement of KERALA which is a UNIDIR meter - hence it wont affect the subsequent Bidirectional data		
#ifdef SPLIT_ENERGIES_REQUIRED
		// 109. Active Energy Lag : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(
				p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energy_lag),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 110
		
		// 110. Active Energy Lead : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(
				p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energy_lead),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	// 111
		
		// 111. Apparent Energy Lag : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(
				p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energy_lag),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	112
		
		// 112. Apparent Energy Lead : Integer32(4) 
		if (buf->from_value <= counter && counter <= buf->to_value){
			length = R_OBIS_EncodeAttribute(
				p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energy_lead),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		counter++;	//	113
#endif

	}

	return (*p_out_len - org_len);
}

Unsigned16 R_OBIS_Class07_FilterOneEventEntry( buffer_t *buf, Unsigned16 child_index, Unsigned16 index, Unsigned8 *p_out_buf, Unsigned16 *p_out_len){
	
	Unsigned16 org_len, i;
	Unsigned16 length;
	class07_event_entry_t *p_buf_event = NULL;
	class07_event_entry_t *p_buf_entry = NULL;

	org_len = *p_out_len;
	/* Point to entry */
	//p_buf_event = (class07_event_entry_t *)buf->p_buffer;
	//p_buf_entry = &p_buf_event[index];
	
// there are 6 types of events - the g_ChildTableClass07 has 6 entries ( child table index 9 to 14)
// these point to the six different types of events 
// the IS specification speaks of 7 entries - so we probably have one missing - it appears
// to be for cover tamper
// within each type of event there may be many records of events of that type -
// the second entry index probably refers to that
	get_tamper_data(child_index, index) ;
	p_buf_entry = &g_Class07_EventBuffer;
	
	
	//if (p_buf_event == NULL || p_buf_entry == NULL)
	//{
	//	return 0;
	//}

	// Encode each element of entry
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;
	{
		// 1. Clock : octet-string(12)
		if (buf->from_value <= 1 && 1 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)&(p_buf_entry->clock_value),12);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 2. Event code Unsigned16(2)
		if (buf->from_value <= 2 && 2 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)&(p_buf_entry->event_code),2);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 3. Current: IR - signed16(4)
		if (buf->from_value <= 3 && 3 <= buf->to_value)
		{
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->current_value_IR),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 4. Current: IY - Unsigned16(2)
		if (buf->from_value <= 4 && 4 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->current_value_IY),4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 5. Current: IB - Unsigned16(2)
		if (buf->from_value <= 5 && 5 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->current_value_IB),4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 6. Voltage : VR - Unsigned16(2)
		if (buf->from_value <= 6 && 6 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->voltage_value_VR),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 7. Voltage : VY - Unsigned16(2)
		if (buf->from_value <= 7 && 7 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->voltage_value_VY),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 8. Voltage : VB - Unsigned16(2)
		if (buf->from_value <= 8 && 8 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->voltage_value_VB),4);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 9. power_factor : PF-R Integer16(2)
		if (buf->from_value <= 9 && 9 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->power_factor_R),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 10. power_factor : PF-Y Integer16(2)
		if (buf->from_value <= 10 && 10 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->power_factor_Y),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 11. power_factor : PF-B Integer16(2)
		if (buf->from_value <= 11 && 11 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->power_factor_B),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 12. Cumulative Energy – kWh : Integer32(4)
		if (buf->from_value <= 12 && 12 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->active_energy),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
#ifdef EVENTDATA_RECORDS_KVAH		
		// 13. Cumulative Energy – kVAh : Integer32(4)
		if (buf->from_value <= 13 && 13 <= buf->to_value){
			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_DOUBLE_LONG,(Unsigned8 *)&(p_buf_entry->apparent_energy),4);
			*p_out_len += length;
			p_out_buf  += length;
		}
#endif		
	}

	return (*p_out_len - org_len);
}

Unsigned16 R_OBIS_Class07_FilterOneNamePlateEntry(buffer_t *buf, Unsigned16 index, Unsigned8 *p_out_buf, Unsigned16 *p_out_len){
	Unsigned16 org_len;
	Unsigned16 length;
	Integer32 temp;

	org_len = *p_out_len;
	/* Encode each element of entry */
	*p_out_buf++ = ATTR_TYPE_STRUCTURE;
	*p_out_buf++ = (Unsigned8)(buf->to_value - buf->from_value + 1);
	*p_out_len  += 2;
	{
		Unsigned8 buffer[40];
		Integer16   size;
		void        *pbuffer;

		// 1. Meter serial number
		if (buf->from_value <= 1 && 1 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_METER_SERIAL, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 2. Manufacturer name 
		if (buf->from_value <= 2 && 2 <= buf->to_value){
			memset(buffer, 0, 40);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_MANUFACT_NAME, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 3. Firmware version for meter 
		if (buf->from_value <= 3 && 3 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_FIRMWARE_NAME, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 4. Meter type
		if (buf->from_value <= 4 && 4 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_METER_TYPE, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 5. Category 
		if (buf->from_value <= 5 && 5 <= buf->to_value)		{
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_CATEGORY_NAME, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}

		// 6. Current rating 
		if (buf->from_value <= 6 && 6 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_CURRRATE_NAME, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_OCTET_STRING,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 7. Internal CT Ratio 
		if (buf->from_value <= 7 && 7 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_CT_RATIO, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 8. Internal VT Ratio 
		if (buf->from_value <= 8 && 8 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_PT_RATIO, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}
		
		// 9. Meter year of manufacture 
		if (buf->from_value <= 9 && 9 <= buf->to_value){
			memset(buffer, 0, 16);
			pbuffer = buffer;
			size =  R_USER_GetNamePlate(NAME_PLATE_YEAR_OF_MANUFACT, pbuffer);
			if (size < 0)
				return 0;

			length = R_OBIS_EncodeAttribute(p_out_buf,OBIS_SERVICE_DATA_BUFFER_LENGTH,ATTR_TYPE_LONG_UNSIGNED,(Unsigned8 *)pbuffer,(Unsigned16)size);
			*p_out_len += length;
			p_out_buf  += length;
		}
	}

	return (*p_out_len - org_len);
}

#endif // (defined(USED_CLASS_07) && USED_CLASS_07 == 1)

#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
/******************************************************************************
* Function Name : R_OBIS_Class09_ScriptExecute
* Interface     : Unsigned8 R_OBIS_Class09_ScriptExecute(
*               :     action_item_t *p_action
*               : );
* Description   : Execute script based on action_item_t
* Arguments     : action_item_t *p_action: script need to execute
* Function Calls: T.B.D
* Return Value  : Unsigned8
*                 :     0    means un-supported.
*                 :     1    means supported.
******************************************************************************/
Unsigned8 R_OBIS_Class09_ScriptExecute(Unsigned8 *p_obis_code, Unsigned16 script_selector)
{
	/* Result of response, def. is not Matched */
	Unsigned8 response_result = 0;
	class09_child_record_t	*p_child_record9;

	uint8_t h;
	
	/*
	 * NOTE:
	 * Append processing for script by User
	 */
	/* Check first script table */
	p_child_record9 = (class09_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(9, 0));

	if (p_child_record9 == NULL)
	{
		return response_result;
	}

	if (R_OBIS_CompareObisCode(p_obis_code, p_child_record9->logical_name))
	{
		switch (script_selector)
		{
			case 0x0001:	/* Capture Data of Billing Period Profile */
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
				//R_OBIS_Class07_BillingUpdate();	
			displayHold=1;	// this will prevent the display function to be called
		  LCD_ClearAll();
			LCD_DisplayDigit(2, LCD_CHAR_R);
			LCD_DisplayDigit(3, LCD_CHAR_E);
			LCD_DisplayDigit(4, 5);
			LCD_DisplayDigit(5, LCD_CHAR_E);
			LCD_DisplayDigit(6, LCD_CHAR_T);
      generate_bill(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second,BT_MANU);
      write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
      e2write_flags&=~E2_W_CURRENTDATA;
			gentimer=0;
			
#endif /* (defined(USED_CLASS_07) && USED_CLASS_07 == 1) */
				return 1;
			default:
				/* Not supported */
				return 0;
//				break;
		}
	}

	/* Check 2nd script table */
	p_child_record9 = (class09_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(9, 1));

	if (p_child_record9 == NULL)
	{
		return response_result;
	}

	if (R_OBIS_CompareObisCode(p_obis_code, p_child_record9->logical_name))
	{
		switch (script_selector)
		{
			case 0x0001:	/* TOU/T O D scripts with tariffs TZ1 */
				R_USER_Class09_ScriptExecute(SCRIPT_ACTION_TZ1);
				return 1;
			case 0x0002:	/* TOU/T O D scripts with tariffs TZ1 */
				R_USER_Class09_ScriptExecute(SCRIPT_ACTION_TZ2);
				return 1;
			case 0x0003:	/* TOU/T O D scripts with tariffs TZ1 */
				R_USER_Class09_ScriptExecute(SCRIPT_ACTION_TZ3);
				return 1;
			case 0x0004:	/* TOU/T O D scripts with tariffs TZ1 */
				R_USER_Class09_ScriptExecute(SCRIPT_ACTION_TZ4);
				return 1;
			default:
				/* Not supported */
				return 0;
		}
	}

	return response_result;
}
#endif /* (defined(USED_CLASS_09) && USED_CLASS_09 == 1) */

#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
/******************************************************************************
* Function Name : R_OBIS_Class20_ProfileTimeCheck
* Interface     : Unsigned8 R_OBIS_Class20_ProfileTimeCheck(
*               :     class20_child_record_t *p_child_record,
*               :     date_time_t *p_current_time
*               : );
* Description   : Check time match or not
* Arguments     : class20_child_record_t *p_child_record: instance need to check
*               : date_time_t *p_current_time: current time
* Function Calls: T.B.D
* Return Value  : Unsigned8
*               :     0    means un-match.
*               :     1    means matched.
******************************************************************************/
void R_OBIS_Class20_ProfileTimeCheck(
	class20_child_record_t *p_child_record,
	date_time_t *p_current_time
)
{
	/* Result of response, def. is not Matched */
	Integer8 match_result = 1;
	Unsigned8 i; /* Counter */
	Unsigned8 j; /* Counter */
	day_profile_t		*p_child_day_profile;
	day_profile_action_t *p_child_day_profile_action;
	date_time_t target_time;
	Unsigned8 tbuf[8];

// create the obis code for day action {   0,   j,   10,   0,   100, 255 }
	tbuf[0]= 0;
	tbuf[1]= 0;	// this number will change
	tbuf[2]= 10;
	tbuf[3]= 0;
	tbuf[4]= 100;
	tbuf[5]= 255;
	
#if (defined(USED_CLASS_20_SEASON_PROFILE) && USED_CLASS_20_SEASON_PROFILE == 1)
	/* Just check change day */
	for (i = 0; i < p_child_record->nr_season_profile_active; i++)
	{
		season_profile_t *p_child_seanson_profile;
		p_child_seanson_profile = (season_profile_t *)(p_child_record->p_season_profile_active + i);

		match_result = R_OBIS_CompareDateTime((Unsigned8 *)p_child_seanson_profile->p_season_start, (Unsigned8 *) p_current_time);
		if (match_result == 0)
		{
			R_USER_Class20_ChangSeason(*p_current_time);
			break; // The list is sorted according to season_start (in increasing order)
		}
	}
#endif

#if (defined(USED_CLASS_20_WEEK_PROFILE) && USED_CLASS_20_WEEK_PROFILE == 1)
	/* Just check change day */
	if (p_current_time->hour == 0 && p_current_time->minute == 0 && p_current_time->second == 0)
	{
		 R_USER_Class20_ChangeWeekDay(*p_current_time);
	}
#endif

#if (defined(USED_CLASS_20_DAY_PROFILE) && USED_CLASS_20_DAY_PROFILE == 1)
	for (i = 0; i < p_child_record->nr_day_profile_active; i++)
	{
		p_child_day_profile = (day_profile_t *)(p_child_record->p_day_profile_active + i);

		for (j = 0; j < p_child_day_profile->nr_day_schedule; j ++)
		{
			p_child_day_profile_action = (day_profile_action_t *)(p_child_day_profile->day_schedule + j);
			
			
			/* Convert to date_time_t */
			target_time.year_high      =  NOT_SPECIFIED;
			target_time.year_low       =  NOT_SPECIFIED;
			target_time.month          =  NOT_SPECIFIED;
			target_time.day_of_month   =  NOT_SPECIFIED;
			target_time.day_of_week    =  NOT_SPECIFIED;
			target_time.hour           =  p_child_day_profile_action->p_start_time->hour;
			target_time.minute         =  p_child_day_profile_action->p_start_time->minute;
//			target_time.second         =  p_child_day_profile_action->p_start_time->second;
			target_time.second         =  NOT_SPECIFIED; 

			match_result = R_OBIS_CompareDateTime((Unsigned8 *)&target_time, (Unsigned8 *) p_current_time);
			
			#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
			if (match_result == 0)
			{
				tbuf[1]=j;
//				R_OBIS_Class09_ScriptExecute(p_child_day_profile_action->script_logical_name, p_child_day_profile_action->script_selector);
				R_OBIS_Class09_ScriptExecute(tbuf, p_child_day_profile_action->script_selector);
			}
			#endif

		}
		
	}
#endif
	return ;
}

// This function is called only when the status of class20 activation is pending
Unsigned8 R_OBIS_Class20_ActivationTimeCheck(
	class20_child_record_t *p_child_record,
	date_time_t *p_current_time
)
{
	/* Result of response, def. is not Matched */
	Integer8 match_result = 1;

	match_result = R_OBIS_CompareDateTime((Unsigned8 *)p_child_record->p_activate_calendar_passive, (Unsigned8 *) p_current_time);
	
	if(match_result>0)
		return 0;	// schedule date time is greater than current time - schedule time is in the future
	else
		return 1;	// schedule date time is equal to current time (now) or scheduled time has gone past

// if the scheduled time has gone past and class20_status is still 1, we need to execute it immediately		
		
//	if (match_result == 0)
//	{
//		return 1;
//	}
//	return 0;
}

Unsigned8 R_OBIS_Class20_Activate_PassiveCalendar(Unsigned16 child_index)
{// we need to transfer all passive data to active data 

	class20_total_data_t total_data;
	
	savePassiveData(&total_data);	// copy all passive data(bits&pieces) to total_data
	loadActiveData(&total_data);	// copy from total_data to bits & pieces active data
  write_alternately(ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS, ALTERNATE_ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS, (uint8_t*)&total_data, sizeof(total_data));	
	
/*	
	Unsigned8 ret = 1;
	Unsigned8 j = 1;
	
	class20_child_record_t	*p_child_record_20 = NULL;
	day_profile_t		*p_child_day_profile_active;	// added 
//	day_profile_t		*p_child_day_profile_passive;	// added 
//	day_profile_action_t *p_day_schedule_active;	// added 
//	day_profile_action_t *p_day_schedule_passive;	// added 
	
	
	p_child_record_20 = (class20_child_record_t *)(R_OBIS_FindChildRecordByIndex(20, child_index));

	if (p_child_record_20 != NULL)
	{
		// Copies all attributes called …_passive to the corresponding attributes called …_active. 
		memcpy(p_child_record_20->p_calendar_name_active, p_child_record_20->p_calendar_name_passive, *(p_child_record_20->p_calendar_name_active_len));

//#if (defined(USED_CLASS_20_SEASON_PROFILE) && USED_CLASS_20_SEASON_PROFILE == 1)
//		memcpy(p_child_record_20->p_season_profile_active, p_child_record_20->p_season_profile_passive, sizeof(season_profile_t) * p_child_record_20->nr_season_profile_active);
//#endif
#if (defined(USED_CLASS_20_WEEK_PROFILE) && USED_CLASS_20_WEEK_PROFILE == 1)
		memcpy(p_child_record_20->p_week_profile_active, p_child_record_20->p_week_profile_passive, sizeof(week_profile_t) * p_child_record_20->nr_week_profile_active);
#endif
#if (defined(USED_CLASS_20_DAY_PROFILE) && USED_CLASS_20_DAY_PROFILE == 1)
//		memcpy(p_child_record_20->p_day_profile_active, p_child_record_20->p_day_profile_passive, sizeof(day_profile_t) * p_child_record_20->nr_day_profile_active);
// The above command is messed up for some reason not known yet - 20/Jul/2019
// A bad temprorary fix is included below

		p_child_day_profile_active = (day_profile_t *)(p_child_record_20->p_day_profile_active);
//		p_child_day_profile_passive = (day_profile_t *)(p_child_record_20->p_day_profile_passive);
	

// This had been commented on 18/Jul/2023 - will need to be redone

		for (j = 0; j < p_child_day_profile_active->nr_day_schedule; j ++)
		{
			TZ_start_time0[j]=TZ_start_time1[j];
			day_action_active0[j].script_selector = day_action_passive0[j].script_selector;
		}

#endif


		// Notify to user interface 
//		ret = R_USER_Class20_Activate_PassiveCalendar(p_child_record_20);
	}
*/	
	
	return 0;
//	return ret;
}

#endif /* (defined(USED_CLASS_20) && USED_CLASS_20 == 1) */


// This function is called only when the status of class22 activation is pending
#if (defined(USED_CLASS_22) && USED_CLASS_22 == 1)

Unsigned8 R_OBIS_Class22_ScheduleCheck(
	class22_child_record_t *p_child_record,
	date_time_t *p_current_time
)
{
	/* Result of response, def. is not Matched */
	Integer8 match_result = 1;
	Unsigned8 i; /* Counter */
	execution_time_date_t *p_target_exetime;
	date_time_t target_time;

	for (i = 0; i < p_child_record->nr_execution_time; i++)
	{
		p_target_exetime =  (execution_time_date_t *)(p_child_record->p_execution_time + i);

		/* Convert execution_time_date_t to date_time_t */
		target_time.year_high      =  p_target_exetime->p_date->year_high;
		target_time.year_low       =  p_target_exetime->p_date->year_low;
		target_time.month          =  p_target_exetime->p_date->month;
		target_time.day_of_month   =  p_target_exetime->p_date->day_of_month;
//		target_time.day_of_week    =  p_target_exetime->p_date->day_of_week;	// commented -29/05/2019
		target_time.day_of_week    =  NOT_SPECIFIED;	// 29/05/2019
		target_time.hour           =  p_target_exetime->p_time->hour;
		target_time.minute         =  p_target_exetime->p_time->minute;
		target_time.second         =  p_target_exetime->p_time->second;
		target_time.hundredths 		 =	NOT_SPECIFIED;	// 29/05/2019
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&target_time, (Unsigned8 *) p_current_time); 
		
//		if (match_result == 0)
//		{
//			return 1;
//		}
		
		if(match_result>0)
			return 0;	// schedule date time is greater than current time - schedule time is in the future
		else
			return 1;	// schedule date time is equal to current time (now) or scheduled time has gone past
		
	}
	return 0;
}
#endif /* (defined(USED_CLASS_22) && USED_CLASS_22 == 1) */

void R_OBIS_Timer_ConstInterruptCallback(void)
{
	// Temp stick counter
	static Unsigned8 rtc_temp_stick = 0;

	// Multiply by 2, because RTC is 0.5s interval, modify this when RTC interval changed 
	// rtc interval is now 1 s - hence modified
	rtc_stick ++;
}

//main()->DLMS_PollingProcessing()->DLMSIntervalProcessing()->R_OBIS_IntervalProcessing()-> R_OBIS_MeterDataProcessing

void R_OBIS_MeterDataProcessing(void)
{
	/* Old stick counter */
	static Unsigned32 old_rtc_stick = 0;
	date_time_t current_time;
	Unsigned8 result;
	Unsigned8 i;
	uint8_t yr;
	
	if (old_rtc_stick != rtc_stick)
	{		
		Unsigned8 rlt = R_USER_GetSpecificTime(CURRENT_TIME, &current_time);
		current_time.day_of_week=NOT_SPECIFIED;	// added 29/05/2019
		current_time.hundredths=NOT_SPECIFIED;	// added 29/05/2019
		if (rlt != 0)
		{
			return;
		}

#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)

#if 0 // Change to update in event.c
		// Block load profile capture
		if (g_Class07_Blockload_CapturePeriod > 0)
		{
			if ((rtc_stick % g_Class07_Blockload_CapturePeriod) == 0)
			{
				// Prevent the update of entry when it is getting by async service
				if (g_Class07_Obj0_Capture_Enable)
				{
					R_USER_WDT_Restart();
					// Capture Data of Blockload Profile 
					//R_OBIS_Class07_BlockloadUpdate();
				}
			}
		}
		// Daily load profile capture 
		if (g_Class07_Dailyload_CapturePeriod > 0)
		{
			if ((rtc_stick % g_Class07_Dailyload_CapturePeriod) == 0)
			{
				R_USER_WDT_Restart();
				// Capture Data of Blockload Profile
				//R_OBIS_Class07_DailyloadUpdate();
			}
		}
#endif

#endif // (defined(USED_CLASS_07) && USED_CLASS_07 == 1)

		// Activity Calendar
#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
		if(class20_status!=0)
		{// there is a pending operation
			/// Activity Calendar pointer
			class20_child_record_t	*p_child_record_20;

			for (i = 0; i < *(g_MasterTable[20].child_list_size); i++)
			{
				R_USER_WDT_Restart();

				p_child_record_20 = (class20_child_record_t *)(
					R_OBIS_FindChildRecordByIndex(20, i));

				// Check activation activate_passive_calendar_time
				result = R_OBIS_Class20_ActivationTimeCheck(p_child_record_20, &current_time);
				if (result == 1)
				{// before activating the new calendar - one should also generate a bill
					yr = (uint8_t)((int16_t)current_time.year_high*(int16_t)256+(int16_t)current_time.year_low - (int16_t)2000);
					generate_bill(current_time.day_of_month, current_time.month, yr, current_time.hour, current_time.minute, current_time.second, BT_TRANS);
// now perform the switch
					R_OBIS_Class20_Activate_PassiveCalendar(i);
					
					class20_status=0;	// job done
					write_passive_tod_activation_data(class20_status);	// located in memoryOps.c
					
//since the active TOD data has been modified it should get saved in the e2rom, to be loaded in case of CPU reset
//					write_active_tod();	// save the new active tod in e2rom - being done in R_OBIS_Class20_Activate_PassiveCalendar
					
				}
				// Check Profile start time (Season, week, day) and execute script
				R_OBIS_Class20_ProfileTimeCheck(p_child_record_20, &current_time);
			}

		}
#endif // (defined(USED_CLASS_20) && USED_CLASS_20 == 1)

		// Single action schedule
#if (defined(USED_CLASS_22) && USED_CLASS_22 == 1)
		if(class22_status!=0)
		{
			Unsigned8 result;
			Unsigned8 i;
			// Single Action Schedule pointer
			class22_child_record_t	*p_child_record_22;

			for (i = 0; i < *(g_MasterTable[22].child_list_size); i++)
			{
				R_USER_WDT_Restart();

				p_child_record_22 = (class22_child_record_t *)(
					R_OBIS_FindChildRecordByIndex(22, i));

				result = R_OBIS_Class22_ScheduleCheck(p_child_record_22, &current_time);
				if (result == 1)
				{
//--------------------------------------------					
					execution_time_date_t *p_target_exetime;
					date_time_t target_time;

					for (i = 0; i < p_child_record_22->nr_execution_time; i++)
					{
						p_target_exetime =  (execution_time_date_t *)(p_child_record_22->p_execution_time + i);

						/* Convert execution_time_date_t to date_time_t */
						target_time.year_high      =  p_target_exetime->p_date->year_high;
						target_time.year_low       =  p_target_exetime->p_date->year_low;
						target_time.month          =  p_target_exetime->p_date->month;
						target_time.day_of_month   =  p_target_exetime->p_date->day_of_month;
						target_time.day_of_week    =  NOT_SPECIFIED;	// 29/05/2019
						target_time.hour           =  p_target_exetime->p_time->hour;
						target_time.minute         =  p_target_exetime->p_time->minute;
						target_time.second         =  p_target_exetime->p_time->second;
						target_time.hundredths 		 =	NOT_SPECIFIED;	// 29/05/2019
					}
//---------------------------------------------					
					yr = (uint8_t)((int16_t)target_time.year_high*(int16_t)256+(int16_t)target_time.year_low - (int16_t)2000);
					generate_bill(target_time.day_of_month, target_time.month, yr, target_time.hour, target_time.minute, target_time.second, BT_TRANS);
					computeNextBillDate();	// this is required since generate_bill does not call this function if bill type is NOT AUTO

// we must advance the bill date, else another bill will get made in the regular process - 13/Jun/2020 - gaurav feedback
					class22_status=0;
					write_single_action_activation_data(class22_status);
// following code commented by us - if we need to make a bill why go through all the fuss						
//	#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
//					R_OBIS_Class09_ScriptExecute(p_child_record_22->p_executed_script->script_logical_name, p_child_record_22->p_executed_script->script_selector);
//	#endif
				}
			}
		}
#endif // (defined(USED_CLASS_22) && USED_CLASS_22 == 1)
	}
		/* Importance: update old_rtc_stick */
		old_rtc_stick = rtc_stick;
}