/***********************************************************************************************************************
* File Name    : em_anti_tamper_adapter.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : EM Anti-tamper Adapter Layer APIs
***********************************************************************************************************************/

#ifndef _EM_ANTI_TAMPER_ADAPTER_H
#define _EM_ANTI_TAMPER_ADAPTER_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"                /* GSCE Standard Typedef */
#include "em_adapter.h"             /* Shared Adapter Layer */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/* Energy select when tamper occured */
typedef enum tagEMExtAntiTamperEnergySelect
{
    EM_EXT_TAMPER_ENERGY_SELECT_UPDATE_NORMAL = 0,      /* Update (accumulate) normally */
    EM_EXT_TAMPER_ENERGY_SELECT_NO_UPDATE,              /* No update */
    EM_EXT_TAMPER_ENERGY_SELECT_MAX_DEMAND,             /* Update by max demand */
    EM_EXT_TAMPER_ENERGY_SELECT_SPECIFIED_POWER,        /* Update by a specified power value */
} EM_EXT_TAMPER_ENERGY_SELECT;

/* Tamper Plugin Result */
typedef struct tagEMExtAntiTamperEventResult
{
    float32_t   specified_power;                    /* Specified power to update when tamper happen */

    union
    {
        uint32_t value;
        struct
        {
            uint8_t  is_neutral_missing             :1;     /* Is Neutral missing occurred or not? */
            uint8_t  is_missing_potential_R         :1;     /* Missing Potential */
            uint8_t  is_missing_potential_Y         :1;     /* Missing Potential */
            uint8_t  is_missing_potential_B         :1;     /* Missing Potential */
            uint8_t  is_voltage_unbalance           :1;     /* Voltage Unbalance */
            uint8_t  is_reverse_current_R           :1;     /* Reverse Current */
            uint8_t  is_reverse_current_Y           :1;     /* Reverse Current */
            uint8_t  is_reverse_current_B           :1;     /* Reverse Current */
            
            uint8_t  is_open_current_R              :1;     /* Open Current */
            uint8_t  is_open_current_Y              :1;     /* Open Current */
            uint8_t  is_open_current_B              :1;     /* Open Current */
            uint8_t  is_current_unbalance           :1;     /* Current Unbalance */
            uint8_t  is_current_bypass              :1;     /* Current Bypass */
            uint8_t  reserved1                      :3;     /* reserved */
            
            uint8_t  reserved2                      :8;     /* reserved */
            
            uint8_t  reserved3                      :8;     /* reserved */
        } details;
    } tamper_status;
    
} EM_EXT_TAMPER_RESULT;

/* Tamper Plugin Detection API */
typedef EM_EXT_TAMPER_RESULT (*EM_EXT_TAMPER_DETECTION_API)(void);  /* Detection API prototype */

/* Tamper Plugin Information */
typedef struct tagEMExtAntiTamperPluginInfo
{
    /*
     * Plugin Configuration
     */

    /* Selection to update energy when Neutral missing tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     neutral_missing_energy_selection;

    /* Selection to update energy when missing potential tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     missing_potential_energy_selection;
    
    /* Selection to update energy when voltage unbalance tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     voltage_unbalance_energy_selection;

    /* Selection to update energy when REV tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     rev_energy_selection;

    /* Selection to update energy when CT open tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     current_open_energy_selection;

    /* Selection to update energy when current unbalance tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     current_unbalance_energy_selection;

    /* Selection to update energy when current bypass tamper occured */
    EM_EXT_TAMPER_ENERGY_SELECT     current_bypass_energy_selection;
    
    /* Reserved */
    EM_EXT_TAMPER_ENERGY_SELECT     reserved;

    /* Plugin Detection (Connector) API, NULL to deactive the plugin */
    EM_EXT_TAMPER_DETECTION_API     fp_detection;       /* Function pointer of detection API */
    
    /* Callback for tamper event, specify NULL to no use callback */
    EM_EXT_CALLBACK                 fp_cb_NeutralMissing;
    EM_EXT_CALLBACK                 fp_cb_MissingPotential_R;
    EM_EXT_CALLBACK                 fp_cb_MissingPotential_Y;
    EM_EXT_CALLBACK                 fp_cb_MissingPotential_B;
    EM_EXT_CALLBACK                 fp_cb_VoltageUnbalance;
    EM_EXT_CALLBACK                 fp_cb_ReverseCurrent_R;
    EM_EXT_CALLBACK                 fp_cb_ReverseCurrent_Y;
    EM_EXT_CALLBACK                 fp_cb_ReverseCurrent_B;
    EM_EXT_CALLBACK                 fp_cb_OpenCurrent_R;
    EM_EXT_CALLBACK                 fp_cb_OpenCurrent_Y;
    EM_EXT_CALLBACK                 fp_cb_OpenCurrent_B;
    EM_EXT_CALLBACK                 fp_cb_CurrentUnbalance;
    EM_EXT_CALLBACK                 fp_cb_CurrentBypass;
    
} EM_EXT_TAMPER_PLUGIN_INFO;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Plugin Registration API
 * Please use this API to register the plugin into the EM Core
 * Note that, all plugin information must be correct,
 * if not, EM_EXT_ERROR will be returned, otherwise, EM_EXT_OK.
 */
uint8_t EM_EXT_TAMPER_Register(__near const EM_EXT_TAMPER_PLUGIN_INFO *p_plugin_info);

#endif /* _EM_ANTI_TAMPER_ADAPTER_H */
