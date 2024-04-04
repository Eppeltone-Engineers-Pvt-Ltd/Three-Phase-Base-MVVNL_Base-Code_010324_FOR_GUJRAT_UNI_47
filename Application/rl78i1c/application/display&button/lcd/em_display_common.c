// em_display_common.c
// This file contains functions related to display application which are common among the various boards
// Functions in this file will be prototyped in em_display.h
#include "typedef.h"            /* GCSE Standard definitions */
#include "r_cg_rtc.h"
#include "emeter3_structs.h"
#include "r_lcd_display.h"      /* LCD Display Application Layer */
#include "r_lcd_config.h"
#include "em_display.h"         /* EM Display Application Layer */

#define _BV(i)	(1<<i)


const uint8_t msg_ver_string[8] = {LCD_CHAR_V, LCD_CHAR_E, LCD_CHAR_R, LCD_CLEAR_DIGIT, 2, 2, LCD_CHAR_B, LCD_CLEAR_DIGIT};

const uint8_t msg_sunday[8] = {5, LCD_CHAR_U, LCD_CHAR_N, LCD_CHAR_D, LCD_CHAR_A,LCD_CHAR_Y,LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_monday[8] = {LCD_CHAR_M, 0, LCD_CHAR_N, LCD_CHAR_D, LCD_CHAR_A,LCD_CHAR_Y,LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_tuesday[8] = {LCD_CHAR_T, LCD_CHAR_U, LCD_CHAR_E, 5,LCD_CHAR_D, LCD_CHAR_A,LCD_CHAR_Y,LCD_CLEAR_DIGIT};
const uint8_t msg_wednesday[8] = {LCD_CHAR_V, LCD_CHAR_E, LCD_CHAR_D, LCD_CHAR_N, LCD_CHAR_D,LCD_CHAR_A,LCD_CHAR_Y, LCD_CLEAR_DIGIT};
const uint8_t msg_thursday[8] = {LCD_CHAR_T, LCD_CHAR_H, LCD_CHAR_U, LCD_CHAR_R, LCD_CHAR_D,LCD_CHAR_A,LCD_CHAR_Y, LCD_CLEAR_DIGIT};
const uint8_t msg_friday[8] = {LCD_CHAR_F, LCD_CHAR_R, 1, LCD_CHAR_D, LCD_CHAR_A,LCD_CHAR_Y,LCD_CLEAR_DIGIT, LCD_CLEAR_DIGIT};
const uint8_t msg_saturday[8] = {5, LCD_CHAR_A, LCD_CHAR_T, LCD_CHAR_R, LCD_CHAR_D,LCD_CHAR_A,LCD_CHAR_Y, LCD_CLEAR_DIGIT};


void lcd_display_Cal_Status(void)
{//0-R, 1-Y, 2-B, 3-S, 4-N, 5-D
  LCD_ClearAll();
	if((Common_Data.meter_calibrated&_BV(0))==0)
  	LCD_DisplayDigit(1, LCD_CHAR_F);
	else
  	LCD_DisplayDigit(1, LCD_CHAR_R);
	
	if((Common_Data.meter_calibrated&_BV(1))==0)
  	LCD_DisplayDigit(2, LCD_CHAR_F);
	else
  	LCD_DisplayDigit(2, LCD_CHAR_Y);
		
	if((Common_Data.meter_calibrated&_BV(2))==0)
  	LCD_DisplayDigit(3, LCD_CHAR_F);
	else
  	LCD_DisplayDigit(3, LCD_CHAR_B);
		
	if((Common_Data.meter_calibrated&_BV(3))==0)
  	LCD_DisplayDigit(4, LCD_CHAR_F);
	else
  	LCD_DisplayDigit(4, 5);
		
	if((Common_Data.meter_calibrated&_BV(4))==0)
  	LCD_DisplayDigit(5, LCD_CHAR_F);
	else
  	LCD_DisplayDigit(5, LCD_CHAR_N);
		
	if((Common_Data.meter_calibrated&_BV(5))==0)
  	LCD_DisplayDigit(6, LCD_CHAR_F);
	else
  	LCD_DisplayDigit(6, LCD_CHAR_D);
}

void lcd_display_pass(void)
{
  LCD_ClearAll();	
  LCD_DisplayDigit(2, LCD_CHAR_P);
	LCD_DisplayDigit(3, LCD_CHAR_A);
	LCD_DisplayDigit(4, 5);
	LCD_DisplayDigit(5, 5);
}

void lcd_display_fail(void)
{
  LCD_ClearAll();	
  LCD_DisplayDigit(2, LCD_CHAR_F);
	LCD_DisplayDigit(3, LCD_CHAR_A);
	LCD_DisplayDigit(4, 1);
	LCD_DisplayDigit(5, LCD_CHAR_L);
}


void lcd_display_error(void)
{
  LCD_DisplayDigit(9, LCD_CHAR_E);
	LCD_DisplayDigit(8, LCD_CHAR_R);
}

/*
void lcd_display_version(void)
{
  LCD_ClearAll();
	lcd_msg((const uint8_t*)&msg_ver_string);
  lcd_display_decimal(2);
}
*/

void lcd_display_version(void)
{
	uint8_t dbuf[8];
	uint8_t i;
	
	dbuf[0] = BOARD_ID/10;
	dbuf[1] = BOARD_ID%10;
	
	dbuf[2] = VERNO/100;
	dbuf[3] = (VERNO - (dbuf[2]*100))/10;
	dbuf[4] = VERNO%10;
	#ifdef BIDIR_METER
		dbuf[5] =	LCD_CHAR_B;
	#else
		dbuf[5] =	LCD_CHAR_U;
	#endif
	
//	dbuf[6] = SPLCODE;	// not used any more
	dbuf[6]=METER_CLASS;
	
	for(i=0;i<7;i++)
	{
    LCD_DisplayDigit(i+1, dbuf[i]);
	}
	lcd_display_decimal(3);
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

void EM_DisplayInstantTime(void)
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

void displayDay(uint8_t day)
{// day will be from 1 to 7 always
	switch(day)
	{
		case 1:
			lcd_msg((const uint8_t*)&msg_monday);
			break;
	
		case 2:
			lcd_msg((const uint8_t*)&msg_tuesday);
			break;
	
		case 3:
			lcd_msg((const uint8_t*)&msg_wednesday);
			break;
	
		case 4:
			lcd_msg((const uint8_t*)&msg_thursday);
			break;
	
		case 5:
			lcd_msg((const uint8_t*)&msg_friday);
			break;
	
		case 6:
			lcd_msg((const uint8_t*)&msg_saturday);
			break;
	
		case 7:
			lcd_msg((const uint8_t*)&msg_sunday);
			break;

		default:
			break;
	
	}
}

void EM_DisplayDate(void)
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

uint8_t BCD2DEC(uint8_t BCD_number)
{
  volatile uint8_t rts;

  rts = (uint8_t)(BCD_number & 0x0f);
  BCD_number = (uint8_t)(((BCD_number & 0xf0) >> 4) * 10);
  rts = (uint8_t)(rts + BCD_number);
  return rts; 
}

float absolute(float input)
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

    LCD_DisplayIntWithPos(disp_data, 7,0);
	
    /* If the digit of hour less than 10 */
    if (is_zero_hour == 1)
    {
//        LCD_DisplayDigit(2, 0); /* Display 0 at lower digit of hour value */
        LCD_DisplayDigit(3, 0); // Display 0 at lower digit of hour value
    }
    if (time_info.hour < 10)
    {
//        LCD_DisplayDigit(1, 0); /* Display 0 at higher digit of hour value */
        LCD_DisplayDigit(2, 0); // Display 0 at higher digit of hour value
    }
    
    /* Clear the 8th digit on LCD */
    LCD_ClearDigit(8);
    
    /* Diplay ":" between hour:min:sec */
//    LCD_DisplaySpSign(S_D1);	// This meter has a single control for COLON.
    LCD_DisplaySpSign(S_D3);
    
    return LCD_INPUT_OK;
}

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
    if ((absolute(fnum) < (ref_value + 1)) && (is_zero_point == 1))
    {
        fnum = 100;
    }
    
    /* Display integer number */
    disp_status = LCD_DisplayIntWithPos((int32_t)fnum, LCD_LAST_POS_DIGIT,0);

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
    if (absolute(fnum) < 0.001)
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
    disp_status = LCD_DisplayIntWithPos((int32_t)fnum, LCD_LAST_POS_DIGIT,0);

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



uint8_t LCD_DisplayIntWithPos(long lNum, int8_t position, int8_t noOfDecimals)
{
    int8_t pos = position;
    int8_t sign;            // store the sign of value: positive, negative
    uint8_t is_disp_error = LCD_INPUT_OK;
		int8_t posValueForSign;
    
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
			posValueForSign = position-noOfDecimals;
			
			if(pos>=posValueForSign)
				pos = (posValueForSign-1);
			
			
//				if((pos <5)&&(pos > LCD_FIRST_POS_DIGIT))	// why this check of pos<5
				if(pos > LCD_FIRST_POS_DIGIT)	// why this check of pos<5
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
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT,1);
  lcd_display_decimal(1);
}

void display_long_two_dp(int32_t templong)
{// this fellow displays a long and two digits in decimal 
  lcd_display_multiple_chars(0,3,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT,2);
  lcd_display_decimal(2);
}

void display_long_three_dp(int32_t templong)
{// this fellow displays a long and three digits in decimal 
  lcd_display_multiple_chars(0,4,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT,3);
  lcd_display_decimal(3);
}

void display_long_five_dp(int32_t templong)
{// this fellow displays a long and five digits in decimal 
  lcd_display_multiple_chars(0,7,LCD_LAST_POS_DIGIT);
  LCD_DisplayIntWithPos(templong,LCD_LAST_POS_DIGIT,5);
  lcd_display_decimal(5);
}

void lcd_msg(const uint8_t* msg)
{
	uint8_t i;
	for(i=0;i<7;i++)
	{
    LCD_DisplayDigit(i+1, msg[i]);
	}
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

/*
void EM_LCD_DisplayPOR(void)
{
    g_is_por = 0x01;
}
*/







