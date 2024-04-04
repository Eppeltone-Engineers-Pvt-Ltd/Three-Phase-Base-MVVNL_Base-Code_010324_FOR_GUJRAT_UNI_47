#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_lcd.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/* Start user code for global. Do not edit comment generated here */
#ifdef LCD_VOLTAGE_WAITTIME
    #undef  LCD_VOLTAGE_WAITTIME
    #define LCD_VOLTAGE_WAITTIME    100
#endif
/* End user code. Do not edit comment generated here */

void R_LCD_Create(void)
{
    volatile uint32_t wt_count;
    
    OSMC |= 0x80;  /* Supply lock to LCD */
    LCDON = 0U;    /* disable LCD clock operation */
    LCDM1 |= _00_LCD_VOLTAGE_HIGH;
    LCDM0 = _00_LCD_DISPLAY_WAVEFORM_A | _0D_LCD_DISPLAY_MODE1;
    LCDM0 |= _40_LCD_VOLTAGE_MODE_INTERNAL;
    /* Set CAPL and CAPH pins */
    ISCLCD &= (uint8_t)~_01_LCD_CAPLH_BUFFER_VALID;
    /* Set VL3 */
    ISCLCD |= _02_LCD_VL3_BUFFER_VALID;
    
		WUTMMCK0 = 1;	// Use Fil	- added on 04/Sep/2020
		
    LCDM1 |= _00_LCD_DISPLAY_PATTERN_A;
    LCDC0 = _00_LCD_SOURCE_CLOCK_FSUB | _04_LCD_CLOCK_FLCD_32;
//    VLCD = _09_LCD_BOOST_VOLTAGE_125V;
//    VLCD = _04_LCD_BOOST_VOLTAGE_100V;	// ok
//    VLCD = _0C_LCD_BOOST_VOLTAGE_140V;	// bad
//    VLCD = _06_LCD_BOOST_VOLTAGE_110V;	// 
    VLCD = _07_LCD_BOOST_VOLTAGE_115V;	// 

    /* Change the waiting time according to the system */
    for (wt_count = 0U; wt_count <= LCD_REFVOLTAGE_WAITTIME; wt_count++)
    {
        NOP();
    }
}

void R_LCD_Start(void)
{
    LCDON = 1U;
}

void R_LCD_Stop(void)
{
    LCDON = 0U;
}

void R_LCD_Set_VoltageOn(void)
{
    volatile uint32_t wt_count;
    
    LCDM0 |= _40_LCD_VOLTAGE_MODE_INTERNAL;
    
    VLCON = 1U;

    /* Delay for 500ms. Under fsub or fIH 6MHz with DSAD interrupt */

    for(wt_count = 0U; wt_count <= LCD_VOLTAGE_WAITTIME; wt_count++)
    {
        NOP();
    }
    
    SCOC = 1U;
}

void R_LCD_Set_VoltageOff(void)
{
    SCOC = 0U;
    VLCON = 0U;
    LCDM0 &= (uint8_t)~_C0_LCD_VOLTAGE_MODE_INITIALVALUE;
}

void R_LCD_PowerOff(void)
{
    R_LCD_Stop();
    
    R_LCD_Set_VoltageOff();
}

void R_LCD_PowerOn(void)
{
    R_LCD_Start();
    R_LCD_Set_VoltageOn();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
