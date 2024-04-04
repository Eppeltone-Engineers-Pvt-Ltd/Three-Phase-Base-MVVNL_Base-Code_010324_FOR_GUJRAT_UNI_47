/***********************************************************************************************************************
* File Name    : wrp_user_uart.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : UART Wrapper Layer Source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"       /* Macro Driver Definitions */
#include "r_cg_sau.h"               /* UART Driver Serial definitions */

/* Wrapper/User */
#include "wrp_user_uart.h"          /* UART Wrapper header */

/* Application */
#include "cmd.h"                    /* CMD Module (of Debug) */
#include "mlsCIComm.h"              /* IEC1107 Module */
#include "pravakComm.h"				// PRAVAK Standard communication

#include "serial.h"					// for DLMS Serial Port 
#include "DLMS_User.h"					// for DLMS

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro Definitions Checking
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/
extern volatile uint8_t tx_in_progress;

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
uint8_t g_received_byte0;	// for UART0
uint8_t g_received_byte2;	// for UART2
uint8_t g_received_byte3;	// for UART3

/***********************************************************************************************************************
* Function Name    : void WRP_UART_Init(void)
* Description      : WRAPPER UART Initialization
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Init()
* Return Value     : None
***********************************************************************************************************************/
void WRP_UART0_Init(void)
{// This function seems to be doing nothing.
    WRP_UART0_DRIVER_Init();	// This function has no lines
}

void WRP_UART2_Init(void)
{// This function seems to be doing nothing.
    WRP_UART2_DRIVER_Init();	// This function has no lines
}

void WRP_UART3_Init(void)
{// This function seems to be doing nothing.
    WRP_UART3_DRIVER_Init();	// This function has no lines
}

/***********************************************************************************************************************
* Function Name    : void WRP_UART_Start(void)
* Description      : WRAPPER UART Start
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Start()
* Return Value     : None
***********************************************************************************************************************/
void WRP_UART0_Start(void)
{// Essentially this wrapper allows only one uart to be used at a time

    WRP_UART0_DRIVER_Start();	// This has been defined as R_UART2_Start or R_UART0_Start in wrp_user_uart.h
    
    /* Start receive data byte */
    WRP_UART0_ReceiveData(&g_received_byte0, 1);
}

void WRP_UART2_Start(void)
{// Essentially this wrapper allows only one uart to be used at a time

    WRP_UART2_DRIVER_Start();	// This has been defined as R_UART2_Start or R_UART0_Start in wrp_user_uart.h
    
    /* Start receive data byte */
    WRP_UART2_ReceiveData(&g_received_byte2, 1);
}

void WRP_UART3_Start(void)
{// Essentially this wrapper allows only one uart to be used at a time

    WRP_UART3_DRIVER_Start();	// This has been defined as R_UART2_Start or R_UART0_Start in wrp_user_uart.h
    
    /* Start receive data byte */
    WRP_UART3_ReceiveData(&g_received_byte3, 1);
}


/***********************************************************************************************************************
* Function Name    : void WRP_UART_Stop(void)
* Description      : WRAPPER UART Stop
* Arguments        : None
* Functions Called : WRP_UART_DRIVER_Stop()
* Return Value     : None
***********************************************************************************************************************/
void WRP_UART0_Stop(void)
{
    WRP_UART0_DRIVER_Stop();
}

void WRP_UART2_Stop(void)
{
    WRP_UART2_DRIVER_Stop();
}

void WRP_UART3_Stop(void)
{
    WRP_UART3_DRIVER_Stop();
}

/***********************************************************************************************************************
* Function Name    : uint8_t WRP_UART_SendData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Send Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART_DRIVER_SendData()
* Return Value     : None
***********************************************************************************************************************/
uint8_t WRP_UART0_SendData(uint8_t *data, uint16_t length)
{
    if (WRP_UART0_DRIVER_SendData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

uint8_t WRP_UART2_SendData(uint8_t *data, uint16_t length)
{
    if (WRP_UART2_DRIVER_SendData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

uint8_t WRP_UART3_SendData(uint8_t *data, uint16_t length)
{
    if (WRP_UART3_DRIVER_SendData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

/***********************************************************************************************************************
* Function Name    : uint8_t WRP_UART_ReceiveData(uint8_t *data, uint16_t length)
* Description      : WRAPPER UART Receive Start
* Arguments        : uint8_t *data: Data buffer
*                  : uint16_t length: Length of buffer
* Functions Called : WRP_UART_DRIVER_ReceiveData()
* Return Value     : None
***********************************************************************************************************************/
uint8_t WRP_UART0_ReceiveData(uint8_t *data, uint16_t length)
{
    if (WRP_UART0_DRIVER_ReceiveData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

uint8_t WRP_UART2_ReceiveData(uint8_t *data, uint16_t length)
{
    if (WRP_UART2_DRIVER_ReceiveData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

uint8_t WRP_UART3_ReceiveData(uint8_t *data, uint16_t length)
{
    if (WRP_UART3_DRIVER_ReceiveData(data, length) != MD_OK)
    {
        return WRP_UART_ERROR;  /* Error */
    }
    else
    {
        return WRP_UART_OK;     /* OK */
    }
}

/***********************************************************************************************************************
* Function Name    : void WRP_UART_SendEndCallback()
* Description      : WRAPPER UART Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
***********************************************************************************************************************/
void WRP_UART0_SendEndCallback(void)
{
    // Put application callback here
/*	
#ifdef __DEBUG
    CMD_SendEndCallback();
#else
        // R ENESAS
    mlsSetUartSendStatus(SEND_IDLE);
#endif
*/
//	SerialTxEnd(CHANNEL_3RD);	// for DLMS
}

void WRP_UART2_SendEndCallback(void)
{
    // Put application callback here
#ifdef OPTICAL_PORT
	PVK_TX_Complete();
#endif

#ifdef USE_DLMS
	SerialTxEnd(CHANNEL_SECONDARY);	// for DLMS
	if(tx_in_progress!=0)
		PVK_TX_Complete();
#endif
}

void WRP_UART3_SendEndCallback(void)
{
    // Put application callback here
#ifdef USE_DLMS	
	SerialTxEnd(CHANNEL_PRIMARY);	// for DLMS	
	if(tx_in_progress!=0)
		PVK_TX_Complete();
#else
	PVK_TX_Complete();
#endif
}



/***********************************************************************************************************************
* Function Name    : void WRP_UART_ReceiveEndCallback()
* Description      : WRAPPER UART Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
***********************************************************************************************************************/
void WRP_UART0_ReceiveEndCallback(void)
{// This function is called whenever one byte has been received.
// The buffer is only one byte long - hence full chance of a byte being lost if baud rates are particularly high
// This is because ADC interrupts come every 250 microseconds, and take 160 microseconds to be processed.
// At 9600 baud, each byte will be received in 1000 microseconds, which should not be a problem
// But at 115200 baud, each byte will be received in 86.8 microseconds. If a byte is received just when and ADC interrupt comes
// chances of the next byte getting lost are very very high
// We will live with this system, assuming that our baud rates are probably going to be 2400,4800, and maybe never more than 9600
    // Put application callback here 
/*	
#ifdef __DEBUG
    CMD_ReceiveEndCallback(g_received_byte0);
#else
    // R ENESAS 
    mlsUartReceiveByte(g_received_byte0);
#endif
*/  
//	SerialRxEnd(CHANNEL_3RD,g_received_byte0);	// for DLMS
    WRP_UART0_ReceiveData(&g_received_byte0, 1);
}

void WRP_UART2_ReceiveEndCallback(void)
{// This function is called whenever one byte has been received.
// The buffer is only one byte long - hence full chance of a byte being lost if baud rates are particularly high
// This is because ADC interrupts come every 250 microseconds, and take 160 microseconds to be processed.
// At 9600 baud, each byte will be received in 1000 microseconds, which should not be a problem
// But at 115200 baud, each byte will be received in 86.8 microseconds. If a byte is received just when and ADC interrupt comes
// chances of the next byte getting lost are very very high
// We will live with this system, assuming that our baud rates are probably going to be 2400,4800, and maybe never more than 9600
    // Put application callback here 
/*	
#ifdef __DEBUG
    CMD_ReceiveEndCallback(g_received_byte2);
#else
    // R ENESAS 
    mlsUartReceiveByte(g_received_byte2);
#endif
*/  
#ifdef OPTICAL_PORT
	PVK_ReceiveByte(g_received_byte2,2);
#endif
#ifdef USE_DLMS
	SerialRxEnd(CHANNEL_SECONDARY,g_received_byte2);	// for DLMS
	PVK_ReceiveByte(g_received_byte2,2);
	if(g_received_byte2==0x0A)
	{
    NOP();
	}
#endif
    WRP_UART2_ReceiveData(&g_received_byte2, 1);
}

void WRP_UART3_ReceiveEndCallback(void)
{// This function is called whenever one byte has been received.
// The buffer is only one byte long - hence full chance of a byte being lost if baud rates are particularly high
// This is because ADC interrupts come every 250 microseconds, and take 160 microseconds to be processed.
// At 9600 baud, each byte will be received in 1000 microseconds, which should not be a problem
// But at 115200 baud, each byte will be received in 86.8 microseconds. If a byte is received just when and ADC interrupt comes
// chances of the next byte getting lost are very very high
// We will live with this system, assuming that our baud rates are probably going to be 2400,4800, and maybe never more than 9600
    // Put application callback here 
/*	
#ifdef __DEBUG
    CMD_ReceiveEndCallback(g_received_byte3);
#else
    // R ENESAS 
    mlsUartReceiveByte(g_received_byte3);
#endif
*/  
/*
#ifndef OPTICAL_PORT
	PVK_ReceiveByte(g_received_byte3);
#endif
*/
#ifdef USE_DLMS	
	SerialRxEnd(CHANNEL_PRIMARY,g_received_byte3);	// for DLMS
	PVK_ReceiveByte(g_received_byte3,3);
	if(g_received_byte3==0x0A)
	{
    NOP();
	}
#else
	PVK_ReceiveByte(g_received_byte3,3);
#endif
	WRP_UART3_ReceiveData(&g_received_byte3, 1);
}


static sau_std_baud_rate_t WRP_UART_GetDriverBaudRate(uint8_t dlms_baud_rate)
{
    switch (dlms_baud_rate)
    {
    	case BAUD_RATE_300:
            return SAU_STD_BAUD_RATE_300;
    	case BAUD_RATE_600:
            return SAU_STD_BAUD_RATE_600;
    	case BAUD_RATE_1200:
            return SAU_STD_BAUD_RATE_1200;
    	case BAUD_RATE_2400:
            return SAU_STD_BAUD_RATE_2400;
    	case BAUD_RATE_4800:
            return SAU_STD_BAUD_RATE_4800;
    	case BAUD_RATE_9600:
            return SAU_STD_BAUD_RATE_9600;
    	case BAUD_RATE_19200:
            return SAU_STD_BAUD_RATE_19200;
        default:
            return SAU_STD_BAUD_RATE_300;
    }
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
//	In RL78/G13 Baud rate in serial communication is calculated based on the following expression:
//		                 operation frequency
//		Baud Rate = ---------------------------------
//                      7_higher_bit_SDR_reg * 2
//								  XTAL_frequency                20 x 10^6
//		operation frequency = ------------------------ = -------------------------
//                                      2 ^ x                    SPS_register
//	Because:
//	+ 7_higher_bit_SDR_reg is fixed = 1000000B
// 	--> baud rate is change by changing the value of SPS_register based on available define
//	Note:
//	<TBD> check if do it need any delay time or not ?!

	if (WRP_UART0_DRIVER_CheckOperating() == 1)
	{
		WRP_UART0_Stop();
		WRP_UART0_DRIVER_SetBaudRate(WRP_UART_GetDriverBaudRate(new_baud_rate));
		WRP_UART0_Start();
	}
	else
	{
		WRP_UART0_DRIVER_SetBaudRate(WRP_UART_GetDriverBaudRate(new_baud_rate));
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
		// 8bits, even none, 1 stop bit 
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
#if 0
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
#endif

/*****************************************************************************************
* function		: void WRP_UART2_ChangeBaudRate (uint8_t new_baud_rate)
* description	: this callback function is used to change the baud rate depend on
                  the value of Z bit in ACK/Condition message
				  <SOH>0ZY<CR><LF>
				  with Z is used to specify the baud rate
* input			: [new_baud_rate] the desirable new baud rate
* output		: none
* return 		: none
*******************************************************************************************/
void WRP_UART2_ChangeBaudRate (uint8_t new_baud_rate)
{
//	In RL78/G13 Baud rate in serial communication is calculated based on the following expression:
//		                 operation frequency
//		Baud Rate = ---------------------------------
//                      7_higher_bit_SDR_reg * 2
//								  XTAL_frequency                20 x 10^6
//		operation frequency = ------------------------ = -------------------------
//                                      2 ^ x                    SPS_register
//	Because:
//	+ 7_higher_bit_SDR_reg is fixed = 1000000B
// 	--> baud rate is change by changing the value of SPS_register based on available define
//	Note:
//	<TBD> check if do it need any delay time or not ?!
	if (WRP_UART2_DRIVER_CheckOperating() == 1)
	{
		WRP_UART2_Stop();
		WRP_UART2_DRIVER_SetBaudRate(WRP_UART_GetDriverBaudRate(new_baud_rate));
		WRP_UART2_Start();
	}
	else
	{
		WRP_UART2_DRIVER_SetBaudRate(WRP_UART_GetDriverBaudRate(new_baud_rate));
	}
}


/*****************************************************************************************
* function		: void WRP_UART2_ConfigHDLCProtocol(void)
* description	: Reconfigure UART to adapt with HDLC protocol
*				  Data bit : 8
*				  Stop bit : 1
*				  Parity   : None
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/

void WRP_UART2_ConfigHDLCProtocol(void)
{
	if (WRP_UART2_DRIVER_CheckOperating() == 1)
	{
		WRP_UART2_Stop();
		// 8bits, even none, 1 stop bit 
		WRP_UART2_DRIVER_ConfigHDLCProtocol();
		WRP_UART2_Start();
	}
	else
	{
		WRP_UART2_DRIVER_ConfigHDLCProtocol();
	}
}


/*****************************************************************************************
* function		: void WRP_UART2_ConfigIECProtocol(void)
* description	: Reconfigure UART to adapt with IEC1107 protocol
*				  Data bit : 7
*				  Stop bit : 1
*				  Parity   : Even
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
#if 0
void WRP_UART2_ConfigIECProtocol(void)
{
	if (WRP_UART2_DRIVER_CheckOperating() == 1)
	{
		WRP_UART2_Stop();
		/* 7bits, even parity, 1 stop bit */
		WRP_UART2_DRIVER_ConfigIECProtocol();
		WRP_UART2_Start();
	}
	else
	{
		/* 7bits, even parity, 1 stop bit */
		WRP_UART2_DRIVER_ConfigIECProtocol();
	}
}
#endif
/*****************************************************************************************
* function		: void WRP_UART3_ChangeBaudRate (uint8_t new_baud_rate)
* description	: this callback function is used to change the baud rate depend on
                  the value of Z bit in ACK/Condition message
				  <SOH>0ZY<CR><LF>
				  with Z is used to specify the baud rate
* input			: [new_baud_rate] the desirable new baud rate
* output		: none
* return 		: none
*******************************************************************************************/

void WRP_UART3_ChangeBaudRate (uint8_t new_baud_rate)
{
//	In RL78/G13 Baud rate in serial communication is calculated based on the following expression:
//		                 operation frequency
//		Baud Rate = ---------------------------------
//                      7_higher_bit_SDR_reg * 2
//								  XTAL_frequency                20 x 10^6
//		operation frequency = ------------------------ = -------------------------
//                                      2 ^ x                    SPS_register
//	Because:
//	+ 7_higher_bit_SDR_reg is fixed = 1000000B
// 	--> baud rate is change by changing the value of SPS_register based on available define
//	Note:
//	<TBD> check if do it need any delay time or not ?!
	if (WRP_UART3_DRIVER_CheckOperating() == 1)
	{
		WRP_UART3_Stop();
		WRP_UART3_DRIVER_SetBaudRate(WRP_UART_GetDriverBaudRate(new_baud_rate));
		WRP_UART3_Start();
	}
	else
	{
		WRP_UART3_DRIVER_SetBaudRate(WRP_UART_GetDriverBaudRate(new_baud_rate));
	}
}


/*****************************************************************************************
* function		: void WRP_UART3_ConfigHDLCProtocol(void)
* description	: Reconfigure UART to adapt with HDLC protocol
*				  Data bit : 8
*				  Stop bit : 1
*				  Parity   : None
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/

void WRP_UART3_ConfigHDLCProtocol(void)
{
	if (WRP_UART3_DRIVER_CheckOperating() == 1)
	{
		WRP_UART3_Stop();
		// 8bits, even none, 1 stop bit //
		WRP_UART3_DRIVER_ConfigHDLCProtocol();
		WRP_UART3_Start();
	}
	else
	{
		WRP_UART3_DRIVER_ConfigHDLCProtocol();
	}
}

/*****************************************************************************************
* function		: void WRP_UART3_ConfigIECProtocol(void)
* description	: Reconfigure UART to adapt with IEC1107 protocol
*				  Data bit : 7
*				  Stop bit : 1
*				  Parity   : Even
* input			: none
* output		: none
* return 		: none
*****************************************************************************************/
#if 0
void WRP_UART3_ConfigIECProtocol(void)
{
	if (WRP_UART3_DRIVER_CheckOperating() == 1)
	{
		WRP_UART3_Stop();
		/* 7bits, even parity, 1 stop bit */
		WRP_UART3_DRIVER_ConfigIECProtocol();
		WRP_UART3_Start();
	}
	else
	{
		/* 7bits, even parity, 1 stop bit */
		WRP_UART3_DRIVER_ConfigIECProtocol();
	}
}
#endif





