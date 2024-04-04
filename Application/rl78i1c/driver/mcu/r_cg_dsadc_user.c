#include "iodefine.h"

#ifdef NO_BOOT_CODE
#pragma interrupt INTDSADZC0 r_dsadzc0_interrupt
#pragma interrupt INTDSADZC1 r_dsadzc1_interrupt
#endif
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#include "r_cg_macrodriver.h"
#include "r_cg_dsadc.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_em_adc.h"
#include "platform.h"

#ifdef METER_ENABLE_MEASURE_CPU_LOAD
#include "load_test.h"
#endif
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "appInterrupts.h"

/* Start user code for global. Do not edit comment generated here */
#ifdef METER_ENABLE_MEASURE_CPU_LOAD
#define DSAD_MEASUREMENT_TIME   (65535) 
uint16_t g_timer0_dsad_counter = 0;

uint16_t g_timer0_dsad_maxcounter = 0;
uint16_t g_timer0_dsad_mincounter = 0;

uint16_t g_dsad_count = DSAD_MEASUREMENT_TIME;
const uint16_t g_dsad_max_count = DSAD_MEASUREMENT_TIME;

uint32_t g_timer0_dsad_sum_counter = 0;

uint16_t g_timer0_diff = 0;
#endif

/* End user code. Do not edit comment generated here */

void r_dsadc_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    #ifdef METER_ENABLE_MEASURE_CPU_LOAD
        if (g_dsad_count < g_dsad_max_count)
        {
            g_dsad_count++;
            LOADTEST_TAU_Start();
        }
    #endif
    /*
     * Sampling chain
     * 1. Start -> DSAD
     * 2. (trigger) -> ELC
     * 3. (ELC) -> SAR-ADC (CH2-CH5)
     * 4. (ADC) -> DTC transfer
     * 5. (DTC Transfer End) -> ADC interrupt
     */
    EM_ADC_InterruptCallback();
        
    /*********************************************************************************************
     * END MAIN PROCESSING
    ********************************************************************************************/  

    #ifdef METER_ENABLE_MEASURE_CPU_LOAD
    g_timer0_diff = LOADTEST_TAU_Diff();
    
    LOADTEST_TAU_Stop();
    
    g_timer0_dsad_sum_counter += g_timer0_diff;
    
    if (g_dsad_count <=  g_dsad_max_count)
    {
        g_timer0_dsad_counter = g_timer0_diff;
        
        if ( g_timer0_dsad_counter > g_timer0_dsad_maxcounter)
        {
            g_timer0_dsad_maxcounter = g_timer0_dsad_counter;
        }
        
        if ( g_timer0_dsad_counter < g_timer0_dsad_mincounter)
        {
            g_timer0_dsad_mincounter = g_timer0_dsad_counter;
        }
    }
        
    #endif
    /* End user code. Do not edit comment generated here */
}

//__interrupt static void r_dsadzc0_interrupt(void)
void r_dsadzc0_interrupt(void)
{
    DSADICLR |= _01_DSAD_ZC0_FLAG_CLEAR;
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

//__interrupt static void r_dsadzc1_interrupt(void)
void r_dsadzc1_interrupt(void)
{
    DSADICLR |= _10_DSAD_ZC1_FLAG_CLEAR;
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
