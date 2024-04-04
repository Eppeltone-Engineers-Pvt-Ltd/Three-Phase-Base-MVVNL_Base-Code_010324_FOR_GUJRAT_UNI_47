#ifndef SURVEY_H
#define SURVEY_H

//void initialiseSurveyData(void);
void initialiseSurveyDataSmall(void);

void do_survey_stuff(void);
void do_survey_stuff_small(void);
void write_surveydata(uint8_t* sdata, uint8_t datasize, uint8_t shour , uint8_t sminute);
void write_survey_date(uint8_t pdate, uint8_t pmonth, uint8_t pyear);
void do_survey_update(void);
void do_survey_update_small(void);

void do_log_update(void);
//void writeLogData(uint8_t sdate,uint8_t smonth,uint8_t syear);
void writeLogData(uint8_t sdate,uint8_t smonth,uint8_t syear, uint8_t shour, uint8_t sminute, uint8_t ssecond);
void computeLoadSurveyEntries(uint8_t hr, uint8_t min);
void computeDailyLogEntries(void);
//void populateSurveyRecord(surveydata_t* surveyData);
void populateSmallSurveyRecord(small_surveydata_t* smallSurveyData);

void ConvertRTC_shortDate(timeinfo_t*dst, ext_rtc_t*src);
void ConvertShortDate_RTC(ext_rtc_t*dst, timeinfo_t*src);

#endif
