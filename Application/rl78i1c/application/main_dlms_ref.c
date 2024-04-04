
/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_main.c
* Version      : Code Generator for RL78/I1B V1.02.00.03 [07 Aug 2014]
* Device(s)    : R5F10MME
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 2/16/2017
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
#define CLEARBIT(PORT,bit)  (PORT &= ~(1<<bit))  //clear bit in port
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_rtc.h"
#include "r_cg_wdt.h"

/* Start user code for include. Do not edit comment generated here */


/*DLMS*/
#include "r_cg_serial.h";
#include "r_cg_timer.h"
#include "DLMS_User.h"

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */


/* End user code. Do not edit comment generated here */

void R_MAIN_UserInit(void);
void get_lp_data()
{
 
}
void get_dlp_data(){}
void get_bill_data(){}
void get_tamper_data(){}
void get_time_data(){}
void find_num_between_lp_entries(){}
void find_num_between_dlp_entries(){}
/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
	CLEARBIT(PM4,2);
	CLEARBIT(P4,2);	
	
    	R_MAIN_UserInit();    

    /* Start user code. Do not edit comment generated here */
    while (1U)
    {
	  DLMS_PollingProcessing();
      	R_WDT_Restart();
    }
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    	
	
R_SAU1_Create();
	R_UART2_Create();
//	R_UART2_Start();
//	R_TAU0_Create();  //RENESAS CODE, UART CREATE
//	R_TAU0_Channel0_Start();
//	TXD2='A';
	DLMS_Initialize();

    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */


/* End user code. Do not edit comment generated here */
