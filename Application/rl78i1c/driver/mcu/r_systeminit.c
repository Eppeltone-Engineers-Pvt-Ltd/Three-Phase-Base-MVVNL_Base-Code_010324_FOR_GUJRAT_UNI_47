/***********************************************************************************************************************
* File Name    : r_systeminit.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements system initializing function.
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
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_sau.h"
#include "r_cg_dsadc.h"
#include "r_cg_tau.h"
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "r_cg_lcd.h"
#include "r_cg_intp.h"
#include "r_cg_macl.h"
#include "r_cg_adc.h"
#include "r_cg_dtc.h"
#include "r_cg_elc.h"
#include "r_cg_vbatt.h"
#include "r_cg_iica.h"
#include "r_cg_lvd.h"
#include "p_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_mcu.h"
#include "startup.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

//extern uint8_t rtcFailed;
extern uint8_t rtcTestFlag;

/***********************************************************************************************************************
* Function Name: R_Systeminit
* Description  : This function initializes every macro.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_Systeminit(void)
{
    MCU_Delay(50000);   /* Delay 50ms */
//    MCU_Delay(2000000);   /* Delay 2000ms */
    
    R_CGC_Create();
    R_LCD_Create();
    R_PORT_Create();
//    rtcFailed = R_RTC_Create();
    R_RTC_Create();	// this must never fail since RTC_SUPPLY is being made 1 always
		P_TIMER_Create();
    R_SAU0_Create();
    R_SAU1_Create();
    R_TAU0_Create();
    R_DSADC_Create();
    R_INTC_Create();
    R_WDT_Create();
		R_LVD_Create();
//    R_LVDVDD_Create();
//    R_LVDVBAT_Create();
    R_LVDVRTC_Create();
    R_LVDEXLVD_Create();
//    R_INTC_Create();
    R_MACL_Create();
    R_ADC_Create();
    R_DTC_Create();
    R_ELC_Create();
//    R_IICA0_Create();
    R_VBATT_Create();
    
    CRC0CTL = 0x80U;
    IAWCTL = 0x80U;	// Invalid Memory Access Control register is enabled 
}


/***********************************************************************************************************************
* Function Name: hdwinit
* Description  : This function initializes hardware setting.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void hdwinit(void)
{
  DI();
  R_Systeminit();
  EI();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
