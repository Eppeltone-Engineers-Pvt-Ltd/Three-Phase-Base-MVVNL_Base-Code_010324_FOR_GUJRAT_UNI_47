/***********************************************************************************************************************
* File Name    : wrp_em_adc.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : ADC Wrapper for RL78/I1C Platform
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_dsadc.h"

#include "wrp_em_sw_config.h"   /* Wrapper Shared Configuration */
#include "wrp_em_adc.h"         /* Wrapper ADC definitions */
#include "wrp_em_adc_data.h"
#include "wrp_mcu.h"
#include "em_core.h"
#include "platform.h"
#include "r_calib.h"

#include "rl78i1c_sw_hpf.h"
#include "r_dscl_iirbiquad_i32_1.h"
#include "r_dscl_iirbiquad_i32_2.h"

#include "r_dscl_iirbiquad_config.h"
#include "rl78i1c_hpf_config.h"

/* Application */
#include "service.h"
#include "emeter3_structs.h"
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
/* CUSTOM DEFINITION FOR OPTIMIZATION */
#define DSADCR0_R           (0x3E0)
#define DSADCR1_R           (0x3E4)
#define DSADCR2_R           (0x3E8)
#define DSADCR3_R           (0x3EC)


#if (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_R == 0)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_R      DSADCR0_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_R == 1)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_R      DSADCR1_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_R == 2)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_R      DSADCR2_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_R == 3)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_R      DSADCR3_R
#endif

#if (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_Y == 0)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_Y      DSADCR0_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_Y == 1)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_Y      DSADCR1_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_Y == 2)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_Y      DSADCR2_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_Y == 3)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_Y      DSADCR3_R
#endif

#if (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_B == 0)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_B      DSADCR0_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_B == 1)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_B      DSADCR1_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_B == 2)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_B      DSADCR2_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_B == 3)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_B      DSADCR3_R
#endif

#if (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL == 0)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL      DSADCR0_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL == 1)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL      DSADCR1_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL == 2)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL      DSADCR2_R
#elif (EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL == 3)
    #define EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL      DSADCR3_R
#endif

/* Sample structure */
#define OFFSET_SAMPLE_PHASE_R_I                  (0x0000)
#define OFFSET_SAMPLE_PHASE_R_V                  (0x0004)
#define OFFSET_SAMPLE_PHASE_R_V90                (0x0008)
#define OFFSET_SAMPLE_PHASE_R_FILTERED_I         (0x000C)
#define OFFSET_SAMPLE_PHASE_R_FILTERED_V         (0x0010)
        
#define OFFSET_SAMPLE_PHASE_Y_I                  (0x0014)
#define OFFSET_SAMPLE_PHASE_Y_V                  (0x0018)
#define OFFSET_SAMPLE_PHASE_Y_V90                (0x001C)
#define OFFSET_SAMPLE_PHASE_Y_FILTERED_I         (0x0020)
#define OFFSET_SAMPLE_PHASE_Y_FILTERED_V         (0x0024)
        
#define OFFSET_SAMPLE_PHASE_B_I                  (0x0028)
#define OFFSET_SAMPLE_PHASE_B_V                  (0x002C)
#define OFFSET_SAMPLE_PHASE_B_V90                (0x0030)
#define OFFSET_SAMPLE_PHASE_B_FILTERED_I         (0x0034)
#define OFFSET_SAMPLE_PHASE_B_FILTERED_V         (0x0038)

#define OFFSET_SAMPLE_NEUTRAL_I                  (0x003C)



#define ABS(x)  ((x) <  0  ? (-(x)) : (x))


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

uint32_t calWatt;
uint8_t total_energy_pulse_remaining_time;

#ifdef REACTIVE_PULSING
uint32_t calVar;
uint8_t total_reactive_energy_pulse_remaining_time;
#endif
/*
// power is assumed to be in watts
	Energy per pulse = (3600*1000*SamplingFrequency)/(Meter_Constant)
	Energy per pulse = (3600000*3906.25)/1600 = 8789062
	Energy per pulse = (3600000*3900)/1600 = 8775000
	
// for meter constant of 6400
	Energy per pulse = (3600000*3900)/6400 = 2193750

*/
//#define ENERGY_PULSE_DURATION	115	// 30 millisecond On
#define ENERGY_PULSE_DURATION	100	// 25 millisecond On
//#define ENERGY_PER_PULSE	8789062	// original calculation
//#define ENERGY_PER_PULSE	8775000	// figure used till 28/06/2020
//#define ENERGY_PER_PULSE	8924175	// increased by 1.7% on 29/Jun/2020 - 29/06/2020

#ifdef METER_CONSTANT_6400
//	#define ENERGY_PER_PULSE	2193750	
	#define ENERGY_PER_PULSE	21937500	// pulsing power is now in deciwatts	- 6400imp/kwh
#else
	#define ENERGY_PER_PULSE	87750000	// restored on 29/06/2020 at 14:03 hoours - 1600 imp/kwh
#endif

#define ACTIVE_PULSE_LED_ON	{P0.5 = 0;}
#define ACTIVE_PULSE_LED_OFF	{P0.5 = 1;}
#define REACTIVE_PULSE_LED_ON	{P0.4 = 0;}
#define REACTIVE_PULSE_LED_OFF	{P0.4 = 1;}

#ifdef __DEBUG
uint8_t g_enable_auto_fstep;

#else
const uint8_t g_enable_auto_fstep = 1;

#endif

/* Static I/F for this module */
/* Changing gain, apply gain value to the port pin */
static void EM_ADC_GainPhaseSet(EM_LINE line);              /* Set a suitable gain phase level to a port */


//extern int32_t testCounter;
//extern int16_t innerCounter;



/******************************************************************************
* Function Name    : static void EM_ADC_GainPhaseSet(EM_LINE line)
* Description      : Set a suitable gain phase level to a port
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
static void EM_ADC_GainPhaseSet(EM_LINE line)
{
    uint8_t current_level = g_EM_ADC_GainPhaseCurrentLevel[line];
    dsad_channel_t channel = g_wrp_adc_channel[line];
    dsad_gain_t gain = g_wrp_adc_channel_gain[line][current_level];
    
    R_DSADC_SetChannelGain(channel, gain);
    
}


/******************************************************************************
* Function Name    : int32_t RL78I1C_DoIntegrate24(int32_t input, uint8_t channel)
* Description      : Wrap around function for future integrator, not implement yet
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
int32_t RL78I1C_DoIntegrate24(int32_t input, uint8_t channel)
{
    /* Not implement yet */
    return input;
}

/******************************************************************************
* Function Name    : static void EM_ADC_Voltage_CirclePushAndPop(void)
* Description      : Shift input signal as expected step for voltage channels
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
#pragma inline_asm EM_ADC_Voltage_CirclePushAndPop
void EM_ADC_Voltage_CirclePushAndPop(void)
{
    ;Push_pos
    MOVW    AX, !_g_wrp_adc_voltage_circle_pos
    ADDW    AX, AX
    MOVW    BC, AX
    
    MOVW    AX, !_g_wrp_adc_buffer_phase_r_v
    MOVW    _g_wrp_adc_voltage_circle_phase_r[BC], AX
    
    MOVW    AX, !_g_wrp_adc_buffer_phase_y_v
    MOVW    _g_wrp_adc_voltage_circle_phase_y[BC], AX
    
    MOVW    AX, !_g_wrp_adc_buffer_phase_b_v
    MOVW    _g_wrp_adc_voltage_circle_phase_b[BC], AX
    
    ;Pop_pos
    MOVW    AX, [SP+0x00]
    ADDW    AX, AX
    MOVW    BC, AX
    
    MOVW    AX, _g_wrp_adc_voltage_circle_phase_r[BC]
    MOVW    !_g_wrp_adc_buffer_phase_r_v, AX
    
    MOVW    AX, _g_wrp_adc_voltage_circle_phase_y[BC]
    MOVW    !_g_wrp_adc_buffer_phase_y_v, AX
    
    MOVW    AX, _g_wrp_adc_voltage_circle_phase_b[BC]
    MOVW    !_g_wrp_adc_buffer_phase_b_v, AX
}
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
#pragma inline_asm EM_ADC_Voltage90_CirclePushAndPop
void EM_ADC_Voltage90_CirclePushAndPop(void)
{
    ;Push_pos
    MOVW    AX, !_g_wrp_adc_voltage90_circle_pos
    ADDW    AX, AX
    MOVW    BC, AX
    
    MOVW    AX, !_g_wrp_adc_buffer_phase_r_v90
    MOVW    _g_wrp_adc_voltage90_circle_phase_r[BC], AX
    
    MOVW    AX, !_g_wrp_adc_buffer_phase_y_v90
    MOVW    _g_wrp_adc_voltage90_circle_phase_y[BC], AX
    
    MOVW    AX, !_g_wrp_adc_buffer_phase_b_v90
    MOVW    _g_wrp_adc_voltage90_circle_phase_b[BC], AX
    
    ;Pop_pos
    MOVW    AX, [SP+0x00]
    ADDW    AX, AX
    MOVW    BC, AX
    
    MOVW    AX, _g_wrp_adc_voltage90_circle_phase_r[BC]
    MOVW    !_g_wrp_adc_buffer_phase_r_v90, AX
    
    MOVW    AX, _g_wrp_adc_voltage90_circle_phase_y[BC]
    MOVW    !_g_wrp_adc_buffer_phase_y_v90, AX
    
    MOVW    AX, _g_wrp_adc_voltage90_circle_phase_b[BC]
    MOVW    !_g_wrp_adc_buffer_phase_b_v90, AX
    
}
#endif

#pragma noinline EM_ADC_DoPhaseDelayAllVoltage
static void EM_ADC_DoPhaseDelayAllVoltage(void)
{
    volatile int16_t pop_pos;
    
    /* Phase adjustment for V signal */
#if (EM_ADC_DELAY_STEP_VOLTAGE > 0)
    if (g_wrp_adc_voltage_circle_pos <= 0)
    {
        pop_pos = g_wrp_adc_voltage_circle_step;
    }
    else
    {
        pop_pos = g_wrp_adc_voltage_circle_pos - 1;
    }

    /* PUSH: Add the signal into the data circle at circlepos position */
    #if 1
    EM_ADC_Voltage_CirclePushAndPop();
    #else
    g_wrp_adc_voltage_circle_phase_r[g_wrp_adc_voltage_circle_pos] = g_wrp_adc_buffer_phase_r_v;
    g_wrp_adc_voltage_circle_phase_y[g_wrp_adc_voltage_circle_pos] = g_wrp_adc_buffer_phase_y_v;
    g_wrp_adc_voltage_circle_phase_b[g_wrp_adc_voltage_circle_pos] = g_wrp_adc_buffer_phase_b_v;
    
    g_wrp_adc_buffer_phase_r_v = g_wrp_adc_voltage_circle_phase_r[pop_pos];
    g_wrp_adc_buffer_phase_y_v = g_wrp_adc_voltage_circle_phase_y[pop_pos];
    g_wrp_adc_buffer_phase_b_v = g_wrp_adc_voltage_circle_phase_b[pop_pos];
    #endif

    /* POP: Get the signal from data circle from (pos + steps) position */
    
    /* Scroll the circle, scroll left */
    g_wrp_adc_voltage_circle_pos--;
    if (g_wrp_adc_voltage_circle_pos == -1)
    {
        g_wrp_adc_voltage_circle_pos = g_wrp_adc_voltage_circle_step;
    }

#endif

#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE        
    /* Phase adjustment for V90 signal */
#if (EM_ADC_DELAY_VOLTAGE90_MAX_STEP > 0)
    if (g_wrp_adc_voltage90_circle_pos <= 0)
    {
        pop_pos = g_wrp_adc_voltage90_circle_step;
    }
    else
    {
        pop_pos = g_wrp_adc_voltage90_circle_pos - 1;
    }

    #if 1
    EM_ADC_Voltage90_CirclePushAndPop();
    #else
    
    /* PUSH: Add the signal into the data circle at circlepos position */
    
    g_wrp_adc_voltage90_circle_phase_r[g_wrp_adc_voltage90_circle_pos] = g_wrp_adc_buffer_phase_r_v90;
    g_wrp_adc_voltage90_circle_phase_y[g_wrp_adc_voltage90_circle_pos] = g_wrp_adc_buffer_phase_y_v90;
    g_wrp_adc_voltage90_circle_phase_b[g_wrp_adc_voltage90_circle_pos] = g_wrp_adc_buffer_phase_b_v90;

    /* POP: Get the signal from data circle from (pos + steps) position */
    g_wrp_adc_buffer_phase_r_v90 = g_wrp_adc_voltage90_circle_phase_r[pop_pos];
    g_wrp_adc_buffer_phase_y_v90 = g_wrp_adc_voltage90_circle_phase_y[pop_pos];
    g_wrp_adc_buffer_phase_b_v90 = g_wrp_adc_voltage90_circle_phase_b[pop_pos];
    #endif
    
    /* Scroll the circle, scroll left */
    g_wrp_adc_voltage90_circle_pos--;
    if (g_wrp_adc_voltage90_circle_pos == -1)
    {
        g_wrp_adc_voltage90_circle_pos = g_wrp_adc_voltage90_circle_step;
    }
#endif

#endif
}

static void EM_ADC_DoPhaseDelayAllCurrent(void)
{
    /* Not implement for current channel */
}

/******************************************************************************
* Function Name    : void EM_ADC_Init(void)
* Description      : Set a suitable gain neutral level to a port
* Arguments        : None
* Functions Called : EM_ADC_DriverInit()
* Return Value     : None
******************************************************************************/
void EM_ADC_Init(void)
{
    /* Init ADC HW Module */
    EM_ADC_DriverInit();
//		g_system.calib.system.vrms_threshold.phase_r = 1.65E+06;    
    g_wrp_adc_voltage_circle_step = EM_ADC_DELAY_STEP_VOLTAGE;	// 6

#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE    
    g_wrp_adc_voltage90_circle_step = EM_ADC_DELAY_STEP_VOLTAGE90;	// 26
#endif

    /* Enable the fstep compensation */
    #ifdef __DEBUG
    g_enable_auto_fstep = 1;
    #endif

    /* Init the HPF16 */
    RL78I1C_InitSoftwareHPF16(EM_SW_PROPERTY_ADC_HPF_CUTOFF_FREQUENCY);
    
    /* Init the IIR Biquad Filter */
    
}

/******************************************************************************
* Function Name    : void EM_ADC_Start(void)
* Description      : Start ADC sampling
* Arguments        : None
* Functions Called : EM_ADC_DriverStart()
* Return Value     : None
******************************************************************************/
void EM_ADC_Start(void)
{
    EM_CALIBRATION calib = EM_GetCalibInfo();
    
    /* Start ADC HW Module */
    EM_ADC_DriverStart();
    
    /* Get the voltage offset */
    g_wrp_adc_offset_phase_r_v = calib.offset.phase_r.v;
    g_wrp_adc_offset_phase_y_v = calib.offset.phase_y.v;
    g_wrp_adc_offset_phase_b_v = calib.offset.phase_b.v;
}

/******************************************************************************
* Function Name    : void EM_ADC_Stop(void)
* Description      : Stop ADC sampling
* Arguments        : None
* Functions Called : EM_ADC_DriverStop()
* Return Value     : None
******************************************************************************/
void EM_ADC_Stop(void)
{
    /* Init ADC HW Module */
    EM_ADC_DriverStop();
    
    g_wrp_adc_voltage_circle_pos = 0;
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE    
    g_wrp_adc_voltage90_circle_pos = 0;
#endif

    /* Clear filer buffer */
    RL78I1C_ResetSoftwareHPF16(0);
    RL78I1C_ResetSoftwareHPF16(1);
    RL78I1C_ResetSoftwareHPF16(2);
    RL78I1C_ResetSoftwareHPF16(3);
    RL78I1C_ResetSoftwareHPF16(4);
    RL78I1C_ResetSoftwareHPF16(5);
    
    /* Clear DSCL Filter */
    
}

#pragma inline_asm EM_ADC_Current_DriverRead
void EM_ADC_Current_DriverRead(void)
{
    /* Phase_R Current */  
;READ_PHASE_R
    MOVW    AX, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_R)
    MOVW    !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_R_I), AX
    MOV     A, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_R+0x02)
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_R_I+0x02), A
    SAR     A, 7
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_R_I+0x03), A

    /* Phase_Y Current */  
;READ_PHASE_Y    
    MOVW    AX, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_Y)
    MOVW    !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_Y_I), AX
    MOV     A, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_Y+0x02)
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_Y_I+0x02), A
    SAR     A, 7
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_Y_I+0x03), A
    
    /* Phase_B Current */  
;READ_PHASE_B
    MOVW    AX, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_B)
    MOVW    !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_B_I), AX
    MOV     A, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE_B+0x02)
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_B_I+0x02), A
    SAR     A, 7
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_PHASE_B_I+0x03), A

    /* Neutral Current */
;READ_NEUTRAL
    MOVW    AX, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL)
    MOVW    !(_g_wrp_adc_samples+OFFSET_SAMPLE_NEUTRAL_I), AX
    MOV     A, !(EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL+0x02)
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_NEUTRAL_I+0x02), A
    SAR     A, 7
    MOV     !(_g_wrp_adc_samples+OFFSET_SAMPLE_NEUTRAL_I+0x03), A
}


int32_t previousVr;
int32_t previousVy;
int32_t previousVb;
int16_t timeY;
int16_t timeB;
int16_t sampleCount;
uint8_t startZero;
uint8_t gotit;
int8_t gPhaseSequence;

#ifdef CURRENT_PHASE_SEQUENCE
int32_t previousIr;
int32_t previousIy;
int32_t previousIb;
int16_t timeYCurrent;
int16_t timeBCurrent;
int16_t sampleCountCurrent;
uint8_t startZeroCurrent;
uint8_t gotitCurrent;
int8_t gPhaseSequenceCurrent;
#endif

int8_t prevACmagnet;
int8_t acMagnetSense;
int16_t acMagnetAbsenceCtr;
int16_t acMagnetPulseCtr;
int8_t pulsesStartedComingFlag;	// this flag is set on the second falling edge of ac magnet line
//int32_t	testResult;
//int32_t	sigmaI_R;
//int32_t	sigmaI_Y;
//int32_t	sigmaI_B;
//int16_t	sampleCount;

//int16_t VoltSamples[200];


/******************************************************************************
* Function Name    : void EM_ADC_GetSamples(void)
*                  :     int32_t *p_volt,
*                  :     int32_t *p_current1,
*                  :     int32_t *p_current2
*                  : );
* Description      : Get ADC Samples
* Arguments        : EM_SAMPLES * p_sample: pointer to the sample buffer
* Functions Called : EM_ADC_Current_DriverReadToBuffer()
*                  : EM_ADC_Current_DriverRead()
*                  : EM_ADC_Voltage_DriverRead()
*                  : EM_ADC_Voltage90_DriverRead()
*                  : EM_ADC_DoPhaseDelayAllVoltage()
*                  : EM_ADC_CopyToOuput()
* Return Value     : None
******************************************************************************/
void EM_ADC_InterruptCallback(void)
{
	int32_t reactive_power;
  uint16_t pulse_duration;
	
  // Read voltage samples first 
  EM_ADC_Voltage_DriverRead(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R, g_wrp_adc_buffer_phase_r_v);
  EM_ADC_Voltage_DriverRead(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y, g_wrp_adc_buffer_phase_y_v);
  EM_ADC_Voltage_DriverRead(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_B, g_wrp_adc_buffer_phase_b_v);
    
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE   
  if (g_wrp_adc_tdr_ignore == 1)
  {
    // Ignore the V90 samples, get integer steps only 
    g_wrp_adc_buffer_phase_r_v90 = g_wrp_adc_buffer_phase_r_v;
    g_wrp_adc_buffer_phase_y_v90 = g_wrp_adc_buffer_phase_y_v;
    g_wrp_adc_buffer_phase_b_v90 = g_wrp_adc_buffer_phase_b_v;
  }
  else
  {     
    EM_ADC_Voltage90_DriverRead(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R, g_wrp_adc_buffer_phase_r_v90);
    EM_ADC_Voltage90_DriverRead(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y, g_wrp_adc_buffer_phase_y_v90);
    EM_ADC_Voltage90_DriverRead(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_B, g_wrp_adc_buffer_phase_b_v90);
  } 
#endif
    
  //  Get current samples  
  EM_ADC_Current_DriverRead();

// the current samples (int32_t) are available in 
//g_wrp_adc_samples.phase_r.i,
//g_wrp_adc_samples.phase_y.i,
//g_wrp_adc_samples.phase_b.i,


  // Do phase delay: for all voltage channels  
  EM_ADC_DoPhaseDelayAllVoltage();
    
  // Do phase delay: for all current channels  
  EM_ADC_DoPhaseDelayAllCurrent();
    
  // Do HPF to maintain the response with I channel +  
  // Copy the voltage signal to output buffer  
  if (g_calib_is_in_calib_mode == 0)
  {
    // Substract the signal offset and do Software HPF 
    g_wrp_adc_samples.phase_r.v = 
    g_wrp_adc_buffer_phase_r_v = RL78I1C_DoSoftwareHPF16(g_wrp_adc_buffer_phase_r_v-g_wrp_adc_offset_phase_r_v, 0);
    
    g_wrp_adc_samples.phase_y.v = 
    g_wrp_adc_buffer_phase_y_v = RL78I1C_DoSoftwareHPF16(g_wrp_adc_buffer_phase_y_v-g_wrp_adc_offset_phase_y_v, 1);
    
    g_wrp_adc_samples.phase_b.v = 
    g_wrp_adc_buffer_phase_b_v = RL78I1C_DoSoftwareHPF16(g_wrp_adc_buffer_phase_b_v-g_wrp_adc_offset_phase_b_v, 2);
    
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
    g_wrp_adc_samples.phase_r.v90 = 
    g_wrp_adc_buffer_phase_r_v90 = RL78I1C_DoSoftwareHPF16(g_wrp_adc_buffer_phase_r_v90-g_wrp_adc_offset_phase_r_v, 3);
    
    g_wrp_adc_samples.phase_y.v90 = 
    g_wrp_adc_buffer_phase_y_v90 = RL78I1C_DoSoftwareHPF16(g_wrp_adc_buffer_phase_y_v90-g_wrp_adc_offset_phase_y_v, 4);
    
    g_wrp_adc_samples.phase_b.v90 = 
    g_wrp_adc_buffer_phase_b_v90 = RL78I1C_DoSoftwareHPF16(g_wrp_adc_buffer_phase_b_v90-g_wrp_adc_offset_phase_b_v, 5);
#endif
  }
  else
  {
    // Pass sample to output directly, calibration will extract the offset from signal  
    g_wrp_adc_samples.phase_r.v = g_wrp_adc_buffer_phase_r_v;
    g_wrp_adc_samples.phase_y.v = g_wrp_adc_buffer_phase_y_v;
    g_wrp_adc_samples.phase_b.v = g_wrp_adc_buffer_phase_b_v;
    
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
    g_wrp_adc_samples.phase_r.v90 = g_wrp_adc_buffer_phase_r_v90;
    g_wrp_adc_samples.phase_y.v90 = g_wrp_adc_buffer_phase_y_v90;
    g_wrp_adc_samples.phase_b.v90 = g_wrp_adc_buffer_phase_b_v90;
#endif
  }
    
    // Call to middleware integrator for signals: enable this if used with Rogowski coil  
#if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
    // Backup the sample before doing integrator for data logging service  
  #ifndef __DEBUG
    g_datalog_before_integrate_phase_r_i = g_wrp_adc_samples.phase_r.i;
    g_datalog_before_integrate_phase_y_i = g_wrp_adc_samples.phase_y.i;
    g_datalog_before_integrate_phase_b_i = g_wrp_adc_samples.phase_b.i;
    g_datalog_before_integrate_neutral_i = g_wrp_adc_samples.neutral.i;
  #endif
  
  // Integrate the signal from Rogowski coil, API will append later  
  g_wrp_adc_samples.phase_r.i = RL78I1C_DoIntegrate24(g_wrp_adc_samples.phase_r.i, 0);
  g_wrp_adc_samples.phase_y.i = RL78I1C_DoIntegrate24(g_wrp_adc_samples.phase_y.i, 1);
  g_wrp_adc_samples.phase_b.i = RL78I1C_DoIntegrate24(g_wrp_adc_samples.phase_b.i, 2);
  g_wrp_adc_samples.neutral.i = RL78I1C_DoIntegrate24(g_wrp_adc_samples.neutral.i, 3);
    
#endif

#ifdef METER_WRAPPER_ADC_COPY_FUNDAMENTAL_SAMPLE
  // Do the IIRBiquad to filter the input signals, put filterd signal to buffer and the sequence of fundamental samples 
  
  // Run the filter on voltage channel at 3906Hz sampling frequency  
  R_DSCL_IIRBiquad_i32i32_c_nr_1(g_wrp_adc_samples.phase_r.v, &g_wrp_adc_samples.phase_r.v_fund, 0);
  R_DSCL_IIRBiquad_i32i32_c_nr_1(g_wrp_adc_samples.phase_y.v, &g_wrp_adc_samples.phase_y.v_fund, 1);
  R_DSCL_IIRBiquad_i32i32_c_nr_1(g_wrp_adc_samples.phase_b.v, &g_wrp_adc_samples.phase_b.v_fund, 2);
  
  // Run the filter on current channel at 1302Hz sampling frequency  
  switch (g_wrp_adc_samples.status.fund_sequence)
  {
    case EM_SW_FUND_SEQUENCE_PHASE_B:
      R_DSCL_IIRBiquad_i32i32_c_nr_2(g_wrp_adc_samples.phase_r.i, &g_wrp_adc_samples.phase_r.i_fund, 0);
      g_wrp_adc_samples.status.fund_sequence = EM_SW_FUND_SEQUENCE_PHASE_R;
      break;
    case EM_SW_FUND_SEQUENCE_PHASE_R:
      R_DSCL_IIRBiquad_i32i32_c_nr_2(g_wrp_adc_samples.phase_y.i, &g_wrp_adc_samples.phase_y.i_fund, 1);
      g_wrp_adc_samples.status.fund_sequence = EM_SW_FUND_SEQUENCE_PHASE_Y;
      break;
    case EM_SW_FUND_SEQUENCE_PHASE_Y:
      R_DSCL_IIRBiquad_i32i32_c_nr_2(g_wrp_adc_samples.phase_b.i, &g_wrp_adc_samples.phase_b.i_fund, 2);
      g_wrp_adc_samples.status.fund_sequence = EM_SW_FUND_SEQUENCE_PHASE_B;
      break;
    default:
      g_wrp_adc_samples.status.fund_sequence = EM_SW_FUND_SEQUENCE_PHASE_B;
      break;
  }
    
#endif

// Debug the signal  
#ifdef __DEBUG
  #ifndef METER_ENABLE_MEASURE_CPU_LOAD
    if (g_sample_count < EM_ADC_MAX_DEBUG_SAMPLE)
    {
      g_sample_dsad[g_sample_count][0]    = g_wrp_adc_samples.phase_r.i;
      g_sample_adc[g_sample_count][0]     = g_wrp_adc_samples.phase_r.v;
      g_sample_v90[g_sample_count][0]     = g_wrp_adc_buffer_phase_r_v90;
      
      g_sample_dsad[g_sample_count][1]    = g_wrp_adc_samples.phase_y.i;
      g_sample_adc[g_sample_count][1]     = g_wrp_adc_samples.phase_y.v;
      g_sample_v90[g_sample_count][1]     = g_wrp_adc_buffer_phase_y_v90;
      
      g_sample_dsad[g_sample_count][2]    = g_wrp_adc_samples.phase_b.i;
      g_sample_adc[g_sample_count][2]     = g_wrp_adc_buffer_phase_b_v;
      g_sample_v90[g_sample_count][2]     = g_wrp_adc_buffer_phase_b_v90;
      
      g_sample_dsad[g_sample_count][3]    = g_wrp_adc_samples.neutral.i;
      
      g_sample_count++;
	  }
  #endif
#endif
    
// METROLOGY AND EXTERNAL APPLICATION PROCESSING:
// The sample is ready. Put it to each application need
     
  if (g_calib_is_in_calib_mode == 0)
  {
/*
// commented by pravak as this feature not required
    #ifndef __DEBUG
    if ((g_raw_data_service_info.is_enable == 1) &&
        (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM))
    {
        // Raw data log processing  
        SERVICE_FillADCRawData(&g_wrp_adc_samples);
    }
    else
    #endif
*/		

//-------------phase sequence test code start ----------------------
// the voltage samples (int32_t) are available in 
//g_wrp_adc_samples.phase_r.v,
//g_wrp_adc_samples.phase_y.v,
//g_wrp_adc_samples.phase_b.v,

//	VoltSamples[sampleCount++]=g_wrp_adc_samples.phase_r.v;
//	if(sampleCount==200)
//	{
//		sampleCount=0;
//	}


	if(startZero==0)
	{// this is the first zero crossing
		if(g_wrp_adc_samples.phase_r.v>0)
		{
			if(previousVr<0)
			{// here we have a zero crossing for the r phase
				sampleCount=0;	// initialise the reference counter
				startZero=1;
				gotit=1;	// the Rphase incidence count is 0
			}
		}
	}
	previousVr=g_wrp_adc_samples.phase_r.v;

	if(startZero!=0)
	{// the R-phase has already zero crossed
		if(g_wrp_adc_samples.phase_y.v>0)
		{
			if(previousVy<0)
			{// here we have a zero crossing
				timeY = sampleCount;
				gotit|=2;
			}
		}
	}
	previousVy=g_wrp_adc_samples.phase_y.v;
	
	if(startZero!=0)
	{// the R-phase and Y-phase has already zero crossed
		if(g_wrp_adc_samples.phase_b.v>0)
		{
			if(previousVb<0)
			{// here we have a zero crossing
				timeB = sampleCount;
				gotit|=4;
			}
		}
	}
	previousVb=g_wrp_adc_samples.phase_b.v;
	
	sampleCount++;

	if(sampleCount>=70)
	{// within 70 samples of the R-phase zero crossing we should have got the reaining two 
		if(gotit==7)
		{
			if(timeB<timeY)
				gPhaseSequence=1;	// RBY
			else
				gPhaseSequence=0;	// RYB
		}
		startZero=0;	// start all over again
	}
		
	
//-------------phase sequence test code end ----------------------

#ifdef CURRENT_PHASE_SEQUENCE
//-------------current sequence test code start ----------------------

// the current samples (int32_t) are available in 
//g_wrp_adc_samples.phase_r.i,
//g_wrp_adc_samples.phase_y.i,
//g_wrp_adc_samples.phase_b.i,

	if(startZeroCurrent==0)
	{// this is the first zero crossing
		if(g_wrp_adc_samples.phase_r.i>0)
		{
			if(previousIr<0)
			{// here we have a zero crossing for the r phase
				sampleCountCurrent=0;	// initialise the reference counter
				startZeroCurrent=1;
				gotitCurrent=1;	// the Rphase incidence count is 0
			}
		}
	}
	previousIr=g_wrp_adc_samples.phase_r.i;

	if(startZeroCurrent!=0)
	{// the R-phase has already zero crossed
		if(g_wrp_adc_samples.phase_y.i>0)
		{
			if(previousIy<0)
			{// here we have a zero crossing
				timeYCurrent = sampleCountCurrent;
				gotitCurrent|=2;
			}
		}
	}
	previousIy=g_wrp_adc_samples.phase_y.i;
	
	if(startZeroCurrent!=0)
	{// the R-phase and Y-phase has already zero crossed
		if(g_wrp_adc_samples.phase_b.i>0)
		{
			if(previousIb<0)
			{// here we have a zero crossing
				timeBCurrent = sampleCountCurrent;
				gotitCurrent|=4;
			}
		}
	}
	previousIb=g_wrp_adc_samples.phase_b.i;
	
	sampleCountCurrent++;

	if(sampleCountCurrent>=70)
	{// within 70 samples of the R-phase zero crossing we should have got the reaining two 
		if(gotitCurrent==7)
		{
			if(timeBCurrent<timeYCurrent)
				gPhaseSequenceCurrent=1;	// RBY
			else
				gPhaseSequenceCurrent=0;	// RYB
		}
		startZeroCurrent=0;	// start all over again
	}

//-------------current sequence test code end ----------------------
#endif

//--------------ac magnet start -------------------------------
// at every rising edge - we will set the signal high and reset the counter

	if(AC_MAGNET_SENSOR==0)	// ac magnet line
	{
		if (prevACmagnet==1)
		{// here we have a falling pulse
			acMagnetPulseCtr++;
			acMagnetAbsenceCtr=0;
			acMagnetSense=1;
		}
		prevACmagnet=0;
	}
	else
	{// here AC_MAGNET_SENSOR is 1
		if(acMagnetSense==1)
		{// acMagnet has been sensed - and the line is high
			acMagnetAbsenceCtr++;
			if(acMagnetAbsenceCtr>3906)
			{// sense line has remained high for one second - it can't be ac magnet
				acMagnetSense=0;
				acMagnetPulseCtr=0;
				pulsesStartedComingFlag=0;
			}
		}
		prevACmagnet=1;
	}

	if(pulsesStartedComingFlag==0)
	{// we will latch the second pulse - this will be our confirmatory signal
		if(acMagnetPulseCtr>1)
		{// a second or subsequent pulse has been received
			pulsesStartedComingFlag=1;
		}
	}	
//-------------ac magnet end-----------------------------------

//    {
  // Normal processing  
	  EM_ADC_IntervalProcessing(&g_wrp_adc_samples);
//    }
  }
  else
  {
    // Calibration processing  
    EM_CALIB_Sampling(&g_wrp_adc_samples);
  }

//-------------------new LED pulsing start-------------------

  pulse_duration = ENERGY_PULSE_DURATION;
	
//  calWatt += ABS(total_active_power_pulsing);
  calWatt += total_active_power_pulsing;	// this is always positive
    
  if(calWatt>= ENERGY_PER_PULSE)// cal will glow after reaching energy that level
  {
    calWatt -= ENERGY_PER_PULSE;
    ACTIVE_PULSE_LED_ON;
    total_energy_pulse_remaining_time = pulse_duration;// ENERGY_PULSE_DURATION earlier
  }
	
  if (total_energy_pulse_remaining_time && --total_energy_pulse_remaining_time == 0)
    ACTIVE_PULSE_LED_OFF;
	
		
//-------------------reactive pulsing start-------------------
#ifdef REACTIVE_PULSING
	
	if(Common_Data.led_function==0)
//  	calVar += ABS(total_reactive_power_pulsing);
  	calVar += total_reactive_power_pulsing;	// this is now always positive
	else
//  	calVar += ABS(total_apparent_power_pulsing);
  	calVar += total_apparent_power_pulsing;	// this is now always positive
	
  if(calVar>= ENERGY_PER_PULSE)// cal will glow after reaching energy that level
  {
    calVar -= ENERGY_PER_PULSE;
    REACTIVE_PULSE_LED_ON;
    total_reactive_energy_pulse_remaining_time = pulse_duration;// ENERGY_PULSE_DURATION earlier
  }
	
  if (total_reactive_energy_pulse_remaining_time && --total_reactive_energy_pulse_remaining_time == 0)
    REACTIVE_PULSE_LED_OFF;
#endif		
		
}

/******************************************************************************
* Function Name    : void EM_ADC_GainPhaseReset(EM_LINE line)
* Description      : Reset phase gain to lowest level
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
void EM_ADC_GainPhaseReset(EM_LINE line)
{
    g_EM_ADC_GainPhaseCurrentLevel[line] = 0;       /* Lowest */
    EM_ADC_GainPhaseSet(line);
}

/******************************************************************************
* Function Name    : void EM_ADC_GainPhaseIncrease(void)
* Description      : Increase phase gain 1 level & set the gain level to port
* Arguments        : None
* Functions Called : EM_ADC_GainPhaseSet()
* Return Value     : None
******************************************************************************/
void EM_ADC_GainPhaseIncrease(EM_LINE line)
{
    if (g_EM_ADC_GainPhaseCurrentLevel[line] >= EM_GAIN_PHASE_NUM_LEVEL_MAX-1)  /* Is highest gain? */
    {
        return; /* Do nothing */
    }
    else    /* Still exist higher gain */
    {
        g_EM_ADC_GainPhaseCurrentLevel[line]++; /* Increase level count */
        EM_ADC_GainPhaseSet(line);              /* Apply gain */
    }
}

/******************************************************************************
* Function Name    : void EM_ADC_GainPhaseDecrease(void)
* Description      : Decrease phase gain 1 level & set the gain level to port
* Arguments        : None
* Functions Called : EM_ADC_GainPhaseSet()
* Return Value     : None
******************************************************************************/
void EM_ADC_GainPhaseDecrease(EM_LINE line)
{
    if (g_EM_ADC_GainPhaseCurrentLevel[line] == 0)  /* Is lowest gain? */
    {
        return; /* Do nothing */
    }
    else    /* Still exist lower gain */
    {
        g_EM_ADC_GainPhaseCurrentLevel[line]--; /* Decrease level count */
        EM_ADC_GainPhaseSet(line);              /* Apply gain */
    }
}
    
/******************************************************************************
* Function Name    : uint8_t EM_ADC_GainPhaseGetLevel(void)
* Description      : Get the current phase gain level
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
uint8_t EM_ADC_GainPhaseGetLevel(EM_LINE line)
{
    return g_EM_ADC_GainPhaseCurrentLevel[line];
}

/******************************************************************************
* Function Name    : void EM_ADC_SetPhaseCorrection_Phase_R(float32_t degree)
* Description      : Set phase correction between I1 and V
* Arguments        : degree: Phase (in degree) of V based on I1
* Functions Called : TBD
* Return Value     : None
******************************************************************************/
void EM_ADC_SetPhaseCorrection_Phase_R(float32_t degree)
{
    /* negative ? the current lead voltage */
    /* Compensate the frequency variation + sampling frequency*/
    float32_t shift_step;
    float32_t fac = EM_GetLineFrequency(LINE_PHASE_R);
    
#if 0   
    //degree = (fac - EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY) * 0.17 ;
    degree = degree - ((fac-EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY)*0.17);
    
    //shift_step = (360 * target_ac)/(384*3906.25);
    shift_step = (0.00024 * fac);       //(360/(384*3906.25) = 0.00024)
#else
    shift_step = EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_50HZ;	// 0.012f aj
    
#endif

    if (g_enable_auto_fstep == 1)
    {
        if (degree <= 0.0f)
        {
            EM_ADC_DriverSetPhaseStep(
                EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R,
                (uint16_t)(
                    (-degree) /
                    shift_step
                )
            );// R_DSADC_AdjustPhaseDelay(channel, step)
        }
        else
        {
            
        }
    }
}

/******************************************************************************
* Function Name    : void EM_ADC_SetPhaseCorrection(float32_t degree)
* Description      : Set phase correction between I1 and V
* Arguments        : degree: Phase (in degree) of V based on I1
* Functions Called : TBD
* Return Value     : None
******************************************************************************/
void EM_ADC_SetPhaseCorrection_Phase_Y(float32_t degree)
{
    /* negative ? the current lead voltage */
    /* Compensate the frequency variation + sampling frequency*/
    float32_t shift_step;
    float32_t fac = EM_GetLineFrequency(LINE_PHASE_Y);
    
#if 0   
    //degree = (fac - EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY) * 0.17 ;
    degree = degree - ((fac-EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY)*0.17);
    
    //shift_step = (360 * target_ac)/(384*3906.25);
    shift_step = (0.00024 * fac);       //(360/(384*3906.25) = 0.00024)
#else
    shift_step = EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_50HZ;
    
#endif

    if (degree <= 0.0f)
    {
        EM_ADC_DriverSetPhaseStep(
            EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y,
            (uint16_t)(
                (-degree) /
                EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_50HZ
            )
        );	// R_DSADC_AdjustPhaseDelay(channel, step)
    }
    else
    {
        
    }
}

/******************************************************************************
* Function Name    : void EM_ADC_SetPhaseCorrection_Phase_B(float32_t degree)
* Description      : Set phase correction between I1 and V
* Arguments        : degree: Phase (in degree) of V based on I1
* Functions Called : TBD
* Return Value     : None
******************************************************************************/
void EM_ADC_SetPhaseCorrection_Phase_B(float32_t degree)
{
    /* negative ? the current lead voltage */
    /* Compensate the frequency variation + sampling frequency*/
    float32_t shift_step;
    float32_t fac = EM_GetLineFrequency(LINE_PHASE_B);
    
#if 0   
    //degree = (fac - EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY) * 0.17 ;
    degree = degree - ((fac-EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY)*0.17);
    
    //shift_step = (360 * target_ac)/(384*3906.25);
    shift_step = (0.00024 * fac);       //(360/(384*3906.25) = 0.00024)
#else
    shift_step = EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_50HZ;
    
#endif
    
    if (degree <= 0.0f)
    {
        EM_ADC_DriverSetPhaseStep(
            EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B,
            (uint16_t)(
                (-degree) /
                EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_50HZ
            )
        );// R_DSADC_AdjustPhaseDelay(channel, step)
    }
    else
    {
        
    }
}

#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
/******************************************************************************
* Function Name    : uint16_t EM_ADC_QuarterShiftTimer_LookupValue(uint16_t fsteps_x100)
* Description      : Lookup for timer value according to scaled x100 fraction steps
* Arguments        : fsteps_x100: scaled fractional step (x100)
* Functions Called : None
* Return Value     : None
******************************************************************************/
uint16_t EM_ADC_QuarterShiftTimer_LookupValue(uint16_t fsteps_x100)
{
    uint16_t value;
    /* Check the table low index */
    if (fsteps_x100 < TIMER_TABLE_OFFSET)
    {
        fsteps_x100 = TIMER_TABLE_OFFSET;
    }
    
    /* Offset */
    fsteps_x100 -= TIMER_TABLE_OFFSET;
    
    /* Check the table high index */
    if (fsteps_x100 >= TIMER_TABLE_SIZE)
    {
        fsteps_x100 = TIMER_TABLE_SIZE-1;
    }
    
    /* Get value from lookup table */
    value = g_wrp_adc_timer_table[fsteps_x100].timer_value;
    
    /* Compensate with calibration offset: Add? */
    value -= g_fsteps_timer_offset;
    
    if (value >= EM_ADC_DRIVER_TAU_MAX_THRESHOLD)
    {
        value = EM_ADC_DRIVER_TAU_MAX_THRESHOLD;
    }
    else if (value <= EM_ADC_DRIVER_TAU_MIN_THRESHOLD)
    {
        value = EM_ADC_DRIVER_TAU_MIN_THRESHOLD;
    }
    
    return value;
}
#endif

/******************************************************************************
* Function Name    : void EM_ADC_SAMP_UpdateStep(float32_t fac)
* Description      : Update the step delay into sw delay circle and hw timer delay
* Arguments        : float32_t fac: measured line frequency
* Functions Called : EM_ADC_QuarterShiftTimer_LookupValue(): lookup timer value
*                  : EM_ADC_TAU_QuarterShiftTimer_SetValue(): set TDR value
* Return Value     : None
******************************************************************************/
void EM_ADC_SAMP_UpdateStep(float32_t fac)
{
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE
    /* Update software integer  */  
    uint8_t     steps;
    uint16_t    fsteps_x100;
    float32_t   total_steps;
    float32_t   us;
    float32_t   rounded_step;
    uint16_t    tdr_value;
    
    /*
     *                                  EM_ADC_DRIVER_IDEAL_SAMPLING_FREQUENCY
     * Calculate ideal total step: = ---------------------------------------------
     *                                              fac * 4.0f
    */
    
    total_steps = (976.5625f / fac); 
    
    rounded_step = ((float32_t)((uint16_t)(total_steps * 100.0f)) / 100);
    
    steps = (uint8_t)rounded_step;
    
    rounded_step = rounded_step - steps;
    rounded_step = rounded_step * 100.0f +0.5f;
    fsteps_x100 = (uint16_t)rounded_step;
    
    /* Reverse the fsteps as required */
    fsteps_x100 = 100 - fsteps_x100;
    
    /* Check to compensate the min and max threshold */
    if (fsteps_x100 >= 5 && 
        fsteps_x100 <= 95)
    {
        steps += 1;
        g_wrp_adc_tdr_ignore = 0;
    }
    else
    {
        if (fsteps_x100 < 5)
        {
            steps += 1;
        }
        g_wrp_adc_tdr_ignore = 1;
    }
    
    /* Set to software delay */
    g_wrp_adc_voltage90_circle_step = steps + EM_ADC_DELAY_STEP_VOLTAGE;

    if (g_wrp_adc_tdr_ignore == 0)
    {
        if (g_enable_auto_fstep == 1)
        {
            /* 
             * Adjust HW Timer Fsteps 
            */
            
            /* Lookup the timer register value table */
            tdr_value = EM_ADC_QuarterShiftTimer_LookupValue(fsteps_x100);
            
            /* Control timer delay, convert to us */
            EM_ADC_TAU_QuarterShiftTimer_SetValue(tdr_value);
        }
    }
#endif    
}

