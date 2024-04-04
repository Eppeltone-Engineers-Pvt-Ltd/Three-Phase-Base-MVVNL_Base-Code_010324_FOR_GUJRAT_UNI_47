
/**
* \addtogroup mlsCIEng
* @{
*/

/**
* @file mlsCIEng.cpp
* @brief mlsCIEng
*
* This port is purposely to debug the CI framework in windows os
*
*/
/*----------------------------------------------------------------------------
OS/Platform dependent include files:
below header file is need a strong consideration when porting between platforms
--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
Standard include files:
These should always be included !
--------------------------------------------------------------------------*/
#include <string.h>
#include "mlsInclude.h"
/*---------------------------------------------------------------------------
Project include files:
-----------------------------------------------------------------------------*/
#include "mlsCIFConfig.h"
#include "mlsCIEng.h"
#include "mlsCIComm.h"
#include "mlsUltil.h"
#include "mlsCIHandler.h"

/*---------------------------------------------------------------------------
* Note: MLSCIF_WINDOW_FLATFORM_ENABLE is defined in mlsCIFConfig.h
* MLSCIF_WINDOW_FLATFORM_ENABLE = 1, Windows flatform
* MLSCIF_WINDOW_FLATFORM_ENABLE = 0, RL78 board flatform
*/
#if (MLSCIF_WINDOW_FLATFORM_ENABLE == 1)
/* Include file of window flatform */
#else
    #define GetTickCount() RL78GetTickCount()
#endif
/*---------------------------------------------------------------------------
Local Types and defines:
--------------------------------------------------------------------------*/
#define INACTIVITY_TIMEOUT  60

/*---------------------------------------------------------------------------
Global variables:
--------------------------------------------------------------------------*/
UInt8 mlsFrameReceiveError = False;
mlsRequestState_t gRequestStatus;
mlsCIEngReceiveState_t ReceiveState;
UInt8 gDataBufferSend[DATA_BUFFER_LEN];
UInt8 gDataBufferReceive[DATA_BUFFER_LEN];
/*---------------------------------------------------------------------------
Static Variables:
--------------------------------------------------------------------------*/
mlsTimer_t InactivityTimer, ResponseTimer;
static mlsCIReqRes_t gResponse;
static mlsCICommConf_t * gCommConfig;
static mlsCICommand_t * gCommandTable;
static Byte lastRequestCtrlCode = 0xff;
/*---------------------------------------------------------------------------
Static Function protoypes:
--------------------------------------------------------------------------*/
static mlsErrorCode_t RequestReceive(mlsCIReqRes_t *req, Int32 timeOutMs);
static mlsErrorCode_t ResponseSend(mlsCIReqRes_t *res);
void mlsError   (mlsCIReqRes_t *req, mlsCIReqRes_t *res, 
                mlsCIResponseErrorCode_t CIResponseErrorCode);
void mlsTimerInit(mlsTimer_t *pTimer);
void startTimer(mlsTimer_t *pTimer);
UInt32 stopTimer(mlsTimer_t *pTimer);
UInt32 getTimerValue(mlsTimer_t *pTimer);
/*---------------------------------------------------------------------------
Exported functions:
--------------------------------------------------------------------------*/
extern UInt32 RL78GetTickCount(void);
extern void mlsSystemTimerInit(void);
extern mlsCIReqRes_t gRequest;

mlsErrorCode_t mlsCIEng_Init(const mlsCICommConf_t * config, 
                             const mlsCICommand_t * commandTable)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    // assign initial state to ReceiveState when initCIEng
    ReceiveState = INITIAL;
    gCommConfig = (mlsCICommConf_t*)config;
    gCommandTable = ( mlsCICommand_t *)commandTable;
    gRequest.dataBlock = &gDataBufferReceive[0];
    gResponse.dataBlock = &gDataBufferSend[0];
    
    /* Init System Timer */
#if (MLSCIF_WINDOW_FLATFORM_ENABLE == 0)
    mlsSystemTimerInit();
#endif
    /* Init Serial */
    returnValue =   gCommConfig->fnSerialInit();
    mlsTimerInit(&InactivityTimer);
    return returnValue; 
}

mlsErrorCode_t mlsCIEng_Deinit(void)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;

    return returnValue; 
}

mlsErrorCode_t mlsCIEng_ProcessStart(void)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    Byte buff[266];
    UInt32 InActivityElapse;
    UInt16 i, len, crc;
    UInt8 Finish;
    commandHandlerFn Handler;
    
    /* init response timer */
    mlsTimerInit(&ResponseTimer);
    if(mlsFrameReceiveError == True)
    {
        /* MLS_PARAMETER_ERROR */
        mlsFrameReceiveError = False;
        mlsError(&gRequest, &gResponse, MLS_PARAMETER_ERROR);
        /* serial flush */
        gCommConfig->fnSerialFlush();
        /* send response */
        ResponseSend(&gResponse);
    }
    /* check if there is an available request */
    if(gRequestStatus == HAVE_REQUEST)
    {   
        /* check if receive etx error */
        if(gRequest.etx != ETX)
        {
            mlsError(&gRequest, &gResponse, MLS_PARAMETER_ERROR);
        }
        else
        {
            /* Processing if get a request */
            InActivityElapse = stopTimer(&InactivityTimer); /* get Inactivity time between sending response and get new request */
            /* Calculate crc processing */
            len = (UInt16)(gRequest.dataLen & 0x00ff);  /* assign len to dataLen */
            buff[0] = gRequest.ctrlCode;            /* crc calculation from ctrlCode to dataBlock */                            
            buff[1] = (Byte) gRequest.blockNumber;
            buff[2] = (Byte) (gRequest.blockNumber >> 8);
            buff[3] = (Byte) gRequest.totalBlock;
            buff[4] = (Byte) (gRequest.totalBlock >> 8);
            buff[5] = gRequest.dataLen;
            
            for(i = 0; i < len; i++)
            {
                buff[i + 6] = gRequest.dataBlock[i];
            }
            crc = mlsUltil_CcittCrc16Calc(&buff[0], (UInt16)len + 6);
            
            if(crc != gRequest.bcc)                     /* Compare crc to request BCC */
            {
                mlsError(&gRequest, &gResponse, MLS_BCC_ERROR);     /* Handle BCC error */              
            }
            else                                /* if have not BCC error */
            {
                if(gRequest.ctrlCode != lastRequestCtrlCode)        /* Check current request with last request */
                {
                    InActivityElapse = 0;
                }
                lastRequestCtrlCode = gRequest.ctrlCode;        /* assign lastRequestCtrlCode to current request ctrCode */
                Handler = mlsHandler_SeekHandler(gRequest.ctrlCode);    
                /* check for the Handler, does it was found in CommandTable */
                if(Handler == (commandHandlerFn)Null)
                {
                    mlsError(&gRequest, &gResponse, MLS_INVALID_REQUEST);   /* Invalid request */       
                }
                else
                {
                    /* Using Handler found to handle this request */
                    returnValue = Handler(&gRequest, &gResponse, InActivityElapse);
                    if(returnValue != MLS_SUCCESS)
                    {
                        if(returnValue == MLS_ERROR_INACTIVITY_TIMEOUT)
                        {
                            /* MLS_TIMEOUT */
                            mlsError(&gRequest, &gResponse, MLS_TIMEOUT);           
                        }
                        else if(returnValue == MLS_ERROR_SEQUENCE_ERROR)
                        {
                            /* MLS_SEQUENCE_ERROR */
                            mlsError(&gRequest, &gResponse, MLS_SEQUENCE_ERROR);            
                        }
                        else if(returnValue == MLS_ERROR_PARAMETER_ERROR)
                        {
                            /* MLS_PARAMETER_ERROR */
                            mlsError(&gRequest, &gResponse, MLS_PARAMETER_ERROR);   
                        }
                        else                                            
                        {
                            /* MLS_ERROR_EXECUTE_ERROR */
                            mlsError(&gRequest, &gResponse, MLS_EXECUTE_ERROR);
                        }
                    }
                }
            }
        }
        /* serial flush */
        gCommConfig->fnSerialFlush();
        /* send response */
        ResponseSend(&gResponse);
        /* start InActivity timer */
        startTimer(&InactivityTimer);   
    }       

        
    return returnValue; 
}

mlsErrorCode_t mlsCIEng_ProcessStop(void)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    
    return returnValue; 
}

static mlsErrorCode_t ResponseSend(mlsCIReqRes_t *res)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    Byte response[DATA_BUFFER_LEN];
    UInt16 crc, len, i;
    UInt32 writtenLeng;
    
        memset(response, 0, DATA_BUFFER_LEN);
    
    res->stx = STX;                     // stx
    res->etx = ETX;                     // etx
    /* calculate BCC before send response */
    response[0] = res->stx;
    response[1] = res->ctrlCode;                // ctrlCode 
    response[2] = (Byte) (res->blockNumber & 0xff);     // byte low of blockNumber
    response[3] = (Byte) ((res->blockNumber >> 8) & 0xff);  // byte high of blockNumber
    response[4] = (Byte) (res->totalBlock & 0xff);      // byte low of totalBlock
    response[5] = (Byte) ((res->totalBlock >> 8) & 0xff);   // byte high of totalBlock
    response[6] = res->dataLen;             // byte dataLen

    len = (UInt16) res->dataLen & 0x00ff;
    for(i = 0; i < len; i++)                // dataBlock
    {
        response[i + 7] = res->dataBlock[i];
    }
    crc = mlsUltil_CcittCrc16Calc((Byte *)response + 1, len + 6);   // crc calculate from ctrlCode to dataBlock
    /* Caculate BCC */
    response[len + 7] = (Byte) (crc & 0xff);        // byte low bcc
    response[7 + len + 1] = (Byte) ((crc >> 8) & 0xff); // byte high bcc
    response[7 + len + 2] = res->etx;           // byte etxt

    /* Write response data to Serial Port */
    returnValue = gCommConfig->fnSerialWrite(&response[0], len + 10, &writtenLeng, INACTIVITY_TIMEOUT);
    
    return returnValue; 
}

void mlsError(mlsCIReqRes_t *req, mlsCIReqRes_t *res,
            mlsCIResponseErrorCode_t error)
{
    /* fill response message with format */
    /* STX, ETX, BCC will be filled into responseSend */                                    
    /* CtrlCode with response and error*/
    res->ctrlCode = (Byte)(req->ctrlCode) | (Byte)0xC0;     
    /* blockNumber */
    res->blockNumber = (UInt16) 0x01;           
    /* totalBlock */
    res->totalBlock = (UInt16) 0x01;            
    /* dataLen */
    res->dataLen = (Byte) 0x01;                 
    /* dataBlock content is error code */
    gDataBufferSend[0] = (Byte) error;
    res->dataBlock = &gDataBufferSend[0];                                   
}
/* Building functions for implement struct mlsTimer_t */
void startTimer(mlsTimer_t *pTimer)
{
    pTimer->timerStatus = TIMERSTART;
    pTimer->startTimerValue = GetTickCount();
}
UInt32 stopTimer(mlsTimer_t *pTimer)
{
    if(pTimer->timerStatus == TIMERSTART)
    {
        pTimer->timerStatus = TIMERSTOP;
        pTimer->stopTimerValue = GetTickCount();
        if(pTimer->stopTimerValue > pTimer->startTimerValue)
        {
            return ( pTimer->stopTimerValue - pTimer->startTimerValue );
        }
        else
        {
            return (TIMER_MAX_TICKCOUNT + pTimer->stopTimerValue - pTimer->startTimerValue);
        }
    }
    else if(pTimer->timerStatus == TIMERINIT)
    {
        return 0;
    }
    pTimer->timerStatus = TIMERSTOP;
    return pTimer->stopTimerValue;
}
/* using when timer stopped or not stoppped */
UInt32 getTimerValue(mlsTimer_t *pTimer)
{
    if(pTimer->timerStatus == TIMERSTART)       // When started
    {
        if(GetTickCount() > pTimer->startTimerValue)
        {
            pTimer->timerValue = (GetTickCount() - pTimer->startTimerValue);        
            return (pTimer->timerValue);
        }
        else
        {
            return (TIMER_MAX_TICKCOUNT + GetTickCount() - pTimer->startTimerValue);
        }
    }
    else if(pTimer->timerStatus == TIMERSTOP)   // When stopped
    {
        if(pTimer->stopTimerValue > pTimer->startTimerValue)
        {
            return ( pTimer->stopTimerValue - pTimer->startTimerValue );
        }
        else
        {
            return (TIMER_MAX_TICKCOUNT + pTimer->stopTimerValue - pTimer->startTimerValue);
        }
    }
    else
    {
        return 0;               // When init
    }
}

void mlsTimerInit(mlsTimer_t *pTimer)
{
    pTimer->timerValue = 0;
    pTimer->stopTimerValue = 0;
    pTimer->timerStatus = TIMERINIT;
}

mlsRequestState_t mlsGetRequestStatus(void)
{
    return gRequestStatus;
}

void mlsSetRequestStatus(mlsRequestState_t status)
{
    gRequestStatus = status;
}
