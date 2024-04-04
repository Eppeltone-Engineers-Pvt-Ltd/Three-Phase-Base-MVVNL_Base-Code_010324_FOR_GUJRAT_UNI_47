// em_display_Kerala_Helper.c
#include "r_cg_macrodriver.h"
#include "typedef.h"            /* GCSE Standard definitions */
#include "emeter3_structs.h"

#include "r_dlms_obis_ic.h"
#include "r_dlms_data.h"

#include "r_lcd_display.h"      /* LCD Display Application Layer */
#include "r_lcd_config.h"
#include "em_display.h"         /* EM Display Application Layer */

#include "memoryOps.h"
#include "e2rom.h"

#include "em_display_Kerala_Helper.h"

extern uint8_t holdState;
extern uint8_t g_tamper_display_state;
extern day_profile_action_t	day_action0[];
extern day_profile_action_t	day_action1[];



void display_TOD_KWH_CURRENT(void)
{
	int32_t tempTodKwh; 
	uint8_t tempchar;
	
	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the first tod scriptid
	}

	tempchar=g_tamper_display_state;// this is now (scriptid-1)
// at least one zone will always be displayed - even if nothing has been defined			
				
	read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
	if(tempchar==presentZone)
	{
		tempTodKwh=myenergydata.zone_kwh;
	}
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
	LCD_DisplayIntWithPos(tempTodKwh/1000, LCD_LAST_POS_DIGIT,0);
	LCD_DisplaykWhSign();
	LCD_DisplaySpSign(S_T8);	// CUM Sign
  LCD_DisplaySpSign(S_TOD);
	LCD_DisplayDigit(9, tempchar+1);
	disp_autoscroll_interval=6;
	fast_display_flag=0;
	
	tempchar++;
	if((0==day_action0[tempchar].script_selector)||(0==day_action0[0].script_selector))
	{// job done - the next zone's script id is 0 or zone 0's script id is 0
		holdState=0;	// allow state to be incremented
	}
}

void display_TOD_KVAH_CURRENT(void)
{
	int32_t tempTodKwh; 
	uint8_t tempchar;
	
	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the first tod scriptid
	}

	tempchar=g_tamper_display_state;// this is now (scriptid-1)
// at least one zone will always be displayed - even if nothing has been defined			
				
	read_alternately(TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC+4, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+tempchar*ZONE_PTR_INC+4, (uint8_t*)&tempTodKwh, sizeof(tempTodKwh));
	if(tempchar==presentZone)
	{
		tempTodKwh=myenergydata.zone_kvah;
	}
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
	LCD_DisplayIntWithPos(tempTodKwh/1000, LCD_LAST_POS_DIGIT,0);
	LCD_DisplaykVAhSign();
  LCD_DisplaySpSign(S_TOD);
	LCD_DisplayDigit(9, tempchar+1);
	disp_autoscroll_interval=6;
	fast_display_flag=0;
	
	tempchar++;
	if((0==day_action0[tempchar].script_selector)||(0==day_action0[0].script_selector))
	{// job done - the next zone's script id is 0 or zone 0's script id is 0
		holdState=0;	// allow state to be incremented
	}
}

void display_TOD_MDKW_DTTM_CURRENT(void)
{
	ext_rtc_t datetime;
	uint16_t tempint;
	uint8_t zone;
	uint8_t displayWhat;

	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the starting state
	}

	zone=g_tamper_display_state/3;	// zone is now (scriptid-1)
	displayWhat = (g_tamper_display_state%3);	// this variable will always be 0,1,2

	switch(displayWhat)
	{
		case 0:
		// MD Value
      EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+8,(uint8_t*)&tempint,2);
			display_long_three_dp(tempint);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
			break;
	
		case 1:
		// MD Date
      EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+12,(uint8_t*)&datetime,sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
			break;
	
		case 2:
		// MD Time
      EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+12,(uint8_t*)&datetime,sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplaykWSign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
		
			zone++;
			if((0==day_action0[zone].script_selector)||(0==day_action0[0].script_selector))
			{// job done - the next zone's script id is 0 or zone 0's script id is 0
				holdState=0;	// allow state to be incremented
			}	
			break;
	}
}

void display_TOD_MDKVA_DTTM_CURRENT(void)
{
	ext_rtc_t datetime;
	uint16_t tempint;
	uint8_t zone;
	uint8_t displayWhat;

	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the starting state
	}

	zone=g_tamper_display_state/3;	// zone is now (scriptid-1)
	displayWhat = (g_tamper_display_state%3);	// this variable will always be 0,1,2

	switch(displayWhat)
	{
		case 0:
		// MD Value
  		EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+10, (uint8_t*)&tempint, sizeof(tempint));
			display_long_three_dp(tempint);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
			break;
	
		case 1:
		// MD Date
  		EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+20, (uint8_t*)&datetime, sizeof(datetime));
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
			break;
	
		case 2:
		// MD Time
  		EPR_Read(TOD_CURRENT_BASE_ADDRESS+zone*ZONE_PTR_INC+20, (uint8_t*)&datetime, sizeof(datetime));
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplaykVASign();
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
		
			zone++;
			if((0==day_action0[zone].script_selector)||(0==day_action0[0].script_selector))
			{// job done - the next zone's script id is 0 or zone 0's script id is 0
				holdState=0;	// allow state to be incremented
			}	
			break;
	}
}

void display_TOD_KWH_BILL(uint8_t tempchar)
{// tempchar gives the bill number (1 means L1)
	int32_t tempLong;
	uint8_t zone;
	uint8_t tmonth;
	
	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the starting state
	}

	zone=g_tamper_display_state;	// zone is now (scriptid-1)
// at least one zone will always be displayed - even if nothing has been defined			

  tmonth = decrementMonth(tempchar);
  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+20+zone*4,(uint8_t*)&tempLong,4);
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
	LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
	LCD_DisplaykWhSign();
	LCD_DisplaySpSign(S_T8);	// CUM Sign
  LCD_DisplaySpSign(S_BILL);
  LCD_DisplaySpSign(S_TOD);
	LCD_DisplayDigit(9, zone+1);
	fast_display_flag=0;
	
	zone++;
	if((0==day_action0[zone].script_selector)||(0==day_action0[0].script_selector))
	{// job done - the next zone's script id is 0 or zone 0's script id is 0
		holdState=0;	// allow state to be incremented
	}	
}




// This function will display only the MD KVA zone wise and not the date time for previous bills (1=L1 etc)
// Each time this function is called the value of g_tamper_display_state will represent the script_id
void display_TOD_MDKVA_BILL(uint8_t tempchar)
{
	int32_t tempLong;
	uint8_t zone;
	uint8_t tmonth;
	
	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the starting state
	}

	zone=g_tamper_display_state;	// zone is now (scriptid-1)
	
  tmonth = decrementMonth(tempchar);
  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+208+zone*4,(uint8_t*)&tempLong,4);
	display_long_three_dp(tempLong);
	LCD_DisplaykVASign();
	LCD_DisplaySpSign(S_BILL);
  LCD_DisplaySpSign(S_TOD);
  LCD_DisplaySpSign(S_MD);
	LCD_DisplayDigit(9, zone+1);
	fast_display_flag=0;
	
	zone++;
	if((0==day_action0[zone].script_selector)||(0==day_action0[0].script_selector))
	{// job done - the next zone's script id is 0 or zone 0's script id is 0
		holdState=0;	// allow state to be incremented
	}	
}

// This function will display the MD KVA zone wise for previous bills (1=L1 etc)
// In this function three values of g_tamper_display_state will be used to display md value, date and time
void display_TOD_MDKVA_DTTM_BILL(uint8_t tempchar)
{
	date_time_t DateTime;
	ext_rtc_t datetime;

	int32_t tempLong;
	uint8_t zone;
	uint8_t tmonth;
	uint8_t displayWhat;
  tmonth = decrementMonth(tempchar);

	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the starting state
	}

	zone=g_tamper_display_state/3;	// zone is now (scriptid-1)
	displayWhat = (g_tamper_display_state%3);	// this variable will always be 0,1,2

	switch(displayWhat)
	{
		case 0:
		// MD Value
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+208+zone*4,(uint8_t*)&tempLong,4);
			display_long_three_dp(tempLong);
			LCD_DisplaykVASign();
  		LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
			break;
	
		case 1:
		// MD Date
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+232+zone*12,(uint8_t*)&DateTime,12);
			ConvertBackToRTC(&datetime, &DateTime);
			lcd_display_date(datetime.date,datetime.month,datetime.year);
			LCD_DisplaykVASign();
  		LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
			break;
	
		case 2:
		// MD Time
      EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+232+zone*12,(uint8_t*)&DateTime,12);
			ConvertBackToRTC(&datetime, &DateTime);
			lcd_display_time(datetime.hour,datetime.minute,datetime.second);
			LCD_DisplaykVASign();
  		LCD_DisplaySpSign(S_BILL);
		  LCD_DisplaySpSign(S_TOD);
		  LCD_DisplaySpSign(S_MD);
			LCD_DisplayDigit(9, zone+1);
			fast_display_flag=0;
		
			zone++;
			if((0==day_action0[zone].script_selector)||(0==day_action0[0].script_selector))
			{// job done - the next zone's script id is 0 or zone 0's script id is 0
				holdState=0;	// allow state to be incremented
			}	
			break;
	}
}

void display_TOD_KVAH_BILL(uint8_t tempchar)
{// tempchar gives the bill number (1 means L1)
	int32_t tempLong;
	uint8_t zone;
	uint8_t tmonth;
	
	if(holdState==0)
	{
		holdState = 1;
		g_tamper_display_state = 0;	// this is a general counter and the value 0 will mean the starting state
	}

	zone=g_tamper_display_state;	// zone is now (scriptid-1)
// at least one zone will always be displayed - even if nothing has been defined			

  tmonth = decrementMonth(tempchar);
  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+56+zone*4,(uint8_t*)&tempLong,4);
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
	LCD_DisplayIntWithPos(tempLong/1000, LCD_LAST_POS_DIGIT,0);
	LCD_DisplaykVAhSign();
	LCD_DisplaySpSign(S_T8);	// CUM Sign
  LCD_DisplaySpSign(S_BILL);
  LCD_DisplaySpSign(S_TOD);
	LCD_DisplayDigit(9, zone+1);
	fast_display_flag=0;
	
	zone++;
	if((0==day_action0[zone].script_selector)||(0==day_action0[0].script_selector))
	{// job done - the next zone's script id is 0 or zone 0's script id is 0
		holdState=0;	// allow state to be incremented
	}	
}




