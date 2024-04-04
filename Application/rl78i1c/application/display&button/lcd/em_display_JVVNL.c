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

// Display tamper condition
#include "format.h"
#include "eeprom.h"
#include "em_core.h"


//#define CHARGER		// only used to create a special display for testing instantaneous power consumption of CHARGER

void handleDisplay_Production(void);
void display_autoScroll_Production(void);

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

void displayCoverOpenStatus(void);
void displayMagnetTamperStatus(void);



#define RETURN_TO_AUTOSCROLL_COUNT	7500	// corresponds to 300 seconds or 5 minutes

#ifdef BIDIR_METER
	#define NO_OF_SUB_DISPLAY_STATES 4	// BIDIR_METER has 4
#else
	#define NO_OF_SUB_DISPLAY_STATES 3	// Regular Gujarat Meter has 3
#endif

uint8_t disp_state;
uint8_t disp_timer;
uint8_t disp_autoscroll_interval=10;
uint8_t holdState;


uint8_t			g_display_mode = 0;	// 0 - Autoscroll, 1- OnDemand, 2-ThirdDisplay, 3-Special Autoscroll
//uint8_t			g_display_update_type =0;	// 0 - Autoincrement, 1- OnKeyPressOnly, 2-AutoIncrementEverysecond, 3-KeyPressIncrementEverySecond
uint8_t     g_is_por = 0;

uint8_t gentimer;
uint8_t tamperTimer;

uint8_t comm_mode;
uint8_t commModeTimer;


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
		disp_autoscroll_interval=10;
		autoscrollInButtonMode=0;
}

void EM_RTC_DisplayInterruptCallback(void)
{//this function is called only in battery mode
  powermgmt_mode_t power_mode;
	
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
					autoscrollInButtonMode=0;
					disp_timer=disp_autoscroll_interval;
				}
			}
		}
		else
		{
			if(g_display_mode==1)
			{
				mode_timeout_ctr++;
				if(mode_timeout_ctr>900)	//  15 minute
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
uint8_t toggleVariable;


void handleDisplay_Gujarat(uint8_t a);
void changeDisplayMode(void);
//void displayRepeatParameters(void);

void changeDisplayMode(void)
{// both keys have been pressed for some time
// 0 - autoscroll, 1 - displayOnDemand, there is no (2 - High resolution display) in JVVNL
	g_display_mode++;
	if(g_display_mode==2)	// this board does not have High resolution display
	{
		g_display_mode=0;
		alt_autoscroll_mode=0;
		sub_disp_state=255;
		makeDisplayReadyForAutoScroll();
	}
	disp_state=255;
	disp_timer = 0;
	scrollLock=0;	// this feature is not being used
	
/*	
	scrollLock=1;	// lock the display for a few seconds to allow following message to remain on LCD
// this variable gets cleared in 3 seconds

	if(g_display_mode==0)
	{
	  LCD_ClearAll();
		LCD_DisplayDigit(2, LCD_CHAR_A);
		LCD_DisplayDigit(3, LCD_CHAR_U);
		LCD_DisplayDigit(4, LCD_CHAR_T);
		LCD_DisplayDigit(5, 0);
//		LCD_DisplayIntWithPos(g_display_mode+1, LCD_LAST_POS_DIGIT,0);
	}
	else
	{
	  LCD_ClearAll();
		LCD_DisplayDigit(2, LCD_CHAR_M);
		LCD_DisplayDigit(3, 0);
		LCD_DisplayDigit(4, LCD_CHAR_D);
		LCD_DisplayDigit(5, LCD_CHAR_E);
		LCD_DisplayIntWithPos(g_display_mode+1, LCD_LAST_POS_DIGIT,0);
	}
	mode_has_been_changed=1;
*/	
// since we will not be displaying anything when mode is changed the following lines are added

	disp_timer=0;
	mode_timeout_ctr=0;
	fast_display_flag=0;
	holdState=0;
	disp_state=255;
	disp_timer=disp_autoscroll_interval;
}

void handleKeyLogic(void)
{// for JVVNL - only FWD key will cause display to change into Pushbutton mode
//	if(mode_has_been_changed!=0)
//		return;
	
	if((fwdKeyPressed==1)&&(revKeyPressed==1))
	{// do nothing
/*		
		bothKeysPressedCtr++;
		fwdKeyPressedCtr=0;
		revKeyPressedCtr=0;
		if(bothKeysPressedCtr>25)	// 125 earlier
		{
			changeDisplayMode();
			someKeyPressed=1;
			bothKeysPressedCtr=0;
			fwdKeyPressed=0;
			revKeyPressed=0;
		}
*/		
		fwdKeyPressed=0;
		revKeyPressed=0;
	}
	else
	{// here both switches may not be pressed or either switch may be pressed
		if((fwdKeyPressed==1)&&(revKeyPressed==0))
		{
			bothKeysPressedCtr=0;
			fwdKeyPressedCtr++;
			if(fwdKeyPressedCtr>10)	// 15 earlier
			{
				if(g_display_mode==0)
				{
					changeDisplayMode();
					someKeyPressed=1;
					bothKeysPressedCtr=0;
					fwdKeyPressed=0;
					revKeyPressed=0;
				}
				else
				{// we are already in pushbutton mode
					someKeyPressed=1;
					fwdKeyPressedCtr=0;
				}
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
					if(g_display_mode==1)
					{// reverse key will work only we are in pushbutton mode
						someKeyPressed=1;
						revKeyPressedCtr=0;
					}
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
		
	if(calswitch!=0)
		handleDisplay_Production();
	else
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


void handleDisplay_Production(void)
{
//	if((g_display_mode==255)||(seconds_flag==0))	// things are not ready yet
	if(seconds_flag==0)	// one second not over yet
		return;

	if(gCalFlag!=0)
		return;

  if((disp_timer >= disp_autoscroll_interval)||(someKeyPressed==1))
	{
		seconds_flag=0;
		someKeyPressed=0;
		disp_timer=0;
		display_autoScroll_Production();
		disp_state++;
	}
}

void display_autoScroll_Production(void)
{

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

  R_WDT_Restart();
  LCD_ClearAll();	
	
	EM_LCD_DisplayStatus();

	if(disp_state>15)
		disp_state=0;

	switch(disp_state)
	{
    case 0:		// Cal staus
			lcd_display_Cal_Status();
			disp_autoscroll_interval=4;
     	break;
			
    case 1:		// Version Number
			lcd_display_version();
			disp_autoscroll_interval=4;
     	break;
	
    case 2:		// Voltage - R
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
			disp_autoscroll_interval=2;
      break;

    case 3:		// Voltage - Y
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
      break;

    case 4:		// Voltage - B
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
	    break;

    case 5:		// Current - R with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			lcd_display_A();
      break;

    case 6:		// Current - Y with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
			lcd_display_A();
      break;

   	case 7:		// Current - B with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
			lcd_display_A();
      break;

   	case 8:		// Neutral Current
			tempLong = (int32_t)(EM_GetCurrentRMS(LINE_NEUTRAL)*100);
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_C);
			lcd_display_A();
			break;	
			
		case 9:		// Line Frequency
			tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
			display_long_two_dp(tempLong);
  		LCD_DisplayDigit(1, LCD_CHAR_F);
	    LCD_DisplayDigit(9, LCD_CHAR_H);
	    LCD_DisplayDigit(8, 2);
     break;

		case 10:		// Phase Sequence - Voltage
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

	    LCD_DisplayDigit(1, LCD_CHAR_V);
	    LCD_DisplayDigit(9, LCD_CHAR_P);
	    LCD_DisplayDigit(8, 5);
     break;

		case 11:		// Phase Sequence - Current
#ifdef CURRENT_PHASE_SEQUENCE		
		  i[LINE_PHASE_R] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_R)*100);
		  i[LINE_PHASE_Y] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_Y)*100);
		  i[LINE_PHASE_B] = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_B)*100);
			
			tempchar = countCurrentGT95(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);

			if(tempchar==3)
			{
				if(gPhaseSequenceCurrent==0)
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
			{// here all currents are not greater than 1A
				LCD_DisplayDigit(3, LCD_MINUS_SIGN);
				LCD_DisplayDigit(4, LCD_MINUS_SIGN);
				LCD_DisplayDigit(5, LCD_MINUS_SIGN);
			}
#endif
	    LCD_DisplayDigit(1, LCD_CHAR_C);
	    LCD_DisplayDigit(9, LCD_CHAR_P);
	    LCD_DisplayDigit(8, 5);
     break;
		 
    case 12:		// pf total with sign
    	pf_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,LINE_TOTAL);
			tempLong = getInstantaneousParams(IPFS,LINE_TOTAL)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_P);	
			LCD_DisplayDigit(2, LCD_CHAR_F);	

			LCD_DisplayDigit(9, LCD_CHAR_T);
			
			if(total_active_power>0)
			{// this is the regular case - import
				if (pf_sign == PF_SIGN_LEAD_C)
				{
		    	LCD_DisplaySpSign(S_LEAD);
				}
				else if (pf_sign == PF_SIGN_LAG_L)
				{
		    	LCD_DisplaySpSign(S_LAG);
				}
			}
			else
			{
				if (pf_sign == PF_SIGN_LEAD_C)
				{
				    LCD_DisplaySpSign(S_LAG);	// show the ulta
				}
				else if (pf_sign == PF_SIGN_LAG_L)
				{
				    LCD_DisplaySpSign(S_LEAD);	// show the ulta
				}
			}
		 break;

    case 13:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
	    break;

    case 14:		// date
			EM_DisplayDate();
      break;

    case 15:		// time
			EM_DisplayInstantTime();
      break;

	}
}

//avoidShowingRepeatParameters
void handleDisplay_Gujarat(uint8_t t)
{// disp_timer is incremented in the interrupt callback function
//	uint8_t previous_disp_state;
  powermgmt_mode_t power_mode;

	if((g_display_mode==255)||(seconds_flag==0))	// things are not ready yet
		return;
		
// the following function allows a temporary message to be displayed on the LCD	
/*
	if(scrollLock!=0)
	{
		if(disp_timer<3)
			return;
		else
		{
			scrollLock=0;
			if(mode_has_been_changed==1)
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
	}
*/		
	power_mode = POWERMGMT_GetMode();	// actual function to be used
		
	if (power_mode == POWERMGMT_MODE1)
	{
		if((disp_state==0)&&(g_display_mode==1))
		{
			disp_autoscroll_interval=5;
		}
		else
			EM_LCD_DisplayStatus();
	}

/*	
	if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)
	{
		if(g_display_mode==0)
			fast_display_flag=1;
	}
*/

  if((disp_timer >= disp_autoscroll_interval)||(someKeyPressed==1)||(fast_display_flag==1))
	{
		seconds_flag=0;
		if(someKeyPressed==0)
		{// if no key has been pressed then whether it is autoscroll or pushbutton mode the display state should increment
			if(fast_display_flag==0)
			{
				if(holdState==0)
					disp_state++;
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
				if(g_display_mode==1)
				{
					if(holdState==0)
						disp_state++;
				}
			}
			
			if(revKeyPressed==1)
			{
				revKeyPressed=0;
				revKeyPressedCtr=0;
				if(g_display_mode==1)
				{
					if(disp_state==0)
						disp_state=21;	// last pushbutton state
					else
						disp_state--;
				}
			}
		}

		if(disp_autoscroll_interval<10)
			disp_autoscroll_interval=10;
			
		switch(g_display_mode)
		{
			case 0:	// autoscroll
				display_autoScroll();
				break;
				
			case 1:	// pushbutton
				if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)
				{
					if(toggleVariable==0)
					{
						display_onDemand();
						toggleVariable=1;
					}
					else
					{
						displayCoverOpenStatus();
						if(holdState==0)
							toggleVariable=0;
					}
				}
				else
					display_onDemand();
				break;
				
			default:
				makeDisplayReadyForAutoScroll();
				break;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------
// this is for UNIDIR meter
uint8_t g_cover_display_state;
uint8_t g_magnet_display_state;


void displayCoverOpenStatus(void)
{// this function gets called every second if cover open is recorded
	ext_rtc_t datetime;
  R_WDT_Restart();
	
	if(holdState==0)
	{
		holdState=1;
		g_cover_display_state=0;
	}
	
	switch(g_cover_display_state)
	{
		case 0:
			if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)
			{// here cover is open
				if((disp_timer%2)==0)	
				{
					LCD_DisplayDigit(2, LCD_CHAR_C);
					LCD_DisplayDigit(3, LCD_MINUS_SIGN);
					LCD_DisplayDigit(4, 0);
					LCD_DisplayDigit(5, LCD_CHAR_P);
					LCD_DisplayDigit(6, LCD_CHAR_E);
					LCD_DisplayDigit(7, LCD_CHAR_N);
				}
				else
				{
					LCD_DisplayDigit(2, LCD_CLEAR_DIGIT);
					LCD_DisplayDigit(3, LCD_CLEAR_DIGIT);
					LCD_DisplayDigit(4, LCD_CLEAR_DIGIT);
					LCD_DisplayDigit(5, LCD_CLEAR_DIGIT);
					LCD_DisplayDigit(6, LCD_CLEAR_DIGIT);
					LCD_DisplayDigit(7, LCD_CLEAR_DIGIT);
				}
				fast_display_flag=1;
				if(disp_timer>=10)
					g_cover_display_state++;
			}
			else
			{// here there is no cover tamper
				LCD_DisplayDigit(1, LCD_CHAR_C);
				LCD_DisplayDigit(2, LCD_MINUS_SIGN);
				LCD_DisplayDigit(3, LCD_CHAR_C);
				LCD_DisplayDigit(4, LCD_CHAR_L);
				LCD_DisplayDigit(5, 0);
				LCD_DisplayDigit(6, 5);
				LCD_DisplayDigit(7, LCD_CHAR_E);
				fast_display_flag=0;
				holdState=0;	// resume normal sequencing
			}
			break;
			
		case 1:	// show the date
			EPR_Read(EVENTS5_BASE_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, 0);
			fast_display_flag=0;
			g_cover_display_state++;
			break;
			
		case 2:	// show the time
			EPR_Read(EVENTS5_BASE_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, 0);
			fast_display_flag=0;
			holdState=0;	// resume normal sequencing
			break;
	}
}

void displayMagnetTamperStatus(void)
{
//	ext_rtc_t datetime;
	tmprdetails_t earlierTamperDetails;	
	uint32_t dc_mag_mask;
	uint32_t ac_mag_mask;
	
  R_WDT_Restart();
	
	if(holdState==0)
	{
		holdState=1;
		g_magnet_display_state=0;
	}
	
	dc_mag_mask = computeMask(MAGNET);
	ac_mag_mask = computeMask(AC_MAGNET);
	
	switch(g_magnet_display_state)
	{
		case 0:
			if(((mytmprdata.tamperEvent_once&dc_mag_mask) != 0)||((mytmprdata.tamperEvent_once&ac_mag_mask) != 0))
			{// here a magnet tamper has been recorded at least once
				LCD_DisplayDigit(1, LCD_CHAR_M);
				LCD_DisplayDigit(2, LCD_CHAR_A);
				LCD_DisplayDigit(3, LCD_CHAR_G);
				LCD_DisplayDigit(4, LCD_CHAR_T);
				LCD_DisplayDigit(5, LCD_CHAR_M);
				LCD_DisplayDigit(6, LCD_CHAR_P);
				LCD_DisplayDigit(7, LCD_CHAR_R);
				g_magnet_display_state++;
			}
			else
			{// here there is no magnet tamper in the meter
				LCD_DisplayDigit(1, LCD_CHAR_M);
				LCD_DisplayDigit(2, LCD_CHAR_A);
				LCD_DisplayDigit(3, LCD_CHAR_G);
				LCD_DisplayDigit(4, LCD_MINUS_SIGN);
				LCD_DisplayDigit(5, LCD_CHAR_N);
				LCD_DisplayDigit(6, LCD_CHAR_I);
				LCD_DisplayDigit(7, LCD_CHAR_L);
				holdState=0;	// resume normal sequencing
			}
			break;
			
		case 1:	// show the date
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
			lcd_display_date(earlierTamperDetails.tamperTime.date,earlierTamperDetails.tamperTime.month,earlierTamperDetails.tamperTime.year);
			LCD_DisplayDigit(9, LCD_CHAR_M);
			LCD_DisplayDigit(8, LCD_CHAR_T);
			g_magnet_display_state++;
			break;
			
		case 2:	// show the time
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&earlierTamperDetails,sizeof(earlierTamperDetails));
			lcd_display_date(earlierTamperDetails.tamperTime.hour,earlierTamperDetails.tamperTime.minute,earlierTamperDetails.tamperTime.second);
			LCD_DisplayDigit(9, LCD_CHAR_M);
			LCD_DisplayDigit(8, LCD_CHAR_T);
			g_magnet_display_state++;
			holdState=0;	// resume normal sequencing
			break;
	}
}


void display_autoScroll(void)
{
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

  R_WDT_Restart();
  LCD_ClearAll();	
	
	if(comm_mode!=0)
	{
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}
	
//	if(disp_state>0)
		EM_LCD_DisplayStatus();

	if(disp_state>2)
		disp_state=0;

	switch(disp_state)
	{
		case 0:
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
			break;
			
    case 1:		// cover open status
			displayCoverOpenStatus();
			break;
			
    case 2:		// magnet tamper status
			displayMagnetTamperStatus();
			break;
	}
}

void display_onDemand(void)
{
//	toddata_t temptoddata;
//	int32_t tempTodKwh;
  powermgmt_mode_t power_mode;
	int32_t tempLong;
	uint16_t tempint;
	ext_rtc_t datetime;
	EM_PF_SIGN pf_sign;    
	uint8_t tempchar;
	uint8_t tmonth;

  R_WDT_Restart();
  LCD_ClearAll();

	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE1)
		EM_LCD_DisplayStatus();

	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>21)
		disp_state=0;	// cannot be increased by more than 14 - last state
	
		
	switch(disp_state)
	{
    case 0:			// all on
      LCD_DisplayAll();
			fast_display_flag=0;
			disp_autoscroll_interval=5;
      break;

    case 1:		// meter number
		  lcd_display_multiple_chars(0,6,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(Common_Data.meterNumber, LCD_LAST_POS_DIGIT,0);
			lcd_display_id();
			fast_display_flag=0;
			disp_autoscroll_interval=10;
     	break;
			
    case 2:		// time
			EM_DisplayInstantTime();
			fast_display_flag=1;
      break;

    case 3:		// date
			EM_DisplayDate();
			fast_display_flag=0;
      break;

    case 4:		// kWH
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     break;

    case 5:		// kVAH
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvah/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVAhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     break;

    case 6:		// kVArH-lag
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lag/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
    	LCD_DisplaySpSign(S_LAG);
			fast_display_flag=0;
     break;

    case 7:		// kVArH-lead
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lead/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
    	LCD_DisplaySpSign(S_LEAD);
			fast_display_flag=0;
     break;

    case 8:		// Current MD in kW
			display_long_three_dp(mymddata.mdkw);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
			
    case 9:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
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
			
    case 16:		// pf total with sign
    	pf_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,LINE_TOTAL);
			tempLong = getInstantaneousParams(IPFS,LINE_TOTAL)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_P);	
			LCD_DisplayDigit(2, LCD_CHAR_F);	

			LCD_DisplayDigit(9, LCD_CHAR_T);
			
			if (pf_sign == PF_SIGN_LEAD_C)
			{
	    	LCD_DisplaySpSign(S_LEAD);
			}
			else if (pf_sign == PF_SIGN_LAG_L)
			{
	    	LCD_DisplaySpSign(S_LAG);
			}
			else if (pf_sign == PF_SIGN_LAG_L)
			{
	    	LCD_ClearSpSign(S_LEAD);
	    	LCD_ClearSpSign(S_LAG);
			}
			fast_display_flag=1;
		 break;

    case 17:			// high resolution kwh
			tempLong = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
			display_long_five_dp(tempLong);
			LCD_DisplaykWhSign();
			fast_display_flag=1;
      break;
			
    case 18:		// L1 - Cum kwh
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

    case 19:		// L1 - Cum kVAh
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+60,(uint8_t*)&tempLong,4);
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykVAhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     	break;
			
    case 20:		// L1 - MD
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+96,(uint8_t*)&tempLong,4);
		  display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     	break;
			
    case 21:		// L1 - Average pf
			tempchar=1;
			tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+12,(uint8_t*)&tempLong,4);
		  display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			LCD_DisplayDigit(1, LCD_CHAR_A);
			LCD_DisplayDigit(2, LCD_CHAR_P);
			fast_display_flag=0;
			break;			
	}
}

/*
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
			
    case 1:			// high resolution kwh - fundamental
			temp_high_res_energy = ((myenergydata.kwh_fundamental%100000)*100)+(int32_t)high_resolution_kwh_fundamental_value;
//			temp_high_res_energy = ((myenergydata.kwh_export%100000)*100)+(int32_t)high_resolution_kwh_value_export;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykWhSign();
			LCD_DisplayDigit(9, LCD_CHAR_F);
			LCD_DisplayDigit(8, LCD_CHAR_D);
			fast_display_flag=1;
      break;
			
    case 2:			// high resolution kvarh lag
			temp_high_res_energy = ((myenergydata.kvarh_lag%100000)*100)+(int32_t)high_resolution_kvarh_lag_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykVArhSign();
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













