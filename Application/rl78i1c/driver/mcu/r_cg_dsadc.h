#ifndef DSADC_H
#define DSADC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Peripheral enable register 1 (PER1)
*/
/* Control of DSAD converter input clock (DSADCEN) */
#define _00_DSAD_CLOCK_STOP                     (0x00U) /* stop supply of input clock */
#define _01_DSAD_CLOCK_SUPPLY                   (0x01U) /* supply input clock */

/*
    Peripheral clock control register (PCKC)
*/
/* Control of DSAD converter operation clock (DSADCK) */
#define _00_DSAD_OPERATION_CLOCK_FIH            (0x00U) /* fIH (=24MHz) is operating */
#define _01_DSAD_OPERATION_CLOCK_FMX            (0x01U) /* fMX (=12MHz) is operating */

/*
    DSAD converter mode register (DSADMR)
*/
/* Sampling frequency selection (DSADFR) */
#define _0000_DSAD_SAMPLING_FREQUENCY_0         (0x0000U) /* 3906.25 Hz */
#define _8000_DSAD_SAMPLING_FREQUENCY_1         (0x8000U) /* 1953.125 Hz */
/* DSAD resolution selection (DSADTYP) */
#define _0000_DSAD_RESOLUTION_24BIT             (0x0000U) /* 24 bits */
#define _4000_DSAD_RESOLUTION_16BIT             (0x4000U) /* 16 bits */
/* DSAD converter power-on control (analog block) of channel 3 (DSADPON3) */
#define _0000_DSAD_CH3_POWER_DOWN               (0x0000U) /* power down */
#define _0800_DSAD_CH3_POWER_ON                 (0x0800U) /* power on */
/* DSAD converter power-on control (analog block) of channel 2 (DSADPON2) */
#define _0000_DSAD_CH2_POWER_DOWN               (0x0000U) /* power down */
#define _0400_DSAD_CH2_POWER_ON                 (0x0400U) /* power on */
/* DSAD converter power-on control (analog block) of channel 1 (DSADPON1) */
#define _0000_DSAD_CH1_POWER_DOWN               (0x0000U) /* power down */
#define _0200_DSAD_CH1_POWER_ON                 (0x0200U) /* power on */
/* DSAD converter power-on control (analog block) of channel 0 (DSADPON0) */
#define _0000_DSAD_CH0_POWER_DOWN               (0x0000U) /* power down */
#define _0100_DSAD_CH0_POWER_ON                 (0x0100U) /* power on */
/* DSAD converter operation enable (analog and digital blocks) of channel 3 (DSADCE3) */
#define _0000_DSAD_CH3_RESET                    (0x0000U) /* electric charge reset */
#define _0008_DSAD_CH3_OPERATION                (0x0008U) /* normal operation */
/* DSAD converter operation enable (analog and digital blocks) of channel 2 (DSADCE2) */
#define _0000_DSAD_CH2_RESET                    (0x0000U) /* electric charge reset */
#define _0004_DSAD_CH2_OPERATION                (0x0004U) /* normal operation */
/* DSAD converter operation enable (analog and digital blocks) of channel 1 (DSADCE1) */
#define _0000_DSAD_CH1_RESET                    (0x0000U) /* electric charge reset */
#define _0002_DSAD_CH1_OPERATION                (0x0002U) /* normal operation */
/* DSAD converter operation enable (analog and digital blocks) of channel 0 (DSADCE0) */
#define _0000_DSAD_CH0_RESET                    (0x0000U) /* electric charge reset */
#define _0001_DSAD_CH0_OPERATION                (0x0001U) /* normal operation */

/*
    DSAD converter gain control register 0 (DSADGCR0)
*/
/* Channel 1 programmable amplifier gain selection (DSADGAIN12 - DSADGAIN10) */
#define _00_DSAD_CH1_PGAGAIN_1                  (0x00U) /* PGA gain: 1 time */
#define _10_DSAD_CH1_PGAGAIN_2                  (0x10U) /* PGA gain: 2 times */
#define _20_DSAD_CH1_PGAGAIN_4                  (0x20U) /* PGA gain: 4 times */
#define _30_DSAD_CH1_PGAGAIN_8                  (0x30U) /* PGA gain: 8 times */
#define _40_DSAD_CH1_PGAGAIN_16                 (0x40U) /* PGA gain: 16 times */
/* Channel 0 programmable amplifier gain selection (DSADGAIN02 - DSADGAIN00) */
#define _00_DSAD_CH0_PGAGAIN_1                  (0x00U) /* PGA gain: 1 time */
#define _01_DSAD_CH0_PGAGAIN_2                  (0x01U) /* PGA gain: 2 times */
#define _02_DSAD_CH0_PGAGAIN_4                  (0x02U) /* PGA gain: 4 times */
#define _03_DSAD_CH0_PGAGAIN_8                  (0x03U) /* PGA gain: 8 times */
#define _04_DSAD_CH0_PGAGAIN_16                 (0x04U) /* PGA gain: 16 times */
#define _05_DSAD_CH0_PGAGAIN_32                 (0x05U) /* PGA gain: 32 times */

/*
    DSAD converter gain control register 1 (DSADGCR1)
*/
/* Channel 3 programmable amplifier gain selection (DSADGAIN32 - DSADGAIN30) */
#define _00_DSAD_CH3_PGAGAIN_1                  (0x00U) /* PGA gain: 1 time */
#define _10_DSAD_CH3_PGAGAIN_2                  (0x10U) /* PGA gain: 2 times */
#define _20_DSAD_CH3_PGAGAIN_4                  (0x20U) /* PGA gain: 4 times */
#define _30_DSAD_CH3_PGAGAIN_8                  (0x30U) /* PGA gain: 8 times */
#define _40_DSAD_CH3_PGAGAIN_16                 (0x40U) /* PGA gain: 16 times */
/* Channel 2 programmable amplifier gain selection (DSADGAIN22 - DSADGAIN20) */
#define _00_DSAD_CH2_PGAGAIN_1                  (0x00U) /* PGA gain: 1 time */
#define _01_DSAD_CH2_PGAGAIN_2                  (0x01U) /* PGA gain: 2 times */
#define _02_DSAD_CH2_PGAGAIN_4                  (0x02U) /* PGA gain: 4 times */
#define _03_DSAD_CH2_PGAGAIN_8                  (0x03U) /* PGA gain: 8 times */
#define _04_DSAD_CH2_PGAGAIN_16                 (0x04U) /* PGA gain: 16 times */
#define _05_DSAD_CH2_PGAGAIN_32                 (0x05U) /* PGA gain: 32 times */

/*
    DSAD Converter HPF Control Register (DSADHPFCR)
*/
/* High-pass filter cutoff frequency selection (DSADCOF1, DSADCOF0) */
#define _00_DSAD_CUTOFF_FREQUENCY_0             (0x00U) /* 0.607 Hz */
#define _40_DSAD_CUTOFF_FREQUENCY_1             (0x40U) /* 1.214 Hz */
#define _80_DSAD_CUTOFF_FREQUENCY_2             (0x80U) /* 2.429 Hz */
#define _C0_DSAD_CUTOFF_FREQUENCY_3             (0xC0U) /* 4.857 Hz */
/* Channel 3 high-pass filter control (DSADTHR3) */
#define _00_DSAD_CH3_HIGHPASS_FILTER_ENABLE     (0x00U) /* enable high-pass filter */
#define _08_DSAD_CH3_HIGHPASS_FILTER_DISABLE    (0x08U) /* disable high-pass filter */
/* Channel 2 high-pass filter control (DSADTHR2) */
#define _00_DSAD_CH2_HIGHPASS_FILTER_ENABLE     (0x00U) /* enable high-pass filter */
#define _04_DSAD_CH2_HIGHPASS_FILTER_DISABLE    (0x04U) /* disable high-pass filter */
/* Channel 1 high-pass filter control (DSADTHR1) */
#define _00_DSAD_CH1_HIGHPASS_FILTER_ENABLE     (0x00U) /* enable high-pass filter */
#define _02_DSAD_CH1_HIGHPASS_FILTER_DISABLE    (0x02U) /* disable high-pass filter */
/* Channel 0 high-pass filter control (DSADTHR0) */
#define _00_DSAD_CH0_HIGHPASS_FILTER_ENABLE     (0x00U) /* enable high-pass filter */
#define _01_DSAD_CH0_HIGHPASS_FILTER_DISABLE    (0x01U) /* disable high-pass filter */

/*
    DSAD Zero crossing interrupt control register (DSADICR)
*/
/* ZC0 */
#define _00_DSAD_ZC0_INPUT_CH2                  (0x00U) /* select channel2 */ 
#define _01_DSAD_ZC0_INPUT_CH1                  (0x01U) /* select channel1 */ 
#define _00_DSAD_ZC0_DECTECTION_MODE_PULSE      (0x00U) /* select pulse mode detection */ 
#define _02_DSAD_ZC0_DECTECTION_MODE_LEVEL      (0x02U) /* select level mode detection */ 
#define _00_DSAD_ZC0_EDGE_SELECT_NONE           (0x00U) /* no edge detection enable */ 
#define _04_DSAD_ZC0_EDGE_SELECT_RISING         (0x04U) /* rising edge detection enable */ 
#define _08_DSAD_ZC0_EDGE_SELECT_FALLING        (0x08U) /* falling edge detection enable */ 
#define _0C_DSAD_ZC0_EDGE_SELECT_BOTH           (0x0CU) /* both rise and fall edge detection enable */ 

/* ZC1 */
#define _00_DSAD_ZC1_INPUT_CH3                  (0x00U) /* select channel3 */ 
#define _10_DSAD_ZC1_INPUT_CH0                  (0x10U) /* select channel0 */ 
#define _00_DSAD_ZC1_DECTECTION_MODE_PULSE      (0x00U) /* select pulse mode detection */ 
#define _20_DSAD_ZC1_DECTECTION_MODE_LEVEL      (0x20U) /* select level mode detection */ 
#define _00_DSAD_ZC1_EDGE_SELECT_NONE           (0x00U) /* no edge detection enable */ 
#define _40_DSAD_ZC1_EDGE_SELECT_RISING         (0x40U) /* rising edge detection enable */ 
#define _80_DSAD_ZC1_EDGE_SELECT_FALLING        (0x80U) /* falling edge detection enable */ 
#define _C0_DSAD_ZC1_EDGE_SELECT_BOTH           (0xC0U) /* both rise and fall edge detection enable */ 

/*
    DSAD Zero crossing interrupt clear register (DSADICLR)
*/
#define _01_DSAD_ZC0_FLAG_CLEAR                 (0x01U) /* Clear interrupt flag in level mode */
#define _10_DSAD_ZC1_FLAG_CLEAR                 (0x10U) /* Clear interrupt flag in level mode */

/*
    DSAD Zero crossing interrupt status register
*/


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _80_DSAD_WAIT_SETUP_TIME                (0x80U)     /* Waiting time for DSAD stabilization */

#define _0003_DSAD_PHC0_VALUE                   (0x0003U)
#define _0002_DSAD_PHC1_VALUE                   (0x0002U)

#define _0101_DSAD_ENABLE_CH0                   (0x0101U)
#define _0202_DSAD_ENABLE_CH1                   (0x0202U)
#define _0404_DSAD_ENABLE_CH2                   (0x0404U)
#define _0808_DSAD_ENABLE_CH3                   (0x0808U)

#define _0F0F_DSAD_ENABLE_ALLCH                 (0x0F0FU)
#define _0B0F_DSAD_ENABLE_ALLCH                 (0x0B0FU)
#define _0303_DSAD_ENABLE_CH0_CH1               (0x0303U)
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    DSADCHANNEL0,
    DSADCHANNEL1,
    DSADCHANNEL2,
    DSADCHANNEL3
} dsad_channel_t;

typedef enum
{
    DSAD_3906HZ = 0U,
    DSAD_1953HZ
} dsad_rate_t;

typedef enum
{
    GAIN_X1 = 0U,
    GAIN_X2,
    GAIN_X4,
    GAIN_X8,
    GAIN_X16,
    GAIN_X32,
    GAIN_NONE,    
} dsad_gain_t;

typedef enum
{
    HPF_0067HZ = 0U,    /* Selection of cut off HPF 0.067 Hz */
    HPF_1214_HZ,        /* Selection of cut off HPF 1.214 Hz */
    HPF_2429_HZ,        /* Selection of cut off HPF 2.429 Hz */
    HPF_4857_HZ         /* Selection of cut off HPF 4.857 Hz */
} dsad_cutoff_t;

typedef struct
{   
    uint8_t  gain0;
    uint8_t  gain1;
    uint16_t phase0;
    uint16_t phase1;
    uint16_t phase2;
    uint16_t phase3;    
} dsad_reg_setting_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_DSADC_Create(void);
void R_DSADC_Start(void);
void R_DSADC_Stop(void);
void R_DSADC_ZC_Start(void);
void R_DSADC_ZC_Stop(void);
void R_DSADC_StartChannel0Only(void);
void R_DSADC_StartChannel1Only(void);
void R_DSADC_StartChannel2Only(void);
void R_DSADC_StartChannel3Only(void);
void R_DSADC_StartCurrentChannels(void);
void R_DSADC_BackupPhaseRegSetting(void);
void R_DSADC_RestorePhaseRegSetting(void);
void R_DSADC_Get_Result(dsad_channel_t channel, int32_t * const buffer);
MD_STATUS R_DSADC_SetHighPassFilterOn(dsad_channel_t channel);
MD_STATUS R_DSADC_SetHighPassFilterOff(dsad_channel_t channel);
MD_STATUS R_DSADC_Set_CutOffFrequency(dsad_cutoff_t frequency);
MD_STATUS R_DSADC_SetChannelGain(dsad_channel_t channel, dsad_gain_t gain);
MD_STATUS R_DSADC_AdjustPhaseDelay(dsad_channel_t channel, uint16_t step);
void R_DSADC_GetGainAndPhase(dsad_reg_setting_t *adc_gain_phase);
void R_DSADC_SetGainAndPhase(dsad_reg_setting_t adc_gain_phase);
void R_DSADC_SetGain(dsad_reg_setting_t adc_gain_phase);
void R_DSADC_StartWithoutGainAndPhase(uint16_t channel_control);
/* Start user code for function. Do not edit comment generated here */
dsad_gain_t R_DSADC_GetGainEnumValue(uint8_t gain);
uint8_t R_DSADC_GetChannelGain(dsad_channel_t channel);
uint16_t R_DSADC_GetChannelPhase(dsad_channel_t channel);
/* End user code. Do not edit comment generated here */
#endif