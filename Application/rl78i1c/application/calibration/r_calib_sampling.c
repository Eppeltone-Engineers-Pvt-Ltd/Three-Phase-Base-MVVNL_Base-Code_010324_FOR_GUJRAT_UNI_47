/****************************************************************************** 
* File Name    : r_calib_sampling.c
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
/* Driver */
#include "r_cg_macrodriver.h"   /* Macro Driver Definitions */

/* Middleware */
#include "typedef.h"            /* GSCE Standard Typedef */

#include "em_core.h"            /* EM Core APIs */
#include "em_conversion.h"      /* EM Conversion APIs */
#include "em_maths.h"           /* EM Maths APIs */
#include "em_type.h"

/* Wrapper */
#include "wrp_em_wdt.h"         /* Wrapper EM WDT */
#include "wrp_em_adc.h"         /* Wrapper EM ADC */
#include "wrp_mcu.h"            /* Wrapper MCU */
#include "wrp_em_sw_config.h"   /* SW config info, store ADC channels mapping */
#include "rl78i1c_sw_hpf.h"
/* Application */
#include <string.h>             /* Standard string lib */
#include <math.h>               /* Standard mathematics lib */
#include "platform.h"           /* Platform definition */
#include "r_calib_sampling.h"   /* Sampling for auto calibration */
#include "r_calib.h"            /* Calculation */
#include "debug.h"

/* Driver layer */

/******************************************************************************
Macro definitions
******************************************************************************/
/*
 * Specify mask value for sampling, un-define if not use.
 */
#define EM_CALIB_MASK_OFF_NOISY_BITS_POSITIVE           (0xF0)
#define EM_CALIB_MASK_OFF_NOISY_BITS_NEGATIVE           (0x0F)

/* Function macro definition */
#define EM_CALIB_ADC_GetGainAndPhaseValue(adc_info)     R_DSADC_GetGainAndPhase((dsad_reg_setting_t*) adc_info)
#define EM_CALIB_ADC_RestoreGainAndPhaseValue(adc_info) R_DSADC_SetGainAndPhase(adc_info)

/* Constant for sampling frequeny calculation */
#define EM_CALIB_FIXED_FSUB                     (32.768f)   /* Fixed value of sub clock */

/* Constant number of sample for gain calculation */
#define EM_CALIB_NUM_SAMPLE_FOR_CALC_GAIN       (100)       /* Its value is >= (fs / 45hz) */

/* Phase shift approximation system */
#define EM_CALIB_PHASE_ERROR_STOP_THRESHOLD     (0.0125f)       /* Stop threshold for delta error calculation */
#define EM_CALIB_PHASE_ERROR_MIN_THRESHOLD      (-0.59529951f)  /* Min threshold for delta error calculation */
#define EM_CALIB_PHASE_ERROR_MAX_THRESHOLD      (1.470963154f)  /* Max threshold for delta error calculation */
#define EM_CALIB_PHASE_ERROR_MAX_ANGLE_ERR      (13.812f)       /* Max abs angle error for setting into Phase Control Register */

/* Phase shift step on I1C device */
#if ((defined EM_CALIB_LINE_FREQUENCY) && (EM_CALIB_LINE_FREQUENCY == 50))
    #define EM_CALIB_PHASE_SHIFT_STEP           (0.0120f)
#elif ((defined EM_CALIB_LINE_FREQUENCY) && (EM_CALIB_LINE_FREQUENCY == 48))
    #define EM_CALIB_PHASE_SHIFT_STEP           (0.01152f)
#elif ((defined EM_CALIB_LINE_FREQUENCY) && (EM_CALIB_LINE_FREQUENCY == 52))
    #define EM_CALIB_PHASE_SHIFT_STEP           (0.01248f)
#elif ((defined EM_CALIB_LINE_FREQUENCY) && (EM_CALIB_LINE_FREQUENCY == 60))
    #define EM_CALIB_PHASE_SHIFT_STEP           (0.0144f)
#else
    #error "Not select current line frequency"
#endif

#define EM_CALIB_QUARTER_SHIFT_PHASE_SELECT     (LINE_PHASE_B)

/* Accumulation step definitions */
#define EM_CALIB_ACCUMULATE_STEP_V              (0x01)  /* Choose Voltage on this step */
#define EM_CALIB_ACCUMULATE_STEP_IP             (0x02)  /* Choose Phase current on this step */
#define EM_CALIB_ACCUMULATE_STEP_IN             (0x04)  /* Choose Neutral current on this step */

#define EM_CALIB_ACCUMULATE_STEP_FS             (0x08)  /* Accumulate to calculate fs */
#define EM_CALIB_ACCUMULATE_STEP_IGAIN          (0x10)  /* Accumulate to calculate phase gain */
#define EM_CALIB_ACCUMULATE_STEP_PHASE_ANGLE    (0x20)  /* Choose Phase angle error on this step */
#define EM_CALIB_ACCUMULATE_STEP_NEUTRAL_ANGLE  (0x40)  /* Choose Neutral angle error on this step */
#define EM_CALIB_ACCUMULATE_STEP_V90_ANGLE      (0x80)  /* Choose V90 angle error on this step */

/*
 * The steps length to do +59.90 and -59.90
 * This setting is according to sampling frequency of DSAD module
 */
#define EM_CALIB_PI                             (3.141592654f)                      /* PI */
 
#if (EM_CALIB_LINE_FREQUENCY == 50)
 
#define EM_CALIB_CIRCLE_DELAY_STEP              (13)                                /* 13 step ~ 59.904 degree */
#define EM_CALIB_CIRCLE_LENGTH                  (EM_CALIB_CIRCLE_DELAY_STEP + 1)    /* 1 more for buffer */
#define EM_CALIB_C                              (1.045522035f)                      /* C in Rad of 59.904 degree */
#define EM_CALIB_SIN2C                          (0.867696054f)                      /* Sin(2C) */
#define EM_CALIB_TANC                           (1.725368131f)                      /* Tan(C) */

#elif (EM_CALIB_LINE_FREQUENCY == 48)
 
#define EM_CALIB_CIRCLE_DELAY_STEP              (13)                                /* 13 step ~ 59.904 degree */
#define EM_CALIB_CIRCLE_LENGTH                  (EM_CALIB_CIRCLE_DELAY_STEP + 1)    /* 1 more for buffer */
#define EM_CALIB_C                              (1.003701154f)                      /* C in Rad of 59.904 degree */
#define EM_CALIB_SIN2C                          (0.906192096f)                      /* Sin(2C) */
#define EM_CALIB_TANC                           (1.570159659f)                      /* Tan(C) */

#elif (EM_CALIB_LINE_FREQUENCY == 52)
 
#define EM_CALIB_CIRCLE_DELAY_STEP              (12)                                /* 13 step ~ 59.904 degree */
#define EM_CALIB_CIRCLE_LENGTH                  (EM_CALIB_CIRCLE_DELAY_STEP + 1)    /* 1 more for buffer */
#define EM_CALIB_C                              (1.003701154f)                      /* C in Rad of 59.904 degree */
#define EM_CALIB_SIN2C                          (0.906192096f)                      /* Sin(2C) */
#define EM_CALIB_TANC                           (1.570159659f)                      /* Tan(C) */

#elif (EM_CALIB_LINE_FREQUENCY == 60)
 
#define EM_CALIB_CIRCLE_DELAY_STEP              (10)                                /* 13 step ~ 59.904 degree */
#define EM_CALIB_CIRCLE_LENGTH                  (EM_CALIB_CIRCLE_DELAY_STEP + 1)    /* 1 more for buffer */
#define EM_CALIB_C                              (0.965097263f)                      /* C in Rad of 59.904 degree */
#define EM_CALIB_SIN2C                          (0.936108653f)                      /* Sin(2C) */
#define EM_CALIB_TANC                           (1.443968007f)                      /* Tan(C) */

#endif

#define ABS(x)                                  ((x) <  0  ? (-(x)) : (x))
#define EM_CALIB_DERIVATION_NUM_DATA            27

#define EM_CALIB_OFFSET_CYCLE_FOR_AVERAGE       10
#define EM_CALIB_OFFSET_DEFAULT_NSAMPLE         (EM_CALIB_OFFSET_CYCLE_FOR_AVERAGE * 90)    /* min 45 Hz line supply */
#define EM_CALIB_OFFSET_STABILIZATION_COUNT     5

#define EM_CALIB_SIGNAL_CYCLE_FOR_STABILITY     (1000)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* Calib information */
calib_data_t        g_calib_info;

static uint8_t      g_calib_line = 0;
static uint32_t     g_calib_number_cycles;
static uint16_t     g_calib_number_cycles_phase_error;
static float32_t    g_calib_volt;
static float32_t    g_calib_current;
static float32_t    g_calib_imax;
static dsad_channel_t   g_calib_channel;
static uint8_t      g_calib_step;
static uint8_t      g_calib_is_start_count_sample = 0;
static uint8_t      g_calib_start_count_1s = 0;

/* Phase shift calculation array 0 */
static int8_t       g_calib_phase_shift0_pos;
static uint8_t      g_calib_phase_shift0_ready_count;
static int8_t       g_calib_phase_shift0_ready;
static int32_t      g_calib_phase_shift0_circle[EM_CALIB_CIRCLE_LENGTH];

/* Phase shift calculation array 1 */
static int8_t       g_calib_phase_shift1_pos;
static uint8_t      g_calib_phase_shift1_ready_count;
static int8_t       g_calib_phase_shift1_ready;
static int32_t      g_calib_phase_shift1_circle[EM_CALIB_CIRCLE_LENGTH];

static uint8_t      g_is_quarter_shift_calibrate;

static int16_t      g_sample_v;
static int16_t      g_sample_v90;
static int32_t      g_sample_i;

/* Storage to backup ADC register setting */
static dsad_reg_setting_t   g_calib_adc_reg_setting;

/******************************************************************************
Private global variables and functions
******************************************************************************/

/* Commons */
static void EM_CALIB_ResetPhaseShiftCircle(void);
static int32_t EM_CALIB_DoPhaseShift0(int32_t signal, uint8_t step);
static int32_t EM_CALIB_DoPhaseShift1(int32_t signal, uint8_t step);
static float32_t EM_CALIB_LookupDerivation(float32_t angle);
static float32_t EM_CALIB_GetAccumulatedValue(acc24_t *p_msb);
static uint8_t EM_CALIB_ConvertIntoADCGain(uint8_t calc_gain);

/* Calibration wrapper definition */
static uint8_t EM_CALIB_ADC_SetCurrentGain(uint8_t gain);
static void EM_CALIB_ADC_SetAngleError(float32_t angle);
static void EM_CALIB_ADC_Start(void);

/* Accumulation & sampling */
static uint8_t EM_CALIB_DoAccumulationSignals(uint8_t steps);
static uint8_t EM_CALIB_DoAccumulateAngleError(uint8_t steps);
static uint8_t EM_CALIB_DoAccumulateAngle90Error(uint8_t steps);

static void EM_CALIB_SamplingUpdateOffset(void);
static void EM_CALIB_SamplingForFs(void);
static void EM_CALIB_SamplingForCalcGain(void);
static void EM_CALIB_SamplingForAngleError(void);
static void EM_CALIB_SamplingForAccSignals(void);
static void EM_CALIB_SamplingForAngle90Error(void);

/******************************************************************************
* Function Name : EM_CALIB_ResetPhaseShiftCircle
* Interface     : static void EM_CALIB_ResetPhaseShiftCircle(void)
* Description   : Reset angle shifting circle and all related flags
* Arguments     : None
* Function Calls: none
* Return Value  : none
******************************************************************************/
static void EM_CALIB_ResetPhaseShiftCircle(void)
{
    memset(g_calib_phase_shift0_circle, 0, EM_CALIB_CIRCLE_LENGTH * sizeof(int32_t));
    memset(g_calib_phase_shift1_circle, 0, EM_CALIB_CIRCLE_LENGTH * sizeof(int32_t));
    
    g_calib_phase_shift0_pos         = 0;
    g_calib_phase_shift0_ready       = 0;
    g_calib_phase_shift0_ready_count = 0;
    
    g_calib_phase_shift1_pos         = 0;
    g_calib_phase_shift1_ready       = 0;
    g_calib_phase_shift1_ready_count = 0;   
}

/******************************************************************************
* Function Name : EM_CALIB_DoPhaseShift0
* Interface     : static int32_t EM_CALIB_DoPhaseShift0(int32_t signal, uint8_t step)
* Description   : Execute shifting input single 90 degree
* Arguments     : int32_t signal: shifted signal
*               : uint8_t step  : expected steps to archive 90 degree shifting
* Function Calls: none
* Return Value  : shifted signal
******************************************************************************/
static int32_t EM_CALIB_DoPhaseShift0(int32_t signal, uint8_t step)
{
    /* PUSH: Add the signal into the data circle at circlepos position */
    g_calib_phase_shift0_circle[g_calib_phase_shift0_pos] = signal;

    /* POP: Get the signal from data circle from (g_calib_phase_shift_pos + steps) position */
    signal = g_calib_phase_shift0_circle[(g_calib_phase_shift0_pos + step) % EM_CALIB_CIRCLE_LENGTH];
    
    g_calib_phase_shift0_pos--;     /* Scroll the circle, scroll left */
    if (g_calib_phase_shift0_pos == -1)
    {
        g_calib_phase_shift0_pos   = EM_CALIB_CIRCLE_LENGTH - 1;
    }
    
    if (g_calib_phase_shift0_ready_count >= EM_CALIB_CIRCLE_LENGTH + 1)
    {
        g_calib_phase_shift0_ready = 1;
    }
    else
    {
        g_calib_phase_shift0_ready_count++;
    }
    
    return signal;
}

/******************************************************************************
* Function Name : EM_CALIB_DoPhaseShift1
* Interface     : static int32_t EM_CALIB_DoPhaseShift1(int32_t signal, uint8_t step)
* Description   : Execute shifting input single 90 degree
* Arguments     : int32_t signal: shifted signal
*               : uint8_t step  : expected steps to archive 90 degree shifting
* Function Calls: none
* Return Value  : shifted signal
******************************************************************************/
static int32_t EM_CALIB_DoPhaseShift1(int32_t signal, uint8_t step)
{
    /* PUSH: Add the signal into the data circle at circlepos position */
    g_calib_phase_shift1_circle[g_calib_phase_shift1_pos] = signal;

    /* POP: Get the signal from data circle from (g_calib_phase_shift_pos + steps) position */
    signal = g_calib_phase_shift1_circle[(g_calib_phase_shift1_pos + step) % EM_CALIB_CIRCLE_LENGTH];
    
    g_calib_phase_shift1_pos--;     /* Scroll the circle, scroll left */
    if (g_calib_phase_shift1_pos == -1)
    {
        g_calib_phase_shift1_pos   = EM_CALIB_CIRCLE_LENGTH - 1;
    }
    
    if (g_calib_phase_shift1_ready_count >= EM_CALIB_CIRCLE_LENGTH + 2)
    {
        g_calib_phase_shift1_ready = 1;
    }
    else
    {
        g_calib_phase_shift1_ready_count++;
    }
    
    return signal;
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForAngleErr
* Interface     : static void EM_CALIB_SamplingForAngleErr(void)
* Description   : Lookup to get derivation value
* Arguments     : None
* Function Calls: None
* Return Value  : None
******************************************************************************/
static float32_t EM_CALIB_LookupDerivation(float32_t angle)
{
    volatile float32_t cos_xc;
    
    /* -sin(2C) / cos^2(x+C), x in Rad */
    angle  *= EM_CALIB_PI;
    angle  /= 180.0f;
    cos_xc  = cosf(angle + EM_CALIB_C); 
    
    return (-EM_CALIB_SIN2C / (cos_xc * cos_xc));
}

/******************************************************************************
* Function Name : EM_CALIB_GetAccumulatedValue
* Interface     : float32_t EM_CALIB_GetAccumulatedValue(acc24_t *p_acc)
* Description   : Get the accumulator value from MSB and LSB parts
* Arguments     : acc24_t * p_acc: pointer to (acc24_t)
* Function Calls: EM_ACC24_GetFloatValue
* Return Value  : float32_t, combined value from MSB and LSB
******************************************************************************/
static float32_t EM_CALIB_GetAccumulatedValue(acc24_t *p_acc)
{
    volatile float32_t result = 0;
    if (p_acc != NULL)
    {
        result += EM_ACC24_GetFloatValue(p_acc);
    }
    return result;
}

/******************************************************************************
* Function Name : EM_CALIB_ConvertIntoADCGain
* Interface     : static uint8_t EM_CALIB_ConvertIntoADCGain(uint8_t calc_gain)
* Description   : Convert the calculated current gain into suitable value to set into ADC module
* Arguments     : Calculated current gain 
* Function Calls: None
* Return Value  : The appropriate gain for ADC module
******************************************************************************/
static uint8_t EM_CALIB_ConvertIntoADCGain(uint8_t calc_gain)
{
    if (calc_gain < 2)
    {
        return 1;
    }
    else if (calc_gain < 4)
    {
        return 2;
    }
    else if (calc_gain < 8)
    {
        return 4;
    }
    else if (calc_gain < 16)
    {
        return 8;
    }
    else if (calc_gain < 32)
    {
        return 16;
    }
    else
    {
        return 32;
    }
}

/******************************************************************************
* Function Name : EM_CALIB_ADC_SetCurrentGain
* Interface     : static uint8_t static uint8_t EM_CALIB_ADC_SetCurrentGain(uint8_t gain)(uint8_t gain)
* Description   : Set current gain into ADC HW module
* Arguments     : gain : Gain value used to set into ADC driver
* Function Calls: R_ADC_SetChannelGain()
* Return Value  : Status of HW setting gain
******************************************************************************/
static uint8_t EM_CALIB_ADC_SetCurrentGain(uint8_t gain)
{
    volatile uint16_t    status;
    volatile dsad_gain_t     gain_value;
    
    if (gain == 1)
    {
        gain_value = GAIN_X1;
    }
    else if (gain == 2)
    {
        gain_value = GAIN_X2;
    }
    else if (gain == 4)
    {
        gain_value = GAIN_X4;
    }
    else if (gain == 8)
    {
        gain_value = GAIN_X8;
    }
    else if (gain == 16)
    {
        gain_value = GAIN_X16;
    }
    else
    {
        gain_value = GAIN_X32;
    }
    
    status = R_DSADC_SetChannelGain(g_calib_channel, gain_value);
    
    return status;
}

/******************************************************************************
* Function Name : EM_CALIB_ADC_SetAngleError
* Interface     : static void EM_CALIB_ADC_SetAngleError(float32_t angle)
* Description   : Shift volt/phase channel due to the set channel and angle
* Arguments     : angle: shifted angle
* Function Calls: R_ADC_AdjustPhaseDelay()
* Return Value  : None
******************************************************************************/
static void EM_CALIB_ADC_SetAngleError(float32_t angle)
{
    if (angle < 0)
    {
        /* Shift phase channel */
        R_DSADC_AdjustPhaseDelay(
            g_calib_channel, 
            (uint16_t)((-angle) / EM_CALIB_PHASE_SHIFT_STEP)
        );
    }
    else
    {
        /* fatal error here: must ensure current channel lead SAR-ADC voltage channel */
    }
}


/******************************************************************************
* Function Name : EM_CALIB_ADC_Start
* Interface     : static void EM_CALIB_ADC_Start(uint8_t is_phase)
* Description   : Start ADC due to the calibrated current channel
* Arguments     : is_phase : 1 is calibrate phase channel
*               :          : 0 is calibrate neutral channel
* Function Calls: R_ADC_StartWithoutGainAndPhase()
* Return Value  : None
******************************************************************************/
static void EM_CALIB_ADC_Start(void)
{
#if 0
    R_DSADC_StartWithoutGainAndPhase(_0F0F_DSAD_ENABLE_ALLCH);
#else
    EM_ADC_Start();
    
    RL78I1C_ResetSoftwareHPF16(0);
    RL78I1C_ResetSoftwareHPF16(1);
    RL78I1C_ResetSoftwareHPF16(2);
    RL78I1C_ResetSoftwareHPF16(3);
    RL78I1C_ResetSoftwareHPF16(4);
    RL78I1C_ResetSoftwareHPF16(5);
    
    R_DSADC_SetChannelGain(DSADCHANNEL0, GAIN_X1);
    R_DSADC_SetChannelGain(DSADCHANNEL1, GAIN_X1);
    R_DSADC_SetChannelGain(DSADCHANNEL2, GAIN_X1);
    R_DSADC_SetChannelGain(DSADCHANNEL3, GAIN_X1);
    
    R_DSADC_AdjustPhaseDelay(DSADCHANNEL0, 0);
    R_DSADC_AdjustPhaseDelay(DSADCHANNEL1, 0);
    R_DSADC_AdjustPhaseDelay(DSADCHANNEL2, 0);
    R_DSADC_AdjustPhaseDelay(DSADCHANNEL3, 0);
    
    MCU_Delay(10000);
//    R_DSADC_SetHighPassFilterOff(DSADCHANNEL0);
//    R_DSADC_SetHighPassFilterOff(DSADCHANNEL1);
//    R_DSADC_SetHighPassFilterOff(DSADCHANNEL2);
//    R_DSADC_SetHighPassFilterOff(DSADCHANNEL3);
    
#endif
}

/******************************************************************************
* Function Name : EM_CALIB_Init
* Interface     : uint8_t EM_CALIB_Init(void)
* Description   : Initialize auto calibration module
* Arguments     : None
* Function Calls: EM_WDT_Init()
*               : EM_ADC_Init()
* Return Value  : None
******************************************************************************/
void EM_CALIB_Init(void)
{
    /* Initialize HW modules */
    EM_WDT_Init();              /* Initialize WDT module */
    EM_ADC_Init();              /* Initialize 24bit - Delta Sigma ADC module */
}

/******************************************************************************
* Function Name : EM_CALIB_Start
* Interface     : void EM_CALIB_Start(
*               :     float32_t imax, 
*               :     float32_t volt, 
*               :     float32_t current, 
*               :     uint16_t cycles, 
*               :     uint16_t cycles_phase_error, 
*               :     uint8_t is_phase
*               : );
* Description   : Start auto calibration
* Arguments     : imax              : Max supported measured current of MCU
*               : volt              : Calibrated voltage
*               : current           : Calibrated current
*               : cycles            : Expected total number cycles for sampling
*               : cycle_phase_error : Expected total number cycles for phase error sampling
*               : is_phase: 1 is Phase, 0 is Neutral
* Function Calls: EM_CALIB_ADC_GetGainAndPhaseValue()
*               : EM_CALIB_ResetPhaseShiftCircle()
*               : EM_CALIB_ADC_Start()
*               : EM_WDT_Start()
*               : MCU_Delay()
*               : EM_WDT_Restart()
* Return Value  : None
******************************************************************************/
void EM_CALIB_Start(
    float32_t imax, 
    float32_t volt, 
    float32_t current, 
    uint16_t cycles,
    uint16_t cycles_phase_error,
    uint8_t line
)
{
    volatile uint8_t i;

    /* Reset data storage */
    memset(&g_calib_info, 0, sizeof(g_calib_info));
    memset(&g_calib_adc_reg_setting,0, sizeof(g_calib_adc_reg_setting));
    g_calib_step = 0;
    
    g_calib_info.offset.v.average = 512;
    g_calib_info.offset.i.average = 60000;
    

    g_calib_info.offset.v.average_prev = 512;
    g_calib_info.offset.i.average_prev = 60000; 
    
    /* Get original value of HW */
    EM_CALIB_ADC_GetGainAndPhaseValue(&g_calib_adc_reg_setting);
    
    /* Reset Phase shift circle */
    EM_CALIB_ResetPhaseShiftCircle();
    
    /* Get calibrated information */
    g_calib_volt                        = volt;
    g_calib_current                     = current;
    g_calib_number_cycles               = cycles;
    g_calib_number_cycles_phase_error   = cycles_phase_error;
    g_calib_imax                        = imax;
    g_calib_line                        = line;
    
    if (line == 0)
    {
        g_calib_channel = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R;
    }
    else if (line == 1)
    {
        g_calib_channel = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y;    
    }
    else if (line == 2)
    {
        g_calib_channel = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B;    
    }
    else if (line == 3)
    {
        g_calib_channel = EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL;    
    }

	
// serious doubt - why is the g_is_quarter_shift_calibrate set to 1 only for LINE_PHASE_B
// this needs to be verified

    if (line == EM_CALIB_QUARTER_SHIFT_PHASE_SELECT)
    {
        g_is_quarter_shift_calibrate = 1;   
    }
    else
    {
        g_is_quarter_shift_calibrate = 0;   
    }
    
    /* Start HW modules */
    EM_CALIB_ADC_Start();   /* Start DSAD module for calibration */
    EM_WDT_Start();
    
    /* Wait until ADC offset stabilized */
    while (g_calib_info.stable_count < EM_CALIB_SIGNAL_CYCLE_FOR_STABILITY)
    {
        EM_WDT_Restart();
    }
}

/******************************************************************************
* Function Name : EM_CALIB_Stop
* Interface     : void EM_CALIB_Stop()
* Description   : Stop auto calibration
* Arguments     : None
* Function Calls: EM_ADC_Stop()
*               : EM_CALIB_ADC_RestoreGainAndPhaseValue()
* Return Value  : None
******************************************************************************/
void EM_CALIB_Stop(void)
{
    /* Stop HW modules */
    EM_ADC_Stop();
    
    /* Restore original value of HW */
    EM_CALIB_ADC_RestoreGainAndPhaseValue(g_calib_adc_reg_setting); 
}

/******************************************************************************
* Function Name : EM_CALIB_CalculateSamplingFrequency
* Interface     : float32_t EM_CALIB_CalculateSamplingFrequency(void)
* Description   : Calculate actual sampling frequency of DSAD
* Arguments     : None
* Function Calls: None
* Return Value  : Calculated sampling frequency
******************************************************************************/
float32_t EM_CALIB_CalculateSamplingFrequency(void)
{
    /* Set sampling step for sampling process */  
    g_calib_step = EM_CALIB_ACCUMULATE_STEP_FS;
    
    /* Clear the number ADC ISR counter */
    g_calib_info.fs.nsample = 0;
    
    g_calib_start_count_1s = 1; 
    /* Wait until finish sampling process */
    g_calib_info.fs.is_done = 0;
    while (g_calib_info.fs.is_done == 0)
    {
        EM_WDT_Restart();
    }
    g_calib_start_count_1s = 0;
    return (g_calib_info.fs.nsample);
}

/******************************************************************************
* Function Name : EM_CALIB_CalculateCurrentGain
* Interface     : static float32_t EM_CALIB_CalculateCurrentGain(void)
* Description   : Calculate and return phase/neutral current gain 
*               : Then set the calculated value into ADC module
* Arguments     : None
* Function Calls: EM_CALIB_ConvertIntoADCGain()
*               : EM_CALIB_ADC_SetCurrentGain()
* Return Value  : Calculated current gain
******************************************************************************/
float32_t EM_CALIB_CalculateAndSetCurrentGain(void)
{
    volatile float32_t   amplified_const;
    volatile uint8_t     igain = 0;
    
    g_calib_step = EM_CALIB_ACCUMULATE_STEP_IGAIN;
    
    /* Reset acknowledged flag */
    g_calib_info.gain.idone = 0;    
    g_calib_info.gain.is_start = 0;
    /* Wait until finish sampling process */
    while (g_calib_info.gain.idone == 0)
    {
        EM_WDT_Restart();
    }
    
    /*
     *          Calculation explanation
     *  Gain is used to make DSAD result reach Max Full Scale at MAX_SUPPORTED_CURRENT
     *                                      *Gain 
     *  MAX_SUPPORTED_CURRENT   --> Amax[LSB] -------> 2^24 [LSB] Full scale of DSAD
     *                 2^24
     *  --> Gain = ------------
     *              Amax [LSB]
     *  And:
     *  MAX_SUPPORTED_CURRENT   -->      Amax       [LSB]
     *       calib_current      -->      Imax   [LSB]
     *         MAX_SUPPORTED_CURRENT
     *  Amax = --------------------- * Imax
     *            calib_current
     *                        2^24                          2^24
     *  --> Gain = ----------------------------- = ----------------------
     *             MAX_SUPPORTED_CURRENT            amplified_val * Imax
     *             --------------------- * Imax
     *             calib_current
     */
    /* Calculate current gain */
    amplified_const = g_calib_imax / g_calib_current;
    igain = (uint8_t)(pow(2,23) / (g_calib_info.gain.imax * amplified_const));

    igain = EM_CALIB_ConvertIntoADCGain(igain);
    
    EM_CALIB_ADC_SetCurrentGain(igain);
    return igain;
}

/******************************************************************************
* Function Name : EM_CALIB_DoAccumulate
* Interface     : uint8_t EM_CALIB_DoAccumulate(void)
* Description   : Control accumulation process of v, i1, i2 channels
*               : Control angle error calculation
* Arguments     : None
* Function Calls: EM_CALIB_DoAccumulationSignals()
*               : EM_CALIB_DoAccumulateAngleError()
* Return Value  : None
******************************************************************************/
uint8_t EM_CALIB_DoAccumulate(void)
{
    volatile uint8_t result = EM_CALIB_OK;
    
    if (g_calib_line < 3)
    {
        /* Accumulate signals */
        //DEBUG_Printf((uint8_t *)"\n\r--- Accumulating signals...");
        result |= EM_CALIB_DoAccumulationSignals(EM_CALIB_ACCUMULATE_STEP_V | EM_CALIB_ACCUMULATE_STEP_IP);
        /* Accumulate phase angle error */
        //DEBUG_Printf((uint8_t *)"\n\r--- Accumulating phase angle error...");
        result |= EM_CALIB_DoAccumulateAngleError(EM_CALIB_ACCUMULATE_STEP_PHASE_ANGLE);
        
        /* 90 degree sample calibration only when use reactive */
        #ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
        if (g_is_quarter_shift_calibrate == 1)
        {
            /* Accumulate v90 angle error */
            //DEBUG_Printf((uint8_t *)"\n\r--- Accumulating v90 angle error...");
            result |= EM_CALIB_DoAccumulateAngle90Error(EM_CALIB_ACCUMULATE_STEP_V90_ANGLE);
        }
        #endif
    }
    else
    {
        /* Accumulate signals */
        //DEBUG_Printf((uint8_t *)"\n\r--- Accumulating signals...");
        result |= EM_CALIB_DoAccumulationSignals(EM_CALIB_ACCUMULATE_STEP_IP);
    }
    
    return result;
}

/******************************************************************************
* Function Name : EM_CALIB_DoAccumulationSignals
* Interface     : static uint8_t EM_CALIB_DoAccumulationSignals(uint8_t steps)
* Description   : Control accumulation process
* Arguments     : uint8_t step: choose channels to accumulate
* Function Calls: EM_ACC24_Clear()
*               : EM_CALIB_GetAccumulatedValue()
* Return Value  : EM_CALIB_ERROR : input argument wrong
*               : EM_CALIB_OK    : input argument OK
******************************************************************************/
static uint8_t EM_CALIB_DoAccumulationSignals(uint8_t steps)
{
    /* Only do for the V, Ip, In */
    if ((steps < EM_CALIB_ACCUMULATE_STEP_V)        ||  /* Choose nothing */
        (steps > (  EM_CALIB_ACCUMULATE_STEP_V      | 
                    EM_CALIB_ACCUMULATE_STEP_IP     | 
                    EM_CALIB_ACCUMULATE_STEP_IN))
        )   /* Get samples of V, IP and IN channels */
    {
        return EM_CALIB_ERROR;
    }
    
    /* Clear storage */
    /* Voltage */
    g_calib_info.storage.vrms   = 0;
    g_calib_info.storage.nvrms  = 0;
    /* Phase */
    g_calib_info.storage.i1rms  = 0;
    g_calib_info.storage.ni1rms = 0;
    /* Neutral */
    g_calib_info.storage.i2rms  = 0;
    g_calib_info.storage.ni2rms = 0;
    
    /* Clear accumulator */
    /* Voltage signal */
    EM_ACC24_Clear(&g_calib_info.acc.vrms);
    g_calib_info.acc.nvrms = 0;
    /* Phase signal */
    EM_ACC24_Clear(&g_calib_info.acc.i1rms);
    g_calib_info.acc.ni1rms = 0;
    /* Neutral signal */
    EM_ACC24_Clear(&g_calib_info.acc.i2rms);
    g_calib_info.acc.ni2rms = 0;
    
    /* Reset data storage for auto calibration */
    g_calib_info.number_of_cycles = 0;  
    g_calib_info.v_start_flag  = 0;
    g_calib_info.i1_start_flag = 0;
    g_calib_info.i2_start_flag = 0; 
    g_calib_info.acc.is_done = 0;
    
    /* Choose accumulated signal, Start sampling process for accumulation */
    g_calib_step = steps;
    
    /* Assign accumulated value into Storage */
    while (g_calib_info.acc.is_done == 0)
    {
        EM_WDT_Restart();
    }
    /*
     * Voltage signal 
     */
    g_calib_info.storage.vrms = EM_CALIB_GetAccumulatedValue(
        &g_calib_info.acc.vrms
    );
    g_calib_info.storage.nvrms = g_calib_info.acc.nvrms;

    /*
     * Phase signal 
     */
    g_calib_info.storage.i1rms = EM_CALIB_GetAccumulatedValue(
        &g_calib_info.acc.i1rms
    );
    g_calib_info.storage.ni1rms = g_calib_info.acc.ni1rms;

    /*
     * Neutral signal 
     */
    g_calib_info.storage.i2rms = EM_CALIB_GetAccumulatedValue(
        &g_calib_info.acc.i2rms
    );
    g_calib_info.storage.ni2rms = g_calib_info.acc.ni2rms;
    
    return 0;
}

/******************************************************************************
* Function Name : EM_CALIB_DoAccumulateAngleError
* Interface     : static uint8_t EM_CALIB_DoAccumulateAngleError(uint8_t steps)
* Description   : Accumulate and scan angle error
* Arguments     : steps : choose calibration steps
* Function Calls: EM_CALIB_GetAccumulatedValue()
*               : EM_CALIB_LookupDerivation()
*               : EM_CALIB_ADC_SetAngleError()
*               : EM_CALIB_ADC_SetAngleError()
*               : EM_CALIB_ResetPhaseShiftCircle()
*               : EM_ACC24_Clear()
* Return Value  : uint8_t, 0 is OK, others are fail
******************************************************************************/
static uint8_t EM_CALIB_DoAccumulateAngleError(uint8_t steps)
{
    volatile float32_t   delta_error = 0;
    volatile uint8_t     rlt_code    = EM_CALIB_OK;
    
    /* Check input condition */
    if ((steps != EM_CALIB_ACCUMULATE_STEP_PHASE_ANGLE) &&
        (steps != EM_CALIB_ACCUMULATE_STEP_NEUTRAL_ANGLE))
    {
        return EM_CALIB_ERROR;
    }
    
    /* Set accumulation steps */
    g_calib_step = steps;
    
    /* Reset data storage */
    g_calib_info.number_of_cycles = 0;  
    g_calib_info.v_start_flag  = 0;
    
    g_calib_info.phase_choosen = 0;                                 /* Choose V as referent channel to shift */
    g_calib_info.last_theta = 0;                                    /* Start at 0 degree */
    g_calib_info.theta = 0;                                         /* No setting angle error */    
    g_calib_info.derivation =
        EM_CALIB_LookupDerivation(g_calib_info.theta);              /* Get initial value of derivate_value */
    g_calib_info.initial_delta_error = 0;                           /* Init delta error = 0 */
    g_calib_info.last_delta_error = 0;                              /* Haven't update delta error */

    EM_CALIB_ResetPhaseShiftCircle();
    
    /* Wait until finish accumulation */
    g_calib_info.acc.angle_is_done = 0; /* Control sampling for each scanning time */
    g_calib_info.is_finish_calc_angle_error = 0;
    while (g_calib_info.is_finish_calc_angle_error == 0)
    {
        if (g_calib_info.acc.angle_is_done == 1)
        {   
            /* Assign Accumulator to Storage */
            g_calib_info.storage.power0_active  = EM_CALIB_GetAccumulatedValue(
                                                    &g_calib_info.acc.power0_active
                                                  );
            g_calib_info.storage.power1_active  = EM_CALIB_GetAccumulatedValue(
                                                    &g_calib_info.acc.power1_active
                                                  );
            g_calib_info.storage.npower         = g_calib_info.acc.npower;
            
            //DEBUG_Printf((uint8_t *)"\n\r Offset I-signal: %.3f", (float32_t)g_calib_info.offset.i.average);
            //DEBUG_Printf((uint8_t *)"\n\r Offset V-signal: %.3f", (float32_t)g_calib_info.offset.v.average);
            
            /* delta error (x) = (cos (-phi + theta) / cos (phi + theta) - 1) */
            delta_error  = (g_calib_info.storage.power0_active / g_calib_info.storage.power1_active);
            delta_error -= 1.0f;
            
            /* For 3phase system, current must be leading voltage */
            if (g_calib_info.theta > 0.0f)
            {
                //DEBUG_Printf((uint8_t *)"\n\r Theta > 0, check the software delay\n\r");
                rlt_code = EM_CALIB_ERROR;
                break;
            }
            
            /* Stop if reach max delay time ? */
            if (ABS(g_calib_info.theta) > EM_CALIB_PHASE_ERROR_MAX_ANGLE_ERR)
            {
                g_calib_info.theta = -90.0f;
                rlt_code = EM_CALIB_ERROR;
                break;
            }
            /* Check the range of delta_error, too large */
            else if (delta_error < EM_CALIB_PHASE_ERROR_MIN_THRESHOLD ||
                     delta_error > EM_CALIB_PHASE_ERROR_MAX_THRESHOLD)
            {
                g_calib_info.theta = -180.0f;
                rlt_code = EM_CALIB_ERROR;
                break;
            }
            
            /*
             * Get the initial delta_error, solve for theta,
             * set last_delta_error then start the approximation
             * with the initial theta.
             */
            if (g_calib_info.initial_delta_error == 0.0f)
            {
                /*
                 * Solve for theta
                 * 
                 * theta = delta_error / [(delta_error + 2) * tanf(C)]
                 */
                g_calib_info.theta = (
                    delta_error / (
                        (delta_error + 2) *
                        EM_CALIB_TANC
                    )
                );
                
                /* Change to degree */
                g_calib_info.theta *= 180.0f;
                g_calib_info.theta /= EM_CALIB_PI;
                
                /* Look up the derivation */
                g_calib_info.derivation = EM_CALIB_LookupDerivation(g_calib_info.theta);
                
                g_calib_info.initial_delta_error = \
                g_calib_info.last_delta_error    = delta_error;
                //DEBUG_Printf((uint8_t *)"\n\r Theta: %f", g_calib_info.theta);
            }
            /* have large error & not change sign ?*/
            else if ((100 * fabsf(delta_error)) >= EM_CALIB_PHASE_ERROR_STOP_THRESHOLD &&
                     (100.0f * delta_error * g_calib_info.last_delta_error > 0.0f))
            {
                /*
                 * Calculate theta, min. grow up is 1step of Phase control register &
                 * Update error, derivation for next approximation
                 */
                g_calib_info.last_delta_error   = delta_error;
                g_calib_info.last_theta         = g_calib_info.theta;
                g_calib_info.theta             -= (
                    ((delta_error / g_calib_info.derivation) * 180.0f) /
                    EM_CALIB_PI
                );
                g_calib_info.derivation         = EM_CALIB_LookupDerivation(g_calib_info.theta);
                //DEBUG_Printf((uint8_t *)"\n\r Theta: %f", g_calib_info.theta);
            }
            /* archive small error */
            else
            {
                /*
                 * roll back to the last when new error rate larger than the old
                 */
                if (fabsf(g_calib_info.last_delta_error) < fabsf(delta_error))
                {
                    g_calib_info.theta = g_calib_info.last_theta;
                }
                
                g_calib_info.is_finish_calc_angle_error = 1;    /* Finish calculate angle error */
                break;
            }
            /* Set new calculated phase angle error */
            if (g_calib_step == EM_CALIB_ACCUMULATE_STEP_PHASE_ANGLE)
            {
                EM_CALIB_ADC_SetAngleError(g_calib_info.theta);
                //DEBUG_Printf((uint8_t *)"\n\r DSADPHCR: %d", *(&DSADPHCR0 + g_calib_channel));
            }
            
            EM_CALIB_ResetPhaseShiftCircle();
            
            g_calib_info.number_of_cycles = 0;  
            g_calib_info.v_start_flag  = 0;
            
            /* Reset accumulator */
            EM_ACC24_Clear(&g_calib_info.acc.power0_active);
            EM_ACC24_Clear(&g_calib_info.acc.power1_active);
            g_calib_info.acc.npower = 0;

            /* Reset accumulation control flag */
            g_calib_info.acc.angle_is_done = 0;         
        }       
        EM_WDT_Restart();
    }

    return rlt_code;
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForAngle90Error
* Interface     : static uint8_t EM_CALIB_SamplingForAngle90Error(uint8_t steps)
* Description   : Accumulate and scan angle error
* Arguments     : steps : choose calibration steps
* Function Calls: EM_CALIB_GetAccumulatedValue()
*               : EM_CALIB_LookupDerivation()
*               : EM_CALIB_ADC_SetAngleError()
*               : EM_CALIB_ADC_SetAngleError()
*               : EM_CALIB_ResetPhaseShiftCircle()
*               : EM_ACC24_Clear()
* Return Value  : uint8_t, 0 is OK, others are fail
******************************************************************************/
#define CPU_CLK                 (24000000.0f)
#define TAU_TDR                 (TDR01)

static uint8_t EM_CALIB_DoAccumulateAngle90Error(uint8_t steps)
{
    volatile float32_t   step_offset, us_offset, reg_offset;
    volatile float32_t   delta_degree;
    volatile uint8_t     rlt_code    = EM_CALIB_OK;
    volatile float32_t backup_vrms, backup_i1rms;
    volatile float32_t apparent_like;
    volatile float32_t require_steps;
    volatile float32_t require_fsteps;
    volatile float32_t require_tdr_setting;
    volatile int16_t inital_reg;
    /* Set accumulation steps */
    g_calib_step = steps;
    
    /* Reset data storage */
    g_calib_info.number_of_cycles = 0;  
    g_calib_info.v_start_flag  = 0;
    
    /* Clear accumulator */
    /* Voltage signal */
    EM_ACC24_Clear(&g_calib_info.acc.vrms);
    g_calib_info.acc.nvrms = 0;
    /* Phase signal */
    EM_ACC24_Clear(&g_calib_info.acc.i1rms);
    g_calib_info.acc.ni1rms = 0;
    /* Power0 signal */
    EM_ACC24_Clear(&g_calib_info.acc.power0_active);
    g_calib_info.acc.npower = 0;
    
    g_calib_info.theta90 = 0;
    
    /* Wait until finish accumulation */
    g_calib_info.acc.angle_is_done = 0; /* Control sampling for each scanning time */
    g_calib_info.is_finish_calc_angle_error = 0;
    
    /* Backup the VRMS and IRMS */
    backup_vrms = g_calib_info.storage.vrms;
    backup_i1rms = g_calib_info.storage.i1rms;
    
    /* Calculate the ideal require steps */
    EM_ADC_SAMP_UpdateStep(EM_CALIB_LINE_FREQUENCY);
    
    require_steps = (90.0f * EM_ADC_DRIVER_IDEAL_SAMPLING_FREQUENCY)/
                        (360 * EM_CALIB_LINE_FREQUENCY);
    
    require_fsteps = 1 - (require_steps - (uint32_t)require_steps);
    
    require_tdr_setting = (require_fsteps / EM_ADC_DRIVER_IDEAL_SAMPLING_FREQUENCY) * (float32_t)CPU_CLK;
    
    TAU_TDR = (uint16_t)require_tdr_setting;
    
    inital_reg = TAU_TDR;
    
    //DEBUG_Printf((uint8_t *)"\n\r Initial TDR value: %u", TAU_TDR);          
    
            
    while (g_calib_info.is_finish_calc_angle_error == 0)
    {
        if (g_calib_info.acc.angle_is_done == 1)
        {   
            /* Assign Accumulator to Storage */
            g_calib_info.storage.power0_active  = EM_CALIB_GetAccumulatedValue(
                                                    &g_calib_info.acc.power0_active
                                                  );
                                                  
            g_calib_info.storage.vrms   = EM_CALIB_GetAccumulatedValue(
                                                    &g_calib_info.acc.vrms
                                                  );    
                                                  
            g_calib_info.storage.i1rms  = EM_CALIB_GetAccumulatedValue(
                                                    &g_calib_info.acc.i1rms
                                                  );
                                                  
            g_calib_info.storage.npower         = g_calib_info.acc.npower;
            
            /*******************************************
             * Calculation method:
             * Apparent power   = VRMS*IRMS
             * Active power     = VRMS*IRMS*cos(theta)
             *                      Active
             * ==> theta = acos(---------------)
             *                      Apparent
             *
             * ****************************************/

            //DEBUG_Printf((uint8_t *)"\n\r Offset I-signal: %.3f", (float32_t)g_calib_info.offset.i.average);
            //DEBUG_Printf((uint8_t *)"\n\r Offset V-signal: %.3f", (float32_t)g_calib_info.offset.v.average);             
             
            g_calib_info.storage.vrms = EM_sqrt( g_calib_info.storage.vrms /
                                                    g_calib_info.storage.npower);
            
            g_calib_info.storage.i1rms = EM_sqrt( g_calib_info.storage.i1rms /
                                                    g_calib_info.storage.npower);                                       
                                                    
            g_calib_info.storage.power0_active = (g_calib_info.storage.power0_active /
                                                    g_calib_info.storage.npower);
                                                    
            apparent_like = g_calib_info.storage.vrms* g_calib_info.storage.i1rms;
            
            g_calib_info.theta90 = g_calib_info.storage.power0_active/ apparent_like;
            
            g_calib_info.theta90 = acos(g_calib_info.theta90);
            
            g_calib_info.theta90 *= 180.0f;
            g_calib_info.theta90 /= EM_CALIB_PI;    
            
            delta_degree = 90.0f - g_calib_info.theta90;
            
            //DEBUG_Printf((uint8_t *)"\n\r Theta: %f", g_calib_info.theta90);
            
            if ( delta_degree > 2.4f || delta_degree < -2.4f)
            {
                rlt_code = EM_CALIB_ERROR;
                break;
            }
            else if (delta_degree >= 0.010f || delta_degree <= -0.010f)
            {
                /*********************************************************************
                 * Calculate the new value of TDR by ratio to target degree:
                 *
                 * dtheta = 90 - measured_theta
                 *
                 *
                 *              dtheta * fs
                 * dstep = ----------------------
                 *               360 * fac
                 *        
                 *              dstep * 10e5 
                 * dus = ------------------------
                 *                  fs
                 *
                 *                  dus
                 * dTDR = -----------------------
                 *          (1/CPU_CLK) * 10e5
                 *
                 *
                 *              dtheta * CPUCLK
                 * ==> dTDR = -------------------
                 *                 360 * fac
                 *
                 * TDR(n+1) = TDR(n) - dTDR
                 *
                 *******************************************************************/
                reg_offset = (delta_degree * CPU_CLK)/(360.0f * EM_CALIB_LINE_FREQUENCY);
                /* Scaling with rise coefficient */
                reg_offset = reg_offset * 0.7;
                TAU_TDR -= reg_offset;
            }
            else /* Within error range, save the result and exit */
            {
                reg_offset = (inital_reg - (int16_t)TAU_TDR);
                g_calib_info.theta90_timer_offset = reg_offset;
                g_calib_info.is_finish_calc_angle_error = 1;
                TAU_TDR = inital_reg;   /* restore the initial setting */

                /* Restore the VRMS and IRMS */
                g_calib_info.storage.vrms = backup_vrms ;
                g_calib_info.storage.i1rms = backup_i1rms ;
    
                break;
            }
            
            g_calib_info.number_of_cycles = 0;  
            g_calib_info.v_start_flag  = 0;
            
            /* Reset accumulator */
            EM_ACC24_Clear(&g_calib_info.acc.vrms);
            EM_ACC24_Clear(&g_calib_info.acc.i1rms);
            EM_ACC24_Clear(&g_calib_info.acc.power0_active);
            g_calib_info.acc.nvrms = 0;
            g_calib_info.acc.ni1rms = 0;
            g_calib_info.acc.npower = 0;

            //DEBUG_Printf((uint8_t *)"\n\r TDR: %d", TAU_TDR);
            
            /* Reset accumulation control flag */
            g_calib_info.acc.angle_is_done = 0;         
        }   
        EM_WDT_Restart();
    }

    return rlt_code;
}


/******************************************************************************
* Function Name : EM_CALIB_Sampling
* Interface     : void EM_CALIB_Sampling(void)
* Description   : Do sampling for calibration module
*               : Callback function which is used to register into ADC ISR
* Arguments     : None
* Function Calls: EM_CALIB_SamplingForFs()
*               : EM_CALIB_SamplingForCalcGain()
*               : EM_CALIB_SamplingForAngleError()
*               : EM_CALIB_SamplingForAccSignals()
* Return Value  : None
******************************************************************************/
void EM_CALIB_Sampling(__near EM_SAMPLES * p_samples)
{
    if (g_calib_line == 0)
    {
        g_sample_v      = (int16_t)p_samples->phase_r.v;
        g_sample_v90    = (int16_t)p_samples->phase_r.v90;
        g_sample_i      = p_samples->phase_r.i;
    }
    else if (g_calib_line == 1)
    {
        g_sample_v      = (int16_t)p_samples->phase_y.v;
        g_sample_v90    = (int16_t)p_samples->phase_y.v90;
        g_sample_i      = p_samples->phase_y.i;
    }
    else if (g_calib_line == 2)
    {
        g_sample_v      = (int16_t)p_samples->phase_b.v;
        g_sample_v90    = (int16_t)p_samples->phase_b.v90;
        g_sample_i      = p_samples->phase_b.i;
    }
    else
    {
        g_sample_i      = p_samples->neutral.i;
    }
    
    EM_CALIB_SamplingUpdateOffset();
    
    g_sample_v = RL78I1C_DoSoftwareHPF16(g_sample_v, 0);
    g_sample_v90 = RL78I1C_DoSoftwareHPF16(g_sample_v90, 1);
    
    if (g_calib_info.stable_count <  EM_CALIB_SIGNAL_CYCLE_FOR_STABILITY)
    {
        g_calib_info.stable_count++;
        return;
    }
    
    
    if (g_calib_step == EM_CALIB_ACCUMULATE_STEP_FS)
    {
        EM_CALIB_SamplingForFs();
    }
    else if (g_calib_step == EM_CALIB_ACCUMULATE_STEP_IGAIN)
    {
        EM_CALIB_SamplingForCalcGain();
    }
    else if ((g_calib_step == EM_CALIB_ACCUMULATE_STEP_PHASE_ANGLE) ||
             (g_calib_step == EM_CALIB_ACCUMULATE_STEP_NEUTRAL_ANGLE))
    {
        EM_CALIB_SamplingForAngleError();
    }
    else if ((g_calib_step == EM_CALIB_ACCUMULATE_STEP_V90_ANGLE))
    {
        EM_CALIB_SamplingForAngle90Error();
    }
    else if ((g_calib_step >= EM_CALIB_ACCUMULATE_STEP_V)   &&
             (g_calib_step <= ( EM_CALIB_ACCUMULATE_STEP_V  | 
                                EM_CALIB_ACCUMULATE_STEP_IP | 
                                EM_CALIB_ACCUMULATE_STEP_IN
                              ))
            )
    {
        EM_CALIB_SamplingForAccSignals();
    }
}


/******************************************************************************
* Function Name : EM_CALIB_SamplingForFs
* Interface     : static void EM_CALIB_SamplingForFs(void)
* Description   : Count up the number of ADC ISR in 1s
* Arguments     : None
* Function Calls: None
* Return Value  : Total number of ADC ISR
******************************************************************************/
static void EM_CALIB_SamplingUpdateOffset(void)
{
    volatile int32_t v, i, v90;
    volatile uint8_t zx;
    static int32_t v_old;
    
    g_calib_info.offset.v.acc       += g_sample_v;
    g_calib_info.offset.i.acc       += g_sample_i;
    
    v = g_sample_v;
    v90 = g_sample_v90;
    i = g_sample_i;
    
    v   -= g_calib_info.offset.v.average;
    v90 -= g_calib_info.offset.v.average;
    i   -= g_calib_info.offset.i.average;
    
    g_calib_info.offset.number_of_samples++;
    
    if ((v >= 0) && (v_old < 0))
    {
        g_calib_info.offset.number_of_cycles++;
    }
    v_old = v;
    
    if (g_calib_info.offset.number_of_cycles >= EM_CALIB_OFFSET_CYCLE_FOR_AVERAGE ||
        g_calib_info.offset.number_of_samples >= EM_CALIB_OFFSET_DEFAULT_NSAMPLE)
    {
        g_calib_info.offset.v.average = (
            g_calib_info.offset.v.acc / g_calib_info.offset.number_of_samples
        );
        
        g_calib_info.offset.v.average = (g_calib_info.offset.v.average +\
                                        g_calib_info.offset.v.average_prev ) / 2;
        
        g_calib_info.offset.v.average_prev = g_calib_info.offset.v.average;                     
               
        g_calib_info.offset.i.average = (
            g_calib_info.offset.i.acc / g_calib_info.offset.number_of_samples
        );
        
        g_calib_info.offset.i.average = (g_calib_info.offset.i.average +\
                                        g_calib_info.offset.i.average_prev ) / 2;
        
        g_calib_info.offset.i.average_prev = g_calib_info.offset.i.average; 
        
        g_calib_info.offset.v_count++;
        
        /* Check for offset ready */
        if (g_calib_info.offset.v_count >= EM_CALIB_OFFSET_STABILIZATION_COUNT)
        {
            g_calib_info.offset.v_ready = 1;
            g_calib_info.offset.v_count = 0;
        }
        
        g_calib_info.offset.v.acc = 0;
        g_calib_info.offset.i.acc = 0;
        g_calib_info.offset.number_of_cycles = 0;           
        g_calib_info.offset.number_of_samples = 0;
    }
    
    return;
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForFs
* Interface     : static void EM_CALIB_SamplingForFs(void)
* Description   : Count up the number of ADC ISR in 1s
* Arguments     : None
* Function Calls: None
* Return Value  : Total number of ADC ISR
******************************************************************************/
static void EM_CALIB_SamplingForFs(void)
{
    MCU_Delay(20);
    if (g_calib_is_start_count_sample == 1)
    {
        g_calib_info.fs.nsample++;
    }
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForCalcGain
* Interface     : static void EM_CALIB_EM_CALIB_SamplingForCalcGain(void)
* Description   : Control sampling process for calculating current gain
* Arguments     : None
* Function Calls: EM_ADC_GetSamples()
* Return Value  : Max value of i1 sample and i2 sample
******************************************************************************/
static void EM_CALIB_SamplingForCalcGain(void)
{
    static uint16_t nsample = 0;

    if (g_calib_info.gain.idone == 1)
    {
        return;
    }
    
    if (g_calib_info.gain.is_start == 0)
    {
        g_calib_info.gain.is_start = 1;
        /* Set initial value for imax and i1min */
        g_calib_info.gain.imax = g_sample_i;
    }
    
    /* Check starting point */
    if (g_calib_info.gain.is_start == 1)
    {
        /* Find max value of i1 */
        if (g_calib_info.gain.imax < g_sample_i)
        {
            g_calib_info.gain.imax = g_sample_i;
        }
        
        /* Increase the number of current sample */
        nsample++;
        
        if (nsample >= EM_CALIB_NUM_SAMPLE_FOR_CALC_GAIN)
        {
            /* Set flag to confirm finish sampling process */
            g_calib_info.gain.idone = 1;
            /* Reset start flag for g_sample_i */
            g_calib_info.gain.is_start = 0;
            nsample = 0;
        }   
    }       
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForAngleErr
* Interface     : static void EM_CALIB_SamplingForAngleErr(void)
* Description   : Sampling for calculate angle error
* Arguments     : None
* Function Calls: EM_ADC_GetSamples()
*               : EM_CALIB_DoPhaseShift0()
*               : EM_CALIB_DoPhaseShift1()
*               : EM_ACC24_Add()
* Return Value  : None
******************************************************************************/
static void EM_CALIB_SamplingForAngleError(void)
{
    volatile int32_t     v_shifted       = 0;
    volatile int32_t     i_shifted       = 0;

    volatile uint16_t    i_mask = 0;
    volatile uint16_t    i_shifted_mask = 0;
    volatile uint16_t    v_mask = 0;
    volatile uint16_t    v_shifted_mask = 0;
    
    volatile uint8_t     zx = 0; /* ZX flag */
    volatile static uint16_t num_sample = 0;
    volatile static int32_t  v_old = 0;
    
//    if (g_calib_info.offset.v_ready == 1)       /* Offset remmoval when v offset is ready */
//    {
//        g_sample_v -= g_calib_info.offset.v.average;            /* Remove offset */
//        g_sample_i -= g_calib_info.offset.i.average;            /* Remove offset */
//    }
//    else
//    {
//        return; 
//    }
    
    if ((g_calib_info.is_finish_calc_angle_error == 1) ||
        (g_calib_info.acc.angle_is_done == 1))
    {
        return;
    }
    
    /* Shift voltage and current to create power0 and power1 */
    v_shifted = EM_CALIB_DoPhaseShift0(g_sample_v, EM_CALIB_CIRCLE_DELAY_STEP); /* v shifted - ~59.90 : i lead v */
    i_shifted = EM_CALIB_DoPhaseShift1(g_sample_i, EM_CALIB_CIRCLE_DELAY_STEP); /* i shifted - ~59.90 : i lag v */
    
    /* Detect ZX */
    if ((g_sample_v >= 0) && (v_old < 0))
    {
        zx = 1;
        if ((g_calib_phase_shift0_ready == 1)   &&  /* Finish shifting to calculate power 0 */
            (g_calib_phase_shift1_ready == 1)   &&  /* Finish shifting to calculate power 1 */
            (g_calib_info.v_start_flag  == 0))      /* Haven't start sampling process */
        {
            g_calib_info.v_start_flag = 1;
            g_calib_phase_shift0_ready = 0;
            g_calib_phase_shift1_ready = 0;
        }
    }
    v_old = g_sample_v;
    
    if (g_calib_info.v_start_flag == 1) /* Start sampling process */
    {
        /* Count up number of line cycle */
        if (zx == 1)
        {
            g_calib_info.number_of_cycles++;
        }
        
        if (g_calib_info.number_of_cycles < g_calib_number_cycles_phase_error)
        {
            /*
             * Calculate Power0 = v_shifted * i
             */
            EM_ACC24_Add(&g_calib_info.acc.power0_active, v_shifted, g_sample_i);

            /*
             * Calculate Power1 = g_sample_v * i_shifted
             */
            EM_ACC24_Add(&g_calib_info.acc.power1_active, g_sample_v, i_shifted);

            /* Count up number of samples */
            num_sample++;
        }
        else
        {
            /* Assign total number samples */
            g_calib_info.acc.npower = num_sample;
            num_sample = 0;
            /* Set flag to confirm finish 1 sampling turn */
            g_calib_info.acc.angle_is_done = 1;
            v_old = 0;
        }
    }
    zx = 0;
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForAngle90Error
* Interface     : static void EM_CALIB_SamplingForAngle90Error(void)
* Description   : Sampling for calculate angle error between v and v90
* Arguments     : None
* Function Calls: EM_ADC_GetSamples()
*               : EM_ACC24_Add()
* Return Value  : None
******************************************************************************/
static void EM_CALIB_SamplingForAngle90Error(void)
{
    volatile uint16_t    i_mask = 0;
    volatile uint16_t    v90_mask = 0;
    
    volatile uint8_t     zx = 0; /* ZX flag */
    static uint16_t num_sample = 0;
    static int32_t  v_old = 0;
    
    if ((g_calib_info.is_finish_calc_angle_error == 1) ||
        (g_calib_info.acc.angle_is_done == 1))
    {
        return;
    }
        
    /* Detect ZX */
    if ((g_sample_v90 >= 0) && (v_old < 0))
    {
        zx = 1;
        if ((g_calib_info.v_start_flag  == 0))      /* Haven't start sampling process */
        {
            g_calib_info.v_start_flag = 1;
        }
    }
    v_old = g_sample_v90;
    
    if (g_calib_info.v_start_flag == 1) /* Start sampling process */
    {
        /* Count up number of line cycle */
        if (zx == 1)
        {
            g_calib_info.number_of_cycles++;
        }
        
        if (g_calib_info.number_of_cycles < g_calib_number_cycles_phase_error)
        {
            
            EM_ACC24_Add(&g_calib_info.acc.vrms, g_sample_v90, g_sample_v90);
            
//          EM_ACC24_Add(&g_calib_info.acc.i1rms, g_sample_i, g_sample_i);
            EM_ACC24_Add(&g_calib_info.acc.i1rms, g_sample_v, g_sample_v);
            
            /*
             * Calculate Power0 = v_shifted * g_sample_i
             */
            /* All signals are LSB ? */
            EM_ACC24_Add(&g_calib_info.acc.power0_active, g_sample_v90, g_sample_v);

            /* Count up number of samples */
            num_sample++;
        }
        else
        {
            /* Assign total number samples */
            g_calib_info.acc.npower = num_sample;
            num_sample = 0;
            /* Set flag to confirm finish 1 sampling turn */
            g_calib_info.acc.angle_is_done = 1;
            v_old = 0;
        }
    }
    zx = 0;
}

/******************************************************************************
* Function Name : EM_CALIB_SamplingForAccSignals
* Interface     : static void EM_CALIB_SamplingForAccSignals(void)
* Description   : Control sampling process for accumulation
* Arguments     : None
* Function Calls: EM_ADC_GetSamples()
*               : EM_ACC24_Add()
* Return Value  : None
******************************************************************************/
static void EM_CALIB_SamplingForAccSignals(void)
{
    volatile uint8_t         v_zx                = 0;    /* ZX flag for volt */
    volatile uint8_t         i_zx                = 0;    /* ZX flag for phase */
    static int32_t  v_old               = 0;    /* Detect ZX of voltage signal */
    static int32_t  i_old               = 0;    /* Detect ZX of phase signal */
    
    static uint16_t v_num_cycle         = 0;    /* Line counter for volt signal */
    static uint16_t i_num_cycle         = 0;    /* Line counter for phase signal */
    static uint16_t v_num_sample        = 0;
    static uint16_t i_num_sample        = 0;
    
    static uint8_t  step_mask   = 0;
    
    volatile uint16_t     v_mask = 0;
    volatile uint16_t    i_mask = 0;
    
    if (g_calib_info.acc.is_done == 1)
    {
        return;
    }
    
    /* Accumulated voltage signal */
    if (((g_calib_step  & EM_CALIB_ACCUMULATE_STEP_V) != 0) && 
        ((step_mask     & EM_CALIB_ACCUMULATE_STEP_V) == 0))
    
    {
        /* Detect ZX */
        if ((g_sample_v >= 0) && (v_old < 0))
        {
            v_zx = 1;
            if (g_calib_info.v_start_flag == 0)
            {
                g_calib_info.v_start_flag = 1;
            }
        }
        v_old = g_sample_v;
        
        /* Check starting point */
        if (g_calib_info.v_start_flag == 1)
        {
            /* Accumulate into LSB */
            EM_ACC24_Add(&g_calib_info.acc.vrms, g_sample_v, g_sample_v);
            
            /* Increase number of sample */
            v_num_sample++;
        
            /* Check ZX */
            if (v_zx == 1)
            {
                v_num_cycle++;
            }
            /* The total number of cycles equal expected line cycles */
            if (v_num_cycle >= g_calib_number_cycles)
            {
                /* Assign total number of sample */
                g_calib_info.acc.nvrms = v_num_sample;
                v_num_sample = 0;
                g_calib_info.v_start_flag = 0;
                v_num_cycle = 0;
                v_old = 0;
                /* Change mask */
                step_mask |= EM_CALIB_ACCUMULATE_STEP_V;
            }   
        }               
    }
    
    /* Accumulated phase current signal */
    if (((g_calib_step  & EM_CALIB_ACCUMULATE_STEP_IP) != 0)    && 
        ((step_mask     & EM_CALIB_ACCUMULATE_STEP_IP) == 0))
    
    {
        /* Detect ZX */
        if ((g_sample_i >= 0) && (i_old < 0))
        {
            i_zx = 1;
            if (g_calib_info.i1_start_flag == 0)
            {
                g_calib_info.i1_start_flag = 1;
            }
        }
        i_old = g_sample_i;
        
        /* Check starting point */
        if (g_calib_info.i1_start_flag == 1)
        {
            /* Accumulate */
            EM_ACC24_Add(&g_calib_info.acc.i1rms, g_sample_i, g_sample_i);
            
            /* Increase number of sample */
            i_num_sample++;
        
            /* Check ZX */
            if (i_zx == 1)
            {
                i_num_cycle++;
            }
            /* The total number of cycles equal expected line cycles */
            if (i_num_cycle >= g_calib_number_cycles)
            {
                g_calib_info.acc.ni1rms = i_num_sample;
                i_num_sample = 0;
                g_calib_info.i1_start_flag = 0;
                i_num_cycle = 0;
                i_old = 0;
                /* Change mask */
                step_mask |= EM_CALIB_ACCUMULATE_STEP_IP;
            }   
        }               
    }

    v_zx = 0;
    i_zx = 0;
    
    /* Check sampling status */
    if (step_mask == g_calib_step)
    {
        g_calib_info.acc.is_done = 1;
        step_mask = 0;
    }
}

/******************************************************************************
* Function Name : EM_CALIB_RTCConstantInterruptCallback
* Interface     : void EM_CALIB_RTCConstantInterruptCallback(void)
* Description   : Callback function, which is used to registered into driver layer
* Arguments     : None
* Function Calls: None
* Return Value  : None
******************************************************************************/
void EM_CALIB_RTCConstantInterruptCallback(void)
{
    static uint8_t is_start_count_time = 0;
    static uint8_t rtc_int_counter = 0;
    
    if (g_calib_start_count_1s == 0)
    {
        return;
    }
    
    if (is_start_count_time == 0)   /* First time detect RTC interrupt */
    {
        is_start_count_time = 1;    
        /* Start count sample */
        g_calib_is_start_count_sample = 1;  
        /* Set RTC counter value */
        #if ((defined EM_CALIB_RTC_CONSTANT_PERIOD) && (EM_CALIB_RTC_CONSTANT_PERIOD == 1000))
            rtc_int_counter = 1;
        #elif ((defined EM_CALIB_RTC_CONSTANT_PERIOD) && (EM_CALIB_RTC_CONSTANT_PERIOD == 500))
            rtc_int_counter = 2;
        #else
            #error "Just choose RTC constant interrupt ONESEC or HALFSEC"
        #endif
    }
    else    /* From the second time detect RTC interupt */
    {
        rtc_int_counter--;
        if (rtc_int_counter == 0)
        {
            g_calib_is_start_count_sample = 0;
            is_start_count_time = 0;
            /* Set flag to confirm finish couting fs */
            g_calib_info.fs.is_done = 1;            
        }       
    }   
}
