/****************************************************************************** 
* File Name    : r_calib.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.04.2013
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_calib.h"            /* Calibration */
#include "r_calib_sampling.h"   /* Accumulation of calibration */

/* Standards library */
#include <math.h>               /* Maths */
#include <string.h>             /* Standard String */

/* Application */
#include "debug.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
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
uint8_t g_calib_is_in_calib_mode = 0;

/******************************************************************************
* Function Name : R_CALIB_CalibrateVoltAndPhase
* Interface     : uint8_t R_CALIB_CalibrateVoltAndPhase(
*               :     uint16_t cycle,
*               :     uint16_t cycle_phase_error,
*               :     float32_t imax,
*               :     float32_t volt,
*               :     float32_t current,
*               :     calib_output_t *output
*               : );
* Description   : Execute auto calibration for voltage and phase current channels
* Arguments     : uint16_t cycle                : Total number of line cycle used to get sample
*               : uint16_t cycle_phase_error    : Total number of line cycle used for phase error calculation
*               : float32_t imax                : Max support measured current
*               : float32_t volt                : Calibrated voltage value
*               : float32_t current             : Calibrated current value
*               : calib_output_t * output       : Structure stores the calibrated result
* Function Calls: EM_CALIB_Init()
*               : EM_CALIB_Start()
*               : EM_CALIB_CalculateSamplingFrequency()
*               : EM_CALIB_CalculateAndSetCurrentGain()
*               : EM_CALIB_DoAccumulate()
* Return Value  : uint8_t, 0 is success, otherwise is error.
******************************************************************************/
uint8_t R_CALIB_CalibrateVoltAndPhase(
    uint16_t    cycle,
    uint16_t    cycle_phase_error,
    float32_t   imax,
    float32_t   volt,
    float32_t   current,
    calib_output_t *output,
    uint8_t     line
)
{
    uint8_t result;
    
    if (output == NULL)
    {
        return 1;
    }

    /* Mark outside enter calibration */
    g_calib_is_in_calib_mode = 1;
    
    EM_CALIB_Init();
    EM_CALIB_Start(imax, volt, current, cycle, cycle_phase_error, line);
    
    /* fs, gain */
    //DEBUG_Printf((uint8_t*)"\n\rCalculating Sampling Frequency");
    output->fs   = EM_CALIB_CalculateSamplingFrequency();
    //DEBUG_Printf((uint8_t*)"\n\rCalculating and Set Current Gain");
    output->gain = EM_CALIB_CalculateAndSetCurrentGain();
    
    /* vcoeff, icoeff, pcoeff, phase shift */
    //DEBUG_Printf((uint8_t*)"\n\rDoing accumulation");
    result = EM_CALIB_DoAccumulate();
    if (result == EM_CALIB_OK)
    {
        output->vcoeff = (sqrt(g_calib_info.storage.vrms / g_calib_info.storage.nvrms)) / volt;
        output->icoeff = (sqrt(g_calib_info.storage.i1rms / g_calib_info.storage.ni1rms)) / current;
        output->pcoeff = output->vcoeff * output->icoeff;
        output->angle_error = g_calib_info.theta;
        output->angle90_offset = g_calib_info.theta90_timer_offset;
        output->voffset = g_calib_info.offset.v.average;
        output->ioffset = g_calib_info.offset.i.average;
    }
    else
    {
        return 1;
    }
    EM_CALIB_Stop();
    
    /* Mark outside release calibration */
    g_calib_is_in_calib_mode = 0;
    
    /* Success */
    return 0;
}

/******************************************************************************
* Function Name : R_CALIB_CalibrateVoltAndNeutral
* Interface     : uint8_t R_CALIB_CalibrateVoltAndNeutral(
*               :     uint16_t cycle,
*               :     uint16_t cycle_phase_error,
*               :     float32_t imax,
*               :     float32_t volt,
*               :     float32_t current,
*               :     calib_output_t *output
*               : );
* Description   : Execute auto calibration for voltage and phase current channels
* Arguments     : uint16_t cycle                : Total number of line cycle used to get sample
*               : uint16_t cycle_phase_error    : Total number of line cycle used for phase error calculation
*               : float32_t imax                : Max support measured current
*               : float32_t volt                : Calibrated voltage value
*               : float32_t current             : Calibrated current value
*               : calib_output_t * output       : Structure stores the calibrated result
* Function Calls: EM_CALIB_Init()
*               : EM_CALIB_Start()
*               : EM_CALIB_CalculateSamplingFrequency()
*               : EM_CALIB_CalculateAndSetCurrentGain()
*               : EM_CALIB_DoAccumulate()
* Return Value  : uint8_t, 0 is success, otherwise is error.
******************************************************************************/
uint8_t R_CALIB_CalibrateVoltAndNeutral(
    uint16_t    cycle,
    uint16_t    cycle_phase_error,
    float32_t   imax,
    float32_t   volt,
    float32_t   current,
    calib_output_t *output
)
{
    if (output == NULL)
    {
        return 1;
    }
    
    /* Mark outside enter calibration */
    g_calib_is_in_calib_mode = 1;
    
    EM_CALIB_Init();
    EM_CALIB_Start(imax, volt, current, cycle, cycle_phase_error, 3);
    
    memset(output, 0, sizeof(calib_output_t));
    
    /* fs, gain */
    output->gain = EM_CALIB_CalculateAndSetCurrentGain();

    /* vcoeff, icoeff, pcoeff, phase shift */
    EM_CALIB_DoAccumulate();
    output->icoeff = (sqrt(g_calib_info.storage.i1rms / g_calib_info.storage.ni1rms)) / current;
    
    EM_CALIB_Stop();
    
    /* Mark outside release calibration */
    g_calib_is_in_calib_mode = 0;
    
    /* Success */
    return 0;
}

/******************************************************************************
* Function Name : R_CALIB_IsInCalibMode
* Interface     : uint8_t R_CALIB_IsInCalibMode(void)
* Description   : Return the current operation mode of EM
* Arguments     : none
* Function Calls: none
* Return Value  : uint8_t, 1 is in calibration processing
******************************************************************************/
uint8_t R_CALIB_IsInCalibMode(void)
{
    return (g_calib_is_in_calib_mode);
}
