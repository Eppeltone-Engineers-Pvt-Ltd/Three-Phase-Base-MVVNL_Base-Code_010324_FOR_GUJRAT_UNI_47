/***********************************************************************************************************************
* File Name    : r_cg_rtc.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for RTC module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_rtc.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_mcu.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "emeter3_structs.h"
#include "utilities.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
#define ABS(x)                  ((x) <  0  ? (-(x)) : (x))                          /* Get absolute value */
/* End user code. Do not edit comment generated here */


//extern uint8_t rtcFailed;
extern uint8_t rtcTestFlag;

/***********************************************************************************************************************
* Function Name: R_RTC_Create
* Description  : This function initializes the RTC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint8_t R_RTC_Create(void)
{
	volatile uint16_t loopCount;
	uint8_t retval=0;
	
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
		RCR5GD=0x00;
		RCR5GD=0x72;
		RCR5GD=0x64;
		RCR5 = 0x00;	// this value is what should noormally be
//		RCR5 = 0;	// VALUES of 2,4,8 seem to speed up the rtc count
		RCR5GD=0x00;
		
		
    /* Disable interrupts */
    RTCAMK = 1U;
    RTCAIF = 0U;
    RTCRMK = 1U;
    RTCRIF = 0U;
    
    /* Disable time capture interrupts */
    RTCIMK1 = 1U;
    RTCIIF1 = 0U;
    RTCIMK2 = 1U;
    RTCIIF2 = 0U;
    
    if (RTCPORSR == 1U)
    {
        /* If this is power on reset, init the time */
        /* Clock setting to use 32KHz source */
        RCR4 = _00_RTC_COUNT_SOURCE_32KHZ_CLOCK;
        
        /* Set sub-clock oscillator : RTCEN bit 0 */
        RCR3.0 = 1U;
				loopCount=0;
				do
				{
					loopCount++;
				}while ((RCR3.0 != 1U)&&(loopCount<30000));
        
				if((loopCount==30000)&&(RCR3.0 != 1U))
				{
					retval=1;
					return retval;
				}
				
        /* Stop all counters : START is bit 0 */
        RCR2.0 = 0U;
        while (RCR2.0 != 0U);
        
        /* Select count mode : CNTMD is bit 7 */
        RCR2.7 = 0U;
        while (RCR2.7 != 0U);

        /* Execute RTC software reset : RESET is bit 1 */
        RCR2.1 = 1U;
        while (RCR2.1 != 0U);
        
				/* Set the adjustment values   */
				RADJ = 0;	// make sure that PMADJ and ADJ are 0
				
				RADJ = 20;	// This will make ADJ = 20
				RADJ |=0x80;	// This will make PMADJ = 10b, meaning subtract
				
        /* Set control registers */
        RCR1 = _04_RTC_PERIOD_INT_ENABLE | _D0_RTC_PERIODIC_INT_PERIOD_1_2 | _00_RTC_RTCOUT_FREQ_1HZ;
//        RCR2 = _40_RTC_HOUR_MODE_24 | _00_RTC_RTCOUT_OUTPUT_DISABLE;
        RCR2 = _40_RTC_HOUR_MODE_24 | _08_RTC_RTCOUT_OUTPUT_ENABLE | _20_RTC_AUTO_ADJUSTMENT_PERIOD_10SEC |_10_RTC_AUTO_ADJUSTMENT_ENABLE;
    }
    
    /* Set RTCOUT as output port */
#ifdef PCB_2_5		
    PM15.0 = 0U; 
#else
    PM6.2 = 0U; 
#endif
    /* Set port related to RTCICn into input port */
    PM15.1 = 1U;
    PM15.2 = 1U;
    /* Set RTCICn falling edge detection into input port */
    EGP1 = 0x00;
    EGN1 = 0x60;
    /* Set time capture control registers */
//    RTCCR1 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _02_RTC_CAPTURE_EVENT_FALLING_EDGE; 
    RTCCR1 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _01_RTC_CAPTURE_EVENT_RISING_EDGE;
    RTCCR2 = _00_RTC_CAPTURE_EVENT_INPUT_DISABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _02_RTC_CAPTURE_EVENT_FALLING_EDGE; 
    RTCICNFEN = 0x77;   /* Noise filter enabled on all inputs - 500ms */

    /* Set ALM interrupt priority low level */
    RTCAPR1 = 1U;
    RTCAPR0 = 1U;
    /* Set RPD interrupt priority low level */
    RTCRPR1 = 1U;
    RTCRPR0 = 1U;
    
    /* Time capture event 1 interrupt priority low level */
    RTCIPR11 = 1U;  
    RTCIPR01 = 1U;
    /* Time capture event 2 interrupt priority low level */
    RTCIPR12 = 1U;  
    RTCIPR02 = 1U;
    
    RTCPORSR = 1U;
		
		return retval;
}

/***********************************************************************************************************************
* Function Name: R_RTC_Start
* Description  : This function starts RTC counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint8_t R_RTC_Start(void)
{
	volatile uint16_t loopCount;
	uint8_t retval=0;

    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Enable PRD interrupt */
    RTCRIF = 0U;    
    RTCRMK = 0U;

    /* Enable time capture interrupts */
    RTCIIF1 = 0U;   
    RTCIMK1 = 0U;
    RTCIIF2 = 0U;   
    RTCIMK2 = 0U;
    
    /* Start RTC counting */
    RCR2.0 = 1U;
		loopCount=0;
		do
		{
			loopCount++;
		}
		while ((RCR2.0 != 1U)&&(loopCount<30000));
		
		if((loopCount==30000)&&(RCR2.0 != 1U))
			retval=1;
			
		return retval;
}

/***********************************************************************************************************************
* Function Name: R_RTC_Stop
* Description  : This function stops RTC counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Stop(void)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Disable PRD interrupt */
    RTCRMK = 1U;
    RTCRIF = 0U;
    
    /* Disable time capture interrupts */
    RTCIMK1 = 1U;
    RTCIIF1 = 0U;
    RTCIMK2 = 1U;
    RTCIIF2 = 0U;
    
    /* Stop all counters */
    RCR2.0 = 0U;
    while (RCR2.0 != 0U);
}

/***********************************************************************************************************************
* Function Name: R_RTC_Restart
* Description  : This function restarts RTC counter.
* Arguments    : counter_write_val -
*                    counter write value
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Restart(rtc_calendarcounter_value_t counter_write_val)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Disable interrupts */
    RTCAMK = 1U;
    RTCAIF = 0U;
    RTCRMK = 1U;
    RTCRIF = 0U;

    /* Disable time capture interrupts */
    RTCIMK1 = 1U;
    RTCIIF1 = 0U;
    RTCIMK2 = 1U;
    RTCIIF2 = 0U;
    
    /* Stop all counters : START is bit 0 */
    RCR2.0 = 0U;
    while (RCR2.0 != 0U);

    /* Execute RTC software reset : RESET is bit 1 */
    RCR2.1 = 1U;
    while (RCR2.1 != 0U);
    
    /* Set control registers */
    RCR2 = _40_RTC_HOUR_MODE_24 | _08_RTC_RTCOUT_OUTPUT_ENABLE;
//    RCR2 = _40_RTC_HOUR_MODE_24 | _00_RTC_RTCOUT_OUTPUT_DISABLE;

    /* Set time capture control registers */
//    RTCCR1 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
    RTCCR1 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _01_RTC_CAPTURE_EVENT_RISING_EDGE;
    RTCCR2 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
    RTCICNFEN = 0x70;   /* Noise filter 250ms */
    
    /* Set counter values */
    RSECCNT = counter_write_val.rseccnt;
    RMINCNT = counter_write_val.rmincnt;
    RHRCNT = counter_write_val.rhrcnt;
    RWKCNT = counter_write_val.rwkcnt;
    RDAYCNT = counter_write_val.rdaycnt;
    RMONCNT = counter_write_val.rmoncnt;
    RYRCNT = counter_write_val.ryrcnt;

    /* Enable PRD interrupt */
    RTCRIF = 0U;    
    RTCRMK = 0U;
    
    /* Enable time capture interrupts */
    RTCIIF1 = 0U;   
    RTCIMK1 = 0U;
    RTCIIF2 = 0U;   
    RTCIMK2 = 0U;   

    /* Start RTC counting */
    RCR2.0 = 1U;
    while (RCR2.0 != 1U);
}

/***********************************************************************************************************************
* Function Name: R_RTC_Get_CalendarCounterValue
* Description  : This function get RTC calendar counter value.
* Arguments    : counter_read_val -
*                    counter read pointer
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Get_CalendarCounterValue(rtc_calendarcounter_value_t * const counter_read_val)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    counter_read_val->rseccnt = RSECCNT;
    counter_read_val->rmincnt = RMINCNT;
    counter_read_val->rhrcnt  = RHRCNT & 0xBF;
    counter_read_val->rwkcnt  = RWKCNT;
    counter_read_val->rdaycnt = RDAYCNT;
    counter_read_val->rmoncnt = RMONCNT;
    counter_read_val->ryrcnt  = RYRCNT;
}

/***********************************************************************************************************************
* Function Name: R_RTC_Set_CalendarCounterValue
* Description  : This function set RTC calendar counter value.
* Arguments    : counter_write_val -
*                    counter write value
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_CalendarCounterValue(rtc_calendarcounter_value_t counter_write_val)
{// all the values are in bcd
    uint16_t wcount;
    uint16_t d;
    uint16_t m;
    uint16_t y;
		
// before we do anything we should find the day of week		
		d=bcdToBin(counter_write_val.rdaycnt);
		m=bcdToBin(counter_write_val.rmoncnt);
		y=bcdToBin((uint8_t)counter_write_val.ryrcnt);	// in the rtc only 00 to 99 is stored
		y=2000+y;
		
		wcount = CalcDayNumFromDate(y,m,d);
		counter_write_val.rwkcnt=(uint8_t)wcount;

// stop the EM
		stopEM();	// located in utilities
		
// we are now set		
		
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Stop all counters : START is bit 0 */
    RCR2.0 = 0U;
    while (RCR2.0 != 0U);

    /* Execute RTC software reset : RESET is bit 1 */
    RCR2.1 = 1U;
    while (RCR2.1 != 0U);
    
    /* Set control registers */
//    RCR2 = _40_RTC_HOUR_MODE_24 | _08_RTC_RTCOUT_OUTPUT_ENABLE;
    RCR2 = _40_RTC_HOUR_MODE_24 | _00_RTC_RTCOUT_OUTPUT_DISABLE;
    RTCCR1 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
    RTCCR2 = _80_RTC_CAPTURE_EVENT_INPUT_ENABLE | _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV | _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
    RTCICNFEN = 0x70;   /* Noise filter 250ms */
    
    /* Set counter values */
    RSECCNT = counter_write_val.rseccnt;
    RMINCNT = counter_write_val.rmincnt;
    RHRCNT = counter_write_val.rhrcnt;
    RWKCNT = counter_write_val.rwkcnt;
    RDAYCNT = counter_write_val.rdaycnt;
    RMONCNT = counter_write_val.rmoncnt;
    RYRCNT = counter_write_val.ryrcnt;
    
    /* Restart counting : START is bit 0 */
    RCR2.0 = 1U;
    while (RCR2.0 != 1U);

// start the EM
		startEM();	// located in utilities

}

/******************************************************************************
* Function Name: R_RTC_Get_TimeCapture0Value
* Description  : Get RTC time capture value of input channel 0
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RTC_Get_TimeCapture0Value(rtc_timecapture_value_t * const counter_read_val)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    RTCCR0 &= 0xFC; /* Clear TCCT: no detection, to clear the TCST */
    while ((RTCCR0 & 0x03) != 0U);
    
    counter_read_val->rseccnt = RSECCP0;
    counter_read_val->rmincnt = RMINCP0;
    counter_read_val->rhrcnt  = RHRCP0 & 0xBF;
    counter_read_val->rdaycnt = RDAYCP0;
    counter_read_val->rmoncnt = RMONCP0;
    
    RTCCR0.2 = 0U;  /* Clear TCST */
    while (RTCCR0.2 != 0U);
    
    RTCCR0 |= _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
}

/******************************************************************************
* Function Name: R_RTC_Get_TimeCapture1Value
* Description  : Get RTC time capture value of input channel 1
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RTC_Get_TimeCapture1Value(rtc_timecapture_value_t * const counter_read_val)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    RTCCR1 &= 0xFC; /* Clear TCCT: no detection, to clear the TCST */
    while ((RTCCR1 & 0x03) != 0U);
    
    counter_read_val->rseccnt = RSECCP1;
    counter_read_val->rmincnt = RMINCP1;
    counter_read_val->rhrcnt  = RHRCP1 & 0xBF;
    counter_read_val->rdaycnt = RDAYCP1;
    counter_read_val->rmoncnt = RMONCP1;
    
    RTCCR1.2 = 0U;  /* Clear TCST */
    while (RTCCR1.2 != 0U);
    
    RTCCR1 |= _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
}

/******************************************************************************
* Function Name: R_RTC_Get_TimeCapture2Value
* Description  : Get RTC time capture value of input channel 2
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RTC_Get_TimeCapture2Value(rtc_timecapture_value_t * const counter_read_val)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    RTCCR2 &= 0xFC; /* Clear TCCT: no detection, to clear the TCST */
    while ((RTCCR2 & 0x03) != 0U);
    
    counter_read_val->rseccnt = RSECCP2;
    counter_read_val->rmincnt = RMINCP2;
    counter_read_val->rhrcnt  = RHRCP2 & 0xBF;
    counter_read_val->rdaycnt = RDAYCP2;
    counter_read_val->rmoncnt = RMONCP2;
    
    RTCCR2.2 = 0U;  /* Clear TCST */
    while (RTCCR2.2 != 0U);
    
    RTCCR2 |= _02_RTC_CAPTURE_EVENT_FALLING_EDGE;
}

/***********************************************************************************************************************
* Function Name: R_RTC_Set_ConstPeriodInterruptOn
* Description  : This function set RTC constant period interrupt and enable the interrupt(PRD).
* Arguments    : period -
*                    new period of constant period interrupt
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_ConstPeriodInterruptOn(rtc_int_period_t period)
{	
//	if(rtcFailed==0)
	{
    uint8_t set_value;
    
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Disable PRD interrupt */
    RTCRMK = 1U;
    RTCRIF = 0U;
    
    set_value = (uint8_t)(period | (RCR1 & 0x0FU) | _04_RTC_PERIOD_INT_ENABLE);
    
    RCR1 = set_value;
    while (RCR1 != set_value);
    RTCRMK = 0U;
	}
}

/***********************************************************************************************************************
* Function Name: R_RTC_Set_ConstPeriodInterruptOff
* Description  : This function disable RTC constant period interrupt(PRD).
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_ConstPeriodInterruptOff(void)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Disable PRD interrupt */
    RTCRMK = 1U;
    RTCRIF = 0U;
    RCR1.2 = 0U;
    while (RCR1.2 != 0U);
    
}

/***********************************************************************************************************************
* Function Name: R_RTC_Set_ClockOutputOn
* Description  : This function enable and set the frequency of clock output
* Arguments    : clk_out -
*                    new clock frequency output
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_ClockOutputOn(rtc_clock_out_t clk_out)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* Set RTCOS */
    if (clk_out == RTCOS_FREQ_1HZ)
    {
        RCR1.3 = 0;
    }
    else
    {
        RCR1.3 = 1;
    }
    /* RTCOE enable */
    RCR2.3 = 1;
}

/***********************************************************************************************************************
* Function Name: R_RTC_Set_ClockOutputOff
* Description  : This function disable RTC clock output
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_ClockOutputOff(void)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* RTCOE disable */
    RCR2.3 = 0;
}

/******************************************************************************
* Function Name: R_RTC_Compensate
* Description  : This function compensate the error on sub-clock by a provided ppm
* Arguments    : float ppm: ppm error of sub-clock crystal
* Return Value : None
*******************************************************************************/
void R_RTC_Compensate(float ppm)
{
    float   abs_ppm = ABS(ppm);
    uint8_t radj_reg = 0;

    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
        
    /* This feature only run on 32.768KHz clock source : RCKSEL is bit 0 */
    if (RCR4.0 == 0U)
    {
        /* Clear adjustment first : PMADJ bit [7:6] */
        while ((RADJ & 0xC0) != 0)
        {
            RADJ &= 0x3F;
        }
        
        /* Enable automatic adj : AADJE bit 4 */
        RCR2.4 = 1U;
        
        /* Enable sub or add */
        if (ppm > 0.0f)
        {
            radj_reg |= _80_RTC_TIMER_ERROR_ADJUST_MINUS;       /* sub */
        }
        else
        {
            radj_reg |= _40_RTC_TIMER_ERROR_ADJUST_PLUS;        /* add */
        }
        
        if (abs_ppm <= 32.043f)         /* 1 minutes adjustment */
        {
            RCR2.5 = 0U;                /* AADJP is bit 5 */
            radj_reg |= ( (uint8_t)(abs_ppm * (32768.0f * 60.0f / 1e6) + 0.5f) ) & 0x3F;
        }
        else if (abs_ppm <= 192.26f)    /* 10-second adjustment */
        {
            RCR2.5 = 1U;                /* AADJP is bit 5 */
            radj_reg |= ( (uint8_t)(abs_ppm * (32768.0f * 10.0f / 1e6) + 0.5f) ) & 0x3F;
        }
        else
        {
            /* Out of control !! no adj at all */
            RCR2.4 = 0;
            RCR2.5 = 0;
            radj_reg = 0;
        }
        
        while (RADJ != radj_reg)
        {
            RADJ = radj_reg;
        }
    }
}

/******************************************************************************
* Function Name: R_RTC_StopCompensation
* Description  : Stop the rtc compensation 
* Arguments    : None
* Return Value : None
*******************************************************************************/
void R_RTC_StopCompensation(void)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    /* AADJE bit 4, AADJP is bit 5 */
    RCR2.4 = 0;
    RCR2.5 = 0;
    
    while (RADJ != 0)
    {
        RADJ = 0;
    }
}

/***********************************************************************************************************************
* Function Name: R_RTC_Set_AlarmValue
* Description  : This function sets alarm value.
* Arguments    : alarm_val -
*                    the expected alarm value(BCD code)
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_AlarmValue(rtc_calendarcounter_value_t time_value, rtc_alarm_enable_t control)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    RCR1 &= (uint8_t)~_01_RTC_ALARM_INT_ENABLE;     /* Disable RTC alarm operation */

    RSECAR  = time_value.rseccnt   | (uint8_t)(control.sec_enb << 7);
    RMINAR  = time_value.rmincnt   | (uint8_t)(control.min_enb << 7);
    RHRAR   = time_value.rhrcnt    | (uint8_t)(control.hr_enb  << 7);
    RDAYAR  = time_value.rdaycnt   | (uint8_t)(control.day_enb << 7);
    RWKAR   = time_value.rwkcnt    | (uint8_t)(control.wk_enb  << 7);
    RMONAR  = time_value.rmoncnt   | (uint8_t)(control.mon_enb << 7);
    RYRAR   = time_value.ryrcnt;
    RYRAREN = (uint8_t)(control.yr_enb << 7);
    
    RCR1 |= _01_RTC_ALARM_INT_ENABLE;   /* enable RTC alarm operation */
    
    /* Check device file again, can not define the alarm vect!! */
    //RTCAMK = 0U;                      /* Enable alarm interrupt */
    //RTCAIF = 0U;
    
}

/***********************************************************************************************************************
* Function Name: R_RTC_Get_AlarmValue
* Description  : This function gets alarm value.
* Arguments    : alarm_val -
*                    the address to save alarm value(BCD code)
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Get_AlarmValue(rtc_calendarcounter_value_t * const time_value, rtc_alarm_enable_t * const control)
{
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    
    RCR1 &= (uint8_t)~_01_RTC_ALARM_INT_ENABLE;     /* Disable RTC alarm operation */

    time_value->rseccnt = RSECAR    &0x7F;
    time_value->rmincnt = RMINAR    &0x7F;
    time_value->rhrcnt  = RHRAR     &0x7F;
    time_value->rdaycnt = RDAYAR    &0x7F;
    time_value->rwkcnt  = RWKAR     &0x7F;
    time_value->rmoncnt = RMONAR    &0x7F;
    time_value->ryrcnt  = (uint8_t)RYRAR;
    
    control->sec_enb = RSECAR  >> 7;
    control->min_enb = RMINAR  >> 7;
    control->hr_enb  = RHRAR   >> 7;
    control->day_enb = RDAYAR  >> 7;
    control->wk_enb  = RWKAR   >> 7;
    control->mon_enb = RMONAR  >> 7;
    control->yr_enb  = RYRAREN >> 7;
    
    RCR1 |= _01_RTC_ALARM_INT_ENABLE;   /* enable RTC alarm operation */
    
    /* Check device file again, can not define the alarm vect!! */
    //RTCAMK = 0U;                      /* Enable alarm interrupt */
    //RTCAIF = 0U;
    
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
