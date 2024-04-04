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



const uint8_t msg_nil_tamper[8] = {LCD_CHAR_T, LCD_CHAR_A, LCD_CHAR_M, LCD_CHAR_P, LCD_CHAR_N, LCD_CHAR_I, LCD_CHAR_L, LCD_CLEAR_DIGIT};
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
const uint8_t Bat_OK[8]     = {LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_T, LCD_MINUS_SIGN, LCD_CHAR_G, LCD_CHAR_D, LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t Bat_LO[8]     = {LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_T, LCD_MINUS_SIGN, LCD_CHAR_B, LCD_CHAR_A, LCD_CHAR_D, LCD_CLEAR_DIGIT};



// In the definition of TAMPER_DISPLAY_MASK event code bits 1 to 15 have been accounted for by 0x0000FFFE
// the other bits are separately and individually accounted
#define TAMPER_DISPLAY_MASK	(0x0000FFFE | MAGNET_MASK | AC_MAGNET_MASK | NEUTRAL_DISTURBANCE_MASK | OVER_CURRENT_Y_MASK | OVER_CURRENT_B_MASK)
void displayTamperName(uint8_t eventCode);
void d_Tampers(void);


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

int8_t scrollLock;

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

				disp_timer = 0;
				scrollLock=1;	// lock the display for a few seconds to allow following message to remain on LCD
			// this variable gets cleared in 3 seconds	
				
			  LCD_ClearAll();
				LCD_DisplayDigit(1, LCD_CHAR_C);
				LCD_DisplayDigit(2, 0);
				LCD_DisplayDigit(3, LCD_CHAR_M);
				LCD_DisplayDigit(4, LCD_CHAR_M);
				LCD_DisplayDigit(5, LCD_MINUS_SIGN);
				LCD_DisplayDigit(6, 0);
				LCD_DisplayDigit(7, LCD_CHAR_N);
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
				if(mode_timeout_ctr>299)
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
				if(mode_timeout_ctr>299)	// 5 minutes
				{
					makeDisplayReadyForAutoScroll();
				}
			}
			else
			{// we are in high res mode
				mode_timeout_ctr++;
				if(mode_timeout_ctr>299)	// five minutes
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


void handleDisplay_Gujarat(uint8_t a);
void changeDisplayMode(void);
//void displayRepeatParameters(void);

void changeDisplayMode(void)
{// both keys have been pressed for some time
// 0 - autoscroll, 1 - displayOnDemand, 2 - High resolution display
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
	scrollLock=1;	// lock the display for a few seconds to allow following message to remain on LCD
// this variable gets cleared in 3 seconds	
	
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
		if(bothKeysPressedCtr>25)	// 125 earlier
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

//int32_t testCtr;

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
/*		
		case 0:
			testCtr--;
//			display_long_one_dp(testCtr);
			LCD_DisplayIntWithPos(testCtr,7,0);
			disp_timer=disp_autoscroll_interval;
			break;
*/			
		
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

			tempchar = countCurrentGT145(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
//#else
//			tempchar = countCurrentGT95(i[LINE_PHASE_R],i[LINE_PHASE_Y],i[LINE_PHASE_B]);
//#endif
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

	if((g_display_mode==255)||(seconds_flag==0))	// things are not ready yet
		return;

		
// the following function allows a temporary message to be displayed on the LCD	
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
		
/*		
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
*/

	power_mode = POWERMGMT_GetMode();	// actual function to be used
		
	if (power_mode == POWERMGMT_MODE1)
	{
#ifdef BIDIR_METER		
		if((g_display_mode==0)&&(disp_state==47))
#else
		if(disp_state==0)
#endif
		{
		}
		else
			EM_LCD_DisplayStatus();
	}

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
//						else
//							g_tamper_display_state++;
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
//				else
//					g_tamper_display_state++;
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
							disp_state=36;	// last autoscroll state - 28 earlier , now 36
							break;
							
						case 1:
							disp_state=35;	// last pushbutton state - 34 earlier , now 35
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
		
		if(disp_autoscroll_interval>10)
			disp_autoscroll_interval=10;
		
			
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
/*
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
			LCD_DisplaykWhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     	break;

    case 1:		// total kvarh - lag
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvarh_lag/1000, LCD_LAST_POS_DIGIT,0);
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
*/

uint32_t gTamperEvent;
//uint8_t g_tamper_display_state;
uint8_t g_cover_display_state;
uint8_t cover_disp_timer;

void displayCOPEN(void)
{// this function gets called every second if cover open is recorded
	ext_rtc_t datetime;

  R_WDT_Restart();
			
	cover_disp_timer++;
	if(cover_disp_timer>=5)
	{
		cover_disp_timer=0;
		g_cover_display_state++;	// this variable is made 255 when the tamper is registered
		
		if(g_cover_display_state>2)
			g_cover_display_state=0;
	}
	else
	{
		EM_LCD_DisplayStatus();
		return;
	}
		
// the actual display will be updated every 5 seconds
  LCD_ClearAll();	
	EM_LCD_DisplayStatus();

	switch(g_cover_display_state)
	{
		case 0:
			LCD_DisplayDigit(2, LCD_CHAR_C);
			LCD_DisplayDigit(3, LCD_MINUS_SIGN);
			LCD_DisplayDigit(4, 0);
			LCD_DisplayDigit(5, LCD_CHAR_P);
			LCD_DisplayDigit(6, LCD_CHAR_E);
			LCD_DisplayDigit(7, LCD_CHAR_N);
//			fast_display_flag=0;
			break;
			
		case 1:	// show the date
			EPR_Read(EVENTS5_BASE_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, 0);
//			fast_display_flag=0;
			break;
			
			
		case 2:	// show the time
			EPR_Read(EVENTS5_BASE_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, 0);
//			fast_display_flag=0;
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
//  LCD_ClearAll();	// shifted below
	
	if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(autoscrollInButtonMode==0))
	{
		displayCOPEN();		
		return;
	}
	
  LCD_ClearAll();	// earlier when above was causing COPEN message to be displayed only once and then blank screen for 4 seconds
	if((comm_mode!=0)&&(autoscrollInButtonMode==0))
	{
		LCD_DisplayDigit(2, LCD_CHAR_D);
		LCD_DisplayDigit(3, LCD_CHAR_N);
		LCD_DisplayDigit(4, LCD_CHAR_L);
		LCD_DisplayDigit(5, LCD_CHAR_D);
		return;
	}
	
	if(disp_state>36)
		disp_state=0;

	if(disp_state>0)
		EM_LCD_DisplayStatus();
		
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
//			display_long_one_dp(myenergydata.kvah/100);
//			display_long_three_dp(myenergydata.kvah);
			LCD_DisplaykVAhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     break;
	 
    case 6:		// Current MD in kW
			display_long_three_dp(mymddata.mdkw);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
		 
    case 7:		// L1 - MD      tempchar = 1;
      tmonth = decrementMonth(1);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+96,(uint8_t*)&tempLong,4);
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
			fast_display_flag=0;
      break;
		 
    case 8:		// Cumulative MD in kW
			display_long_three_dp(ctldata.cumulative_md);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 9:		// Current MD in kVA
			display_long_three_dp(mymddata.mdkva);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
		 
    case 10:		// L1 - MD      tempchar = 1;
      tmonth = decrementMonth(1);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+240,(uint8_t*)&tempLong,4);
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_BILL);
			fast_display_flag=0;
      break;
		 
    case 11:		// Cumulative MD in kVA
			display_long_three_dp(ctldata.cumulative_md_kva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 12:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
	    break;

    case 13:		// billing count
			LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

			
    case 14:		// Total Tamper count
			LCD_DisplayIntWithPos(mytmprdata.tamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

/*			
    case 14:		// Voltage failure count - R
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 15:		// Voltage failure count - Y
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 16:		// Voltage failure count - B
			LCD_DisplayIntWithPos(mytmprdata.voltageFailureCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 17:		// current failure count - R
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 18:		// current failure count - Y
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 19:		// current failure count - B
			LCD_DisplayIntWithPos(mytmprdata.currentFailureCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_F);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 20:		// Voltage unbalance count
			LCD_DisplayIntWithPos(mytmprdata.voltageUnbalanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_V);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 21:		// Current Unbalance count
			LCD_DisplayIntWithPos(mytmprdata.currentUnbalanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_U);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 22:		// Current Bypass count
			LCD_DisplayIntWithPos(mytmprdata.currentBypassCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_B);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 23:		// Current Reversal count - R
			LCD_DisplayIntWithPos(mytmprdata.currentReversalCount_R, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_R);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 24:		// Current Reversal count- Y
			LCD_DisplayIntWithPos(mytmprdata.currentReversalCount_Y, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_Y);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_R);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 25:		// Current Reversal count - B
			LCD_DisplayIntWithPos(mytmprdata.currentReversalCount_B, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, LCD_CHAR_B);
			LCD_DisplayDigit(9, LCD_CHAR_C);
			LCD_DisplayDigit(8, LCD_CHAR_R);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 26:		// Neutral Disturbance count
			LCD_DisplayIntWithPos(mytmprdata.neutralDisturbanceCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_N);
			LCD_DisplayDigit(8, LCD_CHAR_D);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 27:		// Magnet Tamper count
			LCD_DisplayIntWithPos(mytmprdata.magnetTamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_M);
			LCD_DisplayDigit(8, LCD_CHAR_T);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
*/

    case 15:		// Existing tamper if any - last item
			d_Tampers();
			fast_display_flag=0;
      break;

   	case 16:		// average pf for current month
//			tempLong = computeAveragePF(total_energy_lastSaved, apparent_energy_lastSaved, IMPORT);
			tempLong = computeAveragePF(myenergydata.kwh, myenergydata.kvah, IMPORT);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(1, LCD_CHAR_A);
			LCD_DisplayDigit(2, LCD_CHAR_P);
			LCD_DisplayDigit(3, LCD_CHAR_F);
			fast_display_flag=1;
      break;
			
// new states added			
    case 17:		// Voltage - R
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 18:		// Voltage - Y
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 19:		// Voltage - B
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
	    break;

    case 20:		// Current - R with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			lcd_display_A();
			fast_display_flag=1;
      break;

    case 21:		// Current - Y with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
			lcd_display_A();
			fast_display_flag=1;
      break;

   	case 22:		// Current - B with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
			lcd_display_A();
			fast_display_flag=1;
      break;
			
    case 23:		// e2rom check , rtc check - Anomaly Display		
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

			if(0==Common_Data.rtc_failed)
//				LCD_DisplayDigit(5, LCD_CHAR_G);	// battery voltage is Not Low 
				LCD_DisplayDigit(5, 1);	// battery voltage is Not Low 
			else
				LCD_DisplayDigit(5, LCD_CHAR_B);	// battery voltage is Low
//				LCD_DisplayDigit(5, 1);	// battery voltage is Low
				
// now for E2rom check				
			tempchar=perform_E2ROM_Test(0);
			if(tempchar==0)
			{// e2rom is good
				LCD_DisplayDigit(6, 1);	// e2rom is good
			}
			else
			{// E2ROM is failed
				LCD_DisplayDigit(6, LCD_CHAR_M);	// e2rom is failed
			}
			
			LCD_DisplayDigit(9, LCD_CHAR_A);
			LCD_DisplayDigit(8, LCD_CHAR_N);
			fast_display_flag=0;
      break;


			
			
/*	

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

*/			
	}
}

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


	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>35)
		disp_state=0;
	
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if ((power_mode == POWERMGMT_MODE1)&&(disp_state!=0))
		EM_LCD_DisplayStatus();
		
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

    case 5:			// high resolution kwh
			tempLong = ((myenergydata.kwh%100000)*100)+(int32_t)high_resolution_kwh_value;
			display_long_five_dp(tempLong);
			LCD_DisplaykWhSign();
			fast_display_flag=1;
      break;
		 
    case 6:		// kVAH
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			LCD_DisplayIntWithPos(myenergydata.kvah/1000, LCD_LAST_POS_DIGIT,0);
//			display_long_one_dp(myenergydata.kvah/100);
//			display_long_three_dp(myenergydata.kvah);
			LCD_DisplaykVAhSign();
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
     break;

    case 7:		// billing count
			LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 8:		// Total power on time in hours
			tempLong = getMonthlyPowerOnMinutes(); // computes the difference between current and last month power on minutes 
			LCD_DisplayIntWithPos((tempLong/60), LCD_LAST_POS_DIGIT,0);// since 10/02/2024
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_N);
			
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_M);
			fast_display_flag=0;
      break;
			
    case 9:		// Current MD in kW
			display_long_three_dp(mymddata.mdkw);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
     break;
		 
    case 10:		// L1 - MD KW      tempchar = 1;
      tmonth = decrementMonth(1);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+96,(uint8_t*)&tempLong,4);
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_BILL);
			fast_display_flag=0;
      break;
		 
    case 11:		// Cumulative MD in kW
			display_long_three_dp(ctldata.cumulative_md);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 12:		// Current MD in kVA
			display_long_three_dp(mymddata.mdkva);
  		LCD_DisplayDigit(1, LCD_CHAR_R);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
			fast_display_flag=0;
      break;
		 
    case 13:		// L1 - MD      tempchar = 1;
      tmonth = decrementMonth(1);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+240,(uint8_t*)&tempLong,4);
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_BILL);
			fast_display_flag=0;
      break;
		 
    case 14:		// Cumulative MD in kVA
			display_long_three_dp(ctldata.cumulative_md_kva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
		  LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		
    case 15:		// billing count
			LCD_DisplayIntWithPos(Common_Data.bpcount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_P);
		  LCD_DisplaySpSign(S_D1);	// this will convert the P to R
			LCD_DisplayDigit(8, LCD_CHAR_C);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

    case 16:		// Voltage - R
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_R)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_R)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 17:		// Voltage - Y
			tempLong = getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_Y)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
      break;

    case 18:		// Voltage - B
			tempLong =getInstantaneousParams(IVOLTS,LINE_PHASE_B)*100;
//			tempLong = EM_GetPowerFactor(LINE_PHASE_B)*100;
			display_long_two_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
	    LCD_DisplaySpSign(S_T2);	// V
			fast_display_flag=1;
	    break;

    case 19:		// Current - R with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			lcd_display_A();
			fast_display_flag=1;
      break;

    case 20:		// Current - Y with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_Y);
			lcd_display_A();
			fast_display_flag=1;
      break;

   	case 21:		// Current - B with direction
			tempLong = getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*1000;
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_B);
			lcd_display_A();
			fast_display_flag=1;
      break;
			
		case 22:		// Line Frequency
			tempLong = getInstantaneousParams(IFREQS,LINE_TOTAL)*100;
			display_long_two_dp(tempLong);
  		LCD_DisplayDigit(1, LCD_CHAR_F);
	    LCD_DisplayDigit(9, LCD_CHAR_H);
	    LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
     break;

    case 23:		// pf total with sign
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
			fast_display_flag=0;
		 break;

    case 24:		// Instant Load in kW
			tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykWSign();
			fast_display_flag=1;
	    break;

    case 25:		// Instant Load in kVA
			tempLong = getInstantaneousParams(APP_POWER,LINE_TOTAL);
			display_long_three_dp(tempLong);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplaykVASign();
			fast_display_flag=1;
	    break;

    case 26:		// total power on for L1
      tmonth = decrementMonth(1);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+384,(uint8_t*)&tempLong,4);
      LCD_DisplayIntWithPos(tempLong/60, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_N);
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, LCD_CHAR_L);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
			
    case 27:		// total power on for L2
      tmonth = decrementMonth(2);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+384,(uint8_t*)&tempLong,4);
      LCD_DisplayIntWithPos(tempLong/60, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_N);
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, LCD_CHAR_L);
			LCD_DisplayDigit(8, 2);
			fast_display_flag=0;
      break;
			
		case 28:
			d_Tampers();
			fast_display_flag=0;
      break;
			
		case 29:	// Last occured Tamper
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS+8,(uint8_t *)&tempchar,sizeof(tempchar));
			displayTamperName(tempchar);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
		
		case 30:	// Last occured Tamper date
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
		
		case 31:	// Last occured Tamper time
			EPR_Read(LAST_OCCURED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_O);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
		
		case 32:	// Last restored Tamper
			EPR_Read(LAST_RESTORED_DETAILS_ADDRESS+8,(uint8_t *)&tempchar,sizeof(tempchar));
			displayTamperName(tempchar);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
		
		case 33:	// Last restored Tamper date
			EPR_Read(LAST_RESTORED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
		
		case 34:	// Last restored Tamper time
			EPR_Read(LAST_RESTORED_DETAILS_ADDRESS,(uint8_t *)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplayDigit(9, LCD_CHAR_R);
			LCD_DisplayDigit(8, 1);
			fast_display_flag=0;
      break;
		
    case 35:		// Total Tamper count
			LCD_DisplayIntWithPos(mytmprdata.tamperCount, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_C);	// 06/Sep/2020
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;

		
/*			
    case 4:		// kVARH lead
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvarh_lead/100);
//			display_long_three_dp(myenergydata.kvarh_lead);
			LCD_DisplaykVArhSign();
	    LCD_DisplaySpSign(S_LEAD);	// show the ulta
			fast_display_flag=0;
     break;
	
    case 5:		// kVAH
		  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
			display_long_one_dp(myenergydata.kvah/100);
//			display_long_three_dp(myenergydata.kvah);
			LCD_DisplaykVAhSign();
			fast_display_flag=0;
     break;
		 
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
		 
    case 7:		// L1 - MD KVA
      tempchar = 1;
      tmonth = decrementMonth(tempchar);
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+240,(uint8_t*)&tempLong,4);
      display_long_three_dp(tempLong);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_BILL);
			LCD_DisplayDigit(9, 1);
			fast_display_flag=0;
     break;

    case 8:		// Cumulative MD in kVA
			display_long_three_dp(ctldata.cumulative_md_kva);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_MD);
  		LCD_DisplaySpSign(S_T8);	// CUM Sign
			fast_display_flag=0;
      break;
		 
    case 24:		// Meter Version
			display_long_two_dp(101);
			LCD_DisplayDigit(1, LCD_CHAR_V);
			LCD_DisplayDigit(2, LCD_CHAR_E);
			LCD_DisplayDigit(3, LCD_CHAR_R);
			fast_display_flag=0;
      break;
		 
    case 26:		// Total power oFF time in hours
			tempLong = ((int32_t)rtc.date-1)*((int32_t)1440)+((int32_t)rtc.hour)*((int32_t)60)+(int32_t)rtc.minute;
			tempTodKwh = ((Common_Data.cumPowerFailDuration + tempLong) - myenergydata.powerOnminutes_eb)/60;
			if(tempTodKwh<0)
				tempTodKwh = 0;
		
			LCD_DisplayIntWithPos(tempTodKwh, LCD_LAST_POS_DIGIT,0);
			LCD_DisplayDigit(1, 0);
			LCD_DisplayDigit(2, LCD_CHAR_F);
			
			LCD_DisplayDigit(9, LCD_CHAR_T);
			LCD_DisplayDigit(8, LCD_CHAR_M);
			fast_display_flag=0;
      break;
*/			
			
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

void d_Tampers(void)
{
	uint32_t whichevent;
	uint8_t event_code_bit;
	uint8_t whichGroup;

	if(holdState==0)
	{
		gTamperEvent = mytmprdata.tamperEvent_image&TAMPER_DISPLAY_MASK; // make all those bits not to be displayed as 0
		holdState = 1;
	}
	
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
// we are only showing the names of the occured tampers - date and time cases can be referred from TNEB code			
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


















