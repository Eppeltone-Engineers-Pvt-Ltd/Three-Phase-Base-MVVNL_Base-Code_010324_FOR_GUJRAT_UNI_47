#include "r_cg_macrodriver.h"
#include "iodefine.h"
#include "r_cg_crc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

void R_CRC_Clear(void)
{
    CRCD = 0x0000;
}

uint16_t R_CRC_GetResult(void)
{
    return (CRCD);
}

void R_CRC_Calculate(uint8_t *buffer, uint16_t length)
{
    while (length > 0)
    {
        CRCIN = *buffer;
        
        buffer++;
        length--;
    }
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
