/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : mlsCITimer.h
* Version      : 1.00
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 26.12.2012 
******************************************************************************/

#ifndef _MLSCITIMER_H
#define _MLSCITIMER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "mlsTypes.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* Provide functions */
void mlsSystemTimerInit(void);
void mlsStartReceiveTimer(void);
void mlsStopAndResetReceiveTimer(void);
UInt32 mlsGetSystemTimeCount(void);
void mlsResetSystemTimer(void);
UInt32 mlsGetReceiveTime(void);

/*
 * Required functions
 * Register below functions into timer of system
 */
void mlsSystemTimerCallback(void);

#endif /* _MLSCITIMER_H */
