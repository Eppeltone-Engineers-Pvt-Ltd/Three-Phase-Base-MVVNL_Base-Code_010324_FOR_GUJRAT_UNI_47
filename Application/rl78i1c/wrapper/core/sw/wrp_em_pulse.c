/***********************************************************************************************************************
* File Name    : wrp_em_pulse.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper EM PULSE APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "iodefine.h"
#include "wrp_em_sw_config.h"       /* Wrapper Shared Configuration */
#include "wrp_em_pulse.h"           /* EM PULSE Wrapper Layer */
#include "em_type.h"                /* EM Core Type Definitions */
#include "em_constraint.h"          /* EM Core Configuration Constraint */
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro Definitions Checking
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name    : void EM_PULSE_Init(void)
* Description      : EM Pulse Initialization
* Arguments        : None
* Functions Called : EM_PULSE_DriverInit()
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE_Init(void)
{
    EM_PULSE_DriverInit();
}
 /***********************************************************************************************************************
* Function Name    : void EM_PULSE0_On(void)
* Description      : EM Pulse 0 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE0_ON_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE0_On(void)
{
    EM_PULSE0_ON_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE0_Off(void)
* Description      : EM Pulse 0 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE0_OFF_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE0_Off(void)
{
    EM_PULSE0_OFF_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE1_On(void)
* Description      : EM Pulse 1 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE1_ON_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE1_On(void)
{
    EM_PULSE1_ON_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE1_Off(void)
* Description      : EM Pulse 1 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE1_OFF_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE1_Off(void)
{
    EM_PULSE1_OFF_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE2_On(void)
* Description      : EM Pulse 2 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE2_ON_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE2_On(void)
{
    EM_PULSE2_ON_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE2_Off(void)
* Description      : EM Pulse 2 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE2_OFF_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE2_Off(void)
{
    EM_PULSE2_OFF_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE3_On(void)
* Description      : EM Pulse 3 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE3_ON_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE3_On(void)
{
    EM_PULSE3_ON_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE3_Off(void)
* Description      : EM Pulse 3 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE3_OFF_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE3_Off(void)
{
    EM_PULSE3_OFF_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE4_On(void)
* Description      : EM Pulse 4 LED Turned on
* Arguments        : None
* Functions Called : EM_PULSE4_ON_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE4_On(void)
{
    EM_PULSE4_ON_STATEMENT;
}

/***********************************************************************************************************************
* Function Name    : void EM_PULSE4_Off(void)
* Description      : EM Pulse 4 LED Turned off
* Arguments        : None
* Functions Called : EM_PULSE4_OFF_STATEMENT
* Return Value     : None
***********************************************************************************************************************/
void EM_PULSE4_Off(void)
{
    EM_PULSE4_OFF_STATEMENT;
}
