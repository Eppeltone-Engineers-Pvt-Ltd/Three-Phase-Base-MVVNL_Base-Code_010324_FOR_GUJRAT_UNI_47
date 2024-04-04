/***********************************************************************************************************************
* File Name    : storage.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Storage Source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"       /* CG Macro Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */
#include "r_cg_wdt.h"           /* CG WDT Driver */
#include "r_cg_crc.h"           /* CG CRC Driver */
#include "r_cg_dsadc.h"         /* CG ADC Driver */

/* Library */
#include <string.h>             /* Compiler standard library */

/* MW */
#include "em_core.h"            /* EM Core */
#include "lvm.h"                /* Logical Volume Manager */

/* WRP */
#include "wrp_em_adc_data.h"

/* Application */
#include "config_format.h"         /* EEPROM Format Header */
#include "config_storage.h"            /* Storage Header File */
#ifdef __DEBUG
#include "debug.h"          /* Debug header */
#endif
#include "powermgmt.h"
#include "pravakComm.h"


uint8_t LoadCalibrationValues(void);
uint8_t SaveCalibrationValues(void);


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Storage Module State */
typedef enum tagStorageState
{
    CONFIG_STATE_UNINITIALIZED = 0,        /* Uninitialized */
    CONFIG_STATE_INITIALIZED               /* Initialized */
    
} CONFIG_STATE;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define CONFIG_STORAGE_INTERVAL_COUNT          60

                                            /* "SPEM" ASCII code */
                                            /* "53,50,45,4D" ASCII code */
#define CONFIG_STORAGE_HEADER_CODE_MASK     0x5350454D  /* Storage Header Code Mask */

                                            /* "HEHE" ASCII code */
                                            /* "48,45,48,45" ASCII code */
#define CONFIG_STORAGE_HEADER_CODE_TEST     0x48454845  /* Storage Header Code Test */

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Variables */
#ifndef __DEBUG
static
#endif
CONFIG_STORAGE_HEADER      g_cfg_storage_header;                               /* Storage Header */

#ifndef __DEBUG
static
#endif
CONFIG_STORAGE_CRC         g_cfg_storage_crc;                                  /* Storage CRC */

#ifndef __DEBUG
static
#endif
CONFIG_STATE            g_cfg_storage_state = CONFIG_STATE_UNINITIALIZED;  /* Storage State */

#ifndef __DEBUG
static
#endif
uint8_t                 g_cfg_storage_interval_flag;                        /* Storage CRC */

#ifndef __DEBUG
static
#endif
uint8_t                 g_cfg_storage_disable_CRC_checking = 1;                 /* Enable CRC checking */

/* Sub-functions (internal used for module) */
static uint8_t CONFIG_ReadStorageHeader(void);          /* Read EEPROM Header to g_cfg_storage_header */
static uint8_t CONFIG_UpdateStorageHeader(void);        /* Update g_cfg_storage_header to EEPROM Header */
static uint8_t CONFIG_ReadStorageCrc(void);             /* Read EEPROM CRC to g_cfg_storage_header */
static uint8_t CONFIG_UpdateStorageCrc(void);           /* Update g_cfg_storage_crc to EEPROM CRC */

/***********************************************************************************************************************
* Function Name    : static uint8_t CONFIG_ReadStorageHeader(void)
* Description      : Read & Check EEPROM Header, store to g_cfg_storage_header
* Arguments        : None
* Functions Called : LVM_Read()
* Return Value     : Execution Status
*                  :    CONFIG_OK              Normal end, EEPROM is formatted
*                  :    CONFIG_ERROR           EEPROM Header error
*                  :    CONFIG_NOT_FORMATTED   Storage device is not formatted
***********************************************************************************************************************/
static uint8_t CONFIG_ReadStorageHeader(void)
{
    /* Read EEPROM header OK? */
    if (LVM_Read(   CONFIG_STORAGE_HEADER_ADDR,
                    (uint8_t *)&g_cfg_storage_header,
                    CONFIG_STORAGE_HEADER_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Read error */
    }

    /* Is header code & usage ok? */
    else if (   g_cfg_storage_header.header_code != CONFIG_STORAGE_HEADER_CODE_MASK ||
                g_cfg_storage_header.usage       == 0)
    {       
        return CONFIG_NOT_FORMATTED;   /* Not formatted */
    }
    /* Is the usage = sum of all item? */
    else if (g_cfg_storage_header.usage != (    sizeof(g_cfg_storage_header)                +\
                                                g_cfg_storage_header.num_of_config          +\
                                                g_cfg_storage_header.num_of_calib
                                            )
            )
    {
        return CONFIG_NOT_FORMATTED;   /* Not formatted */ 
    }

    /* Success */
    return CONFIG_OK;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t CONFIG_UpdateStorageHeader(void)
* Description      : Update usage & g_cfg_storage_header to EEPROM Header
* Arguments        : None
* Functions Called : LVM_Write()
* Return Value     : Execution Status
*                  :    CONFIG_OK      Normal end, update success
*                  :    CONFIG_ERROR   Write error
***********************************************************************************************************************/
static uint8_t CONFIG_UpdateStorageHeader(void)
{
    //g_cfg_storage_header.header_code = CONFIG_STORAGE_HEADER_CODE_MASK;
    /* Update usage */
    g_cfg_storage_header.usage =    sizeof(g_cfg_storage_header)                   +\
                                    g_cfg_storage_header.num_of_config             +\
                                    g_cfg_storage_header.num_of_calib ;
    
    /* Is write EEPROM OK? */
    if (LVM_Write(  CONFIG_STORAGE_HEADER_ADDR,
                    (uint8_t *)&g_cfg_storage_header,
                    CONFIG_STORAGE_HEADER_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Write error */
    }
    
    /* Success */
    return CONFIG_OK;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t CONFIG_ReadStorageCrc(void)
* Description      : Read EEPROM CRC, store to g_cfg_storage_crc
*                  : Ensure EEPROM is formatted correctly before use this API
* Arguments        : None
* Functions Called : LVM_Read()
* Return Value     : Execution Status
*                  :    CONFIG_OK              Normal end
*                  :    CONFIG_ERROR           Read error
***********************************************************************************************************************/
static uint8_t CONFIG_ReadStorageCrc(void)
{
    /* Read EEPROM CRC OK? */
    if (LVM_Read(   CONFIG_STORAGE_CRC_ADDR,
                    (uint8_t *)&g_cfg_storage_crc,
                    CONFIG_STORAGE_CRC_SIZE ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Read error */
    }
    
    /* Success */
    return CONFIG_OK;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t CONFIG_UpdateStorageHeader(void)
* Description      : Update EEPROM CRC, from g_cfg_storage_crc
*                  : Ensure EEPROM is formatted correctly before use this API
* Arguments        : None
* Functions Called : LVM_Write()
* Return Value     : Execution Status
*                  :    CONFIG_OK              Normal end
*                  :    CONFIG_ERROR           Read error
***********************************************************************************************************************/
static uint8_t CONFIG_UpdateStorageCrc(void)
{
    /* Write EEPROM CRC OK? */
    if (LVM_Write(  CONFIG_STORAGE_CRC_CALIBRATION_ADDR,
                    (uint8_t *)&g_cfg_storage_crc,
                    CONFIG_STORAGE_CRC_CALIBRATION_SIZE ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Read error */
    }
    
    /* Success */
    return CONFIG_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t CONFIG_CalculateCRC16(void)
* Description      : 
* Arguments        : None
* Functions Called : 
* Return Value     : 
***********************************************************************************************************************/
uint16_t CONFIG_CalculateCRC16(config_crc_data * p_crc_data)
{
    uint16_t crc_value = 0;
    
    if (p_crc_data != NULL)
    {
        if ( (p_crc_data->p_calibration != NULL) &&
            (p_crc_data->p_driver_adc_gain0_list != NULL) &&
            (p_crc_data->p_driver_adc_gain1_list != NULL) &&
            (p_crc_data->p_driver_timer_offset != NULL)
        )
        {
            
            /* Clear old CRC to start calc. CRC for calib */
            R_CRC_Clear();
            
            /* SW Phase */
            
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_calibration,
                CONFIG_STORAGE_CALIB_NO_ARRAY_SIZE
            );
            
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_calibration->sw_phase_correction.phase_r.i_phase_degrees,
                CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_SIZE
            );
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_calibration->sw_phase_correction.phase_y.i_phase_degrees,
                CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_SIZE
            );
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_calibration->sw_phase_correction.phase_b.i_phase_degrees,
                CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_SIZE
            );
            
            /* SW Gain */
            
            R_CRC_Calculate(
              (uint8_t *)p_crc_data->p_calibration->sw_gain.phase_r.i_gain_values,
              CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_SIZE
            );
            R_CRC_Calculate(
              (uint8_t *)p_crc_data->p_calibration->sw_gain.phase_y.i_gain_values,
              CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_SIZE
            );
            R_CRC_Calculate(
              (uint8_t *)p_crc_data->p_calibration->sw_gain.phase_b.i_gain_values,
              CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_SIZE
            );
            
            /* ADC Gain */
            
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_driver_adc_gain0_list,
                CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE
            );
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_driver_adc_gain1_list,
                CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE
            );
            
            /* Timer offset */
            
            R_CRC_Calculate(
                (uint8_t *)p_crc_data->p_driver_timer_offset,
                CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_SIZE
            );
            
            crc_value = R_CRC_GetResult();
        }
    }
    
    return crc_value;
}

/***********************************************************************************************************************
* Function Name    : uint8_t CONFIG_Init(void)
* Description      : Storage Initialization
* Arguments        : None
* Functions Called : EPR_Init()
*                  : LVM_Read()
*                  : LVM_Write()
*                  : CONFIG_ReadStorageHeader()
* Return Value     : Execution Status
*                  :    CONFIG_OK      Normal end
*                  :    CONFIG_ERROR   Error occurred
*                  :                    Read/Write error or data validity error
***********************************************************************************************************************/
uint8_t CONFIG_Init(void)
{
    uint32_t    header_code_test;
    uint8_t     status;
    
    g_cfg_storage_state = CONFIG_STATE_UNINITIALIZED;  /* Uninitialized */
    
    /* Driver Initialization */
    LVM_Init();
    
    /* Read EEPROM header code for checking read */
    if (LVM_Read(   CONFIG_STORAGE_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&g_cfg_storage_header.header_code,
                    CONFIG_STORAGE_HEADER_HEADER_CODE_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Read error */
    }
    
    /* write the header code test to check write */
    header_code_test = CONFIG_STORAGE_HEADER_CODE_TEST;
    if (LVM_Write(  CONFIG_STORAGE_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&header_code_test,
                    CONFIG_STORAGE_HEADER_HEADER_CODE_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Write error */
    }
    
    /* If write success, 
     * Re-read the header code on EEPROM and compare with CONFIG_STORAGE_HEADER_CODE_TEST
     * to check data validity */
    if (LVM_Read(   CONFIG_STORAGE_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&header_code_test,
                    CONFIG_STORAGE_HEADER_HEADER_CODE_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Read error */
    }
    /* else is read ok, next check validity */
    else if (header_code_test != CONFIG_STORAGE_HEADER_CODE_TEST)
    {
        return CONFIG_ERROR;   /* Data validity error */
    }

    /* all check OK, restore header code from header_code*/
    if (LVM_Write(  CONFIG_STORAGE_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&g_cfg_storage_header.header_code,
                    CONFIG_STORAGE_HEADER_HEADER_CODE_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;   /* Write error */
    }
    
    /* Successful, at here STORAGE module is initial success */
    g_cfg_storage_state = CONFIG_STATE_INITIALIZED;    /* Initialized */
    
    /* Check data format */
    status = CONFIG_ReadStorageHeader();
//	status=CONFIG_NOT_FORMATTED;	// comment in actual use for test only
    if (status == CONFIG_ERROR)
    {
        return CONFIG_ERROR;   /* Error occurred when read EEPROM Header */
    }
    else if (status == CONFIG_NOT_FORMATTED)
    {
        return CONFIG_NOT_FORMATTED;   /*  Storage data is not formatted */
    }

    /* Update CRC value when devide is formatted */
    if (CONFIG_ReadStorageCrc() != CONFIG_OK)
    {
        return CONFIG_ERROR;   /* Read CRC fail */
    }
    
    return CONFIG_OK;  /* OK, And storage is formatted */
}

/***********************************************************************************************************************
* Function Name    : uint8_t CONFIG_Format(void)
* Description      : Storage Format EEPROM Device to store data
* Arguments        : None
* Functions Called : LVM_Write()
* Return Value     : Execution Status
*                  :    CONFIG_OK          Formatted successfull, device is ready to store data
*                  :    CONFIG_ERROR       Storage device is not initialized
*                  :                        Or, error occurred when write data to EEPROM
***********************************************************************************************************************/
uint8_t CONFIG_Format(void)
{
    uint16_t    count;
    uint32_t    config_addr;
    uint8_t     empty_page[CONFIG_STORAGE_PAGESIZE];
    
    CONFIG_STORAGE_HEADER formatted_header =
    {
        CONFIG_STORAGE_HEADER_CODE_MASK,    /* Header Code (EM Formatted Header Code)  */
        sizeof (CONFIG_STORAGE_HEADER),     /* Usage (number of bytes)                 */
        0,                                  /* Number of EM Configuration bytes        */
        0,                                  /* Number of EM Calibration bytes          */
    };
    
    CONFIG_STORAGE_CRC formatted_crc =
    {
        0
    };
    
    /* Check storage state, only call when initial successfull */
    if (g_cfg_storage_state != CONFIG_STATE_INITIALIZED)
    {
        return CONFIG_ERROR;
    }
    
    /* Create empty_page */
    for (count = 0; count < CONFIG_STORAGE_PAGESIZE; count++)
    {
        empty_page[count] = 0;
    }
    
    /* Init whole dataflash */
    config_addr = CONFIG_STORAGE_START_ADDR;
    while (config_addr < CONFIG_STORAGE_SIZE)
    {
        if (LVM_Write(config_addr, empty_page, CONFIG_STORAGE_PAGESIZE) != LDEV_OK)
        {
            return CONFIG_ERROR;
        }
        
        config_addr += CONFIG_STORAGE_PAGESIZE;
        
        R_WDT_Restart();
    }
    
    /* Write initial EEPROM Header */
    if (LVM_Write(  CONFIG_STORAGE_HEADER_ADDR,
                    (uint8_t *)&formatted_header,
                    CONFIG_STORAGE_HEADER_SIZE  ) != LDEV_OK)
    {
        return CONFIG_ERROR;
    }
    
    /* Update current EEPROM header on RAM */
    g_cfg_storage_header = formatted_header;
    
    /* Update current EEPROM CRC on RAM */
    g_cfg_storage_crc = formatted_crc;
    
    return CONFIG_OK;  /* Formatted */
}

/***********************************************************************************************************************
* Function Name : void CONFIG_EnableCRCChecking(void)
* Interface     : void CONFIG_EnableCRCChecking(void)
* Description   : Enable to check CRC of storage
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void CONFIG_EnableCRCChecking(void)
{
    g_cfg_storage_disable_CRC_checking = 0;
}

/***********************************************************************************************************************
* Function Name : void CONFIG_DisableCRCChecking(void)
* Interface     : void CONFIG_DisableCRCChecking(void)
* Description   : Disable to check CRC of storage
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void CONFIG_DisableCRCChecking(void)
{
    g_cfg_storage_disable_CRC_checking = 1;
}

/***********************************************************************************************************************
* Function Name : CONFIG_PollingProcessing
* Interface     : void CONFIG_PollingProcessing(void)
* Description   : Polling processing for storage module
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void CONFIG_PollingProcessing(void)
{// what is this function for? it does not appear to be called
    if (g_cfg_storage_disable_CRC_checking == 1)
    {
      return;
    }

    /* CRC check for the current calibration data */
    if (g_cfg_storage_interval_flag == 1)
    {
        if (g_cfg_storage_state == CONFIG_STATE_INITIALIZED &&
            g_cfg_storage_header.num_of_calib > 0)
        {
            EM_CALIBRATION      calib;
            dsad_reg_setting_t  regs;
            uint8_t             driver_adc_gain0[EM_GAIN_PHASE_NUM_LEVEL_MAX];
            uint8_t             driver_adc_gain1[EM_GAIN_PHASE_NUM_LEVEL_MAX];
            int16_t             driver_timer_offset;
        
            calib = EM_GetCalibInfo();
            
            /* Clear the temp driver gain list */
            memset(&driver_adc_gain0, 0, sizeof(uint8_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
            memset(&driver_adc_gain1, 0, sizeof(uint8_t) * EM_GAIN_PHASE_NUM_LEVEL_MAX);
            
            /* Clear old CRC to start calc. CRC for calib */
            R_CRC_Clear();

            /* Start CRC calculation */
            R_CRC_Calculate(
              (uint8_t *)&calib,
              sizeof(calib) - (sizeof(float32_t *) * 2 * 3)
            );
            
            /* 
            * Phase degrees list 
            */ 
            R_CRC_Calculate(
                (uint8_t *)calib.sw_phase_correction.phase_r.i_phase_degrees,
                CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_R_SIZE
            );
            R_CRC_Calculate(
                (uint8_t *)calib.sw_phase_correction.phase_y.i_phase_degrees,
                CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_Y_SIZE
            );
            R_CRC_Calculate(
                (uint8_t *)calib.sw_phase_correction.phase_b.i_phase_degrees,
                CONFIG_STORAGE_CALIB_SW_DEGREE_ARRAY_PHASE_B_SIZE
            );
            
            
            /* 
            * Gain I Array 
            */
            R_CRC_Calculate(
              (uint8_t *)calib.sw_gain.phase_r.i_gain_values,
              CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_R_SIZE
            );
            R_CRC_Calculate(
              (uint8_t *)calib.sw_gain.phase_y.i_gain_values,
              CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_Y_SIZE
            );
            R_CRC_Calculate(
              (uint8_t *)calib.sw_gain.phase_b.i_gain_values,
              CONFIG_STORAGE_CALIB_SW_GAIN_ARRAY_PHASE_B_SIZE
            );
            
            /* 
             * ADC Gain Array 
            */
            /* Get the gain first */
            R_DSADC_GetGainAndPhase(&regs);
            
            driver_adc_gain0[0] = regs.gain0;
            driver_adc_gain1[0] = regs.gain1;
            
            R_CRC_Calculate(
                (uint8_t *)driver_adc_gain0,
                CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN0_ARRAY_SIZE
            );
            R_CRC_Calculate(
                (uint8_t *)driver_adc_gain1,
                CONFIG_STORAGE_CALIB_DRIVER_ADC_GAIN1_ARRAY_SIZE
            );
            
            /* Store Driver Timer Offset */
            driver_timer_offset = EM_ADC_SAMP_GetTimerOffset();
            R_CRC_Calculate(
                (uint8_t *)&driver_timer_offset,
                CONFIG_STORAGE_CALIB_DRIVER_ADC_TIMER_OFFSET_SIZE
            );
            
            /* Update the calculated CRC value to EEPROM */
            if (g_cfg_storage_crc.calibration != R_CRC_GetResult())
            {
              #ifdef __DEBUG
              //DEBUG_Printf((uint8_t *)"\n\rSTORAGE: CRC interval verification FAILED!");
              //DEBUG_Printf((uint8_t *)"\n\rSTORAGE: Start reload from EEPROM...");
              #endif

              if (EM_Stop() != EM_OK)
              {
                  #ifdef __DEBUG
                  //DEBUG_Printf((uint8_t *)"\n\r> Stop metrology FAILED!");
                  #endif
              }
              
              if (CONFIG_Restore(CONFIG_ITEM_CALIB) != CONFIG_OK)
              {
                  #ifdef __DEBUG
                  //DEBUG_Printf((uint8_t *)"\n\r> Restore calib page FAILED!");
                  #endif
              }
              
              if (POWERMGMT_IsACLow() == 0 )
              {
                  if (EM_Start() != EM_OK)
                  {
                      #ifdef __DEBUG
                      //DEBUG_Printf((uint8_t *)"\n\r> Restart metrology FAILED!");
                      #endif
                  }
              }
                
            }
            else
            {
              #ifdef __DEBUG
              //DEBUG_Printf((uint8_t *)"\n\rSTORAGE: CRC interval verification OK!");
              #endif
            }

            #ifdef __DEBUG
            //DEBUG_Printf((uint8_t *)"\n\r");
            DEBUG_AckNewLine();
            #endif
        }
        
        /* ACK */
        g_cfg_storage_interval_flag = 0;
    }
}

uint8_t CONFIG_Backup(uint8_t selection)
{
    uint8_t                 status;
    
    /* Check parameter */
    if (selection == CONFIG_ITEM_NONE)
    {
        return CONFIG_ERROR;   /* Params error */
    }
    
    /* Check Storage State */
    if (g_cfg_storage_state != CONFIG_STATE_INITIALIZED)
    {
        return CONFIG_ERROR;   /* Device is not initialized */
    }
    
    /* Get EEPROM Header */
    if (CONFIG_ReadStorageHeader() != CONFIG_OK)
    {
        return CONFIG_ERROR;   /* EEPROM Header error (not formatted) */
    }
    
    /* Is CONFIG_ITEM_CONFIG selected? */
    if ((selection & CONFIG_ITEM_CONFIG) != 0)
    {
        EM_CONFIG config;
        
        /* Get current EM Core config */
        config = EM_GetConfig();
        
        /* Store config to EEPROM */
        if (    LVM_Write(  CONFIG_STORAGE_CONFIG_ADDR, 
                            (uint8_t *)&config,
                            sizeof(config) - sizeof(EM_TIME_SLOT *) ) != LDEV_OK)
        {
            return CONFIG_ERROR;   /* Write error */
        }
        
        /* Store time slot array */
        if (    LVM_Write(  CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_ADDR, 
                            (uint8_t *)config.timeslot_list,
                            CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_SIZE   ) != LDEV_OK)
        {
            return CONFIG_ERROR;   /* Write error */
        }
        
        /* Update number used bytes of config to storage header */
        g_cfg_storage_header.num_of_config = CONFIG_STORAGE_CONFIG_SIZE;
    }

    // Is CONFIG_ITEM_CALIB selected?
    if ((selection & CONFIG_ITEM_CALIB) != 0)
    {
//			status = SaveCalibrationValues();
//			return status;
			status = SaveCalibrationValues();
			
    // Update number used bytes of calib to storage header  
      g_cfg_storage_header.num_of_calib = CONFIG_STORAGE_CALIB_SIZE;  
    }

// The following if is part of this function		
    // Update EEPROM Header  
    if (CONFIG_UpdateStorageHeader() != CONFIG_OK)
    {
        return CONFIG_ERROR;   // Write error  
    }
    
    // Success  
    return CONFIG_OK;
}

uint8_t CONFIG_Restore(uint8_t selection)
{
    uint8_t                 status;
    
    /* Check parameter */
    if (selection == CONFIG_ITEM_NONE)
    {
        return CONFIG_ERROR;   /* Params error */
    }
    
    /* Check Storage State */
    if (g_cfg_storage_state != CONFIG_STATE_INITIALIZED)
    {
        return CONFIG_ERROR;   /* Device is not initialized */
    }
    
    /* Get EEPROM Header */
    if (CONFIG_ReadStorageHeader() != CONFIG_OK)
    {
        return CONFIG_ERROR;   /* EEPROM Header error */
    }
    
    /* Is CONFIG_ITEM_CONFIG selected? */
    if ((selection & CONFIG_ITEM_CONFIG) != 0)
    {
        /* Is EM Configuration data valid in EEPROM? */
        if (g_cfg_storage_header.num_of_config == CONFIG_STORAGE_CONFIG_SIZE)
        {
            EM_CONFIG       config;
            EM_TIME_SLOT    timeslot_list[6];
    
            /* Get config from EEPROM, accept the timeslot_list */
            if (    LVM_Read(   CONFIG_STORAGE_CONFIG_ADDR, 
                                (uint8_t *)&config,
                                sizeof(config) - sizeof(EM_TIME_SLOT *) ) != LDEV_OK)
            {
                return CONFIG_ERROR;   /* Read error */
            }
            
            /* Get time slot array */
            if (    LVM_Read(   CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_ADDR, 
                                (uint8_t *)timeslot_list,
                                CONFIG_STORAGE_CONFIG_TIMESLOT_ARRAY_SIZE   ) != LDEV_OK)
            {
                return CONFIG_ERROR;   /* Read error */
            }

            /* Link the timeslot_list to config */
            config.timeslot_list = timeslot_list;
            
            /* Set to EM Core */
						config.voltage_low = 50;	// 27/Nov/2018
						
            if (EM_SetConfig(&config) != EM_OK)
            {
                return CONFIG_ERROR_DATA_CORRUPT;  /* Data corrupt */
            }
        }
    }

    // Is CONFIG_ITEM_CALIB selected? */
    if ((selection & CONFIG_ITEM_CALIB) != 0)
    {
			status = LoadCalibrationValues();
			return status;
			
		}
    // Success */
    return CONFIG_OK;
}


/***********************************************************************************************************************
* Function Name : CONFIG_RtcCallback
* Interface     : void CONFIG_RtcCallback()
* Description   : RTC Callback to count time
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void CONFIG_RtcCallback(void)
{
    static uint16_t counter = 0;
    
    /* Check to signal the interval flag for CRC check */
    counter++;
    if (counter >= CONFIG_STORAGE_INTERVAL_COUNT)
    {
        g_cfg_storage_interval_flag = 1;
        counter = 0;
    }
}
