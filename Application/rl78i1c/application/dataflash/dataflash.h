/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : dataflash.h
* Version      : 1.00
* Description  : Data Flash Application Layer APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _DATA_FLASH_H
#define _DATA_FLASH_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/******************************************************************************
Typedef definitions
******************************************************************************/
enum tagDFLBlock
{
    DATAFLASH_BLOCK1,
    DATAFLASH_BLOCK2,
};


/******************************************************************************
Macro definitions
******************************************************************************/
#define DATAFLASH_DEVICE_START_ADDR         (0x0000)    /* Data flash device start address */
#define DATAFLASH_DEVICE_SIZE               (0x0800)    /* 2048 bytes */
#define DATAFLASH_DEVICE_BLOCK_SIZE         (0x0400)    /* 1024 bytes */

/* DATAFLASH Return Code */
#define DATAFLASH_OK                        0           /* Normal end */
#define DATAFLASH_ERROR                     1           /* Error in eeprom */
#define DATAFLASH_ERROR_NO_RESPOND          2           /* Device does not respond */
#define DATAFLASH_ERROR_SIZE                3           /* Error the size of reading data flash */

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* Control */
uint8_t DATAFLASH_Init(void);                                               /* DATAFLASH Init */
uint8_t DATAFLASH_Read(uint32_t addr, uint8_t* buf, uint16_t size);         /* DATAFLASH Read */
uint8_t DATAFLASH_Write(uint32_t addr, uint8_t* buf, uint16_t size);        /* DATAFLASH Write */

/* Special for DataFlash */
uint8_t DATAFLASH_WriteBlock(uint8_t blk_no, uint8_t* buf);                 /* DATAFLASH Write block */
uint8_t DATAFLASH_Format(uint8_t blk_no);                                   /* DATAFLASH Format block */

#endif /* _DATAFLASH_H */
