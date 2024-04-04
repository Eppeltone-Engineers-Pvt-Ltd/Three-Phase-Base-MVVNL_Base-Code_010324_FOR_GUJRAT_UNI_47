#include "typedef.h"

#include "emeter3_structs.h"
#include "calendarFunctions.h"

const int8_t daysList[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

int8_t isYearLeap(int16_t year);


int8_t findNumberOfDaysInFeb(int16_t year);
int8_t daysInMonth(int16_t year, int8_t month);
int32_t findNumberOfFullDaysElapsedInThisYear(int16_t year, int8_t month, int8_t date);
int32_t findNumberOfSecondsElapsedInThisYear(ext_rtc_t dt);
int32_t findNumberOfDaysElapsedSinceEpoch(int8_t year);




int8_t isYearLeap(int16_t year)
{// return Value : 0- not a leap year, 1- is a leap year
	if((year%4)==0)
	{
		if((year%400)==0)	// this takes care of 2000,2400,2800 etc.
			return 1;
		else if((year%100)==0)	// this takes care of 2100,2200,2300,.. 2500,2600,2700 etc.
			return 0;
		else
			return 1;
	}
	else
		return 0;
}

int8_t findNumberOfDaysInFeb(int16_t year)
{	
	if(1==isYearLeap(year))
		return 29;
	else
		return 28;
}

int8_t daysInMonth(int16_t year, int8_t month)
{
	int8_t retval;
	if(month==2)
		retval = findNumberOfDaysInFeb(year);
	else
		retval = daysList[month-1];
	return retval;
}

int32_t findNumberOfFullDaysElapsedInThisYear(int16_t year, int8_t month, int8_t date)
{// we will find the number of full days elapsed since year:01:01 time 00:00:00
// if given date is 01 Jan itself the answer returned will be 0.
// month goes from 01 to 12
// count the number of days from the january to month-1
	int32_t noOfDays;
	
	int8_t startMonth  = 1;
	
	noOfDays=0;
	while (startMonth < month)
	{
		noOfDays += daysInMonth(year, startMonth);
		startMonth++;
	}
	
	return (noOfDays+(int32_t)(date-1));	
}

int32_t findNumberOfDaysElapsedSinceEpoch(int8_t year)
{// 2000 will be defined as epcoh year
// this function returns number of days between 01-01-2010 and 01-01-year

	int32_t noOfDays;
	int16_t thisYear = 2000+year;
	int16_t startYear = 2010;
	
	noOfDays=0;
	
	while(startYear<thisYear)
	{
		if(1==isYearLeap(startYear))
			noOfDays += 366;
		else
			noOfDays += 365;
			
		startYear++;
	}
	return noOfDays;
}


int32_t findNumberOfSecondsElapsedInThisYear(ext_rtc_t dt)
{
	int32_t	tempLong;
	
	tempLong = findNumberOfFullDaysElapsedInThisYear((dt.year+2000), dt.month, dt.date);	// full days
	
	tempLong = (tempLong*(int32_t)86400) + (((int32_t)dt.hour*(int32_t)60) + (int32_t)dt.minute)*(int32_t)60 + (int32_t)dt.second;
	
	return tempLong;

}


int32_t findNumberOfSecondsElapsedSinceEpoch(ext_rtc_t dt)
{
	int32_t tempLong;
	
	tempLong = findNumberOfDaysElapsedSinceEpoch(dt.year)*(int32_t)86400;
	tempLong += findNumberOfSecondsElapsedInThisYear(dt);
	
	return tempLong;
}


int32_t diffInSecondsBetween(ext_rtc_t laterDt, ext_rtc_t earlierDt)
{
// function returns the difference in seconds between two dates
// returns 0 if earlierDt is later than laterDt
	int32_t laterSeconds;
	int32_t earlierSeconds;
	
	laterSeconds = findNumberOfSecondsElapsedSinceEpoch(laterDt);
	earlierSeconds = findNumberOfSecondsElapsedSinceEpoch(earlierDt);
	
	if(laterSeconds>earlierSeconds)
		return (laterSeconds-earlierSeconds);
	else
		return 0;

}


