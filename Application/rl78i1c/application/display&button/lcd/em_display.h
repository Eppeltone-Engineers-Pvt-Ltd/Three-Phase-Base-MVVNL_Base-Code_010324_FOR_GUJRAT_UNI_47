/***********************************************************************************************************************
* File Name    : em_display.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Display Application Layer APIs
***********************************************************************************************************************/

#ifndef _EM_DISPLAY_H
#define _EM_DISPLAY_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/* Wrapper layer */
#include "r_drv_lcd_ext.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct tagTimeDataInfo
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
} TIME_DATA_INFO;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
#define LCD_FIRST_DISPLAY_POS       1       /* default is paramerter 0 */
#define LCD_DELAY_TIME              5      /* Create delay time 5s */

// The following define is specifically used for TNEB to begin with
// In the definition of TAMPER_DISPLAY_MASK event code bits 1 to 15 have been accounted for by 0x0000FFFE
// the other bits are separately and individually accounted
#define TAMPER_DISPLAY_MASK	(0x0000FFFE | MAGNET_MASK | AC_MAGNET_MASK | NEUTRAL_DISTURBANCE_MASK | OVER_CURRENT_Y_MASK | OVER_CURRENT_B_MASK)



//Keyboard functions
void KEY_40msInterruptCallback(void);
void KEY_PollingProcessing(void);
void CommunicationTimeoutCheck(void);


// Prototype APIs which are used to display EM information
void handleDisplay(void);
void EM_DisplaySequence(void);
void EM_RTC_DisplayInterruptCallback(void);
void makeDisplayReadyForAutoScroll(void);

/* APIs which are used to control LCD */

void LCD_ClearDisplayCounter(void);
uint8_t getDisplayMode(void);
void LCD_SwitchDisplayMode(uint8_t dispMode);
//void LCD_SwitchAutoRoll(void);// not used any more
void LCD_ChangeNext(void);
void EM_LCD_DisplayPOR(void);

void lcd_display_error(void);

/* APIs which are used to display number on LCD */
uint8_t LCD_DisplayIntWithPos(long lNum, int8_t position, int8_t noOfDecimals);  
uint8_t LCD_DisplayFloat(float32_t fnum);                   
uint8_t LCD_DisplayFloat3Digit(float32_t fnum);

void display_long_one_dp(int32_t templong);
void display_long_two_dp(int32_t templong);
void display_long_three_dp(int32_t templong);
void display_long_five_dp(int32_t templong);

void lcd_display_multiple_chars(uint8_t whichchar, uint8_t howmany, uint8_t unitsPosition);
void lcd_msg(const uint8_t* msg);	// implemented in em_display_TN3P_Helper.c

void lcd_display_id(void);
void lcd_display_decimal(uint8_t whichDecimal);
void lcd_display_A(void);

void lcd_display_date(uint8_t dat, uint8_t mth, uint8_t yr);
void lcd_display_time(uint8_t hr, uint8_t min, uint8_t sec);

void LCD_DisplaykWSign(void);
void LCD_DisplaykVASign(void);
void LCD_DisplaykVArSign(void);
void LCD_DisplaykWhSign(void);
void LCD_DisplaykVAhSign(void);
void LCD_DisplaykVArhSign(void);

uint8_t BCD2DEC(uint8_t BCD_number);
uint8_t LCD_DisplayTime(TIME_DATA_INFO time_info, uint8_t is_used_BCD);
float absolute(float input);
void EM_DisplayInstantTime(void);
void EM_DisplayDate(void);
void displayDay(uint8_t day);

void lcd_display_multiple_chars(uint8_t whichchar, uint8_t howmany, uint8_t unitsPosition);
void lcd_msg(const uint8_t* msg);
void display_long_five_dp(int32_t templong);
void display_long_three_dp(int32_t templong);
void display_long_two_dp(int32_t templong);
void display_long_one_dp(int32_t templong);
void lcd_display_decimal(uint8_t whichDecimal);
//uint8_t LCD_DisplayIntWithPos(long lNum, int8_t position,int8_t noOfDecimals);
uint8_t LCD_DisplayFloat3Digit(float32_t fnum);
uint8_t LCD_DisplayFloat(float32_t fnum);
void lcd_display_version(void);

void lcd_display_fail(void);
void lcd_display_pass(void);
void lcd_display_Cal_Status(void);

#endif /* _EM_DISPLAY_H */
