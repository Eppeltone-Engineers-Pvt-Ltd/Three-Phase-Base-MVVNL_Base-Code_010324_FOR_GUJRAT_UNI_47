// emeter3_structs.h
#ifndef EMETER3_STRUCTS_H
#define EMETER3_STRUCTS_H

#include "board.h"

#ifdef BIDIR_METER
	#define TARIFFS 5	// Bidir meters are permitted only 5 tariffs
#else
	#define TARIFFS 8	// UNIDIR meters are permitted 8 tariffs
#endif

#define PCB_2_5		// Commenting this will produce code for PCB_2.3 

//#define CTL_FAULT_DEBUG	// Please comment this for actual use - only to test 

//#define RECOVERENERGIES	// used for testing only - MUST BE ALWAYS COMMENTED otherwise

//#define MAGTEST	// used to debug MAG icon glowing - to be commented in actual use

//#define TESTING

// The following #define is being shifted to the individual board definitions in board.h
//#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating

//#define CALSW				~(P3.0)	// for testing only
#define CALSW				(P3.0)	// for actual use



#ifdef PCB_2_5
// RTCOUT is P15.0 and RTC-BKUP is P6.2 which in our code has been made P12.5
	#define DC_MAGNET_SENSOR	(P5.0)	
	#define AC_MAGNET_SENSOR (P5.4)	// ac magnet
	#define MAKE_RTC_SUPPLY_LOW 	(P12&=~0x20)
	#define MAKE_RTC_SUPPLY_HIGH	(P12|=0x20)
//	#define MAKE_RTC_SUPPLY_LOW 	(P6&=~0x04)
//	#define MAKE_RTC_SUPPLY_HIGH	(P6|=0x04)
#else
// in PCB_2_3 P15.0 is used as RTC-BKUP, and RTCOUT is P6.2 (alternate output requires bit PIOR03 to be made 1
	#define HALLSENSOR1	(P12.5)		// - INPUT
	#define HALLSENSOR2	(P5.4)		// - INPUT
	#define HALLSENSOR3	(P4.3)		// - INPUT
	#define HALLSENSOR4	(P5.0)		// - INPUT
	#define AC_MAGNET_SENSOR (P4.2)	// ac magnet	- INPUT
	
	#define MAKE_RTC_SUPPLY_LOW 	(P15&=~0x01)
	#define MAKE_RTC_SUPPLY_HIGH	(P15|=0x01)
#endif

#define CURRENT_PHASE_SEQUENCE
#define FREQ_DIST_CHECK

#ifdef BILL_TYPE_MARKING
// various bill types
	#define BT_AUTO		0
	#define BT_MANU		1
	#define BT_TRANS	2
#endif	
	
#ifdef VARIABLE_FREQUENCY_BILLING
// billing month - applicable for automatic billing
	#define BM_NONE			0	// Billing month has not been set by user - use the batchmonth to decide odd or even
	#define BM_ODD			1	// Billing month has been programmed as ODD
	#define BM_EVEN			2	// Billing month has been programmed as EVEN
	#define BM_MONTHLY	3	// Billing month has been programmed as MONTHLY
#endif

#ifdef PROGRAMMABLE_DISPLAY
	#define AUTOSCROLL_OFFSET	0
	#define PUSHBUTTON_OFFSET 28
	#define HIGHRES_OFFSET 118

	#define AUTOSCROLL_SIZE 27
	#define PUSHBUTTON_SIZE 89
	#define HIGHRES_SIZE 9
	
	#define DSP_ARRAY_SIZE	128
#endif

#define IMPORT 0
#define EXPORT 1

#define BILL_KWH_ENERGY					0
#define BILL_KVAH_ENERGY				1
#define BILL_KVARH_LAG_ENERGY		2
#define BILL_KVARH_LEAD_ENERGY	3
#define BILL_KVARH_TOTAL_ENERGY	4

#define BILL_MD_KW	0
#define BILL_MD_KVA	1

#ifdef SIX_DECIMALS
	#define TEMP_MAX_POWER	72000000	// 3*240*100*1000
#else
	#define TEMP_MAX_POWER	7200000	// 3*240*100*100
#endif
#define MAX_POWER	72000	// 3*240*100

//#define KWH_DIVIDER		3600	

#ifdef SIX_DECIMALS
	#define KWH_DIVIDER		3600000	// this is to accomodate 6 decimals - 07/Jun/2020
	#define HIGH_RES_DIVIDER	3600	// this is to get the 4th,5th, and 6th decimals
#else
	#define KWH_DIVIDER		360000	// this is to accomodate 5 decimals - 11/Dec/2019
	#define HIGH_RES_DIVIDER	3600	// this is to get the 4th and 5th decimals
#endif

#ifdef KERALA_UNIDIR
	#define KWH_INCREMENT	100
#else
	#define KWH_INCREMENT	1000
#endif

// PCB Version 1.6 - 01/Nov/2019
#define MDRST_SW	(P0.2)	// MD reset - Bottom switch
#define MIDDLESW	(P15.2)	// Reverse jaane wali - Middle switch
#define TOPSW	(P13.7)	// forward jaane wali	- Top Most switch


// definitions for groups of instantaneous variables
#define IVOLTS	0
#define ICURRENTS	1
#define IPFS	2
#define IFREQS	3
#define ACT_POWER	4
#define APP_POWER	5
#define REACT_POWER_LEAD	6
#define REACT_POWER_LAG	7
#define PHASE_CURRENT_VECT 8
#define PFSIGNVAL 9
#define REACT_POWER_SIGNED 10


#define BIT0                (0x00000001u)
#define BIT1                (0x00000002u)
#define BIT2                (0x00000004u)
#define BIT3                (0x00000008u)
#define BIT4                (0x00000010u)
#define BIT5                (0x00000020u)
#define BIT6                (0x00000040u)
#define BIT7                (0x00000080u)
#define BIT8                (0x00000100u)
#define BIT9                (0x00000200u)
#define BIT10                (0x00000400u)
#define BIT11                (0x00000800u)
#define BIT12                (0x00001000u)
#define BIT13                (0x00002000u)
#define BIT14                (0x00004000u)
#define BIT15                (0x00008000u)
#define BIT16                (0x00010000u)
#define BIT17                (0x00020000u)
#define BIT18                (0x00040000u)
#define BIT19                (0x00080000u)
#define BIT20                (0x00100000u)
#define BIT21                (0x00200000u)
#define BIT22                (0x00400000u)
#define BIT23                (0x00800000u)
#define BIT24                (0x01000000u)
#define BIT25                (0x02000000u)
#define BIT26                (0x04000000u)
#define BIT27                (0x08000000u)
#define BIT28                (0x10000000u)
#define BIT29                (0x20000000u)
#define BIT30                (0x40000000u)
#define BIT31                (0x80000000u)


/*
typedef struct
{
  uint16_t autoscrollInButtonMode	:1;     // if flag is 1, autoscroll parameters can be advanced using the button
  uint16_t seconds_flag       		:1;     // flag set every second
  uint16_t mode_has_been_changed  :1;     // flag to indicate mode has been changed
  uint16_t fwdKeyPressed    			:1;     // flag to indicate fwd key pressed
  uint16_t revKeyPressed       		:1;     // flag to indicate rev key pressed 
  uint16_t someKeyPressed        	:1;     // flag to indicate somekey pressed
  uint16_t fast_display_flag      :1;     // flag to indicate one second display update
  uint16_t prevTopSW         			:1;     // flag to store previous value of TopSW
  uint16_t prevMiddleSW 					:1;     // flag to store previous value of MiddleSW
	uint16_t currentSW							:1;			// flag to store cuurent value of any switch
	uint16_t holdState							:1;			// if this flag is set then disp_state is not incremented
	uint16_t tglbit									:1;			// this bit toggles every second
	uint16_t meterTypeChanged				:1;			// this flag is set when meterType has been changed
}display_int_flag_t;
*/

typedef struct
{
    uint16_t is_rtc_int         :1;     // Flag to detect RTC wake up source 
    uint16_t is_cover_int       :1;     // Flag to detect COVER OPEN wake up source
    uint16_t is_key_int         :1;     // Flag to detect KEY press 
    uint16_t is_magnetic_int    :1;     // Flag to detect MAGNETIC sensor 
    uint16_t is_vbatt_low       :1;     // Flag to detect VBAT Voltage Low 
    uint16_t is_vrtc_low        :1;     // Flag to detect VRTC Voltage Low 
    uint16_t is_ac_low          :1;     // Flag to detect AC Power Low 
    uint16_t is_vdd_low         :1;     // Flag to detect AC Power Low 
    uint16_t is_neutral_missing :1;     // Flag to detect Neutral Missing
		uint16_t is_hw_key_int			:1;			// This flag is set when there is an actual hardware interrupt key press
		uint16_t bijliIsThere				:1;			// This flag is set upon entering Power Mode 1
} powermgmt_int_flag_t;

struct extern_rtc_struct
{
  uint8_t second,minute,hour,day,date,month,year;
  uint8_t Control;
};

typedef struct extern_rtc_struct ext_rtc_t;

struct timeinfo_struct
{// this must be used like this only
  uint8_t minute;
  uint8_t hour;
  uint8_t day; // dummy variable
  uint8_t date;
  uint8_t month;	// 1 - 12
  uint8_t year;
};

typedef struct timeinfo_struct timeinfo_t;

#ifdef BIDIR_METER
	struct toddata_struct		// size = 64
	{
	  int32_t kwh;
	  int32_t kvah;
#ifdef TOD_HAS_KVARH_LAG		
	  int32_t kvarh_lag;
#endif		
	  uint16_t mdkw;
	  uint16_t mdkva;
	  ext_rtc_t mdkw_datetime;
	  ext_rtc_t mdkva_datetime;
	  int32_t kwh_export;
	  int32_t kvah_export;
	  uint16_t mdkw_export;
	  uint16_t mdkva_export;
	  ext_rtc_t mdkw_datetime_export;
	  ext_rtc_t mdkva_datetime_export;
	  uint8_t dummy;
	  uint8_t chksum;
	};
#else
	struct toddata_struct		// size = 32 or 36(for TOD_HAS_KVARH_LAG)
	{
	  int32_t kwh;
	  int32_t kvah;
	  uint16_t mdkw;
	  uint16_t mdkva;
	  ext_rtc_t mdkw_datetime;
	  ext_rtc_t mdkva_datetime;
	#ifdef TOD_HAS_KVARH_LAG	
	  int32_t kvarh_lag;
	#endif	
	  uint8_t dummy;
	  uint8_t chksum;
	};
#endif

typedef struct toddata_struct toddata_t;
/*
struct zonedef_struct		// size = 18
{
#ifdef SIXZONES
	uint16_t ZoneDefinition[6];	// zone times are given in minutes from 0:0 to 23:59
#else
	uint16_t ZoneDefinition[8];	// zone times are given in minutes from 0:0 to 23:59
#endif
	uint8_t dummy;
  uint8_t chksum;
};
typedef struct zonedef_struct zoneDefs_t;
*/

struct energydata_struct
{// size to be determined for different cases
  int32_t kwh;
	#ifdef FUNDAMENTAL_ENERGY_REQUIRED		
  int32_t kwh_fundamental;	// introduced for Gujarat meter
	#endif		
	int32_t kvah;
	#ifdef SPLIT_ENERGIES_REQUIRED		
  int32_t kwh_lag;	// lag value stored separately
  int32_t kwh_lead;	// lead value stored separately
	int32_t kvah_lag;	// lag value stored separately
	int32_t kvah_lead;	// lead value stored separately	
	#endif		
  int32_t kvarh_lag;
  int32_t kvarh_lead;

	#ifdef BIDIR_METER
  int32_t kwh_export;
  int32_t kwh_fundamental_export;
  int32_t kvah_export;
  int32_t kvarh_lag_export;
  int32_t kvarh_lead_export;
	#endif

  int32_t powerOnminutes_eb;
  int32_t cumTamperDuration;// duration of all tampers till date (in minutes?)
	#ifdef DAILY_POWER_ON_LOGGING	
	int32_t dailyPowerOnMinutes;
	#endif
	#ifdef BIDIR_METER
	int32_t kwh_nonSolar_export;
	#endif	
  int32_t zone_kwh;  // consumption
  int32_t zone_kvah;  // consumption
	#ifdef TOD_HAS_REACTIVE_ENERGIES		
  int32_t zone_kvarh_lag;  // consumption
  int32_t zone_kvarh_lead;  // consumption
	#endif
	#ifdef BIDIR_METER
	int32_t zone_kwh_export;  // consumption
  int32_t zone_kvah_export;  // consumption
  int32_t zone_kvarh_lag_export;  // consumption
  int32_t zone_kvarh_lead_export;  // consumption
	int32_t kwh_Solar_import;
	int32_t kwh_Solar_export;
	#endif
  uint8_t dummy;
  uint8_t chksum;
};
typedef struct energydata_struct energydata_t;

struct mddata_struct
{// size 52
  uint16_t mdkw;
  uint16_t mdkva;
  uint16_t tamperCount; // every month
  ext_rtc_t mdkw_datetime;
  ext_rtc_t mdkva_datetime;
  ext_rtc_t billing_datetime;
#ifdef BIDIR_METER	
  uint16_t mdkw_export;
  uint16_t mdkva_export;
  ext_rtc_t mdkw_datetime_export;
  ext_rtc_t mdkva_datetime_export;
#endif	
  uint8_t dummy;
  uint8_t chksum;
};
typedef struct mddata_struct mddata_t;

// in r_dlms_data.h there is a class07_billing_entry_t
// the above type uses 12 bytes for every date time entry - whereas we could do with 8
// since 7(6+1) or 13(12+1) bills need to be stored, it can be a significant amount of e2rom

struct ts_ctldata
{// size 72
  int32_t powerOnminutes_eb;
//  uint32_t powerOnminutes_dg;
  
	int32_t kwh_last_saved; // used to compute MD
  int32_t kvah_last_saved; // used to compute MDKVA
	
#ifdef BIDIR_METER
	int32_t kwh_last_saved_export; // used to compute MD
  int32_t kvah_last_saved_export; // used to compute MDKVA
#endif

  int32_t last_saved_kwh; // used to compute consumption - needed for load survey
  int32_t last_saved_kvah; // used to compute consumption - needed for load survey

#ifdef SURVEY_USES_REACTIVE_DATA	
  int32_t last_saved_kvarh_lag; // used to compute consumption - needed for load survey
  int32_t last_saved_kvarh_lead; // used to compute consumption - needed for load survey
#endif

#ifdef BIDIR_METER
  int32_t last_saved_kwh_export; // used to compute consumption - needed for load survey
  int32_t last_saved_kvah_export; // used to compute consumption - needed for load survey
  int32_t last_saved_kvarh_lag_export; // used to compute consumption - needed for load survey
  int32_t last_saved_kvarh_lead_export; // used to compute consumption - needed for load survey
#endif
	uint32_t cumulative_md;
	uint32_t cumulative_md_kva;

#ifdef BIDIR_METER	
	uint32_t cumulative_md_export;
	uint32_t cumulative_md_kva_export;
#endif
//	int32_t yesterdaysKWH;  // for daily log - don't appear to be used
//  int32_t yesterdaysKWH_export;  // for daily log - don't appear to be used
	
  uint32_t survey_wrt_ptr;
  
  uint32_t energies_wrt_ptr;
//	uint16_t rtc_wrt_ptr;
  uint32_t daily_log_wrt_ptr;
  uint8_t date; // now this is date
  uint8_t month;
  uint8_t year;
  uint8_t hour;
  uint8_t minute;
  uint8_t overflow;
  uint8_t energySource;  // 0 - grid, 85 is DG
  uint8_t chksum;
};
typedef struct ts_ctldata ctldata_t;

#define SUR_OVERFLOW  BIT0
#define DAILYLOG_OVERFLOW  BIT1
#define HISTORY_OVERFLOW	BIT2
#define FAULT_FLAG	BIT6


struct ts_ctldata_backup
{// size 40 - max
  int32_t powerOnminutes_eb;
	uint32_t cumulative_md;
	uint32_t cumulative_md_kva;

#ifdef BIDIR_METER	
	uint32_t cumulative_md_export;
	uint32_t cumulative_md_kva_export;
#endif
	
  uint32_t survey_wrt_ptr;
	uint32_t energies_wrt_ptr;	// figure this out
	uint32_t daily_log_wrt_ptr;
  uint8_t date; 
  uint8_t month;
  uint8_t year;
  uint8_t hour;
  uint8_t minute;
  uint8_t overflow;
  uint8_t energySource;
  uint8_t chksum;
};
typedef struct ts_ctldata_backup ctldataBackup_t;

struct small_surveydata_struct
{// to be computed for each type
// 6 + 6 + 4 + 4 + 2 + 3 + 3 + 2 + 2 = 32 bytes max
	timeinfo_t shortDateTime;	// size =6, 
	
	
	#ifdef SURVEY_USES_AVERAGE_DATA		
	int16_t I_r;		// Current  - 2 decimals
	int16_t I_y;		// Current  	
	int16_t I_b;		// Current
	#endif		
	
	uint16_t kwh_consumption;
	uint16_t kvah_consumption;
	
	#ifdef SURVEY_USES_REACTIVE_DATA		
	uint16_t kvarh_lag_consumption;
	uint16_t kvarh_lead_consumption;
	#endif		
	
	#ifdef SURVEY_USES_EXPORT_DATA		
	uint16_t kwh_consumption_export;
	#endif		
	
	#ifdef SURVEY_USES_AVERAGE_DATA		
	uint8_t V_r;		// Average Voltage - 0 decimals
	uint8_t V_y;		// Average Voltage 
	uint8_t V_b;		// Average Voltage 
	#endif

	#ifdef SURVEY_USES_AVERAGE_PF		
	uint8_t avgPF_r;		// Average pf 
	uint8_t avgPF_y;		// Average pf 
	uint8_t avgPF_b;		// Average pf
	#endif

	#ifdef SURVEY_USES_FREQ_DATA
	uint16_t frequency;
	#endif
	
	#ifdef SURVEY_USES_DEMAND_DATA
	uint16_t apparentDemand;
	uint16_t activeDemand;
	#endif
};
typedef struct small_surveydata_struct small_surveydata_t;

#define BLOCK_KWH_OFFSET				20
#define BLOCK_KVARH_LAG_OFFSET	22
#define BLOCK_KVARH_LEAD_OFFSET	24
#define BLOCK_KVAH_OFFSET				26


#if (defined (TN3P_BOARD) && defined(BIDIR_METER))
struct logdata_struct
{// 40 bytes
	ext_rtc_t datetime;	// 8 bytes
	int32_t kWh_value;		// kWh at 00:00 hours  
	int32_t kVAh_value;		// kVAh at 00:00 hours
	int32_t kVARh_lag_value;
	int32_t kVARh_lead_value;
	
	int32_t kWh_value_export;		// kWh export at 00:00 hours
	int32_t kVAh_value_export;		// kVAh export at 00:00 hours  
	int32_t kVARh_lag_value_export;
	int32_t kVARh_lead_value_export;
};
typedef struct logdata_struct logdata_t;
#endif

#ifdef SMALL_DAILY_LOG
	struct logdata_struct
	{// 16 bytes
		ext_rtc_t datetime;	// 8 bytes
		int32_t kWh_value;		// kWh at 00:00 hours  
	#ifdef BIDIR_METER	
		int32_t kWh_value_export;		// kWh export at 00:00 hours
	#else
		int32_t kVAh_value;		// kWh at 00:00 hours  
	#endif
	};
	typedef struct logdata_struct logdata_t;
#else
	#ifdef DAILY_POWER_ON_LOGGING
		struct logdata_struct
		{// 20 bytes
			ext_rtc_t datetime;	// 8 bytes
			int32_t kWh_value;		// kWh at 00:00 hours  
			int32_t kVAh_value;		// kVAh at 00:00 hours  
			int32_t dailyPonMinutes;		// Power On Minutes for the day at 00:00 hours  
		};
		typedef struct logdata_struct logdata_t;
	#else
		struct logdata_struct
		{// 24 bytes
			ext_rtc_t datetime;	// 8 bytes
			int32_t kWh_value;		// kWh at 00:00 hours  
			int32_t kVAh_value;		// kVAh at 00:00 hours  
			int32_t kVArh_lag_value;		// kVArh_lag at 00:00 hours  
			int32_t kVArh_lead_value;		// kVarh_lead at 00:00 hours  
		};
		typedef struct logdata_struct logdata_t;
	#endif	
#endif

struct common_data_struct
{// size=58 // can be upto 64 bytes
  uint32_t meterNumber;
	uint32_t cumPowerFailDuration;
  uint16_t bpcount;
  uint16_t pgm_count;	// every time rtc is set or md/survey interval changed or tod etc changed this count to increase   
  uint16_t reset_count;
  uint16_t noOfPowerFailures;
  uint16_t overVoltageLimit;  // this number is with 2 decimals.
  uint16_t underVoltageLimit; // this number is with 2 decimals.
	uint16_t rtc_failed;	// this variable name now replaces ct_ratio
//  uint16_t ct_ratio;  // this has been amde an int now - not used anymore
  uint8_t comm_ver[2];
  uint8_t mfgname[8];
  ext_rtc_t meterStartDate;	// when meter is ZAPPED this will capture the rtc
  uint8_t batchmonth;
  uint8_t batchyear;
  uint8_t meter_calibrated;
  uint8_t history_ptr; // this will go from 1 to 12 and will be used to store the bill in the requisite offset locations
  uint8_t mdinterval;
  uint8_t billing_date;
  uint8_t billing_month;	// (0 monthly),(1-odd),(2-even))
//  uint8_t pt_ratio;
  uint8_t autoResetPermitted;	// default value should be 1
  uint8_t survey_interval;
	uint8_t meterType;	// bidirectional(default =0) or unidirectional(1)
	uint8_t manualResetPermitted;	// No (default =0), Yes (1)
	uint8_t lastResetType;	// None(0) - Auto(1) or Manual(2) or CMRI(3)
  uint8_t mdType;	// kw(0) or kva(1) to be displayed
	uint8_t displayProgrammed;	// if 0 the not programmed
	uint8_t new_mdinterval;
	uint8_t new_survey_interval;
	uint8_t led_function;	// pulsing function 0(default - kvar), 1-kva
  uint8_t chksum;
};

typedef struct common_data_struct commondata_t;

struct tmprdetails_data_struct		// size = 9 bytes
{
	ext_rtc_t tamperTime;
	uint8_t tamperCode;
};
typedef struct tmprdetails_data_struct tmprdetails_t;

// even if there are no ifdefs the total size is 60 bytes - hence we can remove the ifdefs and ifndefs
struct tmprdata_struct		// size = 44 bytes
{// allocate 64 bytes
// 5*4 + 7*2 + 8 + 18 = 56 for Gujarat Bidir, 58 for Unidir
  uint32_t tamperEvent_flags;
  uint32_t tamperEvent_on;
  uint32_t tamperEvent_image;
  uint32_t tamperEvent_once;
  uint32_t tamperEvent_overflow;  // the bits here will indicate if the tamper storage area has overflowed


//  uint16_t tamper_event_wrt_ptr; 
  uint16_t tamper_events0_wrt_ptr; // voltage related
  uint16_t tamper_events1_wrt_ptr; // current related
  uint16_t tamper_events2_wrt_ptr; // power failure
  uint16_t tamper_events3_wrt_ptr; // transactions (rtc set, md interval, survey interval, single action schedule, activity calendar (tod))
  uint16_t tamper_events4_wrt_ptr; // others - magnet(permanent/ac/dc), neutral disturbance, very low pf
  uint16_t tamper_events5_wrt_ptr; // non-rollover CoverOpen
	
  uint16_t tamperCount; // this is the cumulative tamper count	
	
	ext_rtc_t latestTamperDateTime; 
	uint8_t latestTamperCode;
	
	uint8_t voltageFailureCount_R;
	uint8_t voltageFailureCount_Y;
	uint8_t voltageFailureCount_B;
	uint8_t currentFailureCount_R;
	uint8_t currentFailureCount_Y;
	uint8_t currentFailureCount_B;
	uint8_t voltageUnbalanceCount;
	uint8_t currentUnbalanceCount;
//#ifdef BIDIR_METER
	uint8_t highVoltageCount;
	uint8_t lowVoltageCount;
//#else
	uint8_t currentBypassCount;
	uint8_t currentReversalCount_R;
//#endif	
	uint8_t neutralDisturbanceCount;
	uint8_t magnetTamperCount;
//	uint8_t unRestoredTampersCount[6];
//#ifndef BIDIR_METER
	uint8_t currentReversalCount_Y;
	uint8_t currentReversalCount_B;
//#endif	
  uint8_t chksum;
};
typedef struct tmprdata_struct tmprdata_t;

/*
struct eventdata_struct		// size = 32, 36
{
	ext_rtc_t	datetime;
	int32_t		active_energy;
#ifdef EVENTDATA_RECORDS_KVAH
	int32_t		apparent_energy;
#endif
	int16_t	current_value_IR;
	int16_t	current_value_IY;
	int16_t	current_value_IB;
	uint16_t	voltage_value_VR;
	uint16_t	voltage_value_VY;
	uint16_t	voltage_value_VB;
	int16_t	power_factor_R;	
	int16_t	power_factor_Y;
	int16_t	power_factor_B;
//	int32_t		active_energy;
	uint8_t	event_code_bit;// contains a number 1 to 26 which corresponds to the bit in the 32 bit tmprdata_t variables
	uint8_t	event_type;	// occurence or restoration
};
*/
// if voltage is stored without decimal then following structure will fit within 32 bytes
struct eventdata_struct
{// 30 bytes max
	ext_rtc_t	datetime;
	int32_t		active_energy;
#ifdef EVENTDATA_RECORDS_KVAH
	int32_t		apparent_energy;
#endif
	int16_t	current_value_IR;
	int16_t	current_value_IY;
	int16_t	current_value_IB;
	uint8_t	voltage_value_VR;
	uint8_t	voltage_value_VY;
	uint8_t	voltage_value_VB;
	int8_t	power_factor_R;	
	int8_t	power_factor_Y;
	int8_t	power_factor_B;
//	int32_t		active_energy;
	uint8_t	event_code_bit;// contains a number 1 to 26 which corresponds to the bit in the 32 bit tmprdata_t variables
	uint8_t	event_type;	// occurence or restoration
};

typedef struct eventdata_struct eventdata_t;



























// external global variables and stuctures

extern toddata_t mytoddata;
//extern zoneDefs_t zoneDefines;	// this is now a local variable - data being stored in TZ_start_time0 array etc.
extern energydata_t myenergydata;
extern mddata_t mymddata;
extern ctldata_t ctldata;
extern commondata_t Common_Data;
extern tmprdata_t mytmprdata;

extern ext_rtc_t rtc;	// this is the rtc used in our app
extern ext_rtc_t capturedRTC;	// this is the rtc used to capture the power fail time



extern int32_t total_active_power;
extern int32_t total_consumed_active_energy;  //uint32_t
extern int32_t total_consumed_active_energy_export;

extern int32_t total_consumed_fundamental_active_energy;

extern int32_t total_energy_lastSaved;  //uint32_t
//extern int32_t zone_kwh_last_saved;   // uint32_t
//extern int32_t zone_kvah_last_saved;  //uint32_t
//extern int32_t zone_kvarh_lag_last_saved;
//extern int32_t zone_kvarh_lead_last_saved;

#ifdef BIDIR_METER
extern int32_t total_energy_lastSaved_export;  //uint32_t
//extern int32_t zone_kwh_last_saved_export;   // uint32_t
//extern int32_t zone_kvah_last_saved_export;  //uint32_t
//extern int32_t zone_kvarh_lag_last_saved_export;
//extern int32_t zone_kvarh_lead_last_saved_export;
#endif

extern int32_t total_reactive_power;
extern int32_t total_reactive_power_lag;
extern int32_t total_reactive_power_lead;
extern int32_t total_consumed_reactive_energy_lag;//uint32_t
extern int32_t total_consumed_reactive_energy_lag_export;
extern int32_t total_consumed_reactive_energy_lead;//uint32_t
extern int32_t total_consumed_reactive_energy_lead_export;

extern int32_t reactive_energy_lead_lastSaved;//uint32_t
extern int32_t reactive_energy_lag_lastSaved;//uint32_t

#ifdef BIDIR_METER
extern int32_t reactive_energy_lead_lastSaved_export;
extern int32_t reactive_energy_lag_lastSaved_export;//uint32_t
#endif

extern int32_t total_apparent_power;
extern int32_t total_consumed_apparent_energy;//uint32_t
extern int32_t total_consumed_apparent_energy_export;
extern int32_t apparent_energy_lastSaved_export;//uint32_t
extern int32_t apparent_energy_lastSaved;

#ifdef SPLIT_ENERGIES_REQUIRED
extern int32_t total_consumed_active_energy_lag;
extern int32_t total_consumed_active_energy_lead;
extern int32_t total_consumed_apparent_energy_lag;
extern int32_t total_consumed_apparent_energy_lead;
#endif

extern int32_t zone_kvarh_lag_last_saved;

#ifdef BIDIR_METER
extern int32_t apparent_energy_lastSaved_export;
#endif

extern int8_t mixedPowers;	// this is 0 if all active energies are of the same sign, else this is 1

//extern int32_t total_consumed_high_res_energy;

extern uint16_t genflags;
// genflags bit definitions
#define SECONDS_FLAG      BIT0
#define MINUTES_FLAG      BIT1
#define SURVEY_INTERVAL_CHANGED	BIT2
#define RTC_IS_STOPPED		BIT3
//#define BATTERY_GONE      BIT2
//#define LATCHBIT          BIT3
//#define EMERGENCY_SUPPLY  BIT4
//#define AC_MAGNET_APPLIED BIT5
//#define NEUTRAL_MISSING   BIT6
//#define SUPPLY_PRESENT    BIT7
//#define DISPLAYONDEMAND   BIT8
//#define RC_TAMPER         BIT9
//#define HIGH_RESOLUTION_MODE BIT10
//#define APPARENT_LOGABLE  BIT11
//#define REACTIVE_LOGABLE  BIT12
//#define GENERATOR_MODE  BIT13
//#define D_TAMPER          BIT14



extern uint16_t e2write_flags;
// e2write_flags bit definitions
#define E2_W_IMPDATA 		BIT0
#define E2_W_CTLDATA    BIT1
#define E2_W_EVENTDATA  BIT3
#define E2_W_CURRENTDATA BIT4
#define E2_W_ZONEDATA 	BIT5


extern uint8_t class20_status;
extern uint8_t class22_status;

extern uint8_t init_done;
extern uint8_t presentZone;
extern uint8_t rtctimer;

extern uint8_t g_TamperStatus;

extern uint8_t g_display_mode;	// 0 - Autoscroll, 1- OnDemand, 2-ThirdDisplay
extern uint8_t disp_state;
extern uint8_t disp_timer;
extern uint8_t disp_autoscroll_interval;
extern uint8_t sleepCommMode;
extern uint8_t sleepCommModeTimer;

extern uint8_t autoscrollInButtonMode;
extern uint8_t alt_autoscroll_mode;
extern uint8_t avoidShowingRepeatParameters;
//extern uint8_t alt_autoscroll_mode;


extern uint8_t secret1[9];//="ABCDEFGH";
extern uint8_t secret2[17];//="RENESAS_P6wRJ21F";

#ifdef SURVEY_USES_AVERAGE_DATA
extern uint16_t average_Vr;
extern uint16_t average_Vy;
extern uint16_t average_Vb;
extern int16_t average_Ir;
extern int16_t average_Iy;
extern int16_t average_Ib;
extern uint16_t nCount;
#endif

#ifdef SURVEY_USES_AVERAGE_PF
extern uint16_t averagePF_r;
extern uint16_t averagePF_y;
extern uint16_t averagePF_b;
#endif

extern uint16_t	g_Class01_EventCode[7];
extern const uint32_t maskTable[];

extern uint8_t	high_resolution_kwh_value;
extern uint8_t	high_resolution_kvah_value;
extern uint8_t	high_resolution_kvarh_lag_value;
extern uint8_t	high_resolution_kvarh_lead_value;
extern uint8_t 	high_resolution_kwh_fundamental_value;
extern uint8_t high_resolution_kwh_value_export;
extern uint8_t	high_resolution_kvah_value_export;
extern uint8_t	high_resolution_kvarh_lag_value_export;
extern uint8_t	high_resolution_kvarh_lead_value_export;

extern int16_t coverOpenCtr;

extern int32_t	testResult;
extern uint16_t fault_code;
extern int32_t gujaratSecondsCounter;
extern uint8_t sub_disp_state;
extern uint8_t comm_mode;
extern uint8_t commModeTimer;
extern uint8_t gentimer;
extern uint8_t displayHold;

extern uint8_t tamperTimer;
//extern int8_t exportMode;
extern int8_t gPhaseSequence;
extern uint8_t startZero;

extern int8_t gPhaseSequenceCurrent;


//extern uint8_t	fast_display_flag;
extern uint8_t gLcdCheck;

#ifdef TESTING
extern int32_t load;
#endif

extern uint8_t g_controlTableIndex;
extern uint8_t g_ControlTable[];

extern int8_t dspArr[];
extern uint8_t g_dsp_progFlag;
extern const int8_t factoryDispPgmBidir[];
extern const int8_t factoryDispPgmUnidir[];

extern uint8_t g_tamper_display_state;
extern uint8_t g_cover_display_state;
extern uint8_t cover_disp_timer;

//extern display_int_flag_t g_display_flag;
extern uint8_t twoWireOperation;
extern int8_t perSecondFlag;	// used for per_second_activity

extern int16_t vR_saved;
extern int16_t vY_saved;
extern int16_t vB_saved;

extern int8_t acMagnetSenseCtr;	
// this counter is reset to 0 whenever a rising edge is detected - it is incremented every second
// and if its value reaches 15 then it implies that ac magnet pulses have stopped coming
// The assumption is that a pulse will come at least once in 10 seconds or so in the worst case
// It has been noticed that even when the acmagnet is on, pulses don't come regularly
// there may be a problem of saturation or something like that
extern int8_t acMagnetSense;	// this is the sensing signal which remains latched for 15 seconds

extern uint8_t fwdKeyPressed;
extern uint8_t revKeyPressed;
extern uint8_t someKeyPressed;
extern uint8_t fast_display_flag;
extern uint8_t autoscrollInButtonMode;

extern powermgmt_int_flag_t g_powermgmt_flag;
extern int16_t acMagnetPulseCtr;
extern int8_t pulsesStartedComingFlag;	// this flag is set on the second falling edge of ac magnet line

extern int8_t mixedPowers;	// this is 0 if all active energies are of the same sign, else this is 1
extern int16_t systemPF;	// this is a signed number between 1000 and -1000

#ifdef SPLIT_ENERGIES_REQUIRED
extern int32_t active_energy_lag_lastSaved;
extern int32_t	active_energy_lead_lastSaved;
extern int32_t	apparent_energy_lag_lastSaved;
extern int32_t	apparent_energy_lead_lastSaved;
#endif

extern uint16_t magnetCtr;
extern	int8_t freqDisturbed;

extern uint8_t failCount;
//extern uint8_t rtcFailed;
extern uint8_t rtcTestFlag;
extern uint8_t rtcTestCounter;

extern int8_t energyIncrementationFlag;
extern uint8_t gCalFlag;

extern int32_t total_active_power_pulsing;	// this is in deciwatts
extern int32_t total_apparent_power_pulsing;
extern int32_t total_reactive_power_pulsing;

extern uint8_t calswitch;


#endif