/***********************************************************************************************************************
* File Name    : wrp_em_hw_property.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper for Setting SW Propery
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_SW_PROPERTY_H
#define _WRAPPER_EM_SW_PROPERTY_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GCSE Standard Definitions */

/******************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct tagEMSoftwareProperty
{   
    struct
    {
        struct
        {
            uint8_t     gain_num_level;                             /* (SW Support Only) ADC gain phase number of level */
            uint8_t     reserved;                                   /* Reserved (for padding) */
            uint32_t    gain_upper_threshold;                       /* (SW Support Only) ADC gain phase upper threshold */
            uint32_t    gain_lower_threshold;                       /* (SW Support Only) ADC gain phase lower threshold */
        } phase_r;              
                        
        struct              
        {               
            uint8_t     gain_num_level;                             /* (SW Support Only) ADC gain phase number of level */
            uint8_t     reserved;                                   /* Reserved (for padding) */
            uint32_t    gain_upper_threshold;                       /* (SW Support Only) ADC gain phase upper threshold */
            uint32_t    gain_lower_threshold;                       /* (SW Support Only) ADC gain phase lower threshold */
        } phase_y;              
                        
        struct              
        {               
            uint8_t     gain_num_level;                             /* (SW Support Only) ADC gain phase number of level */
            uint8_t     reserved;                                   /* Reserved (for padding) */
            uint32_t    gain_upper_threshold;                       /* (SW Support Only) ADC gain phase upper threshold */
            uint32_t    gain_lower_threshold;                       /* (SW Support Only) ADC gain phase lower threshold */
        } phase_b;
        
        float32_t       conversion_time;                            /* (SW Support Only) ADC conversion time */ 
        
    } adc;          /* ADC Module */


} EM_SW_PROPERTY;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Get all HW property information */
__near EM_SW_PROPERTY* EM_SW_GetProperty(void);

#endif  /* _WRAPPER_EM_SW_PROPERTY_H */

