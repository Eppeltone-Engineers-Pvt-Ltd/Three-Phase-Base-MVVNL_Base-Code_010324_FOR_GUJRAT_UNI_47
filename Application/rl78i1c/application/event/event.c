/***********************************************************************************************************************
* File Name    : event.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Event processing source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */

/* Wrapper/User */

/* MW/Core */
#include "em_core.h"        /* EM Core APIs */

/* MW/Core/PLugin */
#include "em_anti_tamper_plugin.h"  /* EM Core Anti-tamper plugin APIs */
#include "em_event_plugin.h"        /* EM Core Event plugin APIs */

/* Application */
#include "platform.h"
#include "storage.h"        /* Storage Header File */
#include "event.h"          /* Event Header File */
#include "powermgmt.h"        /* Power Management Header File */
#include <string.h>         /* String Standard Lib */

/* Debug */
#include "debug.h"          /* Debug */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Event flag */
typedef union tagEventFlag
{
    uint32_t value;
    
    struct tagEventFlagBit
    {
        uint8_t  is_max_demand_end          :1; /* Max demand */
        uint8_t  is_case_open               :1; /* Case open */
        uint8_t  is_magnetic                :1; /* Magnetic */
        uint8_t  is_power_fail              :1; /* Power Fail */
        uint8_t  is_neutral_missing         :1; /* Neutral Missing */

        uint8_t  is_missing_potential_R     :1; /* Missing Potential */
        uint8_t  is_missing_potential_Y     :1; /* Missing Potential */
        uint8_t  is_missing_potential_B     :1; /* Missing Potential */
        uint8_t  is_over_voltage_R          :1; /* Over Voltage */
        uint8_t  is_over_voltage_Y          :1; /* Over Voltage */
        uint8_t  is_over_voltage_B          :1; /* Over Voltage */
        uint8_t  is_low_voltage_R           :1; /* Low Voltage */
        uint8_t  is_low_voltage_Y           :1; /* Low Voltage */
        uint8_t  is_low_voltage_B           :1; /* Low Voltage */
        uint8_t  is_voltage_unbalance       :1; /* Voltage Unbalance */
        
        uint8_t  is_reverse_current_R       :1; /* Reverse Current */
        uint8_t  is_reverse_current_Y       :1; /* Reverse Current */
        uint8_t  is_reverse_current_B       :1; /* Reverse Current */
        uint8_t  is_open_current_R          :1; /* Open Current */
        uint8_t  is_open_current_Y          :1; /* Open Current */
        uint8_t  is_open_current_B          :1; /* Open Current */
        uint8_t  is_over_current_R          :1; /* Over Current */
        uint8_t  is_over_current_Y          :1; /* Over Current */
        uint8_t  is_over_current_B          :1; /* Over Current */
        uint8_t  is_current_unbalance       :1; /* Current Unbalance */
        uint8_t  is_current_bypass          :1; /* Current Bypass */

        uint8_t  is_battery_supplied        :1; /* Battery supplied */

        uint8_t  reserved                   :5; /* reserved */
    } details;
} EVENT_FLAG;

/* Event state */
typedef struct tagEventState
{
    EM_EXT_CALLBACK_STATE max_demand_end;               /* Max demand */
    EM_EXT_CALLBACK_STATE case_open ;                   /* Case open */
    EM_EXT_CALLBACK_STATE magnet;                       /* Magnetic */
    EM_EXT_CALLBACK_STATE power_failed;                 /* Power Fail */
    EM_EXT_CALLBACK_STATE neutral_missing;              /* Neutral Missing */
                                                        
    EM_EXT_CALLBACK_STATE missing_potential_R;          /* Missing Potential */
    EM_EXT_CALLBACK_STATE missing_potential_Y;          /* Missing Potential */
    EM_EXT_CALLBACK_STATE missing_potential_B;          /* Missing Potential */
    EM_EXT_CALLBACK_STATE over_voltage_R;               /* Over Voltage */
    EM_EXT_CALLBACK_STATE over_voltage_Y;               /* Over Voltage */
    EM_EXT_CALLBACK_STATE over_voltage_B;               /* Over Voltage */
    EM_EXT_CALLBACK_STATE low_voltage_R;                /* Low Voltage */
    EM_EXT_CALLBACK_STATE low_voltage_Y;                /* Low Voltage */
    EM_EXT_CALLBACK_STATE low_voltage_B;                /* Low Voltage */
    EM_EXT_CALLBACK_STATE voltage_unbalance;            /* Voltage Unbalance */
                                                        
    EM_EXT_CALLBACK_STATE reverse_current_R;            /* Reverse Current */
    EM_EXT_CALLBACK_STATE reverse_current_Y;            /* Reverse Current */
    EM_EXT_CALLBACK_STATE reverse_current_B;            /* Reverse Current */
    EM_EXT_CALLBACK_STATE open_current_R;               /* Open Current */
    EM_EXT_CALLBACK_STATE open_current_Y;               /* Open Current */
    EM_EXT_CALLBACK_STATE open_current_B;               /* Open Current */
    EM_EXT_CALLBACK_STATE over_current_R;               /* Over Current */
    EM_EXT_CALLBACK_STATE over_current_Y;               /* Over Current */
    EM_EXT_CALLBACK_STATE over_current_B;               /* Over Current */
    EM_EXT_CALLBACK_STATE current_unbalance;            /* Current Unbalance */
    EM_EXT_CALLBACK_STATE current_bypass;               /* Current Bypass */
} EVENT_STATE;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
/* Variables */
//static 
EVENT_STATE              g_event_state   =
{
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Max demand */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Case open */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Magnetic */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Power Fail */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Neutral Missing */
                                    
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Missing Potential */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Missing Potential */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Missing Potential */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Over Voltage */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Over Voltage */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Over Voltage */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Low Voltage */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Low Voltage */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Low Voltage */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Voltage Unbalance */
                                    
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Reverse Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Reverse Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Reverse Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Open Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Open Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Open Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Over Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Over Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Over Current */
    EM_EXT_CALLBACK_STATE_RELEASE,  /* Current Unbalance */
    EM_EXT_CALLBACK_STATE_RELEASE   /* Current Bypass */
};

//static 
EVENT_FLAG                       g_event_flag = 
{
    { 0 },
};

/* all is 0 */
//static 
rtc_calendarcounter_value_t      g_last_rtc_time = {0, 0, 0, 0, 0, 0, 0};

/* Sub-function for this module */
static uint8_t EVENT_BackupSystem(void);
static uint8_t EVENT_AddTamperLog(void);
static uint8_t EVENT_AddEnergyLog(rtc_calendarcounter_value_t *rtctime);

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name : EVENT_IsCaseOpen
* Interface     : uint8_t EVENT_IsCaseOpen(void)
* Description   : Polling check case open
* Arguments     : None
* Function Calls: None
* Return Value  : uint8_t, 0 is not happen, 1 is happen
***********************************************************************************************************************/
uint8_t EVENT_IsCaseOpen(void)
{
    return (g_event_state.case_open == EM_EXT_CALLBACK_STATE_ENTER);
}

uint8_t EVENT_IsPowerFail(void)
{
    return (g_event_state.power_failed == EM_EXT_CALLBACK_STATE_ENTER);
}

uint8_t EVENT_IsNeutralMissing(void)
{
    return (g_event_state.neutral_missing == EM_EXT_CALLBACK_STATE_ENTER);
}

uint8_t EVENT_IsMagnet(void)
{
    return (g_event_state.magnet == EM_EXT_CALLBACK_STATE_ENTER);
}

void EVENT_MaxDemandEnd(uint8_t bchange)
{
//    g_event_flag.details.is_max_demand_end = 1;
}

void EVENT_CaseOpen(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_case_open = 1;
//    g_event_state.case_open   = state;
}

void EVENT_Magnetic(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_magnetic = 1;
//    g_event_state.magnet     = state;
}

void EVENT_PowerFail(EM_EXT_CALLBACK_STATE state)
{// uncommented on 25/10/2019 
    g_event_flag.details.is_power_fail = 1;
    g_event_state.power_failed = state;
}

void EVENT_NeutralMissing(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_neutral_missing = 1;
//    g_event_state.neutral_missing   = state;

//#ifdef POWER_MANAGEMENT_ENABLE    
//    POWERMGMT_NeutralMissing(state);
//#endif
}

void EVENT_MissingPotential_R(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_missing_potential_R = 1;
//    g_event_state.missing_potential_R   = state;
}

void EVENT_MissingPotential_Y(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_missing_potential_Y = 1;
//    g_event_state.missing_potential_Y   = state;
}

void EVENT_MissingPotential_B(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_missing_potential_B = 1;
//    g_event_state.missing_potential_B   = state;
}

void EVENT_OverVoltage_R(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_over_voltage_R = 1;
//    g_event_state.over_voltage_R   = state;
}

void EVENT_OverVoltage_Y(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_over_voltage_Y = 1;
//    g_event_state.over_voltage_Y   = state;
}

void EVENT_OverVoltage_B(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_over_voltage_B = 1;
//    g_event_state.over_voltage_B   = state;
}

void EVENT_LowVoltage_R(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_low_voltage_R = 1;
//    g_event_state.low_voltage_R   = state;
}

void EVENT_LowVoltage_Y(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_low_voltage_Y = 1;
//    g_event_state.low_voltage_Y   = state;
}

void EVENT_LowVoltage_B(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_low_voltage_B = 1;
//    g_event_state.low_voltage_B   = state;
}

void EVENT_VoltageUnbalance(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_voltage_unbalance = 1;
//    g_event_state.voltage_unbalance   = state;
}

void EVENT_ReverseCurrent_R(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_reverse_current_R = 1;
//    g_event_state.reverse_current_R   = state;
}

void EVENT_ReverseCurrent_Y(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_reverse_current_Y = 1;
//    g_event_state.reverse_current_Y   = state;
}

void EVENT_ReverseCurrent_B(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_reverse_current_B = 1;
//    g_event_state.reverse_current_B   = state;
}

void EVENT_OpenCurrent_R(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_open_current_R = 1;
//    g_event_state.open_current_R   = state;
}

void EVENT_OpenCurrent_Y(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_open_current_Y = 1;
//    g_event_state.open_current_Y   = state;
}

void EVENT_OpenCurrent_B(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_open_current_B = 1;
//    g_event_state.open_current_B   = state;
}

void EVENT_OverCurrent_R(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_over_current_R = 1;
//    g_event_state.over_current_R   = state;
}

void EVENT_OverCurrent_Y(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_over_current_Y = 1;
//    g_event_state.over_current_Y   = state;
}

void EVENT_OverCurrent_B(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_over_current_B = 1;
//    g_event_state.over_current_B   = state;
}

void EVENT_CurrentUnbalance(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_current_unbalance = 1;
//    g_event_state.current_unbalance   = state;
}

void EVENT_CurrentBypass(EM_EXT_CALLBACK_STATE state)
{
//    g_event_flag.details.is_current_bypass = 1;
//    g_event_state.current_bypass   = state;
}

void EVENT_BatterySupplied(void)
{
    g_event_flag.details.is_battery_supplied = 1;
}

static uint8_t EVENT_BackupSystem(void)
{
    return (STORAGE_Backup(STORAGE_ITEM_SYS_STATE));
}

static uint8_t EVENT_AddTamperLog(void)
{
    ONE_TAMPER_DATA_LOG         tamper_log;     /* Tamper log data */
    
    /* Get the tamper log */
    STORAGE_GetTamperLog(&tamper_log);	// this function does nothing
    
    /* Write to EEPROM */
    return (STORAGE_AddTamperLog(&tamper_log));
}

/***********************************************************************************************************************
* Function Name    : static uint8_t EVENT_AddEnergyLog(rtc_counter_value_t *rtctime)
* Description      : Event add energy log to EEPROM, reset max demand
* Arguments        : struct RTCCounterValue *rtctime: RTC Time of monthly log
* Functions Called : EM_GetActiveMaxDemand()
*                  : EM_GetApparentMaxDemand()
*                  : EM_ResetMaxDemand()
*                  : EM_GetActiveEnergyTotal()
*                  : EM_GetApparentEnergyTotal()
*                  : memset(), standard library
*                  : EM_GetConfig()
*                  : EM_GetActiveEnergyTariff()
*                  : EM_GetApparentEnergyTariff()
*                  : STORAGE_AddEnergyLog()
* Return Value     : STORAGE status (STORAGE_OK, STORAGE_ERROR)
***********************************************************************************************************************/
static uint8_t EVENT_AddEnergyLog(rtc_calendarcounter_value_t *rtctime)
{
    ONE_MONTH_ENERGY_DATA_LOG   energy_log;
    
    /* Get the energy log */
    STORAGE_GetEnergyLog(&energy_log);
    
    /* Add to EEPROM */
    return (STORAGE_AddEnergyLog(&energy_log));
}

/***********************************************************************************************************************
* Function Name : EVENT_IsAllEventProcessed
* Interface     : uint8_t EVENT_IsAllEventProcessed(void)
* Description   : Check whether all happened events are processed or not.
* Arguments     : None
* Function Calls: None
* Return Value  : uint8_t
***********************************************************************************************************************/
uint8_t EVENT_IsAllEventProcessed(void)
{
    return (
        (g_event_flag.value | 0x00000000) == 0x00000000
    );
    //return 1;
}

/***********************************************************************************************************************
* Function Name    : void EVENT_PollingProcessing(void)
* Description      : Event Polling Processing
* Arguments        : None
* Functions Called : EVENT_BackupSystemAndRTC()
*                  : EVENT_AddTamperLog()
*                  : //DEBUG_Printf()
*                  : DEBUG_AckNewLine()
*                  : RTC_CounterGet()
*                  : EVENT_AddEnergyLog()
*                  : STORAGE_AddEnergyLog()
* Return Value     : None
***********************************************************************************************************************/
void EVENT_PollingProcessing(void)
{
    /* For RTC */
    uint16_t                    timeout;
    uint8_t                     status;
    rtc_calendarcounter_value_t rtctime;
    
    /* When power failed or LVD occurred
     * start to backup System State & RTC Time to EEPROM */
  if (g_event_flag.details.is_power_fail  == 1)
	{// Backup system data
//		EVENT_BackupSystem();
//    EVENT_AddTamperLog();
    g_event_flag.details.is_power_fail  = 0;
	}
    
  if (g_event_flag.details.is_max_demand_end == 1)
  {
//		EVENT_BackupSystem();
    g_event_flag.details.is_max_demand_end = 0;
  }
	
  if (g_event_flag.details.is_case_open == 1)
  {// Write tamper log to EEPROM 
//		EVENT_AddTamperLog();
    g_event_flag.details.is_case_open = 0;
  }
    
  if (g_event_flag.details.is_magnetic == 1)
  {// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_magnetic = 0;
  }
	
  if (g_event_flag.details.is_neutral_missing == 1)
	{// Write tamper log to EEPROM
//			EVENT_AddTamperLog();
      g_event_flag.details.is_neutral_missing = 0;
  }
	
  if (g_event_flag.details.is_missing_potential_R == 1)
	{// Write tamper log to EEPROM
	//	EVENT_AddTamperLog();
    g_event_flag.details.is_missing_potential_R = 0;
  }
	
	if (g_event_flag.details.is_missing_potential_Y == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_missing_potential_Y = 0;
	}
	
	if (g_event_flag.details.is_missing_potential_B == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_missing_potential_B = 0;
	}
	
	if (g_event_flag.details.is_voltage_unbalance == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_voltage_unbalance = 0;
	}

	if (g_event_flag.details.is_reverse_current_R == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_reverse_current_R = 0;
	}
	
	if (g_event_flag.details.is_reverse_current_Y == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_reverse_current_Y = 0;
	}
	
	if (g_event_flag.details.is_reverse_current_B == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_reverse_current_B = 0;
	}
	
	if (g_event_flag.details.is_open_current_R == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_open_current_R = 0;
	}

	if (g_event_flag.details.is_open_current_Y == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_open_current_Y = 0;
	}

	if (g_event_flag.details.is_open_current_B == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_open_current_B = 0;
	}
	
	if (g_event_flag.details.is_current_unbalance == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_current_unbalance = 0;
	}

	if (g_event_flag.details.is_current_bypass == 1)
	{// Write tamper log to EEPROM
//		EVENT_AddTamperLog();
    g_event_flag.details.is_current_bypass = 0;
	}

	if (g_event_flag.details.is_battery_supplied == 1)
	{// Battery is supplied by pressing button - do nothing
 //   g_event_flag.details.is_battery_supplied = 0;
	}

	// Checking for month change to write monthly data log
	R_RTC_Get_CalendarCounterValue(&rtctime);
	status = MD_OK;

	// Get RTC time successful?
	if (status == MD_OK){
    if (g_last_rtc_time.rmoncnt != 0){
      if (g_last_rtc_time.rmoncnt != rtctime.rmoncnt){// Add monthly energy to EEPROM 
				EVENT_AddEnergyLog(&g_last_rtc_time);	// this function does nothing
      }
    }

    // Update RTC time to g_last_rtc_time
    g_last_rtc_time.rseccnt = rtctime.rseccnt;      
    g_last_rtc_time.rmincnt = rtctime.rmincnt;      
    g_last_rtc_time.rhrcnt  = rtctime.rhrcnt;       
    g_last_rtc_time.rdaycnt = rtctime.rdaycnt;      
    g_last_rtc_time.rwkcnt  = rtctime.rwkcnt;       
    g_last_rtc_time.rmoncnt = rtctime.rmoncnt;      
    g_last_rtc_time.ryrcnt  = rtctime.ryrcnt;
	}
	
}

/***********************************************************************************************************************
* Function Name    : uint8_t EVENT_RegisterAll(void)
* Description      : Event Register All
* Arguments        : None
* Functions Called : None
* Return Value     : Execution status
*                  : EVENT_OK       Success
*                  : EVENT_ERROR    Register event failed
***********************************************************************************************************************/
uint8_t EVENT_RegisterAll(void)
{
    uint8_t status = EM_EXT_OK;

    /* Register Tamper Plugin */
    status = EM_EXT_TAMPER_RegisterPlugin(
/*        EVENT_NeutralMissing,
        EVENT_MissingPotential_R,
        EVENT_MissingPotential_Y,
        EVENT_MissingPotential_B,
        EVENT_VoltageUnbalance,
        EVENT_ReverseCurrent_R,
        EVENT_ReverseCurrent_Y,
        EVENT_ReverseCurrent_B,
        EVENT_OpenCurrent_R,
        EVENT_OpenCurrent_Y,
        EVENT_OpenCurrent_B,
        EVENT_CurrentUnbalance,
*/
				EVENT_CurrentBypass
    );
    
    if (status != EM_EXT_OK)
    {
        return EVENT_ERROR; /* Error occurred */
    }

    /* Register Event Plugin */
    status = EM_EXT_EVENT_RegisterPlugin(
//                                            NULL,                   /* Active Noload Callback R */
//                                            NULL,                   /* Active Noload Callback Y */
//                                            NULL,                   /* Active Noload Callback B */
//                                            NULL,                   /* Reactive Noload Callback R */
//                                            NULL,                   /* Reactive Noload Callback Y */
//                                            NULL,                   /* Reactive Noload Callback B */
//                                            NULL,                   /* Apparent Noload Callback R */
//                                            NULL,                   /* Apparent Noload Callback Y */
//                                            NULL,                   /* Apparent Noload Callback B */
//                                            NULL,                   /* Peak Voltage Callback R */
//                                            NULL,                   /* Peak Voltage Callback Y */
//                                            NULL,                   /* Peak Voltage Callback B */
//                                            NULL,                   /* Peak Current Callback R */
//                                            NULL,                   /* Peak Current Callback Y */
//                                            NULL,                   /* Peak Current Callback B */
//                                            NULL,                   /* Frequency Out Of Range Callback R */
//                                            NULL,                   /* Frequency Out Of Range Callback Y */
//                                            NULL,                   /* Frequency Out Of Range Callback B */
//                                            NULL,                   /* SAG (Low) voltage Callback R */
//                                            NULL,                   /* SAG (Low) voltage Callback Y */
//                                            NULL,                   /* SAG (Low) voltage Callback B */
                                            EVENT_MaxDemandEnd//,     /* Max Demand Update End Callback */
//                                            NULL,                   /* Tariff Changed Callback */
//                                            NULL                    /* Energy Update End Callback */
                                        );

    if (status != EM_EXT_OK)
    {
        return EVENT_ERROR; /* Error occurred */
    }

    /* Success all */
    return EVENT_OK;
}
