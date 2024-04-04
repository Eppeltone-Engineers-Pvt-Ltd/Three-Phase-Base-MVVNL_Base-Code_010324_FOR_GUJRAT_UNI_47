/***********************************************************************************************************************
* File Name    : em_event_plugin.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Event Plugin Header
***********************************************************************************************************************/

#ifndef _EM_EVENT_PLUGIN_H
#define _EM_EVENT_PLUGIN_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        // GSCE Standard Typedef 
#include "em_adapter.h"      // Shared Plugin Definition 

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
// Event Plugin Callback API 
typedef void    (*EM_EXT_EVENT_CALLBACK_MDM_CYCEND)     (uint8_t);                              // Max demand cycle end callback prototype             
typedef void    (*EM_EXT_EVENT_CALLBACK_TARIFF_CHANGE)  (uint8_t);                              // Tariff changed callback prototype                   
typedef void    (*EM_EXT_EVENT_CALLBACK_ENERGY_CYCEND)  (void);                                 // Energy cyc 

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
// Event plugin registration 
uint8_t EM_EXT_EVENT_RegisterPlugin(/*
                                    EM_EXT_CALLBACK cb_active_noload_R,                    // Active Noload Callback 
                                    EM_EXT_CALLBACK cb_active_noload_Y,                    // Active Noload Callback 
                                    EM_EXT_CALLBACK cb_active_noload_B,                    // Active Noload Callback 
                                    EM_EXT_CALLBACK cb_reactive_noload_R,                  // Reactive Noload Callback 
                                    EM_EXT_CALLBACK cb_reactive_noload_Y,                  // Reactive Noload Callback 
                                    EM_EXT_CALLBACK cb_reactive_noload_B,                  // Reactive Noload Callback 
                                    EM_EXT_CALLBACK cb_apparent_noload_R,                  // Apparent Noload Callback 
                                    EM_EXT_CALLBACK cb_apparent_noload_Y,                  // Apparent Noload Callback 
                                    EM_EXT_CALLBACK cb_apparent_noload_B,                  // Apparent Noload Callback 
                                    EM_EXT_CALLBACK cb_peak_voltage_R,                     // Peak Voltage Callback 
                                    EM_EXT_CALLBACK cb_peak_voltage_Y,                     // Peak Voltage Callback 
                                    EM_EXT_CALLBACK cb_peak_voltage_B,                     // Peak Voltage Callback 
                                    EM_EXT_CALLBACK cb_peak_current_R,                     // Peak Current Callback 
                                    EM_EXT_CALLBACK cb_peak_current_Y,                     // Peak Current Callback 
                                    EM_EXT_CALLBACK cb_peak_current_B,                     // Peak Current Callback 
                                    EM_EXT_CALLBACK cb_freq_out_range_R,                   // Frequency Out Of Range Callback 
                                    EM_EXT_CALLBACK cb_freq_out_range_Y,                   // Frequency Out Of Range Callback 
                                    EM_EXT_CALLBACK cb_freq_out_range_B,                   // Frequency Out Of Range Callback 
                                    EM_EXT_CALLBACK cb_sag_voltage_R,                      // SAG (Low) voltage Callback 
                                    EM_EXT_CALLBACK cb_sag_voltage_Y,                      // SAG (Low) voltage Callback 
                                    EM_EXT_CALLBACK cb_sag_voltage_B,                      // SAG (Low) voltage Callback 
                                    */
																		EM_EXT_EVENT_CALLBACK_MDM_CYCEND cb_max_demand//,        // Max Demand Update End Callback 
                                    //EM_EXT_EVENT_CALLBACK_TARIFF_CHANGE cb_tariff_changed, // Tariff Changed Callback 
                                    //EM_EXT_EVENT_CALLBACK_ENERGY_CYCEND cb_energy_cycend   // Energy Update End Callback 
                                    );

#endif // _EM_EVENT_PLUGIN_H 
