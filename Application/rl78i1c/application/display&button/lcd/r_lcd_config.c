/***********************************************************************************************************************
* File Name    : r_lcd_config.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Declare const array for LCD configuration
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_lcd_config.h"
#include "r_drv_lcd_ext.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
/*
 * Data mapping:
 * This data must be changed when there is any diffenence on PIN
 * (COM and SEGMENT) connection setting between LCD and MCU
 */
 
#ifdef SEGMENTS_REVERSED	// Seg23 ->	Pin1, Seg22 ->	Pin2 ... Seg 0	->	Pin24
	const LCD_NUM_MAP g_LCD_NumMap[LCD_NUM_DIGIT] = 
	{
	    /* Pos   Address Number Type */
	    {1,    LCD_RAM_START_ADDRESS + 23,    NUM_TYPE_A},
	    {2,    LCD_RAM_START_ADDRESS + 21,    NUM_TYPE_A},
	    {3,    LCD_RAM_START_ADDRESS + 19,    NUM_TYPE_A},
	    {4,    LCD_RAM_START_ADDRESS + 17,    NUM_TYPE_A},
	    {5,    LCD_RAM_START_ADDRESS + 15,    NUM_TYPE_A},
	    {6,    LCD_RAM_START_ADDRESS + 13,    NUM_TYPE_A},
	    {7,    LCD_RAM_START_ADDRESS + 11,    NUM_TYPE_A},
	    {8,    LCD_RAM_START_ADDRESS + 9,    NUM_TYPE_A},
	    {9,    LCD_RAM_START_ADDRESS + 7,    NUM_TYPE_A},
	};

	/* Mapping all special sign of LCD to LCDRAM */
	const LCD_SIGN_MAP g_LCD_SignMap[LCD_NUM_SIGN] = 
	{// there are 33 sysmbols in all
	    /* Number No.   Address   Bit No. */
	    {S_LAG   , LCD_RAM_START_ADDRESS +  0, 1}, /* 00 - OK is for LAG */
	    {S_DATE , LCD_RAM_START_ADDRESS +  0, 3}, /* 01 */
	    {S_TIME , LCD_RAM_START_ADDRESS +  0, 2}, /* 02 */
	    {S_V_UNBAL, LCD_RAM_START_ADDRESS +  3, 2}, /* 03 - VOLTAGE UNBALANCE */
	    {S_IMPORT  , LCD_RAM_START_ADDRESS +  3, 1}, /* 04 - IMPORT */
	    {S_NM   , LCD_RAM_START_ADDRESS + 1, 0}, /* 05 - MISS */
	    {S_MAG  , LCD_RAM_START_ADDRESS + 1, 2}, /* 06 - MAG */
	    {S_BILL , LCD_RAM_START_ADDRESS + 18, 0}, /* 07 */
	    {S_EXPORT   , LCD_RAM_START_ADDRESS + 3, 0}, /* 08 - T2 EXPORT-> */
	    {S_LEAD  , LCD_RAM_START_ADDRESS + 0, 0}, /* 09 - BAT is for LEAD */
	    {S_LEFT_ARROW , LCD_RAM_START_ADDRESS + 22, 0}, /* 10 - LEFT ARROW */ 
	    {S_C_UNBAL , LCD_RAM_START_ADDRESS + 4, 3}, /* 11 - CURRENT UNBALANCE */ 
	    {S_N    , LCD_RAM_START_ADDRESS + 2, 3}, /* 12 - Assigned to N symbol */ 
	    {S_R    , LCD_RAM_START_ADDRESS + 2, 2}, /* 13 */ 
	    {S_Y    , LCD_RAM_START_ADDRESS + 2, 1}, /* 14 */ 
	    {S_B    , LCD_RAM_START_ADDRESS + 2, 0}, /* 15 */ 
	    {S_T8   , LCD_RAM_START_ADDRESS + 20, 0}, /* 16 - alloted CUM*/ 
	    {S_T7   , LCD_RAM_START_ADDRESS + 5, 3}, /* 17 - Symbol h*/ 
	    {S_T6   , LCD_RAM_START_ADDRESS + 5, 2}, /* 18 - Symbol r */ 
	    {S_T5   , LCD_RAM_START_ADDRESS + 5, 1}, /* 19 - Symbol S4 */ 
	    {S_T4   , LCD_RAM_START_ADDRESS + 5, 0}, /* 20 - Symbol S3 */ 
	    {S_T3   , LCD_RAM_START_ADDRESS + 4, 0}, /* 21 - Symbol S2 */ 
	    {S_T2   , LCD_RAM_START_ADDRESS + 4, 1}, /* 22 - Symbol S1 'V'*/ 
	    {S_T1   , LCD_RAM_START_ADDRESS + 4, 2}, /* 23 - Symbol 'K' */    
	    {S_D6   , LCD_RAM_START_ADDRESS + 16, 0}, /* 24 - Decimal Point P1 */
	    {S_D5   , LCD_RAM_START_ADDRESS + 14, 0}, /* 25 - Decimal Point P2 */
	    {S_D4   , LCD_RAM_START_ADDRESS + 12 , 0}, /* 26 - Decimal Point P3 */
	    {S_D3   , LCD_RAM_START_ADDRESS + 3 , 3}, /* 27 - COLon*/
	    {S_D2   , LCD_RAM_START_ADDRESS + 1 , 1}, /* 28 - Decimal Point P4 */
	    {S_D1   , LCD_RAM_START_ADDRESS + 7 , 2}, /* 29 - assigned to H */
	    {S_T9   , LCD_RAM_START_ADDRESS + 1 , 3}, /* 30 - Assigned to BYPASS*/
	    {S_TOD  , LCD_RAM_START_ADDRESS + 10 , 0}, /* 31 */
	    {S_MD   , LCD_RAM_START_ADDRESS + 9 , 0}, /* 32 */
	};
#else
	const LCD_NUM_MAP g_LCD_NumMap[LCD_NUM_DIGIT] = 
	{
	    /* Pos   Address Number Type */
	    {1,    LCD_RAM_START_ADDRESS + 1 ,    NUM_TYPE_A},
	    {2,    LCD_RAM_START_ADDRESS + 3 ,    NUM_TYPE_A},
	    {3,    LCD_RAM_START_ADDRESS + 5 ,    NUM_TYPE_A},
	    {4,    LCD_RAM_START_ADDRESS + 7 ,    NUM_TYPE_A},
	    {5,    LCD_RAM_START_ADDRESS + 9 ,    NUM_TYPE_A},
	    {6,    LCD_RAM_START_ADDRESS + 11,    NUM_TYPE_A},
	    {7,    LCD_RAM_START_ADDRESS + 13,    NUM_TYPE_A},
	    {8,    LCD_RAM_START_ADDRESS + 15,    NUM_TYPE_A},
	    {9,    LCD_RAM_START_ADDRESS + 17,    NUM_TYPE_A},
	};

/* Mapping all special sign of LCD to LCDRAM */
	const LCD_SIGN_MAP g_LCD_SignMap[LCD_NUM_SIGN] = 
	{
	    /* Number No.   Address   Bit No. */
	    {S_OK   , LCD_RAM_START_ADDRESS + 23 , 1}, /* 00 */
	    {S_DATE , LCD_RAM_START_ADDRESS + 23, 3}, /* 01 */
	    {S_TIME , LCD_RAM_START_ADDRESS + 23, 2}, /* 02 */
	    {S_EARTH, LCD_RAM_START_ADDRESS + 20, 2}, /* 03 */
	    {S_REV  , LCD_RAM_START_ADDRESS + 20, 1}, /* 04 */
	    {S_NM   , LCD_RAM_START_ADDRESS + 22, 0}, /* 05 */
	    {S_MAG  , LCD_RAM_START_ADDRESS + 22, 2}, /* 06 */
	    {S_BILL , LCD_RAM_START_ADDRESS + 5, 0}, /* 07 */
	    {S_PF   , LCD_RAM_START_ADDRESS + 20, 0}, /* 08 - -> */
	    {S_BAT  , LCD_RAM_START_ADDRESS + 2, 3}, /* 09 - T10 replaced with BAT */
	    {S_LOGO , LCD_RAM_START_ADDRESS + 1, 0}, /* 10 - Company LOGO */ 
	    {S_COPEN , LCD_RAM_START_ADDRESS + 19, 3}, /* 11 - C-OPEN */ 
	    {S_N    , LCD_RAM_START_ADDRESS + 21, 3}, /* 12 - Assigned to N symbol */ 
	    {S_R    , LCD_RAM_START_ADDRESS + 21, 2}, /* 13 */ 
	    {S_Y    , LCD_RAM_START_ADDRESS + 21, 1}, /* 14 */ 
	    {S_B    , LCD_RAM_START_ADDRESS + 21, 0}, /* 15 */ 
	    {S_T8   , LCD_RAM_START_ADDRESS + 3, 0}, /* 16 - wonder if this is z - alloted CUM*/ 
	    {S_T7   , LCD_RAM_START_ADDRESS + 18, 3}, /* 17 - Symbol h*/ 
	    {S_T6   , LCD_RAM_START_ADDRESS + 18, 2}, /* 18 - Symbol r */ 
	    {S_T5   , LCD_RAM_START_ADDRESS + 18, 1}, /* 19 - Symbol S4 */ 
	    {S_T4   , LCD_RAM_START_ADDRESS + 18, 0}, /* 20 - Symbol S3 */ 
	    {S_T3   , LCD_RAM_START_ADDRESS + 19, 0}, /* 21 - Symbol S2 */ 
	    {S_T2   , LCD_RAM_START_ADDRESS + 19, 1}, /* 22 - Symbol S1 'V'*/ 
	    {S_T1   , LCD_RAM_START_ADDRESS + 19, 2}, /* 23 - Symbol 'K' */    
	    {S_D6   , LCD_RAM_START_ADDRESS +  7, 0}, /* 24 - Decimal Point P1 */
	    {S_D5   , LCD_RAM_START_ADDRESS +  9, 0}, /* 25 - Decimal Point P2 */
	    {S_D4   , LCD_RAM_START_ADDRESS + 11 , 0}, /* 26 - Decimal Point P3 */
	    {S_D3   , LCD_RAM_START_ADDRESS + 20 , 3}, /* 27 - COLon*/
	    {S_D2   , LCD_RAM_START_ADDRESS + 22 , 1}, /* 28 - Decimal Point P4 */
	    {S_D1   , LCD_RAM_START_ADDRESS + 16 , 2}, /* 29 - COLON duplicate - assigned to H */
	    {S_T9   , LCD_RAM_START_ADDRESS + 22 , 3}, /* 30 - Assigned to BYPASS*/
	    {S_TOD  , LCD_RAM_START_ADDRESS + 13 , 0}, /* 31 */
	    {S_MD   , LCD_RAM_START_ADDRESS + 14 , 0}, /* 32 */
	};
#endif

#define _BV(i)	(1<<i)

/*
// The following is the pattern for the GD HI-TECH LCD drawing no DG10105-1
#define a _BV(3)
#define b _BV(7)
#define c _BV(5)
#define d _BV(0)
#define e _BV(1)
#define f _BV(2)
#define g _BV(6)
// Symbol bit is bit 4
*/

// The following is the nibble swapped pattern for the GD HI-TECH LCD drawing no DG10105-1
// This happens to be the correct implementation - for positions 1 to 7
#define a _BV(7)
#define b _BV(3)
#define c _BV(1)
#define d _BV(4)
#define e _BV(5)
#define f _BV(6)
#define g _BV(2)
// Symbol bit is bit 0

const LCD_NUM_TYPE_VALUE g_LCD_NumType[LCD_NUM_DIGIT_CHAR] =
{                               
    {(a|b|c|d|e|f)},	// 0
    {(b|c)},	// 1
    {(a|b|d|e|g)},	// 2
    {(a|b|c|d|g)},	// 3
    {(b|c|f|g)},	// 4
    {(a|c|d|f|g)},	// 5
    {(a|c|d|e|f|g)},	// 6
    {(a|b|c)},	// 7
    {(a|b|c|d|e|f|g)},	// 8
    {(a|b|c|d|f|g)},	// 9
    {(a|b|c|e|f|g)},	// A
    {(c|d|e|f|g)},	// B
    {(a|d|e|f)},	// C
    {(b|c|d|e|g)},	// D
    {(a|d|e|f|g)},	// E
    {(a|e|f|g)},	// F
    {(g)},	// '-'
    {(a|b|e|f|g)},	// P
    {(d|e|f)},	// L
    {(c|e|g)},	// n
    {0x00},	// blank
    {(b|c|d|e|f)},	// U
    {(c|e|f|g)}, // h
		{(d|e|f|g)},	// t
		{(a|b|c|d|f|g)}, // g
		{(a|c|e|g)}, // M
		{(c|d|e)},	// v
		{(e|g)},	// r
		{(b|c|d|f|g)}, // y
		{(e|f)}, //I
		{(c|d|e|g)}, //o
};

// This happens to be the correct implementation - for position 8
#define a8 _BV(0)
#define b8 _BV(5)
#define c8 _BV(7)
#define d8 _BV(3)
#define e8 _BV(2)
#define f8 _BV(1)
#define g8 _BV(6)
// MD Symbol bit is bit 4

const LCD_NUM_TYPE_VALUE g_LCD_NumType8[LCD_NUM_DIGIT_CHAR] =
{// character definitions for position 9                               
    {(a8|b8|c8|d8|e8|f8)},
    {(b8|c8)},
    {(a8|b8|d8|e8|g8)},
    {(a8|b8|c8|d8|g8)},
    {(b8|c8|f8|g8)},
    {(a8|c8|d8|f8|g8)},
    {(a8|c8|d8|e8|f8|g8)},
    {(a8|b8|c8)},
    {(a8|b8|c8|d8|e8|f8|g8)},
    {(a8|b8|c8|d8|f8|g8)},
    {(a8|b8|c8|e8|f8|g8)},
    {(c8|d8|e8|f8|g8)},
    {(a8|d8|e8|f8)},
    {(b8|c8|d8|e8|g8)},
    {(a8|d8|e8|f8|g8)},
    {(a8|e8|f8|g8)},
    {(g8)},
    {(a8|b8|e8|f8|g8)},
    {(d8|e8|f8)},
    {(c8|e8|g8)},	// n
    {0x00},
    {(b8|c8|d8|e8|f8)},
    {(b8|c8|e8|f8|g8)},
		{(d8|e8|f8|g8)},
		{(a8|b8|c8|d8|f8|g8)},
		{(a8|c8|e8|g8)},
		{(c8|d8|e8)},
		{(e8|g8)},
		{(b8|c8|d8|f8|g8)},
		{(e8|f8)}, //I
		{(c8|d8|e8|g8)}, //o
};


// This happens to be the correct implementation - for position 9
#define a9 _BV(0)
#define b9 _BV(4)
#define c9 _BV(7)
#define d9 _BV(3)
#define e9 _BV(2)
#define f9 _BV(1)
#define g9 _BV(5)
// H Symbol bit is bit 6

const LCD_NUM_TYPE_VALUE g_LCD_NumType9[LCD_NUM_DIGIT_CHAR] =
{// character definitions for position 9                               
    {(a9|b9|c9|d9|e9|f9)},
    {(b9|c9)},
    {(a9|b9|d9|e9|g9)},
    {(a9|b9|c9|d9|g9)},
    {(b9|c9|f9|g9)},
    {(a9|c9|d9|f9|g9)},
    {(a9|c9|d9|e9|f9|g9)},
    {(a9|b9|c9)},
    {(a9|b9|c9|d9|e9|f9|g9)},
    {(a9|b9|c9|d9|f9|g9)},
    {(a9|b9|c9|e9|f9|g9)},
    {(c9|d9|e9|f9|g9)},
    {(a9|d9|e9|f9)},
    {(b9|c9|d9|e9|g9)},
    {(a9|d9|e9|f9|g9)},
    {(a9|e9|f9|g9)},
    {(g9)},
    {(a9|b9|e9|f9|g9)},
    {(d9|e9|f9)},
    {(c9|e9|g9)},	// n
    {0x00},
    {(b9|c9|d9|e9|f9)},
    {(b9|c9|e9|f9|g9)},
		{(d9|e9|f9|g9)},
		{(a9|b9|c9|d9|f9|g9)},
		{(a9|c9|e9|g9)},
		{(c9|d9|e9)},
		{(e9|g9)},
		{(b9|c9|d9|f9|g9)},
		{(e9|f9)}, //I
		{(c9|d9|e9|g9)}, //o
};


const LCD_DECIMAL_INFO g_DecInfo  = {6, S_D5};
const LCD_DECIMAL_INFO g_DecInfo1 = {5, S_D4};
const LCD_DECIMAL_INFO g_DecInfo2 = {4, S_D6};

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
