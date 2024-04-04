/******************************************************************************
  Copyright (C) 2013 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : r_dlms_data_meter.h
* Version      : 1.00
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 18.06.2013
******************************************************************************/

#ifndef _R_DLMS_DATA_METER_H
#define _R_DLMS_DATA_METER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_dlms_typedef.h"				/* DLMS Type Definitions */
#include "r_dlms_obis.h"				/* DLMS OBIS Definitions */
#include "r_dlms_user_interface.h"		/* DLMS User Interface Definitions */
#include "DLMS_User.h"					/* DLMS User Definitions */
/* Application */
//#include "eeprom.h"						/* EEPROM MW Layer */
//#include "storage.h"					/* Storage Header File */

/******************************************************************************
Macro definitions
******************************************************************************/
/* Common macro */
#define	_BCD2DEC(x)		{x = (x & 0xF) + (x >> 4) * 10;}				/* 1 byte BCD to DEC Conversion */
#define	_DEC2BCD(x)		{x = (x % 10 ) | (((x / 10) << 4) & 0xF0);}		/* 1 byte DEC to BCD Conversion */


/* ID For EM data */

#define I_R												(0x01)
#define I_Y												(0x02)
#define I_B												(0x03)

#define V_R												(0x04)
#define V_Y												(0x05)
#define V_B												(0x06)

#define PF_R											(0x07)
#define PF_Y											(0x08)
#define PF_B											(0x09)
#define PF_TOTAL									(0x0A)

#define APPARENT_POWER						(0x0B)
#define ACTIVE_POWER							(0x0C)
#define REACTIVE_POWER						(0x0D)

#define CUM_ACTIVE_ENERGY					(0x0E)
#define CUM_REACTIVE_LAG_ENERGY		(0x0F)
#define CUM_REACTIVE_LEAD_ENERGY	(0x10)
#define CUM_APPARENT_ENERGY				(0x11)

#define CUM_POWER_FAILURE_COUNT		(0x12)
#define CUM_POWER_FAILURE_DURN		(0x13)

#define CUM_TAMPER_CNT					(0x14)
#define CUM_BILLING_CNT					(0x15)
#define CUM_PROGAM_CNT							(0x16)

#define BILLING_DATE							(0x17)
#define FREQUENCY									(0x18)

#define EM_ACTIVE_MD							(0x19)
#define EM_APPARENT_MD						(0x1A)
#define EM_REACTIVE_MD						(0x1B)

#define I_N												(0x1C)

#define DEMAND_INTEGRATION_PERIOD		(0x1D)
#define PROFILE_CAPTURE_PERIOD		(0x1E)
#define AVL_BILLING_PERIOD				(0x1F)

#define EM_BLOCK_ENERGY_KWH					(0x20)
#define EM_BLOCK_ENERGY_KVARH_LAG		(0x21)
#define EM_BLOCK_ENERGY_KVARH_LEAD	(0x22)
#define EM_BLOCK_ENERGY_KVAH				(0x23)
#define POWER_ON_DURATION					(0x24)

#define I_R_AVG											(0x25)
#define I_Y_AVG											(0x26)
#define I_B_AVG											(0x27)
#define V_R_AVG											(0x28)
#define V_Y_AVG											(0x29)
#define V_B_AVG											(0x2A)
#define CUM_ACTIVE_ENERGY_EXPORT		(0x2F)
#define CUM_REACTIVE_LAG_ENERGY_EXPORT		(0x30)
#define CUM_REACTIVE_LEAD_ENERGY_EXPORT	(0x31)
#define CUM_APPARENT_ENERGY_EXPORT				(0x32)
#define EM_ACTIVE_MD_EXPORT					(0x33)
#define EM_APPARENT_MD_EXPORT				(0x34)



//#define EM_ACTIVE_TARIFF					(0x21)
//#define EM_REACTIVE_TARIFF					(0x22)
//#define EM_APPARENT_TARIFF					(0x23)

//#define EM_VOLTAGE							(0x31)
//#define EM_AVERAGE_VOLTAGE					(0x32)

//#define EM_CURRENT							(0x41)
//#define EM_CURRENT_RATING					(0x42)
//#define EM_PHASE_CURRENT					(0x43)
//#define EM_NEUTRAL_CURRENT					(0x44)

//#define EM_POWER_FACTOR						(0x51)
//#define EM_AVERAGE_POWER_FACTOR				(0x52)

//#define EM_LINE_FREQ						(0x61)
/*

#define EM_BILL_DATE				(0x73)
#define EM_BILL_TIME				(0x74)
#define EM_PASSWORD				(0x75)

#define EM_POWER_ON_DURATION				(0x81)
#define EM_CUCUMLARIVE_POWER_ON_DURATION	(0x82)


#define EM_AVL_BILLING_PERIOD			(0x94)


#define EM_CUCUMLARIVE_ENERGY_KWH			(0xa0)
#define EM_CUCUMLARIVE_ENERGY_KWH_TZ1		(0xa1)
#define EM_CUCUMLARIVE_ENERGY_KWH_TZ2		(0xa2)
#define EM_CUCUMLARIVE_ENERGY_KWH_TZ3		(0xa3)
#define EM_CUCUMLARIVE_ENERGY_KWH_TZ4		(0xa4)

#define EM_CUCUMLARIVE_ENERGY_KVAH			(0xb0)
#define EM_CUCUMLARIVE_ENERGY_KVAH_TZ1		(0xb1)
#define EM_CUCUMLARIVE_ENERGY_KVAH_TZ2		(0xb2)
#define EM_CUCUMLARIVE_ENERGY_KVAH_TZ3		(0xb3)
#define EM_CUCUMLARIVE_ENERGY_KVAH_TZ4		(0xb4)

#define EM_BLOCK_ENERGY_KWH					(0xc1)
#define EM_BLOCK_ENERGY_KVAH				(0xc2)
*/

/* For Event type */
#define POWER_RELATED_EVENTS						(0)		/* Voltage related events */
#define CURRENT_RELATED_EVENTS						(1)		/* Current related events */
//#define POWERFAILURE_RELATED_EVENTS				(2)		/* Power failure related events */
#define TRANSACTION_RELATED_EVENTS					(2)		/* Transaction related events */
#define OTHER_EVENTS								(3)		/* Other events */
#define NON_ROLLER_EVENTS							(4)		/* Non-rollover events */
//#define CONTROL_EVENTS							(6)		/* Control events for connect/disconnect */
/* For Event ID */
#define EVENT_ID_HIGH_VOLTAGE_OCCUR					(7)		/* High Voltage in any Phase - Occurrence */
#define EVENT_ID_HIGH_VOLTAGE_RESTORE				(8)		/* High Voltage in any Phase - Restoration */
#define EVENT_ID_LOW_VOLTAGE_OCCUR					(9)		/* Low Voltage in any Phase - Occurrence */
#define EVENT_ID_LOW_VOLTAGE_RESTORE				(10)	/* Low Voltage in any Phase - Restoration */
#define EVENT_ID_EARTH_LOADING_OCCUR				(69)	/* Earth Loading  – Occurrence */
#define EVENT_ID_EARTH_LOADING_RESTORE				(70)	/* Earth Loading  – Restoration */
#define EVENT_ID_POWER_FAIL_OCCUR					(101)	/* Power failure  – Occurrence */
#define EVENT_ID_POWER_FAIL_RESTORE					(102)	/* Power failure  – Restoration */
#define EVENT_ID_TRANS_REALTIME						(151)	/* Real Time Clock – Date and Time */
#define EVENT_ID_TRANS_DEMAND_INTEGRA_PERIOD		(152)	/* Demand Integration Period  */
#define EVENT_ID_TRANS_PROFILE_CAPTURE_PERIOD		(153)	/* Profile Capture Period */
#define EVENT_ID_TRANS_SINGLE_ACTION_BILLING		(154)	/* Single-action Schedule for Billing Dates */
#define EVENT_ID_TRANS_ACT_CALENDAR_TIME_ZONE		(155)	/* Activity Calendar for Time Zones */
#define EVENT_ID_OTHER_MAGNET_OCCUR					(201)	/* Influence of Permanent Magnet or AC/ DC Electromagnet – Occurrence */
#define EVENT_ID_OTHER_MAGNET_RESTORE				(202)	/* Influence of Permanent Magnet or AC/ DC Electromagnet – Restoration */
#define EVENT_ID_OTHER_NEUTRAL_DISTUR_OCCUR			(203)	/* DC signal injection in source/load neutral terminal of the Meter through Diode  / Rectifier or Neutral Disturbance – HF - Occurrence */
#define EVENT_ID_OTHER_NEUTRAL_DISTUR_RESTORE		(204)	/* DC signal injection in source/load neutral terminal of the Meter through Diode  / Rectifier or Neutral Disturbance – HF - Restoration */
#define EVENT_ID_OTHER_NEUTRAL_MISS_OCCUR			(207)	/* Single wire operation (Neutral missing) – Occurrence */
#define EVENT_ID_OTHER_NEUTRAL_MISS_RESTORE			(208)	/* Single wire operation (Neutral missing) – Restoration */
#define EVENT_ID_NON_ROLL_OVER_COVER_OPEN			(251)	/* Meter Cover Opening – Occurrence */



/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
/* Class07 - Generic profile, Scaler profile */
Unsigned16 R_OBIS_Class07_FilterOneScalerEntry (
	buffer_t            *buf,
	Unsigned16          index,
	Unsigned16          scaler_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

/* Class07 - Generic profile, Instantaneous snapshot profile */
Unsigned16 R_OBIS_Class07_FilterOneInstEntry(
	buffer_t            *buf,
	Unsigned16          index,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

/* Class07 - Generic profile, Blockload profile */
void R_OBIS_Class07_BlockLoadUpdate_Pause(void);
void R_OBIS_Class07_BlockLoadUpdate_Start(void);
Unsigned16 R_OBIS_Class07_FilterOneBlockLoadEntry(
	buffer_t            *buf,
	Unsigned16          index,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

void R_OBIS_Class07_BlockloadUpdate(void);

/* Class07 - Generic profile, Dailyload profile */
Unsigned16 R_OBIS_Class07_FilterOneDailyLoadEntry(
	buffer_t            *buf,
	Unsigned16          index,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

void R_OBIS_Class07_DailyloadUpdate(void);

/* Class07 - Generic profile, Billing profile */
Unsigned16 R_OBIS_Class07_FilterOneBillingEntry(
	buffer_t            *buf,
	Unsigned16          index,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);
void R_OBIS_Class07_BillingUpdate(void);
void R_OBIS_Class07_BillingDateSync(void);

/* Class07 - Generic profile, Event profile */
Unsigned16 R_OBIS_Class07_FilterOneEventEntry(
	buffer_t            *buf,
	Unsigned16          child_index,
	Unsigned16          index,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

/* Class07 - Generic profile, Name Plate profile */
Unsigned16 R_OBIS_Class07_FilterOneNamePlateEntry(
	buffer_t            *buf,
	Unsigned16          index,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

#endif

#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
/* Class09 - Script table, execute script */
Unsigned8 R_OBIS_Class09_ScriptExecute(Unsigned8 *p_obis_code, Unsigned16 script_selector);
#endif

#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
/* Class22 - Activity calendar, check time to execute */
Unsigned8 R_OBIS_Class20_ActivationTimeCheck(class20_child_record_t *p_child_record, date_time_t *p_current_time);
Unsigned8 R_OBIS_Class20_Activate_PassiveCalendar(Unsigned16 child_index);
#endif

#if (defined(USED_CLASS_22) && USED_CLASS_22 == 1)
/* Class22 - Single Action Schedule, check and fine script id to execute */
Unsigned8 R_OBIS_Class22_ScheduleCheck(class22_child_record_t *p_child_record, date_time_t *p_current_time);
#endif

/* I/F to RTC Time */
Unsigned8 R_OBIS_IsValidDateTime(Unsigned8 *p_date_time);
Unsigned8 R_OBIS_IsValidDate(Unsigned8 *p_date);
Unsigned8 R_OBIS_IsValidTime(Unsigned8 *p_time);

Integer8 R_OBIS_CompareDateTime(Unsigned8 *p_src_date_time, Unsigned8 *p_des_date_time);
Integer8 R_OBIS_CompareDate(Unsigned8 *p_src_date_time,	Unsigned8 *p_des_date_time);
Integer8 R_OBIS_CompareTime(Unsigned8 *p_src_date_time,	Unsigned8 *p_des_date_time);


/* I/F to Event notification */
void R_USER_Event_Report(Unsigned8 event_type, Unsigned16 event_code);

/* Const Callback (0.5 s) */
void R_OBIS_Timer_ConstInterruptCallback(void);

/* Meter data update */
void R_OBIS_MeterDataProcessing(void);

void get_bill_data(unsigned int);
void get_tamper_data(unsigned int,unsigned int);
void get_lp_data(unsigned int);
void get_dlp_data(unsigned int);
void get_time_data(unsigned long int, unsigned long int);
void Set_PP(unsigned int, unsigned char);

#endif /* _R_DLMS_DATA_METER_H */

