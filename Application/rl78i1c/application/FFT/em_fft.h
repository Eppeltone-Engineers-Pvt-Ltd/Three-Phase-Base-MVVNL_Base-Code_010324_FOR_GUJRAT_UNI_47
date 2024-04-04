/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#ifndef _EM_FFT_H
#define _EM_FFT_H

/***********************************************************************************************************************
* File Name    : em_type.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Core Type Definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
//#include "em_type.h"
#include "r_fft_int16.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define EM_FFT_ENABLE
/* The number of input points */
#define FFT_POINTS RFFT512P

/* Define the number data of harmonic output table*/
#define EM_FFT_MAX_HARMONIC 32

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum em_fft_counter_tag
{
    NONE = 0,       /* Initial state */
    FFT_PHASE_R_V,
    FFT_PHASE_Y_V,
    FFT_PHASE_B_V,
    
    FFT_PHASE_R_I,
    FFT_PHASE_Y_I,
    FFT_PHASE_B_I,
    
    CURRENT_NEUTRAL,
    
    DEBUG_CHANNEL   /* Currenly, Not use , for Debug purpose only */
    
} EM_FFT_CHANNEL;

typedef struct tagEMHarmonic
{
    /* Phase_R */
    struct
    {
        int8_t     i[EM_FFT_MAX_HARMONIC];              /* Current */
        int8_t     v[EM_FFT_MAX_HARMONIC];              /* Voltage */
    } phase_r;
    
    /* Phase_Y */
    struct
    {
        int8_t     i[EM_FFT_MAX_HARMONIC];              /* Current */
        int8_t     v[EM_FFT_MAX_HARMONIC];              /* Voltage */
        
    } phase_y;
    
    /* Phase_B */
    struct
    {
        int8_t     i[EM_FFT_MAX_HARMONIC];              /* Current */
        int8_t     v[EM_FFT_MAX_HARMONIC];              /* Voltage */
        
    } phase_b;
    
    /* Status info */
    struct
    {
        int8_t                block;
        int8_t             reserved;        /* Reserved */
    } status;
    
} EM_FFT_HARMONIC;

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
uint16_t getFrequency(void);

__near EM_FFT_HARMONIC * EM_FFT_GetHarmonic(void);

void EM_FFT_PollingProcessing(void);

void EM_FFT_AddSample(void);

extern EM_FFT_HARMONIC em_fft_harmonic;

#ifdef __DEBUG
extern uint16_t em_fft_dump_buffer;
extern R_FFT_INDATA em_fft_buffer[FFT_POINTS];
#endif /* DEBUG */

#endif /*define _EM_FFT_H */

