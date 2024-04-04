/***********************************************************************************************************************
* File Name    : r_cg_macl.h
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for MACL module.
* Creation Date: 
***********************************************************************************************************************/
#ifndef MACL_H
#define MACL_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Multiplier-Accumulator Control Register (MULC)
*/
/* Control of MACL accumulator mode (MACMODE) */
#define _00_MACL_MUL_MODE                       (0x00U) /* Multiplier only mode */
#define _80_MACL_MAC_MODE                       (0x80U) /* Multiply-Accumulate mode */

/* Control of MACL sign mode (MACSM) */
#define _00_MACL_UNSIGNED_MODE                  (0x00U) /* Unsigned operation */
#define _40_MACL_SIGNED_MODE                    (0x40U) /* Signed operation */

/* Control of MACL fixed-point mode (MULFRAC) */
#define _00_MACL_FIXED_POINT_MODE               (0x00U) /* Fixed-point operation */
#define _10_MACL_FRACTION_MODE                  (0x10U) /* Fraction operation */

/* MAC operation overflow status (MACOF) */
#define _00_MACL_MAC_RESULT_NORMAL              (0x00U) /* MAC operation is normal*/
#define _04_MACL_MAC_RESULT_OVERFLOW            (0x04U) /* MAC operation is overflow */

/* MAC result sign (MACSF) */
#define _00_MACL_MAC_RESULT_POSITIVE            (0x00U) /* MAC result is positive */
#define _02_MACL_MAC_RESULT_NEGATIVE            (0x02U) /* MAC result is negative */

/* Multiplication operating status (MULST) */
#define _00_MACL_MUL_COMPLETE                   (0x00U) /* Multiplication is idle */
#define _01_MACL_MUL_OPERATING                  (0x01U) /* Multiplication is in progress */


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_MACL_Create(void);
//void R_MACL_Start(void);
void R_MACL_EnableInterrupt(void);
void R_MACL_DisableInterrupt(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif