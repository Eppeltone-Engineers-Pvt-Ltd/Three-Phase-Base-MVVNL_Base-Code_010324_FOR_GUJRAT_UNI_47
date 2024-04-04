/***********************************************************************************************************************
* File Name    : command.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Command processing source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "iodefine.h"
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_rtc.h"           /* RTC Driver */
#include "r_cg_adc.h"           /* ADC Driver */
#include "r_cg_dsadc.h"         /* DSAD Driver */
#include "r_cg_wdt.h"           /* WDT Driver */

/* Wrapper */
#include "wrp_em_sw_config.h"   /* EM Software Config */
#include "wrp_em_adc.h"
#include "wrp_em_adc_data.h"

/* MW/Core, Common */
#include "em_core.h"            /* EM Core APIs */
#include "em_conversion.h"      /* EM Conversion APIs */
#include "em_maths.h"           /* EM Maths APIs */

/* Application */
#include "storage.h"        /* Storage Header File */
#include "event.h"          /* Event Header File */
#include "r_calib.h"        /* Auto Calibration Module */
#include "config_storage.h"    /* Config Storage Header File */
#include "lvm.h"
#include "platform.h"

/* Debug Only */
#ifdef __DEBUG
#include <stdio.h>          /* Standard IO */
#include <stdarg.h>         /* Variant argument */
#include <string.h>         /* String Standard Lib */
#include <math.h>           /* Mathematic Standard Lib */
#include <limits.h>         /* Number limits definitions */
#include "cmd.h"            /* CMD Prompt Driver */


#ifdef METER_ENABLE_MEASURE_CPU_LOAD
#include "load_test.h"
#endif

#endif

#ifdef __DEBUG

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Command Invoker Function */
typedef uint8_t (*FUNC)(uint8_t *arg_str);

/* Command Item */
typedef struct tagCommandTable
{
    const uint8_t   *cmd_name;
    const uint8_t   *params;
    const uint8_t   *description;
    const FUNC      func;
} COMMAND_ITEM;

/* Measured data */
typedef struct tagMeasuredData
{
    float32_t       vrms;
    float32_t       irms;
    float32_t       power_active;
    float32_t       power_reactive;
    float32_t       power_apprent;
    float32_t       fvrms;
    float32_t       firms;
    float32_t       power_factive;
    float32_t       voltage_thd;
    float32_t       current_thd;
    float32_t       active_thd;
    float32_t       pf;
    EM_PF_SIGN      pf_sign;
    float32_t       fac;
    float32_t       energy_total_active;
    float32_t       energy_total_reactive;
    float32_t       energy_total_apparent;
    float32_t       mdm_active;
    float32_t       mdm_reactive;
    float32_t       mdm_apparent;
    int32_t         offset_v;
    int32_t         offset_i;
} COMMAND_MEASURED_DATA;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Common macro */
#define     _BCD2DEC(x)     {x = (x & 0xF) + (x >> 4) * 10;}                /* 1 byte BCD to DEC Conversion */
#define     _DEC2BCD(x)     {x = (x % 10 ) | (((x / 10) << 4) & 0xF0);}     /* 1 byte DEC to BCD Conversion */

/* Command Status */
#define COMMAND_OK          0
#define COMMAND_ERROR       1

/* Command Invoker Statement & Table Length */
#define INVOKE_COMMAND(index, arg_str)  {                                                   \
                                            if (cmd_table[index].func != NULL)              \
                                            {                                               \
                                                (*cmd_table[index].func)(arg_str);          \
                                            }                                               \
                                        }
#define COMMAND_TABLE_LENGTH    (sizeof(cmd_table) / sizeof(COMMAND_ITEM))

#define NUM_OF_DSAD_CHANNEL             (4)



/***********************************************************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Static variables */
static COMMAND_MEASURED_DATA g_em_core_data[EM_CALC_NUM_OF_LINE];

/* Sub-function (static for module) */
static uint8_t * COMMAND_GetScanOneParam(
    uint8_t *output,
    uint16_t size,
    uint8_t *input,
    uint8_t *ahead_str,
    uint8_t *break_str
);
static uint16_t  COMMAND_ConvertFromHexString(uint8_t *str, uint8_t *b_found);

/* Command Invoker */
static uint8_t COMMAND_InvokeUsage(uint8_t *arg_str);                           /* Usage */
static uint8_t COMMAND_InvokeRestartEM(uint8_t *arg_str);                       /* Restart EM */
static uint8_t COMMAND_InvokeRtc(uint8_t *arg_str);                             /* RTC */
static uint8_t COMMAND_InvokeSetRtc(uint8_t *arg_str);                          /* Set RTC */
static uint8_t COMMAND_InvokeBackup(uint8_t *arg_str);                          /* Backup */
static uint8_t COMMAND_InvokeRestore(uint8_t *arg_str);                         /* Restore */
static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str);                      /* Read EEPROM */
static uint8_t COMMAND_InvokeWriteMemory(uint8_t *arg_str);                     /* Write EEPROM */
static uint8_t COMMAND_InvokeAddEnergyLog(uint8_t *arg_str);                    /* Add energy log */
static uint8_t COMMAND_InvokeAddTamperLog(uint8_t *arg_str);                    /* Add tamper log */
static uint8_t COMMAND_InvokeFormatMemory(uint8_t *arg_str);                    /* Format EEPROM */
static uint8_t COMMAND_InvokeReadPlatformInfo(uint8_t *arg_str);                /* Read Platform Info */
static uint8_t COMMAND_InvokeReadLVMFlag(uint8_t *arg_str);                    /* Write LVM Flag */
static uint8_t COMMAND_InvokeWriteLVMFlag(uint8_t *arg_str);                    /* Write LVM Flag */
static uint8_t COMMAND_InvokeReadDTFL(uint8_t *arg_str);                        /* Read DataFlash Directly */

static uint8_t COMMAND_InvokeDisplay(uint8_t *arg_str);                         /* Display */
static uint8_t COMMAND_InvokeDumpSample(uint8_t *arg_str);                      /* Dump waveform samples */
static uint8_t COMMAND_InvokeCalibration(uint8_t *arg_str);                     /* Calibration */
static uint8_t COMMAND_InvokeSetCalibInfo(uint8_t *arg_str);                    /* Set calibration info */


static uint8_t COMMAND_InvokeDSADPowerGetSet(uint8_t *arg_str);                 /* Get,set DSAD channel power state */
static uint8_t COMMAND_InvokeDSADPhaseGetSet(uint8_t *arg_str);                 /* Get,set DSAD phase delay */
static uint8_t COMMAND_InvokeDSADHpfGetSet(uint8_t *arg_str);                   /* Get,set DSAD HPF information */
static uint8_t COMMAND_InvokeDSADGainGetSet(uint8_t *arg_str);                  /* Get,set DSAD gain */
static uint8_t COMMAND_InvokeDSTimerGetSet(uint8_t *arg_str);                   /* Get,set double sampling timer value */
static uint8_t COMMAND_InvokeSetFstepOffset(uint8_t *arg_str);
static uint8_t COMMAND_InvokeSetAutoFstep(uint8_t *arg_str);
static uint8_t COMMAND_InvokeCPULoad(uint8_t *arg_str);

/* Command Table */
static const COMMAND_ITEM   cmd_table[] = 
{
    /* ---------------------------------------------------------------------------------------------------------------------*/
    /* Command Name     Parameter                   Description                                     Func Pointer */
    /* ---------------------------------------------------------------------------------------------------------------------*/
    {(const uint8_t *)"?"               ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Help"                                                         ,   COMMAND_InvokeUsage             },
    {(const uint8_t *)"cls"             ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Clear screen"                                                 ,   NULL                            },      /* CMD module processed it internally */
    {(const uint8_t *)"display"         ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Display current measured data"                                ,   COMMAND_InvokeDisplay           },  
    {(const uint8_t *)"restart"         ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Restart EM"                                                   ,   COMMAND_InvokeRestartEM         },  
    {(const uint8_t *)"rtc"             ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Display current RTC time"                                     ,   COMMAND_InvokeRtc               },
    {(const uint8_t *)"setrtc"          ,   (const uint8_t *)"dd/mm/yy hh:mm:ss ww"                     ,   (const uint8_t *)"Set RTC time"                                                 ,   COMMAND_InvokeSetRtc            },
    {(const uint8_t *)"backup"          ,   (const uint8_t *)"selection"                                ,   (const uint8_t *)"Backup to memory"                                             ,   COMMAND_InvokeBackup            },
    {(const uint8_t *)"restore"         ,   (const uint8_t *)"selection"                                ,   (const uint8_t *)"Restore from memory"                                          ,   COMMAND_InvokeRestore           },
    {(const uint8_t *)"readmem"         ,   (const uint8_t *)"type(0:EEP,1:DTFL) addr size [cast]"      ,   (const uint8_t *)"Read memory type at addr, size, display value"                ,   COMMAND_InvokeReadMemory        },
    {(const uint8_t *)"writemem"        ,   (const uint8_t *)"type(0:EEP,1:DTFL) addr size value"       ,   (const uint8_t *)"Write memory type at addr, size with value"                   ,   COMMAND_InvokeWriteMemory       },
    {(const uint8_t *)"readlvmflag"     ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Read LogicalVolumeManager flag"                               ,   COMMAND_InvokeReadLVMFlag       },
    {(const uint8_t *)"writelvmflag"    ,   (const uint8_t *)"is_updating bank1_in_use bank2_in_use"    ,   (const uint8_t *)"Write LogicalVolumeManager flag"                              ,   COMMAND_InvokeWriteLVMFlag      },
    {(const uint8_t *)"readdtfl"        ,   (const uint8_t *)"addr size [cast]"                         ,   (const uint8_t *)"Read DataFlash directly"                                      ,   COMMAND_InvokeReadDTFL          },
    {(const uint8_t *)"addenergylog"    ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Add an energy log record to EEPROM"                           ,   COMMAND_InvokeAddEnergyLog      },
    {(const uint8_t *)"addtamperlog"    ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Add an tamper log record to EEPROM"                           ,   COMMAND_InvokeAddTamperLog      },
    {(const uint8_t *)"formatmem"       ,   (const uint8_t *)"type(0:EEP,1:DTFL)"                       ,   (const uint8_t *)"Format memory type (followed format.h)"                       ,   COMMAND_InvokeFormatMemory      },
    {(const uint8_t *)"readinfo"        ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Read platform info"                                           ,   COMMAND_InvokeReadPlatformInfo  },
    {(const uint8_t *)"dump"            ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Dump waveform from EM Core"                                   ,   COMMAND_InvokeDumpSample        },
    {(const uint8_t *)"foffset"         ,   (const uint8_t *)"offset value"                             ,   (const uint8_t *)"Set fstep offset value (in float)"                            ,   COMMAND_InvokeSetFstepOffset    },  
    {(const uint8_t *)"autofstep"       ,   (const uint8_t *)"0:disable, 1:enable"                      ,   (const uint8_t *)"Set the auto Fstep mode"                                      ,   COMMAND_InvokeSetAutoFstep      },  
    {(const uint8_t *)"calib"           ,   (const uint8_t *)"c cp imax v i w "                         ,   (const uint8_t *)"Calibrate (cycle,cycle_phase,imax,V,I,phase_r)"               ,   COMMAND_InvokeCalibration       },
    {(const uint8_t *)"setcalib"        ,   (const uint8_t *)"get:leave empty; set:1 later follow guide",   (const uint8_t *)"Manually set calib info"                                      ,   COMMAND_InvokeSetCalibInfo      },    
    {(const uint8_t *)"dspower"         ,   (const uint8_t *)"get:leave empty; set:ch state(1/0) "      ,   (const uint8_t *)"Get, Set DSAD Channel Power"                                  ,   COMMAND_InvokeDSADPowerGetSet   },
    {(const uint8_t *)"dsphase"         ,   (const uint8_t *)"get:leave empty; set:ch step "            ,   (const uint8_t *)"Get, Set DSAD Phase Delay"                                    ,   COMMAND_InvokeDSADPhaseGetSet   },
    {(const uint8_t *)"dshpf"           ,   (const uint8_t *)"get:leave empty; set:ch state(1/0) cutoff",   (const uint8_t *)"Get, Set DSAD HPF state"                                      ,   COMMAND_InvokeDSADHpfGetSet     },
    {(const uint8_t *)"dsgain"          ,   (const uint8_t *)"get:leave empty; set:ch gain"             ,   (const uint8_t *)"Get, Set DSAD Gain Setting"                                   ,   COMMAND_InvokeDSADGainGetSet    },
    {(const uint8_t *)"dstimer"         ,   (const uint8_t *)"get:leave empty; set:timer value(16bit)"  ,   (const uint8_t *)"Get, Set Double Sampling Timer"                               ,   COMMAND_InvokeDSTimerGetSet     },
    {(const uint8_t *)"cpuload"         ,   (const uint8_t *)""                                         ,   (const uint8_t *)"Measure the CPU Load"                                         ,   COMMAND_InvokeCPULoad           },
    
};

static const uint8_t * g_mem_epr = (const uint8_t *)"EEPROM";
static const uint8_t * g_mem_dtfl = (const uint8_t *)"DATAFLASH";
static const uint8_t g_hpf_type[4][8] = {
    "0.607Hz",
    "1.214Hz",
    "2.429Hz",
    "4.857Hz"
};

///******************************************************************************
//* Function Name    : void matherr(struct exception *x)
//* Description      : Mathematic error exception callback
//* Arguments        : struct exception *x
//* Functions Called : None
//* Return Value     : None
//******************************************************************************/
//void matherr(struct exception *x)
//{
//  g_matherr_type = x->type;
//}

/******************************************************************************
* Function Name : COMMAND_GetUnsigned16
* Interface     : static uint16_t COMMAND_GetUnsigned16(uint8_t *buffer)
* Description   : Get unsigned int 16 bit from buffer
* Arguments     : uint8_t * buffer: Buffer to cast to uint16_t
* Function Calls: None
* Return Value  : static uint16_t
******************************************************************************/
static uint16_t COMMAND_GetUnsigned16(uint8_t *buffer)
{
    uint16_t u16;
    u16  = (uint16_t)buffer[0];
    u16 |= (uint16_t)buffer[1] << 8;
    
    return (u16);
}

/******************************************************************************
* Function Name : COMMAND_GetUnsigned32
* Interface     : static uint32_t COMMAND_GetUnsigned32(uint8_t *buffer)
* Description   : Get unsigned long 32 bit from buffer
* Arguments     : uint8_t * buffer: Buffer to cast to uint32_t
* Function Calls: None
* Return Value  : static uint32_t
******************************************************************************/
static uint32_t COMMAND_GetUnsigned32(uint8_t *buffer)
{
    uint32_t u32;
    u32  = (uint32_t)buffer[0];
    u32 |= (uint32_t)buffer[1] << 8;
    u32 |= (uint32_t)buffer[2] << 16;
    u32 |= (uint32_t)buffer[3] << 24;
    
    return (u32);
}

/***********************************************************************************************************************
* Function Name : COMMAND_IsFloatNumber
* Interface     : static uint8_t COMMAND_IsFloatNumber(uint8_t *buffer)
* Description   : Check whether the input buffer is number or not
* Arguments     : uint8_t * buffer: Buffer to check
* Function Calls: None
* Return Value  : static uin8t_t: 1 if buffer is number or 0 if buffer contain characters
***********************************************************************************************************************/
static uint8_t COMMAND_IsFloatNumber(uint8_t *buffer)
{
    /* Note this function not check the NULL buffer */
    uint8_t dot_count = 0;
    uint8_t plus_count = 0;
    uint8_t minus_count = 0;
    
    while(*buffer != 0)
    {
        if ((*buffer >= '0' && *buffer <='9') || (*buffer == '.') 
            || (*buffer == '+') || (*buffer == '-'))
        {
            /* Check number of '.' sign */
            if (*buffer == '.')
            {
                dot_count++;
                if (dot_count > 1)
                {
                    return 0;   
                }
            }
            
            /* Check number of '+' sign */
            if (*buffer == '+')
            {
                plus_count++;
                if (plus_count > 1)
                {
                    return 0;   
                }
            }
            
            /* Check number of '-' sign */
            if (*buffer == '-')
            {
                minus_count++;
                if (minus_count > 1)
                {
                    return 0;   
                }
            }
            buffer++;
        }
        else
        {
            return 0;   
        }
    }
    
    return 1;
}

/******************************************************************************
* Function Name    : static uint8_t *COMMAND_GetScanOneParam(
*                  :     uint8_t *output,
*                  :     uint16_t size,
*                  :     uint8_t *input,
*                  :     uint8_t *ahead_str,
*                  :     uint8_t *break_str
*                  : );
* Description      : Command Get One Parameter
* Arguments        : uint8_t *output: Output buffer
*                  : uint16_t size: Size of output buffer
*                  : uint8_t *input: Input buffer
*                  : uint8_t *ahead_str: Char array to skip the ahead the arg_str
*                  : uint8_t *break_str: Char array to break the scan processing
* Functions Called : None
* Return Value     : Next location of parameter
******************************************************************************/
static uint8_t *COMMAND_GetScanOneParam(uint8_t *output,
                                        uint16_t size,
                                        uint8_t *input,
                                        uint8_t *ahead_str,
                                        uint8_t *break_str)
{
    uint8_t i, j, break_len, ahead_len;
    
    /* Check parameter */
    if (output == NULL || size == 0 ||
        input  == NULL || break_str == NULL)
    {
        return NULL;
    }

    if (ahead_str != NULL && *ahead_str != 0)
    {
        ahead_len = (uint8_t)strlen((char *)ahead_str);
        
        /* skip all ahead char that found on arg_str */
        while (*input != 0)
        {
            for (j = 0; j < ahead_len; j++)
            {
                if (ahead_str[j] == *input)
                    break;
            }
            
            /* is found in break_str? */
            if (j != ahead_len)
            {
                input++;
            }
            else    /* Not found */
            {
                break;  /* Break the outsize loop */
            }
        }
    }
    
    /* Is have remain char? */
    if (*input != 0)
    {   
        /* clear output buffer */
        memset(output, 0, size);
        
        /* Get the lenght of break_str */
        break_len = (uint8_t)strlen((char *)break_str);
    
        /* Scan the parameter */
        i = 0;
        while (i < size && *input != 0)
        {
            for (j = 0; j < break_len; j++)
            {
                if (break_str[j] == *input)
                    break;
            }

            /* is found in break_str?
             * --> end of scanning */
            if (j != break_len)
            {
                break;  /* end of scanning */
            }
            else    /* Not found --> continue to scan */
            {
                /* Update to buffer */
                output[i] = *input;
            
                /* scan next byte */
                input++;
                i++;
            }
        }
        
        return input;
    }
    else
    {
        return NULL;
    }
}

/******************************************************************************
* Function Name    : static uint16_t COMMAND_HexStringToUnsigned16(uint8_t *str)
* Description      : Command Convert Hex String to number
* Arguments        : uint8_t *str: Hex string
* Functions Called : None
* Return Value     : None
******************************************************************************/
static uint16_t COMMAND_HexStringToUnsigned16(uint8_t *str, uint8_t *b_found)
{
    uint16_t    rlt = 0;
    uint8_t     hexnum;
    
    *b_found = 0; /* Not found */
    
    /* Check parameter */
    if (str == NULL)
    {
        return 0;
    }
    
    /* point to 'x' char */
    while (*str != 0 && *str != 'x')
    {
        str++;
    }
    
    /* end string? */
    if (*str == 0 || (*(str+1)) == 0)
    {
        return 0;
    }
    
    str++;  /* skip 'x' */
    while ( *str != 0  &&       /* not end string?, and */
            ((*str >= '0' &&
              *str <= '9') ||       /* is numberic char?, or */
             (*str >= 'a' &&
              *str <= 'f') ||       /* is in 'a-f'?, or */
             (*str >= 'A' &&
              *str <= 'F')) )       /* is in 'A-F'? */
    {
        *b_found = 1;   /* found */
        
        if (*str >= 'a')
        {
            hexnum = 10 + (*str - 'a');     /* Get number */
        }
        else if (*str >= 'A')
        {
            hexnum = 10 + (*str - 'A');
        }
        else
        {
            hexnum = *str - '0';
        }

        rlt  = rlt << 4;
        rlt += hexnum;

        /* next char */
        str++;
    }
    
    return rlt;
}

/******************************************************************************
* Function Name    : static uint16_t COMMAND_DecStringToNum16_Logic(uint8_t *str)
* Description      : Command Logic part of converting decimal string to 16bit number
* Arguments        : uint8_t *str: Hex string
*                  : uint8_t *b_found: is found number? 
* Functions Called : None
* Return Value     : None
******************************************************************************/
static uint16_t COMMAND_DecStringToNum16_Logic(uint8_t *str, uint8_t *b_found)
{
    uint16_t    rlt = 0;
    uint8_t     decnum;
 
    /* No pointer checking here, this serve as logic implementation only */
    while ( *str != 0  &&       /* not end string?, and */
            (*str >= '0' &&
              *str <= '9'))     /* is numberic char?*/
    {
        *b_found = 1;   /* found */

        decnum = *str - '0';
        
        rlt  = rlt * 10;
        rlt += decnum;

        /* next char */
        str++;
    }
    
    return rlt;
}

/******************************************************************************
* Function Name    : static uint16_t COMMAND_DecStringToUnsigned16(uint8_t *str)
* Description      : Command Convert Dec String to number
* Arguments        : uint8_t *str: Hex string
* Functions Called : None
* Return Value     : None
******************************************************************************/
static uint16_t COMMAND_DecStringToUnsigned16(uint8_t *str, uint8_t *b_found)
{
    uint16_t    rlt;
    
    *b_found = 0; /* Not found */
    
    /* Check parameter */
    if (str == NULL)
    {
        return 0;
    }
        
    /* end string? */
    if (*str == 0)
    {
        return 0;
    }
    
    rlt = COMMAND_DecStringToNum16_Logic(str, b_found);
    
    return rlt;
}


/******************************************************************************
* Function Name    : static uint16_t COMMAND_DecStringToSigned16(uint8_t *str)
* Description      : Command Convert Dec String to number
* Arguments        : uint8_t *str: Hex string
* Functions Called : None
* Return Value     : None
******************************************************************************/
static int16_t COMMAND_DecStringToSigned16(uint8_t *str, uint8_t *b_found)
{
    int16_t     rlt;
    uint16_t    rlt_u;
    int16_t     sign;
    
    *b_found = 0; /* Not found */
    
    /* Check parameter */
    if (str == NULL)
    {
        return 0;
    }
        
    /* end string? */
    if (*str == 0)
    {
        return 0;
    }
    
    sign = (*str == '-') ? -1 : 1;
    if (sign == -1)
    {
        str++;
    }
    
    rlt_u = COMMAND_DecStringToNum16_Logic(str, b_found);
    
    /* Is input number in range of signed 16bit number? */
    if (sign == -1)
    {
        if (rlt_u > ((uint16_t)INT_MAX + 1))
        {
            *b_found = 0;
        }
    }
    else
    {
        if (rlt_u > INT_MAX)
        {
            *b_found = 0;
        }
    }
    
    /* Number checking OK, get sign to result */
    rlt = ((int16_t)rlt_u * sign);
    
    return rlt;
}


/******************************************************************************
* Function Name    : static uint16_t COMMAND_HexBufferToUnsigned16(uint8_t *str, uint16_t len, uint8_t *b_found)
* Description      : Command Convert Hex Buffer to number
* Arguments        : uint8_t *str: Hex string
* Functions Called : None
* Return Value     : None
******************************************************************************/
static uint16_t COMMAND_HexBufferToUnsigned16(uint8_t *buf, uint16_t len, uint8_t *b_found)
{
    uint16_t    pos = 0;
    uint16_t    rlt = 0;
    uint8_t     hexnum;
    
    *b_found = 0; /* Not found */
    
    /* Check parameter */
    if (buf == NULL || len == 0)
    {
        return 0;
    }
    
    /* point to 'x' char */
    while (pos < len && buf[pos] != 'x')
    {
        pos++;
    }
    
    /* end of buffer? */
    if (pos == len)
    {
        return 0;
    }
    
    pos++;  /* skip 'x' */
    while ( pos < len  &&       /* not end string?, and */
            ((buf[pos] >= '0' &&
              buf[pos] <= '9') ||       /* is numberic char?, or */
             (buf[pos] >= 'a' &&
              buf[pos] <= 'f') ||       /* is in 'a-f'?, or */
             (buf[pos] >= 'A' &&
              buf[pos] <= 'F')) )       /* is in 'A-F'? */
    {
        *b_found = 1;   /* found */
        
        if (buf[pos] >= 'a')
        {
            hexnum = 10 + (buf[pos] - 'a');     /* Get number */
        }
        else if (buf[pos] >= 'A')
        {
            hexnum = 10 + (buf[pos] - 'A');
        }
        else
        {
            hexnum = buf[pos] - '0';
        }

        rlt  = rlt << 4;
        rlt += hexnum;

        /* next char */
        pos++;
    }
    
    return rlt;
}

/******************************************************************************
* Function Name : COMMAND_GetPowerFactorSignName
* Interface     : static const uint8_t* COMMAND_GetPowerFactorSignName(EM_PF_SIGN sign)
* Description   : Get the PF sign name
* Arguments     : EM_PF_SIGN sign: The sign of power factor
* Function Calls: None
* Return Value  : static const uint8_t*
******************************************************************************/
static const uint8_t* COMMAND_GetPowerFactorSignName(EM_PF_SIGN sign)
{
    switch (sign)
    {
        case PF_SIGN_LEAD_C:
            return (const uint8_t*)"PF_SIGN_LEAD_C";
        case PF_SIGN_UNITY:
            return (const uint8_t*)"PF_SIGN_UNITY";
        case PF_SIGN_LAG_L:
            return (const uint8_t*)"PF_SIGN_LAG_L";
        default:
            return (const uint8_t*)"";
    }
}

/******************************************************************************
* Function Name : COMMAND_GetPowerFactorSignName
* Interface     : static const uint8_t* COMMAND_GetPowerFactorSignName(EM_PF_SIGN sign)
* Description   : Get the DSAD gain name
* Arguments     : dsad_gain_t gain: The gain of DSAD channel
* Function Calls: None
* Return Value  : static const uint8_t*
******************************************************************************/
static const uint8_t* COMMAND_GetDsadGainName(dsad_gain_t gain)
{
    switch (gain)
    {
        case GAIN_X1:
            return (const uint8_t*)"GAIN_X1";
        case GAIN_X2:
            return (const uint8_t*)"GAIN_X2";
        case GAIN_X4:
            return (const uint8_t*)"GAIN_X4";
        case GAIN_X8:
            return (const uint8_t*)"GAIN_X8";
        case GAIN_X16:
            return (const uint8_t*)"GAIN_X16";
        case GAIN_X32:
            return (const uint8_t*)"GAIN_X32";
        default:
            return (const uint8_t*)"UNKNOWN";
    }
}

/******************************************************************************
* Function Name    : static uint16_t COMMAND_DecBufferToUnsigned16(uint8_t *str, uint16_t len, uint8_t *b_found)
* Description      : Command Convert Dec Buffer to number
* Arguments        : uint8_t *buf: Dec buffer
* Functions Called : None
* Return Value     : None
******************************************************************************/
static uint16_t COMMAND_DecBufferToUnsigned16(uint8_t *buf, uint16_t len, uint8_t *b_found)
{
    uint16_t    pos = 0;
    uint16_t    rlt = 0;
    uint8_t     decnum;
    
    *b_found = 0; /* Not found */
    
    /* Check parameter */
    if (buf == NULL)
    {
        return 0;
    }
        
    /* end string? */
    if (len == 0)
    {
        return 0;
    }
    
    while ( pos < len &&        /* not end string?, and */
            (buf[pos] >= '0' &&
              buf[pos] <= '9'))     /* is numberic char?*/
    {
        *b_found = 1;   /* found */

        decnum = buf[pos] - '0';
        
        rlt  = rlt * 10;
        rlt += decnum;

        /* next char */
        pos++;
    }
    
    return rlt;
}

/********************************************************************************************
* Function Name    : static uint8_t COMMAND_FastInputScan(uint8_t *output, uint8_t * input)
* Description      : Command Convert Fast input parameter
*                  : Ex1: Input "0-3" ==> Output: {0,1,2,3}
*                  : Ex2: Input "0-1,2,3-4" ==> Output: {0,1,2,3,4}
* Arguments        : uint8_t *input: Input argument string
*                  : uint8_t *output: Output decoded argument in byte array
* Functions Called : None
* Return Value     : None
********************************************************************************************/
#define DELIMITER_SIGN              (',')
#define UPTO_SIGN                   ('-')

static uint8_t COMMAND_FastInputScan(uint8_t *output, uint8_t * input, uint8_t len_limit)
{
    uint8_t i;
    
    uint8_t pos_deli_start = 0, pos_deli_stop = 0;
    uint8_t pos_upto;
    
    uint8_t range_start, range_stop;
    uint8_t decoded_num;
    
    uint8_t input_len;
    uint8_t is_found;
    uint8_t upto_cnt = 0;
    
    /* Check for NULL argument */
    if (output == NULL || input == NULL)
    {
        return 1;                           //NULL params
    }
    
    /* Check first character */
    if (*input == DELIMITER_SIGN || *input == UPTO_SIGN)
    {
        return 2;                           //Wrong format  
    }
    
    /* Get length of input string */
    input_len = (uint8_t)strlen((const char *)input);
    
    /* Check last character */
    if (input[input_len] == DELIMITER_SIGN || input[input_len] == UPTO_SIGN)
    {
        return 2;                           //Wrong format  
    }
    
    /* Clear the buffer */
    memset(output, 0, len_limit);
    
    while (pos_deli_stop < input_len)
    {
        /* Find next DELIMITER_SIGN */
        for (i=pos_deli_start; i < input_len; i++)
        {
            pos_deli_stop = i;
            if (input[i] == DELIMITER_SIGN)
            {
                break;              //Found the delimiter
            }
            
            if (i == (input_len - 1))
            {
                pos_deli_stop++;    /* In case reach the end, need to increase one more time*/  
            }
        }
        
        /* Now, got the range of  input*/
        /* Find the UPTO_SIGN */
        for (i=pos_deli_start; i < pos_deli_stop; i++)
        {
            if (input[i] == UPTO_SIGN)
            {
                upto_cnt++;
                pos_upto = i;
            }
        }
        
        if (upto_cnt > 1)
        {
            return 2;                       //Wrong format, duplicate UPTO_SIGN 
        }
        else
        {
            if (upto_cnt == 1)
            {
                //Have upto_sign ==> decode
                /* Range start */
                range_start = (uint8_t)COMMAND_DecBufferToUnsigned16(&input[pos_deli_start],
                                                                        pos_upto - pos_deli_start,
                                                                        &is_found);
                
                /* Range stop */
                range_stop = (uint8_t)COMMAND_DecBufferToUnsigned16(&input[pos_upto+1],
                                                                        pos_deli_stop - 1 - pos_upto,
                                                                        &is_found);
                if (is_found &&
                    (range_start < len_limit) && (range_stop < len_limit))
                {
                    while(range_start <= range_stop)
                    {
                        *output++ = range_start;
                        range_start++;
                    }
                }
                else
                {
                    return 2;               //Wrong data    
                }
            }
            else
            {
                //No upto_sign ==> decode normally
                decoded_num = (uint8_t)COMMAND_DecBufferToUnsigned16(&input[pos_deli_start],
                                                                        pos_deli_stop - pos_deli_start,
                                                                        &is_found);
                if(is_found && 
                    (decoded_num < len_limit))
                {
                    *output++ = decoded_num;
                }
                else
                {
                    return 2;                   //Wrong data    
                }
            }
        }
        upto_cnt = 0;                           /* Reset upto_cnt */
        pos_deli_start = pos_deli_stop + 1;     /* Jump to pos after delimiter */
    }
    
    return 0;
}


/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeUsage(uint8_t *arg_str)
* Description      : Command Invoke Usage
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeUsage(uint8_t *arg_str)
{
    uint8_t i;
    
    CMD_SendString((uint8_t *)"\n\r");
    CMD_SendString((uint8_t *)"----------------------------------------------------------------------------------------------------------\n\r");
    CMD_Printf((uint8_t *)" %- 15s %- 42s %s\n\r", "Command Name", "Parameter", "Description");
    CMD_SendString((uint8_t *)"----------------------------------------------------------------------------------------------------------\n\r");
    for (i = 0; i < COMMAND_TABLE_LENGTH; i++)
    {
        R_WDT_Restart();
        CMD_Printf( (uint8_t *)" %- 15s %- 42s %s\n\r",
                    cmd_table[i].cmd_name,
                    cmd_table[i].params,
                    cmd_table[i].description);
    }
    
    return 0;
}

/******************************************************************************
* Function Name    : static void COMMAND_ResetByIllegalMemory(void)
* Description      : Command Intentionally Reset by using illegal memory access
* Arguments        : None
* Functions Called : None
* Return Value     : None
******************************************************************************/
#pragma inline_asm COMMAND_ResetByIllegalMemory
static void COMMAND_ResetByIllegalMemory(void)
{
    MOVW    HL, #0x1800
    MOV     ES, #0x01
    MOVW     ES:[HL+0x00], AX
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeRestartEM(uint8_t *arg_str)
* Description      : Command Invoke Restart EM
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeRestartEM(uint8_t *arg_str)
{
    CMD_SendString((uint8_t *)"\n\r Prepare illegal memory access...reset immediately...");
    
    COMMAND_ResetByIllegalMemory();
    
    return 0;
}



/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeRtc(uint8_t *arg_str)
* Description      : Command Invoke RTC
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeRtc(uint8_t *arg_str)
{
    rtc_calendarcounter_value_t rtctime;
    
    R_RTC_Get_CalendarCounterValue(&rtctime);
    
    CMD_Printf((uint8_t *)"\n\rRTC Time: %02x/%02x/20%02x %02x:%02x:%02x %02x\n\r",
                rtctime.rdaycnt,
                rtctime.rmoncnt,
                rtctime.ryrcnt,
                rtctime.rhrcnt,
                rtctime.rmincnt,
                rtctime.rseccnt,
                rtctime.rwkcnt);
                
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeSetRtc(uint8_t *arg_str)
* Description      : Command Invoke Set RTC
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeSetRtc(uint8_t *arg_str)
{   
    //uint16_t timeout;
    uint8_t status;
    rtc_calendarcounter_value_t rtctime;
    uint8_t buffer[20];

    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get Day parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.rdaycnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Month parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" /", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.rmoncnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Year parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" /", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.ryrcnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Hour parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" :");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.rhrcnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }

    /* Get Min parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" :", (uint8_t *)" :");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.rmincnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Sec parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" :", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.rseccnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Week parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        rtctime.rwkcnt = (uint8_t)EM_atoi(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    _DEC2BCD(rtctime.rdaycnt);
    _DEC2BCD(rtctime.rmoncnt);
    _DEC2BCD(rtctime.ryrcnt);
    _DEC2BCD(rtctime.rhrcnt);
    _DEC2BCD(rtctime.rmincnt);
    _DEC2BCD(rtctime.rseccnt);

    CMD_SendString((uint8_t *)"Set RTC time...");
    
    R_RTC_Set_CalendarCounterValue(rtctime);
    status = MD_OK;
    
    if (status == MD_OK)
    {
        CMD_SendString((uint8_t *)"OK\n\r");
    }
    else
    {
        CMD_SendString((uint8_t *)"FAILED\n\r");
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeBackup(uint8_t *arg_str)
* Description      : Command Invoke Backup
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeBackup(uint8_t *arg_str)
{
    uint8_t selection;
    uint8_t buffer[20];
    uint8_t b_found;
    uint8_t is_config = 0;
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get Selection parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* Get hex first */
        selection = (uint8_t)COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            selection = (uint8_t)EM_atoi(buffer);
        }
                
        if ((selection == CONFIG_ITEM_NONE) || (selection == STORAGE_ITEM_NONE))
        {
            CMD_SendString((uint8_t *)"Parameter error\n\r");
            return 1;       
        }
        
        /* Backup Item */
        CMD_SendString((uint8_t *)"Backup Item: ");
        if (selection & CONFIG_ITEM_CONFIG)
        {
            CMD_SendString((uint8_t *)"Configuration ");
            is_config = 1;
        }
        if (selection & CONFIG_ITEM_CALIB)
        {
            CMD_SendString((uint8_t *)"Calibration ");
            is_config = 1;
        }
        if (selection & STORAGE_ITEM_SYS_STATE)
        {
            CMD_SendString((uint8_t *)"System State ");
        }
        if (selection & STORAGE_ITEM_RTC_TIME)
        {
            CMD_SendString((uint8_t *)"RTC Time ");
        }
            
        CMD_SendString((uint8_t *)"...");
        
        if (is_config)
        {
            if (CONFIG_Backup(selection) != CONFIG_OK)
            {
                CMD_SendString((uint8_t *)"FAILED\n\r");
            }
            else
            {
                CMD_SendString((uint8_t *)"OK\n\r");
            }
        }
        else
        {
            if (STORAGE_Backup(selection) != STORAGE_OK)
            {
                CMD_SendString((uint8_t *)"FAILED\n\r");
            }
            else
            {
                CMD_SendString((uint8_t *)"OK\n\r");
            }
        }
        
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeRestore(uint8_t *arg_str)
* Description      : Command Invoke Restore
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeRestore(uint8_t *arg_str)
{
    uint8_t selection;
    uint8_t buffer[20];
    uint8_t b_found;
    uint8_t is_config = 0;
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get Selection parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* Get hex first */
        selection = (uint8_t)COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            selection = (uint8_t)EM_atoi(buffer);
        }
                
        if (selection == STORAGE_ITEM_NONE)
        {
            CMD_SendString((uint8_t *)"Parameter error\n\r");
            return 1;       
        }
        
        /* Backup Item */
        CMD_SendString((uint8_t *)"Backup Item: ");
        if (selection && CONFIG_ITEM_CONFIG)
        {
            CMD_SendString((uint8_t *)"Configuration ");
            is_config = 1;
        }
        if (selection & CONFIG_ITEM_CALIB)
        {
            CMD_SendString((uint8_t *)"Calibration ");
            is_config = 1;
        }
        if (selection & STORAGE_ITEM_SYS_STATE)
        {
            CMD_SendString((uint8_t *)"System State ");
        }
        if (selection & STORAGE_ITEM_RTC_TIME)
        {
            CMD_SendString((uint8_t *)"RTC Time ");
        }
    
        CMD_SendString((uint8_t *)"...");
        
        if (is_config)
        {
            if (CONFIG_Restore(selection) != CONFIG_OK)
            {
                CMD_SendString((uint8_t *)"FAILED\n\r");
            }
            else
            {
                CMD_SendString((uint8_t *)"OK\n\r");
            }
        }
        else
        {
            if (STORAGE_Restore(selection) != STORAGE_OK)
            {
                CMD_SendString((uint8_t *)"FAILED\n\r");
            }
            else
            {
                CMD_SendString((uint8_t *)"OK\n\r");
            }
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str)
* Description      : Command Invoke Read EEPROM
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str)
{
    static const uint16_t type_cast_size[7] =
    {
        1,  /* uint8_t */
        1,  /* int8_t */
        2,  /* uint16_t */
        2,  /* int16_t */
        4,  /* uint32_t */
        4,  /* int32_t */
        4,  /* float32_t */
    };
    
    uint32_t    addr;
    uint16_t    size, i, pagesize, count;
    uint8_t     b_found = 0;
    uint8_t     buffer[32];
    int8_t      cast_id = -1;
    uint16_t    u16;
    uint32_t    u32;
    float32_t   f32;
    uint8_t     is_dtfl;
    uint8_t const * p_mem_name;
    
    uint8_t (*FUNC_MemRead)(uint32_t addr, uint8_t *buf, uint16_t size);
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get memory type parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        is_dtfl = buffer[0] - '0';
        
        if (is_dtfl)
        {
            FUNC_MemRead = LVM_Read;
            p_mem_name = g_mem_dtfl;
        }
        else
        {
            FUNC_MemRead = EPR_Read;
            p_mem_name = g_mem_epr;
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Addr parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {
        /* Get hex first */
        addr = COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            addr = EM_atoi(buffer);
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Size parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* Get hex first */
        size = COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            size = EM_atoi(buffer);
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }   
    
    /* Get Cast parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    if (arg_str != NULL)
    {   
        /* found argument */
        if (strcmp("uint8_t", (const char*)buffer) == 0)
        {
            cast_id = 0;    /* uint8_t */
        }
        else if (strcmp("int8_t", (const char*)buffer) == 0)
        {
            cast_id = 1;    /* int8_t */
        }
        else if (strcmp("uint16_t", (const char*)buffer) == 0)
        {
            cast_id = 2;    /* uint16_t */
        }
        else if (strcmp("int16_t", (const char*)buffer) == 0)
        {
            cast_id = 3;    /* int16_t */
        }
        else if (strcmp("uint32_t", (const char*)buffer) == 0)
        {
            cast_id = 4;    /* uint32_t */
        }
        else if (strcmp("int32_t", (const char*)buffer) == 0)
        {
            cast_id = 5;    /* int32_t */
        }
        else if (strcmp("float32_t", (const char*)buffer) == 0)
        {
            cast_id = 6;    /* float32_t */
        }
        else
        {
            CMD_SendString((uint8_t *)"Parameter error\n\r");
            return 1;
        }
    }
    
    if (cast_id != -1)
    {
        CMD_Printf((uint8_t *)"\n\r--- %s Data (cast by %s) ---\n\r",p_mem_name, buffer);
    }
    else
    {
        CMD_Printf((uint8_t *)"\n\r--- %s Data (HEX, Max 1line = 32 bytes) ---\n\r", p_mem_name);   
    }

    count = 0;
    while (size > 0)
    {
        if (size >= 32)
        {
            pagesize = 32;
        }
        else
        {
            pagesize = size;
        }
        
        if (FUNC_MemRead(addr, buffer, pagesize) == EPR_OK)
        {
            if (cast_id != -1)
            {
                for (i = 0; i < pagesize; i += type_cast_size[cast_id])
                {
                    switch (cast_id)
                    {
                        case 0:/* uint8_t */
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%02x) %i\n\r", count, addr+i, (uint8_t)buffer[i], (uint8_t)buffer[i]);
                            break;
                            
                        case 1:/* int8_t */
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%02x) %i\n\r", count, addr+i, (uint8_t)buffer[i], (int8_t)buffer[i]);
                            break;
                            
                        case 2:/* uint16_t */
                            u16 = COMMAND_GetUnsigned16(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%04x) %d\n\r", count, addr+i, u16, u16);
                            break;
                            
                        case 3:/* int16_t */
                            u16 = COMMAND_GetUnsigned16(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%04x) %d\n\r", count, addr+i, u16, (int16_t)u16);
                            break;
                            
                        case 4:/* uint32_t */
                            u32 = COMMAND_GetUnsigned32(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %ld\n\r", count, addr+i, u32, u32);
                            break;
                            
                        case 5:/* int32_t */
                            u32 = COMMAND_GetUnsigned32(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %ld\n\r", count, addr+i, u32, (int32_t)u32);
                            break;
                            
                        case 6:/* float32_t */
                            u32 = COMMAND_GetUnsigned32(&buffer[i]);
                            f32 = *((float32_t *)&u32);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %.06f\n\r", count, addr+i, u32, f32);
                            break;
                            
                        default:
                            CMD_SendString((uint8_t *)"Internal error.\n\r");
                            return 2;
                    }

                    count++;
                }
            }
            else
            {
                for (i = 0; i < pagesize; i++)
                {
                    CMD_Printf((uint8_t *)"%02x ", buffer[i]);
                }
                CMD_SendString((uint8_t *)"\n\r");
            }
            
            /* New page */
            addr += pagesize;
            size -= pagesize;

            /* Break if CTRL+C is pressed */
            if (CMD_IsCtrlKeyReceived())
            {
                CMD_Printf((uint8_t *)"\n\rCTRL + C is pressed! Break!\n\r");
                CMD_AckCtrlKeyReceived();
                
                /* Break printout here */
                break;
            }

            R_WDT_Restart();
        }
        else
        {
            CMD_Printf((uint8_t *)"\n\rRead %s failed at page [addr:0x%04x, size:%i]!\n\r", p_mem_name, addr, pagesize);
            break;
        }
    }
    
    CMD_Printf((uint8_t *)"--- %s Data -----------------\n\r", p_mem_name);
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeWriteMemory(uint8_t *arg_str)
* Description      : Command Invoke Write EEPROM
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeWriteMemory(uint8_t *arg_str)
{
    uint32_t    addr;
    uint16_t    size, value, pagesize;
    uint8_t     b_found = 0;
    uint8_t     b_string = 0;
    uint8_t *   p_str_old;
    uint8_t     buffer[32];
    uint8_t     is_dtfl;
    uint8_t const * p_mem_name;
    
    uint8_t (*FUNC_MemWrite)(uint32_t addr, uint8_t *buf, uint16_t size);
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get memory type parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        is_dtfl = buffer[0] - '0';
        
        if (is_dtfl)
        {
            FUNC_MemWrite = LVM_Write;
            p_mem_name = g_mem_dtfl;
        }
        else
        {
            FUNC_MemWrite = EPR_Write;
            p_mem_name = g_mem_epr;
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Addr parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* Get hex first */
        addr = COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            addr = EM_atoi(buffer);
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Size parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {
        /* Get hex first */
        size = COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            size = EM_atoi(buffer);
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get string parameter */
    p_str_old = arg_str;
    arg_str   = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" (", (uint8_t *)")");
    
    /* is NOT have string parameter? */ 
    if (arg_str == NULL || *arg_str != ')')
    {
        /* scan number */
        arg_str = COMMAND_GetScanOneParam(buffer, 20, p_str_old, (uint8_t *)" ", (uint8_t *)" ");
        
        if (arg_str != NULL &&
            (buffer[0] >= '0' && buffer[0] <= '9'))
        {       
            /* Get hex first */
            value = COMMAND_HexStringToUnsigned16(buffer, &b_found);
            
            /* next for get number */
            if (b_found == 0)
            {
                value = EM_atoi(buffer);
            }
        }
        else
        {
            CMD_SendString((uint8_t *)"Parameter error\n\r");
            return 1;
        }
    }
    else
    {
        b_string = 1;
    }
    
    if (arg_str == NULL)
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }   
    
    CMD_Printf((uint8_t *)"Start write to %s...", p_mem_name);
    
    while (size > 0)
    {
        if (size >= 32)
        {
            pagesize = 32;
        }
        else
        {
            pagesize = size;
        }

        if (b_string != 1)      
        {
            memset(buffer, value, pagesize);
        }

        /* write buffer */
        if (FUNC_MemWrite(addr, buffer, pagesize) == EPR_OK)
        {           
            /* New page */
            addr += pagesize;
            size -= pagesize;
        }
        else
        {
            CMD_Printf((uint8_t *)"\n\rWrite %s failed at page [addr:0x%04x, size:%i]!\n\r", p_mem_name, addr, pagesize);
            return 1;
        }
        
        R_WDT_Restart();
    }
    
    CMD_SendString((uint8_t *)"OK\n\r");    
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeReadLVMFlag(uint8_t *arg_str)
* Description      : Command Invoke Read the LVM Flag byte
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeReadLVMFlag(uint8_t *arg_str)
{
    lvm_flag_t flag;
    
    CMD_SendString((uint8_t *)"\n\r");
    CMD_SendString((uint8_t *)"LVM Flag Read...");
    
    
    /* Only used in read DATAFLASH at this case to debug */
    if (DATAFLASH_Read(PDEV_LVM_FLG_ADDR, (uint8_t *)&flag, PDEV_LVM_FLG_SIZE) != PDEV_OK)
    {
        CMD_SendString((uint8_t *)"FAILED\n\r");
    }
    else
    {
        /* Print out detail */
        CMD_SendString((uint8_t *)"OK\n\r");
        CMD_SendString((uint8_t *)"\n\rLVM Flag Detail: \n\r");
        CMD_Printf((uint8_t *)" - Is updating: %d\n\r",flag.is_updating);
        CMD_Printf((uint8_t *)" - Bank1 in use: %d\n\r",flag.is_bank1_in_use);
        CMD_Printf((uint8_t *)" - Bank1 in use: %d\n\r",flag.is_bank2_in_use);
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeWriteLVMFlag(uint8_t *arg_str)
* Description      : Command Invoke Read the LVM Flag byte
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeWriteLVMFlag(uint8_t *arg_str)
{
    uint8_t     buffer[32];
    lvm_flag_t flag;
    
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get is updating flag */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] == '0' || buffer[0] == '1'))
    {   
        flag.is_updating = buffer[0] - '0';
        
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get bank1 in use flag */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] == '0' || buffer[0] == '1'))
    {   
        flag.is_bank1_in_use = buffer[0] - '0';
        
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get bank2 in use flag */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] == '0' || buffer[0] == '1'))
    {   
        flag.is_bank2_in_use = buffer[0] - '0';
        
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    CMD_SendString((uint8_t *)"LVM Flag Write...");
    
    
    /* Only used in read DATAFLASH at this case to debug */
    if (DATAFLASH_Write(PDEV_LVM_FLG_ADDR, (uint8_t *)&flag, PDEV_LVM_FLG_SIZE) != PDEV_OK)
    {
        CMD_SendString((uint8_t *)"FAILED\n\r");
    }
    else
    {
        /* Write OK */
        CMD_SendString((uint8_t *)"OK\n\r");
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeReadMemory(uint8_t *arg_str)
* Description      : Command Invoke Read EEPROM
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeReadDTFL(uint8_t *arg_str)
{
    static const uint16_t type_cast_size[7] =
    {
        1,  /* uint8_t */
        1,  /* int8_t */
        2,  /* uint16_t */
        2,  /* int16_t */
        4,  /* uint32_t */
        4,  /* int32_t */
        4,  /* float32_t */
    };
    
    uint32_t    addr;
    uint16_t    size, i, pagesize, count;
    uint8_t     b_found = 0;
    uint8_t     buffer[32];
    int8_t      cast_id = -1;
    uint16_t    u16;
    uint32_t    u32;
    float32_t   f32;
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get Addr parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {
        /* Get hex first */
        addr = COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            addr = EM_atoi(buffer);
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get Size parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* Get hex first */
        size = COMMAND_HexStringToUnsigned16(buffer, &b_found);
        
        /* next for get number */
        if (b_found == 0)
        {
            size = EM_atoi(buffer);
        }
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }   
    
    /* Get Cast parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    if (arg_str != NULL)
    {   
        /* found argument */
        if (strcmp("uint8_t", (const char*)buffer) == 0)
        {
            cast_id = 0;    /* uint8_t */
        }
        else if (strcmp("int8_t", (const char*)buffer) == 0)
        {
            cast_id = 1;    /* int8_t */
        }
        else if (strcmp("uint16_t", (const char*)buffer) == 0)
        {
            cast_id = 2;    /* uint16_t */
        }
        else if (strcmp("int16_t", (const char*)buffer) == 0)
        {
            cast_id = 3;    /* int16_t */
        }
        else if (strcmp("uint32_t", (const char*)buffer) == 0)
        {
            cast_id = 4;    /* uint32_t */
        }
        else if (strcmp("int32_t", (const char*)buffer) == 0)
        {
            cast_id = 5;    /* int32_t */
        }
        else if (strcmp("float32_t", (const char*)buffer) == 0)
        {
            cast_id = 6;    /* float32_t */
        }
        else
        {
            CMD_SendString((uint8_t *)"Parameter error\n\r");
            return 1;
        }
    }
    
    if (cast_id != -1)
    {
        CMD_Printf((uint8_t *)"\n\r--- DataFlash Data (cast by %s) ---\n\r", buffer);
    }
    else
    {
        CMD_Printf((uint8_t *)"\n\r--- DataFlash Data (HEX, Max 1line = 32 bytes) ---\n\r");   
    }

    count = 0;
    while (size > 0)
    {
        if (size >= 32)
        {
            pagesize = 32;
        }
        else
        {
            pagesize = size;
        }
        
        if (DATAFLASH_Read(addr, buffer, pagesize) == EPR_OK)
        {
            if (cast_id != -1)
            {
                for (i = 0; i < pagesize; i += type_cast_size[cast_id])
                {
                    switch (cast_id)
                    {
                        case 0:/* uint8_t */
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%02x) %i\n\r", count, addr+i, (uint8_t)buffer[i], (uint8_t)buffer[i]);
                            break;
                            
                        case 1:/* int8_t */
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%02x) %i\n\r", count, addr+i, (uint8_t)buffer[i], (int8_t)buffer[i]);
                            break;
                            
                        case 2:/* uint16_t */
                            u16 = COMMAND_GetUnsigned16(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%04x) %d\n\r", count, addr+i, u16, u16);
                            break;
                            
                        case 3:/* int16_t */
                            u16 = COMMAND_GetUnsigned16(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%04x) %d\n\r", count, addr+i, u16, (int16_t)u16);
                            break;
                            
                        case 4:/* uint32_t */
                            u32 = COMMAND_GetUnsigned32(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %ld\n\r", count, addr+i, u32, u32);
                            break;
                            
                        case 5:/* int32_t */
                            u32 = COMMAND_GetUnsigned32(&buffer[i]);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %ld\n\r", count, addr+i, u32, (int32_t)u32);
                            break;
                            
                        case 6:/* float32_t */
                            u32 = COMMAND_GetUnsigned32(&buffer[i]);
                            f32 = *((float32_t *)&u32);
                            CMD_Printf((uint8_t *)"%05i - 0x%08lx: (0x%08lx) %.06f\n\r", count, addr+i, u32, f32);
                            break;
                            
                        default:
                            CMD_SendString((uint8_t *)"Internal error.\n\r");
                            return 2;
                    }

                    count++;
                }
            }
            else
            {
                for (i = 0; i < pagesize; i++)
                {
                    CMD_Printf((uint8_t *)"%02x ", buffer[i]);
                }
                CMD_SendString((uint8_t *)"\n\r");
            }
            
            /* New page */
            addr += pagesize;
            size -= pagesize;

            /* Break if CTRL+C is pressed */
            if (CMD_IsCtrlKeyReceived())
            {
                CMD_Printf((uint8_t *)"\n\rCTRL + C is pressed! Break!\n\r");
                CMD_AckCtrlKeyReceived();
                
                /* Break printout here */
                break;
            }

            R_WDT_Restart();
        }
        else
        {
            CMD_Printf((uint8_t *)"\n\rRead DataFlash failed at page [addr:0x%04x, size:%i]!\n\r", addr, pagesize);
            break;
        }
    }
    
    CMD_Printf((uint8_t *)"--- DataFlash Data -----------------\n\r");
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeAddEnergyLog(uint8_t *arg_str)
* Description      : Command Invoke Add Energy Log
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeAddEnergyLog(uint8_t *arg_str)
{
    ONE_MONTH_ENERGY_DATA_LOG   * p_energy_log = NULL;
    
    /* Get the energy log */
    STORAGE_GetEnergyLog(p_energy_log);
    
    if (STORAGE_AddEnergyLog(p_energy_log) == STORAGE_OK)
    {
        CMD_Printf((uint8_t *)"OK\n\r");
    }
    else
    {
        CMD_Printf((uint8_t *)"FAILED\n\r");
    }

    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeAddTamperLog(uint8_t *arg_str)
* Description      : Command Invoke Add Tamper Log
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeAddTamperLog(uint8_t *arg_str)
{
    ONE_TAMPER_DATA_LOG         * p_tamper_log = NULL;     /* Tamper log data */
    
    /* Get the tamper log */
    STORAGE_GetTamperLog(p_tamper_log);
    
    if (STORAGE_AddTamperLog(p_tamper_log) == STORAGE_OK)
    {
        CMD_SendString((uint8_t *)"OK\n\r");
    }
    else
    {
        CMD_SendString((uint8_t *)"FAILED\n\r");
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeFormatMemory(uint8_t *arg_str)
* Description      : Command Invoke Format EEPROM
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeFormatMemory(uint8_t *arg_str)
{
    uint8_t buffer[20];
    uint8_t is_dtfl;
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get memory type parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" /");
    
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        is_dtfl = buffer[0] - '0';
    
    }
    
    if (is_dtfl)
    {
        CMD_SendString((uint8_t *)"\n\rFormatting Configuration Page...");
        
        R_WDT_Restart();
        
        if (CONFIG_Format() == CONFIG_OK)
        {
            CMD_SendString((uint8_t *)"OK\n\r");
        }
        else
        {
            CMD_SendString((uint8_t *)"FAILED\n\r");
        }
        
    }
    else
    {
        CMD_SendString((uint8_t *)"\n\rFormatting MeterData...");
        
        R_WDT_Restart();
        
        if (STORAGE_Format() == STORAGE_OK)
        {
            CMD_SendString((uint8_t *)"OK\n\r");
        }
        else
        {
            CMD_SendString((uint8_t *)"FAILED\n\r");
        }
        
    }
    return 0;
}


/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeReadPlatformInfo(uint8_t *arg_str)
* Description      : Command Invoke Read Platform Info
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
static uint8_t COMMAND_InvokeReadPlatformInfo(uint8_t *arg_str)
{
    PLATFORM_INFO platform_info;
    
    CMD_Printf((uint8_t *)"\n\rRead platform information...");
    if (STORAGE_ReadPlatformInfo(&platform_info) != STORAGE_OK)
    {
        CMD_Printf((uint8_t *)"FAILED\n\r");
        return 1;
    }
    else
    {
        CMD_Printf((uint8_t *)"OK\n\r");
    }

    CMD_Printf((uint8_t *)"Unit Code        : %s\n\r", platform_info.unit_code);
    CMD_Printf((uint8_t *)"Type of meter    : %s\n\r", platform_info.type_of_meter);
    CMD_Printf( (uint8_t *)"Manufacture Date : %02x/%02x/20%02x\n\r",
                platform_info.manufacture_date.Day,
                platform_info.manufacture_date.Month,
                platform_info.manufacture_date.Year);
    CMD_Printf((uint8_t *)"Number of reset  : %i\n\r", platform_info.number_of_reset);
    CMD_Printf((uint8_t *)"LCD Scroll Period: %i\n\r", platform_info.lcd_scroll_period);
    
    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeDisplay(uint8_t *arg_str)
* Description      : Command Invoke Display
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeDisplay(uint8_t *arg_str)
{
    uint32_t    timeout;
    EM_PF_SIGN  pf_sign;
    const uint8_t * ptr_sign;
    EM_LINE     line;
    static const uint8_t line_break[]   = "+-----------------------------------------------------------------------------------------------------------------------------------+\n\r";
    static const uint8_t param_format[] = "| %- 22s | %# 15.3f | %# 15.3f | %# 15.3f | %# 15.3f | %# 15.3f | %- 15s|\n\r";
    static const uint8_t param_format_u[] = "| %- 22s | %# 15u | %# 15u | %# 15u | %# 15u | %# 15u | %- 15s|\n\r";
    static const uint8_t param_format_s[] = "| %- 22s | %# 15s | %# 15s | %# 15s | %# 15s | %# 15s | %- 15s|\n\r";
    float32_t vector_value;
    EM_CALIBRATION calib;
    //float32_t     temperature;
    /*  
    v              = EM_GetVoltageRMS(void);
    i              = EM_GetCurrentRMS(void);
    power_active   = EM_GetActivePower(void);
    power_reactive = EM_GetReactivePower(void);
    power_apprent  = EM_GetApparentPower(void);
    pf             = EM_GetPowerFactor(void);
    fac            = EM_GetLineFrequency(void);
    active_total   = EM_GetActiveEnergyTotal(void);
    reactive_total = EM_GetReactiveEnergyTotal(void);
    apparent_total = EM_GetApparentEnergyTotal(void);
    active_mdm     = EM_GetActiveMaxDemand()   / 1000.0f;
    reactive_mdm   = EM_GetReactiveMaxDemand() / 1000.0f;
    apparent_mdm   = EM_GetApparentMaxDemand() / 1000.0f;
    */
    CMD_SendString((uint8_t *)"\n\rWaiting for signal stable...");
    
    timeout = 30000;
    while (timeout > 0)
    {
        timeout--;
    }
    
    calib = EM_GetCalibInfo();
    
    for (line = LINE_PHASE_R; line <= LINE_TOTAL; line++)
    {
        g_em_core_data[line].vrms                   = EM_GetVoltageRMS(line);
        g_em_core_data[line].irms                   = EM_GetCurrentRMS(line);
        g_em_core_data[line].power_active           = EM_GetActivePower(line);
        g_em_core_data[line].power_reactive         = EM_GetReactivePower(line);
        g_em_core_data[line].power_apprent          = EM_GetApparentPower(line);
        g_em_core_data[line].fvrms                  = EM_GetFundamentalVoltageRMS(line);
        g_em_core_data[line].firms                  = EM_GetFundamentalCurrentRMS(line);
        g_em_core_data[line].power_factive          = EM_GetFundamentalActivePower(line);
        g_em_core_data[line].voltage_thd            = EM_GetVoltageTHD(line) * 100.0f;
        g_em_core_data[line].current_thd            = EM_GetCurrentTHD(line) * 100.0f;
        g_em_core_data[line].active_thd             = EM_GetActivePowerTHD(line) * 100.0f;
        g_em_core_data[line].pf                     = EM_GetPowerFactor(line);
        g_em_core_data[line].pf_sign                = EM_GetPowerFactorSign(line);
        g_em_core_data[line].fac                    = EM_GetLineFrequency(line);
        g_em_core_data[line].energy_total_active    = EM_GetActiveEnergyTotal(line);
        g_em_core_data[line].energy_total_reactive  = EM_GetReactiveEnergyTotal(line);
        g_em_core_data[line].energy_total_apparent  = EM_GetApparentEnergyTotal(line);
        g_em_core_data[line].mdm_active             = EM_GetActiveMaxDemand(line)   / 1000.0f;
        g_em_core_data[line].mdm_reactive           = EM_GetReactiveMaxDemand(line) / 1000.0f;
        g_em_core_data[line].mdm_apparent           = EM_GetApparentMaxDemand(line) / 1000.0f;
        if (line < LINE_NEUTRAL)
        {
            g_em_core_data[line].offset_v           = *(&calib.offset.phase_r.v + line*2);
        }
        else
        {
            g_em_core_data[line].offset_v           = 0;
        }
        g_em_core_data[line].offset_i               = EM_GetCurrentOffsetAverage(line);
    }
    vector_value = EM_GetPhaseCurrentSumVector();
    
    CMD_Printf((uint8_t *)"OK \n\r");
    CMD_Printf((uint8_t *)"\n\r");
    
    /* Display table */
    CMD_SendString((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"| %- 22s | %- 15s | %- 15s | %- 15s | %- 15s | %- 15s | %- 15s|\n\r", "Parameter", "Phase R", "Phase Y", "Phase B", "Neutral", "Total", "Unit");
    CMD_SendString((uint8_t *)line_break);
    
    R_WDT_Restart();
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Voltage RMS",
        g_em_core_data[LINE_PHASE_R].vrms,
        g_em_core_data[LINE_PHASE_Y].vrms,
        g_em_core_data[LINE_PHASE_B].vrms,
        g_em_core_data[LINE_NEUTRAL].vrms,
        g_em_core_data[LINE_TOTAL].vrms,
        "Volt"
    );

    CMD_Printf(
        (uint8_t *)param_format, 
        "Current RMS",
        g_em_core_data[LINE_PHASE_R].irms,
        g_em_core_data[LINE_PHASE_Y].irms,
        g_em_core_data[LINE_PHASE_B].irms,
        g_em_core_data[LINE_NEUTRAL].irms,
        g_em_core_data[LINE_TOTAL].irms,
        "Ampere"
    );  

    CMD_Printf(
        (uint8_t *)param_format, 
        "Current Sum Vector",
        0.0f,
        0.0f,
        0.0f,
        vector_value,
        0.0f,
        "Ampere"
    );  
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Power Factor",
        g_em_core_data[LINE_PHASE_R].pf,
        g_em_core_data[LINE_PHASE_Y].pf,
        g_em_core_data[LINE_PHASE_B].pf,
        g_em_core_data[LINE_NEUTRAL].pf,
        g_em_core_data[LINE_TOTAL].pf,
        " "
    );  
    
    CMD_Printf(
        (uint8_t *)param_format_s, 
        "Power Factor Sign",
        COMMAND_GetPowerFactorSignName(g_em_core_data[LINE_PHASE_R].pf_sign),
        COMMAND_GetPowerFactorSignName(g_em_core_data[LINE_PHASE_Y].pf_sign),
        COMMAND_GetPowerFactorSignName(g_em_core_data[LINE_PHASE_B].pf_sign),
        " ",
        COMMAND_GetPowerFactorSignName(g_em_core_data[LINE_TOTAL].pf_sign),
        " "
    );  
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Line Frequency",
        g_em_core_data[LINE_PHASE_R].fac,
        g_em_core_data[LINE_PHASE_Y].fac,
        g_em_core_data[LINE_PHASE_B].fac,
        g_em_core_data[LINE_NEUTRAL].fac,
        g_em_core_data[LINE_TOTAL].fac,
        "Hz"
    );
    
    CMD_SendString((uint8_t *)line_break);
    
    /* Power */
    CMD_Printf(
        (uint8_t *)param_format, 
        "Active Power",
        g_em_core_data[LINE_PHASE_R].power_active,
        g_em_core_data[LINE_PHASE_Y].power_active,
        g_em_core_data[LINE_PHASE_B].power_active,
        g_em_core_data[LINE_NEUTRAL].power_active,
        g_em_core_data[LINE_TOTAL].power_active,
        "Watt"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Reactive Power",
        g_em_core_data[LINE_PHASE_R].power_reactive,
        g_em_core_data[LINE_PHASE_Y].power_reactive,
        g_em_core_data[LINE_PHASE_B].power_reactive,
        g_em_core_data[LINE_NEUTRAL].power_reactive,
        g_em_core_data[LINE_TOTAL].power_reactive,
        "VAr"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Apparent Power",
        g_em_core_data[LINE_PHASE_R].power_apprent,
        g_em_core_data[LINE_PHASE_Y].power_apprent,
        g_em_core_data[LINE_PHASE_B].power_apprent,
        g_em_core_data[LINE_NEUTRAL].power_apprent,
        g_em_core_data[LINE_TOTAL].power_apprent,
        "VA"
    );
    
    
    CMD_SendString((uint8_t *)line_break);
    /* Fundamental */
    CMD_Printf(
        (uint8_t *)param_format, 
        "Filtered Voltage RMS",
        g_em_core_data[LINE_PHASE_R].fvrms,
        g_em_core_data[LINE_PHASE_Y].fvrms,
        g_em_core_data[LINE_PHASE_B].fvrms,
        0.0f,
        0.0f,
        "Volt"
    );

    CMD_Printf(
        (uint8_t *)param_format, 
        "Filtered Current RMS",
        g_em_core_data[LINE_PHASE_R].firms,
        g_em_core_data[LINE_PHASE_Y].firms,
        g_em_core_data[LINE_PHASE_B].firms,
        0.0f,
        0.0f,
        "Ampere"
    );  
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Fund Active Power",
        g_em_core_data[LINE_PHASE_R].power_factive,
        g_em_core_data[LINE_PHASE_Y].power_factive,
        g_em_core_data[LINE_PHASE_B].power_factive,
        g_em_core_data[LINE_NEUTRAL].power_factive,
        0.0f,
        "Watt"
    );

    CMD_Printf(
        (uint8_t *)param_format, 
        "Voltage THD",
        g_em_core_data[LINE_PHASE_R].voltage_thd,
        g_em_core_data[LINE_PHASE_Y].voltage_thd,
        g_em_core_data[LINE_PHASE_B].voltage_thd,
        0.0f,
        0.0f,
        "Percent"
    );

    CMD_Printf(
        (uint8_t *)param_format, 
        "Current THD",
        g_em_core_data[LINE_PHASE_R].current_thd,
        g_em_core_data[LINE_PHASE_Y].current_thd,
        g_em_core_data[LINE_PHASE_B].current_thd,
        0.0f,
        0.0f,
        "Percent"
    );  
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Active Power THD",
        g_em_core_data[LINE_PHASE_R].active_thd,
        g_em_core_data[LINE_PHASE_Y].active_thd,
        g_em_core_data[LINE_PHASE_B].active_thd,
        0.0f,
        0.0f,
        "Percent"
    );
    
    
    CMD_SendString((uint8_t *)line_break);
    
    /* Energy */
    CMD_Printf(
        (uint8_t *)param_format, 
        "Total Active Energy",
        g_em_core_data[LINE_PHASE_R].energy_total_active,
        g_em_core_data[LINE_PHASE_Y].energy_total_active,
        g_em_core_data[LINE_PHASE_B].energy_total_active,
        g_em_core_data[LINE_NEUTRAL].energy_total_active,
        g_em_core_data[LINE_TOTAL].energy_total_active,
        "kWh"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Total Reactive Energy",
        g_em_core_data[LINE_PHASE_R].energy_total_reactive,
        g_em_core_data[LINE_PHASE_Y].energy_total_reactive,
        g_em_core_data[LINE_PHASE_B].energy_total_reactive,
        g_em_core_data[LINE_NEUTRAL].energy_total_reactive,
        g_em_core_data[LINE_TOTAL].energy_total_reactive,
        "kVArh"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Total Apparent Energy",
        g_em_core_data[LINE_PHASE_R].energy_total_apparent,
        g_em_core_data[LINE_PHASE_Y].energy_total_apparent,
        g_em_core_data[LINE_PHASE_B].energy_total_apparent,
        g_em_core_data[LINE_NEUTRAL].energy_total_apparent,
        g_em_core_data[LINE_TOTAL].energy_total_apparent,
        "kVAh"
    );
    
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Active Max Demand",
        g_em_core_data[LINE_PHASE_R].mdm_active,
        g_em_core_data[LINE_PHASE_Y].mdm_active,
        g_em_core_data[LINE_PHASE_B].mdm_active,
        g_em_core_data[LINE_NEUTRAL].mdm_active,
        g_em_core_data[LINE_TOTAL].mdm_active,
        "kWh"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Reactive Max Demand",
        g_em_core_data[LINE_PHASE_R].mdm_reactive,
        g_em_core_data[LINE_PHASE_Y].mdm_reactive,
        g_em_core_data[LINE_PHASE_B].mdm_reactive,
        g_em_core_data[LINE_NEUTRAL].mdm_reactive,
        g_em_core_data[LINE_TOTAL].mdm_reactive,
        "kVArh"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Apparent Max Demand",
        g_em_core_data[LINE_PHASE_R].mdm_apparent,
        g_em_core_data[LINE_PHASE_Y].mdm_apparent,
        g_em_core_data[LINE_PHASE_B].mdm_apparent,
        g_em_core_data[LINE_NEUTRAL].mdm_apparent,
        g_em_core_data[LINE_TOTAL].mdm_apparent,
        "kVAh"
    );
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Offset Voltage",
        (float32_t)g_em_core_data[LINE_PHASE_R].offset_v,
        (float32_t)g_em_core_data[LINE_PHASE_Y].offset_v,
        (float32_t)g_em_core_data[LINE_PHASE_B].offset_v,
        (float32_t)g_em_core_data[LINE_NEUTRAL].offset_v,
        (float32_t)g_em_core_data[LINE_TOTAL].offset_v,
        " "
    );  
    
    CMD_Printf(
        (uint8_t *)param_format, 
        "Offset Current",
        (float32_t)g_em_core_data[LINE_PHASE_R].offset_i,
        (float32_t)g_em_core_data[LINE_PHASE_Y].offset_i,
        (float32_t)g_em_core_data[LINE_PHASE_B].offset_i,
        (float32_t)g_em_core_data[LINE_NEUTRAL].offset_i,
        (float32_t)g_em_core_data[LINE_TOTAL].offset_i,
        " "
    );  
    
    CMD_SendString((uint8_t *)line_break);

    CMD_Printf((uint8_t *)"\n\r Done... \n\r");

    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeDumpSample(uint8_t *arg_str)
* Description      : Command Invoke Dump ADC samples
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeDumpSample(uint8_t *arg_str)
{
    uint16_t i;

    #ifndef METER_ENABLE_MEASURE_CPU_LOAD
    /* Send request to EM Core to dump the waveform */
    g_sample_count = 0;

    R_WDT_Restart();
    while (g_sample_count < g_sample_max_count);
    
    CMD_Printf((uint8_t *)"\n\r");
    CMD_Printf((uint8_t *)"%-9s, %-9s, %-9s, %-9s, %-9s, %-9s, %-9s, %-9s, %-9s, %-9s\n\r", 
                                "V-R","V-Y","V-B","V90-R","V90-Y","V90-B","I-R","I-Y","I-B","I-N");
    
    R_WDT_Restart();
    for (i = 0; i < g_sample_count; i++)
    {
        if (g_sample_adc[i][0] == 0)
        {
            g_sample_adc[i][0] = 1;
        }
        if (g_sample_adc[i][1] == 0)
        {
            g_sample_adc[i][1] = 1;
        }
        if (g_sample_adc[i][2] == 0)
        {
            g_sample_adc[i][2] = 1;
        }
        
        if (g_sample_v90[i][0] == 0)
        {
            g_sample_v90[i][0] = 1;
        }
        if (g_sample_v90[i][1] == 0)
        {
            g_sample_v90[i][1] = 1;
        }
        if (g_sample_v90[i][2] == 0)
        {
            g_sample_v90[i][2] = 1;
        }
        
        if (g_sample_dsad[i][0] == 0)
        {
            g_sample_dsad[i][0] = 1;
        }
        if (g_sample_dsad[i][1] == 0)
        {
            g_sample_dsad[i][1] = 1;
        }
        if (g_sample_dsad[i][2] == 0)
        {
            g_sample_dsad[i][2] = 1;
        }
        if (g_sample_dsad[i][3] == 0)
        {
            g_sample_dsad[i][3] = 1;
        }
        
        CMD_Printf((uint8_t *)"%-9.d, %-9.d, %-9.d, %-9.d, %-9.d, %-9.d, %-9.ld, %-9.ld, %-9.ld, %-9.ld\n\r", 
                                g_sample_adc[i][0], g_sample_adc[i][1], g_sample_adc[i][2], 
                                g_sample_v90[i][0], g_sample_v90[i][1], g_sample_v90[i][2], 
                                g_sample_dsad[i][0], g_sample_dsad[i][1],g_sample_dsad[i][2], g_sample_dsad[i][3]);
    }                 
    #else
    CMD_Printf((uint8_t *)"No support!\n\r");
    #endif
    
    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeSetChannel(uint8_t *arg_str)
* Description      : Command Invoke DSAD Channel Channel Setting
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeSetChannel(uint8_t *arg_str)
{
    uint8_t buffer[40];
    
    uint16_t operation;
    uint8_t is_found;
    uint16_t set_value = 0;
    uint8_t fast_scan_status;
    
    uint8_t dsad_channel[NUM_OF_DSAD_CHANNEL];
    uint8_t i = 0;
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    memset(&dsad_channel, 0, NUM_OF_DSAD_CHANNEL);
    
    if (arg_str != NULL)
    {
        /* Scan the arguments */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        fast_scan_status = COMMAND_FastInputScan(dsad_channel, buffer, NUM_OF_DSAD_CHANNEL);

        if (fast_scan_status != 0)
        {
            CMD_Printf((uint8_t *)"Parameter error, check channel selection\n\r");  
            return 1;
        }
        
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        if (arg_str != NULL)
        {
            operation = COMMAND_DecStringToUnsigned16(buffer, &is_found);
            
            if (is_found != 1 || 
                (operation != 0 && operation != 1))
            {
                CMD_Printf((uint8_t *)"Parameter error, operation is 0 or 1 only\n\r"); 
                return 1;
            }
            
            /* Up to here, parameter is OK, set the value for DSADMR */
            
            while ((i < NUM_OF_DSAD_CHANNEL) &&
                    (dsad_channel[i] != 0 || i == 0))
            {
                if (operation)
                {
                    DSADMR |= 0x0101 << dsad_channel[i];
                }
                else
                {
                    DSADMR &= (uint16_t)(~(0x0101 << dsad_channel[i]));
                }
                i++;
            }
            
            CMD_Printf((uint8_t *)"Set channel OK\n\r");
        }
        else
        {
            CMD_Printf((uint8_t *)"No state input\n\r");
            return 1;
        }
    }
    
    CMD_Printf((uint8_t *)"Channel operation is as below: \n\r");   
#if (MCU_PIN == 100)    
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s\n\r", 
                                "CH0",
                                "CH1",
                                "CH2",
                                "CH3"); 
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s\n\r",
                                (DSADMR & 0x0101) ? "ON" : "OFF",
                                (DSADMR & 0x0202) ? "ON" : "OFF",
                                (DSADMR & 0x0404) ? "ON" : "OFF",
                                (DSADMR & 0x0808) ? "ON" : "OFF");  
#else
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s\n\r", 
                                "CH0",
                                "CH1",
                                "CH2"); 
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s\n\r",
                                (DSADMR & 0x0101) ? "ON" : "OFF",
                                (DSADMR & 0x0202) ? "ON" : "OFF",
                                (DSADMR & 0x0404) ? "ON" : "OFF");
#endif
    return 0;   
}


/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeAdjustPhase(uint8_t *arg_str)
* Description      : Command Invoke DSAD Channel Phase Adjustment
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeAdjustPhase(uint8_t *arg_str)
{
    uint8_t buffer[40];
    uint8_t dsad_channel[NUM_OF_DSAD_CHANNEL];
    uint16_t phase_value;
    uint8_t is_found;
    uint8_t fast_scan_status;
    uint16_t *ptr_phcr_base = (uint16_t *)&DSADPHCR0;
    uint8_t i = 0;
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    if (arg_str != NULL)
    {
        /* Scan the arguments */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        fast_scan_status = COMMAND_FastInputScan(dsad_channel, buffer, NUM_OF_DSAD_CHANNEL);

        if (fast_scan_status != 0)
        {
            CMD_Printf((uint8_t *)"Parameter error, check channel selection\n\r");  
            return 1;
        }
        
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        
        if (arg_str != NULL)
        {
            phase_value = COMMAND_DecStringToUnsigned16(buffer, &is_found);
            
            if (is_found != 1 || 
                (phase_value > 1151))
            {
                CMD_Printf((uint8_t *)"Parameter error, phase value is incorrect\n\r"); 
                return 1;
            }
            
            /* Up to here, parameter is OK, set the value for DSADPHCRx */
            
            while ((i < NUM_OF_DSAD_CHANNEL) &&
                    (dsad_channel[i] != 0 || i == 0))
            {
                *(ptr_phcr_base + dsad_channel[i]) = phase_value;
                i++;
            }
            
            CMD_Printf((uint8_t *)"Set phase OK\n\r");  
        }
        else
        {
            CMD_Printf((uint8_t *)"No phase step input\n\r");
            return 1;   
        }
    }
    
    CMD_Printf((uint8_t *)"Phase value is as below: \n\r"); 
#if (MCU_PIN == 100)    
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s\n\r", 
                                "CH0",
                                "CH1",
                                "CH2",
                                "CH3"); 
    CMD_Printf((uint8_t *)"%-10d %-10d %-10d %-10d\n\r",
                                DSADPHCR0,
                                DSADPHCR1,
                                DSADPHCR2,
                                DSADPHCR3); 
#else
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s\n\r", 
                                "CH0",
                                "CH1",
                                "CH2"); 
    CMD_Printf((uint8_t *)"%-10d %-10d %-10d\n\r",
                                DSADPHCR0,
                                DSADPHCR1,
                                DSADPHCR2); 
#endif
    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeSetFstep(uint8_t *arg_str)
* Description      : Command Invoke Set the Fstep offset
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
extern uint8_t g_enable_auto_fstep;

static uint8_t COMMAND_InvokeSetFstepOffset(uint8_t *arg_str)
{
    uint8_t buffer[40];
    float32_t offset;
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    offset = (float32_t)EM_ADC_SAMP_GetTimerOffset();
    
    if (arg_str != NULL)
    {
        /* Scan value */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");

        if (arg_str != NULL)
        {
            offset = EM_atof(buffer);
        
        }
        
        EM_ADC_SAMP_SetTimerOffset((int16_t)offset);
        
        CMD_Printf((uint8_t *)"Set Fstep timer offset OK\n\r"); 
    }
    
    CMD_Printf((uint8_t *)"Fstep offset value: %f\n\r", offset);
    
    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeSetAutoFstep(uint8_t *arg_str)
* Description      : Command Invoke Set Auto Fstep offset mode
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeSetAutoFstep(uint8_t *arg_str)
{
    uint8_t buffer[40];
    uint8_t status;
    uint8_t i = 0;
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    if (arg_str != NULL)
    {
        /* Scan value */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");

        if (buffer[0] == '0' ||
            buffer[0] == '1')
        {
            status = (uint8_t)EM_atoi(buffer);  
        
        }
        else
        {
            CMD_Printf((uint8_t *)"0 or 1 only\n\r");   
            return 1;
        }
        
        g_enable_auto_fstep = status;
    }
    
    CMD_Printf((uint8_t *)"Fstep status: %s\n\r", (g_enable_auto_fstep == 1 ? "enable" : "disable"));   
    
    return 0;
}

/******************************************************************************
* Function Name   : COMMAND_InvokeCalibration
* Interface       : static void COMMAND_InvokeCalibration(uint8_t *arg_str)
* Description     : Command Invoke Calibration
* Arguments       : uint8_t * arg_str: Arguments string
* Function Calls  : None
* Return Value    : None
******************************************************************************/
static uint8_t COMMAND_InvokeCalibration(uint8_t *arg_str)
{
    uint8_t rlt, count;

    /* RTC time */
    rtc_calendarcounter_value_t rtctime;
    uint32_t start_second, end_second;
    
    /* Result of calib */
    calib_output_t result;
    
    /* Command parameters */
    float32_t       imax, v, i;             /* Voltage and Current for calibrating */
    uint16_t        c, cp;                  /* Number of line cycle for calibrating & phase error loop */
    uint8_t         line;                   
    uint8_t         line_name;
    uint8_t         is_calibrate_v90;
    dsad_channel_t  channel;                /* Choose calibrated channel */
    
    EM_CALIBRATION  calib;                  /* Calibrated data storage for EM core */
    uint8_t         buffer[20];             /* Data buffer to store input arguments */
    
    float32_t       degree_list[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    float32_t       gain_list[EM_GAIN_PHASE_NUM_LEVEL_MAX];
    
    static uint8_t calib_counter = 0;       /* Count the calibration steps */
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");

    memset(&degree_list[0], 0, sizeof(degree_list));
    memset(&gain_list[0], 0, sizeof(gain_list));
    
    /* Get c parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" "); 
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        c = (uint16_t)EM_atol(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get cp parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" "); 
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        cp = (uint16_t)EM_atol(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get imax parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" "); 
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        imax = EM_atof(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get v parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" "); 
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        v = EM_atof(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get i parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" "); 
    if (arg_str != NULL &&
        (buffer[0] >= '0' && buffer[0] <= '9'))
    {   
        /* get number */
        i = EM_atof(buffer);
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Get line parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" "); 
    if (arg_str != NULL)
    {   
        /* get number */
        line_name = buffer[0];
        
        if (line_name == 'r' || line_name =='R' || line_name == '0')
        {
            line = 0;
            line_name = 'R';
        }
        else if (line_name == 'y' || line_name =='Y' || line_name == '1')
        {
            line = 1;
            line_name = 'Y';
        }
        else if (line_name == 'b' || line_name =='B' || line_name == '2')
        {
            line = 2;
            line_name = 'B';
        }
        else if (line_name == 'n' || line_name =='N' || line_name == '3')
        {
            line = 3;
            line_name = 'N';
        }
        else 
        {
            CMD_SendString((uint8_t *)"Parameter error\n\r");
            return 1;
        }
        
    }
    else
    {
        CMD_SendString((uint8_t *)"Parameter error\n\r");
        return 1;
    }
    
    /* Stop EM operation */
    CMD_Printf((uint8_t *)"\n\rStop EM Core...");
    if (EM_Stop() != EM_OK)
    {
        CMD_Printf((uint8_t *)"FAILED\n\r");
        return 1;
    }
    else
    {
        CMD_Printf((uint8_t *)"OK\n\r");
    }

    /* Get RTC time as start time */
    CMD_Printf((uint8_t *)"Start time..."); 
    R_RTC_Get_CalendarCounterValue(&rtctime);
    
    CMD_Printf((uint8_t *)"%02x/%02x/20%02x %02x:%02x:%02x %02x\n\r",
                rtctime.rdaycnt,
                rtctime.rmoncnt,
                rtctime.ryrcnt,
                rtctime.rhrcnt,
                rtctime.rmincnt,
                rtctime.rseccnt,
                rtctime.rwkcnt);
                
    _BCD2DEC(rtctime.rdaycnt);
    _BCD2DEC(rtctime.rmoncnt);
    _BCD2DEC(rtctime.ryrcnt);
    _BCD2DEC(rtctime.rhrcnt);
    _BCD2DEC(rtctime.rmincnt);
    _BCD2DEC(rtctime.rseccnt);

    start_second = (uint32_t)rtctime.rhrcnt * 60 * 60 +     /* Convert HOUR into SECOND */
                    (uint32_t)rtctime.rmincnt * 60 +        /* Convert MINNUTE into SECOND */
                    (uint32_t)rtctime.rseccnt;

    calib = EM_GetCalibInfo();
    
    /* Calibrating */
    if (line < 3)
    {
        CMD_Printf((uint8_t *)"Calibrating volt and current of phase %c...", line_name);
        rlt = R_CALIB_CalibrateVoltAndPhase(c, cp, imax, v, i, &result, line);
        if (rlt != 0)
        {
            CMD_Printf((uint8_t *)"FAILED\n\r");
            
            CMD_Printf((uint8_t *)"Start EM Core...");
            EM_Start();
            
            return 1;
        }
    }   
    else
    {
        CMD_Printf((uint8_t *)"Calibrating line neutral...");
        rlt = R_CALIB_CalibrateVoltAndNeutral(c, cp, imax, v, i, &result);
        if (rlt != 0)
        {
            CMD_Printf((uint8_t *)"FAILED\n\r");
            CMD_Printf((uint8_t *)"Start EM Core...");
            EM_Start();
            return 1;
        }
    }
    
    if (result.angle90_offset > 0 || result.angle90_offset < 0)
    {
        is_calibrate_v90 = 1;
    }
    else
    {
        is_calibrate_v90 = 0;   
    }
    
    CMD_Printf((uint8_t *)"OK\n\r");
    if (line == 3)
    {
        /* Print out all calibrated data */
        CMD_Printf((uint8_t *)"-------------------------------------------\n\r");
        CMD_Printf((uint8_t *)"Current gain       = %f\n\r", result.gain);
        CMD_Printf((uint8_t *)"I coeff            = %f\n\r", result.icoeff);
        CMD_Printf((uint8_t *)"-------------------------------------------\n\r");
    }
    else
    {
        /* Print out all calibrated data */
        CMD_Printf((uint8_t *)"-------------------------------------------\n\r");
        CMD_Printf((uint8_t *)"Sampling frequency = %f\n\r", result.fs);
        CMD_Printf((uint8_t *)"Current gain       = %f\n\r", result.gain);
        CMD_Printf((uint8_t *)"V coeff            = %f\n\r", result.vcoeff);
        CMD_Printf((uint8_t *)"I coeff            = %f\n\r", result.icoeff);
        CMD_Printf((uint8_t *)"Power Coeff        = %f\n\r", result.pcoeff);
        CMD_Printf((uint8_t *)"Phase shift        = %f\n\r", result.angle_error);
        if (is_calibrate_v90 == 1)
        {
            CMD_Printf((uint8_t *)"V90 Step Offset    = %f\n\r", result.angle90_offset);
        }
        CMD_Printf((uint8_t *)"Voltage Offset     = %f\n\r", result.voffset);
        CMD_Printf((uint8_t *)"-------------------------------------------\n\r");
    }

    /* Backup data and check auto calibration result */
    degree_list[0]                                              = result.angle_error;
    /* Gain list not support yet: not just get from the ADC gain */
    gain_list[0]                                                = result.gain;
    if (line == 0)  
    {
        calib.common.sampling_frequency                         = result.fs;
        calib.coeff.phase_r.vrms                                = result.vcoeff;
        calib.coeff.phase_r.irms                                = result.icoeff;
        calib.coeff.phase_r.active_power                        = result.pcoeff;
        calib.coeff.phase_r.reactive_power                      = result.pcoeff;
        calib.coeff.phase_r.apparent_power                      = result.pcoeff;
        calib.sw_phase_correction.phase_r.i_phase_degrees       = degree_list;
        calib.sw_gain.phase_r.i_gain_values                     = gain_list;
        calib.offset.phase_r.v                                  = result.voffset;
        calib.offset.phase_r.i                                  = result.ioffset;
        channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R;
    }
    else if (line == 1)
    {
        calib.common.sampling_frequency                         = result.fs;
        calib.coeff.phase_y.vrms                                = result.vcoeff;
        calib.coeff.phase_y.irms                                = result.icoeff;
        calib.coeff.phase_y.active_power                        = result.pcoeff;
        calib.coeff.phase_y.reactive_power                      = result.pcoeff;
        calib.coeff.phase_y.apparent_power                      = result.pcoeff;
        calib.sw_phase_correction.phase_y.i_phase_degrees       = degree_list;
        calib.sw_gain.phase_y.i_gain_values                     = gain_list;
        calib.offset.phase_y.v                                  = result.voffset;
        calib.offset.phase_y.i                                  = result.ioffset;
        channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y;
    }
    else if (line == 2)
    {
        calib.common.sampling_frequency                         = result.fs;
        calib.coeff.phase_b.vrms                                = result.vcoeff;
        calib.coeff.phase_b.irms                                = result.icoeff;
        calib.coeff.phase_b.active_power                        = result.pcoeff;
        calib.coeff.phase_b.reactive_power                      = result.pcoeff;
        calib.coeff.phase_b.apparent_power                      = result.pcoeff;
        calib.sw_phase_correction.phase_b.i_phase_degrees       = degree_list;
        calib.sw_gain.phase_b.i_gain_values                     = gain_list;
        calib.offset.phase_b.v                                  = result.voffset;
        calib.offset.phase_b.i                                  = result.ioffset;
        channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B;
    }
    else
    {
        calib.coeff.neutral.irms                                = result.icoeff;
        channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL;
    }
    
    if (is_calibrate_v90 == 1)
    {
        EM_ADC_SAMP_SetTimerOffset((int16_t)result.angle90_offset);
    }
    
    /* Set calibrated data */
    CMD_Printf((uint8_t *)"Seting EM Core Calibration Info...");
    if (EM_SetCalibInfo(&calib) != EM_OK)
    {
        CMD_Printf((uint8_t *)"FAILED\n\r");
        return 1;
    }
    else
    {
        CMD_Printf((uint8_t *)"OK\n\r");
    }
    
    /* Driver ADC Gain */
    R_DSADC_SetChannelGain(
        channel,
        R_DSADC_GetGainEnumValue((uint8_t)result.gain)
    );
    

    /* Backup calibrated data into EEPROM */
    CMD_Printf((uint8_t *)"Backup EM Core Calibration Into MCU Configuration Page...");
    if (CONFIG_Backup(CONFIG_ITEM_CALIB) != EM_OK)
    {
        CMD_Printf((uint8_t *)"FAILED\n\r");
        return 1;
    }
    else
    {
        CMD_Printf((uint8_t *)"OK\n\r");
    }
    
    /* Start EM operation */
    CMD_Printf((uint8_t *)"Start EM Core...");
    if (EM_Start() != EM_OK)
    {
        CMD_Printf((uint8_t *)"FAILED\n\r");
        return 1;
    }
    else
    {
        CMD_Printf((uint8_t *)"OK\n\r");
    }

    /* Get RTC time as stop time */
    CMD_Printf((uint8_t *)"Stop time...");  
    R_RTC_Get_CalendarCounterValue(&rtctime);
    
    CMD_Printf((uint8_t *)"%02x/%02x/20%02x %02x:%02x:%02x %02x\n\r",
                rtctime.rdaycnt,
                rtctime.rmoncnt,
                rtctime.ryrcnt,
                rtctime.rhrcnt,
                rtctime.rmincnt,
                rtctime.rseccnt,
                rtctime.rwkcnt);

    _BCD2DEC(rtctime.rdaycnt);
    _BCD2DEC(rtctime.rmoncnt);
    _BCD2DEC(rtctime.ryrcnt);
    _BCD2DEC(rtctime.rhrcnt);
    _BCD2DEC(rtctime.rmincnt);
    _BCD2DEC(rtctime.rseccnt);

    end_second = (uint32_t)rtctime.rhrcnt * 3600 +
                 (uint32_t)rtctime.rmincnt * 60 +
                 (uint32_t)rtctime.rseccnt;
    
    CMD_Printf((uint8_t *)"Calibration time: %i (second)\n\r", end_second - start_second);
    
    /* Request to calibrate next channel */
    if (line < 3)
    {
        CMD_Printf((uint8_t *)"\n\rFINISH CALIBRATION FOR LINE PHASE %c", line_name);
    }
    else if (line == 3)
    {
        CMD_Printf((uint8_t *)"\n\rFINISH CALIBRATION FOR LINE NEUTRAL");
    }
    
    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeSetCalibInfo(uint8_t *arg_str)
* Description      : Command Invoke Set calibration info
* Arguments        : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeSetCalibInfo(uint8_t *arg_str)
{
    EM_CALIBRATION  calib;           /* Calibrated data storage for EM core */
    uint8_t buffer[20];             /* Data buffer to store input arguments */
    
    dsad_reg_setting_t regs;
    uint8_t driver_adc_gain0[6] = {0, 0, 0, 0, 0, 0};
    uint8_t driver_adc_gain1[6] = {0, 0, 0, 0, 0, 0};
    int16_t driver_timer_offset;
    uint16_t rlt;
    dsad_gain_t dsad_gain;
    uint8_t         *p_cmd;
    uint8_t         b_input = 0;
    uint8_t         input_valid;
    MD_STATUS       status;
    uint8_t         is_edit;
    
    CMD_SendString((uint8_t *)"\n\rParameter(s): ");
    CMD_SendString((uint8_t *)arg_str);
    CMD_SendString((uint8_t *)"\n\r");
    
    /* Get Addr parameter */
    arg_str = COMMAND_GetScanOneParam(buffer, 20, arg_str, (uint8_t *)" ", (uint8_t *)" ");
    
    if (arg_str != NULL)
    {
        if ((buffer[0] == '0' || buffer[0] == '1'))
        {
            is_edit = buffer[0] - '0';
        }
    }
    else
    {
        is_edit = 0;
    }
    
    /* Ack the lask received CMD */
    CMD_AckCmdReceivedNoHeader();
    
    /* Set current calibration info */
    calib = EM_GetCalibInfo();
    
    R_DSADC_GetGainAndPhase(&regs);
    
    driver_adc_gain0[0] = regs.gain0;
    driver_adc_gain1[0] = regs.gain1;
    
    driver_timer_offset = EM_ADC_SAMP_GetTimerOffset();
    
    
    /* Display current information */
    CMD_SendString((uint8_t *)"\n\r Current calibration info:");
    CMD_SendString((uint8_t *)"\n\r Common:"); 
    CMD_Printf((uint8_t *)"\n\r - Sampling frequency: %.04f", calib.common.sampling_frequency);
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Coefficient, Phase_R: ");
    CMD_Printf((uint8_t *)"\n\r - VRMS: %.04f", calib.coeff.phase_r.vrms);
    CMD_Printf((uint8_t *)"\n\r - IRMS: %.04f", calib.coeff.phase_r.irms);
    CMD_Printf((uint8_t *)"\n\r - Active: %.04f", calib.coeff.phase_r.active_power);
    CMD_Printf((uint8_t *)"\n\r - Reactive: %.04f", calib.coeff.phase_r.reactive_power);
    CMD_Printf((uint8_t *)"\n\r - Apparent: %.04f", calib.coeff.phase_r.apparent_power);
    
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Coefficient, Phase_Y: ");
    CMD_Printf((uint8_t *)"\n\r - VRMS: %.04f", calib.coeff.phase_y.vrms);
    CMD_Printf((uint8_t *)"\n\r - IRMS: %.04f", calib.coeff.phase_y.irms);
    CMD_Printf((uint8_t *)"\n\r - Active: %.04f", calib.coeff.phase_y.active_power);
    CMD_Printf((uint8_t *)"\n\r - Reactive: %.04f", calib.coeff.phase_y.reactive_power);
    CMD_Printf((uint8_t *)"\n\r - Apparent: %.04f", calib.coeff.phase_y.apparent_power);

    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Coefficient, Phase_B: ");
    CMD_Printf((uint8_t *)"\n\r - VRMS: %.04f", calib.coeff.phase_b.vrms);
    CMD_Printf((uint8_t *)"\n\r - IRMS: %.04f", calib.coeff.phase_b.irms);
    CMD_Printf((uint8_t *)"\n\r - Active: %.04f", calib.coeff.phase_b.active_power);
    CMD_Printf((uint8_t *)"\n\r - Reactive: %.04f", calib.coeff.phase_b.reactive_power);
    CMD_Printf((uint8_t *)"\n\r - Apparent: %.04f", calib.coeff.phase_b.apparent_power);

    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Offset, Phase_R: ");
    CMD_Printf((uint8_t *)"\n\r - Voltage: %.04f", (float32_t)calib.offset.phase_r.v);
    
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Offset, Phase_Y: ");
    CMD_Printf((uint8_t *)"\n\r - Voltage: %.04f", (float32_t)calib.offset.phase_y.v);
    
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Offset, Phase_B: ");
    CMD_Printf((uint8_t *)"\n\r - Voltage: %.04f", (float32_t)calib.offset.phase_b.v);
    
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Gain: ");
    CMD_Printf((uint8_t *)"\n\r Phase_R Current Gain: %s", COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R) )) );
    CMD_Printf((uint8_t *)"\n\r Phase_Y Current Gain: %s", COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y) )) );
    CMD_Printf((uint8_t *)"\n\r Phase_B Current Gain: %s", COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B) )) );
    CMD_Printf((uint8_t *)"\n\r Neutral Current Gain: %s", COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL) )) );
    
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Phase: ");
    CMD_Printf((uint8_t *)"\n\r Phase_R: %.06f degree", *(calib.sw_phase_correction.phase_r.i_phase_degrees));
    CMD_Printf((uint8_t *)"\n\r Phase_Y: %.06f degree", *(calib.sw_phase_correction.phase_y.i_phase_degrees));
    CMD_Printf((uint8_t *)"\n\r Phase_B: %.06f degree", *(calib.sw_phase_correction.phase_b.i_phase_degrees));
    
    
    CMD_SendString((uint8_t *)"\n\r"); 
    CMD_SendString((uint8_t *)"\n\r Timer offset: ");
    CMD_Printf((uint8_t *)"\n\r Double sampling: %.04f ", (float32_t)driver_timer_offset);
    
    CMD_SendString((uint8_t *)"\n\r"); 
    
    if (is_edit)
    {
        /* Stop EM operation */
        CMD_Printf((uint8_t *)"\n\rStop EM Core...");
        if (EM_Stop() != EM_OK)
        {
            CMD_Printf((uint8_t *)"FAILED\n\r");
            return 1;
        }
        else
        {
            CMD_Printf((uint8_t *)"OK\n\r");
        }
        
        CMD_SendString((uint8_t *)"\n\r"); 
        CMD_SendString((uint8_t *)"\n\r --- Follow instruction below to adjust calib info --- ");
        CMD_SendString((uint8_t *)"\n\r --- Leave empty, press enter to ignore changes --- ");
        
        CMD_SendString((uint8_t *)"\n\r SET SAMPLING FREQUENCY: ");
        /* Wait for user input value : sampling_frequency */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Sampling frequency > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.common.sampling_frequency = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        CMD_SendString((uint8_t *)"\n\r SET PHASE_R COEFFICIENTS: ");
        /* PHASE_R COEFF */
        /* Wait for user input value : Phase_R VRMS */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_R VRMS > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.coeff.phase_r.vrms = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Phase_R IRMS */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_R IRMS > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.coeff.phase_r.irms = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Phase_R angle */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_R Angle > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.sw_phase_correction.phase_r.i_phase_degrees[0] = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input: Voltage offset */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_R V-offset > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    int32_t temp;
                    temp = (int32_t)EM_atof(p_cmd);
                    if (temp > 65535.0f || temp < 0.0f)
                    {
                        input_valid = 0;
                        CMD_Printf((uint8_t *)"\n\r   Invalid number, 0-65535 only");
                    }
                    else
                    {
                        calib.offset.phase_r.v = temp;
                    }
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        calib.coeff.phase_r.active_power = 
        calib.coeff.phase_r.reactive_power = 
        calib.coeff.phase_r.apparent_power =    calib.coeff.phase_r.vrms *
                                                calib.coeff.phase_r.irms;
        
        /* PHASE_Y COEFF */
        CMD_SendString((uint8_t *)"\n\r SET PHASE_Y COEFFICIENTS: ");
        /* Wait for user input value : Phase_Y VRMS */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_Y VRMS > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.coeff.phase_y.vrms = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Phase_Y IRMS */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_Y IRMS > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.coeff.phase_y.irms = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Phase_Y angle */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_Y Angle > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.sw_phase_correction.phase_y.i_phase_degrees[0] = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait user input value: Voltage offset */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_Y V-offset > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    int32_t temp;
                    temp = (int32_t)EM_atof(p_cmd);
                    if (temp > 65535.0f || temp < 0.0f)
                    {
                        input_valid = 0;
                        CMD_Printf((uint8_t *)"\n\r   Invalid number, 0-65535 only");
                    }
                    else
                    {
                        calib.offset.phase_y.v = temp;
                    }
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        calib.coeff.phase_y.active_power = 
        calib.coeff.phase_y.reactive_power = 
        calib.coeff.phase_y.apparent_power =    calib.coeff.phase_y.vrms *
                                                calib.coeff.phase_y.irms;
        
        /* PHASE_B COEFF */
        CMD_SendString((uint8_t *)"\n\r SET PHASE_B COEFFICIENTS: ");
        /* Wait for user input value : Phase_B VRMS */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_B VRMS > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.coeff.phase_b.vrms = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Phase_B IRMS */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_B IRMS > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.coeff.phase_b.irms = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Phase_B angle */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_B Angle > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    calib.sw_phase_correction.phase_b.i_phase_degrees[0] = (float32_t)EM_atof(p_cmd);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Voltage offset */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Phase_B V-offset > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                input_valid = COMMAND_IsFloatNumber(p_cmd);
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    int32_t temp;
                    temp = (int32_t)EM_atof(p_cmd);
                    if (temp > 65535.0f || temp < 0.0f)
                    {
                        input_valid = 0;
                        CMD_Printf((uint8_t *)"\n\r   Invalid number, 0-65535 only");
                    }
                    else
                    {
                        calib.offset.phase_b.v = temp;
                    }
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        calib.coeff.phase_b.active_power = 
        calib.coeff.phase_b.reactive_power = 
        calib.coeff.phase_b.apparent_power =    calib.coeff.phase_b.vrms *
                                                calib.coeff.phase_b.irms;
        
        
        /* ADC GAIN Setting */
        
        /* Wait for user input value : ADC Gain */
        CMD_SendString((uint8_t *)"\n\r SET ADC GAIN: ");
        /* Phase_R Gain */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   PHASE_R Gain > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                rlt = COMMAND_DecStringToUnsigned16(p_cmd, &input_valid);
                
                if (rlt != 1 &&
                    rlt != 2 &&
                    rlt != 4 &&
                    rlt != 8 &&
                    rlt != 16 &&
                    rlt != 32 )
                {
                    input_valid = 0;
                }
                
                if (input_valid != 1)
                {
                    CMD_SendString((uint8_t *)"\n\r   Value 1,2,4,8,16,32 only, please retry");
                }
                else
                {
                    dsad_gain = R_DSADC_GetGainEnumValue((uint8_t)rlt);
                    
                    status = R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R, dsad_gain);
                    if (status == MD_ARGERROR)
                    {
                        input_valid = 0;
                        CMD_SendString((uint8_t *)"\n\r   Channel0,2 max gain is 16, please retry");
                    }
                    
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Phase_Y Gain */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   PHASE_Y Gain > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                rlt = COMMAND_DecStringToUnsigned16(p_cmd, &input_valid);
                
                if (rlt != 1 &&
                    rlt != 2 &&
                    rlt != 4 &&
                    rlt != 8 &&
                    rlt != 16 &&
                    rlt != 32 )
                {
                    input_valid = 0;
                }
                
                if (input_valid != 1)
                {
                    CMD_SendString((uint8_t *)"\n\r   Value 1,2,4,8,16,32 only, please retry");
                }
                else
                {
                    dsad_gain = R_DSADC_GetGainEnumValue((uint8_t)rlt);
                    
                    status = R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y, dsad_gain);
                    if (status == MD_ARGERROR)
                    {
                        input_valid = 0;
                        CMD_SendString((uint8_t *)"\n\r   Channel0,2 max gain is 16, please retry");
                    }
                    
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Phase_B Gain */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   PHASE_B Gain > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                rlt = COMMAND_DecStringToUnsigned16(p_cmd, &input_valid);
                
                if (rlt != 1 &&
                    rlt != 2 &&
                    rlt != 4 &&
                    rlt != 8 &&
                    rlt != 16 &&
                    rlt != 32 )
                {
                    input_valid = 0;
                }
                
                if (input_valid != 1)
                {
                    CMD_SendString((uint8_t *)"\n\r   Value 1,2,4,8,16,32 only, please retry");
                }
                else
                {
                    dsad_gain = R_DSADC_GetGainEnumValue((uint8_t)rlt);
                    
                    status = R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B, dsad_gain);
                    if (status == MD_ARGERROR)
                    {
                        input_valid = 0;
                        CMD_SendString((uint8_t *)"\n\r   Channel0,2 max gain is 16, please retry");
                    }
                    
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Neutral Gain */
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Neutral Gain > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                rlt = COMMAND_DecStringToUnsigned16(p_cmd, &input_valid);
                
                if (rlt != 1 &&
                    rlt != 2 &&
                    rlt != 4 &&
                    rlt != 8 &&
                    rlt != 16 &&
                    rlt != 32 )
                {
                    input_valid = 0;
                }
                
                if (input_valid != 1)
                {
                    CMD_SendString((uint8_t *)"\n\r   Value 1,2,4,8,16,32 only, please retry");
                }
                else
                {
                    dsad_gain = R_DSADC_GetGainEnumValue((uint8_t)rlt);
                    
                    status = R_DSADC_SetChannelGain(EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL, dsad_gain);
                    if (status == MD_ARGERROR)
                    {
                        input_valid = 0;
                        CMD_SendString((uint8_t *)"\n\r   Channel0,2 max gain is 16, please retry");
                    }
                    
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        /* Wait for user input value : Timer Offset */
        CMD_SendString((uint8_t *)"\n\r SET TIMER OFFSET: ");
        do
        {
            CMD_SendString((uint8_t *)"\n\r   Timer offset > ");
            while (CMD_IsCmdReceived() == 0)
            {
                R_WDT_Restart();
            }
            p_cmd = CMD_Get();
            b_input = p_cmd[0];
            
            if (p_cmd[0] == 0)
            {
                input_valid = 1;
            }
            else
            {
                rlt = COMMAND_DecStringToSigned16(p_cmd, &input_valid);
                
                if (input_valid != 1)
                {
                    CMD_Printf((uint8_t *)"\n\r   Invalid number, please retry");
                }
                else
                {
                    EM_ADC_SAMP_SetTimerOffset(rlt);
                }
            }
            CMD_AckCmdReceivedNoHeader();
            
        } while ((input_valid != 1));
        
        
        
        CMD_SendString((uint8_t *)"\n\r");
        /* Set calibrated data */
        CMD_Printf((uint8_t *)"Seting EM Core Calibration Info...");
        if (EM_SetCalibInfo(&calib) != EM_OK)
        {
            CMD_Printf((uint8_t *)"FAILED\n\r");
        }
        else
        {
            CMD_Printf((uint8_t *)"OK\n\r");
        }
        
        /* Start EM operation */
        CMD_Printf((uint8_t *)"Start EM Core...");
        if (EM_Start() != EM_OK)
        {
            CMD_Printf((uint8_t *)"FAILED\n\r");
            return 1;
        }
        else
        {
            CMD_Printf((uint8_t *)"OK\n\r");
        }

        /* Backup calibrated data into EEPROM */
        CMD_Printf((uint8_t *)"Backup EM Core Calibration Into EEPROM...");
        if (CONFIG_Backup(CONFIG_ITEM_CALIB) != EM_OK)
        {
            CMD_Printf((uint8_t *)"FAILED\n\r");
            return 1;
        }
        else
        {
            CMD_Printf((uint8_t *)"OK\n\r");
        }
    }
    
    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeSetDSADChannelPower(uint8_t *arg_str)
* Description      : Command Invoke DSAD Channel Power
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeDSADPowerGetSet(uint8_t *arg_str)
{
    uint8_t buffer[40];
    
    uint16_t operation;
    uint8_t is_found;
    uint16_t set_value = 0;
    uint8_t fast_scan_status;
    
    uint8_t dsad_channel[EM_CURRENT_CHANNEL_NUM];
    uint8_t i = 0;
    
    static const uint8_t line_break[]   = "+----------------------------------+\n\r";
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    memset(&dsad_channel, 0, EM_CURRENT_CHANNEL_NUM);
    
    if (arg_str != NULL)
    {
        /* Scan the arguments */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        fast_scan_status = COMMAND_FastInputScan(dsad_channel, buffer, EM_CURRENT_CHANNEL_NUM);

        if (fast_scan_status != 0)
        {
            CMD_Printf((uint8_t *)"Parameter error, check channel selection\n\r");  
            return 1;
        }
        
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        if (arg_str != NULL)
        {
            operation = COMMAND_DecStringToUnsigned16(buffer, &is_found);
            
            if (is_found != 1 || 
                (operation != 0 && operation != 1))
            {
                CMD_Printf((uint8_t *)"Parameter error, operation is 0 or 1 only\n\r"); 
                return 1;
            }
            
            /* Up to here, parameter is OK, set the value for DSADMR */
            
            while ((i < EM_CURRENT_CHANNEL_NUM) &&
                    (dsad_channel[i] != 0 || i == 0))
            {
                if (operation)
                {
                    DSADMR |= 0x0101 << dsad_channel[i];
                }
                else
                {
                    DSADMR &= (uint16_t)(~(0x0101 << dsad_channel[i]));
                }
                i++;
            }
            
            CMD_Printf((uint8_t *)"Set channel power state OK\n\r");
        }
        else
        {
            CMD_Printf((uint8_t *)"No state input\n\r");
            return 1;
        }
    }
    
    CMD_Printf((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"DSAD Channel power state as below: \n\r");
    CMD_Printf((uint8_t *)line_break);  
    
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s\n\r", 
                                "CH0",
                                "CH1",
                                "CH2",
                                "CH3"); 
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s\n\r",
                                (DSADMR & 0x0101) ? "ON" : "OFF",
                                (DSADMR & 0x0202) ? "ON" : "OFF",
                                (DSADMR & 0x0404) ? "ON" : "OFF",
                                (DSADMR & 0x0808) ? "ON" : "OFF");
    return 0;   
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeAdjustPhase(uint8_t *arg_str)
* Description      : Command Invoke DSAD Channel Phase Adjustment
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeDSADPhaseGetSet(uint8_t *arg_str)
{
    uint8_t buffer[40];
    uint8_t dsad_channel[EM_CURRENT_CHANNEL_NUM];
    uint16_t phase_value;
    uint8_t is_found;
    uint8_t fast_scan_status;
    uint16_t *ptr_phcr_base = (uint16_t *)&DSADPHCR0;
    uint8_t i = 0;
    
    static const uint8_t line_break[]   = "+----------------------------------+\n\r";
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    if (arg_str != NULL)
    {
        /* Scan the arguments */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        fast_scan_status = COMMAND_FastInputScan(dsad_channel, buffer, EM_CURRENT_CHANNEL_NUM);

        if (fast_scan_status != 0)
        {
            CMD_Printf((uint8_t *)"Parameter error, check channel selection\n\r");  
            return 1;
        }
        
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        
        if (arg_str != NULL)
        {
            phase_value = COMMAND_DecStringToUnsigned16(buffer, &is_found);
            
            if (is_found != 1 || 
                (phase_value > 1151))
            {
                CMD_Printf((uint8_t *)"Parameter error, phase value is incorrect\n\r"); 
                return 1;
            }
            
            /* Up to here, parameter is OK, set the value for DSADPHCRx */
            
            while ((i < EM_CURRENT_CHANNEL_NUM) &&
                    (dsad_channel[i] != 0 || i == 0))
            {
                *(ptr_phcr_base + dsad_channel[i]) = phase_value;
                i++;
            }
            
            CMD_Printf((uint8_t *)"Set phase OK\n\r");  
        }
        else
        {
            CMD_Printf((uint8_t *)"No phase step input\n\r");
            return 1;   
        }
    }
    
    CMD_Printf((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"DSAD Phase value is as below: \n\r");
    CMD_Printf((uint8_t *)line_break);
    
    
    CMD_Printf((uint8_t *)"%-18s %-12s %-12s %-12s %-12s\n\r", 
                                " ",
                                "CH0",
                                "CH1",
                                "CH2",
                                "CH3");
    CMD_Printf((uint8_t *)"%-18s %-2s%-10x %-2s%-10x %-2s%-10x %-2s%-10x\n\r",
                                "Delay step (hex)",
                                "0x",DSADPHCR0,
                                "0x",DSADPHCR1,
                                "0x",DSADPHCR2,
                                "0x",DSADPHCR3);
    CMD_Printf((uint8_t *)"%-18s %-12d %-12d %-12d %-12d\n\r",
                                "Delay step (dec)",
                                DSADPHCR0,
                                DSADPHCR1,
                                DSADPHCR2,
                                DSADPHCR3);
    return 0;
}


/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeDSADHpfGetSet(uint8_t *arg_str)
* Description      : Command Invoke DSAD Channels High-pass filter
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/

static uint8_t COMMAND_InvokeDSADHpfGetSet(uint8_t *arg_str)
{
    uint8_t buffer[40];
    uint8_t dsad_channel[EM_CURRENT_CHANNEL_NUM];
    uint8_t hpf_cutoff;
    uint8_t state;
    uint8_t is_found;
    uint8_t fast_scan_status;
    uint8_t i = 0;
    
    static const uint8_t line_break[]   = "+----------------------------------+\n\r";
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    if (arg_str != NULL)
    {
        /* Scan channel selection */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        fast_scan_status = COMMAND_FastInputScan(dsad_channel, buffer, EM_CURRENT_CHANNEL_NUM);

        if (fast_scan_status != 0)
        {
            CMD_Printf((uint8_t *)"Parameter error, check channel selection\n\r");  
            return 1;
        }
        
        /* Scan state selection */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");

        if (arg_str != NULL)
        {
            state = (uint8_t)COMMAND_DecStringToUnsigned16(buffer, &is_found);
        
            if (is_found != 1 || 
                state > 1)
            {
                CMD_Printf((uint8_t *)"Parameter error, state selection uncorrect\n\r");    
                return 1;
            }
        }
        else
        {
            CMD_Printf((uint8_t *)"No state parameter input\n\r");
            return 1;       
        }
        
        if (state == 1)
        {
            /* Scan cut-off frequency selection */
            arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
            
            if (arg_str != NULL)
            {
                hpf_cutoff = (uint8_t)COMMAND_DecStringToUnsigned16(buffer, &is_found);
                
                if (is_found != 1 || 
                    hpf_cutoff > 3)
                {
                    CMD_Printf((uint8_t *)"Parameter error, cut-off frequency 0-3 only\n\r");   
                    return 1;
                }
            }
            else
            {
                CMD_Printf((uint8_t *)"No cut-off parameter input\n\r");
                return 1;
            }
        }
        /* Up to here, parameter is OK, set the value for DSADHPFCR */
        
        /* Set channel hpf operation state */
        while ((i < EM_CURRENT_CHANNEL_NUM) &&
                (dsad_channel[i] != 0 || i == 0))
        {
            if (state == 1)
            {
                /* Must clear bit 4 and 5 */
                DSADHPFCR &= (uint8_t)~(1<<dsad_channel[i] | 0x30);
            }
            else
            {
                DSADHPFCR |= (uint8_t)(1<<dsad_channel[i]);
            }
            i++;
        }
        
        if (state == 1)
        {
            /* Set cut-off frequency */
            DSADHPFCR &= 0x0F;
            DSADHPFCR |= (hpf_cutoff << 6);
        }
        CMD_Printf((uint8_t *)"Set HPF OK\n\r");    
    }
    
    CMD_Printf((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"DSAD High-pass filter information: \n\r");
    CMD_Printf((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s %-18s\n\r", 
                                "CH0",
                                "CH1",
                                "CH2",
                                "CH3",
                                "Cut-off Frequency");
    CMD_Printf((uint8_t *)"%-10s %-10s %-10s %-10s %-18s\n\r",
                                (DSADHPFCR&0x01) ? "OFF" : "ON",
                                (DSADHPFCR&0x02) ? "OFF" : "ON",
                                (DSADHPFCR&0x04) ? "OFF" : "ON",
                                (DSADHPFCR&0x08) ? "OFF" : "ON",
                                g_hpf_type[(DSADHPFCR & 0xC0) >> 6]);

    return 0;
}

/******************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeDSADGainGetSet(uint8_t *arg_str)
* Description      : Command Invoke DSAD Gain Get/Set
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
******************************************************************************/
static uint8_t COMMAND_InvokeDSADGainGetSet(uint8_t *arg_str)
{
    uint8_t buffer[40];
    uint8_t dsad_channel[EM_CURRENT_CHANNEL_NUM];
    uint16_t gain_value;
    uint8_t is_found;
    uint8_t fast_scan_status;
    uint8_t *ptr_gain_base = (uint8_t *)&DSADGCR0;
    uint8_t i = 0;
    
    static const uint8_t line_break[]   = "+----------------------------------+\n\r";
    
    CMD_Printf((uint8_t *)"\n\r");  
    
    if (arg_str != NULL)
    {
        /* Scan the arguments */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        fast_scan_status = COMMAND_FastInputScan(dsad_channel, buffer, EM_CURRENT_CHANNEL_NUM);

        if (fast_scan_status != 0)
        {
            CMD_Printf((uint8_t *)"Parameter error, check channel selection\n\r");  
            return 1;
        }
        
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
        
        if (arg_str != NULL)
        {
            gain_value = COMMAND_DecStringToUnsigned16(buffer, &is_found);
            
            if (is_found != 1 || 
                ((gain_value != 1) &&
                (gain_value != 2) &&
                (gain_value != 4) &&
                (gain_value != 8) &&
                (gain_value != 16) &&
                (gain_value != 32) &&
                (1)
                )
                )
            {
                CMD_Printf((uint8_t *)"Parameter error, gain value is 1,2,4,8,16,32 only \n\r"); 
                return 1;
            }
            
            /* Up to here, parameter is OK, set the value for DSADPHCRx */
            
            while ((i < EM_CURRENT_CHANNEL_NUM) &&
                    (dsad_channel[i] != 0 || i == 0))
            {
                R_DSADC_SetChannelGain(
                        (dsad_channel_t)dsad_channel[i],
                        R_DSADC_GetGainEnumValue(gain_value)
                    );
                i++;
            }
            
            CMD_Printf((uint8_t *)"Set gain OK\n\r");  
        }
        else
        {
            CMD_Printf((uint8_t *)"No gain value input\n\r");
            return 1;   
        }
    }
    
    CMD_Printf((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"DSAD Gain value is as below: \n\r");
    CMD_Printf((uint8_t *)line_break);
    
    
    CMD_Printf((uint8_t *)"%-18s %-12s %-12s %-12s %-12s\n\r", 
                                " ",
                                "CH0",
                                "CH1",
                                "CH2",
                                "CH3");
    CMD_Printf((uint8_t *)"%-18s %-12s %-12s %-12s %-12s\n\r",
                                "Gain value",
                                COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain((dsad_channel_t)0) )),
                                COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain((dsad_channel_t)1) )),
                                COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain((dsad_channel_t)2) )),
                                COMMAND_GetDsadGainName( R_DSADC_GetGainEnumValue( R_DSADC_GetChannelGain((dsad_channel_t)3) ))
                        );
    return 0;
}

/***********************************************************************************************************************
* Function Name    : static uint8_t COMMAND_InvokeDSTimerGetSet(uint8_t *arg_str)
* Description      : Invoke get, set value from/to double sampling timer
* Arguments        : uint8_t index: Command index
*                  : uint8_t *arg_str: Arguments string
* Functions Called : None
* Return Value     : uint8_t, execution code, 0 is success
***********************************************************************************************************************/
#define CPU_CLK         (24000000.0f)

static uint8_t COMMAND_InvokeDSTimerGetSet(uint8_t *arg_str)
{
    uint8_t buffer[40];
    uint8_t is_found;
    uint8_t phase_selection;
    
    uint32_t timer_value;
    static const uint8_t line_break[]   = "+----------------------------------+\n\r";
    
    CMD_Printf((uint8_t *)"\n\r");
    if (arg_str != NULL)
    {
        /* Scan setting value */
        arg_str = COMMAND_GetScanOneParam(buffer, 40, arg_str, (uint8_t *)" ", (uint8_t *)" ");
        
    
        timer_value = COMMAND_DecStringToUnsigned16(buffer, &is_found);
        
        if (is_found != 1 || 
            (timer_value > 65535))
        {
            CMD_Printf((uint8_t *)"Parameter error, timer value is incorrect\n\r"); 
            return 1;
        }
        
        TDR01 = (uint16_t)timer_value;
        CMD_Printf((uint8_t *)"Set Double Sampling Timer OK\n\r");  

    }
    CMD_Printf((uint8_t *)line_break);
    CMD_Printf((uint8_t *)"Double sampling timer information: \n\r");
    CMD_Printf((uint8_t *)line_break);
    
#ifdef METER_WRAPPER_ADC_COPY_REACTIVE_SAMPLE    
    CMD_Printf((uint8_t *)"%-23s %d\n\r", 
                                "SW delay steps V90: ",
                                g_wrp_adc_voltage90_circle_step-EM_ADC_DELAY_STEP_VOLTAGE
                );
#endif          

    CMD_Printf((uint8_t *)"%-23s %s\n\r", 
                                "Ignore timer sample: ",
                                (g_wrp_adc_tdr_ignore == 1) ? "Yes" : "No"
                );
                
    CMD_Printf((uint8_t *)"%-14s %-8s %-4i\n\r", 
                                "Timer offset",
                                "(dec)",
                                EM_ADC_SAMP_GetTimerOffset()
                );
    
    CMD_Printf((uint8_t *)"%-14s %-8s 0x%-4x\n\r", 
                                "Timer value: ",
                                "(hex)",
                                TDR01
                );
    CMD_Printf((uint8_t *)"%-14s %-8s %-4u\n\r", 
                                "Timer value",
                                "(dec)",
                                TDR01
                );
    CMD_Printf((uint8_t *)"%-14s %-8s %-4f\n\r", 
                                "Time delay",
                                 "(us)",
                                ((float32_t)TDR01/(float32_t)CPU_CLK) * 1e6
                );

    return 0;
}


/******************************************************************************
* Function Name   : COMMAND_InvokeCPULoad
* Interface       : static void COMMAND_InvokeCPULoad(uint8_t *arg_str)
* Description     : Command Invoke Watch DSAD
* Arguments       : uint8_t * arg_str: Arguments string
* Function Calls  : None
* Return Value    : None
******************************************************************************/
static uint8_t COMMAND_InvokeCPULoad(uint8_t *arg_str)
{
#ifdef METER_ENABLE_MEASURE_CPU_LOAD
    float max_dsad_time     = 0.0f;
    float min_dsad_time     = 0.0f;
    float avg_dsad_time     = 0.0f;
    EM_CALIBRATION calib    = EM_GetCalibInfo();
    
    LOADTEST_TAU_Init();
    
    g_timer0_dsad_sum_counter = 0;
    g_timer0_dsad_maxcounter = 0;
    g_timer0_dsad_mincounter = 0xFFFF;
    g_dsad_count = 0;
    g_timer0_diff = 0;
    
    CMD_Printf((uint8_t *)"\n\r");
    CMD_Printf((uint8_t *)"\n\rStarted measuring DSAD functions performance");
    CMD_Printf((uint8_t *)"\n\rPlease wait for about %0.3f seconds while system measuring",(float)g_dsad_max_count * 0.000256f);
    while(g_dsad_count < g_dsad_max_count)
    {
        R_WDT_Restart();    
    }
    
    /* 
        Substract cycles not related to EM processing (from Timer start to Timer stop
            No need now
    */
    g_timer0_dsad_maxcounter -= (0);
    
    max_dsad_time   = ((float)g_timer0_dsad_maxcounter      / (float)(LOADTEST_CGC_GetClock())) / 1000.0f;
    min_dsad_time   = ((float)g_timer0_dsad_mincounter      / (float)(LOADTEST_CGC_GetClock())) / 1000.0f;
    avg_dsad_time   = ((float)g_timer0_dsad_sum_counter     / ((float)(LOADTEST_CGC_GetClock()) * g_dsad_max_count)) / 1000.0f;
    
    CMD_Printf((uint8_t *)"\n\r-------------------------------------------");
    
    CMD_Printf((uint8_t *)"\n\r% -35s %00.3fus",
                            "Max CPU time spent in DSAD:",
                            max_dsad_time * 1e6f
              );
              
    CMD_Printf((uint8_t *)"\n\r% -35s %00.3f%%",
                            "Max CPU Load in percentage:",
                            ((max_dsad_time * calib.common.sampling_frequency)*100.0f)
              );
              
    CMD_Printf((uint8_t *)"\n\r% -35s %00.3fus",
                            "Min CPU time spent in DSAD:",
                            min_dsad_time * 1e6f
              );
              
    CMD_Printf((uint8_t *)"\n\r% -35s %00.3f%%",
                            "Min CPU Load in percentage:",
                            ((min_dsad_time * calib.common.sampling_frequency)*100.0f)
              );

    CMD_Printf((uint8_t *)"\n\r% -35s %00.3fus",
                            "Average CPU time spent in DSAD:",
                            avg_dsad_time * 1e6f
              );
              
    CMD_Printf((uint8_t *)"\n\r% -35s %00.3f%%",
                            "Average CPU Load in percentage:",
                            ((avg_dsad_time * calib.common.sampling_frequency)*100.0f)
              );              
              
    CMD_Printf((uint8_t *)"\n\r-------------------------------------------");

    LOADTEST_TAU_DeInit();
#else
    CMD_Printf((uint8_t *)"\n\r Not enable. Turn on by define METER_ENABLE_MEASURE_CPU_LOAD");
    
#endif

    
    return 0;   
}

/******************************************************************************
* Function Name : COMMAND_Init
* Interface     : void COMMAND_Init(void)
* Description   : Init the COMMAND application
* Arguments     : None
* Return Value  : None
******************************************************************************/
void COMMAND_Init(void)
{
    CMD_Init();
    CMD_RegisterSuggestion((void *)cmd_table, COMMAND_TABLE_LENGTH);
}

/***********************************************************************************************************************
* Function Name    : void COMMAND_PollingProcessing(void)
* Description      : COMMAND Polling Processing
* Arguments        : None
* Functions Called : None
* Return Value     : None
***********************************************************************************************************************/
void COMMAND_PollingProcessing(void)
{
    uint8_t i, is_found_cmd;
    uint8_t *pstr, *p_strarg;
    uint8_t cmdstr[64];

    /* Processing command line */
    if (CMD_IsCmdReceived())
    {
        /* Get command */
        pstr = CMD_Get();
        memset(cmdstr, 0, 64);

        /* Skip all ahead space */
        while (pstr[0] == ' ' && pstr[0] != 0)
        {
            pstr++;
        }
        
        /* Get command name */
        i = 0;
        while (pstr[0] != ' ' && pstr[0] != 0)
        {
            cmdstr[i++] = *pstr;
            pstr++;
        }
        
        /* Skip all ahead space on argument list */
        while (pstr[0] == ' ' && pstr[0] != 0)
        {
            pstr++;
        }
        p_strarg = pstr;

        /* is end of string */
        if (p_strarg[0] == 0)
        {
            p_strarg = NULL;    /* No parameter */
        }

        /* Find to invoke the command on the cmd_table */
        if (cmdstr[0] != 0)
        {
            is_found_cmd = 0;       /* Not found */
            for (i = 0; i < COMMAND_TABLE_LENGTH; i++)
            {
                /* is matched? */
                if (strcmp((const char *)cmdstr, (const char *)cmd_table[i].cmd_name) == 0)
                {
                    INVOKE_COMMAND(i, p_strarg);        /* Invoke to function */
                    is_found_cmd = 1;                   /* Found command */
                }
            }
            
            /* is NOT found command? */
            if (!is_found_cmd)
            {
                CMD_SendString((uint8_t *)"\n\rUnknown command: [");
                CMD_SendString(CMD_Get());
                CMD_SendString((uint8_t *)"]\n\rType ? for command list\n\r");
            }
        }

        CMD_AckCmdReceived();
    }
}


#endif /* __DEBUG */
