/****************************************************************************** 
* File Name    : service_firmware.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */

/* Global */
#include "typedef.h"            /* GSCE Typedef */

/* Middleware */
#include "em_core.h"            /* EM Core Library */

/* Application */
#include "format.h"             /* EEPROM Format */
#include "eeprom.h"             /* EEPROM Driver */
#include "service.h"            /* Services */

/******************************************************************************
Macro definitions
******************************************************************************/
#define EM_FIRMWARE_MAJOR                   (0x01)          /* The major version set in the firmware */
#define EM_FIRMWARE_MINOR                   (0x00)          /* Minor version set in the firmware     */
#define EM_FIRMWARE_BUILD                   (0x00)          /* Build version set in the firmware     */
#define EM_FIRMWARE_REVISION                (0x00)          /* Revision in the form setting version  */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name : SERVICE_ReadFirmwareVersion
* Interface     : uint8_t SERVICE_ReadFirmwareVersion(
*               :     uint8_t *buff,
*               :     uint8_t len
*               : );
* Description   : Read Firmware Version Service
* Arguments     : uint8_t * buff   : buffer to be filled to
*               : uint8_t len      : Expected length to read
* Function Calls: TBD
* Return Value  : uint8_t, success or not
******************************************************************************/
service_result_t SERVICE_ReadFirmwareVersion(
    uint8_t *buff,          /* Pointer of buffer */
    uint8_t len             /* Length of buffer */
)
{   
    /* Params check, firmware version is 4 bytes */
    if (buff == NULL ||
        len != 4)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Buffer fill */
    /* TODO: use EEPROM to store the firmware version */
    *buff++ = EM_FIRMWARE_MAJOR;
    *buff++ = EM_FIRMWARE_MINOR;
    *buff++ = EM_FIRMWARE_BUILD;
    *buff++ = EM_FIRMWARE_REVISION;
    
    /* Successful */
    return SERVICE_OK;
}
