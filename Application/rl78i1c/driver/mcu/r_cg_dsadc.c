#include "r_cg_macrodriver.h"
#include "iodefine.h"
#include "r_cg_dsadc.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_mcu.h"
#include "wrp_em_sw_config.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/* Start user code for global. Do not edit comment generated here */
static uint16_t g_reg_phase0;
static uint16_t g_reg_phase1;
static uint16_t g_reg_phase2;
static uint16_t g_reg_phase3;
/* End user code. Do not edit comment generated here */

void R_DSADC_Create(void)
{
    DSADCK = _00_DSAD_OPERATION_CLOCK_FIH;      /* OCO clock selected */
    DSADCEN = 1U;   /* Supply clock to ADC */
    DSAMK = 1U;     /* Disalbe INTSAD interrupt */
    DSAIF = 0U;     /* Clear INTSAD interrupt flag */
    
    /* Checkpoint */
    DSAZMK0 = 1U;      /* disable INTDSADZC0 interrupt */
    DSAZIF0 = 0U;       /* clear INTDSADZC0 interrupt flag */
    DSAZMK1 = 1U;      /* disable INTDSADZC1 interrupt */
    DSAZIF1 = 0U;       /* clear INTDSADZC1 interrupt flag */   
    
    /* Set INTDSAD high priority */
    DSAPR0 = 0U;
    DSAPR1 = 0U;

    /* Set INTDSADZC0 level2 priority */
    DSAZPR00 = 1U;
    DSAZPR01 = 1U;
    
    /* Set INTDSADZC0 level2 priority */
    DSAZPR10 = 1U;
    DSAZPR11 = 1U;  
    
    /* Setup for ZC */
    DSADICR = 0U;
    DSADICR = _01_DSAD_ZC0_INPUT_CH1 | _00_DSAD_ZC0_DECTECTION_MODE_PULSE | _00_DSAD_ZC0_EDGE_SELECT_NONE |
              _00_DSAD_ZC1_INPUT_CH3 | _00_DSAD_ZC1_DECTECTION_MODE_PULSE | _00_DSAD_ZC0_EDGE_SELECT_NONE;  
    
    /* Sampling rate at 3906Hz, 24 bits resolution */
    DSADMR = _0000_DSAD_SAMPLING_FREQUENCY_0 | _0000_DSAD_RESOLUTION_24BIT; 
    DSADMR |= _0F0F_DSAD_ENABLE_ALLCH;

    /* High pass filter */
#if 0
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL3);     /* Turn on HPF on channel 3 - Neutral channel */
    R_DSADC_SetHighPassFilterOff(DSADCHANNEL2);
    R_DSADC_SetHighPassFilterOff(DSADCHANNEL1);
    R_DSADC_SetHighPassFilterOff(DSADCHANNEL0);
#else
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL3);
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL2);
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL1);
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL0);
    R_DSADC_Set_CutOffFrequency(EM_SW_PROPERTY_ADC_HPF_CUTOFF_FREQUENCY);
#endif

    /* Gain setting */
    DSADGCR0 = _00_DSAD_CH0_PGAGAIN_1 | _00_DSAD_CH1_PGAGAIN_1;
    DSADGCR1 = _00_DSAD_CH2_PGAGAIN_1 | _00_DSAD_CH3_PGAGAIN_1;
}

void R_DSADC_Start(void)
{
    uint8_t wait;
    
    #if 1
    DSAMK = 1U;  /* disable INTDSAD interrupt */
    DSAIF = 0U;  /* clear INTDSAD interrupt flag */ 
    #endif
    
    DSADMR &= ~(_0F0F_DSAD_ENABLE_ALLCH);   /* Stop conversion */
    DSADMR |= _0F0F_DSAD_ENABLE_ALLCH;  /* Start conversion ADC channel 01-- */
    
    /* Wait for setup time */
    wait = _80_DSAD_WAIT_SETUP_TIME;
    while (1)
    {
        if (DSAIF == 1)
        {
            DSAIF = 0U;  /* Clear INTSAD interrupt flag */
            MCU_Delay(20);
            DTCEN1.5 = 1;
            wait--;
        }
        if (wait == 0)
        {
            break;
        }
    }
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_ZC_Start(void)
{
    /* Checkpoint */
    DSAZMK0 = 1U;      /* disable INTDSADZC0 interrupt */
    DSAZIF0 = 0U;       /* clear INTDSADZC0 interrupt flag */
    DSAZMK1 = 1U;      /* disable INTDSADZC1 interrupt */
    DSAZIF1 = 0U;       /* clear INTDSADZC1 interrupt flag */
    
    /* Clear the flag of level detection mode */
    DSADICLR |= _01_DSAD_ZC0_FLAG_CLEAR | _10_DSAD_ZC1_FLAG_CLEAR;
    DSAZMK0 = 0U;      /* Enable the INTDSADZC0 interrupt */
    DSAZMK1 = 0U;      /* Enable the INTDSADZC1 interrupt */
}

void R_DSADC_ZC_Stop(void)
{
    /* Checkpoint */
    DSAZMK0 = 0U;      /* disable INTDSADZC0 interrupt */
    DSAZIF0 = 0U;       /* clear INTDSADZC0 interrupt flag */
    DSAZMK1 = 0U;      /* disable INTDSADZC1 interrupt */
    DSAZIF1 = 0U;       /* clear INTDSADZC1 interrupt flag */   
}

void R_DSADC_StartWithoutGainAndPhase(uint16_t channel_control)
{
    uint8_t wait;
    
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */  
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH; /* Stop conversion */
    
    /* Gain setting */
    DSADGCR0 = 0x00;
    DSADGCR1 = 0x00;
    
    /* Phase setting */
    DSADPHCR0 = 0x0000;
    DSADPHCR1 = 0x0000;
    DSADPHCR2 = 0x0000;
    DSADPHCR3 = 0x0000;
    
    /* High pass filter */

#if 0
    if ((channel_control & 0x0008) != 0)
    {
        R_DSADC_SetHighPassFilterOn(DSADCHANNEL3);     /* Turn on HPF on channel 3 - Neutral channel */
    }
    R_DSADC_SetHighPassFilterOff(DSADCHANNEL2);     /* Turn off HPF on channel 2 */
    R_DSADC_SetHighPassFilterOff(DSADCHANNEL1);     /* Turn off HPF on channel 1 */
    R_DSADC_SetHighPassFilterOff(DSADCHANNEL0);     /* Turn off HPF on channel 0 */
    R_DSADC_Set_CutOffFrequency(HPF_4857_HZ);       /* Set cut off frequency for HPF */
#else   
    if ((channel_control & 0x0008) != 0)
    {
        R_DSADC_SetHighPassFilterOn(DSADCHANNEL3);      /* Turn on HPF on channel 3 */
    }
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL2);      /* Turn on HPF on channel 2 */
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL1);      /* Turn on HPF on channel 1 */
    R_DSADC_SetHighPassFilterOn(DSADCHANNEL0);      /* Turn on HPF on channel 0 */
    R_DSADC_Set_CutOffFrequency(EM_SW_PROPERTY_ADC_HPF_CUTOFF_FREQUENCY);       /* Set cut off frequency for HPF */
#endif

    DSADMR |= channel_control;      /* Start conversion ADC channel 0-3 */
    
    /* Wait for setup time */
    wait = _80_DSAD_WAIT_SETUP_TIME;
    while (1)
    {
        if(DSAIF == 1)
        {
            DSAIF = 0U;  /* Clear INTSAD interrupt flag */
            wait--;
        }
        if(wait == 0)
        {
            break;
        }
    }
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_StartChannel0Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADPHCR0 = 0x0000;     /* Reset phase adjustment for channel 0 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    DSADMR |= _0101_DSAD_ENABLE_CH0;        /* Start conversion ADC channel 0 only */
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_StartChannel1Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */
    DSADPHCR1 = 0x0000;     /* Reset phase adjustment for channel 0 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    DSADMR |= _0202_DSAD_ENABLE_CH1;        /* Start conversion ADC channel 0 only */
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_StartChannel2Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */  
    DSADPHCR1 = 0x0000;                     /* Reset phase adjustment for channel 2 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    DSADMR |= _0404_DSAD_ENABLE_CH2;        /* Start conversion ADC channel 2 only */
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_StartChannel3Only(void)
{
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTSAD interrupt flag */  
    DSADPHCR3 = 0x0000;                     /* Reset phase adjustment for channel 2 */
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;     /* others off */
    DSADMR |= _0808_DSAD_ENABLE_CH3;        /* Start conversion ADC channel 2 only */
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_StartCurrentChannels(void)
{
    DSAMK = 1U;                                                     /* Disable INTAD interrupt */
    DSAIF = 0U;                                                     /* Clear INTSAD interrupt flag */   
    DSADMR |= (_0101_DSAD_ENABLE_CH0 | _0404_DSAD_ENABLE_CH2);      /* Start conversion ADC channel 0 and 2 only */
    DSADMR &= ~(_0202_DSAD_ENABLE_CH1 | _0808_DSAD_ENABLE_CH3);     /* others off */
    
    #if 1
    DSAMK = 0U;  /* Enable INTSAD interrupt */
    #endif
}

void R_DSADC_BackupPhaseRegSetting(void)
{
    g_reg_phase0 = DSADPHCR0;
    g_reg_phase1 = DSADPHCR1;
    g_reg_phase2 = DSADPHCR2;
    g_reg_phase3 = DSADPHCR3;
}

void R_DSADC_RestorePhaseRegSetting(void)
{
    DSADPHCR0 = g_reg_phase0;
    DSADPHCR1 = g_reg_phase1;
    DSADPHCR2 = g_reg_phase2;
    DSADPHCR3 = g_reg_phase3;
}

void R_DSADC_Stop(void)
{
    DSADMR &= ~_0F0F_DSAD_ENABLE_ALLCH;  /* Stop conversion */
    DSAMK = 1U;  /* Disable INTAD interrupt */
    DSAIF = 0U;  /* Clear INTAD interrupt flag */
}

MD_STATUS R_DSADC_SetHighPassFilterOff(dsad_channel_t channel)
{
    MD_STATUS status = MD_OK;   
    switch(channel)
    {
        case DSADCHANNEL0:
            DSADHPFCR |= _01_DSAD_CH0_HIGHPASS_FILTER_DISABLE;  
            break;
        case DSADCHANNEL1:
            DSADHPFCR |= _02_DSAD_CH1_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL2:
            DSADHPFCR |= _04_DSAD_CH2_HIGHPASS_FILTER_DISABLE;  
            break;
        case DSADCHANNEL3:
            DSADHPFCR |= _08_DSAD_CH3_HIGHPASS_FILTER_DISABLE;  
            break;              
        default:
            /* Channel is invalid */
            status = MD_ARGERROR;
            break;      
    }
    return status;
}

MD_STATUS R_DSADC_SetHighPassFilterOn(dsad_channel_t channel)
{
    MD_STATUS status = MD_OK;       
    switch(channel)
    {
        case DSADCHANNEL0:
            DSADHPFCR &= (uint8_t)~_01_DSAD_CH0_HIGHPASS_FILTER_DISABLE;    
            break;
        case DSADCHANNEL1:
            DSADHPFCR &= (uint8_t)~_02_DSAD_CH1_HIGHPASS_FILTER_DISABLE;
            break;
        case DSADCHANNEL2:
            DSADHPFCR &= (uint8_t)~_04_DSAD_CH2_HIGHPASS_FILTER_DISABLE;    
            break;
        case DSADCHANNEL3:
            DSADHPFCR &= (uint8_t)~_08_DSAD_CH3_HIGHPASS_FILTER_DISABLE;    
            break;              
        default:
            /* Channel is invalid */
            status = MD_ARGERROR;
            break;      
    }
    return status;
}

MD_STATUS R_DSADC_Set_CutOffFrequency(dsad_cutoff_t frequency)
{
    MD_STATUS status = MD_OK;
    
    if(HPF_0067HZ > frequency || frequency > HPF_4857_HZ)
    {
        status = MD_ARGERROR;
    }
    else 
    {
        DSADHPFCR &= 0x0F;
        DSADHPFCR |= (uint8_t)(frequency << 6);
    }
    return status;
}

MD_STATUS R_DSADC_AdjustPhaseDelay(dsad_channel_t channel, uint16_t step)
{
    MD_STATUS status = MD_OK;

    if (step <= 1151)
    {   /* Step must be from 0 to 1151 */
        switch(channel)
        {
            case DSADCHANNEL0:
                DSADPHCR0 = step;    /* Set new step */
                break;
            case DSADCHANNEL1:
                DSADPHCR1 = step;    /* Set new step */
                break;
            case DSADCHANNEL2:
                DSADPHCR2 = step;    /* Set new step */
                break;
            case DSADCHANNEL3:
                DSADPHCR3 = step;    /* Set new step */
                break;          
            default:
                /* Channel is invalid */
                status = MD_ARGERROR;
                break;
        }       
    }
    else
    {
        /* Step is invalid */
        status = MD_ARGERROR;
    }
    
    return status;
}

void R_DSADC_GetGainAndPhase(dsad_reg_setting_t *adc_gain_phase)
{
    /* Get curretn gain setting */
    adc_gain_phase->gain0 = DSADGCR0;
    adc_gain_phase->gain1 = DSADGCR1;
    /* Get phase angle setting */
    adc_gain_phase->phase0 = DSADPHCR0;
    adc_gain_phase->phase1 = DSADPHCR1;
    adc_gain_phase->phase2 = DSADPHCR2;
    adc_gain_phase->phase3 = DSADPHCR3; 
}

void R_DSADC_SetGainAndPhase(dsad_reg_setting_t adc_gain_phase)
{
    DSADGCR0    = adc_gain_phase.gain0;
    DSADGCR1    = adc_gain_phase.gain1;
    DSADPHCR0   = adc_gain_phase.phase0;
    DSADPHCR1   = adc_gain_phase.phase1;
    DSADPHCR2   = adc_gain_phase.phase2;
    DSADPHCR3   = adc_gain_phase.phase3;    
}

void R_DSADC_SetGain(dsad_reg_setting_t adc_gain_phase)
{
    DSADGCR0    = adc_gain_phase.gain0;
    DSADGCR1    = adc_gain_phase.gain1;
}

MD_STATUS R_DSADC_SetChannelGain(dsad_channel_t channel, dsad_gain_t gain)
{
    MD_STATUS   status = MD_OK;
    
    switch(channel)
    {
        case DSADCHANNEL0:  /* Used as Current channel */
            if (gain < GAIN_X1 || gain > GAIN_X32)
            {
                status = MD_ARGERROR;
                break;
            }
            DSADGCR0 &= 0xF0;
            DSADGCR0 |= gain;
            break;
        case DSADCHANNEL1:  /* Used as Voltage channel */
            if (gain < GAIN_X1 || gain > GAIN_X16)
            {
                status = MD_ARGERROR;
                break;
            }
            DSADGCR0 &= 0x0F;
            DSADGCR0 |= (uint8_t)(gain << 4);
            break;
        case DSADCHANNEL2:  /* Used as Current channel */
            if (gain < GAIN_X1 || gain > GAIN_X32)
            {
                status = MD_ARGERROR;
                break;
            }
            DSADGCR1 &= 0xF0;
            DSADGCR1 |= gain;
            break;
        case DSADCHANNEL3:  /* Used as Voltage channel */
            if (gain < GAIN_X1 || gain > GAIN_X16)
            {
                status = MD_ARGERROR;
                break;
            }
            DSADGCR1 &= 0x0F;
            DSADGCR1 |= (uint8_t)(gain << 4);
            break;
        default:
            /* Channel is invalid */
            status = MD_ARGERROR;
            break;
    }
    
    return status;
}

/* Start user code for adding. Do not edit comment generated here */
dsad_gain_t R_DSADC_GetGainEnumValue(uint8_t gain)
{
    dsad_gain_t     gain_value;
    
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
    
    return gain_value;
}


uint8_t R_DSADC_GetChannelGain(dsad_channel_t channel)
{
    uint8_t gain;
    switch(channel)
    {
        case DSADCHANNEL0:
            gain = 1 << (DSADGCR0 & 0x0F);
            break;
        case DSADCHANNEL1:
            gain = 1 << ((DSADGCR0 & 0xF0) >> 4);
            break;
        case DSADCHANNEL2:
            gain = 1 << (DSADGCR1 & 0x0F);
            break;
        case DSADCHANNEL3:
            gain = 1 << ((DSADGCR1 & 0xF0) >> 4);
            break;
        default:
            gain = 0;
            break;
    }
    
    return gain;
}

uint16_t R_DSADC_GetChannelPhase(dsad_channel_t channel)
{
    uint16_t phase_setting;
    
    if ( channel > DSADCHANNEL3 )
    {
        phase_setting = (uint16_t)*(&DSADPHCR0 + channel);
    }
    
    return phase_setting;
}
/* End user code. Do not edit comment generated here */
