/***********************************************************************************************************************
* File Name    : em_type.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Core Type Definitions
***********************************************************************************************************************/

#ifndef _EM_TYPE_H
#define _EM_TYPE_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */
#include "em_constraint.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Sampling type */
#define EM_SW_SAMP_TYPE                                 int32_t                                 /* Sampling Data Type */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* EM source selection */
enum tagEMSource
{
    SRC_ACTIVE = 0,                 /* active source */
    SRC_REACTIVE,                   /* reactive source */
    SRC_APPARENT,                   /* apparent source */
    SRC_ALL                         /* all source */
};

/* EM Wire Selection */
typedef enum tagEMLine
{
    LINE_PHASE_R = 0,
    LINE_PHASE_Y,
    LINE_PHASE_B,
    LINE_NEUTRAL,
    LINE_TOTAL,
} EM_LINE;

/* EM source selection */
typedef enum tagEMSourceDef
{
    SRC_PULSE_INVALID = 0,
    SRC_PULSE_VALID,
} EM_SOURCE_FLAG;
    
/* Fundamental sequence definition */
typedef enum tagEMFundSequence
{
    EM_SW_FUND_SEQUENCE_PHASE_R    = 1,
    EM_SW_FUND_SEQUENCE_PHASE_Y    = 2,
    EM_SW_FUND_SEQUENCE_PHASE_B    = 4,
    EM_SW_FUND_SEQUENCE_PHASE_ALL  = 8,
} EM_FUND_SEQUENCE;

typedef enum tagEMSource EM_SOURCE_MAX_DEMAND;      /* Max demand source selection */
typedef enum tagEMSource EM_SOURCE_ENERGY;          /* Energy source selection */
typedef enum tagEMSource EM_SOURCE_PULSE;           /* Pulse Source Selection */

typedef enum tagEMPFSign
{
    PF_SIGN_LEAD_C  = -1,       /* Lead, C */
    PF_SIGN_UNITY   = 0,        /* Unity */
    PF_SIGN_LAG_L   = 1     /* Lag, L */
} EM_PF_SIGN;

/* EM Status */
typedef struct tagEMStatus
{
    union 
    {
        uint32_t value;
        struct
        {
            uint8_t    noload_active_R             :1;     /* Active No-Load status */
            uint8_t    noload_active_Y             :1;     /* Active No-Load status */
            uint8_t    noload_active_B             :1;     /* Active No-Load status */
            uint8_t    noload_reactive_R           :1;     /* Reactive No-Load status */
            uint8_t    noload_reactive_Y           :1;     /* Reactive No-Load status */
            uint8_t    noload_reactive_B           :1;     /* Reactive No-Load status */
            uint8_t    noload_apparent_R           :1;     /* Apparent No-Load status */
            uint8_t    noload_apparent_Y           :1;     /* Apparent No-Load status */
            uint8_t    noload_apparent_B           :1;     /* Apparent No-Load status */
            uint8_t    peak_voltage_R              :1;     /* Peak voltage */
            uint8_t    peak_voltage_Y              :1;     /* Peak voltage */
            uint8_t    peak_voltage_B              :1;     /* Peak voltage */
            uint8_t    peak_current_R              :1;     /* Peak current */
            uint8_t    peak_current_Y              :1;     /* Peak current */
            uint8_t    peak_current_B              :1;     /* Peak current */
            uint8_t    freq_out_of_range_R         :1;     /* Line frequency is out of range */
            uint8_t    freq_out_of_range_Y         :1;     /* Line frequency is out of range */
            uint8_t    freq_out_of_range_B         :1;     /* Line frequency is out of range */
            uint8_t    sag_voltage_R               :1;     /* SAG voltage */
            uint8_t    sag_voltage_Y               :1;     /* SAG voltage */
            uint8_t    sag_voltage_B               :1;     /* SAG voltage */
            uint8_t    reserved                    :3;    /* Reserved */
            uint8_t    reserved1                    :8;    /* Reserved */
        } details;
        
    } events;
    union 
    {
        uint32_t value;
        struct
        {
            uint8_t    is_neutral_missing          :1;     /* Anti-tampering. Neutral Missing */
            uint8_t    is_missing_potential_R      :1;     /* Anti-tampering. Missing Potential */
            uint8_t    is_missing_potential_Y      :1;     /* Anti-tampering. Missing Potential */
            uint8_t    is_missing_potential_B      :1;     /* Anti-tampering. Missing Potential */
            uint8_t    is_voltage_unbalance        :1;     /* Anti-tampering. Voltage Unbalance */
            uint8_t    is_reverse_current_R        :1;     /* Anti-tampering. Reverse Current */
            uint8_t    is_reverse_current_Y        :1;     /* Anti-tampering. Reverse Current */
            uint8_t    is_reverse_current_B        :1;     /* Anti-tampering. Reverse Current */
            uint8_t    is_open_current_R           :1;     /* Anti-tampering. Open Current */
            uint8_t    is_open_current_Y           :1;     /* Anti-tampering. Open Current */
            uint8_t    is_open_current_B           :1;     /* Anti-tampering. Open Current */
            uint8_t    is_current_unbalance        :1;     /* Anti-tampering. Current Unbalance */
            uint8_t    is_current_bypass           :1;     /* Anti-tampering. Current Bypass */
            uint8_t    reserved                    :3;    /* Reserved */
            uint8_t    reserved1                   :8;    /* Reserved */
            uint8_t    reserved2                   :8;    /* Reserved */
        } details;
        
    } tampers;
    
} EM_STATUS;

/* Time slot information */
typedef struct tagEMTimeSlot
{
    struct
    {
        uint8_t hour;   /* Hour of slot */
        uint8_t minute; /* Minute of slot */
    } timestart;    /* Start time of a slot */
            
    uint8_t     tariffno;   /* Tariff number relating with this start time */
    uint8_t     reserved;   /* Padding to Alignment 2-bytes */
    
} EM_TIME_SLOT;

/* EM Platform Property
 * (Can NOT change in run-time, select ONCE and then FIXED) */
typedef struct tagEMPlatformProperty
{
    uint8_t                 target_ac_source_frequency;         /* Target AC Source frequency (50Hz or 60Hz) */
    uint8_t                 reserved;                           /* No use */
    
} EM_PLATFORM_PROPERTY;

/* EM Configuration
 * (Can change in run-time) */
typedef struct tagEMConfig
{
    uint16_t                voltage_low;                /* Voltage lowest RMS level.  Unit: Volt */
    uint16_t                voltage_high;               /* Voltage highest RMS level. Unit: Volt */
    uint8_t                 current_high;               /* Current highest RMS level. Unit: Ampere */
    uint8_t                 freq_low;                   /* Lowest frequency.  Unit: Hz */
    uint8_t                 freq_high;                  /* Highest frequency. Unit: Hz */
    uint8_t                 max_demand_period;          /* Max demand time of an intergration period. Unit: Minute */
    struct
    {
        uint8_t         pulse0_source;
        uint8_t         pulse0_line;
        uint8_t         pulse1_source;
        uint8_t         pulse1_line;
        uint8_t         pulse2_source;
        uint8_t         pulse2_line;
        uint8_t         pulse3_source;
        uint8_t         pulse3_line;
        uint8_t         pulse4_source;
        uint8_t         pulse4_line;
    } pulse;
    uint8_t                 number_tariff;              /* Number of tariff */
    uint8_t                 number_timeslot;            /* Number of time slot */
    __near EM_TIME_SLOT *   timeslot_list;              /* Timeslot info list */
    
} EM_CONFIG;    /* EM Configuration */

/* EM Calibration Informations
 * (Can change in run-time) */
typedef struct tagEMCalibrationCoeffPhase
{   
    /* Co-efficient (Used for both HW Support & SW Support) */
    /* RMS Co-efficient */
    float32_t   vrms;               /* VRMS Co-efficient */
    float32_t   irms;               /* IRMS Co-efficient - Phase Current Co-efficient */
    
    /* Power Co-efficient (for phase line) */
    float32_t   active_power;       /* Active power co-efficient */
    float32_t   reactive_power;     /* Reactive power co-efficient */
    float32_t   apparent_power;     /* Apparent power co-efficient */
    
} EM_CALIB_COEFF_PHASE;

typedef struct tagEMCalibrationOffsetPhase
{
    EM_SW_SAMP_TYPE v;
    EM_SW_SAMP_TYPE i;
    
} EM_CALIB_OFFSET_PHASE;

typedef struct tagEMCalibrationCoeffNeutral
{   
    float32_t   irms;               /* IRMS Co-efficient - Phase Current Co-efficient */
    
} EM_CALIB_COEFF_NEUTRAL;

typedef struct tagEMCalibPhaseCorrection
{
    __near float32_t*  i_phase_degrees;    /* I Phase Angle Degree List */
    
} EM_CALIB_PHASE;

typedef struct tagEMCalibGain
{
    __near float32_t*  i_gain_values;      /* I Gain Value List */

} EM_CALIB_GAIN;

typedef struct tagEMCalibration
{
    struct 
    {
        float32_t       sampling_frequency;         /* Actual sampling frequency of the meter */
        uint16_t        meter_constant_phase;       /* Meter constant. Unit: Pulse/KWh */
        uint16_t        meter_constant_total;       /* Meter constant. Unit: Pulse/KWh */
        uint16_t        pulse_on_time;              /* Pulse on time. Unit: ms */
        float32_t       rtc_comp_offset;            /* RTC compensation offset */
    } common;
    
    struct
    {
        EM_CALIB_COEFF_PHASE    phase_r;
        EM_CALIB_COEFF_PHASE    phase_y;
        EM_CALIB_COEFF_PHASE    phase_b;
        EM_CALIB_COEFF_NEUTRAL  neutral;
    } coeff;
    
    struct
    {
        EM_CALIB_OFFSET_PHASE   phase_r;
        EM_CALIB_OFFSET_PHASE   phase_y;
        EM_CALIB_OFFSET_PHASE   phase_b;
    } offset;
    
    struct
    {
        EM_CALIB_PHASE  phase_r;
        EM_CALIB_PHASE  phase_y;
        EM_CALIB_PHASE  phase_b;
    } sw_phase_correction;
    
    struct
    {
        EM_CALIB_GAIN   phase_r;
        EM_CALIB_GAIN   phase_y;
        EM_CALIB_GAIN   phase_b;
        
    }  sw_gain;
    
} EM_CALIBRATION;       /* EM Calibration Informations */

/* EM System State */
typedef enum tagEMSystemState
{
    SYSTEM_STATE_UNINITIALIZED = 0,     /* Uninitialized */
    SYSTEM_STATE_INITIALIZED,           /* Initialized */
    SYSTEM_STATE_RUNNING                /* Running */
} EM_SYSTEM_STATE;  /* EM System State */


typedef struct tagEMSamples
{
    /* Phase_R */
    struct
    {
        EM_SW_SAMP_TYPE     i;              /* Current */
        EM_SW_SAMP_TYPE     v;              /* Voltage */
        EM_SW_SAMP_TYPE     v90;            /* Voltage 90 degree phase shift */
        EM_SW_SAMP_TYPE     i_fund;         /* Filtered current for fundamental calculation */
        EM_SW_SAMP_TYPE     v_fund;         /* Filtered voltage for fundamental calculation */
        
    } phase_r;
    
    /* Phase_Y */
    struct
    {
        EM_SW_SAMP_TYPE     i;              /* Current */
        EM_SW_SAMP_TYPE     v;              /* Voltage */
        EM_SW_SAMP_TYPE     v90;            /* Voltage 90 degree phase shift */
        EM_SW_SAMP_TYPE     i_fund;         /* Filtered current for fundamental calculation */
        EM_SW_SAMP_TYPE     v_fund;         /* Filtered voltage for fundamental calculation */
        
    } phase_y;
    
    /* Phase_B */
    struct
    {
        EM_SW_SAMP_TYPE     i;              /* Current */
        EM_SW_SAMP_TYPE     v;              /* Voltage */
        EM_SW_SAMP_TYPE     v90;            /* Voltage 90 degree phase shift */
        EM_SW_SAMP_TYPE     i_fund;         /* Filtered current for fundamental calculation */
        EM_SW_SAMP_TYPE     v_fund;         /* Filtered voltage for fundamental calculation */
        
    } phase_b;
    
    /* Neutral */
    struct 
    {
        EM_SW_SAMP_TYPE     i;              /* Current */
    } neutral;
    
    /* Status info */
    struct
    {
        EM_FUND_SEQUENCE    fund_sequence;   /* Accumulation sequence of filtered signal */
        uint8_t             reserved;        /* Reserved */
    } status;
    
} EM_SAMPLES;

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

#endif /* _EM_TYPE_H */
