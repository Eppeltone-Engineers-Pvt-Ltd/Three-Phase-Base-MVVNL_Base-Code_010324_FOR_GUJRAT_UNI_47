/***********************************************************************************************************************
* File Name    : wrp_em_sw_property.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper setting SW Property APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "wrp_em_sw_config.h"
#include "wrp_em_sw_property.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
static __near const EM_SW_PROPERTY em_sw_property = 
{
    {
        /* ADC */
        /* Phase_R */
        {
            EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL_PHASE_R,
            0,
            EM_SW_PROPERTY_ADC_GAIN_UPPER_THRESHOLD_PHASE_R,
            EM_SW_PROPERTY_ADC_GAIN_LOWER_THRESHOLD_PHASE_R,
        },
        
        /* Phase_Y */
        {
            EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL_PHASE_Y,
            0,
            EM_SW_PROPERTY_ADC_GAIN_UPPER_THRESHOLD_PHASE_Y,
            EM_SW_PROPERTY_ADC_GAIN_LOWER_THRESHOLD_PHASE_Y,
        },
        
        /* Phase_B */
        {
            EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL_PHASE_B,
            0,
            EM_SW_PROPERTY_ADC_GAIN_UPPER_THRESHOLD_PHASE_B,
            EM_SW_PROPERTY_ADC_GAIN_LOWER_THRESHOLD_PHASE_B,
        },
        
        /* SAMP */
        EM_SW_PROPERTY_ADC_CONVERSION_TIME,
     },
};

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : EM_SW_GetProperty
* Interface     : const EM_SW_PROPERTY* EM_SW_GetProperty(void)
* Description   : Get SW Property Setting From Wrapper Layer
* Arguments     : None
* Function Calls: None
* Return Value  : const EM_SW_PROPERTY*, point to the SW Property Structure
***********************************************************************************************************************/
__near EM_SW_PROPERTY * EM_SW_GetProperty(void)
{
    return (__near EM_SW_PROPERTY*)&em_sw_property;
}
