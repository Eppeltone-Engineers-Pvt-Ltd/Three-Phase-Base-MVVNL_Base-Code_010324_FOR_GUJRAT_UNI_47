/***********************************************************************************************************************
* File Name    : debug.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Debug Header file
***********************************************************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#ifdef __DEBUG
#include "cmd.h"            /* Command Driver */
#include "command.h"        /* Command Interface */
#include <string.h>         /* Standard String */
#include <stdarg.h>         /* Variant Args */
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifdef __DEBUG
    #define DEBUG_Init                  COMMAND_Init
    #define DEBUG_Start                 CMD_Start
    #define DEBUG_Stop                  CMD_Stop
    #define DEBUG_Printf                CMD_Printf
    #define DEBUG_PollingProcessing     COMMAND_PollingProcessing
    #define DEBUG_AckNewLine            CMD_AckCmdReceived
#else
    //#define __DEBUG_HEAD              /
    #define DEBUG_Init()                {;}//__DEBUG_HEAD/
    #define DEBUG_Start()               {;}//__DEBUG_HEAD/
    #define DEBUG_Stop()                {;}//__DEBUG_HEAD/
    #define DEBUG_Printf                {;}//__DEBUG_HEAD/
    #define DEBUG_PollingProcessing()   {;}//__DEBUG_HEAD/
    #define DEBUG_AckNewLine()          {;}//__DEBUG_HEAD/
#endif

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
*****************************************************************************/


#endif /* _DEBUG_H */

