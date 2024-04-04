#include "my_macrodriver.h"
#include "My_cg_timer.h"
#include "my_userdefine.h"

extern _Bool DelayTimerUnderFlowFlag;

void R_TAU0_Create(void)
{
    TAU0RES = 1U;
    TAU0RES = 0U;
    TAU0EN = 1U; 
    TPS0 = _0000_TAU_CKM3_fCLK_8 | _0000_TAU_CKM2_fCLK_1 | _0000_TAU_CKM1_fCLK_0 | _0007_TAU_CKM0_fCLK_7;
    // Stop all channels
    TT0 = _0800_TAU_CH3_H8_STOP_TRG_ON | _0200_TAU_CH1_H8_STOP_TRG_ON | _0008_TAU_CH3_STOP_TRG_ON | 
          _0004_TAU_CH2_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0001_TAU_CH0_STOP_TRG_ON;
    TMMK00 = 1U;    
    TMIF00 = 0U;    
    TMMK01 = 1U;    
    TMIF01 = 0U;    
    TMMK01H = 1U;   
    TMIF01H = 0U;   
    TMMK02 = 1U;    
    TMIF02 = 0U;    
    TMMK03 = 1U;    
    TMIF03 = 0U;    
    TMMK03H = 1U;   
    TMIF03H = 0U;   
    TMMK04 = 1U;    
    TMIF04 = 0U;    
    TMMK05 = 1U;    
    TMIF05 = 0U;    
    TMMK06 = 1U;    
    TMIF06 = 0U;    
    TMMK07 = 1U;    
    TMIF07 = 0U;    
    // Set INTTM00 low priority 
    TMPR100 = 1U;
    TMPR000 = 1U;
    // Channel 0 used as interval timer
    TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_TRIGGER_SOFTWARE | 
            _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
    TDR00 = _B71A_TAU_TDR00_VALUE;
    TOM0 &= (uint16_t)~_0001_TAU_CH0_SLAVE_OUTPUT;
    TO0 &= (uint16_t)~_0001_TAU_CH0_OUTPUT_VALUE_1;
    TOE0 &= (uint16_t)~_0001_TAU_CH0_OUTPUT_ENABLE;
}

void R_TAU0_Channel0_Start(void)
{
    DelayTimerUnderFlowFlag = 0;
    TMIF00 = 0U;
    TMMK00 = 0U;
    TS0 |= _0001_TAU_CH0_START_TRG_ON;
}

void R_TAU0_Channel0_Stop(void)
{
    TT0 |= _0001_TAU_CH0_STOP_TRG_ON;
    TMMK00 = 1U;
    TMIF00 = 0U;
}

