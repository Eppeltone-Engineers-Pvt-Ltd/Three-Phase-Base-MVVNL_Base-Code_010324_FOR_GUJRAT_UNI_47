/***********************************************************************************************************************
* File Name    : wrp_em_rtc.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper RTC APIs
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_RTC_H
#define _WRAPPER_EM_RTC_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct tagEMRTCDateTime
{
    uint8_t Sec;       /* Second */
    uint8_t Min;       /* Minute */
    uint8_t Hour;      /* Hour */
    uint8_t Day;       /* Day */
    uint8_t Week;      /* Day of week */
    uint8_t Month;     /* Month */
    uint8_t Year;      /* Year (ony 2 ending digit) */
    uint8_t reserve;   /* Padding */
} EM_RTC_DATE_TIME;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
void EM_RTC_Init(void);
void EM_RTC_Start(void);
void EM_RTC_Stop(void);
void EM_RTC_GetDateTime(EM_RTC_DATE_TIME *datetime);

/* Interrupt callback registration to lower layer */
void EM_RTC_ConstInterruptCallback(void);

#endif /* _WRAPPER_EM_RTC_H */