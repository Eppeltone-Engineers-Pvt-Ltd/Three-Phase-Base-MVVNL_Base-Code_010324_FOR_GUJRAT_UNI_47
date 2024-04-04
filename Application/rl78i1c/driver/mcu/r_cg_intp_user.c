/***********************************************************************************************************************
* File Name    : r_cg_intp_user.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTP module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#ifdef NO_BOOT_CODE
#pragma interrupt INTP0 r_intc0_interrupt
#pragma interrupt INTP1 r_intc1_interrupt
#pragma interrupt INTP2 r_intc2_interrupt
#pragma interrupt INTP3 r_intc3_interrupt
#pragma interrupt INTP4 r_intc4_interrupt
#pragma interrupt INTP5 r_intc5_interrupt
#pragma interrupt INTP6 r_intc6_interrupt
#pragma interrupt INTP7 r_intc7_interrupt
#endif
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intp.h"
/* Start user code for include. Do not edit comment generated here */
#include "platform.h"
#include "key.h"
#include "event.h"
#include "powermgmt.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "appInterrupts.h"
/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_intc0_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_intc0_interrupt(void)
void r_intc0_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    POWERMGMT_KEY_InterruptCallback();	// added - pravak
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_intc1_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_intc1_interrupt(void)
void r_intc1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
#ifdef POWER_MANAGEMENT_ENABLE  
    /* Magnetic event */
    POWERMGMT_MAGNETIC_InterruptCallback();
#endif    

    /* Magnetic event */
    EVENT_Magnetic(EM_EXT_CALLBACK_STATE_ENTER);
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


//----------------------------------------------------------------------------------------------------------------------
//__interrupt static void r_intc2_interrupt(void)
void r_intc2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    
    /* End user code. Do not edit comment generated here */
}

//----------------------------------------------------------------------------------------------------------------------
//__interrupt static void r_intc3_interrupt(void)
void r_intc3_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    
    /* End user code. Do not edit comment generated here */
}

//----------------------------------------------------------------------------------------------------------------------
//__interrupt static void r_intc4_interrupt(void)
void r_intc4_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    
    /* End user code. Do not edit comment generated here */
}
//----------------------------------------------------------------------------------------------------------------------
//__interrupt static void r_intc5_interrupt(void)
void r_intc5_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    
    /* End user code. Do not edit comment generated here */
}
//__interrupt static void r_intc6_interrupt(void)
void r_intc6_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    
    /* End user code. Do not edit comment generated here */
}
//__interrupt static void r_intc7_interrupt(void)
void r_intc7_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    
    /* End user code. Do not edit comment generated here */
}
