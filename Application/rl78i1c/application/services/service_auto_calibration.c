/****************************************************************************** 
* File Name    : service_auto_calibration.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */
#include "r_cg_rtc.h"           /* CG RTC Driver */
#include "r_calib.h"            /* Calibration driver*/

/* Global */
#include "typedef.h"            /* GSCE Typedef */

/* Middleware */
#include "em_core.h"            /* EM Core Library */
/* Wrapper */
#include "wrp_em_sw_config.h"   /* EM Software Config */
#include "wrp_em_adc_data.h"         /* EM Wrapper ADC */     

/* Application */
#include "config_format.h"             /* EEPROM Format */
#include "lvm.h"                /* LVM Driver */
#include "config_storage.h"            /* EEPROM Storage */
#include "service.h"            /* Services */
#include "platform.h"           /* Platform default values */
#include "powermgmt.h"

/* MW/Core, Common */
#include "em_core.h"            /* EM Core APIs */
#include "em_conversion.h"      /* EM Conversion APIs */
#include "em_maths.h"           /* EM Maths APIs */

/* Compiler specific */
#include <string.h>

/******************************************************************************
Macro definitions
******************************************************************************/
#define SERVICE_AUTO_CALIB_PASSWORD         ("SPEMCALIB")

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
//extern void EM_SetVoltageThreshold(void);
extern void EM_SetVoltageThreshold(uint16_t vlow);
extern uint8_t EM_ClearIRMSThreshold(void);
extern uint8_t ClearNoloadActiveReactiveApparentThreshold(void);
/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/******************************************************************************
* Function Name : SERVICE_DoAutoCalibration
* Interface     : uint8_t SERVICE_DoAutoCalibration(
*               :     uint8_t step_id,
*               :     uint8_t *buff_in,
*               :     uint8_t *buff_out,
*               :     uint8_t len_in,
*               :     uint8_t *len_out
*               : );
* Description   : Read Firmware Version Service
* Arguments     : uint8_t step_id   : Step ID of the auto calibration
*               : uint8_t * buff_in : Input buffer
*               : uint8_t * buff_out: Output buffer
*               : uint8_t len_in    : Length of input buffer
*               : uint8_t *len_out  : Store the length of output buffer
* Function Calls: TBD
* Return Value  : uint8_t, success or not
******************************************************************************/
service_result_t SERVICE_DoAutoCalibration(st_service_param *p_params)
{
	uint8_t rlt;                                                    /* Calibration result */
	uint8_t cali_password[] = SERVICE_AUTO_CALIB_PASSWORD;          /* Password to check */
	uint8_t step_id;

	/* Calibration */
	calib_output_t result;                      /* Output data after calibration process */
	EM_CALIBRATION  calib;                      /* Calibrated data storage for EM core */
    
	/* Command parameters */
	float32_t       imax, v, i;                 /* Voltage and Current for calibrating */
	uint16_t        c, cp;                      /* Number of line cycle for calibrating & phase error loop */
	uint8_t         w;                          /* Input supply line */
	dsad_channel_t  channel;                    /* Name the current channel for ADC gain setting */
	uint8_t         is_calibrate_v90;           /* Indicate have timer phase shift offset calibration or not */

	float32_t       degree_list[EM_GAIN_PHASE_NUM_LEVEL_MAX];
	float32_t       gain_list[EM_GAIN_PHASE_NUM_LEVEL_MAX];
	uint8_t         buffer[20];                 /* Data buffer to store input arguments */
	float32_t       f_result;                   /* Temporary data for encoding to output */
	uint8_t         *p_password = NULL;         /* Pointer to password position in buff_in */
	uint8_t         password_len = 0;           /* Length of password in buff_in */
	uint8_t         encoded_len = 0;            /* Length of encoded data */

	/* Step 2 */
	uint8_t         selection = 0;              /* No update */
    
	/* Params check */
	if (p_params->p_id_list == NULL ||
	    p_params->p_buff_in == NULL ||
	    p_params->p_buff_out == NULL ||
	    p_params->p_len_out == NULL ||
	    p_params->p_total_block == NULL
	    )
	{
    return SERVICE_PARAM_ERROR;
	}

	step_id = *p_params->p_id_list;

	memset(&degree_list[0], 0, sizeof(degree_list));
	memset(&gain_list[0], 0, sizeof(gain_list));
    
	switch (step_id)
	{
		case EM_DATA_ID_AUTO_CALIBRATION_STEP1:		// 0x21 - R
		case EM_DATA_ID_AUTO_CALIBRATION_STEP1_2:	// 0x22 - Y
		case EM_DATA_ID_AUTO_CALIBRATION_STEP1_3:	// 0x23	- B
		case EM_DATA_ID_AUTO_CALIBRATION_STEP1_4:	// 0x24	- N
		    
    /* If first calibration, checking for password */
    if (step_id == EM_DATA_ID_AUTO_CALIBRATION_STEP1)
		{
			p_password = p_params->p_buff_in + 28;

			if (memcmp(p_password, cali_password, sizeof(cali_password)) != 0)
			{
				return SERVICE_PARAM_ERROR;
			}
		}
            
		/* Default parameters: GUI not set these information */
		w = CONVERT_ID_TO_CALIB_LINE(step_id);	// ((id & 0x0F)-1)
		c = 150;
		cp = 150;

		/* Get v parameter */
		SERVICE_DecodeFloat32(&v, p_params->p_buff_in);
		SERVICE_DecodeFloat32(&i, p_params->p_buff_in + 4);
		// 8-power factor,12 - ext temp, 16 - freq
		SERVICE_DecodeFloat32(&imax, p_params->p_buff_in + 20);

		/* Stop EM operation */
		if (EM_Stop() != EM_OK)
	  {
	      return SERVICE_EXECUTION_ERROR;
	  }
		/* Get current calib info */
		calib = EM_GetCalibInfo();
            
		/* Doing calibration */
		if (w < LINE_NEUTRAL )	// LINE_NEUTRAL = 3
		{
	    rlt = R_CALIB_CalibrateVoltAndPhase(c, cp, imax, v, i, &result, w);
		}
		else if (w == LINE_NEUTRAL)
		{
	    rlt = R_CALIB_CalibrateVoltAndNeutral(c, cp, imax, v, i, &result);
		}
		else
		{
	    return SERVICE_PARAM_ERROR;
		}

		if (rlt != EM_CALIB_OK)
		{
	    return SERVICE_EXECUTION_ERROR;
		}

		/* Checking calibration result to see whether include timer phase shift offset calibration or not */
		if (result.angle90_offset > 0 || result.angle90_offset < 0)
		{
	    is_calibrate_v90 = 1;
		}
		else
		{
	    is_calibrate_v90 = 0;   
		}
            
    /* Buffer fill: Status */
// The following two lines were in the original code - we are using the next two
// For compatibility with GUI we will need to uncomment these two lines and comment next line
    *p_params->p_buff_out++ = 1;
    encoded_len = 1;

    /* Modify calib info and output depends on step_id */
    degree_list[0]                                              = result.angle_error;
    /* Gain list not support yet: not just get from the ADC gain */
    gain_list[0]                                                = result.gain;
    
    if (step_id == EM_DATA_ID_AUTO_CALIBRATION_STEP1)
    {
      /* Modify the calib info */
      calib.common.sampling_frequency                         = result.fs;
      calib.coeff.phase_r.vrms                                = result.vcoeff;
      calib.coeff.phase_r.irms                                = result.icoeff;
      calib.coeff.phase_r.active_power                        = result.pcoeff;
      calib.coeff.phase_r.reactive_power                      = result.pcoeff;
      calib.coeff.phase_r.apparent_power                      = result.pcoeff;
      calib.sw_phase_correction.phase_r.i_phase_degrees       = degree_list;
      calib.sw_gain.phase_r.i_gain_values                     = gain_list;
      calib.offset.phase_r.v                                  = result.voffset;
      channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_R;
    }
    else if (step_id == EM_DATA_ID_AUTO_CALIBRATION_STEP1_2)
    {
      /* Modify the calib info */
      calib.common.sampling_frequency                         = result.fs;
      calib.coeff.phase_y.vrms                                = result.vcoeff;
      calib.coeff.phase_y.irms                                = result.icoeff;
      calib.coeff.phase_y.active_power                        = result.pcoeff;
      calib.coeff.phase_y.reactive_power                      = result.pcoeff;
      calib.coeff.phase_y.apparent_power                      = result.pcoeff;
      calib.sw_phase_correction.phase_y.i_phase_degrees       = degree_list;
      calib.sw_gain.phase_y.i_gain_values                     = gain_list;
      calib.offset.phase_y.v                                  = result.voffset;
      channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_Y;
    }
    else if (step_id == EM_DATA_ID_AUTO_CALIBRATION_STEP1_3)
    {
      /* Modify the calib info */
      calib.common.sampling_frequency                         = result.fs;
      calib.coeff.phase_b.vrms                                = result.vcoeff;
      calib.coeff.phase_b.irms                                = result.icoeff;
      calib.coeff.phase_b.active_power                        = result.pcoeff;
      calib.coeff.phase_b.reactive_power                      = result.pcoeff;
      calib.coeff.phase_b.apparent_power                      = result.pcoeff;
      calib.sw_phase_correction.phase_b.i_phase_degrees       = degree_list;
      calib.sw_gain.phase_b.i_gain_values                     = gain_list;
      calib.offset.phase_b.v                                  = result.voffset;
      channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_PHASE_B;
    }
    else
    {
      /* Modify the calib info */
      calib.coeff.neutral.irms                                = result.icoeff;
      channel                                                 = EM_ADC_CURRENT_DRIVER_CHANNEL_NEUTRAL;
    }
//            #if (SERVICE_USE_DUMMY_DATA == 1)	// This option has been removed from this code - Refer to SourceV106 folder if needed
            
			// Neutral or Phase channel? 
    if (step_id == EM_DATA_ID_AUTO_CALIBRATION_STEP1_4)
    {
      f_result = result.icoeff;                           //Phase1 V-coeff
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 0 );    // 4 Bytes, float32    P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = result.gain;                             //Phase1 V-ADC gain
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 4 );    // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          

      encoded_len += 8;
    }
    else
    {
      f_result = result.fs;                           //Phase1 V-coeff
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 0 );    // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = result.gain;                             //Phase1 V-ADC gain
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 4 );    // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result =  result.vcoeff;                           //Phase1 V-Phase coeff
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 8 );    // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = result.icoeff;                      //Phase1 V-Phase shift
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 12 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = result.pcoeff;                                    //Phase1 V- Software gain
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 16 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = result.angle_error;                           //Phase1 I-coeff
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 20 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      if (is_calibrate_v90)
      {
          f_result = result.angle90_offset;
      }
      else
      {
          f_result = 0.0f;
      }
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 24 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = result.voffset;                                    //Phase1 I-Phase coeff
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 28 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = 0.0f;                                    //Phase1 I-Phase shift
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 32 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      f_result = 0.0f;                                   //Phase1 I- Software gai
      SERVICE_EncodeFloat32(&f_result, p_params->p_buff_out + 36 );   // 4 Bytes, float32     P1-VRMS-coeff   P1 Voltage RMS coefficient                          
      encoded_len += 10 * sizeof(float32_t);
    }
            
    p_params->p_buff_out += encoded_len;
    *p_params->p_len_out += encoded_len;
    
    
    /* Buffer fill: Message */
    *p_params->p_buff_out++ = 'O';
    *p_params->p_buff_out++ = 'K';
    encoded_len += 2;
    *p_params->p_len_out = encoded_len;
            
    /* Timer phase shift offset values */
    if (is_calibrate_v90 == 1)
    {
      EM_ADC_SAMP_SetTimerOffset((int16_t)result.angle90_offset);
    }
    
    /* Set the calib info to library */
		
		calib.common.meter_constant_phase=EM_CALIB_DEFAULT_METER_CONSTANT_PHASE;
		calib.common.meter_constant_total=EM_CALIB_DEFAULT_METER_CONSTANT_TOTAL;
		
    if (EM_SetCalibInfo(&calib) != EM_OK)
    {
      return SERVICE_EXECUTION_ERROR;
		}

// the following line is being commented and then uncommented	- 09/Sep/2020	
//		EM_SetVoltageThreshold(1);
		EM_SetVoltageThreshold(4);	// 19/Nov/2020
		
		ClearNoloadActiveReactiveApparentThreshold();
		EM_ClearIRMSThreshold();
		
    /* Driver DSAD Gain */
    R_DSADC_SetChannelGain(channel,R_DSADC_GetGainEnumValue((uint8_t)result.gain));
            
    /* Finish, now restart EM operation */
    if (POWERMGMT_IsACLow() == 0 )
    {
      if (EM_Start() != EM_OK)
      {
        return SERVICE_EXECUTION_ERROR;
      }
    }
    break;
            
    case EM_DATA_ID_AUTO_CALIBRATION_STEP2:	// 0x02
    // Get selection from user 
    selection = *p_params->p_buff_in++;
            
    if (selection != 0 && selection != 1)
    {
      return SERVICE_PARAM_ERROR;
    }
            
    // Update EEPROM when user want to update 
    if (selection == 1)
    {
      if (SERVICE_CONFIG_Backup(SERVICE_CONFIG_ITEM_CALIB) == SERVICE_CONFIG_OK)
      {// Success 
        *p_params->p_buff_out = 0;
      }
      else
      {// Error 
        *p_params->p_buff_out = 1;
      }
    }
    else
    {// No update 
      *p_params->p_buff_out = 2;
    }
    encoded_len = 1;
    *p_params->p_len_out = encoded_len;
    break;
            
    default:
        return SERVICE_PARAM_ERROR;
    }
    
    /* Successful */
    return SERVICE_OK;
    
}
