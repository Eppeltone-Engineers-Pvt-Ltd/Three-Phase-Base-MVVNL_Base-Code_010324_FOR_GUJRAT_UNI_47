/****************************************************************************** 
* File Name    : service_em_data.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */

/* Global */
#include "typedef.h"            /* GSCE Typedef */

/* Middleware */
#include "em_core.h"            /* EM Core Library */

/* Application */
#include "format.h"             /* EEPROM Format */
#include "eeprom.h"             /* EEPROM Driver */
#include "service.h"            /* Services */
#include "storage.h"            /* Storage */
#include "powermgmt.h"

/* Compiler specific */
#include <string.h>

/******************************************************************************
Macro definitions
******************************************************************************/
typedef struct tagServiceOneTamperDataLog
{
    /* Total: 10 bytes*/ 
    RTC_DATE_TIME   tamper_rtc_time;        /* Tamper RTC time */
    
    union
    {
        uint16_t    value;
        struct tagServiceTamperBitDef
        {
            uint8_t     is_neutral_missing          :1;     /* Anti-tamper. Neutral Missing      */
            uint8_t     is_missing_potential_R      :1;     /* Anti-tamper. Missing Potential    */
            uint8_t     is_missing_potential_Y      :1;     /* Anti-tamper. Missing Potential    */
            uint8_t     is_missing_potential_B      :1;     /* Anti-tamper. Missing Potential    */
            uint8_t     is_voltage_unbalance        :1;     /* Anti-tamper. Voltage Unbalance    */
            uint8_t     is_reverse_current_R        :1;     /* Anti-tamper. Reverse Current      */
            uint8_t     is_reverse_current_Y        :1;     /* Anti-tamper. Reverse Current      */
            uint8_t     is_reverse_current_B        :1;     /* Anti-tamper. Reverse Current      */
            uint8_t     is_open_current_R           :1;     /* Anti-tamper. Open Current         */
            uint8_t     is_open_current_Y           :1;     /* Anti-tamper. Open Current         */
            uint8_t     is_open_current_B           :1;     /* Anti-tamper. Open Current         */
            uint8_t     is_current_unbalance        :1;     /* Anti-tamper. Current Unbalance    */
            uint8_t     is_current_bypass           :1;     /* Anti-tamper. Current Bypass       */
            uint8_t     case_open                   :1;     /* Case Open Indicator Bit              */
            uint8_t     magnet                      :1;     /* Magnet Indicator Bit                 */
            uint8_t     power_failed                :1;     /* Power Fail Bit                       */
        } details;                                          /* Tamper Type */
    } tamper_type;
    
} SERVICE_ONE_TAMPER_DATA_LOG;

typedef struct tagServiceOneMonthEnergyDataLogPhase
{
    /* Total: 96 bytes */
    struct
    {   
        uint64_t    active;                                         /* Active */
        uint64_t    apparent;                                       /* Apparent */
        uint64_t    reactive;                                       /* Reactive */
    } present_max_demand;                                           /* Present max demand */

    struct
    {
        double64_t   active;                                         /* Active */
        double64_t   apparent;                                       /* Apparent */
        double64_t   reactive;                                       /* Reactive */
    } present_energy_total_tariff;                                  /* Present energy total tariff */

    struct
    {                   
        double64_t   active;                                         /* Active */
        double64_t   apparent;                                       /* Apparent */
        double64_t   reactive;                                       /* Reactive */
    } present_energy_tariff[6];                                     /* Present energy tariff (6 tariffs) */
    
    RTC_DATE_TIME    present_rtc_log;                               /* Present RTC monthly log */
    
} SERVICE_ONE_MONTH_ENERGY_DATA_LOG;

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

/******************************************************************************
* Function Name : SERVICE_ReadEMData
* Interface     : uint8_t SERVICE_ReadEMData(
*               :     uint8_t data_id,
*               :     uint16_t block_id,
*               :     uint8_t *buff,
*               :     uint8_t len
*               : );
* Description   : EM Read Service
* Arguments     : uint8_t data_id  : Data ID
*               : uint16_t block_id: Block ID (if any, for bulk transfer)
*               : uint8_t * buff   : buffer to be filled to
*               : uint8_t len      : Expected length to read
* Function Calls: TBD
* Return Value  : uint8_t, success or not
******************************************************************************/

service_result_t SERVICE_ReadEMData(st_service_param *p_params)
{
    // Successful 
    return SERVICE_OK;
}

service_result_t SERVICE_WriteEMData(st_service_param *p_params)
{
    // Successful  
    return SERVICE_OK;
}

