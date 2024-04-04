/***********************************************************************************************************************
* File Name    : r_cg_sau.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for SAU module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_uart0_tx_address;        /* uart0 transmit buffer address */
volatile uint16_t  g_uart0_tx_count;           /* uart0 transmit data number */
volatile uint8_t * gp_uart0_rx_address;        /* uart0 receive buffer address */
volatile uint16_t  g_uart0_rx_count;           /* uart0 receive data number */
volatile uint16_t  g_uart0_rx_length;          /* uart0 receive data length */

volatile uint8_t * gp_uart2_tx_address;        /* uart2 transmit buffer address */
volatile uint16_t  g_uart2_tx_count;           /* uart2 transmit data number */
volatile uint8_t * gp_uart2_rx_address;        /* uart2 receive buffer address */
volatile uint16_t  g_uart2_rx_count;           /* uart2 receive data number */
volatile uint16_t  g_uart2_rx_length;          /* uart2 receive data length */

volatile uint8_t * gp_uart3_tx_address;        /* uart3 transmit buffer address */
volatile uint16_t  g_uart3_tx_count;           /* uart3 transmit data number */
volatile uint8_t * gp_uart3_rx_address;        /* uart3 receive buffer address */
volatile uint16_t  g_uart3_rx_count;           /* uart3 receive data number */
volatile uint16_t  g_uart3_rx_length;          /* uart3 receive data length */


/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
volatile sau_std_baud_rate_t g_uart1_current_baud_rate;
volatile sau_std_baud_rate_t g_uart3_current_baud_rate;
volatile sau_std_baud_rate_t g_uart0_current_baud_rate;
volatile sau_std_baud_rate_t g_uart2_current_baud_rate;

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**  This function initializes the SAU0 module.
**
**  Parameters:
**  None
**
**  Returns:
**  None
**
**-----------------------------------------------------------------------------
*/
void R_SAU0_Create(void)
{
    SAU0EN = 1U;    /* supply SAU0 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS0 = _0005_SAU_CK00_FCLK_5 | _0050_SAU_CK01_FCLK_5;   /* 4800bps */
//    SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 9600bps */
	
//    SPS0 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;   /* 38400bps */
    R_UART0_Create();
}

/***********************************************************************************************************************
* Function Name: R_UART0_Create
* Description  : This function initializes the UART0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_Create(void)
{
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART0 receive and transmit */
    STMK0 = 1U;    /* disable INTST0 interrupt */   
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRMK0 = 1U;    /* disable INTSR0 interrupt */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
    SREMK0 = 1U;   /* disable INTSRE0 interrupt */
    SREIF0 = 0U;   /* clear INTSRE0 interrupt flag */
    /* Set INTST0 low priority */
    STPR10 = 1U;
    STPR00 = 1U;
    /* Set INTSR0 high priority */
    SRPR10 = 0U;
    SRPR00 = 0U;
    
    /* Baud: 4800bps */
    SMR00 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | 
            _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR00 = _9A00_UART0_TRANSMIT_DIVISOR;
    NFEN0 |= _01_SAU_RXD0_FILTER_ON;
    SIR01 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR01 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR01 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | 
            _0007_SAU_LENGTH_8; 
    SDR01 = _9A00_UART0_RECEIVE_DIVISOR;
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    SOL0 |= _0000_SAU_CHANNEL0_NORMAL;      /* output level normal */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART0 output */
    /* Set RxD0 pin */
    PM0 |= 0x40U;
    /* Set TxD0 pin */
    P0 |= 0x80U;
    PM0 &= 0x7FU;
}
/***********************************************************************************************************************
* Function Name: R_UART0_Start
* Description  : This function starts the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_Start(void)
{
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART0 output */
    SS0 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;    /* enable UART0 receive and transmit */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
    SRMK0 = 0U;    /* enable INTSR0 interrupt */
    STMK0 = 0U;    /* enable INTST0 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_UART0_Stop
* Description  : This function stops the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_Stop(void)
{
    STMK0 = 1U;    /* disable INTST0 interrupt */
    SRMK0 = 1U;    /* disable INTSR0 interrupt */
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART0 receive and transmit */
    SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;    /* disable UART0 output */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_UART0_Receive
* Description  : This function receives UART0 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART0_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart0_rx_count = 0U;
        g_uart0_rx_length = rx_num;
        gp_uart0_rx_address = rx_buf;
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_UART0_Send
* Description  : This function sends UART0 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART0_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart0_tx_address = tx_buf;
        g_uart0_tx_count = tx_num;
        STMK0 = 1U;    /* disable INTST0 interrupt */
        TXD0 = *gp_uart0_tx_address;
        gp_uart0_tx_address++;
        g_uart0_tx_count--;
        STMK0 = 0U;    /* enable INTST0 interrupt */
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_SAU1_Create
* Description  : This function initializes the SAU1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU1_Create(void)
{
    SAU1EN = 1U;    /* supply SAU1 clock */
    NOP();
    NOP();
    NOP();
    NOP();
//    SPS1 = _0005_SAU_CK00_FCLK_5 | _0050_SAU_CK01_FCLK_5;   /* 4800bps */
    SPS1 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 9600bps */
//    SPS1 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;   /* 38400bps */
    R_UART2_Create();
    R_UART3_Create();
}

/***********************************************************************************************************************
* Function Name: R_UART2_Create
* Description  : This function initializes the UART2 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Create(void)
{// this is for optical port at 4800 - to be done - at present it is 9600
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    STMK2 = 1U;    /* disable INTST2 interrupt */
    STIF2 = 0U;    /* clear INTST2 interrupt flag */
    SRMK2 = 1U;    /* disable INTSR2 interrupt */
    SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
    SREMK2 = 1U;   /* disable INTSRE2 interrupt */
    SREIF2 = 0U;   /* clear INTSRE2 interrupt flag */
    /* Set INTST2 low priority */
    STPR12 = 1U;
    STPR02 = 1U;
    /* Set INTSR2 high priority */
    SRPR12 = 0U;
    SRPR02 = 0U;
    SMR10 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | 
            _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR10 = _9A00_UART2_TRANSMIT_DIVISOR;
    NFEN0 |= _10_SAU_RXD2_FILTER_ON;
    SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR11 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR11 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | 
            _0007_SAU_LENGTH_8; 
    SDR11 = _9A00_UART2_RECEIVE_DIVISOR;
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;
    SOL1 |= _0000_SAU_CHANNEL0_NORMAL;    /* output level normal */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART2 output */
    /* Set RxD2 pin */
    PM5 |= 0x20U;
    /* Set TxD2 pin */
    P5 |= 0x40U;
    PM5 &= 0xBFU;
}

/***********************************************************************************************************************
* Function Name: R_UART2_Start
* Description  : This function starts the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Start(void)
{
    SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
    SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART2 output */
    SS1 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;    /* enable UART2 receive and transmit */
    STIF2 = 0U;    /* clear INTST2 interrupt flag */
    SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
    SRMK2 = 0U;    /* enable INTSR2 interrupt */
    STMK2 = 0U;    /* enable INTST2 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_UART2_Stop
* Description  : This function stops the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Stop(void)
{
    STMK2 = 1U;    /* disable INTST2 interrupt */
    SRMK2 = 1U;    /* disable INTSR2 interrupt */
    ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;    /* disable UART2 output */
    STIF2 = 0U;    /* clear INTST2 interrupt flag */
    SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_UART2_Receive
* Description  : This function receives UART2 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart2_rx_count = 0U;
        g_uart2_rx_length = rx_num;
        gp_uart2_rx_address = rx_buf;
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_UART2_Send
* Description  : This function sends UART2 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart2_tx_address = tx_buf;
        g_uart2_tx_count = tx_num;
        STMK2 = 1U;    /* disable INTST2 interrupt */
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
        STMK2 = 0U;    /* enable INTST2 interrupt */
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


/***********************************************************************************************************************
* Function Name: R_UART3_Create
* Description  : This function initializes the UART3 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART3_Create(void)
{// we are using TXD3 and RXD3. Original code was using TXD2 and RXD2
    ST1 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    STMK3 = 1U;    /* disable INTST3 interrupt */
    STIF3 = 0U;    /* clear INTST3 interrupt flag */
    SRMK3 = 1U;    /* disable INTSR3 interrupt */
    SRIF3 = 0U;    /* clear INTSR3 interrupt flag */
    SREMK3 = 1U;   /* disable INTSRE3 interrupt */
    SREIF3 = 0U;   /* clear INTSRE3 interrupt flag */
    /* Set INTST3 low priority */
    STPR13 = 0U;//RKC
    STPR03 = 0U;//RKC
    /* Set INTSR3 low priority */
    SRPR13 = 0U;//RKC
    SRPR03 = 0U;//RKC
    SMR12 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR12 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | 
            _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SDR12 = _9A00_UART2_TRANSMIT_DIVISOR;
   //NFEN0 |= _40_SAU_RXD3_FILTER_ON;
    NFEN0 |= 0x40U;
    SIR13 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR13 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR13 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | 
            _0007_SAU_LENGTH_8; 
//    SDR13 = _9A00_UART3_RECEIVE_DIVISOR;
    SDR13 = 0x9A00U;
    SO1 |= _0004_SAU_CH2_DATA_OUTPUT_1;
    SOL1 &=(uint16_t)~(0x0004U);    /* output level normal */
    SOE1 |= 0x0004U;    /* enable UART3 output */
    /* Set RxD3 pin */
    PFSEG5 &= 0xFEU;
    PM8 |= 0x10U;	// P84
    /* Set TxD3 pin */
    PFSEG5 &= 0xFDU;
    P8 |= 0x20U;	// P85
    PM8 &= 0xDFU;
}
/***********************************************************************************************************************
* Function Name: R_UART3_Start
* Description  : This function starts the UART3 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART3_Start(void)
{
    SO1 |= _0004_SAU_CH2_DATA_OUTPUT_1;    /* output level normal */
    SOE1 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART2 output */
    SS1 |= _0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON;    /* enable UART2 receive and transmit */
    STIF3 = 0U;    /* clear INTST2 interrupt flag */
    SRIF3 = 0U;    /* clear INTSR2 interrupt flag */
    SRMK3 = 0U;    /* enable INTSR2 interrupt */
    STMK3 = 0U;    /* enable INTST2 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_UART3_Stop
* Description  : This function stops the UART3 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART3_Stop(void)
{
    STMK3 = 1U;    /* disable INTST2 interrupt */
    SRMK3 = 1U;    /* disable INTSR2 interrupt */
    ST1 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    SOE1 &= (uint16_t)~(0x0004U);    /* disable UART3 output */
    STIF3 = 0U;    /* clear INTST3 interrupt flag */
    SRIF3 = 0U;    /* clear INTSR3 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_UART3_Receive
* Description  : This function receives UART3 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART3_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart3_rx_count = 0U;
        g_uart3_rx_length = rx_num;
        gp_uart3_rx_address = rx_buf;
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_UART3_Send
* Description  : This function sends UART3 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART3_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart3_tx_address = tx_buf;
        g_uart3_tx_count = tx_num;
        STMK3 = 1U;    /* disable INTST3 interrupt */
        TXD3 = *gp_uart3_tx_address;
        gp_uart3_tx_address++;
        g_uart3_tx_count--;
        STMK3 = 0U;    /* enable INTST3 interrupt */
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */


/***********************************************************************************************************************
* Function Name: R_UART_CalculateSPS
* Description  : This function calculate the require SPS value to set from input baud rate
*              : Note, this function assume 2 things:
*              :    1. All channels are using  SCK00 (only 4 LSB)
*              :    2. The FRQSEL3 in user option byte is 0
*              : Limitation: 
*              :    1. Can only get from 300 - 38400bps
*              :    2. Support 3MHz - 24MHz only
* Arguments    : sau_std_baud_rate_t baud_rate - 
*                    input baud rate value
* Return Value : uint16_t: SPS value to set
***********************************************************************************************************************/
static uint16_t R_UART_CalculateSPS(sau_std_baud_rate_t baud_rate)
{
    uint16_t sps_value;

    switch (baud_rate)
	{
		case SAU_STD_BAUD_RATE_300:
		{
			sps_value = 0x000A;
		}
		break;
		case SAU_STD_BAUD_RATE_600:
		{
			sps_value = 0x0009;
		}
		break;
		case SAU_STD_BAUD_RATE_1200:
		{
			sps_value = 0x0008;
		}
		break;
		case SAU_STD_BAUD_RATE_2400:
		{
			sps_value = 0x0007;
		}
		break;
		case SAU_STD_BAUD_RATE_4800:
		{
			sps_value = 0x0006;
		}
		break;
		case SAU_STD_BAUD_RATE_9600:
		{
	//		sps_value = 0x0005;
	sps_value = 0x0004;//24 mHZ
		}
		break;
		case SAU_STD_BAUD_RATE_19200:
		{
			sps_value = 0x0004;
		}
		break;
		case SAU_STD_BAUD_RATE_38400:
		{
			sps_value = 0x0003;
		}
		break;
        /* 57600 and 115200 not supported for this API due to required changes on SDR also */
        case SAU_STD_BAUD_RATE_57600:
        case SAU_STD_BAUD_RATE_115200:
		default:	/* BAUD_RATE_300 */
		{
			sps_value = 0x000A;
		}
		break;
	}

    /* Offset with value of HOCODIV
     * If not in range of 3MHz-24MHz, clear it to zero
     * Minus and check for HOCODIV range later for safe access
    */
    sps_value -= HOCODIV;
    if (HOCODIV > 3)
    {
        sps_value = 0;
    }
    
    return sps_value;
}
/***********************************************************************************************************************
* Function Name: R_UART0_CheckOperating
* Description  : This function check the operating status of UART0
* Arguments    : None
* Return Value : 0: not operating
*              : 1: operating
***********************************************************************************************************************/
uint8_t R_UART0_CheckOperating(void)
{
	if (((~_0001_SAU_CH0_OUTPUT_ENABLE) | SOE0) == 0xffff)
	{
		return 1;	//UART is operating
	}
	return 0;	//UART is not operating
}

/***********************************************************************************************************************
* Function Name: R_UART0_SetBaudRate
* Description  : This function configure the UART0 baudrate only
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_baud_rate_t new_baud_rate -
*                    New baud rate
* Return Value : None
***********************************************************************************************************************/
void R_UART0_SetBaudRate(sau_std_baud_rate_t new_baud_rate)
{
    uint16_t sps_value;
	/* Clear old setting */
	SPS0 &= (~_000F_SAU_CK00_FCLK_15);
	
    sps_value = R_UART_CalculateSPS(new_baud_rate);
    
    /* Store the current baud rate */
    g_uart0_current_baud_rate = new_baud_rate;

    /* UART0 using CK00 --> keep value, no need to shift */
    SPS0 |= sps_value;
    //SPS0 = 0x33;
}

/***********************************************************************************************************************
* Function Name: R_UART0_OnHocoChanged
* Description  : This function configure the UART0 SPS register in case HOCODIV registered changed
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART0_OnHocoChanged(void)
{
    /* Update the SPS based on the last baud rate setting */
    R_UART0_SetBaudRate(g_uart0_current_baud_rate);
}

/***********************************************************************************************************************
* Function Name: R_UART0_SetConfig
* Description  : This function configure the UART0 (except baudrate).
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_length_t datalen -
*                    Byte data length configuration
*                sau_std_parity_t parity -
*                    Parity configuration
*                sau_std_stopbits_t stopbits -
*                    Stopbits configuration
* Return Value : None
***********************************************************************************************************************/
//void R_UART0_SetConfig(sau_std_length_t datalen, sau_std_parity_t parity, sau_std_stopbits_t stopbits)
//{
//    SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
//    SCR01 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
//}
void R_UART0_ConfigHDLCProtocol(void)
{
	/* Reconfigure UART */
	SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SCR01 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
}

/***********************************************************************************************************************
* Function Name: R_UART1_CheckOperating
* Description  : This function check the operating status of UART1
* Arguments    : None
* Return Value : 0: not operating
*              : 1: operating
***********************************************************************************************************************/
uint8_t R_UART1_CheckOperating(void)
{
	if (((~_0004_SAU_CH2_OUTPUT_ENABLE) | SOE0) == 0xffff)
	{
		return 1;	//UART is operating
	}
	return 0;	//UART is not operating
}

/***********************************************************************************************************************
* Function Name: R_UART1_SetBaudRate
* Description  : This function configure the UART1 baudrate only
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_baud_rate_t new_baud_rate -
*                    New baud rate
* Return Value : None
***********************************************************************************************************************/
void R_UART1_SetBaudRate(sau_std_baud_rate_t new_baud_rate)
{
    uint16_t sps_value;
	/* Clear old setting */
	SPS0 &= (~_000F_SAU_CK00_FCLK_15);
	
    sps_value = R_UART_CalculateSPS(new_baud_rate);
    
    /* Store the current baud rate */
    g_uart1_current_baud_rate = new_baud_rate;

    /* UART1 using CK00 --> keep value, no need to shift */
   SPS0 |= sps_value;
   //SPS0 = 0x33;
}

/***********************************************************************************************************************
* Function Name: R_UART1_OnHocoChanged
* Description  : This function configure the UART1 SPS register in case HOCODIV registered changed
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART1_OnHocoChanged(void)
{
    /* Update the SPS based on the last baud rate setting */
    R_UART1_SetBaudRate(g_uart1_current_baud_rate);
}

/***********************************************************************************************************************
* Function Name: R_UART1_SetConfig
* Description  : This function configure the UART0 (except baudrate).
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_length_t datalen -
*                    Byte data length configuration
*                sau_std_parity_t parity -
*                    Parity configuration
*                sau_std_stopbits_t stopbits -
*                    Stopbits configuration
* Return Value : None
***********************************************************************************************************************/
//void R_UART1_SetConfig(sau_std_length_t datalen, sau_std_parity_t parity, sau_std_stopbits_t stopbits)
//{
//    SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
//    SCR03 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
//}
void R_UART1_ConfigHDLCProtocol(void)
{
	/* Reconfigure UART */
	SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SCR03 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
}
/***********************************************************************************************************************
* Function Name: R_UART2_CheckOperating
* Description  : This function check the operating status of UART2
* Arguments    : None
* Return Value : 0: not operating
*              : 1: operating
***********************************************************************************************************************/
uint8_t R_UART2_CheckOperating(void)
{
	if (((~_0001_SAU_CH0_OUTPUT_ENABLE) | SOE1) == 0xffff)
	{
		return 1;	//UART is operating
	}
	return 0;	//UART is not operating
}

/***********************************************************************************************************************
* Function Name: R_UART2_SetBaudRate
* Description  : This function configure the UART2 baudrate only
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_baud_rate_t new_baud_rate -
*                    New baud rate
* Return Value : None
***********************************************************************************************************************/
void R_UART2_SetBaudRate(sau_std_baud_rate_t new_baud_rate)
{
    uint16_t sps_value;
	/* Clear old setting */
	SPS1 &= (~_000F_SAU_CK00_FCLK_15);
	
	
    sps_value = R_UART_CalculateSPS(new_baud_rate);

    /* Store the current baud rate */
    g_uart2_current_baud_rate = new_baud_rate;
    
    /* UART2 using CK00 --> keep value, no need to shift */
    SPS1 |= sps_value;
 
}

/***********************************************************************************************************************
* Function Name: R_UART2_OnHocoChanged
* Description  : This function configure the UART2 SPS register in case HOCODIV registered changed
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_OnHocoChanged(void)
{
    /* Update the SPS based on the last baud rate setting */
    R_UART2_SetBaudRate(g_uart2_current_baud_rate);
}

/***********************************************************************************************************************
* Function Name: R_UART2_SetConfig
* Description  : This function configure the UART2 (except baudrate).
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_length_t datalen -
*                    Byte data length configuration
*                sau_std_parity_t parity -
*                    Parity configuration
*                sau_std_stopbits_t stopbits -
*                    Stopbits configuration
* Return Value : None
***********************************************************************************************************************/
//void R_UART2_SetConfig(sau_std_length_t datalen, sau_std_parity_t parity, sau_std_stopbits_t stopbits)
//{
//    SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
//    SCR11 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
//}
void R_UART2_ConfigHDLCProtocol(void)
{
	/* Reconfigure UART */
	SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SCR11 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
}

/* End user code. Do not edit comment generated here */
/***********************************************************************************************************************
* Function Name: R_UART3_CheckOperating
* Description  : This function check the operating status of UART2
* Arguments    : None
* Return Value : 0: not operating
*              : 1: operating
***********************************************************************************************************************/
uint8_t R_UART3_CheckOperating(void)
{
	if (((~_0004_SAU_CH2_OUTPUT_ENABLE) | SOE1) == 0xffff)
	{
		return 1;	//UART is operating
	}
	return 0;	//UART is not operating
}

/***********************************************************************************************************************
* Function Name: R_UART3_SetBaudRate
* Description  : This function configure the UART2 baudrate only
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_baud_rate_t new_baud_rate -
*                    New baud rate
* Return Value : None
***********************************************************************************************************************/
void R_UART3_SetBaudRate(sau_std_baud_rate_t new_baud_rate)
{
    uint16_t sps_value;
	/* Clear old setting */
	SPS1 &= (~_000F_SAU_CK00_FCLK_15);
	
    sps_value = R_UART_CalculateSPS(new_baud_rate);

    /* Store the current baud rate */
    g_uart3_current_baud_rate = new_baud_rate;
    
    /* UART3 using CK00 --> keep value, no need to shift */
    SPS1 |= sps_value;
}

/***********************************************************************************************************************
* Function Name: R_UART3_OnHocoChanged
* Description  : This function configure the UART3 SPS register in case HOCODIV registered changed
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART3_OnHocoChanged(void)
{
    /* Update the SPS based on the last baud rate setting */
    R_UART3_SetBaudRate(g_uart3_current_baud_rate);
}

/***********************************************************************************************************************
* Function Name: R_UART3_SetConfig
* Description  : This function configure the UART2 (except baudrate).
*              : NOTE: Use this function when this peripheral is stop only
* Arguments    : sau_std_length_t datalen -
*                    Byte data length configuration
*                sau_std_parity_t parity -
*                    Parity configuration
*                sau_std_stopbits_t stopbits -
*                    Stopbits configuration
* Return Value : None
***********************************************************************************************************************/
void R_UART3_SetConfig(sau_std_length_t datalen, sau_std_parity_t parity, sau_std_stopbits_t stopbits)
{
    SCR12 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
    SCR13 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0080_SAU_LSB | (datalen | parity | stopbits);
}

void R_UART3_ConfigHDLCProtocol(void)
{
	/* Reconfigure UART */
	SCR12 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SCR13 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE |
			_0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
}

/* End user code. Do not edit comment generated here */




