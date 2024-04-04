/****************************************************************************** 
* File Name    : wrp_user_iic.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5d
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : IIC Wrapper Layer Source File
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* Macro Driver Definitions */
#include "r_cg_iica.h"      /* Serial Device Driver */

/* Wrapper/User */
#include "wrp_user_iic.h"       /* Wrapper IIC header */

/* Application */
#include "eeprom.h"             /* EEPROM */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Macro Definitions Checking
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
* Function Name    : void WRP_IIC_Init(void)
* Description      : IIC Initialization
* Arguments        : None
* Functions Called : WRP_IIC_DRIVER_Init()
* Return Value     : None
******************************************************************************/
void WRP_IIC_Init(void)
{
    WRP_IIC_DRIVER_Init();
}

/******************************************************************************
* Function Name    : uint8_t WRP_IIC_SendStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
* Description      : IIC Send Start
* Arguments        : uint8_t device_addr: Device address
*                  : uint8_t *buffer: Buffer to send
*                  : uint16_t length: Buffer length
* Functions Called : WRP_IIC_DRIVER_SendStart()
* Return Value     : Execution Status
*                  :    WRP_IIC_OK      Normal end
*                  :    WRP_IIC_ERROR   Error occurred
******************************************************************************/
uint8_t WRP_IIC_SendStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
{
    if (WRP_IIC_DRIVER_SendStart(device_addr, buffer, length) != MD_OK)
    {
        return WRP_IIC_ERROR;
    }
    else
    {
        return WRP_IIC_OK;
    }
}

/******************************************************************************
* Function Name    : uint8_t WRP_IIC_ReceiveStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
* Description      : IIC Recieve Start
* Arguments        : uint8_t device_addr: Device address
*                  : uint8_t *buffer: Buffer to receive
*                  : uint16_t length: Buffer length
* Functions Called : WRP_IIC_DRIVER_ReceiveStart()
* Return Value     : Execution Status
*                  :    WRP_IIC_OK      Normal end
*                  :    WRP_IIC_ERROR   Error occurred
******************************************************************************/
uint8_t WRP_IIC_ReceiveStart(uint8_t device_addr, uint8_t *buffer, uint16_t length)
{
    if (WRP_IIC_DRIVER_ReceiveStart(device_addr, buffer, length) != MD_OK)
    {
        return WRP_IIC_ERROR;
    }
    else
    {
        return WRP_IIC_OK;
    }
}

/******************************************************************************
* Function Name    : void WRP_IIC_SendEndCallback(void)
* Description      : IIC Wrapper Send End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_IIC_SendEndCallback(void)
{
    /* Put application callback here */
    EPR_SendEndCallback();      /* EEPROM */
}

/******************************************************************************
* Function Name    : void WRP_IIC_ReceiveEndCallback(void)
* Description      : IIC Wraper Receive End Callback
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void WRP_IIC_ReceiveEndCallback(void)
{
    /* Put application callback here */
    EPR_ReceiveEndCallback();   /* EEPROM */
}

