/****************************************************************************** 
* File Name    : eeprom.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CubeSuite Version 1.5
* H/W Platform : Unified Energy Meter Platform
* Description  : EEPROM MW Layer APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* MD Macro Driver */

/* Code Standard */
#include "typedef.h"            /* GSCE Standard Typedef */

/* Wrapper */
#include "wrp_user_iic.h"       /* Wrapper/User IIC */
#include "wrp_mcu.h"            /* Wrapper/Core MCU */

/* Application */
#include "eeprom.h"             /* EEPROM MW Layer */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
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
static uint8_t g_is_send_end    = 0;
static uint8_t g_is_receive_end = 0;

/******************************************************************************
* Function Name: void EPR_Init(void)
* Description  : EPR Initialization
* Arguments    : WRP_IIC_Init()
* Return Value : None
******************************************************************************/
void EPR_Init(void)
{
    /* Init EEPROM device interface mapping */
    WRP_IIC_Init();
    
    /* Clear all ack flag */
    g_is_send_end    = 0;
    g_is_receive_end = 0;
    
//    EPR_WRITE_PROTECT_ENABLE_STATEMENT; /* Enable Write Protect */
}

/******************************************************************************
* Function Name: uint8_t EPR_Read(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Read from eeprom
* Arguments    : addr: Local address in eeprom
*              : buf : Buffer to store the result
*              : size: Expected size want to read
* Return Value : Execution status
*              :    EPR_OK                  Normal end
*              :    EPR_ERROR_NO_RESPOND    Device does not respond
*              :    EPR_ERROR_SIZE          Expected size and address are not suitable
******************************************************************************/
uint8_t EPR_Read(uint32_t addr, uint8_t* buf, uint16_t size)
{
    uint32_t    timeout;
    uint8_t     device_addr;                /* Device address */
    uint8_t     local_addr[2];              /* EEPROM Local address */

    /* Check user buffer */
    if (buf == NULL)
    {
        return EPR_ERROR;   /* parameter error */
    }
    
    /* check the address */
    if (size == 0 ||
        addr + size > EPR_DEVICE_SIZE)
    {
        return EPR_ERROR_SIZE;
    }
    
    /* Get the address */
    device_addr = EPR_DEVICE_BUS_ADDRESS | (uint8_t)((addr >> 15) & 0x02);
    local_addr[0] = (uint8_t)((addr >> 8) & 0xFF);
    local_addr[1] = (uint8_t)(addr & 0xFF);
    
    /* Send the address (HIGH + LOW) */
    g_is_send_end = 0;
    if (WRP_IIC_SendStart(device_addr, local_addr, 2) != WRP_IIC_OK)
    {
        return EPR_ERROR;
    }
    timeout = EPR_SEND_ADDR_MAX_TIMEOUT * 1000;
    while (g_is_send_end == 0)
    {
        MCU_Delay(1);   /* 1us delay */
        
        timeout--;
        if (timeout == 0)
        {
            return EPR_ERROR_NO_RESPOND;
        }
    }
    
    /* Delay after stop operation: 10us (+1us tolerance of MCU_Delay)
     * (Stop Set-up time + Bus free time) */    
    MCU_Delay(11);
    
    /* Read the buffer */
    g_is_receive_end = 0;
    timeout = ((EPR_READ_MAX_TIMEOUT * 1000) / EPR_DEVICE_SIZE) * (size + 1);
    if (WRP_IIC_ReceiveStart(device_addr, buf, size) != WRP_IIC_OK)
    {
        return EPR_ERROR;
    }
    while (g_is_receive_end == 0)
    {
        MCU_Delay(1);   /* 1us delay */
        
        timeout--;
        if (timeout == 0)
        {
            return EPR_ERROR_NO_RESPOND;
        }
    }

    /* Delay after stop operation: 10us (+1us tolerance of MCU_Delay)
     * (Stop Set-up time + Bus free time) */
    MCU_Delay(11);
    
    return EPR_OK;  /* Read succesful */
}

/******************************************************************************
* Function Name: uint8_t EPR_Write(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Write to eeprom
* Arguments    : addr: Local address in eeprom
*              : buf : Buffer to write to eeprom
*              : size: Expected size (in buf) want to write
* Return Value : Execution status
*              :    EPR_OK                  Normal end
*              :    EPR_ERROR_NO_RESPOND    Device does not respond
*              :    EPR_ERROR_SIZE          Expected size and address are not suitable
******************************************************************************/
uint8_t EPR_Write(uint32_t addr, uint8_t* buf, uint16_t size)
{
    uint32_t    timeout;                                /* Timeout counter */
    uint16_t    page_size;                              /* Page size */
    uint8_t     device_addr;                            /* Device address */
    uint8_t     local_buffer[EPR_DEVICE_PAGESIZE+2];    /* EEPROM Local address + buffer */
    uint16_t    pos;
    
    /* Check user buffer */
    if (buf == NULL)
    {
        return EPR_ERROR;   /* parameter error */
    }
    
    /* Check the address */
    if (size == 0 ||
        addr + size > EPR_DEVICE_SIZE)
    {
        return EPR_ERROR_SIZE;
    }
    
//    EPR_WRITE_PROTECT_DISABLE_STATEMENT;    /* Disable write protect */
    /* Write each page with evaluated address, size */
    while (size > 0)
    {
        /* Get the compensator of 1 page size */
        page_size = EPR_DEVICE_PAGESIZE - (uint16_t)(addr % EPR_DEVICE_PAGESIZE);
        if (size < page_size)
        {
            page_size = size;
        }
        
        /* Get the address */
        device_addr = EPR_DEVICE_BUS_ADDRESS | (uint8_t)((addr >> 15) & 0x02);
        local_buffer[0] = (uint8_t)((addr >> 8) & 0xFF);
        local_buffer[1] = (uint8_t)(addr & 0xFF);
    
        /* Copy the page (from buf) to local_buffer
         * max. is EPR_DEVICE_PAGESIZE */
        for (pos = 0; pos < page_size; pos++)
        {
            local_buffer[pos + 2] = buf[pos];
        }
        
        /* Send the address (HIGH + LOW) + 1 page data */
        g_is_send_end = 0;
        if (WRP_IIC_SendStart(device_addr, local_buffer, page_size + 2) != WRP_IIC_OK)
        {
            return EPR_ERROR;
        }
        timeout = EPR_WRITE_MAX_TIMEOUT * 1000;
        while (g_is_send_end == 0)
        {
            MCU_Delay(1);   /* 1us delay */
        
            timeout--;
            if (timeout == 0)
            {
                return EPR_ERROR_NO_RESPOND;
            }
        }
        
        /* Delay after write cycle + stop operation */
        MCU_Delay((EPR_DEVICE_WRITE_CYCLE_TIME+1));
        
        /* Point to next page */
        addr += page_size;
        buf  += page_size;
        size -= page_size;
    }
    
//    EPR_WRITE_PROTECT_ENABLE_STATEMENT; /* Enable write protect */
    
    return EPR_OK;  /* Write succesful */
}


/******************************************************************************
* Function Name: void EPR_SendEndCallback(void)
* Description  : EPR Send End Callback
* Arguments    : None
* Return Value : None
******************************************************************************/
void EPR_SendEndCallback(void)
{
    g_is_send_end = 1;  
}

/******************************************************************************
* Function Name: void EPR_ReceiveEndCallback(void)
* Description  : EPR Receive End Callback
* Arguments    : None
* Return Value : None
******************************************************************************/
void EPR_ReceiveEndCallback(void)
{
    g_is_receive_end = 1;
}
