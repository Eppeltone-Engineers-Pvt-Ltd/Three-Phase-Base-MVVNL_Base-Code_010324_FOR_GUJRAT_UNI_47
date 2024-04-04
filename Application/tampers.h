// tampers.h
#ifndef TAMPERS_H
#define TAMPERS_H

#include "board.h"
#define _BV(i)	(1<<i)
#define ABS(x)  ((x) <  0  ? (-(x)) : (x))


#define NO_TAMPER 0

#define MISSING_POTENTIAL_R 1	//BIT1
#define MISSING_POTENTIAL_Y 2	//BIT2
#define MISSING_POTENTIAL_B 3	//BIT3

#define OVER_VOLTAGE      4	//BIT4
#define UNDER_VOLTAGE     5	//BIT5
#define VOLTAGE_UNBALANCED  6	//BIT6

#define CT_REVERSED_R	7 //BIT7
#define CT_REVERSED_Y	8 //BIT8
#define CT_REVERSED_B 9	//BIT9

#define CT_OPEN_R 10	//BIT10
#define CT_OPEN_Y 11	//BIT11
#define CT_OPEN_B 12	//BIT12

#define CURRENT_UNBALANCED 13	//BIT13
#define CT_BYPASS 14	//BIT14
#define OVER_CURRENT  15	//BIT15

#define POWER_EVENT 16	//BIT16
#define POWER_EVENT_MASK	0x00010000

#define RTC_TRANSACTION	17	//BIT17
#define RTC_TRANSACTION_MASK	0x00020000

#define MD_INTERVAL_TRANSACTION	18	//BIT18
#define MD_INTERVAL_TRANSACTION_MASK	0x00040000

#define SURVEY_INTERVAL_TRANSACTION	19	//BIT19
#define SURVEY_INTERVAL_TRANSACTION_MASK	0x00080000

#define SINGLE_ACTION_TRANSACTION	20	//BIT20
#define SINGLE_ACTION_TRANSACTION_MASK	0x00100000

#define ACTIVITY_CALENDAR_TRANSACTION	21	//BIT21
#define ACTIVITY_CALENDAR_TRANSACTION_MASK	0x00200000

#define MAGNET 22	//BIT22
#define MAGNET_MASK	0x00400000

#define AC_MAGNET 23	//BIT23
#define AC_MAGNET_MASK	0x00800000

#define NEUTRAL_DISTURBANCE 24	//BIT24
#define NEUTRAL_DISTURBANCE_MASK 0x01000000

#define LOW_PF  25	//BIT25
#define LOW_PF_MASK 0x02000000

#define COVER_OPEN  26	//BIT26
#define COVER_OPEN_MASK 0x04000000

#define MODE_UNI  27	//BIT27
#define MODE_UNI_MASK 0x08000000

#define MODE_BIDIR  28	//BIT28
#define MODE_BIDIR_MASK 0x10000000

#define OVER_CURRENT_Y  29	//BIT29
#define OVER_CURRENT_Y_MASK 0x20000000

#define OVER_CURRENT_B  30	//BIT30
#define OVER_CURRENT_B_MASK 0x40000000

#define REVERSE_MASK	0x01
#define MISSPOT_MASK	0x02
#define HALLSENS_MASK	0x04
#define BYPASS_MASK	0x08
#define OVERVOLT_MASK	0x10
#define UNDERVOLT_MASK	0x20
#define CTOPEN_MASK	0x40
#define ACMAG_MASK 0x80

//#define MISSING_POTENTIAL_THRESHOLD (96.0f)
//#define MISSING_POTENTIAL_RETURN_THRESHOLD (180.0f)

//#define OVER_VOLTAGE_THRESHOLD	(276.0f)
//#define OVER_VOLTAGE_RETURN_THRESHOLD	(264.0f)

//#define LOW_VOLTAGE_THRESHOLD	(180.0f)
//#define VOLTAGE_UNBALANCE_THRESHOLD (0.1f)	// 10%
//#define VOLTAGE_UNBALANCE_RESTORATION_THRESHOLD (0.10f)	// 10% of Vref
//#define VOLTAGE_UNBALANCE_MIN_VAL (144.0f)

//#define CURRENT_OPEN_THRESHOLD (0.02f)	// 2%
//#define EXT_CURRENT_VECTOR_THRESHOLD (0.3f)
//#define IVECTOR_RMS_UNBALANCE_THRESHOLD (0.05f)	// 50 mA
//#define CURRENT_UNBALANCE_THRESHOLD (0.3f)

//#define CURRENT_BYPASS_THRESHOLD	(0.9f)	// 10% of Ib(10Amps)
//#define CURRENT_BYPASS_THRESHOLD	(1.000f)
//#define OVER_CURRENT_THRESHOLD (80.0f)

void set_tamperEvent_image(uint32_t flagmask);
void clear_tamperEvent_image(uint32_t flagmask);

// this below is the regular one
//void check_setTamper(uint8_t *ptr, uint32_t flagmask , uint8_t persistence_value);
//void check_clearTamper(uint8_t *ptr, uint32_t flagmask, uint8_t persistence_value);

// these below are for TNEB - may be used for Gujarat too
void check_setTamper(uint16_t *ptr, uint32_t flagmask , uint16_t persistence_value, uint8_t flagBit, uint8_t onDelay);
void check_clearTamper(uint16_t *ptr, uint32_t flagmask, uint16_t persistence_value, uint8_t flagBit, uint8_t onDelay);

uint8_t countVoltageGT180LT276(int16_t vr, int16_t vy, int16_t vb);
uint8_t countVoltageGT276LT300(int16_t vr, int16_t vy, int16_t vb);
uint8_t countCurrentGT95(int16_t ir, int16_t iy, int16_t ib);
uint8_t countCurrentGT45(int16_t ir, int16_t iy, int16_t ib);

uint8_t countVoltageGT144LT276(int16_t vr, int16_t vy, int16_t vb);
uint8_t countCurrentGT145(int16_t ir, int16_t iy, int16_t ib);
uint8_t countVoltageLT50(int16_t vr, int16_t vy, int16_t vb);


void performTamperProcessing(void);
int8_t record_power_fail_event(void);
//void record_power_return_event(void);
void record_transaction_event(uint32_t flagmask);

int16_t findMax(int16_t i1, int16_t i2, int16_t i3);
int16_t findMin(int16_t i1, int16_t i2, int16_t i3);

void clearPersistenceCounters(void);


#endif
