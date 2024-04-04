// em_display.c
// Driver layer 
#include "r_cg_macrodriver.h"
#include "typedef.h"            /* GCSE Standard definitions */
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "r_cg_sau.h"           /* CG Serial Driver */
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
#include "utilities.h"
#include "em_display_Kerala_Helper.h"

// Display tamper condition
#include "format.h"
#include "eeprom.h"
#include "em_core.h"
#include "r_dlms_obis_ic.h"	// for date_time_t
#include "r_dlms_data.h"	// date_time_t to ext_rtc_t conversion functions

void display_autoScroll(void);
void display_onDemand(void);
void display_Third(void);

void EM_LCD_DisplayPOR(void);

void EM_DisplayDate(void);
void EM_DisplayInstantTime(void);

void EM_LCD_DisplayStatus(void);

uint8_t BCD2DEC(uint8_t BCD_number);

// utility icon functions
void LCD_DisplaykWSign(void);
void LCD_DisplaykVASign(void);
void LCD_DisplaykVArSign(void);
void LCD_DisplaykWhSign(void);
void LCD_DisplaykVAhSign(void);
void LCD_DisplaykVArhSign(void);
void lcd_display_date(uint8_t dat, uint8_t mth, uint8_t yr);
void lcd_display_time(uint8_t hr, uint8_t min, uint8_t sec);
//void displayTamperName(uint8_t eventCode);	// this function is superseded in Kerala
void d_Tampers(void);

#define RETURN_TO_AUTOSCROLL_COUNT	7500	// corresponds to 300 seconds or 5 minutes

#ifdef BIDIR_METER
	#define NO_OF_SUB_DISPLAY_STATES 4	// BIDIR_METER has 4
#else
	#define NO_OF_SUB_DISPLAY_STATES 3	// Regular Gujarat Meter has 3
#endif


const uint8_t msg_nil_tamper[8] = {LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CHAR_N, LCD_CHAR_I, LCD_CHAR_L, LCD_CLEAR_DIGIT};
const uint8_t msg_yes_tamper[8] = {LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CHAR_Y, LCD_CHAR_E, 5, LCD_CLEAR_DIGIT};
/*
const uint8_t msg_miss_R[8]     = {LCD_CHAR_M, 1, 5, 5, LCD_CHAR_P, LCD_MINUS_SIGN, LCD_CHAR_R, LCD_CLEAR_DIGIT};
const uint8_t msg_miss_Y[8]     = {LCD_CHAR_M, 1, 5, 5, LCD_CHAR_P, LCD_MINUS_SIGN, LCD_CHAR_Y, LCD_CLEAR_DIGIT};
const uint8_t msg_miss_B[8]     = {LCD_CHAR_M, 1, 5, 5, LCD_CHAR_P, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CLEAR_DIGIT};
const uint8_t msg_over_V[8]     = {LCD_CHAR_O, LCD_CHAR_V, LCD_CHAR_E, LCD_CHAR_R, LCD_MINUS_SIGN, LCD_CHAR_V, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_under_V[8]    = {LCD_CHAR_U, LCD_CHAR_N, LCD_CHAR_D, LCD_CHAR_E, LCD_CHAR_R, LCD_MINUS_SIGN, LCD_CHAR_V, LCD_CLEAR_DIGIT};
const uint8_t msg_unbal_V[8]    = {LCD_CHAR_V, LCD_MINUS_SIGN, LCD_CHAR_U, LCD_CHAR_N, LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_L, LCD_CLEAR_DIGIT};
const uint8_t curr_rev_R[8]    = {LCD_CHAR_C, LCD_CLEAR_DIGIT, LCD_CHAR_R, LCD_CHAR_E, LCD_CHAR_V, LCD_MINUS_SIGN, LCD_CHAR_R, LCD_CLEAR_DIGIT};
const uint8_t curr_rev_Y[8]    = {LCD_CHAR_C, LCD_CLEAR_DIGIT, LCD_CHAR_R, LCD_CHAR_E, LCD_CHAR_V, LCD_MINUS_SIGN, LCD_CHAR_Y, LCD_CLEAR_DIGIT};
const uint8_t curr_rev_B[8]    = {LCD_CHAR_C, LCD_CLEAR_DIGIT, LCD_CHAR_R, LCD_CHAR_E, LCD_CHAR_V, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CLEAR_DIGIT};
const uint8_t curr_fail_R[8]    = {LCD_CHAR_C, LCD_CHAR_F, LCD_CHAR_A, 1, LCD_CHAR_L, LCD_MINUS_SIGN, LCD_CHAR_R, LCD_CLEAR_DIGIT};
const uint8_t curr_fail_Y[8]    = {LCD_CHAR_C, LCD_CHAR_F, LCD_CHAR_A, 1, LCD_CHAR_L, LCD_MINUS_SIGN, LCD_CHAR_Y, LCD_CLEAR_DIGIT};
const uint8_t curr_fail_B[8]    = {LCD_CHAR_C, LCD_CHAR_F, LCD_CHAR_A, 1, LCD_CHAR_L, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CLEAR_DIGIT};
const uint8_t msg_unbal_C[8]    = {LCD_CHAR_C, LCD_MINUS_SIGN, LCD_CHAR_U, LCD_CHAR_N, LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_L, LCD_CLEAR_DIGIT};
const uint8_t msg_bypass_C[8]    = {LCD_CHAR_C, LCD_CHAR_B, LCD_CHAR_Y, LCD_CHAR_P, LCD_CHAR_A, 5, 5, LCD_CLEAR_DIGIT};
const uint8_t msg_over_C[8]     = {LCD_CHAR_O, LCD_CHAR_V, LCD_CHAR_E, LCD_CHAR_R, LCD_MINUS_SIGN, LCD_CHAR_C, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_C_Open[8]     = {LCD_CHAR_C, LCD_MINUS_SIGN, LCD_CHAR_O, LCD_CHAR_P, LCD_CHAR_E, LCD_CHAR_N, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_Magnet[8]     = {LCD_CHAR_M, LCD_CHAR_A, LCD_CHAR_G, LCD_CHAR_N, LCD_CHAR_E, LCD_CHAR_T, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_ND[8]     = {LCD_CHAR_N, LCD_MINUS_SIGN, LCD_CHAR_D, 1, 5, LCD_CHAR_T, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
*/
const uint8_t Bat_OK[8]     = {LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_T, LCD_MINUS_SIGN, LCD_CHAR_G, LCD_CHAR_D, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t Bat_LO[8]     = {LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_T, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_D, LCD_CLEAR_DIGIT};

const uint8_t NVM_OK[8]     = {LCD_CHAR_N, LCD_CHAR_V, LCD_CHAR_M, LCD_MINUS_SIGN, LCD_CHAR_G, LCD_CHAR_D, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t NVM_BAD[8]    = {LCD_CHAR_N, LCD_CHAR_V, LCD_CHAR_M, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_D, LCD_CLEAR_DIGIT};

const uint8_t RTC_OK[8]     = {LCD_CHAR_R, LCD_CHAR_T, LCD_CHAR_C, LCD_MINUS_SIGN, LCD_CHAR_G, LCD_CHAR_D, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t RTC_BAD[8]    = {LCD_CHAR_R, LCD_CHAR_T, LCD_CHAR_C, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_D, LCD_CLEAR_DIGIT};


uint8_t disp_state;
uint8_t disp_timer;
uint8_t disp_autoscroll_interval=6;
uint8_t holdState;
uint8_t g_tamper_display_state;

uint8_t			g_display_mode = 0;	// 0 - Autoscroll, 1- OnDemand, 2-ThirdDisplay, 3-Special Autoscroll
//uint8_t			g_display_update_type =0;	// 0 - Autoincrement, 1- OnKeyPressOnly, 2-AutoIncrementEverysecond, 3-KeyPressIncrementEverySecond
uint8_t     g_is_por = 0;

uint8_t splTimer;
uint8_t gentimer;
uint8_t tamperTimer;

uint8_t comm_mode;
uint8_t commModeTimer;
uint8_t gLcdCheck;

uint8_t kbscanflag;
uint8_t keyPressedCtr;
uint16_t keyNotPressedCtr;

//uint8_t keyNotPressedCtr;
uint8_t keyPressedCtr1;
uint8_t keyNotPressedCtr1;

uint8_t keyPressedCtr2;
uint8_t keyNotPressedCtr2;

int16_t mode_timeout_ctr;
uint8_t	seconds_flag;
uint8_t sub_disp_state=255;
uint8_t avoidShowingRepeatParameters;
uint8_t alt_autoscroll_mode;
uint8_t autoscrollInButtonMode;

//int32_t testCounter;
//int16_t innerCounter;


//typedef void (* __far fp_display_t)(void);

void KEY_40msInterruptCallback(void)
{
	kbscanflag=1;
}

void makeDisplayReadyForAutoScroll(void)
{
		mode_timeout_ctr=0;
		avoidShowingRepeatParameters=0;
		g_display_mode=0;	//revert to autoscroll
		alt_autoscroll_mode=0;
		sub_disp_state=255;
		disp_state=255;
		disp_autoscroll_interval=6;
		autoscrollInButtonMode=0;
		gLcdCheck=0;
}

void EM_RTC_DisplayInterruptCallback(void)
{//this function is called only in battery mode
  powermgmt_mode_t power_mode;
	splTimer++;
	disp_timer++;
	gentimer++;
	seconds_flag=1;
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE3)
	{
		gujaratSecondsCounter++;
		tamperTimer=0;
#ifdef KEY_ACTIVE_HIGH
		if(P13.7!=0)
#else
		if(P13.7==0)
#endif		
		{
			keyPressedCtr++;
			if(keyPressedCtr>4)
			{
				sleepCommMode=1;
				sleepCommModeTimer=0;
				comm_mode=1;
				commModeTimer=0;
//-------------				
// This is to start the communication module	
  SAU0EN = 1U;
  SAU1EN = 1U;
	
// We must also Vcc2 (P57) On
	P5 |=0x80;	// P57=1 - 1000 0000
	
// TXD3(P85) must be output, RXD3(P84) input - already the case
// P85 must be 1
	P8|=0x20;	// 0010 0000 

// IRTXD(P56) must be output, IRRXD(P55) input - already the case
// P56 must be 1
	P5 |=0x40;	// P56=1 - 0100 0000
	
  R_UART2_Start();
	R_UART3_Start();
//-----------------				
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
		if(g_display_mode==0)
		{
			if(autoscrollInButtonMode!=0)
			{
				mode_timeout_ctr++;
				if(mode_timeout_ctr>59)	//  1 minute
				{
					autoscrollInButtonMode=0;	// get out of button mode and start scrolling
					disp_timer=disp_autoscroll_interval;
				}
			}
		}
		else
		{
			if(g_display_mode==1)
			{
				mode_timeout_ctr++;
				if(mode_timeout_ctr>59)	//  1 minute
				{
					makeDisplayReadyForAutoScroll();
				}
			}
			else
			{// we are in high res mode
				// for other modes - let it be thirty minutes - 
			 // in any case for pushbutton mode, after reaching last state it is supposed to go back to autoscroll
				mode_timeout_ctr++;
				if(mode_timeout_ctr>1797)	//  30 minutes - 1800 seconds
				{
					makeDisplayReadyForAutoScroll();
				}
			}
		}
	}

	if(init_done!=0){
		perSecondFlag=1;
	}
}

void CommunicationTimeoutCheck(void)
{
	if(comm_mode!=0)
	{
		commModeTimer++;
		if(commModeTimer>30)
		{
			commModeTimer=0;
			comm_mode=0;
		}
	}
}


#define MAX_STEP_ON_AUTOSCROLL_DISPLAY 21
#define MAX_STEP_ON_DEMAND_DISPLAY 10
#define MAX_STEP_ON_HIGH_RESOLUTION_DISPLAY 3

uint8_t mode_has_been_changed;
uint8_t fwdKeyPressed;
uint8_t revKeyPressed;
uint8_t someKeyPressed;

uint8_t fwdKeyPressedCtr;
uint8_t revKeyPressedCtr;
uint8_t bothKeysPressedCtr;
uint8_t noKeyPressedCtr;

uint8_t	fast_display_flag;
uint8_t	fast_display_counter;

uint8_t prevTopSW=1;
uint8_t prevMiddleSW=1;

int8_t scrollLock;

void handleDisplay_Gujarat(uint8_t a);
void changeDisplayMode(void);
//void displayRepeatParameters(void);

void changeDisplayMode(void)
{// both keys have been pressed for some time
// 0 - autoscroll, 1 - displayOnDemand, 2 - High resolution display
	g_display_mode++;
	if(g_display_mode==3)
	{
		g_display_mode=0;
		alt_autoscroll_mode=0;
		sub_disp_state=255;
		makeDisplayReadyForAutoScroll();
	}
	disp_state=255;
	disp_timer = 0;
	scrollLock=0;	// allow scrolling of other displays too
	
  LCD_ClearAll();
	LCD_DisplayDigit(2, LCD_CHAR_M);
	LCD_DisplayDigit(3, 0);
	LCD_DisplayDigit(4, LCD_CHAR_D);
	LCD_DisplayDigit(5, LCD_CHAR_E);
	LCD_DisplayIntWithPos(g_display_mode+1, LCD_LAST_POS_DIGIT,0);
	mode_has_been_changed=1;
	gLcdCheck=0;
}

void handleKeyLogic(void)
{
	if(mode_has_been_changed!=0)
		return;
	
	if((fwdKeyPressed==1)&&(revKeyPressed==1))
	{
		bothKeysPressedCtr++;
		fwdKeyPressedCtr=0;
		revKeyPressedCtr=0;
		if(bothKeysPressedCtr>100)	// 125 earlier
		{
			changeDisplayMode();
			someKeyPressed=1;
			bothKeysPressedCtr=0;
			fwdKeyPressed=0;
			revKeyPressed=0;
		}
	}
	else
	{// here both switches may not be pressed or either switch may be pressed
		if((fwdKeyPressed==1)&&(revKeyPressed==0))
		{
			bothKeysPressedCtr=0;
			fwdKeyPressedCtr++;
			if(fwdKeyPressedCtr>10)	// 15 earlier
			{
//				if((g_display_mode==0)&&(autoscrollInButtonMode==0))
//				{
//					disp_state=255;
//					sub_disp_state=255;
//				}
				someKeyPressed=1;
				fwdKeyPressedCtr=0;
			}
		}
		else
		{
			if((revKeyPressed==1)&&(fwdKeyPressed==0))
			{
				bothKeysPressedCtr=0;
				revKeyPressedCtr++;
				if(revKeyPressedCtr>10)	// 5 earlier
				{
					someKeyPressed=1;
					revKeyPressedCtr=0;
				}
			}
			else
			{// here no key has been pressed
				noKeyPressedCtr++;
			}
		}
	}
}


void KEY_PollingProcessing(void)
{// Gujarat Board Key processing is different
// this routine is effectively executed every 40 ms
	uint8_t currentSW;

//	if(init_done!=0)	// 19/Aug/2020 - 17:00 - OOPS
	if(init_done==0)	// 19/Aug/2020 - 17:00 - This is the correct thing
		return;
		
	if(kbscanflag==0)
		return;
		
	kbscanflag=0;
	
#ifdef KEY_ACTIVE_HIGH
	currentSW=~TOPSW;
#else
	currentSW=TOPSW;
#endif	
	if(currentSW==0)
	{// key is pressed
		if(prevTopSW==0)
		{
			fwdKeyPressed=1;
		}
	}
	else
	{// here key is not pressed
		if(prevTopSW!=0)
		{
			fwdKeyPressed=0;
		}
	}

	prevTopSW=currentSW;
//-----------------------------	
#ifdef KEY_ACTIVE_HIGH
	currentSW=~MIDDLESW;
#else
	currentSW=MIDDLESW;
#endif	
	
	if(currentSW==0)
	{
		if(prevMiddleSW==0)
		{
			revKeyPressed=1;
		}
	}
	else
	{// here key is not pressed
		if(prevMiddleSW!=0)
		{
			revKeyPressed=0;
		}
	}
	prevMiddleSW=currentSW;
	

// handle key logic

	handleKeyLogic();
	if(someKeyPressed==1)
		seconds_flag=1;	// this is to ensure entry into the handleDisplay function
	handleDisplay_Gujarat(0);
	
	
#ifdef KEY_ACTIVE_HIGH
	if(MDRST_SW!=0)	// switch is active high
#else
	if(MDRST_SW==0)	// switch is active low
#endif	
	{// key is pressed
		if(keyNotPressedCtr2>10)
		{// key has been released for at least half a second
			keyNotPressedCtr2=0;	// one has to release the key for the next press to be effective
		}
		else
		{//key continues to be pressed
			keyPressedCtr2++;	// increments every 40 ms
			if(keyPressedCtr2>50)
			{// key has been pressed for 2 seconds
			  LCD_ClearAll();
				LCD_DisplayDigit(2, LCD_CHAR_R);
				LCD_DisplayDigit(3, LCD_CHAR_E);
				LCD_DisplayDigit(4, 5);
				LCD_DisplayDigit(5, LCD_CHAR_E);
				LCD_DisplayDigit(6, LCD_CHAR_T);
	      generate_bill(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second,BT_MANU);
	      write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
	      e2write_flags&=~E2_W_CURRENTDATA;
				keyPressedCtr2=0;	// this is to prevent reentry into mode 2
				gentimer=0;
				do
				{
					R_WDT_Restart();
					per_second_activity();
					wre2rom();
				}
				while(gentimer<3);
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


//avoidShowingRepeatParameters
void handleDisplay_Gujarat(uint8_t t)
{// disp_timer is incremented in the interrupt callback function
	uint8_t previous_disp_state;
  powermgmt_mode_t power_mode;

//	disp_autoscroll_interval=5;
	
	if((g_display_mode==255)||(seconds_flag==0))	// things are not ready yet
		return;

	if(mode_has_been_changed==1)
	{
		if(disp_timer<3)
			return;
		else
		{
			mode_has_been_changed=0;
			fwdKeyPressed=0;	// this is to ensure that after mode display is cleared user must press either key to proceed
			revKeyPressed=0;
			someKeyPressed=0;
			bothKeysPressedCtr=0;
			disp_timer=0;
			mode_timeout_ctr=0;
			fast_display_flag=0;
			holdState=0;
			disp_state=0;
			disp_timer=disp_autoscroll_interval;	// this is to ensure entry below
		}
	}
		
	power_mode = POWERMGMT_GetMode();	// actual function to be used

	if((power_mode == POWERMGMT_MODE1)&&(gLcdCheck==0))
			EM_LCD_DisplayStatus();

#ifdef MAGTEST	
	return;
#endif	


	if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)
	{
		if(g_display_mode==0)
			fast_display_flag=1;
	}

  if((disp_timer >= disp_autoscroll_interval)||(someKeyPressed==1)||(fast_display_flag==1))
	{
		seconds_flag=0;
		if(someKeyPressed==0)
		{
			if(g_display_mode==0)
			{
				if(autoscrollInButtonMode==0)
				{
					if(fast_display_flag==0)
					{
						if(holdState==0)
							disp_state++;
						else
							g_tamper_display_state++;
					}
					else
					{// here fast_display_flag = 1 - wait for autoscroll interval before incrementing state
						fast_display_counter++;
						if(fast_display_counter>disp_autoscroll_interval)
						{
							fast_display_flag=0;
							fast_display_counter=0;
							disp_state++;
						}
					}
				}
			}
			disp_timer=0;
		}
		else
		{// here some key has been pressed
			disp_timer=0;
			someKeyPressed=0;
			mode_timeout_ctr=0;
			fast_display_flag=0;
			
			if(fwdKeyPressed==1)
			{
				fwdKeyPressed=0;
				fwdKeyPressedCtr=0;
				if(g_display_mode==0)
				{
					if(autoscrollInButtonMode==0)
					{
						autoscrollInButtonMode=1;
						holdState=0;
					}
				}
				
				if(holdState==0)
					disp_state++;
				else
					g_tamper_display_state++;
			}
			
			if(revKeyPressed==1)
			{
				revKeyPressed=0;
				revKeyPressedCtr=0;
				
				if(g_display_mode==0)
				{
					if(autoscrollInButtonMode==0)
					{
						autoscrollInButtonMode=1;
						holdState=0;
					}
				}
				
				if(disp_state==0)
				{// we need to go to the last state
					switch(g_display_mode)
					{
						case 0:
							disp_state=25;	// last autoscroll state
							break;
							
						case 1:
							disp_state=68;	// last pushbutton state
							break;
							
						case 2:
							disp_state=3;	// last highres state
							break;
							
						default:
							break;
							
					}
				}
				else
				{
					disp_state--;
					holdState=0;
				}
			}
		}
		
		if(disp_autoscroll_interval==20)
			disp_autoscroll_interval=6;	// restore it 
			
		gLcdCheck=0;
		
		switch(g_display_mode)
		{
			case 0:	// autoscroll
				display_autoScroll();
				break;
				
			case 1:	// pushbutton
				display_onDemand();
				break;
				
			case 2:	// third display mode
				display_Third();
				break;
				
			default:
				makeDisplayReadyForAutoScroll();
				break;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------
// this is for UNIDIR meter
uint32_t gTamperEvent;
uint8_t g_tamper_display_state;
uint8_t g_cover_display_state;
uint8_t cover_disp_timer;

void display_autoScroll(void)
{
	date_time_t DateTime;
	float32_t tempFloat;
	int32_t tempTodKwh;
	int32_t tempLong;
	int32_t tempLong1;
	uint16_t tempTodMD;
	uint16_t tempint;
  int16_t i[EM_NUM_OF_LINE];
	
	ext_rtc_t datetime;
	EM_PF_SIGN pf_sign;    
	uint8_t tempchar;
	uint8_t tmonth;
	uint8_t zone;
	powermgmt_mode_t power_mode;

  R_WDT_Restart();
  LCD_ClearAll();	
	
	if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(autoscrollInButtonMode==0))
	{
		if((splTimer%2)==0)
		{
			LCD_DisplayDigit(2, LCD_CHAR_C);
			LCD_DisplayDigit(3, LCD_MINUS_SIGN);
			LCD_DisplayDigit(4, 0);
			LCD_DisplayDigit(5, LCD_CHAR_P);
			LCD_DisplayDigit(6, LCD_CHAR_E);
			LCD_DisplayDigit(7, LCD_CHAR_N);
			return;
		}
	}


	if((comm_mode!=0)&&(autoscrollInButtonMode==0))
	{
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}

	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if((power_mode == POWERMGMT_MODE1)&&(gLcdCheck==0))
			EM_LCD_DisplayStatus();

	if(disp_state>25)
		disp_state=0;

	switch(disp_state)
	{
    case 0:			// all on
      LCD_DisplayAll();
			fast_display_flag=0;
			gLcdCheck=1;
      break;			

    case 1:			// battery check
			if(Common_Data.rtc_failed==0)
	    {// Battery is good
				lcd_msg((const uint8_t*)&Bat_OK);
	    }
			else
	    {// Battery is Bad
				lcd_msg((const uint8_t*)&Bat_LO);
	    }
			break;
			
    case 2:		// date
			EM_DisplayDate();
			fast_display_flag=0;
      break;

    case 3:		// time
			EM_DisplayInstantTime();
			fast_display_flag=1;
      break;
			
    case 4:		// present status of occured tampers
			d_Tampers();
			fast_display_flag=0;
      break;

		case 5:		// Phase Sequence - Voltage
				if((g_TamperStatus&MISSPOT_MASK)==0)
				{// here no potential is missing
					if(gPhaseSequence==0)
					{
						LCD_DisplayDigit(3, LCD_CHAR_R);
						LCD_DisplayDigit(4, LCD_CHAR_Y);
						LCD_DisplayDigit(5, LCD_CHAR_B);
					}
					else
					{
						LCD_DisplayDigit(3, LCD_CHAR_R);
						LCD_DisplayDigit(4, LCD_CHAR_B);
						LCD_DisplayDigit(5, LCD_CHAR_Y);
					}
				}
				else
				{// here some potential is missing
					LCD_DisplayDigit(3, LCD_MINUS_SIGN);
					LCD_DisplayDigit(4, LCD_MINUS_SIGN);
					LCD_DisplayDigit(5, LCD_MINUS_SIGN);
				}

	    LCD_DisplayDigit(9, LCD_CHAR_P);
	    LCD_DisplayDigit(8, 5);
			disp_autoscroll_interval=6;
			fast_display_flag=1;
     	break;
			
    case 6:		// total kwh - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			disp_autoscroll_interval=20;
			fast_display_flag=0;
     	break;

		case 7:	// All zones (script_ids) TOD 1,2,3... kwh current
			display_TOD_KWH_CURRENT();
			break;

    case 8:		// L1 - Cum kwh
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+16,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     break;

// TOD wise Bill period kwh
		case 9:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_KWH_BILL(1);	// 1-L1
			break;
		 
    case 10:		// Voltage - R
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 11:		// Voltage - Y
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 12:		// Voltage - B
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
	    break;

    case 13:		// Current - R with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			lcd_display_A();
			fast_display_flag=1;
      break;

    case 14:		// Current - Y with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
			lcd_display_A();
			fast_display_flag=1;
      break;

   	case 15:		// Current - B with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
			lcd_display_A();
			fast_display_flag=1;
      break;

    case 16:		// Average PF for last billing period
			tmonth = decrementMonth(1);
			if(Common_Data.bpcount==0)
				tempLong=0;
			else
      	EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+12,(uint8_t*)&tempLong,sizeof(tempLong));
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_BILL);
			lcd_display_A();
			LCD_DisplayDigit(1, LCD_CHAR_P);
			LCD_DisplayDigit(2, LCD_CHAR_F);
			LCD_DisplayDigit(9, 1);
  		LCD_DisplaySpSign(S_BILL);
			fast_display_flag=0;
	    break;

    case 17:		// pf total with sign
    	pf_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,LINE_TOTAL);
			tempLong = getInstantaneousParams(IPFS,LINE_TOTAL)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_P);	
			LCD_DisplayDigit(2, LCD_CHAR_F);	

			LCD_DisplayDigit(9, LCD_CHAR_T);
			if(ABS(tempLong)>10)
			{
				if (pf_sign == PF_SIGN_LEAD_C)
				{
		    	LCD_DisplaySpSign(S_LEAD);
				}
				else if (pf_sign == PF_SIGN_LAG_L)
				{
		    	LCD_DisplaySpSign(S_LAG);
				}
				else if (pf_sign == PF_SIGN_UNITY)
				{
		    	LCD_ClearSpSign(S_LEAD);
		    	LCD_ClearSpSign(S_LAG);
				}
			}
			fast_display_flag=0;
		 break;

    case 18:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
	    break;

// Bill TOD MD in kva with date and time			
		case 19:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_MDKVA_DTTM_BILL(1);	// 1-L1
			break;
			
    case 20:		// L1 - Cum kVAh
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+52,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVAhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     	break;

// Bill TOD kvah			
// TOD wise Bill period kvah
		case 21:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_KVAH_BILL(1);	// 1-L1
			break;
		 
    case 22:		// total kvarh_lag - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lag/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
    	LCD_DisplaySpSign(S_LAG);
			fast_display_flag=0;
     	break;

    case 23:		// total kvarh_lead - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lead/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
    	LCD_DisplaySpSign(S_LEAD);
			fast_display_flag=0;
     	break;
			
    case 24:		// now for E2rom check
			tempchar=perform_E2ROM_Test(0);
			if(tempchar==0)
			{// e2rom is good
				lcd_msg((const uint8_t*)&NVM_OK);
			}
			else
			{// E2ROM is failed
				lcd_msg((const uint8_t*)&NVM_BAD);
			}
      break;
			
    case 25:		// now for RTC check - always good
			lcd_msg((const uint8_t*)&RTC_OK);
			break;
	}
}

void display_onDemand(void)
{
//	toddata_t temptoddata;
	int32_t tempTodKwh;
  powermgmt_mode_t power_mode;
	int32_t tempLong;
	uint16_t tempint;
	date_time_t DateTime;
	ext_rtc_t datetime;
	EM_PF_SIGN pf_sign;    
	uint8_t tempchar;
	uint8_t tmonth;
	uint8_t zone;

  R_WDT_Restart();
  LCD_ClearAll();

	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if((power_mode == POWERMGMT_MODE1)&&(gLcdCheck==0))
			EM_LCD_DisplayStatus();

	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>68)
		disp_state=0;	// go back to the beginning
	
	switch(disp_state)
	{
		
    case 0:			// all on
      LCD_DisplayAll();
			fast_display_flag=0;
			gLcdCheck=1;
      break;			
		
    case 1:			// battery check
			if(Common_Data.rtc_failed==0)
	    {// Battery is good
				lcd_msg((const uint8_t*)&Bat_OK);
	    }
			else
	    {// Battery is Bad
				lcd_msg((const uint8_t*)&Bat_LO);
	    }
			fast_display_flag=0;
      break;
		
    case 2:			// meter no
			LCD_DisplayIntWithPos(Common_Data.meterNumber, LCD_LAST_POS_DIGIT,0);
			lcd_display_id();	
      break;
			
    case 3:			// frequency
			tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
			display_long_two_dp(tempLong);
		  LCD_DisplayDigit(1, LCD_CHAR_F);
		  LCD_DisplayDigit(9, LCD_CHAR_H);
		  LCD_DisplayDigit(8, 2);
      break;
			
    case 4:		// total kwh - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			disp_autoscroll_interval=20;
			fast_display_flag=0;
     	break;
			
    case 5:		// current month average pf
			tempLong = computeAveragePF(myenergydata.kwh, myenergydata.kvah, IMPORT);
      display_long_three_dp(tempLong);
			lcd_display_A();
			LCD_DisplayDigit(1, LCD_CHAR_P);
			LCD_DisplayDigit(2, LCD_CHAR_F);
			fast_display_flag=0;
     	break;
			
    case 6:		// current month average pf - lag
			tempLong = computeAveragePF_Lag_Lead(myenergydata.kwh_lag, myenergydata.kvah_lag, 0);
      display_long_three_dp(tempLong);
			lcd_display_A();
			LCD_DisplayDigit(1, LCD_CHAR_P);
			LCD_DisplayDigit(2, LCD_CHAR_F);
    	LCD_DisplaySpSign(S_LAG);
			fast_display_flag=0;
     	break;
			
    case 7:		// current month average pf - lead
			tempLong = computeAveragePF_Lag_Lead(myenergydata.kwh_lead, myenergydata.kvah_lead, 1);
      display_long_three_dp(tempLong);
			lcd_display_A();
			LCD_DisplayDigit(1, LCD_CHAR_P);
			LCD_DisplayDigit(2, LCD_CHAR_F);
    	LCD_DisplaySpSign(S_LEAD);
			fast_display_flag=0;
     	break;
			
    case 8:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
	    break;

    case 9:		// Instant Load in kVA
			tempLong = getInstantaneousParams(APP_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykVASign();
			fast_display_flag=1;
	    break;

    case 10:		// Rising MD in kVA
	    tempchar=60/(int32_t)Common_Data.mdinterval;
			tempLong = ((myenergydata.kvah-ctldata.kvah_last_saved)*(int32_t)tempchar);
			
			if(tempLong<0)
				tempLong=0;
			display_long_three_dp(tempLong);	// rising MD
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
	    LCD_DisplaySpSign(S_TIME);	// show the time icon also

// we can also display the minutes elapsed
			tempchar = rtc.minute - ((rtc.minute/Common_Data.mdinterval)*Common_Data.mdinterval); 
  		LCD_DisplayDigit(1, LCD_CHAR_R);
  		LCD_DisplayDigit(2, LCD_CHAR_D);
  		LCD_DisplayDigit(9, tempchar/10);
  		LCD_DisplayDigit(8, tempchar%10);
			fast_display_flag=1;
      break;

    case 11:		// total kvah - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvah/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVAhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     	break;

		case 12:	// All zones (script_ids) TOD 1,2,3... kvah current
			display_TOD_KVAH_CURRENT();
			break;
			
    case 13:		// Current MD in kw
			display_long_three_dp(mymddata.mdkw);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
			
    case 14:		// Cumulative MD in kw
			display_long_three_dp(ctldata.cumulative_md);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

// TOD wise current MD kw with date and time 			
// Current TOD MDkw with date and time			
		case 15:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_MDKW_DTTM_CURRENT();	// Current
			break;

    case 16:		// Resultant MD in kVA
			display_long_three_dp(mymddata.mdkva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
						
    case 17:		// Cumulative MD in kVA
			display_long_three_dp(ctldata.cumulative_md_kva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

			
// TOD wise current MD kva with date and time 			
// Current TOD MDkva with date and time			
		case 18:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_MDKVA_DTTM_CURRENT();	// Current
			break;

// L1 - KWH TOD 1,2,3
		case 19:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_KWH_BILL(1);
			break;

// L2 to L6 - Cumulative kwh
    case 20:		// L2 kwh
    case 21:		// L3 kwh
    case 22:		// L4 kwh
    case 23:		// L5 kwh
    case 24:		// L6 kwh
			tempchar=disp_state - (20-2);	// start from 2 onwards
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+16,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_BILL);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplayDigit(9, tempchar);
			fast_display_flag=0;
      break;
			
// L1 - KVAH TOD 1,2,3
		case 25:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_KVAH_BILL(1);
			break;

// L2 to L6 - Cumulative kVAh
    case 26:		// L2 kvah
    case 27:		// L3 kvah
    case 28:		// L4 kvah
    case 29:		// L5 kvah
    case 30:		// L6 kvah
			tempchar=disp_state - (26-2);	// start from 2 onwards
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+52,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_BILL);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplayDigit(9, tempchar);
			fast_display_flag=0;
      break;
			
// Bill TOD MD in kva WITHOUT date and time			
		case 31:	// All zones (script_ids) TOD 1,2,3... L1 Bill
			display_TOD_MDKVA_BILL(1);	// 1-L1
			break;

// L2 to L6 - MD KVA
    case 32:		// L2 MD KVA
    case 33:		// L3 MD KVA
    case 34:		// L4 MD KVA
    case 35:		// L5 MD KVA
    case 36:		// L6 MD KVA
			tempchar=disp_state - (32-2);	// start from 2 onwards
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+192,(uint8_t*)&tempLong,4);
			display_long_three_dp(tempLong);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_MD);			
			LCD_DisplayDigit(9, tempchar);
			fast_display_flag=0;
      break;

// L1 - L6 KVARH Lag
    case 37:		// L1 kvarh_lag
    case 38:		// L2 kvarh_lag
    case 39:		// L3 kvarh_lag
    case 40:		// L4 kvarh_lag
    case 41:		// L5 kvarh_lag
    case 42:		// L6 kvarh_lag
			tempchar=disp_state - (37-1);	// start from 1 onwards
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+44,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVArhSign();
		  LCD_DisplaySpSign(S_BILL);
  		LCD_DisplaySpSign(S_LAG);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplayDigit(9, tempchar);
			fast_display_flag=0;
      break;

// L1 - L6 KVARH Lead
    case 43:		// L1 kvarh_lead
    case 44:		// L2 kvarh_lead
    case 45:		// L3 kvarh_lead
    case 46:		// L4 kvarh_lead
    case 47:		// L5 kvarh_lead
    case 48:		// L6 kvarh_lead
			tempchar=disp_state - (43-1);	// start from 1 onwards
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+48,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVArhSign();
		  LCD_DisplaySpSign(S_BILL);
  		LCD_DisplaySpSign(S_LEAD);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplayDigit(9, tempchar);
			fast_display_flag=0;
      break;

// No of tamper events - total tamper count
    case 49:		// Total Tamper count
			LCD_DisplayIntWithPos(mytmprdata.tamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

// Latest occured tamper details with date and time - 78,79,80
    case 50:		// latest occured tamper
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS+8,(uint8_t *)&tempchar,sizeof(tempchar));
			tempint=findEventCode(tempchar,1);	// 1 stands for occurence
			LCD_DisplayIntWithPos(tempint, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 1);
			LCD_DisplayDigit(2, LCD_CHAR_D);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;

    case 51:		// latest occured tamper date
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;

    case 52:		// latest occured tamper time
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
			
// Second Latest occured tamper details with date and time - 81,82,83
    case 53:		// second latest occured tamper
			EPR_Read(SECOND_LAST_OCCURED_DETAILS_ADDRESS+8,(uint8_t *)&tempchar,sizeof(tempchar));
			tempint=findEventCode(tempchar,1);	// 1 stands for occurence
			LCD_DisplayIntWithPos(tempint, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 1);
			LCD_DisplayDigit(2, LCD_CHAR_D);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;

    case 54:		// second latest occured tamper date
			EPR_Read(SECOND_LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;

    case 55:		// second latest occured tamper time
			EPR_Read(SECOND_LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;

// Latest restored tamper details with date and time - 84,85,86
    case 56:		// latest restored tamper
			EPR_Read(LAST_RESTORED_DETAILS_ADDRESS+8,(uint8_t *)&tempchar,sizeof(tempchar));
			tempint=findEventCode(tempchar,0);	// 0 stands for restoration
			LCD_DisplayIntWithPos(tempint, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 1);
			LCD_DisplayDigit(2, LCD_CHAR_D);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;

    case 57:		// latest restored tamper date
			EPR_Read(LAST_RESTORED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;

    case 58:		// latest restored tamper time
			EPR_Read(LAST_RESTORED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;

// Second Latest restored tamper details with date and time - 87,88,89
    case 59:		// second latest restored tamper
			EPR_Read(SECOND_LAST_RESTORED_DETAILS_ADDRESS+8,(uint8_t *)&tempchar,sizeof(tempchar));
			tempint=findEventCode(tempchar,0);	// 0 stands for restoration
			LCD_DisplayIntWithPos(tempint, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 1);
			LCD_DisplayDigit(2, LCD_CHAR_D);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;

    case 60:		// second latest restored tamper date
			EPR_Read(SECOND_LAST_RESTORED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;

    case 61:		// second latest restored tamper time
			EPR_Read(SECOND_LAST_RESTORED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;

// Power Off duration
    case 62:		// Total power oFF time in hours - minutes earlier
			tempLong = computePowerOffDurn(rtc);	// use the rtc for current value and returns power fail duration for the month
			tempLong += Common_Data.cumPowerFailDuration;	// now it is cumulative
			
			tempLong = tempLong/60;	// now hours
		
			LCD_DisplayIntWithPos(tempLong, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_F);
			
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_M);
			fast_display_flag=0;
      break;

// Reset Count - billing count
    case 63:		// billing count
			LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

// Program count
    case 64:		// programming count
			LCD_DisplayIntWithPos(Common_Data.pgm_count, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
			
// Bill kwh lag
    case 65:		// L1 - Cum kwh lag
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+308,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_BILL);
	    LCD_DisplaySpSign(S_LAG);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     	break;
			
// Bill kwh lead
    case 66:		// L1 - Cum kwh lead
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+312,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_BILL);
	    LCD_DisplaySpSign(S_LEAD);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     	break;

// Cumulative kwh lead
    case 67:		// total kwh lead - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh_lead/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
	    LCD_DisplaySpSign(S_LEAD);
			fast_display_flag=0;
     	break;

// Cumulative kwh lag
    case 68:		// total kwh lag - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh_lag/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
	    LCD_DisplaySpSign(S_LAG);
			fast_display_flag=0;
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
		
	if(disp_state>3)
		disp_state=0;
	
	switch(disp_state)
	{
    case 0:			// high resolution kwh
			temp_high_res_energy = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykWhSign();
			fast_display_flag=1;
      break;
			
    case 1:			// high resolution kvarh lag
			temp_high_res_energy = ((myenergydata.kvarh_lag%100000)*100)+(int32_t)high_resolution_kvarh_lag_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_LAG);
			fast_display_flag=1;
      break;
			
    case 2:			// high resolution kvarh lead
			temp_high_res_energy = ((myenergydata.kvarh_lead%100000)*100)+(int32_t)high_resolution_kvarh_lead_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_LEAD);
			fast_display_flag=1;
      break;
			
    case 3:			// high resolution kvah
			temp_high_res_energy = ((myenergydata.kvah%100000)*100)+(int32_t)high_resolution_kvah_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykVAhSign();
			fast_display_flag=1;
      break;
	}
}



//This function is being rewritten

void d_Tampers(void)
{
	uint32_t gEvent;

	gEvent = mytmprdata.tamperEvent_image&TAMPER_DISPLAY_MASK; // make all those bits not to be displayed as 0
	
	if(gEvent==0)
	{// hereg_display_flag there are no tampers
		lcd_msg((const uint8_t*)&msg_nil_tamper);	// simbly display tamper nil
//		displayTamperName(0);	// 0 is to display the no tamper message
	}
	else
	{// here there is some tamper present i.e. some bits are 1
		lcd_msg((const uint8_t*)&msg_yes_tamper);	// simbly display tamper yes
	}
}


// Earlier function - prior to 08/Apr/2022
/*
// the function d_Tampers only displays unrestored tampers
void d_Tampers(void)
{
	uint32_t whichevent;
	uint8_t event_code_bit;
	uint8_t whichGroup;

	if(holdState==0)
	{
		gTamperEvent = mytmprdata.tamperEvent_image&TAMPER_DISPLAY_MASK; // make all those bits not to be displayed as 0
		holdState = 1;
		g_tamper_display_state = 0;	// safety
	}
	
//	switch(g_tamper_display_state)
//	{
//		case 0:
			if(gTamperEvent==0)
			{// hereg_display_flag there are no tampers
				displayTamperName(0);	// 0 is to display the no tamper message
			}
			else
			{// here there is some tamper present i.e. some bits are 1
			// only those bits which are to be displayed have not been masked, transaction events and cover open etc have been masked out
		    whichevent = gTamperEvent & ~(gTamperEvent-1);
				event_code_bit=findWhichBitIsOne(whichevent);	// bit numbers 1 to 15, 22, 23, 24, 29 and 30  - bit 0 is not used, others masked out
				displayTamperName(event_code_bit);
				gTamperEvent&=~whichevent;	// job complete for this fellow
			}
			
			if(gTamperEvent==0)
			{// everything has been completed
				holdState=0; 	// allow disp_state to be incremented
			}
//			g_tamper_display_state = 0;	// this version has only one state
//			break;

// we are only showing the names of the occured tampers - date and time cases can be referred from TNEB code			
//	}
}
*/



void EM_LCD_DisplayStatus(void)
{
  int16_t i[EM_NUM_OF_LINE];	// two decimals 

  powermgmt_mode_t power_mode;
	uint8_t blinkbit;
	
	power_mode = POWERMGMT_GetMode();

  i[LINE_PHASE_R] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_R)*100);	// 1Amp = 100
  i[LINE_PHASE_Y] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_Y)*100);
  i[LINE_PHASE_B] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_B)*100);

	
	if(i[LINE_PHASE_R]<0)
		i[LINE_PHASE_R]*=-1;
	if(i[LINE_PHASE_Y]<0)
		i[LINE_PHASE_Y]*=-1;
	if(i[LINE_PHASE_B]<0)
		i[LINE_PHASE_B]*=-1;
	
	if(g_display_mode==0)
	{
		if(((disp_state==0)||(disp_state==255))&&(alt_autoscroll_mode==1)&&(sub_disp_state==255))
		{
				return;
		}
	}

	if (power_mode == POWERMGMT_MODE3)
		return;

	if((gentimer%2)==0)
		blinkbit=0;
	else
		blinkbit=1;

// This is being done here since the icon could be forcibly cleared later on

// volt_unbalance
	if((fault_code&_BV(3))==0)
		LCD_ClearSpSign(S_V_UNBAL);
	else
		LCD_DisplaySpSign(S_V_UNBAL);

		
		
// handling of RYB		
// If voltage and current are available then RYB to be displayed continuously.
// If only voltage is available then the particular phase icon should blink.
// If voltage is not available – then icon should disappear.
	
	if((fault_code&_BV(0))!=0)
	{
		LCD_ClearSpSign(S_R);
		LCD_ClearSpSign(S_V_UNBAL);
	}
	else
	{// here voltage is available
		if(i[LINE_PHASE_R]>10)
			LCD_DisplaySpSign(S_R);	// current is also available
		else
		{// blink
			if(blinkbit==0)
				LCD_ClearSpSign(S_R);
			else
				LCD_DisplaySpSign(S_R);
		}
	}

	if((fault_code&_BV(1))!=0)
	{
		LCD_ClearSpSign(S_Y);
		LCD_ClearSpSign(S_V_UNBAL);
	}
	else
	{// here voltage is available
		if(i[LINE_PHASE_Y]>10)
			LCD_DisplaySpSign(S_Y);	// current is also available
		else
		{// blink
			if(blinkbit==0)
				LCD_ClearSpSign(S_Y);
			else
				LCD_DisplaySpSign(S_Y);
		}
	}

	if((fault_code&_BV(2))!=0)
	{
		LCD_ClearSpSign(S_B);
		LCD_ClearSpSign(S_V_UNBAL);
	}
	else
	{// here voltage is available
		if(i[LINE_PHASE_B]>10)
			LCD_DisplaySpSign(S_B);	// current is also available
		else
		{// blink
			if(blinkbit==0)
				LCD_ClearSpSign(S_B);
			else
				LCD_DisplaySpSign(S_B);
		}
	}
	
// Rev_R(6), Rev_Y(7), Rev_B(8) - no icons  in BIDIR mode		
// Here we are in Unidir Mode	
	if((g_TamperStatus&REVERSE_MASK)!=0)
		LCD_DisplaySpSign(S_LEFT_ARROW);
	else
		LCD_ClearSpSign(S_LEFT_ARROW);

// Neutral Disturbance(9) - If neutral disturbance N icon will blink
	if((fault_code&_BV(9))!=0)
	{
		if((gentimer%2)==0)
			LCD_ClearSpSign(S_N);
		else
			LCD_DisplaySpSign(S_N);
			
		LCD_ClearSpSign(S_V_UNBAL);
	}
	else
	{// here there is no tamper - neutral is proper
		LCD_DisplaySpSign(S_N);		
	}	
// over current and ct open have no indication		
// for over current let the corresponding R Y B icon blink - let it be for present
		
// for CT-open use the MISS icon S_NM

	if(((fault_code&_BV(11))!=0)||((fault_code&_BV(12))!=0)||((fault_code&_BV(13))!=0))
    LCD_DisplaySpSign(S_NM);
	else
    LCD_ClearSpSign(S_NM);

// magnet tamper
//	if(((fault_code&_BV(10))==0)&&((fault_code&_BV(15))==0))

//	if(((fault_code&_BV(10))!=0)||((fault_code&_BV(14))!=0))
//	if(((g_TamperStatus&HALLSENS_MASK)!=0)||((g_TamperStatus&ACMAG_MASK)!=0))
//		LCD_DisplaySpSign(S_MAG);
//	else
//		LCD_ClearSpSign(S_MAG);


//	if(((g_TamperStatus&HALLSENS_MASK)==0)&&((g_TamperStatus&ACMAG_MASK)==0))
	if(((g_TamperStatus&HALLSENS_MASK)==0)&&(pulsesStartedComingFlag==0))
	{
		LCD_ClearSpSign(S_MAG);
		
// current bypass
//		if((fault_code&_BV(4))==0)
		if(((fault_code&_BV(4))==0)||((g_TamperStatus&CTOPEN_MASK)!=0))
			LCD_ClearSpSign(S_T9);
		else
			LCD_DisplaySpSign(S_T9);

// current_unbalance
		if((fault_code&_BV(5))==0)
			LCD_ClearSpSign(S_C_UNBAL);
		else
			LCD_DisplaySpSign(S_C_UNBAL);
	}
	else
	{
		LCD_DisplaySpSign(S_MAG);
		LCD_ClearSpSign(S_T9);			// Bypass
		LCD_ClearSpSign(S_C_UNBAL);	// Current Unbalance
    LCD_ClearSpSign(S_NM);	// CT Open
//		LCD_ClearSpSign(S_LEFT_ARROW);
	}
	
// this is the last thing to be done
// if there is missing potential or neutral disturbanc or mixed powers then Bypass, Current Unbalance, CT open icons to be
// switched off


	if(((fault_code&_BV(0))!=0)||((fault_code&_BV(1))!=0)||((fault_code&_BV(2))!=0)||((fault_code&_BV(9))!=0)||(mixedPowers==1))
//	if(((fault_code&_BV(0))!=0)||((fault_code&_BV(1))!=0)||((fault_code&_BV(2))!=0)||((fault_code&_BV(9))!=0)||((g_TamperStatus&REVERSE_MASK)!=0))
	{
		LCD_ClearSpSign(S_T9);			// Bypass
		LCD_ClearSpSign(S_C_UNBAL);	// Current Unbalance
    LCD_ClearSpSign(S_NM);	// CT Open
//		LCD_ClearSpSign(S_LEFT_ARROW);
	}
}

// The following function will now be called in lieu of displayTamperName

void displayDLMSCode(uint8_t eventCodeBit, uint8_t event_Type)
{
	findEventCode(eventCodeBit,event_Type);
	
}

// This function has been superseded on 08/Apr/2022
// now the dlms codes will be displayed instead
/*
void displayTamperName(uint8_t eventCode)
{
		switch(eventCode)
//		switch(mytmprdata.latestTamperCode)	// this is obviously wrong
		{
			case 0:
				lcd_msg((const uint8_t*)&msg_nil_tamper);
				break;
				
			case 1:	//	MISSING_POTENTIAL_R
				lcd_msg((const uint8_t*)&msg_miss_R);
				break;
			
			case 2:	//	MISSING_POTENTIAL_Y
				lcd_msg((const uint8_t*)&msg_miss_Y);
				break;
				
			case 3:	//	MISSING_POTENTIAL_B
				lcd_msg((const uint8_t*)&msg_miss_B);
				break;
				
			case 4:	//	OVER_VOLTAGE
				lcd_msg((const uint8_t*)&msg_over_V);
				break;
			
			case 5:	//	UNDER_VOLTAGE
				lcd_msg((const uint8_t*)&msg_under_V);
				break;

			case 6:	//	VOLTAGE_UNBALANCED
				lcd_msg((const uint8_t*)&msg_unbal_V);
				break;

			case 7:	//	CT_REVERSED_R
				lcd_msg((const uint8_t*)&curr_rev_R);
				break;
				
			case 8:	//	CT_REVERSED_Y
				lcd_msg((const uint8_t*)&curr_rev_Y);
				break;
				
			case 9:	//	CT_REVERSED_B
				lcd_msg((const uint8_t*)&curr_rev_B);
				break;

 			case 10:	//	CT_OPEN_R
				lcd_msg((const uint8_t*)&curr_fail_R);
				break;
			case 11:	//	CT_OPEN_Y
				lcd_msg((const uint8_t*)&curr_fail_Y);
				break;
			case 12:	//	CT_OPEN_B
				lcd_msg((const uint8_t*)&curr_fail_B);
				break;

			case 13:	//	CURRENT_UNBALANCED
				lcd_msg((const uint8_t*)&msg_unbal_C);
				break;
				
			case 14:	//	CT_BYPASS
				lcd_msg((const uint8_t*)&msg_bypass_C);
				break;
				
			case 15:	//	OVER_CURRENT
			case 16:	//	POWER_EVENT
			case 17:	//	RTC_TRANSACTION
			case 18:	//	MD_INTERVAL_TRANSACTION
			case 19:	//	SURVEY_INTERVAL_TRANSACTION
			case 20:	//	SINGLE_ACTION_TRANSACTION
			case 21:	//	ACTIVITY_CALENDAR_TRANSACTION
			case 25:	//	LOW_PF
				break;
				
			case 26:	//	COVER_OPEN
				lcd_msg((const uint8_t*)&msg_C_Open);
				break;
			case 27:	//	MODE_UNIDIRECTIONAL_SET
			case 28:	//	MODE_BIDIRECTIONAL_SET
				break;

			case 22:	//	MAGNET
			case 23:	//	AC_MAGNET
				lcd_msg((const uint8_t*)&msg_Magnet);
				break;
			case 24:	//	NEUTRAL_DISTURBANCE
				lcd_msg((const uint8_t*)&msg_ND);
				break;
			
			case 29:	//	OVER_CURRENT_Y
			case 30:	//	OVER_CURRENT_B
			
			default:
				break;
		}

}
*/











