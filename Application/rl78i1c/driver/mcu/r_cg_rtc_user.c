/***********************************************************************************************************************
* File Name    : r_cg_rtc_user.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for RTC module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#ifdef NO_BOOT_CODE
#pragma interrupt INTRTCRPD r_rtc_rpd_interrupt
#pragma interrupt INTRTCIC0 r_rtc_time_capture0
#pragma interrupt INTRTCIC1 r_rtc_time_capture1
#pragma interrupt INTRTCIC2 r_rtc_time_capture2
#endif
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_rtc.h"
/* Start user code for include. Do not edit comment generated here */
#include "platform.h"
#include "wrp_em_rtc.h"
#include "led.h"
#include "em_display.h"
#include "storage.h"
#include "r_calib.h"
#include "event.h"
#include "key.h"
#include "powermgmt.h"
#include "r_dlms_data_meter.h"	// for DLMS
#include "emeter3_structs.h"
#include "pravakComm.h"	// for USE_DLMS definition
#include "appInterrupts.h"
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
//volatile rtc_timecapture_value_t g_rtc_timestamp0;
//volatile rtc_timecapture_value_t g_rtc_timestamp1;	// this is for cover interrupt
//volatile rtc_timecapture_value_t g_rtc_timestamp2;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_rtc_prd_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//static void r_rtc_rpd_interrupt(void)
void r_rtc_rpd_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	powermgmt_mode_t    power_mode;
	
	EI();	

	rtcTestCounter++;
	if(rtcTestCounter>59)
		rtcTestCounter=0;

	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE3)
	{
		EM_RTC_DisplayInterruptCallback();
		if(sleepCommMode!=0)
		{
			sleepCommModeTimer++;
			if(sleepCommModeTimer>120)
			{
				sleepCommModeTimer=0;
				sleepCommMode=0;
				turnOff_UARTS();
			}
		}
		CommunicationTimeoutCheck();
	}
	else
	{
    EI();
#ifdef USE_DLMS	
	R_OBIS_Timer_ConstInterruptCallback();	// for DLMS
#endif

    if (R_CALIB_IsInCalibMode() == 1)
    {
        EM_CALIB_RTCConstantInterruptCallback();
    }
    
    POWERMGMT_RTC_InterruptCallback();
    
    //STORAGE_RtcCallback();
	}
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_rtc_time_capture0
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//static void r_rtc_time_capture0(void)
void r_rtc_time_capture0(void)
{
    /* Start user code. Do not edit comment generated here */
//    R_RTC_Get_TimeCapture0Value((rtc_timecapture_value_t * )&g_rtc_timestamp0);
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_rtc_time_capture1
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//static void r_rtc_time_capture1(void)
void r_rtc_time_capture1(void)
{
    /* Start user code. Do not edit comment generated here */
//    R_RTC_Get_TimeCapture1Value((rtc_timecapture_value_t * )&g_rtc_timestamp1);	// for some reason the time was not captured
    
#ifdef POWER_MANAGEMENT_ENABLE
    POWERMGMT_COVER_InterruptCallback();
#endif  
//    EVENT_CaseOpen(EM_EXT_CALLBACK_STATE_ENTER);		// commented - pravak
    
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_rtc_time_capture2
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//static void r_rtc_time_capture2(void)
void r_rtc_time_capture2(void)
{
    /* Start user code. Do not edit comment generated here */
//    R_RTC_Get_TimeCapture2Value((rtc_timecapture_value_t * )&g_rtc_timestamp2);


// following three lines - commented pravak
//#ifdef POWER_MANAGEMENT_ENABLE
//    POWERMGMT_KEY_InterruptCallback();
//#endif
//    KEY_UpPressed();
    
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
