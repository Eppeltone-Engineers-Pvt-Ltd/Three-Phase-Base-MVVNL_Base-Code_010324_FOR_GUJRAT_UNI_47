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

/***********************************************************************************************************************
* File Name    : em_fft.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Platform Property Source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

#include "r_cg_macrodriver.h"
#include "wrp_em_adc_data.h"
#include "em_measurement.h"

#include "em_fft.h"

#include "r_fft_int16.h"  /* Platform Property header file */
#include "math.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifdef EM_FFT_ENABLE

/* The decimal point output. Please check FFT Librabry User's manual */
#if (FFT_POINTS == RFFT512P)
    #define RSLT_DECIMAL_POINT (6)      /* The decimal point */
#else
    #define RSLT_DECIMAL_POINT (7)
#endif   
    
#define RSLT_DECIMAL_POINT_HARMONIC_V   (8)
#define RSLT_DECIMAL_POINT_HARMONIC_I   (7)

#define EM_FFT_FREQ_MIN_50_HZ   (48.0)
#define EM_FFT_FREQ_MAX_50_HZ   (52.0)
#define EM_FFT_FREQ_MIN_60_HZ   (58.0)
#define EM_FFT_FREQ_MAX_60_HZ   (62.0)
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/* Input Data                                                                     
   the position of the decimal point of input data is between bit 15 and bit 14:   
                                                                                 
   bit 15 -++- bit 14                                                            
       MSB||                           LSB                                       
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                                          
      |S| | | | | | | | | | | | | | | | -- int16_t                               
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                                           
          .                                                                       
          |                                                                       
          +-- the decimal point is here
*/
R_FFT_INDATA em_fft_buffer[FFT_POINTS];

/* OUTPUT DATA */
EM_FFT_HARMONIC em_fft_harmonic;

#ifdef __DEBUG
uint16_t em_fft_dump_buffer = 1;
#endif /* DEBUG */

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/* FFT lookup table for 512P to 64P, fundamental is 50Hz */
const uint16_t fft_tbl_50_Hz[EM_FFT_MAX_HARMONIC] = 
{
    #if (FFT_POINTS == RFFT512P)
    #include "r_fft_tbl_50Hz_512p.h"
    #endif /* 50Hz 512p */
    
    #if (FFT_POINTS == RFFT256P)
    #include "r_fft_tbl_50Hz_256p.h"
    #endif /* 50Hz 256p */
};

const uint16_t fft_tbl_60_Hz[EM_FFT_MAX_HARMONIC] = 
{
    #if (FFT_POINTS == RFFT512P)
    #include "r_fft_tbl_60Hz_512p.h"
    #endif /* 60Hz 512p */
    
    #if (FFT_POINTS == RFFT256P)
    #include "r_fft_tbl_60Hz_256p.h"
    #endif /* 60Hz 256p */
};

uint16_t *fft_tbl = (uint16_t *)fft_tbl_50_Hz;

R_FFT_INDATA *em_fft_input = em_fft_buffer;

/* Output Data
 If FFT point is 256.
 the position of the decimal point of output data is between bit 7 and bit 6:     
                                                                                  
               bit 7 -++- bit 6                                                   
    MSB               ||           LSB                                            
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                                             
    |S| | | | | | | | | | | | | | | | -- int16_t                                  
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                                             
                      .                                                           
                      |                                                           
                      +-- the decimal point is here                               
                                                                                  
  If FFT point is 128.                                                            
  the position of the decimal point of output data is between bit 8 and bit 7     
*/
R_FFT_OUTDATA *em_fft_output = em_fft_buffer;

/* Window coefficients [0.0, 1.0] shall be represented as integer values between 0 and 0x7fff.*/
const R_FFT_WINDATA EM_FFT_WINDOW[FFT_POINTS] =
{
   #if (FFT_POINTS == RFFT512P)
   #include "r_fft_win512.h"
   #endif /* 512p */
   
   #if (FFT_POINTS == RFFT256P)
   #include "r_fft_win256.h"
   #endif /* 256p */
};
R_FFT_DATA *em_fft_window = (R_FFT_DATA *)EM_FFT_WINDOW;

/* Data for control fft polling process */
struct
{
	/* Scale down for current signal I (24 bit to 16 bit)*/
	struct
	{
		uint8_t phase_r;
		uint8_t phase_y;
		uint8_t phase_b;

		int32_t max_phase_r;
		int32_t max_phase_y;
		int32_t max_phase_b;
	} input_current_gain;

    uint16_t flag_cal_input_gain;
    
	/* Process */
	enum
	{
			FFT_STEP_CAL = 0,
			FFT_STEP_POWER_CAL,
			FFT_STEP_SCALE,
			FFT_STEP_OUTPUT,
	} current_step;

	/* Variable for switch channel */
	EM_FFT_CHANNEL current_channel;
    
    /* Count the number of sample has added to the fft buffer */
	uint16_t buffer_count;
    
	/* Output */

} fft_data;

/***********************************************************************************************************************
* Function Name    : void EM_FFT_PollingProcessing(void)
* Description      : Do FFT polling processing
* Arguments        : None
* Functions Called : None
* Return Value     : None
***********************************************************************************************************************/
void EM_FFT_PollingProcessing(void)
{
    /* LOCAL VARIABLE */
    int16_t j;
    int16_t max_bit;
    R_FFT_DATA max_harmonic;
    float32_t freq;
    
    /* Working RAM Area */
    R_FFT_DATA em_fft_work[FFT_POINTS * 2];

    /* Output Real and Imaginary data for power calculation */
    float real ,imag;

    /* Finished capture? DO FFT */
    if( 
            (fft_data.buffer_count >= FFT_POINTS)
            &&  (em_fft_input != NULL)
            #ifdef __DEBUG
            &&  (em_fft_dump_buffer)
            #endif
       )
    {
        /* ================== DO FFT =================================================== */
        if(fft_data.current_step == FFT_STEP_CAL)
        {
            /* execute 512-point real FFT with windowing */
            #if (FFT_POINTS == RFFT512P)
            R_rfft512_int16(em_fft_input, em_fft_output, em_fft_window, em_fft_work);
            #endif
		
            #if (FFT_POINTS == RFFT256P)
            R_rfft256_int16(em_fft_input, em_fft_output, em_fft_window, em_fft_work);
            #endif
            
            fft_data.current_step++;
            return;
        }
        /* ================== OUTPUT Analysis ========================================== */
        /* ------ Power calculate, convert to 31 point ----------------*/
        if(fft_data.current_step == FFT_STEP_POWER_CAL)
        {
            freq = EM_GetLineFrequency(LINE_TOTAL);
        
            if( (freq >= EM_FFT_FREQ_MIN_50_HZ) && (freq <= EM_FFT_FREQ_MAX_50_HZ))
            {
                fft_tbl = (uint16_t *)fft_tbl_50_Hz;
            }
            else if ( (freq >= EM_FFT_FREQ_MIN_60_HZ) && (freq <= EM_FFT_FREQ_MAX_60_HZ) )
            {
                fft_tbl = (uint16_t *)fft_tbl_60_Hz;
            }
            else
            {
                /* Just support the fundametal frequency is 50Hz and 60Hz */
                em_fft_buffer[1] = 0;
                fft_data.current_step++;
                return;
            }
            
            for( j = 1; j < EM_FFT_MAX_HARMONIC; j++ )
            {
                real = pow( ( (float)em_fft_output[fft_tbl[j] * 2] / (1 << RSLT_DECIMAL_POINT) ), 2);
                imag = pow( ( (float)em_fft_output[fft_tbl[j] * 2 + 1] / (1 << RSLT_DECIMAL_POINT) ), 2);
                em_fft_buffer[j] = (R_FFT_DATA)(sqrt(real + imag) * (1 << RSLT_DECIMAL_POINT) );
            }
            fft_data.current_step++;
            return;
        }
        
        /* ------ Output scaling ---------------------------------------*/
        if(fft_data.current_step == FFT_STEP_SCALE)
        {
            max_harmonic = em_fft_buffer[1];
            
            if(max_harmonic == 0)
            {
                for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++)
                {
                    em_fft_buffer[j] = 100;
                }
            }
            else
            {
                em_fft_buffer[1] = 100;
                for( j = 2 ; j < EM_FFT_MAX_HARMONIC ; j++)
                {
                    real =  ( (float)em_fft_buffer[j] * (float)100 ) / (float)max_harmonic;

                    em_fft_buffer[j] = (R_FFT_DATA) real;
                    
                }
            }
            fft_data.current_step++;
            return;
        }
        
        /* ------ Save to output ---------------------------------------*/
        if(fft_data.current_step == FFT_STEP_OUTPUT)
        {
            switch(fft_data.current_channel)
            {
             case FFT_PHASE_R_V:
                 for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++ )
                 {
                     em_fft_harmonic.phase_r.v[j] = (int8_t) em_fft_buffer[j];
                 }
                 break;
             case FFT_PHASE_Y_V:
                 for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++ )
                 {
                     em_fft_harmonic.phase_y.v[j] = (int8_t) em_fft_buffer[j];
                 }
                 break;
             case FFT_PHASE_B_V:
                 for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++ )
                 {
					 em_fft_harmonic.phase_b.v[j] = (int8_t) em_fft_buffer[j];
                 }
                 break;
             case FFT_PHASE_R_I:
                 for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++ )
                 {
                     em_fft_harmonic.phase_r.i[j] = (int8_t) em_fft_buffer[j];
                 }
                 break;
             case FFT_PHASE_Y_I:
                 for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++ )
                 {
                     em_fft_harmonic.phase_y.i[j] = (int8_t) em_fft_buffer[j];
                 }
                 break;
             case FFT_PHASE_B_I:
                 for( j = 1 ; j < EM_FFT_MAX_HARMONIC ; j++ )
                 {
                     em_fft_harmonic.phase_b.i[j] = (int8_t) em_fft_buffer[j];
                 }
                 break;
             default:
                 /* Reset channel when finish cycle */
            	 fft_data.current_channel = FFT_PHASE_R_V;
                 fft_data.buffer_count = 0;
                 break;
            }
            fft_data.current_step++;
            return;
        }
        
        /* ================== Process next channel ===================================== */
        /* --------- Change input scale for next cycle ----------*/
        if(fft_data.flag_cal_input_gain == 1)
        { 
            /* Phase R */
            max_bit = 0;
            while( fft_data.input_current_gain.max_phase_r >>= 1)
            {
            	max_bit++;
            }
            max_bit = max_bit - 14; /* 16 bits - (1 sign bit * 2) = 14 bits*/
            if( max_bit < 0 )
            {
            	fft_data.input_current_gain.phase_r = 0;
            }
            else
            {
            	fft_data.input_current_gain.phase_r = max_bit;
            }
            
            /* Phase Y */
            max_bit = 0;
            while( fft_data.input_current_gain.max_phase_y >>= 1)
            {
            	max_bit++;
            }
            max_bit = max_bit - 14;
            if( max_bit < 0 )
            {
            	fft_data.input_current_gain.phase_y = 0;
            }
            else
            {
            	fft_data.input_current_gain.phase_y = max_bit;
            }
            
            /* Phase B */
            max_bit = 0;
            while( fft_data.input_current_gain.max_phase_b >>= 1)
            {
            	max_bit++;
            }
            max_bit = max_bit - 14;
            if( max_bit < 0 )
            {
            	fft_data.input_current_gain.phase_b = 0;
            }
            else
            {
            	fft_data.input_current_gain.phase_b = max_bit;
            }
            
            /*Finish update gain for current input*/
            fft_data.flag_cal_input_gain = 0;
        }


        /* -------- Reset for next channel -----------------------*/
        fft_data.current_step = FFT_STEP_CAL;
        fft_data.current_channel++;
        fft_data.buffer_count = 0;
    }
    else
    {
        /* Do nothing */
        
        /* For debug */
        #ifdef __DEBUG
        /* Get command from debug cmd for dump data from related channel.
            em_fft_dump_buffer = 4: dump data from Phase R Voltage
                                 5:                Phase Y Voltage
                                 6:                Phase B Voltage
                                 7:                Phase R Current
                                 8:                Phase Y Current
                                 9:                Phase B Current
            
        */
        if((( em_fft_dump_buffer >= 4 ) && (em_fft_dump_buffer <= 9)) && (fft_data.current_step < FFT_STEP_OUTPUT))
        {
            fft_data.current_channel = em_fft_dump_buffer - 4;
            fft_data.buffer_count = 0;
            fft_data.current_step = FFT_STEP_OUTPUT;
        }
        #endif /* DEBUG */
    }
}

/***********************************************************************************************************************
* Function Name    : __near EM_FFT_HARMONIC * EM_FFT_GetHarmonic(void)
* Description      : 
* Arguments        : None
* Functions Called : None
* Return Value     : 
***********************************************************************************************************************/
__near EM_FFT_HARMONIC * EM_FFT_GetHarmonic(void)
{
    return &em_fft_harmonic;
}

/***********************************************************************************************************************
* Function Name    : void EM_FFT_AddSample(__near EM_SAMPLES * p_samples)
* Description      : The API to add sample of signal.
*                    Each time sample is ready, please use this function to push signal data into the buffer.
*                    When buffer is full, the polling process will calculate and reset this buffer.
* Arguments        : __near EM_SAMPLES * p_samples
* Functions Called : None
* Return Value     : None
***********************************************************************************************************************/
void EM_FFT_AddSample(void)
{   
    /* SWITCHING CHANNEL */
    
    /* Get enough sample then calculate FFT             */
    /* Polling process will reset fft_data.buffer_count when FFT calculation finished */
    if(fft_data.buffer_count < FFT_POINTS)
    {
        /* Do add data and increase counter */
        switch(fft_data.current_channel)
        {
            case FFT_PHASE_R_V:
                em_fft_buffer[fft_data.buffer_count] = (R_FFT_INDATA)((g_wrp_adc_samples.phase_r.v));
                fft_data.buffer_count++;
                break;
            case FFT_PHASE_Y_V:
                em_fft_buffer[fft_data.buffer_count] = (R_FFT_INDATA)((g_wrp_adc_samples.phase_y.v));
                fft_data.buffer_count++;
                break;
            case FFT_PHASE_B_V:
                em_fft_buffer[fft_data.buffer_count] = (R_FFT_INDATA)((g_wrp_adc_samples.phase_b.v));
                fft_data.buffer_count++;
                break;
            case FFT_PHASE_R_I:
            	/* Check the maximum of input phase r for scale 24bit to 16bit */
            	if(fft_data.input_current_gain.max_phase_r < g_wrp_adc_samples.phase_r.i)
            	{
            		fft_data.input_current_gain.max_phase_r = g_wrp_adc_samples.phase_r.i;
            	}
                em_fft_buffer[fft_data.buffer_count] = (R_FFT_INDATA)((g_wrp_adc_samples.phase_r.i)>>fft_data.input_current_gain.phase_r);
                fft_data.buffer_count++;
                break;
            case FFT_PHASE_Y_I:
            	/* Check the maximum of input phase y for scale 24bit to 16bit */
            	if(fft_data.input_current_gain.max_phase_y < g_wrp_adc_samples.phase_y.i)
            	{
            		fft_data.input_current_gain.max_phase_y = g_wrp_adc_samples.phase_y.i;
            	}
                em_fft_buffer[fft_data.buffer_count] = (R_FFT_INDATA)((g_wrp_adc_samples.phase_y.i)>>fft_data.input_current_gain.phase_y);
                fft_data.buffer_count++;
                break;
            case FFT_PHASE_B_I:
            	/* Check the maximum of input phase b for scale 24bit to 16bit */
            	if(fft_data.input_current_gain.max_phase_b < g_wrp_adc_samples.phase_b.i)
            	{
            		fft_data.input_current_gain.max_phase_b = g_wrp_adc_samples.phase_b.i;
            	}
                em_fft_buffer[fft_data.buffer_count] = (R_FFT_INDATA)((g_wrp_adc_samples.phase_b.i)>>fft_data.input_current_gain.phase_b);
                fft_data.buffer_count++;
                break;
            default:
                /* Reset channel for cycle */
                fft_data.current_channel = FFT_PHASE_R_V;
                fft_data.buffer_count = 0;
                fft_data.flag_cal_input_gain = 1;
                break;
        }
    }
    else
    {
        /* Waiting for polling process calculate FFT */
    }
}

#endif /* ENABLE FFT */
/*
  Copyright (C) 2012, 2016 RENESAS ELECTRONICS CORPORATION AND
                 RENESAS SOLUTIONS CORPORATION ALL RIGHTS RESERVED
*/
