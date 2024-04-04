/***********************************************************************************************************************
* File Name    : em_calibration.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Core Calibration Middleware Header file
***********************************************************************************************************************/

#ifndef _EM_CALIBRATION_H
#define _EM_CALIBRATION_H

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
/* Get/Set/Reset Calibration Info */
EM_CALIBRATION EM_GetCalibInfo(void);
uint8_t EM_SetCalibInfo(__near EM_CALIBRATION * p_calib);

#endif /* _EM_CALIBRATION_H */
