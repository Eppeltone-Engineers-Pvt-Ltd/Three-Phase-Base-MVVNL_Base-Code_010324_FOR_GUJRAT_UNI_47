/***********************************************************************************************************************
* File Name    : wrp_em_adc_data.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : ADC Wrapper for RL78/I1C Platform
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_ADC_DATA_H
#define _WRAPPER_EM_ADC_DATA_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"            /* GSCE Standard Typedef */
#include "em_type.h"            /* EM Core Type Definitions */
#include "wrp_em_sw_config.h"
#include "platform.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    int16_t     pos;        /* Position */
    uint16_t    step;       /* Step */
    
    int16_t     circle_phase_r[EM_ADC_DELAY_STEP_VOLTAGE+1];
    int16_t     circle_phase_y[EM_ADC_DELAY_STEP_VOLTAGE+1];
    int16_t     circle_phase_b[EM_ADC_DELAY_STEP_VOLTAGE+1];

} sw_delay_voltage;

typedef struct
{
    int16_t     pos;        /* Position */
    uint16_t    step;       /* Step */
    
    int16_t     circle_phase_r[EM_ADC_DELAY_VOLTAGE90_MAX_STEP+1];
    int16_t     circle_phase_y[EM_ADC_DELAY_VOLTAGE90_MAX_STEP+1];
    int16_t     circle_phase_b[EM_ADC_DELAY_VOLTAGE90_MAX_STEP+1];

} sw_delay_voltage90;

typedef struct tagEMTimerTable
{
    uint16_t    fsteps_x100;            /* x100 scaled fractional steps */
    uint16_t    timer_value;            /* pre-calculated timer value */
} TIMER_TABLE;

typedef struct tagEM
{
    /* Current signal */
    struct
    {
        EM_SW_SAMP_TYPE i;
        int16_t v;
        int16_t v90;
    } phase_r;                      /* Phase_R */
    
    struct
    {
        EM_SW_SAMP_TYPE i;
        int16_t v;
        int16_t v90;
    } phase_y;                      /* Phase_Y */
    
    struct
    {
        EM_SW_SAMP_TYPE i;
        int16_t v;
        int16_t v90;
    } phase_b;                      /* Phase_B */
    
    struct 
    {
        EM_SW_SAMP_TYPE i;
    } neutral;                      /* Neutral */
     
} WRP_ADC_SAMPLE;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define TIMER_TABLE_SIZE                (81)

#ifdef __DEBUG
#define EM_ADC_MAX_DEBUG_SAMPLE         (140)
#endif

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/
/* Import from wrp_em_adc_data.c */
#if (EM_ADC_DELAY_STEP_VOLTAGE > 0)

extern int16_t     g_wrp_adc_voltage_circle_pos; 
extern uint16_t    g_wrp_adc_voltage_circle_step;

extern int16_t g_wrp_adc_voltage_circle_phase_r[];
extern int16_t g_wrp_adc_voltage_circle_phase_y[];
extern int16_t g_wrp_adc_voltage_circle_phase_b[];

#endif

#if (EM_ADC_DELAY_VOLTAGE90_MAX_STEP > 0)

extern int16_t     g_wrp_adc_voltage90_circle_pos; 
extern uint16_t    g_wrp_adc_voltage90_circle_step;

#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
extern int16_t g_wrp_adc_voltage90_circle_phase_r[];
extern int16_t g_wrp_adc_voltage90_circle_phase_y[];
extern int16_t g_wrp_adc_voltage90_circle_phase_b[];
#endif

#endif

extern int16_t g_wrp_adc_buffer_phase_r_v;
extern int16_t g_wrp_adc_buffer_phase_y_v;
extern int16_t g_wrp_adc_buffer_phase_b_v;

extern int16_t g_wrp_adc_buffer_phase_r_v90;
extern int16_t g_wrp_adc_buffer_phase_y_v90;
extern int16_t g_wrp_adc_buffer_phase_b_v90;

extern int16_t g_wrp_adc_offset_phase_r_v;
extern int16_t g_wrp_adc_offset_phase_y_v;
extern int16_t g_wrp_adc_offset_phase_b_v;

extern EM_SAMPLES g_wrp_adc_samples;

#ifdef __DEBUG

extern uint16_t g_sample_count;
extern const uint16_t g_sample_max_count;
extern int16_t g_sample_v90[][3];
extern int16_t g_sample_adc[][3];
extern int32_t g_sample_dsad[][4];

#endif /* __DEBUG */

extern volatile uint8_t g_wrp_adc_tdr_ignore;
extern int16_t g_fsteps_timer_offset;

extern const TIMER_TABLE    g_wrp_adc_timer_table[TIMER_TABLE_SIZE]; 
extern uint8_t              g_EM_ADC_GainPhaseCurrentLevel[];
extern const dsad_channel_t g_wrp_adc_channel[];
extern const dsad_gain_t    g_wrp_adc_channel_gain[][EM_GAIN_PHASE_NUM_LEVEL_MAX];

#define TIMER_TABLE_OFFSET      (g_wrp_adc_timer_table[0].fsteps_x100)

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

int16_t EM_ADC_SAMP_GetTimerOffset(void);
void EM_ADC_SAMP_SetTimerOffset(int16_t value);

#endif /* _WRAPPER_EM_ADC_DATA_H */

