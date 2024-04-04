/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.  */
/******************************************************************************
* File Name    : wrp_user_uart.c
* Version      : 1.00
* Device(s)    : RL78/G13
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/G13 Energy Meter Platform
* Description  : UART Wrapper Layer Source File
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "iodefine.h"
/* Driver */
#include "r_cg_macrodriver.h"		/* Macro Driver Definitions */
#include "r_cg_serial.h"			/* UART Driver Serial definitions */

/* Wrapper/User */
#include "wrp_user_uart.h"			/* UART Wrapper header */

/* Application */
//#include "cmd.h"					/* CMD Module for debug */
#include "serial.h"					/* DLMS Serial Port */

#include "DLMS_User.h"
/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Macro Definitions Checking
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t g_received_byte;
static uint8_t g_received_byte_1st;
static uint8_t g_received_byte_2nd;

/******************************************************************************
* Function Name    : void WRP_UART0_Init(void)
* Description      : WRAPPER UART Initialization
* Arguments        : None
* Functions Called : WRP_UART0_DRIVER_Init()
* Return Value     : None
******************************************************************************/
void WRP_UART0_Init(void)
{
	WRP_UART0_DRIVER_Init();
}

/******************************************************************************
* Function Name    : void WRP_UART0_Start(void)
* Description      : WRAPPER UART Start
* Arguments        : None
* Functions Called : WRP_UART0_DRIVER_Start()
* Return Value     : None
******************************************************************************/
void WRP_UART0_Start(void)
{
	WRP_UART0_DRIVER_Start();

	/* Start receive data byte */
	WRP_UART0_ReceiveData(&g_received_byte_1st, 1);
}

/******************************************************************************
* Function Name    : void WRP_UART0_Stop(void)
* Description      : WRAPPER UART Stop
* Arguments        : None
* Functions Called : WRP_UART0_DRIVER_Stop()
* Return Value     : None
******************************************************************************/
void WRP_UART0_Stop(void)
{
	WRP_UART0_DRIVER_Stop();
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART0_SendData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Send Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART0_DRIVER_SendData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART0_SendData(uint8_t *data, uint16_t length)
{
	if (WRP_UART0_DRIVER_SendData(data, length) != MD_OK)
	{
		return WRP_UART_ERROR;	/* Error */
	}
	else
	{
		return WRP_UART_OK;		/* OK */
	}
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART0_ReceiveData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Receive Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART0_DRIVER_ReceiveData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART0_ReceiveData(uint8_t *data, uint16_t length)
{
	if (WRP_UART0_DRIVER_ReceiveData(data, length) != MD_OK)
	{
		return WRP_UART_ERROR;	/* Error */
	}
	else
	{
		return WRP_UART_OK;		/* OK */
	}
}

/******************************************************************************
* Function Name    : void WRP_UART0_SendEndCallback()
* Description      : WRAPPER UART Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART0_SendEndCallback()
{
#ifdef _DLMS
	/* DLMS Transmit End */
	SerialTxEnd(CHANNEL_SECONDARY);
#else
	/* CMD */
	#ifdef __DEBUG
	CMD_SendEndCallback();
	#endif
#endif
	
}

/******************************************************************************
* Function Name    : void WRP_UART0_ReceiveEndCallback()
* Description      : WRAPPER UART Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART0_ReceiveEndCallback()
{
#ifdef _DLMS
	/* DLMS Transmit End */
	SerialRxEnd(CHANNEL_SECONDARY, g_received_byte_1st);
#else
	/* Debug Command */
	#ifdef __DEBUG
	CMD_ReceiveEndCallback(g_received_byte_1st);
	#endif
#endif
	/* Register to received next byte */
	WRP_UART0_ReceiveData(&g_received_byte_1st, 1);
}

/******************************************************************************
* Function Name    : void WRP_UART1_Init(void)
* Description      : WRAPPER UART Initialization
* Arguments        : None
* Functions Called : WRP_UART1_DRIVER_Init()
* Return Value     : None
******************************************************************************/
void WRP_UART1_Init(void)
{
	WRP_UART1_DRIVER_Init();
}

/******************************************************************************
* Function Name    : void WRP_UART1_Start(void)
* Description      : WRAPPER UART Start
* Arguments        : None
* Functions Called : WRP_UART1_DRIVER_Start()
* Return Value     : None
******************************************************************************/
void WRP_UART1_Start(void)
{
	WRP_UART1_DRIVER_Start();

	/* Start receive data byte */
	WRP_UART1_ReceiveData(&g_received_byte_2nd, 1);
}

/******************************************************************************
* Function Name    : void WRP_UART1_Stop(void)
* Description      : WRAPPER UART Stop
* Arguments        : None
* Functions Called : WRP_UART1_DRIVER_Stop()
* Return Value     : None
******************************************************************************/
void WRP_UART1_Stop(void)
{
	WRP_UART1_DRIVER_Stop();
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART1_SendData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Send Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART1_DRIVER_SendData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART1_SendData(uint8_t *data, uint16_t length)
{
	if (WRP_UART1_DRIVER_SendData(data, length) != MD_OK)
	{
		return WRP_UART_ERROR;	/* Error */
	}
	else
	{
		return WRP_UART_OK;		/* OK */
	}
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART1_ReceiveData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Receive Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART1_DRIVER_ReceiveData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART1_ReceiveData(uint8_t *data, uint16_t length)
{
	if (WRP_UART1_DRIVER_ReceiveData(data, length) != MD_OK)
	{
		return WRP_UART_ERROR;	/* Error */
	}
	else
	{
		return WRP_UART_OK;		/* OK */
	}
}

/******************************************************************************
* Function Name    : void WRP_UART1_SendEndCallback()
* Description      : WRAPPER UART Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART1_SendEndCallback()
{
#ifdef _DLMS
	/* DLMS Transmit End */
	SerialTxEnd(CHANNEL_3RD);
#else
	/* CMD */
	#ifdef __DEBUG
	CMD_SendEndCallback();
	#endif
#endif	
}

/******************************************************************************
* Function Name    : void WRP_UART1_ReceiveEndCallback()
* Description      : WRAPPER UART Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART1_ReceiveEndCallback()
{
#ifdef _DLMS
	/* DLMS Transmit End */
	SerialRxEnd(CHANNEL_3RD, g_received_byte_2nd);
#else
	/* Debug Command */
	#ifdef __DEBUG
	CMD_ReceiveEndCallback(g_received_byte_2nd);
	#endif
#endif

	/* Register to received next byte */
	WRP_UART1_ReceiveData(&g_received_byte_2nd, 1);
}

/******************************************************************************
* Function Name    : void WRP_UART_Init(void)
* Description      : WRAPPER UART Initialization
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Init()
* Return Value     : None
******************************************************************************/
void WRP_UART_Init(void)
{
	WRP_UART_DRIVER_Init();
}

/******************************************************************************
* Function Name    : void WRP_UART_Start(void)
* Description      : WRAPPER UART Start
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Start()
* Return Value     : None
******************************************************************************/
void WRP_UART_Start(void)
{
	WRP_UART_DRIVER_Start();
	
	/* Start receive data byte */
	WRP_UART_ReceiveData(&g_received_byte, 1);
}

/******************************************************************************
* Function Name    : void WRP_UART_Stop(void)
* Description      : WRAPPER UART Stop
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Stop()
* Return Value     : None
******************************************************************************/
void WRP_UART_Stop(void)
{
	WRP_UART_DRIVER_Stop();
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART_SendData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Send Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART_DRIVER_SendData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART_SendData(uint8_t *data, uint16_t length)
{
	if (WRP_UART_DRIVER_SendData(data, length) != MD_OK)
	{
		return WRP_UART_ERROR;	/* Error */
	}
	else
	{
		return WRP_UART_OK;		/* OK */
	}
}

/******************************************************************************
* Function Name    : uint8_t WRP_UART_ReceiveData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Receive Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART_DRIVER_ReceiveData()
* Return Value     : None
******************************************************************************/
uint8_t WRP_UART_ReceiveData(uint8_t *data, uint16_t length)
{
	if (WRP_UART_DRIVER_ReceiveData(data, length) != MD_OK)
	{
		return WRP_UART_ERROR;	/* Error */
	}
	else
	{
		return WRP_UART_OK;		/* OK */
	}
}

/******************************************************************************
* Function Name    : void WRP_UART_SendEndCallback()
* Description      : WRAPPER UART Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART_SendEndCallback()
{
	/* Put application callback here */
#ifdef _DLMS
	/* DLMS Transmit End */
	SerialTxEnd(CHANNEL_PRIMARY);
#else
	#ifdef __DEBUG
		CMD_SendEndCallback();
	#else
		/* IEC1107 */
		EM_IEC1107_SendEndCallback();
	#endif
#endif
}

/******************************************************************************
* Function Name    : void WRP_UART_ReceiveEndCallback()
* Description      : WRAPPER UART Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_UART_ReceiveEndCallback()
{
	/* Put application callback here */
#ifdef _DLMS
	/* DLMS Transmit End */
	SerialRxEnd(CHANNEL_PRIMARY, g_received_byte);
#else
	#ifdef __DEBUG
		CMD_ReceiveEndCallback(g_received_byte);
	#else
		/* IEC1107 */
		EM_IEC1107_ReceiveCallback(g_received_byte);
	#endif
#endif
	
	WRP_UART_ReceiveData(&g_received_byte, 1);
}

/*****************************************************************************************
* function		: void WRP_UART0_ChangeBaudRate (uint8_t new_baud_rate)
* description	: this callback function is used to change the baud rate depend on
                  the value of Z bit in ACK/Condition message
				  <SOH>0ZY<CR><LF>
				  with Z is used to specify the baud rate
* input			: [new_baud_rate] the desirable new baud rate
* output		: none
* return 		: none
*******************************************************************************************/
void WRP_UART0_ChangeBaudRate (uint8_t new_baud_rate)
{
	/*********************************************************************************************
	In RL78/G13 Baud rate in serial communication is calculated based on the following expression:
		                 operation frequency
		Baud Rate = ---------------------------------
                      7_higher_bit_SDR_reg * 2
								  XTAL_frequency                20 x 10^6
		operation frequency = ------------------------ = -------------------------
                                      2 ^ x                    SPS_register
	Because:
	+ 7_higher_bit_SDR_reg is fixed = 1000000B
 	--> baud rate is change by changing the value of SPS_register based on available define
	Note:
	<TBD> check if do it need any delay time or not ?!
	**********************************************************************************************/
	if (WRP_UART0_DRIVER_CheckOperating() == 1)
	{
		WRP_UART0_Stop();
		WRP_UART0_DRIVER_SetBaudRate(new_baud_rate);
		WRP_UART0_Start();
	}
	else
	{
		WRP_UART0_DRIVER_SetBaudRate(new_baud_rate);
	}
}

/*****************************************************************************************
* function		: void WRP_UART0_ConfigHDLCProtocol(void)
* description	: Reconfigure UART to adapt with HDLC protocol
*				  Data bit : 8
*				  Stop bit : 1
*				  Parity   : None
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
void WRP_UART0_ConfigHDLCProtocol(void)
{
	if (WRP_UART0_DRIVER_CheckOperating() == 1)
	{
		WRP_UART0_Stop();
		/* 8bits, even none, 1 stop bit */
		WRP_UART0_DRIVER_ConfigHDLCProtocol();
		WRP_UART0_Start();
	}
	else
	{
		WRP_UART0_DRIVER_ConfigHDLCProtocol();
	}
}

/*****************************************************************************************
* function		: void WRP_UART0_ConfigIECProtocol(void)
* description	: Reconfigure UART to adapt with IEC1107 protocol
*				  Data bit : 7
*				  Stop bit : 1
*				  Parity   : Even
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
void WRP_UART0_ConfigIECProtocol(void)
{
	if (WRP_UART0_DRIVER_CheckOperating() == 1)
	{
		WRP_UART0_Stop();
		/* 7bits, even parity, 1 stop bit */
		WRP_UART0_DRIVER_ConfigIECProtocol();
		WRP_UART0_Start();
	}
	else
	{
		/* 7bits, even parity, 1 stop bit */
		WRP_UART0_DRIVER_ConfigIECProtocol();
	}
}

/*****************************************************************************************
* function		: void WRP_UART1_ChangeBaudRate (uint8_t new_baud_rate)
* description	: this callback function is used to change the baud rate depend on
                  the value of Z bit in ACK/Condition message
				  <SOH>0ZY<CR><LF>
				  with Z is used to specify the baud rate
* input			: [new_baud_rate] the desirable new baud rate
* output		: none
* return 		: none
*******************************************************************************************/
void WRP_UART1_ChangeBaudRate (uint8_t new_baud_rate)
{
	/*********************************************************************************************
	In RL78/G13 Baud rate in serial communication is calculated based on the following expression:
		                 operation frequency
		Baud Rate = ---------------------------------
                      7_higher_bit_SDR_reg * 2
								  XTAL_frequency                20 x 10^6
		operation frequency = ------------------------ = -------------------------
                                      2 ^ x                    SPS_register
	Because:
	+ 7_higher_bit_SDR_reg is fixed = 1000000B
 	--> baud rate is change by changing the value of SPS_register based on available define
	Note:
	<TBD> check if do it need any delay time or not ?!
	**********************************************************************************************/
	if (WRP_UART1_DRIVER_CheckOperating() == 1)
	{
		WRP_UART1_Stop();
		WRP_UART1_DRIVER_SetBaudRate(new_baud_rate);
		WRP_UART1_Start();
	}
	else
	{
		WRP_UART1_DRIVER_SetBaudRate(new_baud_rate);
	}
}

/*****************************************************************************************
* function		: void WRP_UART1_ConfigHDLCProtocol(void)
* description	: Reconfigure UART to adapt with HDLC protocol
*				  Data bit : 8
*				  Stop bit : 1
*				  Parity   : None
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
void WRP_UART1_ConfigHDLCProtocol(void)
{
	if (WRP_UART1_DRIVER_CheckOperating() == 1)
	{
		WRP_UART1_Stop();
		/* 8bits, even none, 1 stop bit */
		WRP_UART1_DRIVER_ConfigHDLCProtocol();
		WRP_UART1_Start();
	}
	else
	{
		WRP_UART1_DRIVER_ConfigHDLCProtocol();
	}
}

/*****************************************************************************************
* function		: void WRP_UART1_ConfigIECProtocol(void)
* description	: Reconfigure UART to adapt with IEC1107 protocol
*				  Data bit : 7
*				  Stop bit : 1
*				  Parity   : Even
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
void WRP_UART1_ConfigIECProtocol(void)
{
	if (WRP_UART1_DRIVER_CheckOperating() == 1)
	{
		WRP_UART1_Stop();
		/* 7bits, even parity, 1 stop bit */
		WRP_UART1_DRIVER_ConfigIECProtocol();
		WRP_UART1_Start();
	}
	else
	{
		/* 7bits, even parity, 1 stop bit */
		WRP_UART1_DRIVER_ConfigIECProtocol();
	}
}

/*****************************************************************************************
* function		: void WRP_UART_ChangeBaudRate (uint8_t new_baud_rate)
* description	: this callback function is used to change the baud rate depend on
                  the value of Z bit in ACK/Condition message
				  <SOH>0ZY<CR><LF>
				  with Z is used to specify the baud rate
* input			: [new_baud_rate] the desirable new baud rate
* output		: none
* return 		: none
*******************************************************************************************/
void WRP_UART_ChangeBaudRate (uint8_t new_baud_rate)
{
	/*********************************************************************************************
	In RL78/G13 Baud rate in serial communication is calculated based on the following expression:
		                 operation frequency
		Baud Rate = ---------------------------------
                      7_higher_bit_SDR_reg * 2
								  XTAL_frequency                20 x 10^6
		operation frequency = ------------------------ = -------------------------
                                      2 ^ x                    SPS_register
	Because:
	+ 7_higher_bit_SDR_reg is fixed = 1000000B
 	--> baud rate is change by changing the value of SPS_register based on available define
	Note:
	<TBD> check if do it need any delay time or not ?!
	**********************************************************************************************/
	if (WRP_UART_DRIVER_CheckOperating() == 1)
	{
		WRP_UART_Stop();
		WRP_UART_DRIVER_SetBaudRate(new_baud_rate);
		WRP_UART_Start();
	}
	else
	{
		WRP_UART_DRIVER_SetBaudRate(new_baud_rate);
	}
}

/*****************************************************************************************
* function		: void WRP_UART_ConfigHDLCProtocol(void)
* description	: Reconfigure UART to adapt with HDLC protocol
*				  Data bit : 8
*				  Stop bit : 1
*				  Parity   : None
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
void WRP_UART_ConfigHDLCProtocol(void)
{
	if (WRP_UART_DRIVER_CheckOperating() == 1)
	{
		WRP_UART_Stop();
		/* 8bits, even none, 1 stop bit */
		WRP_UART_DRIVER_ConfigHDLCProtocol();
		WRP_UART_Start();
	}
	else
	{
		WRP_UART_DRIVER_ConfigHDLCProtocol();
	}
}

/*****************************************************************************************
* function		: void WRP_UART_ConfigIECProtocol(void)
* description	: Reconfigure UART to adapt with IEC1107 protocol
*				  Data bit : 7
*				  Stop bit : 1
*				  Parity   : Even
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
void WRP_UART_ConfigIECProtocol(void)
{
	if (WRP_UART_DRIVER_CheckOperating() == 1)
	{
		WRP_UART_Stop();
		/* 7bits, even parity, 1 stop bit */
		WRP_UART_DRIVER_ConfigIECProtocol();
		WRP_UART_Start();
	}
	else
	{
		/* 7bits, even parity, 1 stop bit */
		WRP_UART_DRIVER_ConfigIECProtocol();
	}
}