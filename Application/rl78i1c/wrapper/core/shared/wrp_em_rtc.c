/***********************************************************************************************************************
* File Name    : wrp_em_rtc.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper RTC APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "wrp_em_shared_config.h"   /* Wrapper Shared Configuration */
#include "wrp_em_rtc.h"             /* EM RTC Wrapper Layer */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* Internal use of module (do not change) */
#define     _BCD2DEC(x)     (x = (x & 0xF) + (x >> 4) * 10)     /* 1 byte BCD to DEC Conversion */

/***********************************************************************************************************************
Macro Definitions Checking
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name    : void EM_RTC_Init(void)
* Description      : EM RTC Initialization
* Arguments        : None
* Functions Called : EM_RTC_DriverInit()
* Return Value     : None
***********************************************************************************************************************/
void EM_RTC_Init(void)
{
    EM_RTC_DriverInit();
}

/***********************************************************************************************************************
* Function Name    : void EM_RTC_Start(void)
* Description      : EM RTC Start
* Arguments        : None
* Functions Called : EM_RTC_DriverStart()
* Return Value     : None
***********************************************************************************************************************/
void EM_RTC_Start(void)
{
    EM_RTC_DriverStart();
}

/***********************************************************************************************************************
* Function Name    : void EM_RTC_Stop(void)
* Description      : EM RTC Stop
* Arguments        : None
* Functions Called : EM_RTC_DriverStop()
* Return Value     : None
***********************************************************************************************************************/
void EM_RTC_Stop(void)
{
    EM_RTC_DriverStop();
}

/***********************************************************************************************************************
* Function Name    : void EM_RTC_GetDateTime(EM_RTC_DATE_TIME *datetime)
* Description      : Get RTC Date time value. The format of [datetime] element value is in
*                  : decimal number format.
* Arguments        : EM_RTC_DATE_TIME* datetime
*                  :    Pointer to a EM_RTC_DATE_TIME struct to get the date time
*                  :    value. All elements values is in decimal format
* Functions Called : EM_RTC_DriverCounterGet()
* Return Value     : None
***********************************************************************************************************************/
void EM_RTC_GetDateTime(EM_RTC_DATE_TIME *datetime)
{
    rtc_calendarcounter_value_t rtctime;
        
    /* Checking input argument */
    if (datetime == NULL)
    {
        return;
    }
    
    /* If valid, get the RTC counter */
    EM_RTC_DriverCounterGet(&rtctime);
    
    datetime->Sec = rtctime.rseccnt;
    datetime->Min = rtctime.rmincnt;
    datetime->Hour = rtctime.rhrcnt; 
    datetime->Day = rtctime.rdaycnt; 
    datetime->Week = rtctime.rwkcnt;
    datetime->Month = rtctime.rmoncnt;
    datetime->Year = (uint8_t)rtctime.ryrcnt;
    
    /* Convert all to decimal */
    _BCD2DEC(datetime->Sec);  
    _BCD2DEC(datetime->Min);  
    _BCD2DEC(datetime->Hour); 
    _BCD2DEC(datetime->Day);  
    _BCD2DEC(datetime->Week); 
    _BCD2DEC(datetime->Month);
    _BCD2DEC(datetime->Year);
}