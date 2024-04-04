/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : service.h
* Version      : 1.00
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 
******************************************************************************/

#ifndef __SERVICE_RWCALIB_H
#define __SERVICE_RWCALIB_H

/******************************************************************************
Macro definitions
******************************************************************************/
 //Single Phase EM
#define EM_DATA_ID_METER_CONSTANT_CALIB                             (0x01)  /* Meter constant (Pulses/KWh).             */
#define EM_DATA_ID_VOLTAGE_COEFFICIENT                              (0x02)  /* Voltage coefficient                      */
#define EM_DATA_ID_PHASE_CURRENT_COEFFICIENT                        (0x03)  /* Phase current coefficient                */
#define EM_DATA_ID_NEUTRAL_CURRENT_COEFFICIENT                      (0x04)  /* Neutral current coefficient              */
#define EM_DATA_ID_ACTIVE_POWER_COEFFICIENT                         (0x05)  /* Active power coefficient                 */
#define EM_DATA_ID_REACTIVE_POWER_COEFFICIENT                       (0x06)  /* Reactive power coefficient               */
#define EM_DATA_ID_APPARENT_POWER_COEFFICIENT                       (0x07)  /* Apparent power coefficient               */
#define EM_DATA_ID_PHASE_SHIFT_LEVEL_0                              (0x08)  /* Phase-shift level 0 (deg) Phase channel  */
#define EM_DATA_ID_PHASE_SHIFT_LEVEL_1                              (0x09)  /* Phase-shift level 1 (deg) Phase channel  */
#define EM_DATA_ID_PHASE_GAIN_LEVEL_0                               (0x0A)  /* Phase gain value level 0                 */
#define EM_DATA_ID_PHASE_GAIN_LEVEL_1                               (0x0B)  /* Phase gain value level 1                 */
#define EM_DATA_ID_NEUTRAL_GAIN_LEVEL_0                             (0x0C)  /* Neutral gain value level 0               */
#define EM_DATA_ID_NEUTRAL_GAIN_LEVEL_1                             (0x0D)  /* Neutral gain value level 0               */
#define EM_DATA_ID_SAMPLING_FREQUENCY                               (0x0E)  /* SAMPLING_FREQUENCY                       */

/* Phase1 */
#define EM_DATA_ID_P1_VOLTAGE_COEFFICIENT                           (0x12)  /*                     */
#define EM_DATA_ID_P1_CURRENT_COEFFICIENT                           (0x13)  /*                */
#define EM_DATA_ID_P1_DELAY                                         (0x14)  /*  */
#define EM_DATA_ID_P1_ACTIVE_POWER_COEFFICIENT                      (0x15)  /*              */
#define EM_DATA_ID_P1_REACTIVE_COEFFICIENT                          (0x16)  /*               */
#define EM_DATA_ID_P1_APPARENT_COEFFICIENT                          (0x17)  /*                 */
#define EM_DATA_ID_P1_PHASE_SHIFT_LEVEL0                (0x18)  /*                 */
#define EM_DATA_ID_P1_PHASE_SHIFT_LEVEL1                (0x19)  /*               */
#define EM_DATA_ID_P1_PHASE_GAIN_LEVEL0                 (0x1A)  /*                 */
#define EM_DATA_ID_P1_PHASE_GAIN_LEVEL1                 (0x1B)  /*                */
#define EM_DATA_ID_P1_PHASE_VOLTAGE_OFFSET              (0x1C)  /*                */


/* Phase2 */
#define EM_DATA_ID_P2_VOLTAGE_COEFFICIENT                           (0x22)  /*                 */
#define EM_DATA_ID_P2_CURRENT_COEFFICIENT                           (0x23)  /*            */
#define EM_DATA_ID_P2_DELAY                                         (0x24)  /*  */
#define EM_DATA_ID_P2_ACTIVE_POWER_COEFFICIENT                      (0x25)  /*              */
#define EM_DATA_ID_P2_REACTIVE_COEFFICIENT                          (0x26)  /*               */
#define EM_DATA_ID_P2_APPARENT_COEFFICIENT                          (0x27)  /*                 */
#define EM_DATA_ID_P2_PHASE_SHIFT_LEVEL0                (0x28)  /*                 */
#define EM_DATA_ID_P2_PHASE_SHIFT_LEVEL1                (0x29)  /*               */
#define EM_DATA_ID_P2_PHASE_GAIN_LEVEL0                 (0x2A)  /*                 */
#define EM_DATA_ID_P2_PHASE_GAIN_LEVEL1                 (0x2B)  /*                */
#define EM_DATA_ID_P2_PHASE_VOLTAGE_OFFSET              (0x2C)  /*                */

/* Phase3 */    
#define EM_DATA_ID_P3_VOLTAGE_COEFFICIENT                           (0x32)  /* Voltage 3 coefficient                   */
#define EM_DATA_ID_P3_CURRENT_COEFFICIENT                           (0x33)  /* Phase current 3 coefficient             */
#define EM_DATA_ID_P3_DELAY                                         (0x34)  /*  */
#define EM_DATA_ID_P3_ACTIVE_POWER_COEFFICIENT                      (0x35)  /*              */
#define EM_DATA_ID_P3_REACTIVE_COEFFICIENT                          (0x36)  /*               */
#define EM_DATA_ID_P3_APPARENT_COEFFICIENT                          (0x37)  /*                 */
#define EM_DATA_ID_P3_PHASE_SHIFT_LEVEL0                (0x38)  /*                 */
#define EM_DATA_ID_P3_PHASE_SHIFT_LEVEL1                (0x39)  /*               */
#define EM_DATA_ID_P3_PHASE_GAIN_LEVEL0                 (0x3A)  /*                 */
#define EM_DATA_ID_P3_PHASE_GAIN_LEVEL1                 (0x3B)  /*                */
#define EM_DATA_ID_P3_PHASE_VOLTAGE_OFFSET              (0x3C)  /*                */

/* Others */
#define EM_DATA_ID_RTC_COEFFICIENT                                  (0x41)  /* RTC coefficient                          */
#define EM_DATA_ID_EXT_TEMP_SENSOR_COEFFICIENT                      (0x42)  /* Ext.Temp sensor coefficient              */
#define EM_DATA_ID_AD_INPUT_IMPEDANCE_CALIBRATION_REGISTER          (0x43)  /* A/D Input Impedance Calibration Register                         */
#define EM_DATA_ID_METER_CONSTANT_PHASE_CALIB                       (0x44)  /* Meter Phase Constant (Pulses/KWh).             */
#define EM_DATA_ID_REACTIVE_TIMER_OFFSET                            (0x45)  /* Reactive Timer Offset */

#define EM_DATA_ID_AD_GAIN_CALIBRATION_DATA_REGISTER                (0x51)     
#define EM_DATA_ID_AD_PHASE1_VOLTAGE_COMPENSATION_COEFFICIENT       (0x52)  
#define EM_DATA_ID_AD_PHASE2_VOLTAGE_COMPENSATION_COEFFICIENT       (0x53)  
#define EM_DATA_ID_AD_PHASE3_VOLTAGE_COMPENSATION_COEFFICIENT       (0x54)  
#define EM_DATA_ID_AD_PHASE1_CURRENT_COMPENSATION_COEFFICIENT       (0x55)  
#define EM_DATA_ID_AD_PHASE2_CURRENT_COMPENSATION_COEFFICIENT       (0x56)  
#define EM_DATA_ID_AD_PHASE3_CURRENT_COMPENSATION_COEFFICIENT       (0x57)  
#define EM_DATA_ID_RTC_TEMP_COMPENSATION_COEFFICIENT                (0x58)  
#define EM_DATA_ID_PGA_GAIN_SCALING_COMPENSATION_COEFFICIENT        (0x59)  

/* Flag to clear counter and record */
#define EM_DATA_ID_FLAG_CLEAR_KWH_COUNTER                           (0x61)
#define EM_DATA_ID_FLAG_CLEAR_KVARH_COUNTER                         (0x62)
#define EM_DATA_ID_FLAG_CLEAR_KVAH_COUNTER                          (0x63)
#define EM_DATA_ID_FLAG_CLEAR_KWH_MAX_DEMAND_COUNTER                (0x64)
#define EM_DATA_ID_FLAG_CLEAR_KVARH_MAX_DEMAND_COUNTER              (0x65)
#define EM_DATA_ID_FLAG_CLEAR_KVAH_MAX_DEMAND_COUNTER               (0x66)
#define EM_DATA_ID_FLAG_CLEAR_ALL_TAMPER_RECORDS                    (0x67)
#define EM_DATA_ID_FLAG_CLEAR_ALL_ENERGY_RECORDS                    (0x68)

/* Macro to check is it Clearing Flag ID or not */
#define EM_DATA_ID_IS_CLEAR_FLAG(id)                                (   (                                                       \
                                                                            (id >= EM_DATA_ID_FLAG_CLEAR_KWH_COUNTER) &&        \
                                                                            (id <= EM_DATA_ID_FLAG_CLEAR_ALL_ENERGY_RECORDS)    \
                                                                        ) ? 1 : 0                                               \
                                                                    )
                                                                    
#define EM_DATA_ID_IS_SET_GAIN(id)                                  (   (                                                       \
                                                                            (id == EM_DATA_ID_P1_PHASE_GAIN_LEVEL0) ||        \
                                                                            (id == EM_DATA_ID_P2_PHASE_GAIN_LEVEL0) ||   \
                                                                            (id == EM_DATA_ID_P3_PHASE_GAIN_LEVEL0) ||   \
                                                                            (0) \
                                                                        ) ? 1 : 0                                               \
                                                                    )    
                                                                    
#define EM_DATA_ID_IS_INTEGER_DATA(id)                              (   (                                                       \
                                                                            (id == EM_DATA_ID_P1_PHASE_VOLTAGE_OFFSET)  ||   \
                                                                            (id == EM_DATA_ID_P2_PHASE_VOLTAGE_OFFSET)  ||   \
                                                                            (id == EM_DATA_ID_P3_PHASE_VOLTAGE_OFFSET)  ||   \
                                                                            (id == EM_DATA_ID_REACTIVE_TIMER_OFFSET)                ||   \
                                                                            (0)                                                     \
                                                                        ) ? 1 : 0                                               \
                                                                    )
#define EM_DATA_ID_IS_2BYTE_DATA(id)                                (   (                                                       \
                                                                            (id == EM_DATA_ID_REACTIVE_TIMER_OFFSET)                ||   \
                                                                            (0)                                                     \
                                                                        ) ? 1 : 0                                               \
                                                                    )
#define EM_DATA_ID_IS_SIGNED_DATA(id)                               (   (                                                       \
                                                                            (id == EM_DATA_ID_REACTIVE_TIMER_OFFSET)                ||   \
                                                                            (0)                                                     \
                                                                        ) ? 1 : 0                                               \
                                                                    )

#endif /*  __SERVICE_RWCALIB_H */
