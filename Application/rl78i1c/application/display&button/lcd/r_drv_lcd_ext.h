/***********************************************************************************************************************
* File Name    : r_drv_lcd_ext.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : MCU LCD module driver APIs
***********************************************************************************************************************/

#ifndef _R_DRV_LCD_EXT_H
#define _R_DRV_LCD_EXT_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"
#include "r_lcd_config.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/*
 * LCD RAM Definitions
 * The data area which need to be changed when changing MCU
 */
#define     LCD_RAM_START_ADDRESS   0xf0400 /* LCD Memory Start Address */
#define     LCD_RAM_END_ADDRESS     0xf0417 /* LCD Memory End Address */
#define     LCD_RAM_SIZE            (LCD_RAM_END_ADDRESS - LCD_RAM_START_ADDRESS+1)

/* Return information on each function */
#define LCD_INPUT_OK                0x00
#define LCD_WRONG_INPUT_ARGUMENT    0x80

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Controlling LCD RAM */
uint16_t LCD_GetRamSize(void);          /* LCD Get Ram Size */
void LCD_RamClearAll(void);             /* LCD Ram Clear All */
void LCD_RamSetAll(void);               /* LCD Ram Set All */

uint8_t LCD_GetNumInfo(uint8_t disp_value, uint8_t position, LCD_NUM_INFO* num_info);
uint8_t LCD_GetSignInfo(uint8_t sign, LCD_SIGN_INFO* sign_info);
void LCD_WriteRAMDigitInfo(uint32_t ram_addr, uint8_t disp_value);
void LCD_ClearRAMDigitInfo(uint32_t ram_addr, uint8_t disp_value);
void LCD_WriteRAMSignInfo(uint32_t ram_addr, uint8_t disp_value);
void LCD_ClearRAMSignInfo(uint32_t ram_addr, uint8_t disp_value);

#endif /* _R_DRV_LCD_EXT_H */
