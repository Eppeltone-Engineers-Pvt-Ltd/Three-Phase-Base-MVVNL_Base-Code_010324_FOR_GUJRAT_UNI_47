
/**
* @defgroup mlsCIComm 
* @brief  mlsCIComm contains a list of platform specific API 
* @{
*/

/**
* @file mlsCIComm.h
* @brief contains API prototype declaration for use among platforms
*
* This is the mlsCIComm component
*
*/

#ifndef __MLSCICOMM_H__
#define __MLSCICOMM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mlsInclude.h"

/*! \typedef mlsErrorCode_t (* serialReadFn)(Void* dataBuff, UInt32 lenToRead, 
                                          UInt32* lenReaded, Int32 timeOutMs)                                                                            
    Declare function pointer to mlsCIComm_SerialRead or mlsCICommSerialRead                                                                             
 */
typedef mlsErrorCode_t (* serialReadFn)(Void* dataBuff, UInt32 lenToRead, 
                                          UInt32* lenReaded, Int32 timeOutMs);

/*! \typedef mlsErrorCode_t (* serialWriteFn)(Const Void* dataBuff, 
                        UInt32 lenToWrite, UInt32* lenWritten, Int32 timeOutMs)                                                                           
    Declare function pointer to mlsCIComm_SerialWrite or mlsCICommSerialWrite                                                                             
 */
typedef mlsErrorCode_t (* serialWriteFn)(Const Void* dataBuff, 
                        UInt32 lenToWrite, UInt32* lenWritten, Int32 timeOutMs);

/*! \typedef mlsErrorCode_t (* serialInitFn)(void)                                                                          
    Declare function pointer to mlsCIComm_SerialInit or mlsCICommSerialInit                                                                             
 */
typedef mlsErrorCode_t (* serialInitFn)(void);

/*! \typedef mlsErrorCode_t (* serialDeinitFn)(void)                                                                          
    Declare function pointer to mlsCIComm_SerialDeinit or mlsCICommSerialDeinit                                                                             
 */
typedef mlsErrorCode_t (* serialDeinitFn)(void);

/*! \typedef mlsErrorCode_t (* serialFlushFn)(void)                                                                         
    Declare function pointer to mlsCIComm_SerialFlush or mlsCICommSerialFlush                                                                             
 */
typedef mlsErrorCode_t (* serialFlushFn)(void);

/*! \struct mlsCICommConf_t                                                                         
    Declare a struct contain of function pointers                                                                            
 */
typedef struct mlsCIPCommConf
{
    serialInitFn    fnSerialInit;
    serialDeinitFn  fnSerialDeinit;
    serialReadFn    fnSerialRead;
    serialWriteFn   fnSerialWrite;  
    serialFlushFn   fnSerialFlush;
}mlsCICommConf_t;

/*! \enum mlsUartSendState_t                                                                         
    Declare a enum contain of send state
 */
typedef enum
{   
    SEND_IDLE = 0,
    SENDING = 1,
}mlsUartSendState_t;

/*---------------------------------------------------------------------------
   mlsModuleOperation
   --------------------------------------------------------------------------*/
/**
 * @brief mlsCIComm_SerialInit function
 *
 * Init serial CICom
 * @return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCIComm_SerialInit(void);

/**
 * @brief mlsCIComm_SerialDeinit function
 *
 * Deinit serial CICom
 * @return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCIComm_SerialDeinit(void);

/**
 * @brief mlsCIComm_SerialRead function
 *
 * Read a buffer from serial with lenToRead length in timeOutMs. lenReaded is actual bytes readed.
 * @param[in] lenToRead a UInt32, timeOutMs a Int32
 * @param[out] dataBuff a Void*, lenReaded a UInt32*
 * @return Return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCIComm_SerialRead(Void* dataBuff, UInt32 lenToRead, 
                                    UInt32* lenReaded, Int32 timeOutMs);
                                    
/**
 * @brief mlsCIComm_SerialWrite function
 *
 * Write a buffer to serial with lenToWrite length in timeOutMs. lenWritten is actual bytes written.
 * @param[in] lenToWrite a UInt32, timeOutMs a Int32
 * @param[out] dataBuff a Const Void*, lenReaded a UInt32*
 * @return Return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCIComm_SerialWrite(Const Void* dataBuff, 
                        UInt32 lenToWrite, UInt32* lenWritten, Int32 timeOutMs);
                        
/**
 * @brief mlsCIComm_SerialFlush function
 *
 * Flush data in serial
 * @return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCIComm_SerialFlush(void);

/**
 * @brief mlsCICommSerialInit function
 *
 * Init serial CICom
 * @return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCICommSerialInit(void);

/**
 * @brief mlsCICommSerialDeinit function
 *
 * Deinit serial CICom
 * @return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCICommSerialDeinit(void);

/**
 * @brief mlsCICommSerialRead function
 *
 * Read a buffer from serial with lenToRead length in timeOutMs. lenReaded is actual bytes readed.
 * @param[in] lenToRead a UInt32, timeOutMs a Int32
 * @param[out] dataBuff a Void*, lenReaded a UInt32*
 * @return Return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCICommSerialRead(Void* dataBuff, UInt32 lenToRead, 
                                    UInt32* lenReaded, Int32 timeOutMs);
                                    
/**
 * @brief mlsCICommSerialWrite function
 *
 * Write a buffer to serial with lenToWrite length in timeOutMs. lenWritten is actual bytes written.
 * @param[in] lenToWrite a UInt32, timeOutMs a Int32
 * @param[out] dataBuff a Const Void*, lenReaded a UInt32*
 * @return Return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCICommSerialWrite(Const Void* dataBuff, 
                        UInt32 lenToWrite, UInt32* lenWritten, Int32 timeOutMs);

/**
 * @brief mlsCICommSerialFlush function
 *
 * Flush data in serial
 * @return mlsErrorCode_t
 * @retval MLS_SUCCESS value is 0x00000000
 * @retval MLS_ERROR value is 0xFFFFFFFF
 */
mlsErrorCode_t mlsCICommSerialFlush(void);

/**
 * @brief mlsSetUartSendStatus function
 *
 * Set uart send status
 * @param[in] mlsUartSendState_t a status of sending
 * @return void
 */
void mlsSetUartSendStatus(mlsUartSendState_t status);

/**
 * @brief mlsUartReceiveByte function
 *
 * UART receive byte
 * @param[in] Byte a rx_data of received data
 * @return void
 */
void mlsUartReceiveByte(Byte rx_data);

#ifdef __cplusplus
}
#endif
#endif /* __MLSCICOMM_H__ */
/** @} */
