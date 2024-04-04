/***********************************************************************************************************************
* File Name    : em_event_plugin.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Event Plugin
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "em_event_adapter.h"                   // EM Core Event Adapter Header 
#include "em_core.h"                            // EM Core 

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define EM_EXT_EVENT_APPEAR_COUNT               10      // Appear times to check event occurred, recovered 

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
EM_EXT_EVENT_RESULT EM_EXT_EVENT_Detection(void);

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: uint8_t EM_EXT_EVENT_RegisterPlugin(EM_EXT_CALLBACK cb_active_noload,                 
                                                     EM_EXT_CALLBACK cb_reactive_noload,               
                                                     EM_EXT_CALLBACK cb_apparent_noload,               
                                                     EM_EXT_EVENT_CALLBACK_MDM_CYCEND cb_max_demand,      
                                                     EM_EXT_EVENT_CALLBACK_MDM_CYCEND cb_tariff_changed,  
                                                     EM_EXT_EVENT_CALLBACK_ENERGY_CYCEND cb_energy_cycend,
                                                     EM_EXT_CALLBACK cb_peak_voltage,                     
                                                     EM_EXT_CALLBACK cb_peak_current,                     
                                                     EM_EXT_CALLBACK cb_freq_out_range,                   
                                                     EM_EXT_CALLBACK cb_sag_voltage);                     
* Description  : Register event plugin
* Arguments    : EM_EXT_CALLBACK cb_active_noload                     : Active Noload Callback
*              : EM_EXT_CALLBACK cb_reactive_noload                   : Reactive Noload Callback
*              : EM_EXT_CALLBACK cb_apparent_noload                   : Apparent Noload Callback
*              : EM_EXT_EVENT_CALLBACK_MDM_CYCEND cb_max_demand       : Max Demand Update End Callback
*              : EM_EXT_EVENT_CALLBACK_MDM_CYCEND cb_tariff_changed   : Tariff Changed Callback
*              : EM_EXT_EVENT_CALLBACK_ENERGY_CYCEND cb_energy_cycend : Energy Update End Callback
*              : EM_EXT_CALLBACK cb_peak_voltage                      : Peak Voltage Callback
*              : EM_EXT_CALLBACK cb_peak_current                      : Peak Current Callback
*              : EM_EXT_CALLBACK cb_freq_out_range                    : Frequency Out Of Range Callback
*              : EM_EXT_CALLBACK cb_sag_voltage                       : SAG (Low) voltage Callback
* Return Value : Execution status
*              : EM_EXT_OK      Success
*              : EM_EXT_ERROR   Error. Plugin registration is FAILED
***********************************************************************************************************************/
// we will have to registerplugin - this function cannot be completely commented 
// hence only one callback is being registered - the prototype in em_event_plugin.h has also been suitably changed
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
//                                    EM_EXT_EVENT_CALLBACK_TARIFF_CHANGE cb_tariff_changed, // Tariff Changed Callback 
//                                    EM_EXT_EVENT_CALLBACK_ENERGY_CYCEND cb_energy_cycend   // Energy Update End Callback 
)

{
    EM_EXT_EVENT_PLUGIN_INFO plugin_info;
    
    // Initial setting 
    plugin_info.fp_detection           = EM_EXT_EVENT_Detection;
/*    
    plugin_info.fp_cb_active_noload_R    = cb_active_noload_R  ;
    plugin_info.fp_cb_active_noload_Y    = cb_active_noload_Y  ;
    plugin_info.fp_cb_active_noload_B    = cb_active_noload_B  ;
    plugin_info.fp_cb_reactive_noload_R  = cb_reactive_noload_R;
    plugin_info.fp_cb_reactive_noload_Y  = cb_reactive_noload_Y;
    plugin_info.fp_cb_reactive_noload_B  = cb_reactive_noload_B;
    plugin_info.fp_cb_apparent_noload_R  = cb_apparent_noload_R;
    plugin_info.fp_cb_apparent_noload_Y  = cb_apparent_noload_Y;
    plugin_info.fp_cb_apparent_noload_B  = cb_apparent_noload_B;
    plugin_info.fp_cb_peak_voltage_R     = cb_peak_voltage_R   ;
    plugin_info.fp_cb_peak_voltage_Y     = cb_peak_voltage_Y   ;
    plugin_info.fp_cb_peak_voltage_B     = cb_peak_voltage_B   ;
    plugin_info.fp_cb_peak_current_R     = cb_peak_current_R   ;
    plugin_info.fp_cb_peak_current_Y     = cb_peak_current_Y   ;
    plugin_info.fp_cb_peak_current_B     = cb_peak_current_B   ;
    plugin_info.fp_cb_freq_out_range_R   = cb_freq_out_range_R ;
    plugin_info.fp_cb_freq_out_range_Y   = cb_freq_out_range_Y ;
    plugin_info.fp_cb_freq_out_range_B   = cb_freq_out_range_B ;
    plugin_info.fp_cb_sag_voltage_R      = cb_sag_voltage_R    ;
    plugin_info.fp_cb_sag_voltage_Y      = cb_sag_voltage_Y    ;
    plugin_info.fp_cb_sag_voltage_B      = cb_sag_voltage_B    ;
*/
    plugin_info.fp_cb_mdm_end          = cb_max_demand     ;
//    plugin_info.fp_cb_tariff_changed   = cb_tariff_changed ;
//    plugin_info.fp_cb_energy_cycend    = cb_energy_cycend  ;
    // Register to EM Core 
    return EM_EXT_EVENT_Register(&plugin_info);
}


/***********************************************************************************************************************
* Function Name: EM_EXT_EVENT_RESULT EM_EXT_EVENT_Detection(void)
* Description  : EM Event Detection API
*              : This API will be auto-called every 120ms (from EM Core)
* Arguments    : None
* Return Value : EM_EXT_EVENT_RESULT, event result
***********************************************************************************************************************/

// since the api is autocalled we will have a dummy function here

EM_EXT_EVENT_RESULT EM_EXT_EVENT_Detection(void)
{
	static EM_EXT_EVENT_RESULT  event_result = {0};                     // Event result 
	return event_result;
}
/*
EM_EXT_EVENT_RESULT EM_EXT_EVENT_Detection(void)
{   
    static EM_EXT_EVENT_RESULT  event_result = {0};                     // Event result 

    static int8_t               stick_count  = 0;                       // Stick Count 
    static int8_t               noload_act_appear_count[3] = {0,0,0};           // Active Noload Appear Count 
    static int8_t               noload_rea_appear_count[3] = {0,0,0};           // Reactive Noload Appear Count 
    static int8_t               noload_app_appear_count[3] = {0,0,0};           // Apparent Noload Appear Count 
    static int8_t               peak_voltage_appear_count[3] = {0,0,0};         // Peak Voltage Appear Count 
    static int8_t               peak_current_appear_count[3] = {0,0,0};         // Peak Current Appear Count 
    static int8_t               freq_out_range_appear_count[3] = {0,0,0};       // Frequency Out Range Appear Count 
    static int8_t               sag_voltage_appear_count[3] = {0,0,0};          // SAG (Low) Voltage Appear Count 

    EM_LINE                     phase_line;
    EM_CONFIG                   config;
    float32_t                   voltage, freq;

    // Increase stick count 
    stick_count++;
    
    // Get EM Core Configuration 
    config = EM_GetConfig();

    for(phase_line = LINE_PHASE_R; phase_line <= LINE_PHASE_B; phase_line ++)
    {
        // Check active noload condition 
        if (EM_EXT_GetActivePowerSign(phase_line) == 0)
        {
            noload_act_appear_count[phase_line]++;
        }
        else
        {
            noload_act_appear_count[phase_line]--;
        }
        
        // Check reactive noload condition 
        if (EM_EXT_GetReactivePowerSign(phase_line) == 0)
        {
            noload_rea_appear_count[phase_line]++;
        }
        else
        {
            noload_rea_appear_count[phase_line]--;
        }
        
        // Check apparent noload condition 
        if (EM_EXT_GetApparentPowerSign(phase_line) == 0)
        {
            noload_app_appear_count[phase_line]++;
        }
        else
        {
            noload_app_appear_count[phase_line]--;
        }
        
        // Check peak, sag voktage 
        voltage = EM_GetVoltageRMS(phase_line);
        if (voltage > (float32_t)config.voltage_high)
        {
            peak_voltage_appear_count[phase_line]++;
        }
        else
        {
            peak_voltage_appear_count[phase_line]--;    
        }
        if (voltage < (float32_t)config.voltage_low)
        {
            sag_voltage_appear_count[phase_line]++;
        }
        else
        {
            sag_voltage_appear_count[phase_line]--;
        }
        
        // Check peak current 
        if (EM_GetCurrentRMS(phase_line) > (float32_t)config.current_high)
        {
            peak_current_appear_count[phase_line]++;
        }
        else
        {
            peak_current_appear_count[phase_line]--;
        }
        
        // Check frequency out range 
        freq = EM_GetLineFrequency(phase_line);
        if (freq < (float32_t)config.freq_low ||
            freq > (float32_t)config.freq_high)
        {
            freq_out_range_appear_count[phase_line]++;
        }
        else
        {
            freq_out_range_appear_count[phase_line]--;
        }
        
        // Check active noload occurred, same as stick and reach max? 
        if (noload_act_appear_count[phase_line] == stick_count &&
            noload_act_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_active_noload_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_active_noload_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_active_noload_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check active noload recovered, same as stick and reach min?  
        else if (   noload_act_appear_count[phase_line] == (-stick_count) &&
                    noload_act_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_active_noload_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_active_noload_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_active_noload_B = 0;
                    break;
                default:
                    break;
            }
        }
        
        // Check reactive noload occurred, same as stick and reach max? 
        if (noload_rea_appear_count[phase_line] == stick_count &&
            noload_rea_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_reactive_noload_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_reactive_noload_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_reactive_noload_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check reactive noload recovered, same as stick and reach min?  
        else if (   noload_rea_appear_count[phase_line] == (-stick_count) &&
                    noload_rea_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_reactive_noload_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_reactive_noload_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_reactive_noload_B = 0;
                    break;
                default:
                    break;
            }
        }
        
        // Check apparent noload occurred, same as stick and reach max? 
        if (noload_app_appear_count[phase_line] == stick_count &&
            noload_app_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_apparent_noload_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_apparent_noload_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_apparent_noload_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check apparent noload recovered, same as stick and reach min?  
        else if (   noload_app_appear_count[phase_line] == (-stick_count) &&
                    noload_app_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_apparent_noload_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_apparent_noload_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_apparent_noload_B = 0;
                    break;
                default:
                    break;
            }
        }
        // Check peak current occurred, same as stick and reach max? 
        if (peak_current_appear_count[phase_line] == stick_count &&
            peak_current_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_peak_current_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_peak_current_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_peak_current_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check peak current recovered, same as stick and reach min?  
        else if (   peak_current_appear_count[phase_line] == (-stick_count) &&
                    peak_current_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_peak_current_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_peak_current_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_peak_current_B = 0;
                    break;
                default:
                    break;
            }
        }
        
        // Check peak voltage occurred, same as stick and reach max? 
        if (peak_voltage_appear_count[phase_line] == stick_count &&
            peak_voltage_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_peak_voltage_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_peak_voltage_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_peak_voltage_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check peak voltage recovered, same as stick and reach min?  
        else if (   peak_voltage_appear_count[phase_line] == (-stick_count) &&
                    peak_voltage_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_peak_voltage_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_peak_voltage_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_peak_voltage_B = 0;
                    break;
                default:
                    break;
            }
        }
        
        // Check sag voltage occurred, same as stick and reach max? 
        if (sag_voltage_appear_count[phase_line] == stick_count &&
            sag_voltage_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_sag_voltage_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_sag_voltage_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_sag_voltage_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check sag voltage recovered, same as stick and reach min?  
        else if (   sag_voltage_appear_count[phase_line] == (-stick_count) &&
                    sag_voltage_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_sag_voltage_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_sag_voltage_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_sag_voltage_B = 0;
                    break;
                default:
                    break;
            }
        }
        
        // Check freq out range occurred, same as stick and reach max? 
        if (freq_out_range_appear_count[phase_line] == stick_count &&
            freq_out_range_appear_count[phase_line] >= (EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_freq_out_range_R = 1;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_freq_out_range_Y = 1;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_freq_out_range_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check freq out range recovered, same as stick and reach min?  
        else if (   freq_out_range_appear_count[phase_line] == (-stick_count) &&
                    freq_out_range_appear_count[phase_line] <= (-EM_EXT_EVENT_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    event_result.details.is_freq_out_range_R = 0;
                    break;
                case LINE_PHASE_Y:
                    event_result.details.is_freq_out_range_Y = 0;
                    break;
                case LINE_PHASE_B:
                    event_result.details.is_freq_out_range_B = 0;
                    break;
                default:
                    break;
            }
        }
    }
    // Stick count 
    if (stick_count >= EM_EXT_EVENT_APPEAR_COUNT)
    {
        noload_act_appear_count[0] = 0;
        noload_act_appear_count[1] = 0;
        noload_act_appear_count[2] = 0;
        noload_rea_appear_count[0] = 0;
        noload_rea_appear_count[1] = 0;
        noload_rea_appear_count[2] = 0;
        noload_app_appear_count[0] = 0;
        noload_app_appear_count[1] = 0;
        noload_app_appear_count[2] = 0;
        peak_voltage_appear_count[0] = 0;
        peak_voltage_appear_count[1] = 0;
        peak_voltage_appear_count[2] = 0;
        peak_current_appear_count[0] = 0;
        peak_current_appear_count[1] = 0;
        peak_current_appear_count[2] = 0;
        freq_out_range_appear_count[0] = 0;
        freq_out_range_appear_count[1] = 0;
        freq_out_range_appear_count[2] = 0;
        sag_voltage_appear_count[0] = 0;
        sag_voltage_appear_count[1] = 0;
        sag_voltage_appear_count[2] = 0;
    
        stick_count = 0;
    }
    
    // Return tamper result 
    return event_result;
}
*/