/***********************************************************************************************************************
* File Name    : storage.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Storage Source File
***********************************************************************************************************************/

#ifndef _CONFIG_STORAGE_H
#define _CONFIG_STORAGE_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "config_format.h"     /* Storage Format Header */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct tagConfigStorageCrcData
{
    EM_CALIBRATION  * p_calibration;
    uint8_t         * p_driver_adc_gain0_list;
    uint8_t         * p_driver_adc_gain1_list;
    int16_t         * p_driver_timer_offset;
    
} config_crc_data;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Error Status */
#define CONFIG_OK                  0       /* OK */
#define CONFIG_ERROR               1       /* Error */
#define CONFIG_ERROR_DATA_CORRUPT  2       /* Data Storage in EEPROM is corrupted */

/* Warning Status */
#define CONFIG_NOT_FORMATTED       3       /* Storage is NOT formatted */

/* Bit field selection for Backup/Restore */
#define CONFIG_ITEM_NONE           0x00    /* None selection */
#define CONFIG_ITEM_CONFIG         0x01    /* Select EM Configuration */
#define CONFIG_ITEM_CALIB          0x02    /* Select EM Calibration */

#define CONFIG_ITEM_ALL            (        CONFIG_ITEM_CONFIG     |\
                                            CONFIG_ITEM_CALIB      |\
                                            (0)                     \
                                   )   /* Select all */

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* STORAGE Module Controller */
uint8_t CONFIG_Init(void);                                             /* Storage Initialization */
uint8_t CONFIG_Format(void);                                           /* Storage Format Device */
//void CONFIG_PollingProcessing(void);                                   /* Storage Polling Processing */
//void CONFIG_EnableCRCChecking(void);                                   /* Enable CRC checking */
//void CONFIG_DisableCRCChecking(void);                                  /* Disable CRC checking */
uint16_t CONFIG_CalculateCRC16(config_crc_data * p_crc_data);

/* STORAGE Module Write/Add/Backup/Restore */
/* Only call when the Storage device is formatted,
 * if not, error will occurred */
uint8_t CONFIG_Backup(uint8_t selection);                              /* Storage Backup */
uint8_t CONFIG_Restore(uint8_t selection);                             /* Storage Restore */

/* Callback, need register to driver*/
//void CONFIG_RtcCallback(void);                                         /* RTC Constand Interrupt Callback */

#endif /* _CONFIG_STORAGE_H */

