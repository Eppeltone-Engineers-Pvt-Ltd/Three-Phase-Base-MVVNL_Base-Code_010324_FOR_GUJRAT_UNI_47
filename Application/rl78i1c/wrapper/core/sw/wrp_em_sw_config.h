/***********************************************************************************************************************
* File Name    : wrp_em_sw_config.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper Configuration Header file
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_SW_CONFIG_H
#define _WRAPPER_EM_SW_CONFIG_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "em_constraint.h"

    /**************************************************************************
    *   ADC Driver Header
    ***************************************************************************/
    #include "r_cg_dsadc.h"
    #include "r_cg_adc.h"
    #include "r_cg_elc.h"
    #include "r_cg_dtc.h"
    #include "r_cg_tau.h"
    
    /**************************************************************************
    *   PULSE Driver Header
    ***************************************************************************/
    #include "r_cg_port.h"
    
    /**************************************************************************
    *   TIMER Driver Header
    ***************************************************************************/
    #include "r_cg_tau.h"
    
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
*   EM Core Software Block (SW Block)
************************************************************************************************************************/
    /**************************************************************************
    *   SW Wrapper Property Configuration & Settings
    ***************************************************************************/
    #define EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL_PHASE_R            1                   /* (SW Support Only) - Gain switch number of level: 1 or 2  */
    #define EM_SW_PROPERTY_ADC_GAIN_UPPER_THRESHOLD_PHASE_R         3000000             /* (SW Support Only) - Gain switch adc upper threshold      */
    #define EM_SW_PROPERTY_ADC_GAIN_LOWER_THRESHOLD_PHASE_R         300000              /* (SW Support Only) - Gain switch adc lower threshold      */
    
    #define EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL_PHASE_Y            1                   /* (SW Support Only) - Gain switch number of level: 1 or 2  */
    #define EM_SW_PROPERTY_ADC_GAIN_UPPER_THRESHOLD_PHASE_Y         3000000             /* (SW Support Only) - Gain switch adc upper threshold      */
    #define EM_SW_PROPERTY_ADC_GAIN_LOWER_THRESHOLD_PHASE_Y         300000              /* (SW Support Only) - Gain switch adc lower threshold      */

    #define EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL_PHASE_B            1                   /* (SW Support Only) - Gain switch number of level: 1 or 2  */
    #define EM_SW_PROPERTY_ADC_GAIN_UPPER_THRESHOLD_PHASE_B         3000000             /* (SW Support Only) - Gain switch adc upper threshold      */
    #define EM_SW_PROPERTY_ADC_GAIN_LOWER_THRESHOLD_PHASE_B         300000              /* (SW Support Only) - Gain switch adc lower threshold      */
        
    #define EM_SW_PROPERTY_ADC_CONVERSION_TIME                      (1.0f)              /* (SW Support Only) - ADC Delay Time (us) - Just dummy, no use */
    
    /* Define the DSAD High pass cutoff frequency */
    #define EM_SW_PROPERTY_ADC_HPF_CUTOFF_FREQUENCY                 (HPF_4857_HZ)       /* (SW Support Only) DSAD HPF cutoff frequency */ 
    
    /**************************************************************************
    *   PULSE Wrapper Configuration for Settings & I/F mapping
    ***************************************************************************/
    /* I/Fs Mapping 
     *      Macro Name / I/Fs                                   Setting/Description */
//    #define EM_PULSE0_ON_STATEMENT                              {P3.5 = 0;}         /* Pulse 0 LED On */
//    #define EM_PULSE0_OFF_STATEMENT                             {P3.5 = 1;}         /* Pulse 0 LED Off */
//    #define EM_PULSE1_ON_STATEMENT                              {P3.4 = 0;}         /* Pulse 1 LED On */
//    #define EM_PULSE1_OFF_STATEMENT                             {P3.4 = 1;}         /* Pulse 1 LED Off */
//    #define EM_PULSE2_ON_STATEMENT                              {P3.2 = 0;}         /* Pulse 2 LED On */
//    #define EM_PULSE2_OFF_STATEMENT                             {P3.2 = 1;}         /* Pulse 2 LED Off */
//    #define EM_PULSE3_ON_STATEMENT                              {P3.1 = 0;}         /* Pulse 3 LED On */
//    #define EM_PULSE3_OFF_STATEMENT                             {P3.1 = 1;}         /* Pulse 3 LED Off */
//    #define EM_PULSE4_ON_STATEMENT                              {P3.0 = 0;}         /* Pulse 4 LED On */
//    #define EM_PULSE4_OFF_STATEMENT                             {P3.0 = 1;}         /* Pulse 4 LED Off */
//    #define EM_PULSE_DriverInit()                               {;}                 /* Pulse Init */

    #define EM_PULSE0_ON_STATEMENT                              {;}         /* Pulse 0 LED On */
    #define EM_PULSE0_OFF_STATEMENT                             {;}         /* Pulse 0 LED Off */
    #define EM_PULSE1_ON_STATEMENT                              {;}         /* Pulse 1 LED On */
    #define EM_PULSE1_OFF_STATEMENT                             {;}         /* Pulse 1 LED Off */
    #define EM_PULSE2_ON_STATEMENT                              {}         /* Pulse 2 LED On */
    #define EM_PULSE2_OFF_STATEMENT                             {;}         /* Pulse 2 LED Off */
    #define EM_PULSE3_ON_STATEMENT                              {;}         /* Pulse 3 LED On */
    #define EM_PULSE3_OFF_STATEMENT                             {;}         /* Pulse 3 LED Off */
    #define EM_PULSE4_ON_STATEMENT                              {;}         /* Pulse 4 LED On */
    #define EM_PULSE4_OFF_STATEMENT                             {;}         /* Pulse 4 LED Off */
    #define EM_PULSE_DriverInit()                               {;}                 /* Pulse Init */
    
    
    /**************************************************************************
    *   ADC Wrapper Configuration for Settings & I/F mapping
    ***************************************************************************/    
    /* Settings */
    /* Channel Name Specification */
    /* SAR-ADC Channels */

    #define EM_ADC_VOL_DRIVER_START_SCAN_CHANNEL                ADCHANNEL2          /* AD 1st channel in scan chain */

#if (__BOARD_REVISION == 2)
    #define EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R                   ADCHANNEL3          /* AD Driver Voltage Channel ID         */
    #define EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y                   ADCHANNEL4          /* AD Driver Voltage Channel ID         */
    #define EM_ADC_VOL_DRIVER_CHANNEL_PHASE_B                   ADCHANNEL5          /* AD Driver Voltage Channel ID         */
        
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R               DSADCHANNEL0        /* AD Driver Voltage Channel ID         */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y               DSADCHANNEL1        /* AD Driver Voltage Channel ID         */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B               DSADCHANNEL2        /* AD Driver Voltage Channel ID         */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL               DSADCHANNEL3        /* AD Driver Voltage Channel ID         */

    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_R  (0)
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_Y  (1)
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_B  (2)
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL  (3)

#elif (__BOARD_REVISION == 1)
    #define EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R                   ADCHANNEL5          /* AD Driver Voltage Channel ID         */
    #define EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y                   ADCHANNEL3          /* AD Driver Voltage Channel ID         */
    #define EM_ADC_VOL_DRIVER_CHANNEL_PHASE_B                   ADCHANNEL4          /* AD Driver Voltage Channel ID         */
        
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R               DSADCHANNEL3        /* AD Driver Voltage Channel ID         */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y               DSADCHANNEL1        /* AD Driver Voltage Channel ID         */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B               DSADCHANNEL2        /* AD Driver Voltage Channel ID         */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL               DSADCHANNEL0        /* AD Driver Voltage Channel ID         */
    
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_R  (3)
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_Y  (1)
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE_B  (2)
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL  (0)
#else
    #error "Check value of __BOARD_REVISION in compiler macro definition"

#endif

    #define EM_ADC_DRIVER_IDEAL_SAMPLING_FREQUENCY              (3906.25)
    
    #define EM_ADC_GAIN_LEVEL0_PHASE_R                          (GAIN_X1)
    #define EM_ADC_GAIN_LEVEL1_PHASE_R                          (GAIN_X4)
    
    #define EM_ADC_GAIN_LEVEL0_PHASE_Y                          (GAIN_X1)
    #define EM_ADC_GAIN_LEVEL1_PHASE_Y                          (GAIN_X4)
    
    #define EM_ADC_GAIN_LEVEL0_PHASE_B                          (GAIN_X1)
    #define EM_ADC_GAIN_LEVEL1_PHASE_B                          (GAIN_X4)        
    /* 
     * Define the settable value for quarter shift timer to protect sampling
     * This setting depends on how sampling chain implemented and made by experiment
    */
    #define EM_ADC_DRIVER_TAU_MAX_THRESHOLD                     (5775)
    #define EM_ADC_DRIVER_TAU_MIN_THRESHOLD                     (500)
    
    #define EM_ADC_DRIVER_TAU_MAX_PERCENT                       (96)                /* Max 246us setting */
    #define EM_ADC_DRIVER_TAU_MIN_PERCENT                       (8)                 /* Min 20us setting */
    
    #define EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_50HZ              (0.012f)            /* AD Driver Phase step */
    #define EM_ADC_DRIVER_PHASE_SHIFT_STEP_BY_60HZ              (0.014f)            /* AD Driver Phase step */
    
    /*
     * Software Phase Adjustment (by ADC ISR) for 3 channels
     * Set as 0 if not want to delay
     * The maximum step will be auto calculated to adjust the buffer size
     */

    #define EM_ADC_DELAY_STEP_VOLTAGE                           (6)                                         /* Step for phase adjustment of voltage channel */
                                                                
    #define EM_ADC_DELAY_STEP_VOLTAGE90                         (EM_ADC_DELAY_STEP_VOLTAGE + 20)            /* Step for phase adjustment of voltage channel */
    #define EM_ADC_DELAY_VOLTAGE90_MAX_STEP                     (EM_ADC_DELAY_STEP_VOLTAGE90 + 5)
                                                                
                    
    #define EM_ADC_DELAY_STEP_CURRENT_PHASE_R                   (0)                                         /* Step for phase adjustment of voltage channel */
    #define EM_ADC_DELAY_STEP_CURRENT_PHASE_Y                   (0)                                         /* Step for phase adjustment of voltage channel */
    #define EM_ADC_DELAY_STEP_CURRENT_PHASE_B                   (0)                                         /* Step for phase adjustment of voltage channel */
    #define EM_ADC_DELAY_STEP_CURRENT_NEUTRAL                   (0)                                         /* Step for phase adjustment of voltage channel */

    #define EM_ADC_DELAY_STEP_CURRENT_MAX_STEP                  (   ( EM_ADC_DELAY_STEP_CURRENT_PHASE_R >= EM_ADC_DELAY_STEP_CURRENT_PHASE_Y &&\
                                                                      EM_ADC_DELAY_STEP_CURRENT_PHASE_R >= EM_ADC_DELAY_STEP_CURRENT_PHASE_B &&\
                                                                      EM_ADC_DELAY_STEP_CURRENT_PHASE_R >= EM_ADC_DELAY_STEP_CURRENT_NEUTRAL) ? EM_ADC_DELAY_STEP_CURRENT_PHASE_R : \
                                                                    ( EM_ADC_DELAY_STEP_CURRENT_PHASE_Y >= EM_ADC_DELAY_STEP_CURRENT_PHASE_R &&\
                                                                      EM_ADC_DELAY_STEP_CURRENT_PHASE_Y >= EM_ADC_DELAY_STEP_CURRENT_PHASE_B &&\
                                                                      EM_ADC_DELAY_STEP_CURRENT_PHASE_Y >= EM_ADC_DELAY_STEP_CURRENT_NEUTRAL) ? EM_ADC_DELAY_STEP_CURRENT_PHASE_Y : \
                                                                    ( EM_ADC_DELAY_STEP_CURRENT_PHASE_B >= EM_ADC_DELAY_STEP_CURRENT_PHASE_R &&\
                                                                      EM_ADC_DELAY_STEP_CURRENT_PHASE_B >= EM_ADC_DELAY_STEP_CURRENT_PHASE_Y &&\
                                                                      EM_ADC_DELAY_STEP_CURRENT_PHASE_B >= EM_ADC_DELAY_STEP_CURRENT_NEUTRAL) ? EM_ADC_DELAY_STEP_CURRENT_PHASE_B : EM_ADC_DELAY_STEP_CURRENT_NEUTRAL \
                                                                )
    /* I/Fs mapping
     *      Macro Name / I/Fs                                   Setting/Description */
    #define EM_ADC_DriverInit()                                 {\
                                                                    R_ELC_Set(ELC_TRIGGER_SRC_INTDSAD, _01_ELC_EVENT_LINK_AD);  \
                                                                    R_ELC_Set(ELC_TRIGGER_SRC_INTTM01, _01_ELC_EVENT_LINK_AD);  \
                                                                }
    /*
     * Flow to start and stop EM_ADC                                                            
    */
    #define EM_ADC_DriverStart()                                {\
                                                                    R_DTCD0_Start();        \
                                                                    R_DTCD1_Start();        \
                                                                    R_TAU0_Channel1_Start();\
                                                                    R_ADC_HW_Trigger_Start();\
                                                                    R_DSADC_Start();\
                                                                }
    
    #define EM_ADC_DriverStop()                                 {\
                                                                    R_DSADC_Stop();\
                                                                    R_ADC_HW_Trigger_Stop();\
                                                                    R_TAU0_Channel1_Stop();\
                                                                    R_DTCD0_Stop();\
                                                                    R_DTCD1_Stop();\
                                                                }
    
    #define EM_ADC_DriverRead(channel, buffer)                  R_DSADC_Get_Result(channel, buffer)         /* AD Driver Read Result       */
    
    #define EM_ADC_Voltage_DriverRead(channel, buffer)          {\
                                                                    buffer = (uint16_t)g_adc_data[\
                                                                            channel-EM_ADC_VOL_DRIVER_START_SCAN_CHANNEL] >> 1;\
                                                                }
    #define EM_ADC_Voltage90_DriverRead(channel, buffer)        {\
                                                                    buffer = (uint16_t)g_adc_data[\
                                                                            channel-EM_ADC_VOL_DRIVER_START_SCAN_CHANNEL+4] >> 1;\
                                                                }
    
    #define EM_ADC_DriverSetPhaseStep(channel, step)            R_DSADC_AdjustPhaseDelay(channel, step)             /* AD Driver Set Phase Steps   */
    #define EM_ADC_TAU_QuarterShiftTimer_SetValue(value)        R_TAU0_Channel1_SetValue(value)

    
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

#endif /* _WRAPPER_EM_SW_CONFIG_H */
