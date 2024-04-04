#ifndef TOD_H
#define TOD_H

#include "r_dlms_obis.h"
#include "r_dlms_obis_ic.h"
#include "emeter3_structs.h"

typedef struct tag_class20_total_data_t
{// 127 bytes
	stime_t   						start_time0[8];	// TZ_start_time_first set
	stime_t   						start_time1[8];	// TZ_start_time_second set[8]
	stime_t   						start_time2[8];	// TZ_start_time_third set
	stime_t   						start_time3[8];	// TZ_start_time_fourth set[8]
	uint8_t								scripts_day_action_0[8];
	uint8_t								scripts_day_action_1[8];
	uint8_t								scripts_day_action_2[8]; 
	uint8_t								scripts_day_action_3[8];
	uint8_t	  						week_summer[7];	// contains only the day ids from Monday to Sunday of g_Class20_week_profile_acive[0]
	uint8_t	  						week_winter[7];	// contains only the day ids from Monday to Sunday of g_Class20_week_profile_acive[1]
	ext_rtc_t							g_Class20_start_time_1;
	ext_rtc_t							g_Class20_start_time_2;
//	uint8_t								dummy;
	uint8_t								chksum;
}class20_total_data_t;

//void saveActiveData(void);
void saveActiveData(class20_total_data_t *total_data);
void savePassiveData(class20_total_data_t *total_data);
void loadActiveData(class20_total_data_t *total_data);
void loadPassiveData(class20_total_data_t *total_data);
uint8_t findZone(ext_rtc_t *rtcPtr);
#endif
