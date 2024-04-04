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
// Driver  
#include "r_cg_macrodriver.h"       // CG Macro Driver  
#include "r_cg_rtc.h"           // CG RTC Driver  
#include "r_cg_wdt.h"           // CG WDT Driver  
#include "r_cg_crc.h"           // CG CRC Driver  
#include "r_cg_dsadc.h"         // CG ADC Driver  

// Library  
#include <string.h>             // Compiler standard library  

// MW  
#include "em_core.h"            // EM Core  
#include "eeprom.h"         // EEPROM MW (Init/Read/Write)  
#include "e2rom.h"         // pravak implementation

// WRP  
#include "wrp_em_adc.h"

// Application  
#include "format.h"         // EEPROM Format Header  
#include "storage.h"            // Storage Header File  
#include "event.h"
#ifdef __DEBUG
#include "debug.h"          // Debug header  
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
// Storage Module State  
typedef enum tagStorageState
{
    STORAGE_STATE_UNINITIALIZED = 0,        // Uninitialized  
    STORAGE_STATE_INITIALIZED               // Initialized  
    
} STORAGE_STATE;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

                                            // "SPEM" ASCII code  
                                            // "53,50,45,4D" ASCII code  
#define STORAGE_EEPROM_HEADER_CODE_MASK     0x5350454D  // Storage Header Code Mask  

                                            // "HEHE" ASCII code  
                                            // "48,45,48,45" ASCII code  
#define STORAGE_EEPROM_HEADER_CODE_TEST     0x48454845  // Storage Header Code Test  

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
// Variables  
#ifndef __DEBUG
static
#endif
STORAGE_EEPROM_HEADER   g_storage_header;                               // Storage Header  

#ifndef __DEBUG
static
#endif
STORAGE_STATE           g_storage_state = STORAGE_STATE_UNINITIALIZED;  // Storage State  

#ifndef __DEBUG
static
#endif
uint8_t                 g_storage_interval_flag;                        // Storage CRC  

// Sub-functions (internal used for module)  
static uint8_t STORAGE_ReadEEPROMHeader(void);          // Read EEPROM Header to g_storage_header  
static uint8_t STORAGE_UpdateEEPROMHeader(void);        // Update g_storage_header to EEPROM Header  

/***********************************************************************************************************************
* Function Name    : static uint8_t STORAGE_ReadEEPROMHeader(void)
* Description      : Read & Check EEPROM Header, store to g_storage_header
* Arguments        : None
* Functions Called : EPR_Read()
* Return Value     : Execution Status
*                  :    STORAGE_OK              Normal end, EEPROM is formatted
*                  :    STORAGE_ERROR           EEPROM Header error
*                  :    STORAGE_NOT_FORMATTED   Storage device is not formatted
***********************************************************************************************************************/
static uint8_t STORAGE_ReadEEPROMHeader(void)
{
    // Read EEPROM header OK?  
    if (EPR_Read(   STORAGE_EEPROM_HEADER_ADDR,
                    (uint8_t *)&g_storage_header,
                    STORAGE_EEPROM_HEADER_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;   // Read error  
    }

    // Is header code & usage ok?  
    else if (   g_storage_header.header_code != STORAGE_EEPROM_HEADER_CODE_MASK ||
                g_storage_header.usage       == 0)
    {       
        return STORAGE_NOT_FORMATTED;   // Not formatted  
    }
    // Is the usage = sum of all item?  
    else if (g_storage_header.usage != (    sizeof(g_storage_header)                +\
                                            g_storage_header.num_of_sys_state       +\
                                            g_storage_header.num_of_rtc_backup      +\
                                            g_storage_header.num_of_platform_info   +\
                                            g_storage_header.num_of_energy_log
                                             * sizeof(ONE_MONTH_ENERGY_DATA_LOG)    +\
                                            g_storage_header.num_of_tamper_log
                                             * sizeof(ONE_TAMPER_DATA_LOG)  )
            )
    {
        return STORAGE_NOT_FORMATTED;   // Not formatted   
    }

    // Is indexing on energy log valid?  
    else if (   g_storage_header.num_of_energy_log > 0 &&
                g_storage_header.cur_energy_log_index >= g_storage_header.num_of_energy_log)
    {
        return STORAGE_NOT_FORMATTED;   // Not formatted  
    }
    // Is indexing on energy tamper log valid?  
    else if (   g_storage_header.num_of_tamper_log > 0 &&
                g_storage_header.cur_tamper_log_index >= g_storage_header.num_of_tamper_log)
    {
        return STORAGE_NOT_FORMATTED;   // Not formatted  
    }
    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t STORAGE_UpdateEEPROMHeader(void)
* Description      : Update usage & g_storage_header to EEPROM Header
* Arguments        : None
* Functions Called : EPR_Write()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Normal end, update success
*                  :    STORAGE_ERROR   Write error
***********************************************************************************************************************/
static uint8_t STORAGE_UpdateEEPROMHeader(void)
{
    //g_storage_header.header_code = STORAGE_EEPROM_HEADER_CODE_MASK;
    // Update usage  
    g_storage_header.usage = sizeof(g_storage_header)                   +\
                             g_storage_header.num_of_sys_state          +\
                             g_storage_header.num_of_rtc_backup         +\
                             g_storage_header.num_of_platform_info      +\
                             g_storage_header.num_of_energy_log
                              * sizeof(ONE_MONTH_ENERGY_DATA_LOG)       +\
                             g_storage_header.num_of_tamper_log
                              * sizeof(ONE_TAMPER_DATA_LOG) ;
    
    // Is write EEPROM OK?  
    if (EPR_Write(  STORAGE_EEPROM_HEADER_ADDR,
                    (uint8_t *)&g_storage_header,
                    STORAGE_EEPROM_HEADER_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;   // Write error  
    }
    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_Init(void)
* Description      : Storage Initialization
* Arguments        : None
* Functions Called : EPR_Init()
*                  : EPR_Read()
*                  : EPR_Write()
*                  : STORAGE_ReadEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Normal end
*                  :    STORAGE_ERROR   Error occurred
*                  :                    Read/Write error or data validity error
***********************************************************************************************************************/
uint8_t STORAGE_Init(void)
{
    volatile uint32_t    header_code_test;
    volatile uint8_t     status;
    
    g_storage_state = STORAGE_STATE_UNINITIALIZED;  // Uninitialized  
    
    // EEPROM Driver Initialization  
    EPR_Init();	// This function clears g_is_send_end and g_is_receive_end flags
    
    // Read EEPROM header code for checking read  
    if (EPR_Read(   STORAGE_EEPROM_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&g_storage_header.header_code,
                    STORAGE_EEPROM_HEADER_HEADER_CODE_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;   // Read error  
    }
    
    // write the header code test to check write  
    header_code_test = STORAGE_EEPROM_HEADER_CODE_TEST;	// write 4 bytes (int32) from address 0x0000 onwards 
    if (EPR_Write(  STORAGE_EEPROM_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&header_code_test,
                    STORAGE_EEPROM_HEADER_HEADER_CODE_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;   // Write error  
    }
    
    // If write success, 
     // Re-read the header code on EEPROM and compare with STORAGE_EEPROM_HEADER_CODE_TEST
     // to check data validity  
    if (EPR_Read(   STORAGE_EEPROM_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&header_code_test,
                    STORAGE_EEPROM_HEADER_HEADER_CODE_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;   // Read error  
    }
    // else is read ok, next check validity  
    else if (header_code_test != STORAGE_EEPROM_HEADER_CODE_TEST)
    {
        return STORAGE_ERROR;   // Data validity error  
    }

    // all check OK, restore header code from header_code 
    if (EPR_Write(  STORAGE_EEPROM_HEADER_HEADER_CODE_ADDR,
                    (uint8_t *)&g_storage_header.header_code,
                    STORAGE_EEPROM_HEADER_HEADER_CODE_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;   // Write error  
    }
    
    // Successful, at here STORAGE module is initial success  
    g_storage_state = STORAGE_STATE_INITIALIZED;    // Initialized  
    
    // Check data format  
    status = STORAGE_ReadEEPROMHeader();
    if (status == STORAGE_ERROR)
    {
        return STORAGE_ERROR;   // Error occurred when read EEPROM Header  
    }
    else if (status == STORAGE_NOT_FORMATTED)
    {
        return STORAGE_NOT_FORMATTED;   //  Storage data is not formatted  
    }

    return STORAGE_OK;  // OK, And storage is formatted  
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_Format(void)
* Description      : Storage Format EEPROM Device to store data
* Arguments        : None
* Functions Called : EPR_Write()
* Return Value     : Execution Status
*                  :    STORAGE_OK          Formatted successfull, device is ready to store data
*                  :    STORAGE_ERROR       Storage device is not initialized
*                  :                        Or, error occurred when write data to EEPROM
***********************************************************************************************************************/
uint8_t STORAGE_Format(void)
{
    uint16_t    count;
    uint32_t    eeprom_addr;
    uint8_t     empty_page[STORAGE_EEPROM_PAGESIZE];
    
    STORAGE_EEPROM_HEADER formatted_header =
    {
        STORAGE_EEPROM_HEADER_CODE_MASK,    // Header Code (EM Formatted Header Code)   
        sizeof (STORAGE_EEPROM_HEADER),     // Usage (number of bytes)                  
        0,                                  // Number of EM System state bytes          
        0,                                  // Number of RTC Backup bytes               
        0,                                  // Number of EM Platform Information        
        0,                                  // Number of EM Energy Data Log Record      
        0,                                  // Number of EM Tamper Data Log Record      
        0,                                  // Current Energy Log Index                 
        0,                                  // Current Tamper Log Index                 
    };
    
    // Check storage state, only call when initial successfull  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;
    }
    
    // Create empty_page  
    for (count = 0; count < STORAGE_EEPROM_PAGESIZE; count++)
    {
        empty_page[count] = 0;
    }
    
    // Clear Whole EEPROM  
    eeprom_addr = STORAGE_EEPROM_START_ADDR;
    while (eeprom_addr < STORAGE_EEPROM_SIZE)
    {
        if (EPR_Write(eeprom_addr, empty_page, STORAGE_EEPROM_PAGESIZE) != EPR_OK)
        {
            return STORAGE_ERROR;
        }
        
        eeprom_addr += STORAGE_EEPROM_PAGESIZE;
        
        R_WDT_Restart();
    }
    
    // Write initial EEPROM Header  
    if (EPR_Write(  STORAGE_EEPROM_HEADER_ADDR,
                    (uint8_t *)&formatted_header,
                    STORAGE_EEPROM_HEADER_SIZE  ) != EPR_OK)
    {
        return STORAGE_ERROR;
    }
    
    // Update current EEPROM header on RAM  
    g_storage_header = formatted_header;
    
    return STORAGE_OK;  // Formatted  
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_Backup(uint8_t selection)
* Description      : Storage Backup (from EM Core to EEPROM)
* Note             : When STORAGE_ERROR is returned, maybe some data on selected item
*                  : (of [selection] parameter) have been stored to EEPROM
* Arguments        : uint8_t selection: Backup item selection
*                  :      STORAGE_ITEM_SYS_STATE    Select EM System State
*                  :      STORAGE_ITEM_RTC_TIME     Select RTC Time
*                  :      STORAGE_ITEM_ALL          Select all above items
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Write()
*                  : EM_GetConfig()
*                  : EM_GetCalibInfo()
*                  : EM_GetState()
*                  : RTC_CounterGet()
*                  : STORAGE_UpdateEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Backup successfull
*                  :    STORAGE_ERROR   Storage device is not initialized or not formatted
*                  :                    Or, error occurred when write EEPROM
*                  :                    Or, selection = 0 (STORAGE_ITEM_NONE)
***********************************************************************************************************************/
uint8_t STORAGE_Backup(uint8_t selection)
{
    uint8_t                 status;
    
    // Check parameter  
    if (selection == STORAGE_ITEM_NONE)
    {
        return STORAGE_ERROR;   // Params error  
    }
    
    // Check Storage State  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;   // Device is not initialized  
    }
    
    // Get EEPROM Header  
    if (STORAGE_ReadEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // EEPROM Header error (not formatted)  
    }
    
    // Is STORAGE_ITEM_SYS_STATE selected?  
    if ((selection & STORAGE_ITEM_SYS_STATE) != 0)
    {
        uint32_t eeprom_addr;
        uint16_t statesize, copied_size;    
        uint8_t  page_buffer[STORAGE_EEPROM_PAGESIZE];
    
        eeprom_addr = STORAGE_EEPROM_SYSTEM_STATE_ADDR; // Point to EEPROM System State Location  
        copied_size = 0;
        statesize   = 0;
        
        // Start backup the system state  
        do
        {
            // Get 1 page of the state  
            status = EM_GetState(page_buffer, STORAGE_EEPROM_PAGESIZE, &statesize);
            
            // Is end of copy?  
            if (status == EM_OK)
            {
                // Write the remain buffer (statesize - copied_size) to EEPROM  
                if (EPR_Write(eeprom_addr, page_buffer, statesize - copied_size) != EPR_OK)
                {
                    return STORAGE_ERROR;   // Write error  
                }
                
                copied_size = statesize;    // Backup all  
            }
            // Is still copying?  
            else if (status == EM_COPYING)
            {
                // Write 1 page to EEPROM  
                if (EPR_Write(eeprom_addr, page_buffer, STORAGE_EEPROM_PAGESIZE) != EPR_OK)
                {
                    return STORAGE_ERROR;   // Write error  
                }
                
                // Start copy next page  
                eeprom_addr += STORAGE_EEPROM_PAGESIZE;
                copied_size += STORAGE_EEPROM_PAGESIZE;
            }
            else    // Error occurred on Get State of EM Core  
            {
                return STORAGE_ERROR;       // EM Core Get State error  
            }           

        }
        // Only loop when still copying &
         // total copied byte not exceed max buffer size for System State  
        while (status == EM_COPYING && copied_size < STORAGE_EEPROM_SYSTEM_STATE_SIZE);
        
        // Update number used bytes of system state to storage header  
        g_storage_header.num_of_sys_state = statesize;
    }

    // Is STORAGE_ITEM_RTC_TIME selected?  
    if ((selection & STORAGE_ITEM_RTC_TIME) != 0)
    {
        uint16_t                        timeout;    
        rtc_calendarcounter_value_t     rtctime;
        RTC_DATE_TIME                   eeprom_rtctime;
    
        // Get RTC Time  
        R_RTC_Get_CalendarCounterValue(&rtctime);
        status = MD_OK;
        
        // end of getting RTC time (timeout is over),
         // but still not success,
         // break processing here with error status  
        if (status != MD_OK)
        {
            return STORAGE_ERROR;   // Can't get RTC time  
        }
        
        eeprom_rtctime.Sec      = rtctime.rseccnt   ;
        eeprom_rtctime.Min      = rtctime.rmincnt   ;
        eeprom_rtctime.Hour     = rtctime.rhrcnt    ;
        eeprom_rtctime.Day      = rtctime.rdaycnt   ;
        eeprom_rtctime.Week     = rtctime.rwkcnt    ;
        eeprom_rtctime.Month    = rtctime.rmoncnt   ;
        eeprom_rtctime.Year     = (uint8_t)rtctime.ryrcnt    ;
        eeprom_rtctime.reserved = 0             ;
        
        // Store to EEPROM  
        if (EPR_Write(  STORAGE_EEPROM_RTC_TIME_BACKUP_ADDR,
                        (uint8_t *)&eeprom_rtctime,
                        STORAGE_EEPROM_RTC_TIME_BACKUP_SIZE ) != EPR_OK)
        {
            return STORAGE_ERROR;
        }
        
        // Update number used bytes of RTC backup to storage header  
        g_storage_header.num_of_rtc_backup = STORAGE_EEPROM_RTC_TIME_BACKUP_SIZE;
    }

    // Update EEPROM Header  
    if (STORAGE_UpdateEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Write error  
    }
    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_Restore(uint8_t selection)
* Description      : Storage Restore (from EEPROM to EM Core)
* Arguments        : uint8_t selection: Restore item selection
*                  :      STORAGE_ITEM_SYS_STATE    Select EM System State
*                  :      STORAGE_ITEM_RTC_TIME     Select RTC Time
*                  :      STORAGE_ITEM_ALL          Select all above items
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Read()
*                  : EM_SetConfig()
*                  : EM_SetCalibInfo()
*                  : EM_SetState()
*                  : RTC_CounterSet()
* Return Value     : Execution Status
*                  :    STORAGE_OK                  Restore successfull
*                  :    STORAGE_ERROR_DATA_CORRUPT  Related data on EEPROM of selected item is corrupt
*                  :    STORAGE_ERROR               Storage device is not initialized or not formatted
*                  :                                Or, error occurred when read EEPROM,
*                  :                                Or, selection = 0 (STORAGE_ITEM_NONE)
***********************************************************************************************************************/
uint8_t STORAGE_Restore(uint8_t selection)
{
    uint8_t                 status;
    
    // Check parameter  
    if (selection == STORAGE_ITEM_NONE)
    {
        return STORAGE_ERROR;   // Params error  
    }
    
    // Check Storage State  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;   // Device is not initialized  
    }
    
    // Get EEPROM Header  
    if (STORAGE_ReadEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // EEPROM Header error  
    }
    
    // Is STORAGE_ITEM_SYS_STATE selected?  
    if ((selection & STORAGE_ITEM_SYS_STATE) != 0)
    {
        // Is EM System State data valid in EEPROM?  
        if (g_storage_header.num_of_sys_state > 0 &&
            g_storage_header.num_of_sys_state <= STORAGE_EEPROM_SYSTEM_STATE_SIZE)
        {
            uint32_t    eeprom_addr;
            uint16_t    statesize, page_size;   
            uint8_t     page_buffer[STORAGE_EEPROM_PAGESIZE];
    
            eeprom_addr = STORAGE_EEPROM_SYSTEM_STATE_ADDR;         // Point to EEPROM System State Location  
            statesize   = g_storage_header.num_of_sys_state;        // Get number of system state  
            
            // Start backup the system state  
            do
            {
                // Is system state still >= 1 page?  
                if (statesize >= STORAGE_EEPROM_PAGESIZE)
                {
                    page_size = STORAGE_EEPROM_PAGESIZE;
                }
                else
                {
                    page_size = statesize;
                }
            
                // Read system state buffer from EEPROM  
                if (EPR_Read(eeprom_addr, page_buffer, page_size) != EPR_OK)
                {
                    return STORAGE_ERROR;   // Read error  
                }

                // Set to EM Core
                //  In case of EM_OK is returned
                //  the loop will break, END of processing
                //  Else, system state need more than 1 page
                
                status = EM_SetState(page_buffer, page_size, g_storage_header.num_of_sys_state);
                
                // Is still copying?  
                if (status == EM_COPYING)
                {
                    // Point to next page  
                    statesize   -= page_size;               
                    eeprom_addr += page_size;
                }
                // Not OK?  
                else if (status != EM_OK)
                {
                    return STORAGE_ERROR_DATA_CORRUPT;  // Data corrupt  
                }

            }
            // Only loop when still copying & still remain state buffer  
            while (status == EM_COPYING && statesize > 0);
        }
    }

    // Is STORAGE_ITEM_RTC_TIME selected?  
    if ((selection & STORAGE_ITEM_RTC_TIME) != 0)
    {
        // Is RTC Backup data valid in EEPROM?  
        if (g_storage_header.num_of_rtc_backup == STORAGE_EEPROM_RTC_TIME_BACKUP_SIZE)
        {
            uint16_t                        timeout;    
            rtc_calendarcounter_value_t     rtctime;
            RTC_DATE_TIME                   eeprom_rtctime;
    
            // Get RTC Backup from EEPROM  
            if (EPR_Read(   STORAGE_EEPROM_RTC_TIME_BACKUP_ADDR,
                            (uint8_t *)&eeprom_rtctime,
                            STORAGE_EEPROM_RTC_TIME_BACKUP_SIZE ) != EPR_OK)
            {
                return STORAGE_ERROR;   // Read error  
            }
            
            rtctime.rseccnt = eeprom_rtctime.Sec   ;
            rtctime.rmincnt = eeprom_rtctime.Min   ;
            rtctime.rhrcnt  = eeprom_rtctime.Hour  ;
            rtctime.rdaycnt = eeprom_rtctime.Day   ;
            rtctime.rwkcnt  = eeprom_rtctime.Week  ;
            rtctime.rmoncnt = eeprom_rtctime.Month ;
            rtctime.ryrcnt  = eeprom_rtctime.Year  ;
            
            // Set RTC Time  
            R_RTC_Set_CalendarCounterValue(rtctime);
            status = MD_OK;
            
            // end of setting RTC time (time is over),
            // but still not success,
            // break processing here with error status  
            if (status != MD_OK)
            {
                return STORAGE_ERROR;   // Can't set RTC time  
            }
        }
    }
    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_ReadPlatformInfo(PLATFORM_INFO *p_platform_info)
* Description      : Storage Read Platform Information
* Arguments        : PLATFORM_INFO *p_platform_info: Buffer to store the platform info
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Read()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Read successfull
*                  :    STORAGE_ERROR   Error occurred, NULL parameter
*                  :                    Or, Storage device is not initialized or not formatted, or R/W error
***********************************************************************************************************************/
uint8_t STORAGE_ReadPlatformInfo(PLATFORM_INFO *p_platform_info)
{
    // Check parameter  
    if (p_platform_info == NULL)
    {
        return STORAGE_ERROR;   // Params error  
    }
    
    // Check Storage State  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;   // Device is not initialized  
    }

/*
// check this out is it necessary
    // Read & Check eeprom header  
    if (STORAGE_ReadEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Header error  
    }
*/    
    // Read platform info block  
    if (EPR_Read(   STORAGE_EEPROM_PLATFORM_INFO_ADDR,
                    (uint8_t *)p_platform_info,
                    STORAGE_EEPROM_PLATFORM_INFO_SIZE   ) != EPR_OK )
    {
        return STORAGE_ERROR;   // Read error  
    }
    
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_WritePlatformInfo(PLATFORM_INFO *p_platform_info)
* Description      : Storage Write Platform Information
* Arguments        : PLATFORM_INFO *p_platform_info: Buffer of the platform info wanted to write
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Write()
*                  : STORAGE_UpdateEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Write successfull
*                  :    STORAGE_ERROR   Error occurred, NULL parameter
*                  :                    Or, Storage device is not initialized or not formatted, or R/W error
**********************************************************************************************************************/
uint8_t STORAGE_WritePlatformInfo(PLATFORM_INFO *p_platform_info)
{
    // Check parameter  
    if (p_platform_info == NULL)
    {
        return STORAGE_ERROR;   // Params error  
    }
    
    // Check Storage State  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;   // Device is not initialized  
    }
    
    // Read & Check eeprom header  
    if (STORAGE_ReadEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Header error  
    }
    
    // Write platform info block  
    if (EPR_Write(  STORAGE_EEPROM_PLATFORM_INFO_ADDR,
                    (uint8_t *)p_platform_info,
                    STORAGE_EEPROM_PLATFORM_INFO_SIZE   ) != EPR_OK )
    {
        return STORAGE_ERROR;   // Write error  
    }
    
    // Update usage on EEPROM  
    g_storage_header.num_of_platform_info = STORAGE_EEPROM_PLATFORM_INFO_SIZE;
    
    // Update eeprom header  
    if (STORAGE_UpdateEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Write header error  
    }
    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_AddEnergyLog(ONE_MONTH_ENERGY_DATA_LOG *p_energy_log)
* Description      : Storage Add Energy Log Record
* Arguments        : ONE_MONTH_ENERGY_DATA_LOG *p_energy_log: Buffer of a energy log record
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Write()
*                  : STORAGE_UpdateEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Add successfull
*                  :    STORAGE_ERROR   Error occurred, NULL parameter
*                  :                    Or, Storage device is not initialized or not formatted, or R/W error
***********************************************************************************************************************/
uint8_t STORAGE_AddEnergyLog(ONE_MONTH_ENERGY_DATA_LOG *p_energy_log)
{
/*
    uint32_t    eeprom_addr;
    
    // Check parameter  
    if (p_energy_log == NULL)
    {
        return STORAGE_ERROR;   // Params error  
    }
    
    // Check Storage State  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;   // Device is not initialized  
    }
    
    // Read & Check eeprom header  
    if (STORAGE_ReadEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Header error  
    }
    
    // Point to the next element on the array of energy log
    // Update num_of_energy_log & cur_energy_log_index  
    if (g_storage_header.num_of_energy_log == 0)        // Fist time of logging?  
    {
        g_storage_header.num_of_energy_log    = 1;
        g_storage_header.cur_energy_log_index = 0;
    }
    else    // logging than 1 times  
    {
        // is array not full?  
        if (g_storage_header.num_of_energy_log * sizeof(ONE_MONTH_ENERGY_DATA_LOG) < STORAGE_EEPROM_ENERGY_DATA_LOG_SIZE)
        {
            g_storage_header.num_of_energy_log++;
        }
        
        // when num_of_energy_log reached to max (12 months)
        // the g_storage_header.cur_energy_log_index will roll over  
        g_storage_header.cur_energy_log_index++;
        g_storage_header.cur_energy_log_index %= (STORAGE_EEPROM_ENERGY_DATA_LOG_SIZE / sizeof(ONE_MONTH_ENERGY_DATA_LOG));
    }
    
    // Point to element on energy data log array  
    eeprom_addr  = STORAGE_EEPROM_ENERGY_DATA_LOG_ADDR;                                         // Point to ahead of array  
    eeprom_addr += g_storage_header.cur_energy_log_index * sizeof(ONE_MONTH_ENERGY_DATA_LOG);   // Add offset  

    // Write record  
    if (EPR_Write(  eeprom_addr,
                    (uint8_t *)p_energy_log,
                    sizeof(ONE_MONTH_ENERGY_DATA_LOG)   ) != EPR_OK )
    {
        return STORAGE_ERROR;   // Write error  
    }

    // Update eeprom header  
    if (STORAGE_UpdateEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Header error  
    }
*/    
    // Success  
    return STORAGE_OK;
}

/**********************************************************************************************************************
* Function Name    : uint8_t STORAGE_AddTamperLog(ONE_TAMPER_DATA_LOG *p_tamper_log)
* Description      : Storage Add Tamper Log record
* Arguments        : ONE_TAMPER_DATA_LOG *p_tamper_log: Buffer of a tamper log record wanted to write
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Write()
*                  : STORAGE_UpdateEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Add successfull
*                  :    STORAGE_ERROR   Error occurred, NULL parameter
*                  :                    Or, Storage device is not initialized or not formatted, or R/W error
***********************************************************************************************************************/
uint8_t STORAGE_AddTamperLog(ONE_TAMPER_DATA_LOG *p_tamper_log)
{
/*	
    uint32_t    eeprom_addr;
    
    // Check parameter  
    if (p_tamper_log == NULL)
    {
        return STORAGE_ERROR;   // Params error  
    }
    
    // Check Storage State  
    if (g_storage_state != STORAGE_STATE_INITIALIZED)
    {
        return STORAGE_ERROR;   // Device is not initialized  
    }
    
    // Read & Check eeprom header  
    if (STORAGE_ReadEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Header error  
    }
    
    // Point to the next element on the array of tamper log
    // Update num_of_tamper_log & cur_tamper_log_index  
    if (g_storage_header.num_of_tamper_log == 0)        // Fist time of logging?  
    {
        g_storage_header.num_of_tamper_log    = 1;
        g_storage_header.cur_tamper_log_index = 0;
    }
    else    // logging than 1 times  
    {
        // is array not full?  
        if (g_storage_header.num_of_tamper_log * sizeof(ONE_TAMPER_DATA_LOG) < STORAGE_EEPROM_TAMPER_DATA_LOG_SIZE)
        {
            g_storage_header.num_of_tamper_log++;
        }
        
        // when num_of_tamper_log reached to max (200 events)
        // the g_storage_header.cur_tamper_log_index will roll over  
        g_storage_header.cur_tamper_log_index++;
        g_storage_header.cur_tamper_log_index %= STORAGE_EEPROM_TAMPER_DATA_LOG_SIZE / sizeof(ONE_TAMPER_DATA_LOG);
    }
    
    // Point to element on tamper data log array  
    eeprom_addr  = STORAGE_EEPROM_TAMPER_DATA_LOG_ADDR;                                     // Point to head  
    eeprom_addr += g_storage_header.cur_tamper_log_index * sizeof(ONE_TAMPER_DATA_LOG);     // Add offset  

    // Write record  
    if (EPR_Write(  eeprom_addr,
                    (uint8_t *)p_tamper_log,
                    sizeof(ONE_TAMPER_DATA_LOG) ) != EPR_OK )
    {
        return STORAGE_ERROR;   // Write error  
    }

    // Update eeprom header  
    if (STORAGE_UpdateEEPROMHeader() != STORAGE_OK)
    {
        return STORAGE_ERROR;   // Header error  
    }
*/    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_GetEnergyLog(ONE_MONTH_ENERGY_DATA_LOG *p_energy_log)
* Description      : Storage Add Energy Log Record
* Arguments        : ONE_MONTH_ENERGY_DATA_LOG *p_energy_log: Buffer of a energy log record
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Write()
*                  : STORAGE_UpdateEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Add successfull
*                  :    STORAGE_ERROR   Error occurred, NULL parameter
*                  :                    Or, Storage device is not initialized or not formatted, or R/W error
***********************************************************************************************************************/
uint8_t STORAGE_GetEnergyLog(ONE_MONTH_ENERGY_DATA_LOG *p_energy_log)
{
/*
    uint8_t                     i, number_tariff;
    rtc_calendarcounter_value_t rtctime;        // RTC data 
    ONE_MONTH_ENERGY_DATA_LOG   energy_log;
    
    R_RTC_Get_CalendarCounterValue(&rtctime);
    
    // zero all bytes of energy_log buffer  
    memset((uint8_t *)&energy_log, 0, sizeof(ONE_MONTH_ENERGY_DATA_LOG));
    
    // Log RTC time to energy log  
    energy_log.present_rtc_log.Sec   = rtctime.rseccnt;
    energy_log.present_rtc_log.Min   = rtctime.rmincnt;
    energy_log.present_rtc_log.Hour  = rtctime.rhrcnt;
    energy_log.present_rtc_log.Day   = rtctime.rdaycnt;
    energy_log.present_rtc_log.Week  = rtctime.rwkcnt;
    energy_log.present_rtc_log.Month = rtctime.rmoncnt;
    energy_log.present_rtc_log.Year  = (uint8_t)(rtctime.ryrcnt);
    
    // Log max demand  
    energy_log.phase_r.present_max_demand.active   = EM_GetActiveMaxDemand(LINE_PHASE_R);
    energy_log.phase_r.present_max_demand.reactive = EM_GetReactiveMaxDemand(LINE_PHASE_R);
    energy_log.phase_r.present_max_demand.apparent = EM_GetApparentMaxDemand(LINE_PHASE_R);
    
    energy_log.phase_y.present_max_demand.active   = EM_GetActiveMaxDemand(LINE_PHASE_Y);
    energy_log.phase_y.present_max_demand.reactive = EM_GetReactiveMaxDemand(LINE_PHASE_Y);
    energy_log.phase_y.present_max_demand.apparent = EM_GetApparentMaxDemand(LINE_PHASE_Y);
    
    energy_log.phase_b.present_max_demand.active   = EM_GetActiveMaxDemand(LINE_PHASE_B);
    energy_log.phase_b.present_max_demand.reactive = EM_GetReactiveMaxDemand(LINE_PHASE_B);
    energy_log.phase_b.present_max_demand.apparent = EM_GetApparentMaxDemand(LINE_PHASE_B);
    
    energy_log.total.present_max_demand.active   =      energy_log.phase_r.present_max_demand.active +
                                                        energy_log.phase_y.present_max_demand.active +
                                                        energy_log.phase_b.present_max_demand.active;
                                                    
    energy_log.total.present_max_demand.reactive   =    energy_log.phase_r.present_max_demand.reactive +
                                                        energy_log.phase_y.present_max_demand.reactive +
                                                        energy_log.phase_b.present_max_demand.reactive;
                                                    
    energy_log.total.present_max_demand.apparent   =    energy_log.phase_r.present_max_demand.apparent +
                                                        energy_log.phase_y.present_max_demand.apparent +
                                                        energy_log.phase_b.present_max_demand.apparent;
    
    EM_ResetMaxDemand(SRC_ALL, LINE_PHASE_R);       // Clear Max Demand Value for next month accumulate  
    EM_ResetMaxDemand(SRC_ALL, LINE_PHASE_Y);       // Clear Max Demand Value for next month accumulate  
    EM_ResetMaxDemand(SRC_ALL, LINE_PHASE_B);       // Clear Max Demand Value for next month accumulate  
    
    // Log energy total tariff  
    energy_log.phase_r.present_energy_total_tariff.active   = EM_GetActiveEnergyTotal(LINE_PHASE_R);
    energy_log.phase_r.present_energy_total_tariff.reactive = EM_GetReactiveEnergyTotal(LINE_PHASE_R);
    energy_log.phase_r.present_energy_total_tariff.apparent = EM_GetApparentEnergyTotal(LINE_PHASE_R);
                                                       
    energy_log.phase_y.present_energy_total_tariff.active   = EM_GetActiveEnergyTotal(LINE_PHASE_Y);
    energy_log.phase_y.present_energy_total_tariff.reactive = EM_GetReactiveEnergyTotal(LINE_PHASE_Y);
    energy_log.phase_y.present_energy_total_tariff.apparent = EM_GetApparentEnergyTotal(LINE_PHASE_Y);
    
    energy_log.phase_b.present_energy_total_tariff.active   = EM_GetActiveEnergyTotal(LINE_PHASE_B);
    energy_log.phase_b.present_energy_total_tariff.reactive = EM_GetReactiveEnergyTotal(LINE_PHASE_B);
    energy_log.phase_b.present_energy_total_tariff.apparent = EM_GetApparentEnergyTotal(LINE_PHASE_B);
    
    energy_log.total.present_energy_total_tariff.active   =     energy_log.phase_r.present_energy_total_tariff.active +
                                                                energy_log.phase_y.present_energy_total_tariff.active +
                                                                energy_log.phase_b.present_energy_total_tariff.active;
                                                            
    energy_log.total.present_energy_total_tariff.reactive   =   energy_log.phase_r.present_energy_total_tariff.reactive +
                                                                energy_log.phase_y.present_energy_total_tariff.reactive +
                                                                energy_log.phase_b.present_energy_total_tariff.reactive;
                                                            
    energy_log.total.present_energy_total_tariff.apparent   =   energy_log.phase_r.present_energy_total_tariff.apparent +
                                                                energy_log.phase_y.present_energy_total_tariff.apparent +
                                                                energy_log.phase_b.present_energy_total_tariff.apparent;
    
    // Log Energy Tariff  
    number_tariff = EM_GetConfig().number_tariff;
    for (i = 0; i < number_tariff; i++)
    {
        // Phase_R  
        EM_GetActiveEnergyTariff(i, &energy_log.phase_r.present_energy_tariff[i].active, LINE_PHASE_R);
        EM_GetReactiveEnergyTariff(i, &energy_log.phase_r.present_energy_tariff[i].reactive, LINE_PHASE_R);
        EM_GetApparentEnergyTariff(i, &energy_log.phase_r.present_energy_tariff[i].apparent, LINE_PHASE_R);
        
        // Phase_Y  
        EM_GetActiveEnergyTariff(i, &energy_log.phase_y.present_energy_tariff[i].active, LINE_PHASE_Y);
        EM_GetReactiveEnergyTariff(i, &energy_log.phase_y.present_energy_tariff[i].reactive, LINE_PHASE_Y);
        EM_GetApparentEnergyTariff(i, &energy_log.phase_y.present_energy_tariff[i].apparent, LINE_PHASE_Y);
        
        // Phase_B  
        EM_GetActiveEnergyTariff(i, &energy_log.phase_b.present_energy_tariff[i].active, LINE_PHASE_B);
        EM_GetReactiveEnergyTariff(i, &energy_log.phase_b.present_energy_tariff[i].reactive, LINE_PHASE_B);
        EM_GetApparentEnergyTariff(i, &energy_log.phase_b.present_energy_tariff[i].apparent, LINE_PHASE_B);
        
        energy_log.total.present_energy_tariff[i].active    =   energy_log.phase_r.present_energy_tariff[i].active + 
                                                                energy_log.phase_y.present_energy_tariff[i].active +
                                                                energy_log.phase_b.present_energy_tariff[i].active;
                                                                
        energy_log.total.present_energy_tariff[i].reactive  =   energy_log.phase_r.present_energy_tariff[i].reactive + 
                                                                energy_log.phase_y.present_energy_tariff[i].reactive +
                                                                energy_log.phase_b.present_energy_tariff[i].reactive;
                                                                
        energy_log.total.present_energy_tariff[i].apparent  =   energy_log.phase_r.present_energy_tariff[i].apparent + 
                                                                energy_log.phase_y.present_energy_tariff[i].apparent +
                                                                energy_log.phase_b.present_energy_tariff[i].apparent;
        
    }
    
    // Assign the pointer to acquired value  
    *p_energy_log = energy_log;
*/    
    // Success  
    return STORAGE_OK;
}

/***********************************************************************************************************************
* Function Name    : uint8_t STORAGE_GetTamperLog(ONE_TAMPER_DATA_LOG *p_tamper_log)
* Description      : Storage Add Energy Log Record
* Arguments        : ONE_MONTH_ENERGY_DATA_LOG *p_energy_log: Buffer of a energy log record
* Functions Called : STORAGE_ReadEEPROMHeader()
*                  : EPR_Write()
*                  : STORAGE_UpdateEEPROMHeader()
* Return Value     : Execution Status
*                  :    STORAGE_OK      Add successfull
*                  :    STORAGE_ERROR   Error occurred, NULL parameter
*                  :                    Or, Storage device is not initialized or not formatted, or R/W error
***********************************************************************************************************************/
uint8_t STORAGE_GetTamperLog(ONE_TAMPER_DATA_LOG *p_tamper_log)
{
/*	
    rtc_calendarcounter_value_t rtctime;        // RTC data  
    // For Tamper event  
    EM_STATUS                   em_status;      // EM Status  
    ONE_TAMPER_DATA_LOG         tamper_log;
    
    R_RTC_Get_CalendarCounterValue(&rtctime);

    // zero all bytes of tamper_log buffer  
    memset((uint8_t *)&tamper_log, 0, sizeof(ONE_TAMPER_DATA_LOG));

    // Update RTC time to tamper event  
    tamper_log.tamper_rtc_time.Sec   = rtctime.rseccnt;
    tamper_log.tamper_rtc_time.Min   = rtctime.rmincnt;
    tamper_log.tamper_rtc_time.Hour  = rtctime.rhrcnt;
    tamper_log.tamper_rtc_time.Day   = rtctime.rdaycnt;
    tamper_log.tamper_rtc_time.Week  = rtctime.rwkcnt;
    tamper_log.tamper_rtc_time.Month = rtctime.rmoncnt;
    tamper_log.tamper_rtc_time.Year  = (uint8_t)rtctime.ryrcnt;
    
    // Update tamper status to tamper event  
    em_status = EM_GetStatus();
    tamper_log.tamper_type.details.is_neutral_missing        = em_status.tampers.details.is_neutral_missing;
    tamper_log.tamper_type.details.is_missing_potential_R    = em_status.tampers.details.is_missing_potential_R;
    tamper_log.tamper_type.details.is_missing_potential_Y    = em_status.tampers.details.is_missing_potential_Y;
    tamper_log.tamper_type.details.is_missing_potential_B    = em_status.tampers.details.is_missing_potential_B;
    tamper_log.tamper_type.details.is_voltage_unbalance      = em_status.tampers.details.is_voltage_unbalance;
    tamper_log.tamper_type.details.is_reverse_current_R      = em_status.tampers.details.is_reverse_current_R;
    tamper_log.tamper_type.details.is_reverse_current_Y      = em_status.tampers.details.is_reverse_current_Y;
    tamper_log.tamper_type.details.is_reverse_current_B      = em_status.tampers.details.is_reverse_current_B;
    tamper_log.tamper_type.details.is_open_current_R         = em_status.tampers.details.is_open_current_R;
    tamper_log.tamper_type.details.is_open_current_Y         = em_status.tampers.details.is_open_current_Y;
    tamper_log.tamper_type.details.is_open_current_B         = em_status.tampers.details.is_open_current_B;
    tamper_log.tamper_type.details.is_current_unbalance      = em_status.tampers.details.is_current_unbalance;
    tamper_log.tamper_type.details.is_current_bypass         = em_status.tampers.details.is_current_bypass;

    tamper_log.tamper_type.details.case_open                 = EVENT_IsCaseOpen();
    tamper_log.tamper_type.details.magnet                    = EVENT_IsMagnet();
    
		tamper_log.tamper_type.details.power_failed              = EVENT_IsPowerFail();
    
    // Assign the pointer to acquired value  
    p_tamper_log->tamper_rtc_time = tamper_log.tamper_rtc_time;
    p_tamper_log->tamper_type.value = tamper_log.tamper_type.value;
*/    
    // Success  
    return STORAGE_OK;
}
