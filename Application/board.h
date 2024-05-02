#ifndef BOARD_H
#define BOARD_H

// to make boot file search for boot image in pravak_notes.txt

// only one of the following boards must be defined
//#define GUJARAT_BIDIR_LTCT_METER	// use this instead of POINT_FIVE_CLASS_METER
//#define GUJARAT_UNI_DIR_LTCT_METER	
//#define GUJARAT_BI_DIR_METER
#define GUJARAT_UNI_DIR_METER		// we must use a different display file for this case
//#define KERALA_UNIDIR	// this is Unidirectional meter
//#define PVVNL_UNI_DIR_METER
//#define DVVNL_UNI_DIR_METER
//#define JVVNL_UNI_DIR_METER
//#define MVVNL_UNI_DIR_METER // earlier sent using PVVNL_UNI_DIR_METER

// PVVNL_Uni_dir_meter uses its own files as listed below
// tampers_PVVNL.c
// em_display_PVVNL.c

#if defined (PVVNL_UNI_DIR_METER)
	#define CAL_IMAX_CURRENT	60.0
//	#define DVVNL_OPTION	// this is only to be defined for DVVNL_meter
	#define TAMPER_PERSISTENCE_ON  300 // 5 minutes
	#define TAMPER_PERSISTENCE_OFF 120	// 2 minutes
	#define MAGNET_TAMPER_PERSISTENCE_ON  30 // 30 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 30	// 30 seconds
	#define QUICK_FIRST_DISPLAY	// in PVVNL ensures quick turn on of display in the event of cover open
	#define POWER_FAIL_INTERVAL_60	// if not defined then 5 seconds
	#define EVENTDATA_RECORDS_KVAH
	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
  	#define LAST_TAMPER_DETAILS_REQUIRED
	#define CLASS1_METER
	#ifdef DVVNL_OPTION
		#define BOARD_ID	28	// Two digits - from pin code
	#else
		#define BOARD_ID	22	// Two digits - from pin code
	#endif
	#define VERNO			10	// This was sent as 27 for PVVNL - being made 0 now Three digits	- 0 is for test code / samples
	#define METER_CLASS	1	// Class 0.5  or 1
//	#define SPLCODE	LCD_CHAR_U	// Unidirectional,Bidirectional,C (CT meter),T (test code) - NOT USED
	#define MAGNET_IMAX_POWER 14400.0
	#define MAGNET_IMAX_CURRENT 60.0
//	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define DC_INJECTION_COMPENSATION_REQUIRED	// if a good CT is used this is not required
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
//	#define BIDIR_METER
	#define TIMEZONES	8	// this can only be 6 or 8 - no other value permitted
//	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
//	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
//	#define SMALL_DAILY_LOG	// if commented then all energies (unidir) are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
	#define SURVEY_USES_AVERAGE_DATA	
	#define SURVEY_USES_REACTIVE_DATA
//	#define SURVEY_USES_EXPORT_DATA
	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA
	#define SURVEY_USES_DEMAND_DATA
	#define FUNDAMENTAL_ENERGY_REQUIRED
//	#define SPLIT_ENERGIES_REQUIRED
//	#define LAG_ONLY_TARIFF	// This board uses Lead Lag tariff
//	#define TOD_HAS_REACTIVE_ENERGIES
//	#define ND_POWER_ON_ACTUALS	// if not defined Assume voltages to be 240
//	#define TOD_HAS_KVARH_LAG	// define if needed
	#define BILLING_HAS_TAMPER_COUNT	// Bill data also sends cumulative tamper count
#endif


#if defined (GUJARAT_BIDIR_LTCT_METER)
	#define CAL_IMAX_CURRENT	20.0
	#define TAMPER_PERSISTENCE_ON  300 // for this meter some tampers have on time of 15 minutes ie 900 seconds
	#define TAMPER_PERSISTENCE_OFF 300	// 300 seconds
	#define MAGNET_TAMPER_PERSISTENCE_ON  60 // 60 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 60	// 60 seconds
//	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
//	#define CLASS1_METER
	#define BOARD_ID	39	// Two digits - from pin code
	#define VERNO			00	// Three digits - 29 earlier
	#define METER_CLASS	5	// Class 0.5  or 1
	#define MAGNET_IMAX_POWER 2400.0
	#define MAGNET_IMAX_CURRENT 10.0
	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define DC_INJECTION_COMPENSATION_REQUIRED	// if a good CT is used this is not required
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
	#define BIDIR_METER
	#define TIMEZONES	6	// this can only be 6 or 8 - no other value permitted
	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
	#define SMALL_DAILY_LOG	// if commented then all energies (unidir) are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
	#define SURVEY_USES_AVERAGE_DATA
	#define SURVEY_USES_REACTIVE_DATA
	#define SURVEY_USES_EXPORT_DATA
	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA
//	#define SURVEY_USES_DEMAND_DATA
//	#define FUNDAMENTAL_ENERGY_REQUIRED
//	#define SPLIT_ENERGIES_REQUIRED
	#define LAG_ONLY_TARIFF
	#define TOD_HAS_REACTIVE_ENERGIES
//	#define ND_POWER_ON_ACTUALS	// If not defined Assume voltages to be 240
	#define TOD_HAS_KVARH_LAG	// define if needed
//	#define HARMONICS_COMPENSATION_IN_EXPORT_MODE	
//	#define HARMONICS_COMPENSATION_IN_IMPORT_MODE	
#endif

#if defined (GUJARAT_UNI_DIR_LTCT_METER)
	#define CAL_IMAX_CURRENT	20.0
	#define TAMPER_PERSISTENCE_ON  300 // 300 seconds
	#define TAMPER_PERSISTENCE_OFF 300	// 300 seconds
	#define MAGNET_TAMPER_PERSISTENCE_ON  60 // 60 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 60	// 60 seconds
//	#define QUICK_FIRST_DISPLAY	// in PVVNL ensures quick turn on of display in the event of cover open
//	#define POWER_FAIL_INTERVAL_60	// if not defined then 5 seconds
//	#define EVENTDATA_RECORDS_KVAH
//	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
//  #define LAST_TAMPER_DETAILS_REQUIRED
//	#define CLASS1_METER
	#define BOARD_ID	39	// Two digits
	#define VERNO			00	// Three digits - change this
	#define METER_CLASS	5	// Class 0.5  or 1
//	#define SPLCODE	LCD_CHAR_U	// Unidirectional,Bidirectional,C (CT meter),T (test code) - NOT USED
	#define MAGNET_IMAX_POWER 2400.0
	#define MAGNET_IMAX_CURRENT 10.0
	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define DC_INJECTION_COMPENSATION_REQUIRED	// if a good CT is used this is not required
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
//	#define BIDIR_METER
	#define TIMEZONES	8	// this can only be 6 or 8 - no other value permitted
//	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
	#define SMALL_DAILY_LOG	// if commented then all energies (unidir) are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
//	#define SURVEY_USES_AVERAGE_DATA	// unidir meter does not use this
//	#define SURVEY_USES_REACTIVE_DATA
//	#define SURVEY_USES_EXPORT_DATA
//	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA
//	#define SURVEY_USES_DEMAND_DATA
	#define FUNDAMENTAL_ENERGY_REQUIRED
//	#define SPLIT_ENERGIES_REQUIRED
	#define LAG_ONLY_TARIFF
	#define TOD_HAS_REACTIVE_ENERGIES
//	#define ND_POWER_ON_ACTUALS	// if not defined Assume voltages to be 240
	#define TOD_HAS_KVARH_LAG
#endif


// Gujarat_Bi_dir_meter uses its own files as listed below
// tampers.c
// em_display_Gujarat.c

#if defined (GUJARAT_BI_DIR_METER)
	#define CAL_IMAX_CURRENT	60.0
	#define TAMPER_PERSISTENCE_ON  300 // 300 seconds
	#define TAMPER_PERSISTENCE_OFF 300	// 300 seconds
	#define MAGNET_TAMPER_PERSISTENCE_ON  60 // 60 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 60	// 60 seconds
//	#define QUICK_FIRST_DISPLAY	// in PVVNL ensures quick turn on of display in the event of cover open
//	#define POWER_FAIL_INTERVAL_60	// if not defined then 5 seconds
//	#define EVENTDATA_RECORDS_KVAH
	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
//  #define LAST_TAMPER_DETAILS_REQUIRED
	#define CLASS1_METER
	#define BOARD_ID	39	// Two digits - from pin code
	#define VERNO			00	// Three digits - 45 earlier
	#define METER_CLASS	1	// Class 0.5  or 1
//	#define SPLCODE	LCD_CHAR_U	// Unidirectional,Bidirectional,C (CT meter),T (test code) - NOT USED
	#define MAGNET_IMAX_POWER 14400.0
	#define MAGNET_IMAX_CURRENT 60.0
//	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define DC_INJECTION_COMPENSATION_REQUIRED	// if a good CT is used this is not required
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
	#define BIDIR_METER
//	#define TIMEZONES	6	// this can only be 6 or 8 - no other value permitted
//	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
	#define SMALL_DAILY_LOG	// if commented then all energies (unidir) are logged
//	#define DAILY_POWER_ON_LOGGING	// if defined then daily poweron minutes are logged else all energies are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
	#define SURVEY_USES_AVERAGE_DATA
	#define SURVEY_USES_REACTIVE_DATA
	#define SURVEY_USES_EXPORT_DATA
	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA
//	#define SURVEY_USES_DEMAND_DATA
//	#define FUNDAMENTAL_ENERGY_REQUIRED
//	#define SPLIT_ENERGIES_REQUIRED
	#define LAG_ONLY_TARIFF
	#define TOD_HAS_REACTIVE_ENERGIES
//	#define ND_POWER_ON_ACTUALS	// If not defined Assume voltages to be 240
	#define TOD_HAS_KVARH_LAG	// define if needed
//	#define HARMONICS_COMPENSATION_IN_EXPORT_MODE	
//	#define HARMONICS_COMPENSATION_IN_IMPORT_MODE	
//	#define BILLING_HAS_TAMPER_COUNT	// Bill data also sends cumulative tamper count
#endif

// Gujarat_Uni_dir_meter uses its own files as listed below
// tampers.c
// em_display_Gujarat.c

#if defined (GUJARAT_UNI_DIR_METER)
	#define CAL_IMAX_CURRENT	60.0
	#define TAMPER_PERSISTENCE_ON  300 // please make this 300 seconds for actual use
	#define TAMPER_PERSISTENCE_OFF 300	// please make this 300 seconds for actual use
	#define MAGNET_TAMPER_PERSISTENCE_ON  60 // 60 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 60	// 60 seconds
//	#define QUICK_FIRST_DISPLAY	// in PVVNL ensures quick turn on of display in the event of cover open
//	#define POWER_FAIL_INTERVAL_60	// if not defined then 5 seconds
//	#define EVENTDATA_RECORDS_KVAH
	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
//  #define LAST_TAMPER_DETAILS_REQUIRED
	#define CLASS1_METER
	#define BOARD_ID	36	// Two digits - from pin code
	#define VERNO			50	// 37 Three digits
	#define METER_CLASS	1	// Class 0.5  or 1
//	#define SPLCODE	LCD_CHAR_U	// Unidirectional,Bidirectional,C (CT meter),T (test code) - NOT USED
	#define MAGNET_IMAX_POWER 14400.0
	#define MAGNET_IMAX_CURRENT 60.0
//	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define DC_INJECTION_COMPENSATION_REQUIRED	// if a good CT is used this is not required
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
//	#define BIDIR_METER
//	#define TIMEZONES	8	// this can only be 6 or 8 - no other value permitted
//	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
	#define SMALL_DAILY_LOG	// if commented then all energies (unidir) are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
//	#define SURVEY_USES_AVERAGE_DATA	// unidir meter does not use this
//	#define SURVEY_USES_REACTIVE_DATA
//	#define SURVEY_USES_EXPORT_DATA
//	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA
//	#define SURVEY_USES_DEMAND_DATA
	#define FUNDAMENTAL_ENERGY_REQUIRED
//	#define SPLIT_ENERGIES_REQUIRED
	#define LAG_ONLY_TARIFF	
//	#define TOD_HAS_REACTIVE_ENERGIES
//	#define ND_POWER_ON_ACTUALS	// if not defined Assume voltages to be 240
//	#define TOD_HAS_KVARH_LAG	// define if needed
#endif

// Kerala_Unidir uses its own files as listed below
// tampersKerala.c
// em_display_Kerala.c, em_display_Kerala_Helper.c and em_display_Kerala_Helper,h

#if defined (KERALA_UNIDIR)
	#define CAL_IMAX_CURRENT	60.0
	#define TAMPER_PERSISTENCE_ON  900 // 15 minutes 300 seconds
	#define TAMPER_PERSISTENCE_OFF 300	// 5 minutes 300 seconds
	#define MAGNET_TAMPER_PERSISTENCE_ON  300 // 5 minutes 60 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 10	// 10 seconds
//	#define QUICK_FIRST_DISPLAY	// in PVVNL ensures quick turn on of display in the event of cover open
//	#define POWER_FAIL_INTERVAL_60	// if not defined then 5 seconds
//	#define EVENTDATA_RECORDS_KVAH
	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
  #define LAST_TAMPER_DETAILS_REQUIRED
	#define CLASS1_METER
	#define BOARD_ID	13	// Two digits
	#define VERNO			00	// Three digits
	#define METER_CLASS	1	// Class 0.5  or 1
//	#define SPLCODE	LCD_CHAR_U	// Unidirectional,Bidirectional,C (CT meter),T (test code) - NOT USED
	#define MAGNET_IMAX_POWER 14400.0
	#define MAGNET_IMAX_CURRENT 60.0
//	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
//	#define BIDIR_METER
	#define TIMEZONES	6	// this can only be 6 or 8 - no other value permitted
	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
//	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
	#define SMALL_DAILY_LOG	// if commented then all energies (unidir) are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
	#define SURVEY_USES_AVERAGE_DATA
	#define SURVEY_USES_REACTIVE_DATA
//	#define SURVEY_USES_EXPORT_DATA
//	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA	// not required as per amendment
	#define SURVEY_USES_DEMAND_DATA
//	#define FUNDAMENTAL_ENERGY_REQUIRED
	#define SPLIT_ENERGIES_REQUIRED
//	#define LAG_ONLY_TARIFF	// This board uses Lead Lag tariff
//	#define TOD_HAS_REACTIVE_ENERGIES
	#define ND_POWER_ON_ACTUALS	// If defined use actual voltages if >240 else use 240
#endif


// JVVNL_Uni_dir_meter uses its own files as listed below
// tampers.c - same as Gujarat
// em_display_JVVNL.c sourced from Gujarat

#if defined (JVVNL_UNI_DIR_METER)
	#define CAL_IMAX_CURRENT	60.0
	#define POWER_OFF_DISPLAY_MODE_PUSHBUTTON	// only this board uses this option
	#define LAST_TAMPER_DETAILS_REQUIRED	
	#define TAMPER_PERSISTENCE_ON  300 // 300 seconds
	#define TAMPER_PERSISTENCE_OFF 120	// 120 seconds
	#define MAGNET_TAMPER_PERSISTENCE_ON  60 // 60 seconds
	#define MAGNET_TAMPER_PERSISTENCE_OFF 15	// 15 seconds
//	#define QUICK_FIRST_DISPLAY	// in PVVNL ensures quick turn on of display in the event of cover open
//	#define POWER_FAIL_INTERVAL_60	// if not defined then 5 seconds
//	#define EVENTDATA_RECORDS_KVAH
	#define CHECK_CAL_CONDITIONS	// uncomment if voltage and current values are to be checked before calibrating
  #define LAST_TAMPER_DETAILS_REQUIRED	// for this board this is a special case - only magnet tampers will be stored
	#define CLASS1_METER
	#define BOARD_ID	30	// Two digits - from pin code
	#define VERNO			00	// Three digits
	#define METER_CLASS	1	// Class 0.5  or 1
//	#define SPLCODE	LCD_CHAR_U	// Unidirectional,Bidirectional,C (CT meter),T (test code) - NOT USED
	#define MAGNET_IMAX_POWER 14400.0
	#define MAGNET_IMAX_CURRENT 60.0
//	#define METER_CONSTANT_6400	// undef for 1600 imp/kwh
//	#define DC_INJECTION_COMPENSATION_REQUIRED	// if a good CT is used this is not required
//	#define SIX_DECIMALS
	#define REACTIVE_PULSING
//	#define BIDIR_METER
	#define TIMEZONES	8	// this can only be 6 or 8 - no other value permitted
//	#define SIXZONES // if TIMEZONES is 6 then SIXZONES must be defined
//	#define DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
	#define BILL_TYPE_MARKING	// to determine how the bill was made
	#define VARIABLE_FREQUENCY_BILLING	// monthly/bimonthly/odd/even
	#define PROGRAMMABLE_DISPLAY	// allows display sequence to be changed	
//	#define SMALL_DAILY_LOG	// if commented then special daily log case
	#define DAILY_POWER_ON_LOGGING	// if defined then daily poweron minutes are logged else all energies are logged
	#define MD_RESET_SYNCHRONOUS
//	#define KEY_ACTIVE_HIGH	
	#define SURVEY_USES_AVERAGE_DATA	
	#define SURVEY_USES_REACTIVE_DATA
//	#define SURVEY_USES_EXPORT_DATA
	#define SURVEY_USES_AVERAGE_PF	
//	#define SURVEY_USES_FREQ_DATA
//	#define SURVEY_USES_DEMAND_DATA
//	#define FUNDAMENTAL_ENERGY_REQUIRED
//	#define SPLIT_ENERGIES_REQUIRED
	#define LAG_ONLY_TARIFF
	#define TOD_HAS_REACTIVE_ENERGIES
//	#define ND_POWER_ON_ACTUALS	// if not defined Assume voltages to be 240
#endif



#endif
