/***********************************************************************************************************************
* File Name    : r_cg_intp.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTP module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intp.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "emeter3_structs.h"
/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

void R_INTC_Create(void)
{
    PMK0 = 1U;    /* disable INTP0 operation */
    PIF0 = 0U;    /* clear INTP0 interrupt flag */
    PMK1 = 1U;    /* disable INTP1 operation */
    PIF1 = 0U;    /* clear INTP1 interrupt flag */
    PMK2 = 1U;    /* disable INTP2 operation */
    PIF2 = 0U;    /* clear INTP2 interrupt flag */
    PMK3 = 1U;    /* disable INTP3 operation */
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
    PMK4 = 1U;    /* disable INTP4 operation */
    PIF4 = 0U;    /* clear INTP4 interrupt flag */
    PMK5 = 1U;    /* disable INTP5 operation */
    PIF5 = 0U;    /* clear INTP5 interrupt flag */
    PMK6 = 1U;    /* disable INTP6 operation */
    PIF6 = 0U;    /* clear INTP6 interrupt flag */
    PMK7 = 1U;    /* disable INTP7 operation */
    PIF7 = 0U;    /* clear INTP7 interrupt flag */
    /* Set INTP0 low priority */
    PPR10 = 1U;
    PPR00 = 1U;
    /* Set INTP1 low priority */
    PPR11 = 1U;
    PPR01 = 1U;

    EGN0 = _01_INTP0_EDGE_FALLING_SEL | _02_INTP1_EDGE_FALLING_SEL |_04_INTP2_EDGE_FALLING_SEL |_20_INTP5_EDGE_FALLING_SEL;
/* changed on 02-Nov-2019 - INTP0 is rising edge now*/
//    EGN0 = _00_INTP0_EDGE_FALLING_UNSEL | _02_INTP1_EDGE_FALLING_SEL |_04_INTP2_EDGE_FALLING_SEL |_20_INTP5_EDGE_FALLING_SEL;
//    EGP0 = _01_INTP0_EDGE_RISING_SEL;
    
  /* Set INTP5 pin */
//    PM0 |= 0x04U;	// This is incorrect - not for our PCB
}

void R_INTC0_Start(void)
{
    PIF0 = 0U;    // clear INTP0 interrupt flag
    PMK0 = 0U;    // enable INTP0 interrupt 
}

void R_INTC0_Stop(void)
{
    PMK0 = 1U;    // disable INTP0 interrupt 
    PIF0 = 0U;    // clear INTP0 interrupt flag
}

void R_INTC1_Start(void)
{
    PIF1 = 0U;    // clear INTP1 interrupt flag 
    PMK1 = 0U;    // enable INTP1 interrupt 
}

void R_INTC1_Stop(void)
{
    PMK1 = 1U;    // disable INTP1 interrupt
    PIF1 = 0U;    // clear INTP1 interrupt flag
}

void R_INTC2_Start(void)
{
    PIF2 = 0U;    // clear INTP2 interrupt flag 
    PMK2 = 0U;    // enable INTP2 interrupt 
}

void R_INTC2_Stop(void)
{
    PMK2 = 1U;    // disable INTP2 interrupt
    PIF2 = 0U;    // clear INTP2 interrupt flag
}

void R_INTC5_Start(void)
{
    PIF5 = 0U;    // clear INTP1 interrupt flag 
    PMK5 = 0U;    // enable INTP1 interrupt 
}

void R_INTC5_Stop(void)
{
    PMK5 = 1U;    // disable INTP5 interrupt
    PIF5 = 0U;    // clear INTP5 interrupt flag
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
