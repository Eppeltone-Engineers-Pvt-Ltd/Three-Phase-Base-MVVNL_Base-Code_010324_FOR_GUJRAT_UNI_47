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

#ifndef __SERVICE_EMLOGGING_H
#define __SERVICE_EMLOGGING_H

/******************************************************************************
Macro definitions
******************************************************************************/
/* OPERATION CONFIG: */

#define RAW_DATA_STORE_IN_EEPROM                            (0) /* Raw data store in EEPROM? if 0 will store in RAM */
#define RAW_DATA_EEPROM_TYPE                                (0) /* 0: I2C, 1: SPI type */

#if (RAW_DATA_STORE_IN_EEPROM == 1)
    #define RAW_DATA_MAX_CHANNEL                                (3)         /* Maximum channel support */
    
    /* Checking EPR */
    #if (RAW_DATA_EEPROM_TYPE != 0 && RAW_DATA_EEPROM_TYPE != 1)
        #error "ServiceDataLogging: Support I2C and SPI EPR type only"
    #endif
    
    #if (RAW_DATA_MAX_CHANNEL > 3)
        #error "In EPR mode: maximum 3 channel allowed"
    #endif
    
    /* Below starting address is just based on common used EEPROM type in EM EVB and Reference Meter */
    /* Usually, I2C and SPI page size are: 128 and 256 bytes respectively */
    #if (RAW_DATA_EEPROM_TYPE == 0)
        #define RAW_DATA_EEPROM_BUFF_ADDR                       (0x2000)    /* Start offset on memory address */
        #define RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH              (32)        /* Half-buffer sample size */
        #define RAW_DATA_ADC_REAL_NUMBER_SAMPLE                 (2000)
        
    #else
        #define RAW_DATA_EEPROM_BUFF_ADDR                       (0x2000)    /* Start offset on memory address */
        #define RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH              (64)        /* Half-buffer sample size */
        #define RAW_DATA_ADC_REAL_NUMBER_SAMPLE                 (10000)
        
    #endif
    
    #define RAW_DATA_ADC_FS_MAX                                 (3)         /* Maximum allowed fs constraint by signal resolution */
    
    /* When using EEPROM, make sure half buffer size is equal to one page */
    #if ((RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH * 4) != EPR_DEVICE_PAGESIZE)
        #error "In EPR mode, one half buffer must be within 1 EPR page"
    #endif
    
    /* Also make sure the start address modulo EEPROM page size must be 0 */
    #if ((RAW_DATA_EEPROM_BUFF_ADDR % EPR_DEVICE_PAGESIZE) != 0)
        #error "In EPR mode, value of RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH modulo EPR Page size must be 0"
    #endif
    
    #define RAW_DATA_HALF_BUFFER_PAGE_SIZE                  (EPR_DEVICE_PAGESIZE)
    
#else
    #define RAW_DATA_MAX_CHANNEL                            (7)         /* Maximum channel support */
    
    /* Common */
    #define RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH              (50)        // Originally 100 changed to 50 temporarily Half-buffer sample size */
// above line change must be restored to 100 - for DLMS
    #define RAW_DATA_ADC_FS_MAX                             (1)         /* Maximum allowed fs constraint by signal resolution */
    #define RAW_DATA_ADC_REAL_NUMBER_SAMPLE                 (RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH * 2)     /* Number of samples for each buf */
    
    /* Length of double buffer length modulo 50 must be 0 */
    #if ((RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH * 2) % 50 != 0)
        #error "In RAM mode, value of RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH modulo 50 must be 0"
    #endif
    
    #define RAW_DATA_HALF_BUFFER_PAGE_SIZE                  (50)
#endif

#if (RAW_DATA_STORE_IN_EEPROM != 0 && RAW_DATA_STORE_IN_EEPROM != 1)
    #error "ServiceDataLogging: Choose EPR or RAM mode only"
#endif



/* Configure clock used in datalogging mode and normal clock 
 * Leave blank if both mode clock is the same                                
*/
#define SWITCH_DATALOGGING_CLOCK()                      {;}
#define SWITCH_NORMAL_OPERATING_CLOCK()                 {;}

/* ADC CAPTURE */
#define RAW_DATA_ADC_HALF_DOUBLE_BUFFER_LENGTH          (RAW_DATA_ADC_REAL_NUMBER_SAMPLE / 2)        /* Half-buffer size */
#define RAW_DATA_ADC_NUMBER_SAMPLE_NEED                 (((RAW_DATA_ADC_REAL_NUMBER_SAMPLE / RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH) +\
                                                            (((RAW_DATA_ADC_REAL_NUMBER_SAMPLE % RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH) == 0) ? 0 : 1)) *\
                                                            RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH)

#define RAW_DATA_ADC_ORIGINAL_SAMPLING_RATE             (3906)      /* ADC Original sampling rate */
#define RAW_DATA_ADC_INTEGRATOR_STABLE_INT_COUNT        (140)       /* ~2 line cycles */

/* Settings below are fixed, based on protocol */
#define RAW_DATA_ADC_SAMPLE_PER_BLOCK                   (50)        /* number of sample per block */
#define RAW_DATA_ADC_BLOCK_SIZE                         (RAW_DATA_ADC_SAMPLE_PER_BLOCK * sizeof(int32_t))
#define RAW_DATA_ADC_SAMPLING_INFO_BUFF_SIZE            (48)        /* ADC Sampling Characteristics Info size */


/* MEASUREMENT CAPTURE:
 * Number of sample fixed at 10 
 * Reason: value update 1s each --> 10 samples nearly 10s
 * too long will cause timeout on GUI
 * 1sample/sec is enough to see changes
*/
#define RAW_DATA_MEASUREMENT_REAL_NUMBER_SAMPLE         (10)        /* 64 samples for each buf (3 buf) */
#define RAW_DATA_MEASUREMENT_HALF_DOUBLE_BUFFER_LENGTH  (RAW_DATA_MEASUREMENT_REAL_NUMBER_SAMPLE/2)         /* Half-buffer size for measurement */
#define RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK           (5)         /* number of sample per block */
#define RAW_DATA_MEASUREMENT_BLOCK_SIZE                 (RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK * sizeof(float32_t))
#define RAW_DATA_MEASUREMENT_BLOCK_SIZE_LONG            (RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK * sizeof(double64_t))
#define RAW_DATA_MEASUREMENT_FS_DIVIDER                 (24)        /* ~1second for 40ms timer interval */

/* Check user configuration */
#if (RAW_DATA_ADC_REAL_NUMBER_SAMPLE > (65536/4))
    #error "Check the RAW_DATA_ADC_REAL_NUMBER_SAMPLE size"
#endif

#if (RAW_DATA_ADC_REAL_NUMBER_SAMPLE <= RAW_DATA_MEASUREMENT_REAL_NUMBER_SAMPLE)
    #error "ADC Num of sample must larger than that of Measurement"
#endif

/* Common no select ID */
#define RAW_DATA_ID_NO_SELECT                           (0x00)      /* (No select)                          */

/* Table1 Data ID for Phase1 */
#define RAW_DATA_ID_P1_NO_SELECT                        (0x10)      /* (No select)                          */
#define RAW_DATA_ID_P1_VOLTAGE_SAMPLES                  (0x11)      /* Voltage samples                      */
#define RAW_DATA_ID_P1_CURRENT_SAMPLES                  (0x12)      /* I1 samples – Phase                   */
#define RAW_DATA_ID_NEUTRALCURRENT_SAMPLES              (0x13)      /* I2 samples – Neutral                 */
#define RAW_DATA_ID_P1_CURRENT_INTEGRATED_SAMPLES       (0x14)      /* I1 integrated samples – Phase        */
#define RAW_DATA_ID_NEUTRALCURRENT_INTEGRATED_SAMPLES   (0x15)      /* I2 integrated samples – Neutral      */
#define RAW_DATA_ID_P1_VRMS                             (0x16)      /* VRMS (Volt)                          */
#define RAW_DATA_ID_P1_IRMS                             (0x17)      /* I1RMS – Phase channel (A)            */
#define RAW_DATA_ID_INRMS                               (0x18)      /* I2RMS – Phase channel (A)            */
#define RAW_DATA_ID_P1_FREQUENCY                        (0x19)      /* Frequency (Hz)                       */
#define RAW_DATA_ID_P1_POWER_FACTOR                     (0x1A)      /* Power Factor                         */
#define RAW_DATA_ID_P1_ACTIVE_POWER                     (0x1B)      /* Active Power (Watt)                  */
#define RAW_DATA_ID_P1_ACTIVE_ENERGY                    (0x1C)      /* Active Energy (KWh)                  */
#define RAW_DATA_ID_P1_APPARENT_POWER                   (0x1D)      /* Apparent Power (VA)                  */
#define RAW_DATA_ID_P1_APPARENT_ENERGY                  (0x1E)      /* Apparent Energy (KVAh)               */

/* Table1 Data ID for Phase2 */
#define RAW_DATA_ID_P2_NO_SELECT                        (0x20)      /* (No select)                          */
#define RAW_DATA_ID_P2_VOLTAGE_SAMPLES                  (0x21)      /* Phase2 Voltage samples               */
#define RAW_DATA_ID_P2_CURRENT_SAMPLES                  (0x22)      /* Phase2 I1 samples Phase              */
#define RAW_DATA_ID_P2_CURRENT_INTEGRATED_SAMPLES       (0x24)      /* Phase2 I1 integrated samples Phase   */
#define RAW_DATA_ID_P2_VRMS                             (0x26)      /* Phase2 VRMS (Volt)                   */
#define RAW_DATA_ID_P2_IRMS                             (0x27)      /* Phase2 I1RMS Phase channel (A)       */
#define RAW_DATA_ID_P2_FREQUENCY                        (0x29)      /* Phase2 Frequency (Hz)                */
#define RAW_DATA_ID_P2_POWER_FACTOR                     (0x2A)      /* Phase2 Power Factor                  */
#define RAW_DATA_ID_P2_ACTIVE_POWER                     (0x2B)      /* Phase2 Active Power (Watt)           */
#define RAW_DATA_ID_P2_ACTIVE_ENERGY                    (0x2C)      /* Phase2 Active Energy (KWh)           */
#define RAW_DATA_ID_P2_APPARENT_POWER                   (0x2D)      /* Phase2 Apparent Power (VA)           */
#define RAW_DATA_ID_P2_APPARENT_ENERGY                  (0x2E)      /* Phase2 Apparent Energy (KVAh)        */

/* Table1 Data ID for Phase3 */
#define RAW_DATA_ID_P3_NO_SELECT                        (0x30)      /* (No select)                          */
#define RAW_DATA_ID_P3_VOLTAGE_SAMPLES                  (0x31)      /* Phase3 Voltage samples               */
#define RAW_DATA_ID_P3_CURRENT_SAMPLES                  (0x32)      /* Phase3 I1 samples Phase              */
#define RAW_DATA_ID_P3_CURRENT_INTEGRATED_SAMPLES       (0x34)      /* Phase3 I1 integrated samples Phase   */
#define RAW_DATA_ID_P3_VRMS                             (0x36)      /* Phase3 VRMS (Volt)                   */
#define RAW_DATA_ID_P3_IRMS                             (0x37)      /* Phase3 I1RMS Phase channel (A)       */
#define RAW_DATA_ID_P3_FREQUENCY                        (0x39)      /* Phase3 Frequency (Hz)                */
#define RAW_DATA_ID_P3_POWER_FACTOR                     (0x3A)      /* Phase3 Power Factor                  */
#define RAW_DATA_ID_P3_ACTIVE_POWER                     (0x3B)      /* Phase3 Active Power (Watt)           */
#define RAW_DATA_ID_P3_ACTIVE_ENERGY                    (0x3C)      /* Phase3 Active Energy (KWh)           */
#define RAW_DATA_ID_P3_APPARENT_POWER                   (0x3D)      /* Phase3 Apparent Power (VA)           */
#define RAW_DATA_ID_P3_APPARENT_ENERGY                  (0x3E)      /* Phase3 Apparent Energy (KVAh)        */

/* Table1 Data ID for Total parameters */
#define RAW_DATA_ID_TOTAL_NO_SELECT                     (0x40)      /* (No select)                          */
#define RAW_DATA_ID_TOTAL_AVERAGE_FREQUENCY             (0x49)      /* Average Frequency (Hz)               */
#define RAW_DATA_ID_TOTAL_ACTIVE_POWER                  (0x4B)      /* Total Active power (Watt)            */
#define RAW_DATA_ID_TOTAL_ACTIVE_ENERGY                 (0x4C)      /* Total Active energy (KWh)            */
#define RAW_DATA_ID_TOTAL_APPARENT_POWER                (0x4D)      /* Total Apparent power (VA)            */
#define RAW_DATA_ID_TOTAL_APPARENT_ENERGY               (0x4E)      /* Total Apparent energy (KVAh)         */

/* DataID Table 2 */
/* Table2 Data ID for Phase1 */
#define RAW_DATA_ID_P1_NO_SELECT2                       (0x50)      /* (No select)                          */
#define RAW_DATA_ID_P1_VOLTAGE_THD                      (0x56)      /* Phase1 Voltage THD (%)               */
#define RAW_DATA_ID_P1_CURRENT_THD                      (0x57)      /* Phase1 Current THD (%)               */
#define RAW_DATA_ID_P1_REACTIVE_POWER                   (0x5B)      /* Phase1 Reactive power (VAr)          */
#define RAW_DATA_ID_P1_REACTIVE_ENERGY                  (0x5C)      /* Phase1 Reactive energy (KVArh)       */

/* Table2 Data ID for Phase2 */
#define RAW_DATA_ID_P2_NO_SELECT2                       (0x60)      /* (No select)                          */
#define RAW_DATA_ID_P2_VOLTAGE_THD                      (0x66)      /* Phase2 Voltage THD (%)               */
#define RAW_DATA_ID_P2_CURRENT_THD                      (0x67)      /* Phase2 Current THD (%)               */
#define RAW_DATA_ID_P2_REACTIVE_POWER                   (0x6B)      /* Phase2 Reactive power (VAr)          */
#define RAW_DATA_ID_P2_REACTIVE_ENERGY                  (0x6C)      /* Phase2 Reactive energy (KVArh)       */

/* Table2 Data ID for Phase3 */
#define RAW_DATA_ID_P3_NO_SELECT2                       (0x70)      /* (No select)                          */
#define RAW_DATA_ID_P3_VOLTAGE_THD                      (0x76)      /* Phase3 Voltage THD (%)               */
#define RAW_DATA_ID_P3_CURRENT_THD                      (0x77)      /* Phase3 Current THD (%)               */
#define RAW_DATA_ID_P3_REACTIVE_POWER                   (0x7B)      /* Phase3 Reactive power (VAr)          */
#define RAW_DATA_ID_P3_REACTIVE_ENERGY                  (0x7C)      /* Phase3 Reactive energy (KVArh)       */

/* Table2 Data ID for Total parameters */
#define RAW_DATA_ID_TOTAL_NO_SELECT2                    (0x80)      /* (No select)                          */
#define RAW_DATA_ID_TOTAL_AVERAGE_VOLTAGE_THD           (0x86)      /* Average Voltage THD (%)              */
#define RAW_DATA_ID_TOTAL_AVERAGE_CURRENT_THD           (0x87)      /* Average Current THD (%)              */
#define RAW_DATA_ID_TOTAL_REACTIVE_POWER                (0x8B)      /* Total Reactive power (VAr)           */
#define RAW_DATA_ID_TOTAL_REACTIVE_ENERGY               (0x8C)      /* Total Reactive energy (KVArh)        */

/* For integrator control */
#define RAW_DATA_ID_INTEGRATOR_USED_MASK                (0x10)      /* Mask to indicate integrator 0 is used or not */

/* Checking whether input id is voltage or not */
#define RAW_DATA_ID_IS_VOLTAGE_ID(id)               (   (                                               \
                                                            (id == RAW_DATA_ID_P1_VOLTAGE_SAMPLES) ||   \
                                                            (id == RAW_DATA_ID_P2_VOLTAGE_SAMPLES) ||   \
                                                            (id == RAW_DATA_ID_P3_VOLTAGE_SAMPLES) ||   \
                                                            (0)                                         \
                                                        )                                               \
                                                        ? 1 : 0                                         \
                                                    )
#define RAW_DATA_ID_IS_ENERGY_DATA(id)              (                               \
                                                        (id & 0x0F) == 0x0C ||      \
                                                        (id & 0x0F) == 0x0E ||      \
                                                        (0)                         \
                                                    )
/* Get EM_LINE info from ID:
 * 0x0..,0x4..  : P1, LINE_PHASE_R,
 * 0x1..,0x5..  : P2, LINE_PHASE_Y
 * 0x2..,0x6..  : P3, LINE_PHASE_B
 *              : LINE_NEUTRAL, map manually
 * 0x7.., 0x3.. : TOTAL
 */
#define RAW_DATA_ID_GET_LINE_INFO(id)               (   (                                           \
                                                            (id == RAW_DATA_ID_INRMS)    ||         \
                                                            (0)                                     \
                                                        )                                           \
                                                        ? LINE_NEUTRAL :                            \
                                                        ( (                                         \
                                                            ((id & 0xF0) == 0x80)   ||              \
                                                            ((id & 0xF0) == 0x40)   ||              \
                                                            (0)                                     \
                                                          )                                         \
                                                          ? LINE_TOTAL : \
                                                          ( (                                         \
                                                                ((id & 0xF0) == 0x10)   ||              \
                                                                ((id & 0xF0) == 0x50)   ||              \
                                                                (0)                                     \
                                                            )    \
                                                            ? LINE_PHASE_R : \
                                                            ( (                                         \
                                                                    ((id & 0xF0) == 0x20)   ||              \
                                                                    ((id & 0xF0) == 0x60)   ||              \
                                                                    (0)                                     \
                                                               )    \
                                                               ? LINE_PHASE_Y : LINE_PHASE_B\
                                                            )\
                                                          )                                         \
                                                        )                                           \
                                                    )    

                                
/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    int32_t *           p_first_half;
    int32_t *           p_second_half;
    uint32_t            storage_start_addr;
    uint16_t            storage_addr_offset;
    uint16_t            half_buffer_size;
    uint16_t            one_sample_size;
    int8_t              is_selected;
    int8_t              lookup_id;
    uint8_t             is_voltage_id;
    uint8_t             reserved;
} raw_data_channel_info_t;

typedef struct
{
    raw_data_channel_info_t   channel[RAW_DATA_MAX_CHANNEL];
} raw_data_channel_t;

typedef struct
{
    uint8_t     is_enable;
    uint8_t     is_requested;
    uint8_t     is_core_stopped;
    int8_t      is_adc_or_measurement;
    uint8_t     timeout;
    uint8_t     buff_filled_state;              /* Double buffer fill state */
    uint16_t    buff_count;                     /* Buffer index count */
    uint16_t    buff_total_sample;              /* Actual sample count */
    uint16_t    buff_total_saved_sample;        /* Number of samples saved to EEPROM */
    uint16_t    buff_num_of_sample_need;        /* Actual number of samples need to collect */
    uint16_t    buff_rollover_point;            /* Point to rollover the buff index */
} raw_data_service_info_t;

#if (RAW_DATA_STORE_IN_EEPROM == 1)
#define RAW_DATA_Read(addr,buf,size)            EPR_Read(addr,buf,size)
#define RAW_DATA_Write(addr,buf,size)           EPR_Write(addr,buf,size)

#else
#define RAW_DATA_Read(addr,buf,size)            _COM_memcpy_ff(buf,(const void __far *)addr,size)
#define RAW_DATA_Write(addr,buf,size)           {;}                         /* Already write to RAM */


#endif
/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#endif /*  __SERVICE_EMLOGGING_H */
