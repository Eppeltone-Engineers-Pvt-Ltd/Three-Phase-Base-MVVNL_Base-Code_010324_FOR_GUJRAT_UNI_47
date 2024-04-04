/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : r_dlms_data.h
* Version      : 1.00
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 21.02.2013
******************************************************************************/

#ifndef _R_DLMS_DATA_H
#define _R_DLMS_DATA_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_dlms_typedef.h"				/* DLMS Type Definitions       */
#include "r_dlms_obis.h"				/* DLMS OBIS Definitions       */
#include "r_dlms_data_meter.h"				/* DLMS Data meter Definitions */
#include "DLMS_User.h"					/* DLMS User Definitions       */
/* Application */
#include "emeter3_structs.h"
#include "memoryOps.h"
/******************************************************************************
Macro definitions
******************************************************************************/
/* Class ID = 07 - Profile generic class */
//#define CLASS07_BLOCKLOAD_MAX_ENTRY			NO_OF_DAYS_SURVEY_DATA*48		// Max number of entries - 30*48 - modify once memory type etc are finalised
#define CLASS07_BLOCKLOAD_MAX_ENTRY			((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)		// Max number of entries - 30*48 - modify once memory type etc are finalised



#define CLASS07_DAILYLOAD_MAX_ENTRY			NO_OF_DAILY_LOG_DATA		// Max number of entries - modify once memory type etc are finalised

#define CLASS07_BILLING_MAX_ENTRY			7		/* Max number of entries */

#define CLASS07_EVENT_MAX_PROFILE			5		/* Max index of profile objects (from 0 ~ max) */
#define CLASS07_EVENT_MAX_ENTRY				60		/* Max number of entries */

/* Recording interval 1, for blockload profile (in second) */
#define CLASS07_BLOCKLOAD_INTERVAL			1800// for minutes USER
#define CLASS07_DAILYLOAD_INTERVAL			(86400) /* in seconds 24 h*/
/******************************************************************************
Typedef definitions
******************************************************************************/
// this structure has been modified by us - several Int32 have been replace with uint16/int16
// the earlier definition of the following type had int32_t instead of uint16_t
typedef struct tag_class07_blockload_entry_t
{// load survey 
	date_time_t	clock_value;			// Clock - 12 bytes defined in r_dlms_obis_ic.h (188)
	
#ifdef SURVEY_USES_AVERAGE_DATA	
	Integer32	current_value_IR;		// Current  
	Integer32	current_value_IY;		// Current  	
	Integer32	current_value_IB;		// Current  	
	Integer32	voltage_value_VR;		// Average Voltage 
	Integer32	voltage_value_VY;		// Average Voltage 
	Integer32	voltage_value_VB;		// Average Voltage 
#endif

	Integer32	kWh_value;			// Block Energy � kWh  
	
#ifdef SURVEY_USES_REACTIVE_DATA
	Integer32	kvarh_lag_value;		// Block energy, kvarh (lag) 
	Integer32	kvarh_lead_value;		// Block energy, kvarh (lead)
#endif

	Integer32	kVAh_value;			// Block Energy � kVAh
	
#ifdef SURVEY_USES_EXPORT_DATA	
	Integer32	kWh_value_export;			// Block Energy � kWh export
#endif

#ifdef SURVEY_USES_AVERAGE_PF
	Integer32	avgPF_r;			
	Integer32	avgPF_y;			
	Integer32	avgPF_b;			
#endif

#ifdef SURVEY_USES_FREQ_DATA
	Integer32	frequency;
#endif

#ifdef SURVEY_USES_DEMAND_DATA
	Integer32	apparentDemand;
	Integer32 activeDemand;
#endif

} class07_blockload_entry_t;


#ifdef SMALL_DAILY_LOG
typedef struct tag_class07_dailyload_entry_t
{
	date_time_t	clock_value;		// Clock
	Integer32	kWh_value;		// Cumulative Energy � kWh
	#ifdef BIDIR_METER
	Integer32	kWh_value_export;		// Cumulative Energy � kWh export
	#else
	Integer32	kVAh_value;		// Cumulative Energy � kVAh
	#endif	
} class07_dailyload_entry_t;
#else
	#ifdef DAILY_POWER_ON_LOGGING
typedef struct tag_class07_dailyload_entry_t
{
	date_time_t	clock_value;		// Clock
	Integer32	kWh_value;		// Cumulative Energy � kWh
	Integer32	kVAh_value;		// Cumulative Energy � kVAh
	Integer32	dailyPonMinutes;		// Daily Power On Minutes
} class07_dailyload_entry_t;
	#else
typedef struct tag_class07_dailyload_entry_t
{
	date_time_t	clock_value;		// Clock
	Integer32	kWh_value;		// Cumulative Energy � kWh
	Integer32	kVAh_value;		// Cumulative Energy � kVAh
	Integer32	kVArh_lag_value;		// Cumulative Energy � kVArh_lag
	Integer32	kVArh_lead_value;		// Cumulative Energy � kVAh_lead
} class07_dailyload_entry_t;
	#endif
#endif


typedef struct tag_class07_billing_entry_t
{
	date_time_t		bill_date;			// Billing date - 12 bytes  
	Integer32		power_factor;			// System power factor for billing  
	Integer32		active_energy;			// Cumulative Energy � kWh  
	Integer32		active_energyTZ[TARIFFS];		// Cumulative Energy kWh TZ1 ~ TZ5  
	Integer32		kvarh_lag_value;		// Cumulative Energy, kvarh (lag)
	Integer32		kvarh_lead_value;		// Cumulative Energy, kvarh (lead)   
	Integer32		apparent_energy;		// Cumulative Energy � kVAh  
	Integer32		apparent_energyTZ[TARIFFS];		// Cumulative Energy kVAh TZ1 ~ TZ5  
	Integer32		active_MD;			// MD kW  
	date_time_t		active_MD_DT;			// MD kW Date Time 
	Integer32		active_MD_TZ[TARIFFS];			// MD kW  
	date_time_t		active_MD_DT_TZ[TARIFFS];			// MD kW Date Time 
	Integer32		apparent_MD;			// MD kVA  
	date_time_t		apparent_MD_DT;			// MD kVA Date Time 
	Integer32		apparent_MD_TZ[TARIFFS];			// MD kVA  
	date_time_t		apparent_MD_DT_TZ[TARIFFS];			// MD kVA Date Time 
	Integer32		total_power;			// Billing power ON duration in minutes (During Billing period) - now cumulative since 04/02/2024
#ifdef BILLING_HAS_TAMPER_COUNT
	Integer32		tamper_count;			// Billing power ON duration in minutes (During Billing period)
#endif
#ifdef TOD_HAS_KVARH_LAG
	Integer32		reactive_energy_lagTZ[TARIFFS];		// Cumulative Energy kvarh_lag TZ1 ~ TZ5  
#endif

#ifdef BIDIR_METER
	Integer32		active_energy_export;			// Cumulative Energy � kWh  
	Integer32		active_energyTZ_export[TARIFFS];		// Cumulative Energy kWh TZ1 ~ TZ8  
	Integer32		kvarh_lag_value_export;		// Cumulative Energy, kvarh (lag)  
	Integer32		kvarh_lead_value_export;		// Cumulative Energy, kvarh (lag)  
	Integer32		apparent_energy_export;		// Cumulative Energy � kVAh  
	Integer32		apparent_energyTZ_export[TARIFFS];		// Cumulative Energy kVAh TZ1 ~ TZ8
	Integer32		active_MD_export;			// MD kW  
	date_time_t		active_MD_DT_export;			// MD kW Date Time 
	Integer32		active_MD_TZ_export[TARIFFS];			// MD kW  
	date_time_t		active_MD_DT_TZ_export[TARIFFS];			// MD kW Date Time 
	Integer32		apparent_MD_export;			// MD kVA  
	date_time_t		apparent_MD_DT_export;			// MD kVA Date Time 
	Integer32		apparent_MD_TZ_export[TARIFFS];			// MD kVA  
	date_time_t		apparent_MD_DT_TZ_export[TARIFFS];			// MD kVA Date Time
	Integer32		kwh_nonSolar_export;	// added 04/02/2024
	Integer32		kwh_Solar_import;	// added 04/02/2024
	Integer32		kwh_Solar_export;	// added 04/02/2024
#endif	

#ifdef SPLIT_ENERGIES_REQUIRED
	Integer32		active_energy_lag;			// Cumulative Energy � kWh lag  
	Integer32		active_energy_lead;			// Cumulative Energy � kWh lead  
	Integer32		apparent_energy_lag;			// Cumulative Energy � kVAh lag  
	Integer32		apparent_energy_lead;			// Cumulative Energy � kVAh lead  
#endif
} class07_billing_entry_t;

typedef struct tag_class07_event_entry_t
{// this structure has been modified by us - several Int32 have been replace with uint16/int16
	date_time_t		clock_value;			// Clock  
	Unsigned16		event_code;			// Event code   
	Integer32		current_value_IR;		// Current   
	Integer32		current_value_IY;		// Current   	
	Integer32		current_value_IB;		// Current   	
	Integer32		voltage_value_VR;		// Average Voltage  
	Integer32		voltage_value_VY;		// Average Voltage  
	Integer32		voltage_value_VB;		// Average Voltage  
	Integer32		power_factor_R;			// Signed Power Factor  
	Integer32		power_factor_Y;			// Signed Power Factor  
	Integer32		power_factor_B;			// Signed Power Factor  
	Integer32		active_energy;			// Cumulative Energy - kWh  
#ifdef EVENTDATA_RECORDS_KVAH
	Integer32		apparent_energy;			// Cumulative Energy - kWh  
#endif
} class07_event_entry_t;

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
void ConvertBackToRTC(ext_rtc_t*dst, date_time_t*src);
void ConvertRTCDate(date_time_t*dst, ext_rtc_t*src);
int16_t findEventCode(uint8_t event_code_bit, uint8_t event_type);



/* Action Method */
Unsigned8 R_OBIS_ActionObjectMethod(
	st_Cosem_Method_Desc *cosem_method_desc,/* [In] COSEM Method Descriptor */
	service_type_t service_type,			/* [In] Service type */
	Unsigned16 child_id,					/* [In] Child ID in the child object table */
	Unsigned8 *p_out_buf,					/* [Out] Encoded output data */
	Unsigned16 *p_out_len,					/* [Out] Encoded length */
	Unsigned8 *p_data,						/* [In] Data from client */
	Unsigned16 data_len,					/* [In] Data length */
	Unsigned8 *block_transfer				/* [Out] Indicate whether block transfer is used or not */
);

/* Get Attribute */
Unsigned8 R_OBIS_GetObjectAttr(
	st_Cosem_Attr_Desc *cosem_attr_desc,	/* [In] COSEM Obj Descriptor */
	service_type_t service_type,			/* [In] Service type */
	Unsigned16 child_id,					/* [In] Child ID in the child object table */
	Unsigned8 *p_out_buf,					/* [Out] Encoded output data */
	Unsigned16 *p_out_len,					/* [Out] Encoded length */
	Unsigned8 *block_transfer,				/* [Out] Indicate whether block transfer is used or not */
	Unsigned8 *p_data						/* [In] Data from client */
);

/* Set Attribute */
Unsigned8 R_OBIS_SetObjectAttr(
	st_Cosem_Attr_Desc *cosem_attr_desc,	/* [In] COSEM Obj Descriptor */
	service_type_t service_type,			/* [In] Service type */
	Unsigned16 child_id,					/* [In] Child ID in the child object table */
	Unsigned8 *p_data,						/* [In] Data from client */
	Unsigned16 data_len,					/* [In] Data length */
	Unsigned8 block_transfer				/* [In] Indicate whether block transfer is used or not */
);

/* Get Attribute from EEPROM */
Unsigned8 R_OBIS_E2PRGetData(
	attr_access_t		*p_attr_access,	/* Access method option			*/
	attr_type_t 		attr_type,		/* Choice selection				*/
	Integer16			attr_size,		/* Buffer of value				*/
	Unsigned8			*p_out_buf,		/* Data pointer for get			*/
	Unsigned16			*p_out_len		/* Data length pointer for get	*/
);

/* Set Attribute to EEPROM*/
Unsigned8 R_OBIS_E2PRSetData(
	attr_access_t		*p_attr_access,	/* Access method option			*/
	attr_type_t 		attr_type,		/* Choice selection				*/
	Integer16			attr_size,		/* Buffer of value				*/
	Unsigned8			*pdata			/* Data from COSEM Appl layer	*/
);

/* Get 1 block of buffer */
Unsigned8 R_OBIS_BufferBlockGet(
	void				*buffer,		/* [In] pointer to buffer list	*/
	attr_access_t		*p_attr_access,	/* [In] Access method option	*/
	Unsigned16			block_index,	/* [In] Block index in buffer	*/
	Unsigned16			block_size,		/* [In] Block size in byte		*/
	Unsigned8			*p_out_buf,		/* [Out] Data pointer for get	*/
	Unsigned16			*p_out_len		/* [Out] length pointer for get	*/
);

/* Set to 1 block of buffer */
Unsigned8 R_OBIS_BufferBlockSet(
	void				*buffer,		/* [In] pointer to buffer list	 */
	attr_access_t		*p_attr_access,	/* [In] Access method option	 */
	Unsigned16			block_index,	/* [In] Block index in buffer	 */
	Unsigned16			block_size,		/* [In] Block size in byte		 */
	Unsigned8			*p_in_buf,		/* [In] Data pointer for set	 */
	Unsigned16			*p_in_len		/* [Out] length pointer after set */
);

/* Class07 - Profile Generic, buffer encode */
Unsigned8 R_OBIS_Class07_BufferScan (
	buffer_t            *buf,
	Unsigned8           selective_access,
	Unsigned16          child_index,
	Unsigned8           *p_selector_buffer
);
Unsigned8 R_OBIS_Class07_BufferFilter (
	buffer_t            *buf,
	Unsigned16          child_index,
	Unsigned16          request_entries_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

/* Class26 - Utility Table, buffer encode */
Unsigned8 R_OBIS_Class26_BufferScan (
	buffer_t            *buf,
	Unsigned8           selective_access,
	Unsigned16          child_index,
	Unsigned8           *p_selector_buffer
);
Unsigned8 R_OBIS_Class26_BufferFilter (
	buffer_t            *buf,
	Unsigned16          child_index,
	Unsigned16          request_entries_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

/* Buffer Scan By User Func */
Unsigned8 R_OBIS_BufferScanByUserFunc (
	buffer_t            *buf,
	Unsigned8           selective_access,
	Unsigned16          master_index,
	Unsigned16          child_index,
	Unsigned8           *p_selector_buffer
);

/* Buffer Filter By User Func */
Unsigned8 R_OBIS_BufferFilterByUserFunc (
	buffer_t            *buf,
	Unsigned16          master_index,
	Unsigned16          child_index,
	Unsigned16          request_entries_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
);

/* Ack of async restart */
void R_OBIS_DataAsyncServiceRestart(void);
extern void set_passive_tod_zone(void);

#endif /* _R_DLMS_DATA_H */

