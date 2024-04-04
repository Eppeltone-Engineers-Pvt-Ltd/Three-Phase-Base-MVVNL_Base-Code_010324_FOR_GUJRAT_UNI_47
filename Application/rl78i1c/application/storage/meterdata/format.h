/***********************************************************************************************************************
* File Name    : format.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Storage Format Header file

* Note         : NEED TO CHANGE THIS FILE WHEN
*              :   . EM CORE TYPE DEFINITION IS CHANGED, OR
*              :   . EEPROM START ADDRESS IS CHANGED
* Caution      : DATA ALIGNMENT
*              :    DATA ALIGNMENT ON THIS FORMAT IS 2-BYTES ALIGNMENT, EVEN ADDRESS.
*              :    PLEASE DON'T CHANGE TO OTHER ALIGNMENT SETTING.
*              :    WHEN CHANGE THE DATA ALIGNMENT SETTING, IT WILL DAMAGE THE FORMAT
*              :    ON EEPROM
*              :
*              : BIT FIELD FORMAT
*              :    BIT FIELD FORMAT IS FROM LSB FIRST
*              :    PLEASE DON'T CHANGE THIS SETTING ON THE COMPILE OPTION (IF SUPPORTED)

***********************************************************************************************************************/

#ifndef _STORAGE_EEPROM_FORMAT_H
#define _STORAGE_EEPROM_FORMAT_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/* Middleware */
#include "em_type.h"        /* EM Core Type Definitions */
#include "em_constraint.h"

/* Application */
#include "eeprom.h"         /* EEPROM MW */

/******************************************************************************
Macro definitions for Typedef
******************************************************************************/
#define SUPPORT_UNIT_CODE_LEN               (20)
#define SUPPORT_FW_VER_LEN                  (11)
#define SUPPORT_METER_TYPE_LEN              (4)
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/* EEPROM Storage Header */
typedef struct tagStorageEEPROMHeader
{
    /* Total: 20 Bytes */
    uint32_t    header_code;                /* (4 bytes) Header Code (EM Formatted Header Code)  */
    uint16_t    usage;                      /* (2 bytes) Usage (number of bytes)                 */
    uint16_t    num_of_sys_state;           /* (2 bytes) Number of EM System state bytes         */
    uint16_t    num_of_rtc_backup;          /* (2 bytes) Number of RTC Backup bytes              */
    uint16_t    num_of_platform_info;       /* (2 bytes) Number of EM Platform Information       */
    uint16_t    num_of_energy_log;          /* (2 bytes) Number of EM Energy Data Log Record     */
    uint16_t    num_of_tamper_log;          /* (2 bytes) Number of EM Tamper Data Log Record     */
    uint16_t    cur_energy_log_index;       /* (2 bytes) Current Energy Log Index                */
    uint16_t    cur_tamper_log_index;       /* (2 bytes) Current Tamper Log Index                */
    
} STORAGE_EEPROM_HEADER;

/* RTC Date Time */
typedef struct tagRTCDateTime
{
    /* Total: 8 bytes */
    uint8_t Sec;        /* Second */
    uint8_t Min;        /* Minute */
    uint8_t Hour;       /* Hour */
    uint8_t Day;        /* Day */
    uint8_t Week;       /* Day of week */
    uint8_t Month;      /* Month */
    uint8_t Year;       /* Year (ony 2 ending digit) */
    
    uint8_t reserved;   /* Reserved Byte (Padding) - NO USE */
    
} RTC_DATE_TIME;

typedef struct tagPlatformUserInfo
{
    /* Total: 50 bytes, take care about byte padding */
    uint8_t             unit_code[SUPPORT_UNIT_CODE_LEN];           /* Unit Code -size 20 */
    uint8_t             fw_version[SUPPORT_FW_VER_LEN+1];           /* Firmware version - size 11+1 */
    uint8_t             type_of_meter[SUPPORT_METER_TYPE_LEN];      /* Type of meter - size 4 */
    RTC_DATE_TIME       manufacture_date;                           /* Manufacture date */
    uint16_t            number_of_reset;                            /* Number of reset */
    uint16_t            lcd_scroll_period;                          /* LCD Scrolling period */
    uint8_t             tamper_indicator;                           /* Tamper Indicator */   
    uint8_t             reserved;                                   /* Reserved Byte (Padding) - NO USE */
    
} PLATFORM_INFO;

/* One Month Energy Data Log */

typedef struct tagOneMonthEnergyDataLogPhase
{
    /* Total: 96 bytes */
    struct
    {   
        uint32_t    active;                                         
        uint32_t    reactive;                                       
        uint32_t    apparent;                                       
    } present_max_demand;                                           

    struct
    {
        float32_t   active;                                         
        float32_t   reactive;                                       
        float32_t   apparent;                                       
    } present_energy_total_tariff;                                  

    struct
    {                   
        float32_t   active;                                         
        float32_t   reactive;                                       
        float32_t   apparent;                                       
    } present_energy_tariff[6];                                     
    
} ONE_MONTH_ENERGY_DATA_LOG_PER_PHASE;

typedef struct tagOneMonthEnergyDataLog
{
//     Total: (96*4 = 384 + 8) = 392 bytes 
//     This format is active, apparent, reactive
//     Difference compare to EMSDK, but this is to support GUI for compatibility with single phase
//     Single phase has no reactive
    ONE_MONTH_ENERGY_DATA_LOG_PER_PHASE phase_r;
    ONE_MONTH_ENERGY_DATA_LOG_PER_PHASE phase_y;
    ONE_MONTH_ENERGY_DATA_LOG_PER_PHASE phase_b;
    
    ONE_MONTH_ENERGY_DATA_LOG_PER_PHASE total;
    
    RTC_DATE_TIME    present_rtc_log;           // Present RTC monthly log 
    
} ONE_MONTH_ENERGY_DATA_LOG;

// One Tamper Data Log
//  Caution:
//    On the CubeSuite 1.5d / Compile Option / Data Control,
//    The bit field format is now start from LSB bit first,
//    When change the "Assign bit field structure from MSB"
//    to "Yes", the bit order of (tamper_type) in below structure
//    is changed to MSB frist, NOT CORRECT for storage format.
//    SO, PLEASE DON'T CHANGE IT TO YES.
//
typedef struct tagOneTamperDataLog
{
    // Total: 10 bytes
    RTC_DATE_TIME   tamper_rtc_time;        // Tamper RTC time 
    
    union
    {
        uint32_t    value;
        struct tagTamperBitDef
        {
            uint8_t     is_neutral_missing          :1;     // Anti-tempering. Neutral Missing      
            uint8_t     is_missing_potential_R      :1;     // Anti-tempering. Missing Potential    
            uint8_t     is_missing_potential_Y      :1;     // Anti-tempering. Missing Potential    
            uint8_t     is_missing_potential_B      :1;     // Anti-tempering. Missing Potential    
            uint8_t     is_voltage_unbalance        :1;     // Anti-tempering. Voltage Unbalance    
            uint8_t     is_reverse_current_R        :1;     // Anti-tempering. Reverse Current      
            uint8_t     is_reverse_current_Y        :1;     // Anti-tempering. Reverse Current      
            uint8_t     is_reverse_current_B        :1;     // Anti-tempering. Reverse Current      
            uint8_t     is_open_current_R           :1;     // Anti-tempering. Open Current         
            uint8_t     is_open_current_Y           :1;     // Anti-tempering. Open Current         
            uint8_t     is_open_current_B           :1;     // Anti-tempering. Open Current         
            uint8_t     is_current_unbalance        :1;     // Anti-tempering. Current Unbalance    
            uint8_t     is_current_bypass           :1;     // Anti-tempering. Current Bypass       
            uint8_t     case_open                   :1;     // Case Open Indicator Bit              
            uint8_t     magnet                      :1;     // Magnet Indicator Bit                 
            uint8_t     power_failed                :1;     // Power Fail Bit                         
            uint8_t     reserved1                   :8;    // Reserve Bit                            
            uint8_t     reserved2                   :8;    // Reserve Bit                            
        } details;                                      // Tamper Type 
    } tamper_type;
    
} ONE_TAMPER_DATA_LOG;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

// EEPROM Information 
#define STORAGE_EEPROM_START_ADDR                       EPR_DEVICE_START_ADDR   // EEPROM Start address 
#define STORAGE_EEPROM_SIZE                             EPR_DEVICE_SIZE         // EEPROM Size 
#define STORAGE_EEPROM_PAGESIZE                         EPR_DEVICE_PAGESIZE     // EEPROM Page Size

/* EEPROM Storage Type */
#define STORAGE_EEPROM_TYPE_INTEGER8                    0       // uint8_t            
#define STORAGE_EEPROM_TYPE_ARRAY_INTEGER8              1       // array of uint8_t   
#define STORAGE_EEPROM_TYPE_INTEGER16                   2       // uint16_t           
#define STORAGE_EEPROM_TYPE_ARRAY_INTEGER16             3       // array of uint16_t  
#define STORAGE_EEPROM_TYPE_INTEGER32                   4       // uint32_t           
#define STORAGE_EEPROM_TYPE_ARRAY_INTEGER32             5       // array of uint32_t  
#define STORAGE_EEPROM_TYPE_FLOAT32                     6       // float32_t          
#define STORAGE_EEPROM_TYPE_ARRAY_FLOAT32               7       // array of float32_t 
#define STORAGE_EEPROM_TYPE_STRUCT                      8       // (struct)          
#define STORAGE_EEPROM_TYPE_ARRAY_STRUCT                9       // (array of struct)  

/* EEPROM Group Enable/Disable Macro */
#define STORAGE_EEPROM_HEADER_GROUP                     1
#define STORAGE_EEPROM_RTC_TIME_BACKUP_GROUP            1	// 1 earlier
#define STORAGE_EEPROM_PLATFORM_INFO_GROUP              1	
#define STORAGE_EEPROM_SYSTEM_STATE_GROUP               1	// 1 earlier
#define STORAGE_EEPROM_ENERGY_DATA_LOG_GROUP            0	// 1 earlier
#define STORAGE_EEPROM_TAMPER_DATA_LOG_GROUP            0	// 1 earlier

/* EEPROM Header Structure Location + Size
 * 11 Macro Items */
#if STORAGE_EEPROM_HEADER_GROUP

	#define STORAGE_EEPROM_HEADER_ADDR                      (STORAGE_EEPROM_START_ADDR + 0x0000)                                    /* Offset 0x0000 */
	#define STORAGE_EEPROM_HEADER_SIZE                      (sizeof(STORAGE_EEPROM_HEADER))                                         /* 24 Bytes */
	#define STORAGE_EEPROM_HEADER_TYPE                      STORAGE_EEPROM_TYPE_STRUCT                                              /* Structure */
    
    /* Header Code */
    #define STORAGE_EEPROM_HEADER_HEADER_CODE_ADDR          (STORAGE_EEPROM_HEADER_ADDR                   + 0)                  /* First Element */
    #define STORAGE_EEPROM_HEADER_HEADER_CODE_SIZE          (sizeof(uint32_t))
    #define STORAGE_EEPROM_HEADER_HEADER_CODE_TYPE          STORAGE_EEPROM_TYPE_INTEGER32

    /* Usage */
    #define STORAGE_EEPROM_HEADER_USAGE_ADDR                (STORAGE_EEPROM_HEADER_HEADER_CODE_ADDR       + STORAGE_EEPROM_HEADER_HEADER_CODE_SIZE)
    #define STORAGE_EEPROM_HEADER_USAGE_SIZE                (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_USAGE_TYPE                STORAGE_EEPROM_TYPE_INTEGER16

    /* Number of system state bytes */
    #define STORAGE_EEPROM_HEADER_NUM_SYSTEM_STATE_ADDR     (STORAGE_EEPROM_HEADER_NUM_CALIB_ADRR         + STORAGE_EEPROM_HEADER_NUM_CALIB_SIZE)
    #define STORAGE_EEPROM_HEADER_NUM_SYSTEM_STATE_SIZE     (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_NUM_SYSTEM_STATE_TYPE     STORAGE_EEPROM_TYPE_INTEGER16

    /* Number of RTC Backup bytes */
    #define STORAGE_EEPROM_HEADER_NUM_RTC_BACKUP_ADDR       (STORAGE_EEPROM_HEADER_NUM_SYSTEM_STATE_ADDR  + STORAGE_EEPROM_HEADER_NUM_SYSTEM_STATE_SIZE)
    #define STORAGE_EEPROM_HEADER_NUM_RTC_BACKUP_SIZE       (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_NUM_RTC_BACKUP_TYPE       STORAGE_EEPROM_TYPE_INTEGER16
    
    /* Number of plaform information bytes */
    #define STORAGE_EEPROM_HEADER_NUM_PLATFORM_INFO_ADDR    (STORAGE_EEPROM_HEADER_NUM_RTC_BACKUP_ADDR    + STORAGE_EEPROM_HEADER_NUM_RTC_BACKUP_SIZE)
    #define STORAGE_EEPROM_HEADER_NUM_PLATFORM_INFO_SIZE    (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_NUM_PLATFORM_INFO_TYPE    STORAGE_EEPROM_TYPE_INTEGER16

    /* Number of energy log record */
    #define STORAGE_EEPROM_HEADER_NUM_ENERGY_LOG_ADDR       (STORAGE_EEPROM_HEADER_NUM_PLATFORM_INFO_ADDR + STORAGE_EEPROM_HEADER_NUM_PLATFORM_INFO_SIZE)
    #define STORAGE_EEPROM_HEADER_NUM_ENERGY_LOG_SIZE       (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_NUM_ENERGY_LOG_TYPE       STORAGE_EEPROM_TYPE_INTEGER16

    /* Number of tamper log record */
    #define STORAGE_EEPROM_HEADER_NUM_TAMPER_LOG_ADDR       (STORAGE_EEPROM_HEADER_NUM_ENERGY_LOG_ADDR    + STORAGE_EEPROM_HEADER_NUM_ENERGY_LOG_SIZE)
    #define STORAGE_EEPROM_HEADER_NUM_TAMPER_LOG_SIZE       (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_NUM_TAMPER_LOG_TYPE       STORAGE_EEPROM_TYPE_INTEGER16

    /* Current energy log index */
    #define STORAGE_EEPROM_HEADER_ENERGY_LOG_INDEX_ADDR     (STORAGE_EEPROM_HEADER_NUM_TAMPER_LOG_ADDR    + STORAGE_EEPROM_HEADER_NUM_TAMPER_LOG_SIZE)
    #define STORAGE_EEPROM_HEADER_ENERGY_LOG_INDEX_SIZE     (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_ENERGY_LOG_INDEX_TYPE     STORAGE_EEPROM_TYPE_INTEGER16

    /* Current tamper log index */
    #define STORAGE_EEPROM_HEADER_TAMPER_LOG_INDEX_ADDR     (STORAGE_EEPROM_HEADER_ENERGY_LOG_INDEX_ADDR  + STORAGE_EEPROM_HEADER_ENERGY_LOG_INDEX_SIZE)
    #define STORAGE_EEPROM_HEADER_TAMPER_LOG_INDEX_SIZE     (sizeof(uint16_t))
    #define STORAGE_EEPROM_HEADER_TAMPER_LOG_INDEX_TYPE     STORAGE_EEPROM_TYPE_INTEGER16
    
#endif /* STORAGE_EEPROM_HEADER_GROUP */

/* EEPROM RTC Time Backup Location + Size 
 * 1 Macro Item */
#if STORAGE_EEPROM_RTC_TIME_BACKUP_GROUP

	#define STORAGE_EEPROM_RTC_TIME_BACKUP_ADDR             (STORAGE_EEPROM_START_ADDR + 0x0200)                                    /* Offset  */
	#define STORAGE_EEPROM_RTC_TIME_BACKUP_SIZE             (sizeof(RTC_DATE_TIME))                                                 /* 8 Bytes */
	#define STORAGE_EEPROM_RTC_TIME_BACKUP_TYPE             STORAGE_EEPROM_TYPE_STRUCT                                              /* Structure */

#endif /* STORAGE_EEPROM_RTC_TIME_BACKUP_GROUP */

/* EEPROM System State Location + Size
 * 1 Macro Item */
#if STORAGE_EEPROM_SYSTEM_STATE_GROUP

	#define STORAGE_EEPROM_SYSTEM_STATE_ADDR                (STORAGE_EEPROM_START_ADDR + 0x0220)                                    /* Offset  */
	#define STORAGE_EEPROM_SYSTEM_STATE_SIZE                613                                                                     /* 385 Bytes */
	#define STORAGE_EEPROM_SYSTEM_STATE_TYPE                STORAGE_EEPROM_TYPE_ARRAY_INTEGER8                                      /* Array of uint8_t */

#endif /* STORAGE_EEPROM_SYSTEM_STATE_GROUP */

/* EEPROM Platform Information
 * 6 Macro Items */
#if STORAGE_EEPROM_PLATFORM_INFO_GROUP

	#define STORAGE_EEPROM_PLATFORM_INFO_ADDR                   (STORAGE_EEPROM_START_ADDR + 0x0600)                                /* Offset 0x0200 */
	#define STORAGE_EEPROM_PLATFORM_INFO_SIZE                   (sizeof(PLATFORM_INFO))                                             /* 24 Bytes */
	#define STORAGE_EEPROM_PLATFORM_INFO_TYPE                   STORAGE_EEPROM_TYPE_STRUCT                                          /* Structure */

    /* Unit code */
    #define STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_ADDR         (STORAGE_EEPROM_PLATFORM_INFO_ADDR                + 0)          /* First Element */
    #define STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_SIZE         (SUPPORT_UNIT_CODE_LEN)
    #define STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_TYPE         STORAGE_EEPROM_TYPE_ARRAY_INTEGER8

    /* Firmware version */
    #define STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_VERSION_ADDR         (STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_ADDR + STORAGE_EEPROM_PLATFORM_INFO_UNIT_CODE_SIZE)
    #define STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_VERSION_SIZE         (SUPPORT_FW_VER_LEN)
    #define STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_VERSION_TYPE         STORAGE_EEPROM_TYPE_ARRAY_INTEGER8
    
    /* Type of meter */
    #define STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_ADDR     (STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_VERSION_ADDR      + STORAGE_EEPROM_PLATFORM_INFO_FIRMWARE_VERSION_SIZE)
    #define STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_SIZE     (SUPPORT_METER_TYPE_LEN)
    #define STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_TYPE     STORAGE_EEPROM_TYPE_ARRAY_INTEGER8

    /* Manufacture Date */
    #define STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_DATE_ADDR  (STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_ADDR  + STORAGE_EEPROM_PLATFORM_INFO_TYPE_OF_METER_SIZE)
    #define STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_DATE_SIZE  (sizeof(RTC_DATE_TIME))
    #define STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_DATE_TYPE  STORAGE_EEPROM_TYPE_STRUCT
    
    /* Number of reset */
    #define STORAGE_EEPROM_PLATFORM_INFO_NUMBER_OF_RESET_ADDR   (STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_DATE_ADDR + STORAGE_EEPROM_PLATFORM_INFO_MANUFACTURE_DATE_SIZE)
    #define STORAGE_EEPROM_PLATFORM_INFO_NUMBER_OF_RESET_SIZE   (sizeof(uint16_t))
    #define STORAGE_EEPROM_PLATFORM_INFO_NUMBER_OF_RESET_TYPE   STORAGE_EEPROM_TYPE_INTEGER16
    
    /* LCD Scroll Period */
    #define STORAGE_EEPROM_PLATFORM_INFO_LCD_SCROLL_PERIOD_ADDR (STORAGE_EEPROM_PLATFORM_INFO_NUMBER_OF_RESET_ADDR  + STORAGE_EEPROM_PLATFORM_INFO_NUMBER_OF_RESET_SIZE)
    #define STORAGE_EEPROM_PLATFORM_INFO_LCD_SCROLL_PERIOD_SIZE (sizeof(uint16_t))
    #define STORAGE_EEPROM_PLATFORM_INFO_LCD_SCROLL_PERIOD_TYPE STORAGE_EEPROM_TYPE_INTEGER16   

    /* Tamper Indicator */
    #define STORAGE_EEPROM_PLATFORM_INFO_TAMPER_INDICATOR_ADDR  (STORAGE_EEPROM_PLATFORM_INFO_LCD_SCROLL_PERIOD_ADDR  + STORAGE_EEPROM_PLATFORM_INFO_LCD_SCROLL_PERIOD_SIZE)
    #define STORAGE_EEPROM_PLATFORM_INFO_TAMPER_INDICATOR_SIZE  (sizeof(uint8_t))
    #define STORAGE_EEPROM_PLATFORM_INFO_TAMPER_INDICATOR_TYPE  STORAGE_EEPROM_TYPE_INTEGER8    
    
#endif /* STORAGE_EEPROM_PLATFORM_INFO_GROUP */

/* EEPROM Monthly Data Log (12 Months)
 * 1 Macro Item 
 * ONE_MONTH_PER_PHASE: 6 * 8 + (3 * 8 * 6) = 192 bytes
 * ONE_MONTH: 192 * 3 + 8 = 584 bytes
 * Total: 584 *12 = 7008 bytes
 */

/* 
#if STORAGE_EEPROM_ENERGY_DATA_LOG_GROUP

	#define STORAGE_EEPROM_ENERGY_DATA_LOG_ADDR         (STORAGE_EEPROM_START_ADDR + 0x0680)                                        // Offset 0x0240 
	#define STORAGE_EEPROM_ENERGY_DATA_LOG_SIZE         (sizeof(ONE_MONTH_ENERGY_DATA_LOG) * 12)                                    // 200*12 = 2400 Bytes 
	#define STORAGE_EEPROM_ENERGY_DATA_LOG_TYPE         STORAGE_EEPROM_TYPE_ARRAY_STRUCT                                            // Array of Structure 

#endif // STORAGE_EEPROM_ENERGY_DATA_LOG_GROUP


// EEPROM Tamper Data Log (200 Events)
// 1 Macro Item 
#if STORAGE_EEPROM_TAMPER_DATA_LOG_GROUP

	#define STORAGE_EEPROM_TAMPER_DATA_LOG_ADDR         (STORAGE_EEPROM_START_ADDR + STORAGE_EEPROM_ENERGY_DATA_LOG_SIZE)           // Offset right after enery log
	#define STORAGE_EEPROM_TAMPER_DATA_LOG_SIZE         (sizeof(ONE_TAMPER_DATA_LOG) * 200)                                         // 2000 Bytes
	#define STORAGE_EEPROM_TAMPER_DATA_LOG_TYPE         STORAGE_EEPROM_TYPE_ARRAY_STRUCT                                            // Array of Structure

#endif // STORAGE_EEPROM_TAMPER_DATA_LOG_GROUP //
*/
/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

#endif /* _STORAGE_EEPROM_FORMAT_H */

