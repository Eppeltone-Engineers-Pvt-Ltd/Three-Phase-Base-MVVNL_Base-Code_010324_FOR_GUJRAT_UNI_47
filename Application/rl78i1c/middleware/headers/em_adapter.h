/***********************************************************************************************************************
* File Name    : em_adapter.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Shared Adapter Layer APIs
***********************************************************************************************************************/

#ifndef _EM_ADAPTER_H
#define _EM_ADAPTER_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"                /* GSCE Standard Typedef */
#include "em_type.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Enumeration of callback state */
typedef enum tagEMExtCallbackState
{
    EM_EXT_CALLBACK_STATE_ENTER,    /* Enter state */
    EM_EXT_CALLBACK_STATE_RELEASE   /* Release state */
} EM_EXT_CALLBACK_STATE;

/* Plugin/Event Callback API */
typedef void (*EM_EXT_CALLBACK)     (EM_EXT_CALLBACK_STATE);        /* Default plugin/event callback prototype */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef EM_EXT_OK
#define EM_EXT_OK       0   /* OK */
#endif
#ifndef EM_EXT_ERROR
#define EM_EXT_ERROR    1   /* Error */
#endif

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/

/* Shared Adapter Information Provider API */
float32_t EM_EXT_GetVRMS(EM_LINE line);             /* Get VRMS */
float32_t EM_EXT_GetIRMS(EM_LINE line);             /* Get IRMS */
float32_t EM_EXT_GetPhaseCurrentSumVector(void);    /* Get IVECTOR */

int8_t EM_EXT_GetActivePowerSign(EM_LINE line);     /* Get Active Power Sign (+, -, 0) */
int8_t EM_EXT_GetReactivePowerSign(EM_LINE line);   /* Get Reactive Power Sign (+, -, 0) */
int8_t EM_EXT_GetApparentPowerSign(EM_LINE line);   /* Get Apparent Power Sign (+, 0) */


#endif /* _EM_ADAPTER_H */
