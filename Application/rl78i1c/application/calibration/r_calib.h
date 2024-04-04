/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : r_calib.h
* Version      : 1.00
* Description  : 
* Note         : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.04.2013
******************************************************************************/

#ifndef _R_CALIB_H
#define _R_CALIB_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"
#include "em_type.h"
/******************************************************************************
Macro definitions
******************************************************************************/
/*
 * Select operation line frequency, in Hz (50 or 60 only)
 */
#define EM_CALIB_LINE_FREQUENCY         (50)

/*
 * Select MCU pin support (80 or 100 only)
 */
#define EM_CALIB_USING_MCU_PIN          (100)
 
/*
 * Specify constant period interval of RTC (ms) that implemented on RTC driver.
 * Recommend to use 500ms or 1000ms Constant ISR only.
 */
//#define EM_CALIB_RTC_CONSTANT_PERIOD    (500)
#define EM_CALIB_RTC_CONSTANT_PERIOD    (1000)

/* Calibration status */
#define EM_CALIB_OK                     0           /* OK */
#define EM_CALIB_ERROR                  1           /* Error */

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    float32_t   fs;             /* Sampling frequency */
    float32_t   gain;           /* Current gain */
    float32_t   vcoeff;         /* Voltage coefficient */
    float32_t   icoeff;         /* Current coefficient */
    float32_t   pcoeff;         /* Power coefficient, for active, reactive and apparent */
    float32_t   angle_error;    /* Angle error between current and volt */
    float32_t   angle90_offset; /* Angle error betwen volt and volt90 */
    float32_t   voffset;        /* Voltage signal offset */
    float32_t   ioffset;        /* Current signal offset */
} calib_output_t;

/******************************************************************************
Variable Externs
******************************************************************************/
extern uint8_t g_calib_is_in_calib_mode;

/******************************************************************************
Functions Prototypes
******************************************************************************/

/*
 * Module functions
 */
uint8_t R_CALIB_CalibrateVoltAndPhase(
    uint16_t    cycle,
    uint16_t    cycle_phase_error,
    float32_t   imax,
    float32_t   volt,
    float32_t   current,
    calib_output_t *output,
    uint8_t     line
);

uint8_t R_CALIB_CalibrateVoltAndNeutral(
    uint16_t    cycle,
    uint16_t    cycle_phase_error,
    float32_t   imax,
    float32_t   volt,
    float32_t   current,
    calib_output_t *output
);

uint8_t R_CALIB_IsInCalibMode(void);

/*
 * Callback function, must be registered to ADC Conversion End Callback of Driver
 */
void EM_CALIB_Sampling(__near EM_SAMPLES * p_samples);

/*
 * Callback function, must be registered to RTC Constant ISR Callback of Driver
 */
void EM_CALIB_RTCConstantInterruptCallback(void);

#endif /* _R_CALIB_H */

