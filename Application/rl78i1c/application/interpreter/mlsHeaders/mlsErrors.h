/**
 * @defgroup mlsHeaderFile
 * @brief brief description of the module
 *
 * Detail description of the module
 * @{
 */
 
 /**
* @file mlsErrors.h
* @brief project widdefination 
*
* This header file contains project wide defination
*
*/

#ifndef __MLSERRORS_H__
#define __MLSERRORS_H__

#include "mlsTypes.h"
#include "typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t mlsErrorCode_t;

/*---------------------------------------------------
 * Common error codes
 * --------------------------------------------------*/
 /*! \def MLS_SUCCESS                                                                                  
    Global success return status                                                                                  
 */
#define MLS_SUCCESS                                     0x00000000  

/*! \def MLS_TRANSFERING                                                                                  
    Global success return status                                                                                  
 */
#define MLS_TRANSFERING                                 0x0000000F  

/*! \def MLS_ERROR                                                                                  
    Global error return status                                                                                 
 */
#define MLS_ERROR                                       0xFFFFFFFF 


/*! \def MLS_ERROR_REQUEST_UNIDENTIFIED                                                                                  
    Request is unidentified                                                                                 
 */
#define MLS_ERROR_REQUEST_UNIDENTIFIED                  0x00000001


/*! \def MLS_ERROR_REQUEST_IGNORED                                                                                  
    Request is ignored due to another request is in progress                                                                                
 */
#define MLS_ERROR_REQUEST_IGNORED                       0x00000002 


/*! \def MLS_ERROR_RESPONSE_UNIDENTIFIED                                                                                  
    Response is unidentified                                                                               
 */
#define MLS_ERROR_RESPONSE_UNIDENTIFIED                 0x00000003


/*! \def MLS_ERROR_RESPONSE_BCCERROR                                                                                  
    Reponse BCC Error                                                                               
 */
#define MLS_ERROR_RESPONSE_BCCERROR                     0x00000004


/*! \def MLS_ERROR_RESPONSE_TIMEOUT                                                                                  
    Response is time-out                                                                             
 */
#define MLS_ERROR_RESPONSE_TIMEOUT                      0x00000005


/*! \def MLS_ERROR_SERIALTIMEOUT                                                                                  
    Serial communication is time-out                                                                             
 */
#define MLS_ERROR_SERIALTIMEOUT                         0x00000006


/*! \def MLS_ERROR_INACTIVITY_TIMEOUT                                                                                  
    Inactivity is time-out                                                                            
 */
#define MLS_ERROR_INACTIVITY_TIMEOUT                    0x00000007


/*! \def MLS_ERROR_SEQUENCE_ERROR                                                                                  
    Sequence error                                                                            
 */
#define MLS_ERROR_SEQUENCE_ERROR                        0x00000008


/*! \def MLS_ERROR_PARAMETER_ERROR                                                                                  
    Parameter error                                                                           
 */
#define MLS_ERROR_PARAMETER_ERROR                       0x00000009


/*! \def MLS_ERROR_EXECUTE_ERROR                                                                                  
    Execute error                                                                          
 */
#define MLS_ERROR_EXECUTE_ERROR                         0x0000000A

/*! \def MLS_ERROR_SERIALPORT_NOTSET                                                                                  
    Serial Port Not Set                                                                         
 */
#define MLS_ERROR_SERIALPORT_NOTSET                     0x0000000B
#ifdef __cplusplus
}
#endif

#endif /*__MLSERRORS_H__*/

/** @} */

