/***********************************************************************************************************************
* File Name    : r_cg_tau_user.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for TAU module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#ifdef NO_BOOT_CODE
#pragma interrupt INTTM00 r_tau0_channel0_interrupt 
#pragma interrupt INTTM01 r_tau0_channel1_interrupt
#pragma interrupt INTTM02 r_tau0_channel2_interrupt
#pragma interrupt INTTM03 r_tau0_channel3_interrupt
#endif
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_em_timer.h"
#include "wrp_em_rtc.h"
#include "em_display.h"
#include "r_calib.h"
#include "command.h"
#include "service.h"
#include "mlsCITimer.h"
#include "DLMS_User.h"		// for DLMS
#include "r_dlms_data_meter.h"	// for DLMS
#include "powermgmt.h"

/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "pravakComm.h"	// for USE_DLMS definition
#include "key.h"	// for key 40 ms interrupt callback
#include "appInterrupts.h"

int8_t ms40;
int8_t addl_ms40;

extern int8_t energyIncrementationFlag;
//extern uint8_t rtcFailed;
extern uint8_t rtcTestFlag;

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_tau0_channel0_interrupt
* Description  : This function INTTM00 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_tau0_channel0_interrupt(void)
void r_tau0_channel0_interrupt(void)
{// this is a 1 ms interrupt
    // Start user code. Do not edit comment generated here
// The following block needs to be uncommented for GUI to work	
/*	
    #ifndef __DEBUG
    mlsSystemTimerCallback();
    #endif
*/
#ifdef USE_DLMS	
	PhysicalTimeoutCount();	// for DLMS
#endif	
// End user code. Do not edit comment generated here
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel1_interrupt
* Description  : This function INTTM01 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_tau0_channel1_interrupt(void)
void r_tau0_channel1_interrupt(void)
{// this is a 135 microsecond interrupt
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel2_interrupt
* Description  : This function INTTM02 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_tau0_channel2_interrupt(void)
void r_tau0_channel2_interrupt(void)
{
	// this is a 40 ms interrupt
    // Start user code. Do not edit comment generated here 
  EI();
	addl_ms40++;
	if(addl_ms40==25)
	{// one second is over
		addl_ms40=0;
		energyIncrementationFlag=1;
	}
  // End user code. Do not edit comment generated here 
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel3_interrupt
* Description  : This function INTTM03 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_tau0_channel3_interrupt(void)
void r_tau0_channel3_interrupt(void)
{// this is an exact 40 ms timer
  // Start user code. Do not edit comment generated here
	EI();
	ms40++;
	
	if(ms40==25)
	{// one second is over
		ms40=0;
    EM_RTC_ConstInterruptCallback();	// originally this was in r_cg_rtc_user.c
    EM_RTC_DisplayInterruptCallback();

/*
// this should never happen anymore
		if(rtcFailed!=0)
		{
			#ifdef USE_DLMS	
			R_OBIS_Timer_ConstInterruptCallback();	// for DLMS
			#endif

	    if (R_CALIB_IsInCalibMode() == 1)
	    {
	        EM_CALIB_RTCConstantInterruptCallback();
	    }
	    
	    POWERMGMT_RTC_InterruptCallback();
		}
*/
	}
	
	if (R_CALIB_IsInCalibMode() == 0)
  {
    EM_TIMER_InterruptCallback();
  }
	
	KEY_40msInterruptCallback();
  // End user code. Do not edit comment generated here
}



