/***********************************************************************************************************************
* File Name    : r_lcd_display.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : LCD Segment Data display APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"       /* macro driver definitions */
#include "r_lcd_display.h"
#include "r_lcd_config.h"
#include "utilities.h"	// for swapNibbles function
/***********************************************************************************************************************
* Function Name: void LCD_DisplayAll(void)
* Description  : Display all digit on LCD screen
* Arguments    : none
* Output       : none
* Return Value : none
***********************************************************************************************************************/
void LCD_DisplayAll(void)
{
    LCD_RamSetAll();
}

/***********************************************************************************************************************
* Function Name: void LCD_ClearAll(void)
* Description  : Clear all digit on LCD screen
* Arguments    : none
* Output       : none
* Return Value : none
***********************************************************************************************************************/
void LCD_ClearAll(void)
{
    LCD_RamClearAll();
}



void lcd_clearTopLine(void)
{
// the digit positions 1 to 9 (segments 0 to 17) have one icon at bit position 0 at the odd address
// The icons are COMET, CUM, BILL, P1, P2, P3, T O D, MD and 9H
// If we zero all the RAM locations - the icons will also get erased - which is desirable
// we may want the COMET icon to be turned on
// Also the fourth decimal point may have to be specially turned off.
  uint16_t i;
  uint8_t __far * lcdptr = (uint8_t __far *)LCD_RAM_START_ADDRESS;
	uint8_t tempByte;
	
  for (i = 0; i < 18; i++)
  {
    *((uint8_t*)lcdptr + i) = 0x00;
  }

// we may want to separately turn off fourth decimal	which represents a 5 decimals value
	LCD_ClearSpSign(S_D2);	// 5 decimals
// finally we may want to turn on the manufacturer's logo
//	LCD_DisplaySpSign(S_LOGO);
}




/***********************************************************************************************************************
* Function Name: uint8_t uint8_t LCD_DisplayDigit(uint8_t position, uint8_t value)
* Description  : Diplay [value] at [position] on LCD screen
* Arguments    : [value] value need to be display
*              : [position] the position of digit on LCD screen
* Output       : none
* Return Value : LCD_WRONG_INPUT_ARGUMENT: input wrong argument
*              : LCD_INPUT_OK: input OK argument
***********************************************************************************************************************/
uint8_t LCD_DisplayDigit(uint8_t position, uint8_t value)
{
    uint32_t lcdptr;
    uint8_t disp_value, data_status;
    LCD_NUM_INFO num_info;
    
    /* check for range of input argument */
    if ((position > (LCD_NUM_DIGIT)) ||
        (value > (LCD_NUM_DIGIT_CHAR - 1)))
    {
        return LCD_WRONG_INPUT_ARGUMENT;
    }
    
    LCD_ClearDigit(position);
    
    /* Get display number information */
    data_status = LCD_GetNumInfo(value, position, &num_info);
//    if (data_status == LCD_WRONG_INPUT_ARGUMENT)
//    {
//        return LCD_WRONG_INPUT_ARGUMENT;
//    }
    
    lcdptr = num_info.addr;
//    if (lcdptr == 0)
//    {
//        return LCD_WRONG_INPUT_ARGUMENT;
//    }
        
    /* Turn on all related-segment of this digit */
#ifdef SEGMENTS_REVERSED
    disp_value = swapNibbles(num_info.value);
#else
    disp_value = num_info.value;
#endif
		
    LCD_WriteRAMDigitInfo(lcdptr, disp_value);
    
    return LCD_INPUT_OK;
}

/***********************************************************************************************************************
* Function Name: void LCD_ClearDigit(uint8_t position)
* Description  : Clear digit at specified position
* Arguments    : [position] where the digit need to be cleared
* Output       : none
* Return Value : LCD_WRONG_INPUT_ARGUMENT: input wrong argument
*              : LCD_INPUT_OK: input OK argument
***********************************************************************************************************************/
uint8_t LCD_ClearDigit(uint8_t position)
{
    /* Write 20 to specified position to clear the digit */
    uint32_t lcdptr;
    uint8_t disp_value, data_status;
    LCD_NUM_INFO num_info;
    
    /* check for range of input argument */
//    if (position > (LCD_NUM_DIGIT))
//    {
//        return LCD_WRONG_INPUT_ARGUMENT;
//    }
        
    /* Get display number information */
    data_status = LCD_GetNumInfo(LCD_CLEAR_DIGIT, position, &num_info);
//    if (data_status == LCD_WRONG_INPUT_ARGUMENT)
//    {
//        return LCD_WRONG_INPUT_ARGUMENT;
//    }
    
    lcdptr = num_info.addr;
//    if (lcdptr == 0)
//    {
//        return LCD_WRONG_INPUT_ARGUMENT;
//    }
        
    /* Turn off all related-segment of this digit */
#ifdef SEGMENTS_REVERSED
    disp_value = swapNibbles(num_info.value);
#else
    disp_value = num_info.value;
#endif
    LCD_ClearRAMDigitInfo(lcdptr, disp_value);
    
    return LCD_INPUT_OK;
}

/***********************************************************************************************************************
* Function Name: static uint8_t LCD_DisplayRawSign(LCD_EM_SPEC_SIGN raw_sign)
* Description  : Display directly each raw_sign on LCD
* Arguments    : [raw_sign] value need to be displayed
* Output       : none
* Return Value : LCD_WRONG_INPUT_ARGUMENT: input wrong argument
*              : LCD_INPUT_OK: input OK argument
***********************************************************************************************************************/
uint8_t LCD_DisplaySpSign(LCD_EM_SPEC_SIGN raw_sign)
{
    uint32_t lcdptr;
    uint8_t value, sign_status;
    LCD_SIGN_INFO sign_info;
    
    sign_status = LCD_GetSignInfo(raw_sign, &sign_info);

    lcdptr = sign_info.addr;
    value = (uint8_t)(0x1 << sign_info.pos);
    LCD_WriteRAMSignInfo(lcdptr, value);
    
    return LCD_INPUT_OK;
}

/***********************************************************************************************************************
* Function Name: static uint8_t LCD_ClearRawSign(LCD_SPECIAL_SIGN raw_sign)
* Description  : Clear directly each raw_sign on LCD
* Arguments    : [raw_sign] value need to be displayed
* Output       : none
* Return Value : LCD_WRONG_INPUT_ARGUMENT: input wrong argument
*              : LCD_INPUT_OK: input OK argument
***********************************************************************************************************************/
uint8_t LCD_ClearSpSign(LCD_EM_SPEC_SIGN raw_sign)
{
    uint32_t lcdptr;
    uint8_t value, sign_status;
    LCD_SIGN_INFO sign_info;
    
    /* check for range of raw_sign: only in (0, 22) */
    if ((raw_sign > (LCD_NUM_SIGN - 1)) || (raw_sign < 0))
    {
        return LCD_WRONG_INPUT_ARGUMENT;
    }
    
    /* turn on the related raw_sign */
    sign_status = LCD_GetSignInfo(raw_sign, &sign_info);
    if (sign_status != LCD_INPUT_OK)
    {
        return LCD_WRONG_INPUT_ARGUMENT;
    }
    
    lcdptr = sign_info.addr;
    if (lcdptr == 0)
    {
        return LCD_WRONG_INPUT_ARGUMENT;
    }

    /* Write 0x0 to specified position to clear this raw_sign */
    value = (uint8_t)(~(0x1 << sign_info.pos));
    LCD_ClearRAMSignInfo(lcdptr, value);
    
    return LCD_INPUT_OK;
}
