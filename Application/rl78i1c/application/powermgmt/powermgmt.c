/* Driver */
#include "r_cg_macrodriver.h"       /* CG Macro Driver */
#include "r_cg_cgc.h"           /* CG CGC Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */
#include "r_cg_sau.h"           /* CG Serial Driver */
#include "r_cg_dsadc.h"         /* CG DSADC Driver */
#include "r_cg_wdt.h"           /* CG WDT Driver */
#include "r_cg_lcd.h"           /* CG LCD Driver */
#include "r_cg_iica.h"          /* CG COMP Driver */
#include "r_cg_vbatt.h"         /* CG Backup Battery Driver */
#include "r_cg_lvd.h"           /* CG LVD Driver */
#include "r_cg_intp.h"          /* CG Interrupt Driver*/
#include "r_cg_adc.h"           /* CG 10bit ADC Driver */
#include "r_cg_dtc.h"           /* CG DTC Driver */
#include "emeter3_structs.h"

/* Wrapper/User */
#include "wrp_mcu.h"            /* Wrapper/MCU */
#include "wrp_em_sw_config.h"   /* SW configuration */
#include "wrp_em_adc.h"         /* Wrapper ADC */
#include "wrp_em_adc_data.h"
/* MW/Core */
#include "em_core.h"            /* EM Core APIs */

/* Application */
#include "platform.h"
#include "powermgmt.h"          /* Power Management Header File */
#include "r_lcd_display.h"      /* LCD display header file */
#include "em_display.h"         /* LCD parameter header file */
#include "event.h"              /* Event header file */
#include "key.h"                /* Key header file */
#include "startup.h"
#include "config_storage.h"    /* Configuration on chip */
#include "storage.h"            /* Storage header file */
#include "mlsCIApp.h"
#include "startup.h"            /* Start up header file */
#include "debug.h"
#include "e2rom.h"
#include "memoryOps.h"
#include "utilities.h"

#include "tampers.h"
#include "emeter3_app.h"
#include "pravakComm.h"
#include "calendarFunctions.h"

void update_energies_stored(void);
void powerUpDataCheck(uint8_t mode);
void setInterruptPointers(void);


/******************************************************************************
Macro definitions
******************************************************************************/
/*
 * Define the RTC interval wakeup time for MODE3
 *  ONESEC : every 1s
 *  ONEMIN : every 60s (1minute)
 */
#define POWERMGMT_RTC_INTERVAL_WAKEUP_TIME      ONEMIN

#if ((POWERMGMT_RTC_INTERVAL_WAKEUP_TIME != ONESEC) && (POWERMGMT_RTC_INTERVAL_WAKEUP_TIME != ONEMIN))
    #error "POWERMGMT_RTC_INTERVAL_WAKEUP_TIME must be ONESEC or ONEMIN only"
#endif
/* Max counter to enter/release mode */
#define POWERMGMT_MAX_MODE1_COUNTER                 3000
#define POWERMGMT_MAX_MODE2_COUNTER                 5
// the following struct is shifted to emeter3_structs.h
/*
typedef struct
{
    uint16_t is_rtc_int         :1;     // Flag to detect RTC wake up source 
    uint16_t is_cover_int       :1;     // Flag to detect COVER OPEN wake up source
    uint16_t is_key_int         :1;     // Flag to detect KEY press 
    uint16_t is_magnetic_int    :1;     // Flag to detect MAGNETIC sensor 
    uint16_t is_vbatt_low       :1;     // Flag to detect VBAT Voltage Low 
    uint16_t is_vrtc_low        :1;     // Flag to detect VRTC Voltage Low 
    uint16_t is_ac_low          :1;     // Flag to detect AC Power Low 
    uint16_t is_vdd_low         :1;     // Flag to detect AC Power Low 
    uint16_t is_neutral_missing :1;     // Flag to detect Neutral Missing
		uint16_t is_hw_key_int			:1;			// This flag is set when there is an actual hardware interrupt key press
} powermgmt_int_flag_t;
*/
ext_rtc_t capturedRTC;
extern uint32_t calWatt;
extern uint32_t calVar;

//extern display_int_flag_t g_display_flag;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
#ifdef POWER_MANAGEMENT_ENABLE   

#ifndef __DEBUG
static
#endif
powermgmt_mode_t        g_powermgmt_current_mode = POWERMGMT_MODE2;     /* Current mode value in mode diagram */
//powermgmt_mode_t        g_powermgmt_current_mode = POWERMGMT_MODE1;     /* Current mode value in mode diagram */

#ifndef __DEBUG
static
#endif
powermgmt_mode_t        g_powermgmt_previous_mode = POWERMGMT_MODE2;        /* Current mode value in mode diagram */
//powermgmt_mode_t        g_powermgmt_previous_mode = POWERMGMT_MODE4;        /* Current mode value in mode diagram */


#ifndef __DEBUG
static
#endif
//powermgmt_mode_t        g_powermgmt_prev_main_mode = POWERMGMT_MODE1;       /* Current mode value in mode diagram */
powermgmt_mode_t        g_powermgmt_prev_main_mode = POWERMGMT_MODE1;       /* Current mode value in mode diagram */


#ifndef __DEBUG
//static
#endif

powermgmt_int_flag_t    g_powermgmt_flag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    /* Flag to control interrupt in powermgmt */

//static void POWERMGMT_DisplayLCDParams(float32_t kwh);

void POWERMGMT_ACLow_Action(void);

void waitForSupplyToStabilize(void);


static void POWERMGMT_EXLVD_Action(void)
{
    if (g_powermgmt_flag.is_ac_low == 1)
    {
			stopEM();
//        EM_Stop();
        DSADCEN = 0U;                                   // DSAD module 
        ADCEN = 0U;                                     // ADC 10bit module 
        
// we should save kwh and critical data
//				update_energies_stored();
//  			write_alternately(CTLDATA_BASE_ADDRESS, ALTERNATE_CTLDATA_BASE_ADDRESS, (uint8_t*)&ctldata, sizeof(ctldata));
//        EVENT_PowerFail(EM_EXT_CALLBACK_STATE_ENTER);
        // Switch to lower clock 
        R_CGC_OperateAt6MHz();
    }
    else
    {// external vdd is back
        // AC High again, change to normal operation state, but ADCs still not start, wait until VDD is high again  
        // Switch to lower clock 
//				GDIDIS=0;	 // Enable inputs
				
//        EM_Stop();
        
        R_CGC_OperateAt24MHz();
        
//        EM_Start();
    }
}


uint8_t POWERMGMT_ProcessMode3_Pseudo(void)
{
//	R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  /* Turn On one second interrupt to increment disp_timer */
  SAU0EN = 1U;                                    /* SAU0 module */
  SAU1EN = 1U;                                    /* SAU1 module */
  R_UART2_Start();                                /* Start UART2 */
	R_UART3_Start();
  return 0;
}

void POWERMGMT_DoAfterStop(void)
{
	volatile uint8_t config_initial_status;
	
	DI();
	setInterruptPointers();	
	EI();
	
  SAU0EN = 1U;  
  SAU1EN = 1U; 
  R_UART0_Start(); 
  R_UART2_Start(); 
	R_UART3_Start();
	
	POWERMGMT_EXLVD_Action();	// Switch to normal frequency - 24 MHz


//    IICA0EN = 1U;                                   /* IICA0 module */
    DSADCEN = 1U;                                   /* DSAD module */
    TAU0EN = 1U;                                    /* Timer module */
//    SAU0EN = 1U;                                    /* SAU0 module */
//    SAU1EN = 1U;                                    /* SAU1 module */
    ADCEN = 1U;                                     /* ADC 10bit module */
    MACEN = 1U;                                     /* MAC module */
    DTCEN = 1U;                                     /* DTC module */
    VRTCEN = 1U;
    
    R_PORT_Create();                                /* Reinitialize Pin Port */
    R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  /* Set RTC period interrupt */
//    R_RTC_Set_ClockOutputOn(RTCOS_FREQ_1HZ);        /* Set RTC output clock */
    R_TAU0_Channel0_Start();                            /* Stop TAU00 */    
    R_TAU0_Channel1_Start();                            /* Stop TAU00 */    
    R_TAU0_Channel2_Start();                            /* Stop TAU00 */    

		config_initial_status = CONFIG_Init();
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_PHASE_R));
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_PHASE_Y));
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_PHASE_B));
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_NEUTRAL));
    
    // Init default ADC Timer offset value  
    EM_ADC_SAMP_SetTimerOffset(EM_CALIB_DEFAULT_TIMER_OFFSET);

		R_TAU0_Channel3_Start();
		EM_Init((EM_PLATFORM_PROPERTY *)&g_EM_DefaultProperty,(EM_CONFIG *)&g_EM_DefaultConfig,(EM_CALIBRATION *)&g_EM_DefaultCalibration);
		config_data_load(config_initial_status);
		CONFIG_Restore(CONFIG_ITEM_CALIB);
		NOP();
		
		startEM();

//    R_UART0_Start();                                /* Start UART0 */
//    R_UART2_Start();                                /* Start UART2 */
//		R_UART3_Start();

    R_INTC0_Start();
//    R_INTC1_Start();
//    R_IICA0_Start();                                /* Start IICA0 module */
    R_LCD_PowerOn();                                /* Start LCD */
//    R_INTC1_Start();    
}


void waitForSupplyToStabilize(void)
{// this function is called when we are running on sub-system clock 32.768KHz
// something no 17 NOPS should do the trick;

	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();

}

void POWERMGMT_DoBeforeStop(void)
{
		POWERMGMT_EXLVD_Action();	// Switch to a lower frequency - 6 MHz
				
    LCD_ClearAll();
    R_LCD_PowerOff();                               /* Stop LCD module */
//    R_INTC0_Stop();
//    R_INTC1_Stop();
//    R_IICA0_Stop();                                 /* Stop IICA0 module */
    R_UART0_Stop();                                 /* Stop UART0 module */
    R_UART2_Stop();                                 /* Stop UART0 module */
    R_UART3_Stop();                                 /* Stop UART0 module */
    R_TAU0_Channel0_Stop();                         /* Stop TAU00 */
    R_TAU0_Channel1_Stop();                         /* Stop TAU01 */
    R_TAU0_Channel2_Stop();                         /* Stop TAU02 */
    R_TAU0_Channel3_Stop();                         /* Stop TAU03 */
//    R_RTC_Set_ConstPeriodInterruptOn(PES_2_SEC);  /* Set RTC period interrupt to 2 seconds in mode 4 */
		R_RTC_Set_ConstPeriodInterruptOff();            /* Change RTC to 1 sec interrupt and system */
//		R_RTC_Set_ClockOutputOff();                     /* Disable RTC clock output */
    R_PORT_StopCreate();                            /* Reset pin port state */

    VRTCEN = 0U;	// no luck with this too.
		
//		GDIDIS = 1;		
    /* Stop clock supply to all peripherals (except independent RTC) */
    PER0 = 0x00U;
    PER1 = 0x00U;
    PER2 = 0x00U;
// the following code has been commented on 04/Sep/2020		
/*		
// new code start		
		CSS=1;
		while (CLS!=1);	// wait

		NOP();
		BUPPRT=1;
		VBATSEL=0;
		VBATSEL=1;
		BUPPRT=0;
		
		waitForSupplyToStabilize();	// 500 microseconds at 32.768 KHz clock
		CSS=0;
		while (CLS!=0);
// CPU is now running on VBAT and main system clock		
// new code end	
*/
}

uint8_t POWERMGMT_ProcessMode1(void)
{
  timeinfo_t lastTime;  // minutes,hours,day,date,month,year - use this so that existing single phase incSurveyDate may be used.
	int8_t retryCtr;
	int8_t retval;
	
	if(g_softreset_flag==0)
	{// call this function only we come here and meter has not started from reset
		POWERMGMT_DoAfterStop();
	}
		
	LCD_ClearAll();

	readRTC();
		
//		asendbare((uint8_t *)"Ent Mode 1\r\n");  		
  if (EM_GetSystemState() != SYSTEM_STATE_RUNNING)
  {
		startEM();
//      if (EM_Start() != EM_OK)
//      {
//          return 1;   // resume fail
//      }
  }

	
//	setInterruptPointers();	// this function call is shifted to POWERMGMT_DoAfterStop

	powerUpDataCheck(1);	
	
	if(g_softreset_flag==0)
	{// call this function only when the meter has not started from reset
	// not required to be done when meter starts from reset as already done before entering the infinte loop
		do_initial_things(); 
	}
	else
		g_softreset_flag=0;	// make the flag 0
		
	
  LCD_DisplayAll();	// 19/Aug/2020 - 17:00
//	lcd_display_version();

#ifdef POWER_FAIL_INTERVAL_60
	if(diffInSecondsBetween(rtc, capturedRTC)>59)	// one minute
#else
	if(diffInSecondsBetween(rtc, capturedRTC)>5)
#endif	
	{
		retryCtr=0;
		do
		{
			retval = record_power_fail_event();	// both power fail and power return
			retryCtr++;
			if((retryCtr>5)&&(retval==0))
				retval=1;	// give up after 5 tries
		}
		while(retval==0);
		
		clearPersistenceCounters();		// counters will be cleared only if there is a genuine power fail	
	}
// it is possible that record_power_fail_event may return without writing the power return record
// if the EM has not returned voltage values - too bad - wehave retried 5 times
	g_display_mode=0;	// autoscroll
	sub_disp_state=255;
	disp_state=255;
	disp_timer=disp_autoscroll_interval;
	autoscrollInButtonMode=0;
	alt_autoscroll_mode=0;	// extra line
	avoidShowingRepeatParameters=0;
	
	fast_display_flag=0;	// safety
	someKeyPressed=0;	// safety
	fwdKeyPressed=0;
	revKeyPressed=0;
	
	gujaratSecondsCounter=0;
	init_done=1;
	g_powermgmt_flag.bijliIsThere=1;
	
// check for magnet tamper and ac magnet tamper
// the following code will ensure that if these tampers are not recorded, then the mag icon will be turned off
  if((mytmprdata.tamperEvent_image&MAGNET_MASK) == 0)
	{// magnet tamper is NOT there
		g_TamperStatus&=~HALLSENS_MASK;
	}
	
  if((mytmprdata.tamperEvent_image&AC_MAGNET_MASK) == 0)
	{// magnet tamper is NOT there
		pulsesStartedComingFlag=0;
	}
	
// The variable calswitch is updated only when entering POWERMGMT_Mode1	
	if(CALSW==0)
	{// User
		calswitch=0;
		disp_autoscroll_interval=10;
	}
	else
		calswitch=1;

//	clearPersistenceCounters();	// shifted up and is now called only if power fails for more than 5 seconds		
		
  return 0;// Success 
}

uint8_t POWERMGMT_ProcessMode2(void)
{
    // Clear interrupt flag
    if (g_powermgmt_flag.is_rtc_int == 1)       // RTC interrupt wakeup CPU
    {
        g_powermgmt_flag.is_rtc_int = 0;
    }
    return 0;
}


uint8_t POWERMGMT_ProcessMode3(void)
{
//						P3|=0x80;	//E2ROM_SUPPLY_ON - This remains on - SCL, SDA and WP lines are made input
	R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  /* Turn On one second interrupt to increment disp_timer */
	
	P6 =0x05;	// P6.2=1,SDA=0,SCL=1
	PM6 = 0xFA;	// 1111 1010 - P6.2(Out), SDA(Inp), SCL(Out)
	
	R_LCD_PowerOn();

	someKeyPressed=0;	// safety
	fast_display_flag=0;	// safety
	autoscrollInButtonMode=0;	// safety
	alt_autoscroll_mode=0;	// safety
	avoidShowingRepeatParameters=0;
	fwdKeyPressed=0;
	revKeyPressed=0;
	
//	holdState=0;	// safety	
#ifdef POWER_OFF_DISPLAY_MODE_PUSHBUTTON	
	g_display_mode=1;	// pushbutton mode
#else
	g_display_mode=0;	// autoscroll mode
#endif	
	sub_disp_state=0;
	disp_state=255;	// once incremented will start from state 0
	disp_timer=disp_autoscroll_interval;
	
// This is to start the communication module	
//  SAU0EN = 1U;
//  SAU1EN = 1U;
	
// We must also Vcc2 (P57) On
//	P5 |=0x80;	// P57=1 - 1000 0000	// this will be turned on when there is a long key press - 17/Jan/2023
	
// TXD3(P85) must be output, RXD3(P84) input - already the case
// P85 must be 1
	P8|=0x20;	// 0010 0000 

// IRTXD(P56) must be output, IRRXD(P55) input - already the case
// P56 must be 1
	P5 |=0x40;	// P56=1 - 0100 0000
	
	
//  R_UART2_Start();
//	R_UART3_Start();

// We can turn on AUX_OUT too to enable the other pushbuttons
#ifndef PCB_2_5
	P5|=0x08;	// 0000 0100 - P5.3=1
#endif

	checkCoverTamper();
	
  return 0;
}

uint8_t POWERMGMT_ProcessMode4(void)
{
	switch (g_powermgmt_previous_mode)
	{
    case POWERMGMT_MODE2:
// any pending memory operation must be completed before shutting down
			if(e2write_flags!=0)
			{
		  	wre2rom();
		    R_WDT_Restart();
				return 1;
			}
			
      if (EM_GetSystemState() == SYSTEM_STATE_RUNNING)
      {
        if (EM_Stop() == EM_OK)
        {
          // Do nothing
        }
        else
        {
          return 1;   // Stop EM Core fail
        }
			}
			
			if(g_powermgmt_flag.bijliIsThere==1)
			{
				readRTC();
				capturedRTC=rtc;
  			capturedRTC.Control=calcChecksum((uint8_t*)&capturedRTC,sizeof(capturedRTC)-1);	// line added on 29/Dec/2021
// above line is added where the unused Control variable is used to contain the checksum				
			}			
      // clear all the flag
      g_powermgmt_flag.is_rtc_int  = 0;
      g_powermgmt_flag.is_cover_int  = 0;	// reenabled on 05/Sep/2020 - let us not do this to allow polling of cover open pin
      g_powermgmt_flag.is_key_int  = 0;
      g_powermgmt_flag.is_magnetic_int = 0;
      g_powermgmt_flag.is_hw_key_int = 0;	
			
// clear the pulsing variables
			calWatt = 0;
			total_active_power=0;
			total_active_power_pulsing=0;
#ifdef REACTIVE_PULSING
			calVar = 0;
			total_reactive_power=0;
			total_reactive_power_pulsing=0;
			total_apparent_power_pulsing=0;
#endif
			init_done=0;	// this must be done to prevent certain processing from taking place
//			asendbare((uint8_t *)"Xxx\r\n");  		
      POWERMGMT_DoBeforeStop(); // All peripherals off, system will go to STOP mode in polling processing
#ifdef DISPLAY_AUTO_ON_IN_POWER_OFF_MODE
			if(calswitch!=0)
				gujaratSecondsCounter=64795;
			
			if(gujaratSecondsCounter==0)
			{
				R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  /* Turn On one second interrupt to increment disp_timer */
				g_powermgmt_flag.is_key_int  = 1;	// a soft key interrupt to enable entering MODE3 from MODE4
			}
			else
			{
				g_powermgmt_flag.is_key_int  = 0;	// this is to ensure that MODE3 does not get selected
			}
#else
			R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  /* Turn On one second interrupt to increment disp_timer */
			gujaratSecondsCounter=0;
#endif
// should we turn the one second interrupt on - if not then cover tamper may not get logged in power fail mode
			g_powermgmt_flag.bijliIsThere=0;
      return 0;	// Success
						
    case POWERMGMT_MODE3:
      // Stop LCD and IICA 
      R_LCD_PowerOff();
//		R_IICA0_Stop();
      IICA0EN = 0U;               // IICA0
      VRTCEN = 0U;
      R_PORT_StopCreate();
      return 0;	// Success
            
    // Not support previous mode
    case POWERMGMT_MODE1:
    case POWERMGMT_MODE5:
      return 1;
    }
    return 1; //we are not supposed to come here except in case of an error
}

uint8_t POWERMGMT_ProcessMode5(void)
{
    POWERMGMT_DoAfterStop();
    
    if (EM_GetSystemState() != SYSTEM_STATE_RUNNING)
    {
        // Stop EM Core when it running
        if (EM_Start() != EM_OK)
        {
            return 1;
        }
    }

    return 0;
}

static uint8_t POWERMGMT_SwitchMode(powermgmt_mode_t newmode)
{
  uint8_t status = 0;

  // Process each mode 
  switch (newmode)
  {
    case POWERMGMT_MODE1: // Normal Mode
        status = POWERMGMT_ProcessMode1(); 
        break;
        
    case POWERMGMT_MODE2: // Transient Mode
      status = POWERMGMT_ProcessMode2(); 
      break;
        
    case POWERMGMT_MODE3:  //LCD and Cover Open During Sleep Mode
	    status = POWERMGMT_ProcessMode3();
	    break;
        
    case POWERMGMT_MODE4: // Sleep Mode
      status = POWERMGMT_ProcessMode4();
      break;
        
    case POWERMGMT_MODE5: // Neutral Missing Mode
      status = POWERMGMT_ProcessMode5(); 
      break;
        
    case POWERMGMT_MODE7:   // Debug mode
      status = 0;
      break;
        
    default:
      /* Failed */
      status = 1;
      break;              
  }
  return (status);
}
#endif      /* POWER_MANAGEMENT_ENABLE */

powermgmt_mode_t POWERMGMT_GetMode(void)
{
  #ifdef POWER_MANAGEMENT_ENABLE
//		#ifndef MAGNET_INCREMENTATION	
//        return POWERMGMT_MODE1;
//		#else
        return g_powermgmt_current_mode;
//		#endif		
				
  #else
        return POWERMGMT_MODE1;
  #endif
}

/*
	10/Oct/2022
	We have added a check in exiting from MODE1
	If we are in MODE1, and the power fails, we are now counting 
	till 1000, before entering MODE2
	
	However if we are in MODE3 or MODE4 and if power returns, then
	we enter MODE2 immediately
*/

void POWERMGMT_PollingProcessing(void)
{// upon reset the current mode is mode2
  #ifdef POWER_MANAGEMENT_ENABLE
  static uint16_t mode1_counter = 0;
  static uint16_t mode2_counter = 0;
  
  powermgmt_mode_t new_mode = g_powermgmt_current_mode;   
  g_powermgmt_previous_mode = g_powermgmt_current_mode;
	
	
  if (g_powermgmt_previous_mode != POWERMGMT_MODE2)
  {
      g_powermgmt_prev_main_mode = g_powermgmt_previous_mode;
  }
	
	switch (g_powermgmt_current_mode)
  {
    case POWERMGMT_MODE1:
			if(g_display_mode!=255)
			{
  			if(0!=e2write_flags)
					wre2rom();
				else
				{
		      if(g_powermgmt_flag.is_ac_low == 1)
		      {// here power is not there
						mode1_counter++;
						if(mode1_counter>1000)
						{
							mode1_counter=0;
		          new_mode = POWERMGMT_MODE2;
							g_powermgmt_prev_main_mode = POWERMGMT_MODE1;
						}
		      }
					else
					{// here power is there and we are in MODE1 - do nothing
						mode1_counter = 0;
					}
				}
			}
			checkCoverTamper();	// 22/Feb/2020
      break;

    case POWERMGMT_MODE2:
      if (g_powermgmt_flag.is_ac_low == 0)
      {// external vdd is there
				mode1_counter++;
				if(mode1_counter>100)
				{
	        mode1_counter = 0;
	        new_mode = POWERMGMT_MODE1;
					g_display_mode=255;	// This is a flag to indicate that we are entering power mode 1
					disp_timer=disp_autoscroll_interval;
				}
      }
      else
      {// VDD pin has not yet gone to upper limit or VDD is falling
				//if (mode1_counter < POWERMGMT_MAX_MODE1_COUNTER)
        if (mode1_counter < 10)	// for testing - earlier 1000
        {// wait for some time - remain in mode 2
          mode1_counter++;
        }
        else
        {
          // if VDD pin is Below threshold --> Enter Power Fail Mode
          if (g_powermgmt_flag.is_ac_low == 1) 
          {// here external VDD has disappeared
            mode1_counter = 0;
            new_mode = POWERMGMT_MODE4; // sleep mode
				    LCD_ClearAll();
          }
        }
      }
      break;
          
    case POWERMGMT_MODE3:
      if (g_powermgmt_flag.is_ac_low == 1)
      {// here there is no power and we are in mode three
          mode1_counter = 0;
#ifdef DISPLAY_AUTO_ON_IN_POWER_OFF_MODE			
				if((gujaratSecondsCounter>64800)&&(sleepCommMode==0))		// 64800 seconds = 18 hours - please use this
//				if((gujaratSecondsCounter>300)&&(sleepCommMode==0))		// 64800 seconds = 5 minutes - please comment - only for testing
				{
//          mode1_counter = 0;
          new_mode = POWERMGMT_MODE2; // Transient Mode
					g_powermgmt_prev_main_mode = POWERMGMT_MODE3;
				}
				
				if(g_powermgmt_flag.is_key_int==1)
				{
					disp_timer = disp_autoscroll_interval;
					g_powermgmt_flag.is_key_int=0;
					someKeyPressed = 1;
				}
#else
				if((gujaratSecondsCounter>20)&&(sleepCommMode==0))		// 20 seconds
				{
//          mode1_counter = 0;
          new_mode = POWERMGMT_MODE2; // Transient Mode
					g_powermgmt_prev_main_mode = POWERMGMT_MODE3;
				}
				
				if(g_powermgmt_flag.is_key_int==1)
				{// UP key has been pressed
					disp_timer = disp_autoscroll_interval;
					g_powermgmt_flag.is_key_int=0;
					fwdKeyPressed = 1;
					someKeyPressed = 1;
					gujaratSecondsCounter=0;
				}
				
				if(MIDDLESW==0)
				{	
					disp_timer = disp_autoscroll_interval;
					revKeyPressed = 1;
					someKeyPressed = 1;
					gujaratSecondsCounter=0;
				}
#endif				
				checkCoverTamper();	// 22/Feb/2020
      }
      else
      {// here there is power and we are in mode 3 - we need to get out but let us be sure
			// we will come here every second in power off mode
//					mode1_counter++;
//					if(mode1_counter>1)
//					{
	          mode1_counter = 0;
	          new_mode = POWERMGMT_MODE2;
						g_powermgmt_prev_main_mode = POWERMGMT_MODE3;
//					}
      }
      break;

    case POWERMGMT_MODE4:
      if ( g_powermgmt_flag.is_ac_low == 0 ) 
      {// Switch to transition mode and log - ac power has returned
//				mode1_counter++;
//				if(mode1_counter>1)
//				{
	        new_mode = POWERMGMT_MODE2;
	        mode1_counter = 0;
					g_powermgmt_prev_main_mode = POWERMGMT_MODE4;
//				}
      }
			else
			{// here there is no ac power and we are in mode 4
				mode1_counter=0;
				// check if any key has been pressed
				if(g_powermgmt_flag.is_key_int==1)
				{// here switch has been pressed - new mode
#ifdef DISPLAY_AUTO_ON_IN_POWER_OFF_MODE			
					if(g_powermgmt_flag.is_hw_key_int==0)
					{// this was a software interrupt
						if(calswitch!=0)
							gujaratSecondsCounter=64795;	// production mode					
						else
							gujaratSecondsCounter=0;
					}
					else
					{// this is a genuine hardware interrupt
	//					gujaratSecondsCounter=64765;
						gujaratSecondsCounter=64795;
	//					someKeyPressed=1;
					}
	#endif				
					new_mode = POWERMGMT_MODE3;
					g_powermgmt_flag.is_key_int=0;
					g_powermgmt_flag.is_hw_key_int=0;
	#ifndef DISPLAY_AUTO_ON_IN_POWER_OFF_MODE			
					gujaratSecondsCounter=0;
	#endif				
				}
				else
				{// here no key has been pressed - check if the cover has been opened 
					if(g_powermgmt_flag.is_cover_int==1)
					{// here cover open switch has been operated
						new_mode = POWERMGMT_MODE3;
						if(calswitch!=0)
							gujaratSecondsCounter=64795;	// production mode					
						else
							gujaratSecondsCounter=0;
					}
				}
			}
			break;
			
    case POWERMGMT_MODE5: //neutral_missing
      new_mode = POWERMGMT_MODE2;
      break;
				
    case POWERMGMT_MODE7:
			mode1_counter = 0;
			new_mode = POWERMGMT_MODE2;
			break;
			
    default:
      new_mode = POWERMGMT_MODE1;
      break;
	}
    
    /* Change to new mode when need */
  if (new_mode != g_powermgmt_current_mode || new_mode == POWERMGMT_MODE2)
  {   
    if (POWERMGMT_SwitchMode(new_mode) == 0)
    {
        g_powermgmt_current_mode = new_mode;
    }
    else
    {// here there was an error in switching the mode
      /* TODO: Error */
    }
  }
    #endif  /* POWER_MANAGEMENT_ENABLE */
}

void checkCoverTamper(void)
{
	powermgmt_mode_t power_mode;
	uint8_t count;
	
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	
	if(power_mode==POWERMGMT_MODE3)
		count = 2;
	else
	{
//		if(power_mode==POWERMGMT_MODE1)
			count = 100;
//	else
//		count = 200;
	}
	
//  if (g_powermgmt_flag.is_cover_int == 1)
//  {// log the cover open tamper - what happens to cover open tamper when power is there?
//		if(P15.1==0)
		if(P15.1==1)
		{// cover is open if the pin is 1
			coverOpenCtr++;
			if(coverOpenCtr>count)
			{
		  	if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) == 0)
				{
					readRTC();
//			    mytmprdata.tamperCount++;	// this is not to be counted - non-rollover event - 07/Sep/2022 - shubham feedback
//			    mymddata.tamperCount++;
					mytmprdata.tamperEvent_flags |= COVER_OPEN_MASK;
					mytmprdata.tamperEvent_image |= COVER_OPEN_MASK;
					mytmprdata.tamperEvent_on|=COVER_OPEN_MASK;
					e2write_flags |= E2_W_EVENTDATA;
					wre2rom(); // write the tamper here itself
#ifdef QUICK_FIRST_DISPLAY		
					g_cover_display_state=255;	// this variable is used in the display function - 255 ensures that first message will be COPEN
					cover_disp_timer=10;// this will ensure that the first display happens immediately 
#endif					
				}
			}
		}
		else
		{
			coverOpenCtr=0;
		}
		g_powermgmt_flag.is_cover_int = 0;	// job done
//	}// end of if block
}

void POWERMGMT_RTC_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
  g_powermgmt_flag.is_rtc_int = 1;
#endif    
}

void POWERMGMT_COVER_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
  g_powermgmt_flag.is_cover_int = 1;
#endif
}

void POWERMGMT_KEY_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
	  g_powermgmt_flag.is_key_int = 1;
		g_powermgmt_flag.is_hw_key_int =1;
//  g_powermgmt_flag.is_key_int = 0;
#endif
}

void POWERMGMT_VDD_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
// g_powermgmt_flag.is_vdd_low will not be used at all in online meter
  if(LVDVDDF == 0)    //VDD VOLTAGE > DETECTION THRESHOLD
  {// vdd is returning
    g_powermgmt_flag.is_vdd_low = 0;
  }
  else 
  {
    g_powermgmt_flag.is_vdd_low = 1;
//				set_tamperEvent_image(POWER_EVENT);	// record the power fail tamper
// power is failing - vdd going down
  }
#endif
}

// the following function may be called upon startup to create the condition
// of ac power NOT being available - before the LVDEXTLVD is enabled
void POWERMGMT_Set_AC_LOW(void)
{// this function forcibly creates a condition of AC not being there
	g_powermgmt_flag.is_ac_low = 1;
}

void POWERMGMT_Clear_AC_LOW(void)
{// this function forcibly creates a condition of AC not being there
	g_powermgmt_flag.is_ac_low = 0;
}

void POWERMGMT_setCurrentMode1(void)
{
	g_powermgmt_current_mode = POWERMGMT_MODE1;
}


void POWERMGMT_AC_InterruptCallback(void)
{// we are NOW using this interrupt - 24/Dec/2018 - meter has battery connected to DVDD 
#ifdef POWER_MANAGEMENT_ENABLE
    if(LVDEXLVDF == 0)    //EXLVD VOLTAGE > DETECTION THRESHOLD
    {
        g_powermgmt_flag.is_ac_low = 0;
    }
    else 
    {
        g_powermgmt_flag.is_ac_low = 1;
    }
#endif
}

void POWERMGMT_VBATT_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
    if(LVDVBATF == 0)    //EXLVD VOLTAGE > DETECTION THRESHOLD
    {
        g_powermgmt_flag.is_vbatt_low = 0;
    }
    else 
    {
        g_powermgmt_flag.is_vbatt_low = 1;
    }
#endif
}

void POWERMGMT_VRTC_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
	if(rtcTestFlag==0)
	{
    if(LVDVRTCF == 0)    //VRTC VOLTAGE > DETECTION THRESHOLD
    {// here battery is good 
        g_powermgmt_flag.is_vrtc_low = 0;
    }
    else 
    {// here battery is bad
				MAKE_RTC_SUPPLY_HIGH;	// Ensure that backup supply is immediately turned high to avoid further voltage drop
				g_powermgmt_flag.is_vrtc_low = 1;
    }
	}
	else
	{// here rtcTestFlag is not 0 - hence test is in progress - in this case only set the is_vrtc_low flag, do not clear it
    if(LVDVRTCF == 0)    //VRTC VOLTAGE > DETECTION THRESHOLD
    {// here battery is good 
//        g_powermgmt_flag.is_vrtc_low = 0;	// do not update this flag
    }
    else 
    {// here battery is bad
				MAKE_RTC_SUPPLY_HIGH;	// Ensure that backup supply is immediately turned high to avoid further voltage drop
				g_powermgmt_flag.is_vrtc_low = 1;
    }
	}
#endif
}

void POWERMGMT_NeutralMissing(EM_EXT_CALLBACK_STATE state)
{
#ifdef POWER_MANAGEMENT_ENABLE
    if (state == EM_EXT_CALLBACK_STATE_ENTER)
    {
        g_powermgmt_flag.is_neutral_missing = 1;
    }
    else
    {
        g_powermgmt_flag.is_neutral_missing = 0;
    }
#endif
}

void POWERMGMT_MAGNETIC_InterruptCallback(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
    g_powermgmt_flag.is_magnetic_int = 1;
#endif
}

uint8_t POWERMGMT_IsACLow(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
    return ((uint8_t)g_powermgmt_flag.is_ac_low);     /* return value of Monitor flag of comparator 0 */
#else
    return 0;
#endif
}

uint8_t POWERMGMT_IsVBATTLow(void)
{
#ifdef POWER_MANAGEMENT_ENABLE
    return ((uint8_t)g_powermgmt_flag.is_vbatt_low);     /* return value of Monitor flag of comparator 0 */
#else
    return 0;
#endif
}

uint8_t POWERMGMT_IsVRTCLow(void)
{   
#ifdef POWER_MANAGEMENT_ENABLE
//    if(LVDVRTCF == 0)    //VRTC VOLTAGE > DETECTION THRESHOLD
//    {// here battery is good 
//        return 0;
//    }
//    else 
//    {// here battery is bad
//				return 1;
//		}

    return ((uint8_t)g_powermgmt_flag.is_vrtc_low);     /* return value of Monitor flag of comparator 0 */
#else
    return 0;
#endif
}

void turnOff_UARTS(void)
{
//-------------				
// This is to stop the communication module	

  R_UART0_Stop();                                 /* Stop UART0 module */
  R_UART2_Stop();                                 /* Stop UART0 module */
  R_UART3_Stop();                                 /* Stop UART0 module */

// We must also Vcc2 (P57) Off
	P5 &=~0x80;	// P57=0 - 1000 0000
	
//  SAU0EN = 0U;
//  SAU1EN = 0U;
	

/*	
// TXD3(P85) must be output, RXD3(P84) input - already the case
// P85 must be 1
	P8|=0x20;	// 0010 0000 

// IRTXD(P56) must be output, IRRXD(P55) input - already the case
// P56 must be 1
	P5 |=0x40;	// P56=1 - 0100 0000
*/	
}