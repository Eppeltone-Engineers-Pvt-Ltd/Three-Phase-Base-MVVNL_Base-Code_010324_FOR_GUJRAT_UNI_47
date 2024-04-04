/***********************************************************************************************************************
* File Name    : main.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Main processing
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <string.h>
//#include <stdlib.h>
#include <stdio.h>

/* Driver */
#include "r_cg_macrodriver.h"           /* CG Macro Driver */
#include "r_cg_wdt.h"                   /* CG WDT Driver */
#include "r_cg_vbatt.h"                 /* CG VBAT Driver */
#include "r_cg_dtc.h"
#include "r_cg_tau.h"
#include "r_cg_adc.h"
#include "r_lcd_display.h"
#include "r_cg_rtc.h"
#include "r_dlms_obis_ic.h"				// DLMS OBIS IC Definitions  


/* Wrapper/User */
#include "wrp_em_sw_config.h"

/* MW/Core */
#include "em_core.h"                    /* EM Core APIs */

/* Application */
#include "startup.h"                    /* Startup Header File */
#include "platform.h"                   /* Default Platform Information Header */
#include "key.h"                        /* Key Interface */
#include "led.h"                        /* LED Interface */
#include "config_storage.h"                    /* Storage Header File */
#include "event.h"                      /* Event Header File */
#include "em_display.h"                 /* LCD Display Pannel */
#include "debug.h"                      /* Debug */
#include "mlsCIApp.h"
#include "powermgmt.h"                  /* Power Management module */
#include "pravakComm.h"
#include "DLMS_User.h"			// for DLMS
#include "emeter3_structs.h"
#include "emeter3_app.h"
#include "memoryOps.h"
#include "e2rom.h"
#include "utilities.h"
#include "tampers.h"
#include "em_display_TN3P_Helper.h"
#include "survey.h"
#include "calendarFunctions.h"
#include "r_dlms_data_meter.h"
#include "r_dlms_data.h"

#include "iHexParser.h"
#include "trigFunctions.h"
#include "FrequencyDisturbance.h"
#include "p_timer.h"
#include "appInterrupts.h"
#include "MY_ram_vector.h"

/* 
 * CS+ CCRL reload trigger:
 * This is used to reload changes from middlewware 
 * when there's no change on application layer
 * There's no dummy_reload.h file, this just make
 * a "not found" file in dependency list of main.c
 * Hence, trigger re-compile everytime build project
*/
#ifdef DUMMY_FOR_RELOAD
    #include "dummy_reload.h"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

extern Unsigned32				g_Class07_Blockload_CapturePeriod;
extern uint32_t g_Class07_Event_EntriesInUse[];

extern uint8_t amsgbuf[];

extern volatile uint8_t gWhichPort;	// global variable

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
uint8_t init_done;
uint8_t sleepCommMode;
uint8_t sleepCommModeTimer;
int8_t energyIncrementationFlag;
int8_t perSecondFlag;	// used for per_second_activity
//uint8_t rtcFailed;	// if 1 then rtc has failed - Battery removed
uint8_t rtcTestFlag;	// if 1 then rtc is being tested
uint8_t rtcTestCounter;
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: void main(void)
* Description  : Main processing
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/

// uncomment  the following to get 12345 on LCD display
#define LCD_COUNT_UP

// VBATEN and VBATSEL are cleared to 0 only on Power On Reset
// Power On reset uses Internal VDD. Hence one does not expect a reset in Battery mode operations
// except the initial reset issued by the programmer - Thus one may need to rely on Watchdog and other resets
extern date_time_t from_date_time, to_date_time;
uint8_t EPR_Read_Test(uint32_t addr, uint8_t* buf, uint16_t size);
void find_num_between_lp_entries(void);
#ifndef KERALA_UNIDIR
void displayRepeatParameters(void);
#endif
void resetTheMeter(void);
void display_autoScroll(void);
void update_saved_energies(void);
void copyFactoryDisplayProgram(void);
void loadDisplayFileFromE2Rom(void);
extern void energy_incrementation(void);
extern void handleDisplay_Gujarat(uint8_t t);
extern void handleDisplay_Production(void);
void restoreRTC_Date_Time(void);
void performRTC_BatteryTest(void);

#ifdef TESTING
void readTamperRecords(void)
{// g_Class07_Event_EntriesInUse has been read
	unsigned int c1,c2;
	c1=9;
	c2=0;
	while(c2!=g_Class07_Event_EntriesInUse[0])
	{
		get_tamper_data(c1,c2);
		c2++;
	}
}
#endif

#ifdef TN3P_BOARD
void copyFactoryDisplayProgram(void)
{
	uint8_t i;
	
	if(Common_Data.meterType==0)
	{// default is bidirectional meter
		for(i=0;i<DSP_ARRAY_SIZE;i++)
			dspArr[i]=factoryDispPgmBidir[i];
	}
	else
	{
		for(i=0;i<DSP_ARRAY_SIZE;i++)
			dspArr[i]=factoryDispPgmUnidir[i];
	}
}
// this function is not needed for Gujarat
void loadDisplayFileFromE2Rom(void)
{//the appropriate file is stored in the E2rom
	if(Common_Data.meterType==0)
		EPR_Read(DSP_PROG_ADDRESS, (uint8_t*)dspArr, DSP_ARRAY_SIZE);	// e2rom contains the file for either bidir or unidir at same location
	else
		EPR_Read(DSP_PROG_ADDRESS, (uint8_t*)dspArr, DSP_ARRAY_SIZE);	// default is bidirectional meter
}
#endif

#ifdef TESTING
	#define ACTIVE_PULSE_LED_ON	{P0.5 = 0;}
	#define ACTIVE_PULSE_LED_OFF	{P0.5 = 1;}
	#define REACTIVE_PULSE_LED_ON	{P0.4 = 0;}
	#define REACTIVE_PULSE_LED_OFF	{P0.4 = 1;}
#endif

extern float32_t operatingPF; 
extern class07_blockload_entry_t	g_Class07_BlockLoadBuffer;
extern class07_billing_entry_t		g_Class07_BillingBuffer;


void resetTheMeter(void)
{
	while(1);
}

// we are not using the iica0 interrupt - this is just a dummy routine
void r_iica0_interrupt(void);
void r_iica0_interrupt(void)
{
}

void r_dsadc_interrupt_asm(void);


void powerUpDataCheck(uint8_t mode);

void powerUpDataCheck(uint8_t mode)
{// mode = 0, upon reset, mode=1, upon return from power fail
	uint32_t temp_energies_wrt_ptr = 0;	// will contain the value to be stored in ctldata.energies_wrt_ptr
//	timeinfo_t checkDTTM;
	int8_t retval;
	int8_t tempval;

	init_read_common_data(); // verifies checksum at the start
	g_Class07_Blockload_CapturePeriod=Common_Data.survey_interval*60;
	
	init_read_lastBillDate();	// always reads the last bill date

	retval = isCheckSumOK((uint8_t*)&myenergydata,sizeof(myenergydata));
	if(retval==0)
	{// check sum is not ok - we need to pick up these values from the multiple sets that have been stored
		temp_energies_wrt_ptr= init_read_energies(); // this common function reads all the energies
		update_saved_energies();
	}
	else
	{// here the checksum is ok
		if(total_energy_lastSaved==0)
		{// this will happen only if there was a reset - can this cause sum of tods to NOT match cumulative kwh?
			update_saved_energies();
		}
	}
	
// ctldata is tremendously important, it must be validated everytime upon reset or upon return from power fail
// the validation would require comparing the values with the backup values stored

	if(mode==0)
	{// here we have come from reset
// if tempval == 0 then all is good, but if tempval is 1 then it means ctldata has been recovered	
		tempval = init_read_ctl_data(); // sureshot reading of ctldata - verifies checksum at the start
		if(tempval==1)
			recover_ctl_data();
	}
	else
	{// here we have entered POWERMGMT_ProcessMode1
// if tempval == 0 then all is good, but if tempval is 1 then it means ctldata has been recovered	
		tempval = init_read_ctl_data(); // sureshot reading of ctldata
		if(tempval==1)
			recover_ctl_data();
	}

	if(tempval==1)
	{// this implies that ctldata has been recovered - we need to restore the energies
		ctldata.kwh_last_saved = myenergydata.kwh;
		ctldata.kvah_last_saved = myenergydata.kvah;
	
#ifdef BIDIR_METER
		ctldata.kwh_last_saved_export = myenergydata.kwh_export;
		ctldata.kvah_last_saved_export = myenergydata.kvah_export;
#endif

		ctldata.last_saved_kwh = myenergydata.kwh;
		ctldata.last_saved_kvah = myenergydata.kvah;

#ifdef SURVEY_USES_REACTIVE_DATA	
	  ctldata.last_saved_kvarh_lag = myenergydata.kvarh_lag;
	  ctldata.last_saved_kvarh_lead = myenergydata.kvarh_lead;
#endif

#ifdef BIDIR_METER
	  ctldata.last_saved_kwh_export = myenergydata.kwh_export;
	  ctldata.last_saved_kvah_export = myenergydata.kvah_export;
	  ctldata.last_saved_kvarh_lag_export = myenergydata.kvarh_lag_export;
	  ctldata.last_saved_kvarh_lead_export = myenergydata.kvarh_lead_export;
#endif

		if(temp_energies_wrt_ptr!=0)
		{// here init_read_energies returned a value
			ctldata.energies_wrt_ptr=temp_energies_wrt_ptr;
		}
		else
		{// we did not go to init_read_energies
			ctldata.energies_wrt_ptr = discoverEnergiesWrtPtr();
		}
  	ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
		e2write_flags|=E2_W_CTLDATA; // set this flag so that ctldata gets written
	}
	
#ifdef LCD_COUNT_UP	
	if(mode==0)
  	LCD_DisplayDigit(3, 3);
#endif
  if(isCheckSumOK((uint8_t*)&capturedRTC,sizeof(capturedRTC))==0)
	{	
		capturedRTC.second = 0;
		capturedRTC.minute = ctldata.minute;
		capturedRTC.hour = ctldata.hour;
		capturedRTC.day = NOT_SPECIFIED;
		capturedRTC.date = ctldata.date;
		capturedRTC.month = ctldata.month;
		capturedRTC.year = ctldata.year;
	}

#ifdef LCD_COUNT_UP	
	if(mode==0)
	  LCD_DisplayDigit(4, 4);
#endif
	
	init_read_md_data();  // this reads md values etc - current meter data - verifies checksum at the start
	
	init_read_tod();  // zone definitions and current zone data - always reads
	
	init_read_passive_tod();	// read the passive zone definitions and the activation time/status - always reads
	
	init_read_single_action_activation_data();	// read the activation time for single action schedule and status - always reads

	init_read_tamper_data();  // mytmprdata and pointers - verifies checksum at the start
	
	init_read_secret_keys();	// always reads
}

void setInterruptPointers(void);
void setInterruptPointers(void)
{
#ifndef NO_BOOT_CODE	
	//RAM_INTWDTI_ISR
	RAM_INTLVI_ISR=&r_lvd_interrupt;
	RAM_INTP0_ISR=&r_intc0_interrupt;
	RAM_INTP1_ISR=&r_intc1_interrupt;
	RAM_INTP2_ISR=&r_intc2_interrupt;
	RAM_INTP3_ISR=&r_intc3_interrupt;
	RAM_INTP4_ISR=&r_intc4_interrupt;
	RAM_INTP5_ISR=&r_intc5_interrupt;
	RAM_INTP6_ISR=&r_intc6_interrupt;
	RAM_INTP7_ISR=&r_intc7_interrupt;

	RAM_INTST3_ISR=&r_uart3_interrupt_send;
	RAM_INTSR3_ISR=&r_uart3_interrupt_receive;

	RAM_INTST2_ISR=&r_uart2_interrupt_send;
	RAM_INTSR2_ISR=&r_uart2_interrupt_receive;

	RAM_INTST1_ISR=&r_uart1_interrupt_send;
	RAM_INTSR1_ISR=&r_uart1_interrupt_receive;

	RAM_INTST0_ISR=&r_uart0_interrupt_send;
	RAM_INTSR0_ISR=&r_uart0_interrupt_receive;

	RAM_INTIICA0_ISR=&r_iica0_interrupt;

	RAM_INTTM00_ISR=&r_tau0_channel0_interrupt;
	RAM_INTTM01_ISR=&r_tau0_channel1_interrupt;
	RAM_INTTM02_ISR=&r_tau0_channel2_interrupt;
	RAM_INTTM03_ISR=&r_tau0_channel3_interrupt;

	RAM_INTAD_ISR=&r_adc_interrupt;
	RAM_INTIT00_ISR = &p_timer_interrupt;

	RAM_INTRTCIC0_ISR = &r_rtc_time_capture0;
	RAM_INTRTCIC1_ISR = &r_rtc_time_capture1;
	RAM_INTRTCIC2_ISR = &r_rtc_time_capture2;

	RAM_INTDSADZC0_ISR=&r_dsadzc0_interrupt;
	RAM_INTDSADZC1_ISR=&r_dsadzc1_interrupt;

	RAM_INTMACLOF_ISR=&r_macl_interrupt;
	RAM_INTRTCRPD_ISR=&r_rtc_rpd_interrupt;

	RAM_INTLVDVDD_ISR=&r_lvdvdd_interrupt;
	RAM_INTLVDVBAT_ISR=&r_lvdvbat_interrupt;
	RAM_INTLVDVRTC_ISR=&r_lvdvrtc_interrupt;
	RAM_INTLVDEXLVD_ISR=&r_lvdexlvd_interrupt;

	RAM_INTDSAD_ISR=&r_dsadc_interrupt_asm;


	//extern  void (*RAM_INTSRE3_ISR)();
	//extern  void (*RAM_INTOSDC_ISR)();
	//extern  void (*RAM_INTFL_ISR)();

	//extern  void (*RAM_INTIT10_ISR)();
	//extern  void (*RAM_INTIT11_ISR)();

	//extern  void (*RAM_INTSRE2_ISR)();
	//extern  void (*RAM_INTCR_ISR)();
	//extern  void (*RAM_INTAES_ISR)();
	//extern  void (*RAM_INTSRE0_ISR)();
	//extern  void (*RAM_INTSRE1_ISR)();
	//extern  void (*RAM_INTFM_ISR)();
	//extern  void (*RAM_INTRTCALM_ISR)();
	//extern  void (*RAM_INTIT_ISR)();
	//extern  void (*RAM_INTKR_ISR)();

	//extern  void (*RAM_INTDSAD_ISR)();
	//extern  void (*RAM_INTTM04_ISR)();
	//extern  void (*RAM_INTTM05_ISR)();
	//extern  void (*RAM_INTTM06_ISR)();
	//extern  void (*RAM_INTTM07_ISR)();
	//extern  void (*RAM_INTIT01_ISR)();



	// (*RAM_INTLVI_ISR)();
#endif
}

void checkRTC(void);

void checkRTC(void)
{
// we will come here only when the rtc has stopped working and we are in POWERMGMT_MODE1
// we need to determine the best estimate of rtc time
// we can presume that the rtc struct is sound and compare the time with ctldata, and the capturedRTC

	ext_rtc_t temp_rtc;
	ext_rtc_t lastBillDate;
	int32_t temp32;
	
// let us just set the rtc to the ctldata values
	rtc.second = 0;
	rtc.minute = ctldata.minute;
	rtc.hour = ctldata.hour;
	rtc.date=ctldata.date;
	rtc.month = ctldata.month;
	rtc.year = ctldata.year;
	
	

/*	
	temp32 = diffInSecondsBetween(rtc, temp_rtc);	// rtc is the later date, temp_rtc is the earlier date
	if(temp32==0)
	{// here the rtc time is less than the ctldata time - something is wrong
	// we should check the ctldata time with the last bill date
		ConvertBackToRTC(&lastBillDate, &g_Class03_BillingDate);
		temp32 = diffInSecondsBetween(temp_rtc, lastBillDate)/60; // the difference is now in minutes
		if(temp32==0)
		{// here ctldata is not reliable - set the rtc to lastBillDate as a last resort or the meter start date
			if(Common_Data.bpcount==0)
			{// no bill has been made
				rtc = Common_Data.meterStartDate;
			}
			else
				rtc = lastBillDate;
		}
		else
		{// here ctldata date time is later than lastBillDate - we must check the validity - (not greater than two months i.e. 62*24*60 = 89280)
			if(temp32<89280)
			{
				rtc = temp_rtc;
			}
			
		}
	
	// we should check the lastBillDate - g_Class03_BillingDate
		ConvertBackToRTC(&temp_rtc, &g_Class03_BillingDate);
		temp32 = diffInSecondsBetween(rtc, temp_rtc);
		if(temp32==0)
		{// here the rtc time is less than the lastBillDate which is very serious
		}
	}
*/	
}

void main(void)
{
// for debug only - please comment under actual use	
	uint16_t            startup_count = 0;
	powermgmt_mode_t    power_mode;
	int8_t retval;

	setInterruptPointers();
	
// debug start


// debug end 
	
  if (PORSR == 0)
  {// here reset has been caused by a power on reset
    PORSR = 0x01;
//    g_softreset_flag = 0;
  }
  else
  {// here reset has been caused by others
//    g_softreset_flag = 1;
  }

	g_softreset_flag = 1;	// this indicates that meter has been reset
	
	gWhichPort=3;	// RJ11 port

  // Start the meter 
  while (startup() != STARTUP_OK)
  {
      startup_count++;
      R_WDT_Restart();
  }

#ifdef LCD_COUNT_UP	
	LCD_ClearAll();
  LCD_DisplayDigit(1, 1);
#endif

	readRTC();
#ifdef LCD_COUNT_UP	
  LCD_DisplayDigit(2, 2);
#endif

#ifdef FREQ_DIST_CHECK
	initFreqDisturbance();	// initialise the function
#endif
//		asendbare((uint8_t *)"Reset\r\n");  	

	perform_E2ROM_Test(1);

	powerUpDataCheck(0);

#ifdef LCD_COUNT_UP	
  LCD_DisplayDigit(5, 5);
#endif

	
	do_initial_things(); // if meter starts from reset then this function will NOT be executed in powermgmt
#ifdef LCD_COUNT_UP	
  LCD_DisplayDigit(6, 6);
#endif
	
	init_done=1;

	disp_state = 255;	
	disp_timer = disp_autoscroll_interval;	

	
//	gWhichPort=2;	// optical port
	asendbare((uint8_t *)"Entering Loop\r\n");  	

	if(CALSW==0)
		calswitch=0;	// user
	else
		calswitch=1;	// production

  // Start-up OK     
  while (1)
  {
	  R_WDT_Restart();
		
		// Power management control
		POWERMGMT_PollingProcessing();
		power_mode = POWERMGMT_GetMode();	// actual function to be used
		if (power_mode == POWERMGMT_MODE4)
    {
      // Enter Stop Mode
      NOP();
      NOP();
      NOP();
      STOP();
      NOP();
      NOP();
      NOP();
      R_WDT_Restart();
		} 
    else if (power_mode == POWERMGMT_MODE3)
		{
#if (defined(GUJARAT_BI_DIR_METER)||defined(GUJARAT_UNI_DIR_METER)||defined(GUJARAT_UNI_DIR_LTCT_METER)||defined(GUJARAT_BIDIR_LTCT_METER))
			if(calswitch!=0)
			{
				handleDisplay_Production();
			}
			else
			{
				if(disp_timer>=disp_autoscroll_interval)
				{				
					disp_timer=0;
					displayRepeatParameters();
					sub_disp_state++;
	#ifdef BIDIR_METER						
					if(sub_disp_state>1)	// show only the first two parameters
	#else
					if(sub_disp_state>2)	// show only the first three parameters
	#endif						
						sub_disp_state=0;						
				}
			}
#else
	#ifdef POWER_OFF_DISPLAY_MODE_PUSHBUTTON
			g_display_mode=1;
	#else
			g_display_mode=0;
	#endif		
			handleDisplay_Gujarat(0);
#endif

      R_WDT_Restart();
			DLMS_PollingProcessing();
			agetmsg();
      NOP();
      NOP();
      NOP();
			if(sleepCommMode==0)
			{
      	STOP();
			}
      NOP();
      NOP();
      NOP();
      R_WDT_Restart();

		}		
    else if (power_mode == POWERMGMT_MODE1)
    {
			KEY_PollingProcessing();
			EVENT_PollingProcessing();
			DEBUG_PollingProcessing();
			
			if(energyIncrementationFlag!=0)
			{
				energy_incrementation();
				energyIncrementationFlag=0;
			}
            
			if((genflags&MINUTES_FLAG)!=0)
			{
				genflags&=~MINUTES_FLAG;
				per_minute_activity();
			}

			per_second_activity();	// this function is called only when power is there
			
			if((genflags&RTC_IS_STOPPED)!=0)
			{
				checkRTC();	// this function will copy the rtc values from ctldata
				restoreRTC_Date_Time();	// this will update and start the rtc
				genflags&=~RTC_IS_STOPPED;
			}

#ifdef USE_DLMS
			DLMS_PollingProcessing();
			agetmsg();
#else
			agetmsg();
#endif
			wre2rom();
			
      R_WDT_Restart();   
			
    }
		else
		{
      R_WDT_Restart();        
		}
		
		// we will come here in all modes

		if((power_mode == POWERMGMT_MODE3)||(power_mode == POWERMGMT_MODE4))
		{
			performRTC_BatteryTest();
		}
		else
		{
			if(rtcTestFlag!=0)
			{
				MAKE_RTC_SUPPLY_HIGH;
				P_TIMER_Stop();
				R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  // Turn On one second interrupt to increment disp_timer
				rtcTestFlag=0;
			}
		}
		
    if(LVDEXLVDF == 0)	// EXLVD VOLTAGE > DETECTION THRESHOLD
    {
        g_powermgmt_flag.is_ac_low = 0;
    }
    else 
    {
        g_powermgmt_flag.is_ac_low = 1;
    }
  }
}

void restoreRTC_Date_Time(void)
{// This function is called only in main
// the rtc value has been captured before conducting the test

	rtc_calendarcounter_value_t ren_rtc;	// this structure is used to read the renesas rtc
// the following function is not needed as we are loading the structure with values from the rtc struct	
//	R_RTC_Get_CalendarCounterValue(&ren_rtc);	// this way we get to read the RTC - careful about BCD data
	ren_rtc.rseccnt = RTChex2bcd(rtc.second);
	ren_rtc.rmincnt = RTChex2bcd(rtc.minute);
	ren_rtc.rhrcnt = RTChex2bcd(rtc.hour);
	ren_rtc.rdaycnt=RTChex2bcd(rtc.date);
	ren_rtc.rmoncnt=RTChex2bcd(rtc.month);
	ren_rtc.ryrcnt= ((int16_t)RTChex2bcd(rtc.year))&0x00FF;
	R_RTC_Set_CalendarCounterValue(ren_rtc);	// now set the calendar counter - time will get changed
}

void performRTC_BatteryTest(void)
{
// we come here when we are in POWERMGMT_MODE3 or POWERMGMT_MODE4 
	if(Common_Data.rtc_failed==0)
	{// test is to be performed only when Common_Data.rtc_failed is 0
		if(rtcTestFlag==0)
		{
			if(rtcTestCounter==0)
			{
				rtcTestFlag=1;
				readRTC();	// take a backup reading
				R_RTC_Set_ConstPeriodInterruptOff();
				MAKE_RTC_SUPPLY_LOW;
				P_TIMER_Start();
			}
		}
		else
		{// here the rtcTestFlag is 1 and hence the test is in progress
			if(0==POWERMGMT_IsVRTCLow())
			{// battery is good
				if(rtcTestCounter>4)
				{// battery has not failed for 5 seconds - we can stop the test
					MAKE_RTC_SUPPLY_HIGH;
					P_TIMER_Stop();
					R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  // Turn On one second interrupt to increment disp_timer
					rtcTestFlag=0; // we can assume that rtc is ok, no point checking the rtc time
				}
			}
			else
			{// here RTC VOLTAGE < DETECTION THRESHOLD - battery appears to be bad - stop the test immediately
				MAKE_RTC_SUPPLY_HIGH;
				R_WDT_Restart();	// Be safe

				restoreRTC_Date_Time();	// load the previously read value into the rtc
				P_TIMER_Stop();
				R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);  // Turn On one second interrupt to increment disp_timer
				rtcTestFlag=0;
				
				Common_Data.rtc_failed=1;
				Common_Data.chksum=calcChecksum((uint8_t*)&Common_Data,sizeof(Common_Data)-1); 
				e2write_flags|=E2_W_IMPDATA;
			}
		}
	}
}

