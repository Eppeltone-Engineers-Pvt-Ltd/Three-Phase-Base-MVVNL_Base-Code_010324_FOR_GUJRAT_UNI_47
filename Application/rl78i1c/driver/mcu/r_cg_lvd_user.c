/***********************************************************************************************************************
* File Name    : r_lvd_user.c
* Version      : 
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for LVD module.
* Creation Date: 5/7/2015
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/

/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_lvd.h"
/* Start user code for include. Do not edit comment generated here */
#include "platform.h"
#include "powermgmt.h"
/* End user code. Do not edit comment generated here */
#include "appInterrupts.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_lvd_interrupt
* Description  : This function is INTLVI interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//#pragma interrupt r_lvd_interrupt(vect=INTLVI)
//static void r_lvd_interrupt(void)
void r_lvd_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    //R_Wrapper_SignalDisplay();

    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_lvdvdd_interrupt
* Description  : This function is INTLVDVDD interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//#pragma interrupt r_lvdvdd_interrupt(vect=INTLVDVDD)
//static void r_lvdvdd_interrupt(void)
void r_lvdvdd_interrupt(void)
{// this fellow monitors the VDD pin - LVI monitors internal VDD - huge difference if battery backup is used
    // Start user code. Do not edit comment generated here 
    EI();

    POWERMGMT_VDD_InterruptCallback();
    
    // End user code. Do not edit comment generated here
}

/***********************************************************************************************************************
* Function Name: r_lvdvbat_interrupt
* Description  : This function is INTLVDVBAT interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//#pragma interrupt r_lvdvbat_interrupt(vect=INTLVDVBAT)
//static void r_lvdvbat_interrupt(void)
void r_lvdvbat_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();

    POWERMGMT_VBATT_InterruptCallback();

    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_lvdvrtc_interrupt
* Description  : This function is INTLVDVRTC interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//#pragma interrupt r_lvdvrtc_interrupt(vect=INTLVDVRTC)
//static void r_lvdvrtc_interrupt(void)
void r_lvdvrtc_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    POWERMGMT_VRTC_InterruptCallback();

    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_lvdexlvd_interrupt
* Description  : This function is INTLVDEXLVD interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//#pragma interrupt r_lvdexlvd_interrupt(vect=INTLVDEXLVD)
//static void r_lvdexlvd_interrupt(void)
void r_lvdexlvd_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    
    POWERMGMT_AC_InterruptCallback();

    /* End user code. Do not edit comment generated here */
}
/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
