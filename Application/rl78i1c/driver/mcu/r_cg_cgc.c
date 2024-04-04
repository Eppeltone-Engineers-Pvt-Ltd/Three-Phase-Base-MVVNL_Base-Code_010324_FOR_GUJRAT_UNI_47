#include "r_cg_macrodriver.h"
#include "iodefine.h"
#include "r_cg_cgc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"


void R_CGC_Create(void)
{
    volatile uint16_t w_count;

    /* Can only set once */
//    CMC = _40_CGC_HISYS_OSC | _10_CGC_XT1_SELECT_ENA | _01_CGC_SYSOSC_OVER10M ;     

// this has already been set in bootloader code
//#ifdef NO_BOOT_CODE 
    CMC = _10_CGC_XT1_SELECT_ENA | _01_CGC_SYSOSC_OVER10M ;     
//#endif    
    /* Supply clock to access RTC domain register */
    VRTCEN = 1U;
    SCMC = _10_CGC_SUB_OSC | _02_CGC_SUBMODE_NORMAL;
    
    XTSTOP = 0U;
    
    /* Waiting time for sub clock stabilization */
    /* XT1 */
    for (w_count = 0U; w_count <= CGC_SUBWAITTIME; w_count++)
    {
        NOP();
    }
    
    /* Enable XT1 clock for CPU */
    XT1SELDIS = 0U;
    
    /* Turn on fIH */
    HIOSTOP = 0;

    /* Select fMAIN as fCLK */
    CSS = 0U;
    
    /* Select fOCO as fMAIN */
    MCM0 = 0;
    
    /* Select fIH as fOCO */
    MCM1 = 0;
    
    /* Set the fIH division */
    HOCODIV = 0;
//    HOCODIV = 4;
//    HOCODIV = 2;
//    HOCODIV = 1;
    
    VRTCEN = 0U;
}

/*
void R_CGC_Create(void)
{
//	volatile uint32_t w_count;
	
    volatile uint16_t w_count;

    // Can only set once 
//    CMC = _40_CGC_HISYS_OSC | _10_CGC_XT1_SELECT_ENA | _01_CGC_SYSOSC_OVER10M ;	// original     
    CMC = _10_CGC_XT1_SELECT_ENA;	// we are not using external clock/crystal at X1 - however XT1 selection as CPU clock is permitted      
	MSTOP = 1U;	// X1 oscillator/external clock stopped
	
	MIOEN=0;	// middle-speed on-chip oscillator stopped 
	
    // Supply clock to access RTC domain register 
    VRTCEN = 1U;
    SCMC = _10_CGC_SUB_OSC | _02_CGC_SUBMODE_NORMAL;
    
    XTSTOP = 0U;
    
    // Waiting time for sub clock stabilization 
    // XT1 
    for (w_count = 0U; w_count <= CGC_SUBWAITTIME; w_count++)
    {
        NOP();
    }
    
    // Enable XT1 clock for CPU //
    XT1SELDIS = 0U;
    
    // Turn on fIH //
    HIOSTOP = 0;

    // Select fMAIN as fCLK //
    CSS = 0U;
    
    // Select fOCO as fMAIN //
    MCM0 = 0;
    
    // Select fIH as fOCO //
    MCM1 = 0;
    
    // Set the fIH division //
    HOCODIV = 0;
    
    VRTCEN = 0U;

}
*/
/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
