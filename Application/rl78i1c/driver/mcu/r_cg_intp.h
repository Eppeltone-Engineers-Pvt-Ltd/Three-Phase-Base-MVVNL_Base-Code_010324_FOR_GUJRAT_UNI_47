/***********************************************************************************************************************
* File Name    : r_cg_intp.h
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTP module.
* Creation Date: 
***********************************************************************************************************************/
#ifndef INTP_H
#define INTP_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    External Interrupt Rising Edge Enable Register 0 (EGP0)
*/
/* INTPn pin valid edge selection (EGPn) */
#define _01_INTP0_EDGE_RISING_SEL       (0x01U) /* rising edge selected for INTP0 pin */
#define _00_INTP0_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP0 pin */
#define _02_INTP1_EDGE_RISING_SEL       (0x02U) /* rising edge selected for INTP1 pin */
#define _00_INTP1_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP1 pin */
#define _04_INTP2_EDGE_RISING_SEL       (0x04U) /* rising edge selected for INTP2 pin */
#define _00_INTP2_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP2 pin */
#define _08_INTP3_EDGE_RISING_SEL       (0x08U) /* rising edge selected for INTP3 pin */
#define _00_INTP3_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP3 pin */
#define _10_INTP4_EDGE_RISING_SEL       (0x10U) /* rising edge selected for INTP4 pin */
#define _00_INTP4_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP4 pin */
#define _20_INTP5_EDGE_RISING_SEL       (0x20U) /* rising edge selected for INTP5 pin */
#define _00_INTP5_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP5 pin */
#define _40_INTP6_EDGE_RISING_SEL       (0x40U) /* rising edge selected for INTP6 pin */
#define _00_INTP6_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP6 pin */
#define _80_INTP7_EDGE_RISING_SEL       (0x80U) /* rising edge selected for INTP7 pin */
#define _00_INTP7_EDGE_RISING_UNSEL     (0x00U) /* rising edge not selected for INTP7 pin */

/*
    External Interrupt Falling Edge Enable Register 0 (EGN0)
*/
/* INTPn pin valid edge selection (EGNn) */
#define _01_INTP0_EDGE_FALLING_SEL      (0x01U) /* falling edge selected for INTP0 pin */
#define _00_INTP0_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP0 pin */
#define _02_INTP1_EDGE_FALLING_SEL      (0x02U) /* falling edge selected for INTP1 pin */
#define _00_INTP1_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP1 pin */
#define _04_INTP2_EDGE_FALLING_SEL      (0x04U) /* falling edge selected for INTP2 pin */
#define _00_INTP2_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP2 pin */
#define _08_INTP3_EDGE_FALLING_SEL      (0x08U) /* falling edge selected for INTP3 pin */
#define _00_INTP3_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP3 pin */
#define _10_INTP4_EDGE_FALLING_SEL      (0x10U) /* falling edge selected for INTP4 pin */
#define _00_INTP4_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP4 pin */
#define _20_INTP5_EDGE_FALLING_SEL      (0x20U) /* falling edge selected for INTP5 pin */
#define _00_INTP5_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP5 pin */
#define _40_INTP6_EDGE_FALLING_SEL      (0x40U) /* falling edge selected for INTP6 pin */
#define _00_INTP6_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP6 pin */
#define _80_INTP7_EDGE_FALLING_SEL      (0x80U) /* falling edge selected for INTP7 pin */
#define _00_INTP7_EDGE_FALLING_UNSEL    (0x00U) /* falling edge not selected for INTP7 pin */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_INTC_Create(void);
void R_INTC0_Start(void);
void R_INTC0_Stop(void);
void R_INTC1_Start(void);
void R_INTC1_Stop(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif