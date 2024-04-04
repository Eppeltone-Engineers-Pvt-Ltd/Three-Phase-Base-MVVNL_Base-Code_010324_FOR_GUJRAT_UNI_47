/***********************************************************************************************************************
* File Name    : r_lvd.c
* Version      : 
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for LVD module.
* Creation Date: 5/7/2015
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
#include "r_cg_lvd.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_cg_userdefine.h"
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

void R_LVD_Create(void)
{
    /* 
    Setting option byte for choosing the LVD mode:    
    - Interrupt & reset mode(VLVDL: 1.63 V, VLVDH: Rising edge 1.77 V; Falling edge 1.73 V): xx1Axx
    - Reset mode(VLVD: Rising edge 1.67 V; Falling edge 1.63 V): xx1Fxx
    - interrupt mode)(VLVD: Rising edge 1.67 V; Falling edge 1.63 V): xx1Exx
    */  
    LVIMK = 1U;     /* disable INTLVI interrupt */
    LVIIF = 0U;     /* clear INTLVI interrupt flag */
    /* Set INTLVI low priority */
    LVIPR1 = 1U;
    LVIPR0 = 1U;
    
}

void R_LVD_InterruptMode_Start(void)
{
    LVIIF = 0U;     /* clear INTLVI interrupt flag */
    LVIMK = 0U;     /* enable INTLVI interrupt */
    
}

void R_LVDVDD_Create(void)
{
    LVDVDMK = 1U;     /* disable INTLVI interrupt */
    LVDVDIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDVDDEN = 0U;
    /* When developing, set according to voltage source using:
        When switch to backup battery, the VDD also has smaller value voltage
    */
//    LVDVDD = _03_VLVDVDD_LEVEL3;  /* Rising Edge 3.16 V, Falling Edge 3.09 V */
//    LVDVDD = _01_VLVDVDD_LEVEL1;  /* Rising Edge 2.75 V, Falling Edge 2.68 V */
    LVDVDD = _00_VLVDVDD_LEVEL0;  /* Rising Edge 2.54 V, Falling Edge 2.47 V */
    
    /* Set INTLVI low priority */
    LVDVDPR1 = 1U;
    LVDVDPR0 = 1U;
}

void R_LVDVDD_Start(void)
{
    LVDVDDEN = 1U;
    
    LVDVDIF = 0U;     /* clear INTLVI interrupt flag */
    LVDVDMK = 0U;     /* enable INTLVI interrupt */
}

void R_LVDVDD_Stop(void)
{
    LVDVDMK = 1U;     /* disable INTLVI interrupt */
    LVDVDIF = 0U;     /* clear INTLVI interrupt flag */
        
    LVDVDDEN = 0U;
}

void R_LVDVBAT_Create(void)
{
    LVDVBMK = 1U;     /* disable INTLVI interrupt */
    LVDVBIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDVBATEN = 0U;
//    LVDVBAT |= _03_VLVDVBAT_LEVEL3; /* Rising Edge 2.43 V, Falling Edge 2.37 V */ 
//    LVDVBAT |= _02_VLVDVBAT_LEVEL2; /* Rising Edge 2.33 V, Falling Edge 2.27 V */ 
    LVDVBAT |= _01_VLVDVBAT_LEVEL1; /* Rising Edge 2.22 V, Falling Edge 2.16 V */ 
    
    /* Set INTLVI low priority */
    LVDVBPR1 = 1U;
    LVDVBPR0 = 1U;
}

void R_LVDVBAT_Start(void)
{
    LVDVBATEN = 1U;
    
    LVDVBIF = 0U;     /* clear INTLVI interrupt flag */
    LVDVBMK = 0U;     /* enable INTLVI interrupt */
}
void R_LVDVBAT_Stop(void)
{
    LVDVBMK = 1U;     /* disable INTLVI interrupt */
    LVDVBIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDVBATEN = 0U;
}

void R_LVDVRTC_Create(void)
{
    LVDVRMK = 1U;     /* disable INTLVI interrupt */
    LVDVRIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDVRTCEN = 0U;
//    LVDVRTC |= _00_VLVDVRTC_LEVEL0 ;  /* Rising Edge 2.22 V, Falling Edge 2.16V */
//    LVDVRTC |= _01_VLVDVRTC_LEVEL1 ;  /* Rising Edge 2.43 V, Falling Edge 2.37V */
    LVDVRTC |= _02_VLVDVRTC_LEVEL2 ;  /* Rising Edge 2.63 V, Falling Edge 2.57V */
    
    /* Set INTLVI low priority */
    LVDVRPR1 = 1U;
    LVDVRPR0 = 1U;    
}

void R_LVDVRTC_Start(void)
{
    LVDVRTCEN = 1U;
    
    LVDVRIF = 0U;     /* clear INTLVI interrupt flag */
    LVDVRMK = 0U;     /* enable INTLVI interrupt */
}

void R_LVDVRTC_Stop(void)
{
    LVDVRMK = 1U;     /* disable INTLVI interrupt */
    LVDVRIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDVRTCEN = 0U;
}

void R_LVDEXLVD_Create(void)
{
    LVDEXMK = 1U;     /* disable INTLVI interrupt */
    LVDEXIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDEXLVDEN = 0U;
    
    /* Set INTLVI low priority */
    LVDEXPR1 = 1U;
    LVDEXPR0 = 1U;
}

void R_LVDEXLVD_Start(void)
{
    LVDEXLVDEN = 1U;
    
    LVDEXIF = 0U;     /* clear INTLVI interrupt flag */
    LVDEXMK = 0U;     /* enable INTLVI interrupt */
}


void R_LVDEXLVD_Stop(void)
{
    LVDEXMK = 1U;     /* disable INTLVI interrupt */
    LVDEXIF = 0U;     /* clear INTLVI interrupt flag */
    
    LVDEXLVDEN = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
