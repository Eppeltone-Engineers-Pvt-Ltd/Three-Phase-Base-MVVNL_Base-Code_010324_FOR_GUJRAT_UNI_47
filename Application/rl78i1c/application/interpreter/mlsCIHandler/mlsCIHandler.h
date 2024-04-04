
/**
* @defgroup mlsCIHandler
* @brief  mlsCIHandler interface to EM Core library to service the request from
* Terminal
* @{
*/

/**
* @file mlsCIHandler.h
* @brief contains API prototype declaration of mlsCIHandler module
*
* This is the mlsCIHandler component
*
*/

#ifndef __MLSCIHANDLER_H__
#define __MLSCIHANDLER_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "mlsInclude.h"
#include "mlsCIFConfig.h"
#include "mlsCIEng.h"

/*! \struct mlsCIReqRes_t                                                                         
    Declare request/receive struct                                                                           
 */
typedef struct mlsCIEngReqRes
{
    UInt8 stx;
    UInt8 ctrlCode;
    UInt16 blockNumber;
    UInt16 totalBlock;
    UInt8 dataLen;
    UInt8* dataBlock;
    UInt16 bcc;
    UInt8 etx;
}mlsCIReqRes_t;

/*! \typedef mlsErrorCode_t (* commandHandlerFn)(mlsCIReqRes_t * request,mlsCIReqRes_t *response,Int32 inactivityElaspeTimeMs)                                                                         
    Declare fnCommandHandler function pointer                                                                           
 */
typedef mlsErrorCode_t (* commandHandlerFn)(mlsCIReqRes_t * request,mlsCIReqRes_t *response,Int32 inactivityElaspeTimeMs);


/*! \typedef struct mlsCICommand_t
    Declare struct mlsCICommand_t
*/
typedef struct mlsCICommand
{
    Byte code;
    commandHandlerFn fnCommandHandler;
}mlsCICommand_t;

/**
 * @brief mlsHandler_GetCommandTable function
 *
 * Get the address of Handler in CommandTable
 * @param[in] none
 * @return Void*
 */
void * mlsHandler_GetCommandTable(void);

/**
 * @brief mlsHandler_SeekIndex function
 *
 * return the index of the command table
 * @param[in] commandCode is a Byte
 * @return a Int16 index
 */
Int16 mlsHandler_SeekIndex(Byte commandCode);

/**
 * @brief mlsHandler_SeekHandler function
 *
 * return the Handler in the command table of appropriate command code
 * @param[in] commandCode a Byte
 * @return Handler a commandHandlerFn
 */
commandHandlerFn mlsHandler_SeekHandler(Byte commandCode);

#ifdef __cplusplus
}
#endif
#endif /* __MLSCIHANDLER_H__ */
