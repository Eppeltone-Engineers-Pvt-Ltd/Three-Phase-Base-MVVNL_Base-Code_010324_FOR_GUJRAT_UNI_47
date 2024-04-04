/***********************************************************************************************************************
* File Name    : r_cg_dtc.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for DTC module.
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
#include "r_cg_dtc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* 
 * Memory section changed by linker options 
*/
/* TAU01 Data */
#pragma section const T01_Trigger
const uint8_t __near g_dtc_tau01_trigger = 0x02;
#pragma section const

/* ADC Data */
#pragma section bss ADC_Data
volatile uint16_t __near g_adc_data[8];
/* End user code. Do not edit comment generated here */

/* DTC Vector and control data */
#pragma section bss DTC_Vect
volatile uint8_t __near dtc_vectortable[40U];

#pragma section bss DTCD0
volatile st_dtc_data __near dtc_controldata_0;

#pragma section bss DTCD1
volatile st_dtc_data __near dtc_controldata_1;

#pragma section bss

/***********************************************************************************************************************
* Function Name: R_DTC_Create
* Description  : This function initializes the DTC module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTC_Create(void)
{
    /* Enable input clock supply */
    DTCEN = 1U;
    /* Disable all DTC channels operation */
    DTCEN0 = 0x00U;
    DTCEN1 = 0x00U;
    DTCEN2 = 0x00U;
    DTCEN3 = 0x00U;
    DTCEN4 = 0x00U;
    /* Set base address */
    DTCBAR = 0xFDU;

    /* Set DTCD0 */
    dtc_vectortable[10U] = 0x40U;
    dtc_controldata_0.dtccr = _01_DTC_TRANSFER_MODE_REPEAT | _00_DTC_DATA_SIZE_8BITS | _02_DTC_REPEAT_AREA_SOURCE | 
                              _00_DTC_DEST_ADDR_FIXED | _20_DTC_REPEAT_INT_ENABLE | _00_DTC_CHAIN_TRANSFER_DISABLE;
    dtc_controldata_0.dtbls = _01_DTCD0_TRANSFER_BLOCKSIZE;
    dtc_controldata_0.dtcct = _01_DTCD0_TRANSFER_BYTE;
    dtc_controldata_0.dtrld = _01_DTCD0_TRANSFER_BYTE;

    dtc_controldata_0.dtsar = (uint16_t)&g_dtc_tau01_trigger;
    dtc_controldata_0.dtdar = (uint16_t)&TS0L;

    /* Set DTCD1 */
    dtc_vectortable[11U] = 0x48U;
    dtc_controldata_1.dtccr = _01_DTC_TRANSFER_MODE_REPEAT | _40_DTC_DATA_SIZE_16BITS | _00_DTC_SOURCE_ADDR_FIXED | 
                              _00_DTC_REPEAT_AREA_DEST | _00_DTC_REPEAT_INT_DISABLE |  _00_DTC_CHAIN_TRANSFER_DISABLE;
    dtc_controldata_1.dtbls = _01_DTCD1_TRANSFER_BLOCKSIZE;
    dtc_controldata_1.dtcct = _08_DTCD1_TRANSFER_BYTE;
    dtc_controldata_1.dtrld = _08_DTCD1_TRANSFER_BYTE;

    dtc_controldata_1.dtsar = _FF1E_DTCD0_SRC_ADDRESS;
    dtc_controldata_1.dtdar = (uint16_t)&g_adc_data;
}

/***********************************************************************************************************************
* Function Name: R_DTCD0_Start
* Description  : This function enables DTCD0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTCD0_Start(void)
{
    DTCEN1.5 = 1;
}

/***********************************************************************************************************************
* Function Name: R_DTCD0_Stop
* Description  : This function disables DTCD0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTCD0_Stop(void)
{
    DTCEN1.5 = 0;
    dtc_controldata_0.dtccr = _01_DTC_TRANSFER_MODE_REPEAT | _00_DTC_DATA_SIZE_8BITS | _02_DTC_REPEAT_AREA_SOURCE | 
                              _00_DTC_DEST_ADDR_FIXED | _20_DTC_REPEAT_INT_ENABLE | _00_DTC_CHAIN_TRANSFER_DISABLE;
    dtc_controldata_0.dtbls = _01_DTCD0_TRANSFER_BLOCKSIZE;
    dtc_controldata_0.dtcct = _01_DTCD0_TRANSFER_BYTE;
    dtc_controldata_0.dtrld = _01_DTCD0_TRANSFER_BYTE;

    dtc_controldata_0.dtsar = (uint16_t)&g_dtc_tau01_trigger;
    dtc_controldata_0.dtdar = (uint16_t)&TS0L;
}


/***********************************************************************************************************************
* Function Name: R_DTCD1_Start
* Description  : This function enables DTCD1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTCD1_Start(void)
{
    DTCEN1.4 = 1;
}

/***********************************************************************************************************************
* Function Name: R_DTCD1_Stop
* Description  : This function disables DTCD1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DTCD1_Stop(void)
{
    DTCEN1.4 = 0;
    dtc_controldata_1.dtccr = _01_DTC_TRANSFER_MODE_REPEAT | _40_DTC_DATA_SIZE_16BITS | _00_DTC_SOURCE_ADDR_FIXED | 
                              _00_DTC_REPEAT_AREA_DEST | _00_DTC_REPEAT_INT_DISABLE |  _00_DTC_CHAIN_TRANSFER_DISABLE;
    dtc_controldata_1.dtbls = _01_DTCD1_TRANSFER_BLOCKSIZE;
    dtc_controldata_1.dtcct = _08_DTCD1_TRANSFER_BYTE;
    dtc_controldata_1.dtrld = _08_DTCD1_TRANSFER_BYTE;

    dtc_controldata_1.dtsar = _FF1E_DTCD0_SRC_ADDRESS;
    dtc_controldata_1.dtdar = (uint16_t)&g_adc_data;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
