#ifndef UTILITIES_H
#define UTILITIES_H

#include "emeter3_structs.h"

int8_t isLaterDateTime(timeinfo_t first, timeinfo_t second);
int8_t zeroedStruct(uint8_t* buf, int16_t bufsize);
int8_t compareStructs(uint8_t* buf1,uint8_t* buf2, int16_t bufsize);
int8_t isSameSign(int32_t power1, int32_t power2);
void longDelay(int16_t delayInSeconds);
uint8_t checkPhaseVoltagesCurrents(void);
uint8_t checkNeutralCurrent(void);

//void populateZoneDefinitions(zoneDefs_t *zoneDefsPtr);
//void populateZoneDefinitionsPassive(zoneDefs_t *zoneDefsPtr);
//void populateTZ0_array(zoneDefs_t *zoneDefsPtr);
//void populateTZ1_array(zoneDefs_t *zoneDefsPtr);

uint8_t swapNibbles(uint8_t givenByte);

void startEM(void);
void stopEM(void);

uint8_t bcdToBin(uint8_t bcdNum);
void readRTC(void);
void maskRTCbytes(void);
void RTCbcd2hex(void);
//uint16_t CalcDayNumFromDate(uint16_t y, uint16_t m, uint16_t d, uint8_t* buf);
uint16_t CalcDayNumFromDate(uint16_t y, uint16_t m, uint16_t d);
int32_t ComputeTotalMinutesInBillingMonth(ext_rtc_t bdate);
int32_t findNumberOfDaysInCurrentYear(ext_rtc_t bdate);


void delay(void);
void incDate(uint8_t *arr, uint8_t period);

void decptr_maskless(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc);
void incptr_maskless(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc);
void incptr(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc);
void decptr(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc);
uint8_t checkptr(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc);
void clear_structure(uint8_t* arr, uint16_t rec_size);
uint8_t calcChecksum(uint8_t *arr, uint16_t nBytes);
uint8_t isCheckSumOK(uint8_t *arr, uint16_t nBytes);


uint8_t hexchar2nibble(uint8_t c);
uint8_t hexchars2byte(uint8_t cupper,uint8_t clower);
uint8_t nib2ascii(uint8_t temp);
void byt2digs(uint8_t rdbyte, uint8_t* bytehi, uint8_t* bytelo);
uint8_t RTChex2bcd(uint8_t gbyte);
void fill_structure(uint8_t* arr, uint16_t rec_size, uint8_t fillbyte);



#endif