#include "r_cg_macrodriver.h"
#include "iodefine.h"
#include "r_cg_cgc.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_cg_tau.h"
#include "r_cg_sau.h"
#include "r_cg_wdt.h"
#include "r_cg_iica.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

void R_CGC_FclkUseFsub(void)
{
    /* Select fSUB as fCLK */
    CSS = 1U;
    
    /* Stop High-speed on-chip clock */
    HIOSTOP = 1U;
}

void R_CGC_FclkUseFih(void)
{
    volatile uint16_t w_count;
    
    /* Start fIH */
    HIOSTOP = 0U;
    
    /* Wait stabilization time */
    for (w_count = 0U; w_count <= CGC_SUBWAITTIME; w_count++)
    {
        NOP();
    }
    
    /* Select fMAIN as CPU/peripherals clock */
    CSS = 0U;
}

void R_CGC_OperateAt24MHz(void)
{
    HOCODIV = 0x00U;        /*  MCU operate at 24Mhz */
    
    /* UART debug 38400bps 8N1, release 9600 8N1 */
    #ifdef __DEBUG
    SPS0 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;
    SPS1 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;
    #else
//    SPS0 = _0005_SAU_CK00_FCLK_5 | _0050_SAU_CK01_FCLK_5;
//    SPS1 = _0005_SAU_CK00_FCLK_5 | _0050_SAU_CK01_FCLK_5;
		
    SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 9600bps */
    SPS1 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 9600bps */
		
    #endif
    
    TPS0 = _0000_TAU_CKM0_fCLK_0 | _0040_TAU_CKM1_fCLK_4 | _0000_TAU_CKM2_fCLK_1 | _0000_TAU_CKM3_fCLK_8;
//    R_IICA0_SetTransClockAt24MHz();
}

void R_CGC_OperateAt12MHz(void)
{
    HOCODIV = 0x01U;        /*  MCU operate at 12Mhz */
    
    /* UART debug 38400bps 8N1, release 4800 8N1 */
    #ifdef __DEBUG
    SPS0 = _0001_SAU_CK00_FCLK_1 | _0010_SAU_CK01_FCLK_1;
    SPS1 = _0001_SAU_CK00_FCLK_1 | _0010_SAU_CK01_FCLK_1;
    #else
    SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;
    SPS1 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;
    #endif
    
    TPS0 = _0000_TAU_CKM0_fCLK_0 | _0040_TAU_CKM1_fCLK_4 | _0000_TAU_CKM2_fCLK_1 | _0000_TAU_CKM3_fCLK_8;
//    R_IICA0_SetTransClockAt12MHz();
}

void R_CGC_OperateAt6MHz(void)
{
    HOCODIV = 0x02U;        /*  MCU operate at 6Mhz */
    
    /* UART debug 38400bps 8N1, release 4800 8N1 */
    #ifdef __DEBUG
    SPS0 = _0000_SAU_CK00_FCLK_0 | _0000_SAU_CK01_FCLK_0;
    SPS1 = _0000_SAU_CK00_FCLK_0 | _0000_SAU_CK01_FCLK_0;
    #else
//    SPS0 = _0003_SAU_CK00_FCLK_3 | _0030_SAU_CK01_FCLK_3;	// 4800 BAUD
//    SPS1 = _0003_SAU_CK00_FCLK_3 | _0030_SAU_CK01_FCLK_3;	// 4800 baud at 6 Mhz

    SPS0 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;	// 9600 baud at 6 MHz
    SPS1 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;	// 9600 baud at 6 MHz

//    SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 2400bps */
//    SPS1 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;   /* 2400bps */

		#endif
    
    TPS0 = _0000_TAU_CKM0_fCLK_0 | _0040_TAU_CKM1_fCLK_4 | _0000_TAU_CKM2_fCLK_1 | _0000_TAU_CKM3_fCLK_8;
//    R_IICA0_SetTransClockAt6MHz();
}

/* End user code. Do not edit comment generated here */
