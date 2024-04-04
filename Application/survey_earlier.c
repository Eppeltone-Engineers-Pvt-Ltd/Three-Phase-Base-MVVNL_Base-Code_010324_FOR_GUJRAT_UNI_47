// survey.c

/* Driver layer */
#include "r_cg_macrodriver.h"	// located in driver/mcu
#include "typedef.h"            // located right at top of tree below iodefine.h
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"

/* Application layer */
//#include "platform.h"
#include "emeter3_structs.h"
#include "emeter3_app.h"
#include "memoryOps.h"
//#include "r_lcd_display.h"      /* LCD Display Application Layer */
//#include "r_lcd_config.h"
//#include "em_display.h"         /* EM Display Application Layer */
//#include "event.h"
//#include "powermgmt.h"
#include "pravakComm.h"
#include "utilities.h"
#include "e2rom.h"
#include "survey.h"


#include "em_core.h"		// located in middleware/headers


extern uint32_t	g_Class07_Blockload_EntriesInUse;
extern uint16_t	g_Class07_Blockload_CurrentEntry;	

extern uint32_t	g_Class07_Dailyload_EntriesInUse;
extern uint16_t	g_Class07_Dailyload_CurrentEntry;

// How to compute the number of available records
// If the sur_overflow flag is set - then the 
// number of records = (SURVEY_END_ADDRESS - SURVEY_BASE_ADDRESS)/SURVEY_REC_SIZE = 432 (for eg : 9 days, 30 minutes interval
// Since survey_rec_size and number of days are fixed, it is better to define
// number of records = noOfDays*(1440/Common_Data.survey_interval) = 9*48 = 432 (this gives the same figure)
// by recomputing SURVEY_END_ADDRESS which would be required if the interval was changed and by choosing either
// 22 or 35 days depending on the interval being 15 or 30 we may compute the number of records if overflow flag is set.
//
//
// If overflow flag is not set
// no_of_records_per_day = 1440/Common_Data.survey_interval
// then number_of_full days = (ctldata.survey_wrt_ptr - SURVEY_BASE_ADDRESS)/(no_of_records_per_day*SURVEY_REC_SIZE)
// for the current day time_in_minutes = (rtc.hour*60+rtc.minute) - if 0 then 1440
// no_of_records_today = (time_in_minutes/Common_Data.survey_interval)
// hence number_of_records = number_of_full_days*(1440/Common_Data.survey_interval) + no_of_records_today

void do_survey_stuff(void)
{
// only ctldata.minutes is the correct value - everything else is the earlier value  

  surveydata_t surveyData;
  
  if(((int32_t)rtc.minute%(int32_t)Common_Data.survey_interval) == 0) 
//  if(((int32_t)rtc.minute%(int32_t)5) == 0) 
  {// here time is a multiple of survey_interval - we need to capture a survey record
// an individual survey record is 28 btyes long
// individual survey records will carry ext_rtc_t datetime
// compose the survey data
// actually these are supposed to be the average current/voltage during the interval and not the instantaneous value
		surveyData.datetime = rtc;
    surveyData.I_r = (uint16_t)(EM_GetCurrentRMS(LINE_PHASE_R)*100);	// current with 2 decimal places	
    surveyData.I_y = (uint16_t)(EM_GetCurrentRMS(LINE_PHASE_Y)*100);	// current with 2 decimal places	
    surveyData.I_b = (uint16_t)(EM_GetCurrentRMS(LINE_PHASE_B)*100);	// current with 2 decimal places	
    surveyData.V_r = (uint16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*100);	// voltage with 2 decimal places	
    surveyData.V_y = (uint16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*100);	// voltage with 2 decimal places	
    surveyData.V_b = (uint16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*100);	// voltage with 2 decimal places	
		
		if(total_energy_lastSaved>ctldata.last_saved_kwh)
	  	surveyData.kwh_consumption=(total_energy_lastSaved-ctldata.last_saved_kwh)/10;  // two decimals
		else
			surveyData.kwh_consumption=0;
			
		if(apparent_energy_lastSaved>ctldata.last_saved_kvah)
		  surveyData.kvah_consumption=(apparent_energy_lastSaved-ctldata.last_saved_kvah)/10; // two decimals
		else
			surveyData.kvah_consumption=0;
		
		if(reactive_energy_lag_lastSaved>ctldata.last_saved_kvarh_lag)
		  surveyData.kvarh_lag_consumption=(reactive_energy_lag_lastSaved-ctldata.last_saved_kvarh_lag)/10; // two decimals
		else
			surveyData.kvarh_lag_consumption=0;

		if(reactive_energy_lead_lastSaved>ctldata.last_saved_kvarh_lead)
		  surveyData.kvarh_lead_consumption=(reactive_energy_lead_lastSaved-ctldata.last_saved_kvarh_lead)/10; // two decimals
		else
			surveyData.kvarh_lead_consumption=0;

		
    write_surveydata((uint8_t*)&surveyData, SURVEY_REC_SIZE,rtc.hour,rtc.minute);
		

		
//lage haath daily_log bhi check kar lo		
//		if((rtc.hour==0)&&(rtc.minute==0)){
		// time to make a daily log
			
//		}
  }
}

void write_surveydata(uint8_t* sdata, uint8_t datasize, uint8_t shour , uint8_t sminute)
{
// for a 30 minute load survey - the first record at location 0 must be for 00:30 and the last record at location 47 must be for  24:00 hours	
  uint32_t temp_ptr;
	uint16_t timeInMinutes = shour*60+sminute;
	uint8_t localIndex;	// this is the position of the record within the day's data
	
	if(timeInMinutes==0)
		timeInMinutes=1440;
		
//	localIndex = (((int32_t)timeInMinutes/(int32_t)Common_Data.survey_interval) - (int32_t)1);
	localIndex = ((timeInMinutes/Common_Data.survey_interval) - 1);
  temp_ptr=ctldata.survey_wrt_ptr+((int32_t)localIndex*(int32_t)SURVEY_REC_SIZE);  // we are writing data from 0 itself
  //temp_ptr=ctldata.survey_wrt_ptr+((((int32_t)shour*(int32_t)60+(int32_t)sminute)/(int32_t)Common_Data.survey_interval)*(int32_t)SURVEY_REC_SIZE);  // we are writing data from 0 itself
  EPR_Write(temp_ptr, sdata, datasize);

  ctldata.last_saved_kwh=total_energy_lastSaved;
  ctldata.last_saved_kvah=apparent_energy_lastSaved;
  ctldata.last_saved_kvarh_lag=reactive_energy_lag_lastSaved;
  ctldata.last_saved_kvarh_lead=reactive_energy_lead_lastSaved;
	
	
  if((shour==0)&&(sminute==0))
  {// this is a new day - hence the pointer must be incremented
    incptr_maskless((uint32_t*)&ctldata.survey_wrt_ptr,SURVEY_BASE_ADDRESS,SURVEY_END_ADDRESS,(SURVEY_REC_SIZE*(1440/Common_Data.survey_interval)));
    if(ctldata.survey_wrt_ptr==SURVEY_BASE_ADDRESS)
    {// we have an overflow condition here- all the survey locations have been written into
      ctldata.overflow|=SUR_OVERFLOW;  // this value indicates that overflow has occured.
    }
  }
}

/*
// This function is of no use now
void write_survey_date(uint8_t pdate, uint8_t pmonth, uint8_t pyear)
{
  uint8_t arr[4];
  arr[0]=pdate;
  arr[1]=pmonth;
  arr[2]=pyear;
  
  EPR_Write(ctldata.survey_wrt_ptr + 768, arr, 3); // write immeditately after the data
}
*/

void do_survey_update(void)
{// only the last record will be updated

  surveydata_t surveyData;

	timeinfo_t lastTime;  // minute, hour, day, date, month, year - use this so that existing single phase incSurveyDate may be used.
//  unsigned int tempint;

  uint8_t this_interval = rtc.minute/(int32_t)Common_Data.survey_interval;
  uint8_t prev_interval = ctldata.minute/(int32_t)Common_Data.survey_interval;

	uint8_t indexDoneUptilNow;
	uint8_t indexReqdToBeDoneUptilNow;
	
	
// make a copy of ctldata time stamp
  lastTime.minute=ctldata.minute;
	if(lastTime.minute<30)
		lastTime.minute=0;
  lastTime.hour=ctldata.hour;
  lastTime.date=ctldata.date;
  lastTime.month=ctldata.month;
  lastTime.year=ctldata.year;
	lastTime.day = CalcDayNumFromDate((2000+lastTime.year), lastTime.month, lastTime.date);
  
  if((lastTime.year==rtc.year)&&(lastTime.month==rtc.month)&&(lastTime.date==rtc.date)&&(lastTime.hour==rtc.hour)&&(this_interval==prev_interval))
  {// here we know that 30 minute boundary has not been crossed in no power mode.
    return;
  }
  
  incDate((uint8_t*)&lastTime.minute, Common_Data.survey_interval); 

// compose the survey data    
// actually these are supposed to be the average current/voltage during the interval and not the instantaneous value
	surveyData.datetime.second = 0;
	surveyData.datetime.minute = lastTime.minute;
	surveyData.datetime.hour = lastTime.hour;
	surveyData.datetime.date = lastTime.date;
	surveyData.datetime.month = lastTime.month;
	surveyData.datetime.year = lastTime.year;
	surveyData.datetime.day = lastTime.day;



  surveyData.I_r = (uint16_t)(EM_GetCurrentRMS(LINE_PHASE_R)*100);	// current with 2 decimal places	
  surveyData.I_y = (uint16_t)(EM_GetCurrentRMS(LINE_PHASE_Y)*100);	// current with 2 decimal places	
  surveyData.I_b = (uint16_t)(EM_GetCurrentRMS(LINE_PHASE_B)*100);	// current with 2 decimal places	

  surveyData.V_r = (uint16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*100);	// voltage with 2 decimal places	
  surveyData.V_y = (uint16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*100);	// voltage with 2 decimal places	
  surveyData.V_b = (uint16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*100);	// voltage with 2 decimal places	

	if(total_energy_lastSaved>ctldata.last_saved_kwh)
  	surveyData.kwh_consumption=(total_energy_lastSaved-ctldata.last_saved_kwh)/10;  // two decimals
	else
		surveyData.kwh_consumption=0;
		
	if(apparent_energy_lastSaved>ctldata.last_saved_kvah)
	  surveyData.kvah_consumption=(apparent_energy_lastSaved-ctldata.last_saved_kvah)/10; // two decimals
	else
		surveyData.kvah_consumption=0;

	if(reactive_energy_lag_lastSaved>ctldata.last_saved_kvarh_lag)
	  surveyData.kvarh_lag_consumption=(reactive_energy_lag_lastSaved-ctldata.last_saved_kvarh_lag)/10; // two decimals
	else
		surveyData.kvarh_lag_consumption=0;

	if(reactive_energy_lead_lastSaved>ctldata.last_saved_kvarh_lead)
	  surveyData.kvarh_lead_consumption=(reactive_energy_lead_lastSaved-ctldata.last_saved_kvarh_lead)/10; // two decimals
	else
		surveyData.kvarh_lead_consumption=0;

//  if(surveyData.kwh_consumption<0)
//    surveyData.kwh_consumption=0;
  
//  if(surveyData.kvah_consumption<0)
//    surveyData.kvah_consumption=0;

  write_surveydata((uint8_t*)&surveyData, SURVEY_REC_SIZE,lastTime.hour,lastTime.minute);
	
// we have written the record for the last period for which power was available for a fraction of the interval
// now it is possible that since that time - several intervals for the current day may have lapsed
// thus we must do two things
// (a) if the date has not changed, then we must write zero records upto the interval now
// (b) and if the date has changed, then we must write zero records upto 00:00 hours for this day,
//		 then increment the ctldata.survey_wrt_ptr and go on filling zero records for today till the interval now.

	clear_structure((uint8_t*)&surveyData,SURVEY_REC_SIZE);
  if((lastTime.year==rtc.year)&&(lastTime.month==rtc.month)&&(lastTime.date==rtc.date))
	{// here the date is the same
		indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		indexReqdToBeDoneUptilNow = (uint8_t)((rtc.hour*60 + rtc.minute)/Common_Data.survey_interval);
		while(indexDoneUptilNow!=indexReqdToBeDoneUptilNow)
		{
  		incDate((uint8_t*)&lastTime.minute, Common_Data.survey_interval);
			surveyData.datetime.second = 0;
			surveyData.datetime.minute = lastTime.minute;
			surveyData.datetime.hour = lastTime.hour;
			surveyData.datetime.day = lastTime.day;
			surveyData.datetime.date = lastTime.date;
			surveyData.datetime.month = lastTime.month;
			surveyData.datetime.year = lastTime.year;
  		write_surveydata((uint8_t*)&surveyData, SURVEY_REC_SIZE,lastTime.hour,lastTime.minute); // fill zero records
			indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		}
	}
	else{// here the date has changed
		indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		indexReqdToBeDoneUptilNow = 0;
		// fill all the remaining records of lastDate with 0 till 00:00/24:00 hours
		while(indexDoneUptilNow!=indexReqdToBeDoneUptilNow)
		{
  		incDate((uint8_t*)&lastTime.minute, Common_Data.survey_interval); 
			surveyData.datetime.second = 0;
			surveyData.datetime.minute = lastTime.minute;
			surveyData.datetime.hour = lastTime.hour;
			surveyData.datetime.day = lastTime.day;
			surveyData.datetime.date = lastTime.date;
			surveyData.datetime.month = lastTime.month;
			surveyData.datetime.year = lastTime.year;

			write_surveydata((uint8_t*)&surveyData, SURVEY_REC_SIZE,lastTime.hour,lastTime.minute); // fill zero records
			indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		}
// after completion of the above while loop the lastTime.hour and minute would have become 0 and lastTime.date,month,year would be the next day
// however we now want to write records for today's date as give by rtc - hence
		lastTime.year=rtc.year;
		lastTime.month=rtc.month;
		lastTime.date=rtc.date;
		lastTime.day=rtc.day;
		
		indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		indexReqdToBeDoneUptilNow = (uint8_t)((rtc.hour*60 + rtc.minute)/Common_Data.survey_interval);
		while(indexDoneUptilNow!=indexReqdToBeDoneUptilNow)
		{
  		incDate((uint8_t*)&lastTime.minute, Common_Data.survey_interval); 
			surveyData.datetime.second = 0;
			surveyData.datetime.minute = lastTime.minute;
			surveyData.datetime.hour = lastTime.hour;
			surveyData.datetime.day = lastTime.day;
			surveyData.datetime.date = lastTime.date;
			surveyData.datetime.month = lastTime.month;
			surveyData.datetime.year = lastTime.year;

			write_surveydata((uint8_t*)&surveyData, SURVEY_REC_SIZE,lastTime.hour,lastTime.minute); // fill zero records
			indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		}
	// we are now current	- the ctldata time needs to be updated and the ctldata stored
	}
}
/*
struct logdata_struct
{// 16 bytes
	ext_rtc_t datetime;	// 8 bytes
	int32_t kWh_value;		// kWh at 00:00 hours  
	int32_t kVAh_value;		// kVAh at 00:00 hours
};
  uint8_t second,minute,hour,day,date,month,year;

*/

void writeLogData(uint8_t sdate,uint8_t smonth,uint8_t syear, uint8_t shour, uint8_t sminute, uint8_t ssecond)
{
  logdata_t logdata;
  unsigned int temp_ptr;

	logdata.datetime.second=ssecond;  
	logdata.datetime.minute=sminute;  
	logdata.datetime.hour=shour;  
//	logdata.datetime.day=0xff;  
	logdata.datetime.date=sdate;  
	logdata.datetime.month=smonth;  
	logdata.datetime.year=syear;  

	logdata.datetime.day = CalcDayNumFromDate((2000+syear), smonth, sdate);
	
	logdata.kWh_value = total_energy_lastSaved;
  logdata.kVAh_value = apparent_energy_lastSaved;

  EPR_Write(ctldata.daily_log_wrt_ptr, (uint8_t*)&logdata, sizeof(logdata));
  incptr_maskless((uint32_t*)&ctldata.daily_log_wrt_ptr,DAILYLOG_BASE_ADDRESS,DAILYLOG_END_ADDRESS,DAILYLOG_REC_SIZE);
  if(ctldata.daily_log_wrt_ptr==DAILYLOG_BASE_ADDRESS)
  {// we have an overflow condition here- all the survey locations have been written into
    ctldata.overflow|=DAILYLOG_OVERFLOW;  // this value indicates that overflow has occured.
  }
}

void do_log_update(void)
{// only the last record will be updated
  timeinfo_t lastTime;  // minute, hour, day, date, month, year - use this so that existing single phase incSurveyDate may be used.
//  unsigned int tempint;

// make a copy of ctldata time stamp
  lastTime.minute=ctldata.minute;
  lastTime.hour=ctldata.hour;
  lastTime.date=ctldata.date;
  lastTime.month=ctldata.month;
  lastTime.year=ctldata.year;
	lastTime.day = CalcDayNumFromDate((2000+ctldata.year), ctldata.month, ctldata.date);
  
  if((lastTime.year==rtc.year)&&(lastTime.month==rtc.month)&&(lastTime.date==rtc.date))
  {// here we know that date has not changed in no power mode.
    return;
  }
  
  lastTime.minute=30;
  lastTime.hour=23;
  
  incDate((uint8_t*)&lastTime.minute, 30); 
// now we have the new date after the power fail
  writeLogData(lastTime.date , lastTime.month, lastTime.year, lastTime.hour, lastTime.minute,0);

}


void computeLoadSurveyEntries(void){
//	g_Class07_Blockload_EntriesInUse = 400;
//	g_Class07_Blockload_CurrentEntry=1; // for testing
	
	int16_t timeMinutes = ctldata.hour*60+ctldata.minute;
	

	if(0==(ctldata.overflow&SUR_OVERFLOW)){
	// here there is no overflow
		g_Class07_Blockload_EntriesInUse = (((ctldata.survey_wrt_ptr-SURVEY_BASE_ADDRESS)/SURVEY_REC_SIZE)+(timeMinutes/Common_Data.survey_interval));
//		g_Class07_Blockload_CurrentEntry=(g_Class07_Blockload_EntriesInUse-1); // if there are no records then this value may be ffff
		g_Class07_Blockload_CurrentEntry=(g_Class07_Blockload_EntriesInUse); // if there are no records then this value may be ffff
	}
	else{
	// here overflow has occured
//		g_Class07_Blockload_EntriesInUse=(1440/Common_Data.survey_interval)*9;	// this 9 needs to be changed to 45/22 depending on Common_Data.survey_interval(30/15) 
		g_Class07_Blockload_EntriesInUse=(1440/Common_Data.survey_interval)*30;	// this 30 needs to be changed to 45/22 depending on Common_Data.survey_interval(30/15) 
		if(ctldata.survey_wrt_ptr==SURVEY_BASE_ADDRESS)
//			g_Class07_Blockload_CurrentEntry=g_Class07_Blockload_EntriesInUse-1;
			g_Class07_Blockload_CurrentEntry=g_Class07_Blockload_EntriesInUse;
		else
//			g_Class07_Blockload_CurrentEntry=(((ctldata.survey_wrt_ptr-SURVEY_BASE_ADDRESS)/SURVEY_REC_SIZE)-1);
			g_Class07_Blockload_CurrentEntry=(((ctldata.survey_wrt_ptr-SURVEY_BASE_ADDRESS)/SURVEY_REC_SIZE));
	}
	
}

void computeDailyLogEntries(void){

	if(0==(ctldata.overflow&DAILYLOG_OVERFLOW)){
	// here there is no overflow
		g_Class07_Dailyload_EntriesInUse = ((ctldata.daily_log_wrt_ptr-DAILYLOG_BASE_ADDRESS)/DAILYLOG_REC_SIZE);
//		g_Class07_Dailyload_CurrentEntry=(g_Class07_Dailyload_EntriesInUse-1); // if there are no records then this value may be ffff
		g_Class07_Dailyload_CurrentEntry=(g_Class07_Dailyload_EntriesInUse); // if there are no records then this value may be ffff
	}
	else{
	// here overflow has occured
		g_Class07_Dailyload_EntriesInUse= (DAILYLOG_END_ADDRESS - DAILYLOG_BASE_ADDRESS)/DAILYLOG_REC_SIZE;
		if(ctldata.daily_log_wrt_ptr==DAILYLOG_BASE_ADDRESS)
//			g_Class07_Dailyload_CurrentEntry=g_Class07_Dailyload_EntriesInUse-1;
			g_Class07_Dailyload_CurrentEntry=g_Class07_Dailyload_EntriesInUse;
		else
//			g_Class07_Dailyload_CurrentEntry=(((ctldata.daily_log_wrt_ptr-DAILYLOG_BASE_ADDRESS)/DAILYLOG_REC_SIZE)-1);
			g_Class07_Dailyload_CurrentEntry=(((ctldata.daily_log_wrt_ptr-DAILYLOG_BASE_ADDRESS)/DAILYLOG_REC_SIZE));
	}
	
}




