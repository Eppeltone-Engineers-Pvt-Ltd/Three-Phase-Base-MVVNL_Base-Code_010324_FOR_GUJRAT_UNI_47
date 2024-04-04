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
* File Name    : r_cg_tmps.c
* Version      : 
* Device(s)    : R5F10NMJ
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for Internal Temperature Sensor module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tmps.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TMPS_Create
* Description  : This function initializes the temperature sensor.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TMPS_Create(void)
{
    volatile uint16_t w_count;

    ADCEN = 1U;  /* supply AD clock */
    TMPCTL = _00_TMPS_OPERATION_MODE_2;

    /* Change the waiting time according to the system */
    for (w_count = 0U; w_count <= TMPS_TMPCTLSET_WAITTIME; w_count++)
    {
        NOP();
    }
}
/***********************************************************************************************************************
* Function Name: R_TMPS_Start
* Description  : This function starts the temperature sensor operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TMPS_Start(void)
{
    volatile uint16_t w_count;

    TMPEN = 1U;  /* enable temperature sensor operation */

    /* Change the waiting time according to the system */
    for (w_count = 0U; w_count <= TMPS_WAITTIME; w_count++)
    {
        NOP();
    }
}
/***********************************************************************************************************************
* Function Name: R_TMPS_Stop
* Description  : This function stops the temperature sensor operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TMPS_Stop(void)
{
    TMPEN = 0U;  /* disable temperature sensor operation */
}

/* Start user code for adding. Do not edit comment generated here */
/***********************************************************************************************************************
* Function Name: uint8_t R_TMPS_SetMode(tmps_mode_t mode)
* Description  : Set the operation mode to internal temperature sensor
* Arguments    : mode : TMPS_MODE2      : normal range, [-20deg ; 70deg]
*              :        TMPS_MODE3      : low range   , [-40deg ; 50deg]
*              :        TMPS_MODE1      : high range  , [0deg   ; 90deg]
* Return Value : 0 : Set new mode successfully
*              : 1 : Sensor hasn't started when set new mode
***********************************************************************************************************************/
MD_STATUS R_TMPS_SetMode(tmps_mode_t mode)
{
    volatile uint16_t w_count;
    MD_STATUS   status = MD_OK;
    
    if (TMPEN == 1)                         /* Temperature sensor is started */
    {
        TMPCTL  &= 0xF0U;                   /* Reset operation mode */
        TMPCTL  |= mode;                    /* Set new operation mode */
        
        /* Change the waiting time according to the system */
        for (w_count = 0U; w_count <= TMPS_WAITTIME; w_count++)
        {
            NOP();
        }   
    }
    else    /* Temperature sensor is not started */
    {
        status = MD_ERROR;
    }
    return status;
}
/* End user code. Do not edit comment generated here */
