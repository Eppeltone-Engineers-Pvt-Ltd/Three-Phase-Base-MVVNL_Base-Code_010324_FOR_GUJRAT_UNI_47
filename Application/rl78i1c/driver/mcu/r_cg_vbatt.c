/***********************************************************************************************************************
* File Name    : r_cg_vbatt.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for battery backup module.
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
#include "r_cg_vbatt.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_VBATT_Create
* Description  : This function create battery backup function
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_VBATT_Create(void)
{
    /* Enable register write */
    BUPCTL1 = 0x80U;
    
    /* The supply source is depended on Vcc */
    VBATSEL = 0U;

    /* Disable register write */
    BUPCTL1 &= 0x7FU;
}

/***********************************************************************************************************************
* Function Name: R_VBATT_SetOperationOn
* Description  : This function enable battery backup function
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_VBATT_SetOperationOn(void)
{
    /* Enable register write */
    BUPCTL1 = 0x80U;
    
    /* Enable battery backup function */
    VBATEN = 0U;
    VBATEN = 1U;
    
    /* Disable register write */
    BUPCTL1 &= 0x7FU;
}

/***********************************************************************************************************************
* Function Name: R_VBATT_SetOperationOff
* Description  : This function disable battery backup function
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_VBATT_SetOperationOff(void)
{   
    /* Disable battery backup function */
    VBATEN = 1U;
    VBATEN = 0U;
}
/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
