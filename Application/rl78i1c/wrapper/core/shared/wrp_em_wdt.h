/***********************************************************************************************************************
* File Name    : wrp_em_wdt.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper WDT APIs
***********************************************************************************************************************/

#ifndef _WRAPPER_EM_WDT_H
#define _WRAPPER_EM_WDT_H

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
void EM_WDT_Init(void);
void EM_WDT_Start(void);
void EM_WDT_Stop(void);
void EM_WDT_Restart(void);

#endif /* _WRAPPER_EM_WDT_H */
