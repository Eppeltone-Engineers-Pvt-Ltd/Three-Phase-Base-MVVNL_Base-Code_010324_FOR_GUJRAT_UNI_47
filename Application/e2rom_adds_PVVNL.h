// e2rom_adds.h

//#include "emeter3_structs.h"

#ifndef E2ROMADDS_H
#define E2ROMADDS_H

#define E2ROM_TEST_ADDRESS	0x0000	// we will write a message here

#define CALIB_BASE_ADDRESS	0x0100
#define ALTERNATE_CALIB_BASE_ADDRESS	0x0180

#define LIST_ITEMS_BASE_ADDRESS	0x0200
#define ALTERNATE_LIST_ITEMS_BASE_ADDRESS	0x0280

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

#define CTLDATA_BASE_ADDRESS  0x0400  // this is now 80 bytes - reserve 128 bytes
#define ALTERNATE_CTLDATA_BASE_ADDRESS  0x0480  // this is now 80 bytes

#define TOD_CURRENT_BASE_ADDRESS            0x0500  // each zone allocated 64 bytes - hence 8*64= 512 bytes
#define ALTERNATE_TOD_CURRENT_BASE_ADDRESS  0x0700	// ends at 0x0900
#define ZONE_PTR_INC  64

#define PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0900  // 128 bytes
#define ALTERNATE_PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0980  // 128 bytes

#define ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0A00  // 128 bytes
#define ALTERNATE_ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS 0x0A80  // 128 bytes

#define ENERGIES_BASE_ADDRESS 0x0B00
#define ENERGIES_END_ADDRESS 0x1300			// 16 different values of 128 bytes each
#define ENERGIES_PTR_INC 128

#define TAMPER_SNAPSHOT_DATA_BASE_ADDRESS	0x1300
#define TAMPER_SNAPSHOT_DATA_END_ADDRESS 0x1700			// 32 different values of 32 bytes each

// 1700 to 1800 are free here

#define CTLDATA_BACKUP_ADDRESS  0x1800  // this is 64 bytes
#define ALTERNATE_CTLDATA_BACKUP_ADDRESS  0x1840  // this is 64 bytes

// 1900 to 1b00 are free here

#define CURRENT_MD_BASE_ADDRESS  0x1B00  // 64 bytes are now allocated to this
#define ALTERNATE_CURRENT_MD_BASE_ADDRESS  0x1B80  

#define COMMON_DATA_ADDRESS 0x1C00  // we need 64 bytes for this fellow
#define ALTERNATE_COMMON_DATA_ADDRESS  0x1C80

#define MYTMPRDATA_BASE_ADDRESS           0x1D00  // 64 byte aligned
#define ALTERNATE_MYTMPRDATA_BASE_ADDRESS 0x1D80 // 64 byte

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

// 0x7100 to 0x7448 is for DAILY_LOG if needed
// provision is kept to increase the DAILY LOG to 62 days
	#define DAILYLOG_REC_SIZE	sizeof(logdata_t)	// logdata_t is 24 bytes
	#define NO_OF_DAILY_LOG_DATA 35
	#define DAILY_LOG_PTR_INC	sizeof(logdata_t)
	#define DAILYLOG_BASE_ADDRESS  (int32_t)0x07100
	#define DAILYLOG_END_ADDRESS   ((int32_t)DAILYLOG_BASE_ADDRESS+(int32_t)NO_OF_DAILY_LOG_DATA*(int32_t)DAILY_LOG_PTR_INC)	//0x07448
	
// Load survey can start from 0x7800 ends at 0x1D4C0
	#define SURVEY_BASE_ADDRESS  0x07800  // starting value
	#define SURVEY_REC_SIZE sizeof(small_surveydata_t)	// 30 bytes
	#define SURVEY_INTERVAL 30
	#define NO_OF_DAYS_SURVEY_DATA 62	
	#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
	#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
	#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x1D4C0  // 62 days load survey data
#endif