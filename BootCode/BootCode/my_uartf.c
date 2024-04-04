#ifndef __BIT8
typedef struct {
	unsigned int b0:1;
	unsigned int b1:1;
	unsigned int b2:1;
	unsigned int b3:1;
	unsigned int b4:1;
	unsigned int b5:1;
	unsigned int b6:1;
	unsigned int b7:1;
} __Bits8;
#define __BIT8(name, bit) (((volatile __near __Bits8*)&name)->b##bit)
#endif

#include "my_macrodriver.h"
#include "my_cgc.h"
#include "my_port.h"
#include "my_uartf.h"
#include "my_userdefine.h"

extern _Bool rx_flag;
extern _Bool tx_flag;
extern unsigned char rec_val;
extern unsigned int i;

#define _0000_SAU_CK00_FCLK_0            (0x0000U) /* ck00 - fCLK */
#define _0001_SAU_CK00_FCLK_1            (0x0001U) /* ck00 - fCLK/2^1 */
#define _0002_SAU_CK00_FCLK_2            (0x0002U) /* ck00 - fCLK/2^2 */
#define _0003_SAU_CK00_FCLK_3            (0x0003U) /* ck00 - fCLK/2^3 */
#define _0004_SAU_CK00_FCLK_4            (0x0004U) /* ck00 - fCLK/2^4 */
#define _0005_SAU_CK00_FCLK_5            (0x0005U) /* ck00 - fCLK/2^5 */
#define _0006_SAU_CK00_FCLK_6            (0x0006U) /* ck00 - fCLK/2^6 */
#define _0007_SAU_CK00_FCLK_7            (0x0007U) /* ck00 - fCLK/2^7 */
#define _0008_SAU_CK00_FCLK_8            (0x0008U) /* ck00 - fCLK/2^8 */
#define _0009_SAU_CK00_FCLK_9            (0x0009U) /* ck00 - fCLK/2^9 */
#define _000A_SAU_CK00_FCLK_10           (0x000AU) /* ck00 - fCLK/2^10 */
#define _000B_SAU_CK00_FCLK_11           (0x000BU) /* ck00 - fCLK/2^11 */
#define _000C_SAU_CK00_FCLK_12           (0x000CU) /* ck00 - fCLK/2^12 */
#define _000D_SAU_CK00_FCLK_13           (0x000DU) /* ck00 - fCLK/2^13 */
#define _000E_SAU_CK00_FCLK_14           (0x000EU) /* ck00 - fCLK/2^14 */
#define _000F_SAU_CK00_FCLK_15           (0x000FU) /* ck00 - fCLK/2^15 */
/* Section of operation clock (CKm1) (PRSm10,PRSm11,PRSm12,PRSm13,) */
#define _0000_SAU_CK01_FCLK_0            (0x0000U) /* ck01 - fCLK */
#define _0010_SAU_CK01_FCLK_1            (0x0010U) /* ck01 - fCLK/2^1 */
#define _0020_SAU_CK01_FCLK_2            (0x0020U) /* ck01 - fCLK/2^2 */
#define _0030_SAU_CK01_FCLK_3            (0x0030U) /* ck01 - fCLK/2^3 */
#define _0040_SAU_CK01_FCLK_4            (0x0040U) /* ck01 - fCLK/2^4 */
#define _0050_SAU_CK01_FCLK_5            (0x0050U) /* ck01 - fCLK/2^5 */
#define _0060_SAU_CK01_FCLK_6            (0x0060U) /* ck01 - fCLK/2^6 */
#define _0070_SAU_CK01_FCLK_7            (0x0070U) /* ck01 - fCLK/2^7 */
#define _0080_SAU_CK01_FCLK_8            (0x0080U) /* ck01 - fCLK/2^8 */
#define _0090_SAU_CK01_FCLK_9            (0x0090U) /* ck01 - fCLK/2^9 */
#define _00A0_SAU_CK01_FCLK_10           (0x00A0U) /* ck01 - fCLK/2^10 */
#define _00B0_SAU_CK01_FCLK_11           (0x00B0U) /* ck01 - fCLK/2^11 */
#define _00C0_SAU_CK01_FCLK_12           (0x00C0U) /* ck01 - fCLK/2^12 */
#define _00D0_SAU_CK01_FCLK_13           (0x00D0U) /* ck01 - fCLK/2^13 */
#define _00E0_SAU_CK01_FCLK_14           (0x00E0U) /* ck01 - fCLK/2^14 */
#define _00F0_SAU_CK01_FCLK_15           (0x00F0U) /* ck01 - fCLK/2^15 */

#define _0020_SAU_SMRMN_INITIALVALUE     (0x0020U)
/* Selection of macro clock (MCK) of channel n (CKSmn) */
#define _0000_SAU_CLOCK_SELECT_CK00      (0x0000U) /* operation clock CK0 set by PRS register */ 
#define _8000_SAU_CLOCK_SELECT_CK01      (0x8000U) /* operation clock CK1 set by PRS register */
/* Selection of transfer clock (TCLK) of channel n (CCSmn) */
#define _0000_SAU_CLOCK_MODE_CKS         (0x0000U) /* divided operation clock MCK specified by CKSmn bit */  
#define _4000_SAU_CLOCK_MODE_TI0N        (0x4000U) /* clock input from SCK pin (slave transfer in CSI mode) */
/* Selection of start trigger source (STSmn) */
#define _0000_SAU_TRIGGER_SOFTWARE       (0x0000U) /* only software trigger is valid */
#define _0100_SAU_TRIGGER_RXD            (0x0100U) /* valid edge of RXD pin */
/* Controls inversion of level of receive data of channel n in UART mode (SISmn0) */
#define _0000_SAU_EDGE_FALL              (0x0000U) /* falling edge is detected as the start bit */
#define _0040_SAU_EDGE_RISING            (0x0040U) /* rising edge is detected as the start bit */
/* Setting of operation mode of channel n (MDmn2, MDmn1) */
#define _0000_SAU_MODE_CSI               (0x0000U) /* CSI mode */
#define _0002_SAU_MODE_UART              (0x0002U) /* UART mode */
#define _0004_SAU_MODE_IIC               (0x0004U) /* simplified IIC mode */
/* Selection of interrupt source of channel n (MDmn0) */
#define _0000_SAU_TRANSFER_END           (0x0000U) /* transfer end interrupt */
#define _0001_SAU_BUFFER_EMPTY           (0x0001U) /* buffer empty interrupt */

#define _0000_SAU_NOT_COMMUNICATION      (0x0000U) /* does not start communication */
#define _4000_SAU_RECEPTION              (0x4000U) /* reception only */
#define _8000_SAU_TRANSMISSION           (0x8000U) /* transmission only */
#define _C000_SAU_RECEPTION_TRANSMISSION (0xC000U) /* reception and transmission */
/* Selection of data and clock phase in CSI mode (DAPmn, CKPmn) */
#define _0000_SAU_TIMING_1               (0x0000U) /* type 1 */
#define _1000_SAU_TIMING_2               (0x1000U) /* type 2 */
#define _2000_SAU_TIMING_3               (0x2000U) /* type 3 */
#define _3000_SAU_TIMING_4               (0x3000U) /* type 4 */
/* Selection of masking of error interrupt signal (EOCmn) */
#define _0000_SAU_INTSRE_MASK            (0x0000U) /* masks error interrupt INTSREx */
#define _0400_SAU_INTSRE_ENABLE          (0x0400U) /* enables generation of error interrupt INTSREx */
/* Setting of parity bit in UART mode (PTCmn1 - PTCmn0) */
#define _0000_SAU_PARITY_NONE            (0x0000U) /* none parity */
#define _0100_SAU_PARITY_ZERO            (0x0100U) /* zero parity */
#define _0200_SAU_PARITY_EVEN            (0x0200U) /* even parity */
#define _0300_SAU_PARITY_ODD             (0x0300U) /* odd parity */
/* Selection of data transfer sequence in CSI and UART modes (DIRmn) */
#define _0000_SAU_MSB                    (0x0000U) /* MSB */
#define _0080_SAU_LSB                    (0x0080U) /* LSB */
/* Setting of stop bit in UART mode (SLCmn1, SLCmn0) */
#define _0000_SAU_STOP_NONE              (0x0000U) /* none stop bit */
#define _0010_SAU_STOP_1                 (0x0010U) /* 1 stop bit */
#define _0020_SAU_STOP_2                 (0x0020U) /* 2 stop bits */
/* Setting of data length in CSI and UART modes (DLSmn2 - DLSmn0) */
#define _0005_SAU_LENGTH_9               (0x0005U) /* 9-bit data length */
#define _0006_SAU_LENGTH_7               (0x0006U) /* 7-bit data length */
#define _0007_SAU_LENGTH_8               (0x0007U) /* 8-bit data length */

#define _0004_SAU_SIRMN_FECTMN                  (0x0004U) /* clears the FEFmn bit of the SSRmn register to 0 */
/* Clear trigger of parity error flag of channel n (PECTmn) */
#define _0002_SAU_SIRMN_PECTMN                  (0x0002U) /* clears the PEFmn bit of the SSRmn register to 0 */
/* Clear trigger of overrun error flag of channel n (OVCTmn) */
#define _0001_SAU_SIRMN_OVCTMN                  (0x0001U) /* clears the OVFmn bit of the SSRmn register to 0 */

#define _0000_SAU_CHANNEL0_NORMAL        (0x0000U) /* normal bit level */
#define _0001_SAU_CHANNEL0_INVERTED      (0x0001U) /* inverted bit level */
/* Selects inversion of the level of the transmit data of channel n in UART mode (SOLm2) */
#define _0000_SAU_CHANNEL2_NORMAL        (0x0000U) /* normal bit level */
#define _0004_SAU_CHANNEL2_INVERTED      (0x0004U) /* inverted bit level */

#define _00_SAU_RXD0_FILTER_OFF          (0x00U) /* noise filter off */
#define _01_SAU_RXD0_FILTER_ON           (0x01U) /* noise filter on */
/* Use of noise filter of RXD1 pin (SNFEN10) */
#define _00_SAU_RXD1_FILTER_OFF          (0x00U) /* noise filter off */
#define _04_SAU_RXD1_FILTER_ON           (0x04U) /* noise filter on */
/* Use of noise filter of RXD2 pin (SNFEN20) */
#define _00_SAU_RXD2_FILTER_OFF          (0x00U) /* noise filter off */
#define _10_SAU_RXD2_FILTER_ON           (0x10U) /* noise filter on */

#define _0040_SAU_UNDER_EXECUTE          (0x0040U) /* communication is under execution */
/* Buffer register status indication flag of channel n (BFFmn) */
#define _0020_SAU_VALID_STORED           (0x0020U) /* valid data is stored in the SDRmn register */
/* Framing error detection flag of channel n (FEFmn) */
#define _0004_SAU_FRAM_ERROR             (0x0004U) /* a framing error occurs during UART reception */
/* Parity error detection flag of channel n (PEFmn) */
#define _0002_SAU_PARITY_ERROR           (0x0002U) /* a parity error occurs or ACK is not detected */
/* Overrun error detection flag of channel n (OVFmn) */
#define _0001_SAU_OVERRUN_ERROR          (0x0001U) /* an overrun error occurs */

#define _0000_SAU_CH0_START_TRG_OFF      (0x0000U) /* no trigger operation */
#define _0001_SAU_CH0_START_TRG_ON       (0x0001U) /* sets SEm0 to 1 and enters the communication wait status */
/* Operation start trigger of channel 1 (SSm1) */
#define _0000_SAU_CH1_START_TRG_OFF      (0x0000U) /* no trigger operation */
#define _0002_SAU_CH1_START_TRG_ON       (0x0002U) /* sets SEm1 to 1 and enters the communication wait status */
/* Operation start trigger of channel 2 (SSm2) */
#define _0000_SAU_CH2_START_TRG_OFF      (0x0000U) /* no trigger operation */
#define _0004_SAU_CH2_START_TRG_ON       (0x0004U) /* sets SEm2 to 2 and enters the communication wait status */
/* Operation start trigger of channel 3 (SSm3) */
#define _0000_SAU_CH3_START_TRG_OFF      (0x0000U) /* no trigger operation */
#define _0008_SAU_CH3_START_TRG_ON       (0x0008U) /* sets SEm3 to 3 and enters the communication wait status */

#define _0000_SAU_CH0_STOP_TRG_OFF       (0x0000U) /* no trigger operation */
#define _0001_SAU_CH0_STOP_TRG_ON        (0x0001U) /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 1 (STm1) */
#define _0000_SAU_CH1_STOP_TRG_OFF       (0x0000U) /* no trigger operation */
#define _0002_SAU_CH1_STOP_TRG_ON        (0x0002U) /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 2 (STm2) */
#define _0000_SAU_CH2_STOP_TRG_OFF       (0x0000U) /* no trigger operation */
#define _0004_SAU_CH2_STOP_TRG_ON        (0x0004U) /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 3 (STm3) */
#define _0000_SAU_CH3_STOP_TRG_OFF       (0x0000U) /* no trigger operation */
#define _0008_SAU_CH3_STOP_TRG_ON        (0x0008U) /* operation is stopped (stop trigger is generated) */

#define _0000_SAU_CH0_STATUS_DISABLE     (0x0000U) /* operation stops */
#define _0001_SAU_CH0_STATUS_ENABLE      (0x0001U) /* operation is enabled */
/* Indication of operation enable/stop status of channel 1 (SEm1) */
#define _0000_SAU_CH1_STATUS_DISABLE     (0x0000U) /* operation stops */
#define _0002_SAU_CH1_STATUS_ENABLE      (0x0002U) /* operation is enabled */
/* Indication of operation enable/stop status of channel 2 (SEm2) */
#define _0000_SAU_CH2_STATUS_DISABLE     (0x0000U) /* operation stops */
#define _0004_SAU_CH2_STATUS_ENABLE      (0x0004U) /* operation is enabled */
/* Indication of operation enable/stop status of channel 3 (SEm3) */
#define _0000_SAU_CH3_STATUS_DISABLE     (0x0000U) /* operation stops */
#define _0008_SAU_CH3_STATUS_ENABLE      (0x0008U) /* operation is enabled */

#define _0001_SAU_SIRMN_OVCTMN           (0x0001U)
/* Clear trigger of parity error flag of channel n (PECTmn) */
#define _0002_SAU_SIRMN_PECTMN           (0x0002U)
/* Clear trigger of framing error of channel n (FECTmn) */
#define _0004_SAU_SIRMN_FECTMN           (0x0004U)

#define _0000_SAU_CH0_OUTPUT_DISABLE     (0x0000U) /* stops output by serial communication operation */
#define _0001_SAU_CH0_OUTPUT_ENABLE      (0x0001U) /* enables output by serial communication operation */
/* Serial output enable/disable of channel 2 (SOEm2) */
#define _0000_SAU_CH2_OUTPUT_DISABLE     (0x0000U) /* stops output by serial communication operation */
#define _0004_SAU_CH2_OUTPUT_ENABLE      (0x0004U) /* enables output by serial communication operation */

#define _0000_SAU_CH0_DATA_OUTPUT_0      (0x0000U) /* serial data output value is "0" */
#define _0001_SAU_CH0_DATA_OUTPUT_1      (0x0001U) /* serial data output value is "1" */
/* Serial data output of channel 2 (SOm2) */
#define _0000_SAU_CH2_DATA_OUTPUT_0      (0x0000U) /* serial data output value is "0" */
#define _0004_SAU_CH2_DATA_OUTPUT_1      (0x0004U) /* serial data output value is "1" */
/* Serial clock output of channel 0 (CKOm0) */
#define _0000_SAU_CH0_CLOCK_OUTPUT_0     (0x0000U) /* serial clock output value is "0" */
#define _0100_SAU_CH0_CLOCK_OUTPUT_1     (0x0100U) /* serial clock output value is "1" */
/* Serial clock output of channel 2 (CKOm2) */
#define _0000_SAU_CH2_CLOCK_OUTPUT_0     (0x0000U) /* serial clock output value is "0" */
#define _0400_SAU_CH2_CLOCK_OUTPUT_1     (0x0400U) /* serial clock output value is "1" */

#define _0000_SAU_CH0_SNOOZE_OFF         (0x0000U) /* stop the SNOOZE mode function */
#define _0001_SAU_CH0_SNOOZE_ON          (0x0001U) /* use the SNOOZE mode function */
/* Selection of whether to enable or stop the generation of transfer end interrupts (SSECm) */
#define _0000_SAU_INTSRE0_ENABLE         (0x0000U) /* enable the generation of error interrupts (INTSRE0/INTSRE2) */
#define _0002_SAU_INTSRE0_DISABLE        (0x0002U) /* stop the generation of error interrupts (INTSRE0/INTSRE2) */

/* SAU used flag */
#define _00_SAU_IIC_MASTER_FLAG_CLEAR    (0x00U)
#define _01_SAU_IIC_SEND_FLAG            (0x01U)
#define _02_SAU_IIC_RECEIVE_FLAG         (0x02U)
#define _04_SAU_IIC_SENDED_ADDRESS_FLAG  (0x04U)

#define _00_IRDA_RXD_POLARITY_NORMAL     (0x00U) /* IrRxD input is used as received data as is. */
#define _04_IRDA_RXD_POLARITY_INVERTED   (0x04U) /* IrRxD input is used as received data after the polarity is inverted. */
/*IrDA IrTxD data polarity switching(IRTXINV) */
#define _00_IRDA_TXD_POLARITY_NORMAL     (0x00U) /* data to be transmitted is output to IrTxD as is. */
#define _08_IRDA_TXD_POLARITY_INVERTED   (0x08U) /* data to be transmitted is output to IrTxD after the polarity is inverted. */
/* IrDA clock selection(IRCKS0 - IRCKS2) */
#define _00_IRDA_CLK_fCLK0               (0x00U) /* B x 3 / 16 */
#define _10_IRDA_CLK_fCLK1               (0x10U) /* fCLK/2 */
#define _20_IRDA_CLK_fCLK2               (0x20U) /* fCLK/4 */
#define _30_IRDA_CLK_fCLK3               (0x30U) /* fCLK/8 */
#define _40_IRDA_CLK_fCLK4               (0x40U) /* fCLK/16 */
#define _50_IRDA_CLK_fCLK5               (0x50U) /* fCLK/32 */
#define _60_IRDA_CLK_fCLK6               (0x60U) /* fCLK/64 */
/* IrDA data communication enable(IRE) */
#define _80_IRDA_ENABLE                  (0x80U) /* serial I/O pins are used for IrDA data communication. */


#define _1600_CSI00_DIVISOR              (0x1600U)

#define _9A00_UART0_RECEIVE_DIVISOR      (0x9A00U)
#define _9A00_UART0_TRANSMIT_DIVISOR     (0x9A00U)

#define _9A00_UART2_RECEIVE_DIVISOR      (0x9A00U)
#define _9A00_UART2_TRANSMIT_DIVISOR     (0x9A00U)

#define _9A00_UART3_RECEIVE_DIVISOR      (0x9A00U)
#define _9A00_UART3_TRANSMIT_DIVISOR     (0x9A00U)

#define _4C00_UART2_TRANSMIT_DIVISOR     (0x4C00U)
#define _4C00_UART2_RECEIVE_DIVISOR      (0x4C00U)

#define _CE00_UART_DIVISOR                      (0xCE00U)
#define _9A00_UART_DIVISOR                      (0x9A00U)
#define _4C00_UART_DIVISOR                      (0x4C00U)


void R_UART3B_Create(void);
void R_UART3B_Start(void);
void R_UART3B_Stop(void);


void R_SAU1B_Create(void)
{
    SAU1EN = 1U;    /* supply SAU1 clock */
    NOP();
    NOP();
    NOP();
    NOP();
//    SPS1 = _0005_SAU_CK00_FCLK_5 | _0050_SAU_CK01_FCLK_5;   /* 4800bps */
    SPS1 = 0x0022;//_0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 9600bps */
//    SPS1 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;   /* 38400bps */
//    R_UART2_Create();
    R_UART3B_Create();
}

void R_UART3B_Create(void)
{

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
    P8|= 0x20U;	// P85
    PM8 &= 0xDFU;
    R_UART3B_Start();
}

void R_UART3B_Start(void)
{
    SO1 |= _0004_SAU_CH2_DATA_OUTPUT_1;    /* output level normal */
    SOE1 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART2 output */
    SS1 |= _0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON;    /* enable UART2 receive and transmit */
    STIF3 = 0U;    /* clear INTST2 interrupt flag */
    SRIF3 = 0U;    /* clear INTSR2 interrupt flag */
    SRMK3 = 1U;    /* enable INTSR2 interrupt */
    STMK3 = 1U;    /* enable INTST2 interrupt */
}

void R_UART3B_Stop(void)
{
    STMK3 = 1U;    /* disable INTST2 interrupt */
    SRMK3 = 1U;    /* disable INTSR2 interrupt */
    ST1 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART2 receive and transmit */
    SOE1 &= (uint16_t)~(0x0004U);    /* disable UART3 output */
    STIF3 = 0U;    /* clear INTST3 interrupt flag */
    SRIF3 = 0U;    /* clear INTSR3 interrupt flag */
}

/*
void R_UART1_Create_B(void)
{
    ST1 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;
    STMK2 = 1U;
    STIF2 = 0U;
    SRMK2 = 1U;
    SRIF2 = 0U;
    SREMK2 = 1U;
    SREIF2 = 0U;

    SRPR12 = 1U;
    SRPR02 = 1U;

    STPR12 = 1U;
    STPR02 = 1U;
    SMR10 = _0020_SMR10_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR10 = _0004_SCR10_DEFAULT_VALUE | _8000_SAU_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR10 = 0x9a00;
    NFEN0 |= _10_SAU_RXD2_FILTER_ON;
    SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;
    SMR11 = 0x0020 | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR11 = 0x0004 | _4000_SAU_RECEPTION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR11 = 0x9a00;
    SO1 |= _0001_SAUm_CH0_DATA_OUTPUT_1;
    SOL1 &= (uint16_t)~_0001_SAUm_CHANNEL0_INVERTED;
    SOE1 |= _0001_SAUm_CH0_OUTPUT_ENABLE;
    //Set RxD2 pin
//    PFSEG4 &= 0xDFU;
//    PM5 |= 0x20U;
//    Set TxD2 pin
//    PFSEG4 &= 0xBFU;
//    P5 |= 0x40U;
//    PM5 &= 0xBFU;
//    R_UART1_Start_B();
 }
*/

/*
void R_UART1_Start_B(void)
{  
    SO1 |= _0001_SAUm_CH0_DATA_OUTPUT_1;
    SOE1 |= _0001_SAUm_CH0_OUTPUT_ENABLE;
    SS1 |= _0002_SAUm_CH1_START_TRG_ON | _0001_SAUm_CH0_START_TRG_ON;
    STIF2 = 0U;
    SRIF2 = 0U;
    STMK2 = 1U;
    SRMK2 = 1U;
 }
*/
/*
void R_UART1_Stop_B(void)
{    
     STMK2 = 1U;
    SRMK2 = 1U; 
    ST1 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;
    SOE1 &= (uint16_t)~_0001_SAUm_CH0_OUTPUT_ENABLE;
    STIF2 = 0U; 
    SRIF2 = 0U; 
}
*/


/*
__interrupt static void my_uartf0_interrupt_send(void)
{
    tx_flag=1;
}
__interrupt static void my_uartf0_interrupt_error(void)
{
    
}
__interrupt static void my_uartf0_interrupt_receive(void)
{
	rx_flag = 1;
	rec_val = UF0RXB;		//recieve the value
}
*/