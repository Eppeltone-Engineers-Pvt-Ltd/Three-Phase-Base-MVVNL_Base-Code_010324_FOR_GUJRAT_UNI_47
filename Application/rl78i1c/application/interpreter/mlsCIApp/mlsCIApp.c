/****************************************************************************** 
* File Name    : mlsCIApp.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 27.12.2012 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"       /* Macro Driver Definitions */
#include "r_cg_tau.h"               /* CG Timer definitions */

/* Application */
#include "mlsInclude.h"
#include "mlsCIFConfig.h"
#include "mlsCIEng.h"
#include "mlsCIComm.h"
#include "mlsCIHandler.h"

#include "mlsCIApp.h"
#include "service.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
const mlsCICommConf_t g_conf =
{
#if  (MLSCIF_WINDOW_FLATFORM_ENABLE == 1)
    /* conf.fnSerialInit   = */ (serialInitFn)mlsCIComm_SerialInit    ,
    /* conf.fnSerialDeinit = */ (serialDeinitFn)mlsCIComm_SerialDeinit,
    /* conf.fnSerialRead   = */ (serialReadFn)mlsCIComm_SerialRead    ,
    /* conf.fnSerialWrite  = */ (serialWriteFn)mlsCIComm_SerialWrite  ,
    /* conf.fnSerialFlush  = */ (serialFlushFn)mlsCIComm_SerialFlush  ,
#else
    /* conf.fnSerialInit   = */ (serialInitFn)mlsCICommSerialInit     ,
    /* conf.fnSerialDeinit = */ (serialDeinitFn)mlsCICommSerialDeinit ,
    /* conf.fnSerialRead   = */ (serialReadFn)mlsCICommSerialRead     ,
    /* conf.fnSerialWrite  = */ (serialWriteFn)mlsCICommSerialWrite   ,
    /* conf.fnSerialFlush  = */ (serialFlushFn)mlsCICommSerialFlush   ,
#endif
};
    
/******************************************************************************
* Function Name : R_CI_Init
* Interface     : void R_CI_Init(void)
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void R_CI_Init(void)
{
    mlsCIEng_Init(&g_conf, Null);       // init CIEngine
}

/******************************************************************************
* Function Name : R_CI_DeInit
* Interface     : void R_CI_DeInit(void)
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void R_CI_DeInit(void)
{
    mlsCIEng_Deinit();                  // Deinit CIEngine
}

/******************************************************************************
* Function Name : R_CI_Start
* Interface     : void R_CI_Start(void)
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void R_CI_Start(void)
{
}

/******************************************************************************
* Function Name : R_CI_PollingProcessing
* Interface     : void R_CI_PollingProcessing(void)
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
mlsErrorCode_t R_CI_PollingProcessing(void)
{
    mlsErrorCode_t process_status = MLS_SUCCESS;

    process_status = mlsCIEng_ProcessStart();   // CIEngine process
    mlsCIEng_ProcessStop();
        
    return process_status;
}
