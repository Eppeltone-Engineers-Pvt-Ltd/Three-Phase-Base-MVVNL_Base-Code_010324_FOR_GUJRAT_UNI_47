/***********************************************************************************************************************
* File Name    : startup.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Start-up source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "iodefine.h"
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_sau.h"           /* Serial Driver */
#include "r_cg_lcd.h"           /* LCD Driver */
#include "r_cg_rtc.h"           /* RTC Driver */
#include "r_cg_intp.h"          /* INTP Driver */
#include "r_cg_dsadc.h"
#include "r_cg_adc.h"
#include "r_cg_tau.h"
#include "r_cg_lvd.h"
#include "r_cg_vbatt.h"
#include "r_cg_iica.h"
#include "r_cg_wdt.h"
#include "p_timer.h"
#include "string.h"
/* Wrapper/User */
#include "wrp_em_adc_data.h"
#include "wrp_em_timer.h"
/* MW/Core */
#include "em_core.h"        /* EM Core APIs */

/* Application */
#include "platform.h"       /* Default Platform Information Header */
#include "key.h"            /* Key Interface */
#include "led.h"            /* LED Interface */
#include "r_lcd_display.h"            /* LCD Interface */
#include "storage.h"        /* Storage Device */
#include "config_storage.h"    /* Configuration on chip */
#include "startup.h"        /* Startup Header File */
#include "event.h"          /* Event Header File */
#include "mlsCIApp.h"
#include "emeter3_structs.h"
#include "pravakComm.h"
#include "DLMS_User.h"		// for DLMS

/* Debug Only */
#include "debug.h"          /* Debug */
#include "lvm.h"
#include "powermgmt.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define PHM_INIT_OK         0   /* Initial OK */
#define PHM_INIT_ERROR      1   /* Initial Error */
#define PHM_START_OK        0   /* Start OK */
#define PHM_START_ERROR     1   /* Start Error */

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/
uint8_t g_reset_flag;
uint8_t g_softreset_flag;

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#ifdef __DEBUG
static const uint8_t line_break[]   = "+------------------------------------------------------------------------+\n\r";
static const uint8_t param_intro[] = "| %-21s %26s %-21s |\n\r";
static const uint8_t param_text[] = "|     %-67s|\n\r";
static const uint8_t param_info_s[] = "|      . %-22s: %-40s|\n\r";
static const uint8_t param_info_i[] = "|      . %-22s: %-40d|\n\r";
static const uint8_t param_info_date[] = "|      . %-22s: %02x/%02x/20%02x %-29s|\n\r";


static const uint8_t param_itemgroup_start[] = "| %2d. %-54s             |\n\r";
static const uint8_t param_item_start[] = "| %2d. %-51s ";
static const uint8_t param_subitem_start[] = "|     . %-49s ";
static const uint8_t param_subitem_info_start[] = "|     . %-21s ";
static const uint8_t param_subitem_info_middle_rtc[] = "%02x/%02x/20%02x %02x:%02x:%02x %-7s ";
static const uint8_t param_item_end_normal[] = "\x1b[32m[%12s]\x1b[0m |\n\r";
static const uint8_t param_item_end_abnormal[] = "\x1b[31m[%12s]\x1b[0m |\n\r";

#else
static const uint8_t line_break[];
static const uint8_t param_intro[];
static const uint8_t param_text[];
static const uint8_t param_info_s[];
static const uint8_t param_info_i[] ;
static const uint8_t param_info_date[] ;


static const uint8_t param_itemgroup_start[] ;
static const uint8_t param_item_start[];
static const uint8_t param_subitem_start[] ;
static const uint8_t param_subitem_info_start[] ;
static const uint8_t param_subitem_info_middle_rtc[];
static const uint8_t param_item_end_normal[];
static const uint8_t param_item_end_abnormal[];
#endif

static uint8_t phminit(void);
static uint8_t phmstart(void);
static uint8_t meter_data_load(uint8_t init_status);
uint8_t config_data_load(uint8_t init_status);

//extern powermgmt_mode_t g_powermgmt_current_mode;

//#define TEST_STARTUP


/***********************************************************************************************************************
* Function Name    : static uint8_t phminit(void)
* Description      : Initial Peripheral device & module
* Arguments        : None
* Functions Called : EM_IEC1107_Init()
* Return Value     : Execution Status
*                  :    PHM_INIT_OK             Initial peripheral OK
*                  :    PHM_INIT_ERROR          Initial peripheral error
***********************************************************************************************************************/
static uint8_t phminit(void)
{
    /* Currently, we do nothing here, just return OK
     * because all module is initialized by the hdwinit(), on CG_systeminit.c 
     * When change MCU, need to implement this API */
//    DEBUG_Init();
//    DEBUG_Start();
    
    // Renesas Protocol Initialization 

// Commenting the three lines below will stop the GUI from working - please do as needed	
//    #ifndef __DEBUG
//    R_CI_Init();
//    #endif
    
// PRAVAK Communication Initialization
//#ifndef USE_DLMS- commented on 13-Mar-2019	
	PVK_Comm_Init();	// This function starts the UART2 or UART3
//#endif

    /* Success */
    return PHM_INIT_OK;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t phmstart(void)
* Description      : Start Peripheral device & module
* Arguments        : None
* Functions Called : //DEBUG_Printf()
*                  : RTC_CounterEnable()
*                  : RTC_CounterGet()
*                  : INTP0_Enable()
*                  : KEY_Enable()
*                  : EM_IEC1107_Start()
* Return Value     : Execution Status
*                  :    PHM_START_OK            Start peripheral OK
*                  :    PHM_START_ERROR         Start peripheral error
***********************************************************************************************************************/
static uint8_t phmstart(void)
{
		uint8_t rtcStatus;	
    uint8_t                         status;
    uint16_t                        timeout;
    rtc_calendarcounter_value_t     rtctime;

    // Start & check RTC 
    ////DEBUG_Printf((uint8_t *)param_subitem_info_start,"Start RTC module");

    rtcStatus = R_RTC_Start();  
		if(rtcStatus==0)
		{// rtc is OK
    	R_RTC_Set_ConstPeriodInterruptOn(PES_1_SEC);
    
	    // Check RTC module *
	    R_RTC_Get_CalendarCounterValue(&rtctime);
	    status = MD_OK; 
	    if (status != MD_OK)    // RTC start OK?
	    {       
	        #ifdef __DEBUG
	        memset(&rtctime,0, sizeof(rtc_calendarcounter_value_t));
	        //DEBUG_Printf((uint8_t *)param_subitem_info_middle_rtc, 
	                        rtctime.rdaycnt,
	                        rtctime.rmoncnt,
	                        rtctime.ryrcnt,
	                        rtctime.rhrcnt,
	                        rtctime.rmincnt,
	                        rtctime.rseccnt,
	                        " ");
	        //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");
	        #endif
	        // When RTC can start, we need to beak the start-up process here
	          //because the EM can't run without RTC
	        return PHM_START_ERROR;
	    }
	    else
	    {
	        // RTC start success, get & display initial time of module
	        //DEBUG_Printf((uint8_t *)param_subitem_info_middle_rtc,rtctime.rdaycnt,rtctime.rmoncnt,rtctime.ryrcnt,rtctime.rhrcnt,rtctime.rmincnt,rtctime.rseccnt," ");
	        //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
	    }
		}
// since RTC_SUPPLY is always made 1 - this case must never be encountered		
/*		
		else
		{
			rtcFailed = 1;
			P_TIMER_Start();
		}
*/
    // Start other peripheral modules
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Start LCD module"); 
    R_LCD_PowerOn();
    //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");

	
	// PRAVAk Protocol Start
//	PVK_Comm_Start();

	
// DLMS Protocol
//	DLMS_Initialize();

#ifdef USE_DLMS
	DLMS_Initialize();	// one wonders if this function starts the UART or not?
#else
	PVK_Comm_Start();	// This function body contains nothing
#endif


    // Renesas Protocol Start
// The following block will need to be uncommented for GUI to work
/*	
	
    #ifndef __DEBUG
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Start Renesas Protocol"); 
    R_CI_Start();
    //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
    #endif
*/
    // Start interrupt port
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Start INTP module");
    R_INTC0_Start();
//    R_INTC1_Start();
    //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");

		
#ifdef POWER_MANAGEMENT_ENABLE    
    // Start LVD Detection
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Start LVD Detection");
//    R_LVDVDD_Start();	// meter is now always having VDD - hence not needed
    R_LVDVRTC_Start();
    R_LVDEXLVD_Start();
//    R_LVDVBAT_Start();
    //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");    
    
    // Start Battery backup function
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Start backup battery");
    R_VBATT_SetOperationOn();
    //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
#else    
    // Turn off VBAT module in case it already turned on
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Turn off backup battery");
    R_VBATT_SetOperationOff();
    //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
    
#endif

    // Success 
    return PHM_START_OK;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t meter_data_load(uint8_t init_status)
* Description      : Load Meter operation data from EEPROM memory
* Arguments        : uint8_t init_status: previous initialization status
* Return Value     : Execution Status
*                  :    CONFIG_OK         Load data OK
*                  :    CONFIG_ERROR         Load data error
***********************************************************************************************************************/
static uint8_t meter_data_load(uint8_t init_status)
{
	
    // Check device format to get out data from EEPROM
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Check meter data format:");
    // Check device format *
    if (init_status == STORAGE_OK)       // Initial successful, already formatted
    {
        //DEBUG_Printf((uint8_t *)param_item_end_normal,"FORMATTED");
        
        //DEBUG_Printf((uint8_t *)param_subitem_start,"Restore library data from EEPROM:");
        
        if (STORAGE_Restore(STORAGE_ITEM_SYS_STATE) != STORAGE_OK)
        {
            //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");
            
            //DEBUG_Printf((uint8_t *)param_text, "Warning: All counter (energy, max demand, etc) reset");
            
            //DEBUG_Printf((uint8_t *)param_subitem_start,"Re-backup system state:");
            if (STORAGE_Backup(STORAGE_ITEM_SYS_STATE) != STORAGE_OK)
            {
                //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");
                return STORAGE_ERROR;
            }
            else
            {
                //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
            }
        }
        else
        {
            //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
        }
    }
    // Initial successful, but not formatted *
    else if (init_status == STORAGE_NOT_FORMATTED)
    {
        //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"UNFORMATTED");
        //DEBUG_Printf((uint8_t *)param_subitem_start, "Formatting device");
        
        // Format device *
        if (STORAGE_Format() != STORAGE_OK)
        {
            //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");

            // When format fail,
              //we need to beak the start-up process here  //    
            return STORAGE_ERROR;   // Format fail
        }       
        else    // format ok *
        {
            //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
            
            //DEBUG_Printf((uint8_t *)param_subitem_start, "Start first time backup meter data to EEPROM");
            
            // First backup whole system to EEPROM *
            if (STORAGE_Backup(STORAGE_ITEM_ALL) != STORAGE_OK)
            {
                //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");
                return STORAGE_ERROR;   // Fisrt backup not success *
            }
            else
            {
                //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
            }
        }
    }
    
    // Load data sucessfully
    return STORAGE_OK;
}
/***********************************************************************************************************************
* Function Name    : static uint8_t config_data_load(uint8_t init_status)
* Description      : Load Configuration Page from MCU DataFlash memory
* Arguments        : uint8_t init_status: initialization status
* Return Value     : Execution Status
*                  :    CONFIG_OK         Load data OK
*                  :    CONFIG_ERROR         Load data error
***********************************************************************************************************************/
uint8_t config_data_load(uint8_t init_status)
{
	
    // Check device format to get out config data
    //DEBUG_Printf((uint8_t *)param_subitem_start,"Check MCU Configuration Page");
    
    // Check device format
    if (init_status == CONFIG_OK)       // Initial successful, already formatted
    {
      if (CONFIG_Restore(CONFIG_ITEM_CONFIG) != CONFIG_OK)
      {
        NOP();
      }
      else
      {
        NOP();
      }
/*			
      if (CONFIG_Restore(CONFIG_ITEM_CALIB) != CONFIG_OK)
      {
        NOP();
      }
      else
      {
        NOP();
      }
*/			
    }
    // Initial successful, but not formatted 
    else if (init_status == CONFIG_NOT_FORMATTED)
    {
        //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"UNFORMATTED");
        //DEBUG_Printf((uint8_t *)param_subitem_start,"Formatting config data");
        
        // Format device *
        if (CONFIG_Format() != CONFIG_OK)
        {
            //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");

            // When format fail,
              //we need to beak the start-up process here  //    
            return CONFIG_ERROR;   // Format fail 
        }       
        else    // format ok *
        {
            //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
            //DEBUG_Printf((uint8_t *)param_subitem_start,"Start first time backup all config data");
            
            // First backup all configuration data *
//            if (CONFIG_Backup(CONFIG_ITEM_ALL) != CONFIG_OK)
            if (CONFIG_Backup(CONFIG_ITEM_CONFIG) != CONFIG_OK)
            {
                //DEBUG_Printf((uint8_t *)param_item_end_abnormal,"FAILED");
                return CONFIG_ERROR;   // Fisrt backup not success *
            }
            else
            {
                //DEBUG_Printf((uint8_t *)param_item_end_normal,"OK");
            }
        }
    }
    
    // Load data sucessfully  
    return CONFIG_OK;
}


uint8_t startup(void)
{
    volatile rtc_calendarcounter_value_t     rtctime;
    volatile uint8_t                         storage_initial_status;
    volatile uint8_t                         config_initial_status;
    PLATFORM_INFO                   platform_info;
    
    /*****************************************************************
     //Initial all
    ******************************************************************/
    // Initial all peripheral & device  
    if (phminit() != PHM_INIT_OK)
    {
        return STARTUP_ERROR;
    }

    // Begin Start-up  
    
    // Check reset flag  
    #define TRAP        0x80
    #define WDTRF       0x10
    #define RPERF       0x04
    #define IAWRF       0x02
    #define LVIRF       0x01
    
    //DEBUG_Printf((uint8_t *)param_item_start, 1, "Check Reset Flag");
    
    g_reset_flag = RESF;
    if (g_reset_flag == 0x00)
    {
        //DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
    }
    else
    {
        //DEBUG_Printf((uint8_t *)param_item_end_abnormal, "ABNORMAL");
        
        if (g_reset_flag & TRAP)
        {
            //DEBUG_Printf((uint8_t *)param_text, " . Illegal instruction Reset");
        }
        if (g_reset_flag & WDTRF)
        {
            //DEBUG_Printf((uint8_t *)param_text, " . WDT Reset");
        }
        if (g_reset_flag & RPERF)
        {
            //DEBUG_Printf((uint8_t *)param_text, " . RAM Parity Error Reset");
        }
        if (g_reset_flag & IAWRF)
        {
            //DEBUG_Printf((uint8_t *)param_text, " . Illegal-memory access reset");
        }
        if (g_reset_flag & LVIRF)
        {
            //DEBUG_Printf((uint8_t *)param_text, " . LVD Reset");
        }
    }

    // At here, Peripheral OK  
    // Initial storage device  
    
    //DEBUG_Printf((uint8_t *)param_item_start, 2, "Initialize meter data storage");

#ifdef TEST_STARTUP
		asendbare((uint8_t *)"Initialise Data Storage\r\n");  	
#endif		
    storage_initial_status = STORAGE_Init();
    
    if (storage_initial_status == STORAGE_ERROR)
    {
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"FAILED\r\n");  	
#endif
        return STARTUP_ERROR;
    }
    else // storage device is initial successful
    {
        //DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"OK\r\n");  	
#endif
		}    
    
    // Initial configuration page  
    //DEBUG_Printf((uint8_t *)param_item_start, 3, "Initialize MCU Configuration Page");
#ifdef TEST_STARTUP
		asendbare((uint8_t *)"Init MCU Configuration\r\n");  	
#endif		
    config_initial_status = CONFIG_Init();
    if (config_initial_status == CONFIG_ERROR)
    {
        //DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"FAILED\r\n");  	
#endif        
        // When CONFIG_Init fail, we need to beak the start-up process here
          //because the meter can't without dataflash memory  
        return STARTUP_ERROR;
    }
    else
    {
        //DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"OK\r\n");  	
#endif
		}

    // Init default DSAD Channel Gain  
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_PHASE_R));
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_PHASE_Y));
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_PHASE_B));
    R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL, R_DSADC_GetGainEnumValue(EM_CALIB_DEFAULT_GAIN_NEUTRAL));
    
    // Init default ADC Timer offset value  
    EM_ADC_SAMP_SetTimerOffset(EM_CALIB_DEFAULT_TIMER_OFFSET);
    
    // Initial EM by default property, configuration, calibration from MCU ROM  
    //DEBUG_Printf((uint8_t *)param_item_start, 4, "Initialize EM Core by default information on MCU");
#ifdef TEST_STARTUP
		asendbare((uint8_t *)"Init EM Core\r\n");  	
#endif
		
		R_TAU0_Channel3_Start();
		if (EM_Init((EM_PLATFORM_PROPERTY *)&g_EM_DefaultProperty,
                (EM_CONFIG *)&g_EM_DefaultConfig,
                (EM_CALIBRATION *)&g_EM_DefaultCalibration) != EM_OK)       // EM initial with default info on MCU OK?  
    {
        //DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"FAILED\r\n");  	
#endif        
        return STARTUP_ERROR;
    }
    else
    {
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"OK\r\n");  	
#endif
    }
    
     //Start all peripheral, device, EM Core
#ifdef TEST_STARTUP
		asendbare((uint8_t *)"Start other peripheral modules\r\n");  	
#endif
		if (phmstart() != PHM_START_OK)
    {
        // When phmstart() fail, we need to beak the start-up process here
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"FAILED\r\n");  	
#endif
			return STARTUP_ERROR;
    }
    else
    {
#ifdef TEST_STARTUP
			asendbare((uint8_t *)"OK\r\n");  	
#endif        
    }
    
    // Load the meter configuration data
    if (config_data_load(config_initial_status) != CONFIG_OK)
    {
        return STARTUP_ERROR;
    }
    
// Config restore of calib data is being done outside of config_data_load function		
    if (CONFIG_Restore(CONFIG_ITEM_CALIB) != CONFIG_OK)
    {
        NOP();
    }
		
#ifdef TEST_STARTUP
		asendbare((uint8_t *)"Start EM Core\r\n");  	
#endif
	
    if(LVDEXLVDF == 0)    //EXLVD VOLTAGE > DETECTION THRESHOLD
    {
			POWERMGMT_Clear_AC_LOW();	// set the g_powermgmt_flag.is_ac_low flag to 0 - ac power is available
    }
    else 
    {
			POWERMGMT_Set_AC_LOW();	// set the g_powermgmt_flag.is_ac_low flag to 1 - no ac power available
    }


    if (POWERMGMT_IsACLow() == 0 )
    {
        if (EM_Start() != EM_OK)
        {
            //DEBUG_Printf((uint8_t *)param_item_end_abnormal, "FAILED");
#ifdef TEST_STARTUP
						asendbare((uint8_t *)"FAILED\r\n");  	
#endif
						return STARTUP_ERROR;
        }
        else
        {
            //DEBUG_Printf((uint8_t *)param_item_end_normal, "OK");
#ifdef TEST_STARTUP
						asendbare((uint8_t *)"OK\r\n");  	
#endif
				}
    }
		else
		{// here there is no power
      POWERMGMT_DoBeforeStop(); // All peripherals off, system will go to STOP mode in polling processing
    	R_INTC0_Stop();	// Stop the pushbutton interrupt
      NOP();
      NOP();
      NOP();
      STOP();
// we will exit this mode only when a power up interrupt comes
// what hapens if a key has been pressed - taken care by disabling interrupt above
// should the watchdog counting be disabled in STOP mode? - NOT necessary since watchdog does not count in SLEEP/HALT
// Option byte settings are 7E, 39 and E0
      NOP();
      NOP();
      NOP();

			while(1);	// force a reset - when power returns
			
//      R_WDT_Restart();
//			POWERMGMT_ProcessMode1();
//			POWERMGMT_setCurrentMode1();
		}
    
    DEBUG_AckNewLine(); 
    return STARTUP_OK;
}
