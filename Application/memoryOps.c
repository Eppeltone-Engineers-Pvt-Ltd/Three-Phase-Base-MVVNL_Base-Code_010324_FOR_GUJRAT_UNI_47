// memory and utility functions
#include <stdio.h>

#include "r_cg_macrodriver.h"       /* CG Macro Driver */

#include "typedef.h"            // located right at top of tree below iodefine.h
#include "r_cg_wdt.h"
#include "em_core.h"
#include "platform.h"
#include "wrp_mcu.h"            // for MCU_Delay function

#include "r_cg_dsadc.h"
#include "r_cg_rtc.h"           /* CG RTC Driver */

#include "r_dlms_data.h"

#include "wrp_em_adc_data.h"


#include "config_storage.h"            // Storage Header File
#include "emeter3_structs.h"
#include "em_display.h"  
#include "memoryOps.h"
#include "emeter3_app.h"
#include "eeprom.h"
#include "e2rom.h"	// pravak implementation
#include "pravakComm.h"
#include "utilities.h"
#include "tampers.h"
#include "r_dlms_data.h"	// for ConvertRTCDate function
#include "em_display_TN3P_Helper.h"
#include "calendarFunctions.h"
#include "tamperTNEB.h"
#include "survey.h"
#include "tod.h"

/* Standard Lib */
#include <string.h>						/* String Standard Lib - included for memcpy function */


#define _BV(i)	(1<<i)

//extern void	EM_SetVoltageThreshold(void);	// this function will set the threshold value - added on 12/Jun/2020
extern void EM_SetVoltageThreshold(uint16_t vlow);	// this function allows the low voltage value to be passed
extern uint8_t EM_ClearIRMSThreshold(void);
extern uint8_t ClearNoloadActiveReactiveApparentThreshold(void);
extern void LCD_ClearAll(void);

extern uint32_t g_Class07_Billing_EntriesInUse;
extern uint16_t g_Class07_Billing_CurrentEntry;

extern uint16_t g_Class07_Event_CurrentEntry[];
extern uint32_t g_Class07_Event_EntriesInUse[];

extern date_time_t		g_Class03_BillingDate;

extern class07_billing_entry_t		g_Class07_BillingBuffer;

extern time_t TZ_start_time0[];
extern time_t TZ_start_time1[];

extern date_time_t g_Class20_ac_time;
extern time_t g_Class22_billtime;
extern date_t g_Class22_billdate;

//extern volatile rtc_timecapture_value_t g_rtc_timestamp1;	// this is for cover interrupt - not used anymore

/*
void do_survey_update(void);
void do_log_update(void);
void computeLoadSurveyEntries(uint8_t hr, uint8_t min);
void computeDailyLogEntries(void);
void writeLogData(uint8_t sdate,uint8_t smonth,uint8_t syear, uint8_t shour, uint8_t sminute, uint8_t ssecond);
*/

void initialiseCalibValues(EM_CALIBRATION *ptr);
void initialiseListItemValues(list_items_t* ptr);

void computeNextBillDate(void);	// implemented in emeter3_app.c


uint8_t perform_E2ROM12_Test(void)
{// This function checks the functioning of the ICs at Bank 1 and 2
// It uses page address 1 and 2 and two bytes are written at address 0000 and 0001
// returns 0 for success and 1 for NVM failure

	uint32_t address;
	uint8_t msgbuf[2];
	uint8_t readbuf[2];
	uint8_t savebuf[2];
	int8_t success;
	

	success = 0;
	
	msgbuf[0]='H';
	msgbuf[1]='i';
	
	address = 0x10000;
	EPR_Read(address, (uint8_t*)&savebuf, 2);	// read two bytes and save them
	EPR_Write(address, (uint8_t*)&msgbuf, 2);	// write two bytes
	EPR_Read(address, (uint8_t*)&readbuf, 2);	// read two bytes
	
	if((readbuf[0]==msgbuf[0])&&(readbuf[1]==msgbuf[1]))
	{
		EPR_Write(address, (uint8_t*)&savebuf, 2);	// restore two bytes
		success++;
	}

	address = 0x20000;
	msgbuf[0]='h';
	msgbuf[1]='I';
	
	EPR_Read(address, (uint8_t*)&savebuf, 2);	// read two bytes and save them
	EPR_Write(address, (uint8_t*)&msgbuf, 2);	// write two bytes
	EPR_Read(address, (uint8_t*)&readbuf, 2);	// read two bytes
	
	if((readbuf[0]==msgbuf[0])&&(readbuf[1]==msgbuf[1]))
	{
		EPR_Write(address, (uint8_t*)&savebuf, 2);	// restore two bytes
		success++;
	}
	
	if(success==2)
		return 0;
	else
		return 1;
	
}

uint8_t perform_E2ROM_Test(uint8_t test_mode)
{
	uint8_t msgbuf[8];
	uint8_t readbuf[8];
	
	uint8_t i;
	uint8_t result;
	
	msgbuf[0]='P';
	msgbuf[1]='R';
	msgbuf[2]='A';
	msgbuf[3]='V';
	msgbuf[4]='A';
	msgbuf[5]='K';

	EPR_Read(E2ROM_TEST_ADDRESS, (uint8_t*)&readbuf, 6);	// read six bytes
	
	result = 0;
	
	for(i=0;i<6;i++)
	{
		if(msgbuf[i]!=readbuf[i])
			result =1;
	}
	
	if(result==1)
	{// either there was an error or the message had not been written
		if(test_mode==1)
		{// perform a write
			EPR_Write(E2ROM_TEST_ADDRESS, (uint8_t*)&msgbuf, 6);	// read six bytes
		}
	}
	
	return result;
}

void initialiseCalibValues(EM_CALIBRATION *ptr)
{
// init the common parameters	
	ptr->common.sampling_frequency = EM_CALIB_DEFAULT_SAMPLING_FREQUENCY;
	ptr->common.meter_constant_phase = EM_CALIB_DEFAULT_METER_CONSTANT_PHASE;
	ptr->common.meter_constant_total = EM_CALIB_DEFAULT_METER_CONSTANT_TOTAL;
	ptr->common.pulse_on_time = EM_CALIB_DEFAULT_PULSE_ON_TIME;
	ptr->common.rtc_comp_offset = EM_CALIB_DEFAULT_RTC_COMP_OFFSET;
	
// init the coeffs	
	ptr->coeff.phase_r.vrms = EM_CALIB_DEFAULT_COEFF_VRMS_PHASE_R;
	ptr->coeff.phase_r.irms = EM_CALIB_DEFAULT_COEFF_IRMS_PHASE_R;
	ptr->coeff.phase_r.active_power = EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER_PHASE_R;
	ptr->coeff.phase_r.reactive_power = EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER_PHASE_R;
	ptr->coeff.phase_r.apparent_power = EM_CALIB_DEFAULT_COEFF_APPARENT_POWER_PHASE_R;

	ptr->coeff.phase_y.vrms = EM_CALIB_DEFAULT_COEFF_VRMS_PHASE_Y;
	ptr->coeff.phase_y.irms = EM_CALIB_DEFAULT_COEFF_IRMS_PHASE_Y;
	ptr->coeff.phase_y.active_power = EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER_PHASE_Y;
	ptr->coeff.phase_y.reactive_power = EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER_PHASE_Y;
	ptr->coeff.phase_y.apparent_power = EM_CALIB_DEFAULT_COEFF_APPARENT_POWER_PHASE_Y;

	ptr->coeff.phase_b.vrms = EM_CALIB_DEFAULT_COEFF_VRMS_PHASE_B;
	ptr->coeff.phase_b.irms = EM_CALIB_DEFAULT_COEFF_IRMS_PHASE_B;
	ptr->coeff.phase_b.active_power = EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER_PHASE_B;
	ptr->coeff.phase_b.reactive_power = EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER_PHASE_B;
	ptr->coeff.phase_b.apparent_power = EM_CALIB_DEFAULT_COEFF_APPARENT_POWER_PHASE_B;

	ptr->coeff.neutral.irms = EM_CALIB_DEFAULT_COEFF_IRMS_NEUTRAL;
	
// init the offsets	
	ptr->offset.phase_r.v = EM_CALIB_DEFAULT_OFFSET_V_PHASE_R;	
	ptr->offset.phase_r.i = EM_CALIB_DEFAULT_OFFSET_I_PHASE_R;	
	
	ptr->offset.phase_y.v = EM_CALIB_DEFAULT_OFFSET_V_PHASE_Y;	
	ptr->offset.phase_y.i = EM_CALIB_DEFAULT_OFFSET_I_PHASE_Y;	
	
	ptr->offset.phase_b.v = EM_CALIB_DEFAULT_OFFSET_V_PHASE_B;	
	ptr->offset.phase_b.i = EM_CALIB_DEFAULT_OFFSET_I_PHASE_B;

	ptr->sw_phase_correction.phase_r.i_phase_degrees=NULL;
	ptr->sw_phase_correction.phase_y.i_phase_degrees=NULL;
	ptr->sw_phase_correction.phase_b.i_phase_degrees=NULL;
	
	ptr->sw_gain.phase_r.i_gain_values=NULL;
	ptr->sw_gain.phase_y.i_gain_values=NULL;
	ptr->sw_gain.phase_b.i_gain_values=NULL;
}

void initialiseListItemValues(list_items_t* ptr)
{
	ptr->degree_list_r[0]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT_PHASE_R;
	ptr->degree_list_r[1]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT_PHASE_R;

	ptr->degree_list_y[0]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT_PHASE_Y;
	ptr->degree_list_y[1]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT_PHASE_Y;

	ptr->degree_list_b[0]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT_PHASE_B;
	ptr->degree_list_b[1]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT_PHASE_B;
	
	ptr->gain_list_r[0]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE_PHASE_R;
	ptr->gain_list_r[1]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE_PHASE_R;
	
	ptr->gain_list_y[0]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE_PHASE_Y;
	ptr->gain_list_y[1]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE_PHASE_Y;
	
	ptr->gain_list_b[0]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE_PHASE_B;
	ptr->gain_list_b[1]=EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE_PHASE_B;
	
	ptr->driver_adc_gain0[0]=0;
	ptr->driver_adc_gain0[1]=0;
	
	ptr->driver_adc_gain1[0]=0;
	ptr->driver_adc_gain1[1]=0;

	ptr->driver_timer_offset = EM_CALIB_DEFAULT_TIMER_OFFSET;
}


uint8_t SaveCalibrationValues(void)
{
	list_items_t listItems;
  EM_CALIBRATION  calib;
  dsad_reg_setting_t regs;
	float32_t* floatptr;
	
  // Get EM Core calib  
  calib = EM_GetCalibInfo();
	// Upon return from above function the calib structure pointers will be pointing to the degrees_lists and the gain_lists
	// we will need to populate the listItems structure so that everything can be saved in one shot
	
	write_alternately(CALIB_BASE_ADDRESS, ALTERNATE_CALIB_BASE_ADDRESS, (uint8_t*)&calib, sizeof(calib));

// get the degrees list into the listItems	
	floatptr = calib.sw_phase_correction.phase_r.i_phase_degrees;
	listItems.degree_list_r[0]= *floatptr;
	floatptr++;
	listItems.degree_list_r[1]= *floatptr;
	
	floatptr = calib.sw_phase_correction.phase_y.i_phase_degrees;
	listItems.degree_list_y[0]= *floatptr;
	floatptr++;
	listItems.degree_list_y[1]= *floatptr;
	
	floatptr = calib.sw_phase_correction.phase_b.i_phase_degrees;
	listItems.degree_list_b[0]= *floatptr;
	floatptr++;
	listItems.degree_list_b[1]= *floatptr;
	

// get the gain lists into the listItems	
	floatptr = calib.sw_gain.phase_r.i_gain_values;
	listItems.gain_list_r[0]= *floatptr;
	floatptr++;
	listItems.gain_list_r[1]= *floatptr;

	floatptr = calib.sw_gain.phase_y.i_gain_values;
	listItems.gain_list_y[0]= *floatptr;
	floatptr++;
	listItems.gain_list_y[1]= *floatptr;

	floatptr = calib.sw_gain.phase_b.i_gain_values;
	listItems.gain_list_b[0]= *floatptr;
	floatptr++;
	listItems.gain_list_b[1]= *floatptr;

	
  R_DSADC_GetGainAndPhase(&regs);
  
  listItems.driver_adc_gain0[0] = regs.gain0;
  listItems.driver_adc_gain0[1] = 0;
  listItems.driver_adc_gain1[0] = regs.gain1;
  listItems.driver_adc_gain1[1] = 0;
	
  listItems.driver_timer_offset = EM_ADC_SAMP_GetTimerOffset();
	
// We now have all the items in listItems

	write_alternately(LIST_ITEMS_BASE_ADDRESS, ALTERNATE_LIST_ITEMS_BASE_ADDRESS, (uint8_t*)&listItems, sizeof(listItems));

	return CONFIG_OK;
}

uint8_t LoadCalibrationValues(void)
{	
	list_items_t listItems;
  EM_CALIBRATION  calib;
  dsad_reg_setting_t regs;
				
// first read the calib structure from e2rom or initialise
  if(read_alternately(CALIB_BASE_ADDRESS, ALTERNATE_CALIB_BASE_ADDRESS, (uint8_t*)&calib, sizeof(calib))==0)
	{// e2rom does not contain proper calib data
		initialiseCalibValues(&calib);
	}	
	
// now read the listItems structure
  if(read_alternately(LIST_ITEMS_BASE_ADDRESS, ALTERNATE_LIST_ITEMS_BASE_ADDRESS, (uint8_t*)&listItems, sizeof(listItems))==0)
	{// e2rom does not contain proper listItems
		initialiseListItemValues(&listItems);
	}

  calib.sw_phase_correction.phase_r.i_phase_degrees = &(listItems.degree_list_r[0]);
  calib.sw_phase_correction.phase_y.i_phase_degrees = &(listItems.degree_list_y[0]);
  calib.sw_phase_correction.phase_b.i_phase_degrees = &(listItems.degree_list_b[0]);

  calib.sw_gain.phase_r.i_gain_values = &(listItems.gain_list_r[0]);
  calib.sw_gain.phase_y.i_gain_values = &(listItems.gain_list_y[0]);
  calib.sw_gain.phase_b.i_gain_values = &(listItems.gain_list_b[0]);
	
  // Set ADC Gain value into driver  
  regs.gain0 = listItems.driver_adc_gain0[0];
  regs.gain1 = listItems.driver_adc_gain1[0];
  R_DSADC_SetGain(regs);
      
  // Set timer offset into wrapper  
  EM_ADC_SAMP_SetTimerOffset(listItems.driver_timer_offset);
      
  // Set to EM Core  
  if (EM_SetCalibInfo(&calib) != EM_OK)
  {
      return CONFIG_ERROR_DATA_CORRUPT;  // Data corrupt  
  }	
// the following line is being commented and then uncommented	- 09/Sep/2020	
//	EM_SetVoltageThreshold(1);	// this function will set the threshold value
	EM_SetVoltageThreshold(4);	// 19/Nov/2020
  ClearNoloadActiveReactiveApparentThreshold();
	EM_ClearIRMSThreshold();
	return CONFIG_OK;
}

void GetManufacturersName(char *ptr)
{
	sprintf(ptr, "EEPL");
}

uint8_t read_alternately(uint32_t ad, uint32_t alternate_ad, uint8_t* arr, uint16_t rec_size)
{
	uint8_t retval=1;  // success
	R_WDT_Restart();
	EPR_Read(ad, arr, rec_size);
	if(isCheckSumOK(arr,rec_size)==0)
	{// first checksum is not ok
		R_WDT_Restart();
		EPR_Read(alternate_ad, arr, rec_size);
		if(isCheckSumOK(arr,rec_size)==0) 
			retval=0; // second read has also failed
	}
	return retval;
}

void write_alternately(uint32_t ad, uint32_t alternate_ad, uint8_t* arr, uint16_t rec_size)
{// in this function two writes are performed - first to the alternate address and then to the main address
// we could compute the check sum also
	arr[rec_size-1]=calcChecksum(arr,rec_size-1);
	R_WDT_Restart();
	EPR_Write(alternate_ad,arr,rec_size);
	R_WDT_Restart();
	EPR_Write(ad,arr,rec_size);
}

int8_t validate_ctl_data(void)
{// returns 1 if ctldata is good, 0 if bad, 99 if ctldata_backup is itself corrupted
	ctldataBackup_t ctldata_backup;
	timeinfo_t first;	// ctldata timeinfo
	timeinfo_t second;	// backup timeinfo
	
	int8_t retval=1; 	// success data is valid	
	R_WDT_Restart();
	EPR_Read(CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup));
  if(isCheckSumOK((uint8_t*)&ctldata_backup,sizeof(ctldata_backup))==0)
	{// here checksum is not OK
		EPR_Read(ALTERNATE_CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup));
  	if(isCheckSumOK((uint8_t*)&ctldata_backup,sizeof(ctldata_backup))==0)
		{// here too the checksum is not ok
			retval=99;
		}
	}
	
	if(retval==99)
		return retval;
	else
	{// perform data validation
		if(ctldata.powerOnminutes_eb>=ctldata_backup.powerOnminutes_eb)	// ctldata.powerOnminutes_eb - OK
		{
			if(ctldata.cumulative_md>=ctldata_backup.cumulative_md)
			{
				if(ctldata.cumulative_md_kva>=ctldata_backup.cumulative_md_kva)
				{
					first.date = ctldata.date;
					first.month = ctldata.month;
					first.year = ctldata.year;
					first.hour = ctldata.hour;
					first.minute = ctldata.minute;
					
					second.date = ctldata_backup.date;
					second.month = ctldata_backup.month;
					second.year = ctldata_backup.year;
					second.hour = ctldata_backup.hour;
					second.minute = ctldata_backup.minute;
					
					if(isLaterDateTime(first, second)>=0)
					{
						retval=1;	// success 
					}
					else
						retval=0;
				}
				else
					retval=0;
			}
			else
				retval=0;
		}
		else
			retval=0;
	}// else of perform validation
	return retval;
}

void update_ctl_backup_data(void)
{
// this function is used to update the values of cumulative_mds after a bill has been made
// we will read the backup data from memory, make the necessary changes and then write it back
	ctldataBackup_t ctldata_backup;
	
  if(read_alternately(CTLDATA_BACKUP_ADDRESS, ALTERNATE_CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup))==1)
  {// we have good backup data
	// change the md values
		ctldata_backup.cumulative_md = ctldata.cumulative_md;
		ctldata_backup.cumulative_md_kva = ctldata.cumulative_md_kva;
		
	#ifdef BIDIR_METER	
		ctldata_backup.cumulative_md_export = ctldata.cumulative_md_export;
		ctldata_backup.cumulative_md_kva_export = ctldata.cumulative_md_kva_export;
	#endif
	}
	else
	{// here the backup copy is itself bad
		ctldata_backup.powerOnminutes_eb = ctldata.powerOnminutes_eb;	// ctldata.powerOnminutes_eb - OK
		ctldata_backup.cumulative_md = ctldata.cumulative_md;
		ctldata_backup.cumulative_md_kva = ctldata.cumulative_md_kva;
		
	#ifdef BIDIR_METER	
		ctldata_backup.cumulative_md_export = ctldata.cumulative_md_export;
		ctldata_backup.cumulative_md_kva_export = ctldata.cumulative_md_kva_export;
	#endif
	
  	ctldata_backup.survey_wrt_ptr = SURVEY_BASE_ADDRESS;	// safe option
		ctldata_backup.energies_wrt_ptr = ctldata.energies_wrt_ptr;	// safe
		ctldata_backup.daily_log_wrt_ptr = ctldata.daily_log_wrt_ptr;	// safe
	
		ctldata_backup.date = ctldata.date;
		ctldata_backup.month = ctldata.month;
		ctldata_backup.year = ctldata.year;
		ctldata_backup.hour = ctldata.hour;
		ctldata_backup.minute = ctldata.minute;
		ctldata_backup.overflow = ctldata.overflow;
	}
 	ctldata_backup.chksum = calcChecksum((uint8_t*)&ctldata_backup,sizeof(ctldata_backup)-1);

	R_WDT_Restart();
	EPR_Write(CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup));
	
	R_WDT_Restart();
	EPR_Write(ALTERNATE_CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup));
}

void write_ctl_backup_data(int8_t type)
{
// Since the backup is expected to be done at 00:00 hours even when it is not done
// at 00:00 hours, it may be advised to align the survey_wrt_ptr value to the
// day's boundary value (Food for thought?)

	
// This function will get called at 00:00 hours normally (type = 0)
// However, it will also be called when the meter has been zapped (type = 1)
	
// with the help of this backup we will be able to recover ctldata if all is lost
// without too much damage
// however the energies should be read before init_read_ctl_data
// This is so that all the last save energies in ctldata can be equated to the present energies
// This will create absolutely minimum damage
// The actual value of ctldata.survey_wrt_ptr will need to be computed as per the following formula

//ctldata.survey_wrt_ptr=ctldata_backup.survey_wrt_ptr+(((rtc.hour)*60 + rtc.minute)/Common_Data.survey_interval)*SURVEY_PTR_INC;
//ctldata.daily_log_wrt_ptr = ctldata_backup.daily_log_wrt_ptr;
// the energies_wrt_ptr can be restored by reading energies from e2rom

	ctldataBackup_t ctldata_backup;
	
	ctldata_backup.powerOnminutes_eb = ctldata.powerOnminutes_eb;	// ctldata.powerOnminutes_eb - OK
	ctldata_backup.cumulative_md = ctldata.cumulative_md;
	ctldata_backup.cumulative_md_kva = ctldata.cumulative_md_kva;
	
#ifdef BIDIR_METER	
	ctldata_backup.cumulative_md_export = ctldata.cumulative_md_export;
	ctldata_backup.cumulative_md_kva_export= ctldata.cumulative_md_kva_export;
#endif
	if(type==0)
  	ctldata_backup.survey_wrt_ptr = ctldata.survey_wrt_ptr;	// when called at 00:00 hours
	else
  	ctldata_backup.survey_wrt_ptr = SURVEY_BASE_ADDRESS;	// when called after meter is zapped
	
	ctldata_backup.energies_wrt_ptr = ctldata.energies_wrt_ptr; // not sure about this one
	ctldata_backup.daily_log_wrt_ptr = ctldata.daily_log_wrt_ptr;
	
	ctldata_backup.date = ctldata.date;
	ctldata_backup.month = ctldata.month;
	ctldata_backup.year = ctldata.year;
	ctldata_backup.hour = ctldata.hour;
	ctldata_backup.minute = ctldata.minute;
	ctldata_backup.overflow = ctldata.overflow;
	
  ctldata_backup.chksum = calcChecksum((uint8_t*)&ctldata_backup,sizeof(ctldata_backup)-1);
// we can now save two copies of this data	

	R_WDT_Restart();
	EPR_Write(CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup));
	
	R_WDT_Restart();
	EPR_Write(ALTERNATE_CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup));

}

void write_ctl_data(void)
{
	ctldata_t temp_ctl_data;

// the checksum must have been computed before performing this operation	
// alternately for safety	
  if(isCheckSumOK((uint8_t*)&ctldata,sizeof(ctldata))==0)
	{// here checksum is NOT OK - why oh why
	// should be perform a data validity check before doing this - may be later
		ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
	}

// here the checksum is ok

	R_WDT_Restart();
	EPR_Write(CTLDATA_BASE_ADDRESS, (uint8_t*)&ctldata, sizeof(ctldata));
	
// wait for additional 5 ms
  MCU_Delay(5000);	// this function generates delay of 5 ms
// now read the data	
	EPR_Read(CTLDATA_BASE_ADDRESS, (uint8_t*)&temp_ctl_data, sizeof(temp_ctl_data));
	
	if(1==compareStructs((uint8_t*)&ctldata,(uint8_t*)&temp_ctl_data,sizeof(temp_ctl_data)))
	{// here the data has been written correctly
	// repeat for the second write
		R_WDT_Restart();
		EPR_Write(ALTERNATE_CTLDATA_BASE_ADDRESS, (uint8_t*)&ctldata, sizeof(ctldata));
  	MCU_Delay(5000);	// this function generates delay of 5 ms
		EPR_Read(ALTERNATE_CTLDATA_BASE_ADDRESS, (uint8_t*)&temp_ctl_data, sizeof(temp_ctl_data));
		
		if(1==compareStructs((uint8_t*)&ctldata,(uint8_t*)&temp_ctl_data,sizeof(temp_ctl_data)))
		{// here the data has been written correctly - both writes are ok - we can clear the flag
  		e2write_flags&=~E2_W_CTLDATA;
		}
		else		
			e2write_flags|=E2_W_CTLDATA;	// keep the matter pending
	}
	else
		e2write_flags|=E2_W_CTLDATA;	// keep the matter pending

// the write_ctl_data is considered complete only after both locations are read and matched 
}

void write_common_data(void)
{
	commondata_t temp_common_data;
	
// the checksum must have been computed before performing this operation	
// alternately for safety	
  if(isCheckSumOK((uint8_t*)&Common_Data,sizeof(Common_Data))==0)
	{// here checksum is NOT OK - why oh why
	// should be perform a data validity check before doing this - may be later
		Common_Data.chksum=calcChecksum((uint8_t*)&Common_Data,sizeof(Common_Data)-1);
	}

// here the checksum is ok

	R_WDT_Restart();
	EPR_Write(COMMON_DATA_ADDRESS, (uint8_t*)&Common_Data, sizeof(Common_Data));
	
// wait for additional 5 ms
  MCU_Delay(5000);	// this function generates delay of 5 ms
// now read the data	
	EPR_Read(COMMON_DATA_ADDRESS, (uint8_t*)&temp_common_data, sizeof(temp_common_data));
	
	if(1==compareStructs((uint8_t*)&Common_Data,(uint8_t*)&temp_common_data,sizeof(temp_common_data)))
	{// here the data has been written correctly
	// repeat for the second write
		R_WDT_Restart();
		EPR_Write(ALTERNATE_COMMON_DATA_ADDRESS, (uint8_t*)&Common_Data, sizeof(Common_Data));
  	MCU_Delay(5000);	// this function generates delay of 5 ms
		EPR_Read(ALTERNATE_COMMON_DATA_ADDRESS, (uint8_t*)&temp_common_data, sizeof(temp_common_data));
		
		if(1==compareStructs((uint8_t*)&Common_Data,(uint8_t*)&temp_common_data,sizeof(temp_common_data)))
		{// here the data has been written correctly - both writes are ok - we can clear the flag
  		e2write_flags&=~E2_W_IMPDATA;
		}
		else		
			e2write_flags|=E2_W_IMPDATA;	// keep the matter pending
	}
	else
		e2write_flags|=E2_W_IMPDATA;	// keep the matter pending

// the write_common_data is considered complete only after both locations are read and matched 
}
/*
// these functions are incorrect implementations and must NOT be used
// now monthly figures are available in ctldata.powerOnminutes_eb
int32_t getPreviousPowerOnMinutes(void)
{// this function returns the power on minutes in the last bill
	int32_t retval;
	uint32_t addr;
	int32_t offset;
	uint8_t tmonth;

#if (defined(TARIFFS) && TARIFFS == 5)
	offset = 264; // for power on minutes = 7*4 + 3*12 + 16*TARIFFS + 2*12*TARIFFS = 64 + 40*TARIFFS - OK
#else
	offset = 384;
#endif	
	
	if(Common_Data.bpcount!=0)
	{// here we have an earlier bill	
  	tmonth = decrementMonth(1);	// latest bill
		addr=(int32_t)BILLDATA_BASE_ADDRESS + offset + (int32_t)(tmonth-1)*(int32_t)BILL_PTR_INC;			
		EPR_Read(addr, (uint8_t*)&retval, 4);
	}
	else
		retval=0;
		
	return retval;
}

int32_t getMonthlyPowerOnMinutes(void)
{
	int32_t monthlyPonMinutes;

	monthlyPonMinutes = getPreviousPowerOnMinutes();
	
	monthlyPonMinutes = (myenergydata.powerOnminutes_eb - monthlyPonMinutes);
	if(monthlyPonMinutes<0)
		monthlyPonMinutes=0;

	return monthlyPonMinutes;
}
*/
void verifyTOD_energies(uint8_t zone)
{
// the sum of the the energies in the toddata in the memory plus the sum of the mytoddata must equal the cumulative energies in myenergydata
  toddata_t tempTodData;
	int32_t	cum_kwh;
	int32_t	cum_kvah;
#ifdef TOD_HAS_KVARH_LAG
	int32_t	cum_kvarh_lag;		
#endif
#ifdef BIDIR_METER
	int32_t	cum_kwh_export;
	int32_t	cum_kvah_export;
#endif
  uint8_t z;

	cum_kwh = mytoddata.kwh;
	cum_kvah = mytoddata.kvah;
#ifdef TOD_HAS_KVARH_LAG
	cum_kvarh_lag=mytoddata.kvarh_lag;		
#endif
#ifdef BIDIR_METER
	cum_kwh_export = mytoddata.kwh_export;
	cum_kvah_export = mytoddata.kvah_export;
#endif

  for(z=0;z<TARIFFS;z++)
  {
		if(z!=zone)
		{
	    if(read_alternately((int32_t)TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC, (int32_t)ALTERNATE_TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC, (uint8_t*)&tempTodData, sizeof(tempTodData))==0)
	    {// function has failed- the alternate address's values are corrupted
	      clear_structure((uint8_t*)&tempTodData,sizeof(tempTodData)); // this function fills zero bytes in the structure
			}
			
			cum_kwh += tempTodData.kwh;
			cum_kvah +=	tempTodData.kvah;		
			
#ifdef TOD_HAS_KVARH_LAG
			cum_kvarh_lag += tempTodData.kvarh_lag;
#endif
	
#ifdef BIDIR_METER
			cum_kwh_export += tempTodData.kwh_export;
			cum_kvah_export += tempTodData.kwh_export;
#endif
		}
  }

	z = 0;	// we will use this to record any change
// now we can compare the sum of the tod energies with the energies in the meter and reconcile
	if(cum_kwh > myenergydata.kwh)
	{
		myenergydata.kwh = cum_kwh;
		z++;
	}
		
	if(cum_kvah > myenergydata.kvah)
	{
		myenergydata.kvah = cum_kvah;
		z++;
	}
	
#ifdef TOD_HAS_KVARH_LAG
	if(cum_kvarh_lag > myenergydata.kvarh_lag)
	{
		myenergydata.kvarh_lag = cum_kvarh_lag;
		z++;
	}
#endif

#ifdef BIDIR_METER
	if(cum_kwh_export > myenergydata.kwh_export)
	{
		myenergydata.kwh_export = cum_kwh_export;
		z++;
	}
		
	if(cum_kvah_export > myenergydata.kvah_export)
	{
		myenergydata.kvah_export = cum_kvah_export;
		z++;
	}
#endif
	if(z!=0)
  	myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);

}


void recoverTOD_energies(void);	// private function

void recoverTOD_energies(void)
{
// we must ensure that the sum of the zone energies must equal the total energies
  toddata_t tempTodData;
	int32_t	cum_kwh;
	int32_t	cum_kvah;
#ifdef TOD_HAS_KVARH_LAG
	int32_t	cum_kvarh_lag;		
#endif
#ifdef BIDIR_METER
	int32_t	cum_kwh_export;
	int32_t	cum_kvah_export;
#endif
  uint8_t z;

	cum_kwh = 0;
	cum_kvah = 0;
#ifdef TOD_HAS_KVARH_LAG
	cum_kvarh_lag=0;		
#endif
#ifdef BIDIR_METER
	cum_kwh_export = 0;
	cum_kvah_export = 0;
#endif

  for(z=0;z<TARIFFS;z++)
  {
    if(read_alternately((int32_t)TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC, (int32_t)ALTERNATE_TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC, (uint8_t*)&tempTodData, sizeof(tempTodData))==0)
    {// function has failed- the alternate address's values are corrupted
      clear_structure((uint8_t*)&tempTodData,sizeof(tempTodData)); // this function fills zero bytes in the structure
		}
		
		if(tempTodData.kwh < g_Class07_BillingBuffer.active_energyTZ[z])
			tempTodData.kwh = g_Class07_BillingBuffer.active_energyTZ[z];
			
		cum_kwh += tempTodData.kwh;
		
		if(tempTodData.kvah < g_Class07_BillingBuffer.apparent_energyTZ[z])	
			tempTodData.kvah = g_Class07_BillingBuffer.apparent_energyTZ[z];
			
		cum_kvah +=	tempTodData.kvah;		
			
#ifdef TOD_HAS_KVARH_LAG
		if(tempTodData.kvarh_lag < g_Class07_BillingBuffer.reactive_energy_lagTZ[z])	
			tempTodData.kvarh_lag = g_Class07_BillingBuffer.reactive_energy_lagTZ[z];
			
		cum_kvarh_lag += tempTodData.kvarh_lag;
#endif
	
#ifdef BIDIR_METER
		if(tempTodData.kwh_export < g_Class07_BillingBuffer.active_energyTZ_export[z])
			tempTodData.kwh_export = g_Class07_BillingBuffer.active_energyTZ_export[z];
		
		cum_kwh_export += tempTodData.kwh_export;
			
		if(tempTodData.kvah_export < g_Class07_BillingBuffer.apparent_energyTZ_export[z])	
			tempTodData.kvah_export = g_Class07_BillingBuffer.apparent_energyTZ_export[z];
			
		cum_kvah_export += tempTodData.kwh_export;
#endif

// leave the md's alone - as they get updated every 15 minutes
// even if we were to leave them alone - we ought to zero the md values and the date time values - ajay - 08/02/2024
  	write_alternately(((int32_t)TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC), ((int32_t)ALTERNATE_TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC), (uint8_t*)&mytoddata, sizeof(mytoddata));
  }

// now we can compare the sum of the tod energies with the energies in the meter and reconcile
	if(cum_kwh > myenergydata.kwh)
		myenergydata.kwh = cum_kwh;
		
	if(cum_kvah > myenergydata.kvah)
		myenergydata.kvah = cum_kvah;
	
#ifdef TOD_HAS_KVARH_LAG
	if(cum_kvarh_lag > myenergydata.kvarh_lag)
		myenergydata.kvarh_lag = cum_kvarh_lag;
#endif

#ifdef BIDIR_METER
	if(cum_kwh_export > myenergydata.kwh_export)
		myenergydata.kwh_export = cum_kwh_export;
		
	if(cum_kvah_export > myenergydata.kvah_export)
		myenergydata.kvah_export = cum_kvah_export;
#endif

}

void recoverEnergies(void)
{
// this function is called if all memory locations have been corrupted
// the energy will be recovered from the previous bill data
// after recovering the energy record, we must write this same data into all the locations
// to ensure that future writes do not have any problem
//  toddata_t tempTodData;
	ext_rtc_t rtcLast;
	uint32_t addr;
  uint32_t temp_ptr;
	
	uint8_t tmonth;
	uint8_t i;
  uint8_t z;

	
	temp_ptr = ENERGIES_BASE_ADDRESS;

	if(Common_Data.bpcount==0)
	{// there are no bills - hence we must write 0 records
    clear_structure((uint8_t*)&myenergydata,sizeof(myenergydata)); // clear this for once
    myenergydata.chksum=0xff;
	}
	else
	{// we must pickup the data from the latest bill
  	tmonth = decrementMonth(1);	// latest bill
		addr=(int32_t)BILLDATA_BASE_ADDRESS + (int32_t)(tmonth-1)*(int32_t)BILL_PTR_INC;			
		EPR_Read(addr, (uint8_t*)&g_Class07_BillingBuffer, sizeof(g_Class07_BillingBuffer));

		myenergydata.kwh=g_Class07_BillingBuffer.active_energy;
		#ifdef FUNDAMENTAL_ENERGY_REQUIRED		
	  myenergydata.kwh_fundamental=g_Class07_BillingBuffer.active_energy;
		#endif		
		myenergydata.kvah=g_Class07_BillingBuffer.apparent_energy;
		#ifdef SPLIT_ENERGIES_REQUIRED		
	  myenergydata.kwh_lag=g_Class07_BillingBuffer.active_energy_lag;
	  myenergydata.kwh_lead=g_Class07_BillingBuffer.active_energy_lead;
		myenergydata.kvah_lag=g_Class07_BillingBuffer.apparent_energy_lag;
		myenergydata.kvah_lead=g_Class07_BillingBuffer.apparent_energy_lead;
		#endif		
	  myenergydata.kvarh_lag=g_Class07_BillingBuffer.kvarh_lag_value;
	  myenergydata.kvarh_lead=g_Class07_BillingBuffer.kvarh_lead_value;

		#ifdef BIDIR_METER
	  myenergydata.kwh_export=g_Class07_BillingBuffer.active_energy_export;
	  myenergydata.kwh_fundamental_export=g_Class07_BillingBuffer.active_energy_export;
	  myenergydata.kvah_export=g_Class07_BillingBuffer.apparent_energy_export;
	  myenergydata.kvarh_lag_export=g_Class07_BillingBuffer.kvarh_lag_value_export;
	  myenergydata.kvarh_lead_export=g_Class07_BillingBuffer.kvarh_lead_value_export;
		#endif
//	  myenergydata.powerOnminutes_eb=g_Class07_BillingBuffer.total_power;	// this is now the cumulative value 04/02/2024
// The above expression MUST NOT be used
// TO BE DONE
// If the number of bills is less than 10 - no billing overflow 
// then simply add the ponminutes, compute the average, and use it to calculate the cumulative
// powerOnminutes since the meter start date - for the time being
		myenergydata.powerOnminutes_eb = 0;	// please correct this - TO BE DONE
		
	  myenergydata.cumTamperDuration=0;// duration of all tampers till date (in minutes?)
		#ifdef BIDIR_METER
		myenergydata.kwh_nonSolar_export=g_Class07_BillingBuffer.kwh_nonSolar_export;	// added 04/02/2024
		myenergydata.kwh_Solar_import=g_Class07_BillingBuffer.kwh_Solar_import; // added 04/02/2024
		myenergydata.kwh_Solar_export=g_Class07_BillingBuffer.kwh_Solar_export;	// added 04/02/2024
		#endif
		
// now to recover the tariff energies

		recoverTOD_energies();
		

// now load the presentZone energies
		rtcLast.second=0;
		rtcLast.minute=ctldata.minute;
		rtcLast.hour=ctldata.hour;
		rtcLast.date=ctldata.date;
		rtcLast.month=ctldata.month;
		rtcLast.year=ctldata.year;
		rtcLast.day=(uint8_t)CalcDayNumFromDate((2000+rtcLast.year), rtcLast.month, rtcLast.date);	// this is correct
	
// get the toddata corresponding to last time
	  z=findZone(&rtcLast);
		
	  if(read_alternately(TOD_CURRENT_BASE_ADDRESS+z*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+z*ZONE_PTR_INC, (uint8_t*)&mytoddata, sizeof(mytoddata))==0)
		{// none of the locations had valid data
	    clear_structure((uint8_t*)&mytoddata,sizeof(mytoddata)); // this function fills zero bytes in the structure
		}

		myenergydata.zone_kwh = mytoddata.kwh;
		myenergydata.zone_kvah = mytoddata.kvah;

#ifdef TOD_HAS_REACTIVE_ENERGIES		
	  myenergydata.zone_kvarh_lag = mytoddata.kvarh_lag;
//	  myenergydata.zone_kvarh_lead = mytoddata.kvarh_lead;
#endif
	

#ifdef BIDIR_METER
		myenergydata.zone_kwh_export = mytoddata.kwh_export;
	  myenergydata.zone_kvah_export = mytoddata.kvah_export;
//	  myenergydata.zone_kvarh_lag_export = mytoddata.kvarh_lag_export;	// not available in tod
//	  myenergydata.zone_kvarh_lead_export = mytoddata.kvarh_lead_export;	// not available in tod
#endif
	  myenergydata.dummy=0;
  	myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);
	}

// now write this record at all locations
  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
	{
		R_WDT_Restart();
		EPR_Write(temp_ptr, (uint8_t*)&myenergydata, sizeof(myenergydata));
		incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
	}
	
	failCount=0;
//	ctldata.energies_wrt_ptr = ENERGIES_BASE_ADDRESS;
//  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
}

int write_energies(void)
{
// a global variable failCount is introduced
// This variable will be 0 to begin with, and each time a failure occurs, this will be incremented by 1
// Whenever there is success, the failCount will be made 0
// If for some reason failCount reaches the maximum value of ((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC)
// then a recovery procedure will be initiated

// return 0 for success and 99 for failure
// before we write we ought to read the energy(kwh) value already written - (preferably the smallest
// value and which is most likely to be in the location where we currently intend to write.
// we must check whether the value written there is a valid one and this needs to be done
// only if the current kwh is >0.32 kwh  

  energydata_t temp_energy_data;
  uint32_t temp_ptr=ctldata.energies_wrt_ptr;
  uint8_t i=0;
  uint8_t done=0;
  uint8_t noOfRecords;

	noOfRecords = (uint8_t)((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);

// we have modified the erase function to write properly zeroed energy records with checksum
//	asendbare((uint8_t *)"WrEnergiesEnter\r\n");
//	asendHexBufferInAscii((uint8_t *)&ctldata.energies_wrt_ptr, 2);
	
// if the failCount is 0, temp_ptr can be started from ctldata.energies_wrt_ptr
// if the failCount is = no of energy records then initiate the recovery procedure
// else temp_ptr must start from failCount records after ctldata.energies_wrt_ptr

	if(failCount==noOfRecords)
	{
		recoverEnergies();
		failCount=0;
		return 0;	// this is now a success
	}
	
// here things are not so bad	
	for(i=0;i<failCount;i++)
	{
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
	}
	// temp_ptr is now incremented as many times as failCount
	
  do
  {
		R_WDT_Restart();
		EPR_Read(temp_ptr, (uint8_t*)&temp_energy_data, sizeof(temp_energy_data));   // EEPROM Read
	
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
    i++;  // this is to keep track of how many times this loop has been executed

    if(isCheckSumOK((uint8_t*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
      done=1;
    }
  }
  while((done==0)&&(i<3));  // why waste time reading 32 locations if 35 kv is on?

  if(i==3) // there was no good record
	{
		failCount++;	// increment this variable every time a failure occurs
    return 1; // to indicate failure
	}
  else
  {// we have a good record
    if((myenergydata.kwh-temp_energy_data.kwh)>((int32_t)256*(int32_t)KWH_INCREMENT))  // although we write 32 values for safety limit is at 64
    {// for some strange reason our present myenergydata happens to contain a kwh wihch is 2.56 more than the previous stored valid value
      // then we will use the old value
      myenergydata=temp_energy_data;  // old value will have proper checksum
    }
    else
    {// we have a proper energy value to be written
			R_WDT_Restart();
			EPR_Write(ctldata.energies_wrt_ptr, (uint8_t*)&myenergydata, sizeof(myenergydata));
			
			incptr(&ctldata.energies_wrt_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
			ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
    }
		
		failCount=0;	// zero this variable if we have success
    return 0; // to indicate success
  }
}

uint32_t discoverEnergiesWrtPtr(void)
{// this function finds the energies_wrt_ptr value for the minimum energy record
  uint32_t temp_ptr;
  uint32_t retval;
  int i = 0;

  energydata_t temp_energy_data;
  energydata_t min_energy_data;
    
  min_energy_data.kwh=999999999;

  temp_ptr = ENERGIES_BASE_ADDRESS;

  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
		R_WDT_Restart();
    EPR_Read(temp_ptr,(uint8_t*)&temp_energy_data,sizeof(temp_energy_data));

    if(isCheckSumOK((uint8_t*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
      if((temp_energy_data.kwh < min_energy_data.kwh)&&(temp_energy_data.kwh >=0))
			{
        min_energy_data = temp_energy_data; // this is the minimum energy record
				retval = temp_ptr;
			}
    }
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
  }
	
	if (min_energy_data.kwh==999999999)
		retval = ENERGIES_BASE_ADDRESS;
	
	return retval;
}

uint32_t init_read_energies(void)
{// this function reads all the energies - basically the myenergydata
  uint32_t temp_ptr;
  uint32_t retval;
  int i = 0;

  energydata_t temp_energy_data;
  energydata_t min_energy_data;
    
//  min_energy_data.kwh=999999999;
	min_energy_data.powerOnminutes_eb = 999999999;
	
  temp_ptr = ENERGIES_BASE_ADDRESS;
//  ctldata.energies_wrt_ptr = ENERGIES_BASE_ADDRESS; // this was not there earlier.
//  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1); // 15/Jun/2013

// we dont seem to take into account the fact that after an overflow and abusrd max value may come before a 
// genuine minimum value. In such a case the initial max value will get accepted. this is not good
// a correct approach should be to first find the minimum value. Alternately one should ensure while writing
// that a high value greater than 64 increments does not get written.
// I think this approach may be more satisfactuory - computing wise.  

// first get the minimum kwh or powerOnminutes_eb
  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
		R_WDT_Restart();
    EPR_Read(temp_ptr,(uint8_t*)&temp_energy_data,sizeof(temp_energy_data));
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		

    if(isCheckSumOK((uint8_t*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
      if((temp_energy_data.powerOnminutes_eb < min_energy_data.powerOnminutes_eb)&&(temp_energy_data.powerOnminutes_eb >=0))
        min_energy_data = temp_energy_data; // this is the minimum energy record
    }
  }

// now that we have the minimum kwh, which could also be 999999999 in case the memory was blank
  if(min_energy_data.powerOnminutes_eb==999999999)
  {
//    min_energy_data.kwh=0;
//    clear_structure((uint8_t*)&myenergydata,sizeof(myenergydata)); // clear this for once
//    myenergydata.chksum=0xff;  // this shoudl really be 0xff
// this is a serious situation and hence we must resort to recoverEnergies and return	
		recoverEnergies();
		failCount=0;
		retval = ENERGIES_BASE_ADDRESS;
  }
  else
    myenergydata = min_energy_data; // initialise the myenergydata with the lowest value

// we now do a second pass and pick up that max value which satisfies the 64 increments  
// it may be a good idea to reinitialise the temp_ptr
  
  temp_ptr = ENERGIES_BASE_ADDRESS;

  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
		R_WDT_Restart();
    EPR_Read(temp_ptr,(uint8_t*)&temp_energy_data,sizeof(temp_energy_data));
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		

    if(isCheckSumOK((uint8_t*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
			if(temp_energy_data.powerOnminutes_eb >= myenergydata.powerOnminutes_eb)
			{
	      if((temp_energy_data.kwh >= myenergydata.kwh)&&((temp_energy_data.kwh-min_energy_data.kwh)<((int32_t)256*(int32_t)KWH_INCREMENT)))
	      {
	        myenergydata = temp_energy_data; // this is the maximum energy record
	        retval = temp_ptr;
	      }
#ifdef BIDIR_METER				
				else
				{
	      	if((temp_energy_data.kwh_export >= myenergydata.kwh_export)&&((temp_energy_data.kwh_export-min_energy_data.kwh_export)<((int32_t)256*(int32_t)KWH_INCREMENT)))
					{
		        myenergydata = temp_energy_data; // this is the maximum energy record
		        retval = temp_ptr;
					}
				}
#endif				
			}
    }
  }
// during reading we have now taken care that the highest value better be a proper value
// all the last saved values are now being updated upon returning from this routine
//  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1); // some pointers have got changed above
	return retval;
}

void init_read_lastBillDate(void)
{
  uint8_t tmonth; 

	tmonth=decrementMonth(1);
	
	if (Common_Data.bpcount==0)
	{
		g_Class03_BillingDate.year_high =0;
		g_Class03_BillingDate.year_low =0;
		g_Class03_BillingDate.month=0;
		g_Class03_BillingDate.day_of_month=0;
		g_Class03_BillingDate.day_of_week=0;
		g_Class03_BillingDate.hour=0;
		g_Class03_BillingDate.minute=0;
		g_Class03_BillingDate.hundredths =(uint8_t)TIME_HUNDREDTHS_NOT_SPECIFIED;
		g_Class03_BillingDate.deviation_high=128;
		g_Class03_BillingDate.deviation_low=0;
		g_Class03_BillingDate.clock_status=(uint8_t)NOT_SPECIFIED;
	}
	else
	{
		EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC, (uint8_t*)&g_Class03_BillingDate, sizeof(g_Class03_BillingDate));
	}
}

// This function is being rewritten since the billdata_t is no more
void composeCurrentBillData(void)
/// dType (0 - import) and 1 for export data and 2 for combined
{
  toddata_t tempTodData;
//	int32_t kwh_consumption = 0;
//	int32_t kvah_consumption = 0;
	int32_t temp;
  uint8_t z;

	update_energies_stored();
	
	readRTC();
	ConvertRTCDate(&g_Class07_BillingBuffer.bill_date,&rtc);
//	billdata.power_factor = computeAveragePF(); // this is computed below
	g_Class07_BillingBuffer.active_energy = total_energy_lastSaved;
	g_Class07_BillingBuffer.kvarh_lag_value = reactive_energy_lag_lastSaved;
	g_Class07_BillingBuffer.kvarh_lead_value = reactive_energy_lead_lastSaved;
	g_Class07_BillingBuffer.apparent_energy = apparent_energy_lastSaved;
	
#ifdef SPLIT_ENERGIES_REQUIRED	
	g_Class07_BillingBuffer.active_energy_lag = active_energy_lag_lastSaved;
	g_Class07_BillingBuffer.active_energy_lead = active_energy_lead_lastSaved;
	g_Class07_BillingBuffer.apparent_energy_lag = apparent_energy_lag_lastSaved;
	g_Class07_BillingBuffer.apparent_energy_lead = apparent_energy_lead_lastSaved;	
#endif
	
#ifdef BIDIR_METER	
	g_Class07_BillingBuffer.active_energy_export = total_energy_lastSaved_export;
	g_Class07_BillingBuffer.kvarh_lag_value_export = reactive_energy_lag_lastSaved_export;
	g_Class07_BillingBuffer.kvarh_lead_value_export = reactive_energy_lead_lastSaved_export;
	g_Class07_BillingBuffer.apparent_energy_export = apparent_energy_lastSaved_export;
	g_Class07_BillingBuffer.kwh_nonSolar_export = myenergydata.kwh_nonSolar_export;	// added 04/02/2024
	g_Class07_BillingBuffer.kwh_Solar_import = myenergydata.kwh_Solar_import;	// added 04/02/2024
	g_Class07_BillingBuffer.kwh_Solar_export = myenergydata.kwh_Solar_export;	// added 04/02/2024	
#endif	

	g_Class07_BillingBuffer.active_MD = mymddata.mdkw;
	ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT,&mymddata.mdkw_datetime);
	g_Class07_BillingBuffer.apparent_MD = mymddata.mdkva;
	ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT,&mymddata.mdkva_datetime);
	g_Class07_BillingBuffer.total_power = ctldata.powerOnminutes_eb;	// since 12/02/2024 - this is the monthly value
#ifdef BIDIR_METER	
	g_Class07_BillingBuffer.active_MD_export = mymddata.mdkw_export;
	ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT_export,&mymddata.mdkw_datetime_export);

	g_Class07_BillingBuffer.apparent_MD_export = mymddata.mdkva_export;
	ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT_export,&mymddata.mdkva_datetime_export);
#endif	

  for(z=0;z<TARIFFS;z++)
  {
  	read_alternately(TOD_CURRENT_BASE_ADDRESS+z*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+z*ZONE_PTR_INC, (uint8_t*)&tempTodData, sizeof(tempTodData));
// get the values stored in e2rom as they contain the md values		
		if(z==presentZone)
		{// for the present zone substitute the energy values with those in the energydata structure
			tempTodData.kwh=myenergydata.zone_kwh;
			tempTodData.kvah=myenergydata.zone_kvah;
#ifdef TOD_HAS_KVARH_LAG
			tempTodData.kvarh_lag=myenergydata.zone_kvarh_lag;
#endif
#ifdef BIDIR_METER			
//			tempTodData.kvarh_lag=myenergydata.zone_kvarh_lag;
			tempTodData.kwh_export=myenergydata.zone_kwh_export;
			tempTodData.kvah_export=myenergydata.zone_kvah_export;
//			tempTodData.kvarh_lag_export=myenergydata.zone_kvarh_lag_export;
#endif			
		}				
		
//		kwh_consumption +=tempTodData.kwh;
//		kvah_consumption +=tempTodData.kvah;
		
		g_Class07_BillingBuffer.active_energyTZ[z] = tempTodData.kwh;
		g_Class07_BillingBuffer.apparent_energyTZ[z] = tempTodData.kvah;

#ifdef TOD_HAS_KVARH_LAG
			g_Class07_BillingBuffer.reactive_energy_lagTZ[z] = tempTodData.kvarh_lag;
#endif
				
#ifdef BIDIR_METER			
//		g_Class07_BillingBuffer.reactive_energy_lagTZ[z] = tempTodData.kvarh_lag;
		g_Class07_BillingBuffer.active_energyTZ_export[z] = tempTodData.kwh_export;
		g_Class07_BillingBuffer.apparent_energyTZ_export[z] = tempTodData.kvah_export;
#endif
		
		g_Class07_BillingBuffer.active_MD_TZ[z] = tempTodData.mdkw;
		ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT_TZ[z],&tempTodData.mdkw_datetime);
//		billdata->active_MD_DT_TZ[z] = tempTodData.mdkw_datetime;
		g_Class07_BillingBuffer.apparent_MD_TZ[z] = tempTodData.mdkva;
		ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT_TZ[z],&tempTodData.mdkva_datetime);
//		billdata->apparent_MD_DT_TZ[z] = tempTodData.mdkva_datetime;
		
#ifdef BIDIR_METER			
		g_Class07_BillingBuffer.active_MD_TZ_export[z] = tempTodData.mdkw_export;
		ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT_TZ_export[z],&tempTodData.mdkw_datetime_export);
//		billdata->active_MD_DT_TZ_export[z] = tempTodData.mdkw_datetime_export;
		g_Class07_BillingBuffer.apparent_MD_TZ_export[z] = tempTodData.mdkva_export;
		ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT_TZ_export[z],&tempTodData.mdkva_datetime_export);
//		billdata->apparent_MD_DT_TZ_export[z] = tempTodData.mdkva_datetime_export;
#endif		
  }

/*	
// This method is incorrect	
	if(kvah_consumption>0)
		g_Class07_BillingBuffer.power_factor = (kwh_consumption*(int32_t)1000)/kvah_consumption;
	else
		g_Class07_BillingBuffer.power_factor=0;
//	billdata->power_factor = (kwh_consumption*(int32_t)1000)/kvah_consumption;
*/

// This is the correct one
#ifdef BIDIR_METER
	g_Class07_BillingBuffer.power_factor=computeAveragePF(total_energy_lastSaved, apparent_energy_lastSaved, total_energy_lastSaved_export, apparent_energy_lastSaved_export, EXPORT);
#else
	g_Class07_BillingBuffer.power_factor=computeAveragePF(total_energy_lastSaved, apparent_energy_lastSaved,0,0, IMPORT);
#endif

#ifdef BILLING_HAS_TAMPER_COUNT
	g_Class07_BillingBuffer.tamper_count = (int32_t)mytmprdata.tamperCount;
#endif
}


int32_t computeAveragePF_TOD(int32_t currentKWH, int32_t currentKVAH, uint8_t imp_ex_type, uint8_t whichZone)
{
	int32_t apf;
	int32_t prevKWH;
	int32_t prevKVAH;
	int32_t kwhConsumed;
	int32_t kvahConsumed;
	
	apf = 0;
	return apf;
}


#ifdef SPLIT_ENERGIES_REQUIRED
int32_t computeAveragePF_Lag_Lead(int32_t currentKWH, int32_t currentKVAH, uint8_t pfType)
{// this function computes average pf for lag and lead cases - only the current value is considered
// the currentKWH and currentKVAH are the (kwh_lag, kvah_lag) or (kwh_lead, kvah_lead) respectively 
// pfType (0 - Lag, 1 - Lead)
	int32_t apf;
	int32_t prevKWH;
	int32_t prevKVAH;
	int32_t kwhConsumed;
	int32_t kvahConsumed;
	int32_t activeEnergyLagOffset;
	uint8_t tmonth;
	
	apf = 0;
	if(Common_Data.bpcount==0)
	{// no bill has been made hence
		if(currentKVAH!=0)
			apf = (currentKWH * 1000)/currentKVAH;
	}
	else
	{// here a previous bill exists
		activeEnergyLagOffset = (int32_t)68 + ((int32_t)TIMEZONES*(int32_t)40);
	  tmonth = decrementMonth(1);
		if(pfType==0)
		{// lag case
		  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+activeEnergyLagOffset,(uint8_t*)&prevKWH,sizeof(prevKWH));
		  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+activeEnergyLagOffset+8,(uint8_t*)&prevKVAH,sizeof(prevKVAH));
		}
		else
		{// lead case
		  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+activeEnergyLagOffset+4,(uint8_t*)&prevKWH,sizeof(prevKWH));
		  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+activeEnergyLagOffset+12,(uint8_t*)&prevKVAH,sizeof(prevKVAH));
		}
		kwhConsumed = currentKWH - prevKWH;
		kvahConsumed = currentKVAH - prevKVAH;
		
		if((kvahConsumed !=0)&&(kvahConsumed>0))
			apf = (kwhConsumed*1000)/kvahConsumed;
	}
	
	if (apf>1000)
		apf=1000;
	
	return apf;
}
#endif

int32_t computeAveragePF(int32_t currentKWH, int32_t currentKVAH, int32_t currentKWH_export, int32_t currentKVAH_export, uint8_t imp_ex_type)
{// for GUJARAT only the import one will be computed
	uint8_t *ptr0;
	uint8_t *ptr1;
	int32_t apf;
	int32_t prevKWH;
	int32_t prevKVAH;
	int32_t prevKWH_export;
	int32_t prevKVAH_export;
	int32_t kwhConsumed;
	int32_t kvahConsumed;
	int32_t offset;
	uint8_t tmonth;
	
	apf = 0;
	if(Common_Data.bpcount==0)
	{// no bill has been made hence
		if((currentKVAH+currentKVAH_export)!=0)
			apf = ((currentKWH+currentKWH_export) * 1000)/(currentKVAH+currentKVAH_export);
	}
	else
	{// here a previous bill exists
	  tmonth = decrementMonth(1);

		ptr0 = (uint8_t*)&g_Class07_BillingBuffer;
		NOP();
		NOP();
		ptr1 = (uint8_t*)&g_Class07_BillingBuffer.apparent_energy;
		NOP();
		NOP();
		offset = ptr1-ptr0;

	  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+16,(uint8_t*)&prevKWH,sizeof(prevKWH));
	  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&prevKVAH,sizeof(prevKVAH));
			
		kwhConsumed = currentKWH - prevKWH;
		kvahConsumed = currentKVAH - prevKVAH;

	// now to get the values for export data
#ifdef BIDIR_METER	
		if(imp_ex_type!=0)
		{// we have a bidirectional meter here
			ptr1 = (uint8_t*)&g_Class07_BillingBuffer.active_energy_export;
			NOP();
			NOP();
			offset = ptr1-ptr0;
		  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&prevKWH_export,sizeof(prevKWH_export));

			ptr1 = (uint8_t*)&g_Class07_BillingBuffer.apparent_energy_export;
			NOP();
			NOP();
			offset = ptr1-ptr0;
		  EPR_Read(BILLDATA_BASE_ADDRESS+(tmonth-1)*BILL_PTR_INC+offset,(uint8_t*)&prevKVAH_export,sizeof(prevKVAH_export));

			kwhConsumed += (currentKWH_export - prevKWH_export);
			kvahConsumed +=(currentKVAH_export - prevKVAH_export);

		}
#endif

		if(kvahConsumed !=0)
			apf = (kwhConsumed*1000)/kvahConsumed;
	}

	if (apf>1000)
		apf=1000;
					
	return apf;
}

int32_t computePowerOffDurn(ext_rtc_t bdate)
{
	int32_t temp;
	ext_rtc_t lastBillDate;
	
	if(Common_Data.bpcount==0)
		temp = (diffInSecondsBetween(bdate, Common_Data.meterStartDate)/(int32_t)60);	// time difference in minutes
	else
	{// g_Class03_BillingDate is date_time_t - convert it to ext_rtc_t
		ConvertBackToRTC(&lastBillDate, &g_Class03_BillingDate);	
		temp = (diffInSecondsBetween(bdate, lastBillDate)/(int32_t)60);	// time difference in minutes
	}
// temp now contains the total minutes since the last bill was made and ctldata.powerOnminutes_eb is the power on minutes for this month
	temp = temp - ctldata.powerOnminutes_eb;	// this is the power fail duration for this month
	if(temp<0)
		temp=0;	// this is to prevent a -1 as an answer if there has been no power fail at all
	return temp;
}


void write_history_data(ext_rtc_t bdate)
{
  toddata_t tempTodData;
	int32_t temp;
  uint8_t tmonth; 
  uint8_t z;

	update_energies_stored();	
	
  tmonth=Common_Data.history_ptr-1;	// this is correct at this place - always do this - 25/Jul/2020
	
	ConvertRTCDate(&g_Class07_BillingBuffer.bill_date,&bdate);
	g_Class07_BillingBuffer.active_energy = total_energy_lastSaved;
	g_Class07_BillingBuffer.kvarh_lag_value = reactive_energy_lag_lastSaved;
	g_Class07_BillingBuffer.kvarh_lead_value = reactive_energy_lead_lastSaved;
	g_Class07_BillingBuffer.apparent_energy = apparent_energy_lastSaved;
	
#ifdef SPLIT_ENERGIES_REQUIRED	
	g_Class07_BillingBuffer.active_energy_lag = active_energy_lag_lastSaved;
	g_Class07_BillingBuffer.active_energy_lead = active_energy_lead_lastSaved;
	g_Class07_BillingBuffer.apparent_energy_lag = apparent_energy_lag_lastSaved;
	g_Class07_BillingBuffer.apparent_energy_lead = apparent_energy_lead_lastSaved;	
#endif

#ifdef BIDIR_METER	
	g_Class07_BillingBuffer.active_energy_export = total_energy_lastSaved_export;
	g_Class07_BillingBuffer.kvarh_lag_value_export = reactive_energy_lag_lastSaved_export;
	g_Class07_BillingBuffer.kvarh_lead_value_export = reactive_energy_lead_lastSaved_export;
	g_Class07_BillingBuffer.apparent_energy_export = apparent_energy_lastSaved_export;
	
	g_Class07_BillingBuffer.kwh_nonSolar_export = myenergydata.kwh_nonSolar_export;	// added 04/02/2024
	g_Class07_BillingBuffer.kwh_Solar_import = myenergydata.kwh_Solar_import;	// added 04/02/2024
	g_Class07_BillingBuffer.kwh_Solar_export = myenergydata.kwh_Solar_export;	// added 04/02/2024
#endif	
	
	
	g_Class07_BillingBuffer.active_MD = mymddata.mdkw;
	ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT,&mymddata.mdkw_datetime);
	g_Class07_BillingBuffer.apparent_MD = mymddata.mdkva;
	ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT,&mymddata.mdkva_datetime);
	g_Class07_BillingBuffer.total_power = ctldata.powerOnminutes_eb;	// ctldata.powerOnminutes_eb stores the value for the month
#ifdef BIDIR_METER	
	g_Class07_BillingBuffer.active_MD_export = mymddata.mdkw_export;
	ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT_export,&mymddata.mdkw_datetime_export);

	g_Class07_BillingBuffer.apparent_MD_export = mymddata.mdkva_export;
	ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT_export,&mymddata.mdkva_datetime_export);
	
#endif	

	temp = computePowerOffDurn(bdate);
	Common_Data.cumPowerFailDuration = Common_Data.cumPowerFailDuration + temp;	// this will get saved below
	
	g_Class03_BillingDate = g_Class07_BillingBuffer.bill_date;	// it is ok to do it here, but it would be nice to do it right at the end 
	
// save the present zone tod	
  mytoddata.kwh=myenergydata.zone_kwh;
  mytoddata.kvah=myenergydata.zone_kvah;
	
#ifdef TOD_HAS_KVARH_LAG
	mytoddata.kvarh_lag=myenergydata.zone_kvarh_lag;
#endif
	
#ifdef BIDIR_METER			
//  mytoddata.kvarh_lag=myenergydata.zone_kvarh_lag;
	
  mytoddata.kwh_export=myenergydata.zone_kwh_export;
  mytoddata.kvah_export=myenergydata.zone_kvah_export;
//  mytoddata.kvarh_lag=myenergydata.zone_kvarh_lag_export;
#endif

	verifyTOD_energies(presentZone);
	
  write_alternately(((int32_t)TOD_CURRENT_BASE_ADDRESS+(int32_t)presentZone*(int32_t)ZONE_PTR_INC), ((int32_t)ALTERNATE_TOD_CURRENT_BASE_ADDRESS+(int32_t)presentZone*(int32_t)ZONE_PTR_INC), (uint8_t*)&mytoddata, sizeof(mytoddata));

  for(z=0;z<TARIFFS;z++)
  {
    if(read_alternately((int32_t)TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC, (int32_t)ALTERNATE_TOD_CURRENT_BASE_ADDRESS+(int32_t)z*(int32_t)ZONE_PTR_INC, (uint8_t*)&tempTodData, sizeof(tempTodData))==0)
    {// function has failed- the alternate address's values are corrupted
      clear_structure((uint8_t*)&tempTodData,sizeof(tempTodData)); // this function fills zero bytes in the structure
		}		
		g_Class07_BillingBuffer.active_energyTZ[z] = tempTodData.kwh;
		g_Class07_BillingBuffer.apparent_energyTZ[z] = tempTodData.kvah;
#ifdef TOD_HAS_KVARH_LAG
		g_Class07_BillingBuffer.reactive_energy_lagTZ[z] =tempTodData.kvarh_lag;
#endif
#ifdef BIDIR_METER			
//		g_Class07_BillingBuffer.reactive_energy_lagTZ[z] =tempTodData.kvarh_lag;
		g_Class07_BillingBuffer.active_energyTZ_export[z] = tempTodData.kwh_export;
		g_Class07_BillingBuffer.apparent_energyTZ_export[z] = tempTodData.kvah_export;
#endif		
		g_Class07_BillingBuffer.active_MD_TZ[z] = tempTodData.mdkw;
		ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT_TZ[z],&tempTodData.mdkw_datetime);
		
		g_Class07_BillingBuffer.apparent_MD_TZ[z] = tempTodData.mdkva;
		ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT_TZ[z],&tempTodData.mdkva_datetime);
		
#ifdef BIDIR_METER			
		g_Class07_BillingBuffer.active_MD_TZ_export[z] = tempTodData.mdkw_export;
		ConvertRTCDate(&g_Class07_BillingBuffer.active_MD_DT_TZ_export[z],&tempTodData.mdkw_datetime_export);
		
		g_Class07_BillingBuffer.apparent_MD_TZ_export[z] = tempTodData.mdkva_export;
		ConvertRTCDate(&g_Class07_BillingBuffer.apparent_MD_DT_TZ_export[z],&tempTodData.mdkva_datetime_export);
#endif		
  }

#ifdef BIDIR_METER
	g_Class07_BillingBuffer.power_factor=computeAveragePF(total_energy_lastSaved, apparent_energy_lastSaved, total_energy_lastSaved_export, apparent_energy_lastSaved_export, EXPORT);
#else
	g_Class07_BillingBuffer.power_factor=computeAveragePF(total_energy_lastSaved, apparent_energy_lastSaved,0,0, IMPORT);
#endif

#ifdef BILLING_HAS_TAMPER_COUNT
	g_Class07_BillingBuffer.tamper_count = (int32_t)mytmprdata.tamperCount;
#endif

// we have now stuffed all the data into billdata
	EPR_Write((int32_t)BILLDATA_BASE_ADDRESS+(int32_t)tmonth*(int32_t)BILL_PTR_INC, (uint8_t*)&g_Class07_BillingBuffer, sizeof(g_Class07_BillingBuffer));

// being done below
// ctldata ought to be saved too	
}

void performEndJobsForBilling(void)
{
	Common_Data.history_ptr++;
  if(Common_Data.history_ptr==13)
	{
    Common_Data.history_ptr = 1;
		ctldata.overflow |= HISTORY_OVERFLOW;
	}
  Common_Data.bpcount++;
	write_common_data();
	
// initialize the energy record for the next billing cycle;
  ctldata.powerOnminutes_eb=0; // this is figure for every month - from 04/02/2024 onwards - 

//	updateEnergyDataZoneEnergies();
//	clearZoneLastSavedEnergies();
	
// why is clearZoneLastSavedEnergies(); not being called here when it is being called at two other places? 02/Mar/2020
  myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);
	write_energies(); // save the energydata
	
// ctldata ought to be saved too	

}


extern uint32_t g_Class07_Billing_EntriesInUse;
extern uint16_t g_Class07_Billing_CurrentEntry;

void computeBillingEntries(void){
	if(0==(ctldata.overflow&HISTORY_OVERFLOW)){// here there is no overflow
//		g_Class07_Billing_EntriesInUse = (Common_Data.history_ptr-1);
		g_Class07_Billing_EntriesInUse = (Common_Data.history_ptr);	// the first index is for current month's data
		g_Class07_Billing_CurrentEntry=g_Class07_Billing_EntriesInUse;
	}
	else{// here overflow has occured
		g_Class07_Billing_EntriesInUse = 13;	// 12 earlier
		if(Common_Data.history_ptr==1)
			g_Class07_Billing_CurrentEntry=13;	// 12 earlier
		else
			g_Class07_Billing_CurrentEntry=(Common_Data.history_ptr);
//			g_Class07_Billing_CurrentEntry=(Common_Data.history_ptr-1);
	}
}

uint8_t decrementMonth(uint8_t diff)
{
//  signed uint8_t ret = ctldata.month-diff;
  int8_t ret = Common_Data.history_ptr-diff;
  if(ret <= 0)
    ret += 12;
  return ret;
}

void clearMymeterData(void)
{
//	zoneDefs_t zoneDefines;
	toddata_t tempToddata;
	uint8_t *ptr;
  uint8_t i;

//	populateZoneDefinitions(&zoneDefines);
	
// the energy record is already initialised
// the entire mymddata is to be cleared
  
  ptr=(uint8_t*)&mymddata.mdkw; 
  
  for(i=0;i<sizeof(mymddata);i++)
  {
    *ptr=0;
    ptr++;
  }
    
  mymddata.chksum=calcChecksum((uint8_t*)&mymddata,sizeof(mymddata)-1); // size-1

// write into e2rom also
  write_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata));
	
#ifdef MD_RESET_SYNCHRONOUS
  if(((int32_t)rtc.minute%(int32_t)Common_Data.mdinterval) == 0)
	{
	  ctldata.kwh_last_saved=total_energy_lastSaved;
	  ctldata.kvah_last_saved=apparent_energy_lastSaved;
		#ifdef BIDIR_METER			
	  ctldata.kwh_last_saved_export=total_energy_lastSaved_export;
	  ctldata.kvah_last_saved_export=apparent_energy_lastSaved_export;
		#endif
	}
#else
  ctldata.kwh_last_saved=total_energy_lastSaved;
  ctldata.kvah_last_saved=apparent_energy_lastSaved;
	#ifdef BIDIR_METER			
  ctldata.kwh_last_saved_export=total_energy_lastSaved_export;
  ctldata.kvah_last_saved_export=apparent_energy_lastSaved_export;
	#endif
#endif

  // the tod structure contains energies which are cumulative, but the mdkw,mdva, mdkw_export, mdkva_export
	// alongwith their date times need to be cleared for the start of the new bill month
	//clear_structure((uint8_t*)&mytoddata,sizeof(mytoddata)); // clear this for once
  // it makes sense clearing the mytoddata md values, since they are in the ram - changed on 10/02/2024
	
	clear_structure((uint8_t*)&mytoddata.mdkw_datetime,sizeof(ext_rtc_t));
	clear_structure((uint8_t*)&mytoddata.mdkva_datetime,sizeof(ext_rtc_t));
#ifdef BIDIR_METER			
	clear_structure((uint8_t*)&mytoddata.mdkw_datetime_export,sizeof(ext_rtc_t));
	clear_structure((uint8_t*)&mytoddata.mdkva_datetime_export,sizeof(ext_rtc_t));
#endif		

// we do not know when the bill is made whether the tod zone will change or not - however the mytoddata md values
// must be cleared sine the bill is made in ram as well as in e2rom - ram done above and e2rom being done below
// it is presumed that if the zone has changed, it would be done before bill is made in do_inital_things
// and if power has not failed then it will get corrected after the bill has been made

// now also clear the md values in the e2rom	
	
  for(i=0;i<TARIFFS;i++)
  {
    read_alternately(TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC, (uint8_t*)&tempToddata, sizeof(tempToddata));
    
		tempToddata.mdkw=0;
		tempToddata.mdkva=0;
#ifdef BIDIR_METER			
		tempToddata.mdkw_export=0;
		tempToddata.mdkva_export=0;
#endif		
		clear_structure((uint8_t*)&tempToddata.mdkw_datetime,sizeof(ext_rtc_t));
		clear_structure((uint8_t*)&tempToddata.mdkva_datetime,sizeof(ext_rtc_t));
#ifdef BIDIR_METER			
		clear_structure((uint8_t*)&tempToddata.mdkw_datetime_export,sizeof(ext_rtc_t));
		clear_structure((uint8_t*)&tempToddata.mdkva_datetime_export,sizeof(ext_rtc_t));
#endif		
    delay();  // pause a while - this calls clearwdt
    write_alternately((TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC), (uint8_t*)&tempToddata, sizeof(tempToddata));
  }
}

void initialise_ctl_data(void);

void initialise_ctl_data(void)
{
  clear_structure((uint8_t*)&ctldata,sizeof(ctldata)); // this function fills zero bytes in the structure

  ctldata.energies_wrt_ptr=ENERGIES_BASE_ADDRESS;
  ctldata.year = rtc.year;
  ctldata.month = rtc.month;
  ctldata.date = rtc.date;

	ctldata.survey_wrt_ptr=SURVEY_BASE_ADDRESS;
	ctldata.cumulative_md=0;
	ctldata.cumulative_md_kva=0;
	ctldata.daily_log_wrt_ptr=DAILYLOG_BASE_ADDRESS;
// when we start the absolutely first time - then load survey records for the day upto the rtc time now now ought to be zeroed 			
  ctldata.hour = 0;
  ctldata.minute = 0;

	do_survey_update_small();
	do_log_update();	// must be called only for regular meters			
// the above three lines should cause the zeroed records to be created upto rtc time
// after returning
  ctldata.hour = rtc.hour;
  ctldata.minute = rtc.minute;
}

void recover_ctl_data(void)
{
// we will restore the following variables from the saved values
//ctldata.survey_wrt_ptr=ctldata_backup.survey_wrt_ptr+(((rtc.hour)*60 + rtc.minute)/Common_Data.survey_interval)*SURVEY_PTR_INC;
//ctldata.daily_log_wrt_ptr = ctldata_backup.daily_log_wrt_ptr;
// the energies_wrt_ptr can be restored by reading energies from e2rom

	ctldataBackup_t ctldata_backup;

  if(read_alternately(CTLDATA_BACKUP_ADDRESS, ALTERNATE_CTLDATA_BACKUP_ADDRESS, (uint8_t*)&ctldata_backup, sizeof(ctldata_backup))==1)
  {// we have good backup data
		ctldata.powerOnminutes_eb=ctldata_backup.powerOnminutes_eb;	// ctldata.powerOnminutes_eb - OK
		ctldata.cumulative_md = ctldata_backup.cumulative_md;
		ctldata.cumulative_md_kva = ctldata_backup.cumulative_md_kva;
		
	#ifdef BIDIR_METER	
		ctldata.cumulative_md_export = ctldata_backup.cumulative_md_export;
		ctldata.cumulative_md_kva_export= ctldata_backup.cumulative_md_kva_export;
	#endif
		
	  ctldata.survey_wrt_ptr = ctldata_backup.survey_wrt_ptr+(((rtc.hour)*60 + rtc.minute)/Common_Data.survey_interval)*SURVEY_PTR_INC;
	//	ctldata.energies_wrt_ptr = ctldata_backup.energies_wrt_ptr; // this is recovered separately
		ctldata.daily_log_wrt_ptr = ctldata_backup.daily_log_wrt_ptr;

// During recovery make sure that ctldata date time is the same as rtc date time to avoid any update by do initial things		
		ctldata.date = rtc.date;
		ctldata.month = rtc.month;
		ctldata.year = rtc.year;
		ctldata.hour = rtc.hour;
		ctldata.minute = rtc.minute;
		ctldata.overflow = ctldata_backup.overflow;
	}
	else
	{// here the backup copy is itself bad
// The powerOnminutes may be computed by taking the average of the bill data and using it to
// compute the figure for the month by accounting for the current rtc.date - 12/02/2024 - TO BE DONE
		ctldata.powerOnminutes_eb=0;	// ctldata.powerOnminutes_eb - this is now a monthly figure so OK
		ctldata.cumulative_md = 0;
		ctldata.cumulative_md_kva = 0;
		
	#ifdef BIDIR_METER	
		ctldata.cumulative_md_export = 0;
		ctldata.cumulative_md_kva_export=0;
	#endif
		
	  ctldata.survey_wrt_ptr = SURVEY_BASE_ADDRESS+(((rtc.hour)*60 + rtc.minute)/Common_Data.survey_interval)*SURVEY_PTR_INC;
		ctldata.daily_log_wrt_ptr = DAILYLOG_BASE_ADDRESS;
		
// During recovery make sure that ctldata date time is the same as rtc date time to avoid any update by do initial things		
		ctldata.date = rtc.date;
		ctldata.month = rtc.month;
		ctldata.year = rtc.year;
		ctldata.hour = rtc.hour;
		ctldata.minute = rtc.minute;
		ctldata.overflow = 0;
	}
}

int8_t init_read_ctl_data(void)
{
	uint8_t done=0;
	int8_t retval =0;
	
  if(isCheckSumOK((uint8_t*)&ctldata,sizeof(ctldata))==1)
  {// here checksum is OK - just do a sanity check
		if((ctldata.year>=1)&&(ctldata.year<100))
		{// year is ok
			if((ctldata.month>=1)&&(ctldata.month<13))
			{// month is ok
				if((ctldata.date>=1)&&(ctldata.date<32))
				{// date is ok
					if(ctldata.hour<24)
					{// hour is ok
						if(ctldata.minute<60)
						{// minute is ok
							done=1;
						}
					}
				}
			}
		}
	}

	if(done==0)
	{// this means that there is something wrong with the ctldata in ram even though checksum was fine
    if(read_alternately(CTLDATA_BASE_ADDRESS, ALTERNATE_CTLDATA_BASE_ADDRESS, (uint8_t*)&ctldata, sizeof(ctldata))==0)
    {// function has failed
			if(1==zeroedStruct((uint8_t*)&ctldata,sizeof(ctldata)))
			{// if the entire structure read from memory is zeroed - this will happen only if a zap command had been issued
				initialise_ctl_data();
// initialise ctl data has already done survey update, hence the survey_wrt_ptrs are already advanced till now				
				write_ctl_backup_data(1);
// the write ctl backup data equates the backed up survey_wrt_ptr value to the current survey_wrt_ptr value
// which has already been advanced and is upto date
// During recovery, the survey_wrt_ptr is advanced to current time from the value stored in backup
// This is perfectly fine if the backup was made at 00:00 hours, but inherently incorrect when
// it is being done here which could happen at a time other than 00:00 hours
// The fix is to call write ctl backup_data(1) with a type parameter which is 1 in this case
// and 0 when it is called at 00:00 hours
			}
			else
			{
//				recover_ctl_data();
				retval=1;	// this function will be called upon exit by testing retval
			}
    }
	}
	
  checkptr(&ctldata.energies_wrt_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);

// The following pointers are not multiples of 16 or 32 - hence the two checkptr calls may be disastrous	
//  checkptr(&ctldata.survey_wrt_ptr, SURVEY_BASE_ADDRESS, SURVEY_END_ADDRESS, SURVEY_PTR_INC);
//  checkptr(&ctldata.daily_log_wrt_ptr, DAILYLOG_BASE_ADDRESS, DAILYLOG_END_ADDRESS, DAILYLOG_REC_SIZE);
	
  ctldata.chksum=calcChecksum((uint8_t*)&ctldata,sizeof(ctldata)-1);
	
	computeLoadSurveyEntries(rtc.hour,rtc.minute);
	computeDailyLogEntries();
	return retval;
}

void init_read_common_data(void)
{// this is the common meter data andis called calib_data for historical reasons
  if(isCheckSumOK((uint8_t*)&Common_Data,sizeof(Common_Data))==0)
  {
    if(read_alternately(COMMON_DATA_ADDRESS, ALTERNATE_COMMON_DATA_ADDRESS, (uint8_t*)&Common_Data, sizeof(Common_Data))==0)
    {// function has failed- the alternate address's values are corrupted
      clear_structure((uint8_t*)&Common_Data,sizeof(Common_Data)); // this function fills zero bytes in the structure
#ifdef BIDIR_METER
			Common_Data.meterType=0;	// for Bidir meter the meterType is 0 (default)
      Common_Data.mdinterval=15;
			Common_Data.new_mdinterval = 15;
#else
			Common_Data.meterType=1;	// for Bidir meter the meterType is 0 (default)
      Common_Data.mdinterval=30;
			Common_Data.new_mdinterval = 30;
#endif			
			Common_Data.meterNumber=345678;
      Common_Data.overVoltageLimit=28000;  // this number is without decimals.
      Common_Data.underVoltageLimit=15000; // this number is without decimals.
      Common_Data.comm_ver[0]=1;
      Common_Data.comm_ver[1]=1;
      Common_Data.history_ptr=1;
      Common_Data.billing_date=1;
			Common_Data.rtc_failed = 0;	// this variable now contains the status of rtc failure
//      Common_Data.ct_ratio=100;	// this variable not used any more
//      Common_Data.pt_ratio=10;
			Common_Data.autoResetPermitted=1;
      Common_Data.survey_interval=SURVEY_INTERVAL;
			Common_Data.meterStartDate =  rtc;
      Common_Data.batchmonth=rtc.month;
      Common_Data.batchyear=rtc.year;
//			Common_Data.billing_month = 1;	// means billing to be done every odd month
			Common_Data.billing_month = 0;	// means billing to be done monthly
      
      Common_Data.new_survey_interval=SURVEY_INTERVAL;

			GetManufacturersName((char*)&Common_Data.mfgname);
      Common_Data.chksum=calcChecksum((uint8_t*)&Common_Data,sizeof(Common_Data)-1);
// after validating we ought to compute checksum
// This function is called when meter is reset, and the reset count is incremented
// no one seems to be interested in this reset count - in any case it is incremented in the main()
// and thereafter Common_Data is being written alternately
    }    
  }  
}

void init_read_md_data(void)
{// this is the common meter data andis called calib_data for historical reasons
  if(isCheckSumOK((uint8_t*)&mymddata,sizeof(mymddata))==0)
  {// check sum is not ok - pick up the md values from CURRENT_MD_BASE_ADDRESS etc.
    if(read_alternately(CURRENT_MD_BASE_ADDRESS, ALTERNATE_CURRENT_MD_BASE_ADDRESS, (uint8_t*)&mymddata, sizeof(mymddata))==0)
    {
      clear_structure((uint8_t*)&mymddata,sizeof(mymddata)); // this function fills zero bytes in the structure
      mymddata.chksum=calcChecksum((uint8_t*)&mymddata,sizeof(mymddata)-1);
    }
  }
}


void init_read_secret_keys(void)
{
  if(read_alternately(SECRET1_ADDRESS, ALTERNATE_SECRET1_ADDRESS, (uint8_t*)&secret1, 9)==0)
  {
		memcpy(secret1, "ABCDEFGH", 8);
  }
	secret1[8]=0;

	if(secret1[0]==0)
		memcpy(secret1, "ABCDEFGH", 8);
	
  if(read_alternately(SECRET2_ADDRESS, ALTERNATE_SECRET2_ADDRESS, (uint8_t*)&secret2, 17)==0)
  {
		memcpy(secret2, "RENESAS_P6wRJ21F", 16);
  }
	secret2[16]=0;
	
	if(secret2[0]==0)
		memcpy(secret2, "RENESAS_P6wRJ21F", 16);
	
}



void write_single_action_activation_data(uint8_t status)
{
	uint8_t buff[16];

	memcpy(&buff[0],(uint8_t *)&g_Class22_billtime,sizeof(time_t));
	memcpy(&buff[4],(uint8_t *)&g_Class22_billdate,sizeof(date_t));
	buff[9]=status;

//	class22_status =1;	// update the ram variable also
  write_alternately(SINGLE_ACTION_ACTIVATION_TIME_ADDRESS, ALTERNATE_SINGLE_ACTION_ACTIVATION_TIME_ADDRESS, (uint8_t*)&buff, sizeof(buff));	
}


void init_read_single_action_activation_data(void)
{
	uint8_t buff[16];

// we must now read the data at the SINGLE_ACTION_ACTIVATION_TIME_ADDRESS which contains the g_Class22_billdate (five bytes),
// g_Class22_billtime(4 bytes) and one byte of status and one byte of checksum

  if(read_alternately(SINGLE_ACTION_ACTIVATION_TIME_ADDRESS,ALTERNATE_SINGLE_ACTION_ACTIVATION_TIME_ADDRESS,(uint8_t*)&buff,sizeof(buff))!=0)
	{// here we have success
		memcpy((uint8_t *)&g_Class22_billtime,&buff[0],sizeof(time_t));
		memcpy((uint8_t *)&g_Class22_billdate,&buff[4],sizeof(date_t));
		class22_status=buff[9];
	}
	else
	{
		class22_status=255;	// 255 means this is the first time the g_Class22_billdate variables is to be populated
		computeNextBillDate();
		class22_status=0;	// no single action is to be performed
		write_single_action_activation_data(class22_status);
	}
// if there is failure then the default values of g_Class22_billdate and g_Class22_billtime will be used
}


void write_passive_tod_activation_data(uint8_t status)
{
	uint8_t buff[16];

	memcpy(&buff[0],(uint8_t *)&g_Class20_ac_time,sizeof(date_time_t));
	buff[12]=status;

  write_alternately(CLASS20_ACTIVATION_TIME_ADDRESS, ALTERNATE_CLASS20_ACTIVATION_TIME_ADDRESS, (uint8_t*)&buff, sizeof(buff));	
}

void write_passive_tod(void)
{
	class20_total_data_t total_data;
	savePassiveData(&total_data);	// all passive tod definitions will be copied to total_data
  write_alternately(PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS, ALTERNATE_PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS, (uint8_t*)&total_data, sizeof(total_data));	
}

void init_read_passive_tod(void)
{
	class20_total_data_t total_data;
	uint8_t buff[16];

  if(read_alternately(PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS,ALTERNATE_PASSIVE_SEASON_TOD_DEFS_BASE_ADDRESS,(uint8_t*)&total_data,sizeof(total_data))!=0)
  {// here the e2rom contains a proper value - which must be transferred to active set
		loadPassiveData(&total_data);	//passive tod definitions for season, week, days and time arrays
  }
// we must now read the data at the CLASS20_ACTIVATION_TIME_ADDRESS which contains the activation time (twelve bytes) and one byte
// of status and one byte of checksum

  if(read_alternately(CLASS20_ACTIVATION_TIME_ADDRESS,ALTERNATE_CLASS20_ACTIVATION_TIME_ADDRESS,(uint8_t*)&buff,sizeof(buff))!=0)
	{// here we have success
		memcpy((uint8_t *)&g_Class20_ac_time,buff,sizeof(date_time_t));
		class20_status=buff[12];
	}
	else
		class20_status=0;	// 0 means nothing is to be done
	
// if there is failure then the default values of g_Class20_ac_time will be used
}

void init_read_tod(void)
{
	class20_total_data_t total_data;
  if(read_alternately(ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS,ALTERNATE_ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS,(uint8_t*)&total_data,sizeof(total_data))!=0)
  {// here the e2rom contains a proper value - which must be transferred to active set
		loadActiveData(&total_data);	//active tod definitions for season, week, days and time arrays
  }
// else simply return - since we will use the default values of the active data
}

// The following function must be called when the passive tod data is transferred to active tod
// we should save the data in e2rom, so that when a reset occurs the correct active data gets loaded
void write_active_tod(void)
{// this will save the active data distributed in bits and pieces to one composite structure and written into e2rom 
	class20_total_data_t total_data;
	saveActiveData(&total_data);	// all active tod definitions will be copied to total_data
  write_alternately(ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS, ALTERNATE_ACTIVE_SEASON_TOD_DEFS_BASE_ADDRESS, (uint8_t*)&total_data, sizeof(total_data));	
}


void init_read_tamper_data(void)
{// this is the common meter data andis called calib_data for historical reasons

	uint32_t temp_ptr;
  if(isCheckSumOK((uint8_t*)&mytmprdata,sizeof(mytmprdata))==0)
  {
    if(read_alternately(MYTMPRDATA_BASE_ADDRESS, ALTERNATE_MYTMPRDATA_BASE_ADDRESS, (uint8_t*)&mytmprdata, sizeof(mytmprdata))==0)
    {
      clear_structure((uint8_t*)&mytmprdata,sizeof(mytmprdata)); // this function fills zero bytes in the structure
    }
  }

	temp_ptr=mytmprdata.tamper_events0_wrt_ptr;
	checkptr(&temp_ptr, EVENTS0_BASE_ADDRESS, EVENTS0_END_ADDRESS, EVENT_PTR_INC);
	mytmprdata.tamper_events0_wrt_ptr=temp_ptr;
	
	g_Class07_Event_CurrentEntry[0]=(mytmprdata.tamper_events0_wrt_ptr-EVENTS0_BASE_ADDRESS)/EVENT_PTR_INC; // this may be 0
	if((mytmprdata.tamperEvent_overflow&_BV(0))!=0){
//		if(g_Class07_Event_CurrentEntry[0]==0)
//			g_Class07_Event_CurrentEntry[0]=1;
		g_Class07_Event_EntriesInUse[0]=(EVENTS0_END_ADDRESS-EVENTS0_BASE_ADDRESS)/EVENT_PTR_INC;
	}
	else
		g_Class07_Event_EntriesInUse[0]=g_Class07_Event_CurrentEntry[0];
	
	
	temp_ptr=mytmprdata.tamper_events1_wrt_ptr;
	checkptr(&temp_ptr, EVENTS1_BASE_ADDRESS, EVENTS1_END_ADDRESS, EVENT_PTR_INC);
	mytmprdata.tamper_events1_wrt_ptr=temp_ptr;

	g_Class07_Event_CurrentEntry[1]=(mytmprdata.tamper_events1_wrt_ptr-EVENTS1_BASE_ADDRESS)/EVENT_PTR_INC;
	if((mytmprdata.tamperEvent_overflow&_BV(1))!=0){
//		if(g_Class07_Event_CurrentEntry[1]==0)
//			g_Class07_Event_CurrentEntry[1]=1;
		g_Class07_Event_EntriesInUse[1]=(EVENTS1_END_ADDRESS-EVENTS1_BASE_ADDRESS)/EVENT_PTR_INC;
	}
	else
		g_Class07_Event_EntriesInUse[1]=g_Class07_Event_CurrentEntry[1];
	
	
	temp_ptr=mytmprdata.tamper_events2_wrt_ptr;
	checkptr(&temp_ptr, EVENTS2_BASE_ADDRESS, EVENTS2_END_ADDRESS, EVENT_PTR_INC);
	mytmprdata.tamper_events2_wrt_ptr=temp_ptr;

	g_Class07_Event_CurrentEntry[2]=(mytmprdata.tamper_events2_wrt_ptr-EVENTS2_BASE_ADDRESS)/EVENT_PTR_INC;
	if((mytmprdata.tamperEvent_overflow&_BV(2))!=0){
//		if(g_Class07_Event_CurrentEntry[2]==0)
//			g_Class07_Event_CurrentEntry[2]=1;
		g_Class07_Event_EntriesInUse[2]=(EVENTS2_END_ADDRESS-EVENTS2_BASE_ADDRESS)/EVENT_PTR_INC;
	}
	else
		g_Class07_Event_EntriesInUse[2]=g_Class07_Event_CurrentEntry[2];
	
	temp_ptr=mytmprdata.tamper_events3_wrt_ptr;
	checkptr(&temp_ptr, EVENTS3_BASE_ADDRESS, EVENTS3_END_ADDRESS, EVENT_PTR_INC);
	mytmprdata.tamper_events3_wrt_ptr=temp_ptr;

	g_Class07_Event_CurrentEntry[3]=(mytmprdata.tamper_events3_wrt_ptr-EVENTS3_BASE_ADDRESS)/EVENT_PTR_INC;
	if((mytmprdata.tamperEvent_overflow&_BV(3))!=0){
//		if(g_Class07_Event_CurrentEntry[3]==0)
//			g_Class07_Event_CurrentEntry[3]=1;
		g_Class07_Event_EntriesInUse[3]=(EVENTS3_END_ADDRESS-EVENTS3_BASE_ADDRESS)/EVENT_PTR_INC;
	}
	else
		g_Class07_Event_EntriesInUse[3]=g_Class07_Event_CurrentEntry[3];
	
	
	temp_ptr=mytmprdata.tamper_events4_wrt_ptr;
	checkptr(&temp_ptr, EVENTS4_BASE_ADDRESS, EVENTS4_END_ADDRESS, EVENT_PTR_INC);
	mytmprdata.tamper_events4_wrt_ptr=temp_ptr;

	g_Class07_Event_CurrentEntry[4]=(mytmprdata.tamper_events4_wrt_ptr-(int32_t)EVENTS4_BASE_ADDRESS)/(int32_t)EVENT_PTR_INC;
	if((mytmprdata.tamperEvent_overflow&_BV(4))!=0){
//		if(g_Class07_Event_CurrentEntry[4]==0)
//			g_Class07_Event_CurrentEntry[4]=1;
		g_Class07_Event_EntriesInUse[4]=((int32_t)EVENTS4_END_ADDRESS-(int32_t)EVENTS4_BASE_ADDRESS)/(int32_t)EVENT_PTR_INC;
	}
	else
		g_Class07_Event_EntriesInUse[4]=g_Class07_Event_CurrentEntry[4];
	
	temp_ptr=mytmprdata.tamper_events5_wrt_ptr;
	checkptr(&temp_ptr, EVENTS5_BASE_ADDRESS, EVENTS5_END_ADDRESS, EVENT_PTR_INC);
	mytmprdata.tamper_events5_wrt_ptr=temp_ptr;

// this is a special case where only one tamper is recorded
// hence regular code is commented below

	if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK) == 0)
	{
		g_Class07_Event_CurrentEntry[5]=0;
		g_Class07_Event_EntriesInUse[5]=0;
	}
	else
	{
		g_Class07_Event_CurrentEntry[5]=1;
		g_Class07_Event_EntriesInUse[5]=1;
	}

/*
	g_Class07_Event_CurrentEntry[5]=(mytmprdata.tamper_events5_wrt_ptr-(int32_t)EVENTS5_BASE_ADDRESS)/(int32_t)EVENT_PTR_INC;
	if((mytmprdata.tamperEvent_overflow&_BV(5))!=0){
		if(g_Class07_Event_CurrentEntry[5]==0)
			g_Class07_Event_CurrentEntry[5]=1;
//		g_Class07_Event_EntriesInUse[5]=(int32_t)((int32_t)EVENTS5_END_ADDRESS-(int32_t)EVENTS5_BASE_ADDRESS)/(int32_t)EVENT_PTR_INC;	// use this in general
		g_Class07_Event_EntriesInUse[5]=1;
	}
	else
		g_Class07_Event_EntriesInUse[5]=g_Class07_Event_CurrentEntry[5];
*/	
	
  mytmprdata.chksum=calcChecksum((uint8_t*)&mytmprdata,sizeof(mytmprdata)-1);
	
	
  if(isCheckSumOK((uint8_t*)&g_Class01_EventCode,14)==0)
  {
    if(read_alternately(LATEST_EVENT_BASE_ADDRESS, ALTERNATE_LATEST_EVENT_BASE_ADDRESS, (uint8_t*)&g_Class01_EventCode, 14)==0)
    {
      clear_structure((uint8_t*)&g_Class01_EventCode,14); // this function fills zero bytes in the structure
    }
  }
}



const uint32_t maskTable[] =
{
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,
	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,
	0x00001000,
	0x00002000,
	0x00004000,
	0x00008000,
	0x00010000,
	0x00020000,
	0x00040000,
	0x00080000,
	0x00100000,
	0x00200000,
	0x00400000,
	0x00800000,
	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000,
	0x10000000,
	0x20000000,
	0x40000000,
	0x80000000,
};


uint32_t computeMask(uint8_t flagbit)
{
	uint32_t retval;
	
	if(flagbit>31)
		retval=0;
	else
	 retval = maskTable[flagbit];
	
	return retval;
}

uint8_t findWhichBitIsOne(uint32_t givenLong)
{
	uint8_t whichBit=0;
	uint8_t i;
	
	if(givenLong!=0)
	{
		for(i=1;i<32;i++)
		{
			if((givenLong&maskTable[i])!=0)
			{
				whichBit=i;
				i=99;	// break
			}
		}
	}
	return whichBit;
}


void wre2rom(void)
{ 
// this is not necessary anymore	
//  if((moreflags&MS50FLG)==0)
//    return;
  eventdata_t temp_eventdata;
  uint32_t whichevent;
	uint32_t temp_event_wrt_ptr;
	uint32_t temp_base_address;
	uint32_t temp_end_address;
	uint8_t whichGroup;
//	uint8_t giveUp;
	
	if(e2write_flags&E2_W_IMPDATA)		// common data
  {	
//    Common_Data.meter_calibrated = 0xAA;
		write_common_data();
  }

  if(e2write_flags&E2_W_CTLDATA)
  {
		write_ctl_data();	// this function creates 4 copies and clears the E2_W_CTLDATA
  }		

//------------------ tampers recording ------------------  
  if(e2write_flags&E2_W_EVENTDATA)
  {
//    temp_eventdata.eventCode = mytmprdata.tamperEvent_flags & ~(mytmprdata.tamperEvent_flags-1);
    whichevent = mytmprdata.tamperEvent_flags & ~(mytmprdata.tamperEvent_flags-1);
//whichevent is a 32 bit number - we need to find out whichbit is 1

		temp_eventdata.event_code_bit=findWhichBitIsOne(whichevent);	// bit number 1 to 26 (31 max) - bit 0 is not used

	  if((whichevent==RTC_TRANSACTION_MASK)||(whichevent==MD_INTERVAL_TRANSACTION_MASK)||(whichevent==SURVEY_INTERVAL_TRANSACTION_MASK)||(whichevent==SINGLE_ACTION_TRANSACTION_MASK)||(whichevent==ACTIVITY_CALENDAR_TRANSACTION_MASK)||(whichevent==POWER_EVENT_MASK)||(whichevent==MODE_UNI_MASK)||(whichevent==MODE_BIDIR_MASK)||(whichevent==COVER_OPEN_MASK))
	  {// get the current data
			getCurrentInstantaneousData(&temp_eventdata);
			temp_eventdata.active_energy = myenergydata.kwh;
#ifdef EVENTDATA_RECORDS_KVAH
			temp_eventdata.apparent_energy = myenergydata.kvah;
#endif			
			temp_eventdata.datetime = rtc;
	  }
		else
		{// this is a regular tamper event - read the data from memory
			readInstantaneousData(&temp_eventdata, sizeof(temp_eventdata));	
		}
		
    if((mytmprdata.tamperEvent_on & whichevent) != 0)
      temp_eventdata.event_type =1;
		else
      temp_eventdata.event_type =0;

		if(whichevent == POWER_EVENT_MASK)
		{
			if(temp_eventdata.event_type == 0)
			{// power has failed
		    temp_eventdata.current_value_IR = 0;
		    temp_eventdata.current_value_IY = 0;
		    temp_eventdata.current_value_IB = 0;
		    
		    temp_eventdata.voltage_value_VR = 0;
		    temp_eventdata.voltage_value_VY = 0;
		    temp_eventdata.voltage_value_VB = 0;

		    temp_eventdata.power_factor_R = 0;
		    temp_eventdata.power_factor_Y = 0;
		    temp_eventdata.power_factor_B = 0;
			
				temp_eventdata.datetime.second=capturedRTC.second;	// this variable holds the seconds while power is there
				temp_eventdata.datetime.minute=capturedRTC.minute;
				temp_eventdata.datetime.hour=capturedRTC.hour;
				temp_eventdata.datetime.day=capturedRTC.day;
				temp_eventdata.datetime.date=capturedRTC.date;
				temp_eventdata.datetime.month=capturedRTC.month;
				temp_eventdata.datetime.year=capturedRTC.year;
			}
			else
			{// power has returned
				temp_eventdata.datetime = rtc;
				
				if(temp_eventdata.power_factor_R<0)
					temp_eventdata.power_factor_R*=-1;
				if(temp_eventdata.power_factor_Y<0)
					temp_eventdata.power_factor_Y*=-1;
				if(temp_eventdata.power_factor_B<0)
					temp_eventdata.power_factor_B*=-1;
			}
		}
    
// we now need to select which group to write the event into -
// we must ensure that temp_eventdata.event_code_bit value is always between [1 to 26]
		switch(temp_eventdata.event_code_bit)
		{
			case 0:
				break;
				
			case 1:	//	MISSING_POTENTIAL_R
			case 2:	//	MISSING_POTENTIAL_Y
			case 3:	//	MISSING_POTENTIAL_B
			case 4:	//	OVER_VOLTAGE
			case 5:	//	UNDER_VOLTAGE
			case 6:	//	VOLTAGE_UNBALANCED
				temp_event_wrt_ptr=mytmprdata.tamper_events0_wrt_ptr;
				temp_base_address=EVENTS0_BASE_ADDRESS;
				temp_end_address=EVENTS0_END_ADDRESS;
				whichGroup=0;
				break;
				
			case 7:	//	CT_REVERSED_R
			case 8:	//	CT_REVERSED_Y
			case 9:	//	CT_REVERSED_B
			case 10:	//	CT_OPEN_R
			case 11:	//	CT_OPEN_Y
			case 12:	//	CT_OPEN_B
			case 13:	//	CURRENT_UNBALANCED
			case 14:	//	CT_BYPASS
			case 15:	//	OVER_CURRENT
				temp_event_wrt_ptr=mytmprdata.tamper_events1_wrt_ptr;
				temp_base_address=EVENTS1_BASE_ADDRESS;
				temp_end_address=EVENTS1_END_ADDRESS;
				whichGroup=1;
				break;
				
			case 16:	//	POWER_EVENT
				temp_event_wrt_ptr=mytmprdata.tamper_events2_wrt_ptr;
				temp_base_address=EVENTS2_BASE_ADDRESS;
				temp_end_address=EVENTS2_END_ADDRESS;
				whichGroup=2;
				break;

			case 17:	//	RTC_TRANSACTION
			case 18:	//	MD_INTERVAL_TRANSACTION
			case 19:	//	SURVEY_INTERVAL_TRANSACTION
			case 20:	//	SINGLE_ACTION_TRANSACTION
			case 21:	//	ACTIVITY_CALENDAR_TRANSACTION
				temp_event_wrt_ptr=mytmprdata.tamper_events3_wrt_ptr;
				temp_base_address=EVENTS3_BASE_ADDRESS;
				temp_end_address=EVENTS3_END_ADDRESS;
				whichGroup=3;
				break;

			case 22:	//	MAGNET
			case 23:	//	AC_MAGNET
			case 24:	//	NEUTRAL_DISTURBANCE
			case 25:	//	LOW_PF
				temp_event_wrt_ptr=mytmprdata.tamper_events4_wrt_ptr;
				temp_base_address=EVENTS4_BASE_ADDRESS;
				temp_end_address=EVENTS4_END_ADDRESS;
				whichGroup=4;
				break;
				
			case 26:	//	COVER_OPEN
				temp_event_wrt_ptr=mytmprdata.tamper_events5_wrt_ptr;
				temp_base_address=EVENTS5_BASE_ADDRESS;
				temp_end_address=EVENTS5_END_ADDRESS;
				whichGroup=5;
				break;
		}
		
		if(temp_eventdata.event_code_bit!=0)
		{
			if(whichevent == POWER_EVENT_MASK)
			{// for power fail events 0 is for occurence
				if(temp_eventdata.event_type==0)
					g_Class01_EventCode[whichGroup]=findEventCode(temp_eventdata.event_code_bit, temp_eventdata.event_type);
			}
			else
			{// for other events 1 is for occurence - we are now recording both occurences and restorations hence foolowing if is commented
//				if(temp_eventdata.event_type!=0)
					g_Class01_EventCode[whichGroup]=findEventCode(temp_eventdata.event_code_bit, temp_eventdata.event_type);
			}
				
	    EPR_Write(temp_event_wrt_ptr, (uint8_t*)&temp_eventdata, sizeof(temp_eventdata));
	    incptr(&temp_event_wrt_ptr, temp_base_address, temp_end_address, EVENT_PTR_INC);
			g_Class07_Event_CurrentEntry[whichGroup]++;
	    if(temp_event_wrt_ptr==temp_base_address)
	    {// we have an overflow condition here- all the tamper locations have been written into
	      mytmprdata.tamperEvent_overflow|=_BV(whichGroup);  // set this flag for this group of events
				g_Class07_Event_CurrentEntry[whichGroup]=0;	// this was 1 earlier
	    }
			
			if((mytmprdata.tamperEvent_overflow&_BV(whichGroup))!=0)
				g_Class07_Event_EntriesInUse[whichGroup]=(temp_end_address-temp_base_address)/EVENT_PTR_INC;
			else
				g_Class07_Event_EntriesInUse[whichGroup]=g_Class07_Event_CurrentEntry[whichGroup];
			

// we must restore the wrt_ptr
			switch(whichGroup)
			{
				case 0:
					mytmprdata.tamper_events0_wrt_ptr=temp_event_wrt_ptr;
					break;
			
				case 1:
					mytmprdata.tamper_events1_wrt_ptr=temp_event_wrt_ptr;
					break;
			
				case 2:
					mytmprdata.tamper_events2_wrt_ptr=temp_event_wrt_ptr;
					break;
			
				case 3:
					mytmprdata.tamper_events3_wrt_ptr=temp_event_wrt_ptr;
					break;
			
				case 4:
					mytmprdata.tamper_events4_wrt_ptr=temp_event_wrt_ptr;
					break;
			
				case 5:
					mytmprdata.tamper_events5_wrt_ptr=temp_event_wrt_ptr;
					break;
			
			}
			
	    mytmprdata.tamperEvent_flags &= ~whichevent;	// whichevent is a 32 bit number
	// now that the pointers have been changed - we must save mytmprdata
	    write_alternately(MYTMPRDATA_BASE_ADDRESS, ALTERNATE_MYTMPRDATA_BASE_ADDRESS, (uint8_t*)&mytmprdata, sizeof(mytmprdata));
	    write_alternately(LATEST_EVENT_BASE_ADDRESS,ALTERNATE_LATEST_EVENT_BASE_ADDRESS,(uint8_t*)&g_Class01_EventCode,14);
	    if(mytmprdata.tamperEvent_flags == 0)
	      e2write_flags &= ~E2_W_EVENTDATA;
		}// end of 		if(temp_eventdata.event_code_bit!=0)
    return;
// in init_read it is sufficient to read mytmprdata    
  }// end of   if(e2write_flags&E2_W_EVENTDATA)
}


uint8_t verifyEnergyAreaInit(void)
{//rohit
  energydata_t temp_energy_data;
  uint32_t temp_ptr=ENERGIES_BASE_ADDRESS;
	uint8_t retval=0;
	uint8_t i;
  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
		
    EPR_Read(temp_ptr,(uint8_t*)&temp_energy_data,sizeof(temp_energy_data));
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		

    if(isCheckSumOK((uint8_t*)&temp_energy_data,sizeof(temp_energy_data))==0)
		{// here checksum is not ok
			retval=1;// Error
		}
  }
	return retval;
}

void clearRAMStructures(void)
{// clear everything in ram
  
// we should also update the e2rom and store properly checksummed kwh values

  energydata_t temp_energy_data;
	toddata_t temp_tod_data;
		
  uint32_t temp_ptr=ENERGIES_BASE_ADDRESS;

  uint8_t i;
  
  clear_structure((uint8_t*)&temp_energy_data,sizeof(temp_energy_data));
  temp_energy_data.chksum=calcChecksum((uint8_t*)&temp_energy_data, sizeof(temp_energy_data)-1); // temp energy data now has a properly zeroed record with checksum

  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
		EPR_Write(temp_ptr, (uint8_t*)&temp_energy_data, sizeof(temp_energy_data));
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
  }

// we need to do the same for toddata also - added on 10/Oct/2022

  clear_structure((uint8_t*)&temp_tod_data,sizeof(temp_tod_data));
	
	for(i=0;i<TARIFFS;i++)
	{// the zeroed records will be written with proper checksums
  	write_alternately(((int32_t)TOD_CURRENT_BASE_ADDRESS+(int32_t)i*(int32_t)ZONE_PTR_INC), ((int32_t)ALTERNATE_TOD_CURRENT_BASE_ADDRESS+(int32_t)i*(int32_t)ZONE_PTR_INC), (uint8_t*)&temp_tod_data, sizeof(temp_tod_data));
	}			
	
// what about default tod zone defs.
  

// now also do what we came to do i.e. clear the ram structures
	clear_structure((uint8_t*)&Common_Data,sizeof(Common_Data));
	clear_structure((uint8_t*)&ctldata,sizeof(ctldata));
	clear_structure((uint8_t*)&myenergydata,sizeof(myenergydata));
	clear_structure((uint8_t*)&mymddata,sizeof(mymddata));
	clear_structure((uint8_t*)&mytmprdata,sizeof(mytmprdata));
	clear_structure((uint8_t*)&mytoddata,sizeof(mytoddata));
	
//  clear_structure((uint8_t*)&rtc,sizeof(rtc));
// what about tamper data
//  Clear_Calib_Parms_in_RAM(0);
//  Clear_Calib_Parms_in_RAM(1);
//  Clear_Calib_Parms_in_RAM(2);
//  Clear_Neutral_Calib_Parms_in_RAM();
//  asendbare("DONE",port);  
//  send_message(port,tx_msg[port].len);
  
//  waitForSeconds(2);
  
//  kick_watchdog();
//  WDI_TOGGLE;
//    __disable_interrupt();  // we are clearing the watchdog in the interrupt routines - hence the necessity of diabling interrupts to force a reset
//  _DINT();  // both of these are the same

//	while(1); // force a reset - commented on 01/Nov/2018 for testing
}    


void clearE2RomCalibration(void)
{// clear calibration area
	uint8_t zarr[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint16_t addr = 0x0100;
	while(addr != 0x300)
	{
		R_WDT_Restart();
		EPR_Write(addr, zarr, 32);  // EEPROM Write 
		addr += 32;
		LCD_DisplayIntWithPos(addr,7,0);
	}
}

void clearE2Rom(void)
{// clear everything
//	int8_t check;
	uint8_t zarr[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//	uint16_t addr = 0x1000;
	uint16_t addr = 0x0300;
//	uint16_t addr = 0x0A00;
//	uint16_t addr = 0x0000;
	while(addr != 0x8000)
	{
		R_WDT_Restart();
		EPR_Write(addr, zarr, 32);  // EEPROM Write 
		addr += 32;
//		lcd_clearTopLine();
//		lcd_display_long(addr,5);
		LCD_DisplayIntWithPos(addr,7,0);
	}
}

void getFullMeterNumber(uint8_t* tbuf)
{// we need to read the alphanumeric meter number from memory into tbuf
	uint8_t buf[16];
	
	if(0==read_alternately(METER_NUMBER_BASE_ADDRESS, ALTERNATE_METER_NUMBER_BASE_ADDRESS, (uint8_t*)buf, 16))
	{// failure
		tbuf[0]=0x00;	// return a null string	
	}
	else
	{
		strcpy((char*)tbuf,(char*)buf);
	}
}

uint8_t bufCompare(uint8_t* buf, uint8_t*tbuf, uint8_t nBytes)
{
	uint8_t i;
	uint8_t retval;
	retval = 0;
	for (i=0;i<nBytes;i++)
	{
		if(buf[i]!=tbuf[i])
		{
			retval=1;
			i=nBytes;	// break
		}
	}
	return retval;
}

uint8_t checkMemory(uint32_t addr, uint8_t* buf, uint8_t*tbuf, uint8_t nBytes)
{// addr given is 0x0fffe, 0x1fffe and 0x2fffe
// memory should be 24C512 (64K)
// if instead 24C256 is used then the address will be mapped to 0x07ffe
// if instead 24C128 is used then the address will be mapped to 0x03ffe
// hence if we write two bytes at 0xnfffe, and the same bytes are read at 
// 0xn7fffe, or 0xn3fffe or 0xn1fffe or 0xn0fffe then we have an issue

	uint32_t temp_ptr;
	uint8_t retval;

//begin by writing at address, two bytes
	temp_ptr = addr;
	EPR_Write(temp_ptr, (uint8_t*)buf, nBytes);
	longDelay(1);
	tbuf[0]=0x00;
	tbuf[1]=0x00;
// now read at written address			
	EPR_Read(temp_ptr, (uint8_t*)tbuf, nBytes);	// read nBytes
	retval = bufCompare(buf,tbuf,nBytes);
	if(retval!=0)
		return retval;	// we probably do not have a chip installed
	
// here we seem to have written correctly at the address

// now check for 24C256
	temp_ptr&=~((uint32_t)BIT15);	// make bit 15 0
	tbuf[0]=0x00;
	tbuf[1]=0x00;
// now read at new address			
	EPR_Read(temp_ptr, (uint8_t*)tbuf, nBytes);	// read nBytes
	retval = bufCompare(buf,tbuf,nBytes);
	if(retval==0)
		return 1;	// if we read successfully at this address we have a problem
		
// now check for 24C128
	temp_ptr&=~((uint32_t)BIT14);	// make bit 14 0
	tbuf[0]=0x00;
	tbuf[1]=0x00;
// now read at new address			
	EPR_Read(temp_ptr, (uint8_t*)tbuf, nBytes);	// read nBytes
	retval = bufCompare(buf,tbuf,nBytes);
	if(retval==0)
		return 1;	// if we read successfully at this address we have a problem
		
// now check for 24C64
	temp_ptr&=~((uint32_t)BIT13);	// make bit 13 0
	tbuf[0]=0x00;
	tbuf[1]=0x00;
// now read at new address			
	EPR_Read(temp_ptr, (uint8_t*)tbuf, nBytes);	// read nBytes
	retval = bufCompare(buf,tbuf,nBytes);
	if(retval==0)
		return 1;	// if we read successfully at this address we have a problem
		
// now check for 24C32
	temp_ptr&=~((uint32_t)BIT12);	// make bit 12 0
	tbuf[0]=0x00;
	tbuf[1]=0x00;
// now read at new address			
	EPR_Read(temp_ptr, (uint8_t*)tbuf, nBytes);	// read nBytes
	retval = bufCompare(buf,tbuf,nBytes);
	if(retval==0)
		return 1;	// if we read successfully at this address we have a problem
	else
		return 0;	// success - we have the correct memory installed
}

void recordInstantaneousDateTimeEnergy(uint32_t flagmask)
{
  eventdata_t temp_eventdata;
	uint32_t addr;
	
	readRTC();

	temp_eventdata.event_code_bit = findWhichBitIsOne(flagmask);
	addr = TAMPER_SNAPSHOT_DATA_BASE_ADDRESS + (int32_t)temp_eventdata.event_code_bit*(int32_t)EVENT_PTR_INC;
	
	temp_eventdata.datetime=rtc;
  temp_eventdata.active_energy = myenergydata.kwh;

#ifdef EVENTDATA_RECORDS_KVAH
	temp_eventdata.apparent_energy = myenergydata.kvah;
  EPR_Write(addr, (uint8_t*)&temp_eventdata, 16);	// write 16 bytes only - datetime and energy
#else
  EPR_Write(addr, (uint8_t*)&temp_eventdata, 12);	// write 12 bytes only - datetime and energy
#endif
}


void recordInstantaneousData(uint32_t flagmask)
{// record data excluding datetime and energy which has already been recorded
  eventdata_t temp_eventdata;
	uint32_t addr;
	
	temp_eventdata.event_code_bit = findWhichBitIsOne(flagmask);

#ifdef EVENTDATA_RECORDS_KVAH
	addr = TAMPER_SNAPSHOT_DATA_BASE_ADDRESS + (int32_t)temp_eventdata.event_code_bit*(int32_t)EVENT_PTR_INC + (int32_t)16;
#else
	addr = TAMPER_SNAPSHOT_DATA_BASE_ADDRESS + (int32_t)temp_eventdata.event_code_bit*(int32_t)EVENT_PTR_INC + (int32_t)12;
#endif
	getCurrentInstantaneousData(&temp_eventdata);
// Now store this data
#ifdef EVENTDATA_RECORDS_KVAH
  EPR_Write(addr, (uint8_t*)&temp_eventdata.current_value_IR, (sizeof(temp_eventdata)-16));
#else
  EPR_Write(addr, (uint8_t*)&temp_eventdata.current_value_IR, (sizeof(temp_eventdata)-12));
#endif	
// the status of the event - occurence or restoration has not been stored
	
}

void readInstantaneousData(eventdata_t* temp_eventdata_ptr , int16_t datasize)
{
  uint8_t eventCodeBit;
	uint32_t addr;

	eventCodeBit = temp_eventdata_ptr->event_code_bit;	// save this code
	
	addr = TAMPER_SNAPSHOT_DATA_BASE_ADDRESS + (int32_t)eventCodeBit*(int32_t)EVENT_PTR_INC;
	EPR_Read(addr, (uint8_t*)temp_eventdata_ptr, datasize);
	
//	if(eventCodeBit==temp_eventdata_ptr->event_code_bit)
//		things are good
//	else
// 		things are not good
}

void getCurrentInstantaneousData(eventdata_t* temp_eventdata_ptr)
{
//	readRTC();
	uint16_t tempint;
#ifdef PVVNL_UNI_DIR_METER	
	int8_t regular_char;
#endif

	temp_eventdata_ptr->current_value_IR = (int16_t)(getInstantaneousParams(ICURRENTS,LINE_PHASE_R)*100.0);
  temp_eventdata_ptr->current_value_IY = (int16_t)(getInstantaneousParams(ICURRENTS,LINE_PHASE_Y)*100.0);
  temp_eventdata_ptr->current_value_IB = (int16_t)(getInstantaneousParams(ICURRENTS,LINE_PHASE_B)*100.0);
  
	tempint = (uint16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_R)*10.0);
	tempint = (tempint + 5)/10;
	temp_eventdata_ptr->voltage_value_VR = tempint%256;
	if (tempint>255)
			temp_eventdata_ptr->current_value_IR |= _BV(14);
	
	tempint = (uint16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*10.0);
	tempint = (tempint + 5)/10;
	temp_eventdata_ptr->voltage_value_VY = tempint%256;
	if (tempint>255)
			temp_eventdata_ptr->current_value_IY |= _BV(14);
	
	tempint = (uint16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_B)*10.0);
	tempint = (tempint + 5)/10;
	temp_eventdata_ptr->voltage_value_VB = tempint%256;
	if (tempint>255)
			temp_eventdata_ptr->current_value_IB |= _BV(14);
	
//	temp_eventdata_ptr->voltage_value_VR = (uint16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_R)*10.0);
//  temp_eventdata_ptr->voltage_value_VY = (uint16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_Y)*10.0);
//  temp_eventdata_ptr->voltage_value_VB = (uint16_t)(getInstantaneousParams(IVOLTS,LINE_PHASE_B)*10.0);

#ifdef PVVNL_UNI_DIR_METER	
	regular_char = (int8_t)(getInstantaneousParams(IPFS,LINE_PHASE_R)*100.0);	// 1000 earlier - now only two decimals
	if(regular_char<0)
		regular_char*=-1;
	temp_eventdata_ptr->power_factor_R = regular_char;
	
	regular_char = (int8_t)(getInstantaneousParams(IPFS,LINE_PHASE_Y)*100.0);	// 1000 earlier - now only two decimals
	if(regular_char<0)
		regular_char*=-1;
	temp_eventdata_ptr->power_factor_Y = regular_char;
	
	regular_char = (int8_t)(getInstantaneousParams(IPFS,LINE_PHASE_B)*100.0);	// 1000 earlier - now only two decimals
	if(regular_char<0)
		regular_char*=-1;
	temp_eventdata_ptr->power_factor_B = regular_char;
#else	
  temp_eventdata_ptr->power_factor_R = (int8_t)(getInstantaneousParams(IPFS,LINE_PHASE_R)*100.0);	// 1000 earlier - now only two decimals
  temp_eventdata_ptr->power_factor_Y = (int8_t)(getInstantaneousParams(IPFS,LINE_PHASE_Y)*100.0);
  temp_eventdata_ptr->power_factor_B = (int8_t)(getInstantaneousParams(IPFS,LINE_PHASE_B)*100.0);
#endif    
}

ext_rtc_t readInstantaneousDateTime(uint8_t eventCodeBit)
{
	uint32_t addr;
	ext_rtc_t retval;
	
	addr = TAMPER_SNAPSHOT_DATA_BASE_ADDRESS + (int32_t)eventCodeBit*(int32_t)EVENT_PTR_INC;
	EPR_Read(addr, (uint8_t*)&retval, sizeof(retval));
	return retval;
}







