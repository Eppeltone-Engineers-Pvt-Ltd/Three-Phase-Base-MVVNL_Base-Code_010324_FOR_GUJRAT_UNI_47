/***********************************************************************************************************************
* File Name    : 
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Shared Adapter Layer APIs
***********************************************************************************************************************/

#ifndef _RL78I1C_SW_HPF_CONFIG_H
#define _RL78I1C_SW_HPF_CONFIG_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"                /* GSCE Standard Typedef */
#include "math.h"

/* Check to ensure RL78-S3 CPU core is using */
#if (__RL78_S3__ != 1)
    #error "Must be using RL78-S3 CPU core"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct tagSoftwareHPF16Buffer
{
    int16_t v_input_prev;
    int16_t v_output_prev;
    int32_t remainder_acc;
} RL78I1C_HPF16_BUFFER;

typedef enum tagSoftwareHPF16
{
    RL78I1C_HPF16_OK,
    RL78I1C_HPF16_ERROR,
    
} RL78I1C_HPF16_STATUS;
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define RL78I1C_HPF16_MAX_NUM_OF_CHANNEL                 (6)
#define RL78I1C_HPF_CUTOFF_FREQ_REG_SETTING_MAX          (3)

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
RL78I1C_HPF16_STATUS RL78I1C_InitSoftwareHPF16(uint8_t cutoff_def);
void RL78I1C_ResetSoftwareHPF16(uint8_t channel);
int16_t RL78I1C_DoSoftwareHPF16(int16_t input, uint8_t channel);

#endif /* _EM_SW_HPF_CONFIG_H */