/****************************************************************************** 
* File Name    : service_rw_calibration.c
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
#include <string.h>
#include "r_cg_dsadc.h"

/* Global */
#include "typedef.h"            /* GSCE Typedef */

/* Wrapper */
#include "wrp_em_sw_config.h"

/* Middleware */
#include "em_core.h"            /* EM Core Library */

/* Application */
#include "service.h"            /* Services */
#include "service_rw_calibration.h"
#include "config_storage.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct tagMemoryConfig
{
    uint8_t     data_id;    /* Data ID of Config */
    uint32_t    address;    /* Address of Config on Memory */   
    uint16_t    length;     /* Length of Config on Memory */    
} MEMORY_CONFIG;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static const MEMORY_CONFIG g_memory_config_map[] = 
{
    {
        EM_DATA_ID_SAMPLING_FREQUENCY                               ,
        CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_ADDR                ,
        CONFIG_STORAGE_CALIB_SAMPLING_FREQUENCY_SIZE                ,
    },      
        
    {       
        EM_DATA_ID_METER_CONSTANT_PHASE_CALIB                       ,
        CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_ADDR              ,
        CONFIG_STORAGE_CALIB_METER_CONSTANT_PHASE_SIZE              ,
    },      
        
    {       
        EM_DATA_ID_METER_CONSTANT_CALIB                             ,
        CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_ADDR              ,
        CONFIG_STORAGE_CALIB_METER_CONSTANT_TOTAL_SIZE              ,
    },
    {
        EM_DATA_ID_REACTIVE_TIMER_OFFSET                            ,
        CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_ADDR           ,
        CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_SIZE           ,
    },
            
    /* Voltage coefficient, 4 bytes, float32_t */       
    {       
        EM_DATA_ID_P1_VOLTAGE_COEFFICIENT                           ,
        CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_R_SIZE                ,
    },      
    {       
        EM_DATA_ID_P2_VOLTAGE_COEFFICIENT                       ,
        CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_Y_SIZE                ,
    },      
    {       
        EM_DATA_ID_P3_VOLTAGE_COEFFICIENT                       ,
        CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_VRMS_PHASE_B_SIZE                ,
    },      
            
    /* Phase current coefficient 4 bytes, float32_t */      
    {       
        EM_DATA_ID_P1_CURRENT_COEFFICIENT                       ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_R_SIZE                ,
    },      
    {       
        EM_DATA_ID_P2_CURRENT_COEFFICIENT                       ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_Y_SIZE                ,
    },      
    {       
        EM_DATA_ID_P3_CURRENT_COEFFICIENT                       ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_PHASE_B_SIZE                ,
    },      
    {       
        EM_DATA_ID_NEUTRAL_CURRENT_COEFFICIENT                  ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_ADDR                ,
        CONFIG_STORAGE_CALIB_COEFF_IRMS_NEUTRAL_SIZE                ,
    },
    
    /* ACTIVE_POWER Coefficient */
    {
        EM_DATA_ID_P1_ACTIVE_POWER_COEFFICIENT                  ,
        CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_ADDR        ,
        CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_R_SIZE        ,
    },  
    {   
        EM_DATA_ID_P2_ACTIVE_POWER_COEFFICIENT                  ,
        CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_ADDR        ,
        CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_Y_SIZE        ,
    },  
    {   
        EM_DATA_ID_P3_ACTIVE_POWER_COEFFICIENT                  ,
        CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_ADDR        ,
        CONFIG_STORAGE_CALIB_COEFF_ACTIVE_POWER_PHASE_B_SIZE        ,
    },
    
    /* REACTIVE_POWER Coefficient */
    {
        EM_DATA_ID_P1_REACTIVE_COEFFICIENT                      ,
        CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_ADDR      ,
        CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_R_SIZE      ,
    },  
    {   
        EM_DATA_ID_P2_REACTIVE_COEFFICIENT                      ,
        CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_ADDR      ,
        CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_Y_SIZE      ,
    },  
    {   
        EM_DATA_ID_P3_REACTIVE_COEFFICIENT                      ,
        CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_ADDR      ,
        CONFIG_STORAGE_CALIB_COEFF_REACTIVE_POWER_PHASE_B_SIZE      ,
    },  
        
    /* APPARENT_POWER Coefficient */ 
    {   
        EM_DATA_ID_P1_APPARENT_COEFFICIENT                      ,
        CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_ADDR      ,
        CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_R_SIZE      ,
    },  
    {   
        EM_DATA_ID_P2_APPARENT_COEFFICIENT                      ,
        CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_ADDR      ,
        CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_Y_SIZE      ,
    },  
    {   
        EM_DATA_ID_P3_APPARENT_COEFFICIENT                      ,
        CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_ADDR      ,
        CONFIG_STORAGE_CALIB_COEFF_APPARENT_POWER_PHASE_B_SIZE      ,
    }, 
    
    /* Phase Shift Degree */
    {   
        EM_DATA_ID_P1_PHASE_SHIFT_LEVEL0                      ,
        CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_ADDR      ,
        sizeof(float32_t)      ,
    },  
    {   
        EM_DATA_ID_P2_PHASE_SHIFT_LEVEL0                      ,
        CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_ADDR      ,
        sizeof(float32_t)      ,
    },  
    {   
        EM_DATA_ID_P3_PHASE_SHIFT_LEVEL0                      ,
        CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_ADDR      ,
        sizeof(float32_t)      ,
    },
    
    /* Phase Gain */
    {   
        EM_DATA_ID_P1_PHASE_GAIN_LEVEL0                      ,
        CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_ADDR                      ,
        sizeof(float32_t)                      ,
        
    },  
    {   
        EM_DATA_ID_P2_PHASE_GAIN_LEVEL0                      ,
        CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_ADDR                      ,
        sizeof(float32_t)                      ,
        
    },  
    {   
        EM_DATA_ID_P3_PHASE_GAIN_LEVEL0                      ,
        CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_ADDR                      ,
        sizeof(float32_t)                      ,
        
    },
    
    /* Voltage Offset */
    {   
        EM_DATA_ID_P1_PHASE_VOLTAGE_OFFSET                      ,
        CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_ADDR                    ,
        CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_R_SIZE                    ,
    },  
    {   
        EM_DATA_ID_P2_PHASE_VOLTAGE_OFFSET                      ,
        CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_ADDR      ,
        CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_Y_SIZE      ,
    },  
    {   
        EM_DATA_ID_P3_PHASE_VOLTAGE_OFFSET                      ,
        CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_ADDR     ,
        CONFIG_STORAGE_CALIB_OFFSET_VOLTAGE_PHASE_B_SIZE      ,
    },
};

static const uint16_t g_memory_config_map_size = (sizeof(g_memory_config_map) / sizeof(MEMORY_CONFIG));

/******************************************************************************
* Function Name : SERVICE_GetMemoryConfigPointer
* Interface     : static MEMORY_CONFIG SERVICE_GetMemoryConfigPointer(uint8_t data_id)
* Description   : Get the relative MEMORY_CONFIG info from data_id
* Arguments     : uint8_t data_id: Input of data id
* Function Calls: None
* Return Value  : static MEMORY_CONFIG, null when error happen
******************************************************************************/
static MEMORY_CONFIG* SERVICE_GetMemoryConfigPointer(uint8_t data_id)
{
    uint8_t i;
    
    for (i = 0; i < g_memory_config_map_size; i++)
    {
        if (g_memory_config_map[i].data_id == data_id)
        {
            /* Found element */
            return ((MEMORY_CONFIG*)&g_memory_config_map[i]);
        }
    }
    
    /* Not found */
    return NULL;
}

/******************************************************************************
* Function Name : SERVICE_UpdateCRC
* Interface     : static void SERVICE_UpdateCRC(void)
* Description   : Update the CRC After Writing Calibration to Memory
*               : This make the CRC checking when restore valid
* Arguments     : None
* Function Calls: None
* Return Value  : None
******************************************************************************/
static uint8_t SERVICE_UpdateCRC(void)
{
    EM_CALIBRATION  calib;
    dsad_reg_setting_t regs;
    uint16_t        config_crc16;
    uint16_t        calculated_crc;
    config_crc_data crc_data;
    float32_t       degree_list_r[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    float32_t       degree_list_y[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    float32_t       degree_list_b[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    float32_t       gain_list_r[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    float32_t       gain_list_y[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    float32_t       gain_list_b[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    uint8_t         driver_adc_gain0[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    uint8_t         driver_adc_gain1[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    int16_t         driver_timer_offset;
    
    /* Clear the temp driver gain list */
//    memset(&degree_list_r, 0, sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&degree_list_y, 0, sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&degree_list_b, 0, sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&gain_list_r, 0, sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&gain_list_y, 0, sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&gain_list_b, 0, sizeof(float32_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&driver_adc_gain0, 0, sizeof(uint8_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
//    memset(&driver_adc_gain1, 0, sizeof(uint8_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
    
    /* Get calib from EEPROM, accept sw phase & gain value list pointer */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_ADDR, 
                        (uint8_t *)&calib,
                        CONFIG_STORAGE_CALIB_NO_ARRAY_SIZE) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    
    /* 
     * Get SW Phase Correction array 
    */
    /* Phase_R */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_ADDR, 
                        (uint8_t *)degree_list_r,
                        CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_SIZE   ) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    calib.sw_phase_correction.phase_r.i_phase_degrees = degree_list_r;
    
    /* Phase_Y */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_ADDR, 
                        (uint8_t *)degree_list_y,
                        CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_SIZE   ) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    calib.sw_phase_correction.phase_y.i_phase_degrees = degree_list_y;
    
    /* Phase_B */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_ADDR, 
                        (uint8_t *)degree_list_b,
                        CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_SIZE   ) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    calib.sw_phase_correction.phase_b.i_phase_degrees = degree_list_b;
    
    /* 
    * Get SW Gain array 
    */
    /* Phase_R */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_ADDR, 
                      (uint8_t *)gain_list_r,
                      CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_SIZE ) != LDEV_OK)
    {
      return 1;   /* Read error */
    }
    calib.sw_gain.phase_r.i_gain_values = gain_list_r;

    /* Phase_Y */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_ADDR, 
                      (uint8_t *)gain_list_y,
                      CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_SIZE ) != LDEV_OK)
    {
      return 1;   /* Read error */
    }
    calib.sw_gain.phase_y.i_gain_values = gain_list_y;

    /* Phase_B */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_ADDR, 
                      (uint8_t *)gain_list_b,
                      CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_SIZE ) != LDEV_OK)
    {
      return 1;   /* Read error */
    }
    calib.sw_gain.phase_b.i_gain_values = gain_list_b;
    
    /* 
     * Get Driver ADC Gain Array 
    */
    /* Gain0 */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_ADDR, 
                        (uint8_t *)driver_adc_gain0,
                        CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE    ) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    
    /* Gain1 */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_ADDR, 
                        (uint8_t *)driver_adc_gain1,
                        CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_SIZE    ) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    
    /* Timer offset */
    if (    SERVICE_CONFIG_Read(   CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_ADDR, 
                        (uint8_t *)&driver_timer_offset,
                        CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_SIZE   ) != LDEV_OK)
    {
        return 1;   /* Read error */
    }
    
    /* Verify with CRC16 on EEPROM */
    crc_data.p_calibration = &calib;
    crc_data.p_driver_adc_gain0_list = &driver_adc_gain0[0];
    crc_data.p_driver_adc_gain1_list = &driver_adc_gain1[0];
    crc_data.p_driver_timer_offset = &driver_timer_offset;
    
    calculated_crc = CONFIG_CalculateCRC16(&crc_data);
    
    /* Update the Config */
    SERVICE_CONFIG_Write(   CONFIG_STORAGE_CRC_ADDR,
                            (uint8_t *)&calculated_crc,
                            CONFIG_STORAGE_CRC_SIZE
                        );
                        
    return 0;
}

/******************************************************************************
* Function Name : SERVICE_ReadCalibrationData
* Interface     : uint8_t SERVICE_ReadCalibrationData(
*               :     uint8_t data_id,
*               :     uint8_t *buff,
*               :     uint8_t len
*               : );
* Description   : Read Calibration Data Service
* Arguments     : uint8_t data_id  : Data ID
*               : uint8_t * buff   : buffer to be filled to
*               : uint8_t len      : Expected length to read
* Function Calls: TBD
* Return Value  : uint8_t, success or not
******************************************************************************/
service_result_t SERVICE_ReadCalibrationData(st_service_param *p_params)
{
    /* Variables */
    MEMORY_CONFIG *p_memory_config;
    uint32_t u32_data;
    uint16_t u16_data;
    float32_t f_data;    
    uint8_t DataID = *p_params->p_id_list;
    uint8_t is_signed = EM_DATA_ID_IS_SIGNED_DATA(DataID);
    uint8_t is_2byte = EM_DATA_ID_IS_2BYTE_DATA(DataID);
    uint8_t is_integer = EM_DATA_ID_IS_INTEGER_DATA(DataID);
    /* Params check */
    if (p_params == NULL)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Check internal params pointer */
    if (    p_params->p_id_list == NULL         || 
            p_params->p_buff_out == NULL            ||
            p_params->p_len_out == NULL             ||
            p_params->p_total_block == NULL     ||
            (0)
        )
    {
        return SERVICE_PARAM_ERROR;
    }
    
    if (EM_DATA_ID_IS_CLEAR_FLAG(DataID) == 1)
    {
        /* Dummy return 0 value */
        memset(p_params->p_buff_out, 0, 4);
    }
    else if (is_integer == 1)
    {
        /* Set the total block */
        *p_params->p_total_block = 1;
        
        /* Get EEPROM info */
        p_memory_config = SERVICE_GetMemoryConfigPointer(DataID);
            
        if (p_memory_config != NULL)
        {
            if (is_2byte)
            {
                // Else, everything is OK 
                if (SERVICE_CONFIG_Read(
                        p_memory_config->address,
                        (uint8_t *)&u16_data,
                        p_memory_config->length
                    ) != SERVICE_CONFIG_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                if (is_signed == 1)
                {
                    f_data = (float32_t)(int16_t)u16_data;
                }
                else
                {
                    f_data = (float32_t)u16_data;
                }
            }
            else
            {
                // Else, everything is OK 
                if (SERVICE_CONFIG_Read(
                        p_memory_config->address,
                        (uint8_t *)&u32_data,
                        p_memory_config->length
                    ) != SERVICE_CONFIG_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                if (is_signed == 1)
                {
                    f_data = (float32_t)(int32_t)u32_data;
                }
                else
                {
                    f_data = (float32_t)u32_data;
                }
            }
        }
        else
        {
            return SERVICE_PARAM_ERROR;
        }
        
        
        SERVICE_EncodeFloat32(&f_data, p_params->p_buff_out);
        
    }
    else
    {
        /* Set the total block */
        *p_params->p_total_block = 1;
        
        /* Get EEPROM info */
        p_memory_config = SERVICE_GetMemoryConfigPointer(DataID);
            
        if (p_memory_config != NULL)
        {
            // Else, everything is OK 
            if (SERVICE_CONFIG_Read(
                    p_memory_config->address,
                    p_params->p_buff_out,
                    p_memory_config->length
                ) != SERVICE_CONFIG_OK)
            {
                return SERVICE_EXECUTION_ERROR;
            }
        }
        else
        {
            return SERVICE_PARAM_ERROR;
        }
        
    }
    /* Fixed length 4 bytes: as in Renesas protocol spec */
    *p_params->p_len_out = 4;
    
    /* Successful */
    return SERVICE_OK;
}

/******************************************************************************
* Function Name : SERVICE_WriteCalibrationData
* Interface     : uint8_t SERVICE_WriteCalibrationData(
*               :     uint8_t data_id,
*               :     uint8_t *buff,
*               :     uint8_t len
*               : );
* Description   : Write Calibration Data Service
* Arguments     : uint8_t data_id  : Data ID
*               : uint8_t * buff   : buffer to be written to
*               : uint8_t len      : Expected length to write
* Function Calls: TBD
* Return Value  : uint8_t, success or not
******************************************************************************/
service_result_t SERVICE_WriteCalibrationData(st_service_param *p_params)
{
    /* Variables */
    MEMORY_CONFIG *p_memory_config;
    uint8_t DataID = *p_params->p_id_list;
    uint32_t addr, size;
    uint16_t u16buff;
    uint16_t u16_data;
    int16_t i16_data;
    uint32_t u32_data;
    int32_t i32_data;
    float32_t f_data;
    uint8_t *p_data;
    dsad_gain_t dsad_gain;
    dsad_reg_setting_t regs;
    uint8_t driver_adc_gain0[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    uint8_t driver_adc_gain1[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    uint8_t is_signed = EM_DATA_ID_IS_SIGNED_DATA(DataID);
    uint8_t is_2byte = EM_DATA_ID_IS_2BYTE_DATA(DataID);
    uint8_t is_integer = EM_DATA_ID_IS_INTEGER_DATA(DataID);
    
    /* Params check */
    if (p_params == NULL)
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Check internal params pointer */
    if (    p_params->p_id_list == NULL         || 
            p_params->p_buff_in == NULL            ||
            p_params->p_len_in == NULL             ||
            p_params->p_total_block == NULL     ||
            (0)
        )
    {
        return SERVICE_PARAM_ERROR;
    }
    
    /* Set the total block */
    *p_params->p_total_block = 1;
    
    if (EM_DATA_ID_IS_CLEAR_FLAG(DataID) == 1)
    {
        /* Check with compiler whether this empty page is all 0s or not */
        static const uint8_t    empty_pages[SERVICE_MEM_PAGE_SIZE];
        
        /* Flag to clear counter and record processing */
        switch (DataID)
        {
            case EM_DATA_ID_FLAG_CLEAR_KWH_COUNTER:
                EM_ResetEnergyTotal(SRC_ACTIVE, LINE_PHASE_R);
                EM_ResetEnergyTotal(SRC_ACTIVE, LINE_PHASE_Y);
                EM_ResetEnergyTotal(SRC_ACTIVE, LINE_PHASE_B);
                EM_ResetEnergyTotal(SRC_ACTIVE, LINE_TOTAL);
                
                if (SERVICE_MEM_Backup(SERVICE_MEM_ITEM_SYS_STATE) != STORAGE_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                break;
            case EM_DATA_ID_FLAG_CLEAR_KVARH_COUNTER:
                EM_ResetEnergyTotal(SRC_REACTIVE, LINE_PHASE_R);
                EM_ResetEnergyTotal(SRC_REACTIVE, LINE_PHASE_Y);
                EM_ResetEnergyTotal(SRC_REACTIVE, LINE_PHASE_B);
                EM_ResetEnergyTotal(SRC_REACTIVE, LINE_TOTAL);
                
                if (SERVICE_MEM_Backup(SERVICE_MEM_ITEM_SYS_STATE) != STORAGE_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                break;
            case EM_DATA_ID_FLAG_CLEAR_KVAH_COUNTER:
                EM_ResetEnergyTotal(SRC_APPARENT, LINE_PHASE_R);
                EM_ResetEnergyTotal(SRC_APPARENT, LINE_PHASE_Y);
                EM_ResetEnergyTotal(SRC_APPARENT, LINE_PHASE_B);
                EM_ResetEnergyTotal(SRC_APPARENT, LINE_TOTAL);
                
                if (SERVICE_MEM_Backup(SERVICE_MEM_ITEM_SYS_STATE) != STORAGE_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                break;
            case EM_DATA_ID_FLAG_CLEAR_KWH_MAX_DEMAND_COUNTER:
                EM_ResetMaxDemand(SRC_ACTIVE, LINE_PHASE_R);
                EM_ResetMaxDemand(SRC_ACTIVE, LINE_PHASE_Y);
                EM_ResetMaxDemand(SRC_ACTIVE, LINE_PHASE_B);
                EM_ResetMaxDemand(SRC_ACTIVE, LINE_TOTAL);
                
                if (SERVICE_MEM_Backup(SERVICE_MEM_ITEM_SYS_STATE) != STORAGE_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                break;
            case EM_DATA_ID_FLAG_CLEAR_KVARH_MAX_DEMAND_COUNTER:
                EM_ResetMaxDemand(SRC_REACTIVE, LINE_PHASE_R);
                EM_ResetMaxDemand(SRC_REACTIVE, LINE_PHASE_Y);
                EM_ResetMaxDemand(SRC_REACTIVE, LINE_PHASE_B);
                EM_ResetMaxDemand(SRC_REACTIVE, LINE_TOTAL);
                
                if (SERVICE_MEM_Backup(SERVICE_MEM_ITEM_SYS_STATE) != STORAGE_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                break;
            case EM_DATA_ID_FLAG_CLEAR_KVAH_MAX_DEMAND_COUNTER:
                EM_ResetMaxDemand(SRC_APPARENT, LINE_PHASE_R);
                EM_ResetMaxDemand(SRC_APPARENT, LINE_PHASE_Y);
                EM_ResetMaxDemand(SRC_APPARENT, LINE_PHASE_B);
                EM_ResetMaxDemand(SRC_APPARENT, LINE_TOTAL);
                
                if (SERVICE_MEM_Backup(SERVICE_MEM_ITEM_SYS_STATE) != STORAGE_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                break;
            case EM_DATA_ID_FLAG_CLEAR_ALL_TAMPER_RECORDS:
/*                
                addr = STORAGE_EEPROM_TAMPER_DATA_LOG_ADDR;
                size = STORAGE_EEPROM_TAMPER_DATA_LOG_SIZE;
                while (size > 0)
                {
                    if (size >= SERVICE_MEM_PAGE_SIZE)
                    {
                        u16buff = SERVICE_MEM_PAGE_SIZE;
                        size -= SERVICE_MEM_PAGE_SIZE;
                    }
                    else
                    {
                        u16buff = size;
                        size = 0;
                    }
                    
                    SERVICE_MEM_Write(addr, (uint8_t *)empty_pages, u16buff);
                    addr += u16buff;
                }
*/                
                break;
            case EM_DATA_ID_FLAG_CLEAR_ALL_ENERGY_RECORDS:
/*
				addr = STORAGE_EEPROM_ENERGY_DATA_LOG_ADDR;
                size = STORAGE_EEPROM_ENERGY_DATA_LOG_SIZE;
                while (size > 0)
                {
                    if (size >= SERVICE_MEM_PAGE_SIZE)
                    {
                        u16buff = SERVICE_MEM_PAGE_SIZE;
                        size -= SERVICE_MEM_PAGE_SIZE;
                    }
                    else
                    {
                        u16buff = size;
                        size = 0;
                    }
                    
                    SERVICE_MEM_Write(addr, (uint8_t *)empty_pages, u16buff);
                    addr += u16buff;
                }
*/
				break;
				
            default:
                return SERVICE_PARAM_ERROR;
        }
    }
    else
    {
        if (is_integer == 1)
        {
            /* Get EEPROM info */
            p_memory_config = SERVICE_GetMemoryConfigPointer(DataID);
                
            SERVICE_DecodeFloat32(&f_data, p_params->p_buff_in);
            
            if (is_2byte)
            {
                if (is_signed)
                {
                    i16_data = (int16_t)f_data;
                    p_data = (uint8_t *)&i16_data;
                }
                else
                {
                    u16_data = (uint16_t)f_data;
                    p_data = (uint8_t *)&u16_data;
                }
            }
            else
            {
                if (is_signed)
                {
                    i32_data = (int16_t)f_data;
                    p_data = (uint8_t *)&i32_data;
                }
                else
                {
                    u32_data = (uint16_t)f_data;
                    p_data = (uint8_t *)&u32_data;
                }
            }
            
            if (p_memory_config != NULL)
            {
                // Else, everything is OK 
                if (SERVICE_CONFIG_Write(
                        p_memory_config->address,
                        p_data,
                        p_memory_config->length
                    ) != SERVICE_CONFIG_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
            }
            else
            {
                return SERVICE_PARAM_ERROR;
            }
        }
        else
        {
            /* Read normal, Get from Memory info */
            p_memory_config = SERVICE_GetMemoryConfigPointer(DataID);
            
            if (p_memory_config != NULL)
            {
                /* Length check */
                if (p_memory_config->length != *p_params->p_len_in)
                {
                    return SERVICE_PARAM_ERROR;
                }
                
                /* Else, everything is OK */
                if (SERVICE_CONFIG_Write(
                        p_memory_config->address,
                        p_params->p_buff_in,
                        p_memory_config->length
                    ) != SERVICE_CONFIG_OK)
                {
                    return SERVICE_EXECUTION_ERROR;
                }
                
                /* After set the software gain, need also set the ADC Gain for operation without gain switch (1 gain)
                */
                if (EM_DATA_ID_IS_SET_GAIN(DataID) == 1)
                {
                    /* Get the gain value in integer */
                    SERVICE_DecodeFloat32(&f_data, p_params->p_buff_in);
                    u16_data = (uint16_t)f_data;
                    dsad_gain = R_DSADC_GetGainEnumValue((uint8_t)u16_data);
                    
                    switch (DataID)
                    {
                        case EM_DATA_ID_P1_PHASE_GAIN_LEVEL0:
                            R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R, dsad_gain);
                            break;
                        case EM_DATA_ID_P2_PHASE_GAIN_LEVEL0:
                            R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y, dsad_gain);
                            break;
                        case EM_DATA_ID_P3_PHASE_GAIN_LEVEL0: 
                            R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B, dsad_gain);
                            break;
                        default:
                            break;
                    }
                    
                    R_DSADC_GetGainAndPhase(&regs);
                    
                    driver_adc_gain0[0] = regs.gain0;
                    driver_adc_gain1[0] = regs.gain1;
                    
                    /* Write to calibration storage memory */
                    if (SERVICE_CONFIG_Write(
                            CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_ADDR,
                            (uint8_t *)driver_adc_gain0,
                            CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE   
                        ) != SERVICE_CONFIG_OK)
                    {
                        return SERVICE_EXECUTION_ERROR;
                    }
                    if (SERVICE_CONFIG_Write(
                            CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_ADDR,
                            (uint8_t *)driver_adc_gain1,
                            CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_SIZE   
                        ) != SERVICE_CONFIG_OK)
                    {
                        return SERVICE_EXECUTION_ERROR;
                    }
                    
                }
            }
            else
            {
                return SERVICE_PARAM_ERROR;
            }
        }
        
        /* Update the Storage memory CRC word for checking when restore again */
        SERVICE_UpdateCRC();
        
        /* Stop before writing new calibration to system data */
        if (EM_Stop() != EM_OK)
        {
            return SERVICE_EXECUTION_ERROR;
        }
        
        /* Get new calibration data from storage memory */
        if (SERVICE_CONFIG_Restore(SERVICE_CONFIG_ITEM_CALIB) != SERVICE_CONFIG_OK)
        {
            return SERVICE_EXECUTION_ERROR;
        }
        
        /* Restart EM */
        if (EM_Start() != EM_OK)
        {
            return SERVICE_EXECUTION_ERROR;
        }
        
    }
    /* Successful */
    return SERVICE_OK;
}
