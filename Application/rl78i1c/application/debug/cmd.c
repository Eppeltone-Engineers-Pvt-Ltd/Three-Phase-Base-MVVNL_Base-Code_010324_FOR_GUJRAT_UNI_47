/***********************************************************************************************************************
* File Name    : cmd.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : CMD Prompt Middleware Layer APIs
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "iodefine.h"
#include "r_cg_macrodriver.h"   /* Macro Driver Definitions */
#include "r_cg_wdt.h"           /* WDT Driver */

#include "typedef.h"            /* GSCE Standard Typedef */

/* Wrapper */
#include "wrp_user_uart.h"      /* Wrapper UART Layer */
#include "wrp_em_wdt.h"         /* Wrapper EM WDT */

/* Application */
#include "cmd.h"                /* CMD Prompt Middleware Layer */
#include <stdio.h>              /* Standard IO */
#include <stdarg.h>             /* Variables argument */

#ifdef __DEBUG

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct tagCMDFlag
{
    uint8_t bIsSent             :1;     /* is byte sent? */
    uint8_t bIsReceived         :1;     /* is byte received? */
    uint8_t bIsStarted          :1;     /* is CMD MW Started? */
    uint8_t bIsCmdReceived      :1;     /* is CMD Received? */
    uint8_t bIsCtrlKeyReceived  :1;     /* is control key received? */
    uint8_t                     :3;     /* Reserved */
} CMD_FLAG;

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

typedef uint8_t * KEYWORD;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* WDT I/F Mapping */
/* Link to EM Core */
#define     CMD_DEVICE_WDTRestart()                 EM_WDT_Restart()

/* Interface mapping, for each driver,
 * we must re-map again about the CMD_DRIVER interface
 */
/* Link to Wrapper/User */
#define     CMD_DEVICE_Init()                       WRP_UART_Init()
#define     CMD_DEVICE_Start()                      WRP_UART_Start()
#define     CMD_DEVICE_Stop()                       WRP_UART_Stop()
#define     CMD_DEVICE_SendData(data, length)       WRP_UART_SendData(data, length)
#define     CMD_DEVICE_ReceiveData(data, length)    WRP_UART_ReceiveData(data, length)

/* CMD */
#define CMD_KEY_ESC             0x1b    /* ESC key code */
#define CMD_KEY_UP              0x41    /* UP key code */
#define CMD_KEY_DOWN            0x42    /* DOWN key code */
#define CMD_KEY_RIGHT           0x43    /* RIGHT key code */
#define CMD_KEY_LEFT            0x44    /* LEFT key code */
#define CMD_KEY_ENTER           0x0d    /* ENTER key code */
#define CMD_KEY_BACKSPACE       0x08    /* BACKSPACE key code */
#define CMD_KEY_DELETE          0x7f    /* DELETE key code */
#define CMD_KEY_HOME            0x31    /* HOME func key code */
#define CMD_KEY_END             0x34    /* END func key code */
#define CMD_KEY_TAB             0x09    /* TAB key code */

/* Control keys */
#define CMD_KEY_CTRL_SPACE      0x00    /* CTRL+SPACE key code */
#define CMD_KEY_CTRL_C          0x03    /* CTRL+C key code */

#define CMD_BUF_SIZE            64      /* Each CMD is 64 bytes */
#define CMD_LAST_CMD_LENGTH     10      /* Number of CMD to remember */

/* VT100 */
#define VT100_CMD_SIZE          20      /* Max. VT100 CMD size */

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Static functions */
static void     s_strcpy(uint8_t *src, uint8_t *des, uint16_t length);
static void     s_stradd(uint8_t *src, uint8_t *des);
static uint16_t s_strlen(uint8_t *str);
static int8_t   s_strcmp(uint8_t *src, uint8_t *des);
static void     s_sendstring(uint8_t *str, uint8_t bwait);
static void     s_restorelastcmd(uint8_t position);
static void     s_getcommand(uint8_t ch);
static void s_refresh_line(uint16_t position, uint8_t b_show_hint, uint8_t b_sync);
static uint16_t s_backspace_1char_and_display(uint16_t position);
static uint16_t s_delete_1char_and_display(uint16_t position);
static uint16_t s_insert_1char_and_display(uint8_t c, uint16_t position);
static COMMAND_ITEM* s_search_suggest(uint8_t *keyword);

/* CMD Name, Version Definitions */
const uint8_t g_cnst_cmd_prompt[]       = "CMD> ";
const uint8_t g_cnst_cmd_newline[]      = "\n\r";

/* VT100 Key code */
const uint8_t g_cnst_cmd_backchar[]     = {8, ' ', 8, 0};
#if (CMD_USING_BUILT_IN_CMD)
const uint8_t g_cnst_cmd_cls[]          = {CMD_KEY_ESC, '[', '2', 'J', CMD_KEY_ESC, '[', 'H', 0};
#endif

const uint8_t g_cnst_cmd_save_cursor[]      = {CMD_KEY_ESC, '7', 0};
const uint8_t g_cnst_cmd_restore_cursor[]   = {CMD_KEY_ESC, '8', 0};
const uint8_t g_cnst_cmd_cursor_left[]      = {CMD_KEY_ESC, '[', 'D', 0};
const uint8_t g_cnst_cmd_cursor_right[]     = {CMD_KEY_ESC, '[', 'C', 0};

const uint8_t g_cnst_cmdf_func[]            = {CMD_KEY_ESC, '[', 0};
const uint8_t g_cnst_cmdf_erase_right[]     = {'K', 0};

/* CMD Variables */
static CMD_MODE         g_cmd_mode;                                     /* CMD Mode */
static CMD_FLAG         g_cmd_flag;                                     /* CMD Status Flags */
static uint8_t          g_cmd[CMD_BUF_SIZE + 1];                        /* CMD String */
#if (CMD_USING_SUGGESTION)
static uint8_t          g_cmd_suggest_keyword[CMD_BUF_SIZE + 1];        /* CMD suggest keyword */
static COMMAND_ITEM     *g_cmd_suggest_tbl;                             /* Command table for suggestion */
static COMMAND_ITEM     *g_cmd_suggest_output;                          /* Suggestion output */
static uint16_t         g_cmd_suggest_tbl_size;                         /* Command table size */
static uint16_t         g_cmd_suggest_last_pos;                         /* Last suggest position */
static uint8_t          g_hint[CMD_BUF_SIZE + 1];                       /* Temporary hint buffer */
#endif
static uint8_t          g_buff[CMD_BUF_SIZE * 2];                       /* Temporary buffer to send the key */
#if (CMD_USING_LAST_CMD)
static uint8_t          g_lastcmd[CMD_LAST_CMD_LENGTH][CMD_BUF_SIZE];   /* CMD Last String */
static uint8_t          g_lastcmd_len;                                  /* Last CMD Length */
static uint8_t          g_lastcmd_pos;                                  /* Current last CMD position */
#endif
static uint16_t         g_pos;                                          /* Current CMD Position */
static uint16_t         g_edit_pos;                                     /* Current CMD Edit Position */
static uint8_t          g_char;                                         /* 1 byte buffer */
static uint8_t          g_ctrl_key;                                     /* 1 byte Control key */
static uint8_t          g_cmd_need_refresh;                             /* flag indicate current CMD display need refresh */

static void s_strcpy(uint8_t *src, uint8_t *des, uint16_t length)
{
    uint16_t i;
    if (length <= 0)
    {
        return;
    }
    for (i = 0; i < length; i++)
        des[i] = src[i];
    /* enclose string */
    des[i] = 0;
}

static void s_stradd(uint8_t *src, uint8_t *des)
{
    uint16_t len = s_strlen(src);
    if (len < 1)
    {
        return;
    }
    s_strcpy(src, des+s_strlen(des), len);
}

static int8_t s_strcmp(uint8_t *src, uint8_t *des)
{
    uint16_t len = s_strlen(src);
    if (len != s_strlen(des))
    {
        return 0;
    }
    while (len > 0)
    {
        len--;
        if (src[len] != des[len])
            return 0;
    }

    return 1;   /* matched */
}

static int8_t s_exist(uint8_t *src, uint8_t *des)
{
    uint16_t len = s_strlen(src);
    if (len > s_strlen(des))
    {
        return 0;
    }
    while (len > 0)
    {
        len--;
        if (src[len] != des[len])
            return 0;
    }

    return 1;   /* matched */
}

static uint16_t s_strlen(uint8_t *str)
{
    uint16_t rlt = 0;
    
    /* Empty string? */
    if (str == 0 || *str == 0)
    {
        return 0;
    }
        
    while (*str++ != 0)
        rlt++;
    
    return rlt;
}

static void s_sendstring(uint8_t *str, uint8_t bwait)
{
    /* string is NULL */
    if (str == 0)
    {
        return;
    }
    
    /* string is empty */
    if (*str == 0)
    {
        return;
    }
    
    if (bwait)
    {
        /* wait for sending complete */
        while (g_cmd_flag.bIsSent == 0)
        {
            CMD_DEVICE_WDTRestart();
        }
    }
    
    g_cmd_flag.bIsSent = 0;
    CMD_DEVICE_SendData(str, s_strlen(str));    

    if (bwait)
    {
        /* wait for sending complete */
        while (g_cmd_flag.bIsSent == 0)
        {
            CMD_DEVICE_WDTRestart();
        }
    }
}

static void s_refresh_line(uint16_t position, uint8_t b_show_hint, uint8_t b_sync)
{
    uint16_t len = 0;
    uint8_t  num_str[10];
    
    g_buff[0] = 0;
    s_stradd((uint8_t *)g_cnst_cmd_restore_cursor, g_buff);
    s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);          /* erase right */
    s_stradd((uint8_t *)g_cnst_cmdf_erase_right, g_buff);
    
    s_stradd(g_cmd, g_buff);
    len += g_pos - position;
    
    #if (CMD_USING_SUGGESTION)
    if (b_show_hint && g_hint[0] != 0)
    {
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);  /* format font blue */
        s_stradd((uint8_t *)"32m", g_buff);
        s_stradd(g_hint, g_buff);
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);  /* reset */
        s_stradd((uint8_t *)"0m", g_buff);
        len += s_strlen(g_hint);
    }
    #endif
    
    if (len > 0)
    {
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);
        sprintf((char *)num_str, "%iD", len);
        s_stradd(num_str, g_buff);
    }
    
    s_sendstring(g_buff, b_sync);
}

#if (CMD_USING_SUGGESTION)
static COMMAND_ITEM* s_search_suggest(uint8_t *keyword)
{
    uint16_t i, j, len, suggest_pos;
    uint16_t found;
    
    if (keyword == NULL)
    {
        return NULL;
    }
    
    len = s_strlen(keyword);
    
    if (g_cmd_suggest_tbl != NULL && g_cmd_suggest_tbl_size != 0)
    {
        for (i = 0; i < g_cmd_suggest_tbl_size; i++)
        {
            /* search by keyword */ 
            found = 1;
            suggest_pos = (i + g_cmd_suggest_last_pos + 1) % g_cmd_suggest_tbl_size;
            for (j = 0; j < len; j++)
            {
                if (keyword[j] != (g_cmd_suggest_tbl + suggest_pos)->cmd_name[j])
                {
                    found = 0;
                }
            }
            
            if (found)
            {
                g_cmd_suggest_last_pos = suggest_pos;
                return (g_cmd_suggest_tbl + suggest_pos);
            }
        }
    }
    
    return NULL;
}

static void s_check_clear_suggestion(void)
{
    if (g_cmd_suggest_output != NULL &&
        s_exist((uint8_t *)g_cmd_suggest_output->cmd_name, g_cmd) == 0)
    {
        g_cmd_suggest_output = NULL;
        g_hint[0] = 0;
    }
}

#endif

static uint16_t s_backspace_1char_and_display(uint16_t position)
{
    g_buff[0] = 0;
    if (position < g_pos)
    {
        s_strcpy(g_cmd + position, g_buff, g_pos - position);
        s_strcpy(g_buff, g_cmd + position - 1, g_pos - position);
    }
    else if (position == g_pos)
    {
        g_cmd[g_pos - 1] = 0;
    }
    
    g_cmd[g_pos] = 0; /* clear last byte */
    
    g_pos--;
    position--;

#if (CMD_USING_SUGGESTION)
    s_check_clear_suggestion();
#endif

#if (CMD_USING_HINT_WHEN_TYPING) && (CMD_USING_SUGGESTION)
    if (g_cmd_suggest_output != NULL && g_hint[0] != 0)
    {
        s_refresh_line(position, 1, 0);
    }
    else
    {
        if (g_buff[0] != 0)
        {
            s_refresh_line(position, 0, 0);
        }
        else
        {
            g_buff[0] = 0;
            s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);          /* erase right */
            s_stradd((uint8_t *)g_cnst_cmdf_erase_right, g_buff);
            s_stradd((uint8_t *)g_cnst_cmd_backchar, g_buff);
            s_sendstring(g_buff, 0);
        }
    }
#else
    if (g_buff[0] != 0)
    {
        s_refresh_line(position, 0, 0);
    }
    else
    {
        g_buff[0] = 0;
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);          /* erase right */
        s_stradd((uint8_t *)g_cnst_cmdf_erase_right, g_buff);
        s_stradd((uint8_t *)g_cnst_cmd_backchar, g_buff);
        s_sendstring(g_buff, 0);
    }
#endif

    return position;
}

static uint16_t s_delete_1char_and_display(uint16_t position)
{
    g_buff[0] = 0;
    if (position + 1 < g_pos)
    {
        s_strcpy(g_cmd + position + 1, g_buff, g_pos - position);
        s_strcpy(g_buff, g_cmd + position, g_pos - position);
    }
    else if (position + 1 == g_pos)
    {
        g_cmd[position] = 0;
    }
    else
    {
        return position;
    }
    
    g_cmd[g_pos] = 0; /* clear last byte */
    g_pos--;

#if (CMD_USING_SUGGESTION)
    s_check_clear_suggestion();
#endif

#if (CMD_USING_HINT_WHEN_TYPING) && (CMD_USING_SUGGESTION)
    if (g_cmd_suggest_output != NULL && g_hint[0] != 0)
    {
        s_refresh_line(position, 1, 0);
    }
    else
    {
        s_refresh_line(position, 0, 0);
    }
#else
    s_refresh_line(position, 0, 0);
#endif

    return position;
}

static uint16_t s_insert_1char_and_display(uint8_t c, uint16_t position)
{
    g_buff[0] = 0;
    if (position < g_pos)
    {
        s_strcpy(g_cmd + position, g_buff, g_pos - position);
        g_cmd[position] = c;
        s_strcpy(g_buff, g_cmd + position + 1, g_pos - position);
    }
    else
    {
        g_cmd[g_pos] = c;
    }
    g_cmd[g_pos + 1] = 0;
    
    g_pos++;
    position++;

    #if (CMD_USING_SUGGESTION)
    s_check_clear_suggestion();
    #endif

#if (CMD_USING_HINT_WHEN_TYPING) && (CMD_USING_SUGGESTION)
    if (g_cmd_suggest_output != NULL && g_hint[0] != 0)
    {
        s_refresh_line(position, 1, 0);
    }
    else
    {
        if (g_buff[0] != 0)
        {
            s_refresh_line(position, 0, 0);
        }
        else
        {
            g_buff[0] = c;
            g_buff[1] = 0;
            s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);          /* erase right */
            s_stradd((uint8_t *)g_cnst_cmdf_erase_right, g_buff);
            s_sendstring(g_buff, 0);
        }
    }
#else
    if (g_buff[0] != 0)
    {
        s_refresh_line(position, 0, 0);
    }
    else
    {
        g_buff[0] = c;
        g_buff[1] = 0;
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);          /* erase right */
        s_stradd((uint8_t *)g_cnst_cmdf_erase_right, g_buff);
        s_sendstring(g_buff, 0);
    }
#endif
    
    return position;
}

#if (CMD_USING_LAST_CMD)

static void s_restorelastcmd(uint8_t position)
{   
    /* format buffer command to send */
    g_buff[0] = 0;
    if (g_pos > 0)
    {   
        s_stradd((uint8_t *)g_cnst_cmd_restore_cursor, g_buff);
        s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);          /* erase right */
        s_stradd((uint8_t *)g_cnst_cmdf_erase_right, g_buff);
    }
    s_stradd(g_lastcmd[position], g_buff);                  /* send the last cmd string */
    s_sendstring(g_buff, 0);

    g_pos = (uint8_t)s_strlen(g_lastcmd[position]);         /* update g_pos by the length of g_lastcmd */
    g_edit_pos = g_pos;                                     /* update current edit position */
    s_strcpy(g_lastcmd[position], g_cmd, g_pos);            /* copy last cmd to cmd */
}
#endif

static void s_getcommand(uint8_t ch)
{
    static uint8_t func_key_flag = 0;
    static uint8_t selected_cmd = 0;

    /* Normal key ? */
    if (func_key_flag == 0)
    {
        #if (CMD_USING_SUGGESTION)
        /* clear suggest keyword & last pos when key in something, not TAB */
        if (ch != CMD_KEY_TAB)
        {
            g_cmd_suggest_keyword[0] = 0;
            g_cmd_suggest_last_pos = 0;
        }
        #endif
        
        switch (ch)
        {
            case CMD_KEY_CTRL_C:    /* CTRL + C */
                g_ctrl_key = ch;
                g_cmd_flag.bIsCtrlKeyReceived = 1;
                
                break;
                
            case CMD_KEY_CTRL_SPACE:    /* CTRL + SPACE */
            
                /* Show current hint of command if found */
                #if (CMD_USING_SUGGESTION)
                if (g_cmd_suggest_output != NULL)
                {
                    s_refresh_line(g_edit_pos, 1, 0);
                }
                #endif
                
                g_ctrl_key = ch;
                g_cmd_flag.bIsCtrlKeyReceived = 1;
                
                break;
                
            case CMD_KEY_ENTER:     /* Enter */
                g_cmd[g_pos] = 0;   /* Terminal current CMD */

                #if (CMD_USING_LAST_CMD)
                /* copy to the last cmd */
                if (g_pos > 0)
                {
                    uint8_t i;
                    
                    /* Find the current CMD on the last CMD list */
                    for (i = 0; i < g_lastcmd_len; i++)
                    {
                        if (s_strcmp(g_cmd, g_lastcmd[i]))
                        {
                            g_lastcmd_pos = i;
                            break;
                        }
                    }
                    
                    /* is not found? */
                    if (i == g_lastcmd_len)
                    {                   
                        /* is last CMD list not full? */
                        if (g_lastcmd_len < CMD_LAST_CMD_LENGTH)
                        {
                            g_lastcmd_len++;
                        }
                        
                        /* Next position */
                        if (g_lastcmd_len > 1)
                        {
                            g_lastcmd_pos++;
                            g_lastcmd_pos %= g_lastcmd_len;
                        }
                        
                        /* Copy to last cmd list */
                        g_lastcmd[g_lastcmd_pos][g_pos] = 0;
                        s_strcpy(g_cmd, g_lastcmd[g_lastcmd_pos], g_pos);
                    }
                    
                    /* Update selected_cmd to select UP, DOWN */
                    selected_cmd = g_lastcmd_pos;                   
                }
                #endif
            
                #if (CMD_USING_BUILT_IN_CMD)
                if (s_strcmp(g_cmd, (uint8_t *)"cls"))
                {
                    s_sendstring((uint8_t *)g_cnst_cmd_cls, 0);         /* Send asynch */
                }
                #endif
                
                #if (CMD_USING_SUGGESTION)
                g_cmd_suggest_output = NULL;
                g_hint[0] = 0;
                #endif
                g_pos = 0;                      /* Start receive next cmd */
                g_edit_pos = 0;                 /* Start receive next cmd */
                g_cmd_flag.bIsCmdReceived = 1;  /* Cmd is received */
                
                break;
                
            case CMD_KEY_BACKSPACE:     /* Backspace */
                if (g_pos > 0 && g_edit_pos > 0)
                {
                    g_edit_pos = s_backspace_1char_and_display(g_edit_pos);
                }
                
                break;
                
            case CMD_KEY_DELETE:        /* Delete */        
                if (g_pos > 0 && g_edit_pos < g_pos)
                {
                    g_edit_pos = s_delete_1char_and_display(g_edit_pos);
                }
                break;
                
            case CMD_KEY_ESC:           /* ESC */
                
                func_key_flag = 1;      /* Function key is pressed */
                break;

            case CMD_KEY_TAB:           /* TAB */
                
                #if (CMD_USING_SUGGESTION)
                if (g_pos != 0 && g_cmd[0] != 0)
                {
                    if (g_cmd_suggest_keyword[0] == 0)
                    {
                        s_stradd(g_cmd, g_cmd_suggest_keyword);
                    }
                    g_cmd_suggest_output = s_search_suggest((uint8_t *)g_cmd_suggest_keyword);
                    g_hint[0] = 0;
                    if (g_cmd_suggest_output != NULL)
                    {
                        g_pos = s_strlen((uint8_t *)g_cmd_suggest_output->cmd_name);
                        g_edit_pos = g_pos;
                        s_strcpy((uint8_t *)g_cmd_suggest_output->cmd_name, g_cmd, g_pos);
                        if (g_cmd_suggest_output->params[0] != 0)
                        {
                            s_stradd((uint8_t *)"  [params: ", g_hint);
                            s_stradd((uint8_t *)g_cmd_suggest_output->params, g_hint);
                            s_stradd((uint8_t *)"]", g_hint);
                        }
                    }
                    else
                    {
                        s_stradd((uint8_t *)"  [keyword not found]", g_hint);
                    }
                    
                    s_refresh_line(g_edit_pos, 1, 0);
                }
                #endif
                
                break;
                
            default:                    /* Others key */
                if (g_pos < CMD_BUF_SIZE-1)
                {
                    //g_cmd[g_pos++] = ch;
                    //g_edit_pos++;
                    //CMD_DEVICE_SendData(&ch, 1);
                    
                    g_edit_pos = s_insert_1char_and_display(ch, g_edit_pos);
                    #if (CMD_USING_SUGGESTION)
                    s_check_clear_suggestion();
                    #endif
                }
                
                break;
        };
    }
    /* Pre-func key */
    else if (func_key_flag == 1)
    {
        switch (ch)
        {
            case '[':                   /* Temp key of func key */
                func_key_flag = 2;
                break;
            
            default:
                func_key_flag = 0;      /* Restart to default state */
                break;
        }
    }
    /* Final-func key */
    else if (func_key_flag == 2)
    {
        uint8_t i;
        
        switch (ch)
        {
            case CMD_KEY_UP:        /* UP key */
                #if (CMD_USING_LAST_CMD)
                /* is have last CMD? */
                if (g_lastcmd_len > 0)
                {
                    s_restorelastcmd(selected_cmd);
                
                    if (selected_cmd == 0)
                    {
                        selected_cmd = g_lastcmd_len - 1;
                    }
                    else
                    {
                        selected_cmd--; /* Previous cmd */
                    }
                }
                #endif

                /* ACK for finished processing func key */
                func_key_flag = 0;      
        
                break;
                
            case CMD_KEY_DOWN:      /* DOWN key */
                #if (CMD_USING_LAST_CMD)
                
                /* is have last CMD? */
                if (g_lastcmd_len > 0)
                { 
                    s_restorelastcmd(selected_cmd);
                    
                    selected_cmd++;     /* Next cmd */
                    selected_cmd %= g_lastcmd_len;
                }
                #endif
                
                /* ACK for finished processing func key */
                func_key_flag = 0;      
        
                break;
    
            case CMD_KEY_LEFT:      /* LEFT key */

                if (g_pos > 0 && g_edit_pos > 0)
                {   
                    s_sendstring((uint8_t *)g_cnst_cmd_cursor_left, 0);
                    g_edit_pos--;
                }

                /* ACK for finished processing func key */
                func_key_flag = 0;      
        
                break;
            case CMD_KEY_RIGHT:     /* RIGHT key */
                
                if (g_pos > 0 && g_edit_pos < g_pos)
                {   
                    s_sendstring((uint8_t *)g_cnst_cmd_cursor_right, 0);
                    g_edit_pos++;
                }
                
                /* ACK for finished processing func key */
                func_key_flag = 0;      

                break;
                
            case CMD_KEY_HOME:      /* HOME key */
            
                s_sendstring((uint8_t *)g_cnst_cmd_restore_cursor, 0);
                g_edit_pos = 0;
                
                func_key_flag = 3;
                break;
                
            case CMD_KEY_END:       /* END key */
            
                g_buff[0] = 0;  
                for (i = g_pos - g_edit_pos; i > 0; i--)
                {
                    s_stradd((uint8_t *)g_cnst_cmd_cursor_right, g_buff);       
                }
                g_edit_pos = g_pos;
                s_sendstring(g_buff, 0);
                
                func_key_flag = 3;
                break;
                
            default:
                break;
        };
    }
    else if (func_key_flag == 3)
    {
        /* Extend processing for END & HOME keys: ignorge the ch */
        func_key_flag = 0;
    }
}

/***********************************************************************************************************************
* Function Name: void CMD_SendEndCallback()
* Description  : CMD Send End Callback Processing
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_SendEndCallback(void)
{
    g_cmd_flag.bIsSent = 1;     /* Send end */
}

/***********************************************************************************************************************
* Function Name: void CMD_ReceiveEndCallback()
* Description  : CMD Receive End Callback Processing
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_ReceiveEndCallback(
#if !(CMD_USING_RECEIVE_ISR_INSIDE)
    uint8_t receive_byte
#endif
)
{   
    #if !(CMD_USING_RECEIVE_ISR_INSIDE)
    g_char = receive_byte;
    #endif
    if (g_cmd_mode == CMD_MODE_COMMAND)
    {
        s_getcommand(g_char);

        /* Get next byte */
        g_cmd_flag.bIsReceived = 0;
        #if (CMD_USING_RECEIVE_ISR_INSIDE)
        CMD_DEVICE_ReceiveData(&g_char, 1);     
        #endif
    }
    else if (g_cmd_mode == CMD_MODE_RECEIVE_DATA)
    {
        g_cmd_flag.bIsReceived = 1;
    }
}

/***********************************************************************************************************************
* Function Name: void CMD_Init();
* Description  : CMD Initialization
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_Init(void)
{
    unsigned char i, j;
    
    /* Init CMD System */
    g_cmd_mode = CMD_MODE_COMMAND;          /* Default is command mode */
    g_cmd_flag.bIsCtrlKeyReceived = 0;      /* No Control key pressed */
    g_cmd_flag.bIsCmdReceived = 0;          /* No CMD is inputed */
    g_cmd_flag.bIsSent = 1;                 /* all bytes are sent */
    g_cmd_flag.bIsReceived = 0;             /* No byte received */
    g_cmd_flag.bIsStarted = 0;              /* CMD is not started */
    g_pos = 0;
    g_edit_pos = 0;
    g_char = 0;
    g_ctrl_key = 0;
    g_cmd_need_refresh = 0;
    #if (CMD_USING_SUGGESTION)
    g_hint[0] = 0;
    g_cmd_suggest_keyword[0] = 0;
    g_cmd_suggest_tbl = NULL;
    g_cmd_suggest_tbl_size = 0;
    g_cmd_suggest_last_pos = 0;
    #endif
    
    #if (CMD_USING_LAST_CMD)
    g_lastcmd_len = 0;
    g_lastcmd_pos = 0;
    #endif

    /* Empty current CMD, last CMD */
    for (i = 0; i < CMD_BUF_SIZE; i++)
    {
        g_cmd[i] = 0;
    }
    #if (CMD_USING_LAST_CMD)
    for (i = 0; i < CMD_LAST_CMD_LENGTH; i++)
    {
        for (j = 0; j < CMD_BUF_SIZE; j++)
        {
            g_lastcmd[i][j] = 0;
        }
    }
    #endif
    
    /* Init Serial Driver by interface mapping */
    CMD_DEVICE_Init();
}

/***********************************************************************************************************************
* Function Name: void CMD_Start()
* Description  : Start Receive Cmd
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_Start(void)
{
    /* stop to ensure the functions */
    CMD_DEVICE_Stop();
    
    /* mask as started */
    g_cmd_flag.bIsStarted = 1;
    
    /* Start Serial Driver by interface mapping */
    CMD_DEVICE_Start();
    
    /* Print CMD Promt */
    //s_sendstring((uint8_t *)g_cnst_cmd_header, 1);
    //s_sendstring((uint8_t *)g_cnst_cmd_newline, 1);
    
    /* Start receive the first char */
    #if (CMD_USING_RECEIVE_ISR_INSIDE)
    CMD_DEVICE_ReceiveData(&g_char, 1);
    #endif
}

/***********************************************************************************************************************
* Function Name: void CMD_Stop()
* Description  : Stop Receive Cmd
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_Stop(void)
{
    /* Stop Serial Driver by interface mapping */
    CMD_DEVICE_Stop();
    
    /* mask as stopped */
    g_cmd_flag.bIsStarted = 0;  
}

/***********************************************************************************************************************
* Function Name: uint8_t CMD_ChangeMode(CMD_MODE mode)
* Description  : Change CMD Mode
* Arguments    : mode: expected mode to change
* Return Value : Status execution
*              : 0 OK
*              : 1 ERROR    Mode is not exist
***********************************************************************************************************************/
uint8_t CMD_ChangeMode(CMD_MODE mode)
{
    if (mode != CMD_MODE_COMMAND &&
        mode != CMD_MODE_RECEIVE_DATA)
    {
        return 1;   /* Error */
    }
    
    g_cmd_mode = mode;
    
    return 0;       /* OK */
}

/***********************************************************************************************************************
* Function Name: uint8_t CMD_GetReceivedByte(uint8_t* byte)
* Description  : Get received byte
* Arguments    : byte: buffer to store received byte
* Return Value : 1/0: Received or not
***********************************************************************************************************************/
uint8_t CMD_GetReceivedByte(uint8_t* byte)
{
    if (g_cmd_flag.bIsReceived)
    {
        *byte = g_char;                     /* Return 1 byte buffer */
        
        /* Get next byte */
        g_cmd_flag.bIsReceived = 0;         /* Clear as finished receive the byte */
        #if (CMD_USING_RECEIVE_ISR_INSIDE)
        CMD_DEVICE_ReceiveData(&g_char, 1);
        #endif
        
        return 1;
    }
    
    return 0;
}

/***********************************************************************************************************************
* Function Name: uint8_t CMD_IsCmdReceived()
* Description  : Is CMD received ?
* Arguments    : None
* Return Value : 1/0: Received or not
***********************************************************************************************************************/
uint8_t CMD_IsCmdReceived(void)
{
    return g_cmd_flag.bIsCmdReceived;
}

/***********************************************************************************************************************
* Function Name: void CMD_AckCmdReceived()
* Description  : Ack CMD received
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_AckCmdReceived(void)
{
    g_cmd_flag.bIsCmdReceived = 0;
    
    /* ACK that CTRL + C is processed */
    CMD_AckCtrlKeyReceived();
    
    g_buff[0] = 0;
    s_stradd((uint8_t *)g_cnst_cmdf_func, g_buff);  /* reset */
    s_stradd((uint8_t *)"0m", g_buff);
    s_stradd((uint8_t *)g_cnst_cmd_newline, g_buff);    
    s_stradd((uint8_t *)g_cnst_cmd_prompt, g_buff);
    s_stradd((uint8_t *)g_cnst_cmd_save_cursor, g_buff);
    s_sendstring(g_buff, 1);    /* Send synch */

    if (g_cmd_need_refresh == 1)
    {
        s_refresh_line(g_edit_pos, 1, 1);
        
        g_cmd_need_refresh = 0;
    }
}

/******************************************************************************
* Function Name: void CMD_AckCmdReceivedNoHeader( )
* Description  : Ack CMD received with no header print out
* Arguments    : None
* Return Value : None
******************************************************************************/
void CMD_AckCmdReceivedNoHeader(void)
{   
    g_cmd_flag.bIsCmdReceived = 0;
    
    /* ACK that CTRL + C is processed */
    CMD_AckCtrlKeyReceived();
}

/***********************************************************************************************************************
* Function Name: uint8_t CMD_IsCtrlKeyReceived()
* Description  : Is CTRL + C received ?
* Arguments    : None
* Return Value : 1/0: Received or not
***********************************************************************************************************************/
uint8_t CMD_IsCtrlKeyReceived(void)
{
    return g_cmd_flag.bIsCtrlKeyReceived;
}

/***********************************************************************************************************************
* Function Name: void CMD_AckCtrlKeyReceived()
* Description  : Ack CTRL+C is processed
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void CMD_AckCtrlKeyReceived(void)
{
    g_cmd_flag.bIsCtrlKeyReceived = 0;
    g_ctrl_key = 0;
}

/******************************************************************************
* Function Name : CMD_RegisterSuggestion
* Interface     : void CMD_RegisterSuggestion(void *table, uint16_t table_length)
* Description   : Register suggestion table
* Arguments     : void * table         : COMMAND_ITEM table
*               : uint16_t table_length: Length of the table
* Return Value  : None
******************************************************************************/
void CMD_RegisterSuggestion(void *table, uint16_t table_length)
{
    #if (CMD_USING_SUGGESTION)
    g_cmd_suggest_tbl = (COMMAND_ITEM *)table;
    g_cmd_suggest_tbl_size = table_length;
    #endif
}

/***********************************************************************************************************************
* Function Name: uint8_t* CMD_Get()
* Description  : Get the current CMD string
* Arguments    : None
* Return Value : Current CMD String
***********************************************************************************************************************/
uint8_t* CMD_Get(void)
{
    return g_cmd;
}

/***********************************************************************************************************************
* Function Name: void CMD_SendString(uint8_t *str)
* Description  : Send string to CMD Prompt
* Arguments    : str: Expected string that wanted to send to CMD Prompt
* Return Value : None
***********************************************************************************************************************/
void CMD_SendString(uint8_t *str)
{
    if (g_pos != 0 && g_cmd[0] != 0)
    {
        g_cmd_need_refresh = 1;
    }
    s_sendstring(str, 1);
}

/***********************************************************************************************************************
* Function Name: void CMD_SendStringA(uint8_t *str, uint8_t size)
* Description  : Send string to CMD Prompt (Asynch)
* Arguments    : str: Expected string that wanted to send to CMD Prompt
*              : size: Size of the string
* Return Value : None
***********************************************************************************************************************/
void CMD_SendStringA(uint8_t *str, uint8_t size)
{
    g_cmd_flag.bIsSent = 1; /* Mark as already send */
    CMD_DEVICE_SendData(str, size);
}

/***********************************************************************************************************************
* Function Name: void CMD_Printf(uint8_t *format, ...)
* Description  : CMD Prompt Printf
* Arguments    : uint8_t *format: Formated string
*              : ... : Variables argument
* Return Value : None
***********************************************************************************************************************/
void CMD_Printf(uint8_t *format, ...)
{
    #define CMD_PRINTF_MAX_STR_LENGTH       200

    uint8_t     buffer[CMD_PRINTF_MAX_STR_LENGTH];  /* max 96 bytes */
    va_list     arg = NULL;
    
    /* Parse the argument list, print to buffer string */
    va_start(arg, format);
    
    /* Format the string */
    vsprintf((char *)buffer, (const char *)format, arg);
    buffer[CMD_PRINTF_MAX_STR_LENGTH-1] = 0;    /* Terminal string to prevent un-expected error */

    if (g_pos != 0 && g_cmd[0] != 0)
    {
        g_cmd_need_refresh = 1;
    }
    
    /* Send string */
    s_sendstring(buffer, 1);

    va_end(arg);
}

#endif /* __DEBUG */

