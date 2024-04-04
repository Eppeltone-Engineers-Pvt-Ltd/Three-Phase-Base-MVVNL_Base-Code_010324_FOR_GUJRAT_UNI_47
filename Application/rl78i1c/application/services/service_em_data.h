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

#ifndef __SERVICE_EMDATA_H
#define __SERVICE_EMDATA_H


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "em_core.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Phase 1 */
#define EM_DATA_ID_P1_ACTIVE_POWER                  (0xB1)  /* Active Power (Watt)                      */
#define EM_DATA_ID_P1_ACTIVE_ENERGY                 (0xB2)  /* Active Energy (KWh)                      */
#define EM_DATA_ID_P1_APPARENT_POWER                (0xB3)  /* Apparent Power (VA)                      */
#define EM_DATA_ID_P1_APPARENT_ENERGY               (0xB4)  /* Apparent Energy (KVAh)                   */
#define EM_DATA_ID_P1_AMPERE_HOUR                   (0xB5)  /* Ampere Hour (Ah)                         */
#define EM_DATA_ID_P1_VRMS                          (0xB6)  /* VRMS (Volt)                              */
#define EM_DATA_ID_P1_IRMS                          (0xB7)  /* I1RMS Phase channel (Ampere)             */
#define EM_DATA_ID_INRMS                            (0xB8)  /* I2RMS Neutral channel (Ampere)           */
#define EM_DATA_ID_P1_FREQUENCY                     (0xB9)  /* Frequency (Hz)                           */
#define EM_DATA_ID_P1_POWER_FACTOR                  (0xBA)  /* Power factor.                            */
#define EM_DATA_ID_P1_REACTIVE_POWER                (0xBB)  /* Phase 1 Reactive Power (VAr)             */
#define EM_DATA_ID_P1_REACTIVE_ENERGY               (0xBC)  /* Phase 1 Reactive Energy (KVArh)          */
#define EM_DATA_ID_P1_VOLTAGE_THD                   (0xBD)  /* Phase 1 Voltage THD (%)                  */
#define EM_DATA_ID_P1_CURRENT_THD                   (0xBE)  /* Phase 1 Current THD (%)                  */
#define EM_DATA_ID_COST_H_SOCKET1                   (0xBF)  /* Cost/h Socket 1                          */

/* Phase 2 */
#define EM_DATA_ID_P2_ACTIVE_POWER                  (0xC1)  /* Phase 2 Active Power (Watt)              */
#define EM_DATA_ID_P2_ACTIVE_ENERGY                 (0xC2)  /* Phase 2 Active Energy (KWh)              */
#define EM_DATA_ID_P2_APPARENT_POWER                (0xC3)  /* Phase 2 Apparent Power (VA)              */
#define EM_DATA_ID_P2_APPARENT_ENERGY               (0xC4)  /* Phase 2 Apparent Energy (KVAh)           */
#define EM_DATA_ID_P2_AMPERE_HOUR                   (0xC5)  /* Phase 2 Ampere Hour (Ah)                 */
#define EM_DATA_ID_P2_VRMS                          (0xC6)  /* Phase 2 VRMS (Volt)                      */
#define EM_DATA_ID_P2_IRMS                          (0xC7)  /* Phase 2 I1RMS Phase channel (Ampere)     */
#define EM_DATA_ID_P2_FREQUENCY                     (0xC9)  /* Phase 2 Frequency (Hz)                   */
#define EM_DATA_ID_P2_POWER_FACTOR                  (0xCA)  /* Phase 2 Power factor.                    */
#define EM_DATA_ID_P2_REACTIVE_POWER                (0xCB)  /* Phase 2 Reactive Power (VAr)             */
#define EM_DATA_ID_P2_REACTIVE_ENERGY               (0xCC)  /* Phase 2 Reactive Energy (KVArh)          */
#define EM_DATA_ID_P2_VOLTAGE_THD                   (0xCD)  /* Phase 2 Voltage THD (%)                  */
#define EM_DATA_ID_P2_CURRENT_THD                   (0xCE)  /* Phase 2 Current THD (%)                  */
#define EM_DATA_ID_COST_H_SOCKET2                   (0xCF)  /* Cost/h Socket 2                          */

/* Phase 3 */
#define EM_DATA_ID_P3_ACTIVE_POWER                  (0xD1)  /* Phase 3 Active Power (Watt)              */
#define EM_DATA_ID_P3_ACTIVE_ENERGY                 (0xD2)  /* Phase 3 Active Energy (KWh)              */
#define EM_DATA_ID_P3_APPARENT_POWER                (0xD3)  /* Phase 3 Apparent Power (VA)              */
#define EM_DATA_ID_P3_APPARENT_ENERGY               (0xD4)  /* Phase 3 Apparent Energy (KVAh)           */
#define EM_DATA_ID_P3_AMPERE_HOUR                   (0xD5)  /* Phase 3 Ampere Hour (Ah)                 */
#define EM_DATA_ID_P3_VRMS                          (0xD6)  /* Phase 3 VRMS (Volt)                      */
#define EM_DATA_ID_P3_IRMS                          (0xD7)  /* Phase 3 I1RMS Phase channel (Ampere)     */
#define EM_DATA_ID_P3_FREQUENCY                     (0xD9)  /* Phase 3 Frequency (Hz)                   */
#define EM_DATA_ID_P3_POWER_FACTOR                  (0xDA)  /* Phase 3 Power factor.                    */
#define EM_DATA_ID_P3_REACTIVE_POWER                (0xDB)  /* Phase 3 Reactive Power (VAr)             */
#define EM_DATA_ID_P3_REACTIVE_ENERGY               (0xDC)  /* Phase 3 Reactive Energy (KVArh)          */
#define EM_DATA_ID_P3_VOLTAGE_THD                   (0xDD)  /* Phase 3 Voltage THD (%)                  */
#define EM_DATA_ID_P3_CURRENT_THD                   (0xDE)  /* Phase 3 Current THD (%)                  */

/* Accumulative parameters */
#define EM_DATA_ID_TOTAL_ACTIVE_POWER               (0xE1)  /* Total Active Power (Watt)                */
#define EM_DATA_ID_TOTAL_ACTIVE_ENERGY              (0xE2)  /* Total Active Energy (KWh)                */
#define EM_DATA_ID_TOTAL_APPARENT_POWER             (0xE3)  /* Total Apparent Power (VA)                */
#define EM_DATA_ID_TOTAL_APPARENT_ENERGY            (0xE4)  /* Total Apparent Energy (KVAh)             */
#define EM_DATA_ID_TOTAL_AMPERE_HOUR                (0xE5)  /* Total Ampere Hour (Ah)                   */
#define EM_DATA_ID_AC_VRMS                          (0xE6)  /* Total VRMS l (Ampere)                    */
#define EM_DATA_ID_AC_IRMS                          (0xE7)  /* Total IRMS l (Ampere)                    */
#define EM_DATA_ID_AC_INRMS                         (0xE8)  /* I2RMS Neutral channel (Ampere)           */
#define EM_DATA_ID_AVERAGE_FREQUENCY                (0xE9)  /* Average Frequency (Hz)                   */
#define EM_DATA_ID_SYSTEM_POWER_FACTOR              (0xEA)  /* System Power factor.                     */
#define EM_DATA_ID_TOTAL_REACTIVE_POWER             (0xEB)  /* Total Reactive Power (VAr)               */
#define EM_DATA_ID_TOTAL_REACTIVE_ENERGY            (0xEC)  /* Total Reactive Energy (KVArh)            */
#define EM_DATA_ID_AVERAGE_VOLTAGE_THD              (0xED)  /* Average Voltage THD (%)                  */
#define EM_DATA_ID_AVERAGE_CURRENT_THD              (0xEE)  /* Average Current THD (%)                  */
#define EM_DATA_ID_COST_H_ACCUMULATE                (0xEF)  /* Cost/h Accumulate                        */

#define EM_DATA_ID_TOTAL_P1_OUT_ACTIVE_ENERGY       (0xF1)  /* P1 Out-coming Active Energy (KWh)        */
#define EM_DATA_ID_TOTAL_P2_OUT_ACTIVE_ENERGY       (0xF2)  /* P2 Out-coming ActiveEnergy (KWh)         */
#define EM_DATA_ID_TOTAL_TOTAL_OUT_ACTIVE_ENERGY    (0xF3)  /* Total Out-coming Active Energy (KWh)     */

#define EM_DATA_ID_P1_FUNDAMENTAL_ACTIVE_POWER      (0xFA)  /* P1 Fundamental Active Power (Watt)       */
#define EM_DATA_ID_P2_FUNDAMENTAL_ACTIVE_POWER      (0xFB)  /* P2 Fundamental Active Power (Watt)       */
#define EM_DATA_ID_P3_FUNDAMENTAL_ACTIVE_POWER      (0xFC)  /* P3 Fundamental Active Power (Watt)       */
#define EM_DATA_ID_TOTAL_FUNDAMENTAL_ACTIVE_POWER   (0xFD)  /* Total Fundamental Active Power (Watt)    */

#define EM_DATA_ID_LED1_OUTPUT_CONFIG               (0x10)  /* LED1 output configuration                */
#define EM_DATA_ID_LED2_OUTPUT_CONFIG               (0x11)  /* LED2 output configuration                */
#define EM_DATA_ID_LED3_OUTPUT_CONFIG               (0x12)  /* LED3 output configuration                */
#define EM_DATA_ID_LED4_OUTPUT_CONFIG               (0x13)  /* LED4 output configuration                */
#define EM_DATA_ID_LED5_OUTPUT_CONFIG               (0x14)  /* LED5 output configuration                */

#define EM_DATA_ID_METER_ID                         (0x01)  /* Meter ID (octet-string ASCII)            */
#define EM_DATA_ID_ENERGY_LOGGED_DATA               (0x02)  /* Energy logged data.                      */
#define EM_DATA_ID_TAMPER_LOGGED_DATA               (0x03)  /* Tamper logged data.                      */
#define EM_DATA_ID_TARIFFS_CONFIGURATION            (0x04)  /* Tariffs configuration.                   */
#define EM_DATA_ID_RTC_DATE_TIME                    (0x05)  /* RTC Date & Time.                         */
#define EM_DATA_ID_MAX_DEMAND_DURATION              (0x06)  /* Max Demand Duration (minutes)            */
#define EM_DATA_ID_METER_CONSTANT                   (0x07)  /* Meter constant                           */
#define EM_DATA_ID_METER_CONSTANT_TOTAL             (0x07)  /* Meter constant for Total Phase           */
#define EM_DATA_ID_FIRMWARE_VERSION                 (0x08)  /* Firmware version                         */
#define EM_DATA_ID_COST_KWH                         (0x09)  /* Cost/kWh                                 */

/* Logic processing for defined ID */

#define EM_DATA_ID_IS_MEASUREMENT(id)               ( (id & 0x80) == 0x80 )
#define EM_DATA_ID_IS_CONFIGURATION(id)             (   (                                                   \
                                                            (id == EM_DATA_ID_TARIFFS_CONFIGURATION)    ||  \
                                                            (id == EM_DATA_ID_MAX_DEMAND_DURATION)      ||  \
                                                            ( ( (id >= EM_DATA_ID_LED1_OUTPUT_CONFIG) &&    \
                                                                (id <= EM_DATA_ID_LED5_OUTPUT_CONFIG)       \
                                                              )                                             \
                                                            )                                   ||          \
                                                            (0)                                             \
                                                        )                                                   \
                                                        ? 1 : 0                                             \
                                                    )

/* Checking if measurement data is energy data
 * 0x..2, 0x..4, 0x..C
 */
#define EM_DATA_ID_IS_ENERGY_DATA(id)               (   (                                               \
                                                            (id & 0x0F) == 0x02 ||                      \
                                                            (id & 0x0F) == 0x04 ||                      \
                                                            (id & 0x0F) == 0x0C ||                      \
                                                            (0)                                         \
                                                        ) &&                                            \
                                                        id != EM_DATA_ID_P3_FUNDAMENTAL_ACTIVE_POWER    \
                                                    )

/* Get EM_LINE info from ID:
 * 0xB.., 0xFA : P1, LINE_PHASE_R,
 * 0xC.., 0xFB : P2, LINE_PHASE_Y
 * 0xD.., 0xFC : P3, LINE_PHASE_B
 *       : LINE_NEUTRAL, map manually
 * 0xE.., 0xFD : TOTAL
 */
 
#define EM_DATA_ID_GET_LINE_INFO(id)               (   (                                           \
                                                            (id == EM_DATA_ID_INRMS)    ||          \
                                                            (id == EM_DATA_ID_AC_INRMS) ||          \
                                                            (0)                                     \
                                                        )                                           \
                                                        ? LINE_NEUTRAL :                            \
                                                        ( (                                         \
                                                            ((id & 0xF0) == 0xE0)   ||              \
                                                            ((id) == 0xFD)          ||              \
                                                            (0)                                     \
                                                          )                                         \
                                                          ? LINE_TOTAL : \
                                                          ( (                                         \
                                                                ((id & 0xF0) == 0xB0)   ||              \
                                                                ((id) == 0xFA)          ||              \
                                                                (0)                                     \
                                                            )    \
                                                            ? LINE_PHASE_R : \
                                                            ( (                                         \
                                                                    ((id & 0xF0) == 0xC0)   ||              \
                                                                    ((id) == 0xFB)   ||              \
                                                                    (0)                                     \
                                                               )    \
                                                               ? LINE_PHASE_Y : LINE_PHASE_B\
                                                            )\
                                                          )                                         \
                                                        )                                           \
                                                    )    
  

/* Get LED Configuration from Data:
 * Pulse source
 *  0x..2: active
 *  0x..4: apparent
 *  0x..C: reactive
 * Pulse line
 *  0xB..: Phase_R
 *  0xC..: Phase_Y
 *  0xD..: Phase_B
 *  0xE..: Total
*/
#define EM_DATA_GET_LED_CONFIG_SOURCE(config)        (   (                                           \
                                                                ((config&0x0F) == 0x02)    ||          \
                                                                (0)                                     \
                                                            )                                           \
                                                            ? SRC_ACTIVE :                            \
                                                            ( (                                         \
                                                                ((config & 0x0F) == 0x04)   ||              \
                                                                (0)                                     \
                                                               )                                        \
                                                               ? SRC_APPARENT : SRC_REACTIVE            \
                                                            )\
                                                        )
                                                          

#define EM_DATA_GET_LED_CONFIG_LINE(config)        (   (                                           \
                                                                ((config&0xF0) == 0xB0)    ||          \
                                                                (0)                                     \
                                                            )                                           \
                                                            ? LINE_PHASE_R :                            \
                                                            ( (                                         \
                                                                ((config & 0xF0) == 0xC0)   ||              \
                                                                (0)                                     \
                                                               )                                        \
                                                               ? LINE_PHASE_Y :                         \
                                                                ( (                                         \
                                                                    ((config & 0xF0) == 0xD0)   ||              \
                                                                    (0)                                     \
                                                                   )                                        \
                                                                   ? LINE_PHASE_B : LINE_TOTAL            \
                                                                )\
                                                            )   \
                                                        )

#define EM_DATA_PULSE_SETTING_TO_CONFIG(src,line)   (   (   (src == SRC_ACTIVE) ? 0x02 :                \
                                                            ((src == SRC_APPARENT) ? 0x04 : 0x0C)       \
                                                        ) |                                             \
                                                        (   (line == LINE_PHASE_R) ? 0xB0 :             \
                                                            ((line == LINE_PHASE_Y) ? 0xC0 :            \
                                                            ((line == LINE_PHASE_B) ? 0xD0 : 0xE0))     \
                                                        )                                               \
                                                    )
/* LED config offset: 
 * LED1: 0x10
 * .
 * .
 * .
 * LED5: 0x14
*/
#define EM_DATA_ID_GET_LED_CONFIG_OFFSET(config)        (config & 0x0F)
                                                    
                                                    
#endif /*  __SERVICE_EMDATA_H */
