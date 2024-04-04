
/**
* @defgroup mlsCIEng
* @brief  mlsCIEng hosts the engine of Command Interpreter module, which receive 
* Request on serial line, handle the request then send Response back to the 
* Terminal
* @{
*/

/**
* @file mlsCIEng.h
* @brief contains API prototype declaration of mlsCIEng module
*
* This is the mlsCIEng component
*
*/

#ifndef __MLSCIENG_H__
#define __MLSCIENG_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "mlsInclude.h"
#include "mlsCIFConfig.h"
#include "mlsCIHandler.h"
#include "mlsCIComm.h"

typedef enum 
{
    INITIAL = 0,
    WAIT_STX = 1,
    WAIT_CTRLCODE = 2,
    WAIT_BLOCKNUMBER_L = 3,
    WAIT_BLOCKNUMBER_H = 4,
    WAIT_TOTALBLOCK_L = 5,
    WAIT_TOTALBLOCK_H = 6,
    WAIT_DATALEN = 7,
    WAIT_DATABLOCK = 8,
    WAIT_BCC_L = 9,
    WAIT_BCC_H = 10,
    WAIT_ETX = 11,
    IDLE  = 12,
}mlsCIEngReceiveState_t;

typedef enum
{
    WAITING_REQUEST = 0,
    HAVE_REQUEST = 1,
    
}mlsRequestState_t;


#define DATA_BUFFER_LEN     266

/*! \def STX                                                                            
    Start of text                                                                           
 */
#define STX                                 (0x02)


/*! \def ETX                                                                     
    End of text                                                                    
 */
#define ETX                                 (0x03)


/*! \def MLSCIENG_CTRLCODE_READSFR                                                                     
    Read SFR control code                                                                   
 */
#define MLSCIENG_CTRLCODE_READSFR           (0x01)


/*! \def MLSCIENG_CTRLCODE_WRITESFR                                                                     
    Write SFR control code                                                                         
 */
#define MLSCIENG_CTRLCODE_WRITESFR          (0x02)


/*! \def MLSCIENG_CTRLCODE_READEMVALUE                                                                     
    Read EM control code                                                                     
 */
#define MLSCIENG_CTRLCODE_READEMVALUE       (0x03)


/*! \def MLSCIENG_CTRLCODE_WRITEEMVALUE                                                                      
    Write EM control code                                                                      
 */
#define MLSCIENG_CTRLCODE_WRITEEMVALUE      (0x04)


/*! \def MLSCIENG_CTRLCODE_RAWDATALOGGING                                                                      
    Do data logging control code                                                                      
 */
#define MLSCIENG_CTRLCODE_RAWDATALOGGING    (0x05)


/*! \def MLSCIENG_CTRLCODE_CALIREAD                                                                      
    Calibration Read control code                                                                      
 */
#define MLSCIENG_CTRLCODE_CALIREAD          (0x06)


/*! \def MLSCIENG_CTRLCODE_CALIWRITE                                                                      
    Calibration Write control code                                                                      
 */
#define MLSCIENG_CTRLCODE_CALIWRITE         (0x07)


/*! \def MLSCIENG_CTRLCODE_AUTOCALIBRATION                                                                      
    Auto Calibration control code                                                                      
 */
#define MLSCIENG_CTRLCODE_AUTOCALIBRATION   (0x08)


/*! \def MLSCIENG_CTRLCODE_FIRMWAREVER                                                                            
    Read Firmware Version control code                                                                            
 */
#define MLSCIENG_CTRLCODE_FIRMWAREVER       (0x0F)


/*! \def MLSCIENG_CTRLCODE_ACK                                                                      
    Control Code ACK                                                                      
 */
#define MLSCIENG_CTRLCODE_ACK               (0xFF)


/*! \def RESPONSE_FROM_REQUEST(request)                                                                      
    Control Code Request from Response                                                                    
 */
#define RESPONSE_FROM_REQUEST(request)      (request | (UInt8) 1 << 7 )


/*! \def EMREAD_ID2                                                                   
    EM Read ID 0x02                                                                    
 */
#define EMREAD_ID2                          (0x02)


/*! \def EMREAD_ID3                                                                   
    EM Read ID 0x03                                                                    
 */
#define EMREAD_ID3                          (0x03)


/*! \def AUTOCALI_ID1                                                                   
    Auto Calibration ID 0x01                                                                  
 */
#define AUTOCALI_ID1                        (0x01)


/*! \def AUTOCALI_ID2                                                                   
    Auto Calibration ID 0x02                                                                 
 */
#define AUTOCALI_ID2                        (0x02)


/*! \def INACTIVITY_1                                                                         
    Inactivity time out case 1                                                                       
 */
#define INACTIVITY_SINGLE_SUBSEQUENCE_REQUEST       (6000)

#define READTIME    10
/*! \def INACTIVITY_2                                                                         
    Inactivity time out case 2                                                                       
 */
#define INACTIVITY_BULK_REQUEST                     (12000)


/*! \def INACTIVITY_3                                                                         
    Inactivity time out case 3                                                                       
 */
#define INACTIVITY_AUTO_CALI                        (300000)

/*! \enum mlsCIResponseErrorCode_t
    The errors table
*/
typedef enum
{
    MLS_BCC_ERROR = 0xC4,
    MLS_INVALID_REQUEST = 0xC1,
    MLS_TIMEOUT = 0xC0,
    MLS_SEQUENCE_ERROR = 0xC5,
    MLS_PARAMETER_ERROR = 0xC6,
    MLS_EXECUTE_ERROR = 0xC7,   /* add */
}mlsCIResponseErrorCode_t;


/*! \enum mlsTimerStatus_t
    Timer struct
*/
typedef enum
{
    TIMERINIT   = -1,
    TIMERSTART  = 0,
    TIMERSTOP   = 1,
}mlsTimerStatus_t;


/*! \enum mlsTimer_t
    Timer struct
*/
typedef struct
{
    UInt32 timerValue;
    UInt32 startTimerValue;
    UInt32 stopTimerValue;
    mlsTimerStatus_t timerStatus;
}mlsTimer_t;

/*! \enum mlsTransferTypeTimeout_t
    Type of transfer
*/
typedef enum
{
    SINGLE_TRANSMIT = 0,
    BULKTRANSFER = 1,
    AUTO_CALI_REQ = 2,
}mlsTransferTypeTimeout_t;
/**
 * @brief Sample function
 *
 * Detail description of funtion
 * @pre pre-condition
 * @post post-condition
 * @param[in] param1 input param
 * @param[out] param2 output param
 * @param[in,out] param3 input, output param
 * @return Return values description
 * @retval 0 description of return value 0
 * @retval 1 description of return value 1
 * @see referencedFunction()
 */
mlsErrorCode_t mlsCIEng_Init(const mlsCICommConf_t * config, const mlsCICommand_t * commandTable);
mlsErrorCode_t mlsCIEng_Deinit(void);
mlsErrorCode_t mlsCIEng_ProcessStart(void);
mlsErrorCode_t mlsCIEng_ProcessStop(void);
mlsRequestState_t mlsGetRequestStatus(void);
void mlsSetRequestStatus(mlsRequestState_t status);
#ifdef __cplusplus
}
#endif
#endif /* __MLSCIENG_H__ */

/** @} */
