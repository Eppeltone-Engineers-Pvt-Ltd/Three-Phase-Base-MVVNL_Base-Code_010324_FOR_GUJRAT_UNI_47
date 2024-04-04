/*******************************************************************************	
* File Name    : 
* Version      : 1.00
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12/02/2009 1.00    First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include	"r_cg_macrodriver.h"
#include	"r_cg_port.h"
#include	"wrp_user_uart.h"

#include	"serial.h"

#include	<string.h>

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
#ifdef __DEBUG_DLMS
Unsigned8 g_receive_data[256];
Unsigned8 g_index = 0;
#endif

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name: InitSerial
* Description  : Initialization of UART unit to enable 
*				 serial receive/transmit operations
* Arguments    : None
* Return Value : None
******************************************************************************/
void InitSerial(Unsigned8 channel)
{
	switch (channel)
	{
		case CHANNEL_PRIMARY:
			// Start UART - Opto Isolated Port
			WRP_UART3_Init();
			WRP_UART3_Start();
			break;
		case CHANNEL_SECONDARY:
			// Start UART - Optical Port
			WRP_UART2_Init();
			WRP_UART2_Start();
			break;
		case CHANNEL_3RD:
			// Start UART - Third Port (probably for modem at present)
			WRP_UART0_Init();
			WRP_UART0_Start();
			break;
		default:
			/* Do nothing */
			break;
	}

	#ifdef __DEBUG_DLMS
	memset(g_receive_data, 0, 256);
	#endif
}

/******************************************************************************
* Function Name   : SerialTxEnd
* Interface       : void SerialTxEnd(void)
* Description     : 
* Arguments       : None:
* Function Calls  : 
* Return Value    : None
******************************************************************************/
void SerialTxEnd(Unsigned8 channel)
{
	/* Notify to physical layer */
	PhysicalSendEndCallback(channel);
}

/******************************************************************************
* Function Name   : SerialRxEnd
* Interface       : void SerialRxEnd(void)
* Description     : 
* Arguments       : None:
* Function Calls  : 
* Return Value    : None
******************************************************************************/
void SerialRxEnd(Unsigned8 channel, Unsigned8 byte)
{
	/* Raise callback */
	#ifdef __DEBUG_DLMS
	P0.0 = 1;
	#endif

	/* Put data to physical layer */
	PhysicalReceiveCallback(channel, byte);

	#ifdef __DEBUG_DLMS
	P0.0 = 0;
	g_receive_data[g_index++] = byte;
	#endif
}

/****************************************************************************
* Function Name: SerialTxBlock
* Description  : Transmit block of data through serial communication
* Arguments    : Unsigned8 channel: channel ID 
*              : Unsigned8*  BlockPtr: Pointer to block start address 
*			   : Integer16   Length	 : Length of the data 
* Return Value : none
******************************************************************************/
void SerialTxBlock(Unsigned8 channel, Unsigned8* BlockPtr, Integer16 Length)
{
	/* Start serial transmit */
	switch (channel)
	{
		case CHANNEL_PRIMARY:
			WRP_UART3_SendData(BlockPtr, Length);
			break;
		case CHANNEL_SECONDARY:
			WRP_UART2_SendData(BlockPtr, Length);
			break;
		case CHANNEL_3RD:
			WRP_UART0_SendData(BlockPtr, Length);
			break;
		default:
			/* Do nothing */
			break;
	}
}

/*****************************************************************************************
* function		: SerialConfig(Unsigned8 channel, Unsigned8 new_baud_rate, Unsigned8 new_protocol)
* description	: Reconfigure UART to adapt with new baud_rate,new protocol
* input			: Unsigned8 channel
* output		: none
* return 		: none		  
*****************************************************************************************/
void SerialConfig(Unsigned8 channel, Unsigned8 new_baud_rate, Unsigned8 new_protocol)
{
	/* Set Baud rate of UART channel */
	if(new_baud_rate != BAUD_RATE_NOT_SPECIFIED)
	{
			switch (channel)
		{
			case CHANNEL_PRIMARY:
				WRP_UART3_ChangeBaudRate(new_baud_rate);
				break;
			case CHANNEL_SECONDARY:
				WRP_UART2_ChangeBaudRate(new_baud_rate);
				break;
			case CHANNEL_3RD:
				WRP_UART0_ChangeBaudRate(new_baud_rate);
				break;
			default:
				/* Do nothing */
				break;
		}
	}

	/* Reconfigure UART to adapt with new protocol */
	if(new_protocol == IEC_PROTOCOL)
	{
		switch (channel)
		{
			case CHANNEL_PRIMARY:
//				WRP_UART3_ConfigIECProtocol();
				break;
			case CHANNEL_SECONDARY:
//				WRP_UART2_ConfigIECProtocol();
				break;
			case CHANNEL_3RD:
//				WRP_UART0_ConfigIECProtocol();
				break;
			default:
				/* Do nothing */
				break;
		}
	}
	else if(new_protocol == HDLC_PROTOCOL)
	{
		switch (channel)
		{
			case CHANNEL_PRIMARY:
				WRP_UART3_ConfigHDLCProtocol();
				break;
			case CHANNEL_SECONDARY:
				WRP_UART2_ConfigHDLCProtocol();
				break;
			case CHANNEL_3RD:
				WRP_UART0_ConfigHDLCProtocol();
				break;
			default:
				/* Do nothing */
				break;
		}
	}
	else
	{
		/* Do nothing */
	}
}
