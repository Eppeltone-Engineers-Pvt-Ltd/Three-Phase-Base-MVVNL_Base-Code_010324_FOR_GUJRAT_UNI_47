/***********************************************************************************************************************
* File Name    : r_cg_sau_user.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for SAU module.
* Creation Date: 
***********************************************************************************************************************/
#include "iodefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/

#ifdef NO_BOOT_CODE
#pragma interrupt INTST0 r_uart0_interrupt_send
#pragma interrupt INTSR0 r_uart0_interrupt_receive
#pragma interrupt INTST2 r_uart2_interrupt_send
#pragma interrupt INTSR2 r_uart2_interrupt_receive
#pragma interrupt INTST3 r_uart3_interrupt_send
#pragma interrupt INTSR3 r_uart3_interrupt_receive
#endif
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
/* Start user code for include. Do not edit comment generated here */
#include "wrp_user_uart.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"
#include "appInterrupts.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_uart0_tx_address;        /* uart0 transmit buffer address */
extern volatile uint16_t  g_uart0_tx_count;           /* uart0 transmit data number */
extern volatile uint8_t * gp_uart0_rx_address;        /* uart0 receive buffer address */
extern volatile uint16_t  g_uart0_rx_count;           /* uart0 receive data number */
extern volatile uint16_t  g_uart0_rx_length;          /* uart0 receive data length */

extern volatile uint8_t * gp_uart2_tx_address;         /* uart2 send buffer address */
extern volatile uint16_t  g_uart2_tx_count;            /* uart2 send data number */
extern volatile uint8_t * gp_uart2_rx_address;         /* uart2 receive buffer address */
extern volatile uint16_t  g_uart2_rx_count;            /* uart2 receive data number */
extern volatile uint16_t  g_uart2_rx_length;           /* uart2 receive data length */

extern volatile uint8_t * gp_uart3_tx_address;         /* uart3 send buffer address */
extern volatile uint16_t  g_uart3_tx_count;            /* uart3 send data number */
extern volatile uint8_t * gp_uart3_rx_address;         /* uart3 receive buffer address */
extern volatile uint16_t  g_uart3_rx_count;            /* uart3 receive data number */
extern volatile uint16_t  g_uart3_rx_length;           /* uart3 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_uart0_interrupt_receive
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_uart0_interrupt_receive(void)
void r_uart0_interrupt_receive(void)
{
    uint8_t rx_data;
    uint8_t err_type;
    
    EI();
    
    err_type = (uint8_t)(SSR01 & 0x0007U);
    SIR01 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart0_callback_error(err_type);
    }
    
    rx_data = RXD0;

    if (g_uart0_rx_length > g_uart0_rx_count)
    {
        *gp_uart0_rx_address = rx_data;
        gp_uart0_rx_address++;
        g_uart0_rx_count++;

        if (g_uart0_rx_length == g_uart0_rx_count)
        {
            r_uart0_callback_receiveend();
        }
    }
    else
    {
        r_uart0_callback_softwareoverrun(rx_data);
    }
}
/***********************************************************************************************************************
* Function Name: r_uart0_interrupt_send
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_uart0_interrupt_send(void)
void r_uart0_interrupt_send(void)
{
    if (g_uart0_tx_count > 0U)
    {
        TXD0 = *gp_uart0_tx_address;
        gp_uart0_tx_address++;
        g_uart0_tx_count--;
    }
    else
    {
        r_uart0_callback_sendend();
    }
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_receiveend
* Description  : This function is a callback function when UART0 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_receiveend(void)
{
    // Start user code. Do not edit comment generated here 
	WRP_UART0_ReceiveEndCallback();
    // End user code. Do not edit comment generated here 
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_softwareoverrun
* Description  : This function is a callback function when UART0 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_sendend
* Description  : This function is a callback function when UART0 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_sendend(void)
{
    // Start user code. Do not edit comment generated here 
    WRP_UART0_SendEndCallback();
    // End user code. Do not edit comment generated here
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_error
* Description  : This function is a callback function when UART0 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
	
    if (err_type & _0004_SAU_FRAM_ERROR != 0)
    {
            
    }
    
    /* Handle parity error */
    if (err_type & _0002_SAU_PARITY_ERROR != 0)
    {
            
    }
    
    /* Handle overrun error */
    if (err_type & _0001_SAU_OVERRUN_ERROR != 0)
    {
            
    }
    
    /* Common */
    WRP_UART0_Stop();
    WRP_UART0_Start();
    
	
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_receive
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_uart2_interrupt_receive(void)
void r_uart2_interrupt_receive(void)
{
    uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR11 & 0x0007U);
    SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart2_callback_error(err_type);
    }
    
    rx_data = RXD2;

    if (g_uart2_rx_length > g_uart2_rx_count)
    {
        *gp_uart2_rx_address = rx_data;
        gp_uart2_rx_address++;
        g_uart2_rx_count++;

        if (g_uart2_rx_length == g_uart2_rx_count)
        {
            r_uart2_callback_receiveend();
        }
    }
    else
    {
        r_uart2_callback_softwareoverrun(rx_data);
    }
}
/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_send
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_uart2_interrupt_send(void)
void r_uart2_interrupt_send(void)
{
    if (g_uart2_tx_count > 0U)
    {
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
    }
    else
    {
        r_uart2_callback_sendend();
    }
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_receiveend
* Description  : This function is a callback function when UART2 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    WRP_UART2_ReceiveEndCallback();
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_softwareoverrun
* Description  : This function is a callback function when UART2 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_sendend
* Description  : This function is a callback function when UART2 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    WRP_UART2_SendEndCallback();
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_error
* Description  : This function is a callback function when UART2 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* Handle framing error */
    if (err_type & _0004_SAU_FRAM_ERROR != 0)
    {
            
    }
    
    /* Handle parity error */
    if (err_type & _0002_SAU_PARITY_ERROR != 0)
    {
            
    }
    
    /* Handle overrun error */
    if (err_type & _0001_SAU_OVERRUN_ERROR != 0)
    {
            
    }
    
    /* Common */
    WRP_UART2_Stop();
    WRP_UART2_Start();
    
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


/***********************************************************************************************************************
* Function Name: r_uart3_interrupt_receive
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_uart3_interrupt_receive(void)
void r_uart3_interrupt_receive(void)
{
    uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR13 & 0x0007U);
    SIR13 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart3_callback_error(err_type);
    }
    
    rx_data = RXD3;

    if (g_uart3_rx_length > g_uart3_rx_count)
    {
        *gp_uart3_rx_address = rx_data;
        gp_uart3_rx_address++;
        g_uart3_rx_count++;

        if (g_uart3_rx_length == g_uart3_rx_count)
        {
            r_uart3_callback_receiveend();
        }
    }
    else
    {
        r_uart3_callback_softwareoverrun(rx_data);
    }
}
/***********************************************************************************************************************
* Function Name: r_uart3_interrupt_send
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
//__interrupt static void r_uart3_interrupt_send(void)
void r_uart3_interrupt_send(void)
{
    if (g_uart3_tx_count > 0U)
    {
        TXD3 = *gp_uart3_tx_address;
        gp_uart3_tx_address++;
        g_uart3_tx_count--;
    }
    else
    {
        r_uart3_callback_sendend();
    }
}
/***********************************************************************************************************************
* Function Name: r_uart3_callback_receiveend
* Description  : This function is a callback function when UART2 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_receiveend(void)
{
    // Start user code. Do not edit comment generated here 
    WRP_UART3_ReceiveEndCallback();
    // End user code. Do not edit comment generated here 
}
/***********************************************************************************************************************
* Function Name: r_uart3_callback_softwareoverrun
* Description  : This function is a callback function when UART2 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart3_callback_sendend
* Description  : This function is a callback function when UART2 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_sendend(void)
{
    // Start user code. Do not edit comment generated here 
    WRP_UART3_SendEndCallback();
    // End user code. Do not edit comment generated here 
}
/***********************************************************************************************************************
* Function Name: r_uart3_callback_error
* Description  : This function is a callback function when UART3 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* Handle framing error */
    if (err_type & _0004_SAU_FRAM_ERROR != 0)
    {
            
    }
    
    /* Handle parity error */
    if (err_type & _0002_SAU_PARITY_ERROR != 0)
    {
            
    }
    
    /* Handle overrun error */
    if (err_type & _0001_SAU_OVERRUN_ERROR != 0)
    {
            
    }
    
    /* Common */

	WRP_UART3_Stop();
    WRP_UART3_Start();
    
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


// dummy routines

void r_uart1_interrupt_receive(void)
{
//  uint8_t rx_data;
//  rx_data = RXD1;
}

void r_uart1_interrupt_send(void)
{
}

