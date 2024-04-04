/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : eeprom.h
* Version      : 1.00
* Description  : EEPROM Application Layer APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EEPROM_H
#define _EEPROM_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */
#include "board.h"
/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
/*
 * Device Information
 * Re-map these below information when change EEPROM device or I2C bus
 */
#define EPR_DEVICE_BUS_ADDRESS              0xA0            /* EEPROM device bus address */
#define EPR_BUS_SPEED                       1000000         /* Max I2C Bus speed (Hz) */
//#define EPR_BUS_SPEED                       100000         /* Max I2C Bus speed (Hz) */

#define EPR_DEVICE_START_ADDR               0x0000          /* EEPROM device start address */
#define EPR_DEVICE_SIZE                     (int32_t)0x30000         /* EEPROM device size (total bytes) */
//#define EPR_DEVICE_SIZE                     0x8000         /* EEPROM device size (total bytes) */
#define EPR_DEVICE_PAGESIZE                 0x80            /* 128 bytes/page */
//#define EPR_DEVICE_PAGESIZE                 0x40            /* 64 bytes/page */
//#define EPR_DEVICE_WRITE_CYCLE_TIME         (2500)          /* Setting 2.5ms, Spec is 5ms */
//#define EPR_DEVICE_WRITE_CYCLE_TIME         (5000)          /* Setting 5ms, Spec is 5ms */
#define EPR_DEVICE_WRITE_CYCLE_TIME         (10000)          /* Setting 5ms, Spec is 5ms */
// permanently allowing e2rom to be written - Write Protect is made low
//#define EPR_WRITE_PROTECT_DISABLE_STATEMENT {P6.2 = 1U;}    /* Command or interface to disable write protection, IICA board on EVB not have WP */
//#define EPR_WRITE_PROTECT_ENABLE_STATEMENT  {P6.2 = 0U;}    /* Command or interface to enable write protection, IICA board on EVB not have WP */
//#define EPR_WRITE_PROTECT_DISABLE_STATEMENT {P6.2 = 0U;}    /* Command or interface to disable write protection, IICA board on EVB not have WP */
//#define EPR_WRITE_PROTECT_ENABLE_STATEMENT  {P6.2 = 1U;}    /* Command or interface to enable write protection, IICA board on EVB not have WP */

/*
 * Delay Specification for EEPROM
 * If the EEPROM is totally different, please consider to change these
 * below information.
 *
 * Normally, these information can be reused.
 */
#define EPR_1BYTE_TIMEOUT                   (40000.0f / EPR_BUS_SPEED)                                  /* 1 byte timeout (ms) for read/write */
//#define EPR_1BYTE_TIMEOUT                   1                                  /* 1 byte timeout (ms) for read/write */
#define EPR_SEND_ADDR_MAX_TIMEOUT           ((uint32_t)(EPR_1BYTE_TIMEOUT * 3))                         /* 2 byte addr + 1 offset */
#define EPR_READ_MAX_TIMEOUT                ((uint32_t)(EPR_1BYTE_TIMEOUT * (EPR_DEVICE_SIZE+1)))       /* Device size + 1 offset */
#define EPR_WRITE_MAX_TIMEOUT               ((uint32_t)(EPR_1BYTE_TIMEOUT * (EPR_DEVICE_PAGESIZE+1)))   /* Page bytes + 1 offset */

/* EEPROM Return Code */
#define EPR_OK                              0           /* Normal end */
#define EPR_ERROR                           1           /* Error in eeprom */
#define EPR_ERROR_NO_RESPOND                2           /* Device does not respond */
#define EPR_ERROR_SIZE                      3           /* Expected size and address are not suitable */

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* Control */
//void    EPR_Init(void);                                         /* EEPROM Init */
//uint8_t EPR_Read(uint32_t addr, uint8_t* buf, uint16_t size);   /* EEPROM Read */
//uint8_t EPR_Write(uint32_t addr, uint8_t* buf, uint16_t size);  /* EEPROM Write */

/*
 * Callbacks,
 * Please register these below callbacks to driver callback interfaces
 * before using this module.
 */
//void    EPR_SendEndCallback(void);
//void    EPR_ReceiveEndCallback(void);

#endif /* _EEPROM_H */
