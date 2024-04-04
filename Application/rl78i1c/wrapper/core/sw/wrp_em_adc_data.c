/***********************************************************************************************************************
* File Name    : wrp_em_adc_data.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : ADC Wrapper for RL78/I1C Platform
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "wrp_em_sw_config.h"   /* Wrapper Shared Configuration */
#include "wrp_em_adc.h"         /* Wrapper ADC definitions */
#include "wrp_em_adc_data.h"
#include "wrp_mcu.h"
#include "em_core.h"
#include "platform.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
#pragma section text    WrpAdcText
#pragma section const   WrpAdcConst
#ifdef __DEBUG
#pragma section bss     WrpAdcBss
#pragma section data    WrpAdcData
#endif

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Software delay for voltage channel */
#if (EM_ADC_DELAY_STEP_VOLTAGE > 0)

int16_t     g_wrp_adc_voltage_circle_pos; 
uint16_t    g_wrp_adc_voltage_circle_step;

int16_t g_wrp_adc_voltage_circle_phase_r[EM_ADC_DELAY_STEP_VOLTAGE+1];
int16_t g_wrp_adc_voltage_circle_phase_y[EM_ADC_DELAY_STEP_VOLTAGE+1];
int16_t g_wrp_adc_voltage_circle_phase_b[EM_ADC_DELAY_STEP_VOLTAGE+1];

#endif
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
#if (EM_ADC_DELAY_VOLTAGE90_MAX_STEP > 0)

int16_t     g_wrp_adc_voltage90_circle_pos; 
uint16_t    g_wrp_adc_voltage90_circle_step;

int16_t g_wrp_adc_voltage90_circle_phase_r[EM_ADC_DELAY_VOLTAGE90_MAX_STEP+1];
int16_t g_wrp_adc_voltage90_circle_phase_y[EM_ADC_DELAY_VOLTAGE90_MAX_STEP+1];
int16_t g_wrp_adc_voltage90_circle_phase_b[EM_ADC_DELAY_VOLTAGE90_MAX_STEP+1];

#endif
#endif

/* 16bit of voltage sample, no struct for optimization */
int16_t g_wrp_adc_buffer_phase_r_v;
int16_t g_wrp_adc_buffer_phase_y_v;
int16_t g_wrp_adc_buffer_phase_b_v;

int16_t g_wrp_adc_buffer_phase_r_v90;
int16_t g_wrp_adc_buffer_phase_y_v90;
int16_t g_wrp_adc_buffer_phase_b_v90;

int16_t g_wrp_adc_offset_phase_r_v;
int16_t g_wrp_adc_offset_phase_y_v;
int16_t g_wrp_adc_offset_phase_b_v;

/* Sample storage of wrapper */
EM_SAMPLES g_wrp_adc_samples;

#ifdef __DEBUG

uint16_t    g_sample_count = EM_ADC_MAX_DEBUG_SAMPLE;
const uint16_t g_sample_max_count = EM_ADC_MAX_DEBUG_SAMPLE;
int16_t g_sample_v90[EM_ADC_MAX_DEBUG_SAMPLE][3];
int16_t g_sample_adc[EM_ADC_MAX_DEBUG_SAMPLE][3];
int32_t g_sample_dsad[EM_ADC_MAX_DEBUG_SAMPLE][4];

#endif /* __DEBUG */

volatile uint8_t g_wrp_adc_tdr_ignore;

/* Gain Switch Wrapper */
const dsad_channel_t g_wrp_adc_channel[EM_NUM_OF_LINE] =
{
    EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R,
    EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y,
    EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B,
    EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL
};

uint8_t g_EM_ADC_GainPhaseCurrentLevel[EM_NUM_OF_PHASE];        /* The current level of phase gain */

const dsad_gain_t g_wrp_adc_channel_gain[EM_NUM_OF_PHASE][EM_GAIN_PHASE_NUM_LEVEL_MAX] = 
{
    {
        EM_ADC_GAIN_LEVEL0_PHASE_R,
        EM_ADC_GAIN_LEVEL1_PHASE_R,
    },
    {
        EM_ADC_GAIN_LEVEL0_PHASE_Y,
        EM_ADC_GAIN_LEVEL1_PHASE_Y,
    },
    {
        EM_ADC_GAIN_LEVEL0_PHASE_B,
        EM_ADC_GAIN_LEVEL1_PHASE_B,
    },
};


/* Reactive timer */
int16_t g_fsteps_timer_offset;

// very strange that this table is used instead of the formula
// column2 = int(61.4 * column1)
const TIMER_TABLE g_wrp_adc_timer_table[TIMER_TABLE_SIZE] = 
{
    { 10    ,   614     },
    { 11    ,   675     },
    { 12    ,   737     },
    { 13    ,   798     },
    { 14    ,   860     },
    { 15    ,   921     },
    { 16    ,   983     },
    { 17    ,   1044    },
    { 18    ,   1105    },
    { 19    ,   1167    },
    { 20    ,   1228    },
    { 21    ,   1290    },
    { 22    ,   1351    },
    { 23    ,   1413    },
    { 24    ,   1474    },
    { 25    ,   1536    },
    { 26    ,   1597    },
    { 27    ,   1658    },
    { 28    ,   1720    },
    { 29    ,   1781    },
    { 30    ,   1843    },
    { 31    ,   1904    },
    { 32    ,   1966    },
    { 33    ,   2027    },
    { 34    ,   2088    },
    { 35    ,   2150    },
    { 36    ,   2211    },
    { 37    ,   2273    },
    { 38    ,   2334    },
    { 39    ,   2396    },
    { 40    ,   2457    },
    { 41    ,   2519    },
    { 42    ,   2580    },
    { 43    ,   2641    },
    { 44    ,   2703    },
    { 45    ,   2764    },
    { 46    ,   2826    },
    { 47    ,   2887    },
    { 48    ,   2949    },
    { 49    ,   3010    },
    { 50    ,   3072    },
    { 51    ,   3133    },
    { 52    ,   3194    },
    { 53    ,   3256    },
    { 54    ,   3317    },
    { 55    ,   3379    },
    { 56    ,   3440    },
    { 57    ,   3502    },
    { 58    ,   3563    },
    { 59    ,   3624    },
    { 60    ,   3686    },
    { 61    ,   3747    },
    { 62    ,   3809    },
    { 63    ,   3870    },
    { 64    ,   3932    },
    { 65    ,   3993    },
    { 66    ,   4055    },
    { 67    ,   4116    },
    { 68    ,   4177    },
    { 69    ,   4239    },
    { 70    ,   4300    },
    { 71    ,   4362    },
    { 72    ,   4423    },
    { 73    ,   4485    },
    { 74    ,   4546    },
    { 75    ,   4608    },
    { 76    ,   4669    },
    { 77    ,   4730    },
    { 78    ,   4792    },
    { 79    ,   4853    },
    { 80    ,   4915    },
    { 81    ,   4976    },
    { 82    ,   5038    },
    { 83    ,   5099    },
    { 84    ,   5160    },
    { 85    ,   5222    },
    { 86    ,   5283    },
    { 87    ,   5345    },
    { 88    ,   5406    },
    { 89    ,   5468    },
    { 90    ,   5529    },

};


/******************************************************************************
* Function Name    : int16_t EM_ADC_SAMP_GetTimerOffset(void)
* Description      : Get value of quarter shift timer offset value
* Arguments        : None
* Functions Called : None
* Return Value     : int16_t: value of timer offset
******************************************************************************/
int16_t EM_ADC_SAMP_GetTimerOffset(void)
{
    return g_fsteps_timer_offset;   
}

/******************************************************************************
* Function Name    : int16_t EM_ADC_SAMP_SetTimerOffset(void)
* Description      : Set value to quarter shift timer offset value
* Arguments        : int16_t value: value of timer offset
* Functions Called : None
* Return Value     : 
******************************************************************************/
void EM_ADC_SAMP_SetTimerOffset(int16_t value)
{
    g_fsteps_timer_offset = value;
}