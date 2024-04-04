// memoryOps.h

#include "emeter3_structs.h"

#if defined (JVVNL_UNI_DIR_METER)
	#include "e2rom_adds_JVVNL.h"
#elif defined(PVVNL_UNI_DIR_METER)
	#include "e2rom_adds_PVVNL.h"
#elif defined(GUJARAT_UNI_DIR_LTCT_METER)
	#include "e2rom_adds_GUJARAT_LTCT_UNDIR.h"
#elif defined(GUJARAT_BIDIR_LTCT_METER)
	#include "e2rom_adds_GUJARAT_LTCT_UNDIR.h"
#else
	#include "e2rom_adds.h"	
#endif



#ifndef MEMORYOPS_H
#define MEMORYOPS_H

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

void GetManufacturersName(char *ptr);
//void write_properly(unsigned int ad, char* arr, char rec_size);	// replace with EPR_Write
uint8_t LoadCalibrationValues(void);
uint8_t SaveCalibrationValues(void);
int write_energies(void);
uint32_t init_read_energies(void);
void write_history_data(ext_rtc_t bdate);
uint8_t decrementMonth(uint8_t diff);
void clearMymeterData(void);
void computeBillingEntries(void);
uint8_t findWhichBitIsOne(uint32_t givenLong);
uint32_t computeMask(uint8_t flagbit);
void composeCurrentBillData(void);	
//void composeCurrentBillData(billdata_t *billdata);	
int8_t init_read_ctl_data(void);
void init_read_common_data(void);
void write_active_tod(void);
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
//int32_t computeAveragePF(int32_t currentKWH, int32_t currentKVAH, uint8_t imp_ex_type);
int32_t computeAveragePF(int32_t currentKWH, int32_t currentKVAH, int32_t currentKWH_export, int32_t currentKVAH_export, uint8_t imp_ex_type);
int32_t computeAveragePF_TOD(int32_t currentKWH, int32_t currentKVAH, uint8_t imp_ex_type, uint8_t whichZone);
int32_t computePowerOffDurn(ext_rtc_t bdate);
uint8_t perform_E2ROM_Test(uint8_t test_mode);	// if 0 then only read, if 1 then write also
uint8_t perform_E2ROM12_Test(void);	// tests banks 1 and 2 , returns 0 for success, 1 for failure
int32_t computeAveragePF_Lag_Lead(int32_t currentKWH, int32_t currentKVAH, uint8_t pfType);	// for SPLIT_ENERGIES_REQUIRED_CASE
void clearE2RomCalibration(void);
void getFullMeterNumber(uint8_t* tbuf);
uint8_t checkMemory(uint32_t addr, uint8_t* buf, uint8_t*tbuf, uint8_t nBytes);
uint8_t bufCompare(uint8_t* buf, uint8_t*tbuf, uint8_t nBytes);
void recordInstantaneousDateTimeEnergy(uint32_t flagmask);
void recordInstantaneousData(uint32_t flagmask);
void readInstantaneousData(eventdata_t* temp_eventdata_ptr , int16_t datasize);
void getCurrentInstantaneousData(eventdata_t* temp_eventdata_ptr);
void recoverEnergies(void);
ext_rtc_t readInstantaneousDateTime(uint8_t eventCodeBit);
void update_energies_stored(void);
void write_ctl_data(void);
void write_common_data(void);
void write_ctl_backup_data(int8_t type);	// 0 - when called at 00:00 hours, 1 -  when called after zapping the meter
int8_t validate_ctl_data(void);
uint32_t discoverEnergiesWrtPtr(void);
void recover_ctl_data(void);
void update_ctl_backup_data(void);	// update cumulative md values when a bill is made
int32_t getPreviousPowerOnMinutes(void);	// this returns the cumulative powerOnMinutes in the lastBill
int32_t getMonthlyPowerOnMinutes(void);	// this returns the powerOnMinutes for this month

void verifyTOD_energies(uint8_t zone);
// this function verfies that the sum of all the energies in the tod registers equals the cumulative energies
// in the myenergydata

#endif