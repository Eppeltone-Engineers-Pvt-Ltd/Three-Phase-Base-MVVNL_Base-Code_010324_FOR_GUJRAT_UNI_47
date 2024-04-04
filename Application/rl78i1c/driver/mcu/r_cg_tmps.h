/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2012 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_tmps.h
* Version      : 
* Device(s)    : R5F10NMJ
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for Internal Temperature Sensor.
* Creation Date: 
***********************************************************************************************************************/

#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Peripheral enable register 0 (PER0)
*/
/* Control of temperature sensor operation input clock (ADCEN) */
#define _00_TMPS_CLOCK_STOP              (0x00U) /* stop supply of input clock */
#define _20_TMPS_CLOCK_SUPPLY            (0x20U) /* supply input clock */

/*
    Temperature sensor control test register (TMPCTL)
*/
/* Temperature sensor operation control (TMPEN) */
#define _80_TMPS_OPERATION_ENABLE        (0x80U) /* enable temperature sensor operation control */
#define _00_TMPS_OPERATION_DISABLE       (0x00U) /* disable temperature sensor operation control */
/* Temperature sensor operation selection (TMPSEL1, TMPSEL0) */
#define _00_TMPS_OPERATION_MODE_2        (0x00U) /* normal-temperature range (mode 2) */
#define _01_TMPS_OPERATION_MODE_3        (0x01U) /* low-temperature range (mode 3) */
#define _02_TMPS_OPERATION_MODE_1        (0x02U) /* high-temperature range (mode 1) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define TMPS_WAITTIME                    (100U)
#define TMPS_TMPCTLSET_WAITTIME          (30U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    TMPS_MODE2 = 0,
    TMPS_MODE3,
    TMPS_MODE1,
} tmps_mode_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_TMPS_Create(void);
void R_TMPS_Start(void);
void R_TMPS_Stop(void);

/* Start user code for function. Do not edit comment generated here */
MD_STATUS R_TMPS_SetMode(tmps_mode_t mode);
/* End user code. Do not edit comment generated here */
#endif
