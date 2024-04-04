/// File Name    : r_dlms_data.c
// Drivers  
#include "typedef.h"
#include "emeter3_structs.h"

// DLMS  
#include "r_dlms_data.h"				// DLMS Data Definitions  
#include "r_dlms_obis.h"				// DLMS OBIS Definitions  
#include "r_dlms_obis_ic.h"				// DLMS OBIS IC Definitions  
#include "DLMS_User.h"					// DLMS User Definitions  
#if SHA1_ENABLE
#include "r_sha.h"					// SHA-1 Library  
#endif

#if MD5_ENABLE
#include "r_md5.h"					// MD5 Library  
#endif

#if AES_ENABLE
// HLS  
#include "r_dlms_data_hls.h"				// DLMS Data High level security Definitions  
#endif

// Standard Lib  
#include <string.h>						// String Standard Lib  

//R ENESAS DRIVER, TO BE REMOVE 
#include "eeprom.h"
////#include "r_cg_userdefine.h"
#include "r_dlms_typedef.h"				// DLMS Type Definitions  
#include "em_core.h"

#include "memoryOps.h"
#include "e2rom.h"
#include "survey.h"
#include "tampers.h"
#include "utilities.h"
//Macro definitions

//Typedef definitions

extern uint32_t g_Class07_Event_EntriesInUse[];
extern uint32_t g_Class07_Event_MaxEntries[];
//Imported global variables and functions (from other files)
void generate_bill(uint8_t dat, uint8_t mth, uint8_t yr, uint8_t hrs, uint8_t mnts, uint8_t secs, uint8_t bill_type);
int8_t is_rtc_good(ext_rtc_t p_rtc);

// * Imported from r_dlms_data_ic.c
// * For data access
// *
// * All of following variables are linked into each specified-child-table.
// * Refer to each child-table in r_dlms_data_ic.c
// Class ID = 07 - Profile generic class  
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)

//Unsigned32  range_end_entry, range_start_entry;	// these are now local variables
// Clock (ATTR_TYPE_OCTET_STRING, 12bytes)  
void find_num_between_lp_entries(date_time_t* from_date_time, date_time_t* to_date_time, uint32_t* range_start_entry, uint32_t* range_end_entry);
void find_num_between_dlp_entries(date_time_t* from_date_time, date_time_t* to_date_time, uint32_t* range_start_entry, uint32_t* range_end_entry);

//void find_num_between_lp_entries(void);
//void find_num_between_dlp_entries(void);
extern const Unsigned16 	g_Class07_BlockloadCaptureObjectsLength;
extern const Unsigned16 	g_Class07_BillingCaptureObjectsLength;
extern const Unsigned16 	g_Class07_InstCaptureObjectsLength;
//date_time_t	from_date_time;// now a local variable
//date_time_t	to_date_time;	// now a local variable

#endif
// Class ID = 64 - Security setup class  
#if (defined(USED_CLASS_64) && USED_CLASS_64 == 1)
extern const class64_child_record_t g_ChildTableClass64[];  // Security setup reference list    
extern const Unsigned16 g_ChildTableLengthClass64;  		// Security setup reference length  
#endif
#if (defined(USED_CLASS_01) && USED_CLASS_01 == 1)
// Block load profile parameter  
extern Unsigned16		g_Class01_EventCode[7];// = {69,101,121,201,251,0,0};
#endif
//Exported global variables and functions (to be accessed by other files)

//Private global variables and functions
// Class ID = 07 - Profile generic class  
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
	extern Unsigned32					g_Class07_Blockload_EntriesInUse;
	extern Unsigned16					g_Class07_Blockload_CurrentEntry;
	extern Unsigned32					g_Class07_Dailyload_EntriesInUse;
	extern Unsigned16					g_Class07_Dailyload_CurrentEntry;
	extern Unsigned32					g_Class07_Billing_EntriesInUse;
	extern Unsigned16					g_Class07_Billing_CurrentEntry;

//extern Unsigned16					g_Class07_Event_CurrentEntry[];
//extern Unsigned32					g_Class07_Event_EntriesInUse[];

const scaler_unit_t		g_Class07_InstScalerBuffer[] =
{
	//{SCALAR_NULL, PHY_UNIT_NONE				},	// Clock  
	{SCALAR_AMPERE, PHY_UNIT_AMPERE				},	// Ir current   
	{SCALAR_AMPERE, PHY_UNIT_AMPERE				},	// Iy current  
	{SCALAR_AMPERE, PHY_UNIT_AMPERE				},	// Ib current  
	{SCALAR_VOLTAGE, PHY_UNIT_VOLT				},	// Vr Voltage   
	{SCALAR_VOLTAGE, PHY_UNIT_VOLT				},	// Vy Voltage   
	{SCALAR_VOLTAGE, PHY_UNIT_VOLT				},	// Vb Voltage   
	{SCALAR_PF, PHY_UNIT_NONE				},	// Signed Power Factor  - R
	{SCALAR_PF, PHY_UNIT_NONE				},	// Signed Power Factor  - Y
	{SCALAR_PF, PHY_UNIT_NONE				},	// Signed Power Factor  - B
	{SCALAR_PF, PHY_UNIT_NONE				},	// Signed Power Factor  - Three phase
	{SCALAR_HERTZ, PHY_UNIT_HERTZ				},	// Frequency  
	{SCALAR_VA, PHY_UNIT_VOLT_AMPERE			},	// Apparent Power  
	{SCALAR_WATT, PHY_UNIT_WATT				},	// Signed Active Power    
	{SCALAR_WATT, PHY_UNIT_VAR				},	// Signed Reactive Power    
	{SCALAR_NULL, PHY_UNIT_MIN				},	// Cumulative power ON duration in minutes  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR				},	// Cumulative Energy - kWh  
	{SCALAR_WH, PHY_UNIT_VAR_HOUR				},	// Cumulative Energy - kvarh(Lag) 
	{SCALAR_VAH, PHY_UNIT_VAR_HOUR				},	// Cumulative Energy - kvarh(Lead) 
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR			},	// Cumulative Energy - kVAh 
	{SCALAR_MD, PHY_UNIT_WATT				},	// Maximum Demand , kW  
	//{SCALAR_NULL, PHY_UNIT_NONE				},	// MD kW time  
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE			},	// Maximum Demand , kVA  
	//{SCALAR_NULL, PHY_UNIT_NONE				},	// MD kVA time  
#ifdef BIDIR_METER	
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy – kWh export 
#endif
};

Unsigned8			g_Class07_Obj0_Capture_Enable = 1;

class07_blockload_entry_t	g_Class07_BlockLoadBuffer;

const scaler_unit_t		g_Class07_BlockloadScalerBuffer[] =
{
	//{SCALAR_NULL, PHY_UNIT_NONE				},	// RTC time  
#ifdef SURVEY_USES_AVERAGE_DATA	
	{SCALAR_AMPERE, PHY_UNIT_AMPERE				},	// Phase current   
	{SCALAR_AMPERE, PHY_UNIT_AMPERE				},	// Neutral current  
	{SCALAR_AMPERE, PHY_UNIT_AMPERE				},	// Neutral current  
	{SCALAR_NULL, PHY_UNIT_VOLT				},	// Voltage (no decimals)  
	{SCALAR_NULL, PHY_UNIT_VOLT				},	// Voltage (no decimals)
	{SCALAR_NULL, PHY_UNIT_VOLT				},	// Voltage (no decimals)
#endif	
	{SCALAR_BL_WH, PHY_UNIT_WATT_HOUR				},	// Cumulative Energy - kWh  
#ifdef SURVEY_USES_REACTIVE_DATA
	{SCALAR_BL_WH, PHY_UNIT_VAR_HOUR				},	// Cumulative Energy - kvarh(Lag) 
	{SCALAR_BL_WH, PHY_UNIT_VAR_HOUR				},	// Cumulative Energy - kvarh(Lead) 
#endif
	{SCALAR_BL_WH, PHY_UNIT_VOLT_AMPERE_HOUR			},	// Cumulative Energy - kVAh 
#ifdef SURVEY_USES_EXPORT_DATA	
	{SCALAR_BL_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy - kWh export 
#endif
#ifdef SURVEY_USES_AVERAGE_PF
	{SCALAR_AMPERE, PHY_UNIT_NONE			},	// Block Power Factor R-phase  - Scalar Ampere is used since a value of -2 is required
	{SCALAR_AMPERE, PHY_UNIT_NONE			},	// Block Power Factor Y-phase  - Scalar Ampere is used since a value of -2 is required
	{SCALAR_AMPERE, PHY_UNIT_NONE			},	// Block Power Factor B-phase  - Scalar Ampere is used since a value of -2 is required
#endif
#ifdef SURVEY_USES_FREQ_DATA	// SCALAR_HERTZ
	{SCALAR_APF, PHY_UNIT_HERTZ			},	// Frequency	
#endif
#ifdef SURVEY_USES_DEMAND_DATA
	{SCALAR_VA, PHY_UNIT_VOLT_AMPERE			},	// Demand kva
	{SCALAR_VA, PHY_UNIT_WATT			},	// Demand kw
#endif
};

class07_dailyload_entry_t	g_Class07_DailyLoadBuffer;

#ifdef SMALL_DAILY_LOG
const scaler_unit_t			g_Class07_DailyloadScalerBuffer[] =
{
	//{SCALAR_NULL, PHY_UNIT_NONE		},	// RTC time  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR		},	// Cumulative Energy – kWh  
	#ifdef BIDIR_METER	
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy – kWh export 
	#else
	{SCALAR_WH, PHY_UNIT_VOLT_AMPERE_HOUR			},	// Cumulative Energy - kVAh 
	#endif
};
#else
	#ifdef DAILY_POWER_ON_LOGGING
	const scaler_unit_t			g_Class07_DailyloadScalerBuffer[] =
	{
		{SCALAR_WH, PHY_UNIT_WATT_HOUR		},	// Cumulative Energy – kWh  
		{SCALAR_WH, PHY_UNIT_VOLT_AMPERE_HOUR			},	// Cumulative Energy - kVAh 
		{SCALAR_NULL, PHY_UNIT_NONE		},	// Daily Power On Minutes  
	};
	#else
	const scaler_unit_t			g_Class07_DailyloadScalerBuffer[] =
	{
		{SCALAR_WH, PHY_UNIT_WATT_HOUR		},	// Cumulative Energy – kWh  
		{SCALAR_WH, PHY_UNIT_VOLT_AMPERE_HOUR			},	// Cumulative Energy - kVAh 
		{SCALAR_WH, PHY_UNIT_VAR_HOUR		},	// Cumulative Energy – kVarh_lag  
		{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy - kVArh_lead 
	};
	#endif
#endif

class07_billing_entry_t		g_Class07_BillingBuffer;

const scaler_unit_t			g_Class07_BillingScalerBuffer[] =
{// all date time elements are not described
	{SCALAR_APF, PHY_UNIT_NONE			},	// Average power factor for billing  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy – kWh  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ1  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ2  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ3  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ4  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ5  
#if (defined(TARIFFS) && TARIFFS == 8)
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ6  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ7  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR	},	// Cumulative Energy kWh TZ8  
#endif
	{SCALAR_WH, PHY_UNIT_VAR_HOUR	},	// Cumulative Energy kvarh (Lag)  
	{SCALAR_WH, PHY_UNIT_VAR_HOUR	},	// Cumulative Energy kvarh (Lead))  
	
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy – kVAh  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ1  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ2  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ3  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ4  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ5
#if (defined(TARIFFS) && TARIFFS == 8)
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ6  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ7  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ8
#endif
	
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW  
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ1 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ2 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ3 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ4 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ5 
#if (defined(TARIFFS) && TARIFFS == 8)
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ6 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ7 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ8 
#endif
	
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA  
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ1 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ2 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ3 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ4 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ5 
#if (defined(TARIFFS) && TARIFFS == 8)
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ6 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ7 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ8 
#endif
	
	{SCALAR_NULL, PHY_UNIT_MIN		},	// Billing power ON duration in minutes  

#ifdef BILLING_HAS_TAMPER_COUNT
	{SCALAR_NULL, PHY_UNIT_NONE		},	// cumulative tamper count  
#endif
	
#ifdef TOD_HAS_KVARH_LAG
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ1
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ2
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ3
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ4
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ5
#if (defined(TARIFFS) && TARIFFS == 8)
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ6
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ7
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh TZ8
#endif
#endif

#ifdef BIDIR_METER
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy – kWh -export   
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy kWh TZ1-export   
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy kWh TZ2 -export  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy kWh TZ3 -export  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy kWh TZ4 -export  
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy kWh TZ5 -export  

	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh (Lag)  -export 
	{SCALAR_WH, PHY_UNIT_VAR_HOUR			},	// Cumulative Energy kvarh (Lead)) -export  

	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy – kVAh  -export 
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ1 -export  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ2 -export  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ3 -export  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ4 -export  
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh TZ5 -export  

	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW  -export 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ1 -export 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ2 -export 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ3 -export 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ4 -export 
	{SCALAR_MD, PHY_UNIT_WATT			},	// MD kW for TZ5 -export 
	
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA  -export 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ1 -export 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ2 -export 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ3 -export 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ4 -export 
	{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	},	// MD kVA TZ5 -export 
	
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy – kwh_nonSolar_export   
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy – kwh_Solar_import   
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy – kwh_Solar_export   
#endif

#ifdef SPLIT_ENERGIES_REQUIRED
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy – kWh lag 
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy kWh lead
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy – kVAh lag 
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR		},	// Cumulative Energy kVAh lead
#endif	
};

class07_event_entry_t	g_Class07_EventBuffer;//[CLASS07_EVENT_MAX_PROFILE][CLASS07_EVENT_MAX_ENTRY];
extern Unsigned16				g_Class07_Event_CurrentEntry[] ;
extern Unsigned32				g_Class07_Event_EntriesInUse[];

const scaler_unit_t		g_Class07_Event0ScalerBuffer[] =
{
	//{SCALAR_NULL, PHY_UNIT_NONE				},	// Clock  
	//{SCALAR_NULL, PHY_UNIT_NONE				},	// Event code   
	{SCALAR_AMPERE, PHY_UNIT_AMPERE			},	// Current  IR 
	{SCALAR_AMPERE, PHY_UNIT_AMPERE			},	// Current  IY 
	{SCALAR_AMPERE, PHY_UNIT_AMPERE			},	// Current  IB 
	{SCALAR_NULL, PHY_UNIT_VOLT			},	// Voltage  IR 
	{SCALAR_NULL, PHY_UNIT_VOLT			},	// Voltage  IY 
	{SCALAR_NULL, PHY_UNIT_VOLT			},	// Voltage  IB 
	{SCALAR_AMPERE, PHY_UNIT_NONE			},	// Signed Power Factor R-phase 
	{SCALAR_AMPERE, PHY_UNIT_NONE			},	// Signed Power Factor Y-phase 
	{SCALAR_AMPERE, PHY_UNIT_NONE			},	// Signed Power Factor B-phase 
	{SCALAR_WH, PHY_UNIT_WATT_HOUR			},	// Cumulative Energy - kWh 
#ifdef EVENTDATA_RECORDS_KVAH	
	{SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR			},	// Cumulative Energy - kVAh  
#endif	
};

#endif


void ConvertRTCDate(date_time_t*dst, ext_rtc_t*src)
{
	int16_t year = 2000+src->year;
	dst->year_high=year>>8;
	dst->year_low=year&0xff;
	dst->month=src->month;
	dst->day_of_month=src->date;
	dst->day_of_week=src->day;
	dst->hour=src->hour;
	dst->minute=src->minute;
	dst->second=src->second;
	dst->hundredths=(uint8_t)TIME_HUNDREDTHS_NOT_SPECIFIED;	// not available
	dst->deviation_high=128;
	dst->deviation_low=0;	// together the two mean NOT SPECIFIED 
	dst->clock_status=(uint8_t)NOT_SPECIFIED;
	if(dst->day_of_week==0)
		dst->day_of_week=7;
}

void ConvertBackToRTC(ext_rtc_t*dst, date_time_t*src)
{
	int16_t year = ((src->year_high)*256 + src->year_low);
	if(year>2000)
		dst->year = year - 2000;
	else
		dst->year = 0;
		
	dst->month = src->month;
	dst->date = src->day_of_month;
	dst->day=src->day_of_week;
	dst->hour = src->hour;
	dst->minute = src->minute;
	dst->second =src->second;
}

int8_t is_rtc_good(ext_rtc_t p_rtc)
{
	int8_t result;	// 1 = good, 0 - bad
	
	if((p_rtc.year<19)||(p_rtc.month>12)||(p_rtc.date>31)||(p_rtc.hour>23)||(p_rtc.minute>59)||(p_rtc.second>59))
		result=0;	// bad
	else
		result=1;	// good
		
	return result;
}

/*
uint32_t findAddress(uint16_t index, uint16_t recsPerDay)
{
	uint32_t addr;
	addr = SURVEY_BASE_ADDRESS + (index/recsPerDay)*SURVEY_PTR_INC;	// got the page address
	addr += (index%recsPerDay)*sizeof(surveydata_t);
	return addr;
}
*/
void getRecordDateTime(int16_t index, date_time_t*dst);
void getDailyLogRecordDateTime(int16_t index, date_time_t*dst);

void getRecordDateTime(int16_t index, date_time_t*dst)
{
	uint32_t addr;
	ext_rtc_t record_rtc_time;
	timeinfo_t shortTime;
	int16_t year;
	
	index = index%NO_OF_SURVEY_RECORDS; // This is to ensure that we remain within the buffer
	addr=(int32_t)SURVEY_BASE_ADDRESS + (int32_t)index*(int32_t)SURVEY_PTR_INC;
	EPR_Read(addr, (uint8_t*)&shortTime, sizeof(timeinfo_t));	// we will get 8 bytes
//	ConvertShortDate_RTC(&record_rtc_time,&shortTime);
	record_rtc_time.year=shortTime.year;
	record_rtc_time.month=shortTime.month;
	record_rtc_time.date = shortTime.date;
	record_rtc_time.hour=shortTime.hour;
	record_rtc_time.minute=shortTime.minute;
	record_rtc_time.day = shortTime.day;
	record_rtc_time.second=0;
	
//	ConvertRTCDate(&record_date_time,&record_rtc_time);
	year = 2000+record_rtc_time.year;
	dst->year_high=year>>8;
	dst->year_low=year&0xff;
	dst->month=record_rtc_time.month;
	dst->day_of_month=record_rtc_time.date;
	dst->day_of_week=record_rtc_time.day;
	dst->hour=record_rtc_time.hour;
	dst->minute=record_rtc_time.minute;
	dst->second=record_rtc_time.second;
	dst->hundredths=(uint8_t)TIME_HUNDREDTHS_NOT_SPECIFIED;	// not available
	dst->deviation_high=128;
	dst->deviation_low=0;	// together the two mean NOT SPECIFIED 
	dst->clock_status=(uint8_t)NOT_SPECIFIED;
	if(dst->day_of_week==0)
		dst->day_of_week=7;
	dst->second = 0xff;
	dst->hundredths = 0xff;
}


void getDailyLogRecordDateTime(int16_t index, date_time_t*dst)
{
	uint32_t addr;
	ext_rtc_t record_rtc_time;
	int16_t year;
	
	index = index%NO_OF_DAILY_LOG_DATA; // This is to ensure that we remain within the buffer
	addr=(int32_t)DAILYLOG_BASE_ADDRESS + (int32_t)index*(int32_t)DAILY_LOG_PTR_INC;
	EPR_Read(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
	record_rtc_time.second=0;
	
//	ConvertRTCDate(&record_date_time,&record_rtc_time);
	year = 2000+record_rtc_time.year;
	dst->year_high=year>>8;
	dst->year_low=year&0xff;
	dst->month=record_rtc_time.month;
	dst->day_of_month=record_rtc_time.date;
	dst->day_of_week=record_rtc_time.day;
	dst->hour=record_rtc_time.hour;
	dst->minute=record_rtc_time.minute;
	dst->second=record_rtc_time.second;
	dst->hundredths=(uint8_t)TIME_HUNDREDTHS_NOT_SPECIFIED;	// not available
	dst->deviation_high=128;
	dst->deviation_low=0;	// together the two mean NOT SPECIFIED 
	dst->clock_status=(uint8_t)NOT_SPECIFIED;
	if(dst->day_of_week==0)
		dst->day_of_week=7;
	dst->second = 0xff;
	dst->hundredths = 0xff;
}



/*
int8_t test_function(date_time_t* from_date_time, date_time_t* to_date_time);

int8_t test_function(date_time_t* from_date_time, date_time_t* to_date_time)
{
	date_time_t record_date_time;
	int8_t match_result;
	
	match_result = R_OBIS_CompareDateTime((Unsigned8 *)from_date_time, (Unsigned8 *)to_date_time);
	
	if(match_result>0)
	{// from_date_time is > to_date_time - we could also swap the two entries
		record_date_time = *from_date_time;
		*from_date_time = *to_date_time;
		*to_date_time = record_date_time;
	}
	return match_result;
}
*/

void find_num_between_lp_entries(date_time_t* from_date_time, date_time_t* to_date_time, uint32_t* range_start_entry, uint32_t* range_end_entry)
{
// This function must return the range_start_entry and range_end_entry value between 0 and (NO_OF_SURVEY_RECORDS-1)	
// which must be referred to the oldest record which would be 0, if buffer is not full and the first_index if the buffer has overflowed
	date_time_t record_date_time;
//	ext_rtc_t record_rtc_time;
//	timeinfo_t shortTime;
	
	uint32_t addr;
	int16_t search_index; 
	int16_t first_index;	// oldest record
	int16_t last_index;	// the last_index is the newest record
	int16_t low_index;
	int16_t high_index;
	int16_t previous_search_index;
	int8_t match_result,done;
	
//To find the index of the entry which matches the start time, start from the oldest record
//The ctldata.survey_wrt_ptr always points to the oldest record, if there is overflow
//If there is no overflow, then the oldest record index is 0

	uint16_t no_of_records_per_day =	1440/Common_Data.survey_interval;

	if(g_Class07_Blockload_EntriesInUse==NO_OF_SURVEY_RECORDS)
	{// here we have an overflow condition
		first_index = (ctldata.survey_wrt_ptr - (int32_t)SURVEY_BASE_ADDRESS)/(int32_t)SURVEY_PTR_INC;	// oldest record
// the following formula ensures that last_index will always be greater than search_index
// we must take care to finally return / use the last_index as (last_index%NO_OF_SURVEY_RECORDS)
		last_index = (first_index-1)+ NO_OF_SURVEY_RECORDS;
	}
	else
	{// here there is no overflow
		first_index=0;	// oldest record
		last_index = g_Class07_Blockload_EntriesInUse - 1;	// newest record index
	}
	
// we should make sure that valid from_date_time and to_date_time have been provided
// towards this end - to_date_time must be >=from_date_time
// compare 	record_date_time with from_date_time
	match_result = R_OBIS_CompareDateTime((Unsigned8 *)from_date_time, (Unsigned8 *)to_date_time);
	
	if(match_result>0)
	{// from_date_time is > to_date_time - we should swap the two entries
//  - NO MORE SWAPPING	
//		record_date_time = *from_date_time;
//		*from_date_time = *to_date_time;
//		*to_date_time = record_date_time;

// the following two lines cause the last entry to be sent
//		*range_start_entry=last_index-first_index+1;
//		*range_end_entry=last_index-first_index+1;

// the following two lines sends no data
		*range_start_entry=0;
		*range_end_entry=0;
		return;
	}
		
// we should also determine if from_date_time lies within our data
// from_date_time should be greater than the oldest record time, and less than newest record time

	done=0;

	getRecordDateTime(first_index,&record_date_time);
	match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
	if(match_result>0)
	{// start_date>from_date i.e from_date<start_date 
		*range_start_entry=first_index;	// should be (first_index-first_index) - must be done before exiting
		search_index=first_index;
		
	// check if the to_date_time<start date
//	getRecordDateTime(first_index,&record_date_time);	// we already have read this above
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
		if(match_result>0)
		{//to_date_time<start_time
			*range_start_entry=0;
			*range_end_entry=0;
			return;
		}
	// here from_date<start_date AND to_date_time>=start_time		
	}
	else
	{// here the from_date_time is >= start_time 
	// we should verify that the from_date_time<=end_time
		getRecordDateTime(last_index,&record_date_time);
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
		if(match_result<0)
		{// from_date_time > end time
		// the following two lines sends no data
			*range_start_entry=0;
			*range_end_entry=0;
			return;
		}
		else
		{// here the from_date_time lies within the data available
		// we have already checked the first_index and the last_index
		// let us do a binary search now
		
			low_index = first_index;
			high_index = last_index;
			previous_search_index = -1;
			
			done=0;
			do
			{
				search_index = (low_index+high_index)/2;
				if(search_index==previous_search_index)
				{
					done=1;
					// we should run one more check on search_index+1
					// when we come here then the record_date_time is < from_date_time 
					// before calling it a day - if the next record is also <= from_date_time then it would be a better result
					// also if the next record is > from_date_time
					getRecordDateTime((search_index+1),&record_date_time);
					match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
					if(match_result<=0)
						search_index=search_index+1;	// this may be a better result
				}
				else
				{
					getRecordDateTime(search_index,&record_date_time);
					match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
					if(match_result==0)
					{
						done=1;
					}
					else
					{
						if(match_result>0)
						{//record_date_time is > from_date_time - from_date_time is behind the search_index
							high_index=search_index;
						}
						else
						{// here match_result is <0
						// record_date_time is < from_date_time - from_date_time is ahead of the search_index
							low_index=search_index;
						}
					}
					previous_search_index = search_index;
				}
			}
			while(done==0);
			
// after exiting thie while we will have the start_entry
			*range_start_entry=search_index;
		}
	}
// at this place the range_start_entry has been determined
// now we should look for the end_entry - which should lie between the search_index and the last_index

	low_index = search_index;
	high_index = last_index;
	previous_search_index = -1;
	
	done=0;
		
	do
	{
		search_index = (low_index+high_index)/2;
		if(search_index==previous_search_index)
		{
			done=1;
			// we should run one more check on search_index+1
			// when we come here then the record_date_time is < to_date_time 
			// before calling it a day - if the next record is also <= from_date_time then it would be a better result
			// also if the next record is > from_date_time
			getRecordDateTime((search_index+1),&record_date_time);
			match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
			if(match_result<=0)
				search_index=search_index+1;	// this may be a better result
		}
		else
		{
			getRecordDateTime(search_index,&record_date_time);
			match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
			if(match_result==0)
			{
				done=1;
			}
			else
			{
				if(match_result>0)
				{//record_date_time is > to_date_time - to_date_time is behind the search_index
					high_index=search_index;
				}
				else
				{// here match_result is <0
				// record_date_time is < to_date_time - to_date_time is ahead of the search_index
					low_index=search_index;
				}
			}
			previous_search_index = search_index;
		}
	}
	while(done==0);
		
// upon exiting this loop, we will have the range_end_entry
	*range_end_entry = search_index;	

// Convert the range_start_entry and range_end_entry between 0 and (NO_OF_SURVEY_RECORDS-1) 
	*range_start_entry = *range_start_entry - first_index + 1;
	*range_end_entry = *range_end_entry - first_index + 1;
}

void find_num_between_dlp_entries(date_time_t* from_date_time, date_time_t* to_date_time, uint32_t* range_start_entry, uint32_t* range_end_entry)
{
// This function must return the range_start_entry and range_end_entry value between 0 and (NO_OF_SURVEY_RECORDS-1)	
// which must be referred to the oldest record which would be 0, if buffer is not full and the first_index if the buffer has overflowed
	date_time_t record_date_time;
//	ext_rtc_t record_rtc_time;
//	timeinfo_t shortTime;
	
	uint32_t addr;
	int16_t search_index; 
	int16_t first_index;	// oldest record
	int16_t last_index;	// the last_index is the newest record
	int16_t low_index;
	int16_t high_index;
	int16_t previous_search_index;
	int8_t match_result,done;
	
//To find the index of the entry which matches the start time, start from the oldest record
//The ctldata.survey_wrt_ptr always points to the oldest record, if there is overflow
//If there is no overflow, then the oldest record index is 0

	uint16_t no_of_records_per_day =	1;	// there is only one record per day

	if(g_Class07_Dailyload_EntriesInUse==NO_OF_DAILY_LOG_DATA)
	{// here we have an overflow condition
		first_index = (ctldata.daily_log_wrt_ptr - (int32_t)DAILYLOG_BASE_ADDRESS)/(int32_t)DAILY_LOG_PTR_INC;	// oldest record
// the following formula ensures that last_index will always be greater than search_index
// we must take care to finally return / use the last_index as (last_index%NO_OF_SURVEY_RECORDS)
		last_index = (first_index-1)+ NO_OF_DAILY_LOG_DATA;
	}
	else
	{// here there is no overflow
		first_index=0;	// oldest record
		last_index = g_Class07_Dailyload_EntriesInUse - 1;	// newest record index
	}
	
// we should make sure that valid from_date_time and to_date_time have been provided
// towards this end - to_date_time must be >=from_date_time
// compare 	record_date_time with from_date_time
	match_result = R_OBIS_CompareDateTime((Unsigned8 *)from_date_time, (Unsigned8 *)to_date_time);
	
	if(match_result>0)
	{// from_date_time is > to_date_time - we should return no data
		*range_start_entry=0;
		*range_end_entry=0;
		return;
	}
		
// we should also determine if from_date_time lies within our data
// from_date_time should be greater than the oldest record time, and less than newest record time

	done=0;

	getDailyLogRecordDateTime(first_index,&record_date_time);
	match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
	if(match_result>0)
	{// start_date>from_date i.e from_date<start_date 
		*range_start_entry=first_index;	// should be (first_index-first_index) - must be done before exiting
		search_index=first_index;
		
	// check if the to_date_time<start date
//	getDailyLogRecordDateTime(first_index,&record_date_time);	// we already have read this above
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
		if(match_result>0)
		{//to_date_time<start_time
			*range_start_entry=0;
			*range_end_entry=0;
			return;
		}
	// here from_date<start_date AND to_date_time>=start_time		
	}
	else
	{// here the from_date_time is >= start_time 
	// we should verify that the from_date_time<=end_time
		getDailyLogRecordDateTime(last_index,&record_date_time);
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
		if(match_result<0)
		{// from_date_time > end time
		// the following two lines sends no data
			*range_start_entry=0;
			*range_end_entry=0;
			return;
		}
		else
		{// here the from_date_time lies within the data available
		// we have already checked the first_index and the last_index
		// let us do a binary search now
		
			low_index = first_index;
			high_index = last_index;
			previous_search_index = -1;
			
			done=0;
			do
			{
				search_index = (low_index+high_index)/2;
				if(search_index==previous_search_index)
				{
					done=1;
					// we should run one more check on search_index+1
					// when we come here then the record_date_time is < from_date_time 
					// before calling it a day - if the next record is also <= from_date_time then it would be a better result
					// also if the next record is > from_date_time
					getDailyLogRecordDateTime((search_index+1),&record_date_time);
					match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
					if(match_result<=0)
						search_index=search_index+1;	// this may be a better result
				}
				else
				{
					getDailyLogRecordDateTime(search_index,&record_date_time);
					match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
					if(match_result==0)
					{
						done=1;
					}
					else
					{
						if(match_result>0)
						{//record_date_time is > from_date_time - from_date_time is behind the search_index
							high_index=search_index;
						}
						else
						{// here match_result is <0
						// record_date_time is < from_date_time - from_date_time is ahead of the search_index
							low_index=search_index;
						}
					}
					previous_search_index = search_index;
				}
			}
			while(done==0);
			
// after exiting thie while we will have the start_entry
			*range_start_entry=search_index;
		}
	}
// at this place the range_start_entry has been determined
// now we should look for the end_entry - which should lie between the search_index and the last_index

	low_index = search_index;
	high_index = last_index;
	previous_search_index = -1;
	
	done=0;
		
	do
	{
		search_index = (low_index+high_index)/2;
		if(search_index==previous_search_index)
		{
			done=1;
			// we should run one more check on search_index+1
			// when we come here then the record_date_time is < to_date_time 
			// before calling it a day - if the next record is also <= from_date_time then it would be a better result
			// also if the next record is > from_date_time
			getDailyLogRecordDateTime((search_index+1),&record_date_time);
			match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
			if(match_result<=0)
				search_index=search_index+1;	// this may be a better result
		}
		else
		{
			getDailyLogRecordDateTime(search_index,&record_date_time);
			match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
			if(match_result==0)
			{
				done=1;
			}
			else
			{
				if(match_result>0)
				{//record_date_time is > to_date_time - to_date_time is behind the search_index
					high_index=search_index;
				}
				else
				{// here match_result is <0
				// record_date_time is < to_date_time - to_date_time is ahead of the search_index
					low_index=search_index;
				}
			}
			previous_search_index = search_index;
		}
	}
	while(done==0);
		
// upon exiting this loop, we will have the range_end_entry
	*range_end_entry = search_index;	

// Convert the range_start_entry and range_end_entry between 0 and (NO_OF_SURVEY_RECORDS-1) 
	*range_start_entry = *range_start_entry - first_index + 1;
	*range_end_entry = *range_end_entry - first_index + 1;
}
/*
void find_num_between_dlp_entries(date_time_t* from_date_time, date_time_t* to_date_time, uint32_t* range_start_entry, uint32_t* range_end_entry)
{
	date_time_t record_date_time;
	ext_rtc_t record_rtc_time;

	uint32_t addr;
	int16_t search_index, initial_search_index;
	
	int8_t match_result,loop_count,done;
	
//To find the index of the entry which matches the start time, start from the oldest record
//The ctldata.daily_log_wrt_ptr always points to the oldest record, if there is overflow
//If there is no overflow, then the oldest record index is 0

	if(g_Class07_Dailyload_EntriesInUse==NO_OF_DAILY_LOG_DATA)
	{// here we have an overflow condition
		search_index = (ctldata.daily_log_wrt_ptr - (int32_t)DAILYLOG_BASE_ADDRESS)/(int32_t)DAILY_LOG_PTR_INC;	// oldest record
	}
	else
		search_index=0;	// oldest record
		
	initial_search_index=search_index;
	
// Search in the increasing time direction	
	addr=(int32_t)DAILYLOG_BASE_ADDRESS + (int32_t)search_index*(int32_t)DAILY_LOG_PTR_INC;

	match_result = 0;
	loop_count=0;	// keeps count of number of days
	done=0;
	
	do
	{
//		EPR_Read_Test(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
		EPR_Read(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
		ConvertRTCDate(&record_date_time,&record_rtc_time);
		record_date_time.second = 0xff;
		record_date_time.hundredths = 0xff;
	
// compare 	record_date_time with from_date_time
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
		
		loop_count++;
		
		if(match_result>=0)
			done=1;	// record_date_time >= from_date_time
		else
		{
			search_index++;	// increase the search index by 1
			if((search_index-initial_search_index)>g_Class07_Dailyload_EntriesInUse)
				search_index = initial_search_index + g_Class07_Dailyload_EntriesInUse;
			addr=(int32_t)DAILYLOG_BASE_ADDRESS + (int32_t)(search_index%NO_OF_DAILY_LOG_DATA)*(int32_t)sizeof(logdata_t);
		}
	}
	while ((done==0)&&(loop_count<NO_OF_DAILY_LOG_DATA));
	
// we will exit the loop if we have either found or crossed the from_date_time
// we will also exit the loop if we have searched all the data

	if((done==0)&&(loop_count==NO_OF_DAILY_LOG_DATA))
	{// no record was found with date_time >= from_date_time
		*range_start_entry=1;
		*range_end_entry=1;
		return;
	}

// here the done==1 and loop_count<NO_OF_DAYS_SURVEY_DATA

	if(match_result==0)
	{
		*range_start_entry=search_index+1;
	}
	else
	{// here match_result is > 0 - we have overshot - hence the actual record is in the past
	// search the past no_of_records_per_day by going back - this time search ends if match==0 or match <0
	// addr already points to the record whose datetime is > from_date_time
	// search_index is the index of that record
	
		loop_count=0;
		done=0;
		
		do
		{
			if(search_index==initial_search_index)
				done=1;	// no need to check any further - we have already reached the first record
			else
			{
			  decptr_maskless(&addr, DAILYLOG_BASE_ADDRESS, DAILYLOG_END_ADDRESS, DAILY_LOG_PTR_INC);
				search_index--;	// do we need to check for this value becoming -ve
//			EPR_Read_Test(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
				EPR_Read(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
				ConvertRTCDate(&record_date_time,&record_rtc_time);
				record_date_time.second = 0xff;
				record_date_time.hundredths = 0xff;
		
	// compare 	record_date_time with from_date_time
				match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)from_date_time);
				
				loop_count++;
				
				if(match_result<=0)
					done=1;	// record_date_time >= from_date_time
			}
		}
//		while ((done==0)&&(loop_count<no_of_records_per_day));
		while (done==0);
		
		// when we come out of the loop we would have the desired record	
		*range_start_entry=search_index+1;
	}

// we now have the range_start_entry - now to find the range_end_entry
// start from search_index and increase in steps of no_of_records_per_day till match_result>=0 or loop_count==NO_OF_DAILY_LOG_DATA 
// in either case we have the range_end_entry

	loop_count=0;
	done=0;

	search_index++;	// increase the search index by 1
	if((search_index-initial_search_index)>g_Class07_Dailyload_EntriesInUse)
		search_index = initial_search_index + g_Class07_Dailyload_EntriesInUse;
	addr=(int32_t)DAILYLOG_BASE_ADDRESS + (int32_t)(search_index%NO_OF_DAILY_LOG_DATA)*(int32_t)DAILY_LOG_PTR_INC;
	
	do
	{
//		EPR_Read_Test(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
		EPR_Read(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
		ConvertRTCDate(&record_date_time,&record_rtc_time);
		record_date_time.second = 0xff;
		record_date_time.hundredths = 0xff;
	
// compare 	record_date_time with from_date_time
		match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
		
		loop_count++;
		
		if(match_result>=0)
			done=1;	// record_date_time >= from_date_time
		else
		{
			search_index++;	// increase the search index by 1
			if((search_index-initial_search_index)>g_Class07_Dailyload_EntriesInUse)
				search_index = initial_search_index + g_Class07_Dailyload_EntriesInUse;
			addr=(int32_t)DAILYLOG_BASE_ADDRESS + (int32_t)(search_index%NO_OF_DAILY_LOG_DATA)*(int32_t)DAILY_LOG_PTR_INC;
		}
	}
	while ((done==0)&&(loop_count<NO_OF_DAILY_LOG_DATA));

// we come out of the loop when record time matches or crosses the to_date_time	
	
	if(match_result==0)
		*range_end_entry = search_index+1;	// we have a match
	else
	{// we have overshot - keep going back till entry is found or match_result becomes -ve
	
		loop_count=0;
		done=0;
		
		do
		{
			if(search_index==initial_search_index)
				done=1;	// no need to check any further - we have already reached the first record
			else
			{
			  decptr_maskless(&addr, DAILYLOG_BASE_ADDRESS, DAILYLOG_END_ADDRESS, DAILY_LOG_PTR_INC);
				search_index--;	// do we need to check for this value becoming -ve
//			EPR_Read_Test(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
				EPR_Read(addr, (uint8_t*)&record_rtc_time, sizeof(ext_rtc_t));	// we will get 8 bytes
				ConvertRTCDate(&record_date_time,&record_rtc_time);
				record_date_time.second = 0xff;
				record_date_time.hundredths = 0xff;
		
	// compare 	record_date_time with from_date_time
				match_result = R_OBIS_CompareDateTime((Unsigned8 *)&record_date_time, (Unsigned8 *)to_date_time);
				
				loop_count++;
				
				if(match_result<=0)
					done=1;	// record_date_time >= from_date_time
			}
		}
//		while ((done==0)&&(loop_count<no_of_records_per_day));
		while (done==0);
	
		*range_end_entry = search_index+1;
	}
}
*/
void get_bill_data(unsigned int c1){
// bill data for one month - this is fairly huge
//	billdata_t billdata;
	int32_t addr;	
	int i;
	int tmonth;

// c1 is the index value received from client 
// if entries in use is 6 - then we will get c1 from 0,1,2,3,4,5
	if (c1==0)
	{
		composeCurrentBillData();
	}
	else
	{
		tmonth = g_Class07_Billing_CurrentEntry - c1 -1;
		if(tmonth<0)
			tmonth=tmonth+12;
		
		addr=(int32_t)BILLDATA_BASE_ADDRESS + (int32_t)tmonth*(int32_t)BILL_PTR_INC;			
		EPR_Read(addr, (uint8_t*)&g_Class07_BillingBuffer, sizeof(g_Class07_BillingBuffer));	// we will get 420+ bytes
	}
}

int16_t findEventCode(uint8_t event_code_bit, uint8_t event_type)
{
	int16_t dlmsEventCode=0;
	switch (event_code_bit)
	{
		case 1:
			if (event_type==1)
				dlmsEventCode=1;	// R-Ph voltage missing
			else
				dlmsEventCode=2;	// R-Ph voltage restored
			break;
			
		case 2:
			if (event_type==1)
				dlmsEventCode=3;	// Y-Ph voltage missing
			else
				dlmsEventCode=4;	// Y-Ph voltage restored
			break;
			
		case 3:
			if (event_type==1)
				dlmsEventCode=5;	// B-Ph voltage missing
			else
				dlmsEventCode=6;	// Y-Ph voltage restored
			break;
			
		case 4:
			if (event_type==1)	// Over voltage occurence
				dlmsEventCode=7;
			else
				dlmsEventCode=8;	// Over votlage restored
			break;
			
		case 5:
			if (event_type==1)
				dlmsEventCode=9;	// Low voltage
			else
				dlmsEventCode=10;
			break;
			
		case 6:
			if (event_type==1)
				dlmsEventCode=11;	// Voltage unbalance occurence
			else
				dlmsEventCode=12;
			break;
			
		case 7:
			if (event_type==1)
				dlmsEventCode=51;	// CT reverse R ph occurence
			else
				dlmsEventCode=52;
			break;
			
		case 8:
			if (event_type==1)
				dlmsEventCode=53;	// CT reverse Y ph occurence
			else
				dlmsEventCode=54;
			break;
			
		case 9:
			if (event_type==1)
				dlmsEventCode=55;	// CT reverse B ph occurence
			else
				dlmsEventCode=56;
			break;
			
		case 10:
			if (event_type==1)
				dlmsEventCode=57;	// CT OPEN R ph occurence
			else
				dlmsEventCode=58;
			break;
			
		case 11:
			if (event_type==1)
				dlmsEventCode=59;	// CT OPEN Y ph occurence
			else
				dlmsEventCode=60;
			break;
			
		case 12:
			if (event_type==1)
				dlmsEventCode=61;	// CT OPEN B ph occurence
			else
				dlmsEventCode=62;
			break;
			
		case 13:
			if (event_type==1)
				dlmsEventCode=63;	// Current Unbalance occurence
			else
				dlmsEventCode=64;
			break;
			
		case 14:
			if (event_type==1)
				dlmsEventCode=65;	// Current Bypass occurence
			else
				dlmsEventCode=66;
			break;
			
		case 15:
			if (event_type==1)
				dlmsEventCode=67;	// Overcurrent any phase occurence
			else
				dlmsEventCode=68;
			break;
			
		case 16:
			if (event_type==1)
//				dlmsEventCode=101;
				dlmsEventCode=102;	// Power Supply resotred
			else
//				dlmsEventCode=102;
				dlmsEventCode=101;	// Power Supply failed
			break;
			
		case 17:
			dlmsEventCode=151;	// RTC changed
			break;
			
		case 18:
			dlmsEventCode=152;	// MD INtegration period changed
			break;
			
		case 19:	
			dlmsEventCode=153;	// Load survey interval changed
			break;
			
		case 20:
			dlmsEventCode=154;	// Single action schedule
			break;
			
		case 21:
			dlmsEventCode=155;	// Activity calendar - Time Zone
			break;
			
		case 22:
		case 23:
			if (event_type==1)
				dlmsEventCode=201;	// Magnet Occurence
			else
				dlmsEventCode=202;
			break;
			
		case 24:
			if (event_type==1)
				dlmsEventCode=203;	// Neutral Disturbance occurence
			else
				dlmsEventCode=204;
			break;
			
		case 25:
			if (event_type==1)
				dlmsEventCode=205;	// Low pf occurence
			else
				dlmsEventCode=206;
			break;
			
		case 26:
			dlmsEventCode=251;	// Cover open
			break;
	}
	return dlmsEventCode;
}

void get_tamper_data(unsigned int c1,unsigned int c2){
// the first value is the child_index, and the second value refers to the index of the records of that group
// child index will technically go from 9 to 14 - being the entries in the g_ChildTableClass07
// The (childindex -9) will translate from 0 to 5.
// c2 represents the record number in the subgroup

	unsigned int childIndex = c1-9;
	eventdata_t eventdata;

	uint8_t* src_ptr;
	uint8_t* dst_ptr;

	unsigned int addr;
	int i;

	if(g_Class07_Event_EntriesInUse[childIndex]==g_Class07_Event_MaxEntries[childIndex])
	{// over flow condition
		i=g_Class07_Event_CurrentEntry[childIndex]+c2;
		
		if(i>=g_Class07_Event_MaxEntries[childIndex])
			i=i-g_Class07_Event_MaxEntries[childIndex];
	}
	else
	{
		i=c2;
	}
		
	
	switch(childIndex){
		case 0:	// This is event block 0
//			addr=EVENTS0_BASE_ADDRESS + i*sizeof(eventdata_t);
			addr=EVENTS0_BASE_ADDRESS + i*EVENT_PTR_INC;
			if (addr>EVENTS0_END_ADDRESS)
				addr=EVENTS0_BASE_ADDRESS;
			EPR_Read(addr, (uint8_t*)&eventdata, sizeof(eventdata_t));	// we will get 32 bytes
			break;
		
		case 1:	// This is event block 1
//			addr=EVENTS1_BASE_ADDRESS + i*sizeof(eventdata_t);
			addr=EVENTS1_BASE_ADDRESS + i*EVENT_PTR_INC;
			if (addr>EVENTS1_END_ADDRESS)
				addr=EVENTS1_BASE_ADDRESS;
			EPR_Read(addr, (uint8_t*)&eventdata, sizeof(eventdata_t));	// we will get 32 bytes
			break;
		
		case 2:	// This is event block 2
//			addr=EVENTS2_BASE_ADDRESS + i*sizeof(eventdata_t);
			addr=EVENTS2_BASE_ADDRESS + i*EVENT_PTR_INC;
			if (addr>EVENTS2_END_ADDRESS)
				addr=EVENTS2_BASE_ADDRESS;
			EPR_Read(addr, (uint8_t*)&eventdata, sizeof(eventdata_t));	// we will get 32 bytes
			break;
		
		case 3:	// This is event block 3
//			addr=EVENTS3_BASE_ADDRESS + i*sizeof(eventdata_t);
			addr=EVENTS3_BASE_ADDRESS + i*EVENT_PTR_INC;
			if (addr>EVENTS3_END_ADDRESS)
				addr=EVENTS3_BASE_ADDRESS;
			EPR_Read(addr, (uint8_t*)&eventdata, sizeof(eventdata_t));	// we will get 32 bytes
			break;
		
		case 4:	// This is event block 4
//			addr=EVENTS4_BASE_ADDRESS + i*sizeof(eventdata_t);
			addr=EVENTS4_BASE_ADDRESS + i*EVENT_PTR_INC;
			if (addr>EVENTS4_END_ADDRESS)
				addr=EVENTS4_BASE_ADDRESS;
			EPR_Read(addr, (uint8_t*)&eventdata, sizeof(eventdata_t));	// we will get 32 bytes
			break;
		
		case 5:	// This is event block 5
//			addr=EVENTS5_BASE_ADDRESS + i*sizeof(eventdata_t);
			addr=EVENTS5_BASE_ADDRESS + i*EVENT_PTR_INC;
			if (addr>EVENTS5_END_ADDRESS)
				addr=EVENTS5_BASE_ADDRESS;
			EPR_Read(addr, (uint8_t*)&eventdata, sizeof(eventdata_t));	// we will get 32 bytes
			break;
		
	}
// The record is now available in eventdata and needs to be transferred to g_Class07_EventBuffer

	ConvertRTCDate(&g_Class07_EventBuffer.clock_value,&eventdata.datetime);
// the remaining values cannot be copied straightaway - since the data types are different
	g_Class07_EventBuffer.event_code=findEventCode(eventdata.event_code_bit,eventdata.event_type);
	g_Class07_EventBuffer.current_value_IR=(eventdata.current_value_IR&(~_BV(14)));
	g_Class07_EventBuffer.current_value_IY=(eventdata.current_value_IY&(~_BV(14)));
	g_Class07_EventBuffer.current_value_IB=(eventdata.current_value_IB&(~_BV(14)));
	
	if((eventdata.current_value_IR&_BV(14))==0)
		g_Class07_EventBuffer.voltage_value_VR=eventdata.voltage_value_VR;
	else
		g_Class07_EventBuffer.voltage_value_VR=(int32_t)256+(int32_t)eventdata.voltage_value_VR;
	
	if((eventdata.current_value_IY&_BV(14))==0)
		g_Class07_EventBuffer.voltage_value_VY=eventdata.voltage_value_VY;
	else
		g_Class07_EventBuffer.voltage_value_VY=(int32_t)256+(int32_t)eventdata.voltage_value_VY;
		
	if((eventdata.current_value_IB&_BV(14))==0)
		g_Class07_EventBuffer.voltage_value_VB=eventdata.voltage_value_VB;
	else
		g_Class07_EventBuffer.voltage_value_VB=(int32_t)256+(int32_t)eventdata.voltage_value_VB;
		
		
//	g_Class07_EventBuffer.voltage_value_VR=eventdata.voltage_value_VR;
//	g_Class07_EventBuffer.voltage_value_VY=eventdata.voltage_value_VY;
//	g_Class07_EventBuffer.voltage_value_VB=eventdata.voltage_value_VB;
	
	g_Class07_EventBuffer.power_factor_R=eventdata.power_factor_R;
	g_Class07_EventBuffer.power_factor_Y=eventdata.power_factor_Y;
	g_Class07_EventBuffer.power_factor_B=eventdata.power_factor_B;
	g_Class07_EventBuffer.active_energy=eventdata.active_energy;
#ifdef EVENTDATA_RECORDS_KVAH	
	g_Class07_EventBuffer.apparent_energy=eventdata.apparent_energy;
#endif	
}


void get_lp_data(unsigned int c1){
// load survey profile	
// when the index is one - i.e. when the first record is sought c1=0
// the index must always lie between 0 and (NO_OF_SURVEY_RECORDS-1)
// The index is always referenced to the g_Class07_Blockload_CurrentEntry if 
// g_Class07_Blockload_EntriesInUse == NO_OF_SURVEY_RECORDS
// else the index was referenced to 0 if the buffer had not overflowed
	small_surveydata_t surveydata;
	ext_rtc_t tempRTC;
	uint32_t addr;
	unsigned int i;
	
	uint8_t* src_ptr;
	uint8_t* dst_ptr;
	
	if(g_Class07_Blockload_EntriesInUse==NO_OF_SURVEY_RECORDS)
	{// over flow condition
		i=g_Class07_Blockload_CurrentEntry+c1;
		if(i>=NO_OF_SURVEY_RECORDS)
			i=i-NO_OF_SURVEY_RECORDS;
	}
	else
	{
		i=c1;
	}
	
// the following if block is only for debugging	
//	if(c1>1437)
//	{
//		addr=SURVEY_BASE_ADDRESS + i*sizeof(surveydata_t);
//	}
	
//	addr=SURVEY_BASE_ADDRESS + i*sizeof(surveydata_t);
	addr=(int32_t)SURVEY_BASE_ADDRESS + (int32_t)i*(int32_t)SURVEY_PTR_INC;

	EPR_Read(addr, (uint8_t*)&surveydata, sizeof(small_surveydata_t));	// we will get 28 bytes for BIDIR and 10 bytes for UNIDIR
// now to use this to fill the g_Class07_BlockLoadBuffer
// only the date_time_t(12) and ext_rtc_t(8) are different

	ConvertShortDate_RTC(&tempRTC,&surveydata.shortDateTime);
	ConvertRTCDate(&g_Class07_BlockLoadBuffer.clock_value,&tempRTC);

// the remaining values cannot be copied straightaway - since the data types are different
#ifdef SURVEY_USES_AVERAGE_DATA		
	g_Class07_BlockLoadBuffer.current_value_IR=(surveydata.I_r&(~_BV(14)));	// remove the voltage 9th bit
	g_Class07_BlockLoadBuffer.current_value_IY=(surveydata.I_y&(~_BV(14)));	// remove the voltage 9th bit
	g_Class07_BlockLoadBuffer.current_value_IB=(surveydata.I_b&(~_BV(14)));	// remove the voltage 9th bit
	
	if((surveydata.I_r&_BV(14))==0)
		g_Class07_BlockLoadBuffer.voltage_value_VR=surveydata.V_r;
	else
		g_Class07_BlockLoadBuffer.voltage_value_VR=(int32_t)256+(int32_t)surveydata.V_r;
		
	if((surveydata.I_y&_BV(14))==0)
		g_Class07_BlockLoadBuffer.voltage_value_VY=surveydata.V_y;
	else
		g_Class07_BlockLoadBuffer.voltage_value_VY=(int32_t)256+(int32_t)surveydata.V_y;
		
	if((surveydata.I_b&_BV(14))==0)
		g_Class07_BlockLoadBuffer.voltage_value_VB=surveydata.V_b;
	else
		g_Class07_BlockLoadBuffer.voltage_value_VB=(int32_t)256+(int32_t)surveydata.V_b;
#endif		
		
	g_Class07_BlockLoadBuffer.kWh_value=surveydata.kwh_consumption;
	
#ifdef SURVEY_USES_REACTIVE_DATA
	g_Class07_BlockLoadBuffer.kvarh_lag_value=surveydata.kvarh_lag_consumption;
	g_Class07_BlockLoadBuffer.kvarh_lead_value=surveydata.kvarh_lead_consumption;
#endif

	g_Class07_BlockLoadBuffer.kVAh_value=surveydata.kvah_consumption;
	
#ifdef SURVEY_USES_EXPORT_DATA	
	g_Class07_BlockLoadBuffer.kWh_value_export=surveydata.kwh_consumption_export;	
#endif

#ifdef SURVEY_USES_AVERAGE_PF
	g_Class07_BlockLoadBuffer.avgPF_r=surveydata.avgPF_r;
	g_Class07_BlockLoadBuffer.avgPF_y=surveydata.avgPF_y;
	g_Class07_BlockLoadBuffer.avgPF_b=surveydata.avgPF_b;
#endif

#ifdef SURVEY_USES_FREQ_DATA
	g_Class07_BlockLoadBuffer.frequency=surveydata.frequency;
#endif

#ifdef SURVEY_USES_DEMAND_DATA
	g_Class07_BlockLoadBuffer.apparentDemand=surveydata.apparentDemand;
	g_Class07_BlockLoadBuffer.activeDemand=surveydata.activeDemand;
#endif

}


void get_dlp_data(unsigned int c1){
	logdata_t logdata;
	uint32_t addr;
	unsigned int i;

	if(g_Class07_Dailyload_EntriesInUse==NO_OF_DAILY_LOG_DATA)
	{// over flow condition
		i=g_Class07_Dailyload_CurrentEntry+c1;
		if(i>=NO_OF_DAILY_LOG_DATA)
			i=i-NO_OF_DAILY_LOG_DATA;
	}
	else
	{
		i=c1;
	}
	
//	addr=DAILYLOG_BASE_ADDRESS + c1*sizeof(logdata_t);
	addr=(int32_t)DAILYLOG_BASE_ADDRESS + (int32_t)i*(int32_t)DAILY_LOG_PTR_INC;
	EPR_Read(addr, (uint8_t*)&logdata, sizeof(logdata_t));	// we will get 16 bytes

// now to use this to fill the  g_Class07_DailyLoadBuffer
// only the date_time_t(12) and ext_rtc_t(8) are different
	ConvertRTCDate(&g_Class07_DailyLoadBuffer.clock_value,&logdata.datetime);

// the remaining values can be copied straightaway
#ifdef SMALL_DAILY_LOG
		g_Class07_DailyLoadBuffer.kWh_value=logdata.kWh_value;
	#ifdef BIDIR_METER	
		g_Class07_DailyLoadBuffer.kWh_value_export  =logdata.kWh_value_export;
	#else
		g_Class07_DailyLoadBuffer.kVAh_value  =logdata.kVAh_value;
	#endif
#else
	#ifdef DAILY_POWER_ON_LOGGING
		g_Class07_DailyLoadBuffer.dailyPonMinutes=logdata.dailyPonMinutes;
	#else
		g_Class07_DailyLoadBuffer.kWh_value=logdata.kWh_value;
		g_Class07_DailyLoadBuffer.kVAh_value  =logdata.kVAh_value;
		g_Class07_DailyLoadBuffer.kVArh_lag_value  =logdata.kVArh_lag_value;
		g_Class07_DailyLoadBuffer.kVArh_lead_value  =logdata.kVArh_lead_value;
	#endif
#endif
}

void get_time_data(unsigned long int c1, unsigned long int c2){}

Unsigned8 R_OBIS_ActionObjectMethod(
	st_Cosem_Method_Desc *cosem_method_desc,
	service_type_t service_type,
	Unsigned16 child_id,
	Unsigned8 *p_out_buf,
	Unsigned16 *p_out_len,
	Unsigned8 *p_data,
	Unsigned16 data_len,
	Unsigned8 *block_transfer
)
{
	// Result of action, def. is not Matched  
	Unsigned8 response_result = ACTION_RESULT_TYPE_UNMATCH;

	// Vars used to handle processing  
	Unsigned16	class_id;
	Unsigned16	method_id = (cosem_method_desc->Method_ID & 0x00FF);
	Unsigned8	b_have_client_data = (Unsigned8)(cosem_method_desc->Method_ID >> 8);

	// Vars used to encode data  
	void        *pbuffer;
	Unsigned16  length;
	Unsigned8   u8_data;
	Unsigned16  u16_data;
	Unsigned32  u32_data;
	Integer32   f32_data;
	attr_type_t type;

	// Params check  
	if (cosem_method_desc == NULL ||
		p_out_buf == NULL ||
		p_out_len == NULL ||
		block_transfer == NULL)
	{
		return TYPE_UNMATCH;
	}
	
	// Get information from attr descriptor  
	class_id = (
		((Unsigned16)cosem_method_desc->Class_ID[0] << 8) |
		((Unsigned16)cosem_method_desc->Class_ID[1])
	);

//	 * Default, marked to response to object layer that not use block transfer
//	 * Change this value to 1 at relative class processing (in below) to inform to
//	 * object layer that R_OBIS_GetObjectAttr() need to use block transfer
	*block_transfer = 0;

	// Default clear the optional data result to 0  
	*p_out_buf = ATTR_TYPE_NULL_DATA;
	*p_out_len = 0;

//	 * Temporary if (0) to prevent compile error
//	 * when no class is used
	if (0)
	{
		// Do nothing  
	}
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
	else if (class_id == 7)
	{
		class07_child_record_t	*p_child_record;

		// Get the child record  
		p_child_record = (class07_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);
	
		// Method 1 - reset(data)?  
		if (method_id == 1)
		{
			switch(child_id)
			{
				case 2: // Block Load Profile  
					g_Class07_Blockload_CurrentEntry = 0;
					g_Class07_Blockload_EntriesInUse = 0;
					ctldata.survey_wrt_ptr=SURVEY_BASE_ADDRESS;
					ctldata.overflow&=~SUR_OVERFLOW;
					e2write_flags|=E2_W_CTLDATA;
					break;
				case 4: // Daily profile  
					g_Class07_Dailyload_CurrentEntry = 0;
					g_Class07_Dailyload_EntriesInUse = 0;
					ctldata.daily_log_wrt_ptr=DAILYLOG_BASE_ADDRESS;
					ctldata.overflow&=~DAILYLOG_OVERFLOW;
					e2write_flags|=E2_W_CTLDATA;
					break;
				case 6: // Billing profile  
					g_Class07_Billing_CurrentEntry = 1;	// 0 earlier
					g_Class07_Billing_EntriesInUse = 1;	// 0 earlier
					Common_Data.history_ptr=1;
					e2write_flags|=E2_W_IMPDATA;
					ctldata.overflow&=~HISTORY_OVERFLOW;
					e2write_flags|=E2_W_CTLDATA;
					break;
				case 9:  // Event 0 profile  
				case 10: // Event 1 profile  
				case 11: // Event 2 profile  
				case 12: // Event 3 profile  
				case 13: // Event 4 profile  
				case 14: // Event 5 profile  
					g_Class07_Event_CurrentEntry[child_id-9] = 0;
					g_Class07_Event_EntriesInUse[child_id-9] = 0;
					break;
				default:
					// Do nothing 
					break;
			}
			response_result = ACTION_RESULT_SUCCESS;
		}
		// Method 2 - capture(data)  
		else if (method_id == 2)
		{
			switch(child_id)
			{
				case 2: // Block Load Profile
					// this option is not permitted as we are recording data as per the time - hence nay entry
					// written out of time is likely to be overwritten
					//R_OBIS_Class07_BlockloadUpdate();	// this function is replaced by our own
					break;
				case 4: // Daily profile 
					//R_OBIS_Class07_DailyloadUpdate(); // this function is replaced by our own
			    writeLogData(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second); 
			    computeDailyLogEntries();
					break;
				case 6: // Billing profile
					//R_OBIS_Class07_BillingUpdate(); // this function is replaced by our own
					generate_bill(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second, BT_TRANS);
					break;
				default:
					// Do nothing 
					break;
			}
			response_result = ACTION_RESULT_SUCCESS;
		}
		else if (method_id == 3)
		{
			//NOT support function for old version method
		}
		else if (method_id == 4)
		{
			//NOT support function for old version method
		}
		else
		{
			// Do nothing  
		}
	}
#endif
#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)
	else if (class_id == 9)
	{
		class09_child_record_t	*p_child_record;

		// Get the child record  
		p_child_record = (class09_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);
	
		// Method 1 - execute (data)?  
		if (method_id == 1)
		{
			// Do nothing  
			u16_data = ((Unsigned16)*(p_data + 1) << 8) | *(p_data + 2);
			for (u8_data = 0; u8_data < p_child_record->nr_scripts; u8_data ++)
			{
				script_t *check_script = (script_t *)(p_child_record->p_scripts + u8_data);

				if(u16_data == check_script->script_id)
				{
					//Execute script 
					if(R_OBIS_Class09_ScriptExecute(p_child_record->logical_name, u16_data))
					{
						response_result = ACTION_RESULT_SUCCESS;
					}
					break;
				}
				// Ensure no reset  
				R_USER_WDT_Restart();
			}
		}
		else
		{
			// Do nothing  
		}
	}
#endif
#if (defined(USED_CLASS_15) && USED_CLASS_15 == 1)
	else if (class_id == 15)
	{
		class15_child_record_t	*p_child_record;

		// Get the child record  
		p_child_record = (class15_child_record_t *)(
		R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);
	
		// Method 1 - reply_to_HLS_authentication (data)?  
		if (method_id == 1)
		{
//			 * TODO: put the custom verification method here, using
//			 * + secret method = p_child_record->mechanism_id (MD5, SHA-1, GMAC, etc ?)
//			 * + shared key    = p_child_record->p_shared_key
//			 * + StoC          = p_child_record->p_secret_key
//			 * + CtoS          = COSEMOpenGetHLSCtoS()
//			 * + f(StoC)       = p_data +2
//			 * + f(CtoS)       = p_out_buf
			switch (child_id)
			{
				// do the same for all associations  
				default:
				{
					Unsigned16	i;					// counter  

					Unsigned8	*p_client_fStoC;	// f(StoC) received from client  
					Unsigned8	*p_client_CtoS;		// CtoS received from client  
					Unsigned8	key[64];			// store challenge | shared key used to hash  
					Unsigned16	length = 0;			// Actual length on the key  
#if (MD5_ENABLE || SHA1_ENABLE)
					Unsigned8	hash_value[20];		// 160-bits hash result  
					// Init hash_value all 0  
					memset(hash_value, 0, 20);
#endif

					// check settings on g_ChildClass15[] & client data  
					if (p_child_record->p_authen_status == NULL ||
						p_child_record->p_secret_key == NULL ||
						b_have_client_data == 0)
					{
						// failure  
						response_result = ACTION_RESULT_SCOPE_ACCESS_VIOLATED;
						break;
					}
					
//					 * Default the authentication status is failure,
//					 * will be update to success if all verification OK
					response_result = ACTION_RESULT_SCOPE_ACCESS_VIOLATED;
					*p_child_record->p_authen_status = AUTHEN_STATUS_FAILURE;

					// Restore secret  
					{
						Unsigned16	len = strlen((void *)p_child_record->p_secret_key);
						R_USER_Class15_Secret_Restore(p_child_record->p_secret_key,
												len,
												p_child_record->mechanism_id
												);
					}

					// Use a custom secret method?  
					if (p_child_record->mechanism_id == MECHANISM_ID2_HIGH_SECURITY)
					{
//						 * TODO: put the custom verification method here
//						 * Following example:
//						 *        + use ECB encode for challenge with encypt key is shared key
//						 *        + Max size is key[64]
#if AES_ENABLE
						// Calculate f(StoC)  
						length = R_OBIS_Aes_Ecbenc(p_child_record->p_shared_key, key, p_child_record->p_secret_key);

						// Check client is authenticated or not: compare with f(StoC) from client  
						if (*(p_data+0) != ATTR_TYPE_OCTET_STRING ||
							*(p_data+1) != (Unsigned8)length)
						{
							// not match, break as failure  
							break;
						}
						p_client_fStoC = p_data + 2;
						for (i = 0; i < length; i++, p_client_fStoC++)
						{
							if (key[i] != *p_client_fStoC)
							{
								break;
							}
						}
						if (i < length)
						{
							// not match, break as failure  
							break;
						}

						// Client is authenticated, processes f(CtoS) send to client  
						*(p_out_buf+0) = ATTR_TYPE_OCTET_STRING;
						p_client_CtoS = COSEMOpenGetHLSCtoS();

						length = R_OBIS_Aes_Ecbenc(p_client_CtoS, (p_out_buf + 2), p_child_record->p_secret_key);
						*(p_out_buf+1) = (Unsigned8)length;
						*p_out_len = length + 2;

						// Success  
						*p_child_record->p_authen_status = AUTHEN_STATUS_SUCCESS;
						response_result = ACTION_RESULT_SUCCESS;
#endif 					
					}
					
#if MD5_ENABLE
					// Use MD5?  
					else if (p_child_record->mechanism_id == MECHANISM_ID3_HIGH_SECURITY_MD5)
					{
						// Create the f(StoC) again  
						i = strlen((void *)p_child_record->p_shared_key);
						memcpy(key, p_child_record->p_shared_key, i);
						length = i;
						
						i = strlen((void *)p_child_record->p_secret_key);
						memcpy(key + length, p_child_record->p_secret_key, i);
						length += i;
						
						R_Md5_HashDigest(
							key,
							hash_value,
							length
						);
						
						// Verify own f(StoC) with the one that send from client  
						if (*(p_data+0) != ATTR_TYPE_OCTET_STRING ||
							*(p_data+1) != 16)
						{
							// MD5 not match, break as failure  
							break;
						}

						p_client_fStoC = p_data + 2;				
						for (i = 0; i < 16; i++, p_client_fStoC++)
						{
							if (hash_value[i] != *p_client_fStoC)
							{
								break;
							}
						}
						if (i < 16)
						{
							// MD5 not match, break as failure  
							break;
						}
						
//						 * If reached here: means verify OK, then make the f(CtoS) and
//						 * send back to client
						*p_out_buf++ = ATTR_TYPE_OCTET_STRING;
						*p_out_buf++ = 16;
						*p_out_len   = 18;
						
						// Make f(CtoS) (16 bytes) then store to p_out_buf  
						p_client_CtoS = COSEMOpenGetHLSCtoS();
						i = strlen((void *)p_client_CtoS);
						memcpy(key, p_client_CtoS, i);
						length = i;
						
						i = strlen((void *)p_child_record->p_secret_key);
						memcpy(key + length, p_child_record->p_secret_key, i);
						length += i;
						
						R_Md5_HashDigest(
							key,
							p_out_buf,
							length
						);

						// Success  
						*p_child_record->p_authen_status = AUTHEN_STATUS_SUCCESS;
						response_result = ACTION_RESULT_SUCCESS;
					}
#endif

#if SHA1_ENABLE
					// Use SHA-1?  
					else if (p_child_record->mechanism_id == MECHANISM_ID4_HIGH_SECURITY_SHA1)
					{
						R_sha1	work;	// work area of SHA1  

						// Create the f(StoC) again  
						i = strlen((void *)p_child_record->p_shared_key);
						memcpy(key, p_child_record->p_shared_key, i);
						length = i;
						
						i = strlen((void *)p_child_record->p_secret_key);
						memcpy(key + length, p_child_record->p_secret_key, i);
						length += i;
						
						memset((void *)&work, 0, sizeof(R_sha1));
						if (R_Sha1_HashDigest(
								key,
								hash_value,
								length,
								(R_SHA_INIT | R_SHA_FINISH),
								&work) != R_PROCESS_COMPLETE)
						{
							// SHA-1 hash failure  
							break;
						}
						
						// Verify own f(StoC) with the one that send from client  
						if (*(p_data+0) != ATTR_TYPE_OCTET_STRING ||
							*(p_data+1) != 20)
						{
							// SHA-1 not match  
							break;
						}

						p_client_fStoC = p_data + 2;				
						for (i = 0; i < 20; i++, p_client_fStoC++)
						{
							if (hash_value[i] != *p_client_fStoC)
							{
								break;
							}
						}
						if (i < 20)
						{
							// SHA-1 not match  
							break;
						}
						
//						 * If reached here: means verify OK, then make the f(CtoS) and
//						 * send back to client
						*p_out_buf++ = ATTR_TYPE_OCTET_STRING;
						*p_out_buf++ = 20;
						*p_out_len   = 22;
						
						// Make f(CtoS) (20 bytes) then store to p_out_buf  
						p_client_CtoS = COSEMOpenGetHLSCtoS();
						i = strlen((void *)p_client_CtoS);
						memcpy(key, p_client_CtoS, i);
						length = i;
						
						i = strlen((void *)p_child_record->p_secret_key);
						memcpy(key + length, p_child_record->p_secret_key, i);
						length += i;
						
						memset((void *)&work, 0, sizeof(R_sha1));
						if (R_Sha1_HashDigest(
								key,
								p_out_buf,
								length,
								(R_SHA_INIT | R_SHA_FINISH),
								&work) != R_PROCESS_COMPLETE)
						{
							// SHA-1 hash failure  
							break;
						}
						
						// Success  
						*p_child_record->p_authen_status = AUTHEN_STATUS_SUCCESS;
						response_result = ACTION_RESULT_SUCCESS;
					}
#endif
					// Use GMAC?  
					else if (p_child_record->mechanism_id == MECHANISM_ID5_HIGH_SECURITY_GMAC)
					{
//						 * GMAC verification is supported on global_key_transfer method
//						 * of security_setup_ref, not here
					}
					else
					{
						// do nothing, use default status as failure  
					}
				}
				
				break;
			}
		}
		// Method 2 - change_HLS_secret (data)?  
		else if (method_id == 2)
		{
			Unsigned8	*p_client_secret = NULL;	// secret received from client  
			// Actual length on the key  
			Unsigned8	length = (Unsigned8)strlen((void *)p_child_record->p_secret_key);

			// Check client data format  
			if (*(p_data+0) != ATTR_TYPE_OCTET_STRING ||
				*(p_data+1) != (Unsigned8)length)
			{
				// not match, break as failure  
				return (response_result);
			}
			// Point to secret  
			p_client_secret = p_data + 2;

			memcpy(p_child_record->p_secret_key, p_client_secret, length);
			// Backup secret  
			u8_data = R_USER_Class15_Secret_Backup(p_child_record->p_secret_key,
										(Unsigned8)length,
										p_child_record->mechanism_id
										);
			if (u8_data != 0)
			{
				return response_result;
			}

			response_result = ACTION_RESULT_SUCCESS;
		}
		// Method 3 - add_object (data)?  
		else if (method_id == 3)
		{
			// Do nothing  
		}
		// Method 4 - remove_object (data)?  
		else if (method_id == 4)
		{
			// Do nothing  
		}
		else
		{
			// Do nothing  
		}
	}
#endif
#if (defined(USED_CLASS_18) && USED_CLASS_18 == 1)
//	 * Handle all requests for class 18
//	 * Just do on following actions only:
//	 * 1. image_transfer_initiate (data)
//	 * 2. image_block_transfer (data)
//	 * 3. image_verify (data)
//	 * 4. image_activate (data)
	else if (class_id == 18)
	{
		// TODO: support these methods  
		response_result = ACTION_RESULT_RD_WRT_DENIED;
	}
#endif
#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
//	 * Handle all requests for class 20
//	 * Just do on following actions only:
//	 * 1. activate_passive_calendar (data)
	else if (class_id == 20)
	{
		// Method 1 - activate_passive_calendar (data)?  
		if (method_id == 1)
		{
			R_USER_WDT_Restart();
			
			record_transaction_event(ACTIVITY_CALENDAR_TRANSACTION_MASK);	// in this function rtc is read
			Common_Data.pgm_count++;
			
// the following two lines are not required as generate_bill saves Common_Data
//			e2write_flags|=E2_W_IMPDATA;
//			wre2rom(); // write the tamper here itself			
			generate_bill(rtc.date, rtc.month, rtc.year, rtc.hour, rtc.minute, rtc.second, BT_TRANS);
			u8_data = R_OBIS_Class20_Activate_PassiveCalendar(child_id);
			
//			write_active_tod();	// being done in R_OBIS_Class20_Activate_PassiveCalendar
			if(u8_data == 0)
			{
				response_result = ACTION_RESULT_SUCCESS;
			}
		}
	}
#endif
#if (defined(USED_CLASS_70) && USED_CLASS_70 == 1)
//	 * Handle all requests for class 70
//	 * Just do on following actions only:
//	 * 1. remote_disconnect (data)
//	 * 2. remote_reconnect (data)
	else if (class_id == 70)
	{
		//switch(method_id)
		//{
		//	case 1: break;
		//	case 2: break;
		//}
		//Set_PP(method_id,38);
		// TODO: support these methods  
		response_result = ACTION_RESULT_SUCCESS;
	}
#endif
	else
	{
		return ACTION_RESULT_TYPE_UNMATCH;
	}
	
	return (response_result);
}


//int32_t gResult;

Unsigned8 R_OBIS_GetObjectAttr(
	st_Cosem_Attr_Desc *cosem_attr_desc,
	service_type_t service_type,
	Unsigned16 child_id,
	Unsigned8  *p_out_buf,
	Unsigned16 *p_out_len,
	Unsigned8 *block_transfer,
	Unsigned8 *p_data
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;

	// Vars used to handle processing  
	Unsigned16 class_id;
	Unsigned16 attr_id;

	// Vars used to encode data  
	void        *pbuffer;
	Integer16   size;
	Unsigned8	u8_data;
	Unsigned16  u16_data;
	Unsigned32  u32_data;
	Integer32   f32_data;
	attr_type_t type;

	// Params check  
	if (cosem_attr_desc == NULL ||
		p_out_buf == NULL ||
		p_out_len == NULL ||
		block_transfer == NULL)
	{
		return TYPE_UNMATCH;
	}

	// Get information from attr descriptor  
	class_id = (
		((Unsigned16)cosem_attr_desc->Class_ID[0] << 8) |
		((Unsigned16)cosem_attr_desc->Class_ID[1])
	);
	attr_id = cosem_attr_desc->Attr_ID;

//	 * Default, marked to response to object layer that not use block transfer
//	 * Change this value to 1 at relative class processing (in below) to inform to
//	 * object layer that R_OBIS_GetObjectAttr() need to use block transfer
	*block_transfer = 0;

//	 * Temporary if (0) to prevent compile error
//	 * when no class is used
	if (0)
	{
		// Do nothing  
	}
#if (defined(USED_CLASS_01) && USED_CLASS_01 == 1)
//	 * Handle all requests for class 01
//	 * Just do on following attributes only:
//	 * 1. Attr 2 - value : CHOICE
//	 * All other attributes are handled in R_OBIS_DistributeDataClass() function.
	else if (class_id == 1)
	{
		class01_child_record_t	*p_child_record1;

		date_time_t date_time;

		// Get the child record  
		p_child_record1 = (class01_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);

		// Attr 2 - value : CHOICE ?  
		if (attr_id == 2)
		{
			Unsigned8 u8;
			Unsigned16 u16;
			Unsigned32 u32;
//			Unsigned8 buffer[16];
			Unsigned8 buffer[40];
			memset(buffer, 0, 40);
			// Default  
			pbuffer = buffer;

			// Get the size from child table definition  
			size = p_child_record1->value.choice.size;

			// Get the buffer pointer  
			switch (child_id)
			{
				
				case 0:
					
					size =  R_USER_GetNamePlate(NAME_PLATE_LOGICAL_NAME, pbuffer);
					if (size < 0)
					{
						return 0;
					}
						break;
					
				case 1:		// Meter serial number  
					size =  R_USER_GetNamePlate(NAME_PLATE_METER_SERIAL, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;

				case 2:		// Manufacture Name  
					size =  R_USER_GetNamePlate(NAME_PLATE_MANUFACT_NAME, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;

				case 3:		// Firmware Version for meter  
					size =  R_USER_GetNamePlate(NAME_PLATE_FIRMWARE_NAME, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;

				case 4:		// Meter Type  
					size =  R_USER_GetNamePlate(NAME_PLATE_METER_TYPE, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;

				case 5:		// Category  
					size =  R_USER_GetNamePlate(NAME_PLATE_CATEGORY_NAME, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;

				case 6:		// Current rating  
					size =  R_USER_GetNamePlate(NAME_PLATE_CURRRATE_NAME, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;

				case 7:		// Meter Year of Manufacture  
					size =  R_USER_GetNamePlate(NAME_PLATE_YEAR_OF_MANUFACT, pbuffer);
					if (size < 0)
					{
						return 0;
					}
					break;
							
				case 8:		// Demand Integration Period  
					u16 = (Unsigned16)R_USER_GetEMData(DEMAND_INTEGRATION_PERIOD);
					pbuffer = (Unsigned8 *)&u16;
					break;
					
				case 9:		// Profile Capture Period   //NEED TO CHECK // Block load capture period??  
					u16 = (Unsigned16)R_USER_GetEMData(PROFILE_CAPTURE_PERIOD);
					pbuffer = (Unsigned8 *)&u16;
					break;

				case 11:	// Cumulative tamper count  
					u16 = (Unsigned16)R_USER_GetEMData(CUM_TAMPER_CNT);
					pbuffer = (Unsigned8 *)&u16;
					break;

				case 12:	// Cumulative billing count  
					u32 = (Unsigned32)R_USER_GetEMData(CUM_BILLING_CNT);
					pbuffer = (Unsigned8 *)&u32;
					break;

				case 13:	// Cumulative programming count  
					u16 = (Unsigned16)R_USER_GetEMData(CUM_PROGAM_CNT);
					pbuffer = (Unsigned8 *)&u16;
					break;

				case 14:	// EventCode  
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
					pbuffer = &g_Class01_EventCode[child_id-14];
					break;
				case 20:	// Available Billing Period  
					u8 = (Unsigned8)R_USER_GetEMData(AVL_BILLING_PERIOD);;   //NEED IMPLEMENTATION
					pbuffer = (Unsigned8 *)&u8;
					break;
				case 21:	// No. Of Power Failure  
//					u32 = 10;//(Unsigned32)R_USER_GetEMData(EM_CUCUMLARIVE_BILLING_CNT);
					u32 = (Unsigned32)R_USER_GetEMData(CUM_POWER_FAILURE_COUNT);
					pbuffer = (Unsigned8 *)&u32;
					break;
				case 22:	// Internal CT ratio*  
					u16 =1; //(Unsigned16)R_USER_GetEMData(EM_CUCUMLARIVE_PROGAM_CNT);
					pbuffer = (Unsigned8 *)&u16;
					break;
				case 23:	// Internal VT ratio*  
					u16 = 1;
					pbuffer = (Unsigned8 *)&u16;
					break;
				case 24:	// Phase Sequence  
					if((g_TamperStatus&MISSPOT_MASK)==0)
					{// here no potential is missing
						u16=gPhaseSequence;
					}
					else
					{// here some potential is missing
						u16=255;
					}
					pbuffer = (Unsigned8 *)&u16;
					break;
				case 25:	// Rising Demand kva  
			    u8=60/(int32_t)Common_Data.mdinterval;
					if(myenergydata.kvah>ctldata.kvah_last_saved)
						u16 = ((myenergydata.kvah-ctldata.kvah_last_saved)*(int32_t)u8);
					else
					{
						ctldata.kvah_last_saved=myenergydata.kvah;
						u16=0;
					}
					pbuffer = (Unsigned8 *)&u16;
					break;
					
				case 26:	// Rising Demand kva elapsed time  
					u16 = rtc.minute - ((rtc.minute/Common_Data.mdinterval)*Common_Data.mdinterval); 
					pbuffer = (Unsigned8 *)&u16;
					break;
					
				case 27:	// Self Diagnostic - rtcBattery(bit0), memcheck(bit1)
					u16 = Common_Data.rtc_failed;	// 1 - batt bad, 0 - batt good 
					if(0!=perform_E2ROM12_Test())
						u16=u16+2;
					pbuffer = (Unsigned8 *)&u16;
					break;
					
				default:
					break;
			}
			// Determind the size, based on buffer  
			if (pbuffer != NULL &&
				size == -1)
			{
				size = strlen((void*)pbuffer);
			}

			// Encode & indicate as success when buffer is valid  
			if (pbuffer != NULL &&
				size != -1)
			{
				*p_out_len = R_OBIS_EncodeAttribute(
					p_out_buf,								// Output buffer, pointed to g_ServiceBuffer  
					OBIS_SERVICE_DATA_BUFFER_LENGTH,		// Max length of g_ServiceBuffer  
					p_child_record1->value.choice.type,		// Type  
					(Unsigned8 *)pbuffer,					// Buffer  
					size									// Length  
				);

				// Success  
				response_result = DATA_ACCESS_RESULT_SUCCESS;
			}
		}
		else
		{
			// Do nothing  
		}
	}
#endif
#if (defined(USED_CLASS_03) && USED_CLASS_03 == 1)
//	 * Handle all requests for class 03
//	 * Just do on following attributes only:
//	 * 1. Attr 2 - value : CHOICE.
//	 * 2. Attr 3 - scaler_unit : scal_unit_type
//	 * All other attributes are handled in R_OBIS_DistributeRegisterClass() function.
	else if (class_id == 3)
	{
		class03_child_record_t	*p_child_record3;

		// Get the child record  
		p_child_record3 = (class03_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);

		// Attr 2 - value : CHOICE ?  
		if (attr_id == 2)
		{
			Integer32 temp32;
			Integer32 tempAnother;
			date_time_t temp_time;
			Unsigned16 u16;
			
			// Get the size from child table definition  
			size = p_child_record3->value.choice.size;

			// Get the buffer pointer  
			switch (child_id)
			{
				case 0:		// Ir  
					temp32 = R_USER_GetEMData(I_R);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 1:		// Iy  
					temp32 = R_USER_GetEMData(I_Y);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 2:		// Ib  
					temp32 = R_USER_GetEMData(I_B);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 3:		// Vr  
					temp32 = R_USER_GetEMData(V_R);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 4:		// Vy  
					temp32 = R_USER_GetEMData(V_Y);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 5:		// Vb  
					temp32 = R_USER_GetEMData(V_B);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 6:		// Signed Power Factor  
					temp32 = R_USER_GetEMData(PF_R);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 7:		// Signed Power Factor  
					temp32 = R_USER_GetEMData(PF_Y);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 8:		// Signed Power Factor  
					temp32 = R_USER_GetEMData(PF_B);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 9:		// Signed Power Factor  
					temp32 = R_USER_GetEMData(PF_TOTAL);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 10:		// Frequency  
					temp32 = R_USER_GetEMData(FREQUENCY);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 11:		// Apparent Power  
					temp32 = R_USER_GetEMData(APPARENT_POWER);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 12:		// Active Power  
					temp32 = R_USER_GetEMData(ACTIVE_POWER);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 13:		// Reactive Power  
					temp32 = R_USER_GetEMData(REACTIVE_POWER);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 14:		// Cumulative Energy - kWh  
					temp32 = R_USER_GetEMData(CUM_ACTIVE_ENERGY);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 15:		// Cumulative Energy - kvarh(lag)  
					temp32 = R_USER_GetEMData(CUM_REACTIVE_LAG_ENERGY);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 16:		// Cumulative Energy - kvarh(lead)  
					temp32 = R_USER_GetEMData(CUM_REACTIVE_LEAD_ENERGY);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 17:		// Cumulative Energy - kVAh  
					temp32 = R_USER_GetEMData(CUM_APPARENT_ENERGY);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 18:		// Cumulative power-failure duration  
// there is another implementation in r_dlms_user_interface.R_USER_GetEMData
// we might as well call that function
					temp32 = R_USER_GetEMData(CUM_POWER_FAILURE_DURN);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 19:		// Billing date  
					temp32 = Common_Data.billing_date;	// this should be a string?
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 20:		// Block Energy kwh  
					temp32 = R_USER_GetEMData(EM_BLOCK_ENERGY_KWH);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 21:		// Block Energy kvarh lag  
					temp32 = R_USER_GetEMData(EM_BLOCK_ENERGY_KVARH_LAG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 22:		// Block Energy kvarh lead  
					temp32 = R_USER_GetEMData(EM_BLOCK_ENERGY_KVARH_LEAD);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 23:		// Block Energy kvah  
					temp32 = R_USER_GetEMData(EM_BLOCK_ENERGY_KVAH);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 24:		// Power On Duration in Minutes for Current Month  
					temp32 = R_USER_GetEMData(POWER_ON_DURATION);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 25:		// Cumulative Tamper Count
					temp32 = R_USER_GetEMData(CUM_TAMPER_CNT);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

#ifdef BIDIR_METER
				case 26:		// Ir - avg 
					temp32 = R_USER_GetEMData(I_R_AVG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 27:		// Iy - avg 
					temp32 = R_USER_GetEMData(I_Y_AVG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 28:		// Ib  - avg
					temp32 = R_USER_GetEMData(I_B_AVG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 29:		// Vr  - avg
					temp32 = R_USER_GetEMData(V_R_AVG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 30:		// Vy  - avg
					temp32 = R_USER_GetEMData(V_Y_AVG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 31:		// Vb  - avg
					temp32 = R_USER_GetEMData(V_B_AVG);					
					pbuffer = (Unsigned8 *)&temp32;
					break;

				case 32:		// Cumulative Energy - kWh  Export
					temp32 = R_USER_GetEMData(CUM_ACTIVE_ENERGY_EXPORT);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 33:		// Cumulative Energy - kvarh(lag)  Export  
					temp32 = R_USER_GetEMData(CUM_REACTIVE_LAG_ENERGY_EXPORT);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 34:		// Cumulative Energy - kvarh(lead)   Export
					temp32 = R_USER_GetEMData(CUM_REACTIVE_LEAD_ENERGY_EXPORT);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				case 35:		// Cumulative Energy - kVAh   Export
					temp32 = R_USER_GetEMData(CUM_APPARENT_ENERGY_EXPORT);					
					pbuffer = (Unsigned8 *)&temp32;
					break;
#endif

#ifdef BIDIR_METER
				case 36:		// high res kwh
#else
				case 26:		// high res kwh
#endif
					temp32 = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 37:		// high res kvah
#else
				case 27:		// high res kvah
#endif
					temp32 = ((myenergydata.kvah%100000)*100)+(int32_t)high_resolution_kvah_value;
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 38:		// high res kvarh_lag -38
#else
				case 28:		// high res kvarh_lag -38
#endif
					temp32 = ((myenergydata.kvarh_lag%100000)*100)+(int32_t)high_resolution_kvarh_lag_value;
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 39:		// high res kvarh_lead
#else
				case 29:		// high res kvarh_lead
#endif
					temp32 = ((myenergydata.kvarh_lead%100000)*100)+(int32_t)high_resolution_kvarh_lead_value;
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 40:		// Cumulative Energy - kwh lag
#else
				case 30:		// Cumulative Energy - kwh lag
#endif

	#ifdef SPLIT_ENERGIES_REQUIRED		
					temp32 = myenergydata.kwh_lag;
  #else
					temp32 = 0;
	#endif
//					temp32 = 12345678;
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 41:		// Cumulative Energy - kwh lead
#else
				case 31:		// Cumulative Energy - kwh lead
#endif
	#ifdef SPLIT_ENERGIES_REQUIRED		
					temp32 = myenergydata.kwh_lead;
	#else
					temp32 = 0;
	#endif
//					temp32 = 87654321;
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 42:		// Cumulative Energy - kVAh lag
#else
				case 32:		// Cumulative Energy - kVAh lag
#endif
	#ifdef SPLIT_ENERGIES_REQUIRED		
					temp32 = myenergydata.kvah_lag;
  #else
					temp32 = 0;
	#endif
	
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
#ifdef BIDIR_METER
				case 43:		// Cumulative Energy - kVAh lead
#else
				case 33:		// Cumulative Energy - kVAh lead
#endif
	#ifdef SPLIT_ENERGIES_REQUIRED		
					temp32 = myenergydata.kvah_lead;
  #else
					temp32 = 0;
	#endif
					
					pbuffer = (Unsigned8 *)&temp32;
					break;
					
				default:
					break;
			}
			// Determind the size, based on buffer  
			if (pbuffer != NULL &&
				size == -1)
			{
				size = strlen((void*)pbuffer);
			}

			// Encode & indicate as success when buffer is valid  
			if (pbuffer !=NULL)
			{
				*p_out_len = R_OBIS_EncodeAttribute(
					p_out_buf,								// Output buffer, pointed to g_ServiceBuffer  
					OBIS_SERVICE_DATA_BUFFER_LENGTH,		// Max length of g_ServiceBuffer  
					p_child_record3->value.choice.type,		// Type  
					(Unsigned8 *)pbuffer,					// Buffer  
					size									// Length  
				);

				// Success  
				response_result = DATA_ACCESS_RESULT_SUCCESS;
			}
		}
		else
		{
			// Do nothing  
		}
	}
#endif
#if (defined(USED_CLASS_04) && USED_CLASS_04 == 1)
//	 * Handle all requests for class 04
//	 * Just do on following attributes only:
//	 * 1. Logical name (OBIS code) of the object.
//	 * 2. Access right definition for 1 object   
//	 * 3  Value                                  
//	 * 4  Scaler, unit                           
//	 * 5  Status                                 
//	 * 6  Capture time buffer                    
//	 * All other attributes are handled in R_OBIS_DistributeRegisterClass() function.
	else if (class_id == 4)
	{
		class04_child_record_t	*p_child_record4;

		// Get the child record  
		p_child_record4 = (class04_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);

		// Attr 2 - value : CHOICE ?  
		if (attr_id == 2)
		{
			Unsigned32 temp_f32;
			void *buff = NULL;

			// Get the buffer pointer  
			switch (child_id)
			{
				case 0:		// Maximum Demand  kW  
					temp_f32 = R_USER_GetEMData(EM_ACTIVE_MD);
					buff = &temp_f32;
					break;
					
				case 1:		// Maximum Demand  kVA  
					temp_f32 = R_USER_GetEMData(EM_APPARENT_MD);
					buff = &temp_f32;
					break;

				case 2:		// Maximum Demand  kW - export  
					temp_f32 = R_USER_GetEMData(EM_ACTIVE_MD_EXPORT);
					buff = &temp_f32;
					break;
					
				case 3:		// Maximum Demand  kVA  - export
					temp_f32 = R_USER_GetEMData(EM_APPARENT_MD_EXPORT);
					buff = &temp_f32;
					break;
					
					
				default:
					break;
			}

			// Encode & indicate as success when buffer is valid  
			if (buff !=NULL)
			{
				*p_out_len = R_OBIS_EncodeAttribute(
					p_out_buf,								// Output buffer, pointed to g_ServiceBuffer  
					OBIS_SERVICE_DATA_BUFFER_LENGTH,		// Max length of g_ServiceBuffer  
					p_child_record4->value.choice.type,		// Type  
					(Unsigned8 *)buff,						// Buffer  
					p_child_record4->value.choice.size		// Length  
				);

				// Success  
				response_result = DATA_ACCESS_RESULT_SUCCESS;
			}
		}
		// Attr 5 - capture_time : octet-string  
		if (attr_id == 5)
		{
			date_time_t temp_time;
			switch (child_id)
			{
				case 0:		// Maximum Demand  kW  
					{
						Unsigned8 rlt;
						// Get time  
						rlt = R_USER_GetSpecificTime(LAST_ACTIVE_MD_TIME, &temp_time);
						if (rlt != 0)
						{
							return (response_result);
						}
					}
					break;
				case 1:		// Maximum Demand  kVA  
					{
						Unsigned8 rlt;
						// Get time  
						rlt = R_USER_GetSpecificTime(LAST_APPARENT_MD_TIME, &temp_time);
						if (rlt != 0)
						{
							return (response_result);
						}
						break;
					}
					
//					break;
				default:
					return response_result;
			}
			*p_out_len = R_OBIS_EncodeAttribute(
						p_out_buf,								// Output buffer, pointed to g_ServiceBuffer  
						OBIS_SERVICE_DATA_BUFFER_LENGTH,		// Max length of g_ServiceBuffer  
						ATTR_TYPE_OCTET_STRING,					// Type  
						(Unsigned8 *)&temp_time,				// Buffer  
						sizeof(date_time_t)						// Length  
						);
			// Success  
			response_result = DATA_ACCESS_RESULT_SUCCESS;
		}
		else
		{
			// Do nothing  
		}
	}
#endif
#if (defined(USED_CLASS_08) && USED_CLASS_08 == 1)
//	 * Handle all requests for class 08
//	 * NOTE: THE CLASS 08 ALLOW ONLY 1 INSTANCE TO BE IMPLEMENTED.
//	 * Just do on following attributes only:
//	 * 1. Attr 2 - time : octet-string.
//	 * 2. Attr 3 - time_zone : long
//	 * 3. Attr 4 - status : unsigned
//	 * 4. Attr 5 - daylight_savings_begin : octet-string
//	 * 5. Attr 6 - daylight_savings_end : octet-string
//	 * 6. Attr 7 - daylight_savings_deviation : integer
//	 * 7. Attr 8 - daylight_savings_enabled : boolean
//	 * 8. Attr 9 - clock_base : enum
//	 * All other attributes are handled in R_OBIS_DistributeClockClass() function.
	else if (class_id == 8)
	{
		// Attr 2 - time : octet-string.  
		if (attr_id == 2)
		{
			date_time_t class8_currenttime;
			Unsigned8 rlt;
			switch (child_id)
			{
				case 0:		// RTC time  
					// Get current time  
					rlt = R_USER_GetSpecificTime(CURRENT_TIME, &class8_currenttime);
					if (rlt != 0)
					{
						return (response_result);
					}

					*p_out_len = R_OBIS_EncodeAttribute(
						p_out_buf,								// Output buffer, pointed to g_ServiceBuffer  
						OBIS_SERVICE_DATA_BUFFER_LENGTH,		// Max length of g_ServiceBuffer  
						ATTR_TYPE_OCTET_STRING,					// Type  
						(Unsigned8 *)&class8_currenttime,	// Buffer  
						sizeof(date_time_t)						// Length  
						);

						// Success  
					response_result = DATA_ACCESS_RESULT_SUCCESS;
					break;
				default:
					break;
			}
		}
		else
		{
			// Do nothing  
		}

	}
#endif
	else
	{
		return SCOPE_ACCESS_VIOLATED;
	}

	return (response_result);
}

Unsigned8 R_OBIS_SetObjectAttr(
	st_Cosem_Attr_Desc *cosem_attr_desc,
	service_type_t service_type,
	Unsigned16 child_id,
	Unsigned8 *p_data,
	Unsigned16 data_len,
	Unsigned8 block_transfer
)
{
	// Result of response, def. is not Matched
	Unsigned8 response_result = TYPE_UNMATCH;

	// Vars used to handle processing  
	Unsigned16 class_id;
	Unsigned16 attr_id;

	// Vars used to encode data  
	void        *pbuffer;
	Unsigned16  length;
	Unsigned8	u8_data;
	Unsigned16  u16_data;
	Unsigned32  u32_data;
	Integer32   f32_data;
	attr_type_t type;

			date_time_t user_time;
			date_time_t current_time;
			int8_t match_result;
			Unsigned8 ret;
	
	
	// Params check  
	if (cosem_attr_desc == NULL ||
		p_data == NULL ||
		data_len == 0)
	{
		return TYPE_UNMATCH;
	}

	// Get information from attr descriptor  
	class_id = (
		((Unsigned16)cosem_attr_desc->Class_ID[0] << 8) |
		((Unsigned16)cosem_attr_desc->Class_ID[1])
	);
	attr_id = cosem_attr_desc->Attr_ID;

	if (0)
	{
		// Do nothing  
	}//RC
	#if (defined(USED_CLASS_01) && USED_CLASS_01 == 1)
//	 * Handle all requests for class 01
//	 * Just do on following attributes only:
//	 * 1. Attr 2 - value : CHOICE
//	 *
//	 * All other attributes are handled in R_OBIS_DistributeDataClass() function.

	else if (class_id == 1)
	{
		class01_child_record_t	*p_child_record1;
		// Get the child record  
		p_child_record1 = (class01_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(class_id, child_id)
		);

		// Attr 2 - value : CHOICE ?  
		if (attr_id == 2)
		{
			Unsigned16 u16;
			Integer16 size;
			void *buffer = NULL;

		u16 = 0;
		length = R_OBIS_DecodeAttribute(
						(Unsigned8 *)&u16,							// value_buf  
						p_child_record1->value.choice.size,			// value_buf_len  
						p_child_record1->value.choice.type,			// attr_type  
						p_data,										// buf  
						OBIS_SERVICE_DATA_BUFFER_LENGTH				// buf_len  
					);

			// Get the buffer pointer  
			switch (child_id)
			{
				case 8:		// Demand Integration Period  
					length = (Unsigned16)R_USER_SetEMData(DEMAND_INTEGRATION_PERIOD, (void *)u16);
					response_result = DATA_ACCESS_RESULT_SUCCESS;
					break;
				case 9:		// Block load capture period  
					length = (Unsigned16)R_USER_SetEMData(PROFILE_CAPTURE_PERIOD, (void *)u16);
					response_result = DATA_ACCESS_RESULT_SUCCESS;
					break;
//				case 11:	// Cumulative tamper count  
//					length = (Unsigned16)R_USER_SetEMData(EM_CUCUMLARIVE_TAMPER_CNT, (void *)&u16);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//				case 12:	// Cumulative billing count  
//					length = (Unsigned16)R_USER_SetEMData(EM_CUCUMLARIVE_BILLING_CNT, (void *)&u16);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//				case 13:	// Cumulative programming count  
//					length = (Unsigned16)R_USER_SetEMData(EM_CUCUMLARIVE_PROGAM_CNT, (void *)&u16);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 20:
//					Set_PP(u16,31);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 21:
//					Set_PP(u16,32);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 22:
//					Set_PP(u16,33);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 23:
//					Set_PP(u16,34);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 24:
//					Set_PP(u16,35);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 25:
//					Set_PP(u16,36);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;
//					
//				case 26:
//					Set_PP(u16,37);
//					response_result = DATA_ACCESS_RESULT_SUCCESS;
//					break;

				default:
					break;
				
			}
		}
	}
#endif
#if (defined(USED_CLASS_08) && USED_CLASS_08 == 1)
//	 * Handle all requests for class 08
//	 *
//	 * NOTE: THE CLASS 08 ALLOW ONLY 1 INSTANCE TO BE IMPLEMENTED.
//	 *
//	 * Just do on following attributes only:
//	 * 1. Attr 2 - time : octet-string.
//	 * 2. Attr 3 - time_zone : long
//	 * 3. Attr 4 - status : unsigned
//	 * 4. Attr 5 - daylight_savings_begin : octet-string
//	 * 5. Attr 6 - daylight_savings_end : octet-string
//	 * 6. Attr 7 - daylight_savings_deviation : integer
//	 * 7. Attr 8 - daylight_savings_enabled : boolean
//	 * 8. Attr 9 - clock_base : enum
//	 *
//	 * All other attributes are handled in R_OBIS_DistributeClockClass() function.
	else if (class_id == 8)
	{
		// Attr 2 - time : octet-string.  
		if (attr_id == 2)
		{
			switch (child_id)
			{
				case 0:		// RTC time  
					length = R_OBIS_DecodeAttribute(
							(Unsigned8 *)&user_time,					// value_buf  
							sizeof(date_time_t),						// value_buf_len  
							ATTR_TYPE_DATE_TIME,						// attr_type  
							p_data,										// buf  
							OBIS_SERVICE_DATA_BUFFER_LENGTH				// buf_len  
						);

					if (length == sizeof(date_time_t) + 2)
					{// gaurav
						user_time.day_of_week=NOT_SPECIFIED;
						user_time.hundredths=NOT_SPECIFIED;
						user_time.deviation_high=NOT_SPECIFIED;
						user_time.deviation_low=NOT_SPECIFIED;
						user_time.clock_status=NOT_SPECIFIED;
						
//						if (R_OBIS_IsValidDateTime((Unsigned8 *)&user_time)== 0) // Invalid
						if (R_OBIS_IsValidDateTime(p_data) == 0) // Invalid
						{
							response_result=ACTION_RESULT_TYPE_UNMATCH;
							break;
						}
						
//----------------------------------------------------------------------	

						ConvertRTCDate(&current_time, &rtc);
						match_result = R_OBIS_CompareDate((Unsigned8 *)&user_time, (Unsigned8 *)&current_time);
#ifndef TESTING						
						if(match_result<0)
						{// given date is in the past - not acceptable
							response_result=ACTION_RESULT_TYPE_UNMATCH;
							break;
						}
#endif						
//----------------------------------------------------------------------	

						// Set current time  
						ret = R_USER_SetSpecificTime(CURRENT_TIME, user_time);
						if(ret != 0)
						{
							return response_result;
						}
						// Success  
						record_transaction_event(RTC_TRANSACTION_MASK);
						response_result = DATA_ACCESS_RESULT_SUCCESS;
					}
					break;
				default:
					break;
			}
		}
		else if (attr_id == 3)
		{
			
		}
		else
		{
			// Do nothing  
		}

	}
#endif
#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)
//	 * Handle all requests for class 01
//	 * Just do on following attributes only:
//	 * 1. Attr 2 - value : CHOICE
//	 *
//	 * All other attributes are handled in R_OBIS_DistributeDataClass() function.
	else if (class_id == 20)
	{
		if (attr_id == 10)
		{
			//Set_PP(0,5);
		}
		
	}
#endif
	else
	{
		return SCOPE_ACCESS_VIOLATED;
	}

	return (response_result);
}

void R_OBIS_DataAsyncServiceRestart(void)
{
	Unsigned8 i;
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
	class07_child_record_t *p_child_record = NULL;

	//R_OBIS_Class07_BlockLoadUpdate_Start();

	// Reset buffer block transfer process  for all  
	for (i = 0; i < *(g_MasterTable[7].child_list_size); i++)
	{
		p_child_record = (class07_child_record_t *)(
			R_OBIS_FindChildRecordByIndex(7, i)
		);
		if(p_child_record != NULL)
		{
			p_child_record->p_buff->async_remain_entries = 0;
		}
	}
#endif

#if (defined(USED_CLASS_64) && USED_CLASS_64 == 1)
	{
		class64_child_record_t *p_child_record64 = NULL;

	// Reset frame counter for all  
		for (i = 0; i < *(g_MasterTable[64].child_list_size); i++)
		{
			p_child_record64 = (class64_child_record_t *)(
				R_OBIS_FindChildRecordByIndex(64, i)
				);
			if(p_child_record64 != NULL)
			{
				*(p_child_record64->p_frame_counter) = 0;
				memset(p_child_record64->p_dedicated_unicast_key, 0, 16);
			}
		}
	}
#endif
}

Unsigned8 R_OBIS_E2PRGetData(
	attr_access_t		*p_attr_access,
	attr_type_t 		attr_type,
	Integer16			attr_size,
	Unsigned8			*p_out_buf,
	Unsigned16			*p_out_len
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
//
//	// Vars used to encode data  
//	Unsigned8       buffer[OBIS_SERVICE_DATA_BUFFER_LENGTH];
//
//	// Params check  
//	if (p_out_buf == NULL ||
//		p_out_len == NULL ||
//		p_attr_access == NULL ||
//		p_attr_access->method != ACCESS_MTD_E2PR)
//	{
//		return TYPE_UNMATCH;
//	}
//
//	if (attr_type != ATTR_TYPE_ARRAY &&
//		attr_type != ATTR_TYPE_STRUCTURE &&
//		attr_type != ATTR_TYPE_COMPACT_ARRAY)
//	{
//		// EEPROM access  
//		// write buffer  
//		if (E2PR_READ(p_attr_access->addr, buffer, p_attr_access->size) == E2PR_OK)
//		{
//			// Success  
//			response_result = DATA_ACCESS_RESULT_SUCCESS;
//		}
//		else
//		{
//			return RD_WRT_DENIED;
//		}
//		// Encode inside  
//		*p_out_len = R_OBIS_EncodeAttribute(
//			p_out_buf,							// Output buffer, pointed to g_ServiceBuffer  
//			OBIS_SERVICE_DATA_BUFFER_LENGTH,	// Max length of g_ServiceBuffer  
//			attr_type,							// Type  
//			buffer,								// Buffer  
//			p_attr_access->size					// Length  
//		);
//
//	}
//	else
//	{
//		// need to call to user distributor ?  
//		return SCOPE_ACCESS_VIOLATED;
//	}

	return (response_result);
}

Unsigned8 R_OBIS_E2PRSetData(
	attr_access_t		*p_attr_access,
	attr_type_t 		attr_type,
	Integer16			attr_size,
	Unsigned8			*pdata
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
//	Unsigned16 decoded_length;
//
//	// Vars used to encode data  
//	Unsigned8       buffer[OBIS_SERVICE_DATA_BUFFER_LENGTH];
//
//	// Params check  
//	if (p_attr_access == NULL ||
//		p_attr_access->method != ACCESS_MTD_E2PR)
//	{
//		return TYPE_UNMATCH;
//	}
//
//	if (attr_type != ATTR_TYPE_ARRAY &&
//		attr_type != ATTR_TYPE_STRUCTURE &&
//		attr_type != ATTR_TYPE_COMPACT_ARRAY)
//	{
//		// Decode attribute  
//		decoded_length = R_OBIS_DecodeAttribute(
//			buffer,									// Output value buffer  
//			p_attr_access->size,					// Size of buffer  
//			attr_type,								// Type  
//			pdata,									// Get from data pointer of HDLC protocol stack  
//			OBIS_SERVICE_DATA_BUFFER_LENGTH			// Encoded length from HDLC  
//		);
//
//		// EEPROM access  
//		// write buffer  
//		if (E2PR_WRITE(p_attr_access->addr, buffer, p_attr_access->size) == E2PR_OK)
//		{
//			// Success  
//			response_result = DATA_ACCESS_RESULT_SUCCESS;
//		}
//		else
//		{
//			response_result = RD_WRT_DENIED;
//		}
//
//
//	}
//	else
//	{
//		// need to call to user distributor ?  
//	}

	return (response_result);
}
#if (defined(USED_CLASS_26) && USED_CLASS_26 == 1)
Unsigned8 R_OBIS_BufferBlockGet(
	void				*buffer,
	attr_access_t		*p_attr_access,
	Unsigned16			block_index,
	Unsigned16			block_size,
	Unsigned8			*p_out_buf,
	Unsigned16			*p_out_len
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
	Unsigned8			*p_temp_buf;

	// Params check  
	if (p_out_buf == NULL ||
		p_out_len == NULL)
	{
		return response_result;
	}

	*p_out_len = 0;

	if (p_attr_access == NULL)
	{
		if (buffer != NULL)
		{
			p_temp_buf = (Unsigned8 *)buffer;
			p_temp_buf += (block_size * block_index);
			memcpy(p_out_buf, p_temp_buf, block_size);
		}
		else
		{
			return response_result;
		}
	}
	else
	{
		switch (p_attr_access->method)
		{
			case ACCESS_MTD_MEM:
				p_temp_buf = (Unsigned8 *)buffer;
				p_temp_buf += (block_size * block_index);
				memcpy(p_out_buf, p_temp_buf, block_size);
				break;
			case ACCESS_MTD_E2PR:
				if (E2PR_READ(p_attr_access->addr + (block_index * block_size), p_out_buf, block_size) != E2PR_OK)
				{
					return RD_WRT_DENIED;
				}
				break;
			case ACCESS_MTD_USERFUNC:
				// TODO: append code here  
				break;
			default:
				break;
		}
	}

	// Success  
	*p_out_len = block_size;

	return DATA_ACCESS_RESULT_SUCCESS;
}

Unsigned8 R_OBIS_BufferBlockSet(
	void				*buffer,
	attr_access_t		*p_attr_access,
	Unsigned16			block_index,
	Unsigned16			block_size,
	Unsigned8			*p_in_buf,
	Unsigned16			*p_in_len
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
	Unsigned8 *p_temp_buf;

	// Params check  
	if (p_in_buf == NULL ||
		p_in_len == NULL)
	{
		return response_result;
	}

	*p_in_len = 0;

	if (p_attr_access == NULL)
	{
		if (buffer != NULL)
		{
			p_temp_buf = (Unsigned8 *) buffer;
			p_temp_buf += (block_index * block_size);
			memcpy(p_temp_buf, p_in_buf, block_size);
		}
		else
		{
			return response_result;
		}
	}
	else
	{
		switch (p_attr_access->method)
		{
			case ACCESS_MTD_MEM:
				p_temp_buf = (Unsigned8 *) buffer;
				p_temp_buf += (block_index * block_size);
				memcpy(p_temp_buf, p_in_buf, block_size);
				break;
			case ACCESS_MTD_E2PR:
				if (E2PR_WRITE(p_attr_access->addr + (block_index * block_size), p_in_buf, block_size) != E2PR_OK)
				{
					return RD_WRT_DENIED;
				}
				break;
			case ACCESS_MTD_USERFUNC:
				// TODO: append code here  
				break;
			default:
				break;
		}
	}

	// Success  
	*p_in_len = block_size;

	return DATA_ACCESS_RESULT_SUCCESS;
}
#endif

Unsigned8 R_OBIS_BufferScanByUserFunc (
	buffer_t            *buf,
	Unsigned8           selective_access,
	Unsigned16          master_index,
	Unsigned16          child_index,
	Unsigned8           *p_selector_buffer
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
	//
//	 * Temporary if (0) to prevent compile error
//	 * when no class is used
	  
	if (0)
	{
		// Do nothing  
	}
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
	else if (master_index == 7)
	{
		response_result = R_OBIS_Class07_BufferScan(
								buf,								// [In] Pointer to buffer_t                       
								selective_access,					// [In] Selective access                          
								child_index,						// [In] Id of the row in child list               
								p_selector_buffer					// [In] Pointer to the selector (from Client)     
							);
	}
#endif
#if (defined(USED_CLASS_26) && USED_CLASS_26 == 1)
	else if (master_index == 26)
	{
		response_result = R_OBIS_Class26_BufferScan(
								buf,								// [In] Pointer to buffer_t                       
								selective_access,					// [In] Selective access                          
								child_index,						// [In] Id of the row in child list               
								p_selector_buffer					// [In] Pointer to the selector (from Client)     
							);
	}
#endif
	else
	{
		// Do nothing  
	}
	return response_result;
}

Unsigned8 R_OBIS_BufferFilterByUserFunc (
	buffer_t            *buf,
	Unsigned16          master_index,
	Unsigned16          child_index,
	Unsigned16          request_entries_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;
//	 * Temporary if (0) to prevent compile error
//	 * when no class is used
	if (0)
	{
		// Do nothing  
	}
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
	else if (master_index == 7)
	{
		response_result = R_OBIS_Class07_BufferFilter(
								buf,								// [In] Pointer to buffer_t                 
								child_index,						// [In] Child ID                            
								request_entries_nr,					// [In] Number of entries need to filter    
								p_out_buf,							// [Out] Data pointer for get               
								p_out_len							// [Out] Data length pointer for get        
							);
	}
#endif
#if (defined(USED_CLASS_26) && USED_CLASS_26 == 1)
	else if (master_index == 26)
	{
		response_result = R_OBIS_Class26_BufferFilter(
								buf,								// [In] Pointer to buffer_t                 
								child_index,						// [In] Child ID                            
								request_entries_nr,					// [In] Number of entries need to filter    
								p_out_buf,							// [Out] Data pointer for get               
								p_out_len							// [Out] Data length pointer for get        
							);
	}
#endif
	else
	{
		// Do nothing  
	}
	return response_result;
}

#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
Unsigned8 R_OBIS_Class07_ScanByRange(
	Integer16               restricting_object_id,
	Unsigned16              child_index,
	Unsigned8               *p_selector_buffer
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;

	// Vars to decode selective access selector  
	void        *pbuffer = NULL;
	Unsigned16  i, length;
	Unsigned16  actual_index;
	Unsigned16  decoded_length;
	Unsigned16  current_entry, entries_in_use, max_entries;
	class07_child_record_t  *p_child_record;
	buffer_t                *buf;

	date_time_t from_date_time;
	date_time_t to_date_time;
	uint32_t range_start_entry;
	uint32_t range_end_entry;
	
	
	 // Find OBIS object in child table by visibility, obis code, access right.	  
	p_child_record = (class07_child_record_t *)R_OBIS_FindChildRecordByIndex(7, child_index);

	// Params check  
	if ((restricting_object_id<0)||(restricting_object_id >= *p_child_record->p_capture_obj_length)||(p_selector_buffer== NULL))
		return response_result;

	// Get buffer pointer  
	buf = (buffer_t *)p_child_record->p_buff;

		
// The first step is to extract the from_date_time and to_date_time from the p_selector_buffer
// These are date_time_t structures which are to be copied from p_selector_buffer to 
// from_date_time and to_date_time which happen to be global variables for some reason

	R_USER_WDT_Restart();
	decoded_length = R_OBIS_DecodeAttribute(
		(Unsigned8 *)&from_date_time,			// destination value_buf  
		sizeof(date_time_t),					// destination value_buf_len  
		ATTR_TYPE_DATE_TIME,					// attr_type  
		p_selector_buffer,						// input buffer data stream  
		OBIS_SERVICE_DATA_BUFFER_LENGTH			// buf_len  
	);
	p_selector_buffer += decoded_length;	// move the pointer to next portion of buffer to be read/processed.

	// decode to_date_time  
	decoded_length = R_OBIS_DecodeAttribute(
		(Unsigned8 *)&to_date_time,				// value_buf  
		sizeof(date_time_t),					// value_buf_len  
		ATTR_TYPE_DATE_TIME,					// attr_type  
		p_selector_buffer,						// buf  
		OBIS_SERVICE_DATA_BUFFER_LENGTH			// buf_len  
	);
	p_selector_buffer += decoded_length;	
	
	
// it is required to convert the time range entries into index range entries
// the functions find_num* in the block below should compute the values of range_end_entry and range_start_entry

	if (child_index == 2)	// Block Load Profile
		find_num_between_lp_entries(&from_date_time, &to_date_time, &range_start_entry, &range_end_entry);
//		find_num_between_lp_entries();
		
	else if (child_index == 4)	// Daily Load Profile
		find_num_between_dlp_entries(&from_date_time, &to_date_time, &range_start_entry, &range_end_entry);
//		find_num_between_dlp_entries();

	buf->to_entry   = range_end_entry;//USER
	buf->from_entry = range_start_entry;// USER
	response_result = DATA_ACCESS_RESULT_SUCCESS;

	return response_result;

// The entire following code has been commented - as found
// The commented code was occupying a lot of text space - hence it has been deleted
// It may be referrred to in Copy(5) or earlier
	
}

Unsigned8 R_OBIS_Class07_BufferScan (
	buffer_t            *buf,
	Unsigned8           selective_access,
	Unsigned16          child_index,
	Unsigned8           *p_selector_buffer
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;

	// Vars value after scan  
	Unsigned16 					i;
	Unsigned16					entries_in_use;
	void 						*pbuffer = NULL;
	class07_child_record_t		*p_child_record;
	Unsigned16					decoded_length;

	// Params check  
	if (buf == NULL)
	{
		return response_result;
	}

//	 * Find OBIS object in child table by
//	 * visibility, obis code, access right.
//
	p_child_record = (class07_child_record_t *)R_OBIS_FindChildRecordByIndex(7, child_index);

	// Get current info of buffer  
	entries_in_use = (Unsigned16)*p_child_record->p_entries_in_use;

	// Default: select all entries, columns
	// the index refers to the number of rows of data available - essentially the number of entries
	// the value index refers to the columns - hence from column to column will allow part of the data to be got
	// this will include the case of getting data for a single column
	buf->from_entry = 1;
	buf->to_entry   = entries_in_use;
	buf->from_value = 1;
	buf->to_value   = *p_child_record->p_capture_obj_length; // All column  

	buf->async_remain_entries   = entries_in_use;

	if (selective_access == 0)
	{
		// select all entries, columns  
		return DATA_ACCESS_RESULT_SUCCESS;
	}
	else if (selective_access == 1)
	{
	// check the access selector  
		if (*(p_selector_buffer) == 0x01)		// by range  
		{
			// Vars to decode data from selector buffer  
			Integer16 restricting_object_id = -1;
//			* Decode the range descriptor
//			*
//			* range_descriptor ::= structure
//			* {
//			*     restricting_object: capture_object_definition
//			*     from_value : CHOICE(string), CHOICE(Integer32) or CHOICE(Integer32)
//			*     from_value : CHOICE(string), CHOICE(Integer32) or CHOICE(Integer32)
//			*     selected_values : array(null)
//			* }

			// Skip the access selector: p_selector_buffer++  
			// Skip ATTR_TYPE_STRUCTURE, Length: p_selector_buffer += 2  
			p_selector_buffer += 3;

			// Decode 4 elements  
			{
//				* 1. restricting_object: capture_object_definition
//				*
//				* capture_object_definition ::= structure
//				* {
//				*     class_id: long-unsigned
//				*     logical_name: octet-string
//				*     attribute_index: integer
//				*     data_index: long-unsigned
//				* }
//
				// Skip ATTR_TYPE_STRUCTURE, Length: p_selector_buffer += 2  
				p_selector_buffer += 2;
				{
//					// 1. Skip class_id (ATTR_TYPE_LONG_UNSIGNED): p_selector_buffer += 3   
//					// 2. Point to logical_name  
//					// Skip ATTR_TYPE_OCTET_STRING, Length: p_selector_buffer += 2  
					p_selector_buffer += 5;

					pbuffer = p_selector_buffer;

//					// Skip logical_name: p_selector_buffer += 6  
//					// 3. Skip attribute_index (ATTR_TYPE_INTEGER): p_selector_buffer += 2  
//					// 4. Skip data_index (ATTR_TYPE_LONG_UNSIGNED): p_selector_buffer += 3  
					p_selector_buffer += 11;
				}

//				* Seach the type which indicated
//				* in capture_object_definition

				for (i = 0; i < *p_child_record->p_capture_obj_length; i++)
				{
					if (R_OBIS_CompareObisCode(
							(Unsigned8 *)pbuffer,
							p_child_record->p_capture_objects[i].logical_name
						) == 1)
					{
						restricting_object_id = (Integer16)i;
						break;
					}
				}

				// when not found, return as not filter any thing  
				if (i >= *p_child_record->p_capture_obj_length)
				{
					buf->from_entry = 0;
					buf->to_entry   = 0;
				}
				else
				{
//					// Filter the entry list based on selected capture object
//					* Decode from_entry, to_entry, from_value, to_value
					response_result = R_OBIS_Class07_ScanByRange(
						restricting_object_id,          // [In] Restricting object id                  
						child_index,                    // [In] Id of the row in child list            
						p_selector_buffer               // [In] Pointer to the selector (from Client)  
					);
				}
			}
		}
		else if (*(p_selector_buffer) == 0x02)	// by entry  
		{
//			* Decode the entry descriptor
//			*
//			* entry_descriptor ::= structure
//			* {
//			*     from_entry : double-long-unsigned
//			*     to_entry : double-long-unsigned
//			*     from_selected_value : long-unsigned
//			*     to_selected_value : long-unsigned
//			* }
			// Skip the access selector: p_selector_buffer++  
			// Skip ATTR_TYPE_STRUCTURE, Length: p_selector_buffer += 2  
			p_selector_buffer += 3;

			// 1. from_entry : double-long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&buf->from_entry,				// value_buf  
				4,											// value_buf_len  
				ATTR_TYPE_DOUBLE_LONG_UNSIGNED,				// attr_type  
				p_selector_buffer,							// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH				// buf_len  
			);
			p_selector_buffer += decoded_length;

			// 2. to_entry : double-long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&buf->to_entry,			// value_buf  
				4,										// value_buf_len  
				ATTR_TYPE_DOUBLE_LONG_UNSIGNED,			// attr_type  
				p_selector_buffer,						// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH			// buf_len  
			);
			p_selector_buffer += decoded_length;

			// 3. from_selected_value : long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&buf->from_value,			// value_buf  
				2,										// value_buf_len  
				ATTR_TYPE_LONG_UNSIGNED,				// attr_type  
				p_selector_buffer,						// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH			// buf_len  
			);
			p_selector_buffer += decoded_length;

			// 4. to_selected_value : long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&buf->to_value,			// value_buf  
				2,										// value_buf_len  
				ATTR_TYPE_LONG_UNSIGNED,				// attr_type  
				p_selector_buffer,						// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH			// buf_len  
			);
			p_selector_buffer += decoded_length;
		}

//		* exceptions for to_entry
//		* when its value is 0, select to highest
		if (buf->to_entry == 0)
		{
			if(buf->from_entry == 0)
			{
				// exceptions for no entry case  
				buf->async_remain_entries = 0;
				return DATA_ACCESS_RESULT_SUCCESS;
			}
			else
			{
				buf->to_entry = entries_in_use;
			}
		}
//		* exceptions for to_value
//		* when its value is 0, select to highest
		if (buf->to_value == 0)
		{
			buf->to_value = (Unsigned16) *p_child_record->p_capture_obj_length;
		}
		
		if (buf->from_value == 0)
		{
			buf->from_value = 1;
		}

		// Success 
		response_result = DATA_ACCESS_RESULT_SUCCESS;

		if (buf->from_entry == 0 || 
			buf->to_entry < buf->from_entry ||
			buf->from_value == 0 || 
			buf->to_value < buf->from_value)
		{
			// Error  
			return SCOPE_ACCESS_VIOLATED;
		}

		// Calculate Number of entries need to filter  
		buf->async_remain_entries = (Unsigned16)(buf->to_entry - buf->from_entry + 1);
	}

	return response_result;
}

Unsigned8 R_OBIS_Class07_BufferFilter (
	buffer_t            *buf,
	Unsigned16          child_index,
	Unsigned16          request_entries_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
)
{
	// Vars to access to entry  
	Unsigned16					total_entry;
	Unsigned16					index;
	Integer32					async_index;
	Unsigned16					current_entry, entries_in_use, max_entries, capture_obj_length;
	Unsigned16					length = 0;
	class07_child_record_t		*p_child_record;
	Unsigned8					temp_len;

//	 * Find OBIS object in child table by
//	 * visibility, obis code, access right.
	p_child_record = (class07_child_record_t *)R_OBIS_FindChildRecordByIndex(7, child_index);

	// Get current info of buffer  
	current_entry          = *buf->p_current_buf_index;
	entries_in_use         = (Unsigned16)*p_child_record->p_entries_in_use;
	max_entries            = (Unsigned16)*p_child_record->p_profile_entries;
	capture_obj_length     = (Unsigned16)*p_child_record->p_capture_obj_length;

	// Start encoding  
	*p_out_len = 0;

	if (request_entries_nr == 0)
	{
		// Null array  
		*p_out_buf++ = ATTR_TYPE_ARRAY;
		*p_out_buf++ = 0;
		*p_out_len   = 2;

		return DATA_ACCESS_RESULT_SUCCESS;
	}

	// Check range  
	if (buf->from_entry > entries_in_use)
	{
		// Over-range: customize HERE, default: return error   
		return TYPE_UNMATCH;
	}
	if (buf->to_entry > entries_in_use)
	{
		// Over-range: customize HERE, select to highest   
		buf->to_entry = entries_in_use;
		buf->async_remain_entries = (Unsigned16)(buf->to_entry - buf->from_entry + 1);
	}

	// Calculate asyn offset for this transfer   
	async_index = buf->to_entry - buf->from_entry + 1; // Max entries  
	async_index -= buf->async_remain_entries;

	if (async_index < 0)
	{
		// Error, need to debug here  
		return TYPE_UNMATCH;
	}

//	* Determine this is first transfer or not ?
//	* by check: async_index == 0 ?
	if (async_index == 0)
	{
		if ((capture_obj_length < buf->to_value) || (capture_obj_length < buf->from_value))
		{
			// Over-range: customize HERE, default: return error   
			return TYPE_UNMATCH;
		}

		// encode the array header  
		*p_out_buf++ = ATTR_TYPE_ARRAY;
		temp_len = BERLengthEncode(p_out_buf, buf->async_remain_entries);
		p_out_buf += temp_len;
		*p_out_len  += temp_len + 1;

		if (child_index == 2)
		{
			// Mark to start async service, pause the capturing  
			R_OBIS_Class07_BlockLoadUpdate_Pause();
		}
		else
		{
			// TODO : start async service for other objects  
		}
	}
//	 * Encode for object of class 07
//	 * Based on following info:
//	 * 1. buf->from_entry,
//	 * 2. buf->to_entry,
//	 * 3. buf->from_value,
//	 * 4. buf->to_value
	// access to the list of selected entries (FIFO)  
	//index  = ((current_entry + max_entries) - entries_in_use) + (Unsigned16)(buf->from_entry + async_index) - 1;
	//index  %= (max_entries);
	

	index  =  (Unsigned16)(buf->from_entry + async_index) - 1;
	
	// Content of array  
	total_entry = request_entries_nr;
	while (total_entry > 0)
	{
		switch(child_index)
		{
			case 0: // Instantaneous parameters snapshot  
			{
				length = R_OBIS_Class07_FilterOneInstEntry(buf, index, p_out_buf, p_out_len);
			}
			break;
			case 2: // Block Load Profile  
			{
				length = R_OBIS_Class07_FilterOneBlockLoadEntry(buf, index, p_out_buf, p_out_len);
			}
			break;
			case 4: // Daily Load Scaler Profile  
			{
				length = R_OBIS_Class07_FilterOneDailyLoadEntry(buf, index, p_out_buf, p_out_len);
			}
			break;
			case 6: // Billing profile  
			{
				length = R_OBIS_Class07_FilterOneBillingEntry(buf, index, p_out_buf, p_out_len);
			}
			break;
			case 9:  // Event 00 profile  
			case 10: // Event 01 profile  
			case 11: // Event 02 profile  
			case 12: // Event 03 profile  
			case 13: // Event 04 profile  
			case 14: // Event 05 profile  
			{
				//length = R_OBIS_Class07_FilterOneEventEntry(buf, index, p_out_buf, p_out_len);
				length = R_OBIS_Class07_FilterOneEventEntry(buf,child_index, index, p_out_buf, p_out_len);
			}
			break;
			case 1: // Instantaneous Scaler Profile  
			case 3: // Block Load Scaler Profile  
			case 5: // Daily Load Scaler Profile  
			case 7: // Billing Scaler Profile  
			case 8: // Event 00 Scaler Profile  
			{
				length = R_OBIS_Class07_FilterOneScalerEntry(buf, index,
											*p_child_record->p_capture_obj_length,
											p_out_buf, p_out_len);
			}
			break;
			case 15: // Name plate profile  
			{
				length = R_OBIS_Class07_FilterOneNamePlateEntry(buf, index, p_out_buf, p_out_len);
			}
			break;
			default:
			{
				// TODO : encode for other objects  
				return TYPE_UNMATCH;
			}
//			break;
		}

		// count down entries counter  
		total_entry--;
		// next entry  
		p_out_buf += length;
		index  = (index + 1) % max_entries;
	}

//	* Determine this is last transfer or not ?
//	* by check: request_entries_nr >= async_remain_entries
	if (request_entries_nr >= buf->async_remain_entries)
	{
		if (child_index == 2)
		{
			// Last transfer, can continue capturing  
			R_OBIS_Class07_BlockLoadUpdate_Start();
		}
		else
		{
			// TODO : start async service for other objects  
		}
	}

	return DATA_ACCESS_RESULT_SUCCESS;
}
#endif // (defined(USED_CLASS_07) && USED_CLASS_07 == 1)  

#if (defined(USED_CLASS_26) && USED_CLASS_26 == 1)
Unsigned8 R_OBIS_Class26_BufferScan (
	buffer_t            *buf,
	Unsigned8           selective_access,
	Unsigned16          child_index,
	Unsigned8           *p_selector_buffer
)
{
	// Result of response, def. is not Matched  
	Unsigned8 response_result = TYPE_UNMATCH;

	// Vars value after scan  
	Unsigned16 					i;
	Unsigned32					offset_value = 0;
	Unsigned16					count_value = 0;
	Unsigned8					len_value = 0;
	Unsigned32					entries_in_use;
	void 						*pbuffer = NULL;
	class26_child_record_t				*p_child_record;
	Unsigned16					decoded_length;

	// Params check  
	if (buf                   == NULL)
	{
		return response_result;
	}

//	 * Find OBIS object in child table by
//	 * visibility, obis code, access right.
	p_child_record = (class26_child_record_t *)R_OBIS_FindChildRecordByIndex(26, child_index);

	// Default: select all entries  
	buf->from_entry = 1;						// Offset  
	buf->to_entry   = *p_child_record->p_len;	// Offset + Count  

	buf->async_remain_entries   = (Unsigned16)*p_child_record->p_len;

	if (selective_access == 0)
	{
		// select all entries, columns  
		return DATA_ACCESS_RESULT_SUCCESS;
	}
	else if (selective_access == 1)
	{
		// check the access selector  
		if (*(p_selector_buffer) == 0x01)		// by offset_access  
		{
//			* Decode the offset_selector descriptor
//			*
//			* offset_selector ::= structure
//			* {
//			*     Offset:  double-long-unsigned
//			*     Count:  long-unsigned
//			* }
			// Skip the access selector  
			p_selector_buffer++;
			// Skip ATTR_TYPE_STRUCTURE, Length  
			p_selector_buffer += 2;
			// 1. Offset:  double-long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&offset_value,					// value_buf  
				sizeof(Unsigned32),							// value_buf_len  
				ATTR_TYPE_DOUBLE_LONG_UNSIGNED,				// attr_type  
				p_selector_buffer,							// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH				// buf_len  
			);
			p_selector_buffer += decoded_length;

			// 2.  Count:  long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&count_value,					// value_buf  
				sizeof(Unsigned16),							// value_buf_len  
				ATTR_TYPE_LONG_UNSIGNED,					// attr_type  
				p_selector_buffer,							// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH				// buf_len  
			);
			p_selector_buffer += decoded_length;
		}
		else if (*(p_selector_buffer) == 0x02)	// by index_selector  
		{
//			* Decode the index_selector descriptor
//			*
//			* entry_descriptor ::= structure
//			* {
//			*     Index:  array of long-unsigned
//			*     Count:  long-unsigned
//			* }
			// Skip the access selector  
			p_selector_buffer++;
			// Skip ATTR_TYPE_STRUCTURE, Length  
			p_selector_buffer += 2;
			// 1. Index:  array of long-unsigned  
			// Skip ATTR_TYPE_ARRAY  
			p_selector_buffer ++;
			// Get Length  
			len_value = *p_selector_buffer++;

			for (i = 0; i < len_value; i++)
			{
				offset_value += *p_selector_buffer++;
			}
			// 2. Count:  long-unsigned  
			decoded_length = R_OBIS_DecodeAttribute(
				(Unsigned8 *)&count_value,				// value_buf  
				sizeof(Unsigned32),						// value_buf_len  
				ATTR_TYPE_DOUBLE_LONG_UNSIGNED,			// attr_type  
				p_selector_buffer,						// buf  
				OBIS_SERVICE_DATA_BUFFER_LENGTH			// buf_len  
			);
			p_selector_buffer += decoded_length;
		}
		else
		{
			// Do nothing  
			count_value = 0;
		}

		buf->from_entry = offset_value + 1;						// Offset  
		buf->to_entry   = buf->from_entry + count_value;		// Offset + Count  
	}
	return DATA_ACCESS_RESULT_SUCCESS;
}

Unsigned8 R_OBIS_Class26_BufferFilter (
	buffer_t            *buf,
	Unsigned16          child_index,
	Unsigned16          request_entries_nr,
	Unsigned8           *p_out_buf,
	Unsigned16          *p_out_len
)
{
     // Vars to access to entry  
     Unsigned16                    index, i;
     Integer32                    async_index;
     class26_child_record_t        *p_child_record;
     Unsigned8                    temp_ret;
     Unsigned16                    temp_len;

     //
      * Find OBIS object in child table by
      * visibility, obis code, access right.
       
     p_child_record = (class26_child_record_t *)R_OBIS_FindChildRecordByIndex(26, child_index);

     // Start encoding  
     *p_out_len = 0;

     if (request_entries_nr == 0)
     {
         // Null octect string  
         *p_out_buf++ = ATTR_TYPE_OCTET_STRING;
         *p_out_buf++ = 0;
         *p_out_len   = 2;

         return DATA_ACCESS_RESULT_SUCCESS;
     }

     // Calculate asyn offset for this transfer   
     async_index = (buf->to_entry - buf->from_entry + 1)- buf->async_remain_entries; // Max entries  

     if (async_index < 0)
     {
         // Error, need to debug here  
         return TYPE_UNMATCH;
     }
     else if (async_index == 0)
     {
         // First transfer  
         // encode the octect string header  
         *p_out_buf++ = ATTR_TYPE_OCTET_STRING;
         *p_out_buf++ = (Unsigned8)buf->async_remain_entries;
         *p_out_len  += 2;
     }

     // access to the list of selected entries (FIFO)  
     // index  = buf->from_entry + ((buf->to_entry - buf->from_entry + 1)- buf->async_remain_entries)  
     index  =  (Unsigned16)(buf->to_entry - buf->async_remain_entries + 1);

     // Content of array  
     for (i = 0; i < request_entries_nr; i++)
     {
         temp_ret = R_OBIS_BufferBlockGet(
             p_child_record->p_buff->p_buffer,    	// [In] pointer to buffer list     
             p_child_record->p_buff->p_access,    	// [In] Access method option     
             index,                    				// [In] Block index in buffer     
             1,                    					// [In] Block size in byte  
             p_out_buf,                				// [Out] Data pointer for get     
             &temp_len                				// [Out] length pointer for get     
             );
         if (temp_ret != DATA_ACCESS_RESULT_SUCCESS)
         {
             return temp_ret;
         }
         *p_out_len  += temp_len;
         p_out_buf += temp_len;
         index++;
     }
     // Successful  
     return DATA_ACCESS_RESULT_SUCCESS;
}
#endif // (defined(USED_CLASS_26) && USED_CLASS_26 == 1)  
