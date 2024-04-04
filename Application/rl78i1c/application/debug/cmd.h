/***********************************************************************************************************************
* File Name    : cmd.h
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : CMD Prompt MW Layer APIs
***********************************************************************************************************************/

#ifndef _CMD_PROMPT_H
#define _CMD_PROMPT_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum tagCMDMode
{
    CMD_MODE_COMMAND = 0,   /* Command */
    CMD_MODE_RECEIVE_DATA   /* Binary receive data */
} CMD_MODE;

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define     CMD_USING_LAST_CMD              1       /* 1: USE, 0: UN-USE */
#define     CMD_USING_BUILT_IN_CMD          1       /* 1: USE, 0: UN-USE */
#define     CMD_USING_RECEIVE_ISR_INSIDE    0       /* 1: USE, 0: UN-USE */
#define     CMD_USING_SUGGESTION            1       /* 1: USE, 0: UN-USE */
#define     CMD_USING_HINT_WHEN_TYPING      0       /* 1: USE, 0: UN-USE */

/***********************************************************************************************************************
Variable Externs
***********************************************************************************************************************/

/***********************************************************************************************************************
Functions Prototypes
***********************************************************************************************************************/
/* Control */
void    CMD_Init(void);                                 /* CMD Init */
void    CMD_Start(void);                                /* Start Receive Cmd */
void    CMD_Stop(void);                                 /* Stop Receive Cmd */
uint8_t CMD_ChangeMode(CMD_MODE mode);              /* Change CMD Mode */
uint8_t CMD_GetReceivedByte(uint8_t* byte);         /* Get received byte */
uint8_t CMD_IsCmdReceived(void);                        /* Is CMD received ? */
void    CMD_AckCmdReceived(void);                       /* Ack CMD received */
uint8_t CMD_IsCtrlKeyReceived(void);                    /* Is CTRL+C key received ? */
void    CMD_AckCtrlKeyReceived(void);                   /* Ack CTRL+C received */
void    CMD_RegisterSuggestion(                     /* Register suggestion table */
    void *table,
    uint16_t table_length
);
/* Get Command */
uint8_t* CMD_Get(void);                                 /* Get the current CMD string */

/* Print out string */
void    CMD_SendString(uint8_t *str);               /* Send string to CMD Prompt */
void    CMD_SendStringA(uint8_t *str, uint8_t size);/* Send string to CMD Prompt (asynch) */
void    CMD_Printf(uint8_t *format, ...);           /* CMD Prompt Printf */

/* Callback */
void    CMD_SendEndCallback(void);
void    CMD_ReceiveEndCallback(
#if !(CMD_USING_RECEIVE_ISR_INSIDE)
    uint8_t receive_byte
#endif
);
void CMD_AckCmdReceivedNoHeader(void);

#endif /* _CMD_PROMPT_H */
