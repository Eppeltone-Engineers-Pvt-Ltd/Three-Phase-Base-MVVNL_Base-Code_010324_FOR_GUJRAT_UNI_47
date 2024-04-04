// memoryOps.h

#include "emeter3_structs.h"

#ifndef MEMORYOPS_H

struct list_items_struct
{
	float32_t degree_list_r[2];
	float32_t degree_list_y[2];
	float32_t degree_list_b[2];
	
	float32_t gain_list_r[2];
	float32_t gain_list_y[2];
	float32_t gain_list_b[2];
	
	uint8_t driver_adc_gain0[2];
	uint8_t driver_adc_gain1[2];

	int16_t	driver_timer_offset;
	uint8_t	dummy;
	uint8_t	chksum;
};

typedef struct list_items_struct list_items_t;


#define E2ROM_TEST_ADDRESS	0x0000	// we will write a message here

#define CALIB_BASE_ADDRESS	0x0800
#define ALTERNATE_CALIB_BASE_ADDRESS	0x0880

#define LIST_ITEMS_BASE_ADDRESS	0x0900
#define ALTERNATE_LIST_ITEMS_BASE_ADDRESS	0x0980

#define PASSIVE_ZONE_DEFINITIONS_BASE_ADDRESS 0x0A00  // 32 bytes
#define ALTERNATE_PASSIVE_ZONE_DEFINITIONS_BASE_ADDRESS 0x0A20  // 32 bytes

#define CLASS20_ACTIVATION_TIME_ADDRESS	0x0A40	// 16 bytes
#define ALTERNATE_CLASS20_ACTIVATION_TIME_ADDRESS	0x0A50	// 16 bytes

#define SINGLE_ACTION_ACTIVATION_TIME_ADDRESS	0x0A60	// 16 bytes
#define ALTERNATE_SINGLE_ACTION_ACTIVATION_TIME_ADDRESS	0x0A70	// 16 bytes

#define SECRET1_ADDRESS							0x0A80
#define ALTERNATE_SECRET1_ADDRESS		0x0A90

#define SECRET2_ADDRESS							0x0AA0	// 32 bytes
#define ALTERNATE_SECRET2_ADDRESS		0x0AC0	// 32 bytes

#define LATEST_EVENT_BASE_ADDRESS		0x0AE0		// 16 bytes
#define ALTERNATE_LATEST_EVENT_BASE_ADDRESS	0x0AF0	// 16 bytes

// next free address 0x0B00

#define CTLDATA_BASE_ADDRESS  0x0B00  // this is now 80 bytes - reserve 128 bytes
#define ALTERNATE_CTLDATA_BASE_ADDRESS  0x0B80  // this is now 80 bytes


#ifdef BIDIR_METER
	#define TOD_CURRENT_BASE_ADDRESS            0x0C00  // each zone needs 64 bytes - hence 8*64= 512
	#define ALTERNATE_TOD_CURRENT_BASE_ADDRESS  0x0E00	// ends at 0x1000
	#define ZONE_PTR_INC  64

#else
	#define TOD_CURRENT_BASE_ADDRESS            0x0C00  // each zone needs 32 bytes - hence 8*32= 256
	#define ALTERNATE_TOD_CURRENT_BASE_ADDRESS  0x0D00
	#define ZONE_PTR_INC  32
#endif

#define ENERGIES_BASE_ADDRESS 0x1000
#define ENERGIES_END_ADDRESS 0x2000			// 32 different values of 128 bytes each
#define ENERGIES_PTR_INC 128
//#define ENERGIES_PTR_INC 64

//#define CTLDATA_BASE_ADDRESS  0x1800  // this is now 64 bytes
//#define ALTERNATE_CTLDATA_BASE_ADDRESS  0x1840  // this is now 64 bytes

#define CURRENT_MD_BASE_ADDRESS  0x2000  // 64 bytes are now allocated to this
#define ALTERNATE_CURRENT_MD_BASE_ADDRESS  0x2080  

#define COMMON_DATA_ADDRESS 0x2100  // we need 64 bytes for this fellow
#define ALTERNATE_COMMON_DATA_ADDRESS  0x2180

#define MYTMPRDATA_BASE_ADDRESS           0x2200  // 64 byte aligned
#define ALTERNATE_MYTMPRDATA_BASE_ADDRESS 0x2280 // 64 byte

#define ZONE_DEFINITIONS_BASE_ADDRESS 0x2300  // 32 bytes
#define ALTERNATE_ZONE_DEFINITIONS_BASE_ADDRESS 0x2320  // 32 bytes

// 0x2340 to 0x2400 bytes are spare here


// we should keep 12 records
#ifdef BIDIR_METER
#define BILLDATA_BASE_ADDRESS	0x2400	// each record is something like 420 bytes - reserve 512 bytes
#define BILLDATA_END_ADDRESS	0x3C00	// for 12 months data this will be = 12*512 = 6144 bytes
#define BILL_PTR_INC 512
#else
#define BILLDATA_BASE_ADDRESS	0x2400	// each record is something like 272 bytes - reserve 384 bytes
#define BILLDATA_END_ADDRESS	0x3600	// for 12 months data this will be = 12*384 = 4608 bytes
#define BILL_PTR_INC 384
#endif
// we will not need HISTORY_BASE_ADDRESS and TOD_HISTORY_BASE_ADDRESS any more

//#define HISTORY_BASE_ADDRESS 0x2000 // must be located on 64 byte boundary
//#define HISTORY_END_ADDRESS  0x2300  // 768 bytes (12*64)
//#define HISTORY_PTR_INC 64



//#define TOD_HISTORY_BASE_ADDRESS  0x2600  // each month's history needs 256 bytes - i.e 12*256=
//#define TOD_HISTORY_END_ADDRESS  0x3200  // each month's history needs 256 bytes - i.e 12*256=
//#define TOD_PTR_INC  256

// Common storage is not to be used
// individual group events have their own address blocks
// currently 40 events of 32 bytes each can be stored in each block
// except cover open which will store only one entry
#define EVENT_PTR_INC 32
//#define EVENT_BASE_ADDRESS	0x2E40
//#define EVENT_END_ADDRESS		0x4740

// Gujarat needs 200 occurences + 200 restorations


#ifdef BIDIR_METER
	#define TAMPER_START_ADDRESS	(int32_t)0x3C00
#else
	#define TAMPER_START_ADDRESS (int32_t)0x3C00
#endif

	#define EVENTS0_BASE_ADDRESS (int32_t)TAMPER_START_ADDRESS	// 150 entries - voltage related
	#define EVENTS0_END_ADDRESS	((int32_t)EVENTS0_BASE_ADDRESS + (int32_t)(150*32)) // 0x4100

	#define EVENTS1_BASE_ADDRESS (int32_t)EVENTS0_END_ADDRESS	// 150 entries - current related
	#define EVENTS1_END_ADDRESS 	((int32_t)EVENTS1_BASE_ADDRESS + (int32_t)(150*32))

	#define EVENTS2_BASE_ADDRESS (int32_t)EVENTS1_END_ADDRESS	// 50 entries	- power fail
	#define EVENTS2_END_ADDRESS ((int32_t)EVENTS2_BASE_ADDRESS + (int32_t)(50*32))
 
	#define EVENTS3_BASE_ADDRESS (int32_t)EVENTS2_END_ADDRESS	// 20 entries - transactions
	#define EVENTS3_END_ADDRESS ((int32_t)EVENTS3_BASE_ADDRESS + (int32_t)(20*32))

	#define EVENTS4_BASE_ADDRESS (int32_t)EVENTS3_END_ADDRESS	// 40 entries - magnet
	#define EVENTS4_END_ADDRESS ((int32_t)EVENTS4_BASE_ADDRESS + (int32_t)(40*32))

	#define EVENTS5_BASE_ADDRESS (int32_t)EVENTS4_END_ADDRESS	//  entries
	#define EVENTS5_END_ADDRESS ((int32_t)EVENTS5_BASE_ADDRESS + (int32_t)(1*32))	// 1 entries - cover open



// 0x8000 to 0x9000 is for DAILY_LOG if needed

#define DAILYLOG_REC_SIZE	16	// logdata_t is 16 bytes
#define NO_OF_DAILY_LOG_DATA 62
#define DAILY_LOG_PTR_INC	16
#define DAILYLOG_BASE_ADDRESS  (int32_t)0x07000  // starting value
#define DAILYLOG_END_ADDRESS   ((int32_t)DAILYLOG_BASE_ADDRESS+(int32_t)NO_OF_DAILY_LOG_DATA*(int32_t)DAILY_LOG_PTR_INC)
// for Gujarat Bard DAILYLOG_BASE_ADDRESS will not be used
// Daily log information will be stored alongwith losd survey data


// Load survey can start from 0x7400

#ifdef BIDIR_METER
	#define SURVEY_BASE_ADDRESS  0x07400  // starting value - for actual use PLEASE UNCOMMENT
//	#define SURVEY_BASE_ADDRESS  0x0F580  // starting value - FOR TESTING ONLY
//	#define SURVEY_BASE_ADDRESS  0xFFCE  // starting value - FOR TESTING ONLY
//	#define SURVEY_BASE_ADDRESS  0x1F6D7  // starting value - FOR TESTING ONLY
	#define SURVEY_REC_SIZE sizeof(small_surveydata_t)
	#define SURVEY_INTERVAL 15
	#define NO_OF_DAYS_SURVEY_DATA 62	
	#define NO_OF_SURVEY_RECORDS	((int32_t)NO_OF_DAYS_SURVEY_DATA*(int32_t)1440/(int32_t)SURVEY_INTERVAL)
	#define SURVEY_PTR_INC	sizeof(small_surveydata_t)
	#define SURVEY_END_ADDRESS   ((int32_t)SURVEY_BASE_ADDRESS+(int32_t)NO_OF_SURVEY_RECORDS*(int32_t)SURVEY_PTR_INC)// 0x7A40  // 30 days load survey data
#endif


void GetManufacturersName(char *ptr);


//void write_properly(unsigned int ad, char* arr, char rec_size);	// replace with EPR_Write


uint8_t LoadCalibrationValues(void);
uint8_t SaveCalibrationValues(void);

int write_energies(void);
void init_read_energies(void);

void write_history_data_new(ext_rtc_t bdate);
//void write_history_data(void);
uint8_t decrementMonth(uint8_t diff);
void clearMymeterData(void);
void computeBillingEntries(void);

uint8_t findWhichBitIsOne(uint32_t givenLong);
uint32_t computeMask(uint8_t flagbit);

void composeCurrentBillData(void);	
//void composeCurrentBillData(billdata_t *billdata);	



void init_read_ctl_data(void);
void init_read_common_data(void);
void init_read_tod(void);
void init_read_passive_tod(void);
void write_passive_tod(void);
void write_passive_tod_activation_data(uint8_t status);

void init_read_single_action_activation_data(void);
void write_single_action_activation_data(uint8_t status);

void init_read_md_data(void);
void init_read_tamper_data(void);
void init_read_lastBillDate(void);

void write_alternately(uint32_t ad, uint32_t alternate_ad, uint8_t* arr, uint16_t rec_size);
uint8_t read_alternately(uint32_t ad, uint32_t alternate_ad, uint8_t* arr, uint16_t rec_size);
void wre2rom(void);

void performEndJobsForBilling(void);

void clearRAMStructures(void);
void clearE2Rom(void);
void delay(void);
uint8_t verifyEnergyAreaInit(void);
void init_read_secret_keys(void);

int32_t computeAveragePF(int32_t currentKWH, int32_t currentKVAH, uint8_t imp_ex_type);
int32_t computeAveragePF_TOD(int32_t currentKWH, int32_t currentKVAH, uint8_t imp_ex_type, uint8_t whichZone);
int32_t computePowerOffDurn(ext_rtc_t bdate);

uint8_t perform_E2ROM_Test(uint8_t test_mode);

#define MEMORYOPS_H
#endif