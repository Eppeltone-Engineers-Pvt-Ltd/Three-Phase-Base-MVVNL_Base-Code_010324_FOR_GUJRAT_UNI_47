/***********************************************************************************************************************
* File Name    : r_cg_dtc.h
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for DTC module.
* Creation Date: 
***********************************************************************************************************************/
#ifndef DTC_H
#define DTC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/* 
    Peripheral Enable Register 1 (PER1) 
*/
/* Control of D/A converter input clock (DTCEN) */
#define _00_DTC_CLOCK_STOP                  (0x00U) /* stop supply of input clock */
#define _08_DTC_CLOCK_SUPPLY                (0x08U) /* supplie input clock */

/*
    DTC Control Register j (DTCCRj) (j = 0 to 23)
*/
/* Data size selection (SZ) */
#define _00_DTC_DATA_SIZE_8BITS             (0x00U) /* 8 bits */
#define _40_DTC_DATA_SIZE_16BITS            (0x40U) /* 16 bits */
/* Repeat mode interrupt enable bit (RPTINT) */
#define _00_DTC_REPEAT_INT_DISABLE          (0x00U) /* disable interrupt generation */
#define _20_DTC_REPEAT_INT_ENABLE           (0x20U) /* enable interrupt generation */
/* Chain transfer enable bit (CHNE) */
#define _00_DTC_CHAIN_TRANSFER_DISABLE      (0x00U) /* disable chain transfers */
#define _10_DTC_CHAIN_TRANSFER_ENABLE       (0x10U) /* enable chain transfers */
/* Destination address control bit (DAMOD) */
#define _00_DTC_DEST_ADDR_FIXED             (0x00U) /* destination address fixed */
#define _08_DTC_DEST_ADDR_INCREMENTED       (0x08U) /* destination address incremented */
/* Source address control bit (SAMOD) */
#define _00_DTC_SOURCE_ADDR_FIXED           (0x00U) /* source address fixed */
#define _04_DTC_SOURCE_ADDR_INCREMENTED     (0x04U) /* source address incremented */
/* Repeat area select bit (RPTSEL) */
#define _00_DTC_REPEAT_AREA_DEST            (0x00U) /* transfer destination is the repeat area */
#define _02_DTC_REPEAT_AREA_SOURCE          (0x02U) /* transfer source is the repeat area */
/* Transfer mode select bit (MODE) */
#define _00_DTC_TRANSFER_MODE_NORMAL        (0x00U) /* normal mode */
#define _01_DTC_TRANSFER_MODE_REPEAT        (0x01U) /* repeat mode */

/*  
    DTC Activation Enable Register 0 (DTCEN0)
*/
/* DTC activation enable bit (DTCEN07 - DTCEN00) */
#define _00_DTC_INTP0_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP0) */
#define _40_DTC_INTP0_ACTIVATION_ENABLE     (0x40U) /* enable activation (INTP0) */
#define _00_DTC_INTP1_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP1) */
#define _20_DTC_INTP1_ACTIVATION_ENABLE     (0x20U) /* enable activation (INTP1) */
#define _00_DTC_INTP2_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP2) */
#define _10_DTC_INTP2_ACTIVATION_ENABLE     (0x10U) /* enable activation (INTP2) */
#define _00_DTC_INTP3_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP3) */
#define _08_DTC_INTP3_ACTIVATION_ENABLE     (0x08U) /* enable activation (INTP3) */
#define _00_DTC_INTP4_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP4) */
#define _04_DTC_INTP4_ACTIVATION_ENABLE     (0x04U) /* enable activation (INTP4) */
#define _00_DTC_INTP5_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP5) */
#define _02_DTC_INTP5_ACTIVATION_ENABLE     (0x02U) /* enable activation (INTP5) */
#define _00_DTC_INTP6_ACTIVATION_DISABLE    (0x00U) /* disable activation (INTP6) */
#define _01_DTC_INTP6_ACTIVATION_ENABLE     (0x01U) /* enable activation (INTP6) */

/*  
    DTC Activation Enable Register 1 (DTCEN1)
*/
/* DTC activation enable bit (DTCEN17 - DTCEN10) */
#define _00_DTC_INTP7_ACTIVATION_DISABLE    (0x00U) /* disable activation (INT7) */
#define _80_DTC_INTP7_ACTIVATION_ENABLE     (0x80U) /* enable activation (INT7) */
#define _00_DTC_KEYRT_ACTIVATION_DISABLE    (0x00U) /* disable activation (KEYRT signal detection) */
#define _40_DTC_KEYRT_ACTIVATION_ENABLE     (0x40U) /* enable activation (KEYRT signal detection) */
#define _00_DTC_DSAD_ACTIVATION_DISABLE     (0x00U) /* disable activation (24-bit DSAD converter) */
#define _20_DTC_DSAD_ACTIVATION_ENABLE      (0x20U) /* enable activation (24-bit DSAD converter) */
#define _00_DTC_AD_ACTIVATION_DISABLE       (0x00U) /* disable activation (10-bit SAR-type A/D conversion end) */
#define _10_DTC_AD_ACTIVATION_ENABLE        (0x10U) /* enable activation (10-bit SAR-type A/D conversion end) */
#define _00_DTC_UART0R_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART0 reception transfer end) */
#define _08_DTC_UART0R_ACTIVATION_ENABLE    (0x08U) /* enable activation (UART0 reception transfer end) */
#define _00_DTC_UART0T_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART0 transmission/CSI00/IIC00 transfer end) */
#define _04_DTC_UART0T_ACTIVATION_ENABLE    (0x04U) /* enable activation (UART0 transmission/CSI00/IIC00 transfer end) */
#define _00_DTC_UART1R_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART1 reception transfer end) */
#define _02_DTC_UART1R_ACTIVATION_ENABLE    (0x02U) /* enable activation (UART1 reception transfer end) */
#define _00_DTC_UART1T_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART1 transmission/IIC10 transfer end) */
#define _01_DTC_UART1T_ACTIVATION_ENABLE    (0x01U) /* enable activation (UART1 transmission/IIC10 transfer end) */

/*  
    DTC Activation Enable Register 2 (DTCEN2)
*/
/* DTC activation enable bit (DTCEN27 - DTCEN20) */
#define _00_DTC_UART2R_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART2 reception transfer end) */
#define _80_DTC_UART2R_ACTIVATION_ENABLE    (0x80U) /* enable activation (UART2 reception transfer end) */
#define _00_DTC_UART2T_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART2 transmission transfer end) */
#define _40_DTC_UART2T_ACTIVATION_ENABLE    (0x40U) /* enable activation (UART2 transmission transfer end) */
#define _00_DTC_UART3R_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART3 reception transfer end) */
#define _20_DTC_UART3R_ACTIVATION_ENABLE    (0x20U) /* enable activation (UART3 reception transfer end) */
#define _00_DTC_UART3T_ACTIVATION_DISABLE   (0x00U) /* disable activation (UART3 transmission/CSI30/IIC30 transfer end) */
#define _10_DTC_UART3T_ACTIVATION_ENABLE    (0x10U) /* enable activation (UART3 transmission/CSI30/IIC30 transfer end) */
#define _00_DTC_TAU00_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU00) */
#define _08_DTC_TAU00_ACTIVATION_ENABLE     (0x08U) /* enable activation (TAU00) */
#define _00_DTC_TAU01_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU01) */
#define _04_DTC_TAU01_ACTIVATION_ENABLE     (0x04U) /* enable activation (TAU01) */
#define _00_DTC_TAU02_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU02) */
#define _02_DTC_TAU02_ACTIVATION_ENABLE     (0x02U) /* enable activation (TAU02) */
#define _00_DTC_TAU03_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU03) */
#define _01_DTC_TAU03_ACTIVATION_ENABLE     (0x01U) /* enable activation (TAU03) */

/*  
    DTC Activation Enable Register 3 (DTCEN3)
*/
/* DTC activation enable bit (DTCEN37 - DTCEN30) */

#define _00_DTC_TAU04_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU04) */
#define _80_DTC_TAU04_ACTIVATION_ENABLE     (0x80U) /* enable activation (TAU04) */
#define _00_DTC_TAU05_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU05) */
#define _40_DTC_TAU05_ACTIVATION_ENABLE     (0x40U) /* enable activation (TAU05) */
#define _00_DTC_TAU06_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU06) */
#define _20_DTC_TAU06_ACTIVATION_ENABLE     (0x20U) /* enable activation (TAU06) */
#define _00_DTC_TAU07_ACTIVATION_DISABLE    (0x00U) /* disable activation (TAU07) */
#define _10_DTC_TAU07_ACTIVATION_ENABLE     (0x10U) /* enable activation (TAU07) */
#define _00_DTC_8bitIT00_ACTIVATION_DISABLE (0x00U) /* disable activation (8-bit interval timer 00) */
#define _08_DTC_8bitIT00_ACTIVATION_ENABLE  (0x08U) /* enable activation (8-bit interval timer 00) */
#define _00_DTC_8bitIT01_ACTIVATION_DISABLE (0x00U) /* disable activation (8-bit interval timer 01) */
#define _04_DTC_8bitIT01_ACTIVATION_ENABLE  (0x04U) /* enable activation (8-bit interval timer 01) */
#define _00_DTC_8bitIT10_ACTIVATION_DISABLE (0x00U) /* disable activation (8-bit interval timer 10) */
#define _02_DTC_8bitIT10_ACTIVATION_ENABLE  (0x02U) /* enable activation (8-bit interval timer 10) */
#define _00_DTC_8bitIT11_ACTIVATION_DISABLE (0x00U) /* disable activation (8-bit interval timer 11) */
#define _01_DTC_8bitIT11_ACTIVATION_ENABLE  (0x01U) /* enable activation (8-bit interval timer 11) */


/*  
    DTC Activation Enable Register 4 (DTCEN4)
*/
/* DTC activation enable bit (DTCEN47 - DTCEN40) */
#define _00_DTC_12bitIT_ACTIVATION_DISABLE  (0x00U) /* disable activation (12-bit interval timer) */
#define _80_DTC_12bitIT_ACTIVATION_ENABLE   (0x80U) /* enable activation (12-bit interval timer) */
#define _00_DTC_AES_ACTIVATION_DISABLE      (0x00U) /* disable activation (AES Encryption/Decryption end) */
#define _40_DTC_AES_ACTIVATION_ENABLE       (0x40U) /* enable activation (AES Encryption/Decryption end) */
#define _00_DTC_DSAZC0_ACTIVATION_DISABLE   (0x00U) /* disable activation (DSAD Zero-cross 0 detection) */
#define _20_DTC_DSAZC0_ACTIVATION_ENABLE    (0x20U) /* enable activation (DSAD Zero-cross 0 detection) */
#define _00_DTC_DSAZC1_ACTIVATION_DISABLE   (0x00U) /* disable activation (DSAD Zero-cross 1 detection) */
#define _10_DTC_DSAZC1_ACTIVATION_ENABLE    (0x10U) /* enable activation (DSAD Zero-cross 1 detection) */

#define _00_DTC_RTCIC0_ACTIVATION_DISABLE   (0x00U) /* disable activation (RTCIC0) */
#define _08_DTC_RTCIC0_ACTIVATION_ENABLE    (0x08U) /* enable activation (RTCIC0) */
#define _00_DTC_RTCIC1_ACTIVATION_DISABLE   (0x00U) /* disable activation (RTCIC1) */
#define _04_DTC_RTCIC1_ACTIVATION_ENABLE    (0x04U) /* enable activation (RTCIC1) */
#define _00_DTC_RTCIC2_ACTIVATION_DISABLE   (0x00U) /* disable activation (RTCIC2) */
#define _02_DTC_RTCIC2_ACTIVATION_ENABLE    (0x02U) /* enable activation (RTCIC2) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _FF1E_DTCD0_SRC_ADDRESS             (0xFF1EU)
#define _DF00_DTCD0_DEST_ADDRESS            (0xDF00U)
#define _01_DTCD0_TRANSFER_BYTE             (0x01U)
#define _08_DTCD1_TRANSFER_BYTE             (0x08U)
#define _01_DTCD0_TRANSFER_BLOCKSIZE        (0x01U)
#define _01_DTCD1_TRANSFER_BLOCKSIZE        (0x01U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    uint8_t  dtccr;
    uint8_t  dtbls;
    uint8_t  dtcct;
    uint8_t  dtrld;
    uint16_t dtsar;
    uint16_t dtdar;
}st_dtc_data;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_DTC_Create(void);
void R_DTCD0_Start(void);
void R_DTCD0_Stop(void);
void R_DTCD1_Start(void);
void R_DTCD1_Stop(void);

/* Start user code for function. Do not edit comment generated here */
extern volatile uint16_t g_adc_data[8];
/* End user code. Do not edit comment generated here */
#endif