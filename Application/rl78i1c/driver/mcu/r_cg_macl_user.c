/***********************************************************************************************************************
* File Name    : r_cg_macl_user.c
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
#ifdef NO_BOOT_CODE
#pragma interrupt INTMACLOF r_macl_interrupt
#endif
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
#include "appInterrupts.h"
/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint8_t g_macl_interrupt_jmp = 0;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_dsadc_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_macl_interrupt(void)
void r_macl_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    g_macl_interrupt_jmp = 1;
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
