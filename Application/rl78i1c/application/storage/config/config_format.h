/***********************************************************************************************************************
* File Name    : cfg_format.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Storage Format Header file

* Note         : NEED TO CHANGE THIS FILE WHEN
*              :   . EM CORE TYPE DEFINITION IS CHANGED, OR
*              :   . CONFIG START ADDRESS IS CHANGED
* Caution      : DATA ALIGNMENT
*              :    DATA ALIGNMENT ON THIS FORMAT IS 2-BYTES ALIGNMENT, EVEN ADDRESS.
*              :    PLEASE DON'T CHANGE TO OTHER ALIGNMENT SETTING.
*              :    WHEN CHANGE THE DATA ALIGNMENT SETTING, IT WILL DAMAGE THE FORMAT
*              :    ON CONFIG
*              :
*              : BIT FIELD FORMAT
*              :    BIT FIELD FORMAT IS FROM LSB FIRST
*              :    PLEASE DON'T CHANGE THIS SETTING ON THE COMPILE OPTION (IF SUPPORTED)

***********************************************************************************************************************/

#ifndef _CONFIG_STORAGE_FORMAT_H
#define _CONFIG_STORAGE_FORMAT_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/* Middleware */
#include "em_type.h"        /* EM Core Type Definitions */
#include "em_constraint.h"

/* Application */
#include "dataflash.h"         /* Dataflash MW */

/******************************************************************************
Macro definitions for Typedef
******************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/* CONFIG Storage Header */
typedef struct tagCfgStorageHeader
{
    /* Total: 10 Bytes */
    uint32_t    header_code;                /* (4 bytes) Header Code (EM Formatted Header Code)  */
    uint16_t    usage;                      /* (2 bytes) Usage (number of bytes)                 */
    uint16_t    num_of_config;              /* (2 bytes) Number of EM Configuration bytes        */
    uint16_t    num_of_calib;               /* (2 bytes) Number of EM Calibration bytes          */
    
} CONFIG_STORAGE_HEADER;

/* CONFIG CRC Code */
typedef struct tagCfgStorageCRC
{
    /* Total: 2 Bytes */
    uint16_t    calibration;                /* (2 bytes) CRC Code for EM Calib                   */

} CONFIG_STORAGE_CRC;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* CONFIG Information */
#define CONFIG_STORAGE_START_ADDR                       LDEV_DEVICE_START_ADDR   /* CONFIG Start address */
#define CONFIG_STORAGE_SIZE                             LDEV_DEVICE_SIZE         /* CONFIG Size */
#define CONFIG_STORAGE_PAGESIZE                         LDEV_DEVICE_PAGESIZE     /* CONFIG Page Size */

/* CONFIG Storage Type */
#define CONFIG_STORAGE_TYPE_INTEGER8                    0       /* uint8_t            */
#define CONFIG_STORAGE_TYPE_ARRAY_INTEGER8              1       /* array of uint8_t   */
#define CONFIG_STORAGE_TYPE_INTEGER16                   2       /* uint16_t           */
#define CONFIG_STORAGE_TYPE_ARRAY_INTEGER16             3       /* array of uint16_t  */
#define CONFIG_STORAGE_TYPE_INTEGER32                   4       /* uint32_t           */
#define CONFIG_STORAGE_TYPE_ARRAY_INTEGER32             5       /* array of uint32_t  */
#define CONFIG_STORAGE_TYPE_FLOAT32                     6       /* float32_t          */
#define CONFIG_STORAGE_TYPE_ARRAY_FLOAT32               7       /* array of float32_t */
#define CONFIG_STORAGE_TYPE_STRUCT                      8       /* (struct)           */
#define CONFIG_STORAGE_TYPE_ARRAY_STRUCT                9       /* (array of struct)  */

/* CONFIG Group Enable/Disable Macro */
#define CONFIG_STORAGE_HEADER_GROUP                     1
#define CONFIG_STORAGE_CRC_GROUP                        1
#define CONFIG_STORAGE_CONFIG_GROUP                     1
#define CONFIG_STORAGE_CALIB_GROUP                      1

/* CONFIG Header Structure Location + Size
 * 11 Macro Items */
#if CONFIG_STORAGE_HEADER_GROUP

	#define CONFIG_STORAGE_HEADER_ADDR                      (CONFIG_STORAGE_START_ADDR + 0x0000)                                    /* Offset 0x0000 */
	#define CONFIG_STORAGE_HEADER_SIZE                      (sizeof(CONFIG_STORAGE_HEADER))                                         /* 24 Bytes */
	#define CONFIG_STORAGE_HEADER_TYPE                      CONFIG_STORAGE_TYPE_STRUCT                                              /* Structure */
    
    /* Header Code */
    #define CONFIG_STORAGE_HEADER_HEADER_CODE_ADDR          (CONFIG_STORAGE_HEADER_ADDR                   + 0)                  /* First Element */
    #define CONFIG_STORAGE_HEADER_HEADER_CODE_SIZE          (sizeof(uint32_t))
    #define CONFIG_STORAGE_HEADER_HEADER_CODE_TYPE          CONFIG_STORAGE_TYPE_INTEGER32

    /* Usage */
    #define CONFIG_STORAGE_HEADER_USAGE_ADDR                (CONFIG_STORAGE_HEADER_HEADER_CODE_ADDR       + CONFIG_STORAGE_HEADER_HEADER_CODE_SIZE)
    #define CONFIG_STORAGE_HEADER_USAGE_SIZE                (sizeof(uint16_t))
    #define CONFIG_STORAGE_HEADER_USAGE_TYPE                CONFIG_STORAGE_TYPE_INTEGER16

    /* Number of configuration bytes */
    #define CONFIG_STORAGE_HEADER_NUM_CONFIG_ADDR           (CONFIG_STORAGE_HEADER_USAGE_ADDR             + CONFIG_STORAGE_HEADER_USAGE_SIZE)
    #define CONFIG_STORAGE_HEADER_NUM_CONFIG_SIZE           (sizeof(uint16_t))
    #define CONFIG_STORAGE_HEADER_NUM_CONFIG_TYPE           CONFIG_STORAGE_TYPE_INTEGER16

    /* Number of calibration bytes */
    #define CONFIG_STORAGE_HEADER_NUM_CALIB_ADDR            (CONFIG_STORAGE_HEADER_NUM_CONFIG_ADDR        + CONFIG_STORAGE_HEADER_NUM_CONFIG_SIZE)
    #define CONFIG_STORAGE_HEADER_NUM_CALIB_SIZE            (sizeof(uint16_t))
    #define CONFIG_STORAGE_HEADER_NUM_CALIB_TYPE            CONFIG_STORAGE_TYPE_INTEGER16
    
#endif /* CONFIG_STORAGE_HEADER_GROUP */

/* CONFIG CRC Structure Location + Size
 * 1 Macro Items */
#if CONFIG_STORAGE_CRC_GROUP

	#define CONFIG_STORAGE_CRC_ADDR                         (CONFIG_STORAGE_START_ADDR + 0x001A)                                    /* Offset 0x0018 */
	#define CONFIG_STORAGE_CRC_SIZE                         (sizeof(CONFIG_STORAGE_CRC))                                            /* 2 Bytes */
	#define CONFIG_STORAGE_CRC_TYPE                         CONFIG_STORAGE_TYPE_STRUCT                                              /* Structure */
    
    /* Calibration */
    #define CONFIG_STORAGE_CRC_CALIBRATION_ADDR         (CONFIG_STORAGE_CRC_ADDR                    + 0)                        /* First Element */
    #define CONFIG_STORAGE_CRC_CALIBRATION_SIZE         (sizeof(uint16_t))
    #define CONFIG_STORAGE_CRC_CALIBRATION_TYPE         CONFIG_STORAGE_TYPE_INTEGER16

#endif /* CONFIG_STORAGE_CRC_GROUP */

/* CONFIG EM Core Structure location + size
 * 11 Macro Items */
#if CONFIG_STORAGE_CONFIG_GROUP

	#define CONFIG_STORAGE_CONFIG_ADDR                      (CONFIG_STORAGE_START_ADDR + 0x001C)                                    /* Offset 0x0020 */
	#define CONFIG_STORAGE_CONFIG_SIZE                      (sizeof(EM_CONFIG) - sizeof(EM_TIME_SLOT *)   + sizeof(EM_TIME_SLOT)*6) /* 38 Bytes */
	#define CONFIG_STORAGE_CONFIG_TYPE                      CONFIG_STORAGE_TYPE_STRUCT                                              /* Structure */

    /* Voltage low */
    #define CONFIG_STORAGE_CONFIG_VOLTAGE_LOW_ADDR          (CONFIG_STORAGE_CONFIG_ADDR                   + 0)                  /* First Element */
    #define CONFIG_STORAGE_CONFIG_VOLTAGE_LOW_SIZE          (sizeof(uint16_t))
    #define CONFIG_STORAGE_CONFIG_VOLTAGE_LOW_TYPE          CONFIG_STORAGE_TYPE_INTEGER16

    /* Voltage high */
    #define CONFIG_STORAGE_CONFIG_VOLTAGE_HIGH_ADDR         (CONFIG_STORAGE_CONFIG_VOLTAGE_LOW_ADDR       + CONFIG_STORAGE_CONFIG_VOLTAGE_LOW_SIZE)
    #define CONFIG_STORAGE_CONFIG_VOLTAGE_HIGH_SIZE         (sizeof(uint16_t))
    #define CONFIG_STORAGE_CONFIG_VOLTAGE_HIGH_TYPE         CONFIG_STORAGE_TYPE_INTEGER16

    /* Current high */
    #define CONFIG_STORAGE_CONFIG_CURRENT_HIGH_ADDR         (CONFIG_STORAGE_CONFIG_VOLTAGE_HIGH_ADDR      + CONFIG_STORAGE_CONFIG_VOLTAGE_HIGH_SIZE)
    #define CONFIG_STORAGE_CONFIG_CURRENT_HIGH_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_CURRENT_HIGH_TYPE         CONFIG_STORAGE_TYPE_INTEGER8

    /* Freq low */
    #define CONFIG_STORAGE_CONFIG_FREQ_LOW_ADDR             (CONFIG_STORAGE_CONFIG_CURRENT_HIGH_ADDR      + CONFIG_STORAGE_CONFIG_CURRENT_HIGH_SIZE)
    #define CONFIG_STORAGE_CONFIG_FREQ_LOW_SIZE             (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_FREQ_LOW_TYPE             CONFIG_STORAGE_TYPE_INTEGER8

    /* Freq high */
    #define CONFIG_STORAGE_CONFIG_FREQ_HIGH_ADDR            (CONFIG_STORAGE_CONFIG_FREQ_LOW_ADDR          + CONFIG_STORAGE_CONFIG_FREQ_LOW_SIZE)
    #define CONFIG_STORAGE_CONFIG_FREQ_HIGH_SIZE            (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_FREQ_HIGH_TYPE            CONFIG_STORAGE_TYPE_INTEGER8

    /* Max demand period */
    #define CONFIG_STORAGE_CONFIG_MDM_PERIOD_ADDR           (CONFIG_STORAGE_CONFIG_FREQ_HIGH_ADDR         + CONFIG_STORAGE_CONFIG_FREQ_HIGH_SIZE)
    #define CONFIG_STORAGE_CONFIG_MDM_PERIOD_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_MDM_PERIOD_TYPE           CONFIG_STORAGE_TYPE_INTEGER8

    /* Pulse0 default configuration */              
    #define CONFIG_STORAGE_CONFIG_PULSE0_SOURCE_ADDR         (CONFIG_STORAGE_CONFIG_MDM_PERIOD_ADDR + CONFIG_STORAGE_CONFIG_MDM_PERIOD_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE0_SOURCE_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE0_SOURCE_TYPE         CONFIG_STORAGE_TYPE_INTEGER8
                    
    #define CONFIG_STORAGE_CONFIG_PULSE0_LINE_ADDR           (CONFIG_STORAGE_CONFIG_PULSE0_SOURCE_ADDR + CONFIG_STORAGE_CONFIG_PULSE0_SOURCE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE0_LINE_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE0_LINE_TYPE           CONFIG_STORAGE_TYPE_INTEGER8
                
    /* Pulse1 default configuration */              
    #define CONFIG_STORAGE_CONFIG_PULSE1_SOURCE_ADDR         (CONFIG_STORAGE_CONFIG_PULSE0_LINE_ADDR + CONFIG_STORAGE_CONFIG_PULSE0_LINE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE1_SOURCE_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE1_SOURCE_TYPE         CONFIG_STORAGE_TYPE_INTEGER8
                    
    #define CONFIG_STORAGE_CONFIG_PULSE1_LINE_ADDR           (CONFIG_STORAGE_CONFIG_PULSE1_SOURCE_ADDR + CONFIG_STORAGE_CONFIG_PULSE1_SOURCE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE1_LINE_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE1_LINE_TYPE           CONFIG_STORAGE_TYPE_INTEGER8

    /* Pulse2 default configuration */
    #define CONFIG_STORAGE_CONFIG_PULSE2_SOURCE_ADDR         (CONFIG_STORAGE_CONFIG_PULSE1_LINE_ADDR + CONFIG_STORAGE_CONFIG_PULSE1_LINE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE2_SOURCE_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE2_SOURCE_TYPE         CONFIG_STORAGE_TYPE_INTEGER8

    #define CONFIG_STORAGE_CONFIG_PULSE2_LINE_ADDR           (CONFIG_STORAGE_CONFIG_PULSE2_SOURCE_ADDR + CONFIG_STORAGE_CONFIG_PULSE2_SOURCE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE2_LINE_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE2_LINE_TYPE           CONFIG_STORAGE_TYPE_INTEGER8
                
    /* Pulse3 default configuration */              
    #define CONFIG_STORAGE_CONFIG_PULSE3_SOURCE_ADDR         (CONFIG_STORAGE_CONFIG_PULSE2_LINE_ADDR + CONFIG_STORAGE_CONFIG_PULSE2_LINE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE3_SOURCE_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE3_SOURCE_TYPE         CONFIG_STORAGE_TYPE_INTEGER8

    #define CONFIG_STORAGE_CONFIG_PULSE3_LINE_ADDR           (CONFIG_STORAGE_CONFIG_PULSE3_SOURCE_ADDR + CONFIG_STORAGE_CONFIG_PULSE3_SOURCE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE3_LINE_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE3_LINE_TYPE           CONFIG_STORAGE_TYPE_INTEGER8
                
    /* Pulse4 default configuration */              
    #define CONFIG_STORAGE_CONFIG_PULSE4_SOURCE_ADDR         (CONFIG_STORAGE_CONFIG_PULSE3_LINE_ADDR + CONFIG_STORAGE_CONFIG_PULSE3_LINE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE4_SOURCE_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE4_SOURCE_TYPE         CONFIG_STORAGE_TYPE_INTEGER8

    #define CONFIG_STORAGE_CONFIG_PULSE4_LINE_ADDR           (CONFIG_STORAGE_CONFIG_PULSE4_SOURCE_ADDR + CONFIG_STORAGE_CONFIG_PULSE4_SOURCE_SIZE)
    #define CONFIG_STORAGE_CONFIG_PULSE4_LINE_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_PULSE4_LINE_TYPE           CONFIG_STORAGE_TYPE_INTEGER8
    
    /* Number of tariff */
    #define CONFIG_STORAGE_CONFIG_NUM_TARIFF_ADDR           (CONFIG_STORAGE_CONFIG_PULSE4_LINE_ADDR   + CONFIG_STORAGE_CONFIG_PULSE4_LINE_SIZE)
    #define CONFIG_STORAGE_CONFIG_NUM_TARIFF_SIZE           (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_NUM_TARIFF_TYPE           CONFIG_STORAGE_TYPE_INTEGER8

    /* Number of time slot */
    #define CONFIG_STORAGE_CONFIG_NUM_TIMESLOT_ADDR         (CONFIG_STORAGE_CONFIG_NUM_TARIFF_ADDR        + CONFIG_STORAGE_CONFIG_NUM_TARIFF_SIZE)
    #define CONFIG_STORAGE_CONFIG_NUM_TIMESLOT_SIZE         (sizeof(uint8_t))
    #define CONFIG_STORAGE_CONFIG_NUM_TIMESLOT_TYPE         CONFIG_STORAGE_TYPE_INTEGER8
    
    /* Times lot array, 6 elements,
     * 1 element is a EM_TIME_SLOT struct 
     * EM_TIME_SLOT is defined in em_type.h, structure as below
     *   . First byte  (uint8_t) is time_start.hour
     *   . Second byte (uint8_t) is time_start.min
     *   . Third byte  (uint8_t) is tariff_no
     *   . Fourth byte (uint8_t) is reserved (NO USE)
     */
    #define CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_ADDR       (CONFIG_STORAGE_CONFIG_NUM_TIMESLOT_ADDR      + CONFIG_STORAGE_CONFIG_NUM_TIMESLOT_SIZE)
    #define CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_SIZE       (sizeof(EM_TIME_SLOT) * 6)
    #define CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_TYPE       CONFIG_STORAGE_TYPE_ARRAY_STRUCT
    
#endif /* CONFIG_STORAGE_CONFIG_GROUP */

/* EM Calibration Structure location + size :
 * 15 Macro Items */
/*      Common:                     = 14 bytes
    *   Coeff, phase:   20*3        = 60 bytes 
    *   Coeff, neutral:    4        = 4  bytes
    *   Offset:                     = 24 bytes  
    *   SW Gain Pointer Array: 3*2  = 6  bytes
    *   SW Phase Pointer Array:3*2  = 6  bytes
    *   ==>         Total           = 114 bytes                 (1)
    *   CAUTIONS: Pointer size assume to be 2 bytes
    *   SW Gain:
    *       . 4 array sw gain:      = -3*2      = -6 bytes
    *       . Each array 6*4bytes   =  6*4*3    =  72 bytes
    *   ==> 66 bytes (2)
    *   SW Phase:
    *       . 3 array sw phase:     = -3*2      = -6 bytes
    *       . Each array 6*4 bytes  = 6*4*3     = 72 bytes
    *   ==> 66 bytes (3)
    *
    *   ==> Total: (1) + (2) + (3) = 246 bytes  (4)
    *   Two driver setting (not in EM_CALIBRATION)
    *   DSAD Gain Setting: 
    *       . DSAD gain array 6 value: 1byte*6*2register = (1*6*2)  = 12 bytes
    *       . Phase shift timer value:                              = 2 bytes
    *   ==> Total: 14 bytes (5)
    * Total everything: (4) + (5) = 246 + 14 = 260 bytes
*/
#if CONFIG_STORAGE_CALIB_GROUP
	#define CONFIG_STORAGE_CALIB_ADDR                       (CONFIG_STORAGE_START_ADDR + 0x00A2)                                    /* Offset 0x0060 */
	#define CONFIG_STORAGE_CALIB_NO_ARRAY_SIZE              (sizeof(EM_CALIBRATION) - 12)
	#define CONFIG_STORAGE_CALIB_SIZE                       (CONFIG_STORAGE_CALIB_NO_ARRAY_SIZE   +\
	                                                         (sizeof(float32_t) * 6 * EM_GAIN_PHASE_NUM_LEVEL_MAX)    +\
    	                                                     (2 * EM_GAIN_PHASE_NUM_LEVEL_MAX)                           +\
        	                                                 (2)                            \
            	                                            )
	#define CONFIG_STORAGE_CALIB_TYPE                       CONFIG_STORAGE_TYPE_STRUCT                                              /* Structure */

    /* Sampling frequency */
    #define CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_ADDR                    (CONFIG_STORAGE_CALIB_ADDR                    + 0)                  /* First Element */
    #define CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_SIZE                    (sizeof(float32_t))
    #define CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_TYPE                    CONFIG_STORAGE_TYPE_FLOAT32
                    
    /* Meter constant phase */              
    #define CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_ADDR                  (CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_ADDR + CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_SIZE)
    #define CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_SIZE                  (sizeof(uint16_t))
    #define CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_TYPE                  CONFIG_STORAGE_TYPE_INTEGER16
    
    /* Meter constant total */              
    #define CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_ADDR                  (CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_ADDR + CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_SIZE)
    #define CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_SIZE                  (sizeof(uint16_t))
    #define CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_TYPE                  CONFIG_STORAGE_TYPE_INTEGER16
                
    /* Pulse on time */             
    #define CONFIG_STORAGE_CALIB_PULSE_ON_TIME_ADDR                         (CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_ADDR     + CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_SIZE)
    #define CONFIG_STORAGE_CALIB_PULSE_ON_TIME_SIZE                         (sizeof(uint16_t))
    #define CONFIG_STORAGE_CALIB_PULSE_ON_TIME_TYPE                         CONFIG_STORAGE_TYPE_INTEGER16
                    
    /* RTC compensation default offset */               
    #define CONFIG_STORAGE_CALIB_RTC_COMP_OFFSET_ADDR                       (CONFIG_STORAGE_CALIB_PULSE_ON_TIME_ADDR     + CONFIG_STORAGE_CALIB_PULSE_ON_TIME_SIZE)
    #define CONFIG_STORAGE_CALIB_RTC_COMP_OFFSET_SIZE                       (sizeof(float32_t))
    #define CONFIG_STORAGE_CALIB_RTC_COMP_OFFSET_TYPE                       CONFIG_STORAGE_TYPE_FLOAT32
                
    /* Coefficient */
    /* Phase_R */
        /* VRMS Coefficient */
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_ADDR                (CONFIG_STORAGE_CALIB_RTC_COMP_OFFSET_ADDR      + CONFIG_STORAGE_CALIB_RTC_COMP_OFFSET_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
        /* IRMS Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_ADDR                (CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_ADDR         + CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
        /* Active Power Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_ADDR        (CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_ADDR        + CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_SIZE        (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_TYPE        CONFIG_STORAGE_TYPE_FLOAT32
    
        /* Reactive Power Coefficient */    
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_ADDR      (CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_ADDR + CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_SIZE      (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_TYPE      CONFIG_STORAGE_TYPE_FLOAT32

        /* Apparent Power Coefficient */
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_ADDR      (CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_ADDR + CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_SIZE      (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_TYPE      CONFIG_STORAGE_TYPE_FLOAT32

    /* Phase_Y */
        /* VRMS Coefficient */
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_ADDR                (CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_ADDR      + CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
        /* IRMS Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_ADDR                (CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_ADDR         + CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
        /* Active Power Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_ADDR        (CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_ADDR        + CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_SIZE        (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_TYPE        CONFIG_STORAGE_TYPE_FLOAT32
    
        /* Reactive Power Coefficient */    
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_ADDR      (CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_ADDR + CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_SIZE      (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_TYPE      CONFIG_STORAGE_TYPE_FLOAT32

        /* Apparent Power Coefficient */
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_ADDR      (CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_ADDR + CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_SIZE      (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_TYPE      CONFIG_STORAGE_TYPE_FLOAT32

    /* Phase_B */
        /* VRMS Coefficient */
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_ADDR                (CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_ADDR      + CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
        /* IRMS Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_ADDR                (CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_ADDR         + CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
        /* Active Power Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_ADDR        (CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_ADDR        + CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_SIZE        (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_TYPE        CONFIG_STORAGE_TYPE_FLOAT32
    
        /* Reactive Power Coefficient */    
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_ADDR      (CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_ADDR + CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_SIZE      (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_TYPE      CONFIG_STORAGE_TYPE_FLOAT32

        /* Apparent Power Coefficient */
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_ADDR      (CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_ADDR + CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_SIZE      (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_TYPE      CONFIG_STORAGE_TYPE_FLOAT32
    
    /* IRMS Coefficient */  
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_ADDR                (CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_ADDR         + CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_SIZE                (sizeof(float32_t))
        #define CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_TYPE                CONFIG_STORAGE_TYPE_FLOAT32
    
    /* Signal Offset */
    /* Phase_R Voltage offset */
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_ADDR           (CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_ADDR         + CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_SIZE)
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_SIZE           (sizeof(int32_t))
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_TYPE           CONFIG_STORAGE_TYPE_INTEGER32
    /* Phase_R Current offset */
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_R_ADDR           (CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_ADDR         + CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_R_SIZE           (sizeof(int32_t))
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_R_TYPE           CONFIG_STORAGE_TYPE_INTEGER32
 
    /* Phase_Y Voltage offset */
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_ADDR           (CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_R_ADDR         + CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_SIZE           (sizeof(int32_t))
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_TYPE           CONFIG_STORAGE_TYPE_INTEGER32
   /* Phase_Y Current offset */
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_Y_ADDR           (CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_ADDR         + CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_Y_SIZE           (sizeof(int32_t))
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_Y_TYPE           CONFIG_STORAGE_TYPE_INTEGER32

    /* Phase_B Voltage offset */
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_ADDR           (CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_Y_ADDR         + CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_SIZE           (sizeof(int32_t))
        #define CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_TYPE           CONFIG_STORAGE_TYPE_INTEGER32
    /* Phase_B Current offset */
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_B_ADDR           (CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_ADDR         + CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_B_SIZE           (sizeof(int32_t))
        #define CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_B_TYPE           CONFIG_STORAGE_TYPE_INTEGER32
        
    /* SW Phase Correction */
    /* Phase_R */
        /* SW Phase Correction Degree Array, 6 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_ADDR           (CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_B_ADDR          + CONFIG_STORAGE_CALIB_OFFSET_CURRENT_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_SIZE           (sizeof(float32_t) * EM_ANGLE_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_TYPE           CONFIG_STORAGE_TYPE_ARRAY_FLOAT32
    /* Phase_Y */
        /* SW Phase Correction Degree Array, 6 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_ADDR           (CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_ADDR          + CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_SIZE           (sizeof(float32_t) * EM_ANGLE_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_TYPE           CONFIG_STORAGE_TYPE_ARRAY_FLOAT32
    /* Phase_B */
        /* SW Phase Correction Degree Array, 3 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_ADDR           (CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_ADDR          + CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_SIZE           (sizeof(float32_t) * EM_ANGLE_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_TYPE           CONFIG_STORAGE_TYPE_ARRAY_FLOAT32

    /* SW Gain */
    /* Phase_R */
        /* SW Phase Channel Gain Array, 3 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_ADDR             (CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_ADDR   + CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_SIZE             (sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_TYPE             CONFIG_STORAGE_TYPE_ARRAY_FLOAT32
        
    /* Phase_Y */
        /* SW Phase Channel Gain Array, 3 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_ADDR             (CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_ADDR   + CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_SIZE             (sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_TYPE             CONFIG_STORAGE_TYPE_ARRAY_FLOAT32
        
    /* Phase_B */
        /* SW Phase Channel Gain Array, 3 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_ADDR             (CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_ADDR   + CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_SIZE             (sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_TYPE             CONFIG_STORAGE_TYPE_ARRAY_FLOAT32
        
    /* Neutral */
        /* SW Phase Channel Gain Array, 3 elements
         * 1 element is a float32_t type */
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_NEUTRAL_ADDR             (CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_ADDR   + CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_SIZE)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_NEUTRAL_SIZE             (sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX)
        #define CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_NEUTRAL_TYPE             CONFIG_STORAGE_TYPE_ARRAY_FLOAT32
    
    /* ADC Gain0 Value, 3 elements
     * 1 element is a uint8_t type */
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_ADDR                (CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_NEUTRAL_ADDR     + CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_NEUTRAL_SIZE)
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE                (sizeof(uint8_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX)
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_TYPE                CONFIG_STORAGE_TYPE_ARRAY_INTEGER8

    /* ADC Gain1 Value, 3 elements
     * 1 element is a uint8_t type */
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_ADDR                (CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_ADDR     + CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE)
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_SIZE                (sizeof(uint8_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX)
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_TYPE                CONFIG_STORAGE_TYPE_ARRAY_INTEGER8

    /* ADC Quarter Shift Timer Offset Value, 1 elements */
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_ADDR               (CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_ADDR     + CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_SIZE)
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_SIZE               (sizeof(int16_t))
    #define CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_TYPE               CONFIG_STORAGE_TYPE_INTEGER16
    
    
#endif /* CONFIG_STORAGE_CALIB_GROUP */


/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

#endif /* _CONFIG_STORAGE_FORMAT_H */

