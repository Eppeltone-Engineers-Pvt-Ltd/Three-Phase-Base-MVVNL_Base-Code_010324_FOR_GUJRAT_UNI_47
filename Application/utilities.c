// utilities.c
#include "r_cg_macrodriver.h"

#include "typedef.h"            // located right at top of tree below iodefine.h
#include "em_core.h"
#include "em_adapter.h"
#include "em_operation.h"
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "r_dlms_obis_ic.h"
//#include "em_display.h"  
#include "emeter3_structs.h"
#include "wrp_mcu.h"
//#include "memoryOps.h"
//#include "emeter3_app.h"
//#include "eeprom.h"
//#include "e2rom.h"	// pravak implementation
//#include "pravakComm.h"
#include "energyIntegration.h"
#include "utilities.h"

// the following are private functions
//uint8_t IsGreaterOrEqual(uint16_t presentTime, uint16_t zoneTime);
//uint8_t IsLess(uint16_t presentTime, uint16_t zoneTime);
//uint8_t findLastZone(zoneDefs_t *zoneDefsPtr);

//extern time_t TZ_start_time0[];
//extern time_t TZ_start_time1[];

//extern day_profile_action_t	day_action0[];
//extern day_profile_action_t	day_action1[];

int8_t isLaterDateTime(timeinfo_t first, timeinfo_t second)
{// return values 1-greater, 0-equal, -1 less
	int16_t first_days;
	int16_t second_days;
	int16_t first_minutes;
	int16_t second_minutes;
	int8_t retval=1; // success first date time is greater than second
	
	first_days = (int16_t)first.year*365 + (int16_t)first.month*12 + (int16_t)first.date;
	second_days = (int16_t)second.year*365 + (int16_t)second.month*12 + (int16_t)second.date;
	
	first_minutes = (int16_t)first.hour*60 + (int16_t)first.minute;
	second_minutes = (int16_t)second.hour*60 + (int16_t)second.minute;
	
	if(first_days>second_days)
		retval = 1;
	else
	{
		if(first_days<second_days)
			retval = -1;
		else
		{// here the year, month and date are the same
			if(first_minutes>second_minutes)
				retval = 1;
			else
			{
				if(first_minutes<second_minutes)
					retval = -1;
				else
					retval = 0;	// the two are equal
			}
		}
	}
	return retval;
}


int8_t zeroedStruct(uint8_t* buf, int16_t bufsize)
{// this function determines if the structure is filled with all zeros
// returns 1 if all bytes are 0

	int16_t i;
	int8_t retval=1;	// success - all bytes in the structure are 0x00 
	
	for(i=0;i<bufsize;i++)
	{
		if(buf[i]==0x00)
			continue;
		else
		{// we have a non zero value
			retval=0;	// failed - structure is not fully zeroed
			break;
		}
	}
	return retval;
}

int8_t compareStructs(uint8_t* buf1,uint8_t* buf2, int16_t bufsize)
{// this function compares two buffers - returns 1 if buffers match, 0 if buffers don't match

	int16_t i;
	int8_t retval=1;	// success
	
	for(i=0;i<bufsize;i++)
	{
		if(buf1[i]==buf2[i])
			continue;
		else
		{// we have a problem - mismatch
			retval=0;
			break;
		}
	}
	return retval;
}

int8_t isSameSign(int32_t power1, int32_t power2)
{
	int8_t retval=0;	// False
	
	if(power1>=0)
	{
		if(power2>=0)
			retval=1;
	}
	else
	{// here power1 is -ve
		if(power2<0)
			retval=1;
	}
	return retval;	// if same sign then 1 ,else 0	
}

void longDelay(int16_t delayInSeconds)
{
	int16_t i;
	
	for(i=0;i<delayInSeconds;i++)
	{
    R_WDT_Restart();
  	MCU_Delay(100000);
		if(energyIncrementationFlag!=0)
		{
			energy_incrementation();
			energyIncrementationFlag=0;
		}
	}
}

uint8_t checkPhaseVoltagesCurrents(void)
{// returns 0 if all is good, non zero otherwise
  int16_t i;	// two decimals 
  int16_t v;	// one decimal
	uint8_t retval;
	
	retval=0;
	
#ifdef CHECK_CAL_CONDITIONS
	v = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_R)*10);
	if((v<2100)||(v>2700))
		retval++;
		
	v = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_Y)*10);
	if((v<2100)||(v>2700))
		retval++;

	v = (int16_t)(EM_GetVoltageRMS(LINE_PHASE_B)*10);
	if((v<2100)||(v>2700))
		retval++;
		
  i = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_R)*100);
	if(i<0)
		i*=-1;

	if((i<800)||(i>1200))
		retval++;

  i = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_Y)*100);
	if(i<0)
		i*=-1;
	
	if((i<800)||(i>1200))
		retval++;

  i = (int16_t)(EM_EXT_GetIRMS(LINE_PHASE_B)*100);
	if(i<0)
		i*=-1;
	
	if((i<800)||(i>1200))
		retval++;

#endif

	return retval;
}

uint8_t checkNeutralCurrent(void)
{
  int16_t i;	// two decimals 
	uint8_t retval;
	
	
	retval=0;
#ifdef CHECK_CAL_CONDITIONS
//  i = (int16_t)(EM_GetCurrentRMS(LINE_NEUTRAL)*100);
  i = (int16_t)(EM_EXT_GetIRMS(LINE_NEUTRAL)*100);

	if(i<0)
		i*=-1;
	
	if((i<800)||(i>1200))
		retval++;	
#endif	
	return retval;
}



uint8_t swapNibbles(uint8_t givenByte)
{// this function swaps the nibbles
	uint8_t retval;
	uint8_t temp;
	
	retval = (givenByte >> 4) & 0x0F; // get the upper nibble of given byte into lower nibble position
	temp = (givenByte & 0x0F);	// get the lower nibble of given byte into temp;
	retval = retval + (temp<<4);	// add the two to get the final result
	return retval;
}

void stopEM(void)
{
	EM_SYSTEM_STATE last_state;
	uint8_t result;
	result= EM_ERROR;
	last_state = EM_GetSystemState();
	if (last_state == SYSTEM_STATE_INITIALIZED)
	{
		return;
	}
	else
	{
		do
		{
			if (last_state == SYSTEM_STATE_RUNNING)
			{	
				result = EM_Stop();
			}
			last_state = EM_GetSystemState();
		}
		while (result == EM_ERROR);
	}
}

void startEM(void)
{
	EM_SYSTEM_STATE last_state;
	uint8_t result;
	result= EM_ERROR;
	last_state = EM_GetSystemState();
	if (last_state == SYSTEM_STATE_RUNNING)
	{
		return;
	}
	else
	{
		do
		{
			if (last_state == SYSTEM_STATE_INITIALIZED)
			{	
				result = EM_Start();
			}
			last_state = EM_GetSystemState();
		}
		while (result == EM_ERROR);
	}
}

int32_t ComputeTotalMinutesInBillingMonth(ext_rtc_t bdate)
{// given a bill date which is expected to be 01/mm/.... hhh/mm/ss
// typically bills are made on the 1st of a month at 00:00 hours
// we need to account for the case where the bill is made on some other date and time
// we have no idea when the previous bill ws made - this can only be checked by reading the billing history
// as a simple implementation
// assume biil date is always 1, we then need to find the number of days in the previous month

	int32_t retval;
	
	uint8_t no_of_days=0;
	uint8_t prev_month=bdate.month-1;
	uint8_t prev_year=bdate.year;
	
	uint16_t full_year = prev_year+2000;
	
	if (prev_month==0)
	{
		prev_month=12;
		full_year = full_year-1;
	}
	
	if(prev_month==2)
	{
		if((prev_year%4)==0)
		{
			no_of_days=29;
		}
		else
			no_of_days=28;
	}
	else if(prev_month==4 || prev_month==6 || prev_month==9 || prev_month==11)
		no_of_days=30;
	else
		no_of_days=31;

	retval = 	(int32_t)no_of_days*(int32_t)1440;
	return retval;
}


/*

// call example
		CalcDayNumFromDate(year,month,date);	// all parameters must be unsigned int yyyy, mm, dd



// may need to include <string.h> if sprintf functionality is required

h = day of week  (0=Saturday, 1=Sunday, 2=Monday..... 6=Friday)
q is the day of the month
m is the month (3=March, 4=April, 5=May ...12=December, 13=January, 14=February

Y is the year, but for Jan and Feb Y = Y-1 (i.e previous year)

h = (q + floor(13*(m+1)/5) + Y + floor(Y/4) - floor(Y/100) + floor(Y/400)) mod 7


For an ISP(ISO?) week date Day-of-Week d (1=Monday to 7= Sunday) use
	d = ((h+5)mod7)+1

For use with DLMS as per definition in r_dlms_user_interface.c of gDayOfWeek (0=Sunday,1=Monday to 6= Saturday) use
	d = ((h+5)mod7) + 1 but if d becomes 7 make it 0

	
	
	
*/

//uint16_t CalcDayNumFromDate(uint16_t y, uint16_t m, uint16_t d, uint8_t* buf)
uint16_t CalcDayNumFromDate(uint16_t y, uint16_t m, uint16_t d)
{// for the case of input, months m are numbered naturally Jan=1, upto Dec =12
// m = 0 is not permitted	
	uint16_t h;
	uint16_t term1;
	uint16_t term2;
	uint16_t term3;
	uint16_t term4;
	
	
	if(m<3){
		m=m+12;	// this make jan = 13 and feb = 14
		y = y -1;
	}
	
	term1 = 2.6*(m+1);
	term2 = 0.25*y;
	term3 = 0.01*y;
	term4 = 0.0025*y;
	
	h = (d + term1 + y + term2 - term3 + term4)%7;
	
	h = ((h+5)%7)+1;	//ISO 1=Monday, .... 6=Saturday, 7 = Sunday
//	h = h%7;	// 0=Sunday, 1=Monday, .... 6=Saturday	- DLMS
/*	
	switch(h)
	{
		case 0:
			strcpy((char*)buf, "Sunday");
			break;

		case 1:
			strcpy((char*)buf, "Monday");
			break;
			
		case 2:
			strcpy((char*)buf, "Tuesday");
			break;
			
		case 3:
			strcpy((char*)buf, "Wednesday");
			break;
			
		case 4:
			strcpy((char*)buf, "Thursday");
			break;
			
		case 5:
			strcpy((char*)buf, "Friday");
			break;
			
		case 6:
			strcpy((char*)buf, "Saturday");
			break;
			
//		case 7:
//			strcpy((char*)buf, "Sunday");
//			break;
			
	}
*/		
	return h;	
}

uint8_t bcdToBin(uint8_t bcdNum)
{
	uint8_t binNum;
	
	binNum = (bcdNum/16)*10 + bcdNum%16;
	return binNum;

}

uint8_t RTChex2bcd(uint8_t gbyte)
{ // this function will convert a given byte in hex(binary) to a packed bcd
	uint8_t retval;
	retval = (gbyte/10)*16 + (gbyte%10);
	return retval;
}

void RTCbcd2hex(void)
{ // this function will convert all the relevant variables into hex(binary) from packed bcd
	uint8_t*ptr = &rtc.second;
	uint8_t i;

  for(i=0;i<7;i++){
    ptr[i] = (ptr[i]/16)*10 + ptr[i]%16;
  }
}

void maskRTCbytes(void)
{
  rtc.second&=0x7f;
  rtc.minute&=0x7f;
  rtc.hour&=0x3f;
  rtc.day&=0x07;
  rtc.date&=0x3f; //0011 1111 
  rtc.month&=0x1f;//0001 1111
  rtc.year&=0xff; //0001 1111

  if((rtc.date==0)||(rtc.date>0x31))
    rtc.date=1;

  if((rtc.month==0)||(rtc.month>0x12))
    rtc.month=1;

  if((rtc.year==0)||(rtc.year>0x99))
    rtc.year=1;
}

void readRTC(void)
{
		rtc_calendarcounter_value_t ren_rtc;	// this structure is used to read the renesas rtc

		R_RTC_Get_CalendarCounterValue(&ren_rtc);	// this way we get to read the RTC - careful about BCD data
			
		rtc.second  = ren_rtc.rseccnt;
		rtc.minute  = ren_rtc.rmincnt;
		rtc.hour  = ren_rtc.rhrcnt;
		rtc.day = ren_rtc.rwkcnt;	// day of week
		rtc.date  = ren_rtc.rdaycnt;
		rtc.month  = ren_rtc.rmoncnt;
		rtc.year  = (uint8_t)ren_rtc.ryrcnt;
		maskRTCbytes();
		RTCbcd2hex();				
		rtc.day = (uint8_t)CalcDayNumFromDate((2000+rtc.year), rtc.month, rtc.date);
}


void delay(void)
{
  volatile uint16_t i;
  R_WDT_Restart();   
  for(i=0;i<2000;i++);
}

void incDate(uint8_t *arr, uint8_t period)
{// the period decides the increment amount allowing values other than 30 to be used eg: 15,
// this function uses the timeinfo_t structure to pass the time and date, and includes 6 bytes as
// detailed below
// this function will now be a common function and will be used to compute the new time date whenever required  
/*
  arr[0]  minute
  arr[1]  hour
  arr[2]  day
  arr[3]  date
  arr[4]  month
  arr[5]  year
*/
    // 
  uint8_t changemonthflag=0;

  arr[0]=arr[0]+period;
  
  if(arr[0]>=60)
  {
    arr[0]=0;
    arr[1]=arr[1]+1;

    if(arr[1]==24)
    {// here day has changed
      arr[1]=0;
			
			arr[2]=(arr[2]+1)%7;
			
      arr[3]=arr[3]+1;
      if(arr[3]<=28)
        return;
      
      if((arr[3]==31)&&(((arr[4]==4)||(arr[4]==6)||(arr[4]==9)||(arr[4]==11))))
        changemonthflag=1;  // for april,june,september and november, a 31 date calls for month change
      else
      {
      // here month is not 4,6,9 or 11 or date is not 31
      // month could be 1,2,3,5,7,8,10, or 12
              
        if(arr[4]==2)
        {// here handle the case for February
          if((arr[5]%4)==0)
          {// this is a leap year
            if(arr[3]==30)
              changemonthflag=1;
          }
          else
          {// this is not a leap year
            if(arr[3]==29)
              changemonthflag=1;
          }
        }
        else
        {// here handle the case for 1,3,5,7,8,10,12 - balance months of 31 days
          if(arr[3]==32)
            changemonthflag=1;
        }
      }
      
      if(changemonthflag==1)
      {
        arr[3]=1;
        arr[4]=arr[4]+1;
        
        if(arr[4]==13)
        {      
          arr[4]=1;
          arr[5]=arr[5]+1;
        }
      }    
    }
  }
}

void fill_structure(uint8_t* arr, uint16_t rec_size, uint8_t fillbyte)
{
	int i;
  for(i=0;i<rec_size;i++)
    arr[i]=fillbyte;
}


void clear_structure(uint8_t* arr, uint16_t rec_size)
{
	int i;
  for(i=0;i<rec_size;i++)
    arr[i]=0;
  
}

void decptr_maskless(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc)
{
  if(base_ad == *ptr)
    *ptr = end_ad - ptr_inc ;
  else
    *ptr = *ptr - ptr_inc;
}


void incptr_maskless(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc)
{
//  unsigned int mask = ~(ptr_inc-1);
  
//  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned

  *ptr = *ptr+ptr_inc;
  if(*ptr >= end_ad)
    *ptr = base_ad;
}

void incptr(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc)
{
  uint16_t mask = ~(ptr_inc-1);
  
  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned

  *ptr = *ptr+ptr_inc;
  if(*ptr >= end_ad)
    *ptr = base_ad;
}

/*
// this function does not appear to be used
void decptr(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc)
{
  uint32_t mask = ~(ptr_inc-1);
  
  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned

  if(base_ad == *ptr)
    *ptr = end_ad - ptr_inc ;
  else
    *ptr = *ptr - ptr_inc;
}
*/

uint8_t checkptr(uint32_t *ptr, uint32_t base_ad, uint32_t end_ad, uint16_t ptr_inc)
{// the alignment of the pointer must also be checked
  uint32_t mask = ~(ptr_inc-1);
  
//  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned

  if((*ptr <base_ad)||(*ptr >= end_ad))
  {
    *ptr = base_ad;
    return 0;// since address was out of bounds
  }
  else
    return 1; // since address was within bounds
}


uint8_t isCheckSumOK(uint8_t *arr, uint16_t nBytes)
{// returns 1 if check sum is OK, 0 otherwise
  uint16_t i;
  uint8_t sum = 0;
  for(i=0; i<(nBytes-1); i++)
    sum +=arr[i];
  
  sum=~sum;
  if(sum == arr[i])
    return 1;
  else
    return 0;
}

uint8_t calcChecksum(uint8_t *arr, uint16_t nBytes)
{
  uint16_t i;
  uint8_t sum = 0;
  for(i=0; i<nBytes; i++)
    sum +=arr[i];
  sum=~sum;
  return sum;  
}

// general utilities


uint8_t hexchar2nibble(uint8_t c)
{// this function is similar (almost identical to hexToDec above)
  if(c<='9' && c>='0')
    return (c-'0');
  else if(c<='F' && c>='A')
    return (c-'A'+10);
  else if(c<='f' && c>='a')
    return (c-'a'+10);
  else
    return 0;		// error case
}


uint8_t hexchars2byte(uint8_t cupper,uint8_t clower)
// takes 2 characters (hex string) and returns the corresponding unsigned char
{
  uint8_t retbyte;
  retbyte = hexchar2nibble(cupper)*16;
  retbyte += hexchar2nibble(clower);
  return retbyte;
}


uint8_t nib2ascii(uint8_t temp)
//converts number in temp (0 to 15) to hex character 0 to F
{
	uint8_t retchar;
	if(temp<=9)
		retchar='0'+temp;
	else
		retchar='A'+temp-10;
	return retchar;
}

/*
// example code for using byt2digs
byt2digs(adval2,&dighi,&diglo);
wrtxbuf(dighi);
wrtxbuf(diglo);
*/

void byt2digs(uint8_t rdbyte, uint8_t* bytehi, uint8_t* bytelo)
{
	uint8_t temp=0x0f;
	temp=temp & rdbyte; //lower nibble in temp

	//convert nibble to ascii
	*bytelo=nib2ascii(temp);
	rdbyte=rdbyte/16;
	temp= 0x0f & rdbyte;
	//temp now has upper nibble in lower nibble position
	*bytehi=nib2ascii(temp);
}





