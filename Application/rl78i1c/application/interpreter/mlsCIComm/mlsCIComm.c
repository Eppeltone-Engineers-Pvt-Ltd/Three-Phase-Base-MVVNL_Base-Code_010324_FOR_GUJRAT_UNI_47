
/**
* \addtogroup mlsCIPort
* @{
*/

/**
* @file mlsCIPortRL78.cpp
* @brief mlsCIPort ported to RL78 board
*
* This port is to be able to run on RL78 board
*
*/

/*----------------------------------------------------------------------------
Standard include files:
These should always be included !
--------------------------------------------------------------------------*/
#include "r_cg_macrodriver.h"
#include "mlsCIComm.h"
#include "mlsInclude.h"
/*---------------------------------------------------------------------------
Project include files:
-----------------------------------------------------------------------------*/
#include "mlsErrors.h"
#include "mlsCIFConfig.h"
#include "mlsTypes.h"
#include "mlsCIEng.h"
#include "mlsCITimer.h"

/* Wrapper/User */
#include "wrp_user_uart.h"

/* Application/System */
#include <string.h>

/*---------------------------------------------------------------------------
Local Types and defines:
--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Global variables:
--------------------------------------------------------------------------*/
mlsCIReqRes_t gRequest;
extern mlsCIEngReceiveState_t ReceiveState;

/*---------------------------------------------------------------------------
Static Variables:
--------------------------------------------------------------------------*/
static UInt8 currentDataLen;
static mlsUartSendState_t UartSendStatus = SEND_IDLE;
static uint8_t UartSendbuff[266];

/*---------------------------------------------------------------------------
Static Function protoypes:
--------------------------------------------------------------------------*/
static MD_STATUS mlsUartSend(uint8_t *txbuf, uint16_t txnum);
mlsUartSendState_t mlsGetUartSendStatus(void);

/*---------------------------------------------------------------------------
Exported functions:
--------------------------------------------------------------------------*/

/******************************************************************************
* Function Name : mlsUartSend
* Interface     : static MD_STATUS mlsUartSend(uint8_t *txbuf, uint16_t txnum)
* Description   : 
* Arguments     : uint8_t * txbuf:
*               : uint16_t txnum :
* Function Calls: 
* Return Value  : static MD_STATUS
******************************************************************************/
static MD_STATUS mlsUartSend(uint8_t *txbuf, uint16_t txnum)
{
    MD_STATUS status = MD_OK;

    while(mlsGetUartSendStatus() == SENDING);   // wait if the last response is sending (not finish)
    /* if the last response sending finished */
    memcpy(UartSendbuff, txbuf, txnum);     // copy to the other buffer for avoid conflicting between the last and current sending
    mlsSetUartSendStatus(SENDING);          // notice that uart sending
    mlsSetRequestStatus(WAITING_REQUEST);       // notice that system is able to receive new request
    
    status = WRP_UART_SendData(UartSendbuff, txnum);    // uart send
    
    return status;
}

/******************************************************************************
* Function Name : mlsGetUartSendStatus
* Interface     : mlsUartSendState_t mlsGetUartSendStatus()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : mlsUartSendState_t
******************************************************************************/
mlsUartSendState_t mlsGetUartSendStatus(void)
{
    return UartSendStatus;
}

/******************************************************************************
* Function Name : mlsSetUartSendStatus
* Interface     : void mlsSetUartSendStatus(mlsUartSendState_t status)
* Description   : 
* Arguments     : mlsUartSendState_t status:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsSetUartSendStatus(mlsUartSendState_t status)
{
    UartSendStatus = status;
}

/******************************************************************************
* Function Name : mlsUartReceiveByte
* Interface     : void mlsUartReceiveByte(Byte rx_data)
* Description   : Interrupt callback to receive a byte from UART
* Arguments     : Byte rx_data: Received byte
* Function Calls: TBD
* Return Value  : None
******************************************************************************/
void mlsUartReceiveByte(Byte rx_data)
{
    if(mlsGetRequestStatus() == WAITING_REQUEST)
    {
        switch(ReceiveState)
        {
        case INITIAL:
            ReceiveState = WAIT_STX;                    // switch to next state
        case WAIT_STX:
            if(rx_data == STX)
            {
                ReceiveState = WAIT_CTRLCODE;               // switch to next state
                gRequest.stx = STX;                 // STX
                mlsStartReceiveTimer();                 // start timer0 to check the time receive
            }
            else
            {
                ReceiveState = INITIAL;
            }
            break;
        case WAIT_CTRLCODE:
            ReceiveState = WAIT_BLOCKNUMBER_L;              // switch to next state
            gRequest.ctrlCode = rx_data;                    // ctrlCode
            break;
        case WAIT_BLOCKNUMBER_L:
            ReceiveState = WAIT_BLOCKNUMBER_H;              // switch to next state
            gRequest.blockNumber = (UInt16) rx_data & 0x00ff;       // byte low of blockNumber
            break;
        case WAIT_BLOCKNUMBER_H:
            ReceiveState = WAIT_TOTALBLOCK_L;               // switch to next state
            gRequest.blockNumber |= ((UInt16) rx_data << 8) & 0xff00;   // byte high of blockNumber
            break;
        case WAIT_TOTALBLOCK_L:
            ReceiveState = WAIT_TOTALBLOCK_H;               // switch to next state
            gRequest.totalBlock = (UInt16) rx_data & 0x00ff;        // byte low of totalBlock
            break;
        case WAIT_TOTALBLOCK_H:
            ReceiveState = WAIT_DATALEN;                    // switch to next state
            gRequest.totalBlock |= ((UInt16) rx_data << 8) & 0xff00;    // byte high of totalBlock
            break;
        case WAIT_DATALEN:
            gRequest.dataLen = rx_data;                 // dataLen
            currentDataLen = (UInt8) gRequest.dataLen & 0xff;
            if(currentDataLen == 0)                     // read firmware version, ignore WAIT_DATABLOCK
            {
                ReceiveState = WAIT_BCC_L;
            }
            else
            {
                ReceiveState = WAIT_DATABLOCK;              // switch to next state
            }
            break;
        case WAIT_DATABLOCK:
            if(currentDataLen > 0)
            {
                ReceiveState = WAIT_DATABLOCK;                              // not switch to next state
                gRequest.dataBlock[((UInt8) gRequest.dataLen & 0xff) - currentDataLen] = rx_data;   // fill dataBlock
                currentDataLen--;
                if(currentDataLen == 0)
                {
                    ReceiveState = WAIT_BCC_L;              // switch to next state
                }
            }
            break;
        case WAIT_BCC_L:
            ReceiveState = WAIT_BCC_H;                      // switch to next state
            gRequest.bcc = (UInt16) rx_data & 0x00ff;           // byte low of bcc
            break;
        case WAIT_BCC_H:
            ReceiveState = WAIT_ETX;                    // switch to next state
            gRequest.bcc |= ((UInt16) rx_data << 8) & 0xff00;       // byte high of bcc
            break;
        case WAIT_ETX:
            ReceiveState = IDLE;                        // switch to next state
            gRequest.etx = rx_data;                     // etx
        case IDLE:
            mlsStopAndResetReceiveTimer();
            ReceiveState = INITIAL;                     // switch to next state
            mlsSetRequestStatus(HAVE_REQUEST);              // inform that having a request available
            break;
        default:                                // never reach here
            break;
        }
    }
}

/******************************************************************************
* Function Name : mlsCICommSerialInit
* Interface     : mlsErrorCode_t mlsCICommSerialInit()
* Description   : CI Engine Serial Port Initialize
* Arguments     : None
* Function Calls: WRP_UART_Init
*               : WRP_UART_Start
* Return Value  : mlsErrorCode_t
******************************************************************************/
mlsErrorCode_t mlsCICommSerialInit(void)
{
//  mlsErrorCode_t returnValue = MLS_SUCCESS;

    /* Specify the serial port initialization here */
    WRP_UART_Init();
    WRP_UART_Start();

    return MLS_SUCCESS;
}

/******************************************************************************
* Function Name : mlsCICommSerialDeinit
* Interface     : mlsErrorCode_t mlsCICommSerialDeinit()
* Description   : CI Engine Serial Port Deinitialize
* Arguments     : None
* Function Calls: WRP_UART_Stop
* Return Value  : mlsErrorCode_t
******************************************************************************/
mlsErrorCode_t mlsCICommSerialDeinit(void)
{
//  mlsErrorCode_t returnValue = MLS_SUCCESS;
        
    WRP_UART_Stop();

    return MLS_SUCCESS;
}

/******************************************************************************
* Function Name : mlsCICommSerialRead
* Interface     : mlsErrorCode_t mlsCICommSerialRead(Void* dataBuff,
*               :     UInt32 lenToRead,
*               :     UInt32* lenReaded,
*               :     Int32 timeOutMs
*               : );
* Description   : 
* Arguments     : Void* dataBuff   :
*               : UInt32 lenToRead :
*               : UInt32* lenReaded:
*               : Int32 timeOutMs  :
* Function Calls: 
* Return Value  : mlsErrorCode_t
******************************************************************************/
mlsErrorCode_t mlsCICommSerialRead(void* dataBuff, UInt32 lenToRead, 
                UInt32* lenReaded, Int32 timeOutMs)
{
//  mlsErrorCode_t returnValue = MLS_SUCCESS;

    return MLS_SUCCESS;
}

/******************************************************************************
* Function Name : mlsCICommSerialWrite
* Interface     : mlsErrorCode_t mlsCICommSerialWrite(
*               :     Const Void* dataBuff,
*               :     UInt32 lenToWrite,
*               :     UInt32* lenWritten,
*               :     Int32 timeOutMs
*               : );
* Description   : 
* Arguments     : Const Void* dataBuff:
*               : UInt32 lenToWrite   :
*               : UInt32* lenWritten  :
*               : Int32 timeOutMs     :
* Function Calls: 
* Return Value  : mlsErrorCode_t
******************************************************************************/
mlsErrorCode_t mlsCICommSerialWrite(const void* dataBuff,
                UInt32 lenToWrite, UInt32* lenWritten, Int32 timeOutMs)
{
//  mlsErrorCode_t returnValue = MLS_SUCCESS;
    
    if(mlsUartSend((uint8_t *)dataBuff, (UInt16)lenToWrite))
    {
        return MLS_ERROR_SERIALTIMEOUT;
    }
    
    return MLS_SUCCESS;
}

/******************************************************************************
* Function Name : mlsCIPortSleep
* Interface     : void mlsCIPortSleep(Int32 ms)
* Description   : 
* Arguments     : Int32 ms:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsCIPortSleep(Int32 ms)
{
    //mlsErrorCode_t returnValue = MLS_SUCCESS;
    //todo 
}

/******************************************************************************
* Function Name : mlsCICommSerialFlush
* Interface     : mlsErrorCode_t mlsCICommSerialFlush()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : mlsErrorCode_t
******************************************************************************/
mlsErrorCode_t mlsCICommSerialFlush(void)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
        
    return returnValue;
}

/******************************************************************************
* Function Name : RL78GetTickCount
* Interface     : UInt32 RL78GetTickCount()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : UInt32
******************************************************************************/
UInt32 RL78GetTickCount(void)
{
    return mlsGetSystemTimeCount();
}
