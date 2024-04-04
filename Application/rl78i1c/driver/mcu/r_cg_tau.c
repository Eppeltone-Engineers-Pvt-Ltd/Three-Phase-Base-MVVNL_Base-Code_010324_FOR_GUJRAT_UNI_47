/***********************************************************************************************************************
* File Name    : r_cg_tau.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for TAU module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TAU0_Create
* Description  : This function initializes the TAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Create(void)
{
    TAU0EN = 1U;    /* supplies input clock */
    TPS0 = _0000_TAU_CKM0_fCLK_0 | _0040_TAU_CKM1_fCLK_4 | _0000_TAU_CKM2_fCLK_1 | _0000_TAU_CKM3_fCLK_8;
    /* Stop all channels */
    TT0 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON | 
          _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON | 
          _0040_TAU_CH6_STOP_TRG_ON | _0080_TAU_CH7_STOP_TRG_ON | _0200_TAU_CH1_H8_STOP_TRG_ON | 
          _0800_TAU_CH3_H8_STOP_TRG_ON;
    /* Mask channel 0 interrupt */
    TMMK00 = 1U;    /* disable INTTM00 interrupt */
    TMIF00 = 0U;    /* clear INTTM00 interrupt flag */
    /* Mask channel 1 interrupt */
    TMMK01 = 1U;    /* disable INTTM01 interrupt */
    TMIF01 = 0U;    /* clear INTTM01 interrupt flag */
    /* Mask channel 1 higher 8 bits interrupt */
    TMMK01H = 1U;    /* disable INTTM01H interrupt */
    TMIF01H = 0U;    /* clear INTTM01H interrupt flag */
    /* Mask channel 2 interrupt */
    TMMK02 = 1U;    /* disable INTTM02 interrupt */
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
    /* Mask channel 3 interrupt */
    TMMK03 = 1U;    /* disable INTTM03 interrupt */
    TMIF03 = 0U;    /* clear INTTM03 interrupt flag */
    /* Mask channel 3 higher 8 bits interrupt */
    TMMK03H = 1U;    /* disable INTTM03H interrupt */
    TMIF03H = 0U;    /* clear INTTM03H interrupt flag */
    /* Mask channel 4 interrupt */
    TMMK04 = 1U;    /* disable INTTM04 interrupt */
    TMIF04 = 0U;    /* clear INTTM04 interrupt flag */
    /* Mask channel 5 interrupt */
    TMMK05 = 1U;    /* disable INTTM05 interrupt */
    TMIF05 = 0U;    /* clear INTTM05 interrupt flag */
    /* Mask channel 6 interrupt */
    TMMK06 = 1U;    /* disable INTTM06 interrupt */
    TMIF06 = 0U;    /* clear INTTM06 interrupt flag */
    /* Mask channel 7 interrupt */
    TMMK07 = 1U;    /* disable INTTM07 interrupt */
    TMIF07 = 0U;    /* clear INTTM07 interrupt flag */
    /* Set INTTM00 low priority */
    TMPR100 = 1U;
    TMPR000 = 1U;
    /* Set INTTM01 low priority */
    TMPR101 = 1U;
    TMPR001 = 1U;
    /* Set INTTM02 low priority */
    TMPR102 = 1U;
    TMPR002 = 0U;
    
    /* Channel 0 used as interval timer */
    TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
    TDR00 = _5DBF_TAU_TDR00_VALUE;
    TO0 &= ~_0001_TAU_CH0_OUTPUT_VALUE_1;
    TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;       
    
    /* Channel 1 used as one shot counter */
    TMR01 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0000_TAU_TRIGGER_SOFTWARE | _0008_TAU_MODE_ONE_COUNT | _0000_TAU_START_INT_UNUSED;
    TDR01 = _0CB0_TAU_TDR01_VALUE;
    TO0 &= ~_0002_TAU_CH1_OUTPUT_VALUE_1;
    TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;

    /* Channel 2 used as interval timer */
    TMR02 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
    TDR02 = _EA5F_TAU_TDR02_VALUE; // original
//    TDR02 = (0xE9ABU);	// 0.3% fast
//    TDR02 = (0xE755U);	// used since 29/06/2020
    TOM0 &= ~_0004_TAU_CH2_OUTPUT_COMBIN;
    TOL0 &= ~_0004_TAU_CH2_OUTPUT_LEVEL_L;
    TO0 &= ~_0004_TAU_CH2_OUTPUT_VALUE_1;
    TOE0 &= ~_0004_TAU_CH2_OUTPUT_ENABLE;
    
		 /* Channel 3 used as interval timer */
    TMR03 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
            _0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
//    TDR03 = _EA5F_TAU_TDR02_VALUE;	// original value
//    TDR03 = (0xE9ABU);	// used till 28/06/2020
//    TDR03 = (0xE9ABU);	// used till 28/06/2020
//    TDR03 = (0xE755U);	// used since 29/06/2020
    TDR03 = _EA5F_TAU_TDR02_VALUE;	// original value restored on 29/06/2020 - 14:05
		
// Above value will speed up the 40 ms timer interrupt by 0.3% which will cause the kwhregister to accumulate a higher
// value in the actual time interval of 1 second - This is to account for slippage that occurs due to too many calculations
// Not a very good thing - at best an engineering compromise - 01/Jan/2020
    TOM0 &= ~0x0008;//_0008_TAU_CH3_OUTPUT_COMBIN;
    TOL0 &= ~_0008_TAU_CH3_OUTPUT_LEVEL_L;
    TO0 &= ~_0008_TAU_CH3_OUTPUT_VALUE_1;
    TOE0 &= ~_0008_TAU_CH3_OUTPUT_ENABLE;
    
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Start
* Description  : This function starts TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Start(void)
{
    TMIF00 = 0U;    /* clear INTTM00 interrupt flag */
    TMMK00 = 0U;    /* enable INTTM00 interrupt */
    TS0 |= _0001_TAU_CH0_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Stop
* Description  : This function stops TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Stop(void)
{
    /* Mask channel 0 interrupt */
    TMMK00 = 1U;    /* disable INTTM00 interrupt */
    TMIF00 = 0U;    /* clear INTTM00 interrupt flag */
    TT0 |= _0001_TAU_CH0_STOP_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Start
* Description  : This function starts TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Start(void)
{
    TS0 |= _0002_TAU_CH1_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Stop
* Description  : This function stops TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Stop(void)
{
    TT0 |= _0002_TAU_CH1_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel2_Start
* Description  : This function starts TAU0 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel2_Start(void)
{
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
    TMMK02 = 0U;    /* enable INTTM02 interrupt */
    TS0 |= _0004_TAU_CH2_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel2_Stop
* Description  : This function stops TAU0 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel2_Stop(void)
{
    TT0 |= _0004_TAU_CH2_STOP_TRG_ON;
    /* Mask channel 2 interrupt */
    TMMK02 = 1U;    /* disable INTTM02 interrupt */
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_SetValue
* Description  : This function set value into TDR01.
* Arguments    : uint16_t reg_value: value of TDR01
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_SetValue(uint16_t reg_value)
{
    TDR01 = reg_value;
}



/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Start
* Description  : This function starts TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Start(void)
{
    TMIF03 = 0U;    /* clear INTTM03 interrupt flag */
    TMMK03 = 0U;    /* enable INTTM03 interrupt */
    TS0 |= _0008_TAU_CH3_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Stop
* Description  : This function stops TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Stop(void)
{
    TT0 |= _0008_TAU_CH3_STOP_TRG_ON;
    /* Mask channel 3 interrupt */
    TMMK03 = 1U;    /* disable INTTM03 interrupt */
    TMIF03 = 0U;    /* clear INTTM03 interrupt flag */
}

/* End user code. Do not edit comment generated here */
