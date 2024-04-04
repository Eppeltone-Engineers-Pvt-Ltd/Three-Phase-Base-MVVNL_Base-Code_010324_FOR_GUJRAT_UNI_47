#include "my_macrodriver.h"
#include "my_cgc.h"
#include "my_userdefine.h"

void MY_CGC_Create(void)
{
	volatile uint32_t w_count;

	CMC = 0x11;//as per Application code
	MSTOP = 1U;
	MCM0 = 0U;
	XTSTOP = 0U;
	for (w_count = 0U; w_count <= 30; w_count++)
	{
	    NOP();
	}

	OSMC = 0x00;//_10_CGC_RTC_IT_LCD_CLK_FIL;
	CSS = 0U;
	HIOSTOP = 0U; 
	CKS0 = 0x08;
	P4 &= 0xF7U;
	PM4 &= 0xF7U;
	PCLOE0 = 1U;
	// for 6 MHz on Chip Main Clock. 	
}
