#include "My_ram_vector.h"

#pragma section bss .ramvect

void (*RAM_INTWDTI_ISR)(void);
void (*RAM_INTLVI_ISR)(void);
void (*RAM_INTP0_ISR)(void);
void (*RAM_INTP1_ISR)(void);
void (*RAM_INTP2_ISR)(void);
void (*RAM_INTP3_ISR)(void);
void (*RAM_INTP4_ISR)(void);
void (*RAM_INTP5_ISR)(void);
void (*RAM_INTST2_ISR)(void);
void (*RAM_INTSR2_ISR)(void);
void (*RAM_INTSRE2_ISR)(void);
void (*RAM_INTCR_ISR)(void);
void (*RAM_INTAES_ISR)(void);
void (*RAM_INTST0_ISR)(void);
void (*RAM_INTIICA0_ISR)(void);
void (*RAM_INTSR0_ISR)(void);
void (*RAM_INTSRE0_ISR)(void);
void (*RAM_INTST1_ISR)(void);
void (*RAM_INTSR1_ISR)(void);
void (*RAM_INTSRE1_ISR)(void);
void (*RAM_INTTM00_ISR)(void);
void (*RAM_INTFM_ISR)(void);
void (*RAM_INTTM01_ISR)(void);
void (*RAM_INTTM02_ISR)(void);
void (*RAM_INTTM03_ISR)(void);
void (*RAM_INTAD_ISR)(void);
void (*RAM_INTRTCRPD_ISR)(void);
void (*RAM_INTIT_ISR)(void);
void (*RAM_INTKR_ISR)(void);
void (*RAM_INTST3_ISR)(void);
void (*RAM_INTSR3_ISR)(void);
void (*RAM_INTDSAD_ISR)(void);
void (*RAM_INTTM04_ISR)(void);
void (*RAM_INTTM05_ISR)(void);
void (*RAM_INTP6_ISR)(void);
void (*RAM_INTP7_ISR)(void);
void (*RAM_INTRTCIC0_ISR)(void);
void (*RAM_INTRTCIC1_ISR)(void);
void (*RAM_INTRTCIC2_ISR)(void);
void (*RAM_INTTM06_ISR)(void);
void (*RAM_INTTM07_ISR)(void);
void (*RAM_INTIT00_ISR)(void);
void (*RAM_INTIT01_ISR)(void);
void (*RAM_INTSRE3_ISR)(void);
void (*RAM_INTMACLOF_ISR)(void);
void (*RAM_INTOSDC_ISR)(void);
void (*RAM_INTFL_ISR)(void);
void (*RAM_INTDSADZC0_ISR)(void);
void (*RAM_INTDSADZC1_ISR)(void);
void (*RAM_INTIT10_ISR)(void);
void (*RAM_INTIT11_ISR)(void);
void (*RAM_INTLVDVDD_ISR)(void);
void (*RAM_INTLVDVBAT_ISR)(void);
void (*RAM_INTLVDVRTC_ISR)(void);
void (*RAM_INTLVDEXLVD_ISR)(void);
