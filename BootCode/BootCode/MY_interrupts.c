#include "iodefine.h"

#pragma interrupt watchdog_intrpt(vect=INTWDTI)
#pragma interrupt lvi_intrpt(vect=INTLVI)
#pragma interrupt intp0_intrpt(vect=INTP0)
#pragma interrupt intp1_intrpt(vect=INTP1)
#pragma interrupt intp2_intrpt(vect=INTP2)
#pragma interrupt intp3_intrpt(vect=INTP3)
#pragma interrupt intp4_intrpt(vect=INTP4)
#pragma interrupt intp5_intrpt(vect=INTP5)
#pragma interrupt intst2_interrupt(vect=INTST2) 
#pragma interrupt intsr2_interrupt(vect=INTSR2)
#pragma interrupt intsre2_interrupt(vect=INTSRE2)
#pragma interrupt intcr_interrupt(vect=INTCR)
#pragma interrupt intaes_interrupt(vect=INTAESF)
#pragma interrupt intst0_interrupt(vect=INTST0)
#pragma interrupt intiica0_interrupt(vect=INTIICA0)
#pragma interrupt intsr0_interrupt(vect=INTSR0)
#pragma interrupt intsre0_interrupt(vect=INTSRE0)
#pragma interrupt intst1_interrupt(vect=INTST1)
#pragma interrupt intsr1_interrupt(vect=INTSR1)
#pragma interrupt intsre1_interrupt(vect=INTSRE1)
#pragma interrupt inttm00_interrupt(vect=INTTM00)
#pragma interrupt intfm_interrupt(vect=INTFM)
#pragma interrupt inttm01_interrupt(vect=INTTM01)
#pragma interrupt inttm02_interrupt(vect=INTTM02)
#pragma interrupt inttm03_interrupt(vect=INTTM03)
#pragma interrupt intad_interrupt(vect=INTAD)
#pragma interrupt intrtcrpd_interrupt(vect=INTRTCRPD)
#pragma interrupt intit_interrupt(vect=INTIT)
#pragma interrupt intkr_interrupt(vect=INTKR)
#pragma interrupt intst3_interrupt(vect=INTST3)
#pragma interrupt intsr3_interrupt(vect=INTSR3)
#pragma interrupt intdsad_interrupt(vect=INTDSAD)
#pragma interrupt inttm04_interrupt(vect=INTTM04)
#pragma interrupt inttm05_interrupt(vect=INTTM05)
#pragma interrupt intp6_interrupt(vect=INTP6)
#pragma interrupt intp7_interrupt(vect=INTP7)
#pragma interrupt intrtcic0_interrupt(vect=INTRTCIC0)
#pragma interrupt intrtcic1_interrupt(vect=INTRTCIC1)
#pragma interrupt intrtcic2_interrupt(vect=INTRTCIC2)
#pragma interrupt inttm06_interrupt(vect=INTTM06)
#pragma interrupt inttm07_interrupt(vect=INTTM07)
#pragma interrupt intit00_interrupt(vect=INTIT00)
#pragma interrupt intit01_interrupt(vect=INTIT01)
#pragma interrupt intsre3_interrupt(vect=INTSRE3)
#pragma interrupt intmaclof_interrupt(vect=INTMACLOF)
#pragma interrupt intosdc_interrupt(vect=INTOSDC)
#pragma interrupt intfl_interrupt(vect=INTFL)
#pragma interrupt intdsadzc0_interrupt(vect=INTDSADZC0)
#pragma interrupt intdsadzc1_interrupt(vect=INTDSADZC1)
#pragma interrupt intit10_interrupt(vect=INTIT10)
#pragma interrupt intit11_interrupt(vect=INTIT11)
#pragma interrupt intlvdvdd_interrupt(vect=INTLVDVDD)
#pragma interrupt intlvdvbat_interrupt(vect=INTLVDVBAT)
#pragma interrupt intlvdvrtc_interrupt(vect=INTLVDVRTC)
#pragma interrupt intlvdexlvd_interrupt(vect=INTLVDEXLVD)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "MY_ram_vector.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void watchdog_intrpt(void)
{
    (*RAM_INTWDTI_ISR)();
}

static void lvi_intrpt(void)
{
    (*RAM_INTLVI_ISR)();
}

static void intp0_intrpt(void)
{
    (*RAM_INTP0_ISR)();
}

static void intp1_intrpt(void)
{
    (*RAM_INTP1_ISR)();
}

static void intp2_intrpt(void)
{
    (*RAM_INTP2_ISR)();
}

static void intp3_intrpt(void)
{
    (*RAM_INTP3_ISR)();
}

static void intp4_intrpt(void)
{
    (*RAM_INTP4_ISR)();
}

static void intp5_intrpt(void)
{
    (*RAM_INTP5_ISR)();
}

static void intst2_interrupt(void)
{
    (*RAM_INTST2_ISR)();
}

static void intsr2_interrupt(void)
{
    (*RAM_INTSR2_ISR)();
}

static void intsre2_interrupt(void)
{
    (*RAM_INTSRE2_ISR)();
}

static void intcr_interrupt(void)
{
    (*RAM_INTCR_ISR)();
}

static void intaes_interrupt(void)
{
    (*RAM_INTAES_ISR)();
}

static void intst0_interrupt(void)
{
    (*RAM_INTST0_ISR)();
}

static void intiica0_interrupt(void)
{
    (*RAM_INTIICA0_ISR)();
}

static void intsr0_interrupt(void)
{
    (*RAM_INTSR0_ISR)();
}

static void intsre0_interrupt(void)
{
    (*RAM_INTSRE0_ISR)();
}

static void intst1_interrupt(void)
{
    (*RAM_INTST1_ISR)();
}

static void intsr1_interrupt(void)
{
    (*RAM_INTSR1_ISR)();
}

static void intsre1_interrupt(void)
{
    (*RAM_INTSRE1_ISR)();
}

static void inttm00_interrupt(void)
{
    (*RAM_INTTM00_ISR)();
}

static void intfm_interrupt(void)
{
    (*RAM_INTFM_ISR)();
}

static void inttm01_interrupt(void)
{
    (*RAM_INTTM01_ISR)();
}

static void inttm02_interrupt(void)
{
    (*RAM_INTTM02_ISR)();
}

static void inttm03_interrupt(void)
{
    (*RAM_INTTM03_ISR)();
}

static void intad_interrupt(void)
{
    (*RAM_INTAD_ISR)();
}

static void intrtcrpd_interrupt(void)
{
    (*RAM_INTRTCRPD_ISR)();
}

static void intit_interrupt(void)
{
    (*RAM_INTIT_ISR)();
}

static void intkr_interrupt(void)
{
    (*RAM_INTKR_ISR)();
}

static void intst3_interrupt(void)
{
    (*RAM_INTST3_ISR)();
}

static void intsr3_interrupt(void)
{
    (*RAM_INTSR3_ISR)();
}

static void intdsad_interrupt(void)
{
   (*RAM_INTDSAD_ISR)();
}

static void inttm04_interrupt(void)
{
    (*RAM_INTTM04_ISR)();
}

static void inttm05_interrupt(void)
{
    (*RAM_INTTM05_ISR)();
}

static void intp6_interrupt(void)
{
    (*RAM_INTP6_ISR)();
}

static void intp7_interrupt(void)
{
    (*RAM_INTP7_ISR)();
}

static void intrtcic0_interrupt(void)
{
    (*RAM_INTRTCIC0_ISR)();
}

static void intrtcic1_interrupt(void)
{
    (*RAM_INTRTCIC1_ISR)();
}

static void intrtcic2_interrupt(void)
{
    (*RAM_INTRTCIC2_ISR)();
}

static void inttm06_interrupt(void)
{
    (*RAM_INTTM06_ISR)();
}
static void inttm07_interrupt(void)
{
    (*RAM_INTTM07_ISR)();
}

static void intit00_interrupt(void)
{
    (*RAM_INTIT00_ISR)();
}
static void intit01_interrupt(void)
{
    (*RAM_INTIT01_ISR)();
}

static void intsre3_interrupt(void)
{
    (*RAM_INTSRE3_ISR)();
}

static void intmaclof_interrupt(void)
{
    (*RAM_INTMACLOF_ISR)();
}

static void intosdc_interrupt(void)
{
    (*RAM_INTOSDC_ISR)();
}

static void intfl_interrupt(void)
{
    (*RAM_INTFL_ISR)();
}

static void intdsadzc0_interrupt(void)
{
    (*RAM_INTDSADZC0_ISR)();
}

static void intdsadzc1_interrupt(void)
{
    (*RAM_INTDSADZC1_ISR)();
}

static void intit10_interrupt(void)
{
    (*RAM_INTIT10_ISR)();
}

static void intit11_interrupt(void)
{
    (*RAM_INTIT11_ISR)();
}

static void intlvdvdd_interrupt(void)
{
    (*RAM_INTLVDVDD_ISR)();
}

static void intlvdvbat_interrupt(void)
{
    (*RAM_INTLVDVBAT_ISR)();
}

static void intlvdvrtc_interrupt(void)
{
    (*RAM_INTLVDVRTC_ISR)();
}

static void intlvdexlvd_interrupt(void)
{
    (*RAM_INTLVDEXLVD_ISR)();
}




