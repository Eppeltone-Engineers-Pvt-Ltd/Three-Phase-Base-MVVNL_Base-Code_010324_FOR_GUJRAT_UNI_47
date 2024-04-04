// tod.c
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "typedef.h"            // located right at top of tree below iodefine.h
#include "tod.h"
//#include "emeter3_structs.h"
#include "r_dlms_data.h"	// for ConvertRTCDate

extern class20_child_record_t g_ChildTableClass20[];

extern stime_t TZ_start_time_active0[];
extern stime_t TZ_start_time_active1[];
extern stime_t TZ_start_time_active2[];
extern stime_t TZ_start_time_active3[];

extern day_profile_action_t	day_action_active0[];
extern day_profile_action_t	day_action_active1[];
extern day_profile_action_t	day_action_active2[];
extern day_profile_action_t	day_action_active3[];

extern day_profile_t g_Class20_day_profile_active[];

extern week_profile_t	g_Class20_week_profile_active[];

extern date_time_t	g_Class20_start_time_active1;
extern date_time_t	g_Class20_start_time_active2;


extern stime_t TZ_start_time_passive0[];
extern stime_t TZ_start_time_passive1[];
extern stime_t TZ_start_time_passive2[];
extern stime_t TZ_start_time_passive3[];

extern day_profile_action_t	day_action_passive0[];
extern day_profile_action_t	day_action_passive1[];
extern day_profile_action_t	day_action_passive2[];
extern day_profile_action_t	day_action_passive3[];

extern week_profile_t	g_Class20_week_profile_passive[];

extern date_time_t	g_Class20_start_time_passive1;
extern date_time_t	g_Class20_start_time_passive2;

extern ext_rtc_t rtc;	// this is the rtc used in our app


//extern void ConvertBackToRTC(ext_rtc_t*dst, date_time_t*src);
//extern void ConvertRTCDate(date_time_t*dst, ext_rtc_t*src);

//uint8_t findSeason(void);
uint8_t findSeason(ext_rtc_t *rtcPtr);
//uint8_t findSeason(uint8_t gdate, uint8_t gmonth, uint8_t ghour, uint8_t gminute);

//uint8_t findDayID(uint8_t season);
uint8_t findDayID(uint8_t season, uint8_t wday);

uint8_t findLastZone(uint8_t *scripts);
uint8_t IsGreaterOrEqual(uint16_t presentTime, uint16_t zoneTime);
uint8_t IsLess(uint16_t presentTime, uint16_t zoneTime);


// the following are private functions
uint8_t IsGreaterOrEqual(uint16_t presentTime, uint16_t zoneTime)
{
	zoneTime = zoneTime & 0x0fff;	// remove the tariff register nibble
	
// 0 is a special case so if someone has put 24:00 hours then we must treat it as 0

	if(zoneTime >= 1440)	// the greater than sign is for those great persons who may type 24:59
		zoneTime = 0;
		
	if(presentTime>=zoneTime)
		return 1;
	else
		return 0;
}

uint8_t IsLess(uint16_t presentTime, uint16_t zoneTime)
{
	zoneTime = zoneTime & 0x0fff;	// remove the tariff register nibble
	
// 0 is a special case so if someone has put 00:00 hours then we must treat it as 24:00
	if(zoneTime == 0)
		zoneTime = 1440;

	if(presentTime<zoneTime)
		return 1;
	else
		return 0;	

}

//uint8_t findSeason(uint8_t gdate, uint8_t gmonth, uint8_t ghour, uint8_t gminute)
uint8_t findSeason(ext_rtc_t *rtcPtr)
{// we need to compare the present date with the starting times of the two seasons
// It is important to note that the g_Class20_start_time_active1 must be an earlier date than g_Class20_start_time_active2
	date_time_t currentTime;
	int8_t match_result1, match_result2;
	uint8_t retval = 0;
	
	currentTime.day_of_month = rtcPtr->date;
	currentTime.month = rtcPtr->month;
	currentTime.hour = rtcPtr->hour;
	currentTime.minute = rtcPtr->minute;
	
//	ConvertRTCDate(&currentTime, &rtc);
	
	currentTime.year_high = (uint8_t)NOT_SPECIFIED;
	currentTime.year_low = (uint8_t)NOT_SPECIFIED;	
	currentTime.day_of_week = (uint8_t)NOT_SPECIFIED;

	if((g_Class20_start_time_active1.day_of_month==0)||(g_Class20_start_time_active1.month==0))
	{
		return 0;
	}

	if((g_Class20_start_time_active2.day_of_month==0)||(g_Class20_start_time_active2.month==0))
	{
		return 0;
	}
	
	match_result1=R_OBIS_CompareDateTime((uint8_t*)&currentTime,(uint8_t*)&g_Class20_start_time_active1); 
	if(match_result1==0)
	{
		retval = 0; // since the date and time etc matches it is season 0
	}
	else
	{// here currentTime is either greater than or less than g_Class20_start_time_active1
		if(match_result1>0)
		{// here currentTIme > g_Class20_start_time_active1
			// we need to determine if it is less than g_Class20_start_time_active2
			match_result2=R_OBIS_CompareDateTime((uint8_t*)&currentTime,(uint8_t*)&g_Class20_start_time_active2);
			if(match_result2<0)
			{// here currentTime is less than the start time of the next season
				retval=0;
			}
			else
			{// here current time >= g_Class20_start_time_active2
				retval=1;
			}
		}
		else
		{// here match_result1 is <0 i.e. currentTime is < g_Class20_start_time_active1
			retval=1;
		}
	}
	return retval;
}

uint8_t findDayID(uint8_t season, uint8_t wday)
{
	int i;
	uint8_t *ptr0;
	uint8_t retval;
	
	if(season==0)
		ptr0 = (uint8_t*)&g_Class20_week_profile_active[0].week_profile_name_len;
	else
		ptr0 = (uint8_t*)&g_Class20_week_profile_active[1].week_profile_name_len;
	
	ptr0+=wday;	// this is the day of the week
	retval=*ptr0;
	if(retval==0)
		retval=1;
	if(retval>4)
		retval=1;
	return retval;
}

uint8_t findLastZone(uint8_t *scripts)
{
	uint8_t  retval;
	uint8_t i;

	retval=7;	// if all 8 zones are defined then we must return 7
	
	if(scripts[0]==0)	// if the first zone itself is not defined return 0
		retval=0;
	else{	
		for(i=1;i<8;i++)
		{
			if(scripts[i]==0)	// if the ith zone script selector is 0, return i-1
			{ // mask off the register bits
				retval=i-1;
				break;
			}
		}
	}
	return retval;
}

uint8_t findZone(ext_rtc_t *rtcPtr)
{// this fellow is rewritten completely - will return values 0,1,2,3,4,5,6,7,8 only

	day_profile_t *day_profile_ptr;
	day_profile_action_t *day_profile_action_ptr;

	uint16_t tempMinutes;
	uint16_t zoneTimes[8];
	uint8_t scripts[8];
	uint8_t i, j, noOfdaySchedule;
	uint8_t retval;
	uint8_t tempchar;
	uint8_t season;
	uint8_t dayId;
	uint8_t foundFlag;
	
// the first step is to find what the season is
	
//	season = findSeason(gdate,gmonth,ghour,gminute);	// return 0 or 1
//	season = findSeason(rtcPtr->date,rtcPtr->month,rtcPtr->hour,rtcPtr->minute);	// return 0 or 1
	season = findSeason(rtcPtr);	// return 0 or 1
	
	dayId = findDayID(season, rtcPtr->day);
	
	foundFlag = 0;
	day_profile_ptr = &g_Class20_day_profile_active[0];	// there is only one day_profile table
	
	for(i=0;i<g_ChildTableClass20[0].nr_day_profile_active;i++)
	{
		if(day_profile_ptr->day_id == dayId)
		{
			foundFlag = 1;
			break;
		}
		else
		{
			day_profile_ptr++;
		}
	}
	
	if(foundFlag==0)
	{// for some reason the dayID was not found - use the first entry of the day_profile
		day_profile_ptr = &g_Class20_day_profile_active[0];	// there is only one day_profile table
	}

// now create the time_array and the scripts array	
	
	day_profile_action_ptr = day_profile_ptr->day_schedule;
	noOfdaySchedule = day_profile_ptr->nr_day_schedule;
	
	for(j=0;j<noOfdaySchedule;j++)
	{
		tempMinutes = (day_profile_action_ptr->p_start_time->hour)*60 + day_profile_action_ptr->p_start_time->minute;
		if(tempMinutes>=1440)
			tempMinutes = 1440;
		zoneTimes[j]= tempMinutes;
		scripts[j] = (day_profile_action_ptr->script_selector);
		if(scripts[j]>TARIFFS)
			scripts[j]=TARIFFS;	// all scripts >TARIFFS will be stored in TARIFF
		day_profile_action_ptr++;
	}
	
// we now have the time data and the scripts in the two arrays	
	
	tempMinutes = rtcPtr->hour*60+rtcPtr->minute;
	
	retval = (uint8_t)scripts[0];
	if(scripts[0]==0)
    return retval; // 0
  else{// here the first zone script selector is not 0 - however the time in minutes could still be 0
		retval = (uint8_t)(scripts[0]-1);
		if(IsGreaterOrEqual(tempMinutes,zoneTimes[0])!=0)
		{// check the next zone
			if(scripts[1]==0)	// if the next zone is not defined then
				return retval;	// (scripts[0]-1)
			else{// here the next zone is also defined
				if(IsLess(tempMinutes,zoneTimes[1])!=0)
					return retval;	// (scripts[0]-1)
				else{// here time is >=zoneTimes[1]
					retval = (uint8_t)(scripts[1]-1);
					if(zoneTimes[2]==0)	// if the next zone is not defined then
						return retval;	// (scripts[1]-1)
					else{//here the next zone is defined
						if(IsLess(tempMinutes,zoneTimes[2])!=0)
							return retval;	// (scripts[1]-1)
						else{// here time is >=zoneTimes[2]
							retval = (uint8_t)(scripts[2]-1);
							if(scripts[3]==0)	// if the next zone is not defined then
								return retval;	// (scripts[2]-1)
							else{//here the next zone is defined
								if(IsLess(tempMinutes,zoneTimes[3])!=0)
									return retval;	// (scripts[2]-1)
								else{// here time is >=zonedef[3]
									retval = (uint8_t)(scripts[3]-1);
									if(scripts[4]==0)	// if the next zone is not defined then
										return retval;	// (scripts[4]-1)
									else{//here the next zone is defined
										if(IsLess(tempMinutes,zoneTimes[4])!=0)
											return retval;	// (scripts[4]-1)
										else{// here time is >=zonedef[4]
											retval = (uint8_t)(scripts[4]-1);
											if(scripts[5]==0)	// if the next zone is not defined then
												return retval;	// (scripts[4]-1)
											else{//here the next zone is defined
												if(IsLess(tempMinutes,zoneTimes[5])!=0)
													return retval;	// (scripts[4]-1)
												else{// here time is >=zonedef[5]
													retval = (uint8_t)(scripts[5]-1);
													if(scripts[6]==0)	// if the next zone is not defined then
														return retval;	// (scripts[5]-1)
													else{//here the next zone is defined
														if(IsLess(tempMinutes,zoneTimes[6])!=0)
															return retval;	// (scripts[5]-1)
														else{// here time is >=zonedef[6]
															retval = (uint8_t)(scripts[6]-1);
															if(scripts[7]==0)	// if the next zone is not defined then
																return retval;	// (scripts[6]-1)
															else{//here the next zone is defined
																if(IsLess(tempMinutes,zoneTimes[7])!=0)
																	return retval;	// (scripts[6]-1)
																else
																// here time is >=zonedef[7]
																// there is no timezone after this - hence if time is more than zone 7
																// or it is less than zone 7 - it belongs to zone 7
																	retval = (uint8_t)(scripts[7]-1);
																	return retval;	// (scripts[7]-1)
															}
														}														
													}
												}
											}								
										}
									}								
								}				
							}
						}
					}
				}
			}
		}
		else{// here the time is less than the first zone value - it thus belongs to the last zone
			tempchar = findLastZone(&scripts[0]);
			retval = (uint8_t)(scripts[tempchar]-1);
			return retval;
		}
  }// first else
}



void loadActiveData(class20_total_data_t *total_data)
{// This is a reverse function - it fills the RAM variables from the total_data
	day_profile_action_t *day_profile_action_ptr;
	date_time_t tempDateTime;
	Unsigned8 *ptr0;	// for accessing the day types for the seven days of the summer week
	Unsigned8 *ptr1;	// for accessing the day types for the seven days of the winter week
	int i;	

	for(i=0;i<8;i++)
	{
		TZ_start_time_active0[i] = total_data->start_time0[i];
		TZ_start_time_active1[i] = total_data->start_time1[i];
		TZ_start_time_active2[i] = total_data->start_time2[i];
		TZ_start_time_active3[i] = total_data->start_time3[i];
	}
	
	day_profile_action_ptr = &day_action_active0[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_0[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_active0[i];
		day_profile_action_ptr++;
	}
	
	day_profile_action_ptr = &day_action_active1[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_1[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_active1[i];
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_active2[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_2[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_active2[i];
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_active3[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_3[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_active3[i];
		day_profile_action_ptr++;
	}
	
	for (i=1;i<8;i++)
	{
		ptr0 = (Unsigned8*)&g_Class20_week_profile_active[0].week_profile_name_len;
		ptr0+=i;
		*ptr0 = total_data->week_summer[i-1]; 
		
		ptr1 = (Unsigned8*)&g_Class20_week_profile_active[1].week_profile_name_len;
		ptr1+=i;
		*ptr1 = total_data->week_winter[i-1];			
	}

	
	ConvertRTCDate(&tempDateTime,&total_data->g_Class20_start_time_1);	// convert rtc_t to date_time_t
	tempDateTime.year_high=0;
	tempDateTime.year_low=0;
	tempDateTime.day_of_week=0;
	memcpy((uint8_t*)&g_Class20_start_time_active1,(uint8_t *)&tempDateTime,sizeof(date_time_t));
	
	ConvertRTCDate(&tempDateTime,&total_data->g_Class20_start_time_2);	// convert rtc_t to date_time_t
	tempDateTime.year_high=0;
	tempDateTime.year_low=0;
	tempDateTime.day_of_week=0;
	memcpy((uint8_t*)&g_Class20_start_time_active2,(uint8_t *)&tempDateTime,sizeof(date_time_t));
	
	
//	memcpy((uint8_t*)&g_Class20_start_time_active1,(uint8_t *)&total_data->g_Class20_start_time_1,sizeof(date_time_t));
//	memcpy((uint8_t*)&g_Class20_start_time_active2,(uint8_t *)&total_data->g_Class20_start_time_2,sizeof(date_time_t));
		
}

void loadPassiveData(class20_total_data_t *total_data)
{// This is a reverse function - it fills the RAM variables from the total_data
	day_profile_action_t *day_profile_action_ptr;
	date_time_t tempDateTime;
	Unsigned8 *ptr0;	// for accessing the day types for the seven days of the summer week
	Unsigned8 *ptr1;	// for accessing the day types for the seven days of the winter week
	int i;	
	

	for(i=0;i<8;i++)
	{
		TZ_start_time_passive0[i] = total_data->start_time0[i];
		TZ_start_time_passive1[i] = total_data->start_time1[i];
		TZ_start_time_passive2[i] = total_data->start_time2[i];
		TZ_start_time_passive3[i] = total_data->start_time3[i];
	}
	
	day_profile_action_ptr = &day_action_passive0[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_0[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_passive0[i];
		day_profile_action_ptr++;
	}
	
	day_profile_action_ptr = &day_action_passive1[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_1[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_passive1[i];
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_passive2[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_2[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_passive2[i];
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_passive3[0];
	for(i=0;i<8;i++)
	{
		day_profile_action_ptr->script_selector = total_data->scripts_day_action_3[i];
		day_profile_action_ptr->p_start_time = &TZ_start_time_passive3[i];
		day_profile_action_ptr++;
	}
	
	for (i=1;i<8;i++)
	{
		ptr0 = (Unsigned8*)&g_Class20_week_profile_passive[0].week_profile_name_len;
		ptr0+=i;
		*ptr0 = total_data->week_summer[i-1]; 
		
		ptr1 = (Unsigned8*)&g_Class20_week_profile_passive[1].week_profile_name_len;
		ptr1+=i;
		*ptr1 = total_data->week_winter[i-1];			
	}

/*
void ConvertBackToRTC(ext_rtc_t*dst, date_time_t*src);
void ConvertRTCDate(date_time_t*dst, ext_rtc_t*src);

	ConvertBackToRTC(&rtcTemp,&g_Class20_start_time_passive1);	// convert date_time_t to rtc_t
	memcpy((uint8_t *)&total_data->g_Class20_start_time_1,(uint8_t*)&rtcTemp,sizeof(ext_rtc_t));
	
	ConvertBackToRTC(&rtcTemp,&g_Class20_start_time_passive2);	// convert date_time_t to rtc_t
	memcpy((uint8_t *)&total_data->g_Class20_start_time_2,(uint8_t*)&rtcTemp,sizeof(ext_rtc_t));
*/	
	ConvertRTCDate(&tempDateTime,&total_data->g_Class20_start_time_1);	// convert rtc_t to date_time_t
	tempDateTime.year_high=0;
	tempDateTime.year_low=0;
	tempDateTime.day_of_week=0;
	memcpy((uint8_t*)&g_Class20_start_time_passive1,(uint8_t *)&tempDateTime,sizeof(date_time_t));
	
	ConvertRTCDate(&tempDateTime,&total_data->g_Class20_start_time_2);	// convert rtc_t to date_time_t
	tempDateTime.year_high=0;
	tempDateTime.year_low=0;
	tempDateTime.day_of_week=0;
	memcpy((uint8_t*)&g_Class20_start_time_passive2,(uint8_t *)&tempDateTime,sizeof(date_time_t));
	
//	memcpy((uint8_t*)&g_Class20_start_time_passive1,(uint8_t *)&total_data->g_Class20_start_time_1,sizeof(date_time_t));
//	memcpy((uint8_t*)&g_Class20_start_time_passive2,(uint8_t *)&total_data->g_Class20_start_time_2,sizeof(date_time_t));
		
}

void saveActiveData(class20_total_data_t *total_data)
{// ram data is stored into a single structure
	day_profile_action_t *day_profile_action_ptr;
	ext_rtc_t rtcTemp;
	Unsigned8 *ptr0;	// for accessing the day types for the seven days of the summer week
	Unsigned8 *ptr1;	// for accessing the day types for the seven days of the winter week
	
	int i;	

	for(i=0;i<8;i++)
	{
		total_data->start_time0[i] = TZ_start_time_active0[i];
		total_data->start_time1[i] = TZ_start_time_active1[i];
		total_data->start_time2[i] = TZ_start_time_active2[i];
		total_data->start_time3[i] = TZ_start_time_active3[i];
	}
	
	day_profile_action_ptr = &day_action_active0[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_0[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}
	
	day_profile_action_ptr = &day_action_active1[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_1[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_active2[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_2[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_active3[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_3[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}
	
	for (i=1;i<8;i++)
	{
		ptr0 = (Unsigned8*)&g_Class20_week_profile_active[0].week_profile_name_len;
		ptr0+=i;
		total_data->week_summer[i-1] = *ptr0; 
		
		ptr1 = (Unsigned8*)&g_Class20_week_profile_active[1].week_profile_name_len;
		ptr1+=i;
		total_data->week_winter[i-1] = *ptr1;			
	}

	ConvertBackToRTC(&rtcTemp,&g_Class20_start_time_active1);	// convert date_time_t to rtc_t
	memcpy((uint8_t *)&total_data->g_Class20_start_time_1,(uint8_t*)&rtcTemp,sizeof(ext_rtc_t));
	
	ConvertBackToRTC(&rtcTemp,&g_Class20_start_time_active2);	// convert date_time_t to rtc_t
	memcpy((uint8_t *)&total_data->g_Class20_start_time_2,(uint8_t*)&rtcTemp,sizeof(ext_rtc_t));
	
//	memcpy((uint8_t *)&total_data->g_Class20_start_time_1,(uint8_t*)&g_Class20_start_time_active1,sizeof(date_time_t));
//	memcpy((uint8_t *)&total_data->g_Class20_start_time_2,(uint8_t*)&g_Class20_start_time_active2,sizeof(date_time_t));
}

void savePassiveData(class20_total_data_t *total_data)
{// ram data is stored into a single structure
	day_profile_action_t *day_profile_action_ptr;
	ext_rtc_t rtcTemp;
	Unsigned8 *ptr0;	// for accessing the day types for the seven days of the summer week
	Unsigned8 *ptr1;	// for accessing the day types for the seven days of the winter week
	
	int i;	

	for(i=0;i<8;i++)
	{
		total_data->start_time0[i] = TZ_start_time_passive0[i];
		total_data->start_time1[i] = TZ_start_time_passive1[i];
		total_data->start_time2[i] = TZ_start_time_passive2[i];
		total_data->start_time3[i] = TZ_start_time_passive3[i];
	}
	
	day_profile_action_ptr = &day_action_passive0[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_0[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}
	
	day_profile_action_ptr = &day_action_passive1[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_1[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_passive2[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_2[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}

	day_profile_action_ptr = &day_action_passive3[0];
	for(i=0;i<8;i++)
	{
		total_data->scripts_day_action_3[i]= day_profile_action_ptr->script_selector;
		day_profile_action_ptr++;
	}
	
	for (i=1;i<8;i++)
	{
		ptr0 = (Unsigned8*)&g_Class20_week_profile_passive[0].week_profile_name_len;
		ptr0+=i;
		total_data->week_summer[i-1] = *ptr0; 
		
		ptr1 = (Unsigned8*)&g_Class20_week_profile_passive[1].week_profile_name_len;
		ptr1+=i;
		total_data->week_winter[i-1] = *ptr1;			
	}
/*
void ConvertBackToRTC(ext_rtc_t*dst, date_time_t*src);
void ConvertRTCDate(date_time_t*dst, ext_rtc_t*src);
*/	

	ConvertBackToRTC(&rtcTemp,&g_Class20_start_time_passive1);	// convert date_time_t to rtc_t
	memcpy((uint8_t *)&total_data->g_Class20_start_time_1,(uint8_t*)&rtcTemp,sizeof(ext_rtc_t));
	
	ConvertBackToRTC(&rtcTemp,&g_Class20_start_time_passive2);	// convert date_time_t to rtc_t
	memcpy((uint8_t *)&total_data->g_Class20_start_time_2,(uint8_t*)&rtcTemp,sizeof(ext_rtc_t));
	
	
//	memcpy((uint8_t *)&total_data->g_Class20_start_time_1,(uint8_t*)&g_Class20_start_time_passive1,sizeof(date_time_t));
//	memcpy((uint8_t *)&total_data->g_Class20_start_time_2,(uint8_t*)&g_Class20_start_time_passive2,sizeof(date_time_t));
}

