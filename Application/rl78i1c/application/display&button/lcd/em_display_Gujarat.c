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

#define RETURN_TO_AUTOSCROLL_COUNT	7500	// corresponds to 300 seconds or 5 minutes

#ifdef BIDIR_METER
	#define NO_OF_SUB_DISPLAY_STATES 4	// BIDIR_METER has 4
#else
	#define NO_OF_SUB_DISPLAY_STATES 3	// Regular Gujarat Meter has 3
#endif

uint8_t disp_state;
uint8_t disp_timer;
uint8_t disp_autoscroll_interval=10;


uint8_t			g_display_mode = 0;	// 0 - Autoscroll, 1- OnDemand, 2-ThirdDisplay, 3-Special Autoscroll
//uint8_t			g_display_update_type =0;	// 0 - Autoincrement, 1- OnKeyPressOnly, 2-AutoIncrementEverysecond, 3-KeyPressIncrementEverySecond
uint8_t     g_is_por = 0;

uint8_t gentimer;
uint8_t tamperTimer;

uint8_t comm_mode;
uint8_t commModeTimer;

uint8_t previousMode;	// 0 - import, 1 - export

uint8_t kbscanflag;
uint8_t keyPressedCtr;
//uint16_t keyNotPressedCtr;

//uint8_t keyNotPressedCtr;
uint8_t keyPressedCtr1;
//uint8_t keyNotPressedCtr1;

uint8_t keyPressedCtr2;
uint8_t keyNotPressedCtr2;

int16_t mode_timeout_ctr;
uint8_t	seconds_flag;
uint8_t sub_disp_state=255;
uint8_t avoidShowingRepeatParameters;
uint8_t alt_autoscroll_mode;
uint8_t autoscrollInButtonMode;
uint8_t displayHold;

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
	int16_t compareValue = 297; // five minutes
	
	disp_timer++;
	gentimer++;
	if(displayHold!=0)
	{
		if(gentimer>3)
			displayHold=0;
	}
	
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
// what happens if the key is stuck
			if(keyPressedCtr<60)
			{
				if(keyPressedCtr>4)
				{
					if(sleepCommMode==0)
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
			}
			else
			{// here the key has been pressed for 60 seconds or more - looks like the key is stuck
				// don't do anything till the key gets unstuck - this will prevent the UARTs from
				// getting reactivated in MODE3
				keyPressedCtr=61;	// keep it at 61 to avoid overflow of uint8_t
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
		if(avoidShowingRepeatParameters!=0)
		{
			mode_timeout_ctr++;
#ifdef BIDIR_METER
			if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)
				compareValue = 297;	// for cover open case - 5 minutes
			else
				compareValue = 897;	// for no cover open - 15 minutes

			if(mode_timeout_ctr>compareValue)	//  5 minutes or 15 minutes
#else
			if(mode_timeout_ctr>297)	// make this 295 - 5 minutes - 300 seconds	- please use this
//			if(mode_timeout_ctr>57)	// for testing only - 1 minutes - 60 seconds
#endif
			{
				makeDisplayReadyForAutoScroll();
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
//uint8_t noKeyPressedCtr;

uint8_t	fast_display_flag;
uint8_t	fast_display_counter;

uint8_t prevTopSW=1;
uint8_t prevMiddleSW=1;

int8_t scrollLock;

void handleDisplay_Gujarat(uint8_t a);
void changeDisplayMode(void);
void displayRepeatParameters(void);

void changeDisplayMode(void)
{// both keys have been pressed for some time
// 0 - autoscroll, 1 - displayOnDemand, 2 - High resolution display
	g_display_mode++;
	if(g_display_mode==3)
	{
		g_display_mode=0;
		alt_autoscroll_mode=0;
		sub_disp_state=255;
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
		if(bothKeysPressedCtr>25)	// 15 earlier
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
			if(fwdKeyPressedCtr>4)	// 1 earlier
			{
				if((g_display_mode==0)&&(autoscrollInButtonMode==0))
				{
					disp_state=255;
					sub_disp_state=255;
				}
				someKeyPressed=1;
				fwdKeyPressedCtr=0;
			}
		}
		else
		{// forward key is definitely not pressed
			fwdKeyPressedCtr=0;
			if((revKeyPressed==1)&&(fwdKeyPressed==0))
			{
				bothKeysPressedCtr=0;
				revKeyPressedCtr++;
				if(revKeyPressedCtr>4)	// 10 earlier
				{
					someKeyPressed=1;
					revKeyPressedCtr=0;
				}
			}
			else
			{// here no key has been pressed
//				noKeyPressedCtr++;
				revKeyPressedCtr=0;
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

	if(displayHold==0)
	{
		if(calswitch!=0)
			handleDisplay_Production();
		else
			handleDisplay_Gujarat(0);
	}
		
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
				displayHold=1;	// this will prevent the display function to be called
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
/*
// By using the displayHold feature which gets cleared in 3 seconds - the following is not required
				do
				{
					R_WDT_Restart();
					per_second_activity();
					wre2rom();
				}
				while(gentimer<3);
*/				
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

#ifdef CHARGER
	disp_state=0;
#else
	if(disp_state>15)
		disp_state=0;
#endif

	switch(disp_state)
	{
#ifdef CHARGER
    case 0:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			disp_autoscroll_interval=1;
	    break;
#else
    case 0:		// Cal staus
			lcd_display_Cal_Status();
			disp_autoscroll_interval=4;
     	break;

#endif			
			
			
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
// The array i is being used to store voltages
				i[LINE_PHASE_R] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*10);
				i[LINE_PHASE_Y] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*10);
				i[LINE_PHASE_B] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*10);
				
				tempchar = countVoltageLT50(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
				if(tempchar==0)
//				if((g_TamperStatus&MISSPOT_MASK)==0)
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
	uint8_t previous_disp_state;
  powermgmt_mode_t power_mode;

//	disp_autoscroll_interval=5;
	
	if((g_display_mode==255)||(seconds_flag==0))	// things are not ready yet
		return;
		
		
	power_mode = POWERMGMT_GetMode();	// actual function to be used
		
	if (power_mode == POWERMGMT_MODE1)
	{
#ifdef BIDIR_METER		
		if((g_display_mode==0)&&(disp_state==49))
#else
		if((g_display_mode==0)&&(disp_state==0))
#endif
		{
		}
		else
			EM_LCD_DisplayStatus();
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
					previous_disp_state = disp_state;
					if(fast_display_flag==0)
					{
						if(alt_autoscroll_mode==0)
							disp_state++;
						else
						{
							sub_disp_state++;
//							if(sub_disp_state==3)
							if(sub_disp_state==NO_OF_SUB_DISPLAY_STATES)
							{
								sub_disp_state=255;
								alt_autoscroll_mode=0;
								disp_state++;
							}
						}
					}
					else
					{// here fast_display_flag = 1 - wait for autoscroll interval before incrementing state
						fast_display_counter++;
						if(fast_display_counter>disp_autoscroll_interval)
						{
//							disp_state++;
							fast_display_flag=0;
							fast_display_counter=0;
							alt_autoscroll_mode=1;
							sub_disp_state=255;
						}
					}
				}
			}
			else
			{// here we are in those modes which are supposed to operate by pushbutton
			// once button is pressed scrollLock will be made 1, and now display will not scroll
				if(scrollLock==0)
				{
					disp_autoscroll_interval=10;
					disp_state++;	// allow the other displays to scroll if no button is pressed
				}
			}
			disp_timer=0;
		}
		else
		{// here some key has been pressed
			if(mode_has_been_changed==1)
			{
				someKeyPressed=1;	// make sure this remains 1
				if(disp_timer>=2)
				{
					mode_has_been_changed=0;
					fwdKeyPressed=0;	// this is to ensure that after mode display is cleared user must press either key to proceed
					revKeyPressed=0;
				}
				return;
			}
			else
			{
				disp_timer=0;
				someKeyPressed=0;
				mode_timeout_ctr=0;
				avoidShowingRepeatParameters=1;
				scrollLock=1;	// now it will scroll only with press of button
			}
			
			if(fwdKeyPressed==1)
			{
				fwdKeyPressed=0;
				fwdKeyPressedCtr=0;
				if(g_display_mode==0)
				{
					if(autoscrollInButtonMode==0)
					{
						autoscrollInButtonMode=1;
						sub_disp_state=0;
						displayRepeatParameters();	
					}
					else
					{// here autoscrollInButtonMode is 1 and fwdKey is pressed
						if(sub_disp_state==255)
						{
							disp_state++;
							
#ifdef BIDIR_METER								
							if(disp_state>49)
#else
							if(disp_state>19)
#endif
							{// this means that now we are going to come back to state 0
								sub_disp_state=0;
								displayRepeatParameters();	
							}
							else
							 display_autoScroll();
						}
						else
						{
							sub_disp_state++;
//							if(sub_disp_state==3)
							if(sub_disp_state==NO_OF_SUB_DISPLAY_STATES)
							{
								sub_disp_state=255;
								disp_state=0;
								display_autoScroll();
							}
							else
								displayRepeatParameters();
						}
					}
//					return;
				}
				else
					disp_state++;
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
						sub_disp_state=0;
						displayRepeatParameters();	
					}
					else
					{// here autoscrollInButtonMode is in button mode and REV key is pressed
						if((sub_disp_state>=1)&&(sub_disp_state!=255))
						{
							sub_disp_state--;
							displayRepeatParameters();
						}
						else
						{// here sub_disp_state is either 0 or it is 255
							if((sub_disp_state==0)||(sub_disp_state==255))
							{
								sub_disp_state=255;
								
								if(disp_state==0)
								{// rev button has been pressed
#ifdef BIDIR_METER								
									disp_state=49;
#else
									disp_state=19;
#endif
									display_autoScroll();
								}
								else
								{
									if((disp_state>=1)&&(disp_state!=255))
									{
										disp_state--;
										display_autoScroll();
									}
								}
							}
						}
//						return;	
					}
				}
				else
				{
					if(disp_state==0)
					{
						if(g_display_mode==1)
						{// we are in displayOnDemand mode
#ifdef BIDIR_METER
							disp_state=14;
#else
							disp_state=26;
#endif							
						}
						else
						{
							if(g_display_mode==2)
							{// we are in high res display mode - this has same number of display states for both UNIDIR and BIDIR
								disp_state=3;
							}
						}
					}
					else
					{
//						if(disp_state>=1)
							disp_state--;
					}
				}
			}
		}
		
//		if(mode_has_been_changed==1)
//			return;
			
		switch(g_display_mode)
		{
			case 0:	// autoscroll
				if(autoscrollInButtonMode==0)
				{
					if(alt_autoscroll_mode==0)
					{
						display_autoScroll();
						if((previous_disp_state!=disp_state)&&(fast_display_flag==0)&&(avoidShowingRepeatParameters==0))
						{
							alt_autoscroll_mode=1;
							sub_disp_state=255;
						}
					}
					else
						displayRepeatParameters();
				}
				else
				{// here autoscroll is in button mode
					if(fast_display_flag==1)
						display_autoScroll();
				}
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


#ifdef BIDIR_METER
void displayRepeatParameters(void)
{
	uint16_t tempint;
  powermgmt_mode_t power_mode;
	uint8_t tempchar;
	uint8_t tmonth;
	
  LCD_ClearAll();
	disp_autoscroll_interval=10;
	
	if(someKeyPressed==0)
	{	
		if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(autoscrollInButtonMode==0))
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
	else
		someKeyPressed=0;
	
	if((comm_mode!=0)&&(autoscrollInButtonMode==0))
	{
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}
	
	
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE1)
		EM_LCD_DisplayStatus();

	if(sub_disp_state>3)
		sub_disp_state=0;	

	switch(sub_disp_state)
	{
    case 0:		// total kwh - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
  		LCD_DisplaySpSign(S_IMPORT);
     	break;

    case 1:		// total kwh - Cumulative Export
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh_export/1000, LCD_LAST_POS_DIGIT,0);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
  		LCD_DisplaySpSign(S_EXPORT);
     	break;
			
    case 2:		// total kvarh - lag
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lag/1000, LCD_LAST_POS_DIGIT,0);
//			display_long_one_dp(myenergydata.kvarh_lag/100);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
  		LCD_DisplaySpSign(S_IMPORT);
  		LCD_DisplaySpSign(S_LAG);
     	break;
			
    case 3:		// Cumulative MD in kW
			LCD_DisplayIntWithPos(ctldata.cumulative_md/1000, LCD_LAST_POS_DIGIT,0);
//			display_long_three_dp(ctldata.cumulative_md);
//	    LCD_DisplayDigit(1, LCD_CHAR_C);
			LCD_DisplaykWSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
		  LCD_DisplaySpSign(S_MD);
  		LCD_DisplaySpSign(S_IMPORT);
      break;
	}
}

// this is a private function for the display_autoScroll to prevent flickering
// when tod is displayed
void displayPrepare(void);

void displayPrepare(void)
{
  LCD_ClearAll();	
	if(disp_state<49)
		EM_LCD_DisplayStatus();
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
	
	
	if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(autoscrollInButtonMode==0))
	{
	  LCD_ClearAll();	
		LCD_DisplayDigit(2, LCD_CHAR_C);
		LCD_DisplayDigit(3, LCD_MINUS_SIGN);
		LCD_DisplayDigit(4, 0);
		LCD_DisplayDigit(5, LCD_CHAR_P);
		LCD_DisplayDigit(6, LCD_CHAR_E);
		LCD_DisplayDigit(7, LCD_CHAR_N);
		return;
	}
	
	if((comm_mode!=0)&&(autoscrollInButtonMode==0))
	{
	  LCD_ClearAll();	
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}

/*	
  LCD_ClearAll();	
	if(disp_state<47)
		EM_LCD_DisplayStatus();
*/

	if(disp_state>49)
		disp_state=0;

	switch(disp_state)
	{
    case 0:		// meter number
			displayPrepare();
		  lcd_display_multiple_chars(0,6,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(Common_Data.meterNumber, LCD_LAST_POS_DIGIT,0);
			lcd_display_id();
			fast_display_flag=0;
     	break;
	
    case 1:		// Voltage - R
			displayPrepare();
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 2:		// Voltage - Y
			displayPrepare();
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 3:		// Voltage - B
			displayPrepare();
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
	    break;

    case 4:		// Current - R with direction
			displayPrepare();
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			lcd_display_A();
			fast_display_flag=1;
      break;

    case 5:		// Current - Y with direction
			displayPrepare();
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
			lcd_display_A();
			fast_display_flag=1;
      break;

   	case 6:		// Current - B with direction
			displayPrepare();
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
			lcd_display_A();
			fast_display_flag=1;
      break;
/*
   	case 7:		// Neutral Current
			tempLong = (int32_t)(EM_GetCurrentRMS(LINE_NEUTRAL)*100);
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_C);
			lcd_display_A();
			fast_display_flag=1;
			break;	
			
   	case 8:		// Vector Current
			tempLong = (int32_t)(EM_GetPhaseCurrentSumVector()*100);
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_C);
			lcd_display_A();
			fast_display_flag=1;
			break;	
*/			
			
		case 7:		// Line Frequency
			displayPrepare();
			tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
			display_long_two_dp(tempLong);
  		LCD_DisplayDigit(1, LCD_CHAR_F);
	    LCD_DisplayDigit(9, LCD_CHAR_H);
	    LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
     break;

		case 8:		// Phase Sequence - Voltage
				displayPrepare();
// The array i is being used to store voltages
				i[LINE_PHASE_R] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*10);
				i[LINE_PHASE_Y] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*10);
				i[LINE_PHASE_B] = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*10);
				
				tempchar = countVoltageLT50(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
				if(tempchar==0)
//				if((g_TamperStatus&MISSPOT_MASK)==0)
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
			fast_display_flag=1;
     break;

		case 9:		// Phase Sequence - Current
			displayPrepare();
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
			fast_display_flag=1;
     break;
		 
    case 10:		// pf total with sign
			displayPrepare();
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
			fast_display_flag=1;
		 break;

    case 11:		// Instant Load in kW
			displayPrepare();
//			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);

			if(((mytmprdata.tamperEvent_image&maskTable[MAGNET])==0)&&((mytmprdata.tamperEvent_image&maskTable[AC_MAGNET])==0))
			{// no magnet tamper here
				if((fault_code&_BV(9))==0)
				{// normal case
					tempLong = (int32_t)EM_GetActivePower(LINE_PHASE_R);	// with sign
					tempLong += (int32_t)EM_GetActivePower(LINE_PHASE_Y);	// with sign
					tempLong += (int32_t)EM_GetActivePower(LINE_PHASE_B);	// with sign
				}
				else
				{// here there is neutral tamper
					tempLong=ABS(240*EM_GetCurrentRMS(LINE_PHASE_R));	// is the ABS necessary
					tempLong+=ABS(240*EM_GetCurrentRMS(LINE_PHASE_Y));
					tempLong+=ABS(240*EM_GetCurrentRMS(LINE_PHASE_B));
				}
			}
			else
			{// here there is magnet
				tempLong=(int32_t)(MAGNET_IMAX_POWER*3.0);
			}

//			tempLong = (int32_t)EM_GetActivePower(LINE_PHASE_R);	// with sign
//			tempLong += (int32_t)EM_GetActivePower(LINE_PHASE_Y);	// with sign
//			tempLong += (int32_t)EM_GetActivePower(LINE_PHASE_B);	// with sign
			
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
	    break;

/*			
    case 12:		// neutral_current
			tempLong = EM_GetCurrentRMS(LINE_NEUTRAL)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_C);
			lcd_display_A();
			fast_display_flag=1;
			break;
			
    case 12:		// vector_current
			tempLong = EM_GetPhaseCurrentSumVector()*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_C);
			lcd_display_A();
			fast_display_flag=1;
			break;
*/			

    case 12:		// day
			tempchar = (uint8_t)CalcDayNumFromDate((2000+rtc.year), rtc.month, rtc.date);
			displayPrepare();
			displayDay(tempchar);
			fast_display_flag=0;
      break;
			
    case 13:		// date
			displayPrepare();
			EM_DisplayDate();
			fast_display_flag=0;
      break;

    case 14:		// time
			displayPrepare();
			EM_DisplayInstantTime();
			fast_display_flag=1;
      break;

    case 15:		// Rising MD in kW
			displayPrepare();
	    tempchar=60/(int32_t)Common_Data.mdinterval;
			tempLong = (int32_t)EM_GetActivePower(LINE_PHASE_R);
			tempLong += (int32_t)EM_GetActivePower(LINE_PHASE_Y);
			tempLong += (int32_t)EM_GetActivePower(LINE_PHASE_B);
			
			tempLong1 = tempLong;
			if(tempLong1<0)
				tempLong1*=-1;	// this is now always positive
				
			if(tempLong1<7)
			{// here there is zero active power
				if(previousMode==0)
				{// import mode
					tempTodKwh = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);
					if(tempTodKwh<0)
					{	ctldata.kwh_last_saved=myenergydata.kwh;
						tempTodKwh=0;
					}
					display_long_three_dp(tempTodKwh);	// rising MD
			  	LCD_DisplaySpSign(S_IMPORT);
				}
				else
				{// export mode
					tempTodKwh = ((myenergydata.kwh_export-ctldata.kwh_last_saved_export)*(int32_t)tempchar);
					if(tempTodKwh<0)
					{	ctldata.kwh_last_saved_export=myenergydata.kwh_export;
						tempTodKwh=0;
					}
					display_long_three_dp(tempTodKwh);	// rising MD
			  	LCD_DisplaySpSign(S_EXPORT);
				}
			}
			else
			{// here there is some active power - may be positive or negative
				if(tempLong>0)
				{// import case - present active power is +ve
					previousMode = 0;	// import mode
					tempTodKwh = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);
					if(tempTodKwh<0)
					{	ctldata.kwh_last_saved=myenergydata.kwh;
						tempTodKwh=0;
					}
					display_long_three_dp(tempTodKwh);	// rising MD
			  	LCD_DisplaySpSign(S_IMPORT);
				}
				else
				{// export case - present active power is -ve
					previousMode = 1;	// export mode
					tempTodKwh = ((myenergydata.kwh_export-ctldata.kwh_last_saved_export)*(int32_t)tempchar);
					if(tempTodKwh<0)
					{	ctldata.kwh_last_saved_export=myenergydata.kwh_export;
						tempTodKwh=0;
					}
					display_long_three_dp(tempTodKwh);	// rising MD
			  	LCD_DisplaySpSign(S_EXPORT);
				}
			}
			
			LCD_DisplaykWSign();
	    LCD_DisplaySpSign(S_TIME);	// show the time icon also

// we can also display the minutes elapsed
			tempchar = rtc.minute - ((rtc.minute/Common_Data.mdinterval)*Common_Data.mdinterval); 
  		LCD_DisplayDigit(1, LCD_CHAR_R);
  		LCD_DisplayDigit(2, LCD_CHAR_D);
  		LCD_DisplayDigit(9, tempchar/10);
  		LCD_DisplayDigit(8, tempchar%10);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=1;
      break;

    case 16:		// total kwh - Cumulative  Import KWH 
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_IMPORT);
			fast_display_flag=1;
     	break;
			
    case 17:		// total kwh - Cumulative Export
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kwh_export/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_EXPORT);
			fast_display_flag=1;
     	break;
			
    case 18:		// total kvarh - lag import
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvarh_lag/100);
			LCD_DisplaykVArhSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplaySpSign(S_LAG);
			fast_display_flag=1;
     	break;
			
    case 19:		// total kvarh - lag export
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvarh_lag_export/100);
			LCD_DisplaykVArhSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplaySpSign(S_LAG);
			fast_display_flag=1;
     	break;
			
    case 20:		// total kvah import
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvah/100);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_IMPORT);
			fast_display_flag=1;
     	break;
			
    case 21:		// total kvah export
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvah_export/100);
			LCD_DisplaykVAhSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_EXPORT);
			fast_display_flag=1;
     	break;
			
// Cum Import KWH peak hours

    case 22:		//Import kwh	-	Peak hours script id 1, (scriptid-1)=0
			tempchar=0;	// this is now (scriptid-1)
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh; // myenergydata.zone_kwh - no plus sign needed
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));			
			
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
			fast_display_flag=1;
     break;

// Cum Import KWH night hours
    case 23:			// Import kwh - Night Hours script id 2, (scriptid-1)=1
			tempchar=1; // this is now (scriptid-1)
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;	// no plus sign needed
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));

			displayPrepare();
			lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_H);
			fast_display_flag=1;
      break;

// Cum Import KWH remaining hours
    case 24:			// T O D - kwh - Remaining Hours , script id 3 & 4, (scriptid-1)=2 & 3
			tempchar=2;// this is now (scriptid-1)
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;	// no plus sign needed
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			
// this particular script id is being clubbed with the upper one - SPECIAL			
			tempchar=3;
			if(tempchar==presentZone)
				tempLong=myenergydata.zone_kwh;	// no plus sign needed
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempLong, sizeof(tempLong));
				
			tempTodKwh+=tempLong;
			
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, LCD_CHAR_H);
			fast_display_flag=1;
      break;

// MD Import kw for present Billing Period
    case 25:		// Current MD in kW
			displayPrepare();
			display_long_three_dp(mymddata.mdkw);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaySpSign(S_IMPORT);
			fast_display_flag=0;
      break;
			
// MD Import kw for present Billing Period - Peak Hours
    case 26:		//MD Import kw	-	Peak hours script id 1, (scriptid-1)=0
			tempchar=0;	// this is now (scriptid -1)
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+12+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodMD, sizeof(tempTodMD));
			displayPrepare();
			display_long_three_dp(tempTodMD);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     break;

// MD Import kw for present Billing Period - Night Hours
    case 27:		//MD Import kw	-	script id 2, (scriptid-1)=1
			tempchar=1;	// this is now (scriptid-1)
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+12+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodMD, sizeof(tempTodMD));
			displayPrepare();
			display_long_three_dp(tempTodMD);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     break;

// MD Import kw for present Billing Period - Remaining Hours
    case 28:		//MD Import kw	-	Remaining hours script id 3 & 4, (scriptid-1)=2 & 3
			tempchar=2; // this is now (scriptid-1)
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+12+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodMD, sizeof(tempTodMD));

// this particular script id is being clubbed with the upper one - SPECIAL			
			
			tempchar=3;
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+12+tempchar*ZONE_PTR_INC, (uint8_t*)&tempint, sizeof(tempint));

// we will not display date and time
			if(tempint>tempTodMD)
			{// we display tempTodMD which should contain the higher value
				tempTodMD = tempint;
			}
			
			displayPrepare();
			display_long_three_dp(tempTodMD);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     break;

// MD Import kw for last Billing Period - this is now 4 bytes
    case 29:		// L1 - MD
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+72,(uint8_t*)&tempLong,4);
			displayPrepare();
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplaykWSign();
			LCD_DisplaySpSign(S_IMPORT);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
	    break;

// MD Import kw for last Billing Period - Peak Hours - 4bytes
    case 30:		//MD Import kw	-	Peak hours script id 1, (scriptid-1)=0
		// this information is to be picked up from Billing area in e2rom
      tmonth = decrementMonth(1);
			tempchar=0;	// this is now (scriptid-1)
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 88 + (tempchar*4),(uint8_t*)&tempLong1,sizeof(tempLong1));
			displayPrepare();
			display_long_three_dp(tempLong1);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     	break;

// MD Import kw for last Billing Period - Night Hours
    case 31:		//MD Import kw	-	Night hours script id 2, (scriptid-1)=1
		// this information is to be picked up from Billing area in e2rom
      tmonth = decrementMonth(1);
			tempchar=1; // this is now (scriptid-1)
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 88 + (tempchar*4),(uint8_t*)&tempLong1,sizeof(tempLong1));
			displayPrepare();
			display_long_three_dp(tempLong1);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     	break;

// MD Import kw for last Billing Period - Remaining Hours
    case 32:		//MD Import kw	-	Remaining hours script id 3 & 4, (scriptid-1)=2 & 3
		// this information is to be picked up from Billing area in e2rom
      tmonth = decrementMonth(1);
			tempchar=2;	// this is now (scriptid-1)
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 88 + (tempchar*4),(uint8_t*)&tempLong1,sizeof(tempLong1));
			
// this particular script id is being clubbed with the upper one - SPECIAL			
				
			tempchar=3;
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 88 + (tempchar*4),(uint8_t*)&tempLong,sizeof(tempLong));

			if(tempLong1>tempLong)
			{// we display tempTodMD which should contain the higher value
				tempLong = tempLong1;
			}
			
			displayPrepare();
			display_long_three_dp(tempLong);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     	break;

// MD Export kw for present Billing Period
    case 33:		// Current MD Export in kW
			displayPrepare();
			display_long_three_dp(mymddata.mdkw_export);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaySpSign(S_EXPORT);
			fast_display_flag=0;
      break;

// MD Export kw for present Billing Period - Peak Hours
    case 34:		//MD Export kw	-	Peak hours script id 1, (scriptid-1)=0
			tempchar=0;
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+40+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodMD, sizeof(tempTodMD));
			displayPrepare();
			display_long_three_dp(tempTodMD);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     break;

// MD Export kw for present Billing Period - Night Hours
    case 35:		//MD Export kw	- Night hours	script id 2, (scriptid-1)=1
			tempchar=1;	// this is now (scriptid-1)
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+40+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodMD, sizeof(tempTodMD));
			displayPrepare();
			display_long_three_dp(tempTodMD);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     break;

// MD Export kw for present Billing Period - Remaining Hours
    case 36:		//MD Export kw	-	Remaining hours scriptid 3 & 4, (scriptid-1)=2 & 3
			tempchar=2;	// this is now (scriptid-1)
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+40+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodMD, sizeof(tempTodMD));
			
// this particular script id is being clubbed with the upper one - SPECIAL			

			tempchar=3;
			EPR_Read(TOD_CURRENT_BASE_ADDRESS+40+tempchar*ZONE_PTR_INC, (uint8_t*)&tempint, sizeof(tempint));

// we will not display date and time
			if(tempint>tempTodMD)
			{// we display tempTodMD which should contain the higher value
				tempTodMD = tempint;
			}
			
			displayPrepare();
			display_long_three_dp(tempTodMD);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     break;

// MD Export kw for last Billing Period
    case 37:		// L1 - MD Export 
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+344,(uint8_t*)&tempLong,sizeof(tempLong));
			displayPrepare();
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplaykWSign();
			LCD_DisplaySpSign(S_EXPORT);
			fast_display_flag=0;
	    break;

// MD Export kw for last Billing Period - Peak Hours
    case 38:		//MD Export kw	-	Peak hours , scriptid 1, (scriptid-1)=0
		// this information is to be picked up from Billing area in e2rom
      tmonth = decrementMonth(1);
			tempchar=0;	// this is now (scriptid-1)
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 360 + (tempchar*4),(uint8_t*)&tempLong1,sizeof(tempLong1));
			displayPrepare();
			display_long_three_dp(tempLong1);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     	break;
			
// MD Export kw for last Billing Period - Night Hours
    case 39:		//MD Export kw	-	Night hours scriptid 2, (scriptid-1)=1
		// this information is to be picked up from Billing area in e2rom
      tmonth = decrementMonth(1);
			tempchar=1;	// this is now (scriptid-1)
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 360 + (tempchar*4),(uint8_t*)&tempLong1,sizeof(tempLong1));
			displayPrepare();
			display_long_three_dp(tempLong1);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     	break;

// MD Export kw for last Billing Period - Remaining Hours
    case 40:		//MD Export kw	-	Remaining hours scriptid 3 & 4, (scriptid-1)=2 & 3
		// this information is to be picked up from Billing area in e2rom
      tmonth = decrementMonth(1);
			tempchar=2;	// this is now (scriptid-1)
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 360 + (tempchar*4),(uint8_t*)&tempLong1,sizeof(tempLong1));
			
// this particular script id is being clubbed with the upper one - SPECIAL			

			tempchar=3;
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC + 360 + (tempchar*4),(uint8_t*)&tempLong,sizeof(tempLong));

			if(tempLong1>tempLong)
			{// we display tempTodMD which should contain the higher value
				tempLong = tempLong1;
			}

			displayPrepare();
			display_long_three_dp(tempLong);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplaySpSign(S_EXPORT);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, LCD_CHAR_H);
		  LCD_DisplaySpSign(S_MD);	// this must be done after displaying digit 8
			fast_display_flag=0;
     	break;

// Non Solar hour Export kwh
    case 41:		//Non Solar Export kwh
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kwh_nonSolar_export/100);
			LCD_DisplaykWhSign();
			LCD_DisplaySpSign(S_EXPORT);
 	    LCD_DisplayDigit(9, LCD_CHAR_N);
 	    LCD_DisplayDigit(8, 5);
			fast_display_flag=1;
			break;

// Solar hour Export kwh
    case 42:		//Solar hours Export kwh
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kwh_Solar_export/100);
			LCD_DisplaykWhSign();
			LCD_DisplaySpSign(S_EXPORT);
// 	    LCD_DisplayDigit(9, LCD_CHAR_N);
 	    LCD_DisplayDigit(9, 5);
 	    LCD_DisplayDigit(8, LCD_CHAR_O);
			fast_display_flag=1;
			break;

// Solar hours Import kwh
    case 43:		//Solar hours Import kwh
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kwh_Solar_import/100);
			LCD_DisplaykWhSign();
			LCD_DisplaySpSign(S_IMPORT);
// 	    LCD_DisplayDigit(9, LCD_CHAR_N);
 	    LCD_DisplayDigit(9, 5);
 	    LCD_DisplayDigit(8, LCD_CHAR_O);
			fast_display_flag=1;
			break;

// Cumulative MD in Import KW
    case 44:		// Cumulative MD in kW
			displayPrepare();
			display_long_three_dp(ctldata.cumulative_md);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
		  LCD_DisplaySpSign(S_MD);
  		LCD_DisplaySpSign(S_IMPORT);
			fast_display_flag=0;
      break;

// Reset Count - billing count
    case 45:		// billing count
			displayPrepare();
			LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

// Total tamper Count
    case 46:		// Total Tamper count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.tamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

// Programming Count
    case 47:		// programming count
			displayPrepare();
			LCD_DisplayIntWithPos(Common_Data.pgm_count, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

// Anomaly / circuit check in meter display
/*
		 	RTC failure and EEPROM status
        Digit 1- if �1� then it means that at present a voltage related tamper is going on
        Digit 2- if �1� then it means that at present a current related tamper is going on
        Digit 3- if �1� then it means that at present any other tamper is going on
        Digit 4- if �1� then it means that at present RTC battery is low
        Digit 5- if �1� then it means that at that EEPROM failure is there
    If none of the above anomaly is there then we will show �00000� or �GGGGG� if all are there then 
			we will show �11111�/�FFFFF� or sir may decide any other type of display which is easier to implement.				
*/

    case 48:		// e2rom check , rtc check
			displayPrepare();
		
			tempLong = maskTable[MISSING_POTENTIAL_R]|maskTable[MISSING_POTENTIAL_Y]|maskTable[MISSING_POTENTIAL_B];
			tempLong |= maskTable[OVER_VOLTAGE]|maskTable[UNDER_VOLTAGE]|maskTable[VOLTAGE_UNBALANCED];
			
			if((mytmprdata.tamperEvent_image&tempLong)==0)
			{// there are no voltage related tampers
				LCD_DisplayDigit(2, 1);
			}
			else
			{// there are voltage related tampers
				LCD_DisplayDigit(2, LCD_CHAR_V);
			}

			tempLong = maskTable[CT_REVERSED_R]|maskTable[CT_REVERSED_Y]|maskTable[CT_REVERSED_B];
			tempLong |= maskTable[CT_OPEN_R]|maskTable[CT_OPEN_Y]|maskTable[CT_OPEN_B];
			tempLong |= maskTable[CURRENT_UNBALANCED]|maskTable[CT_BYPASS]|maskTable[OVER_CURRENT];
			
			if((mytmprdata.tamperEvent_image&tempLong)==0)
			{// there are no current related tampers
				LCD_DisplayDigit(3, 1);
			}
			else
			{// there are current related tampers
				LCD_DisplayDigit(3, LCD_CHAR_C);
			}

			tempLong = maskTable[MAGNET]|maskTable[AC_MAGNET]|maskTable[NEUTRAL_DISTURBANCE]|maskTable[COVER_OPEN];
			
			if((mytmprdata.tamperEvent_image&tempLong)==0)
			{// there are no other tampers
				LCD_DisplayDigit(4, 1);
			}
			else
			{// there are other tampers
				LCD_DisplayDigit(4, LCD_CHAR_O);
			}
// now for rtc battery check - this is being made 1 always - better still remove it
/*
			if(0==POWERMGMT_ISVRTCLOW())
//				LCD_DisplayDigit(5, LCD_CHAR_G);	// battery voltage is Not Low 
				LCD_DisplayDigit(5, 1);	// battery voltage is Not Low 
			else
//				LCD_DisplayDigit(5, LCD_CHAR_F);	// battery voltage is Low
				LCD_DisplayDigit(5, 1);	// battery voltage is Low
*/				
// now for E2rom check				
			tempchar=perform_E2ROM_Test(0);
			if(tempchar==0)
			{// e2rom is good
				LCD_DisplayDigit(5, 1);	// e2rom is good
			}
			else
			{// E2ROM is failed
				LCD_DisplayDigit(5, LCD_CHAR_M);	// e2rom is failed
			}
			
			LCD_DisplayDigit(9, LCD_CHAR_A);
			LCD_DisplayDigit(8, LCD_CHAR_N);
      break;

// Display check
    case 49:		// all on
      LCD_DisplayAll();
			fast_display_flag=0;
//      disp_state=255;
      break;
	}
}

//int8_t testctr;
void display_onDemand(void)
{
//	toddata_t temptoddata;
	int32_t tempTodKwh;
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
		
	if(disp_state>14)
		disp_state=0;	// cannot be increased by more than 14 - last state
	
		
	switch(disp_state)
	{
/*		
    case 0:		// Instant Load in kVA
			testctr++;
			if(testctr>=10)
			{
				testctr=0;
				tempchar=acMagnetSenseCtr;
				acMagnetSenseCtr=0;
			}
  		display_long_one_dp(tempchar);
			fast_display_flag=1;
	    break;
*/		
		
    case 0:		// Average PF for last billing period
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
		
    case 1:		// Instant Load in kVA
			tempLong = getInstantaneousParams(APP_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplaykVASign();
			fast_display_flag=1;
	    break;
		
    case 2:		// Instant Load in kVAR
//			tempLong = getInstantaneousParams(REACT_POWER_LAG,LINE_TOTAL);
			tempLong = getInstantaneousParams(REACT_POWER_SIGNED,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplaykVArSign();
			fast_display_flag=1;
	    break;

    case 3:		// Voltage failure count - R
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 4:		// Voltage failure count - Y
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 5:		// Voltage failure count - B
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 6:		// current failure count - R
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 7:		// current failure count - Y
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 8:		// current failure count - B
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 9:		// Voltage unbalance count
			LCD_DisplayIntWithPos(mytmprdata.voltageUnbalanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 10:		// Current Unbalance count
			LCD_DisplayIntWithPos(mytmprdata.currentUnbalanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 11:		// High Voltage count
			LCD_DisplayIntWithPos(mytmprdata.highVoltageCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_H);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 12:		// Low Voltage count
			LCD_DisplayIntWithPos(mytmprdata.lowVoltageCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_L);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 13:		// Neutral Disturbance count
			LCD_DisplayIntWithPos(mytmprdata.neutralDisturbanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_D);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 14:		// Magnet Tamper count
			LCD_DisplayIntWithPos(mytmprdata.magnetTamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_M);
			LCD_DisplayDigit(8, LCD_CHAR_T);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
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
  		LCD_DisplaySpSign(S_IMPORT);
			fast_display_flag=1;
      break;
			
    case 1:			// high resolution kwh - export
//			temp_high_res_energy = ((myenergydata.kwh_fundamental%100000)*100)+(int32_t)high_resolution_kwh_fundamental_value;
			temp_high_res_energy = ((myenergydata.kwh_export%100000)*100)+(int32_t)high_resolution_kwh_value_export;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_EXPORT);
			fast_display_flag=1;
      break;
			
    case 2:			// high resolution kvarh lag
			temp_high_res_energy = ((myenergydata.kvarh_lag%100000)*100)+(int32_t)high_resolution_kvarh_lag_value;
			display_long_five_dp(temp_high_res_energy);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_IMPORT);
  		LCD_DisplaySpSign(S_LAG);
			fast_display_flag=1;
      break;
			
    case 3:			// high resolution kvah
			temp_high_res_energy = ((myenergydata.kvah%100000)*100)+(int32_t)high_resolution_kvah_value;
			display_long_five_dp(temp_high_res_energy);
  		LCD_DisplaySpSign(S_IMPORT);
			LCD_DisplaykVAhSign();
			fast_display_flag=1;
      break;
	}
}

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
// If voltage is not available � then icon should disappear.
	
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
// Here we are in Bidir Mode
	if(calswitch!=0)
	{
		if((g_TamperStatus&REVERSE_MASK)!=0)
			LCD_DisplaySpSign(S_LEFT_ARROW);
		else
			LCD_ClearSpSign(S_LEFT_ARROW);
	}
		
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
		if(calswitch!=0)
			LCD_ClearSpSign(S_LEFT_ARROW);	// production code only
	}
	
// this is the last thing to be done
// if there is missing potential or neutral disturbanc or mixed powers then Bypass, Current Unbalance, CT open icons to be
// switched off
	if(((fault_code&_BV(0))!=0)||((fault_code&_BV(1))!=0)||((fault_code&_BV(2))!=0)||((fault_code&_BV(9))!=0)||(mixedPowers==1))
	{
		LCD_ClearSpSign(S_T9);			// Bypass
		LCD_ClearSpSign(S_C_UNBAL);	// Current Unbalance
    LCD_ClearSpSign(S_NM);	// CT Open
		if(calswitch!=0)
			LCD_ClearSpSign(S_LEFT_ARROW);	// production code only
	}
}

#else
//----------------------------------------------------------------------------------------------------------------------------------------
// this is for UNIDIR meter
void displayRepeatParameters(void)
{
//	uint16_t tempint;
	int32_t templong;
  powermgmt_mode_t power_mode;
	uint8_t tempchar;
	uint8_t tmonth;
	
  LCD_ClearAll();
	disp_autoscroll_interval=10;

	if(someKeyPressed==0)
	{	
		if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(autoscrollInButtonMode==0))
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
	else
		someKeyPressed=0;
		
	if((comm_mode!=0)&&(autoscrollInButtonMode==0))
	{
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}
	
	
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE1)
		EM_LCD_DisplayStatus();

	if(sub_disp_state>2)
		sub_disp_state=0;	

	switch(sub_disp_state)
	{
    case 0:		// total kwh - Cumulative Forwarded
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kwh/1000, LCD_LAST_POS_DIGIT,0);
			display_long_one_dp(myenergydata.kwh/100);
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     	break;

    case 1:		// total kvarh - lag
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lag/1000, LCD_LAST_POS_DIGIT,0);
			display_long_one_dp(myenergydata.kvarh_lag/100);
			LCD_DisplaykVArhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
  		LCD_DisplaySpSign(S_LAG);
			fast_display_flag=0;
     	break;
			
    case 2:		// L1 - MD      tempchar = 1;
      tmonth = decrementMonth(1);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+96,(uint8_t*)&templong,4);
      display_long_three_dp(templong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
			fast_display_flag=0;
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
	
	if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(autoscrollInButtonMode==0))
	{
		LCD_DisplayDigit(2, LCD_CHAR_C);
		LCD_DisplayDigit(3, LCD_MINUS_SIGN);
		LCD_DisplayDigit(4, 0);
		LCD_DisplayDigit(5, LCD_CHAR_P);
		LCD_DisplayDigit(6, LCD_CHAR_E);
		LCD_DisplayDigit(7, LCD_CHAR_N);
		return;
	}
	
	if((comm_mode!=0)&&(autoscrollInButtonMode==0))
	{
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}
	
	if(disp_state>0)
		EM_LCD_DisplayStatus();

	if(disp_state>19)
		disp_state=0;

	switch(disp_state)
	{
    case 0:			// all on
      LCD_DisplayAll();
			fast_display_flag=0;
      break;
		
    case 1:		// meter number
		  lcd_display_multiple_chars(0,6,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(Common_Data.meterNumber, LCD_LAST_POS_DIGIT,0);
			lcd_display_id();
			fast_display_flag=0;
     	break;
	
    case 2:		// Voltage - R
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 3:		// Voltage - Y
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 4:		// Voltage - B
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
	    break;

    case 5:		// Current - R with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			lcd_display_A();
			fast_display_flag=1;
      break;

    case 6:		// Current - Y with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
			lcd_display_A();
			fast_display_flag=1;
      break;

   	case 7:		// Current - B with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
			lcd_display_A();
			fast_display_flag=1;
      break;
			
		case 8:		// Line Frequency
			tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
			display_long_two_dp(tempLong);
  		LCD_DisplayDigit(1, LCD_CHAR_F);
	    LCD_DisplayDigit(9, LCD_CHAR_H);
	    LCD_DisplayDigit(8, 2);
			fast_display_flag=1;
     break;

    case 9:		// pf total with sign
    	pf_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,LINE_TOTAL);
			tempLong = getInstantaneousParams(IPFS,LINE_TOTAL)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_P);	
			LCD_DisplayDigit(2, LCD_CHAR_F);	

			LCD_DisplayDigit(9, LCD_CHAR_T);

			if(tempLong==0)
			{// if pf is 0, then do not show LEAD or LAG
	    	LCD_ClearSpSign(S_LEAD);
	    	LCD_ClearSpSign(S_LAG);
			}
			else
			{
				if (pf_sign == PF_SIGN_LEAD_C)
				{
		    	LCD_DisplaySpSign(S_LEAD);
				}
				else if (pf_sign == PF_SIGN_LAG_L)
				{
		    	LCD_DisplaySpSign(S_LAG);
				}
				else
				{
		    	LCD_ClearSpSign(S_LEAD);
		    	LCD_ClearSpSign(S_LAG);
				}
			}
			fast_display_flag=1;
		 break;

    case 10:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
	    break;

    case 11:		// date
			EM_DisplayDate();
			fast_display_flag=0;
      break;

    case 12:		// time
			EM_DisplayInstantTime();
			fast_display_flag=1;
      break;

    case 13:		// Rising MD in kW
	    tempchar=60/(int32_t)Common_Data.mdinterval;
			tempLong = ((myenergydata.kwh-ctldata.kwh_last_saved)*(int32_t)tempchar);
			
			if(tempLong<0)
				tempLong=0;
			display_long_three_dp(tempLong);	// rising MD
			LCD_DisplaykWSign();
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

    case 14:		// Current MD in kW
			display_long_three_dp(mymddata.mdkw);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
			
    case 15:		// Cumulative MD in kW
			display_long_three_dp(ctldata.cumulative_md);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 16:		// billing count
			LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 17:			// TOD1 - kwh - Night Hours (Time Zone 0 and 5), scriptid 1, (scriptid-1)=0 - new script id is 2, (scriptid-1)=1
			tempchar=1;	// this is now (scriptid-1)
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_T);
			fast_display_flag=1;
      break;

    case 18:		// TOD2 - kwh - Off Peak Hours	(Time Zone 3), scriptid 4, (scriptid-1)=3 - no change old and new script ids are same
			tempchar=3;	//this is now (scriptid-1)
//  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempLong, sizeof(tempLong));
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, 0);
			LCD_DisplayDigit(8, LCD_CHAR_P);
			fast_display_flag=1;
     break;
	
    case 19:		// TOD3	- kwh	-	Peak hours (Time Zone 2 and 4), scriptid 3, (scriptid-1)=2 - new script id is 1, (scriptid-1)=0
			tempchar=0;	// this is now (scriptid-1)
//  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
//			display_long_three_dp(tempTodKwh);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
//      disp_state=255;
			fast_display_flag=1;
      break;			
	}
}

// this is a private function for the display_onDemand to prevent flickering
// when tod is displayed
void displayPrepare(void);

void displayPrepare(void)
{
  powermgmt_mode_t power_mode;
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	
  LCD_ClearAll();
	if (power_mode == POWERMGMT_MODE1)
		EM_LCD_DisplayStatus();
}

void display_onDemand(void)
{
//	toddata_t temptoddata;
	int32_t tempTodKwh;
//  powermgmt_mode_t power_mode;
	int32_t tempLong;
	uint16_t tempint;
	ext_rtc_t datetime;
	EM_PF_SIGN pf_sign;    
	uint8_t tempchar;
	uint8_t tmonth;

  R_WDT_Restart();

/*	
  LCD_ClearAll();
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE1)
		EM_LCD_DisplayStatus();
*/

	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>26)
		disp_state=0;	// cannot be increased by more than 14 - last state
	
		
	switch(disp_state)
	{
    case 0:		// kwh	-	Peak hours (Time Zone 2 and 4), scriptid 3. (scriptid-1)=2 - new script id is 1, (scriptid-1)=0
			tempchar=0;	// this is now (scriptid-1)
//  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, LCD_CHAR_P);
			LCD_DisplayDigit(8, LCD_CHAR_H);
			fast_display_flag=1;
     break;
	
		
    case 1:			// kwh - Night Hours (Time Zone 0 and 5), scriptid 1, (scriptid-1)=0 - new script id is 2, (scriptid-1)=1
			tempchar=1;// this is now (scriptid-1)
//  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			displayPrepare();
			lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_T);
			fast_display_flag=1;
      break;

    case 2:		// kwh - rest Hours	(Time Zone 1) , scriptid 2, (scriptid-1)=1 - new script id is 3, (scriptid-1)=2
			tempchar=2;	// this is now (scriptide-1)
//  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, LCD_CHAR_H);
			fast_display_flag=1;
     break;
			
    case 3:		// kwh - Off Peak Hours	(Time Zone 3), scriptid 4, (scriptid-1)=3 - no change in this script id
			tempchar=3;	// this is now (scriptid-1)
//  		read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			if(tempchar==presentZone)
				tempTodKwh=myenergydata.zone_kwh;
			else
				EPR_Read(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
			
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(tempTodKwh/100);
			LCD_DisplaykWhSign();
		  LCD_DisplaySpSign(S_TOD);
			LCD_DisplayDigit(9, 0);
			LCD_DisplayDigit(8, LCD_CHAR_P);
			fast_display_flag=1;
     break;
	
    case 4:		// kVARH lead
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvarh_lead/100);
			LCD_DisplaykVArhSign();
	    LCD_DisplaySpSign(S_LEAD);	// show the ulta
			fast_display_flag=1;
     break;
	
    case 5:		// kVAH
			displayPrepare();
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvah/100);
			LCD_DisplaykVAhSign();
			fast_display_flag=1;
     break;

    case 6:		// Current MD in kVA
			displayPrepare();
			display_long_three_dp(mymddata.mdkva);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
     break;
/*
    case 7:		// Current MD KVA date
			lcd_display_date(mymddata.mdkva_datetime.date,mymddata.mdkva_datetime.month,mymddata.mdkva_datetime.year);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
     break;
		 
    case 8:		// Current MD KVA time
			lcd_display_time(mymddata.mdkva_datetime.hour,mymddata.mdkva_datetime.minute,mymddata.mdkva_datetime.second);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
     break;
*/		 
    case 7:		// L1 - MD KVA
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+240,(uint8_t*)&tempLong,4);
			displayPrepare();
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     break;

    case 8:		// Cumulative MD in kVA
			displayPrepare();
			display_long_three_dp(ctldata.cumulative_md_kva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 9:		// Voltage failure count - R
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 10:		// Voltage failure count - Y
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 11:		// Voltage failure count - B
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 12:		// current failure count - R
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 13:		// current failure count - Y
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 14:		// current failure count - B
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 15:		// Voltage unbalance count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.voltageUnbalanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 16:		// Current Unbalance count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentUnbalanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 17:		// Current Bypass count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentBypassCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_B);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 18:		// Current Reversal count - R
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentReversalCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_R);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 19:		// Current Reversal count- Y
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentReversalCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_R);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 20:		// Current Reversal count - B
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.currentReversalCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_R);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 21:		// Neutral Disturbance count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.neutralDisturbanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_D);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 22:		// Magnet Tamper count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.magnetTamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_M);
			LCD_DisplayDigit(8, LCD_CHAR_T);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 23:		// Total Tamper count
			displayPrepare();
			LCD_DisplayIntWithPos(mytmprdata.tamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_C);	// 06/Sep/2020
//			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 24:		// Meter Version
			displayPrepare();
			display_long_one_dp(470);
			LCD_DisplayDigit(1, LCD_CHAR_V);
			LCD_DisplayDigit(2, LCD_CHAR_E);
			LCD_DisplayDigit(3, LCD_CHAR_R);
			fast_display_flag=0;
      break;
		 
    case 25:		// Total power on time in hours
			displayPrepare();
		//	tempLong = getMonthlyPowerOnMinutes(); // computes the difference between current and last month power on minutes  
			tempLong = myenergydata.powerOnminutes_eb;
			LCD_DisplayIntWithPos((tempLong/60), LCD_LAST_POS_DIGIT,0);// since 10/02/2024
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_N);
			
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_M);
			fast_display_flag=0;
      break;
		 
    case 26:		// Total power oFF time in hours
			tempLong = computePowerOffDurn(rtc);	// use the rtc for current value and returns power fail duration for the month
			tempLong += Common_Data.cumPowerFailDuration;	// now it is cumulative
			
			tempLong = tempLong/60;	// now hours
		
			displayPrepare();
			LCD_DisplayIntWithPos(tempLong, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_F);
			
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_M);
		
/*		
//----------------------------------		earlier code ----------------------------------
			tempLong = ((int32_t)rtc.date-1)*((int32_t)1440)+((int32_t)rtc.hour)*((int32_t)60)+(int32_t)rtc.minute;
			tempTodKwh = ((Common_Data.cumPowerFailDuration + tempLong) - myenergydata.powerOnminutes_eb)/60;
			if(tempTodKwh<0)
				tempTodKwh = 0;
		
			displayPrepare();
			LCD_DisplayIntWithPos(tempTodKwh, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_F);
			
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_M);
//-----------------------------------------------------------------------------------------		
*/
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
    	LCD_DisplaySpSign(S_LAG);
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
// If voltage is not available � then icon should disappear.
	
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
		LCD_ClearSpSign(S_LEFT_ARROW);
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
		LCD_ClearSpSign(S_LEFT_ARROW);
	}
}
#endif













