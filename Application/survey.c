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
#include "tampers.h"	// for _BV definition

#include "em_core.h"		// located in middleware/headers


extern uint32_t	g_Class07_Blockload_EntriesInUse;
extern uint16_t	g_Class07_Blockload_CurrentEntry;	

extern uint32_t	g_Class07_Dailyload_EntriesInUse;
extern uint16_t	g_Class07_Dailyload_CurrentEntry;

void ConvertRTC_shortDate(timeinfo_t*dst, ext_rtc_t*src)
{// the short_date_time format has six bytes for storing year, month, date, hour, minute, second
	dst->year=src->year;
	dst->month=src->month;
	dst->date = src->date;
	dst->hour=src->hour;
	dst->minute=src->minute;
	dst->day = src->day;
//	dst->second=src->second;
}

void ConvertShortDate_RTC(ext_rtc_t*dst, timeinfo_t*src)
{
	dst->year=src->year;
	dst->month=src->month;
	dst->date = src->date;
	dst->hour=src->hour;
	dst->minute=src->minute;
	dst->day = src->day;
	dst->second=0;
}

void initialiseSurveyDataSmall(void)
{
	#ifdef SURVEY_USES_AVERAGE_DATA	
	average_Ir=0;
	average_Iy=0;
	average_Ib=0;
	average_Vr=0;
	average_Vy=0;
	average_Vb=0;
	#endif	

	#ifdef SURVEY_USES_AVERAGE_PF
	averagePF_r=0;
	averagePF_y=0;
	averagePF_b=0;
	#endif	

	ctldata.last_saved_kwh=total_energy_lastSaved;
	ctldata.last_saved_kvah=apparent_energy_lastSaved;

	#ifdef SURVEY_USES_REACTIVE_DATA
	ctldata.last_saved_kvarh_lag=reactive_energy_lag_lastSaved;
	ctldata.last_saved_kvarh_lead=reactive_energy_lead_lastSaved;
	#endif

	#ifdef SURVEY_USES_EXPORT_DATA
	ctldata.last_saved_kwh_export=total_energy_lastSaved_export;
	#endif

	#ifdef SURVEY_USES_AVERAGE_DATA	
	nCount=0;
	#endif	

// The following options do not require any initialisation 
//#ifdef SURVEY_USES_FREQ_DATA
//#ifdef SURVEY_USES_DEMAND_DATA


//	no_load_time=0;
}

void populateSmallSurveyRecord(small_surveydata_t* surveyData)
{// this is for unidirectional case
	uint16_t tempint;
	uint16_t compareValue;
	
	compareValue = (72000)/(60/Common_Data.survey_interval);

	#ifdef SURVEY_USES_AVERAGE_DATA	
  surveyData->I_r = average_Ir;	// average current with 2 decimal places	
  surveyData->I_y = average_Iy;	// average current with 2 decimal places	
  surveyData->I_b = average_Ib;	// average current with 2 decimal places
	
	tempint = ((average_Vr+5)/10);
  surveyData->V_r = tempint%256;	// average voltage with 2 decimal places	
	if(tempint>255)
		surveyData->I_r |= _BV(14);	// set the bit 14 if Current - Voltage is not permitted to go beyond 511
		
	tempint = ((average_Vy+5)/10);
  surveyData->V_y = tempint%256;	// average voltage with 2 decimal places	
	if(tempint>255)
		surveyData->I_y |= _BV(14);	// set the bit 14 if Current - Voltage is not permitted to go beyond 511
		
	tempint = ((average_Vb+5)/10);
  surveyData->V_b = tempint%256;	// average voltage with 2 decimal places
	if(tempint>255)
		surveyData->I_b |= _BV(14);	// set the bit 14 if Current - Voltage is not permitted to go beyond 511
	#endif

	#ifdef SURVEY_USES_AVERAGE_PF
	surveyData->avgPF_r = averagePF_r;		
	surveyData->avgPF_y = averagePF_y;		
	surveyData->avgPF_b = averagePF_b;		
	#endif

	if(total_energy_lastSaved>ctldata.last_saved_kwh)
  	surveyData->kwh_consumption=(total_energy_lastSaved-ctldata.last_saved_kwh);  // two decimals
	else
		surveyData->kwh_consumption=0;
		
  if(surveyData->kwh_consumption>compareValue)
		surveyData->kwh_consumption=0;
		
		
	if(apparent_energy_lastSaved>ctldata.last_saved_kvah)
	  surveyData->kvah_consumption=(apparent_energy_lastSaved-ctldata.last_saved_kvah); // two decimals
	else
		surveyData->kvah_consumption=0;

  if(surveyData->kvah_consumption>compareValue)
		surveyData->kvah_consumption=0;
		
	#ifdef SURVEY_USES_REACTIVE_DATA	
	if(reactive_energy_lag_lastSaved>ctldata.last_saved_kvarh_lag)
	  surveyData->kvarh_lag_consumption=(reactive_energy_lag_lastSaved-ctldata.last_saved_kvarh_lag); // two decimals
	else
		surveyData->kvarh_lag_consumption=0;

  if(surveyData->kvarh_lag_consumption>compareValue)
		surveyData->kvarh_lag_consumption=0;
		
		
	if(reactive_energy_lead_lastSaved>ctldata.last_saved_kvarh_lead)
	  surveyData->kvarh_lead_consumption=(reactive_energy_lead_lastSaved-ctldata.last_saved_kvarh_lead); // two decimals
	else
		surveyData->kvarh_lead_consumption=0;
		
  if(surveyData->kvarh_lead_consumption>compareValue)
		surveyData->kvarh_lead_consumption=0;
		
	#endif

	
	#ifdef SURVEY_USES_EXPORT_DATA	
	if(total_energy_lastSaved_export>ctldata.last_saved_kwh_export)
	  surveyData->kwh_consumption_export=(total_energy_lastSaved_export-ctldata.last_saved_kwh_export); // two decimals
	else
		surveyData->kwh_consumption_export=0;		
		
  if(surveyData->kwh_consumption_export>compareValue)
		surveyData->kwh_consumption_export=0;
		
	#endif

	#ifdef SURVEY_USES_FREQ_DATA		
	surveyData->frequency = getInstantaneousParams(IFREQS,LINE_TOTAL)*10;
	#endif

	#ifdef SURVEY_USES_DEMAND_DATA
// surveyData->kwh_consumption and surveyData->kvah_consumption have already been computed above
	if(surveyData->kwh_consumption < 32500)
		surveyData->activeDemand = ((int32_t)surveyData->kwh_consumption) * ((int32_t)60/(int32_t)Common_Data.survey_interval);
	else
		surveyData->activeDemand = 65535;
	
	if(surveyData->kvah_consumption < 32500)
		surveyData->apparentDemand = ((int32_t)surveyData->kvah_consumption) * ((int32_t)60/(int32_t)Common_Data.survey_interval);
	else
		surveyData->apparentDemand = 65535;
	#endif

	#ifdef SURVEY_USES_AVERAGE_DATA
	nCount=0;
	#endif	
}

void do_survey_stuff_small(void)
{
	small_surveydata_t smallSurveyData;
// the if is being checked outside in per_minute activity	
	ConvertRTC_shortDate(&smallSurveyData.shortDateTime , &rtc);
	populateSmallSurveyRecord(&smallSurveyData);
  write_surveydata((uint8_t*)&smallSurveyData, sizeof(smallSurveyData),rtc.hour,rtc.minute);
}

void do_survey_update_small(void)
{
  small_surveydata_t surveyData;
	timeinfo_t lastTime;  // minute, hour, day, date, month, year - use this so that existing single phase incSurveyDate may be used.
//  unsigned int tempint;

  uint8_t this_interval = rtc.minute/(int32_t)Common_Data.survey_interval;
  uint8_t prev_interval = ctldata.minute/(int32_t)Common_Data.survey_interval;

	uint8_t indexDoneUptilNow;
	uint8_t indexReqdToBeDoneUptilNow;
	
// make a copy of ctldata time stamp
//  lastTime.minute=ctldata.minute;
	lastTime.minute =	(int32_t)prev_interval* (int32_t)Common_Data.survey_interval;
// by using the above expresseion - the next two steps won't be necessary	
//	if(lastTime.minute<30)
//		lastTime.minute=0;
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
//	surveyData.shortDateTime.second = 0;
	surveyData.shortDateTime.minute = lastTime.minute;
	surveyData.shortDateTime.hour = lastTime.hour;
	surveyData.shortDateTime.day = lastTime.day;
	surveyData.shortDateTime.date = lastTime.date;
	surveyData.shortDateTime.month = lastTime.month;
	surveyData.shortDateTime.year = lastTime.year;

	populateSmallSurveyRecord(&surveyData);
  write_surveydata((uint8_t*)&surveyData, sizeof(surveyData),lastTime.hour,lastTime.minute);
	
// we have written the record for the last period for which power was available for a fraction of the interval
// now it is possible that since that time - several intervals for the current day may have lapsed
// thus we must do two things
// (a) if the date has not changed, then we must write zero records upto the interval now
// (b) and if the date has changed, then we must write zero records upto 00:00 hours for this day,
//		 then increment the ctldata.survey_wrt_ptr and go on filling zero records for today till the interval now.

	clear_structure((uint8_t*)&surveyData,sizeof(surveyData));
//	surveyData.noSupplyTime = Common_Data.survey_interval;	// this entire period there was no supply
  if((lastTime.year==rtc.year)&&(lastTime.month==rtc.month)&&(lastTime.date==rtc.date))
	{// here the date is the same
		indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		indexReqdToBeDoneUptilNow = (uint8_t)((rtc.hour*60 + rtc.minute)/Common_Data.survey_interval);
		while(indexDoneUptilNow!=indexReqdToBeDoneUptilNow)
		{
  		incDate((uint8_t*)&lastTime.minute, Common_Data.survey_interval);
//			surveyData.shortDateTime.second = 0;
			surveyData.shortDateTime.minute = lastTime.minute;
			surveyData.shortDateTime.hour = lastTime.hour;
			surveyData.shortDateTime.day = lastTime.day;
			surveyData.shortDateTime.date = lastTime.date;
			surveyData.shortDateTime.month = lastTime.month;
			surveyData.shortDateTime.year = lastTime.year;
//			surveyData.noSupplyTime = Common_Data.survey_interval;
			write_surveydata((uint8_t*)&surveyData, sizeof(surveyData),lastTime.hour,lastTime.minute); // fill zero records
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
//			surveyData.shortDateTime.second = 0;
			surveyData.shortDateTime.minute = lastTime.minute;
			surveyData.shortDateTime.hour = lastTime.hour;
			surveyData.shortDateTime.day = lastTime.day;
			surveyData.shortDateTime.date = lastTime.date;
			surveyData.shortDateTime.month = lastTime.month;
			surveyData.shortDateTime.year = lastTime.year;
//			surveyData.noSupplyTime = Common_Data.survey_interval;
			write_surveydata((uint8_t*)&surveyData, sizeof(surveyData),lastTime.hour,lastTime.minute); // fill zero records
			indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		}
// For the Gujarat Board case, upon exiting from the above while loop we would have executed the case for 00:00 hours,
// the write_surveydata function would have incremented the ctldata.survey_wrt_ptr by 256 bytes to point to the next
// page. However the current date would not have been written into it.
// The log_update function will also not help as it will be using the ctldata_wrt_ptr and will still be using the
// ctldata.date month etc which have not been updated yet

// Since the ctldata.survey_wrt_ptr is already implemented we can
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
//			surveyData.shortDateTime.second = 0;
			surveyData.shortDateTime.minute = lastTime.minute;
			surveyData.shortDateTime.hour = lastTime.hour;
			surveyData.shortDateTime.day = lastTime.day;
			surveyData.shortDateTime.date = lastTime.date;
			surveyData.shortDateTime.month = lastTime.month;
			surveyData.shortDateTime.year = lastTime.year;
//			surveyData.noSupplyTime = Common_Data.survey_interval;
			write_surveydata((uint8_t*)&surveyData, sizeof(surveyData),lastTime.hour,lastTime.minute); // fill zero records
			indexDoneUptilNow = (uint8_t)((lastTime.hour*60 + lastTime.minute)/Common_Data.survey_interval);
		}
	// we are now current	- the ctldata time needs to be updated and the ctldata stored
	}
}



//-----------------------------------------------------------------------------------------------------------
void write_surveydata(uint8_t* sdata, uint8_t datasize, uint8_t shour , uint8_t sminute)
{
// for a 30 minute load survey - the first record at location 0 must be for 00:30 and the last record at location 47 must be for  24:00 hours	
  uint32_t temp_ptr;
	uint16_t timeInMinutes = shour*60+sminute;
	uint8_t localIndex;	// this is the position of the record within the day's data
	uint16_t ptr_inc;

// SURVEY_PTR_INC must be ALWAYS DEFINED for each type of meter	
	ptr_inc = SURVEY_PTR_INC;
	
	if(timeInMinutes==0)
		timeInMinutes=1440;

  EPR_Write(ctldata.survey_wrt_ptr, sdata, datasize);
  incptr_maskless(&ctldata.survey_wrt_ptr, SURVEY_BASE_ADDRESS, SURVEY_END_ADDRESS, ptr_inc);
  if(ctldata.survey_wrt_ptr==SURVEY_BASE_ADDRESS)
  {// we have an overflow condition here- all the survey locations have been written into
    ctldata.overflow|=SUR_OVERFLOW;  // this value indicates that overflow has occured.
  }

  ctldata.last_saved_kwh=total_energy_lastSaved;
  ctldata.last_saved_kvah=apparent_energy_lastSaved;
	
#ifdef SURVEY_USES_REACTIVE_DATA
	ctldata.last_saved_kvarh_lag=reactive_energy_lag_lastSaved;
  ctldata.last_saved_kvarh_lead=reactive_energy_lead_lastSaved;
#endif

#ifdef BIDIR_METER	
  ctldata.last_saved_kwh_export=total_energy_lastSaved_export;
  ctldata.last_saved_kvah_export=apparent_energy_lastSaved_export;
  ctldata.last_saved_kvarh_lag_export=reactive_energy_lag_lastSaved_export;
  ctldata.last_saved_kvarh_lead_export=reactive_energy_lead_lastSaved_export;
#endif	
}

void writeLogData(uint8_t sdate,uint8_t smonth,uint8_t syear, uint8_t shour, uint8_t sminute, uint8_t ssecond)
{
  logdata_t logdata;
	uint32_t temp_ptr;
//  unsigned int temp_ptr;

	logdata.datetime.second=ssecond;  
	logdata.datetime.minute=sminute;  
	logdata.datetime.hour=shour;  
//	logdata.datetime.day=0xff;  
	logdata.datetime.date=sdate;  
	logdata.datetime.month=smonth;  
	logdata.datetime.year=syear;  

	logdata.datetime.day = CalcDayNumFromDate((2000+syear), smonth, sdate);
	
	logdata.kWh_value = total_energy_lastSaved;
//	logdata.kVAh_value = apparent_energy_lastSaved;	// being done as the #else of the following if

#ifdef BIDIR_METER
	logdata.kWh_value_export = total_energy_lastSaved_export;
#else
	logdata.kVAh_value = apparent_energy_lastSaved;
#endif

#ifndef SMALL_DAILY_LOG
	#ifdef DAILY_POWER_ON_LOGGING	
		logdata.dailyPonMinutes = myenergydata.dailyPowerOnMinutes;
		myenergydata.dailyPowerOnMinutes=0;
	  myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);	
	#else
		logdata.kVArh_lag_value = reactive_energy_lag_lastSaved;
		logdata.kVArh_lead_value = reactive_energy_lead_lastSaved;
	#endif
#endif

//	logdata.kVAh_value_export = apparent_energy_lastSaved_export;
//	logdata.kVARh_lag_value_export = reactive_energy_lag_lastSaved_export;
//	logdata.kVARh_lead_value_export = reactive_energy_lead_lastSaved_export;
	
  EPR_Write(ctldata.daily_log_wrt_ptr, (uint8_t*)&logdata, sizeof(logdata));
  incptr_maskless((uint32_t*)&ctldata.daily_log_wrt_ptr,DAILYLOG_BASE_ADDRESS,DAILYLOG_END_ADDRESS,DAILY_LOG_PTR_INC);
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

void computeLoadSurveyEntries(uint8_t hr, uint8_t min){
	int16_t timeMinutes = hr*60 + min;
	
	uint16_t ptr_inc;
	int16_t no_of_survey_records;

// SURVEY_PTR_INC must be ALWAYS DEFINED for each type of meter	
	ptr_inc=SURVEY_PTR_INC;
	no_of_survey_records = NO_OF_SURVEY_RECORDS;

	if(0==(ctldata.overflow&SUR_OVERFLOW)){
		g_Class07_Blockload_EntriesInUse = (((int32_t)ctldata.survey_wrt_ptr-(int32_t)SURVEY_BASE_ADDRESS)/(int32_t)ptr_inc);
		g_Class07_Blockload_CurrentEntry=(g_Class07_Blockload_EntriesInUse); // if there are no records then this value may be ffff
	}
	else{
		g_Class07_Blockload_EntriesInUse=no_of_survey_records;
		if(ctldata.survey_wrt_ptr==SURVEY_BASE_ADDRESS)
			g_Class07_Blockload_CurrentEntry=g_Class07_Blockload_EntriesInUse;
		else
			g_Class07_Blockload_CurrentEntry=((((int32_t)ctldata.survey_wrt_ptr-(int32_t)SURVEY_BASE_ADDRESS)/(int32_t)ptr_inc));
	}
}

void computeDailyLogEntries(void){
	if(0==(ctldata.overflow&DAILYLOG_OVERFLOW)){
		g_Class07_Dailyload_EntriesInUse = (((int32_t)ctldata.daily_log_wrt_ptr-(int32_t)DAILYLOG_BASE_ADDRESS)/(int32_t)DAILY_LOG_PTR_INC);
		g_Class07_Dailyload_CurrentEntry=(g_Class07_Dailyload_EntriesInUse); // if there are no records then this value may be ffff
	}
	else{
		g_Class07_Dailyload_EntriesInUse= ((int32_t)DAILYLOG_END_ADDRESS - (int32_t)DAILYLOG_BASE_ADDRESS)/(int32_t)DAILY_LOG_PTR_INC;
		if(ctldata.daily_log_wrt_ptr==DAILYLOG_BASE_ADDRESS)
			g_Class07_Dailyload_CurrentEntry=g_Class07_Dailyload_EntriesInUse;
		else
			g_Class07_Dailyload_CurrentEntry=((((int32_t)ctldata.daily_log_wrt_ptr-(int32_t)DAILYLOG_BASE_ADDRESS)/(int32_t)DAILY_LOG_PTR_INC));
	}
}



