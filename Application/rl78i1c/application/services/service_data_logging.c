/***********************************************************************************************************************
* File Name    : service_data_logging.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Meter Data Logging Service
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_cgc.h"           /* CG CGC Driver */
#include "r_cg_wdt.h"           /* CG WDT Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */
#include "r_cg_dsadc.h"         /* CG DSADC Driver */
#include "r_cg_adc.h"           /* CG ADC Driver */

/* Global */
#include "typedef.h"            /* GSCE Typedef */

/* Middleware */
#include "em_core.h"            /* EM Core Library */

/* Wrapper */
#include "wrp_mcu.h"            /* MCU Wrapper */
#include "wrp_em_adc.h"
#include "wrp_em_sw_config.h"   /* SW Config Wrapper */
#include "wrp_em_rtc.h"
#include "wrp_em_timer.h"

/* Application */
#include "format.h"             /* EEPROM Format */
#include "eeprom.h"             /* EEPROM Driver */
#include "storage.h"            /* EEPROM Storage */
#include "service.h"            /* Services */
#include <string.h>             /* String library */
#include <math.h>               /* Math library */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define RAW_DATA_FIRST_HALF                             (0x00)         /* first half buffer filled */
#define RAW_DATA_SECOND_HALF                            (0x01)         /* second half buffer filled */
#define RAW_DATA_UNFILLED                               (0xFF)          /* buffer un-filled state */

/* Debug */
#define RAW_DATA_DEBUG_TIME_PORT_ON                     {;}//{P5.0 = 1;}    /* Debug signal ON */
#define RAW_DATA_DEBUG_TIME_PORT_OFF                    {;}//{P5.0 = 0;}    /* Debug signal ON */
#define RAW_DATA_DEBUG_EEPROM_TIME                      (0)         /* Enable/Disable debug time for EEPROM storage */
#define RAW_DATA_DEBUG_ADC_SAMPLING_TIME                (0)         /* Enable/Disable debug time for ADC sampling */
#define RAW_DATA_DEBUG_MEASUREMENT_SAMPLING_TIME        (0)         /* Enable/Disable debug time for measurements sampling */

#if (RAW_DATA_STORE_IN_EEPROM == 1)
    #define RAW_DATA_STORAGE_START_ADDR                 (RAW_DATA_EEPROM_BUFF_ADDR)
    #define RAW_DATA_STORAGE_MAX_SIZE                   ( EPR_DEVICE_SIZE - RAW_DATA_EEPROM_BUFF_ADDR - (RAW_DATA_HALF_BUFFER_PAGE_SIZE * RAW_DATA_MAX_CHANNEL)) /* Compensate page rounding to prevent overflow */

#else
    #define RAW_DATA_STORAGE_START_ADDR                 (&g_raw_data_buffer[0])
    #define RAW_DATA_STORAGE_MAX_SIZE                   (RAW_DATA_MAX_CHANNEL * RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH * 2 * 4)

#endif

#define RAW_DATA_STORAGE_MAX_SAMPLE                     (((RAW_DATA_STORAGE_MAX_SIZE / 4) / RAW_DATA_ADC_SAMPLE_PER_BLOCK)  * RAW_DATA_ADC_SAMPLE_PER_BLOCK)

/* Check default number of sample of user setting */
#if ((RAW_DATA_ADC_REAL_NUMBER_SAMPLE*RAW_DATA_MAX_CHANNEL) > RAW_DATA_STORAGE_MAX_SAMPLE)
    #error "Check the maximum size allowed for RAW_DATA_ADC_REAL_NUMBER_SAMPLE setting"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
/* Export for accessing in wrp_em_adc */
#if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
EM_SW_SAMP_TYPE g_datalog_before_integrate_phase_r_i;
EM_SW_SAMP_TYPE g_datalog_before_integrate_phase_y_i;
EM_SW_SAMP_TYPE g_datalog_before_integrate_phase_b_i;
EM_SW_SAMP_TYPE g_datalog_before_integrate_neutral_i;
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
raw_data_channel_t          g_raw_data_channel;
raw_data_service_info_t     g_raw_data_service_info;

/* Use int32_t as buffer type to make even alignment */
int32_t g_raw_data_buffer[RAW_DATA_MAX_CHANNEL * RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH * 2];
/* Local samples reading storage */
static EM_SAMPLES  *         g_raw_data_samples_ptr;

/* fs divider to reduce sampling frequency */
static uint8_t g_adc_integration_stable = 0;
static uint8_t g_adc_fs_divider = 1;
static uint8_t g_adc_fs_divider_counter = 1;
static uint8_t g_measurement_fs_divider_counter = 1;


/***********************************************************************************************************************
* Function Name : SERVICE_LookupChannelID
* Interface     : static int8_t SERVICE_LookupChannelID(int8_t data_id)
* Description   : 
* Arguments     : int8_t data_id:
* Function Calls: 
* Return Value  : static int8_t
***********************************************************************************************************************/
static int8_t SERVICE_LookupChannelID(int8_t data_id)
{
    int8_t result;
    
    switch (data_id)
    {
        case RAW_DATA_ID_NO_SELECT:
        case RAW_DATA_ID_P1_NO_SELECT:
        case RAW_DATA_ID_P2_NO_SELECT:
        case RAW_DATA_ID_P3_NO_SELECT:
            result = -1;
            break;
            
        case RAW_DATA_ID_P1_VOLTAGE_SAMPLES:
            result = EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R;
            break;
        
        case RAW_DATA_ID_P2_VOLTAGE_SAMPLES:
            result = EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y;
            break;          

        case RAW_DATA_ID_P3_VOLTAGE_SAMPLES:
            result = EM_ADC_VOL_DRIVER_CHANNEL_PHASE_B;
            break;
            
        case RAW_DATA_ID_P1_CURRENT_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R;
            break;

        case RAW_DATA_ID_P2_CURRENT_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y;
            break;

        case RAW_DATA_ID_P3_CURRENT_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B;
            break;
            
        case RAW_DATA_ID_NEUTRALCURRENT_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL;
            break;

        case RAW_DATA_ID_P1_CURRENT_INTEGRATED_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R | RAW_DATA_ID_INTEGRATOR_USED_MASK;
            break;

        case RAW_DATA_ID_P2_CURRENT_INTEGRATED_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y | RAW_DATA_ID_INTEGRATOR_USED_MASK;
            break;      

        case RAW_DATA_ID_P3_CURRENT_INTEGRATED_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B | RAW_DATA_ID_INTEGRATOR_USED_MASK;
            break;          
            
        case RAW_DATA_ID_NEUTRALCURRENT_INTEGRATED_SAMPLES:
            result = EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL | RAW_DATA_ID_INTEGRATOR_USED_MASK;
            break;
            
        /* Other -> keep as input */
        default:
            result = data_id;
            break;
    }
    
    return (result);
}

/***********************************************************************************************************************
* Function Name : SERVICE_LookupGroup
* Interface     : static int8_t SERVICE_LookupGroup(uint8_t *data_ids)
* Description   : 
* Arguments     : uint8_t * data_ids:
* Function Calls: 
* Return Value  : static int8_t
***********************************************************************************************************************/
static int8_t SERVICE_LookupGroup(uint8_t *data_ids)
{
    uint8_t i;
    
    /* Only check first ID, since all must be the same */
    if ( ((*(data_ids+0) & 0x0F) >= 0x01) &&
         ((*(data_ids+0) & 0x0F) <= 0x05) )
    {
        return RAW_DATA_GROUP_ADC_WAVEFORM;
    }
    else if ( ((*(data_ids+0) & 0x0F) >= 0x06) &&
              ((*(data_ids+0) & 0x0F) <= 0x0E) )
    {
        return RAW_DATA_GROUP_MEASUREMENT;
    }

    /* Not found */
    return (-1);
}


/***********************************************************************************************************************
* Function Name : SERVICE_GetADCRawData
* Interface     : static int32_t SERVICE_GetADCRawData(int8_t channel)
* Description   : Get ADC Raw Data based on selected channel ID
* Arguments     : int8_t channel: Selected channel ID
* Function Calls: TBD
* Return Value  : static int32_t
***********************************************************************************************************************/
static int32_t SERVICE_GetADCRawData(int8_t channel, uint8_t is_voltage)
{
    int32_t result;
    uint8_t integrator_mask;
    
    /* Current and voltage is in different modules: DSADC and ADC 
     * Reading therefore will be different based on the channel_id
     * When calling here, the channel is already parsed to ID in wrp_em_sw_config.h
     * The ID of voltage and current may be the same. So is_voltage variable is needed to determine voltage or current
    */
    if (is_voltage == 1)
    {
        /* Voltage ID */
        if (channel == EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R)
        {
            result = g_raw_data_samples_ptr->phase_r.v;
        }
        else if ( channel == EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y)
        {
            result = g_raw_data_samples_ptr->phase_y.v;
        }
        else
        {
            result = g_raw_data_samples_ptr->phase_b.v;
        }
    }
    else
    {
        /* Current ID */
        integrator_mask = channel & 0xF0;
        channel = channel & 0x0F;
        
        if (channel == EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R)
        {
            result = g_raw_data_samples_ptr->phase_r.i;
            
            #if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
            if (integrator_mask)
            {
                result = g_datalog_before_integrate_phase_r_i;
            }
            #endif
        }
        else if ( channel == EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y)
        {
            result = g_raw_data_samples_ptr->phase_y.i;
                
            #if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
            if (integrator_mask)
            {
                result = g_datalog_before_integrate_phase_y_i;
            }
            #endif
        }
        else if ( channel == EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B)
        {
            result = g_raw_data_samples_ptr->phase_b.i;
            
            #if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
            if (integrator_mask)
            {
                result = g_datalog_before_integrate_phase_b_i;
            }
            #endif
        }
        else
        {
            result = g_raw_data_samples_ptr->neutral.i;
            #if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
            if (integrator_mask)
            {
                result = g_datalog_before_integrate_neutral_i;
            }
            #endif
        }
    }

#if (SERVICE_USE_DUMMY_DATA == 1)
    /* Avoid 0 value to see wrong value if any */
    result = channel+1;
    
    if (is_voltage == 0)
    {
        if (integrator_mask)
        {
            result = channel+8;
        }
    }
    else
    {
        /* Increase 4 to see difference between DSAD and SAR */
        result = channel+4;
    }
    
#endif

    return (result);
}

/***********************************************************************************************************************
* Function Name : SERVICE_DoRawDataLogging
* Interface     : service_result_t SERVICE_DoRawDataLogging(st_service_param *p_params)
* Description   : Raw Data Logging Service
* Arguments     : uint8_t *data_id_list  : 3 Data Id of request
*               : uint8_t current_data_id: Current selected data id
*               : uint16_t block_id      : Block ID
*               : uint16_t total_block   : Total block
*               : uint8_t *buff          : Pointer buffer for raw logged data
*               : uint8_t *len_out       : Store the length of output buffer
* Function Calls: TBD
* Return Value  : uint8_t, success or not
***********************************************************************************************************************/
service_result_t SERVICE_DoRawDataLogging(st_service_param *p_params)
{
    static uint16_t s_total_block;
    static uint8_t s_active_ids;
    
    uint8_t     i;
    uint8_t     is_data_id_valid = 1;
    uint32_t    addr_start;             /* Start address of each time readout from external memory */
    uint32_t    addr_end;               /* End address of each time readout from external memory */
    uint32_t    addr_limit;             /* Limit address accessed from external memory */
    uint32_t    addr_limit_user;        /* User setting of limit address, always smaller than addr_limit */         
    uint16_t    length;                 /* Length of data to read from storage area */
    uint16_t    encoded_length = 0;     
    uint32_t    time_out;               /* Timeout for data acquisition */
    uint16_t    req_samples;
    
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
    
    /* In case GUI not send the number of samples down, default will get from header */
    /* First block? */
    if (p_params->block_id == 1)
    {
        /* Reset request */
        g_raw_data_service_info.is_enable = 0;
        g_raw_data_service_info.is_requested = 1;
        g_raw_data_service_info.buff_total_sample = 0;
        g_raw_data_service_info.buff_total_saved_sample = 0;
        g_raw_data_service_info.buff_count = 0;
        g_raw_data_service_info.buff_filled_state = RAW_DATA_UNFILLED;
        
        g_raw_data_service_info.is_adc_or_measurement = SERVICE_LookupGroup(p_params->p_id_list);
        
        /* Get number of requested samples, if no, get default number of sample */
        if (*p_params->p_req_samples == 0)
        {
            if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
            {
                *p_params->p_req_samples = 
                req_samples = RAW_DATA_ADC_REAL_NUMBER_SAMPLE;
            }
        }
        else
        {
            req_samples = *p_params->p_req_samples;
            
            /* Make sure requested samples is multiple of 50 */
            if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
            {
                req_samples = (req_samples / RAW_DATA_ADC_SAMPLE_PER_BLOCK ) * RAW_DATA_ADC_SAMPLE_PER_BLOCK;
            }
        }
        
        /* For Measurement: fix the number of measurement due to large time needed */
        if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_MEASUREMENT)
        {
            *p_params->p_req_samples = 
            req_samples = RAW_DATA_MEASUREMENT_REAL_NUMBER_SAMPLE;
        }
        
        /* Calculate the sample needed */
        if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
        {
            g_raw_data_service_info.buff_num_of_sample_need = ((req_samples / RAW_DATA_HALF_BUFFER_PAGE_SIZE) +
                                                                (((req_samples % RAW_DATA_HALF_BUFFER_PAGE_SIZE) == 0) ? 0 : 1));
            
            g_raw_data_service_info.buff_num_of_sample_need *= RAW_DATA_HALF_BUFFER_PAGE_SIZE;
        }
        else
        {
            /* Fixed at 10 samples */
            g_raw_data_service_info.buff_num_of_sample_need = req_samples;
        }
        
        /* The buffer roll-over point 
         * If data in RAM: the point will be half of sample need
         * If data in EPR: 
         *      + the point will be half of double buffer length when doing ADC Raw Data Log
         *      + the point will be half of sample need when doing Measurement Log
        */
        #if (RAW_DATA_STORE_IN_EEPROM == 0)
            g_raw_data_service_info.buff_rollover_point = g_raw_data_service_info.buff_num_of_sample_need / 2;
        #else
            if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
            {
                g_raw_data_service_info.buff_rollover_point = RAW_DATA_HALF_DOUBLE_BUFFER_LENGTH;
            }
            else
            {
                g_raw_data_service_info.buff_rollover_point = g_raw_data_service_info.buff_num_of_sample_need / 2;
            }
        #endif
        
        /* Calculating the total block and number of active ids */
        
        /* Reset first */
        s_total_block = 0;
        s_active_ids =0;

        for (i=0; i<RAW_DATA_MAX_CHANNEL ;i++)
        {
            /* Lookup ID */
            g_raw_data_channel.channel[i].lookup_id = SERVICE_LookupChannelID((int8_t)(*(p_params->p_id_list+i)));
     
            if (g_raw_data_channel.channel[i].lookup_id != -1)
            {
                if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
                {
                    g_raw_data_channel.channel[i].is_voltage_id = RAW_DATA_ID_IS_VOLTAGE_ID(*(p_params->p_id_list+i));
                    
                    if (*(p_params->p_id_list+i) != 0)
                    {
                        s_active_ids++;
                        
                        s_total_block += (req_samples / RAW_DATA_ADC_SAMPLE_PER_BLOCK);
                    }
                }
                else if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_MEASUREMENT)
                {  
                    if (*(p_params->p_id_list+i) != 0)
                    {
                        s_active_ids++;
                        
                        s_total_block += (req_samples / RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK);
                    }
                }
            }
        }
                
        /* Find total */ 
        *(p_params->p_block_per_id) = s_total_block / s_active_ids;
        *(p_params->p_total_block) = s_total_block;
        
        /* Check support capability for RAW_DATA_ADC number of sample */
        /* Each sample is 4 byte, if req_samples * active_ids * 4 > RAW_DATA_STORAGE_MAX_SIZE */
        if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
        {
            if ((req_samples * s_active_ids) > RAW_DATA_STORAGE_MAX_SAMPLE)
            {
                
                uint16_t max_samples;
                max_samples = RAW_DATA_STORAGE_MAX_SAMPLE / s_active_ids;
                
                /* Max samples in multiple of 50 */
                max_samples = (max_samples / RAW_DATA_ADC_SAMPLE_PER_BLOCK) *RAW_DATA_ADC_SAMPLE_PER_BLOCK;
                                
                *p_params->p_total_block = 1;
                *p_params->p_buff_out++ = 0x00;
                *p_params->p_buff_out++ = (uint8_t)(max_samples & 0x00FF);
                *p_params->p_buff_out++ = (uint8_t)(max_samples >> 8);
                *p_params->p_len_out = 3;
                return SERVICE_REQUEST_SIZE_ERROR;
            }
        }
        
        /* Lookup eeprom start address and offset for each dataID */
        for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
        {
            /* Set default address offset for eeprom write */
            g_raw_data_channel.channel[i].storage_start_addr = 0;
            
            /* Lookup the size of one sample*/
            if ( g_raw_data_channel.channel[i].lookup_id != -1 ) 
            {
                // Energy value: 64bit, else 32bit
                if ( RAW_DATA_ID_IS_ENERGY_DATA((*(p_params->p_id_list + i))) )
                {
                    g_raw_data_channel.channel[i].one_sample_size = sizeof(double64_t);
                }
                else
                {
                    g_raw_data_channel.channel[i].one_sample_size = sizeof(float32_t);
                }
            }
            else
            {
                g_raw_data_channel.channel[i].one_sample_size = 0;
            }
            
            
            /* Lookup the size of half buffer */
            g_raw_data_channel.channel[i].half_buffer_size = (uint16_t)g_raw_data_channel.channel[i].one_sample_size * 
                                                            g_raw_data_service_info.buff_rollover_point;

            /* Lookup the eeprom start address  */
            if (i == 0)
            {
                /* First one start at predefined address */
                g_raw_data_channel.channel[i].storage_start_addr = (uint32_t)RAW_DATA_STORAGE_START_ADDR;
                g_raw_data_channel.channel[i].p_first_half = &g_raw_data_buffer[0];
            }
            else
            {
                /* Other start next to end eeprom address of previous channel */
                g_raw_data_channel.channel[i].storage_start_addr = g_raw_data_channel.channel[i-1].storage_start_addr +
                                                            (uint32_t)((uint32_t)g_raw_data_channel.channel[i-1].one_sample_size *
                                                             (uint32_t)g_raw_data_service_info.buff_num_of_sample_need);
                                                             
                g_raw_data_channel.channel[i].p_first_half = g_raw_data_channel.channel[i-1].p_first_half + 
                                                            ((g_raw_data_channel.channel[i-1].half_buffer_size/4)*2);
            }
            
            /* Assign pointer to raw data log buffer */
            g_raw_data_channel.channel[i].p_second_half = (g_raw_data_channel.channel[i].p_first_half + 
                                                        (g_raw_data_channel.channel[i].half_buffer_size / 4));
            
            g_raw_data_channel.channel[i].storage_addr_offset = 0;
        }
        
        if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
        {
            uint16_t sampling_rate;         /* Sampling rate information  */
            
            g_adc_integration_stable = 0;
            g_adc_fs_divider = 0;
            
            for(i=0; i< RAW_DATA_MAX_CHANNEL; i++)
            {
                if (*(p_params->p_id_list+i) != RAW_DATA_ID_NO_SELECT)
                {
                    if (g_adc_fs_divider < RAW_DATA_ADC_FS_MAX )
                    {
                        g_adc_fs_divider++;
                    }
                }
            }
            
            /* Stop EM RTC and TIMER to reduce CPU load */
            
            EM_TIMER_Stop();
            EM_RTC_Stop();
            SWITCH_DATALOGGING_CLOCK();
            
            /* Enable interrupt for buffer filling */
            g_raw_data_service_info.is_enable = 1;
            
            /* Raw ADC will get all sample at a time */
            /* wait until half of specified samples are filled into EEPROM */
            time_out = 5000000;
            while (g_raw_data_service_info.buff_total_saved_sample <
                   g_raw_data_service_info.buff_num_of_sample_need)
            {
                SERVICE_RawDataLogPollingProcess();
                
                /* Check to cancel when overrun happen */
                time_out--;
                if (time_out == 0)
                {
                    SERVICE_RawDataLogCancelRequest();
                    return SERVICE_EXECUTION_ERROR;
                }
            }
            
            /* Restart EM RTC and TIMER */
            EM_TIMER_Start();
            EM_RTC_Start();
            SWITCH_NORMAL_OPERATING_CLOCK();
            
            /* Format the logging characteristics, 48 bytes */
            memset(p_params->p_buff_out, 0, RAW_DATA_ADC_SAMPLING_INFO_BUFF_SIZE);
            sampling_rate = RAW_DATA_ADC_ORIGINAL_SAMPLING_RATE / g_adc_fs_divider;
            
            /* Sampling rate: 2 bytes */
            *(p_params->p_buff_out +  0) = (uint8_t)(sampling_rate);
            *(p_params->p_buff_out +  1) = (uint8_t)(sampling_rate >> 8);
            
            /* ADC Voltage, I1, I2 Channel ID */
            *(p_params->p_buff_out +  2) = (uint8_t)(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_R);
            *(p_params->p_buff_out +  3) = (uint8_t)(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_Y);
            *(p_params->p_buff_out +  4) = (uint8_t)(EM_ADC_VOL_DRIVER_CHANNEL_PHASE_B);
            
            *(p_params->p_buff_out +  5) = (uint8_t)(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R);
            *(p_params->p_buff_out +  6) = (uint8_t)(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y);
            *(p_params->p_buff_out +  7) = (uint8_t)(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B);
            *(p_params->p_buff_out +  8) = (uint8_t)(EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL);
            
            /* DSADGCR0, DSADGCR1 - Gain setting */
            *(p_params->p_buff_out +  15) = (uint8_t)(R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R));
            *(p_params->p_buff_out +  17) = (uint8_t)(R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y));
            *(p_params->p_buff_out +  19) = (uint8_t)(R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B));
            *(p_params->p_buff_out +  21) = (uint8_t)(R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL));
            
            /* The rest not filled in */
            
            p_params->p_buff_out += RAW_DATA_ADC_SAMPLING_INFO_BUFF_SIZE;
            encoded_length += RAW_DATA_ADC_SAMPLING_INFO_BUFF_SIZE;
        }
        else if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_MEASUREMENT)
        {
            /* Enable interrupt for buffer filling */
            g_raw_data_service_info.is_enable = 1;
            
                
            /* Raw Measurement:
             * Due to long capture time: 1sample/sec, the capture will split into block
             * The capture will be right before reading eeprom
             * wait until enough sample are filled into EEPROM below 
            */
            
        }
    }
    
    /* From second block, the number of sample already set by service, get back for further process */
    req_samples = *p_params->p_req_samples;
    
    /* Encode data response id */
    *p_params->p_buff_out++ = p_params->current_id;
    encoded_length += 1;
    
    /* Lookup address for eeprom reading */
    if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
    {       
        /* Lookup the EEPROM data */
        for (i=0; i<RAW_DATA_MAX_CHANNEL; i++)
        {
            if( p_params->current_id == *(p_params->p_id_list+i))
            {
                if (g_raw_data_channel.channel[i].lookup_id == -1)
                {
                }
                else
                {                    
                    addr_start = g_raw_data_channel.channel[i].storage_start_addr;
                    addr_limit = addr_start +
                                (g_raw_data_service_info.buff_num_of_sample_need * g_raw_data_channel.channel[i].one_sample_size);
                    addr_limit_user = addr_start +
                                (req_samples * g_raw_data_channel.channel[i].one_sample_size);
                    addr_start += ((p_params->block_id - 1) % *p_params->p_block_per_id) * RAW_DATA_ADC_BLOCK_SIZE;
                    addr_end    = addr_start + 
                                (RAW_DATA_ADC_SAMPLE_PER_BLOCK * g_raw_data_channel.channel[i].one_sample_size);        
                    length = (RAW_DATA_ADC_SAMPLE_PER_BLOCK * g_raw_data_channel.channel[i].one_sample_size);
                    break;
                }
            }
        }
    }
    else if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_MEASUREMENT)
    {
        /* Capture the measurement data */
        uint16_t prev_total_saved_sample = g_raw_data_service_info.buff_total_saved_sample;
        
        time_out = 5000000;
        
        while ( (g_raw_data_service_info.buff_total_saved_sample ==
                 prev_total_saved_sample) &&
                 (g_raw_data_service_info.buff_total_saved_sample <
                 g_raw_data_service_info.buff_num_of_sample_need)
              )
        {
            SERVICE_RawDataLogPollingProcess();
            
            /* Check to cancel when overrun happen */
            time_out--;
            if (time_out == 0)
            {
                SERVICE_RawDataLogCancelRequest();
                return SERVICE_EXECUTION_ERROR;
            }
        }
        
        /* Lookup the EEPROM data */
        for (i=0; i<RAW_DATA_MAX_CHANNEL; i++)
        {
            if( p_params->current_id == *(p_params->p_id_list+i))
            {
                if (g_raw_data_channel.channel[i].lookup_id == -1)
                {
                    
                }
                else
                {
                    addr_start = g_raw_data_channel.channel[i].storage_start_addr;
                    addr_limit = addr_start +
                                (g_raw_data_service_info.buff_num_of_sample_need * (uint16_t)g_raw_data_channel.channel[i].one_sample_size);
                    addr_limit_user = addr_start +
                                (req_samples * g_raw_data_channel.channel[i].one_sample_size);
                    addr_start += ((p_params->block_id - 1) % *p_params->p_block_per_id) * 
                                    (RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK * 
                                        g_raw_data_channel.channel[i].one_sample_size);
                    addr_end    = addr_start + 
                                (RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK * g_raw_data_channel.channel[i].one_sample_size);
                    length = (RAW_DATA_MEASUREMENT_SAMPLE_PER_BLOCK * g_raw_data_channel.channel[i].one_sample_size);           
                    break;
                }
            }
        }
    }
    
    /*
     * Check the supportable of EEPROM then read out data from eeprom
     */
    if ((is_data_id_valid == 1 &&
        addr_start < addr_limit_user) ||
        addr_end < addr_limit)
    {
        RAW_DATA_Read(  addr_start,
                        p_params->p_buff_out,
                        length
                      );
        /* If it outside required range of user setting, clear to 0 */
        if (addr_end > addr_limit_user)
        {
            memset((p_params->p_buff_out+(addr_limit_user-addr_start)), 0, 
                            (uint16_t)(addr_end - addr_limit_user));    
        }
    }
    else
    {
        /* Dummy fill 0 for remain */
        memset(p_params->p_buff_out, 0, length);
    }
    encoded_length += length;
    
    /* Successful */
    *p_params->p_len_out = (uint8_t)encoded_length;
    
    /* Successful */
    return SERVICE_OK;
}

/***********************************************************************************************************************
* Function Name : SERVICE_FillADCRawData
* Interface     : void SERVICE_FillADCRawData(void)
* Description   : 
* Arguments     : None:
* Function Calls: 
* Return Value  : None
***********************************************************************************************************************/
void SERVICE_FillADCRawData(__near EM_SAMPLES * p_samples)
{
    /* Variables */
    uint16_t buff_index;
    uint16_t i;
    
    /* Read the samples */
    g_raw_data_samples_ptr = p_samples;
    
    /* Do nothing when not select ADC to capture or buffer full filled */
    if (g_raw_data_service_info.is_enable == 0 ||
        g_raw_data_service_info.is_adc_or_measurement != RAW_DATA_GROUP_ADC_WAVEFORM ||
        g_raw_data_service_info.buff_total_sample >= g_raw_data_service_info.buff_num_of_sample_need)
    {
        return;
    }
    /* fs divider */
    if (g_adc_fs_divider_counter < g_adc_fs_divider)
    {
        g_adc_fs_divider_counter++;
    }
    else
    {
        /* wait the integration stable */
        if (g_adc_integration_stable < RAW_DATA_ADC_INTEGRATOR_STABLE_INT_COUNT)
        {
            /* Read only */
            for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
            {
                SERVICE_GetADCRawData(g_raw_data_channel.channel[i].lookup_id, g_raw_data_channel.channel[i].is_voltage_id);
            }
            /* Break, wait until the integration is stable */
            g_adc_integration_stable++;
            return;
        }
    
        #if (RAW_DATA_DEBUG_ADC_SAMPLING_TIME == 1)
        RAW_DATA_DEBUG_TIME_PORT_ON;
        #endif
        /* Get roll-over index */
        buff_index = g_raw_data_service_info.buff_count % g_raw_data_service_info.buff_rollover_point;

        /* half-first */
        if (g_raw_data_service_info.buff_count < g_raw_data_service_info.buff_rollover_point)
        {
            for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
            {
                if (g_raw_data_channel.channel[i].lookup_id != -1)
                {
                    *(g_raw_data_channel.channel[i].p_first_half + buff_index) = SERVICE_GetADCRawData(
                                                                                                        g_raw_data_channel.channel[i].lookup_id, 
                                                                                                        g_raw_data_channel.channel[i].is_voltage_id
                                                                                                );
                }
            }
        }
        else /* second-half */
        {
            for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
            {
                if (g_raw_data_channel.channel[i].lookup_id != -1)
                {
                    *(g_raw_data_channel.channel[i].p_second_half + buff_index) = SERVICE_GetADCRawData(
                                                                                                        g_raw_data_channel.channel[i].lookup_id, 
                                                                                                        g_raw_data_channel.channel[i].is_voltage_id
                                                                                                    );
                }
            }
        }
        
        /* sample counting */
        g_raw_data_service_info.buff_count++;
        g_raw_data_service_info.buff_total_sample++;
        /*
         * check to raise the full filled status to g_raw_data_buff_filled_state
         * 0 is first-half filled, 1 is second-half filled
         */
         
        if (g_raw_data_service_info.buff_count == g_raw_data_service_info.buff_rollover_point)
        {
            g_raw_data_service_info.buff_filled_state = RAW_DATA_FIRST_HALF;
        }
        else if (g_raw_data_service_info.buff_count >= (g_raw_data_service_info.buff_rollover_point * 2))
        {
            /* marked as buff 1 alreadfy filled */
            g_raw_data_service_info.buff_filled_state = RAW_DATA_SECOND_HALF;
            g_raw_data_service_info.buff_count = 0;
        }
        
        /* next counting of divider */
        g_adc_fs_divider_counter = 1;
        
        #if (RAW_DATA_DEBUG_ADC_SAMPLING_TIME == 1)
        RAW_DATA_DEBUG_TIME_PORT_OFF;
        #endif
    }
}

/***********************************************************************************************************************
* Function Name : SERVICE_GrabBufferFromMeasurement
* Interface     : static void SERVICE_GrabBufferFromMeasurement(uint8_t data_id, void *p_data)
* Description   : 
* Arguments     : uint8_t data_id:
*               : int32_t * p_i32:
* Function Calls: 
* Return Value  : static void
***********************************************************************************************************************/
static void SERVICE_GrabBufferFromMeasurement(uint8_t data_id, void *p_data)
{
    float32_t *p_f32 = (float32_t *)p_data;
    double64_t *p_d64 = (double64_t *)p_data;
    EM_LINE     line;
    
    line = (EM_LINE)RAW_DATA_ID_GET_LINE_INFO(data_id);
    
    switch (data_id)
    {
        case RAW_DATA_ID_P1_VRMS:
        case RAW_DATA_ID_P2_VRMS:
        case RAW_DATA_ID_P3_VRMS:
            
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetVoltageRMS(line);
            #endif
            
            break;

        case RAW_DATA_ID_P1_IRMS:
        case RAW_DATA_ID_P2_IRMS:
        case RAW_DATA_ID_P3_IRMS:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetCurrentRMS(line);
            #endif
            
            break;

        case RAW_DATA_ID_INRMS:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetCurrentRMS(line);
            #endif
            
            break;

        case RAW_DATA_ID_P1_FREQUENCY:
        case RAW_DATA_ID_P2_FREQUENCY:
        case RAW_DATA_ID_P3_FREQUENCY:
        case RAW_DATA_ID_TOTAL_AVERAGE_FREQUENCY:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetLineFrequency(line);
            #endif
            
            break;

        case RAW_DATA_ID_P1_POWER_FACTOR:
        case RAW_DATA_ID_P2_POWER_FACTOR:
        case RAW_DATA_ID_P3_POWER_FACTOR:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetPowerFactor(line);
            #endif
            
            break;

        case RAW_DATA_ID_P1_ACTIVE_POWER:
        case RAW_DATA_ID_P2_ACTIVE_POWER: 
        case RAW_DATA_ID_P3_ACTIVE_POWER:
        case RAW_DATA_ID_TOTAL_ACTIVE_POWER:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetActivePower(line);
            #endif
            
            break;

        case RAW_DATA_ID_P1_ACTIVE_ENERGY:
        case RAW_DATA_ID_P2_ACTIVE_ENERGY:
        case RAW_DATA_ID_P3_ACTIVE_ENERGY:
        case RAW_DATA_ID_TOTAL_ACTIVE_ENERGY:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                SERVICE_FTOD(p_d64, data_id);
            #else
                SERVICE_FTOD(p_d64, (EM_GetActiveEnergyTotal(line)));
            #endif
            
            break;

        case RAW_DATA_ID_P1_APPARENT_POWER:
        case RAW_DATA_ID_P2_APPARENT_POWER:
        case RAW_DATA_ID_P3_APPARENT_POWER:
        case RAW_DATA_ID_TOTAL_APPARENT_POWER:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetApparentPower(line);
            #endif
            
            break;

        case RAW_DATA_ID_P1_APPARENT_ENERGY:
        case RAW_DATA_ID_P2_APPARENT_ENERGY: 
        case RAW_DATA_ID_P3_APPARENT_ENERGY:
        case RAW_DATA_ID_TOTAL_APPARENT_ENERGY:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                SERVICE_FTOD(p_d64, data_id);
            #else
                SERVICE_FTOD(p_d64, (EM_GetApparentEnergyTotal(line)));
            #endif
            
            break;

        case RAW_DATA_ID_P1_REACTIVE_POWER:
        case RAW_DATA_ID_P2_REACTIVE_POWER:
        case RAW_DATA_ID_P3_REACTIVE_POWER:
        case RAW_DATA_ID_TOTAL_REACTIVE_POWER:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = EM_GetReactivePower(line);
            #endif
            
            break;          
            
        case RAW_DATA_ID_P1_REACTIVE_ENERGY:
        case RAW_DATA_ID_P2_REACTIVE_ENERGY:
        case RAW_DATA_ID_P3_REACTIVE_ENERGY:
        case RAW_DATA_ID_TOTAL_REACTIVE_ENERGY: 
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                SERVICE_FTOD(p_d64, data_id);
            #else
                SERVICE_FTOD(p_d64, (EM_GetReactiveEnergyTotal(line)));
            #endif
            
            break;
            
        case RAW_DATA_ID_P1_VOLTAGE_THD:
        case RAW_DATA_ID_P2_VOLTAGE_THD:
        case RAW_DATA_ID_P3_VOLTAGE_THD:
        case RAW_DATA_ID_TOTAL_AVERAGE_VOLTAGE_THD:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = 0.0f; //TODO: Integrate with EM API 
            #endif
            
            break;
            
        case RAW_DATA_ID_P1_CURRENT_THD:
        case RAW_DATA_ID_P2_CURRENT_THD:
        case RAW_DATA_ID_P3_CURRENT_THD:
        case RAW_DATA_ID_TOTAL_AVERAGE_CURRENT_THD:
        
            #if (SERVICE_USE_DUMMY_DATA == 1)
                *(p_f32) = (float32_t)(data_id);
            #else
                *(p_f32) = 0.0f; //TODO: Integrate with EM API 
            #endif
            
        default:
            *p_f32 = 0;
            break;
    }
}

/***********************************************************************************************************************
* Function Name : SERVICE_FillMeasurementRawData
* Interface     : void SERVICE_FillMeasurementRawData(void)
* Description   : 
* Arguments     : None
* Function Calls: 
* Return Value  : None
***********************************************************************************************************************/
void SERVICE_FillMeasurementRawData(void)
{
    /* Variables */
    uint16_t buff_index;
    uint16_t compensate_index;
    uint8_t i;
    /* Do nothing when not select ADC to capture or buffer full filled */
    if (g_raw_data_service_info.is_enable == 0 ||
        g_raw_data_service_info.is_adc_or_measurement != RAW_DATA_GROUP_MEASUREMENT ||
        g_raw_data_service_info.buff_total_sample >= RAW_DATA_MEASUREMENT_REAL_NUMBER_SAMPLE)
    {
        return;
    }
    
    /* fs divider */
    if (g_measurement_fs_divider_counter < RAW_DATA_MEASUREMENT_FS_DIVIDER)
    {
        g_measurement_fs_divider_counter++;
    }
    else
    {
        #if (RAW_DATA_DEBUG_MEASUREMENT_SAMPLING_TIME == 1)
        RAW_DATA_DEBUG_TIME_PORT_ON;
        #endif
        /* Get roll-over index */
        buff_index = g_raw_data_service_info.buff_count % g_raw_data_service_info.buff_rollover_point;

        /* half-first */
        if (g_raw_data_service_info.buff_count < RAW_DATA_MEASUREMENT_HALF_DOUBLE_BUFFER_LENGTH)
        {
            for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
            {
                /* buff 0 - 0 */
                if (g_raw_data_channel.channel[i].lookup_id != -1)
                {
                    
                    if (g_raw_data_channel.channel[i].one_sample_size == 8)
                    {
                        compensate_index = buff_index * 2;
                    }
                    else
                    {
                        compensate_index = buff_index;
                    }
                    
                    SERVICE_GrabBufferFromMeasurement(
                        g_raw_data_channel.channel[i].lookup_id,
                        (g_raw_data_channel.channel[i].p_first_half + compensate_index)
                    );
                }
            }
        }
        else /* second-half */
        {
            for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
            {
                /* buff 0 - 0 */
                if (g_raw_data_channel.channel[i].lookup_id != -1)
                {
                    uint16_t compensate_index;
                    
                    if (g_raw_data_channel.channel[i].one_sample_size == 8)
                    {
                        compensate_index = buff_index * 2;
                    }
                    else
                    {
                        compensate_index = buff_index;
                    }
                    
                    SERVICE_GrabBufferFromMeasurement(
                        g_raw_data_channel.channel[i].lookup_id,
                        (g_raw_data_channel.channel[i].p_second_half + compensate_index)
                    );
                }
            }
        }
        
        /* sample counting */
        g_raw_data_service_info.buff_count++;
        g_raw_data_service_info.buff_total_sample++;
        
        /*
         * check to raise the full filled status to g_raw_data_buff_filled_state
         * 0 is first-half filled, 1 is second-half filled
         */
        if (g_raw_data_service_info.buff_count == g_raw_data_service_info.buff_rollover_point)
        {
            g_raw_data_service_info.buff_filled_state = RAW_DATA_FIRST_HALF;
        }
        else if (g_raw_data_service_info.buff_count >= (g_raw_data_service_info.buff_rollover_point * 2))
        {
            /* marked as buff 1 alreadfy filled */
            g_raw_data_service_info.buff_filled_state = RAW_DATA_SECOND_HALF;
            g_raw_data_service_info.buff_count = 0;
        }
        
        /* next counting of divider */
        g_measurement_fs_divider_counter = 1;
        
        #if (RAW_DATA_DEBUG_MEASUREMENT_SAMPLING_TIME == 1)
        RAW_DATA_DEBUG_TIME_PORT_OFF;
        #endif
    }
}

/***********************************************************************************************************************
* Function Name : SERVICE_RawDataLogPollingProcess
* Interface     : void SERVICE_RawDataLogPollingProcess(void)
* Description   : Polling processing while interrupt collecting raw data or measurements
* Arguments     : None:
* Function Calls: RAW_DATA_Write(),
*               : R_WDT_Restart(),
*
* Return Value  : void
***********************************************************************************************************************/
void SERVICE_RawDataLogPollingProcess(void)
{
    uint16_t i;
    if (g_raw_data_service_info.is_requested == 1)
    {
        R_WDT_Restart();
        if (g_raw_data_service_info.buff_filled_state != RAW_DATA_UNFILLED)
        {
            #if (RAW_DATA_DEBUG_EEPROM_TIME == 1)
            RAW_DATA_DEBUG_TIME_PORT_ON;
            #endif
            
            /* write eeprom for buffer fill */
            if (g_raw_data_service_info.buff_filled_state == RAW_DATA_FIRST_HALF)
            {
                /* Fill EEPROM for each channel */
                #if 1
                for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
                {
                    if (g_raw_data_channel.channel[i].lookup_id != -1)
                    {
                        RAW_DATA_Write(
                                    g_raw_data_channel.channel[i].storage_start_addr + g_raw_data_channel.channel[i].storage_addr_offset,
                                    (uint8_t *)g_raw_data_channel.channel[i].p_first_half,
                                    (uint16_t)g_raw_data_channel.channel[i].half_buffer_size
                                );
                        g_raw_data_channel.channel[i].storage_addr_offset += g_raw_data_channel.channel[i].half_buffer_size;
                        
                    }
                }
                #else
                if (g_raw_data_channel.channel[0].lookup_id != -1)
                {
                    RAW_DATA_Write(
                                g_raw_data_channel.channel[0].storage_start_addr + g_raw_data_channel.channel[0].storage_addr_offset,
                                (uint8_t *)g_raw_data_channel.channel[0].p_first_half,
                                (uint16_t)g_raw_data_channel.channel[0].half_buffer_size
                            );
                    g_raw_data_channel.channel[0].storage_addr_offset += g_raw_data_channel.channel[0].half_buffer_size;
                    
                }
                if (g_raw_data_channel.channel[1].lookup_id != -1)
                {
                    RAW_DATA_Write(
                                g_raw_data_channel.channel[1].storage_start_addr + g_raw_data_channel.channel[1].storage_addr_offset,
                                (uint8_t *)g_raw_data_channel.channel[1].p_first_half,
                                (uint16_t)g_raw_data_channel.channel[1].half_buffer_size
                            );
                    g_raw_data_channel.channel[1].storage_addr_offset += g_raw_data_channel.channel[1].half_buffer_size;
                    
                }
                if (g_raw_data_channel.channel[2].lookup_id != -1)
                {
                    RAW_DATA_Write(
                                g_raw_data_channel.channel[2].storage_start_addr + g_raw_data_channel.channel[2].storage_addr_offset,
                                (uint8_t *)g_raw_data_channel.channel[2].p_first_half,
                                (uint16_t)g_raw_data_channel.channel[2].half_buffer_size
                            );
                    g_raw_data_channel.channel[2].storage_addr_offset += g_raw_data_channel.channel[2].half_buffer_size;
                    
                }
                #endif
            }
            else if (g_raw_data_service_info.buff_filled_state == RAW_DATA_SECOND_HALF)
            {
                #if 1
                /* Fill EEPROM for each channel */
                for (i=0; i< RAW_DATA_MAX_CHANNEL; i++)
                {
                    if (g_raw_data_channel.channel[i].lookup_id != -1)
                    {
                        RAW_DATA_Write(
                                    g_raw_data_channel.channel[i].storage_start_addr + g_raw_data_channel.channel[i].storage_addr_offset,
                                    (uint8_t *)g_raw_data_channel.channel[i].p_second_half,
                                    (uint16_t)g_raw_data_channel.channel[i].half_buffer_size
                                );
                        g_raw_data_channel.channel[i].storage_addr_offset += g_raw_data_channel.channel[i].half_buffer_size;       
                    }
                }
                #else
                
                if (g_raw_data_channel.channel[0].lookup_id != -1)
                {
                    RAW_DATA_Write(
                                g_raw_data_channel.channel[0].storage_start_addr + g_raw_data_channel.channel[0].storage_addr_offset,
                                (uint8_t *)g_raw_data_channel.channel[0].p_second_half,
                                (uint16_t)g_raw_data_channel.channel[0].half_buffer_size
                            );
                    g_raw_data_channel.channel[0].storage_addr_offset += g_raw_data_channel.channel[0].half_buffer_size;       
                }
                if (g_raw_data_channel.channel[1].lookup_id != -1)
                {
                    RAW_DATA_Write(
                                g_raw_data_channel.channel[1].storage_start_addr + g_raw_data_channel.channel[1].storage_addr_offset,
                                (uint8_t *)g_raw_data_channel.channel[1].p_second_half,
                                (uint16_t)g_raw_data_channel.channel[1].half_buffer_size
                            );
                    g_raw_data_channel.channel[1].storage_addr_offset += g_raw_data_channel.channel[1].half_buffer_size;       
                }
                if (g_raw_data_channel.channel[2].lookup_id != -1)
                {
                    RAW_DATA_Write(
                                g_raw_data_channel.channel[2].storage_start_addr + g_raw_data_channel.channel[2].storage_addr_offset,
                                (uint8_t *)g_raw_data_channel.channel[2].p_second_half,
                                (uint16_t)g_raw_data_channel.channel[2].half_buffer_size
                            );
                    g_raw_data_channel.channel[2].storage_addr_offset += g_raw_data_channel.channel[2].half_buffer_size;       
                }
                #endif
            }
            
            /* ACK */
            g_raw_data_service_info.buff_total_saved_sample += g_raw_data_service_info.buff_rollover_point;      
            
            g_raw_data_service_info.buff_filled_state = RAW_DATA_UNFILLED;
            
            #if (RAW_DATA_DEBUG_EEPROM_TIME == 1)
            RAW_DATA_DEBUG_TIME_PORT_OFF;
            #endif
        }
        
    }
}

/***********************************************************************************************************************
* Function Name : SERVICE_RawDataLogCancelRequest
* Interface     : void SERVICE_RawDataLogCancelRequest(void)
* Description   : Cancel current data log request
* Arguments     : None
* Function Calls: 
* Return Value  : None
***********************************************************************************************************************/
void SERVICE_RawDataLogCancelRequest(void)
{
    g_raw_data_service_info.is_enable = 0;
    g_raw_data_service_info.is_requested = 0;
    
    if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_ADC_WAVEFORM)
    {
        if (g_raw_data_service_info.is_core_stopped == 1)
        {
            /* Restart EM RTC and TIMER */
            EM_TIMER_Start();
            EM_RTC_Start();
            SWITCH_NORMAL_OPERATING_CLOCK();
            g_raw_data_service_info.is_core_stopped = 0;
        }
    }
    else if (g_raw_data_service_info.is_adc_or_measurement == RAW_DATA_GROUP_MEASUREMENT)
    {
        /* Do nothing */
    }
    
    g_raw_data_service_info.buff_count = 0;
    g_raw_data_service_info.buff_total_sample = 0;
    g_raw_data_service_info.buff_total_saved_sample = 0;
    g_raw_data_service_info.timeout = 0;
    
    g_raw_data_service_info.is_adc_or_measurement = -1;
}

