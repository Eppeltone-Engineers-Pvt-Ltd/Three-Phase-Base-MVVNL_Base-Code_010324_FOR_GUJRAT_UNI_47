/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : service.h
* Version      : 1.00
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 
******************************************************************************/

#ifndef _SERVICE_H
#define _SERVICE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"                /* GSCE Typedef */

#include "format.h"                 /* EEPROM Format */
#include "eeprom.h"                 /* EEPROM Driver */
#include "storage.h"                /* Storage */
#include "platform.h"               /* Meter platform */ 

#include "config_format.h"          /* CONFIG Format */
#include "lvm.h"                    /* CONFIG Driver */
#include "config_storage.h"         /* CONFIG Storage */

#include "service_em_data.h"
#include "service_rw_calibration.h"
#include "service_data_logging.h"
#include "service_auto_calibration.h"
#include "lib64bit.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Raw Data Log Service */
#define RAW_DATA_GROUP_ADC_WAVEFORM                 (0) /* ADC capture */
#define RAW_DATA_GROUP_MEASUREMENT                  (1) /* Measurement */

/* COMMON */
#define SERVICE_USE_DUMMY_DATA                      (0) /* Use dummy or not */

#define SERVICE_FTOD(d,f)                           CONV_ftod_asm(d, f)

/* Common memory Read & Write, except Raw data log service, it will used difference macro */
#define SERVICE_MEM_PAGE_SIZE                       EPR_DEVICE_PAGESIZE

#define SERVICE_MEM_ITEM_SYS_STATE                  STORAGE_ITEM_SYS_STATE

#define SERVICE_MEM_Read(addr,buff,size)            EPR_Read(addr,buff,size)
#define SERVICE_MEM_Write(addr,buff,size)           EPR_Write(addr,buff,size)
#define SERVICE_MEM_Backup(item)                    STORAGE_Backup(item)

#define SERVICE_MEM_OK                              STORAGE_OK
#define SERVICE_MEM_ERROR                           STORAGE_ERROR

/* Common memory Read & Write, except Raw data log service, it will used difference macro */
#define SERVICE_CONFIG_ITEM_CONFIG                  CONFIG_ITEM_CONFIG
#define SERVICE_CONFIG_ITEM_CALIB                   CONFIG_ITEM_CALIB

#define SERVICE_CONFIG_Read(addr,buff,size)         LVM_Read(addr,buff,size)
#define SERVICE_CONFIG_Write(addr,buff,size)        LVM_Write(addr,buff,size)
#define SERVICE_CONFIG_Backup(item)                 CONFIG_Backup(item)
#define SERVICE_CONFIG_Restore(item)                CONFIG_Restore(item)

#define SERVICE_CONFIG_OK                           CONFIG_OK
#define SERVICE_CONFIG_ERROR                        CONFIG_ERROR

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum service_result_g 
{
    SERVICE_OK  = 0,                        /* Service OK */
    SERVICE_ERROR,                          /* Service Common Error */
    SERVICE_PARAM_ERROR,                    /* Service Parameters Error */
    SERVICE_EXECUTION_ERROR,                /* Service Execution Error */
    SERVICE_REQUEST_SIZE_ERROR,             /* Service Request Size Error */
} service_result_t;

typedef struct tagServiceInterface
{
    uint32_t    addr;                       /* For SFR read/write */
    uint8_t     current_id;
    uint16_t    block_id;
    uint8_t *   p_id_list;
    uint8_t *   p_buff_in;
    uint8_t *   p_buff_out;
    uint8_t *   p_len_in;
    uint8_t *   p_len_out;
    
    uint16_t *  p_req_samples;
    uint16_t *  p_block_per_id;
    uint16_t *  p_total_block;
    
} st_service_param;

/******************************************************************************
Variable Externs
******************************************************************************/
extern raw_data_service_info_t  g_raw_data_service_info;

#if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
extern EM_SW_SAMP_TYPE g_datalog_before_integrate_phase_r_i;
extern EM_SW_SAMP_TYPE g_datalog_before_integrate_phase_y_i;
extern EM_SW_SAMP_TYPE g_datalog_before_integrate_phase_b_i;
extern EM_SW_SAMP_TYPE g_datalog_before_integrate_neutral_i;
#endif

/******************************************************************************
Functions Prototypes
******************************************************************************/

/* Encode and Decode float32_t to/from string buffer */
void SERVICE_EncodeFloat32(float32_t *p_f32, uint8_t *buff);
void SERVICE_DecodeFloat32(float32_t *p_f32, uint8_t *buff);
void SERVICE_EncodeFloat64(double64_t *p_d64, uint8_t *buff);
void SERVICE_DecodeFloat64(double64_t *p_d64, uint8_t *buff);
void SERVICE_EncodeUnsign16(uint16_t *p_u16, uint8_t *buff);
void SERVICE_DecodeUnsign16(uint16_t *p_u16, uint8_t *buff);

/* 0x01 - Read SFR */
service_result_t SERVICE_ReadSFR(st_service_param *p_params);

/* 0x02 - Write SFR */
service_result_t SERVICE_WriteSFR(st_service_param *p_params);

/* 0x03 - EM Read */
service_result_t SERVICE_ReadEMData(st_service_param *p_params);

/* 0x04 - EM Write */
service_result_t SERVICE_WriteEMData(st_service_param *p_params);

/* 0x05 - Do data logging */
service_result_t SERVICE_DoRawDataLogging(st_service_param *p_params);

/* 0x06 - Read calibration data */
service_result_t SERVICE_ReadCalibrationData(st_service_param *p_params);

/* 0x07 - Write calibration data */
service_result_t SERVICE_WriteCalibrationData(st_service_param *p_params);

/* 0x08 - Do auto calibration */
service_result_t SERVICE_DoAutoCalibration(st_service_param *p_params);

/* 0x0F - Read firmware version */
service_result_t SERVICE_ReadFirmwareVersion(
    uint8_t *buff,          /* Pointer of buffer */
    uint8_t len             /* Length of buffer */
);

/* ADC Buffer Fill - by ADC interrupt, rate: fs */
void SERVICE_FillADCRawData(__near EM_SAMPLES * p_samples);

/* Measurement Buffer Fill - by timer interrupt, 40ms interval */
void SERVICE_FillMeasurementRawData(void);

/* Raw Data Log Polling Process */
void SERVICE_RawDataLogPollingProcess(void);

/* Raw Data Log Cancel Bulk Transfer */
void SERVICE_RawDataLogCancelRequest(void);

#endif /* _SERVICE_H */
