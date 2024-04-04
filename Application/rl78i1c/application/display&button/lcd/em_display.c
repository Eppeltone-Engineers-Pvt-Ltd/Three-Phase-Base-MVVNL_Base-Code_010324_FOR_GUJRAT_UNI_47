// em_display.c - This is for regular boards 
// For Gujarat Board there is a separate display file em_display_Gujarat.c
// Driver layer 
#include "r_cg_macrodriver.h"
#include "typedef.h"            /* GCSE Standard definitions */
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "emeter3_structs.h"
 
// Application layer 
#include "platform.h"
#include "r_lcd_display.h"      /* LCD Display Application Layer */
#include "r_lcd_config.h"
#include "em_display.h"         /* EM Display Application Layer */
#include "event.h"
#include "powermgmt.h"
#include "pravakComm.h"
#include "emeter3_app.h"
#include "memoryOps.h"
#include "e2rom.h"
#include "tampers.h"

// Display tamper condition
#include "format.h"
#include "eeprom.h"
#include "em_core.h"


void display_autoScroll(void);
void display_onDemand(void);
void display_Third(void);

void EM_LCD_DisplayPOR(void);

static void EM_DisplayDate(void);
static void EM_DisplayInstantTime(void);

static void EM_LCD_DisplayStatus(void);

static uint8_t BCD2DEC(uint8_t BCD_number);

// utility icon functions
void LCD_DisplaykWSign(void);
void LCD_DisplaykVASign(void);
void LCD_DisplaykVArSign(void);
void LCD_DisplaykWhSign(void);
void LCD_DisplaykVAhSign(void);
void LCD_DisplaykVArhSign(void);
void lcd_display_date(uint8_t dat, uint8_t mth, uint8_t yr);
void lcd_display_time(uint8_t hr, uint8_t min, uint8_t sec);

#define RETURN_TO_AUTOSCROLL_COUNT	7500	// corresponds to 300 seconds or 5 minutes

uint8_t disp_state;
uint8_t disp_timer;
uint8_t disp_autoscroll_interval=10;


uint8_t			g_display_mode = 0;	// 0 - Autoscroll, 1- OnDemand, 2-ThirdDisplay, 3-Special Autoscroll
uint8_t     g_is_por = 0;

uint8_t gentimer;
uint8_t tamperTimer;

uint8_t kbscanflag;
uint8_t keyPressedCtr;
uint16_t keyNotPressedCtr;

//uint8_t keyNotPressedCtr;
uint8_t keyNotPressedCtr1;

uint8_t keyPressedCtr2;
uint8_t keyNotPressedCtr2;


//int32_t testCounter;
//int16_t innerCounter;


//typedef void (* __far fp_display_t)(void);

void KEY_40msInterruptCallback(void)
{
	kbscanflag=1;
}

void KEY_PollingProcessing(void)
{// this routine is effectively executed every 40 ms 
	if(kbscanflag==0)
		return;
		
	kbscanflag=0;
	
//	if(KEY1==0)
	if(TOPSW==0)	// switch is active low
//	if(KEY3!=0)	// switch is active high
	{// key is pressed
//		keyPressedCtr++;
		if(keyNotPressedCtr>5)
		{// key has been released for at least half a second
			keyNotPressedCtr=0;	// one has to release the key for the next press to be effective
			if(g_display_mode==0)
			{// we are in autoscroll mode
				g_display_mode=1;	// switch to displayOnDemand mode
				disp_state=0;
				disp_timer=disp_autoscroll_interval;
			}
			else if(g_display_mode==1)
			{// we are in onDemandMode and key has been pressed
				disp_state++;
				disp_timer=disp_autoscroll_interval;	// advance to next state
			}
			else if(g_display_mode==2)
			{// here we are in high resolution mode - exit
/*				
				if(disp_state==3)	// this number is to be changed as per number of states in high res
				{
					g_display_mode=0;	// switch to displayOnDemand mode
					disp_state=0;
				}
*/				
				disp_state++;
				disp_timer=disp_autoscroll_interval;
			}
		}
		else
		{//key continues to be pressed
			keyPressedCtr++;	// increments every 40 ms
			if(keyPressedCtr>125)
			{// key has been pressed for 5 seconds
				g_display_mode=2;	// Third display mode
				disp_state=0;
				disp_timer=disp_autoscroll_interval;
				keyPressedCtr=0;	// this is to prevent reentry into mode 2
			}
		}
	}
	else
	{// here key has been released
		keyPressedCtr=0;
		keyNotPressedCtr++;
		if(keyNotPressedCtr>10000)	// 10000 earlier
			keyNotPressedCtr=10000;
	}
	
	if(MIDDLESW==0)	// switch is active low
//	if(KEY1!=0)	// switch is active high
	{	
		if(keyNotPressedCtr1>10)
		{// key has been released for at least half a second
			keyNotPressedCtr1=0;	// one has to release the key for the next press to be effective
//			if(disp_state>=2)
//				disp_state=disp_state-2;
			if(disp_state>=1)
				disp_state=disp_state-1;
			disp_timer = disp_autoscroll_interval;
		}
	}
	else
	{// here key has been released
		keyNotPressedCtr1++;
		if(keyNotPressedCtr1>250)	// 10000 earlier
			keyNotPressedCtr1=250;
	}
	
	if(MDRST_SW==0)	// switch is active low
//	if(KEY2!=0)	// switch is active high
	{// key is pressed
		if(keyNotPressedCtr2>50)
		{// key has been released for at least two seconds
			keyNotPressedCtr2=0;	// one has to release the key for the next press to be effective
		}
		else
		{//key continues to be pressed
			keyPressedCtr2++;	// increments every 40 ms
			if(keyPressedCtr2>10)
			{// key has been pressed for 1 seconds
	      generate_bill(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second);
	      write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
	      e2write_flags&=~E2_W_CURRENTDATA;
				keyPressedCtr2=0;	// this is to prevent reentry into mode 2
			}
		}
	}
	else
	{// here key has been released
//		keyPressedCtr=0;
		keyNotPressedCtr2++;
		if(keyNotPressedCtr2>250)	// 10000 earlier
			keyNotPressedCtr2=250;
	}
	
	
}


void EM_RTC_DisplayInterruptCallback(void)
{//this function is called only in battery mode
  powermgmt_mode_t power_mode;
	
	disp_timer++;
	gentimer++;
//	seconds_flag=1;
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE3)
	{
		tamperTimer=0;
		if(P13.7==0)
		{
			keyPressedCtr++;
			if(keyPressedCtr>4)
			{
				sleepCommMode=1;
				sleepCommModeTimer=0;
			}
		}
		else
		{
			keyPressedCtr=0;
		}
	}
	else
	{
		tamperTimer++;
	}

	if(init_done!=0){
		per_second_activity();
	}
}


void handleDisplay(void)
{// disp_timer is incremented in the interrupt callback function

	if(g_display_mode==255)	// things are not ready yet
		return;

	if(keyNotPressedCtr>RETURN_TO_AUTOSCROLL_COUNT) // 40 ms scan - 25 times/sec - 300 seconds or 5 minutes
  {
		if(g_display_mode!=0){
			g_display_mode=0;	// return to autoscroll
			disp_state=0;
		}
  }
		
  if(disp_timer >= disp_autoscroll_interval)
  {
    disp_timer = 0;
    if(g_display_mode == 0)
	    display_autoScroll();
    else if(g_display_mode == 2)
			display_Third();
		else
      display_onDemand();
  }
}

void lcd_display_A(void)
{
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
}

void lcd_display_id(void)
{
  LCD_DisplayDigit(9, 5);
  LCD_DisplayDigit(8, LCD_CHAR_L);
}

void lcd_display_date(uint8_t dat, uint8_t mth, uint8_t yr)
{
  TIME_DATA_INFO disp_data;
  // Set time data to display 
  disp_data.hour  = dat;
  disp_data.min   = mth;
  disp_data.sec   = yr;
	//  LCD_DisplayTime(disp_data, 1);
  LCD_DisplayTime(disp_data, 0);
	// Display DATE sign 
  LCD_DisplaySpSign(S_DATE);
}		 

void lcd_display_time(uint8_t hr, uint8_t min, uint8_t sec)
{
  TIME_DATA_INFO disp_data;
  // Set time data to display 
  disp_data.hour  = hr;
  disp_data.min   = min;
  disp_data.sec   = sec;

  LCD_DisplayTime(disp_data, 0);
  LCD_DisplaySpSign(S_TIME);
}

void display_autoScroll(void)
{
	int32_t tempLong;
	uint16_t tempint;
	ext_rtc_t datetime;
	EM_PF_SIGN pf_sign;    
	uint8_t tempchar;
	uint8_t tmonth;

  R_WDT_Restart();
  LCD_ClearAll();

//	if(disp_state>1)
		EM_LCD_DisplayStatus();

//	disp_state=0;	// this is to ensure only magnet display
	
	if(disp_state>15)
		disp_state=0;

	switch(disp_state)
	{
    case 0:			// all on
			display_long_three_dp(myenergydata.kwh);
			LCD_DisplaykWhSign();
      disp_autoscroll_interval = 10;
      disp_state++;
      break;

    case 1:			// all on
			display_long_three_dp(myenergydata.kvah);
			LCD_DisplaykVAhSign();
      disp_state++;
      break;

    case 2:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplaykWSign();
      disp_state++;
     	break;

    case 3:		// Instant Load in kVA
			tempLong = getInstantaneousParams(APP_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplaykVASign();
      disp_state++;
     break;

    case 4:		// Current MD in kW
			display_long_three_dp(mymddata.mdkw);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
      disp_state++;
     	break;

    case 5:		// Current MD date
			lcd_display_date(mymddata.mdkw_datetime.date,mymddata.mdkw_datetime.month,mymddata.mdkw_datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
      disp_state++;
     	break;
		 
    case 6:		// Current MD time
			lcd_display_time(mymddata.mdkw_datetime.hour,mymddata.mdkw_datetime.minute,mymddata.mdkw_datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
      disp_state++;
     	break;

    case 7:		// Current MD in kVA
			display_long_three_dp(mymddata.mdkva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
      disp_state++;
     	break;

    case 8:		// Current MD KVA date
			lcd_display_date(mymddata.mdkva_datetime.date,mymddata.mdkva_datetime.month,mymddata.mdkva_datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
      disp_state++;
     	break;
		 
    case 9:		// Current MD KVA time
			lcd_display_time(mymddata.mdkva_datetime.hour,mymddata.mdkva_datetime.minute,mymddata.mdkva_datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
      disp_state++;
     	break;

// previous month md in kw and kva with date and time

    case 10:		// L1 - MD KW
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+90,(uint8_t*)&tempint,2);
      display_long_three_dp(tempint);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, 1);
      disp_state++;
     break;

    case 11:		// L1 - MD KW date
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+92,(uint8_t*)&datetime,8);
			lcd_display_date(datetime.date,datetime.month,datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, 1);
      disp_state++;
     break;
		 
    case 12:		// L1 - MD KW time
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+92,(uint8_t*)&datetime,8);
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, 1);
      disp_state++;
     break;

    case 13:		// L1 - MD KVA
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+180,(uint8_t*)&tempint,2);
      display_long_three_dp(tempint);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, 1);
      disp_state++;
     break;

    case 14:		// L1 - MD date
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+182,(uint8_t*)&datetime,8);
			lcd_display_date(datetime.date,datetime.month,datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, 1);
      disp_state++;
     break;
		 
    case 15:		// L1 - MD time
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+182,(uint8_t*)&datetime,8);
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, 1);
			disp_state=0;
     break;
	}
}

void display_onDemand(void)
{
	
	toddata_t temptoddata;
  powermgmt_mode_t power_mode;
	int32_t tempLong;
	int32_t tempLong1;
	int32_t tempLong2;
	uint16_t tempint;
	ext_rtc_t datetime;
	EM_PF_SIGN pf_sign;    
	uint8_t tempchar;
	uint8_t tmonth;

  R_WDT_Restart();
  LCD_ClearAll();

	if(disp_state>72)
		disp_state=72;

	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if ((power_mode == POWERMGMT_MODE1)&&(disp_state!=0))
		EM_LCD_DisplayStatus();

	switch(disp_state)
	{
    case 0:			// all on
      LCD_DisplayAll();
      disp_autoscroll_interval = 10;
      break;

    case 1:		// date
			EM_DisplayDate();
     break;

    case 2:		// time
			EM_DisplayInstantTime();
      disp_autoscroll_interval = 1;
     break;

    case 3:		// meter number
			LCD_DisplayIntWithPos(Common_Data.meterNumber, LCD_LAST_POS_DIGIT);
			lcd_display_id();
      disp_autoscroll_interval = 10;
     break;
		
    case 4:
			display_long_three_dp(myenergydata.kwh);
			LCD_DisplaykWhSign();
      break;

    case 5:
			display_long_three_dp(myenergydata.kvah);
			LCD_DisplaykVAhSign();
      break;

    case 6:		// total kvarh - lag
			display_long_three_dp(myenergydata.kvarh_lag);
			LCD_DisplaykVArhSign();
 	    LCD_DisplayDigit(9, LCD_CHAR_L);
 	    LCD_DisplayDigit(8, LCD_CHAR_G);
     break;

    case 7:		// total kvarh - lead
			display_long_three_dp(myenergydata.kvarh_lead);
			LCD_DisplaykVArhSign();
 	    LCD_DisplayDigit(9, LCD_CHAR_L);
 	    LCD_DisplayDigit(8, LCD_CHAR_D);
      disp_state++;
     break;
		
    case 8:		// Current MD in kW
			display_long_three_dp(mymddata.mdkw);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
     	break;

    case 9:		// Current MD KW date
			lcd_display_date(mymddata.mdkw_datetime.date,mymddata.mdkw_datetime.month,mymddata.mdkw_datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
     	break;
		 
    case 10:		// Current MD KW time
			lcd_display_time(mymddata.mdkw_datetime.hour,mymddata.mdkw_datetime.minute,mymddata.mdkw_datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
     	break;

    case 11:		// Current MD in kVA
			display_long_three_dp(mymddata.mdkva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
     	break;

    case 12:		// Current MD KVA date
			lcd_display_date(mymddata.mdkva_datetime.date,mymddata.mdkva_datetime.month,mymddata.mdkva_datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
     	break;
		 
    case 13:		// Current MD KVA time
			lcd_display_time(mymddata.mdkva_datetime.hour,mymddata.mdkva_datetime.minute,mymddata.mdkva_datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
     	break;

// previous six months md in kw and kva with date and time

    case 14:		// L1 - MD KW
    case 20:		// L2 - MD KW
    case 26:		// L3 - MD KW
    case 32:		// L4 - MD KW
    case 38:		// L5 - MD KW
    case 44:		// L6 - MD KW
      tempchar = ((disp_state-14)/6)+1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+90,(uint8_t*)&tempint,2);
      display_long_three_dp(tempint);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, tempchar);
     break;

    case 15:		// L1 - MD KW date
    case 21:		// L2 - MD KW date
    case 27:		// L3 - MD KW date
    case 33:		// L4 - MD KW date
    case 39:		// L5 - MD KW date
    case 45:		// L6 - MD KW date
      tempchar = ((disp_state-15)/6)+1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+92,(uint8_t*)&datetime,8);
			lcd_display_date(datetime.date,datetime.month,datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, tempchar);
     break;
		 
    case 16:		// L1 - MD KW time
    case 22:		// L2 - MD KW time
    case 28:		// L3 - MD KW time
    case 34:		// L4 - MD KW time
    case 40:		// L5 - MD KW time
    case 46:		// L6 - MD KW time
      tempchar = ((disp_state-16)/6)+1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+92,(uint8_t*)&datetime,8);
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, tempchar);
     break;

    case 17:		// L1 - MD KVA
    case 23:		// L2 - MD KVA
    case 29:		// L3 - MD KVA
    case 35:		// L4 - MD KVA
    case 41:		// L5 - MD KVA
    case 47:		// L6 - MD KVA
      tempchar = ((disp_state-17)/6)+1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+180,(uint8_t*)&tempint,2);
      display_long_three_dp(tempint);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, tempchar);
     break;

    case 18:		// L1 - MD date
    case 24:		// L2 - MD date
    case 30:		// L3 - MD date
    case 36:		// L4 - MD date
    case 42:		// L5 - MD date
    case 48:		// L6 - MD date
      tempchar = ((disp_state-18)/6)+1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+182,(uint8_t*)&datetime,8);
			lcd_display_date(datetime.date,datetime.month,datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, tempchar);
     break;
		 
    case 19:		// L1 - MD time
    case 25:		// L2 - MD time
    case 31:		// L3 - MD time
    case 37:		// L4 - MD time
    case 43:		// L5 - MD time
    case 49:		// L6 - MD time
      tempchar = ((disp_state-19)/6)+1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+182,(uint8_t*)&datetime,8);
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, tempchar);
     break;
		
    case 50:		// reset count
			LCD_DisplayIntWithPos(Common_Data.reset_count, LCD_LAST_POS_DIGIT);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
     break;

    case 51:		// Rising MD in kW
	    tempchar=60/(int32_t)Common_Data.mdinterval;
			tempLong = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);
			if(tempLong<0)
				tempLong=0;
			display_long_three_dp(tempLong);	// rising MD
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
// we can also display the minutes elapsed
			tempchar = rtc.minute - ((rtc.minute/Common_Data.mdinterval)*Common_Data.mdinterval); 
  		LCD_DisplayDigit(9, tempchar/10);
  		LCD_DisplayDigit(8, tempchar%10);
      disp_autoscroll_interval = 1;
      break;

    case 52:		// Rising MD in kVA
	    tempchar=60/(int32_t)Common_Data.mdinterval;
			tempLong = ((myenergydata.kvah-ctldata.kvah_last_saved)*(int32_t)tempchar);
			if(tempLong<0)
				tempLong=0;
			display_long_three_dp(tempLong);	// rising MD
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
// we can also display the minutes elapsed
			tempchar = rtc.minute - ((rtc.minute/Common_Data.mdinterval)*Common_Data.mdinterval); 
  		LCD_DisplayDigit(9, tempchar/10);
  		LCD_DisplayDigit(8, tempchar%10);
      disp_autoscroll_interval = 1;
      break;
		
		
    case 53:		// Voltage - R
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
      disp_autoscroll_interval = 1;
      break;

    case 54:		// Voltage - Y
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
      break;

    case 55:		// Voltage - B
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
	    break;

    case 56:		// Current - R
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			lcd_display_A();
      break;

    case 57:		// Current - Y
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			lcd_display_A();
      break;

   	case 58:		// Current - B
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			lcd_display_A();
      break;

    case 59:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplaykWSign();
     break;

    case 60:		// Instant Load in kVA
			tempLong = getInstantaneousParams(APP_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplaykVASign();
      break;
			
    case 61:		// pf total with sign
    	pf_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,LINE_TOTAL);
			tempLong = getInstantaneousParams(IPFS,LINE_TOTAL)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_F);
			if (pf_sign == PF_SIGN_LEAD_C)
			{
			    LCD_DisplayDigit(1, LCD_CHAR_C);
			}
			else if (pf_sign == PF_SIGN_LAG_L)
			{
			    LCD_DisplayDigit(1, LCD_CHAR_L);
			}
			else if (pf_sign == PF_SIGN_UNITY)
			{
			    LCD_ClearDigit(1);
			}
		 break;
			
    case 62:		// average pf of current month
			if(Common_Data.reset_count==0)
			{
				tempLong = (myenergydata.kwh*1000)/myenergydata.kvah;
			}
			else
			{//read last months kwh and kvah
	      tempchar = 1;
	      tmonth = decrementMonth(tempchar);
	      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+10,(uint8_t*)&tempLong1,4);
	      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+54,(uint8_t*)&tempLong2,4);
				tempLong = ((myenergydata.kwh-tempLong1)*1000)/(myenergydata.kvah - tempLong2);
			}
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_F);
	    LCD_DisplayDigit(1, LCD_CHAR_A);
	    LCD_DisplayDigit(2, LCD_CHAR_V);
			break;
			
		case 63:		// Line Frequency
			tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
			display_long_two_dp(tempLong);
	    LCD_DisplayDigit(9, LCD_CHAR_H);
	    LCD_DisplayDigit(8, 2);
     break;

    case 64:		// month and year of manufacture
			LCD_DisplayDigit(2, Common_Data.batchmonth/10);
			LCD_DisplayDigit(3, Common_Data.batchmonth%10);
			LCD_DisplayDigit(4, LCD_MINUS_SIGN);
			LCD_DisplayDigit(5, Common_Data.batchyear/10);
			LCD_DisplayDigit(6, Common_Data.batchyear%10);
      disp_autoscroll_interval = 10;
		 break;
		
    case 65:			// TOD1 - kwh
			tempchar=(disp_state-65);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kwh+=zone_kwh_last_saved;
			}				
			display_long_three_dp(temptoddata.kwh);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 1);
      break;

    case 66:		// TOD2 - kwh
			tempchar=(disp_state-65);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kwh+=zone_kwh_last_saved;
			}				
			display_long_three_dp(temptoddata.kwh);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 2);
     break;
	
    case 67:		// TOD3	- kwh
			tempchar=(disp_state-65);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kwh+=zone_kwh_last_saved;
			}				
			display_long_three_dp(temptoddata.kwh);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 3);
     break;
	
    case 68:		// TOD4	- kwh
			tempchar=(disp_state-65);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kwh+=zone_kwh_last_saved;
			}				
			display_long_three_dp(temptoddata.kwh);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 4);
     break;
		
    case 69:			// TOD1 - kvah
			tempchar=(disp_state-69);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kvah+=zone_kvah_last_saved;
			}				
			display_long_three_dp(temptoddata.kvah);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 1);
      break;

    case 70:		// TOD2 - kvah
			tempchar=(disp_state-69);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kvah+=zone_kvah_last_saved;
			}				
			display_long_three_dp(temptoddata.kvah);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 2);
     break;
	
    case 71:		// TOD3	- kvah
			tempchar=(disp_state-69);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kvah+=zone_kvah_last_saved;
			}				
			display_long_three_dp(temptoddata.kvah);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 3);
     break;
	
    case 72:		// TOD4	- kvah
			tempchar=(disp_state-69);
  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&temptoddata, sizeof(temptoddata));
			if(tempchar==presentZone)
			{
				temptoddata.kvah+=zone_kvah_last_saved;
			}				
			display_long_three_dp(temptoddata.kvah);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(8, 4);
     break;
		
	}
}

void display_Third(void)
{
	int32_t temp_high_res_energy;
	
  R_WDT_Restart();
  LCD_ClearAll();

	EM_LCD_DisplayStatus();

	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>1)
		disp_state=1;
	
	switch(disp_state)
	{
    case 0:			// high resolution kwh
			temp_high_res_energy = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykWhSign();
      disp_autoscroll_interval = 1;
      break;
			
    case 1:			// high resolution kvah
			temp_high_res_energy = ((myenergydata.kvah%100000)*100)+(int32_t)high_resolution_kvah_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykVAhSign();
      disp_autoscroll_interval = 1;
      break;
	}
}



static void EM_LCD_DisplayStatus(void)
{
  powermgmt_mode_t power_mode;
	power_mode = POWERMGMT_GetMode();


	
	if(g_display_mode==0)
	{
		if(((disp_state==0)||(disp_state==255)))
		{
				return;
		}
	}

	if (power_mode == POWERMGMT_MODE3)
		return;
	
	if((g_TamperStatus&REVERSE_MASK)!=0)
    LCD_DisplaySpSign(S_REV);
	else
    LCD_ClearSpSign(S_REV);
	
	if((g_TamperStatus&MISSPOT_MASK)!=0)
	{// here some potenital is missing
    LCD_DisplaySpSign(S_NM);
	}
	else
    LCD_ClearSpSign(S_NM);
	
	if((g_TamperStatus&HALLSENS_MASK)!=0)
    LCD_DisplaySpSign(S_MAG);
	else
    LCD_ClearSpSign(S_MAG);
	
	if((g_TamperStatus&BYPASS_MASK)!=0)
    LCD_DisplaySpSign(S_T9);
	else
    LCD_ClearSpSign(S_T9);
/*
	if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) == 0)
    LCD_ClearSpSign(S_COPEN);
	else
    LCD_DisplaySpSign(S_COPEN);
*/	
		
	if((mytmprdata.tamperEvent_image&maskTable[NEUTRAL_DISTURBANCE])==0)
    LCD_ClearSpSign(S_N);
	else
    LCD_DisplaySpSign(S_N);

//Bits 0,1,2 - Missing R,Y,B
//Bits 3,4,5 - CT Open R,Y.B
// Bit 6 - Other fault
		
//	if(((fault_code&_BV(0))!=0)||((fault_code&_BV(3))!=0))
	if((fault_code&_BV(0))!=0)
	  LCD_ClearSpSign(S_R);		// R
	else
		LCD_DisplaySpSign(S_R);
	
//	if(((fault_code&_BV(1))!=0)||((fault_code&_BV(4))!=0))
	if((fault_code&_BV(1))!=0)
	  LCD_ClearSpSign(S_Y);		// Y
	else
		LCD_DisplaySpSign(S_Y);
	
//	if(((fault_code&_BV(2))!=0)||((fault_code&_BV(5))!=0))
	if((fault_code&_BV(2))!=0)
	  LCD_ClearSpSign(S_B);		// B
	else
		LCD_DisplaySpSign(S_B);
	
	if((fault_code&_BV(6))!=0)
	{
		if((gentimer%2)==0)
		{
	  	LCD_ClearSpSign(S_R);		// R
		  LCD_ClearSpSign(S_Y);		// Y
		  LCD_ClearSpSign(S_B);		// B
		}
	}
}



void LCD_DisplaykWSign(void)
{
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T3);
  LCD_DisplaySpSign(S_T4);

}

void LCD_DisplaykWhSign(void)
{
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T3);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T7);
}

void LCD_DisplaykVASign(void)
{
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
}

void LCD_DisplaykVArSign(void)
{
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
  LCD_DisplaySpSign(S_T6);
}

void LCD_DisplaykVAhSign(void)
{
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
  LCD_DisplaySpSign(S_T7);
}


void LCD_DisplaykVArhSign(void)
{
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
  LCD_DisplaySpSign(S_T6);
  LCD_DisplaySpSign(S_T7);
}
/*
static void EM_DisplayUnitCode(void)
{
    LCD_DisplayDigit(1, 3);
    LCD_DisplayDigit(2, LCD_CHAR_P);
    LCD_DisplayDigit(3, LCD_CHAR_H);
	
    //LCD_DisplayDigit(4, 0);
    //LCD_DisplayDigit(5, 0);
    //LCD_DisplayDigit(6, 2);
    //LCD_DisplayDigit(7, 4);
    LCD_DisplayDigit(4, LCD_CHAR_E);
    LCD_DisplayDigit(5, LCD_CHAR_E);
    LCD_DisplayDigit(6, LCD_CHAR_P);
    LCD_DisplayDigit(7, LCD_CHAR_L);
}

static void EM_LCD_DisplaySoftVersion(void)
{
    LCD_DisplayDigit(1, LCD_CHAR_A);
    LCD_DisplayDigit(2, LCD_CHAR_E);
    LCD_DisplayDigit(4, 1);
    LCD_DisplayDigit(5, 0);
    LCD_DisplayDigit(6, 0);
    LCD_DisplaySpSign(S_D6);	// Decimal point
    LCD_DisplayDigit(9, LCD_CHAR_B);
    LCD_DisplayDigit(8, 3);
}
*/
static void EM_DisplayInstantTime(void)
{
    rtc_calendarcounter_value_t data;
    TIME_DATA_INFO              disp_data;
    
    // Get RTC data
    R_RTC_Get_CalendarCounterValue(&data);
    
    // Set time data to display 
    disp_data.hour  = data.rhrcnt;
    disp_data.min   = data.rmincnt;
    disp_data.sec   = data.rseccnt;
    LCD_DisplayTime(disp_data, 1);
    
    // Display TIME sign 
    LCD_DisplaySpSign(S_TIME);
}

static void EM_DisplayDate(void)
{
    rtc_calendarcounter_value_t data;
    TIME_DATA_INFO disp_data;
    
    // Get RTC data
    R_RTC_Get_CalendarCounterValue(&data);
    
    // Set time data to display
    disp_data.hour  = data.rdaycnt;
    disp_data.min   = data.rmoncnt;
    disp_data.sec   = (uint8_t)data.ryrcnt;
    LCD_DisplayTime(disp_data, 1);
    
    // Display DATE sign
    LCD_DisplaySpSign(S_DATE);
}



static uint8_t BCD2DEC(uint8_t BCD_number)
{
    volatile uint8_t rts;

    rts = (uint8_t)(BCD_number & 0x0f);
    BCD_number = (uint8_t)(((BCD_number & 0xf0) >> 4) * 10);
    rts = (uint8_t)(rts + BCD_number);
    return rts; 
}

static float abs(float input)
{
    if (input < 0)
    {
        return (-input);
    }
    else
    {
        return (input);
    }
}

uint8_t LCD_DisplayTime(TIME_DATA_INFO time_info, uint8_t is_used_BCD)
{
    uint32_t    disp_data;
    uint8_t     is_zero_hour = 0;
    
    /* If input data is BCD type, convert it into normal type */
    if (is_used_BCD == 1)
    {
        time_info.sec  = BCD2DEC(time_info.sec);
        time_info.min  = BCD2DEC(time_info.min);
        time_info.hour = BCD2DEC(time_info.hour);
    }
    
    /* Display time on LCD */
    /* Check hour value to guarantee it differs than 0 */
    if (time_info.hour <= 0)
    {
        is_zero_hour = 1;
        /* Add 1 to hour to compensate displayed value */
        time_info.hour = 1;
    }
    
    disp_data  = time_info.hour * (uint32_t)10000;
    disp_data += time_info.min * (uint32_t)100;
    disp_data += time_info.sec;

    //LCD_DisplayIntWithPos(disp_data, 6);

    LCD_DisplayIntWithPos(disp_data, 7);	// ajay
	
    /* If the digit of hour less than 10 */
    if (is_zero_hour == 1)
    {
//        LCD_DisplayDigit(2, 0); /* Display 0 at lower digit of hour value */
        LCD_DisplayDigit(3, 0); // Display 0 at lower digit of hour value - ajay
    }
    if (time_info.hour < 10)
    {
//        LCD_DisplayDigit(1, 0); /* Display 0 at higher digit of hour value */
        LCD_DisplayDigit(2, 0); // Display 0 at higher digit of hour value - ajay
    }
    
    /* Clear the 8th digit on LCD */
    LCD_ClearDigit(8);
    
    /* Diplay ":" between hour:min:sec */
//    LCD_DisplaySpSign(S_D1);	// This meter has a single control for COLON.
    LCD_DisplaySpSign(S_D3);
    
    return LCD_INPUT_OK;
}

/***********************************************************************************************************************
* Function Name: void LCD_DisplayFloat(float32_t fnum)
* Description  : Display float number on LCD screen
* Arguments    : [fNum] displayed number
* Output       : none
* Return Value : LCD_INPUT_WRONG_ARGUMENT: length of integer part is more than 5
               : LCD_OK: length of input number is ok
***********************************************************************************************************************/
uint8_t LCD_DisplayFloat(float32_t fnum)
{
    /* Information of input number */
    uint8_t     i;          
    //uint8_t   sign;           /* Get sign of input number */
    //float         f_fra_part;     /* Integer and fraction part of fnum at float type */
    uint8_t     num_fra_digit;  /* Get the number of number on fractional part */
    
    /* Information for displaying process */
    uint8_t     disp_status = LCD_INPUT_OK;
    uint8_t     is_zero_point = 0;
    uint8_t     is_input_float_number = 0;  /* Check the input type */
    uint32_t    ref_value = 1;
    
    /* Check the validation of fnum */
    if (fnum > 9999999) /* Out of allowable range of float type
                         * supported by CA78K0 compiler */
    {
        disp_status = LCD_WRONG_INPUT_ARGUMENT;
    }
    
    /* Get the number of digit on fractional part */
    num_fra_digit = (LCD_LAST_POS_DIGIT - g_DecInfo.pos) + 1;
    
    /* Check if input number is flaoting type or integer type */
    //f_fra_part = fnum - (int32_t)fnum;
    is_input_float_number = 1;
    
    /* Check if the input number is on [0,1] */
    if ((fnum >= 0) && (fnum < 1))
    {
        is_zero_point = 1;
        /* Compensate data when displayed digit less than number of digit on fractional part */
        fnum += 1;          
    }
    if ((fnum > -1) && (fnum <0 ))
    {
        is_zero_point = 1;
        /* Compensate data when displayed digit less than number of digit on fractional part */
        fnum -= 1;          
    }
    
    /* Convert the floating type into integer type */
    if (is_input_float_number == 1)
    {
        for (i = 0; i < num_fra_digit; i++)
        {
            fnum *= 10;
            ref_value *= 10;
        }
    }
    
    /* The input number is less than 1/(10^(fra_digit)) */
    if ((abs(fnum) < (ref_value + 1)) && (is_zero_point == 1))
    {
        fnum = 100;
    }
    
    /* Display integer number */
    disp_status = LCD_DisplayIntWithPos((int32_t)fnum, LCD_LAST_POS_DIGIT);

    /* Display 0 if input number in [0,1] */
    if (is_zero_point == 1)
    {
        LCD_DisplayDigit(g_DecInfo.pos - 1, 0);
    }
    
    /* Display decimal point */
    if (is_input_float_number == 1)
    {
        LCD_DisplaySpSign(g_DecInfo.sign);
    }

    return disp_status;
}

uint8_t LCD_DisplayFloat3Digit(float32_t fnum)
{
    /* Information of input number */
    uint8_t     i;
    //uint8_t   sign;           /* Get sign of input number */
    //float         f_fra_part;     /* Integer and fraction part of fnum at float type */
    uint8_t     num_fra_digit;  /* Get the number of number on fractional part */
    
    /* Information for displaying process */
    uint8_t     disp_status = LCD_INPUT_OK;
    uint8_t     is_zero_point = 0;
    uint8_t     is_input_float_number = 0;  /* Check the input type */
    uint32_t    ref_value = 1;
    
    /* Check the validation of fnum */
    if (abs(fnum) < 0.001)
    {
        fnum = 0;
    }
    if (fnum >= 9999999) /* Out of allowable range of float type
                         * supported by CA78K0 compiler */
    {
        disp_status = LCD_WRONG_INPUT_ARGUMENT;
        fnum = fnum/1000;
    }
    
    /* Get the number of digit on fractional part */    
    num_fra_digit = (LCD_LAST_POS_DIGIT - g_DecInfo1.pos) + 1;
    
    /* Check if input number is flaoting type or integer type */
    //f_fra_part = fnum - (int32_t)fnum;
    is_input_float_number = 1;
    
    /* Check if the input number is on [0,1] */
    if ((fnum >= 0) && (fnum < 1))
    {
        is_zero_point = 1;
        /* Compensate data when displayed digit less than number of digit on fractional part */
        fnum += 1;          
    }
    if ((fnum > -1) && (fnum <0 ))
    {
        is_zero_point = 1;
        /* Compensate data when displayed digit less than number of digit on fractional part */
        fnum -= 1;          
    }
    
    /* Convert the floating type into integer type */
    if (is_input_float_number == 1)
    {
        for (i = 0; i < num_fra_digit; i++)
        {
            fnum *= 10;
            ref_value *= 10;
        }
    }   
    
    /* Display integer number */
    disp_status = LCD_DisplayIntWithPos((int32_t)fnum, LCD_LAST_POS_DIGIT);

    /* Display 0 if input number in [0,1] */
    if (is_zero_point == 1)
    {
        LCD_DisplayDigit(g_DecInfo1.pos - 1, 0);
    }
    
    /* Display decimal point */
    if (is_input_float_number == 1)
    {
        LCD_DisplaySpSign(g_DecInfo1.sign);
    }

    return disp_status;
}

uint8_t LCD_DisplayIntWithPos(long lNum, int8_t position)
{
    int8_t pos = position;
    int8_t sign;            // store the sign of value: positive, negative
    uint8_t is_disp_error = LCD_INPUT_OK;
    
    // check pos
    if ((pos > LCD_LAST_POS_DIGIT) || (pos < LCD_FIRST_POS_DIGIT))
    {
        return LCD_WRONG_INPUT_ARGUMENT;
    }
    
    // get the sign of value 
    sign = (lNum < 0) ? -1 : 1;
    lNum *= sign;               // Get absolusted value of fNum for displaying 
    
    // display all digit 
    do
    {
        LCD_DisplayDigit(pos--, (uint8_t)(lNum % 10));
        lNum /= 10;
    } while (lNum && pos >= LCD_FIRST_POS_DIGIT);
    
    if ((lNum != 0) && (pos < LCD_FIRST_POS_DIGIT)) // overflow
    {
        is_disp_error = LCD_WRONG_INPUT_ARGUMENT;
    }
    
    // display the sign if there are some remain digit in the main region 
    if (sign < 0)
    {
				if((pos <5)&&(pos > LCD_FIRST_POS_DIGIT))
            LCD_DisplayDigit(pos--, LCD_MINUS_SIGN);
		}
    return is_disp_error;
}

void lcd_display_decimal(uint8_t whichDecimal)
{
	switch(whichDecimal)
	{
		case 1:
			LCD_DisplaySpSign(S_D4);	// 1 decimals
			break;
	
		case 2:
			LCD_DisplaySpSign(S_D5);	// 2 decimals
			break;
	
		case 3:
			LCD_DisplaySpSign(S_D6);	// 3 decimals
			break;
	
		case 4:
			break;
	
		case 5:
    	LCD_DisplaySpSign(S_D2);	// 5 decimals
			break;
			
		default:
			break;
	}
}

void display_long_one_dp(int32_t templong)
{// this fellow displays a long and three digits in decimal 
  lcd_display_multiple_chars(0,2,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT);
  lcd_display_decimal(1);
}

void display_long_two_dp(int32_t templong)
{// this fellow displays a long and two digits in decimal 
  lcd_display_multiple_chars(0,3,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT);
  lcd_display_decimal(2);
}

void display_long_three_dp(int32_t templong)
{// this fellow displays a long and three digits in decimal 
  lcd_display_multiple_chars(0,4,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT);
  lcd_display_decimal(3);
}

void display_long_five_dp(int32_t templong)
{// this fellow displays a long and five digits in decimal 
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT);
  lcd_display_decimal(5);
}



void lcd_display_multiple_chars(uint8_t whichchar, uint8_t howmany, uint8_t unitsPosition)
{
  uint8_t pos = unitsPosition;

  while(howmany!=0 && pos>=1)
  {
    LCD_DisplayDigit(pos, whichchar);
    howmany--;
    pos--;
  }
}


void EM_LCD_DisplayPOR(void)
{
    g_is_por = 0x01;
}


/*
const fp_display_t fp_displayOnDemand[] = 
{
  LCD_DisplayAll,
  EM_DisplayDate,
  EM_DisplayInstantTime,
  NULL,
};

const fp_display_t fp_display[] = 
{
    LCD_DisplayAll,
//    EM_DisplayUnitCode,
//    EM_LCD_DisplaySoftVersion,
  EM_DisplayDate,
  EM_DisplayInstantTime,
	
    EM_LCD_DisplayInstantVolt_Phase_R,
    EM_LCD_DisplayInstantVolt_Phase_Y,
    EM_LCD_DisplayInstantVolt_Phase_B,
////    EM_LCD_DisplayInstantVolt_Total,
////    EM_LCD_DisplayInstantCurrent_Total,
////    EM_LCD_DisplayInstantCurrent_Neutral,
////    EM_LCD_DisplayPhaseCurrentSumVector,
    EM_LCD_DisplayPowerFactor_Phase_R,
    EM_LCD_DisplayPowerFactor_Phase_Y,
    EM_LCD_DisplayPowerFactor_Phase_B,
////    EM_LCD_DisplayPowerFactor_Total,
    EM_LCD_DisplayLineFrequency,
////    EM_LCD_DisplayActEnergy_Phase_R,
////    EM_LCD_DisplayActEnergy_Phase_Y,
////    EM_LCD_DisplayActEnergy_Phase_B,
//    EM_LCD_DisplayActEnergy_Total,
//    EM_LCD_DisplayReactEnergy_Phase_R,
//    EM_LCD_DisplayReactEnergy_Phase_Y,
//    EM_LCD_DisplayReactEnergy_Phase_B,
    EM_LCD_New_DisplayReactEnergy_Total,
//    EM_LCD_DisplayInstantReactPower_Total,
//    EM_LCD_DisplayAppEnergy_Phase_R,
//    EM_LCD_DisplayAppEnergy_Phase_Y,
//    EM_LCD_DisplayAppEnergy_Phase_B,
    EM_LCD_DisplayAppEnergy_Total,
//    //EM_LCD_DisplayActEnergyTariff_Phase_R,
//    //EM_LCD_DisplayActEnergyTariff_Phase_Y,
//    //EM_LCD_DisplayActEnergyTariff_Phase_B,
//    //EM_LCD_DisplayActEnergyTariff_Total,
//    //EM_LCD_DisplayAppEnergyTariff_Phase_R,
//    //EM_LCD_DisplayAppEnergyTariff_Phase_Y,
//    //EM_LCD_DisplayAppEnergyTariff_Phase_B,
//    //EM_LCD_DisplayAppEnergyTariff_Total,
//    EM_LCD_DisplayInstantActPower_Phase_R,
//    EM_LCD_DisplayInstantActPower_Phase_Y,
//    EM_LCD_DisplayInstantActPower_Phase_B,
    EM_LCD_DisplayInstantActPower_Total,
//    EM_LCD_DisplayInstantReactPower_Phase_R,
//    EM_LCD_DisplayInstantReactPower_Phase_Y,
//    EM_LCD_DisplayInstantReactPower_Phase_B,
    EM_LCD_DisplayInstantReactPower_Total,
    //EM_LCD_DisplayInstantFundamentalActPower,
//    EM_LCD_DisplayInstantAppPower_Phase_R,
//    EM_LCD_DisplayInstantAppPower_Phase_Y,
//    EM_LCD_DisplayInstantAppPower_Phase_B,
    EM_LCD_DisplayInstantAppPower_Total,
//		EM_LCD_DisplayActMaxDemand,
//		EM_LCD_DisplayActMaxDemandDate,
//		EM_LCD_DisplayActMaxDemandTime,
		EM_LCD_DisplayAppMaxDemand,
		EM_LCD_DisplayAppMaxDemandDate,
		EM_LCD_DisplayAppMaxDemandTime,
		
    //EM_LCD_DisplayActMaxDemand_Phase_R,
//    //EM_LCD_DisplayActMaxDemand_Phase_Y,
//    //EM_LCD_DisplayActMaxDemand_Phase_B,
//    //EM_LCD_DisplayAppMaxDemand_Phase_R,
//    //EM_LCD_DisplayAppMaxDemand_Phase_Y,
//    //EM_LCD_DisplayAppMaxDemand_Phase_B,
//    EM_LCD_DisplayVoltageTHD_Phase_R,
//    EM_LCD_DisplayVoltageTHD_Phase_Y,
//    EM_LCD_DisplayVoltageTHD_Phase_B,
//    EM_LCD_DisplayCurrentTHD_Phase_R,
//    EM_LCD_DisplayCurrentTHD_Phase_Y,
//    EM_LCD_DisplayCurrentTHD_Phase_B,
    NULL,
};
*/

/*
static void EM_DisplayUnitCode(void);
static void EM_LCD_DisplaySoftVersion(void);
static void EM_DisplayDate(void);
static void EM_DisplayInstantTime(void);

static void EM_LCD_DisplayInstantVolt_Phase_R(void);
static void EM_LCD_DisplayInstantVolt_Phase_Y(void);
static void EM_LCD_DisplayInstantVolt_Phase_B(void);
static void EM_LCD_DisplayInstantVolt_Total(void);

static void EM_LCD_DisplayInstantCurrent_Neutral(void);
static void EM_LCD_DisplayInstantCurrent_Total(void);

static void EM_LCD_DisplayPowerFactor_Phase_R(void);
static void EM_LCD_DisplayPowerFactor_Phase_Y(void);
static void EM_LCD_DisplayPowerFactor_Phase_B(void);
static void EM_LCD_DisplayPowerFactor_Total(void);

static void EM_LCD_DisplayLineFrequency(void);

static void EM_LCD_DisplayActEnergy_Phase_R(void);
static void EM_LCD_DisplayActEnergy_Phase_Y(void);
static void EM_LCD_DisplayActEnergy_Phase_B(void);
//static void EM_LCD_DisplayActEnergy_Total(void);

static void EM_LCD_DisplayReactEnergy_Phase_R(void);
static void EM_LCD_DisplayReactEnergy_Phase_Y(void);
static void EM_LCD_DisplayReactEnergy_Phase_B(void);
static void EM_LCD_DisplayReactEnergy_Total(void);
static void EM_LCD_New_DisplayReactEnergy_Total(void);

static void EM_LCD_DisplayAppEnergy_Phase_R(void);
static void EM_LCD_DisplayAppEnergy_Phase_Y(void);
static void EM_LCD_DisplayAppEnergy_Phase_B(void);
static void EM_LCD_DisplayAppEnergy_Total(void);

static void EM_LCD_DisplayInstantActPower_Phase_R(void);
static void EM_LCD_DisplayInstantActPower_Phase_Y(void);
static void EM_LCD_DisplayInstantActPower_Phase_B(void);
static void EM_LCD_DisplayInstantActPower_Total(void);

static void EM_LCD_DisplayInstantReactPower_Phase_R(void);
static void EM_LCD_DisplayInstantReactPower_Phase_Y(void);
static void EM_LCD_DisplayInstantReactPower_Phase_B(void);
static void EM_LCD_DisplayInstantReactPower_Total(void);

//static void EM_LCD_DisplayInstantFundamentalActPower(void);

static void EM_LCD_DisplayInstantAppPower_Phase_R(void);
static void EM_LCD_DisplayInstantAppPower_Phase_Y(void);
static void EM_LCD_DisplayInstantAppPower_Phase_B(void);
static void EM_LCD_DisplayInstantAppPower_Total(void);

static void EM_LCD_DisplayPhaseCurrentSumVector(void);

static void EM_LCD_DisplayVoltageTHD_Phase_R(void);
static void EM_LCD_DisplayVoltageTHD_Phase_Y(void);
static void EM_LCD_DisplayVoltageTHD_Phase_B(void);
static void EM_LCD_DisplayCurrentTHD_Phase_R(void);
static void EM_LCD_DisplayCurrentTHD_Phase_Y(void);
static void EM_LCD_DisplayCurrentTHD_Phase_B(void);

//static void EM_LCD_DisplayActEnergyTariff_Phase_R(void);
//static void EM_LCD_DisplayActEnergyTariff_Phase_Y(void);
//static void EM_LCD_DisplayActEnergyTariff_Phase_B(void);
//static void EM_LCD_DisplayActEnergyTariff_Total(void);

//static void EM_LCD_DisplayAppEnergyTariff_Phase_R(void);
//static void EM_LCD_DisplayAppEnergyTariff_Phase_Y(void);
//static void EM_LCD_DisplayAppEnergyTariff_Phase_B(void);
//static void EM_LCD_DisplayAppEnergyTariff_Total(void);

//static void EM_LCD_DisplayActMaxDemand(void);
//static void EM_LCD_DisplayActMaxDemandDate(void);
//static void EM_LCD_DisplayActMaxDemandTime(void);
//static void EM_LCD_DisplayActMaxDemand_Phase_R(void);
//static void EM_LCD_DisplayActMaxDemand_Phase_Y(void);
//static void EM_LCD_DisplayActMaxDemand_Phase_B(void);

static void EM_LCD_DisplayAppMaxDemand(void);
static void EM_LCD_DisplayAppMaxDemandDate(void);
static void EM_LCD_DisplayAppMaxDemandTime(void);
//static void EM_LCD_DisplayAppMaxDemand_Phase_R(void);
//static void EM_LCD_DisplayAppMaxDemand_Phase_Y(void);
//static void EM_LCD_DisplayAppMaxDemand_Phase_B(void);
*/




/*
static void EM_LCD_DisplayInstantVolt_Phase_R(void)
{
    float32_t volt;
    volt = EM_GetVoltageRMS(LINE_PHASE_R);
    LCD_DisplayFloat(volt);
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_R);
}

static void EM_LCD_DisplayInstantVolt_Phase_Y(void)
{
    float32_t volt;
    volt = EM_GetVoltageRMS(LINE_PHASE_Y);
    LCD_DisplayFloat(volt);
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_Y);
}

static void EM_LCD_DisplayInstantVolt_Phase_B(void)
{
    float32_t volt;
    volt = EM_GetVoltageRMS(LINE_PHASE_B);
    LCD_DisplayFloat(volt);
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_B);
}

static void EM_LCD_DisplayInstantVolt_Total(void)
{
    float32_t volt;
    volt = EM_GetVoltageRMS(LINE_TOTAL);
    LCD_DisplayFloat(volt);
    LCD_DisplaySpSign(S_T2);
}


static void EM_LCD_DisplayInstantCurrent_Neutral(void)
{
    float32_t current;
    current = EM_GetCurrentRMS(LINE_NEUTRAL);
    LCD_DisplayFloat(current);
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplayDigit(9, LCD_CHAR_N);
}

static void EM_LCD_DisplayInstantActPower_Total(void)
{
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	

	LCD_DisplayIntWithPos(total_active_power, LCD_LAST_POS_DIGIT);	
	LCD_DisplaySpSign(S_D6);	// 3 decimals
	
    LCD_DisplaySpSign(S_T1);
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_T3);
    LCD_DisplaySpSign(S_T4);
}

static void EM_LCD_DisplayInstantReactPower_Total(void)
{
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	

	LCD_DisplayIntWithPos(total_reactive_power_lag, LCD_LAST_POS_DIGIT);	
	LCD_DisplaySpSign(S_D6);	// 3 decimals
    
    LCD_DisplaySpSign(S_T1);
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplaySpSign(S_T6);
    
}
static void EM_LCD_DisplayAppMaxDemand(void)
{
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	LCD_DisplayIntWithPos(mymddata.mdkva,7);
	LCD_DisplaykVASign();
  LCD_DisplaySpSign(S_MD);
  LCD_DisplaySpSign(S_D6);
}

static void EM_LCD_DisplayAppMaxDemandDate(void)
{
  TIME_DATA_INFO disp_data;
  disp_data.hour  = mymddata.mdkva_datetime.date;
  disp_data.min   = mymddata.mdkva_datetime.month;
  disp_data.sec   = mymddata.mdkva_datetime.year;
  LCD_DisplayTime(disp_data, 0);
    
  LCD_DisplaySpSign(S_DATE);
	LCD_DisplaykVASign();
  LCD_DisplaySpSign(S_MD);
}

static void EM_LCD_DisplayAppMaxDemandTime(void)
{
  TIME_DATA_INFO disp_data;
  disp_data.hour  = mymddata.mdkva_datetime.hour;
  disp_data.min   = mymddata.mdkva_datetime.minute;
  disp_data.sec   = mymddata.mdkva_datetime.second;
  LCD_DisplayTime(disp_data, 0);
  LCD_DisplaySpSign(S_TIME);
	LCD_DisplaykVASign();
  LCD_DisplaySpSign(S_MD);
}

static void EM_LCD_DisplayInstantAppPower_Total(void)
{
	LCD_DisplayIntWithPos(total_apparent_power, LCD_LAST_POS_DIGIT);	
	LCD_DisplaySpSign(S_D6);	// 3 decimals
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
}

static void EM_LCD_DisplayLineFrequency(void)
{
    float32_t line_freq;
    line_freq = EM_GetLineFrequency(LINE_TOTAL);
    LCD_DisplayFloat(line_freq);
    LCD_DisplaySpSign(S_T7);
}

static void EM_LCD_DisplayPowerFactor_Phase_R(void)
{
    float32_t   pf;
    EM_PF_SIGN  pf_sign;
    
    pf = EM_GetPowerFactor(LINE_TOTAL);
    pf_sign = EM_GetPowerFactorSign(LINE_PHASE_R);
    LCD_DisplayFloat(pf);
    LCD_DisplayDigit(9, LCD_CHAR_P);
    LCD_DisplayDigit(8, LCD_CHAR_F);
    LCD_DisplaySpSign(S_R);
    
    if (pf_sign == PF_SIGN_LEAD_C)
    {
        LCD_DisplayDigit(1, LCD_CHAR_C);
    }
    else if (pf_sign == PF_SIGN_LAG_L)
    {
        LCD_DisplayDigit(1, LCD_CHAR_L);
    }
    else if (pf_sign == PF_SIGN_UNITY)
    {
        LCD_ClearDigit(1);
    }
}

static void EM_LCD_DisplayPowerFactor_Phase_Y(void)
{
    float32_t   pf;
    EM_PF_SIGN  pf_sign;
    
    pf = EM_GetPowerFactor(LINE_TOTAL);
    pf_sign = EM_GetPowerFactorSign(LINE_PHASE_Y);
    
    LCD_DisplayFloat(pf);
    
    LCD_DisplayDigit(9, LCD_CHAR_P);
    LCD_DisplayDigit(8, LCD_CHAR_F);
    
    LCD_DisplaySpSign(S_Y);
    
    if (pf_sign == PF_SIGN_LEAD_C)
    {
        LCD_DisplayDigit(1, LCD_CHAR_C);
    }
    else if (pf_sign == PF_SIGN_LAG_L)
    {
        LCD_DisplayDigit(1, LCD_CHAR_L);
    }
    else if (pf_sign == PF_SIGN_UNITY)
    {
        LCD_ClearDigit(1);
    }
}

static void EM_LCD_DisplayPowerFactor_Phase_B(void)
{
    float32_t   pf;
    EM_PF_SIGN  pf_sign;
    
    pf = EM_GetPowerFactor(LINE_TOTAL);
    pf_sign = EM_GetPowerFactorSign(LINE_PHASE_B);
    
    LCD_DisplayFloat(pf);
    
    LCD_DisplayDigit(9, LCD_CHAR_P);
    LCD_DisplayDigit(8, LCD_CHAR_F);
    
    LCD_DisplaySpSign(S_B);
    
    if (pf_sign == PF_SIGN_LEAD_C)
    {
        LCD_DisplayDigit(1, LCD_CHAR_C);
    }
    else if (pf_sign == PF_SIGN_LAG_L)
    {
        LCD_DisplayDigit(1, LCD_CHAR_L);
    }
    else if (pf_sign == PF_SIGN_UNITY)
    {
        LCD_ClearDigit(1);
    }
}

static void EM_LCD_DisplayPowerFactor_Total(void)
{
    float32_t   pf;
    EM_PF_SIGN  pf_sign;
    
    // Modify to get enough length for LCD if necessary 
    pf = EM_GetPowerFactor(LINE_TOTAL);
    pf_sign = EM_GetPowerFactorSign(LINE_TOTAL);
    
    // Display measured value
    LCD_DisplayFloat(pf);
    
    // Display "PF" sign 
    LCD_DisplayDigit(9, LCD_CHAR_P);
    LCD_DisplayDigit(8, LCD_CHAR_F);
    
    if (pf_sign == PF_SIGN_LEAD_C)
    {
        LCD_DisplayDigit(1, LCD_CHAR_C);
    }
    else if (pf_sign == PF_SIGN_LAG_L)
    {
        LCD_DisplayDigit(1, LCD_CHAR_L);
    }
    else if (pf_sign == PF_SIGN_UNITY)
    {
        LCD_ClearDigit(1);
    }
}

static void EM_LCD_DisplayActEnergy_Total(void)
{
// ajayjani
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	
	LCD_DisplayIntWithPos(myenergydata.kwh, LCD_LAST_POS_DIGIT);
	LCD_DisplaySpSign(S_D6);	// 3 decimals
	
  // Display "kWh" sign 
  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T3);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T7);
}


static void EM_LCD_New_DisplayReactEnergy_Total(void)
{
// ajayjain1
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	

//	LCD_DisplayIntWithPos(total_reactive_power_lag, LCD_LAST_POS_DIGIT);	
	LCD_DisplayIntWithPos(myenergydata.kvarh_lag, LCD_LAST_POS_DIGIT);
	LCD_DisplaySpSign(S_D6);	// 3 decimals
    
    LCD_DisplaySpSign(S_T1);
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplaySpSign(S_T6);
	  LCD_DisplaySpSign(S_T7);	// h
}


static void EM_LCD_DisplayReactEnergy_Total(void)
{
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	
//	LCD_DisplayIntWithPos(myenergydata.kvarh_lag, LCD_LAST_POS_DIGIT);
	LCD_DisplayIntWithPos(total_reactive_power_lag, LCD_LAST_POS_DIGIT);	
	LCD_DisplaySpSign(S_D6);	// 3 decimals
		
  // Display "kVArh" sign 
  LCD_DisplaySpSign(S_T1);	// k
  LCD_DisplaySpSign(S_T2);	// V
  LCD_DisplaySpSign(S_T4);	// /
  LCD_DisplaySpSign(S_T5);	// -\
  LCD_DisplaySpSign(S_T6);	// r
  LCD_DisplaySpSign(S_T7);	// h
}



static void EM_LCD_DisplayAppEnergy_Total(void)
{
  LCD_DisplayDigit(7, 0);
  LCD_DisplayDigit(6, 0);
  LCD_DisplayDigit(5, 0);
  LCD_DisplayDigit(4, 0);
	
	LCD_DisplayIntWithPos(myenergydata.kvah, LCD_LAST_POS_DIGIT);
	LCD_DisplaySpSign(S_D6);	// 3 decimals

  LCD_DisplaySpSign(S_T1);
  LCD_DisplaySpSign(S_T2);
  LCD_DisplaySpSign(S_T4);
  LCD_DisplaySpSign(S_T5);
  LCD_DisplaySpSign(S_T7);
}

static void EM_LCD_DisplayPhaseCurrentSumVector(void)
{
    float32_t vector = EM_GetPhaseCurrentSumVector();
    LCD_DisplayFloat(vector);
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplayDigit(9, 5); 
}

static void EM_LCD_DisplayVoltageTHD_Phase_R(void)
{
    float32_t thd = (float32_t)EM_GetVoltageTHD(LINE_PHASE_R);
    LCD_DisplayFloat(thd);
    
    LCD_DisplayDigit(9, LCD_CHAR_H);
    LCD_DisplayDigit(8, LCD_CHAR_D); 

    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_R);
}

static void EM_LCD_DisplayVoltageTHD_Phase_Y(void)
{
    float32_t thd = (float32_t)EM_GetVoltageTHD(LINE_PHASE_Y);
    LCD_DisplayFloat(thd);
    LCD_DisplayDigit(9, LCD_CHAR_H);
    LCD_DisplayDigit(8, LCD_CHAR_D); 
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_Y);
}

static void EM_LCD_DisplayVoltageTHD_Phase_B(void)
{
    float32_t thd = (float32_t)EM_GetVoltageTHD(LINE_PHASE_B);
    LCD_DisplayFloat(thd);
    LCD_DisplayDigit(9, LCD_CHAR_H);
    LCD_DisplayDigit(8, LCD_CHAR_D); 
    LCD_DisplaySpSign(S_T2);
    LCD_DisplaySpSign(S_B);
}

static void EM_LCD_DisplayCurrentTHD_Phase_R(void)
{
    float32_t thd = (float32_t)EM_GetCurrentTHD(LINE_PHASE_R);
    LCD_DisplayFloat(thd);
    LCD_DisplayDigit(9, LCD_CHAR_H);
    LCD_DisplayDigit(8, LCD_CHAR_D); 
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplaySpSign(S_R);
}

static void EM_LCD_DisplayCurrentTHD_Phase_Y(void)
{
    float32_t thd = (float32_t)EM_GetCurrentTHD(LINE_PHASE_Y);
    LCD_DisplayFloat(thd);
    LCD_DisplayDigit(9, LCD_CHAR_H);
    LCD_DisplayDigit(8, LCD_CHAR_D); 
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplaySpSign(S_Y);
}

static void EM_LCD_DisplayCurrentTHD_Phase_B(void)
{
    float32_t thd = (float32_t)EM_GetCurrentTHD(LINE_PHASE_B);
    LCD_DisplayFloat(thd);
    LCD_DisplayDigit(9, LCD_CHAR_H);
    LCD_DisplayDigit(8, LCD_CHAR_D); 
    LCD_DisplaySpSign(S_T4);
    LCD_DisplaySpSign(S_T5);
    LCD_DisplaySpSign(S_B);
}
*/


//static void EM_LCD_DisplayActEnergyTariff(void)
//{
//  float   act_energy;
//  uint8_t current_tariff;
//  uint8_t return_status;
    
//  /* Get the current active active tariff */
//  current_tariff = EM_GetActiveTariff();
    
//  /* Get active energy at active tariff */
//  return_status = EM_GetActiveEnergyTariff(LINE_TOTAL, current_tariff, &act_energy);
//  if (return_status == EM_OK)
//  {
//      /* Modify to get enough length for LCD if necessary */
//      /* TBD */
        
//      /* Display measured value */
//      return_status = LCD_DisplayFloat(act_energy);
//      if (return_status == LCD_WRONG_INPUT_ARGUMENT)
//      {
//          /* TODO: notice input value is out of range */
//      }
        
//      /* Display "kWh" sign */
//      LCD_DisplaySpSign(S_T1);
//      LCD_DisplaySpSign(S_T2);
//      LCD_DisplaySpSign(S_T3);
//      LCD_DisplaySpSign(S_T4);
//      LCD_DisplaySpSign(S_T7);
        
//      /* Display tariff number at digit 8 */
//      LCD_DisplayDigit(8, current_tariff);
//  }   
//}

//static void EM_LCD_DisplayAppEnergyTariff(void)
//{
//  float   app_energy;
//  uint8_t current_tariff;
//  uint8_t return_status;
    
//  /* Get the current active active tariff */
//  current_tariff = EM_GetActiveTariff();
    
//  /* Get active energy at active tariff */
//  return_status = EM_GetReactiveEnergyTariff(LINE_TOTAL, current_tariff, &app_energy);
//  if (return_status == EM_OK)
//  {
//      /* Modify to get enough length for LCD if necessary */
//      /* TBD */
        
//      /* Display measured value */
//      return_status = LCD_DisplayFloat(app_energy);
//      if (return_status == LCD_WRONG_INPUT_ARGUMENT)
//      {
//          /* TODO: notice input value is out of range */
//      }
        
//      /* Display "kVAh" sign */
//      LCD_DisplaySpSign(S_T1);
//      LCD_DisplaySpSign(S_T2);
//      LCD_DisplaySpSign(S_T4);
//      LCD_DisplaySpSign(S_T5);
//      LCD_DisplaySpSign(S_T7);
            
//      /* Display tariff number at digit 8 */
//      LCD_DisplayDigit(8, current_tariff);
//  }   
//}

//static void EM_LCD_DisplayAppMaxDemand(void)
//{
//  uint32_t    appMD;
//  uint8_t     return_status;
    
//  appMD = EM_GetApparentMaxDemand(LINE_TOTAL);
    
//  /* Modify to get enough length for LCD if necessary */
//  /* TBD */
    
//  /* Display measured value */
//  return_status = LCD_DisplayIntWithPos(appMD, 8);
//  if (return_status == LCD_WRONG_INPUT_ARGUMENT)
//  {
//      /* TODO: notice input value is out of range */
//  }
    
//  /* Display "VA" sign */
//  LCD_DisplaySpSign(S_T2);
//  LCD_DisplaySpSign(S_T4);
//  LCD_DisplaySpSign(S_T5);
    
//  /* Display "MD" sign on LCD screen */
//  LCD_DisplaySpSign(S_MD);
//}









