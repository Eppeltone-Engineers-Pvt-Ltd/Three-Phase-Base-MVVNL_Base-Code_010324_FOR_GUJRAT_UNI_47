/***********************************************************************************************************************
* File Name    : r_cg_elc.h
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for ELC module.
* Creation Date: 
***********************************************************************************************************************/

#ifndef ELC_H
#define ELC_H

#include "r_cg_macrodriver.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Event Output Destination Selection Register n (ELSELRn)
*/
/* Event link selection (ELSELn2 - ELSELn0) */
#define _00_ELC_EVENT_LINK_OFF      (0x00U) /* prohibit event link */
#define _01_ELC_EVENT_LINK_AD       (0x01U) /* link destination AD */
#define _02_ELC_EVENT_LINK_TAU00    (0x02U) /* link destination TAU00 */
#define _03_ELC_EVENT_LINK_TAU01    (0x03U) /* link destination TAU01 */
#define _04_ELC_EVENT_LINK_TAU05    (0x04U) /* link destination TAU05 */
#define _05_ELC_EVENT_LINK_TAU07    (0x05U) /* link destination TAU07 */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define ELC_TRIGGER_SRC_COUNT           (22U)
#define ELC_TRIGGER_DEST_COUNT          (5U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum tagElcSrc
{
    ELC_TRIGGER_SRC_INTP0 = 0,      /* External interrupt edge detection 0 */
    ELC_TRIGGER_SRC_INTP1,          /* External interrupt edge detection 1 */
    ELC_TRIGGER_SRC_INTP2,          /* External interrupt edge detection 2 */
    ELC_TRIGGER_SRC_INTP3,          /* External interrupt edge detection 3 */
    ELC_TRIGGER_SRC_INTP4,          /* External interrupt edge detection 4 */
    ELC_TRIGGER_SRC_INTP5,          /* External interrupt edge detection 5 */
    ELC_TRIGGER_SRC_INTP6,          /* External interrupt edge detection 6 */
    ELC_TRIGGER_SRC_INTP7,          /* External interrupt edge detection 7 */
    ELC_TRIGGER_SRC_INTKR,          /* Key return signal detection */
    ELC_TRIGGER_SRC_INTIT,          /* 12-bit interval timer count completion */
    ELC_TRIGGER_SRC_INTIT00,        /* 8-bit interval timer channel 00 count completion */
    ELC_TRIGGER_SRC_INTIT10,        /* 8-bit interval timer channel 10 count completion */
    ELC_TRIGGER_SRC_INTRTCRPD,      /* RTC fixed periodic cycle signal */
    ELC_TRIGGER_SRC_INTTM00,        /* TAU0 channel 0 count end / capture end */
    ELC_TRIGGER_SRC_INTTM01,        /* TAU0 channel 1 count end / capture end */
    ELC_TRIGGER_SRC_INTTM02,        /* TAU0 channel 2 count end / capture end */
    ELC_TRIGGER_SRC_INTTM03,        /* TAU0 channel 3 count end / capture end */
    ELC_TRIGGER_SRC_INTTM05,        /* TAU0 channel 5 count end / capture end */
    ELC_TRIGGER_SRC_INTTM07,        /* TAU0 channel 7 count end / capture end */
    ELC_TRIGGER_SRC_INTDSAD,        /* Delta-sigma analog to digital translation end */
    ELC_TRIGGER_SRC_INTDSADZC0,     /* Zero crossing detection 0 */
    ELC_TRIGGER_SRC_INTDSADZC1,     /* Zero crossing detection 1 */
} elc_src_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_ELC_Create(void);
void R_ELC_Stop(uint32_t src);

/* Start user code for function. Do not edit comment generated here */
void R_ELC_Set(elc_src_t src, uint8_t dest);
void R_ELC_Clear(elc_src_t src);
/* End user code. Do not edit comment generated here */
#endif
