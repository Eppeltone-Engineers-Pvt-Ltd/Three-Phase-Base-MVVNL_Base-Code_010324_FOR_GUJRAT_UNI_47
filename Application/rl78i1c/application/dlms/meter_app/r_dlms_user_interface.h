/******************************************************************************
  Copyright (C) 2016 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : r_dlms_user_interface.h
* Version      : 1.00
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.03.2016
******************************************************************************/

#ifndef _R_DLMS_USER_INTERFACE_H
#define _R_DLMS_USER_INTERFACE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_dlms_typedef.h"				/* DLMS Type Definitions */
#include "r_dlms_obis.h"				/* DLMS OBIS Definitions */
#include "DLMS_User.h"					/* DLMS User Definitions */
/* Application */
#include "eeprom.h"						/* EEPROM MW Layer */
#include "storage.h"					/* Storage Header File */

/******************************************************************************
Macro definitions
******************************************************************************/
/* Common macro */

/************************************************/
/* Storage information for User interface data  */
/************************************************/
/* Wrapper defination for ouside memory access */
#define E2PR_OK							EPR_OK

#define E2PR_READ(addr,buffer,size)		EPR_Read(addr,buffer,size)
#define E2PR_WRITE(addr,buffer,size)	EPR_Write(addr,buffer,size)

#define DLMS_ADDR_NOTSPECIFIC			(0x00)

/* Logical Device Name */
#define DLMS_LDN_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_LDN_ADDR)
#define DLMS_LDN_SIZE										(STORAGE_EEPROM_PLATFORM_INFO_LDN_SIZE)

/* Meter serial number */
#define DLMS_MSN_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_ADDR)
#define DLMS_MSN_SIZE										(STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_SIZE)

/* Manufacture Name */
#define DLMS_MFN_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_ADDR)
#define DLMS_MFN_SIZE										(STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_SIZE)

/* Firmware version for meter */
#define DLMS_FVM_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_ADDR)
#define DLMS_FVM_SIZE										(STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_SIZE)

/* Meter Type */
#define DLMS_MT_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_ADDR)
#define DLMS_MT_SIZE										(STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_SIZE)

/* Category */
#define DLMS_CATERGORY_ADDR									(STORAGE_EEPROM_PLATFORM_INFO_CATEGORY_ADDR)
#define DLMS_CATERGORY_SIZE									(STORAGE_EEPROM_PLATFORM_INFO_CATEGORY_SIZE)

/* Current rating  */
#define DLMS_CR_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_CURRENT_RATE_ADDR)
#define DLMS_CR_SIZE										(STORAGE_EEPROM_PLATFORM_INFO_CURRENT_RATE_SIZE)

/* Meter Year of Manufacture */
#define DLMS_MYM_ADDR										(STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_DATE_ADDR + 6)
#define DLMS_MYM_SIZE										(1) // Year only

/* Class 07 */
#define DLMS_BLOCK_DATA_LOG_ADDR							(STORAGE_EEPROM_BLOCK_DATA_LOG_ADDR)
#define DLMS_BLOCK_USED_ADDR								(STORAGE_EEPROM_BLOCK_USED_ADDR)
#define DLMS_BLOCK_INDEX_ADDR								(STORAGE_EEPROM_BLOCK_INDEX_ADDR)

#define DLMS_DAILY_DATA_LOG_ADDR							(STORAGE_EEPROM_DAILY_DATA_LOG_ADDR)
#define DLMS_DAILY_USED_ADDR								(STORAGE_EEPROM_DAILY_USED_ADDR)
#define DLMS_DAILY_INDEX_ADDR								(STORAGE_EEPROM_DAILY_INDEX_ADDR)

#define DLMS_BILLING_DATA_LOG_ADDR							(STORAGE_EEPROM_ENERGY_DATA_LOG_ADDR)

#define DLMS_EVENT_DATA_LOG_ADDR							0x0
#define DLMS_EVENT_USED_ADDR								(STORAGE_EEPROM_EVENT_USED_ADDR)
#define DLMS_EVENT_EVENT_INDEX_ADDR							0x0 //Update of EEPROM address


/* Recording interval 1, for blockload profile (in second) */
//#define CLASS07_BLOCKLOAD_INTERVAL			BLOCKLOAD_INTERVAL
//#define CLASS07_DAILYLOAD_INTERVAL			DAILYLOAD_INTERVAL /* 24 h*/

#define TOT_EVENT_TYPE          7 
#define HISTORY_SIZE            7 	//13

/* Class 15 */
#define DLMS_LLS_SECRET_ADDR								(STORAGE_EEPROM_LLS_SECRET_ADDR)
#define DLMS_LLS_SECRET_SIZE								(STORAGE_EEPROM_LLS_SECRET_SIZE)
#define DLMS_HLS_SECRET_ADDR								(STORAGE_EEPROM_HLS_SECRET_ADDR)
#define DLMS_HLS_SECRET_SIZE								(STORAGE_EEPROM_HLS_SECRET_SIZE)
/* ID For Name plate data */
#define NAME_PLATE_LOGICAL_NAME			(0x01)
#define NAME_PLATE_METER_SERIAL			(0x02)
#define NAME_PLATE_MANUFACT_NAME			(0x03)
#define NAME_PLATE_FIRMWARE_NAME			(0x04)
#define NAME_PLATE_METER_TYPE			(0x05)
#define NAME_PLATE_CATEGORY_NAME			(0x06)
#define NAME_PLATE_CURRRATE_NAME			(0x07)
#define NAME_PLATE_YEAR_OF_MANUFACT		(0x08)
#define NAME_PLATE_CT_RATIO				(0x09)
#define NAME_PLATE_PT_RATIO				(0x0A)

/* ID For Time */
#define CURRENT_TIME						(0x11)
#define LASTBILLING_TIME					(0x12)
#define LAST_ACTIVE_MD_TIME				(0x13)
#define LAST_APPARENT_MD_TIME			(0x14)
/* ID For Script Execute */
#define SCRIPT_ACTION_TZ1				(0x01)
#define SCRIPT_ACTION_TZ2				(0x02)
#define SCRIPT_ACTION_TZ3				(0x03)
#define SCRIPT_ACTION_TZ4				(0x04)
/* Scaler */
#define SCALAR_NULL 0
#define SCALAR_VOLTAGE -1
#define SCALAR_AMPERE -2
#define SCALAR_PF -3
#define SCALAR_APF -3
#define SCALAR_HERTZ -1
#define SCALAR_VA 0
#define SCALAR_WATT 0
#define SCALAR_MD 0
#define SCALAR_WH 0
#define SCALAR_VAH 0
#define SCALAR_CON -1
#define SCALAR_BL_WH 0
#define SCALAR_BL_VAH 0
#define SCALAR_WH_HIRES -2
#define SCALAR_VAH_HIRES -2
#define SCALAR_VARH_HIRES -2

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Same as RTC Date Time */
typedef struct tagDLMS_RTCDateTime
{
	/* Total: 8 bytes */
	Unsigned8	Sec;		/* Second */
	Unsigned8	Min;		/* Minute */
	Unsigned8	Hour;		/* Hour */
	Unsigned8	Day;		/* Day */
	Unsigned8	Week;		/* Day of week */
	Unsigned8	Month;		/* Month */
	Unsigned8	Year;		/* Year (ony 2 ending digit) */
	
	Unsigned8	reserved;	/* Reserved Byte (Padding) - NO USE */
	
} DLMS_RTC_DATE_TIME;

#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)

typedef struct tag_class07_event_entry_t_new
{
	/* Total: 16 + 8 + 2 bytes */
	Integer32 current;
	Integer32 voltage;
	Integer32 power_factor;
	Integer32 active;						/* Active */
	DLMS_RTC_DATE_TIME	present_rtc_log;	/* Present RTC monthly log */
	uint16_t	eventID;
} class07_event_entry_t_new;
#endif
/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* I/F to Time Access */


void convert_data_string(unsigned long int, unsigned char);

Unsigned8 R_USER_GetSpecificTime(Unsigned16 time_id, date_time_t *p_date_time);
Unsigned8 R_USER_SetSpecificTime(Unsigned16 time_id, date_time_t date_time);

/* I/F to WDT */
void R_USER_WDT_Restart(void);

/* I/F to DLMS name plate */
Integer16 R_USER_GetNamePlate(Unsigned16 nameplate_id, Unsigned8* p_data);

/* I/F to EM APIs*/
Integer32 R_USER_GetEMData(Unsigned16 em_data);
Integer8 R_USER_SetEMData(Unsigned16 em_data, Unsigned8 *p_em_value);

/* I/F to Event notification */
void R_USER_Event_Report(Unsigned8 event_type, Unsigned16 event_code);

void R_OBIS_Update_Secret(void);

#if (defined(USED_CLASS_04) && USED_CLASS_04 == 1)
/* Class04 - Extended Register */
Unsigned8 R_OBIS_Class04_GetActiveMD(
	Integer32           *p_value,
	date_time_t         *p_time
);

Unsigned8 R_OBIS_Class04_GetApparentMD(
	Integer32           *p_value,
	date_time_t         *p_time
);
#endif

#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
// Class07 - Generic profile, Blockload profile 
/*
Unsigned16 R_USER_Class07_GetBlockLoadEntry(
	class07_blockload_entry_t_new		*buf,
	Unsigned16						index
);
*/

Unsigned8 R_USER_Class07_BlockLoadProfile_SetPrams(
	Unsigned32		EntriesInUse,			/* Number of entries in use */
	Unsigned32		CurrentEntries,			/* Index of current entries from 0 */
	Unsigned32		MaxEntries				/* Number of max entries */
);
Unsigned8 R_USER_Class07_BlockLoadProfile_GetPrams(
	Unsigned32		*p_EntriesInUse,		/* Number of entries in use */
	Unsigned32		*p_CurrentEntries,		/* Index of current entries from 0 */
	Unsigned32		*p_MaxEntries			/* Number of max entries */
);

/* Class07 - Generic profile, Dailyload profile */
/*
Unsigned16 R_USER_Class07_GetDailyLoadEntry(
	class07_dailyload_entry_t_new		*buf,
	Unsigned16						index
);
*/
/*
Unsigned8 R_USER_Class07_DailyloadProfile_SetPrams(
	Unsigned32		EntriesInUse,			// Number of entries in use 
	Unsigned32		CurrentEntries,			// Index of current entries from 0 
	Unsigned32		MaxEntries				// Number of max entries 
);
Unsigned8 R_USER_Class07_DailyloadProfile_GetPrams(
	Unsigned32		*p_EntriesInUse,		// Number of entries in use 
	Unsigned32		*p_CurrentEntries,		// Index of current entries from 0 
	Unsigned32		*p_MaxEntries			// Number of max entries 
);
*/
/* Class07 - Generic profile, Billing profile */
/*
Unsigned16 R_USER_Class07_GetBillingEntry(
	class07_billing_entry_t_new			*buf,
	Unsigned16						index
);
*/

Unsigned8 R_USER_Class07_BillingProfile_SetPrams(
	Unsigned32		EntriesInUse,			/* Number of entries in use */
	Unsigned32		CurrentEntries,			/* Index of current entries from 0 */
	Unsigned32		MaxEntries				/* Number of max entries */
);
Unsigned8 R_USER_Class07_BillingProfile_GetPrams(
	Unsigned32		*p_EntriesInUse,		/* Number of entries in use */
	Unsigned32		*p_CurrentEntries,		/* Index of current entries from 0 */
	Unsigned32		*p_MaxEntries			/* Number of max entries */
);

/* Class07 - Generic profile, Event profile */
Unsigned16 R_USER_Class07_GetEventEntry(
	Unsigned32						event_type,
	class07_event_entry_t_new			*buf,
	Unsigned16						index
);

Unsigned8 R_USER_Class07_EventProfile_SetPrams(
	Unsigned16		event_type,				/* Event object index from 0 */
	Unsigned32		EntriesInUse,			/* Number of entries in use */
	Unsigned32		CurrentEntries,			/* Index of current entries from 0 */
	Unsigned32		MaxEntries				/* Number of max entries */
);
Unsigned8 R_USER_Class07_EventProfile_GetPrams(
	Unsigned16		event_type,				/* Event object index from 0 */
	Unsigned32		*p_EntriesInUse,		/* Number of entries in use */
	Unsigned32		*p_CurrentEntries,		/* Index of current entries from 0 */
	Unsigned32		*p_MaxEntries			/* Number of max entries */
);

Unsigned8 R_OBIS_Class07_AddEventEntry(Unsigned8 event_type, uint16_t event_code);

#endif

#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
/* Class09 - Script table */
Unsigned8 R_USER_Class09_ScriptExecute(Unsigned16 ID);

#endif /* #if (defined(USED_CLASS_09) && USED_CLASS_09 == 1) */

#if (defined(USED_CLASS_15) && USED_CLASS_15 == 1)
/* Class15 -  Association LN */
Unsigned8 R_USER_Class15_Secret_Backup(
	Unsigned8             *p_secret,
	Unsigned16            length,
	Unsigned8             mechanism_id
);

Unsigned8 R_USER_Class15_Secret_Restore(
	Unsigned8             *p_secret,
	Unsigned16            length,
	Unsigned8             mechanism_id
);

#endif /* #if (defined(USED_CLASS_15) && USED_CLASS_15 == 1) */

#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
/* Class20 - Activity Calendar */
Unsigned8 R_USER_Class20_Active_Passive_Time(void);
Unsigned8 R_USER_Class20_GetTariffSeasonTable(
	season_profile_t    *buf,
	Unsigned16          index,
	Unsigned16          is_active
);
Unsigned8 R_USER_Class20_GetTariffWeekTable(
	week_profile_t      *buf,
	Unsigned16          index,
	Unsigned16          is_active
);
Unsigned8 R_USER_Class20_GetTariffDayTable(
	day_profile_t       *buf,
	Unsigned16          index,
	Unsigned16          is_active
);

Unsigned8 R_USER_Class20_Activate_PassiveCalendar(class20_child_record_t *p_child_record_20);

#if (defined(USED_CLASS_20_SEASON_PROFILE) && USED_CLASS_20_SEASON_PROFILE == 1)
void R_USER_Class20_ChangSeason(date_time_t current_time);
#endif /* #if (defined(USED_CLASS_20_SEASON_PROFILE) && USED_CLASS_20_SEASON_PROFILE == 1) */

#if (defined(USED_CLASS_20_WEEK_PROFILE) && USED_CLASS_20_WEEK_PROFILE == 1)
void R_USER_Class20_ChangeWeekDay(date_time_t current_time);

#endif /* #if (defined(USED_CLASS_20_WEEK_PROFILE) && USED_CLASS_20_WEEK_PROFILE == 1) */

#endif /* #if (defined(USED_CLASS_20) && USED_CLASS_20 == 1) */
void R_USER_Set_RTC_timezone(void);

//void Set_PP();

#endif /* _R_DLMS_USER_INTERFACE_H */

