/***********************************************************************************************************************
* File Name    : em_anti_tamper_plugin.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Anti-tamper Plugin Header
***********************************************************************************************************************/

#ifndef _EM_ANTI_TAMPER_PLUGIN_H
#define _EM_ANTI_TAMPER_PLUGIN_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */
#include "em_adapter.h"      /* Shared Plugin Definition */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Anti-tamper plugin registration */
uint8_t EM_EXT_TAMPER_RegisterPlugin(
/*
		EM_EXT_CALLBACK     cb_neutral_missing,
    EM_EXT_CALLBACK     cb_MissingPotential_R,
    EM_EXT_CALLBACK     cb_MissingPotential_Y,
    EM_EXT_CALLBACK     cb_MissingPotential_B,
    EM_EXT_CALLBACK     cb_VoltageUnbalance,
    EM_EXT_CALLBACK     cb_ReverseCurrent_R,
    EM_EXT_CALLBACK     cb_ReverseCurrent_Y,
    EM_EXT_CALLBACK     cb_ReverseCurrent_B,
    EM_EXT_CALLBACK     cb_OpenCurrent_R,
    EM_EXT_CALLBACK     cb_OpenCurrent_Y,
    EM_EXT_CALLBACK     cb_OpenCurrent_B,
    EM_EXT_CALLBACK     cb_CurrentUnbalance,
*/
		EM_EXT_CALLBACK     cb_CurrentBypass
);

#endif /* _EM_ANTI_TAMPER_PLUGIN_H */
