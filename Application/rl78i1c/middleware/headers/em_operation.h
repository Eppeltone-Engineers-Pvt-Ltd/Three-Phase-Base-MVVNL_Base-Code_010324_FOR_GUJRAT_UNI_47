/***********************************************************************************************************************
* File Name    : em_operation.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Core Operation Middleware Header file
***********************************************************************************************************************/

#ifndef _EM_OPERATION_H
#define _EM_OPERATION_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "em_constraint.h"  /* EM Constraint Definitions */
#include "em_errcode.h"     /* EM Error Code Definitions */
#include "em_type.h"        /* EM Type Definitions */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Core Operation */
uint8_t EM_Init(__near  EM_PLATFORM_PROPERTY *p_property, __near  EM_CONFIG * p_config, __near EM_CALIBRATION * p_calib);
uint8_t EM_Start(void);
uint8_t EM_Stop(void);
EM_SYSTEM_STATE EM_GetSystemState(void);

/* Configuration */
EM_CONFIG EM_GetConfig(void);
uint8_t EM_SetConfig(__near EM_CONFIG * p_config);

/* Recovery */
uint8_t EM_GetState(__near uint8_t *buf, uint16_t bufsize, __near uint16_t *statesize);
uint8_t EM_SetState(__near uint8_t *buf, uint16_t bufsize, uint16_t statesize);

/* Status */
EM_STATUS EM_GetStatus(void);

/* Max demand */
uint8_t EM_ResetMaxDemand(EM_SOURCE_MAX_DEMAND source, EM_LINE line);

/* Energy */
uint8_t EM_ResetEnergyTariff(uint8_t tariff, EM_SOURCE_ENERGY source, EM_LINE line);
uint8_t EM_ResetEnergyTotal(EM_SOURCE_ENERGY source, EM_LINE line);
uint8_t EM_AddEnergyToCurrentTariff(float32_t power, float32_t second, EM_LINE line);

/* Tariff */
uint8_t EM_GetActiveTariff(void);

/* Neutral Missing Operation */
void EM_NM_EnterNeutralMissingMode(void);
void EM_NM_ReleaseNeutralMissingMode(void);
uint8_t EM_NM_IsNeutralMissing(void);

#endif /* _EM_OPERATION_H */
