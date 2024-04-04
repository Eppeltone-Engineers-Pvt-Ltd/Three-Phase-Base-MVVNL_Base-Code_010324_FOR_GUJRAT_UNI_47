/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : powermgmt.h
* Version      : 1.00
* Description  : Power Management Module
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 18.11.2012 
******************************************************************************/

#ifndef _POWERMGMT_H
#define _POWERMGMT_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */
#include "em_adapter.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum tagPowermgmtMode
{
    POWERMGMT_MODE1 = 1U,
    POWERMGMT_MODE2,
    POWERMGMT_MODE3,
    POWERMGMT_MODE4,
    POWERMGMT_MODE5,
    POWERMGMT_MODE7,
} powermgmt_mode_t;


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
void POWERMGMT_PollingProcessing(void);
powermgmt_mode_t POWERMGMT_GetMode(void);

/* Callback functions */
void POWERMGMT_RTC_InterruptCallback(void);         /* To be registered into interrupt RTC driver */
void POWERMGMT_COVER_InterruptCallback(void);       /* To be registered into interrupt COVER driver */
void POWERMGMT_KEY_InterruptCallback(void);         /* To be registered into interrupt KEY driver */
void POWERMGMT_VDD_InterruptCallback(void);         /* To be registered into interrupt VDDLVD driver */
void POWERMGMT_MAGNETIC_InterruptCallback(void);    /* To be registered into interrupt MAGNETIC driver */
void POWERMGMT_VRTC_InterruptCallback(void);        /* To be registered into interrupt VRTCLVD driver */
void POWERMGMT_VBATT_InterruptCallback(void);       /* To be registered into interrupt VBATTLVD driver */
void POWERMGMT_AC_InterruptCallback(void);          /* To be registered into interrupt EXLVD driver */
uint8_t POWERMGMT_IsACLow(void);
uint8_t POWERMGMT_IsVBATTLow(void);
uint8_t POWERMGMT_IsVRTCLow(void);
void POWERMGMT_DoAfterStop(void);
void POWERMGMT_DoBeforeStop(void);

void POWERMGMT_NeutralMissing(EM_EXT_CALLBACK_STATE state);
void checkCoverTamper(void);
void POWERMGMT_Set_AC_LOW(void);
void POWERMGMT_Clear_AC_LOW(void);
void POWERMGMT_setCurrentMode1(void);


uint8_t POWERMGMT_ProcessMode1(void);
uint8_t POWERMGMT_ProcessMode2(void);
uint8_t POWERMGMT_ProcessMode3(void);
uint8_t POWERMGMT_ProcessMode3_Pseudo(void);
uint8_t POWERMGMT_ProcessMode4(void);
uint8_t POWERMGMT_ProcessMode5(void);
uint8_t POWERMGMT_ProcessMode7(void);

void turnOff_UARTS(void);


#endif /* _POWERMGMT_H */

