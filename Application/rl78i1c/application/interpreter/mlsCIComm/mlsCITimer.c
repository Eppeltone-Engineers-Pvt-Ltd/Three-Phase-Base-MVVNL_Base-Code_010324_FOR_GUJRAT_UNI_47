/****************************************************************************** 
* File Name    : mlsCITimer.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 26.12.2012 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"       /* Macro Driver Definitions */
#include "r_cg_tau.h"               /* CG Timer definitions */

/* Application */
#include "mlsCITimer.h"
#include "mlsCIEng.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define SERIAL_DATA_RECEIVE_TIME    250

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
UInt32 mlsSystemTimeCount = 0;
UInt32 mlsReceiveTime = 0;
UInt32 mlsIsReceiveTimerStarted = 0;
extern UInt8 mlsFrameReceiveError;
extern mlsCIEngReceiveState_t ReceiveState;

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name : mlsSystemTimerInit
* Interface     : void mlsSystemTimerInit()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsSystemTimerInit(void)
{
    R_TAU0_Channel0_Start();
}

/******************************************************************************
* Function Name : mlsStartReceiveTimer
* Interface     : void mlsStartReceiveTimer()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsStartReceiveTimer(void)
{
    mlsReceiveTime = 0;
    mlsIsReceiveTimerStarted = 1;   /* mark as started */
}

/******************************************************************************
* Function Name : mlsStopAndResetReceiveTimer
* Interface     : void mlsStopAndResetReceiveTimer()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsStopAndResetReceiveTimer(void)
{
    mlsIsReceiveTimerStarted = 0;   /* mark as stopped */
    mlsReceiveTime = 0;
}

/******************************************************************************
* Function Name : mlsGetSystemTimeCount
* Interface     : UInt32 mlsGetSystemTimeCount()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : UInt32
******************************************************************************/
UInt32 mlsGetSystemTimeCount(void)
{
    return mlsSystemTimeCount;
}

/******************************************************************************
* Function Name : mlsResetSystemTimer
* Interface     : void mlsResetSystemTimer()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsResetSystemTimer(void)
{
    mlsSystemTimeCount = 0;
}

/******************************************************************************
* Function Name : mlsGetReceiveTime
* Interface     : UInt32 mlsGetReceiveTime()
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : UInt32
******************************************************************************/
UInt32 mlsGetReceiveTime(void)
{
    return mlsReceiveTime;
}

/******************************************************************************
* Function Name : mlsSystemTimerCallback
* Interface     : void mlsSystemTimerCallback(void)
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
******************************************************************************/
void mlsSystemTimerCallback(void)
{
    if (mlsIsReceiveTimerStarted == 1)
    {
        mlsReceiveTime ++;
        // if time receive is 100 ms and not receive finish -> frame error / serial timeout
        if((mlsReceiveTime > SERIAL_DATA_RECEIVE_TIME) && (mlsGetRequestStatus() != HAVE_REQUEST))
        {
            mlsFrameReceiveError = True;    // set flag frame error
            ReceiveState = INITIAL;         // assign receive state is INITIAL to receiving next request
            mlsIsReceiveTimerStarted = 0;   // stop timer0
        }
    }
    
    mlsSystemTimeCount++;
    if(mlsSystemTimeCount > TIMER_MAX_TICKCOUNT)
    {
        mlsSystemTimeCount = 0;
    }
}
