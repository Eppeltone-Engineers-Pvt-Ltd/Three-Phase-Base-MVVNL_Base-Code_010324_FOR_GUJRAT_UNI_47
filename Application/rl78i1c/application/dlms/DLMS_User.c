/******************************************************************************
* File Name    : DLMS_User.c
*         : 10.07.2012
******************************************************************************/

#include	"Timer.h"
#include	"r_dlms_obis.h"
#include	"DLMS_User.h"

Unsigned8	RxBufferMain[MAX_RECIEVE_BUFFER_SIZE];
Unsigned8	RxBufferSub[MAX_RECIEVE_BUFFER_SIZE];
Unsigned8	TxBuffer[MAX_TRANSMIT_BUFFER_SIZE];
Unsigned8	ServerBuffer[MAX_SERVER_BUFFER_SIZE];

st_channel_info	channel_info[MAX_CONNMGR_CHANNEL_NUMBER];
const Unsigned8	ServerPhysicalAddress[] = {0x00, 0x01, 0x02, 0x00};
const Unsigned8	DeviceAddress[] = "RES001";			// max 32 character
const Unsigned8	ManufacturerID[] = "REN";			// 3 character  
const Unsigned8	ID[] = "SPEM";						// max 16 character  

/* Config parameter for DLMS library here */
const st_ServerConfig UserServerConfig = 
{
	channel_info,									/* Channel info           */
	InitSerial,										/* SerialInit_FuncPtr     */
	SerialConfig,									/* SerialConfig_FuncPtr   */
	SerialTxBlock,									/* SerialTx_FuncPtr       */
	ServerBuffer,									/* *Server_Buffer         */
	(Unsigned8 *)ServerPhysicalAddress,							/* *ServerPhysicalAddress */
	(Unsigned8 *)DeviceAddress,									/* *DeviceAddress         */
	(Unsigned8 *)ManufacturerID,									/* *ManufacturerID        */
	(Unsigned8 *)ID,												/* *ID                    */
	MAX_SERVER_BUFFER_SIZE,							/* Server_BufferSize      */
	6000,											/* Response_Timeout       */
	6000,											/* Inactivity_Timeout     */
	300,											/* Interframe_Timeout     */
	sizeof(channel_info)/ sizeof(st_channel_info),	/* Channel max count      */
	PHYADD_1BYTE_SUPPORTED,							/* AdressByteMode         */
	IEC_BAUDRATE,									/* IEC_BaudRate           */
	HDLC_BAUDRATE,									/* HDLC_BaudRate          */
	MAX_WINDOWS_SIZE,								/* Windows size           */
	0,												/* Dummy                  */
};

void DLMS_Initialize(void)
{
	TimerRCinit();

	/* Initialize the stack library */
	DLMSInit(UserServerConfig);

	/* Add 1st channel */
	CONNMGR_AddChannel(
		CHANNEL_PRIMARY,				/* Channel ID      */
		TxBuffer,						/* Tx buffer start */
		MAX_TRANSMIT_BUFFER_SIZE,		/* Tx buffer size  */
		RxBufferMain,					/* Rx buffer start */
		MAX_RECIEVE_BUFFER_SIZE			/* Rx buffer size  */
	);

	if (CONNMGR_MaxChannelNumber() >= 2)
	{
		/* Add 2nd channel */
		CONNMGR_AddChannel(
			CHANNEL_SECONDARY,			/* Channel ID      */
			TxBuffer,					/* Tx buffer start */
			MAX_TRANSMIT_BUFFER_SIZE,	/* Tx buffer size  */
			RxBufferSub,				/* Rx buffer start */
			MAX_RECIEVE_BUFFER_SIZE		/* Rx buffer size  */
		);
	}
	/* Set priority channel */
	//CONNMGR_RegisterPriorityChannel(CHANNEL_PRIMARY);
	CONNMGR_RegisterPriorityChannel(CHANNEL_SECONDARY);

	/* Initialize object layer */
	COSEMObjectLayerInit(	g_ServiceBuffer,
							OBIS_SERVICE_DATA_BUFFER_LENGTH,
							R_OBIS_DecodeObject);

	R_OBIS_ObjectLayerInit();
}


void DataLinkConnectionReset(void)
{
	/* Reset the async service */
	R_OBIS_AsyncRestart();			/* Reset on Distributor function */
}

void DLMSIntervalProcessing(void)
{
	/* Update async data */
	R_OBIS_IntervalProcessing();
}

void DLMS_PollingProcessing(void)
{
	/* DLMS process of request and response message */
	DLMSMessageProcess();

	/* DLMS Interval Processing */
	 DLMSIntervalProcessing();
}