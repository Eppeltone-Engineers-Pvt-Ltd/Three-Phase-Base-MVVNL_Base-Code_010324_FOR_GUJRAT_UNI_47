/****************************************************************************** 
* File Name    : dataflash.c
* Version      : 1.00
* Device(s)    : RL78/G13
* Tool-Chain   : CubeSuite Version 1.5
* H/W Platform : Unified Energy Meter Platform
* Description  : DATAFLASH MW Layer APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* MD Macro Driver */
#include "r_cg_wdt.h"           /* MD WDT Driver */

/* Code Standard */
#include "typedef.h"            /* GSCE Standard Typedef */

/* Application */
#include "pfdl_types.h"         /* Data Flash Type */
#include "pfdl.h"               /* Data Flash Library */
#include "dataflash.h"          /* Data Flash Memory Application */

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
__near const pfdl_descriptor_t g_data_flash_descriptor = 
{
    24,     /* 24 MHz */
    0x00,   /* Full speed */
};

__near pfdl_request_t g_data_flash_req;

#define DATAFLASH_ASSIGN_ERASE_REQ(block) \
{ \
    g_data_flash_req.index_u16 = block; \
    g_data_flash_req.data_pu08 = 0; \
    g_data_flash_req.bytecount_u16 = 0; \
    g_data_flash_req.command_enu = PFDL_CMD_ERASE_BLOCK; \
};

#define DATAFLASH_ASSIGN_WRITE_REQ(addr, buffer, size) \
{ \
    g_data_flash_req.index_u16 = addr; \
    g_data_flash_req.data_pu08 = buffer; \
    g_data_flash_req.bytecount_u16 = size; \
    g_data_flash_req.command_enu = PFDL_CMD_WRITE_BYTES; \
};

#define DATAFLASH_ASSIGN_READ_REQ(addr, buffer, size) \
{ \
    g_data_flash_req.index_u16 = addr; \
    g_data_flash_req.data_pu08 = buffer; \
    g_data_flash_req.bytecount_u16 = size; \
    g_data_flash_req.command_enu = PFDL_CMD_READ_BYTES; \
};

/******************************************************************************
* Function Name: void DATAFLASH_Init(void);
* Description  : DATAFLASH Initialization
* Arguments    : None
* Return Value : uint8_t, execution status
******************************************************************************/
uint8_t DATAFLASH_Init(void)
{
    /*
     * Skip this initialization
     * Data flash is always ok when the device is startup
     */
    
    return DATAFLASH_OK;
}

/******************************************************************************
* Function Name: uint8_t DATAFLASH_Read(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Read from data flash
* Arguments    : addr: Local address in data flash
*              : buf : Buffer to store the result
*              : size: Expected size want to read
* Return Value : Execution status
*              :    DATAFLASH_OK            Normal end
*              :    DATAFLASH_ERROR         Parameter error
*              :    DATAFLASH_ERROR_SIZE    Expected size and address are not suitable
******************************************************************************/
uint8_t DATAFLASH_Read(uint32_t addr, uint8_t* buf, uint16_t size)
{
    pfdl_status_t ret;
    
    /* Check user buffer */
    if (buf == NULL)
    {
        return DATAFLASH_ERROR; /* parameter error */
    }
    
    /* check the address */
    if (size == 0 ||
        addr + size > DATAFLASH_DEVICE_SIZE)
    {
        return DATAFLASH_ERROR_SIZE;
    }
    
    /* Open data flash */
    ret = PFDL_Open((pfdl_descriptor_t *)&g_data_flash_descriptor);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
        
    /* Read whole block */
    DATAFLASH_ASSIGN_READ_REQ((uint16_t)addr, buf, size);
    ret = PFDL_Execute(&g_data_flash_req);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }

    /* Close data flash */
    PFDL_Close();
    
    return DATAFLASH_OK;    /* Read succesful */
}

/******************************************************************************
* Function Name: uint8_t DATAFLASH_Write(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Write to data flash
* Arguments    : addr: Local address in data flash
*              : buf : Buffer to write to data flash
*              : size: Expected size (in buf) want to write
* Return Value : Execution status
*              :    DATAFLASH_OK            Normal end
*              :    DATAFLASH_ERROR         Parameter error or data flash execution error
*              :    DATAFLASH_ERROR_SIZE    Expected size and address are not suitable
******************************************************************************/
uint8_t DATAFLASH_Write(uint32_t addr, uint8_t* buf, uint16_t size)
{
    pfdl_status_t   ret;
    uint8_t         block_buffer[1024]; /* data flash 1 block buffer */
        
    /* Check user buffer */
    if (buf == NULL)
    {
        return DATAFLASH_ERROR; /* parameter error */
    }
    
    /* check the address */
    if (size == 0 ||
        addr + size > DATAFLASH_DEVICE_SIZE)
    {
        return DATAFLASH_ERROR_SIZE;
    }
    
    /* Open data flash */
    ret = PFDL_Open((pfdl_descriptor_t *)&g_data_flash_descriptor);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
    
    /* Block by block processing */
    while (size > 0)
    {
        uint16_t block_number = (uint16_t)(addr / DATAFLASH_DEVICE_BLOCK_SIZE);
        uint16_t block_addr   = block_number * DATAFLASH_DEVICE_BLOCK_SIZE;
        uint16_t addr_1b      = (uint16_t)(addr % DATAFLASH_DEVICE_BLOCK_SIZE);
        uint16_t block_size;
        uint16_t i;
    
        /* Get the compensator of 1 block size */
        block_size = DATAFLASH_DEVICE_BLOCK_SIZE - addr_1b;
        if (size < block_size)
        {
            block_size = size;
        }
        
        /* Read whole block */
        DATAFLASH_Read(block_addr, block_buffer, DATAFLASH_DEVICE_BLOCK_SIZE);
        
        /* Erase that whole block */
        DATAFLASH_ASSIGN_ERASE_REQ(block_number);
        ret = PFDL_Execute(&g_data_flash_req);
        while (ret == PFDL_BUSY)
        {
            ret = PFDL_Handler();
        }
        if (ret != PFDL_OK)
        {
            return DATAFLASH_ERROR; /* data flash error */
        }
        
        /* Re-write that block with modified data on the buffer */
        for (i = 0; i < block_size; i++)
        {
            block_buffer[addr_1b + i] = *(buf + i);
        }
        DATAFLASH_ASSIGN_WRITE_REQ(block_addr, block_buffer, DATAFLASH_DEVICE_BLOCK_SIZE);
        ret = PFDL_Execute(&g_data_flash_req);
        while (ret == PFDL_BUSY)
        {
            ret = PFDL_Handler();
        }
        if (ret != PFDL_OK)
        {
            return DATAFLASH_ERROR; /* data flash error */
        }
        
        /* Point to next page */
        addr += block_size;
        buf  += block_size;
        size -= block_size;
        
        R_WDT_Restart();
    }
    
    /* Close data flash */
    PFDL_Close();
    
    return DATAFLASH_OK;    /* Write succesful */
}

/******************************************************************************
* Function Name: uint8_t DATAFLASH_WriteBlock(uint8_t blk_no, uint8_t* buf)
* Description  : Write buf to one block, 
* Arguments    : addr: Local address in data flash
*              : buf : Buffer to write to data flash
*              : size: Expected size (in buf) want to write
* Return Value : Execution status
*              :    DATAFLASH_OK            Normal end
*              :    DATAFLASH_ERROR         Parameter error or data flash execution error
*              :    DATAFLASH_ERROR_SIZE    Expected size and address are not suitable
******************************************************************************/
uint8_t DATAFLASH_WriteBlock(uint8_t blk_no, uint8_t* buf)
{
    pfdl_status_t   ret;
    uint16_t addr_1b;
    /* Check user buffer */
    if (buf == NULL)
    {
        return DATAFLASH_ERROR; /* parameter error */
    }
    
    /* Check block index, is out of range? */
    if (blk_no >=  (DATAFLASH_DEVICE_SIZE /DATAFLASH_DEVICE_BLOCK_SIZE))
    {
        return DATAFLASH_ERROR_SIZE;
    }
    
    /* Open data flash */
    ret = PFDL_Open((pfdl_descriptor_t *)&g_data_flash_descriptor);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
    
    /* Erase that whole block */
    DATAFLASH_ASSIGN_ERASE_REQ(blk_no);
    ret = PFDL_Execute(&g_data_flash_req);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
    
    /* Get the start address of block */
    addr_1b = (uint16_t)(blk_no * DATAFLASH_DEVICE_BLOCK_SIZE);
   
    DATAFLASH_ASSIGN_WRITE_REQ(addr_1b, buf, DATAFLASH_DEVICE_BLOCK_SIZE);
    ret = PFDL_Execute(&g_data_flash_req);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
    
    R_WDT_Restart();
    
    /* Close data flash */
    PFDL_Close();
    
    return DATAFLASH_OK;    /* Write succesful */
}

/******************************************************************************
* Function Name: uint8_t DATAFLASH_Format(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Write to data flash
* Arguments    : addr: Local address in data flash
*              : buf : Buffer to write to data flash
*              : size: Expected size (in buf) want to write
* Return Value : Execution status
*              :    DATAFLASH_OK            Normal end
*              :    DATAFLASH_ERROR         Parameter error or data flash execution error
*              :    DATAFLASH_ERROR_SIZE    Expected size and address are not suitable
******************************************************************************/
uint8_t DATAFLASH_Format(uint8_t blk_no)
{
    pfdl_status_t   ret;
    
    /* Check block index, is out of range? */
    if (blk_no >=  (DATAFLASH_DEVICE_SIZE /DATAFLASH_DEVICE_BLOCK_SIZE))
    {
        return DATAFLASH_ERROR_SIZE;
    }
    
    /* Open data flash */
    ret = PFDL_Open((pfdl_descriptor_t *)&g_data_flash_descriptor);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
    
    /* Erase that whole block */
    DATAFLASH_ASSIGN_ERASE_REQ(blk_no);
    ret = PFDL_Execute(&g_data_flash_req);
    while (ret == PFDL_BUSY)
    {
        ret = PFDL_Handler();
    }
    if (ret != PFDL_OK)
    {
        return DATAFLASH_ERROR; /* data flash error */
    }
    
    R_WDT_Restart();
    
    /* Close data flash */
    PFDL_Close();
    
    return DATAFLASH_OK;    /* Write succesful */
}
