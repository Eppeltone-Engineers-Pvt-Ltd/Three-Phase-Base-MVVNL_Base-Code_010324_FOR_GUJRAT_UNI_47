/****************************************************************************** 
* File Name    : service_sfr.c
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
#include "typedef.h"        /* GSCE Typedef */
#include "iodefine.h"

/* Application */
#include "service.h"        /* Services */

/******************************************************************************
Macro definitions
******************************************************************************/
/*
 * Turn on if want to restrict the number of byte
 * to be read/write to 2 bytes only
 */
#define SFR_LIMIT_MANIPULATION_8_16        (1)

/*
 * For Read/Write SFR, I1B device
 * 
 * We limit the addr:
 *  - SFR  area, from 0xFFF00 to FFFFF
 *  - SFR2 area, from 0xF0000 to F07FF
 * parameter error will happen if out of range
 */
#define SFR_START_ADDR      (0x000FFF00)    /* SFR Start addr */
#define SFR_END_ADDR        (0x000FFFFF)    /* SFR End addr */
#define SFR2_START_ADDR     (0x000F0000)    /* SFR2 Start addr */
#define SFR2_END_ADDR       (0x000F07FF)    /* SFR2 End addr */

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
* Function Name : SERVICE_CheckSFRParams
* Interface     : static uint8_t SERVICE_CheckSFRParams(uint32_t addr, uint8_t *p_params->p_buff, uint8_t len)
* Description   : Check SFR area parameters
* Arguments     : uint32_t addr : Addr to read
*               : uint8_t * p_params->p_buff: Buffer to filled
*               : uint8_t len   : Number of byte to fill
* Function Calls: None
* Return Value  : static uint8_t, OK or not
*               :      SERVICE_PARAM_ERROR  : SFR Params Error
*               :      SERVICE_OK           : SFR Params Validation is OK
******************************************************************************/
static uint8_t SERVICE_CheckSFRParams(st_service_param *p_params)
{
    uint32_t end_addr;
    uint32_t addr;
    uint8_t len;
    
    /* Params check */
    if (p_params == NULL)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Check internal params pointer */
    if (    (p_params->p_buff_in == NULL && p_params->p_buff_out == NULL)           ||
            p_params->p_len_out == NULL             ||
            p_params->p_total_block == NULL     ||
            (0)
        )
    {
        return SERVICE_PARAM_ERROR;
    }
    
    addr = p_params->addr;
    len  = *p_params->p_len_out;
    
    if (addr == 0)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Check start addr */
    if ( (!(addr >= SFR_START_ADDR  && addr <= SFR_END_ADDR )) &&
         (!(addr >= SFR2_START_ADDR && addr <= SFR2_END_ADDR)) )
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Check addr+len-1 */
    if (len > 1)
    {
        end_addr = addr + len - 1;          
        if ( (!(end_addr >= SFR_START_ADDR  && end_addr <= SFR_END_ADDR )) &&
             (!(end_addr >= SFR2_START_ADDR && end_addr <= SFR2_END_ADDR)) )
        {
            return SERVICE_PARAM_ERROR;
        }
    }

#if (SFR_LIMIT_MANIPULATION_8_16 == 1)
    /* Check parameters */
    if (len != 1 && len != 2)
    {
        return SERVICE_PARAM_ERROR;
    }
#endif    
    
    /* Success */
    return SERVICE_OK;
}

/******************************************************************************
* Function Name : SERVICE_ReadSFR
* Interface     : uint8_t SERVICE_ReadSFR(uint32_t addr, uint8_t *buff, uint8_t len)
* Description   : Read SFR area
* Arguments     : uint32_t addr : Addr to read
*               : uint8_t * buff: Buffer to filled
*               : uint8_t len   : Number of byte to fill
* Function Calls: None
* Return Value  : uint8_t, success or not
******************************************************************************/
service_result_t SERVICE_ReadSFR(st_service_param *p_params)
{
    uint16_t reg;
    uint32_t addr;
    uint8_t len;
    uint8_t backup_vrtcen = VRTCEN;
    
    /* Check parameters */
    if (SERVICE_CheckSFRParams(p_params) != SERVICE_OK)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    addr = p_params->addr;
    len = *p_params->p_len_out;
    
    /* Set the total block: only 1 */
    *p_params->p_total_block = 1;
    
    VRTCEN = 1U;
    /*
     * All params passed,
     * fill to [buff], [len] byte
     */
#if (SFR_LIMIT_MANIPULATION_8_16 == 1)
    if (len == 1)
    {
        *p_params->p_buff_out = *((__far uint8_t *)addr);
        *p_params->p_len_out = 1;
    }
    else if (len == 2)
    {
        reg       = *((__far uint16_t *)addr);
        *(p_params->p_buff_out+0) = (uint8_t)(reg);
        *(p_params->p_buff_out+1) = (uint8_t)(reg >> 8);
    }
#else
    for (count = 0; count < len;)
    {
        UInt32_t count = 0;
        
        *p_params->p_buff_out = *((__far uint8_t *)addr);
        p_params->p_buff_out++;
        addr++;
        count++;
    }
#endif

    VRTCEN = backup_vrtcen;
    
    /* Read successful */
    return SERVICE_OK;
}

/******************************************************************************
* Function Name : SERVICE_WriteSFR
* Interface     : uint8_t SERVICE_WriteSFR(uint32_t addr, uint8_t *p_params->p_buff, uint8_t len)
* Description   : Write SFR area
* Arguments     : uint32_t addr : Addr to write
*               : uint8_t * p_params->p_buff: Buffer to fill SFR RAM
*               : uint8_t len   : Number of byte to fill
* Function Calls: None
* Return Value  : uint8_t, success or not
******************************************************************************/
service_result_t SERVICE_WriteSFR(st_service_param *p_params)
{
    uint16_t reg;
    uint32_t addr;
    uint8_t len;

    uint8_t backup_vrtcen = VRTCEN;
    
    /* Check parameters */
    if (SERVICE_CheckSFRParams(p_params) != SERVICE_OK)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    addr = p_params->addr;
    len = *p_params->p_len_out;
    
    /* Set the total block: only 1 */
    *p_params->p_total_block = 1;
    
    VRTCEN = 1U;
    /*
     * All params passed,
     * fill by [p_params->p_buff], [len] byte
     */
#if (SFR_LIMIT_MANIPULATION_8_16 == 1)   
    if (len == 1)
    {
        *((__far uint8_t *)addr) = *p_params->p_buff_in;
    }
    else if (len == 2)
    {
        reg  = (uint16_t)(*(p_params->p_buff_in+0));
        reg |= (uint16_t)(*(p_params->p_buff_in+1)) << 8;
        *((__far uint16_t *)addr) = reg;
    }
#else
    for (count = 0; count < len;)
    {
        if (len % 2 == 0)
        {
            reg  = (uint16_t)(*(p_params->p_buff_in+0));
            reg |= (uint16_t)(*(p_params->p_buff_in+1)) << 8;
            *((__far uint16_t *)addr) = reg;
            p_params->p_buff_in  += 2;
            addr  += 2;
            count += 2;
        }
        else
        {
            *((__far uint8_t *)addr) = *p_params->p_buff_in;
            p_params->p_buff_in++;
            addr++;
            count++;
        }
    }
#endif

    VRTCEN = backup_vrtcen;
    /* Write successful */
    return SERVICE_OK;
}
