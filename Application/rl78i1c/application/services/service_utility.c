/****************************************************************************** 
* File Name    : service_utility.c
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

/* Global */
#include "typedef.h"            /* GSCE Typedef */

#include "service.h"

/******************************************************************************
* Function Name : SERVICE_EncodeFloat32
* Interface     : void SERVICE_EncodeFloat32(float32_t *p_f32, uint8_t *buff)
* Description   : Encode float32_t format
* Arguments     : float32_t *p_f32: Pointer to float32_t value
*               : uint8_t * buff  : Buffer to store the encoded buffer
* Function Calls: None
* Return Value  : None
******************************************************************************/
void SERVICE_EncodeFloat32(float32_t *p_f32, uint8_t *buff)
{
    uint8_t *p_u8;
    
    if (p_f32 == NULL || buff == NULL)
    {
        return;
    }
    
    p_u8 = (uint8_t *)p_f32;
    *(buff+0) = *(p_u8+0);
    *(buff+1) = *(p_u8+1);
    *(buff+2) = *(p_u8+2);
    *(buff+3) = *(p_u8+3);
}

/******************************************************************************
* Function Name : SERVICE_DecodeFloat32
* Interface     : void SERVICE_DecodeFloat32(float32_t *p_f32, uint8_t *buff)
* Description   : Encode float32_t format
* Arguments     : float32_t *p_f32: Pointer to store float32_t value
*               : uint8_t * buff  : Buffer of the encoded buffer
* Function Calls: None
* Return Value  : None
******************************************************************************/
void SERVICE_DecodeFloat32(float32_t *p_f32, uint8_t *buff)
{
    uint8_t *p_u8;
    
    if (p_f32 == NULL || buff == NULL)
    {
        return;
    }
    
    p_u8 = (uint8_t *)p_f32;
    *(p_u8+0) = *(buff+0);
    *(p_u8+1) = *(buff+1);
    *(p_u8+2) = *(buff+2);
    *(p_u8+3) = *(buff+3);
}


/******************************************************************************
* Function Name : SERVICE_EncodeUnsign16
* Interface     : SERVICE_EncodeUnsign16(uint16_t *p_u16, uint8_t *buff)
* Description   : Encode uint16_t format
* Arguments     : uint16_t *p_u16: Pointer to uint16_t value
*               : uint8_t * buff  : Buffer to store the encoded buffer
* Function Calls: None
* Return Value  : None
******************************************************************************/
void SERVICE_EncodeUnsign16(uint16_t *p_u16, uint8_t *buff)
{
    uint8_t *p_u8;
    
    if (p_u16 == NULL || buff == NULL)
    {
        return;
    }
    
    p_u8 = (uint8_t *)p_u16;
    *(buff+0) = *(p_u8+0);
    *(buff+1) = *(p_u8+1);
}

/******************************************************************************
* Function Name : SERVICE_DecodeUnsign16
* Interface     : SERVICE_DecodeUnsign16(uint16_t *p_u16, uint8_t *buff)
* Description   : Decode uint16_t format
* Arguments     : uint16_t *p_u16: Pointer to uint16_t value
*               : uint8_t * buff  : Buffer to store the encoded buffer
* Function Calls: None
* Return Value  : None
******************************************************************************/
void SERVICE_DecodeUnsign16(uint16_t *p_u16, uint8_t *buff)
{
    uint8_t *p_u8;
    
    if (p_u16 == NULL || buff == NULL)
    {
        return;
    }
    
    p_u8 = (uint8_t *)p_u16;
    *(p_u8+0) = *(buff+0);
    *(p_u8+1) = *(buff+1);
}

/******************************************************************************
* Function Name : SERVICE_EncodeFloat64
* Interface     : void SERVICE_EncodeFloat64(double64_t *p_d64, uint8_t *buff)
* Description   : Encode double64_t format
* Arguments     : double64_t *p_d64: Pointer to double64_t value
*               : uint8_t * buff  : Buffer to store the encoded buffer
* Function Calls: None
* Return Value  : None
******************************************************************************/
void SERVICE_EncodeFloat64(double64_t *p_d64, uint8_t *buff)
{
     uint8_t *p_u8;
    // 0xbfc00000
     if (p_d64 == NULL || buff == NULL)
     {
         return;
     }
    
     p_u8 = (uint8_t *)p_d64;

    *(buff+0) = *(p_u8+0);
    *(buff+1) = *(p_u8+1);
    *(buff+2) = *(p_u8+2);
    *(buff+3) = *(p_u8+3);
    *(buff+4) = *(p_u8+4);
    *(buff+5) = *(p_u8+5);
    *(buff+6) = *(p_u8+6);
    *(buff+7) = *(p_u8+7);
}


/******************************************************************************
* Function Name : SERVICE_DecodeFloat64
* Interface     : void SERVICE_DecodeFloat64(double64_t *p_d64, uint8_t *buff)
* Description   : Decode double64_t format
* Arguments     : double64_t *p_d64: Pointer to double64_t value
*               : uint8_t * buff  : Buffer to store the decoded buffer
* Function Calls: None
* Return Value  : None
******************************************************************************/
void SERVICE_DecodeFloat64(double64_t *p_d64, uint8_t *buff)
{
    uint8_t *p_u8;
    
    if (p_d64 == NULL || buff == NULL)
    {
        return;
    }
    
    p_u8 = (uint8_t *)p_d64;
    *(p_u8+0) = *(buff+0);
    *(p_u8+1) = *(buff+1);
    *(p_u8+2) = *(buff+2);
    *(p_u8+3) = *(buff+3);
    *(p_u8+4) = *(buff+4);
    *(p_u8+5) = *(buff+5);
    *(p_u8+6) = *(buff+6);
    *(p_u8+7) = *(buff+7);
}