/***********************************************************************************************************************
* File Name    : r_cg_rtc.h
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : This file implements device driver for RTC module.
* Creation Date: 
***********************************************************************************************************************/
#ifndef RTC_H
#define RTC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    RTC Control Register 1 (RCR1)
*/
/* Alarm Interrupt Enable (AIE) */
#define _00_RTC_ALARM_INT_DISABLE                   (0x00U)
#define _01_RTC_ALARM_INT_ENABLE                    (0x01U)
/* Carry Interrupt Enable (CIE) */
#define _00_RTC_CARRY_INT_DISABLE                   (0x00U)
#define _02_RTC_CARRY_INT_ENABLE                    (0x02U)
/* Periodic Interrupt Enable (PIE) */
#define _00_RTC_PERIOD_INT_DISABLE                  (0x00U)
#define _04_RTC_PERIOD_INT_ENABLE                   (0x04U)
/* RTCOUT Output Select (RTCOS) */
#define _00_RTC_RTCOUT_FREQ_1HZ                     (0x00U)
#define _08_RTC_RTCOUT_FREQ_64HZ                    (0x08U)
/* Periodic Interrupt Select (PES[3:0]) */
#define _00_RTC_PERIODIC_INT_PERIOD_DISABLE         (0x00U)
#define _60_RTC_PERIODIC_INT_PERIOD_1_256           (0x60U)
#define _70_RTC_PERIODIC_INT_PERIOD_1_128           (0x70U)
#define _80_RTC_PERIODIC_INT_PERIOD_1_64            (0x80U)
#define _90_RTC_PERIODIC_INT_PERIOD_1_32            (0x90U)
#define _A0_RTC_PERIODIC_INT_PERIOD_1_16            (0xA0U)
#define _B0_RTC_PERIODIC_INT_PERIOD_1_8             (0xB0U)
#define _C0_RTC_PERIODIC_INT_PERIOD_1_4             (0xC0U)
#define _D0_RTC_PERIODIC_INT_PERIOD_1_2             (0xD0U)
#define _E0_RTC_PERIODIC_INT_PERIOD_1               (0xE0U)
#define _F0_RTC_PERIODIC_INT_PERIOD_2               (0xF0U)

/*
    RTC Control Register 2 (RCR2)
*/
/* Start (START) */
#define _00_RTC_RCR2_START_STOP                     (0x00U)
#define _01_RTC_RCR2_START_START                    (0x01U)
/* RTC Software Reset (RESET) */
#define _00_RTC_RCR2_RESET_READY_READONLY           (0x00U)
#define _02_RTC_RCR2_RESET                          (0x02U)
/* 30-Second Adjustment (ADJ30) */
#define _00_RTC_30SEC_ADJUSTMENT_READY_READONLY     (0x00U)
#define _04_RTC_30SEC_ADJUSTMENT                    (0x04U)
/* RTCOUT Output Enable (RTCOE) */
#define _00_RTC_RTCOUT_OUTPUT_DISABLE               (0x00U)
#define _08_RTC_RTCOUT_OUTPUT_ENABLE                (0x08U)
/* Automatic Adjustment Enable (AADJE) */
#define _00_RTC_AUTO_ADJUSTMENT_DISABLE             (0x00U)
#define _10_RTC_AUTO_ADJUSTMENT_ENABLE              (0x10U)
/* Automatic Adjustment Period Select (AADJP) */
#define _00_RTC_AUTO_ADJUSTMENT_PERIOD_1MIN         (0x00U)
#define _20_RTC_AUTO_ADJUSTMENT_PERIOD_10SEC        (0x20U)
#define _00_RTC_AUTO_ADJUSTMENT_PERIOD_32SEC        (0x00U)
#define _20_RTC_AUTO_ADJUSTMENT_PERIOD_8SEC         (0x20U)
/* Hours Mode (HR24) */
#define _00_RTC_HOUR_MODE_12                        (0x00U)
#define _40_RTC_HOUR_MODE_24                        (0x40U)
/* Count Mode Select (CNTMD) */
#define _00_RTC_COUNT_MODE_CAL                      (0x00U)
#define _80_RTC_COUNT_MODE_BIN                      (0x80U)

/*
    RTC Control Register (RCR3)
*/
/* Sub-clock Control (RTCEN) */
#define _00_RTC_SUBCLOCK_DISABLE                    (0x00U)
#define _01_RTC_SUBCLOCK_ENABLE                     (0x01U)
/* Sub-clock Oscillator Drive Capacity Control (RTCDV[1:0]) */
#define _00_RTC_SUBCLOCK_DRIVE_MID                  (0x00U)
#define _02_RTC_SUBCLOCK_DRIVE_HIGH                 (0x02U)
#define _0A_RTC_SUBCLOCK_DRIVE_LOW                  (0x0AU)

/*
    RTC Control Register (RCR4)
*/
/* Count source selection (RCKSEL) */
#define _00_RTC_COUNT_SOURCE_32KHZ_CLOCK            (0x00U)
#define _01_RTC_COUNT_SOURCE_ENHANCING_CLOCK        (0x01U)

/*
    Timer Error Adjustment Register (RADJ)
*/
/* Plus-Minus (PMADJ[1:0]) */
#define _40_RTC_TIMER_ERROR_ADJUST_PLUS             (0x40U)
#define _80_RTC_TIMER_ERROR_ADJUST_MINUS            (0x80U)

/*
    Hour Counter (RHRCNT)
*/
/* PM */
#define _00_RTC_HOUR_ATTRIB_AM                      (0x00U)
#define _40_RTC_HOUR_ATTRIB_PM                      (0x40U)

/*
    Second Alarm Register (RSECAR)
*/
/* ENB */
#define _00_RTC_ALARM_SECOND_DISABLE                (0x00U)
#define _80_RTC_ALARM_SECOND_ENABLE                 (0x80U)

/*
    Minute Alarm Register (RMINAR)
*/
/* ENB */
#define _00_RTC_ALARM_MINUTE_DISABLE                (0x00U)
#define _80_RTC_ALARM_MINUTE_ENABLE                 (0x80U)

/*
    Hour Alarm Register (RHRAR)
*/
/* ENB */
#define _00_RTC_ALARM_HOUR_DISABLE                  (0x00U)
#define _80_RTC_ALARM_HOUR_ENABLE                   (0x80U)
/* PM */
#define _00_RTC_ALARM_HOUR_ATTRIB_AM                (0x00U)
#define _40_RTC_ALARM_HOUR_ATTRIB_PM                (0x40U)

/* 
    Day-of-Week Alarm Register (RWKAR)
*/
/* ENB */
#define _00_RTC_ALARM_WEEK_DISABLE                  (0x00U)
#define _80_RTC_ALARM_WEEK_ENABLE                   (0x80U)

/*
    Date Alarm Register (RDAYAR)
*/
/* ENB */
#define _00_RTC_ALARM_DAY_DISABLE                   (0x00U)
#define _80_RTC_ALARM_DAY_ENABLE                    (0x80U)

/*
    Month Alarm Register (RMONAR)
*/
/* ENB */
#define _00_RTC_ALARM_MONTH_DISABLE                 (0x00U)
#define _80_RTC_ALARM_MONTH_ENABLE                  (0x80U)

/*
    Year Alarm Enable Register (RYRAREN)
*/
/* ENB */
#define _00_RTC_ALARM_YEAR_DISABLE                  (0x00U)
#define _80_RTC_ALARM_YEAR_ENABLE                   (0x80U)

/*
    Capture control register (RTCCRn)
*/
/* Capture control bit (TCCT[1:0]) */
#define _00_RTC_CAPTURE_EVENT_NO_DETECTION          (0x00U)
#define _01_RTC_CAPTURE_EVENT_RISING_EDGE           (0x01U)
#define _02_RTC_CAPTURE_EVENT_FALLING_EDGE          (0x02U)
#define _03_RTC_CAPTURE_EVENT_BOTH_EDGE             (0x03U)
/* Time capture status bit (TCST) */
#define _00_RTC_CAPTURE_EVENT_NOT_DETECTED          (0x00U)
#define _04_RTC_CAPTURE_EVENT_IS_DETECTED           (0x04U)
/* Time capture noise filter control (TCNF[1:0]) */
#define _00_RTC_CAPTURE_EVENT_NOISE_FILTER_OFF      (0x00U)
#define _20_RTC_CAPTURE_EVENT_NOISE_FILTER_ON       (0x20U)
#define _30_RTC_CAPTURE_EVENT_NOISE_FILTER_ON_32DIV (0x30U)
/* Time input terminal enable (TCEN) */
#define _00_RTC_CAPTURE_EVENT_INPUT_DISABLE         (0x00U)
#define _80_RTC_CAPTURE_EVENT_INPUT_ENABLE          (0x80U)

/*
    Capture input noise filter enable register (RTCICNFEN)
*/
/* RTCIC0 (RTCIC0NF1 RTCIC0NF0) */
#define _00_RTCIC0_NOISE_FILTER_OFF                 (0x00U)
#define _01_RTCIC0_NOISE_FILTER_OFF                 (0x01U)
#define _10_RTCIC0_NOISE_FILTER_ON_FSX_12           (0x10U)
#define _11_RTCIC0_NOISE_FILTER_ON_FSX_13           (0x11U)

/* RTCIC1 (RTCIC1NF1 RTCIC1NF0) */
#define _00_RTCIC1_NOISE_FILTER_OFF                 (0x00U)
#define _02_RTCIC1_NOISE_FILTER_OFF                 (0x02U)
#define _20_RTCIC1_NOISE_FILTER_ON_FSX_12           (0x20U)
#define _22_RTCIC1_NOISE_FILTER_ON_FSX_13           (0x22U)

/* RTCIC2 (RTCIC2NF1 RTCIC2NF0) */
#define _00_RTCIC2_NOISE_FILTER_OFF                 (0x00U)
#define _04_RTCIC2_NOISE_FILTER_OFF                 (0x04U)
#define _40_RTCIC2_NOISE_FILTER_ON_FSX_12           (0x40U)
#define _44_RTCIC2_NOISE_FILTER_ON_FSX_13           (0x44U)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct 
{
    uint8_t rseccnt;
    uint8_t rmincnt;
    uint8_t rhrcnt;
    uint8_t rdaycnt;
    uint8_t rwkcnt;
    uint8_t rmoncnt;
    uint16_t ryrcnt;
} rtc_calendarcounter_value_t;

typedef struct
{
    uint8_t sec_enb;
    uint8_t min_enb;
    uint8_t hr_enb;
    uint8_t day_enb;
    uint8_t wk_enb;
    uint8_t mon_enb;
    uint8_t yr_enb;
} rtc_alarm_enable_t;

typedef struct 
{
    uint8_t rseccnt;
    uint8_t rmincnt;
    uint8_t rhrcnt;
    uint8_t rdaycnt;
    uint8_t rmoncnt;
} rtc_timecapture_value_t;

typedef enum
{
    PES_1_256_SEC = 0x60U,
    PES_1_128_SEC = 0x70U,
    PES_1_64_SEC = 0x80U,
    PES_1_32_SEC = 0x90U,
    PES_1_16_SEC = 0xA0U,
    PES_1_8_SEC = 0xB0U,
    PES_1_4_SEC = 0xC0U,
    PES_1_2_SEC = 0xD0U,
    PES_1_SEC = 0xE0U,
    PES_2_SEC = 0xF0U
} rtc_int_period_t;

typedef enum
{
    RTCOS_FREQ_1HZ = 0x00U,
    RTCOS_FREQ_64HZ = 0x08U,
} rtc_clock_out_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
uint8_t R_RTC_Create(void);
uint8_t R_RTC_Start(void);
void R_RTC_Stop(void);
void R_RTC_Restart(rtc_calendarcounter_value_t counter_write_val);
void R_RTC_Get_CalendarCounterValue(rtc_calendarcounter_value_t * const counter_read_val);
void R_RTC_Set_CalendarCounterValue(rtc_calendarcounter_value_t counter_write_val);
void R_RTC_Get_TimeCapture0Value(rtc_timecapture_value_t * const counter_read_val);
void R_RTC_Get_TimeCapture1Value(rtc_timecapture_value_t * const counter_read_val);
void R_RTC_Get_TimeCapture2Value(rtc_timecapture_value_t * const counter_read_val);
void R_RTC_Set_ConstPeriodInterruptOn(rtc_int_period_t period);
void R_RTC_Set_ConstPeriodInterruptOff(void);
void R_RTC_Set_ClockOutputOn(rtc_clock_out_t clk_out);
void R_RTC_Set_ClockOutputOff(void);
void R_RTC_Compensate(float ppm);
void R_RTC_StopCompensation(void);
void R_RTC_Set_AlarmValue(rtc_calendarcounter_value_t time_value, rtc_alarm_enable_t control);
void R_RTC_Get_AlarmValue(rtc_calendarcounter_value_t * const time_value, rtc_alarm_enable_t * const control);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif