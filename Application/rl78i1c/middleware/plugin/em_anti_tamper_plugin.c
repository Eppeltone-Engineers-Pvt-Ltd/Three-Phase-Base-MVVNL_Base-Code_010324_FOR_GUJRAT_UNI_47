/***********************************************************************************************************************
* File Name    : em_anti_tamper_plugin.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Anti-tempering Plugin Component
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "em_anti_tamper_adapter.h"     // EM Core Anti-tamper Adapter Header 
#include "platform.h"
#include "powermgmt.h"
#include "emeter3_structs.h"
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
// Common 
#define ABS(x)      ((x) <  0  ? (-(x)) : (x))          // Get absolute value 
#define MAX(x, y)   ((x) < (y) ? (   y) : (x))          // Get max value 
#define MIN(x, y)   ((x) < (y) ? (   x) : (y))          // Get min value 

// For ELT, REV event checking 
#define EM_EXT_TAMPER_THRESHOLD                     (0.3f)      // Start check when >= 30 % 
#define EM_EXT_VOLTAGE_UNBALANCE_THRESHOLD          (0.005f)    // Start check when offest ~ 0.2 % 
#define EM_EXT_CURRENT_VECTOR_THRESHOLD             (0.3f)      // Difference between Neutral and Vector current for CT Open check 
#define EM_EXT_TAMPER_APPEAR_COUNT                  (30)        // Appear times to check ELT, REV occurred, recovered 
#define EM_EXT_IVECTOR_RMS_UNBALANCE_THRESHOLD      (0.05f)     // RMS Min Threshold for IVector to determine Current Unbalance 
#define EM_EXT_CURRENT_BYPASS_THRESHOLD             (0.025f)    // Start check when greater than 25mA 

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
EM_EXT_TAMPER_RESULT EM_EXT_TAMPER_Detection(void);

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: uint8_t EM_EXT_TAMPER_RegisterPlugin(
*              :     EM_EXT_CALLBACK cb_neutral_missing,
*              :     EM_EXT_CALLBACK cb_earth,
*              :     EM_EXT_CALLBACK cb_reverse
*              : );
* Description  : Register anti-tamper plugin
* Arguments    : EM_EXT_CALLBACK cb_neutral_missing: Neutral missing callback
*              : EM_EXT_CALLBACK cb_earth          : Earth connected callback
*              : EM_EXT_CALLBACK cb_reverse        : Reverse current callback
* Return Value : Execution status
*              : EM_EXT_OK      Success
*              : EM_EXT_ERROR   Error. Plugin registration is FAILED
***********************************************************************************************************************/
// we will have to registerplugin - this function cannot be completely commented
// hence only one callback is being registered - the prototype in em_anti_tamper_plugin.h has also been suitably changed

uint8_t EM_EXT_TAMPER_RegisterPlugin(
/*
    EM_EXT_CALLBACK     cb_neutral_missing,
    EM_EXT_CALLBACK     cb_MissingPotential_R,
    EM_EXT_CALLBACK     cb_MissingPotential_Y,
    EM_EXT_CALLBACK     cb_MissingPotential_B,
    EM_EXT_CALLBACK     cb_VoltageUnbalance,
    EM_EXT_CALLBACK     cb_ReverseCurrent_R,
    EM_EXT_CALLBACK     cb_ReverseCurrent_Y,
    EM_EXT_CALLBACK     cb_ReverseCurrent_B,
    EM_EXT_CALLBACK     cb_OpenCurrent_R,
    EM_EXT_CALLBACK     cb_OpenCurrent_Y,
    EM_EXT_CALLBACK     cb_OpenCurrent_B,
    EM_EXT_CALLBACK     cb_CurrentUnbalance,
*/
    EM_EXT_CALLBACK     cb_CurrentBypass
)
{
    EM_EXT_TAMPER_PLUGIN_INFO plugin_info;
    
    // Initial setting 
//    plugin_info.neutral_missing_energy_selection   = EM_EXT_TAMPER_ENERGY_SELECT_SPECIFIED_POWER;
//    plugin_info.missing_potential_energy_selection = EM_EXT_TAMPER_ENERGY_SELECT_SPECIFIED_POWER;
//    plugin_info.voltage_unbalance_energy_selection = EM_EXT_TAMPER_ENERGY_SELECT_UPDATE_NORMAL;
//    plugin_info.rev_energy_selection               = EM_EXT_TAMPER_ENERGY_SELECT_UPDATE_NORMAL;
//    plugin_info.current_open_energy_selection      = EM_EXT_TAMPER_ENERGY_SELECT_SPECIFIED_POWER;
//    plugin_info.current_unbalance_energy_selection = EM_EXT_TAMPER_ENERGY_SELECT_UPDATE_NORMAL;
    plugin_info.current_bypass_energy_selection    = EM_EXT_TAMPER_ENERGY_SELECT_UPDATE_NORMAL;

    // Map to user detection & callback APIs 
    plugin_info.fp_detection               = EM_EXT_TAMPER_Detection;
 /*
		plugin_info.fp_cb_NeutralMissing       = cb_neutral_missing;
    plugin_info.fp_cb_MissingPotential_R   = cb_MissingPotential_R;
    plugin_info.fp_cb_MissingPotential_Y   = cb_MissingPotential_Y;
    plugin_info.fp_cb_MissingPotential_B   = cb_MissingPotential_B;
    plugin_info.fp_cb_VoltageUnbalance     = cb_VoltageUnbalance  ;
    plugin_info.fp_cb_ReverseCurrent_R     = cb_ReverseCurrent_R  ;
    plugin_info.fp_cb_ReverseCurrent_Y     = cb_ReverseCurrent_Y  ;
    plugin_info.fp_cb_ReverseCurrent_B     = cb_ReverseCurrent_B  ;
    plugin_info.fp_cb_OpenCurrent_R        = cb_OpenCurrent_R     ;
    plugin_info.fp_cb_OpenCurrent_Y        = cb_OpenCurrent_Y     ;
    plugin_info.fp_cb_OpenCurrent_B        = cb_OpenCurrent_B     ;
    plugin_info.fp_cb_CurrentUnbalance     = cb_CurrentUnbalance  ;
*/
		plugin_info.fp_cb_CurrentBypass        = cb_CurrentBypass     ;
    
    // Register to EM Core 
    return EM_EXT_TAMPER_Register(&plugin_info);
}

/***********************************************************************************************************************
* Function Name: EM_EXT_TAMPER_RESULT EM_EXT_TAMPER_Detection(void)
* Description  : EM Tamper Detection API
*              : This API will be auto-called every 120ms (from EM Core)
* Arguments    : None
* Return Value : EM_EXT_TAMPER_RESULT, tamper result
***********************************************************************************************************************/

// since the following function is being called anyways every 120 ms - we can use it to pass the assumed power back to the EM Core
// so as to cause it to pulse as per the assumed power
// a flag may be used for this purpose - if the flag is set use the global variable assumedPower
// if the flag is clear 


EM_EXT_TAMPER_RESULT EM_EXT_TAMPER_Detection(void)
{
    // Tamper result 
    static EM_EXT_TAMPER_RESULT tamper_result =
    {
        // No power 
        0.0f,
        // No tamper 
        0,
    };

/*
    static int8_t   missing_potential_appear_count[3] = {0, 0, 0};      // Missing potential appear Count 
    static int8_t   reverse_current_appear_count[3]   = {0, 0, 0};      // Reverse Current appear Count 
    static int8_t   open_current_appear_count[3]      = {0, 0, 0};      // Open Current appear Count 
    static int8_t   neutral_missing_appear_count      = 0;              // Neutral Missing appear Count 
    static int8_t   voltage_unbalance_appear_count    = 0;              // Voltage Unbalance appear Count 
    static int8_t   current_unbalance_appear_count    = 0;              // Current Unbalance appear Count 
    static int8_t   current_bypass_appear_count       = 0;              // Current Bypass appear Count 

    static int8_t   stick_count  = 0;           // Stick Count 
    float32_t       max, min, sub;
    float32_t       i[EM_NUM_OF_LINE];
    float32_t       v[EM_NUM_OF_PHASE];
    float32_t       ivector;
    EM_LINE         phase_line;
    
#ifdef  POWER_MANAGEMENT_ENABLE
    // Do not check the tamper when AC is low 
    if (POWERMGMT_IsACLow() == 1)
    {
        return tamper_result;
    }
#endif    
    // Stick count increase 
    stick_count++;

    // TODO: define power here 
    tamper_result.specified_power = 0.0f;

    i[LINE_NEUTRAL] = EM_EXT_GetIRMS(LINE_NEUTRAL);
    ivector = EM_EXT_GetPhaseCurrentSumVector();

    for(phase_line = LINE_PHASE_R; phase_line <= LINE_PHASE_B; phase_line ++)
    {
        // Check missing potential condition 
        v[phase_line] = EM_EXT_GetVRMS(phase_line);
        if (v[phase_line] == 0)
        {
            missing_potential_appear_count[phase_line]++;
        }
        else
        {
            missing_potential_appear_count[phase_line]--;
        }
        // Check reverse current condition 
        if (EM_EXT_GetActivePowerSign(phase_line) < 0)
        {
            reverse_current_appear_count[phase_line]++;
        }
        else
        {
            reverse_current_appear_count[phase_line]--;
        }
        // Check open current condition 
        i[phase_line] = EM_EXT_GetIRMS(phase_line);
        if ( (ABS(i[LINE_NEUTRAL] - ivector) > (EM_EXT_CURRENT_VECTOR_THRESHOLD * i[LINE_NEUTRAL])) )
        {
            if (i[phase_line] == 0.0f)
            {
                open_current_appear_count[phase_line]++;
            }
        }
        else
        {
            open_current_appear_count[phase_line]--;
        }

        // Check missing potential occurred, same as stick and reach max? 
        if (missing_potential_appear_count[phase_line] == stick_count &&
            missing_potential_appear_count[phase_line] >= (EM_EXT_TAMPER_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    tamper_result.tamper_status.details.is_missing_potential_R = 1;
                    break;
                case LINE_PHASE_Y:
                    tamper_result.tamper_status.details.is_missing_potential_Y = 1;
                    break;
                case LINE_PHASE_B:
                    tamper_result.tamper_status.details.is_missing_potential_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check missing potential recovered, same as stick and reach min? 
        else if (   missing_potential_appear_count[phase_line] == (-stick_count) &&
                    missing_potential_appear_count[phase_line] <= (-EM_EXT_TAMPER_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    tamper_result.tamper_status.details.is_missing_potential_R = 0;
                    break;
                case LINE_PHASE_Y:
                    tamper_result.tamper_status.details.is_missing_potential_Y = 0;
                    break;
                case LINE_PHASE_B:
                    tamper_result.tamper_status.details.is_missing_potential_B = 0;
                    break;
                default:
                    break;
            }
        }
        // Check reverse current occurred, same as stick and reach max? 
        if (reverse_current_appear_count[phase_line] == stick_count &&
            reverse_current_appear_count[phase_line] >= (EM_EXT_TAMPER_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    tamper_result.tamper_status.details.is_reverse_current_R = 1;
                    break;
                case LINE_PHASE_Y:
                    tamper_result.tamper_status.details.is_reverse_current_Y = 1;
                    break;
                case LINE_PHASE_B:
                    tamper_result.tamper_status.details.is_reverse_current_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check reverse current recovered, same as stick and reach min? 
        else if (   reverse_current_appear_count[phase_line] == (-stick_count) &&
                    reverse_current_appear_count[phase_line] <= (-EM_EXT_TAMPER_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    tamper_result.tamper_status.details.is_reverse_current_R = 0;
                    break;
                case LINE_PHASE_Y:
                    tamper_result.tamper_status.details.is_reverse_current_Y = 0;
                    break;
                case LINE_PHASE_B:
                    tamper_result.tamper_status.details.is_reverse_current_B = 0;
                    break;
                default:
                    break;
            }
        }
        // Check open current occurred, same as stick and reach max? 
        if (open_current_appear_count[phase_line] == stick_count &&
            open_current_appear_count[phase_line] >= (EM_EXT_TAMPER_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    tamper_result.tamper_status.details.is_open_current_R = 1;
                    break;
                case LINE_PHASE_Y:
                    tamper_result.tamper_status.details.is_open_current_Y = 1;
                    break;
                case LINE_PHASE_B:
                    tamper_result.tamper_status.details.is_open_current_B = 1;
                    break;
                default:
                    break;
            }
        }
        // Check open current recovered, same as stick and reach min? 
        else if (   open_current_appear_count[phase_line] == (-stick_count) &&
                    open_current_appear_count[phase_line] <= (-EM_EXT_TAMPER_APPEAR_COUNT))
        {
            switch(phase_line)
            {
                case LINE_PHASE_R:
                    tamper_result.tamper_status.details.is_open_current_R = 0;
                    break;
                case LINE_PHASE_Y:
                    tamper_result.tamper_status.details.is_open_current_Y = 0;
                    break;
                case LINE_PHASE_B:
                    tamper_result.tamper_status.details.is_open_current_B = 0;
                    break;
                default:
                    break;
            }
        }
    }
    
    // Check  voltage unbalance condition 
    if ((ABS(v[LINE_PHASE_R] - v[LINE_PHASE_Y]) > (EM_EXT_VOLTAGE_UNBALANCE_THRESHOLD * v[LINE_PHASE_R])) ||
        (ABS(v[LINE_PHASE_Y] - v[LINE_PHASE_B]) > (EM_EXT_VOLTAGE_UNBALANCE_THRESHOLD * v[LINE_PHASE_Y])) ||
        (ABS(v[LINE_PHASE_B] - v[LINE_PHASE_R]) > (EM_EXT_VOLTAGE_UNBALANCE_THRESHOLD * v[LINE_PHASE_B]))
    )
    {
        voltage_unbalance_appear_count++;
    }
    else
    {
        voltage_unbalance_appear_count--;
    }
    // Check voltage unbalance occurred, same as stick and reach max? 
    if (voltage_unbalance_appear_count == stick_count &&
        voltage_unbalance_appear_count >= (EM_EXT_TAMPER_APPEAR_COUNT))
    {
        tamper_result.tamper_status.details.is_voltage_unbalance = 1;
    }
    // Check voltage unbalance recovered, same as stick and reach min? 
    else if (   voltage_unbalance_appear_count == (-stick_count) &&
                voltage_unbalance_appear_count <= (-EM_EXT_TAMPER_APPEAR_COUNT))
    {
        tamper_result.tamper_status.details.is_voltage_unbalance = 0;
    }

    // Check current unbalance condition 
    if (ivector > EM_EXT_IVECTOR_RMS_UNBALANCE_THRESHOLD)
    {
        current_unbalance_appear_count++;
    }
    else
    {
        current_unbalance_appear_count--;
    }
    // Check current unbalance occurred, same as stick and reach max? 
    if (current_unbalance_appear_count == stick_count &&
        current_unbalance_appear_count >= (EM_EXT_TAMPER_APPEAR_COUNT))
    {
        tamper_result.tamper_status.details.is_current_unbalance = 1;
    }
    // Check current bypass recovered, same as stick and reach min? 
    else if (   current_unbalance_appear_count == (-stick_count) &&
                current_unbalance_appear_count <= (-EM_EXT_TAMPER_APPEAR_COUNT))
    {
        tamper_result.tamper_status.details.is_current_unbalance = 0;
    }
    
    // Check current bypass condition 
    if (    (   ABS(i[LINE_NEUTRAL] - ivector) > EM_EXT_CURRENT_BYPASS_THRESHOLD) &&  
                i[LINE_PHASE_R] > 0.0f && 
                i[LINE_PHASE_Y] > 0.0f && 
                i[LINE_PHASE_B] > 0.0f &&
                (1)
        )
    {
        current_bypass_appear_count++;
    }
    else
    {
        current_bypass_appear_count--;
    }
    // Check current bypass occurred, same as stick and reach max? 
    if (current_bypass_appear_count == stick_count &&
        current_bypass_appear_count >= (EM_EXT_TAMPER_APPEAR_COUNT))
    {
        tamper_result.tamper_status.details.is_current_bypass = 1;
    }
    // Check current bypass recovered, same as stick and reach min? 
    else if (   current_bypass_appear_count == (-stick_count) &&
                current_bypass_appear_count <= (-EM_EXT_TAMPER_APPEAR_COUNT))
    {
        tamper_result.tamper_status.details.is_current_bypass = 0;
    }

    // Stick count 
    if (stick_count >= EM_EXT_TAMPER_APPEAR_COUNT)
    {
        missing_potential_appear_count[0] = 0;
        missing_potential_appear_count[1] = 0;
        missing_potential_appear_count[2] = 0;
        reverse_current_appear_count[0]   = 0;
        reverse_current_appear_count[1]   = 0;
        reverse_current_appear_count[2]   = 0;
        open_current_appear_count[0]      = 0;  
        open_current_appear_count[1]      = 0;  
        open_current_appear_count[2]      = 0;  
        neutral_missing_appear_count      = 0;
        voltage_unbalance_appear_count    = 0;
        current_unbalance_appear_count    = 0; 
        current_bypass_appear_count       = 0;
        
        stick_count = 0;
    }
*/    
    // Return tamper result 
    tamper_result.specified_power = (float)total_active_power;
    return tamper_result;
}
