
/**
* @file mlsCIFConfig.h
* @brief project wide defination 
*
* This header file contains project wide defination
*
*/

#ifndef __MLSCIFCONFIG_H__
#define __MLSCIFCONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mlsinclude.h"

/*! \def MLSCIF_WINDOW_FLATFORM_ENABLE                                                                                  
    1: windows platform
    0: RL78 platform                                                                                     
 */
#define MLSCIF_WINDOW_FLATFORM_ENABLE 0

/*! \def MLSCIF_DEBUG_ENABLE                                                                                  
    Using this flag to debug                                                                                    
 */
#define MLSCIF_DEBUG_ENABLE 

/*! \def MLS_CIENG_USE_DUMMY_DATA                                                                                  
    Using this flag to use dummy data                                                                                  
 */
#define MLS_CIENG_USE_DUMMY_DATA    0

/*! \def TIMER_MAX_TICKCOUNT                                                                                  
    If timer match max tick count, it will reset                                                                              
 */
#define TIMER_MAX_TICKCOUNT 4000000000


/*! \enum mlsReadSFR_t
    The information of Read SFR
*/
typedef enum
{
    READSFR_REQ_CTRLCODE = 0x01,        /*!< Enum value READSFR_REQ_CTRLCODE. */
    READSFR_REQ_BLOCKNUMBER = 1,        /*!< Enum value READSFR_REQ_BLOCKNUMBER. */
    READSFR_REQ_TOTALBLOCK = 1,         /*!< Enum value READSFR_REQ_TOTALBLOCK. */
    READSFR_REQ_MIN_DATAREAD = 1,       /*!< Enum value READSFR_REQ_MIN_DATAREAD. */
    READSFR_REQ_MAX_DATAREAD = 200,     /*!< Enum value READSFR_REQ_MAX_DATAREAD. */

    READSFR_RES_BLOCKNUMBER = 1,        /*!< Enum value READSFR_RES_BLOCKNUMBER. */
    READSFR_RES_TOTALBLOCK = 1,         /*!< Enum value READSFR_RES_TOTALBLOCK. */
    READSFR_RES_DATALEN = 1,            /*!< Enum value READSFR_RES_DATALEN. */
}mlsReadSFR_t;

/*! \enum mlsWriteSFR_t
    The information of Write SFR
*/
typedef enum
{
    WRITESFR_REQ_MIN_DATAWRITE = 1,     /*!< Enum value WRITESFR_REQ_MIN_DATAWRITE. */
    WRITESFR_REQ_MAX_DATAWRITE = 200,   /*!< Enum value WRITESFR_REQ_MAX_DATAWRITE. */

    WRITESFR_RES_BLOCKNUMBER = 1,       /*!< Enum value WRITESFR_RES_BLOCKNUMBER. */
    WRITESFR_RES_TOTALBLOCK = 1,        /*!< Enum value WRITESFR_RES_TOTALBLOCK. */
    WRITESFR_RES_DATALEN = 5,           /*!< Enum value WRITESFR_RES_DATALEN. */
}mlsWriteSFR_t;


/*! \enum mlsReadFirmwareVersion_t
    The information of Read Firmware Version
*/
typedef enum
{
    READ_FIRMVER_RES_BLOCKNUMBER = 1,   /*!< Enum value READ_FIRMVER_RES_BLOCKNUMBER. */
    READ_FIRMVER_RES_TOTALBLOCK = 1,    /*!< Enum value READ_FIRMVER_RES_TOTALBLOCK. */
    READ_FIRMVER_RES_DATALEN = 4,       /*!< Enum value READ_FIRMVER_RES_DATALEN. */

    FIRMWARE_MAJOR = 0xAA,              /*!< Enum value FIRMWARE_MAJOR. */
    FIRMWARE_MINOR = 0xBB,              /*!< Enum value FIRMWARE_MINOR. */
    FIRMWARE_BUILD = 1,                 /*!< Enum value FIRMWARE_BUILD. */
    FIRMWARE_REVISION = 0,              /*!< Enum value FIRMWARE_REVISION. */
}mlsReadFirmwareVersion_t;


/*! \enum mlsReadCalibrationData_t
    The information of Read Calibration Data
*/
typedef enum
{
    READ_CALIBRATION_REQ_MAX_DATAID  = 0xB6,/*!< Enum value READ_CALIBRATION_REQ_MAX_DATAID. */
    READ_CALIBRATION_REQ_MIN_DATAID  = 1,   /*!< Enum value READ_CALIBRATION_REQ_MIN_DATAID. */

    READ_CALIBRATION_RES_BLOCKNUMBER = 1,   /*!< Enum value READ_CALIBRATION_RES_BLOCKNUMBER. */
    READ_CALIBRATION_RES_TOTALBLOCK  = 1,   /*!< Enum value READ_CALIBRATION_RES_TOTALBLOCK. */

    READ_CALIBRATION_RES_DATALEN_1 = 3,     /*!< Enum value READ_CALIBRATION_RES_DATALEN_1. */
    READ_CALIBRATION_RES_DATALEN_2 = 5,     /*!< Enum value READ_CALIBRATION_RES_DATALEN_2. */
}mlsReadCalibrationData_t;


/*! \enum mlsWriteCalibration_t
    The information of Write Calibration Data
*/
typedef enum
{
    WRITE_CALIBRATION_REQ_MAX_DATAID  = 0xB6,   /*!< Enum value WRITE_CALIBRATION_REQ_MAX_DATAID. */
    WRITE_CALIBRATION_REQ_MIN_DATAID  = 1,  /*!< Enum value WRITE_CALIBRATION_REQ_MIN_DATAID. */

    WRITE_CALIBRATION_RES_BLOCKNUMBER = 1,  /*!< Enum value WRITE_CALIBRATION_RES_BLOCKNUMBER. */
    WRITE_CALIBRATION_RES_TOTALBLOCK  = 1,  /*!< Enum value WRITE_CALIBRATION_RES_TOTALBLOCK. */
    WRITE_CALIBRATION_RES_DATALEN     = 1,  /*!< Enum value WRITE_CALIBRATION_RES_DATALEN. */
}mlsWriteCalibration_t;


/*! \enum mlsEMRead_t
    The information of EM Read
*/
typedef enum 
{
    
    EMREAD_RES_BLOCKNUMBER_MODE1 = 1,       /*!< Enum value EMREAD_RES_BLOCKNUMBER_MODE1. */
    EMREAD_RES_TOTALBLOCK_MODE1  = 1,       /*!< Enum value EMREAD_RES_TOTALBLOCK_MODE1. */
    EMREAD_RES_DATALEN_MODE1     = 1,       /*!< Enum value EMREAD_RES_DATALEN_MODE1. */

    EMREAD_REQ_MIN_BLOCKNUMBER_MODE2 = 1,   /*!< Enum value EMREAD_REQ_MIN_BLOCKNUMBER_MODE2. */
    EMREAD_RES_DATALEN_MODE2     = 201,     /*!< Enum value EMREAD_RES_DATALEN_MODE2. */
    EMREAD_RES_TOTALBLOCK_MODE2  = 12,      /*!< Enum value EMREAD_RES_TOTALBLOCK_MODE2. */
    EMREAD_RES_DATABLOCK0_MODE2  = 2,       /*!< Enum value EMREAD_RES_DATABLOCK0_MODE2. */

    EMREAD_RES_DATABLOCK0_MODE3 = 3,        /*!< Enum value EMREAD_RES_DATABLOCK0_MODE3. */
    EMREAD_RES_DATALEN_MODE3   = 201,       /*!< Enum value EMREAD_RES_DATALEN_MODE3. */
    EMREAD_REQ_MIN_BLOCKNUMBER_MODE3 = 1,   /*!< Enum value EMREAD_REQ_MIN_BLOCKNUMBER_MODE3. */
    EMREAD_REQ_MAX_BLOCKNUMBER_MODE3 = 10,  /*!< Enum value EMREAD_REQ_MAX_BLOCKNUMBER_MODE3. */
    EMREAD_RES_TOTALBLOCK_MODE3  = 10,      /*!< Enum value EMREAD_RES_TOTALBLOCK_MODE3. */
}mlsEMRead_t;


/*! \enum mlsEMWrite_t
    The information of EM Write
*/
typedef enum
{
    EMWRITE_RES_BLOCKNUMBER = 1,            /*!< Enum value EMWRITE_RES_BLOCKNUMBER. */
    EMWRITE_RES_TOTALBLOCK = 1,             /*!< Enum value EMWRITE_RES_TOTALBLOCK. */
    EMWRITE_RES_DATALEN = 1,                /*!< Enum value EMWRITE_RES_DATALEN. */
    EMWRITE_RES_DATABLOCK0 = 4,             /*!< Enum value EMWRITE_RES_DATABLOCK0. */
}mlsEMWrite_t;


/*! \enum mlsDoDataLogging_t
    The information of Do data logging
*/
typedef enum
{   
    DATA_LOGGING_RES_ADC_DATALEN                    = 201,  /*!< Enum value DATA_LOGGING_RES_ADC_DATALEN. */
    DATA_LOGGING_RES_ADC_DATALEN_FIRST_BLOCK        = 249,  /*!< Enum value DATA_LOGGING_RES_ADC_DATALEN_FIRST_BLOCK. */
    DATA_LOGGING_RES_MEASUREMENT_DATALEN            = 21,   /*!< Enum value DATA_LOGGING_RES_MEASUREMENT_DATALEN. */
    DATA_LOGGING_RES_MEASUREMENT_DATALEN_LONG       = 41,   /*!< Enum value DATA_LOGGING_RES_MEASUREMENT_DATALEN_LONG */
    DATA_LOGGING_RES_ADC_TOTALBLOCK                 = 200,  /*!< Enum value DATA_LOGGING_RES_ADC_TOTALBLOCK. */
    DATA_LOGGING_RES_ADC_MIN_BLOCKNUMBER            = 1,    /*!< Enum value DATA_LOGGING_RES_ADC_MIN_BLOCKNUMBER. */
    DATA_LOGGING_RES_ADC_MAX_BLOCKNUMBER            = 200,  /*!< Enum value DATA_LOGGING_RES_ADC_MAX_BLOCKNUMBER. */

    DATA_LOGGING_RES_MEASUREMENT_MIN_BLOCKNUMBER    = 1,    /*!< Enum value DATA_LOGGING_RES_MEASUREMENT_MIN_BLOCKNUMBER. */
    DATA_LOGGING_RES_MEASUREMENT_MAX_BLOCKNUMBER    = 20,   /*!< Enum value DATA_LOGGING_RES_MEASUREMENT_MAX_BLOCKNUMBER. */

    DATA_LOGGING_MIN_BLOCKNUMBER                    = 1,    /*!< Enum value DATA_LOGGING_MIN_BLOCKNUMBER. */
    DATA_LOGGING_ADC_TOTALBLOCK                     = 20,   /*!< Enum value DATA_LOGGING_ADC_TOTALBLOCK. */
    DATA_LOGGING_MEASUREMENT_TOTALBLOCK             = 10,   /*!< Enum value DATA_LOGGING_MEASUREMENT_TOTALBLOCK. */
    DATA_LOGGING_MAX_SUPPORT_CHANNEL                = 7,    /*!< Enum value DATA_LOGGING_MAX_SUPPORT_CHANNEL. */
    DATA_LOGGING_REQ_SAMPLE_LENGTH                  = 2,    /*!< Enum value DATA_LOGGING_REQ_SAMPLE_LENGTH. */
    DATA_LOGGING_REQ_NORMAL_DATA_LEN                =   DATA_LOGGING_MAX_SUPPORT_CHANNEL + 
                                                        DATA_LOGGING_REQ_SAMPLE_LENGTH,    /*!< Enum value DATA_LOGGING_MAX_SUPPORT_CHANNEL. */
    DATA_LOGGING_REQ_NO_SAMPLE_DATA_LEN             = DATA_LOGGING_MAX_SUPPORT_CHANNEL, 
}mlsDoDataLogging_t;


/*! \enum mlsAutoCalibration_t
    The information of Auto Calibration
*/
typedef enum
{
    AUTOCALIBRATION_RES_BLOCKNUMBER = 1,                /*!< Enum value AUTOCALIBRATION_RES_BLOCKNUMBER. */
    AUTOCALIBRATION_RES_TOTALBLOCK = 1,                 /*!< Enum value AUTOCALIBRATION_RES_TOTALBLOCK. */
    AUTOCALIBRATION_RES_DATALEN_ID1 = 70,               /*!< Enum value AUTOCALIBRATION_RES_DATALEN_ID1. */
    AUTOCALIBRATION_RES_DATALEN_ID2 = 2,                /*!< Enum value AUTOCALIBRATION_RES_DATALEN_ID2. */

}mlsAutoCalibration_t;

#ifdef __cplusplus
}
#endif

#endif /*__MLSCIFCONFIG_H__  */
/** @} */
