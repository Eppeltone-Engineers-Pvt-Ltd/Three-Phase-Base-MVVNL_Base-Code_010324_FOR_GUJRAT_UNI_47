/***********************************************************************************************************************
* File Name    : r_cg_macl.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for MACL module.
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
#include "r_cg_macl.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_MACL_Create
* Description  : This function initializes the MACL module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MACL_Create(void)
{
    MACEN = 1U;     /* Supply MACL clock */
    
    MACMK = 1U;     /* disable the mac overflow interrupt */    
    MACIF = 0U;     /* clear the mac overflow interrupt flag */

    /*Set INTMACLOF low priority */
    MACPR0 = 1U;
    MACPR1 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_MACL_EnableInterrupt
* Description  : This function enable the MACL module interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MACL_EnableInterrupt(void)
{
    MACIF = 0U;     /* clear the mac overflow interrupt flag */
    MACMK = 0U;     /* enable the mac overflow interrupt */
}

/***********************************************************************************************************************
* Function Name: R_MACL_DisableInterrupt
* Description  : This function disable the MACL module interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MACL_DisableInterrupt(void)
{
    MACMK = 1U;     /* disable the mac overflow interrupt */
    MACIF = 0U;     /* clear the mac overflow interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
