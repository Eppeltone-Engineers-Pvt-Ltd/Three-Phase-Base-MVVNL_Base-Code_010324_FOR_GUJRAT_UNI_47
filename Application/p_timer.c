#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "p_timer.h"
#include "powermgmt.h"
#include "r_dlms_data_meter.h"	// for DLMS
#include "em_display.h"
#include "emeter3_structs.h"
#include "appInterrupts.h"
extern uint8_t rtcTestFlag;

#ifdef NO_BOOT_CODE
#pragma interrupt INTIT00 p_timer_interrupt
#endif
// Either the p_timer_interrupt or the r_rtc_rpd_interrupt will be enabled at a time

//static void p_timer_interrupt(void)
void p_timer_interrupt(void)
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
	
//	if(rtcFailed!=0)
//		pravakSoftRTC();	// call the software rtc function
}

// This function creates a 16-bit timer which generates a 1-second interrupt
void P_TIMER_Create(void)
{

  /* Disable interrupts */
	ITMK00 = 1U;	// set the mask
	ITIF00 = 0U;	// clear the flag	
	
	
	TRTCR0 = 0x80;	// 1001 0000	- 16 bit operation
	TRTCR0|= 0x10;	// 0001 0000	-	Clock is supplied
	
	TRTMD0 = 0;	// Use fIL
	
	TRTCMP0 = 15000;	// this should give a one second interrupt for 15Khz
	
  /* Set interrupt priority low level */
	ITPR000 = 1U;
	ITPR100 = 1U;
	
}

void P_TIMER_Start(void)
{
  /* Enable interrupts */
  ITMK00 = 0U;    
	TRTCR0|=0x01;	// Start counting
}

void P_TIMER_Stop(void)
{
	TRTCR0&=~0x01;	// Stop counting
  /* Disable interrupts */
	ITMK00 = 1U;	// set the mask
	ITIF00 = 0U;	// clear the flag	
}
