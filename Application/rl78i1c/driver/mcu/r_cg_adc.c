/***********************************************************************************************************************
* File Name    : r_cg_adc.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for ADC module.
* Creation Date: 7/14/2015
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_mcu.h"
#include "r_cg_dtc.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* Data area for ADC Result Buffer */

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_ADC_Create
* Description  : This function initializes the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Create(void)
{
    ADCEN = 1U;  /* supply AD clock */
    ADM0 = _00_AD_ADM0_INITIALVALUE;  /* disable AD conversion and clear ADM0 register */
    ADMK = 1U;  /* disable INTAD interrupt */
    ADIF = 0U;  /* clear INTAD interrupt flag */
    /* Set INTAD high priority */
    ADPR1 = 0U;
    ADPR0 = 0U;
    /* The reset status of ADPC is analog input, so it's unnecessary to set. */
    /* Set ANI0 - ANI5 pin */
    PM2 |= 0x3FU;
    ADM0 = _30_AD_CONVERSION_CLOCK_4 | _00_AD_TIME_MODE_NORMAL_1 | _40_AD_OPERMODE_SCAN;
    ADM1 = _80_AD_TRIGGER_HARDWARE_NOWAIT | _20_AD_CONVMODE_ONESELECT | _01_AD_TRIGGER_ELC;
    ADM2 = _80_AD_POSITIVE_INTERVOLT | _00_AD_NEGATIVE_VSS | _00_AD_AREA_MODE_1 | _00_AD_RESOLUTION_10BIT;
    ADUL = _FF_AD_ADUL_VALUE;
    ADLL = _00_AD_ADLL_VALUE;
    ADS = _02_AD_INPUT_CHANNEL_2_5;
}
/***********************************************************************************************************************
* Function Name: R_ADC_Start
* Description  : This function starts the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Start(void)
{
    ADCS = 1U;  /* enable AD conversion */
}

/***********************************************************************************************************************
* Function Name: R_ADC_Start_NoInterrupt
* Description  : This function starts the AD converter with no interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Start_NoInterrupt(void)
{
    ADCS = 1U;  /* enable AD conversion */
}

/***********************************************************************************************************************
* Function Name: R_ADC_Stop
* Description  : This function stops the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Stop(void)
{
    ADCS = 0U;  /* disable AD conversion */
    ADMK = 1U;  /* disable INTAD interrupt */
    ADIF = 0U;  /* clear INTAD interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_ADC_Set_OperationOn
* Description  : This function enables comparator operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Set_OperationOn(void)
{
    /* Turn on reference voltage */
    ADM2 = _80_AD_POSITIVE_INTERVOLT | _00_AD_NEGATIVE_VSS | _00_AD_AREA_MODE_1 | _00_AD_RESOLUTION_10BIT;
    ADCE = 1U;  /* enable AD comparator */
}
/***********************************************************************************************************************
* Function Name: R_ADC_Set_OperationOff
* Description  : This function stops comparator operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Set_OperationOff(void)
{
    ADM2 = 0x00U;   /* Turn off reference voltage convesve power */
    ADCE = 0U;  /* disable AD comparator */
}

/* Start user code for adding. Do not edit comment generated here */\
/***********************************************************************************************************************
* Function Name: R_ADC_User_Start
* Description  : This function start ADC operation when use HW trigger mode
*              : ADCS and ADCE bit will be set to wait for interrupt signal
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_HW_Trigger_Start(void)
{
    R_ADC_Set_OperationOn();
    
    MCU_Delay(5);
    
    ADCS = 1U;  /* enable AD conversion */
}

/***********************************************************************************************************************
* Function Name: R_ADC_User_Start
* Description  : This function start ADC operation when use HW trigger mode
*              : ADCS and ADCE bit will be set to wait for interrupt signal
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_HW_Trigger_Stop(void)
{
    ADCS = 0U;  /* disable AD conversion */
    ADMK = 1U;  /* disable INTAD interrupt */
    ADIF = 0U;  /* clear INTAD interrupt flag */
    
    R_ADC_Set_OperationOff();
    
    MCU_Delay(5);
}

/***********************************************************************************************************************
* Function Name: R_ADC_IsBusy
* Description  : This function check whether the ADC is busy converting or not
* Arguments    : None
* Return Value : ADC Conversion End status: 
               :                            0 - Not busy / idle
               :                            1 - Converting
***********************************************************************************************************************/
uint8_t R_ADC_IsBusy(void)
{
    return (ADCS == 1);
}

/* End user code. Do not edit comment generated here */
