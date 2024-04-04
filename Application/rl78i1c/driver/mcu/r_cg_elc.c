/***********************************************************************************************************************
* File Name    : r_cg_elc.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for ELC module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_elc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_ELC_Create
* Description  : This function initializes the ELC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ELC_Create(void)
{
    ELSELR00 = _00_ELC_EVENT_LINK_OFF;
    ELSELR01 = _00_ELC_EVENT_LINK_OFF;
    ELSELR02 = _00_ELC_EVENT_LINK_OFF;
    ELSELR03 = _00_ELC_EVENT_LINK_OFF;
    ELSELR04 = _00_ELC_EVENT_LINK_OFF;
    ELSELR05 = _00_ELC_EVENT_LINK_OFF;
    ELSELR06 = _00_ELC_EVENT_LINK_OFF;
    ELSELR07 = _00_ELC_EVENT_LINK_OFF;
    ELSELR08 = _00_ELC_EVENT_LINK_OFF;
    ELSELR09 = _00_ELC_EVENT_LINK_OFF;
    ELSELR10 = _00_ELC_EVENT_LINK_OFF;
    ELSELR11 = _00_ELC_EVENT_LINK_OFF;
    ELSELR12 = _00_ELC_EVENT_LINK_OFF;
    ELSELR13 = _00_ELC_EVENT_LINK_OFF;
    ELSELR14 = _00_ELC_EVENT_LINK_OFF;
    ELSELR15 = _00_ELC_EVENT_LINK_OFF;
    ELSELR16 = _00_ELC_EVENT_LINK_OFF;
    ELSELR17 = _00_ELC_EVENT_LINK_OFF;
    ELSELR18 = _00_ELC_EVENT_LINK_OFF;
    ELSELR19 = _00_ELC_EVENT_LINK_OFF;
    ELSELR20 = _00_ELC_EVENT_LINK_OFF;
    ELSELR21 = _00_ELC_EVENT_LINK_OFF;
}

/***********************************************************************************************************************
* Function Name: R_ELC_Stop
* Description  : This function stops the ELC event resources.
* Arguments    : src -
*                    event resources to be stopped (ELSELRn)
* Return Value : None
***********************************************************************************************************************/
void R_ELC_Stop(uint32_t src)
{
    volatile uint32_t   w_count;
    uint8_t           * sfr_addr;

    sfr_addr = (uint8_t *)&ELSELR00;
    
    for (w_count = 0U; w_count < ELC_TRIGGER_SRC_COUNT; w_count++) 
    {
        if (((src >> w_count) & 0x1U) == 0x1U)
        {
            *sfr_addr = _00_ELC_EVENT_LINK_OFF;
        }
        
        sfr_addr++;
    }
}

/* Start user code for adding. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_ELC_Set
* Description  : This function set the ELC event link.
* Arguments    : src -
*                    event resources to be stoped (ELSELRn)
*              : dest - 
*                    event destination
* Return Value : None
***********************************************************************************************************************/
void R_ELC_Set(elc_src_t src, uint8_t dest)
{
    uint8_t * sfr_addr = (uint8_t *)(&ELSELR00 + src);
    *sfr_addr = dest;
}

/***********************************************************************************************************************
* Function Name: R_ELC_Clear
* Description  : This function clear the ELC event link.
* Arguments    : src -
*                    event resources to be stoped (ELSELRn)
* Return Value : None
***********************************************************************************************************************/
void R_ELC_Clear(elc_src_t src)
{
    uint8_t  *sfr_addr = (uint8_t *)(&ELSELR00 + src);
    *sfr_addr = _00_ELC_EVENT_LINK_OFF;
}

/* End user code. Do not edit comment generated here */
