#ifndef E2ROM_H
#define E2ROM_H

//------------  top level functions -------------
void inite2rom(void);
void wre2rom(void);
//unsigned char rde2rom(unsigned int addr);
void readMultiple_e2rom(char* buf, unsigned int addr, char nBytes, char device);
char decrementMonth(char diff);
char decrementMonthHistory(char diff);
void incrementMonth(void);

unsigned int read_md(char pmonth);
char read_pf(char pmonth);
char read_md_date(char pmonth);
char read_md_hr(char pmonth);
char read_md_min(char pmonth);
void clearMymeterData(void);
char read_md_month(char pmonth);
char read_md_year(char pmonth);
void write_kwh(void);
void write_kvah(void);
void write_current_log_data(void);
void write_meter_data(void);
void write_history_data(void);
void write_pon_data(void);
void write_critical_data(void);
//void write_tod_current(char z);
void write_this_zone_kwh(int32_t kwh_value, char z);
void write_this_zone_md(unsigned int md_value, char z);
void write_this_zone_kvah(int32_t kvah_value, char z);
//void zap(unsigned int start_ad, unsigned int end_ad);
void zap(unsigned int start_ad, unsigned int end_ad, char zapbyte);;
void clear_structure(char* arr, char rec_size);
int write_energies(void);
void init_read_energies(void);


#if defined (PRODUCTION) || defined (TESTING)
void cleare2rom(void);
#endif

void dump(unsigned int s_addr, char nbytes);

//------------ timing functions -----------------
void dely(void);

//------------- internal functions ---------------------
void e2rom_crtstop(void);
void e2rom_crtstrt(void);
void e2rom_restrt(void);
void e2rom_noack(void);
void e2rom_sndack(void);
char e2rom_rcvbyt(void);
char sende2rom(char a);
char pageWriteE2rom(unsigned int e2address,char e2buf[],char nBytes,char device);
//void incptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, char ptr_inc);
void incptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc);
void incptr_maskless(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc);
void decptr_maskless(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc);
//void decptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, char ptr_inc);
void decptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc);
char readMultiple_e2rom_internal(char* buf, unsigned int addr, char nBytes, char device);
unsigned int checke2rom(void);
void write_zone_definitions(void);
void write_new_zone_definitions(void);
void write_properly(unsigned int ad, char* arr, char rec_size);
void write_alternately(unsigned int ad, unsigned int alternate_ad, char* arr, char rec_size);
void write_mytmpr_data(void);
void writeSurveyData(void);
char  read_alternately(unsigned int ad, unsigned int alternate_ad, char* arr, char rec_size);

#endif
