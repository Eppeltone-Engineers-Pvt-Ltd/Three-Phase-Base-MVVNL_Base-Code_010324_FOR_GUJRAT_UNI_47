// emeter3_structs.h
#ifndef EMETER3_STRUCTS_H
#define EMETER3_STRUCTS_H

#include "board.h"


#define TEMP_MAX_POWER	10800000	// 3*240*150*100
#define MAX_POWER	108000	// 3*240*150

//#define KWH_DIVIDER		3600	
#define KWH_DIVIDER		360000	// this is to accomodate 5 decimals - 11/Dec/2019
#define HIGH_RES_DIVIDER	3600	// this is to get the 4th and 5th decimals
#define KWH_INCREMENT	1000
#define TOD
// #define TOD_HAS_MD_SUPPORT	// not used any more
#define LOAD_SURVEY

//#define REACTIVE_PULSING	// shifted to board.h

// PCB Version 1.6 - 01/Nov/2019
#define MDRST_SW	(P0.2)	// MD reset - Bottom switch
#define MIDDLESW	(P15.2)	// Reverse jaane wali - Middle switch
#define TOPSW	(P13.7)	// forward jaane wali	- Top Most switch



//#define TESTING

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

struct toddata_struct		// size = 64
{
  int32_t kwh;
  int32_t kvah;
  int32_t kvarh_lag;
  uint16_t mdkw;
  uint16_t mdkva;
  ext_rtc_t mdkw_datetime;
  ext_rtc_t mdkva_datetime;

#ifdef BIDIR_METER
  int32_t kwh_export;
  int32_t kvah_export;
  //int32_t kvarh_lag_export;
  uint16_t mdkw_export;
  uint16_t mdkva_export;
  ext_rtc_t mdkw_export_datetime;
  ext_rtc_t mdkva_export_datetime;
#endif

  uint8_t ZoneDefLowerByte;
  uint8_t ZoneDefUpperByte;
  uint8_t dummy;
  uint8_t chksum;
};

typedef struct toddata_struct toddata_t;


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


#ifdef BIDIR_METER
struct energydata_struct
{// size 62
  int32_t kwh;
  int32_t kwh_fundamental;	// introduced for Gujarat meter

	int32_t kvah;
  int32_t kvarh_lag;
  int32_t kwh_export;
  int32_t kvah_export;
  int32_t kvarh_lag_export;
  uint32_t powerOnminutes_eb;
//  uint32_t powerOnminutes_dg;
	int32_t kwh_nonSolar;
	
  int32_t zone_kwh;  // consumption
  int32_t zone_kvah;  // consumption
  int32_t zone_kvarh_lag;  // consumption
  int32_t zone_kwh_export;  // consumption
  int32_t zone_kvah_export;  // consumption
  int32_t zone_kvarh_lag_export;  // consumption
  uint8_t dummy;
  uint8_t chksum;
};
#else
struct energydata_struct
{// size 58
  int32_t kwh;
//  int32_t kwh_eb;
  int32_t kwh_fundamental;	// introduced for Gujarat meter
  int32_t kwh_dg;
  int32_t kvah;
  int32_t kvah_eb;
  int32_t kvah_dg;
  int32_t kvarh_lead;
  int32_t kvarh_lag;
  uint32_t powerOnminutes_eb;
  uint32_t powerOnminutes_dg;
  int32_t zone_kwh;  // consumption
  int32_t zone_kvah;  // consumption
  int32_t zone_kvarh_lead;  // consumption
  int32_t zone_kvarh_lag;  // consumption
  uint8_t dummy;
  uint8_t chksum;
};
#endif
typedef struct energydata_struct energydata_t;

#ifdef BIDIR_METER
struct mddata_struct
{// size 52
//  int32_t kwh_previous_bill; // used to compute consumption
//  int32_t kvah_previous_bill;  // used to compute consumption
  uint16_t mdkw;
  uint16_t mdkva;
  uint16_t tamperCount; // every month
  ext_rtc_t mdkw_datetime;
  ext_rtc_t mdkva_datetime;
  ext_rtc_t billing_datetime;
  uint16_t mdkw_export;
  uint16_t mdkva_export;
  ext_rtc_t mdkw_export_datetime;
  ext_rtc_t mdkva_export_datetime;
  uint8_t dummy;
  uint8_t chksum;
};
#else
struct mddata_struct
{// size 32
//  int32_t kwh_previous_bill; // used to compute consumption
//  int32_t kvah_previous_bill;  // used to compute consumption
  uint16_t mdkw;
  uint16_t mdkva;
  uint16_t tamperCount; // every month
  ext_rtc_t mdkw_datetime;
  ext_rtc_t mdkva_datetime;
  ext_rtc_t billing_datetime;
  uint8_t dummy;
  uint8_t chksum;
};
#endif
typedef struct mddata_struct mddata_t;

// in r_dlms_data.h there is a class07_billing_entry_t
// the above type uses 12 bytes for every date time entry - whereas we could do with 8
// since 7(6+1) or 13(12+1) bills need to be stored, it can be a significant amount of e2rom

#ifdef BIDIR_METER
struct billdata_struct
{// 32*4(int32) + 29*8(rtc) + 30*2(int16) = (128 + 232 + 60) bytes * 13= 420*13 = bytes
	ext_rtc_t bill_date;	// 8 bytes
	int16_t		power_factor;			// System power factor for billing
	int32_t		active_energy;			// Cumulative Energy – kWh  
	int32_t		active_energyTZ[6];		// Cumulative Energy kWh TZ1 ~ TZ8  
	int32_t		kvarh_lag_value;		// Cumulative Energy, kvarh (lag)  
	int32_t		kvarh_lead_value;		// Cumulative Energy, kvarh (lead)   
	int32_t		apparent_energy;		// Cumulative Energy – kVAh  
	int32_t		apparent_energyTZ[6];		// Cumulative Energy kVAh TZ1 ~ TZ8findzone
	uint16_t		active_MD;			// MD kW  
	ext_rtc_t		active_MD_DT;			// MD kW Date Time 
	uint16_t		active_MD_TZ[6];			// MD kW  
	ext_rtc_t		active_MD_DT_TZ[6];			// MD kW Date Time 
	uint16_t		apparent_MD;			// MD kVA  
	ext_rtc_t		apparent_MD_DT;			// MD kVA Date Time 
	uint16_t		apparent_MD_TZ[6];			// MD kVA  
	ext_rtc_t		apparent_MD_DT_TZ[6];			// MD kVA Date Time 
	uint16_t		total_power;			// Billing power ON duration in minutes (During Billing period)
	
	int32_t		active_energy_export;			// Cumulative Energy – kWh  
	int32_t		active_energyTZ_export[6];		// Cumulative Energy kWh TZ1 ~ TZ8  
	int32_t		kvarh_lag_value_export;		// Cumulative Energy, kvarh (lag)  
	int32_t		kvarh_lead_value_export;		// Cumulative Energy, kvarh (lag)  
	int32_t		apparent_energy_export;		// Cumulative Energy – kVAh  
	int32_t		apparent_energyTZ_export[6];		// Cumulative Energy kVAh TZ1 ~ TZ8findzone
	uint16_t		active_MD_export;			// MD kW  
	ext_rtc_t		active_MD_DT_export;			// MD kW Date Time 
	uint16_t		active_MD_TZ_export[6];			// MD kW  
	ext_rtc_t		active_MD_DT_TZ_export[6];			// MD kW Date Time 
	uint16_t		apparent_MD_export;			// MD kVA  
	ext_rtc_t		apparent_MD_DT_export;			// MD kVA Date Time 
	uint16_t		apparent_MD_TZ_export[6];			// MD kVA  
	ext_rtc_t		apparent_MD_DT_TZ_export[6];			// MD kVA Date Time 
};	
#else
struct billdata_struct
{// 20*4(int32) + 19*8(rtc) + 20*2(int16) = (80 +152 + 40) bytes * 13= 3536 bytes
	ext_rtc_t bill_date;	// 8 bytes
	int16_t		power_factor;			// System power factor for billing
	int32_t		active_energy;			// Cumulative Energy – kWh  
	int32_t		active_energyTZ[8];		// Cumulative Energy kWh TZ1 ~ TZ8  
	int32_t		kvarh_lag_value;		// Cumulative Energy, kvarh (lag)  
	int32_t		kvarh_lead_value;		// Cumulative Energy, kvarh (lead)   
	int32_t		apparent_energy;		// Cumulative Energy – kVAh  
	int32_t		apparent_energyTZ[8];		// Cumulative Energy kVAh TZ1 ~ TZ8findzone
	uint16_t		active_MD;			// MD kW  
	ext_rtc_t		active_MD_DT;			// MD kW Date Time 
	uint16_t		active_MD_TZ[8];			// MD kW  
	ext_rtc_t		active_MD_DT_TZ[8];			// MD kW Date Time 
	uint16_t		apparent_MD;			// MD kVA  
	ext_rtc_t		apparent_MD_DT;			// MD kVA Date Time 
	uint16_t		apparent_MD_TZ[8];			// MD kVA  
	ext_rtc_t		apparent_MD_DT_TZ[8];			// MD kVA Date Time 
	uint16_t		total_power;			// Billing power ON duration in minutes (During Billing period)
};
#endif

typedef struct billdata_struct billdata_t;



struct ts_ctldata
{// size 80
  uint32_t powerOnminutes_eb;
//  uint32_t powerOnminutes_dg;
  
	int32_t kwh_last_saved; // used to compute MD
  int32_t kvah_last_saved; // used to compute MDKVA
	
#ifdef BIDIR_METER
	int32_t kwh_last_saved_export; // used to compute MD
  int32_t kvah_last_saved_export; // used to compute MDKVA
#endif	
	
  int32_t last_saved_kwh; // used to compute consumption - needed for load survey
  int32_t last_saved_kvah; // used to compute consumption - needed for load survey
  int32_t last_saved_kvarh_lag; // used to compute consumption - needed for load survey
  int32_t last_saved_kvarh_lead; // used to compute consumption - needed for load survey

#ifdef BIDIR_METER
  int32_t last_saved_kwh_export; // used to compute consumption - needed for load survey
#endif

#ifdef GUJARAT_BOARD
	uint32_t cumulative_md;
	uint32_t cumulative_md_kva;
#endif

#ifdef BIDIR_METER
	int32_t yesterdaysKWH;  // for daily log
  int32_t yesterdaysKWH_export;  // for daily log
#else
	int32_t yesterdaysKWH;  // for daily log
  int32_t yesterdaysKVAH;  // for daily log
#endif    
	
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


#ifdef GUJARAT_BOARD
#ifdef BIDIR_METER
	struct surveydata_struct
	{// 30 bytes
		ext_rtc_t datetime;	// 8 bytes
		uint16_t I_r;		// Current  
		uint16_t I_y;		// Current  	
		uint16_t I_b;		// Current  	
		uint16_t V_r;		// Average Voltage 
		uint16_t V_y;		// Average Voltage 
		uint16_t V_b;		// Average Voltage 
		uint16_t kwh_consumption;
		uint16_t kvarh_lag_consumption;
		uint16_t kvarh_lead_consumption;
		uint16_t kvah_consumption;
		uint16_t kwh_consumption_export;
	};
	#else
	struct surveydata_struct
	{// 4 bytes - being made 32 bytes by adding two dummies
		uint16_t kwh_consumption;
		uint16_t kvah_consumption;
	};
	#endif
#endif

#ifdef PUNJAB_BOARD
struct surveydata_struct
{// 28 bytes
	ext_rtc_t datetime;	// 8 bytes
	uint16_t I_r;		// Current  
	uint16_t I_y;		// Current  	
	uint16_t I_b;		// Current  	
	uint16_t V_r;		// Average Voltage 
	uint16_t V_y;		// Average Voltage 
	uint16_t V_b;		// Average Voltage 
	uint16_t kwh_consumption;
	uint16_t kvarh_lag_consumption;
	uint16_t kvarh_lead_consumption;
	uint16_t kvah_consumption;
//	uint16_t dummy1;
//	uint16_t dummy2;
};
#endif
typedef struct surveydata_struct surveydata_t;

#define BLOCK_KWH_OFFSET				20
#define BLOCK_KVARH_LAG_OFFSET	22
#define BLOCK_KVARH_LEAD_OFFSET	24
#define BLOCK_KVAH_OFFSET				26



#ifdef BIDIR_METER
struct logdata_struct
{// 16 bytes
	ext_rtc_t datetime;	// 8 bytes
	int32_t kWh_value;		// kWh at 00:00 hours  
	int32_t kWh_value_export;		// kWh export at 00:00 hours
};
#else
struct logdata_struct
{// 16 bytes
	ext_rtc_t datetime;	// 8 bytes
	int32_t kWh_value;		// kWh at 00:00 hours  
	int32_t kVAh_value;		// kVAh at 00:00 hours
};
#endif
typedef struct logdata_struct logdata_t;


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
  uint16_t ct_ratio;  // this has been amde an int now
  uint8_t comm_ver[2];
  uint8_t mfgname[8];
  uint8_t unitname[8];
  uint8_t batchmonth;
  uint8_t batchyear;
  uint8_t meter_calibrated;
  uint8_t history_ptr; // this will go from 1 to 12 and will be used to store the bill in the requisite offset locations
  uint8_t mdinterval;
  uint8_t billing_date;
  uint8_t CtRatio; // not used any more underscore removed and C R capitalised
  uint8_t pt_ratio;
  uint8_t survey_interval;
  uint8_t meter_id;  // for rs485
  uint8_t rb;  // whatever this is - if this is 8 then all phases to be connected. (1-None,2-B,3-Y,4-Y&B,5-R,6-R&B,7-R&Y,8-all 
  uint8_t sampling_time;  // in seconds
  uint8_t reconnect_time;  // in seconds
  uint8_t cutoffMode_eb; // 0 kw, 85 - kva
  uint8_t cutoffMode_dg; // 0 kw, 85 - kva
  uint8_t dailyOverLoadEvents; // no of events permitted per day
  uint8_t dummy;
  uint8_t chksum;
};

typedef struct common_data_struct commondata_t;

struct tmprdata_struct		// size = 44 bytes
{// allocate 64 bytes
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
	uint8_t currentBypassCount;
	uint8_t currentReversalCount;
	uint8_t neutralDisturbanceCount;
	uint8_t magnetTamperCount;
  uint8_t chksum;
};
typedef struct tmprdata_struct tmprdata_t;


struct eventdata_struct		// size = 32
{
	ext_rtc_t	datetime;
	uint16_t	current_value_IR;
	uint16_t	current_value_IY;
	uint16_t	current_value_IB;
	uint16_t	voltage_value_VR;
	uint16_t	voltage_value_VY;
	uint16_t	voltage_value_VB;
	int16_t	power_factor_R;	
	int16_t	power_factor_Y;
	int16_t	power_factor_B;
	int32_t		active_energy;
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
extern int32_t total_apparent_power;
extern int32_t total_reactive_power_lag;
extern int32_t total_reactive_power_lead;
extern int32_t total_reactive_power;

extern int32_t total_energy_lastSaved;  //uint32_t
extern int32_t zone_kwh_last_saved;   // uint32_t
extern int32_t zone_kvah_last_saved;  //uint32_t
extern int32_t reactive_energy_lead_lastSaved;//uint32_t
extern int32_t reactive_energy_lag_lastSaved;//uint32_t
extern int32_t apparent_energy_lastSaved;//uint32_t

extern int32_t total_consumed_high_res_energy;


extern uint16_t genflags;
// genflags bit definitions
#define SECONDS_FLAG      BIT0
#define MINUTES_FLAG      BIT1
#define BATTERY_GONE      BIT2
#define LATCHBIT          BIT3
#define EMERGENCY_SUPPLY  BIT4
#define AC_MAGNET_APPLIED BIT5
#define NEUTRAL_MISSING   BIT6
#define SUPPLY_PRESENT    BIT7
#define DISPLAYONDEMAND   BIT8
#define RC_TAMPER         BIT9
#define HIGH_RESOLUTION_MODE BIT10
//#define APPARENT_LOGABLE  BIT11
//#define REACTIVE_LOGABLE  BIT12
//#define GENERATOR_MODE  BIT13
#define D_TAMPER          BIT14



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
extern uint8_t secret1[9];//="ABCDEFGH";
extern uint8_t secret2[17];//="RENESAS_P6wRJ21F";

extern uint16_t average_Vr;
extern uint16_t average_Vy;
extern uint16_t average_Vb;
extern uint16_t average_Ir;
extern uint16_t average_Iy;
extern uint16_t average_Ib;
extern uint16_t	g_Class01_EventCode[7];
extern const uint32_t maskTable[];

extern uint8_t	high_resolution_kwh_value;
extern uint8_t	high_resolution_kvah_value;
extern uint8_t	high_resolution_kvarh_lag_value;
extern uint8_t	high_resolution_kvarh_lead_value;
extern uint8_t 	high_resolution_kwh_fundamental_value;
extern uint8_t coverOpenCtr;

extern int32_t	testResult;
extern uint8_t fault_code;
#ifdef GUJARAT_BOARD
	extern int32_t gujaratSecondsCounter;
	extern uint8_t sub_disp_state;
	extern uint8_t comm_mode;
	extern uint8_t commModeTimer;
#endif
extern uint8_t gentimer;
extern uint8_t tamperTimer;

#endif