/***********************************************************************************************************************
* File Name    : wrp_user_uart.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : UART Wrapper Layer Source File
***********************************************************************************************************************/

#ifndef _WRAPPER_USER_UART_H
#define _WRAPPER_USER_UART_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define WRP_UART_OK     0   /* OK */
#define WRP_UART_ERROR  1   /* Error */

/* Interface mapping, for each driver,
 * we must re-map again about the UART_DRIVER interface */
 
/* Initialization */
#define     WRP_UART0_DRIVER_Init()                      {;}
#define     WRP_UART2_DRIVER_Init()                      {;}
#define     WRP_UART3_DRIVER_Init()                      {;}

/* Control */
#ifdef __DEBUG
	#define     WRP_UART0_DRIVER_Start()                     R_UART0_Start()
	#define     WRP_UART0_DRIVER_Stop()                      R_UART0_Stop()

	#define     WRP_UART2_DRIVER_Start()                     R_UART2_Start()
	#define     WRP_UART2_DRIVER_Stop()                      R_UART2_Stop()

	#define     WRP_UART3_DRIVER_Start()                     R_UART3_Start()
	#define     WRP_UART3_DRIVER_Stop()                      R_UART3_Stop()

/* Send/Receive data */
	#define     WRP_UART0_DRIVER_SendData(data, length)      R_UART0_Send(data, length)
	#define     WRP_UART0_DRIVER_ReceiveData(data, length)   R_UART0_Receive(data, length)

	#define     WRP_UART2_DRIVER_SendData(data, length)      R_UART2_Send(data, length)
	#define     WRP_UART2_DRIVER_ReceiveData(data, length)   R_UART2_Receive(data, length)

	#define     WRP_UART3_DRIVER_SendData(data, length)      R_UART3_Send(data, length)
	#define     WRP_UART3_DRIVER_ReceiveData(data, length)   R_UART3_Receive(data, length)

/* Addtional APIs */
	#define		WRP_UART0_DRIVER_CheckOperating()			R_UART0_CheckOperating()
	#define		WRP_UART0_DRIVER_SetBaudRate(new_baud_rate)	R_UART0_SetBaudRate(new_baud_rate)
	#define		WRP_UART0_DRIVER_ConfigHDLCProtocol()		R_UART0_ConfigHDLCProtocol()
	//#define		WRP_UART0_DRIVER_ConfigIECProtocol()		R_UART0_ConfigIECProtocol()

	#define		WRP_UART2_DRIVER_CheckOperating()			R_UART2_CheckOperating()
	#define		WRP_UART2_DRIVER_SetBaudRate(new_baud_rate)	R_UART2_SetBaudRate(new_baud_rate)
	#define		WRP_UART2_DRIVER_ConfigHDLCProtocol()		R_UART2_ConfigHDLCProtocol()
	//#define		WRP_UART2_DRIVER_ConfigIECProtocol()		R_UART2_ConfigIECProtocol()

	#define		WRP_UART3_DRIVER_CheckOperating()			R_UART3_CheckOperating()
	#define		WRP_UART3_DRIVER_SetBaudRate(new_baud_rate)	R_UART3_SetBaudRate(new_baud_rate)
	#define		WRP_UART3_DRIVER_ConfigHDLCProtocol()		R_UART3_ConfigHDLCProtocol()
	//#define		WRP_UART3_DRIVER_ConfigIECProtocol()			R_UART3_ConfigIECProtocol()

#else
	#define     WRP_UART0_DRIVER_Start()                     R_UART0_Start()
	#define     WRP_UART0_DRIVER_Stop()                      R_UART0_Stop()

	#define     WRP_UART2_DRIVER_Start()                     R_UART2_Start()
	#define     WRP_UART2_DRIVER_Stop()                      R_UART2_Stop()

	#define     WRP_UART3_DRIVER_Start()                     R_UART3_Start()
	#define     WRP_UART3_DRIVER_Stop()                      R_UART3_Stop()

/* Send/Receive data */
	#define     WRP_UART0_DRIVER_SendData(data, length)      R_UART0_Send(data, length)
	#define     WRP_UART0_DRIVER_ReceiveData(data, length)   R_UART0_Receive(data, length)

	#define     WRP_UART2_DRIVER_SendData(data, length)      R_UART2_Send(data, length)
	#define     WRP_UART2_DRIVER_ReceiveData(data, length)   R_UART2_Receive(data, length)

	#define     WRP_UART3_DRIVER_SendData(data, length)      R_UART3_Send(data, length)
	#define     WRP_UART3_DRIVER_ReceiveData(data, length)   R_UART3_Receive(data, length)

/* Addtional APIs */
	#define		WRP_UART0_DRIVER_CheckOperating()			R_UART0_CheckOperating()
	#define		WRP_UART0_DRIVER_SetBaudRate(new_baud_rate)	R_UART0_SetBaudRate(new_baud_rate)
	#define		WRP_UART0_DRIVER_ConfigHDLCProtocol()		R_UART0_ConfigHDLCProtocol()
	//#define		WRP_UART0_DRIVER_ConfigIECProtocol()		R_UART0_ConfigIECProtocol()

	#define		WRP_UART2_DRIVER_CheckOperating()			R_UART2_CheckOperating()
	#define		WRP_UART2_DRIVER_SetBaudRate(new_baud_rate)	R_UART2_SetBaudRate(new_baud_rate)
	#define		WRP_UART2_DRIVER_ConfigHDLCProtocol()		R_UART2_ConfigHDLCProtocol()
	//#define		WRP_UART2_DRIVER_ConfigIECProtocol()		R_UART2_ConfigIECProtocol()

	#define		WRP_UART3_DRIVER_CheckOperating()			R_UART3_CheckOperating()
	#define		WRP_UART3_DRIVER_SetBaudRate(new_baud_rate)	R_UART3_SetBaudRate(new_baud_rate)
	#define		WRP_UART3_DRIVER_ConfigHDLCProtocol()		R_UART3_ConfigHDLCProtocol()
	//#define		WRP_UART3_DRIVER_ConfigIECProtocol()			R_UART3_ConfigIECProtocol()

#endif

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* UART Wrapper Controller */
void    WRP_UART0_Init(void);
void    WRP_UART2_Init(void);
void    WRP_UART3_Init(void);

void    WRP_UART0_Start(void);
void    WRP_UART2_Start(void);
void    WRP_UART3_Start(void);

void    WRP_UART0_Stop(void);
void    WRP_UART2_Stop(void);
void    WRP_UART3_Stop(void);

uint8_t WRP_UART0_SendData(uint8_t *data, uint16_t length);
uint8_t WRP_UART2_SendData(uint8_t *data, uint16_t length);
uint8_t WRP_UART3_SendData(uint8_t *data, uint16_t length);

uint8_t WRP_UART0_ReceiveData(uint8_t *data, uint16_t length);
uint8_t WRP_UART2_ReceiveData(uint8_t *data, uint16_t length);
uint8_t WRP_UART3_ReceiveData(uint8_t *data, uint16_t length);

/*
 * Callback Registration
 * Callback, put 2 below interfaces to the serial user callback
 *      Send End Callback   : WRP_UART_SendEndCallback
 *      Receive End Callback: WRP_UART_ReceiveEndCallback
 */
void    WRP_UART0_SendEndCallback(void);
void    WRP_UART2_SendEndCallback(void);
void    WRP_UART3_SendEndCallback(void);

void    WRP_UART0_ReceiveEndCallback(void);
void    WRP_UART2_ReceiveEndCallback(void);
void    WRP_UART3_ReceiveEndCallback(void);

void WRP_UART0_ChangeBaudRate (uint8_t new_baud_rate);
void WRP_UART0_ConfigHDLCProtocol(void); // For HDLC support 
void WRP_UART0_ConfigIECProtocol(void);	// For Mode E support

void WRP_UART2_ChangeBaudRate (uint8_t new_baud_rate);
void WRP_UART2_ConfigHDLCProtocol(void); // For HDLC support 
void WRP_UART2_ConfigIECProtocol(void);	// For Mode E support

void WRP_UART3_ChangeBaudRate (uint8_t new_baud_rate);
void WRP_UART3_ConfigHDLCProtocol(void); // For HDLC support 
void WRP_UART3_ConfigIECProtocol(void);	// For Mode E support


#endif /* _WRAPPER_USER_UART_H */
