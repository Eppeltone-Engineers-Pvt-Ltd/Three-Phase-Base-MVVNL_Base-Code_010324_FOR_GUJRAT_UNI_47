
/**
* \addtogroup mlsCIHandler
* @{
*/

/**
* @file mlsCIHandler.cpp
* @brief mlsCIHandler
*
* This file is to implement Command Handler
*
*/

/*----------------------------------------------------------------------------
Standard include files:
These should always be included !
--------------------------------------------------------------------------*/
#include "mlsInclude.h"
/*---------------------------------------------------------------------------
Project include files:
-----------------------------------------------------------------------------*/
#include "mlsCIFConfig.h"
#include "mlsCIEng.h"
#include "mlsUltil.h"
#include "em_errcode.h"

/* Application */
#include "service.h"

#if (MLS_CIENG_USE_DUMMY_DATA == 1)
#include "mlsDummy.h"
#endif

/*---------------------------------------------------------------------------
Local Types and defines:
--------------------------------------------------------------------------*/
/* For service 0x0F - Read firmware version */
#define FIRMWARE_VERSION_MAJOR (Byte)1
#define FIRMWARE_VERSION_MINOR (Byte)0
#define FIRMWARE_VERSION_BUILD (Byte)0
#define FIRMWARE_VERSION_REV   (Byte)0

typedef enum
{
    DATA_LOGGING_BEGIN = 0,
    DATA_LOGGING_DOING = 1,
    DATA_LOGGING_FINISH = 2,
}mlsDoDataLogStatus_t;

/*---------------------------------------------------------------------------
Global variables:
--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Static Function protoypes:
--------------------------------------------------------------------------*/
static mlsErrorCode_t cmdReadSFR(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                          Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdWriteSFR(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                           Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdReadEM(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdWriteEM(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                          Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdRawDataLogging(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                 Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdCalibrateRead(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdCalibrateWrite(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                 Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdFirmwareVersion(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                  Int32 inactivityElaspeTimeMs);
static mlsErrorCode_t cmdAutoCalibration(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                  Int32 inactivityElaspeTimeMs);
static UInt16 totalBlockCal(UInt8 DataID);
static Byte dataLenLoggingCal(UInt16 blockID, UInt8 DataID);
/*---------------------------------------------------------------------------
Static Variables:
--------------------------------------------------------------------------*/
static UInt16 gBulkTransferBlockNumber;
static UInt8 gIsBulkTransfer;

static Const mlsCICommand_t gkCommandTable[] = 
{
    { MLSCIENG_CTRLCODE_READSFR,            (cmdReadSFR)            },
    { MLSCIENG_CTRLCODE_WRITESFR,           (cmdWriteSFR)           },
    { MLSCIENG_CTRLCODE_READEMVALUE,        (cmdReadEM)             },
    { MLSCIENG_CTRLCODE_WRITEEMVALUE,       (cmdWriteEM)            },
    { MLSCIENG_CTRLCODE_RAWDATALOGGING,     (cmdRawDataLogging)     },
    { MLSCIENG_CTRLCODE_CALIREAD,           (cmdCalibrateRead)      },
    { MLSCIENG_CTRLCODE_CALIWRITE,          (cmdCalibrateWrite)     },
    { MLSCIENG_CTRLCODE_AUTOCALIBRATION,    (cmdAutoCalibration)    },
    { MLSCIENG_CTRLCODE_FIRMWAREVER,        (cmdFirmwareVersion)    },
    { 0x00, (commandHandlerFn)Null}
};

/*---------------------------------------------------------------------------
Exported functions:
--------------------------------------------------------------------------*/

void * mlsHandler_GetCommandTable(void)
{
    return (void*)&gkCommandTable[0];
}
/**
* return the index of the command table
*/
Int16 mlsHandler_SeekIndex(Byte commandCode)
{
    Int16 idx;

    for (idx = 0; idx< sizeof(gkCommandTable)/sizeof(gkCommandTable[0]); idx++)
    {
        if (commandCode == gkCommandTable[idx].code)
        {
            return idx;
        }
    }
    //return -1 if not found
    return -1;
}

/**
* return the Handler in the command table of appropriate command code
*/
commandHandlerFn mlsHandler_SeekHandler(Byte commandCode)
{
    Int16 idx;

    for (idx = 0; idx< sizeof(gkCommandTable)/sizeof(gkCommandTable[0]); idx++)
    {
        if (commandCode == gkCommandTable[idx].code)
        {
            return gkCommandTable[idx].fnCommandHandler;
        }
    }
    return (commandHandlerFn)Null;
}

/**
* reset all bulk transfer request
*/
void mlsHandler_ResetBulkTransfer(void)
{
    gIsBulkTransfer = 0;
    gBulkTransferBlockNumber = 0;
}


/***********************************************************************************************************************
* Function Name : cmdReadSFR
* Interface     : mlsErrorCode_t cmdReadSFR(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                          Int32 inactivityElaspeTimeMs)
* Description   : Read EM SFR Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdReadSFR(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                          Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    UInt32 SFRAddr;
    UInt8 req_len;
    st_service_param service_params;
    
    req_len  = (UInt8)request->dataBlock[4];
    /* Check parameter error */
    if( (req_len < READSFR_REQ_MIN_DATAREAD) || (req_len > READSFR_REQ_MAX_DATAREAD) || 
        (request->dataLen != 5) ||
        (request->blockNumber != 0x0001) || (request->totalBlock != 0x0001) )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    /* fill in the response message for prepare response */
    /* STX, ETX, BCC will be filled into responseSend */    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_READSFR);
    /* BlockNumber */
    response->blockNumber = READSFR_RES_BLOCKNUMBER;
    /* SFR datalen is different, it's length will be set by handler */
    response->dataLen = req_len;
    
    /* Get the SFR information and pass 4bytes addr + 1byte req length info to response */
    SFRAddr =   (((UInt32) request->dataBlock[0]) & 0x000000ff) | 
                (((UInt32) request->dataBlock[1] << 8) & 0x0000ff00) | 
                (((UInt32) request->dataBlock[2] << 16) & 0x00ff0000) |
                (((UInt32) request->dataBlock[3] << 24) & 0xff000000);
    response->dataBlock[0] = request->dataBlock[0];
    response->dataBlock[1] = request->dataBlock[1];
    response->dataBlock[2] = request->dataBlock[2];
    response->dataBlock[3] = request->dataBlock[3];
    response->dataBlock[4] = request->dataBlock[4];

    /* Fill the services params list, only need param */
    service_params.addr = SFRAddr;
    service_params.p_buff_out = &response->dataBlock[5];
    service_params.p_len_out = &response->dataLen;

    service_params.p_total_block = &response->totalBlock;
    
    /* Run service processing */
    if (SERVICE_ReadSFR(&service_params) != SERVICE_OK
        )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    /* Padding data len with 5 byte information */
    response->dataLen += 5;     /* Ouput length: 4bytes addr, 1byte length, nbyte data  */ 
    
    return returnValue;
}

/***********************************************************************************************************************
* Function Name : cmdWriteSFR
* Interface     : mlsErrorCode_t cmdWriteSFR(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                          Int32 inactivityElaspeTimeMs)
* Description   : Write EM SFR Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdWriteSFR(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                  Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    UInt32 SFRAddr;
    UInt8 req_len;
    st_service_param service_params;

    /* Check parameter error */
    if( ((UInt8)request->dataLen - 5) != (UInt8)request->dataBlock[4])
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    req_len  = (UInt8)request->dataBlock[4];
    if( (req_len <  WRITESFR_REQ_MIN_DATAWRITE) || (req_len >  WRITESFR_REQ_MAX_DATAWRITE) ||
        (request->blockNumber != 0x0001) || (request->totalBlock != 0x0001) )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    /* fill in the response message for prepare response */
    /* STX, ETX, BCC will be filled into responseSend */    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_WRITESFR);
    /* BlockNumber */
    response->blockNumber = WRITESFR_RES_BLOCKNUMBER;
    /* SFR datalen is different, it's length will be set by handler */
    response->dataLen = req_len;
    
    SFRAddr =   (((UInt32) request->dataBlock[0]) & 0x000000ff) | 
                (((UInt32) request->dataBlock[1] << 8) & 0x0000ff00) | 
                (((UInt32) request->dataBlock[2] << 16) & 0x00ff0000) |
                (((UInt32) request->dataBlock[3] << 24) & 0xff000000);
                
    /* Fill the services params list, only need param */
    service_params.addr = SFRAddr;
    service_params.p_buff_in = &request->dataBlock[5];
    service_params.p_len_out = &response->dataLen;

    service_params.p_total_block = &response->totalBlock;
    
    /* Run service processing */
    if (SERVICE_WriteSFR(&service_params) != SERVICE_OK)
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    /* Padding data len with 5 byte information */
    response->dataLen += 5;     /* Ouput length: 4bytes addr, 1byte length, nbyte data  */ 

    /*4 bytes addr and 1 byte data-len of response are 4 bytes addr and 1 
    byte data-len of request, so just need to assign response dataBlock to 
    request dataBlock*/
    response->dataBlock = request->dataBlock;
    
    return returnValue;
}

/***********************************************************************************************************************
* Function Name : cmdReadEM
* Interface     : mlsErrorCode_t cmdReadEM(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs)
* Description   : Read EM Data Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdReadEM(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    UInt16 ReadEMMode;
    st_service_param service_params;
    
    ReadEMMode = ((UInt16)request->dataBlock[0]) & 0x00ff;
    
    /* ControlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_READEMVALUE);

    /* ID */
    response->dataBlock[0] = request->dataBlock[0]; 

    /* Check timeout error */
    if( ((inactivityElaspeTimeMs>= INACTIVITY_BULK_REQUEST) && 
        (request->blockNumber == 0x0002))   ||
        ((inactivityElaspeTimeMs>=INACTIVITY_SINGLE_SUBSEQUENCE_REQUEST) 
        && (request->blockNumber > 0x0002)))
    {
        mlsHandler_ResetBulkTransfer();
        return MLS_ERROR_INACTIVITY_TIMEOUT;
    }
    
    /* Is first block? */
    if(request->blockNumber == 0x01)// 0x01
    {
        /* At first block, mark as not bulkTransfer */
        gIsBulkTransfer = 0;
        
        // Assign the BlockNumber to the first request
        gBulkTransferBlockNumber = 1; 
    }
    
    /* When bulk transfer, check the BlockNumber for find the SEQUENCE_ERROR */
    if(gIsBulkTransfer == 1)
    {
        if(gBulkTransferBlockNumber != request->blockNumber)
        {
            mlsHandler_ResetBulkTransfer();
            return MLS_ERROR_SEQUENCE_ERROR;
        }
    }

    /* fill in the response message for prepare response */
    /* STX, ETX, BCC will be filled into responseSend */    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_READEMVALUE);
    /* BlockNumber */
    response->blockNumber = gBulkTransferBlockNumber;

    /* Fill the services params list, only need param */
    service_params.block_id = gBulkTransferBlockNumber;
    service_params.p_id_list = (uint8_t*)&ReadEMMode;
    service_params.p_buff_out = &response->dataBlock[1];
    service_params.p_len_out = &response->dataLen;

    service_params.p_total_block = &response->totalBlock;
    
    /* Run service processing */
    if (SERVICE_ReadEMData(&service_params) != SERVICE_OK)
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }

    /* Length: data + 1byte of block len */
    response->dataLen += 1;
    /* Increase the BlockNumber for the next request */
    gBulkTransferBlockNumber++;
    
    /* Check to inform bulkTransferMode */
    if (response->totalBlock > 1)
    {
        gIsBulkTransfer = 1;
    }
    
    /* Is reach needed total block? */
    if(response->blockNumber == response->totalBlock)
    {
        gIsBulkTransfer = 0;    /* Stop bulk transfer */
    }
            
    return returnValue;
}

/***********************************************************************************************************************
* Function Name : cmdWriteEM
* Interface     : mlsErrorCode_t cmdWriteEM(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs)
* Description   : Write EM Data Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdWriteEM(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                          Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    st_service_param service_params;
    
    response->blockNumber = EMWRITE_RES_BLOCKNUMBER;
    /* Substract the length of DataID */
    request->dataLen -= 1;
    
    /* Fill the services params list, only need param */
    service_params.block_id = response->blockNumber;
    service_params.p_id_list = (uint8_t*)&request->dataBlock[0];
    service_params.p_buff_in = &request->dataBlock[1];
    service_params.p_len_in = &request->dataLen;

    service_params.p_total_block = &response->totalBlock;
    
    /* Run service processing */
    if (SERVICE_WriteEMData(&service_params) != SERVICE_OK)
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }       

    /* fill in the response message for prepare response */
    /* STX, ETX, BCC will be filled into responseSend */    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_WRITEEMVALUE);
    
    /* DataLen */
    response->dataLen = EMWRITE_RES_DATALEN;
    
    /* Data Block */
    response->dataBlock =  request->dataBlock;  
    
    return returnValue;
}

/***********************************************************************************************************************
* Function Name : cmdRawDataLogging
* Interface     : mlsErrorCode_t cmdRawDataLogging(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs)
* Description   : EM Raw Data Logging Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdRawDataLogging(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                 Int32 inactivityElaspeTimeMs)
{
    static UInt16 totalBlock;
    static UInt16 NumberOfSample;
    static UInt8 processingID = 0;
    static mlsDoDataLogStatus_t dataLogStatus;
    static Byte DataID[DATA_LOGGING_MAX_SUPPORT_CHANNEL];
    static Byte last_id;
    static UInt16 block_per_id;
    
    st_service_param service_params;
    
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    service_result_t result;
    
    UInt16 i, j;
    
    /*check data-length error. if it is the request, data-length must be 3,
    else (an ACK request) data-length must be 1*/
    if (((request->blockNumber == 1)&&(request->totalBlock == 1)&&
        (   request->dataLen!=DATA_LOGGING_REQ_NO_SAMPLE_DATA_LEN && 
            request->dataLen!=DATA_LOGGING_REQ_NORMAL_DATA_LEN))||
        (((request->blockNumber != 1)||(request->totalBlock != 1)) &&
        (request->dataLen!=0x01)))
    {
        SERVICE_RawDataLogCancelRequest();
        return MLS_ERROR_PARAMETER_ERROR;
    }

    /*if First ACK request and inactivity timeout*/
    if((inactivityElaspeTimeMs > INACTIVITY_BULK_REQUEST) && 
        (request->blockNumber == 0x0001) && (request->dataLen == 0x01 ) )
    {
        processingID = 0;
        gBulkTransferBlockNumber = 0;
        dataLogStatus = DATA_LOGGING_FINISH;
        last_id = 0;
        SERVICE_RawDataLogCancelRequest();
        return MLS_ERROR_INACTIVITY_TIMEOUT;
    }

    if((inactivityElaspeTimeMs > INACTIVITY_SINGLE_SUBSEQUENCE_REQUEST) && 
        (request->blockNumber > 1))
    {   
        processingID = 0;
        gBulkTransferBlockNumber = 0;
        dataLogStatus = DATA_LOGGING_FINISH;
        last_id = 0;
        SERVICE_RawDataLogCancelRequest();
        return MLS_ERROR_INACTIVITY_TIMEOUT;
    }
    
    /* if already finished the process and have ACK -> sequence error*/
    if ((dataLogStatus == DATA_LOGGING_FINISH) && (request->dataLen == 0x01))
    {
        return MLS_ERROR_SEQUENCE_ERROR;
    }

    //If first Request (datalen == 3), 5 if with no of sample
    if(request->dataLen == (Byte)DATA_LOGGING_REQ_NO_SAMPLE_DATA_LEN || request->dataLen == (Byte)DATA_LOGGING_REQ_NORMAL_DATA_LEN)
    {
        UInt8 num[2];
        uint8_t error_count = 0;
        
        /* Datalog data_id group array (for checking)
           0: is no selected,
           1: is adc raw data
           2: is measurement raw data
           For three phase: 15 data_id */
        for (i=0; i < DATA_LOGGING_MAX_SUPPORT_CHANNEL; i++)
        {
            if (request->dataBlock[i] == 0)
            {
                error_count++;
            }
        }
        if (error_count == DATA_LOGGING_MAX_SUPPORT_CHANNEL)
        {
            return MLS_ERROR_PARAMETER_ERROR;
        }
        
        /* store Data IDs to handle in next ACKs, and calculate total block */
        for(i = 0; i < DATA_LOGGING_MAX_SUPPORT_CHANNEL; i++)
        {
            DataID[i] = request->dataBlock[i];
        }
        
        /* Get number of sample requested (will be 0 if not requested) */
        num[0] = request->dataBlock[DATA_LOGGING_MAX_SUPPORT_CHANNEL];//F4
        num[1] = request->dataBlock[DATA_LOGGING_MAX_SUPPORT_CHANNEL+1];//01
        NumberOfSample = num[0];
        NumberOfSample = ((UInt16) num[1] << 8) | num[0];

        /* Change status to data being logged, bulktransfer on */
        dataLogStatus = DATA_LOGGING_DOING;
        gIsBulkTransfer = 1;
        
        /* mark that, we are processing first ID */
        processingID = 0;
        gBulkTransferBlockNumber = 1;
    }
    else if (request->dataLen == 0x01)
    {
        /*
         * check the ACK id with the last ID
         * block number of ACK must match with the last block from response
         */
        if (last_id != request->dataBlock[0] ||
            (gBulkTransferBlockNumber-1) != request->blockNumber)
        {
            /* reset bulk transfer sequence */
            processingID = 0;
            gBulkTransferBlockNumber = 0;
            dataLogStatus = DATA_LOGGING_FINISH;
            last_id = 0;
            SERVICE_RawDataLogCancelRequest();
            return MLS_ERROR_PARAMETER_ERROR;
        }
    }
    
    if(dataLogStatus == DATA_LOGGING_DOING)
    {
        /* un-expected end */
        if (processingID == DATA_LOGGING_MAX_SUPPORT_CHANNEL)
        {
            processingID = 0;
            gBulkTransferBlockNumber = 0;
            dataLogStatus = DATA_LOGGING_FINISH;
            last_id = 0;
            SERVICE_RawDataLogCancelRequest();
            return MLS_ERROR_PARAMETER_ERROR;
        }
        else
        {
            /* Do nothing */
        }

        last_id = DataID[processingID];
        
        /* fill ctrlCode, blockNum, totalBlock into */
        response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_RAWDATALOGGING);
        response->blockNumber = gBulkTransferBlockNumber;
        
        /* TODO: to integrate with EM-CORE library */
        /* to get (response->dataLen-1) bytes of data and fill into 
        &response->dataBock[1]*/
        
        /* Fill the services params list */
        service_params.current_id = last_id;
        service_params.block_id = gBulkTransferBlockNumber;
        service_params.p_id_list = (uint8_t*)&DataID[0];
        service_params.p_buff_out = &response->dataBlock[0];
        service_params.p_len_out = &response->dataLen;
        
        service_params.p_req_samples = &NumberOfSample;
        service_params.p_block_per_id = &block_per_id;
        service_params.p_total_block = &response->totalBlock;
        
        result = SERVICE_DoRawDataLogging(&service_params);
        
        if (result == SERVICE_REQUEST_SIZE_ERROR)
        {
            processingID = 0;
            dataLogStatus = DATA_LOGGING_FINISH;
            SERVICE_RawDataLogCancelRequest();
            gIsBulkTransfer=0;
            last_id = 0;
        }
        else
        {
            if (result != SERVICE_OK)
            {
                SERVICE_RawDataLogCancelRequest();
                return MLS_ERROR_PARAMETER_ERROR;
            }
            else
            {
                /* Do nothing */
            }
        }
        /* check for ending of raw data log sequence */
        if ( (gBulkTransferBlockNumber >= response->totalBlock))
        {
            processingID = 0;
            dataLogStatus = DATA_LOGGING_FINISH;
            SERVICE_RawDataLogCancelRequest();
            gIsBulkTransfer=0;
            last_id = 0;
        }
        
        /* Increase block number for next transfer */
        gBulkTransferBlockNumber ++;
        
        /* Is the block number  */
        /* skip NULL id */
        while ( ( ( (DataID[processingID] & 0x0F) == 0) ||
                  (gBulkTransferBlockNumber > (block_per_id * (processingID+1)))) &&
                  (processingID < DATA_LOGGING_MAX_SUPPORT_CHANNEL) )
        {
            processingID++;
        }

    }
    return returnValue;
}

/***********************************************************************************************************************
* Function Name : cmdCalibrateRead
* Interface     : mlsErrorCode_t cmdCalibrateRead(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs)
* Description   : Read EM Calibration Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdCalibrateRead(mlsCIReqRes_t *request, mlsCIReqRes_t *response,
                                Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    Byte DataID;
    st_service_param service_params;
    
    /* Check parameter error */
    if( (request->blockNumber != 1) ||
        (request->totalBlock != 1) ||
        (request->dataLen != 1) )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    /* fill in the response message for prepare response */
    /* STX, ETX, BCC will be filled into responseSend */    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_CALIREAD);
    /* BlockNumber */
    response->blockNumber = READ_CALIBRATION_RES_BLOCKNUMBER;
    /* Data Len */
    DataID = request->dataBlock[0];

    /* Fill in response data block */
    response->dataBlock[0] = request->dataBlock[0];

    /* Fill the services params list, only need param */
    service_params.p_id_list = (uint8_t*)&DataID;
    service_params.p_buff_out = &response->dataBlock[1];
    service_params.p_len_out = &response->dataLen;

    service_params.p_total_block = &response->totalBlock;

    /* TODO: to integrate with EM-CORE library */
    if (SERVICE_ReadCalibrationData(&service_params) != SERVICE_OK)
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    
    response->dataLen += 1;

    return returnValue;
}

/***********************************************************************************************************************
* Function Name : cmdCalibrateWrite
* Interface     : mlsErrorCode_t cmdCalibrateWrite(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                         Int32 inactivityElaspeTimeMs)
* Description   : Write EM Calibration Handler
* Arguments     : mlsCIReqRes_t *request        : input request pointer
*               : mlsCIReqRes_t *response       : output response pointer
*               : Int32 inactivityElaspeTimeMs  : inactivity counter
* Function Calls: TBD
* Return Value  : mlsErrorCode_t
***********************************************************************************************************************/
mlsErrorCode_t cmdCalibrateWrite(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                 Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    st_service_param service_params;
    
    /* Substract the length of DataID */
    request->dataLen -= 1;
    
    if( (request->blockNumber != 1) ||
        (request->totalBlock != 1)  )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    

    /* fill in the response message for prepare response */
    /* STX, ETX, BCC will be filled into responseSend */    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_CALIWRITE);
    /* BlockNumber */
    response->blockNumber = WRITE_CALIBRATION_RES_BLOCKNUMBER;
    /* Data Len */
    response->dataLen = WRITE_CALIBRATION_RES_DATALEN;
    /* Data Block */
    response->dataBlock = request->dataBlock;

    /* Fill the services params list, only need param */
    service_params.p_id_list = (uint8_t*)&request->dataBlock[0];
    service_params.p_buff_in = &request->dataBlock[1];
    service_params.p_len_in = &request->dataLen;

    service_params.p_total_block = &response->totalBlock;

    
    /* TODO: to integrate with EM-CORE library */
    /*data to write to EM core has request->dataLen-1 bytes starting from 
    &request->dataBlock[1]*/
    if (SERVICE_WriteCalibrationData(&service_params) != SERVICE_OK)
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }

    return returnValue;
}

mlsErrorCode_t cmdFirmwareVersion(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                  Int32 inactivityElaspeTimeMs)
{
    mlsErrorCode_t returnValue = MLS_SUCCESS;
    /* Check parameter error */
    if( (request->blockNumber != 1) ||
        (request->totalBlock != 1) ||
        (request->dataLen != 0) )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    else
    {
        /* fill in the response message for prepare response */
        /* STX, ETX, BCC will be filled into responseSend */    
        /* CtrlCode */
        response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_FIRMWAREVER);
        /* BlockNumber */
        response->blockNumber = READ_FIRMVER_RES_BLOCKNUMBER;
        /* Total Block */
        response->totalBlock = READ_FIRMVER_RES_TOTALBLOCK; 
        /* Data length */
        response->dataLen = READ_FIRMVER_RES_DATALEN;

        if (SERVICE_ReadFirmwareVersion(
            (uint8_t *)(&response->dataBlock[0]),
            READ_FIRMVER_RES_DATALEN
        ) != SERVICE_OK)
        {
            return MLS_ERROR_PARAMETER_ERROR;
        }
    }

    return returnValue;
}
mlsErrorCode_t cmdAutoCalibration(mlsCIReqRes_t *request,mlsCIReqRes_t *response,
                                  Int32 inactivityElaspeTimeMs)
{
    st_service_param service_params;

    mlsErrorCode_t returnValue = MLS_SUCCESS;
    
    if( (request->blockNumber != 1) ||
        (request->totalBlock != 1)  )
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
        
    /* fill in the response message for prepare response */
    /* STX */
    response->stx = STX;    
    /* CtrlCode */
    response->ctrlCode = RESPONSE_FROM_REQUEST(MLSCIENG_CTRLCODE_AUTOCALIBRATION);	// (request | (UInt8) 1 << 7 )
    /* BlockNumber */
    response->blockNumber = AUTOCALIBRATION_RES_BLOCKNUMBER;
    /* Total Block */
    response->totalBlock = AUTOCALIBRATION_RES_TOTALBLOCK;  
    /* Data length */
    /* response->dataLen = AUTOCALIBRATION_RES_DATALEN_ID1; */
    /* Response DataBlock */
    response->dataBlock[0] = request->dataBlock[0];

    /* Fill the services params list, only need param */
    service_params.p_id_list = (uint8_t*)&request->dataBlock[0];
    service_params.p_buff_in = &request->dataBlock[1];
    service_params.p_buff_out = &response->dataBlock[1];
    service_params.p_len_out = &response->dataLen;
    
    if (SERVICE_DoAutoCalibration(&service_params) != SERVICE_OK)
    {
        return MLS_ERROR_PARAMETER_ERROR;
    }
    else
    {
        /* success */
        response->dataLen++;    /* 1 more for step ID */
    }
            
    return returnValue;
}

