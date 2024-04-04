/***********************************************************************************************************************
* File Name    : r_calib_sampling.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Meter Calibration Sampling APIs
***********************************************************************************************************************/

#ifndef _R_CALIB_SAMPLING_H
#define _R_CALIB_SAMPLING_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"                /* GSCE Standard Typedef */
#include "em_accumulation.h"        /* 24-bit accumulation header */

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/* Accumulator type */
typedef acc24_t accumulator_t;
    
/* DC Offset */
typedef struct tagEMSoftwareDcOffset
{
    int32_t     acc;        /* Signal accumulator */
    int32_t     average;    /* Offset value */
    int32_t     average_prev;
} EM_SW_DCOFFSET;   /* DC Offset */
    
/* Calculation data */
typedef struct tag_calib_data_t
{   
    /* Common for accumulation */
    uint32_t number_of_cycles;
    
    /* Stable count */
    uint16_t stable_count;
    
    /* Common control flag */
    uint8_t  v_start_flag;
    uint8_t  i1_start_flag;
    uint8_t  i2_start_flag; 
    
    struct
    {
        EM_SW_DCOFFSET  v;
        EM_SW_DCOFFSET  i;
        uint16_t        v_ready;
        uint16_t        v_count;
        uint8_t         reserved;
        uint8_t         number_of_cycles;
        uint16_t        number_of_samples;
    } offset;
    
    /* Accumulator */
    struct
    {
        /* For vcoeff, icoeff, pcoeff */
        uint8_t         is_done;            /* Finish accumulated flag */       
        
        accumulator_t   vrms;               /* VRMS Accumulator */
        uint32_t        nvrms;              /* Number of sample of VRMS */
        
        accumulator_t   i1rms;              /* I1RMS Accumulator */
        uint32_t        ni1rms;             /* Number of sample of I1RMS */
        
        accumulator_t   i2rms;              /* I2RMS Accumulator */
        uint32_t        ni2rms;             /* Number of sample of I2RMS */

        /* For phase angle error 0, 1 */
        accumulator_t   power0_active;      /* Accumulator for Active Power 0 */
        accumulator_t   power1_active;      /* Accumulator for Active Power 1 */
        uint32_t        npower;             /* Number of sample of power sample */      
        uint8_t         angle_is_done;      /* Finish accumulated flag */
        
    } acc;
    
    /* For phase shift controller */
    uint8_t     phase_choosen;              /* Phase choosen, 0 is Ip -> V, 1 is V -> Ip */
    uint8_t     is_finish_calc_angle_error; /* Flag indicate finish angle calculation or not */
    float32_t   last_theta;                 /* Last theta angle */
    float32_t   theta;                      /* Calculate theta angle */
    float32_t   theta90;                    /* Calculate theta angle */
    float32_t   theta90_timer_offset;       /* Calculate theta angle */
    float32_t   derivation;                 /* Derivation value */
    float32_t   initial_delta_error;        /* Initial delta error rate */
    float32_t   last_delta_error;           /* Last delta error */
    
    struct
    {
        float32_t   vrms;                   /* VRMS */
        uint32_t    nvrms;                  /* Number of sample of VRMS */
        
        float32_t   i1rms;                  /* I1RMS */
        uint32_t    ni1rms;                 /* Number of sample of I1RMS */
        
        float32_t   i2rms;                  /* I2RMS */
        uint32_t    ni2rms;                 /* Number of sample of I2RMS */
        
        float32_t   power0_active;          /* Active Power 0 */
        float32_t   power1_active;          /* Active Power 1 */
        uint32_t    npower;                 /* Number of sample of power */
        
    } storage;
    
    /* For sampling frequency calculation */
    struct
    {
        uint8_t     is_done;                /* Flag acknowledge finish sampling for fs */
        uint16_t    nsample;                /* Total sample in one line cycle */
        
    } fs;
    
    /* For gain calculation */
    struct
    {
        uint8_t     idone;                  /* Flag acknowledge finish sampling for phase gain */       
        uint8_t     is_start;               /* Start the checking for gain or not */
        uint8_t     igain;                  /* Calculated phase gain */
        int32_t     imax;                   /* Max value in one line cycle */
        
    } gain;
    
} calib_data_t;

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/
extern calib_data_t g_calib_info;

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
void EM_CALIB_Init(void);
void EM_CALIB_Start(
    float32_t imax, 
    float32_t volt, 
    float32_t current, 
    uint16_t cycles, 
    uint16_t cycle_phase_error,
    uint8_t line
);
void EM_CALIB_Stop(void);
float32_t EM_CALIB_CalculateSamplingFrequency(void);
float32_t EM_CALIB_CalculateAndSetCurrentGain(void);
uint8_t EM_CALIB_DoAccumulate(void);

#endif /* _R_CALIB_SAMPLING_H */

