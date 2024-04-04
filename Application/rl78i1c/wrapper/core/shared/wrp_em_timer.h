/***********************************************************************************************************************
* File Name    : wrp_em_timer.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper EM TIMER APIs
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_TIMER_H
#define _WRAPPER_EM_TIMER_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

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
void EM_TIMER_Init(void);
void EM_TIMER_Start(void);
void EM_TIMER_Stop(void);

/* Interrupt callback registration to lower layer */
void EM_TIMER_InterruptCallback(void);

#endif /* _WRAPPER_EM_TIMER_H */