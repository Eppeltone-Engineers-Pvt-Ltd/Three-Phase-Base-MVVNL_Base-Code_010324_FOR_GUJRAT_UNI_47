/***********************************************************************************************************************
* File Name    : wrp_em_adc.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : ADC Wrapper for RL78/I1C Platform
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_ADC_H
#define _WRAPPER_EM_ADC_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */
#include "em_type.h"        /* EM Core Type Definitions */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* ADC Control */
void EM_ADC_Init(void);                                 /* ADC Initialization              */
void EM_ADC_Start(void);                                /* ADC Start                       */
void EM_ADC_Stop(void);                                 /* ADC Stop                        */

/* ADC Get Samples (V, V90, Ip, In)     */
void EM_ADC_GetSamples(__near EM_SAMPLES * p_sample);

/* Phase Gain Control */
void EM_ADC_GainPhaseReset(EM_LINE line);                       /* Reset phase gain to lowest      */
void EM_ADC_GainPhaseIncrease(EM_LINE line);                    /* Increase phase gain 1 level     */
void EM_ADC_GainPhaseDecrease(EM_LINE line);                    /* Decrease phase gain 1 level     */
uint8_t EM_ADC_GainPhaseGetLevel(EM_LINE line);                 /* Get current phase gain level    */

/*
 * ADC Set Phase Correction & Gain
 * The degree must be converted into steps then
 * setup into the ADC driver by this API.
 *
 * The base signal is Current.
 *  . degree < 0 --> delay the current
 *  . degree > 0 --> delay the voltage
 */
void EM_ADC_SetPhaseCorrection_Phase_R(float32_t degree);
void EM_ADC_SetPhaseCorrection_Phase_Y(float32_t degree);
void EM_ADC_SetPhaseCorrection_Phase_B(float32_t degree);

void EM_ADC_SAMP_UpdateStep(float32_t fac);

/* ADC Interval Processing, to be registered in wrapper layer */
void EM_ADC_IntervalProcessing(__near EM_SAMPLES * p_samples);  /* ADC Interrupt Callback Processing */
void EM_ADC_InterruptCallback(void);
#endif /* _WRAPPER_EM_ADC_H */

