// em_display.c
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
#include "em_display_TN3P_Helper.h"

// Display tamper condition
#include "format.h"
#include "eeprom.h"
#include "em_core.h"


void display_autoScroll(void);
void display_onDemand(void);
void display_Third(void);
void EM_LCD_DisplayStatus(void);

void EM_LCD_DisplayPOR(void);

typedef void (* __far fp_display_t)(void);


// utility icon functions
int8_t dspArr[128];
uint8_t g_dsp_progFlag;


display_int_flag_t g_display_flag = {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0};    /* Flags to display functions */


const int8_t factoryDispPgmBidir[128] = 
{
	2, 3, 4, 5, 6, 7, 8, 13, 14, 15, 19, 16, 20, 25, 26, 27, 28, 29, 30, 35, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 2, 3,42,25,26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 41, 36, 37, 38,
 39,40,23,24,15,19,16, 20, 17, 18, 21, 22, 13, 14,  4, 43, 44, 45, 46, 47,
  5,48,49,50,51,52, 7,  8,  9, 11, 10, 12, 53, 54, 55, 58, 59, 60, 61, 62,
 63,64,65,66,67,68,69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
 83,84,85,86,87,88,56, 57, -1, -1,
 89,90,91,92,93,94,95,96,-1,-1
};

const int8_t factoryDispPgmUnidir[128] = 
{
	2, 3, 4, 7, 13, 15, 16, 25, 26, 27, 28, 29, 30, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 2, 3,42,25,26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 41, 36, 37, 38,
 39,40,23,15,16,17,18, 13, 4, 43, 44, 45, 46, 47, 7, 9, 10, 53, 55, 58,
 59,60,61,62,63,64,65, 72, 74, 76, 78, 80, 82, 84, 85, 86, 87, 88, 56, 57, 
 -1,-1,-1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 -1,-1,-1,-1,-1,-1,-1, -1, -1, -1,
 89,90,91,92,93,94,95,96,-1,-1
};

// we can use the above array to load the factory default display through a command

const fp_display_t fp_display[] = 
{
	d_LCDcheck,
	d_MeterSlNo,
	d_CurrentDate,
	d_CurrentTime,
	d_CumKWH_Import,
	d_CumKWH_Export,
	d_netCumKWH,
	d_CumKVAH_Import,
	d_CumKVAH_Export,
	d_CumKVARH_Lag_Import,
	d_CumKVARH_Lead_Import,
	d_CumKVARH_Lag_Export,
	d_CumKVARH_Lead_Export,
	d_AVGPF_Current_Import,
	d_AVGPF_Current_Export,
	d_MD_Current_Import,
	d_MD_L1_Import,
	d_MD_BillDate_Import,
	d_MD_BillTime_Import,
	d_MD_Current_Export,
	d_MD_L1_Export,
	d_MD_BillDate_Export,
	d_MD_BillTime_Export,
	d_RisingDemand_Import,
	d_RisingDemand_Export,
	d_Vrms_R,
	d_Vrms_Y,
	d_Vrms_B,
	d_Irms_R,
	d_Irms_Y,
	d_Irms_B,
	d_Irms_Neutral,
	d_SignedPF_R,
	d_SignedPF_Y,
	d_SignedPF_B,
	d_SystemSignedPF,
	d_ActivePower_R,
	d_ActivePower_Y,
	d_ActivePower_B,
	d_ActivePower_Total,
	d_ApparentPower_Total,
	d_Frequency,
	d_PhaseSequence,
	d_CumKWH_TOD1_Import,
	d_CumKWH_TOD2_Import,
	d_CumKWH_TOD3_Import,
	d_CumKWH_TOD4_Import,
	d_CumKWH_TOD5_Import,
	d_CumKWH_TOD1_Export,
	d_CumKWH_TOD2_Export,
	d_CumKWH_TOD3_Export,
	d_CumKWH_TOD4_Export,
	d_CumKWH_TOD5_Export,
	d_CumMD_Import,
	d_CumMD_Export,
	d_resetCount,
	d_LastBillDate,
	d_LastBillTime,
	d_PowerOff_Current,
	d_PowerOff_Cum,
	d_CumKWH_L1_Import,
	d_CumKWH_TOD1_L1_Import,
	d_CumKWH_TOD2_L1_Import,
	d_CumKWH_TOD3_L1_Import,
	d_CumKWH_TOD4_L1_Import,
	d_CumKWH_TOD5_L1_Import,
	d_CumKWH_L1_Export,
	d_CumKWH_TOD1_L1_Export,
	d_CumKWH_TOD2_L1_Export,
	d_CumKWH_TOD3_L1_Export,
	d_CumKWH_TOD4_L1_Export,
	d_CumKWH_TOD5_L1_Export,
	d_CumKVAH_L1_Import,
	d_CumKVAH_L1_Export,
	d_AVGPF_L1_Import,
	d_AVGPF_L1_Export,
	d_CumKWH_L2_Import,
	d_CumKWH_L2_Export,
	d_CumKVAH_L2_Import,
	d_CumKVAH_L2_Export,
	d_AVGPF_L2_Import,
	d_AVGPF_L2_Export,
	d_MD_L2_Import,
	d_MD_L2_Export,
	d_PowerOff_Bill_L2,
	d_Tampers,
	d_Last_Restored_Tamper,
	d_Tamper_Count,
	d_Magnet_Tamper_Indication,
	d_HighRes_KWH_Import,
	d_HighRes_KVAH_Import,
	d_HighRes_KVARH_LAG_Import,
	d_HighRes_KVARH_LEAD_Import,
	d_HighRes_KWH_Export,
	d_HighRes_KVAH_Export,
	d_HighRes_KVARH_LAG_Export,
	d_HighRes_KVARH_LEAD_Export,
  NULL,
};

#define RETURN_TO_AUTOSCROLL_COUNT	7500	// corresponds to 300 seconds or 5 minutes

#ifdef GUJARAT_BOARD
	#ifdef BIDIR_METER
		#define NO_OF_SUB_DISPLAY_STATES 4	// BIDIR_METER has 4
	#else
		#define NO_OF_SUB_DISPLAY_STATES 3	// Regular Gujarat Meter has 3
	#endif
#else
		#define NO_OF_SUB_DISPLAY_STATES 4	// other boards may not be needng this
#endif

uint8_t disp_state;
uint8_t disp_timer;
uint8_t disp_autoscroll_interval=6;


uint8_t			g_display_mode = 0;	// 0 - Autoscroll, 1- OnDemand, 2-ThirdDisplay, 3-Special Autoscroll
uint8_t     g_is_por = 0;

uint8_t gentimer;
uint8_t tamperTimer;
uint8_t commModeTimer;

uint8_t comm_mode;
uint8_t kbscanflag;

uint8_t keyPressedCtr;
uint16_t keyNotPressedCtr;

//uint8_t keyNotPressedCtr;
uint8_t keyPressedCtr1;
uint8_t keyNotPressedCtr1;

uint8_t keyPressedCtr2;
uint8_t keyNotPressedCtr2;

int16_t mode_timeout_ctr;
uint8_t sub_disp_state=255;


//typedef void (* __far fp_display_t)(void);


#define MAX_STEP_ON_AUTOSCROLL_DISPLAY 21
#define MAX_STEP_ON_DEMAND_DISPLAY 10
#define MAX_STEP_ON_HIGH_RESOLUTION_DISPLAY 3

uint8_t fwdKeyPressedCtr;
uint8_t revKeyPressedCtr;
uint8_t bothKeysPressedCtr;
uint8_t noKeyPressedCtr;

uint8_t	fast_display_counter;

//g_display_flag.prevTopSW=1;
//g_display_flag.prevMiddleSW=1;

uint8_t gLcdCheck;

void handleDisplay_Gujarat(uint8_t a);
void changeDisplayMode(void);
//void displayRepeatParameters(void);


void KEY_40msInterruptCallback(void)
{
	kbscanflag=1;
}

void makeDisplayReadyForAutoScroll(void)
{
		mode_timeout_ctr=0;
		g_display_mode=0;	//revert to autoscroll
		sub_disp_state=0;	// for TNEB this variable is not needed
		disp_state=255;
		disp_autoscroll_interval=6;
		g_display_flag.autoscrollInButtonMode=0;
		gLcdCheck=0;
}

void EM_RTC_DisplayInterruptCallback(void)
{//this function is called only in battery mode as well as in non-battery mode
  powermgmt_mode_t power_mode;
	
	disp_timer++;
	gentimer++;
	
	if(g_display_flag.tglbit==0)
		g_display_flag.tglbit=1;
	else
		g_display_flag.tglbit=0;
	
	g_display_flag.seconds_flag=1;
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE3)
	{
		gujaratSecondsCounter++;
		tamperTimer=0;
		if(P13.7==0)	// TOPSW
		{
			gujaratSecondsCounter=0;
			keyPressedCtr++;
			if(keyPressedCtr>5)
			{
				sleepCommMode=1;
				sleepCommModeTimer=0;
// instead of setting the comm_mode flag here which causes the DNLD msg to appear
// we can signal that COMM has been turned on
// the actual comm_mode flag ought to be set only when reception starts				
//				comm_mode=1;	// Setting this flag causes DNLD to appear on lcd - msg disappears after a few seconds
				d_COMM_ON();
				disp_timer = disp_autoscroll_interval-2;	// this will cause COMM-On msg to remain for 2 seconds
				commModeTimer=0;
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
		
		mode_timeout_ctr++;
		if((g_display_mode==0)&&(g_display_flag.autoscrollInButtonMode!=0))
		{
			if(mode_timeout_ctr>=15)	// 15 seconds
			{
				g_display_flag.autoscrollInButtonMode=0;	// simply get out of the mode and continue
//				makeDisplayReadyForAutoScroll();
				gLcdCheck=0;
			}
		}
		else if((g_display_mode==1))
		{
			if(mode_timeout_ctr>=60)	// 1 minute
				makeDisplayReadyForAutoScroll();
		}
		else if(g_display_mode==2)
		{
			if(mode_timeout_ctr>=900)	// 15 minutes
				makeDisplayReadyForAutoScroll();
		}
	}

	if(init_done!=0){
		perSecondFlag=1;
//		per_second_activity();
//		CommunicationTimeoutCheck();
	}
}

void CommunicationTimeoutCheck(void)
{
	if(comm_mode!=0)
	{
		commModeTimer++;
		if(commModeTimer>4)
		{
			commModeTimer=0;
			comm_mode=0;
		}
	}
}

void changeDisplayMode(void)
{// both keys have been pressed for some time
// 0 - autoscroll, 1 - displayOnDemand, 2 - High resolution display
	g_display_mode++;
	if(g_display_mode>=3)
	{
//		g_display_mode=0;
		g_display_mode=1;
	}
	disp_state=255;
	disp_timer = 0;
	g_display_flag.holdState=0;	// whenever display mode changes this flag must be reset 
	
  LCD_ClearAll();
	LCD_DisplayDigit(2, LCD_CHAR_M);
	LCD_DisplayDigit(3, 0);
	LCD_DisplayDigit(4, LCD_CHAR_D);
	LCD_DisplayDigit(5, LCD_CHAR_E);
	LCD_DisplayIntWithPos(g_display_mode+1, LCD_LAST_POS_DIGIT);
	g_display_flag.mode_has_been_changed=1;
//	g_display_flag.someKeyPressed=1;	// dangerous - deliberately commented - may be removed altogether later
	gLcdCheck=0;
}

void handleKeyLogic(void)
{
	if(g_display_flag.mode_has_been_changed!=0)
		return;
	
	if((g_display_flag.fwdKeyPressed==1)&&(g_display_flag.revKeyPressed==1))
	{
		bothKeysPressedCtr++;
		fwdKeyPressedCtr=0;
		revKeyPressedCtr=0;

		if(g_display_mode==0)
		{
			if(bothKeysPressedCtr>5)
			{
/*			
	it is important to make the mode_timeout_ctr 0 before calling changeDisplayMode
	since a timer imterrupt may come if timeout_ctr which is incremented in the RTC_interrupt
	routine crosses a value then instead of mode_2 it may go to mode_0

*/
				mode_timeout_ctr=0;	
				changeDisplayMode();
				bothKeysPressedCtr=6;
				g_display_flag.fwdKeyPressed=0;
				g_display_flag.revKeyPressed=0;
			}
		}
		else if(g_display_mode==1)
		{
			if(bothKeysPressedCtr>125)
			{
				mode_timeout_ctr=0;
				changeDisplayMode();
				bothKeysPressedCtr=126;
				g_display_flag.fwdKeyPressed=0;
				g_display_flag.revKeyPressed=0;
			}
		}
		else if(g_display_mode==2)
		{
			if(bothKeysPressedCtr>125)
			{
				mode_timeout_ctr=0;
				changeDisplayMode();
//				g_display_flag.someKeyPressed=1;
				bothKeysPressedCtr=126;
				g_display_flag.fwdKeyPressed=0;
				g_display_flag.revKeyPressed=0;
			}
		}
//*/		
	}// end of bothkeys pressed
	else
	{// here both switches may not be pressed or either switch may be pressed
		bothKeysPressedCtr=0; // this counter should definitely be made 0
		if((g_display_flag.fwdKeyPressed==1)&&(g_display_flag.revKeyPressed==0))
		{
			fwdKeyPressedCtr++;
//			if(fwdKeyPressedCtr>15)
			if(fwdKeyPressedCtr>5)
			{
/*				
				if((g_display_mode==0)&&(g_display_flag.autoscrollInButtonMode==0))
				{
					disp_state=255;
					sub_disp_state=255;
				}
*/
				g_display_flag.someKeyPressed=1;
			}
		}
		else
		{
			if((g_display_flag.revKeyPressed==1)&&(g_display_flag.fwdKeyPressed==0))
			{
				revKeyPressedCtr++;
//				if(revKeyPressedCtr>15)
				if(revKeyPressedCtr>5)
					g_display_flag.someKeyPressed=1;
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
// the kbscanflag is set in the KEY_40msInterruptCallback routine located in this file itself
//	uint8_t currentSW;

	if(kbscanflag==0)
		return;
	kbscanflag=0;
	g_display_flag.currentSW=TOPSW;
	
	if(g_display_flag.currentSW==0)	// Top switch is active low
	{// key is pressed
		if(g_display_flag.prevTopSW==0)
		{
			g_display_flag.fwdKeyPressed=1;
		}
	}
	else
	{// here key is not pressed
		if(g_display_flag.prevTopSW!=0)
		{
			g_display_flag.fwdKeyPressed=0;
		}
	}

	g_display_flag.prevTopSW=g_display_flag.currentSW;
//-----------------------------	
	g_display_flag.currentSW=MIDDLESW;
	
	if(g_display_flag.currentSW==0)	// switch is active low
	{
		if(g_display_flag.prevMiddleSW==0)
		{
			g_display_flag.revKeyPressed=1;
		}
	}
	else
	{// here key is not pressed
		if(g_display_flag.prevMiddleSW!=0)
		{
			g_display_flag.revKeyPressed=0;
		}
	}
	g_display_flag.prevMiddleSW=g_display_flag.currentSW;
	

// handle key logic

	handleKeyLogic(); // this function sets the someKeyPressed after checking debounce
	
//	if(g_display_flag.someKeyPressed==1)
//		g_display_flag.seconds_flag=1;	// this is to ensure entry into the handleDisplay function
	handleDisplay_Gujarat(0);
	
	if(Common_Data.manualResetPermitted!=0)
	{
		if(MDRST_SW==0)	// switch is active low
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
		      generate_bill(rtc.date,rtc.month,rtc.year,rtc.hour,rtc.minute,rtc.second, BT_MANU);
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
}


//avoidShowingRepeatParameters
void handleDisplay_Gujarat(uint8_t t)
{// disp_timer is incremented in the EM_RTC_DisplayInterruptCallback function located in this file itself
//	uint8_t previous_disp_state;	// this variable is doing nothing in TNEB
  powermgmt_mode_t power_mode;

// seconds_flag is set every second or whenever a key has been pressed
	if((g_display_mode==255)||(g_display_flag.seconds_flag==0))	// things are not ready yet
		return;

	if(g_display_flag.mode_has_been_changed==1)
	{
		if(disp_timer<3)
			return;
		else
		{
			g_display_flag.mode_has_been_changed=0;
			g_display_flag.fwdKeyPressed=0;
			g_display_flag.revKeyPressed=0;
			g_display_flag.someKeyPressed=0;
			bothKeysPressedCtr=0;	// this is too prevent false triggering of the display mode
			disp_timer=0;
			mode_timeout_ctr=0;
			g_display_flag.fast_display_flag=0;
			g_display_flag.holdState=0;
			disp_state=0;
			disp_timer=disp_autoscroll_interval;	// this is to ensure entry below
		}
	}
		
	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if((power_mode == POWERMGMT_MODE1)&&(gLcdCheck==0))
		EM_LCD_DisplayStatus();

  if((disp_timer >= disp_autoscroll_interval)||(g_display_flag.someKeyPressed==1)||(g_display_flag.fast_display_flag==1))
	{// main entry point
		g_display_flag.seconds_flag=0;
//--------------------handle the two cases of somekeypressed or not--------------
		if(g_display_flag.someKeyPressed==0)
		{// here no key has been pressed - we come here only if fast display flag is 1 or autoscroll interval has been reached
		// we are executing the following code only for mode 0 - all other modes are handled below
			if(g_display_mode==0)
			{
				if(g_display_flag.autoscrollInButtonMode == 0)
				{
					if(g_display_flag.fast_display_flag==0)
					{// autoscroll interval has been completed
						if(g_display_flag.holdState==0)
							disp_state++;
						else
							g_tamper_display_state++;
					}
					else
					{// here fast_display_flag = 1 - wait for autoscroll interval before incrementing state
						fast_display_counter++;
						if(fast_display_counter>=disp_autoscroll_interval)
						{
							g_display_flag.fast_display_flag=0;
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
			g_display_flag.someKeyPressed=0;
			mode_timeout_ctr=0;
			g_display_flag.fast_display_flag=0;
//			g_display_flag.holdState=0;
			
			if(g_display_flag.fwdKeyPressed==1)
			{
				g_display_flag.fwdKeyPressed=0;
				fwdKeyPressedCtr=0;
				if(g_display_mode==0)
				{
					if(g_display_flag.autoscrollInButtonMode==0)
					{
						g_display_flag.autoscrollInButtonMode=1;
						g_display_flag.holdState=0;	// this is to prevent the C-Open etc display from holding the state
					}
				}
				
				if(g_display_flag.holdState==0)
					disp_state++;
				else
					g_tamper_display_state++;
			}
			
			if(g_display_flag.revKeyPressed==1)
			{
				g_display_flag.revKeyPressed=0;
				revKeyPressedCtr=0;
				if(disp_state>=1)
				{
					disp_state--;
					g_display_flag.holdState=0;	// if reverse key is pressed then clear this thing
				}
			}
		}
//--------------------handling of the two cases of somekeypressed or not completed--------------
		
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


//#define HARMONICS_TEST
/*
void displayRepeatParameters(void)
{
}
*/

//uint8_t greaterZone; // this variable stores the zone number which contains the highest value of md and is used to extract datetime subsequently 

void display_autoScroll(void)
{
	int8_t tempIndex;
  R_WDT_Restart();
	
	if(((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) != 0)&&(g_display_flag.autoscrollInButtonMode==0))
	{
		d_COPEN();
		return;
	}
	
	if((comm_mode!=0)&&(g_display_flag.autoscrollInButtonMode==0))
	{
		d_DNLD();
		return;
	}

  LCD_ClearAll();	
	
	if(gLcdCheck==0)
		EM_LCD_DisplayStatus();

// the disp_state is the index for our dspArr
// it cannot be greater than 26		
	if(disp_state>26)
		disp_state=0;

// we need to get the index of the function array to be executed at this disp_state
// which is essentially 
	tempIndex = dspArr[AUTOSCROLL_OFFSET+disp_state];
	if(tempIndex<0)	// the last element of dspArr contains -1
	{
		tempIndex = dspArr[AUTOSCROLL_OFFSET];
		disp_state=0;	
	}
// Now we can execute the function call
	(fp_display[tempIndex])();	
}

void display_onDemand(void)
{
	int8_t tempIndex;
	powermgmt_mode_t    power_mode;
  R_WDT_Restart();
  LCD_ClearAll();

	power_mode = POWERMGMT_GetMode();	// actual function to be used
	if (power_mode == POWERMGMT_MODE1)
	{
		if(gLcdCheck==0)
			EM_LCD_DisplayStatus();
	}
	
	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>89)
		disp_state=89;	// cannot be increased any more than last state

// we need to get the index of the function array to be executed at this disp_state
// which is essentially 
	tempIndex = dspArr[PUSHBUTTON_OFFSET+disp_state];
	if(tempIndex<0)	// the last element of dspArr contains -1
	{
		disp_state--;	// decrement the disp_state	
		tempIndex = dspArr[PUSHBUTTON_OFFSET+disp_state];
	}
	
// Now we can execute the function call
	(fp_display[tempIndex])();		
		
}

void display_Third(void)
{
	int8_t tempIndex;
	
  R_WDT_Restart();
  LCD_ClearAll();

	EM_LCD_DisplayStatus();

	if(disp_state==255)
		disp_state=0;
		
	if(disp_state>7)
		disp_state=7;
	
// we need to get the index of the function array to be executed at this disp_state
// which is essentially 
	tempIndex = dspArr[HIGHRES_OFFSET+disp_state];
	if(tempIndex<0)	// the last element of dspArr contains -1
	{
		disp_state--;	// decrement the disp_state	
		tempIndex = dspArr[HIGHRES_OFFSET+disp_state];
	}
	
// Now we can execute the function call
	(fp_display[tempIndex])();		
		
		
/*		
	switch(disp_state)
	{
    case 0:			// high resolution kwh import
			d_HighRes_KWH_Import();
      break;

    case 1:			// high resolution kvah import
			d_HighRes_KVAH_Import();
      break;
			
    case 2:			// high resolution kvarh lag
			d_HighRes_KVARH_LAG_Import();
      break;
			
    case 3:			// high resolution kvarh lead
			d_HighRes_KVARH_LEAD_Import();
      break;
						
    case 4:			// high resolution kwh - export
			d_HighRes_KWH_Export();
      break;
			
    case 5:			// high resolution kvah export
			d_HighRes_KVAH_Export();
      break;
			
    case 6:			// high resolution kvarh lag export
			d_HighRes_KVARH_LAG_Export();
      break;
			
    case 7:			// high resolution kvarh lead
			d_HighRes_KVARH_LEAD_Export();
      break;
	}
*/	
}

void EM_LCD_DisplayStatus(void)
{
  powermgmt_mode_t power_mode;
	power_mode = POWERMGMT_GetMode();

/*	
	if(g_display_mode==0)
	{
//		if(((disp_state==0)||(disp_state==255))&&(alt_autoscroll_mode==1)&&(sub_disp_state==255))
//		if(((disp_state==0)||(disp_state==255))&&(sub_disp_state==255))
//		if((disp_state==0)||(disp_state==255))
		if(disp_state==255)
		{
				return;
		}
	}
*/

	if (power_mode == POWERMGMT_MODE3)
		return;
		
// reverse has a combined icon
	if(((fault_code&_BV(6))!=0)||((fault_code&_BV(7))!=0)||((fault_code&_BV(8))!=0))
    LCD_DisplaySpSign(S_LEFT_ARROW);
	else
    LCD_ClearSpSign(S_LEFT_ARROW);

/*
	if((g_TamperStatus&REVERSE_MASK)!=0)
    LCD_DisplaySpSign(S_LEFT_ARROW);
	else
    LCD_ClearSpSign(S_LEFT_ARROW);
*/

// Missing potentials

	if((fault_code&_BV(0))!=0)
		LCD_ClearSpSign(S_R);
	else
		LCD_DisplaySpSign(S_R);
		
	if((fault_code&_BV(1))!=0)
		LCD_ClearSpSign(S_Y);
	else
		LCD_DisplaySpSign(S_Y);
		
	if((fault_code&_BV(2))!=0)
		LCD_ClearSpSign(S_B);
	else
		LCD_DisplaySpSign(S_B);		
		
// volt_unbalance
	if((fault_code&_BV(3))==0)
		LCD_ClearSpSign(S_V_UNBAL);
	else
		LCD_DisplaySpSign(S_V_UNBAL);

// current bypass
	if((fault_code&_BV(4))==0)
		LCD_ClearSpSign(S_T9);
	else
		LCD_DisplaySpSign(S_T9);
		
// current_unbalance
	if((fault_code&_BV(5))==0)
		LCD_ClearSpSign(S_C_UNBAL);
	else
		LCD_DisplaySpSign(S_C_UNBAL);
		
// over current and ct open have no indication		

// for CT-open use the MISS icon S_NM

	if(((fault_code&_BV(11))!=0)||((fault_code&_BV(12))!=0)||((fault_code&_BV(13))!=0))
    LCD_DisplaySpSign(S_NM);
	else
    LCD_ClearSpSign(S_NM);

//	if((fault_code&_BV(13))==0)
//		LCD_ClearSpSign(S_NM);
//	else
//		LCD_DisplaySpSign(S_NM);

// for over current let the corresponding R Y B icon blink - let it be for present

// Neutral Disturbance - If neutral goes missing N icon will disappear
	if((fault_code&_BV(9))!=0)	// this is a reverse indication
		LCD_ClearSpSign(S_N);
	else
		LCD_DisplaySpSign(S_N);

// magnet tamper
	if((fault_code&_BV(10))==0)
		LCD_ClearSpSign(S_MAG);
	else
		LCD_DisplaySpSign(S_MAG);


		
// two wire operation - blink RYB icons
	if(twoWireOperation!=0)
	{
		if((gentimer%2)==0)
		{
	  	LCD_ClearSpSign(S_R);		// R
		  LCD_ClearSpSign(S_Y);		// Y
		  LCD_ClearSpSign(S_B);		// B
		}
		else
		{
	  	LCD_DisplaySpSign(S_R);		// R
		  LCD_DisplaySpSign(S_Y);		// Y
		  LCD_DisplaySpSign(S_B);		// B
		}
	}

		
/*		
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

	if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) == 0)
    LCD_ClearSpSign(S_COPEN);
	else
    LCD_DisplaySpSign(S_COPEN);
	
		
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
*/
}












