/***********************************************************************************************************************
* File Name    : r_lvd.h
* Version      : 
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for LVD module.
* Creation Date: 5/7/2015
***********************************************************************************************************************/

#ifndef LVD_H
#define LVD_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Voltage detection register (LVIM)
*/
/* LVIS register rewrite enable/disable control (LVISEN) */
#define _00_LVD_REWRITE_LVIS_DISABLE    (0x00U) /* disabling rewriting */
#define _80_LVD_REWRITE_LVIS_ENABLE     (0x80U) /* enabling rewriting */
/* LVD output mask status flag (LVIOMSK) */
#define _00_LVD_MASK_INVALIDITY         (0x00U) /* mask is invalid */
#define _02_LVD_MASK_VALIDITY           (0x02U) /* mask is valid */
/* Voltage detection flag (LVIF) */
#define _00_LVD_FLAG_OVER               (0x00U) /* supply voltage (VDD) >= detection voltage (VLVI), or when LVD operation is disabled */
#define _01_LVD_FLAG_LESS               (0x01U) /* supply voltage (VDD) < detection voltage (VLVI) */

/*
    Voltage detection level register (LVIS)
*/
/* Operation mode of voltage detection (LVIMD) */
#define _00_LVD_MODE_INT                (0x00U) /* interrupt mode */
#define _80_LVD_MODE_RESET              (0x80U) /* reset mode */
/* LVD detection level (LVILV) */
#define _00_LVD_LEVEL_INT               (0x00U) /* high-voltage detection level (VLVIH) */
#define _01_LVD_LEVEL_RESET             (0x01U) /* low-voltage detection level (VLVIL) */

/*
    LVDVDD register (LVDVDD)        
*/

#define _00_VLVDVDD_LEVEL0              (0x00U) /* Rising Edge 2.54 V, Falling Edge 2.47 V */
#define _01_VLVDVDD_LEVEL1              (0x01U) /* Rising Edge 2.75 V, Falling Edge 2.68 V */
#define _02_VLVDVDD_LEVEL2              (0x02U) /* Rising Edge 2.95 V, Falling Edge 2.88 V */
#define _03_VLVDVDD_LEVEL3              (0x03U) /* Rising Edge 3.16 V, Falling Edge 3.09 V */
#define _04_VLVDVDD_LEVEL4              (0x04U) /* Rising Edge 3.47 V, Falling Edge 3.40 V */
#define _05_VLVDVDD_LEVEL5              (0x05U) /* Rising Edge 3.78 V, Falling Edge 3.71 V */

/*
    LVDVBAT register (LVDVBAT)      
*/

#define _00_VLVDVBAT_LEVEL0              (0x00U) /* Rising Edge 2.12 V, Falling Edge 2.06 V */
#define _01_VLVDVBAT_LEVEL1              (0x01U) /* Rising Edge 2.22 V, Falling Edge 2.16 V */
#define _02_VLVDVBAT_LEVEL2              (0x02U) /* Rising Edge 2.33 V, Falling Edge 2.27 V */
#define _03_VLVDVBAT_LEVEL3              (0x03U) /* Rising Edge 2.43 V, Falling Edge 2.37 V */
#define _04_VLVDVBAT_LEVEL4              (0x04U) /* Rising Edge 2.53 V, Falling Edge 2.47 V */
#define _05_VLVDVBAT_LEVEL5              (0x05U) /* Rising Edge 2.63 V, Falling Edge 2.57 V */
#define _06_VLVDVBAT_LEVEL6              (0x06U) /* Rising Edge 2.74 V, Falling Edge 2.68 V */

/*
    LVDVRTC register (LVDVRTC)      
*/

#define _00_VLVDVRTC_LEVEL0              (0x00U) /* Rising Edge 2.22 V, Falling Edge 2.16 V */
#define _01_VLVDVRTC_LEVEL1              (0x01U) /* Rising Edge 2.43 V, Falling Edge 2.37 V */
#define _02_VLVDVRTC_LEVEL2              (0x02U) /* Rising Edge 2.63 V, Falling Edge 2.57 V */
#define _03_VLVDVRTC_LEVEL3              (0x03U) /* Rising Edge 3.84 V, Falling Edge 2.78 V */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_LVD_Create(void);
void R_LVD_InterruptMode_Start(void);
void R_LVDVDD_Create(void);
void R_LVDVDD_Start(void);
void R_LVDVDD_Stop(void);
void R_LVDVBAT_Create(void);
void R_LVDVBAT_Start(void);
void R_LVDVBAT_Stop(void);
void R_LVDVRTC_Create(void);
void R_LVDVRTC_Start(void);
void R_LVDVRTC_Stop(void);
void R_LVDEXLVD_Create(void);
void R_LVDEXLVD_Start(void);
void R_LVDEXLVD_Stop(void);


/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
