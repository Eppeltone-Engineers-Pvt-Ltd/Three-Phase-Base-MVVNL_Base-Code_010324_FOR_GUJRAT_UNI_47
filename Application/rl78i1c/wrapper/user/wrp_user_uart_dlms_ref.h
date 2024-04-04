/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : wrp_user_uart.h
* Version      : 1.00
* Description  : Wrapper User UART
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _WRAPPER_USER_UART_H
#define _WRAPPER_USER_UART_H

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
#define WRP_UART_OK		0	/* OK */
#define WRP_UART_ERROR	1	/* Error */

/* Interface mapping, for each driver,
 * we must re-map again about the UART_DRIVER interface */
 
/* Initialization */
#define		WRP_UART0_DRIVER_Init()						R_UART0_Create()
#define		WRP_UART1_DRIVER_Init()						R_UART1_Create()
#define		WRP_UART_DRIVER_Init()						R_UART2_Create()

/* Control */
#define		WRP_UART0_DRIVER_Start()					R_UART0_Start()
#define		WRP_UART1_DRIVER_Start()					R_UART1_Start()
#define		WRP_UART_DRIVER_Start()						R_UART2_Start()

#define		WRP_UART0_DRIVER_Stop()						R_UART0_Stop()
#define		WRP_UART1_DRIVER_Stop()						R_UART1_Stop()
#define		WRP_UART_DRIVER_Stop()						R_UART2_Stop()

/* Send/Receive data */
#define		WRP_UART0_DRIVER_SendData(data, length)		R_UART0_Send(data, length)
#define		WRP_UART1_DRIVER_SendData(data, length)		R_UART1_Send(data, length)
#define		WRP_UART_DRIVER_SendData(data, length)		R_UART2_Send(data, length)

#define		WRP_UART0_DRIVER_ReceiveData(data, length)	R_UART0_Receive(data, length)
#define		WRP_UART1_DRIVER_ReceiveData(data, length)	R_UART1_Receive(data, length)
#define		WRP_UART_DRIVER_ReceiveData(data, length)	R_UART2_Receive(data, length)

/* Addtional APIs */
#define		WRP_UART0_DRIVER_CheckOperating()			R_UART0_CheckOperating()
#define		WRP_UART0_DRIVER_SetBaudRate(new_baud_rate)	R_UART0_SetBaudRate(new_baud_rate)
#define		WRP_UART0_DRIVER_ConfigHDLCProtocol()		R_UART0_ConfigHDLCProtocol()
#define		WRP_UART0_DRIVER_ConfigIECProtocol()		R_UART0_ConfigIECProtocol()

#define		WRP_UART1_DRIVER_CheckOperating()			R_UART1_CheckOperating()
#define		WRP_UART1_DRIVER_SetBaudRate(new_baud_rate)	R_UART1_SetBaudRate(new_baud_rate)
#define		WRP_UART1_DRIVER_ConfigHDLCProtocol()		R_UART1_ConfigHDLCProtocol()
#define		WRP_UART1_DRIVER_ConfigIECProtocol()		R_UART1_ConfigIECProtocol()

#define		WRP_UART_DRIVER_CheckOperating()			R_UART_CheckOperating()
#define		WRP_UART_DRIVER_SetBaudRate(new_baud_rate)	R_UART_SetBaudRate(new_baud_rate)
#define		WRP_UART_DRIVER_ConfigHDLCProtocol()		R_UART_ConfigHDLCProtocol()
#define		WRP_UART_DRIVER_ConfigIECProtocol()			R_UART_ConfigIECProtocol()
/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* UART Wrapper Controller */
void	WRP_UART0_Init(void);
void	WRP_UART0_Start(void);
void	WRP_UART0_Stop(void);
uint8_t	WRP_UART0_SendData(uint8_t *data, uint16_t length);
uint8_t	WRP_UART0_ReceiveData(uint8_t *data, uint16_t length);

void	WRP_UART1_Init(void);
void	WRP_UART1_Start(void);
void	WRP_UART1_Stop(void);
uint8_t	WRP_UART1_SendData(uint8_t *data, uint16_t length);
uint8_t	WRP_UART1_ReceiveData(uint8_t *data, uint16_t length);

void	WRP_UART_Init(void);
void	WRP_UART_Start(void);
void	WRP_UART_Stop(void);
uint8_t	WRP_UART_SendData(uint8_t *data, uint16_t length);
uint8_t	WRP_UART_ReceiveData(uint8_t *data, uint16_t length);

/*
 * Callback Registration
 * Callback, put 2 below interfaces to the serial user callback
 *		Send End Callback	: WRP_UART0_SendEndCallback
 *		Receive End Callback: WRP_UART0_ReceiveEndCallback
 */
void	WRP_UART0_SendEndCallback();
void	WRP_UART0_ReceiveEndCallback();

void	WRP_UART1_SendEndCallback();
void	WRP_UART1_ReceiveEndCallback();

void	WRP_UART_SendEndCallback();
void	WRP_UART_ReceiveEndCallback();

void WRP_UART0_ChangeBaudRate (uint8_t new_baud_rate);
/* For HDLC support */
void WRP_UART0_ConfigHDLCProtocol(void);
/* For Mode E support */
void WRP_UART0_ConfigIECProtocol(void);

void WRP_UART1_ChangeBaudRate (uint8_t new_baud_rate);
/* For HDLC support */
void WRP_UART1_ConfigHDLCProtocol(void);
/* For Mode E support */
void WRP_UART1_ConfigIECProtocol(void);

void WRP_UART_ChangeBaudRate (uint8_t new_baud_rate);
/* For HDLC support */
void WRP_UART_ConfigHDLCProtocol(void);
/* For Mode E support */
void WRP_UART_ConfigIECProtocol(void);

#endif /* _WRAPPER_USER_UART_H */
