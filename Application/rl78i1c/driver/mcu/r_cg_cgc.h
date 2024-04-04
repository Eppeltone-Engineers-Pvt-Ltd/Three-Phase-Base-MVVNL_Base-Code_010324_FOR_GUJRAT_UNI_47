/***********************************************************************************************************************
* File Name    : r_cg_cgc.h
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for CGC module.
* Creation Date: 
***********************************************************************************************************************/
#ifndef CGC_H
#define CGC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Clock operation mode control register (CMC)
*/
/* High-speed system clock pin operation mode (EXCLK, OSCSEL) */
#define _C0_CGC_HISYS_PIN           (0xC0U)
#define _00_CGC_HISYS_PORT          (0x00U) /* X1, X2 as I/O port */
#define _40_CGC_HISYS_OSC           (0x40U) /* X1, X2 as crystal/ceramic resonator connection */
#define _80_CGC_HISYS_PORT1         (0x80U) /* X1, X2 as I/O port */
#define _C0_CGC_HISYS_EXT           (0xC0U) /* X1 as I/O port, X2 as external clock input */

/* Subsystem clock selection control (XT1SELEN) */
#define _00_CGC_XT1_SELECT_DIS      (0x00U) /* Subclock can not be select as fCLK */
#define _10_CGC_XT1_SELECT_ENA      (0x10U) /* Subclock can be select as fCLK */

/* Control of X1 high-speed system clock oscillation frequency (AMPH) */
#define _00_CGC_SYSOSC_UNDER10M     (0x00U) /* 1MHz <= fX <= 10MHz */
#define _01_CGC_SYSOSC_OVER10M      (0x01U) /* 10MHz < fX <= 20MHz */

/*
    Sub Clock operation mode control register (SCMC)
*/
/* Subsystem clock pin operation mode (EXCLKS, OSCSELS) */
#define _30_CGC_SUB_PIN             (0x30U)
#define _00_CGC_SUB_PORT            (0x00U) /* XT1, XT2 as I/O port */
#define _10_CGC_SUB_OSC             (0x10U) /* XT1, XT2 as crystal connection */
#define _20_CGC_SUB_PORT1           (0x20U) /* XT1, XT2 as I/O port */
#define _30_CGC_SUB_EXT             (0x30U) /* XT1 as I/O port, XT2 as external clock input */
/* XT1 oscillator oscillation mode selection (AMPHS1, AMPHS0) */
#define _00_CGC_SUBMODE_DEFAULT     (0x00U)
#define _00_CGC_SUBMODE_LOW         (0x00U) /* low power consumption oscillation */
#define _02_CGC_SUBMODE_NORMAL      (0x02U) /* normal oscillation */
#define _04_CGC_SUBMODE_ULOW        (0x04U) /* ultra-low power consumption oscillation */

/*
    Sub Clock operation status control register (SCSC)
*/
#define _00_CGC_SUB_OPER            (0x00U) /* XT1 oscillator operating */
#define _40_CGC_SUB_STOP            (0x40U) /* XT1 oscillator stopped */

/*
    System clock control register (CKC)
*/
/* Status of CPU/peripheral hardware clock fCLK (CLS) */
#define _00_CGC_CPUCLK_MAIN         (0x00U) /* main system clock (fMAIN) */
#define _80_CGC_CPUCLK_SUB          (0x80U) /* subsystem clock (fSUB) */
/* Selection of CPU/peripheral hardware clock fCLK (CSS) */
#define _00_CGC_CPUCLK_SELMAIN      (0x00U) /* main system clock (fMAIN) */
#define _40_CGC_CPUCLK_SELSUB       (0x40U) /* subsystem clock (fSUB) */
/* Status of Main system clock fMAIN (MCS) */
#define _00_CGC_MAINCLK_HIO         (0x00U) /* internal OCO clock (fOCO) */
#define _20_CGC_MAINCLK_HISYS       (0x20U) /* high-speed system clock (fMX) */
/* Selection of Main system clock fMAIN (MCM0) */
#define _00_CGC_MAINCLK_SELHIO      (0x00U) /* inter OCO clock (fOCO) */
#define _10_CGC_MAINCLK_SELHISYS    (0x10U) /* high-speed system clock (fMX) */
/* Status of Main On chip clock fOCO (MCS1) */
#define _00_CGC_OCOCLK_HIO          (0x00U) /* high-speed OCO clock (fIH) */
#define _02_CGC_OCOCLK_MIO          (0x02U) /* middle-speed OCO clock (fIM) */
/* Selection of Main On chip clock fOCO (MCM1) */
#define _00_CGC_OCOCLK_SELHIO       (0x00U) /* high-speed OCO clock (fIH) */
#define _01_CGC_OCOCLK_SELMIO       (0x01U) /* middle-speed OCO clock (fIM) */


/*
    Clock operation status control register (CSC)
*/
/* Control of high-speed system clock operation (MSTOP) */
#define _00_CGC_HISYS_OPER          (0x00U) /* X1 oscillator/external clock operating */
#define _80_CGC_HISYS_STOP          (0x80U) /* X1 oscillator/external clock stopped */
/* Subsystem clock operation (XT1SELDIS) */
#define _00_CGC_SUB_ENA             (0x00U) /* XT1 oscillator operating */
#define _40_CGC_SUB_DIS             (0x40U) /* XT1 oscillator stopped */
/* Middle speed clock operation (MIOEN) */
#define _00_CGC_MIO_STOP            (0x00U) /* XT1 oscillator operating */
#define _02_CGC_MIO_OPER            (0x40U) /* XT1 oscillator stopped */
/* High-speed OCO operation (HIOSTOP) */
#define _00_CGC_HIO_OPER            (0x00U) /* high-speed OCO operating */
#define _01_CGC_HIO_STOP            (0x01U) /* high-speed OCO stopped */

/*
    Oscillation stabilization time counter status register (OSTC)
*/
/* Oscillation stabilization time status (MOST18 - MOST8) */
#define _00_CGC_OSCSTAB_STA0        (0x00U) /* < 2^8/fX */
#define _80_CGC_OSCSTAB_STA8        (0x80U) /* 2^8/fX */
#define _C0_CGC_OSCSTAB_STA9        (0xC0U) /* 2^9/fX */
#define _E0_CGC_OSCSTAB_STA10       (0xE0U) /* 2^10/fX */
#define _F0_CGC_OSCSTAB_STA11       (0xF0U) /* 2^11/fX */
#define _F8_CGC_OSCSTAB_STA13       (0xF8U) /* 2^13/fX */
#define _FC_CGC_OSCSTAB_STA15       (0xFCU) /* 2^15/fX */
#define _FE_CGC_OSCSTAB_STA17       (0xFEU) /* 2^17/fX */
#define _FF_CGC_OSCSTAB_STA18       (0xFFU) /* 2^18/fX */

/*
    Oscillation stabilization time select register (OSTS)
*/
/* Oscillation stabilization time selection (OSTS2 - OSTS0) */
#define _00_CGC_OSCSTAB_SEL8        (0x00U) /* 2^8/fX */
#define _01_CGC_OSCSTAB_SEL9        (0x01U) /* 2^9/fX */
#define _02_CGC_OSCSTAB_SEL10       (0x02U) /* 2^10/fX */
#define _03_CGC_OSCSTAB_SEL11       (0x03U) /* 2^11/fX */
#define _04_CGC_OSCSTAB_SEL13       (0x04U) /* 2^13/fX */
#define _05_CGC_OSCSTAB_SEL15       (0x05U) /* 2^15/fX */
#define _06_CGC_OSCSTAB_SEL17       (0x06U) /* 2^17/fX */
#define _07_CGC_OSCSTAB_SEL18       (0x07U) /* 2^18/fX */


/*
    Operation speed mode control register (OSMC)
*/
/* Setting in subsystem clock HALT mode (RTCLPC) */
#define _00_CGC_SUBINHALT_ON        (0x00U) /* enables supply of subsystem clock to peripheral functions */
#define _80_CGC_SUBINHALT_OFF       (0x80U) /* stops supply to peripheral functions except RTC, interval timer, PCLBUZ and LCD */
/* Selection of operation clock for highaccuracy real-time clock, 12-bit interval timer, and LCD (WUTMMCK0) */
#define _00_CGC_RTC_IT_LCD_CLK_FSUB (0x00U) /* Subsystem clock (fSUB) */
#define _10_CGC_RTC_IT_LCD_CLK_FIL  (0x10U) /* Low-speed on-chip oscillator clock (fIL) */

/*
    Illegal memory access detection control register (IAWCTL)
*/
/* Illegal memory access detection control (IAWEN) */
#define _00_CGC_ILLEGAL_ACCESS_OFF  (0x00U) /* disables illegal memory access detection */
#define _80_CGC_ILLEGAL_ACCESS_ON   (0x80U) /* enables illegal memory access detection */
/* RAM guard area (GRAM1, GRAM0) */
#define _00_CGC_RAM_GUARD_OFF       (0x00U) /* invalid, it is possible to write RAM */
#define _10_CGC_RAM_GUARD_ARAE0     (0x10U) /* 128 bytes from RAM bottom address */
#define _20_CGC_RAM_GUARD_ARAE1     (0x20U) /* 256 bytes from RAM bottom address */
#define _30_CGC_RAM_GUARD_ARAE2     (0x30U) /* 512 bytes from RAM bottom address */
/* PORT register guard (GPORT) */
#define _00_CGC_PORT_GUARD_OFF      (0x00U) /* invalid, it is possible to write PORT register */
#define _04_CGC_PORT_GUARD_ON       (0x04U) /* valid, it is impossible to write PORT register, but possible for read */
/* Interrupt register guard (GINT) */
#define _00_CGC_INT_GUARD_OFF       (0x00U) /* invalid, it is possible to write interrupt register */
#define _02_CGC_INT_GUARD_ON        (0x02U) /* valid, it is impossible to write, but possible for read */
/* CSC register guard (GCSC) */
#define _00_CGC_CSC_GUARD_OFF       (0x00U) /* invalid, it is possible to write CSC register */
#define _01_CGC_CSC_GUARD_ON        (0x01U) /* valid, it is impossible to write CSC register, but possible for read */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define CGC_SUBWAITTIME             (160U)   /* change the waiting time according to the system */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    HIOCLK, 
    SYSX1CLK, 
    SYSEXTCLK,
    SUBXT1CLK, 
    SUBEXTCLK
} clock_mode_t;


/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_CGC_Create(void);

/* Start user code for function. Do not edit comment generated here */
typedef enum
{
    HOCO_24M_X,
    HOCO_12M_16M,
    HOCO_6M_8M,
    HOCO_3M_4M,
    HOCO_1M5_2M,
    HOCO_X_1M,
} hoco_mode_t;

typedef enum
{
    MOCO_4M,
    MOCO_2M,
    MOCO_1M,
} moco_mode_t;

void R_CGC_FclkUseFmx(void);
void R_CGC_FclkUseFsub(void);
void R_CGC_FclkUseFih(void);
void R_CGC_FclkUsePLL(void);
void R_CGC_FclkUseFim(void);

void R_CGC_ChangeFihClk(hoco_mode_t value);
void R_CGC_ChangeFimClk(moco_mode_t value);

void R_CGC_OperateAt24MHz(void);
void R_CGC_OperateAt12MHz(void);
void R_CGC_OperateAt6MHz(void);

/* End user code. Do not edit comment generated here */
#endif