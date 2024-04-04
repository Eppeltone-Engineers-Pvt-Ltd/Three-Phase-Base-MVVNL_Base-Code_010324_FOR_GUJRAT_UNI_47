#ifndef APPINTERRUPTS_H
#define APPINTERRUPTS_H

extern void r_lvd_interrupt(void);
extern void r_lvdvdd_interrupt(void);
extern void r_lvdvbat_interrupt(void);
extern void r_lvdvrtc_interrupt(void);
extern void r_lvdexlvd_interrupt(void);

void r_intc0_interrupt(void);
void r_intc1_interrupt(void);
void r_intc2_interrupt(void);
void r_intc3_interrupt(void);
void r_intc4_interrupt(void);
void r_intc5_interrupt(void);
void r_intc6_interrupt(void);
void r_intc7_interrupt(void);

void r_uart0_interrupt_receive(void);
void r_uart0_interrupt_send(void);

void r_uart2_interrupt_receive(void);
void r_uart2_interrupt_send(void);

void r_uart3_interrupt_receive(void);
void r_uart3_interrupt_send(void);

void r_uart1_interrupt_receive(void);	// dummy
void r_uart1_interrupt_send(void);	// dummy

void r_iica0_interrupt(void);

void r_tau0_channel0_interrupt(void);
void r_tau0_channel1_interrupt(void);
void r_tau0_channel2_interrupt(void);
void r_tau0_channel3_interrupt(void);

void r_adc_interrupt(void);
void p_timer_interrupt(void);

void r_rtc_time_capture0(void);
void r_rtc_time_capture1(void);
void r_rtc_time_capture2(void);

void r_dsadzc0_interrupt(void);
void r_dsadzc1_interrupt(void);

void r_macl_interrupt(void);
void r_rtc_rpd_interrupt(void);




#endif