/***********************************************************************************************************************
* File Name    : platform.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Platform Property Source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"   /* Platform Property header file */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/* Default platform property */
__near const EM_PLATFORM_PROPERTY g_EM_DefaultProperty = 
{
    EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY         ,       /* Target AC Source Frequency */
    0                                                       ,       /* Reserved */
};

/* Time slot list */
__near const EM_TIME_SLOT g_EM_ConfigDefaultTimeSlot[] = 
{
    EM_CONFIG_DEFAULT_TIMESLOT0_INFO,   /* Time slot 0 */
    EM_CONFIG_DEFAULT_TIMESLOT1_INFO,   /* Time slot 1 */
    EM_CONFIG_DEFAULT_TIMESLOT2_INFO,   /* Time slot 2 */
    EM_CONFIG_DEFAULT_TIMESLOT3_INFO,   /* Time slot 3 */
    EM_CONFIG_DEFAULT_TIMESLOT4_INFO,   /* Time slot 4 */
    EM_CONFIG_DEFAULT_TIMESLOT5_INFO,   /* Time slot 5 */
};

/* Default platform configuration */
__near const EM_CONFIG g_EM_DefaultConfig = 
{
    EM_CONFIG_DEFAULT_VOLTAGE_LOW           ,   /* Voltage lowest RMS level.  Unit: Volt */
    EM_CONFIG_DEFAULT_VOLTAGE_HIGH          ,   /* Voltage highest RMS level. Unit: Volt */
    EM_CONFIG_DEFAULT_CURRENT_HIGH          ,   /* Current highest RMS level. Unit: Ampere */
    EM_CONFIG_DEFAULT_FREQ_LOW              ,   /* Lowest frequency.  Unit: Hz */
    EM_CONFIG_DEFAULT_FREQ_HIGH             ,   /* Highest frequency. Unit: Hz */
    EM_CONFIG_DEFAULT_MAX_DEMAND_PERIOD     ,   /* Max demand time of an intergration period. Unit: Minute */
    
    /* Pulse */
    {
        EM_CONFIG_DEFAULT_PULSE0_SOURCE              ,
        EM_CONFIG_DEFAULT_PULSE0_LINE                ,

        EM_CONFIG_DEFAULT_PULSE1_SOURCE              ,
        EM_CONFIG_DEFAULT_PULSE1_LINE                ,

        EM_CONFIG_DEFAULT_PULSE2_SOURCE              ,
        EM_CONFIG_DEFAULT_PULSE2_LINE                ,

        EM_CONFIG_DEFAULT_PULSE3_SOURCE              ,
        EM_CONFIG_DEFAULT_PULSE3_LINE                ,

        EM_CONFIG_DEFAULT_PULSE4_SOURCE              ,
        EM_CONFIG_DEFAULT_PULSE4_LINE                ,
    },
    
    EM_CONFIG_DEFAULT_NUMBER_TARIFF         ,   /* Number of time slot */   
    EM_CONFIG_DEFAULT_NUMBER_TIMESLOT       ,   /* Number of tariff */
    (__near EM_TIME_SLOT *)g_EM_ConfigDefaultTimeSlot              ,                   /* Timeslot info list */   
};

/* SW Phase Correction Angle List */
__near const float32_t g_EM_DefaultCalibPhaseAngleList_Phase_R[] = 
{
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT_PHASE_R  ,       /* Phase Gain Level 0 Phase Shift Angle (in degree) */
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT_PHASE_R  ,     /* Phase Gain Level 1 Phase Shift Angle (in degree) */
};
__near const float32_t g_EM_DefaultCalibPhaseAngleList_Phase_Y[] = 
{
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT_PHASE_Y,     /* Phase Gain Level 0 Phase Shift Angle (in degree) */
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT_PHASE_Y,     /* Phase Gain Level 1 Phase Shift Angle (in degree) */
};
__near const float32_t g_EM_DefaultCalibPhaseAngleList_Phase_B[] = 
{
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT_PHASE_B,     /* Phase Gain Level 0 Phase Shift Angle (in degree) */
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT_PHASE_B,     /* Phase Gain Level 1 Phase Shift Angle (in degree) */
};

/* SW Gain Value List (Phase Channel) */
__near const float32_t g_EM_DefaultCalibPhaseGainValueList_Phase_R[] = 
{
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE_PHASE_R,       /* Phase Gain Level 0 Value (lowest, value is 1.0, fixed)   */
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE_PHASE_R,       /* Phase Gain Level 1 Value     |                           */
};
__near const float32_t g_EM_DefaultCalibPhaseGainValueList_Phase_Y[] = 
{
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE_PHASE_Y,       /* Phase Gain Level 0 Value (lowest, value is 1.0, fixed)   */
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE_PHASE_Y,       /* Phase Gain Level 1 Value     |                           */
};
__near const float32_t g_EM_DefaultCalibPhaseGainValueList_Phase_B[] = 
{
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE_PHASE_B,       /* Phase Gain Level 0 Value (lowest, value is 1.0, fixed)   */
    EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE_PHASE_B,       /* Phase Gain Level 1 Value     |                           */
};

/* Platform default calibration */
__near const EM_CALIBRATION g_EM_DefaultCalibration = 
{
    /* Common */
    {
        EM_CALIB_DEFAULT_SAMPLING_FREQUENCY         ,                   /* Actual sampling frequency of the meter     */
        EM_CALIB_DEFAULT_METER_CONSTANT_PHASE       ,                   /* Meter Constant. Pulses/KWh                 */
        EM_CALIB_DEFAULT_METER_CONSTANT_TOTAL       ,                   /* Meter Constant. Pulses/KWh                 */
        EM_CALIB_DEFAULT_PULSE_ON_TIME              ,                   /* Pulse On Time                              */
        EM_CALIB_DEFAULT_RTC_COMP_OFFSET            ,                   /* RTC compensation default offset            */
    },
    /* Coeff */
    {
        {
            EM_CALIB_DEFAULT_COEFF_VRMS_PHASE_R                         ,                   /* VRMS Co-efficient  At Gain 1.0f            */
            EM_CALIB_DEFAULT_COEFF_IRMS_PHASE_R                         ,                   /* I1RMS Co-efficient At Gain 1.0f            */
            EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER_PHASE_R                 ,                   /* Active Power Co-efficient At Gain 1.0f     */
            EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER_PHASE_R               ,                   /* Reactive Power Co-efficient At Gain 1.0f   */
            EM_CALIB_DEFAULT_COEFF_APPARENT_POWER_PHASE_R               ,                   /* Apparent Power Co-efficient At Gain 1.0f   */
        },
        {
            EM_CALIB_DEFAULT_COEFF_VRMS_PHASE_Y                         ,                   /* VRMS Co-efficient  At Gain 1.0f            */
            EM_CALIB_DEFAULT_COEFF_IRMS_PHASE_Y                         ,                   /* I1RMS Co-efficient At Gain 1.0f            */
            EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER_PHASE_Y                 ,                   /* Active Power Co-efficient At Gain 1.0f     */
            EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER_PHASE_Y               ,                   /* Reactive Power Co-efficient At Gain 1.0f   */
            EM_CALIB_DEFAULT_COEFF_APPARENT_POWER_PHASE_Y               ,                   /* Apparent Power Co-efficient At Gain 1.0f   */
        },
        {
            EM_CALIB_DEFAULT_COEFF_VRMS_PHASE_B                         ,                   /* VRMS Co-efficient  At Gain 1.0f            */
            EM_CALIB_DEFAULT_COEFF_IRMS_PHASE_B                         ,                   /* I1RMS Co-efficient At Gain 1.0f            */
            EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER_PHASE_B                 ,                   /* Active Power Co-efficient At Gain 1.0f     */
            EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER_PHASE_B               ,                   /* Reactive Power Co-efficient At Gain 1.0f   */
            EM_CALIB_DEFAULT_COEFF_APPARENT_POWER_PHASE_B               ,                   /* Apparent Power Co-efficient At Gain 1.0f   */
        },
        {
            EM_CALIB_DEFAULT_COEFF_IRMS_NEUTRAL                         ,
        },
        
    },
    /* Offset */
    {
        {
            EM_CALIB_DEFAULT_OFFSET_V_PHASE_R                           ,                   /* Voltage offset */
            EM_CALIB_DEFAULT_OFFSET_I_PHASE_R                           ,                   /* Current offset */   
        },
        {
            EM_CALIB_DEFAULT_OFFSET_V_PHASE_Y                           ,                   /* Voltage offset */
            EM_CALIB_DEFAULT_OFFSET_I_PHASE_Y                           ,                   /* Current offset */   
        },
        {
            EM_CALIB_DEFAULT_OFFSET_V_PHASE_B                           ,                   /* Voltage offset */
            EM_CALIB_DEFAULT_OFFSET_I_PHASE_B                           ,                   /* Current offset */   
        },
    },
    
    /* Angle */
    {
        {
            (float32_t *)g_EM_DefaultCalibPhaseAngleList_Phase_R        ,                       /* Point to phase angle list */
        },
        {
            (float32_t *)g_EM_DefaultCalibPhaseAngleList_Phase_Y        ,                       /* Point to phase angle list */
        },
        {
            (float32_t *)g_EM_DefaultCalibPhaseAngleList_Phase_B        ,                       /* Point to phase angle list */
        },
    },
    
    /* SW Gain */
    {
        {
            (float32_t *)g_EM_DefaultCalibPhaseGainValueList_Phase_R    ,                       /* Point to gain value list (phase line) */
        },
        
        {
            (float32_t *)g_EM_DefaultCalibPhaseGainValueList_Phase_Y    ,                       /* Point to gain value list (phase line) */
        },
        
        {
            (float32_t *)g_EM_DefaultCalibPhaseGainValueList_Phase_B    ,                       /* Point to gain value list (phase line) */
        },
    },

};


/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
