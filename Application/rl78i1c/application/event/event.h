/***********************************************************************************************************************
* File Name    : event.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Event processing source File
***********************************************************************************************************************/

#ifndef _EVENT_H
#define _EVENT_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "em_core.h"                /* EM Core */
#include "em_anti_tamper_plugin.h"  /* EM Core Anti-tamper Plugin */
#include "em_event_plugin.h"        /* EM Core Event Plugin */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define EVENT_OK        0
#define EVENT_ERROR     1


/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Call to this API to register all event callback */
uint8_t EVENT_RegisterAll(void);

/* Call to this API on while loop of main() */
void EVENT_PollingProcessing(void);

/* Event acknowledgement functions */
uint8_t EVENT_IsAllEventProcessed(void);
uint8_t EVENT_IsCaseOpen(void);
uint8_t EVENT_IsPowerFail(void);
uint8_t EVENT_IsNeutralMissing(void);
uint8_t EVENT_IsMagnet(void);

/* Event callback API */
void EVENT_MaxDemandEnd(uint8_t bchange);
void EVENT_CaseOpen(EM_EXT_CALLBACK_STATE state);
void EVENT_Magnetic(EM_EXT_CALLBACK_STATE state);
void EVENT_PowerFail(EM_EXT_CALLBACK_STATE state);
void EVENT_NeutralMissing(EM_EXT_CALLBACK_STATE state);

void EVENT_MissingPotential_R(EM_EXT_CALLBACK_STATE state);
void EVENT_MissingPotential_Y(EM_EXT_CALLBACK_STATE state);
void EVENT_MissingPotential_B(EM_EXT_CALLBACK_STATE state);

void EVENT_OverVoltage_R(EM_EXT_CALLBACK_STATE state);
void EVENT_OverVoltage_Y(EM_EXT_CALLBACK_STATE state);
void EVENT_OverVoltage_B(EM_EXT_CALLBACK_STATE state);

void EVENT_LowVoltage_R(EM_EXT_CALLBACK_STATE state);
void EVENT_LowVoltage_Y(EM_EXT_CALLBACK_STATE state);
void EVENT_LowVoltage_B(EM_EXT_CALLBACK_STATE state);

void EVENT_VoltageUnbalance(EM_EXT_CALLBACK_STATE state);

void EVENT_ReverseCurrent_R(EM_EXT_CALLBACK_STATE state);
void EVENT_ReverseCurrent_Y(EM_EXT_CALLBACK_STATE state);
void EVENT_ReverseCurrent_B(EM_EXT_CALLBACK_STATE state);

void EVENT_OpenCurrent_R(EM_EXT_CALLBACK_STATE state);
void EVENT_OpenCurrent_Y(EM_EXT_CALLBACK_STATE state);
void EVENT_OpenCurrent_B(EM_EXT_CALLBACK_STATE state);

void EVENT_OverCurrent_R(EM_EXT_CALLBACK_STATE state);
void EVENT_OverCurrent_Y(EM_EXT_CALLBACK_STATE state);
void EVENT_OverCurrent_B(EM_EXT_CALLBACK_STATE state);

void EVENT_CurrentUnbalance(EM_EXT_CALLBACK_STATE state);
void EVENT_CurrentBypass(EM_EXT_CALLBACK_STATE state);

#endif /* _EVENT_H */

