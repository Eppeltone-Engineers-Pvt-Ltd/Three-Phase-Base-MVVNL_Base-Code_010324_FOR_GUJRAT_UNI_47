/******************************************************************************
  Copyright (C) 2016 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : lvm.h
* Version      : 1.00
* Description  : Logical Volume Manager APIs
******************************************************************************
* History : 08.06.2016 Version Description
******************************************************************************/

#ifndef _LOGICAL_VOLUME_MANAGER_H
#define _LOGICAL_VOLUME_MANAGER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/* Start user code for include */
#include "dataflash.h"      /* Physical device */

/* End user code */
/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct tag_lvm_flag_t
{
    uint16_t            is_updating             :1;
    uint16_t            is_bank1_in_use         :1;
    uint16_t            is_bank2_in_use         :1;
    uint16_t            reserved                :5; /* (Reserved Bits ) - (NO USE) */
    uint16_t            code                    :8;
} lvm_flag_t;
/******************************************************************************
Macro definitions
******************************************************************************/
/* Physical device specification */
/* Map to memory APIs and definitions */
#define PDEV_INIT()                             DATAFLASH_Init()
#define PDEV_READ(addr, buf, size)              DATAFLASH_Read(addr, buf, size)
#define PDEV_WRITE(addr, buf, size)             DATAFLASH_Write(addr, buf, size)
#define PDEV_WRITEBANK(blk_no, buf)             DATAFLASH_WriteBlock(blk_no, buf)
#define PDEV_FORMAT(blk_no)                     DATAFLASH_Format(blk_no)

#define PDEV_OK                                 DATAFLASH_OK
#define PDEV_ERROR                              DATAFLASH_ERROR

#define PDEV_DEVICE_START_ADDR                  DATAFLASH_DEVICE_START_ADDR
#define PDEV_DEVICE_SIZE                        DATAFLASH_DEVICE_SIZE

#define PDEV_MEMORY_BANK_NUM                    2
#define PDEV_BANK1                              DATAFLASH_BLOCK1
#define PDEV_BANK2                              DATAFLASH_BLOCK2

#define PDEV_DEVICE_BANK_SIZE                   DATAFLASH_DEVICE_BLOCK_SIZE

/* End user code */

/* Do NOT modify below */
#define PDEV_CODE_CHECK                         0x5A

#define PDEV_LVM_FLG_ADDR                       PDEV_DEVICE_START_ADDR
#define PDEV_LVM_FLG_SIZE                       (sizeof(lvm_flag_t))



/* Logical device specification */
/* Start user code for declaration */
#define LDEV_OK                                 DATAFLASH_OK
#define LDEV_ERROR                              DATAFLASH_ERROR
#define LDEV_ERROR_NO_RESPOND                   DATAFLASH_ERROR_NO_RESPOND
#define LDEV_ERROR_SIZE                         DATAFLASH_ERROR_SIZE

#define LDEV_DEVICE_START_ADDR                  0x00000
#define LDEV_DEVICE_SIZE                        (PDEV_DEVICE_BANK_SIZE -  PDEV_LVM_FLG_SIZE)
#define LDEV_DEVICE_PAGESIZE                    146             /* LDEV_DEVICE_SIZE Modulo result 0 */

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
uint8_t LVM_Init(void);                                         /* Logical Volume Manager Init */
uint8_t LVM_Read(uint32_t addr, uint8_t* buf, uint16_t size);   /* Logical Volume Manager Read */
uint8_t LVM_Write(uint32_t addr, uint8_t* buf, uint16_t size);  /* Logical Volume Manager Write */

#endif /* _LOGICAL_VOLUME_MANAGER_H */
