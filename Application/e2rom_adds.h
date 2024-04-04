// e2rom_adds.h

//#include "emeter3_structs.h"

#ifndef E2ROMADDS_H
#define E2ROMADDS_H

#define E2ROM_TEST_ADDRESS	0x0000	// we will write a message here

#define CALIB_BASE_ADDRESS	0x0100
#define ALTERNATE_CALIB_BASE_ADDRESS	0x0180

#define LIST_ITEMS_BASE_ADDRESS	0x0200
#define ALTERNATE_LIST_ITEMS_BASE_ADDRESS	0x0280

// not used anymore - now all season data is stored using the saveActiveData/loadActiveData/savePassiveData/loadPassiveData functions
// the entire data gets stored in less than 128 bytes - for base address and alternate addresses we will need (128+128)*2 = 512 bytes
// check out address 0x0780 below
//#define PASSIVE_ZONE_DEFINITIONS_BASE_ADDRESS 0x0300  // 32 bytes
//#define ALTERNATE_PASSIVE_ZONE_DEFINITIONS_BASE_ADDRESS 0x0320  // 32 bytes

#define CLASS20_ACTIVATION_TIME_ADDRESS	0x0340	// 16 bytes
#define ALTERNATE_CLASS20_ACTIVATION_TIME_ADDRESS	0x0350	// 16 bytes

#define SINGLE_ACTION_ACTIVATION_TIME_ADDRESS	0x0360	// 16 bytes
#define ALTERNATE_SINGLE_ACTION_ACTIVATION_TIME_ADDRESS	0x0370	// 16 bytes

#define SECRET1_ADDRESS							0x0380
#define ALTERNATE_SECRET1_ADDRESS		0x0390

#define SECRET2_ADDRESS							0x03A0	// 32 bytes
#define ALTERNATE_SECRET2_ADDRESS		0x03C0	// 32 bytes

#define LATEST_EVENT_BASE_ADDRESS		0x03E0		// 16 bytes
#define ALTERNATE_LATEST_EVENT_BASE_ADDRESS	0x03F0	// 16 bytes

// next free address 0x0400

#define CTLDATA_BASE_ADDRESS  0x0400  // this is now 80 bytes - reserve 128 bytes
#define ALTERNATE_CTLDATA_BASE_ADDRESS  0x0480  // this is now 80 bytes


// for the sake of simplicity for both Bidir and Unidir meters the TOD_CURRENT AND ALTERNATE_TOD_CURRENT addresses are being kept the same
// also there are only five tariff zones - this further reduces the e2rom requirement
#ifdef BIDIR_METER
	#define TOD_CURRENT_BASE_ADDRESS            0x0500  // each zone needs 64 bytes - hence 8*64= 512
	#define ALTERNATE_TOD_CURRENT_BASE_ADDRESS  0x0700	// ends at 0x0900
	#define ZONE_PTR_INC  64
#else
	#define TOD_CURRENT_BASE_ADDRESS            0x0500  // each zone allocated 64 bytes - hence 8*64= 512 bytes
	#define ALTERNATE_TOD_CURRENT_BASE_ADDRESS  0x0700	// ends at 0x0900
	#define ZONE_PTR_INC  64
#endif

// 0x0900 to 0x0b00 may be allocated

#define PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0900  // 128 bytes
#define ALTERNATE_PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0980  // 128 bytes

#define ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0A00  // 128 bytes
#define ALTERNATE_ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0A80  // 128 bytes
// 512 bytes are used for the active and passive season tod defs - next available address 0x0B00

#ifdef EVENTDATA_RECORDS_KVAH
	#define ENERGIES_BASE_ADDRESS 0x0B00
	#define ENERGIES_END_ADDRESS 0x1300			// 16 different values of 128 bytes each
	#define ENERGIES_PTR_INC 128
	
	#define TAMPER_SNAPSHOT_DATA_BASE_ADDRESS	0x1300
	#define TAMPER_SNAPSHOT_DATA_END_ADDRESS 0x1700			// 32 different values of 32 bytes each
#else
	#ifdef KERALA_UNIDIR
		#define ENERGIES_BASE_ADDRESS 0x0B00
		#define ENERGIES_END_ADDRESS 0x1300			// 32 different values of upto 64 bytes each
		#define ENERGIES_PTR_INC 64
	
		#define TAMPER_SNAPSHOT_DATA_BASE_ADDRESS	0x1300
		#define TAMPER_SNAPSHOT_DATA_END_ADDRESS 0x1700			// 32 different values of 32 bytes each
	#else
		#define ENERGIES_BASE_ADDRESS 0x0B00
		#define ENERGIES_END_ADDRESS 0x1300			// 16 different values of 128 bytes each
		#define ENERGIES_PTR_INC 128
		
		#define TAMPER_SNAPSHOT_DATA_BASE_ADDRESS	0x1300
		#define TAMPER_SNAPSHOT_DATA_END_ADDRESS 0x1700			// 32 different values of 32 bytes each
	#endif
#endif

// TAMPER_SNAPSHOT_DATA_END_ADDRESS always ends at 0x1700 above
// even for the case of EVENTDATA_RECORDS_KVAH - since eventdata_t now stores voltage without decimal and pf with two decimals
// 1700 to 1800 are free here

#define CTLDATA_BACKUP_ADDRESS  0x1800  // this is 64 bytes
#define ALTERNATE_CTLDATA_BACKUP_ADDRESS  0x1840  // this is 64 bytes

// For Gujarat Board only 16 energy values will be stored with increment of 1000
// This will free up 0x1300 to 0x1B00 storage
// which can be used to store parameters snapshot at the time of occurence for each tamper 
// 0x1300 to 0x1700 are being used - hence 0x1900 to 0x1b00 are spare

// 1880 to 1b00 are free here

#define CURRENT_MD_BASE_ADDRESS  0x1B00  // 64 bytes are now allocated to this
#define ALTERNATE_CURRENT_MD_BASE_ADDRESS  0x1B80  

#define COMMON_DATA_ADDRESS 0x1C00  // we need 64 bytes for this fellow
#define ALTERNATE_COMMON_DATA_ADDRESS  0x1C80

#define MYTMPRDATA_BASE_ADDRESS           0x1D00  // 64 byte aligned
#define ALTERNATE_MYTMPRDATA_BASE_ADDRESS 0x1D80 // 64 byte

// - not used any more
//#define ZONE_DEFINITIONS_BASE_ADDRESS 0x1E00  // 32 bytes
//#define ALTERNATE_ZONE_DEFINITIONS_BASE_ADDRESS 0x1E20  // 32 bytes

// 0x1E40 to 0x1F00 bytes are spare here - 192

//#ifdef KERALA_UNIDIR
#ifdef LAST_TAMPER_DETAILS_REQUIRED
	#define LAST_OCCURED_DETAILS_ADDRESS					0x1E40
	#define SECOND_LAST_OCCURED_DETAILS_ADDRESS		0x1E50
	#define LAST_RESTORED_DETAILS_ADDRESS					0x1E60
	#define SECOND_LAST_RESTORED_DETAILS_ADDRESS	0x1E70
#endif

#define METER_NUMBER_BASE_ADDRESS	0x1E80	// 16 bytes
#define ALTERNATE_METER_NUMBER_BASE_ADDRESS 0x1E90 // 16 bytes

// 0x1EA0 to 0x1F00 bytes are spare here


// we should keep 12 records
#ifdef BIDIR_METER
	#define BILLDATA_BASE_ADDRESS	0x1F00	// each record is something like 596 bytes - reserve 640 bytes
	#define BILLDATA_END_ADDRESS	0x3D00	// for 12 months data this will be = 12*640 = 6144 bytes
	#define BILL_PTR_INC 640
#else
	#define BILLDATA_BASE_ADDRESS	0x1F00	// each record is something like 388 bytes - reserve 512 bytes
	#define BILLDATA_END_ADDRESS	0x3700	// for 12 months data this will be = 12*512 = 6144 bytes
	#define BILL_PTR_INC 512
#endif
// we will not need HISTORY_BASE_ADDRESS and TOD_HISTORY_BASE_ADDRESS any more


#ifdef KERALA_UNIDIR
	#define EVENT_PTR_INC 32
	#define TAMPER_START_ADDRESS	(int32_t)0x3700
	
	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 100 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(100*32)) // 0x4380

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 200 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(200*32))	// 0x5C80

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 100 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(100*32))	// 0x6900
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))	// 0x6B80

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 100 entries - magnet+3200
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(100*32))	// 0x7800

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open - 0x7820
#endif


#ifdef POINT_FIVE_CLASS_METER
// Gujarat needs 200 occurences + 200 restorations
	#define EVENT_PTR_INC 32
	#ifdef BIDIR_METER
	#define TAMPER_START_ADDRESS	(int32_t)0x3D00
	#else
	#define TAMPER_START_ADDRESS (int32_t)0x3D00
	#endif

	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 150 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(150*32)) // 0x4FC0

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 150 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(150*32))	// 0x6280

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 50 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(50*32))	// 0x68C0
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))	// 0x6B40

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 40 entries - magnet
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(40*32))	// 0x7040

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open - 0x7060
#endif

#ifdef JVVNL_UNI_DIR_METER
// JVVNL uses the same as Gujarat 200 occurences + 200 restorations
	#define EVENT_PTR_INC 32
	#ifdef BIDIR_METER
	#define TAMPER_START_ADDRESS	(int32_t)0x3D00
	#else
	#define TAMPER_START_ADDRESS (int32_t)0x3D00
	#endif

	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 150 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(150*32)) // 0x4FC0

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 150 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(150*32))	// 0x6280

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 50 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(50*32))	// 0x68C0
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))	// 0x6B40

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 40 entries - magnet
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(40*32))	// 0x7040

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open - 0x7060
#endif


#ifdef GUJARAT_UNI_DIR_METER
// Gujarat needs 200 occurences + 200 restorations
	#define EVENT_PTR_INC 32
	#ifdef BIDIR_METER
	#define TAMPER_START_ADDRESS	(int32_t)0x3D00
	#else
	#define TAMPER_START_ADDRESS (int32_t)0x3D00
	#endif

	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 150 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(150*32)) // 0x4FC0

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 150 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(150*32))	// 0x6280

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 50 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(50*32))	// 0x68C0
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))	// 0x6B40

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 40 entries - magnet
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(40*32))	// 0x7040

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open - 0x7060
#endif

#ifdef GUJARAT_BI_DIR_METER
// Gujarat needs 200 occurences + 200 restorations
	#define EVENT_PTR_INC 32
	#ifdef BIDIR_METER
	#define TAMPER_START_ADDRESS	(int32_t)0x3D00
	#else
	#define TAMPER_START_ADDRESS (int32_t)0x3D00
	#endif

	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 150 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(150*32)) // 0x4FC0

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 150 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(150*32))	// 0x6280

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 50 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(50*32))	// 0x68C0
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))	// 0x6B40

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 40 entries - magnet
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(40*32))	// 0x7040

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open - 0x7060
#endif

#ifdef PVVNL_UNI_DIR_METER
// PVVNL eventdata_t is < 32 bytes
	#define EVENT_PTR_INC 32
	#define TAMPER_START_ADDRESS (int32_t)0x3D00

	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 150 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(150*32)) // 0x4fc0

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 150 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(150*32))	// 0x6280

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 50 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(50*32))	// 0x68c0
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))	// 0x6B40

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 40 entries - magnet
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(40*32))	// 0x7040

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open - 0x7060
#endif



// 0x8000 to 0x9000 is for DAILY_LOG if needed

#ifdef KERALA_UNIDIR
	#define DAILYLOG_REC_SIZE	16	// logdata_t is 16 bytes
	#define NO_OF_DAILY_LOG_DATA 90
	#define DAILY_LOG_PTR_INC	16
	#define DAILYLOG_BASE_ADDRESS  (int32_t)0x07840  // starting value
	#define DAILYLOG_END_ADDRESS   ((int32_t)DAILYLOG_BASE_ADDRESS+(int32_t)NO_OF_DAILY_LOG_DATA*(int32_t)DAILY_LOG_PTR_INC)
#else
	#ifdef SMALL_DAILY_LOG
		#define DAILYLOG_REC_SIZE	16	// logdata_t is 16 bytes
		#ifdef BIDIR_METER
			#define NO_OF_DAILY_LOG_DATA 62
		#else
			#define NO_OF_DAILY_LOG_DATA 180
		#endif
		#define DAILY_LOG_PTR_INC	16
		#define DAILYLOG_BASE_ADDRESS  (int32_t)0x07080  // starting value
		#define DAILYLOG_END_ADDRESS   ((int32_t)DAILYLOG_BASE_ADDRESS+(int32_t)NO_OF_DAILY_LOG_DATA*(int32_t)DAILY_LOG_PTR_INC)
	#else	
		#define DAILYLOG_REC_SIZE	sizeof(logdata_t)	// logdata_t is 24 bytes
		#define NO_OF_DAILY_LOG_DATA 35
		#define DAILY_LOG_PTR_INC	sizeof(logdata_t)
		#define DAILYLOG_BASE_ADDRESS  (int32_t)0x0A3C0
		#define DAILYLOG_END_ADDRESS   ((int32_t)DAILYLOG_BASE_ADDRESS+(int32_t)NO_OF_DAILY_LOG_DATA*(int32_t)DAILY_LOG_PTR_INC)	//0x0A7A0
	#endif
#endif
// Load survey can start from 0x7400

#ifdef KERALA_UNIDIR
		#define SURVEY_BASE_ADDRESS  0x08000  // starting value - for actual use
		#define SURVEY_REC_SIZE sizeof(small_surveydata_t)	// this is 28 bytes for KERALA_UNDIR
		#define SURVEY_INTERVAL 30
		#define NO_OF_DAYS_SURVEY_DATA 90	
		#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
		#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
		#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)
#endif

#ifdef PVVNL_UNI_DIR_METER
		#define SURVEY_BASE_ADDRESS  0x07080  // starting value
		#define SURVEY_REC_SIZE sizeof(small_surveydata_t)	// 30 bytes
		#define SURVEY_INTERVAL 30
		#define NO_OF_DAYS_SURVEY_DATA 62	
		#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
		#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
		#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x7A40  // 30 days load survey data
#endif

#ifdef GUJARAT_BI_DIR_METER
	// for BIDIR_METER small_surveydata_t is 28 bytes
		#define SURVEY_BASE_ADDRESS  0x07480  // starting value - for actual use PLEASE UNCOMMENT
		#define SURVEY_REC_SIZE sizeof(small_surveydata_t)
		#define SURVEY_INTERVAL 15
		#define NO_OF_DAYS_SURVEY_DATA 62	
		#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
		#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
		#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x7A40  // 30 days load survey data
#endif

#ifdef GUJARAT_UNI_DIR_METER
	// this is for undirectional meter - small_surveydata_t is 10 bytes
		#define SURVEY_BASE_ADDRESS  0x7C00  // starting value - for actual use PLEASE UNCOMMENT
		#define SURVEY_REC_SIZE sizeof(small_surveydata_t)
		#define SURVEY_INTERVAL 30
		#define NO_OF_DAYS_SURVEY_DATA 180	
		#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
		#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
		#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x7A40  // 30 days load survey data
#endif

#ifdef JVVNL_UNI_DIR_METER
	// this is for undirectional meter - small_surveydata_t is 26 bytes
		#define SURVEY_BASE_ADDRESS  0x7C00  // starting value - for actual use PLEASE UNCOMMENT
		#define SURVEY_REC_SIZE sizeof(small_surveydata_t)
		#define SURVEY_INTERVAL 30
		#define NO_OF_DAYS_SURVEY_DATA 75	
		#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
		#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
		#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x7A40  // 30 days load survey data
#endif

#ifdef GUJARAT_UNI_DIR_LTCT_METER
	// this is for undirectional meter - small_surveydata_t is 10 bytes
		#define SURVEY_BASE_ADDRESS  0x7C00  // starting value - for actual use PLEASE UNCOMMENT
		#define SURVEY_REC_SIZE sizeof(small_surveydata_t)
		#define SURVEY_INTERVAL 30
		#define NO_OF_DAYS_SURVEY_DATA 180	
		#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
		#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
		#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x7A40  // 30 days load survey data
#endif
#endif