/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : wrp_user_uart.h
* Version      : 1.00
* Description  : Wrapper User IIC
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _WRAPPER_USER_IIC_H
#define _WRAPPER_USER_IIC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define WRP_IIC_OK      0   /* OK */
#define WRP_IIC_ERROR   1   /* Error */

/* I/Fs mapping */
#define WRP_IIC_DRIVER_Init()                           {;}                                             /* IIC Initialization */
#define WRP_IIC_DRIVER_SendStart(addr, buf, size)       R_IICA0_Master_Send(addr, buf, size, 1)         /* IIC Send */
#define WRP_IIC_DRIVER_ReceiveStart(addr, buf, size)    R_IICA0_Master_Receive(addr, buf, size, 1)      /* IIC Recieve */

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* IIC Wrapper Controller */
void    WRP_IIC_Init(void);                                                             /* Init */
uint8_t WRP_IIC_SendStart(uint8_t device_addr, uint8_t *buffer, uint16_t length);       /* Send Start */
uint8_t WRP_IIC_ReceiveStart(uint8_t device_addr, uint8_t *buffer, uint16_t length);        /* Receive Start */

/* 
 * Callback registration
 * Callback, put 2 below interfaces to the serial user callback
 *      Send End Callback   : WRP_IIC_SendEndCallback
 *      Receive End Callback: WRP_IIC_ReceiveEndCallback
 */
void    WRP_IIC_SendEndCallback(void);
void    WRP_IIC_ReceiveEndCallback(void);

#endif /* _WRAPPER_USER_IIC_H */
