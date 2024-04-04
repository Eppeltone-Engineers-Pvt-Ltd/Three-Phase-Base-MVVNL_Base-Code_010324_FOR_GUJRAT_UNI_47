/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : em_accumulation.h
* Version      : 1.00
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.11.2012 
******************************************************************************/

#ifndef _EM_ACCUMULATION_H
#define _EM_ACCUMULATION_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"        /* GSCE Typedef */

/******************************************************************************
Macro definitions
******************************************************************************/
/* Direct memcopy from src to dest 
 * Workaround for CCL, if use dest=src, it will output one redundant instruction
 * Method: direct addressing will remove that redundant instruction                                                       
*/
#define EM_ACC24_DIRECT_SET(dest, src)                  {\
                                                            *(((uint16_t *)&(dest))) = *(((uint16_t *)&(src))); \
                                                            *(((uint16_t *)&(dest)) + 1) = *(((uint16_t *)&(src)) + 1); \
                                                            *(((uint16_t *)&(dest)) + 2) = *(((uint16_t *)&(src)) + 2); \
                                                            *(((uint16_t *)&(dest)) + 3) = *(((uint16_t *)&(src)) + 3); \
                                                        }
/* Direct clear acc24_t 
 * Workaround for CCL, if use acc =0, it will output one redundant instruction
 * Method: direct addressing will remove that redundant instruction                                                       
*/
#define EM_ACC24_DIRECT_CLEAR(acc)                      {\
                                                            *(((uint16_t *)&(acc))) = 0;          \
                                                            *(((uint16_t *)&(acc)) + 1) = 0;          \
                                                            *(((uint16_t *)&(acc)) + 2) = 0;          \
                                                            *(((uint16_t *)&(acc)) + 3) = 0;          \
                                                        }

#define EM_ACC24_DIRECT_MACS_MODE()                      {MULC = 0xC0;}
#define EM_ACC24_DIRECT_MULS_MODE()                      {MULC = 0x40;}

/* Direct 24bit multiply-accumulation */
#define EM_ACC24_DIRECT_ADD(p_acc, signal0, signal1)    {\
                                                            /* Configure the MACL, Signed MAC mode */   \
                                                            MULC = 0xC0;                                \
                                                            /* Init MULRx with p_acc */                 \
                                                            MULR0 = (*((uint16_t *)(p_acc)));           \
                                                            MULR1 = (*((uint16_t *)(p_acc) + 1));       \
                                                            MULR2 = (*((uint16_t *)(p_acc) + 2));       \
                                                            MULR3 = (*((uint16_t *)(p_acc) + 3));       \
                                                            /* Load signal0 */                          \
                                                            MAC32SL = (*((uint16_t *)&(signal0)));      \
                                                            MAC32SH = (*((uint16_t *)&(signal0) + 1));  \
                                                            /* Load signal1 */                          \
                                                            MULBL = (*((uint16_t *)&(signal1)));        \
                                                            MULBH = (*((uint16_t *)&(signal1) + 1));    \
                                                            /* Wait 2 more cycles */                    \
                                                            __nop();                                      \
                                                            __nop();                                      \
                                                            /* Read MULRx to p_acc */                   \
                                                            (*((uint16_t *)(p_acc)))     = MULR0;       \
                                                            (*((uint16_t *)(p_acc) + 1)) = MULR1;       \
                                                            (*((uint16_t *)(p_acc) + 2)) = MULR2;       \
                                                            (*((uint16_t *)(p_acc) + 3)) = MULR3;       \
                                                        }

/* Direct 32bit multiplication */
#define EM_DIRECT_MULS32(p_product, signal0, signal1)   {\
                                                            /* Configure the MACL, Signed MUL mode */   \
                                                            MULC = 0x40;                                \
                                                            /* Load signal0 */                          \
                                                            MUL32SL = (*((uint16_t *)&(signal0)));      \
                                                            MUL32SH = (*((uint16_t *)&(signal0) + 1));  \
                                                            /* Load signal1 */                          \
                                                            MULBL = (*((uint16_t *)&(signal1)));        \
                                                            MULBH = (*((uint16_t *)&(signal1) + 1));    \
                                                            /* Wait 2 more cycles */                    \
                                                            __nop();                                      \
                                                            __nop();                                      \
                                                            /* Read MULRx to p_product */               \
                                                            (*((uint16_t *)(p_product)))     = MULR0;   \
                                                            (*((uint16_t *)(p_product) + 1)) = MULR1;   \
                                                            (*((uint16_t *)(p_product) + 2)) = MULR2;   \
                                                            (*((uint16_t *)(p_product) + 3)) = MULR3;   \
                                                        }

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef int64_t acc24_t;

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
void EM_ACC24_Add(acc24_t __near *p_acc, int32_t signal0, int32_t signal1);
void EM_MULS32(int64_t __near *p_product, int32_t signal0, int32_t signal1);
float32_t EM_ACC24_GetFloatValue(acc24_t __near *p_acc);
void EM_ACC24_Clear(acc24_t __near * p_acc);
void EM_ACC24_AddThenClearSrc(acc24_t __near * p_dest, acc24_t __near * p_src);

#endif /* _EM_ACCUMULATION_H */

