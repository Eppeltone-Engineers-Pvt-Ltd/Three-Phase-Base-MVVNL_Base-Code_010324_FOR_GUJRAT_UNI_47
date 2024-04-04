
#include "e2rom.h"

void dely(void)
{
volatile char i=0;
// frequency dependent

  i = mysysvar.mcu_freq*2; // this was 1 earlier
  do
  {
    i--;
  }while (i!=0);
}

//void incptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, char ptr_inc)
void incptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc)
{
  unsigned int mask = ~(ptr_inc-1);
  
  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned

  *ptr = *ptr+ptr_inc;
  if(*ptr >= end_ad)
    *ptr = base_ad;
}

void incptr_maskless(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc)
{
//  unsigned int mask = ~(ptr_inc-1);
unsigned int tempPtrValue;
//  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned
  tempPtrValue=*ptr;
  if((tempPtrValue%ptr_inc)!=0)
  { // houston, we have a situation here
    *ptr= ((int32_t)tempPtrValue/(int32_t)ptr_inc)*(int32_t)ptr_inc;
  }

  *ptr = *ptr+ptr_inc;
  if(*ptr >= end_ad)
    *ptr = base_ad;
}

// although this code is not used - uncommenting causes the hex files to be different

void decptr_maskless(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc)
{
  unsigned int tempPtrValue;

  tempPtrValue=*ptr;
  if((tempPtrValue%ptr_inc)!=0)
  { // houston, we have a situation here
    *ptr= ((int32_t)tempPtrValue/(int32_t)ptr_inc)*(int32_t)ptr_inc;
  }

  if(base_ad == *ptr)
    *ptr = end_ad - ptr_inc ;
  else
    *ptr = *ptr - ptr_inc;
}


//void decptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, char ptr_inc)
void decptr(unsigned int *ptr, unsigned int base_ad, unsigned int end_ad, unsigned int ptr_inc)
{
  unsigned int mask = ~(ptr_inc-1);
  
  *ptr = *ptr&mask; //make sure that the pointer is boundary aligned

  if(base_ad == *ptr)
    *ptr = end_ad - ptr_inc ;
  else
    *ptr = *ptr - ptr_inc;
}


void e2rom_crtstop(void)
{
  SDA_e2rom_MAKEOUT;
  SDA_e2rom_LOW;
  dely();
  SCLK_e2rom_HI;
  dely();
  // after this i have to change direction of SDA_e2rom
  SDA_e2rom_HI;
  dely();
  SDA_e2rom_MAKEINP;
}

void e2rom_crtstrt(void)
{
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  SDA_e2rom_LOW;
  dely();
  SCLK_e2rom_LOW;  //lcall sclklo
  dely();
}

void e2rom_restrt(void)
{
  SDA_e2rom_HI;
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  dely();
  SCLK_e2rom_HI;
  dely();
  SDA_e2rom_LOW;
  dely();
  SCLK_e2rom_LOW;
  dely();
}

void e2rom_sndack(void)
{// this fellow is called during multiple reads
// the rcvbyt function keeps SDA_LOW and SDA_OUT
// hence all we need to do is to clock it.
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  SDA_e2rom_LOW;
  dely();
  SCLK_e2rom_HI;
  dely();
  SCLK_e2rom_LOW;
  dely();
  SDA_e2rom_MAKEINP;
  SDA_e2rom_HI;
}

void e2rom_noack(void)
{
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  SDA_e2rom_HI;
  dely();
  SCLK_e2rom_HI;
  dely();
  SCLK_e2rom_LOW;
  dely();
  SDA_e2rom_LOW;
}

char e2rom_rcvbyt(void)
{

  char i;
  char retByte=0;

  SDA_e2rom_HI;
  SDA_e2rom_MAKEINP;
  //msb first since   rlc A
  i=8;
  i--;

  dely();
  SCLK_e2rom_HI;

  if(MY_SDA_e2rom!=0)
    retByte |= _BV(i);

  dely();
  
  do
  {
    SCLK_e2rom_LOW;
    dely();
    SCLK_e2rom_HI;
    
    i--;
    if(MY_SDA_e2rom!=0)
      retByte |= _BV(i);
    dely();
  }while(i!=0);

  SCLK_e2rom_LOW;
  dely();

  return retByte;
}

char sende2rom(char a)
{// this fellow will return e2rom_ackflag which is now a local variable.

char mask=0x80;
char e2rom_ackflag;  

//  e2rom_sndbyt(a);
// The following code was initially in e2rom_sndbyt(a)
//----------------------------------------------------
//check direction of MY_SDA_e2rom
// sda is output when this function is called
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin

  if( (a & mask) ==0)
    SDA_e2rom_LOW;
  else SDA_e2rom_HI; 

  dely();
  SCLK_e2rom_HI;
  dely();
  SCLK_e2rom_LOW;

  do
  {	
    dely();
    mask = mask/2;
    if( (a & mask) ==0)
      SDA_e2rom_LOW;
    else 
      SDA_e2rom_HI; 
    dely();
    SCLK_e2rom_HI;
    dely();
    SCLK_e2rom_LOW;
  }while(mask!=1);

  SDA_e2rom_MAKEINP;
  SDA_e2rom_HI;
  dely();

// The following code was initially in e2rom_rcvack()
//---------------------------------------------  
  e2rom_ackflag=1;
  SCLK_e2rom_HI;
  dely();
  if(MY_SDA_e2rom!=0)
    e2rom_ackflag=0;
  SCLK_e2rom_LOW;
  dely();
//---------------------------------------------  
  return e2rom_ackflag;
}

char pageWriteE2rom(unsigned int e2address, char e2buf[], char nBytes, char device)
// write nBytes in e2buf at e2address
// return: 0 for success 1 for failure
{
  char e2adhi = e2address/256;
  char e2adlo = e2address%256;
  char i=0;
  char byt;
  char device_code=0xa0;
  
//  clearwdt(); // shifted to wherever pageWrite is called
  
  if (device==RTC)
  {
    #ifdef PHILIPS_RTC
      device_code=0xa2;	// PHILIPS RTC device code
    #else
      device_code=0xd0;
    #endif
  }	
  e2rom_crtstrt();

  if(sende2rom(device_code)==0)
  {
    e2rom_crtstop();
    return 1;
  }

  if(device != RTC)
  {
    if(sende2rom(e2adhi)==0)
    {
      e2rom_crtstop();
      return 1;
    }
  }

  if(sende2rom(e2adlo)==0)
  {
    e2rom_crtstop();
    return 1;
  }

  do
  {
    byt=e2buf[i];
    i++;
    if(sende2rom(byt)==0)
    {
      e2rom_crtstop();
      return 1;
    }
  }while(i<nBytes);		
  
  e2rom_crtstop();
  moreflags&=~MS50FLG;		// signifying that e2rom is now busy for 50 ms

  return 0;
}

void incrementMonth(void)
{
  ctldata.month++;
  if(ctldata.month==13)
  {
    ctldata.month=1;
    ctldata.year++;
  }
}

char decrementMonth(char diff)
{
//  signed char ret = ctl data.month-diff;
  signed char ret = Calib_Data.history_ptr-diff;
  if(ret <= 0)
    ret += 12;
  return ret;
}

#ifndef PRODUCTION
void clearMymeterData(void)
{
  char i;
  char tempchar;

  myenergydata.powerOnMinutes=0; // only this variable needs to be cleared
  
  myenergydata.chksum=calcChecksum((char*)&myenergydata,sizeof(myenergydata)-1); // size-1
  
// the entire mymddata is to be cleared
// we must preserve the comm_count variable  

  tempchar=mymddata.comm_count; // save the value
  char *ptr=(char*)&mymddata.md;
  
  for(i=0;i<sizeof(mymddata);i++)
  {
    *ptr=0;
    ptr++;
  }
  
  mymddata.comm_count=tempchar; // restore the value
  
  mymddata.chksum=calcChecksum((char*)&mymddata,sizeof(mymddata)-1); // size-1
  
  ctldata.kwh_last_saved=total_energy_lastSaved;
  ctldata.kvah_last_saved=apparent_energy_lastSaved;
  ctldata.chksum=calcChecksum((char*)&ctldata,sizeof(ctldata)-1); // 15/Jun/2013
  
  #if defined(TOD) 
  clear_structure((char*)&mytoddata,sizeof(mytoddata)); // clear this for once
  
  for(i=0;i<8;i++)
  {// since we are clearing it we don't have to read the current one - assuming the ZoneDefinitions array is intact.
#if defined(TOD_HAS_KVAH)
    read_alternately(TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC, (char*)&mytoddata, 8); // change this to 8 if tod has kvah
#else
    read_alternately(TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC, (char*)&mytoddata, 4);
#endif    
    mytoddata.ZoneDefLowerByte=zoneDefines.ZoneDefinition[i]%256;  // lower byte first
    mytoddata.ZoneDefUpperByte=zoneDefines.ZoneDefinition[i]/256;  // upper byte second
    delay();  // pause a while - this calls clearwdt
    write_alternately((TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+i*ZONE_PTR_INC), (char*)&mytoddata, sizeof(mytoddata));
  }
  
  myenergydata.presentZone=findZone(rtc1.hours,rtc1.minutes);
  myenergydata.chksum=calcChecksum((char*)&myenergydata,sizeof(myenergydata)-1); // size-1
  read_alternately(TOD_CURRENT_BASE_ADDRESS+myenergydata.presentZone*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+myenergydata.presentZone*ZONE_PTR_INC, (char*)&mytoddata, sizeof(mytoddata));
  #endif  
}
#endif

void write_history_data(void)
{ 
#ifndef PRODUCTION
  #ifdef TOD
  toddata_t tempTodData;  // used to read T O D data  which now will store the Zonedefinition also
  #endif
  int32_t tempkwh;
  int32_t tempkvah;

  //  char tempDataArr[20]; // we can use the same array to store ABC 
  char checkmain;
  char tmonth; 
  #ifdef TOD  
  signed char z;  // this is to allow it to become negative for ABC
  #endif
  
  tmonth=Calib_Data.history_ptr-1;
  
// while writing history data make sure to save the energies lastSaved  
  tempkwh=myenergydata.kwh;
  myenergydata.kwh=total_energy_lastSaved; // while writing history data make sure to save the total_energy_lastSaved

  tempkvah=myenergydata.kvah;
  myenergydata.kvah=apparent_energy_lastSaved;

  myenergydata.chksum=calcChecksum((char*)&myenergydata,sizeof(myenergydata)-1); // since we have changed the energy values

  clearwdt(); // this function must be before the loop
  
  do
  {
    waitForMS50FLG(); // this function specially written
    checkmain = pageWriteE2rom(HISTORY_BASE_ADDRESS+tmonth*HISTORY_PTR_INC, (char*)&myenergydata, 12, E2ROM); // store only the first twelve bytes
  }while(checkmain!=0);
  
// having written mymeterdata - now restore the actual energy
  myenergydata.kwh=tempkwh;  
  myenergydata.kvah=tempkvah;  
  myenergydata.chksum=calcChecksum((char*)&myenergydata,sizeof(myenergydata)-1); // since we have changed the energy values

// we now need to store the md data
// History ptr inc is 32 - we use only 12 bytes to store the energy data, we can store the md data with an offset of 12

// we must compute the average-pf first.

  mymddata.avg_pf=compute_average_pf();
  mymddata.powerOnMinutes=myenergydata.powerOnMinutes;
  mymddata.chksum=calcChecksum((char*)&mymddata,sizeof(mymddata)-1);
  
  clearwdt(); // this function must be before the loop

  do
  {
    waitForMS50FLG();
    checkmain = pageWriteE2rom(HISTORY_BASE_ADDRESS+12+tmonth*HISTORY_PTR_INC, (char*)&mymddata, sizeof(mymddata), E2ROM);  // store the twenty bytes starting at 12
  }while(checkmain!=0);

  Calib_Data.history_ptr++;
  if(Calib_Data.history_ptr==13)
    Calib_Data.history_ptr = 1;
// we are incrementing this after the write has taken place.
// this value will get saved since after every write_history_data call we save the critical data.  
  Calib_Data.bpcount++;
  e2write_flags |= E2_W_IMPDATA;

// also write the TOD_billing  
  #if defined(TOD)
// each zone consists of 26 bytes of data, there are eight sets of such data
// we need to read each of these sets of 26 bytes and copy them into the BILLING LOCATION

// is the current zone data which was in ram synchronised with e2rom
// yes it is, since in per minute activity the minute task is performed before billing.
// however, if md has not been generated then we may lose the energies - hence
// should we be using the present or the previouszone  
  
  mytoddata.kwh+=zone_kwh_last_saved;
#if defined(TOD_HAS_KVAH)  
  mytoddata.kvah+=zone_kvah_last_saved;
#endif    
  write_alternately((TOD_CURRENT_BASE_ADDRESS+myenergydata.presentZone*ZONE_PTR_INC), (ALTERNATE_TOD_CURRENT_BASE_ADDRESS+myenergydata.presentZone*ZONE_PTR_INC), (char*)&mytoddata, sizeof(mytoddata));

  myenergydata.zone_kwh-=zone_kwh_last_saved;
  myenergydata.zone_kvah-=zone_kvah_last_saved;

// required to be zeroed, since while changing zone definitions when tod data is being sent the zone kwh last saved value gets added for the current zone which if not zeroed would result in erroneous value.
  zone_kwh_last_saved=0;  
  zone_kvah_last_saved=0;
    
  myenergydata.chksum=calcChecksum((char*)&myenergydata,sizeof(myenergydata)-1); // since we have changed the energy values
  
  for(z=0;z<8;z++)
  {
    read_alternately(TOD_CURRENT_BASE_ADDRESS+z*ZONE_PTR_INC, ALTERNATE_TOD_CURRENT_BASE_ADDRESS+z*ZONE_PTR_INC, (char*)&tempTodData, sizeof(tempTodData));
    tempTodData.ZoneDefLowerByte=zoneDefines.ZoneDefinition[z]%256;  // lower byte first
    tempTodData.ZoneDefUpperByte=zoneDefines.ZoneDefinition[z]/256;  // upper byte second
    
    tempTodData.chksum=calcChecksum((char*)&tempTodData,sizeof(tempTodData)-1); // size-1
    
    delay();  // pause a while - this calls clearwdt
    do
    {
      waitForMS50FLG();
      checkmain = pageWriteE2rom(TOD_HISTORY_BASE_ADDRESS+tmonth*TOD_PTR_INC+z*ZONE_PTR_INC,(char*)&tempTodData, sizeof(tempTodData), E2ROM);  // earlier 11 - now Zone Defs also stored
    }while(checkmain!=0);

    delay();  // pause a while
  }
  #endif
#endif
}


int write_energies(void)
{// return 0 for success and 99 for failure
// before we write we ought to read the energy(kwh) value already written - (preferably the smallest
// value and which is most likely to be in the location where we currently intend to write.
// we must check whether the value written there is a valid one and this needs to be done
// only if the current kwh is >0.32 kwh  

  energydata_t temp_energy_data;
  unsigned int temp_ptr=ctldata.energies_wrt_ptr;
  char i=0;
  char done=0;


// we have modified the erase function to write properly zeroed energy records with checksum
  
  do
  {
    readMultiple_e2rom((char*)&temp_energy_data,temp_ptr,sizeof(temp_energy_data),E2ROM);
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
    i++;  // this is to keep track of how many times this loop has been executed

    if(isCheckSumOK((char*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
      done=1;
    }
  }
  while((done==0)&&(i<3));  // why waste time reading 32 locations if 35 kv is on?

  
  if(i==3) // there was no good record
    return 1; // to indicate failure
  else
  {// we have a good record
    if((myenergydata.kwh-temp_energy_data.kwh)>((int32_t)256*(int32_t)KWH_INCREMENT))  // 64 earlier
    {// for some strange reason our present myenergydata happens to contain a kwh wihch is 2.56 more than the previous stored valid value
      // then we will use the old value
      myenergydata=temp_energy_data;  // old value will have proper checksum
    }
    else
    {// we have a proper energy value to be written
      write_properly(ctldata.energies_wrt_ptr, (char*)&myenergydata, sizeof(myenergydata));
      incptr(&ctldata.energies_wrt_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		
      ctldata.chksum=calcChecksum((char*)&ctldata,sizeof(ctldata)-1);
    }
    return 0; // to indicate success
  }
}



void init_read_energies(void)
{// this function reads all the energies - basically the myenergydata
// now instead of individually storing kwh,kvah etc copies we store the entire myenergydata
// which is 22 bytes long - we save 32 copies hence we need a memmory of 32*32= 1024 bytes  
  unsigned int temp_ptr;
  int i = 0;

  energydata_t temp_energy_data;
  energydata_t min_energy_data;
//  energydata_t max_energy_data;
    
  min_energy_data.kwh=999999999;

  temp_ptr = ENERGIES_BASE_ADDRESS;
  ctldata.energies_wrt_ptr = ENERGIES_BASE_ADDRESS; // this was not there earlier.
  ctldata.chksum=calcChecksum((char*)&ctldata,sizeof(ctldata)-1); // 15/Jun/2013

// we dont seem to take into account the fact that after an overflow and abusrd max value may come before a 
// genuine minimum value. In such a case the initial max value will get accepted. this is not good
// a correct approach should be to first find the minimum value. Alternately one should ensure while writing
// that a high value greater than 64 increments does not get written.
// I think this approach may be more satisfactuory - computing wise.  


// first get the minimum kwh
  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
    readMultiple_e2rom((char*)&temp_energy_data,temp_ptr,sizeof(temp_energy_data),E2ROM);
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		

    if(isCheckSumOK((char*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
      if(temp_energy_data.kwh < min_energy_data.kwh)
        min_energy_data = temp_energy_data; // this is the minimum energy record
    }
  }

// now that we have the minimum kwh, which could also be 999999999 in case the memory was blank
  if(min_energy_data.kwh==999999999)
  {
    min_energy_data.kwh=0;
    clear_structure((char*)&myenergydata,sizeof(myenergydata)); // added on 19/Jan/2012 after sending code to Ald and Isc
    myenergydata.chksum=0xff;  // added on 19/Jan/2012 after sending code to Ald and Isc
  }
  else
    myenergydata = min_energy_data; // initialise the myenergydata with the lowest value

// we now do a second pass and pick up that max value which satisfies the 64 increments  
// it may be a good idea to reinitialise the temp_ptr
  
  temp_ptr = ENERGIES_BASE_ADDRESS;

  for(i=0; i<((ENERGIES_END_ADDRESS-ENERGIES_BASE_ADDRESS)/ENERGIES_PTR_INC);i++)
  {
    readMultiple_e2rom((char*)&temp_energy_data,temp_ptr,sizeof(temp_energy_data),E2ROM);
    incptr(&temp_ptr, ENERGIES_BASE_ADDRESS, ENERGIES_END_ADDRESS, ENERGIES_PTR_INC);		

    if(isCheckSumOK((char*)&temp_energy_data,sizeof(temp_energy_data))!=0)
    {// here energy values are fine (NOT corrupted)
      if((temp_energy_data.kwh > myenergydata.kwh)&&((temp_energy_data.kwh-min_energy_data.kwh)<((int32_t)256*(int32_t)KWH_INCREMENT)))
      {
        myenergydata = temp_energy_data; // this is the maximum energy record
        ctldata.energies_wrt_ptr = temp_ptr;
        ctldata.chksum=calcChecksum((char*)&ctldata,sizeof(ctldata)-1); // 15/Jun/2013
      }
    }
  }
  
// during reading we have now taken care that the highest value better be a proper value
  
  total_energy_lastSaved = myenergydata.kwh;
  apparent_energy_lastSaved = myenergydata.kvah;
  
#ifdef TOD
  zone_kwh_last_saved = myenergydata.zone_kwh;
  zone_kvah_last_saved = myenergydata.zone_kvah;
#endif  

  ctldata.chksum=calcChecksum((char*)&ctldata,sizeof(ctldata)-1); // some pointers have got changed above
}

#ifdef TOD
void write_zone_definitions(void)
{
  zoneDefines.chksum=calcChecksum((char*)&zoneDefines,sizeof(zoneDefines)-1); // size-1
  write_alternately(ZONE_DEFINITIONS_BASE_ADDRESS, ALTERNATE_ZONE_DEFINITIONS_BASE_ADDRESS, (char*)&zoneDefines, sizeof(zoneDefines));
}
#endif

void write_properly(unsigned int ad, char* arr, char rec_size)
{
  char checkmain;
  clearwdt(); // this function must be before the loop
  do
  {
    waitForMS50FLG();
    checkmain = pageWriteE2rom(ad, arr, rec_size,E2ROM);
  }while(checkmain!=0);
}

char read_alternately(unsigned int ad, unsigned int alternate_ad, char* arr, char rec_size)
{
  char retval=1;  // success
  readMultiple_e2rom(arr, ad, rec_size,E2ROM);
  if(isCheckSumOK(arr,rec_size)==0)
  {// first checksum is not ok
    readMultiple_e2rom(arr, alternate_ad, rec_size,E2ROM);
    if(isCheckSumOK(arr,rec_size)==0) 
      retval=0; // second read has also failed
  }
  return retval;
}

void write_alternately(unsigned int ad, unsigned int alternate_ad, char* arr, char rec_size)
{// in this function two writes are performed - first to the alternate address and then to the main address
// we could compute the checksum also
  arr[rec_size-1]=calcChecksum(arr,rec_size-1);
  write_properly(alternate_ad,arr,rec_size);
  write_properly(ad,arr,rec_size);
  
}

void clear_structure(char* arr, char rec_size)
{
  char i;
  for(i=0;i<rec_size;i++)
    arr[i]=0;
  
// we could also compute the checksum and avoid a reset  
// COMPUTING CHECKSUM IS DANGEROUS  SINCE AFTER RESET IT DOES NOT PICK THE DEFAULT VALUES SINCE CHECKSUM IS OK
}

void wre2rom(void)
{
  rtc_t rtc2;

  if((moreflags&MS50FLG)==0)
    return;
// don't make the flag 0 here, it will be made 0 in the pageWriteE2rom function

#ifndef PRODUCTION  
  #ifdef BLOCK_TAMPERS  
  unsigned int event_wrt_ptr;
  unsigned int event_base_address;
  unsigned int event_end_address;
  #endif  
  
//------------------ tampers recording ------------------  
//char tempchar;
  if(e2write_flags&E2_W_EVENTDATA)	// do we need to store the event
  {// this fellow is 8 bytes
    eventdata_t temp_eventdata;
    
    char whichevent;
    
    temp_eventdata.eventCode = mytmprdata.tamperEvent_flags & ~(mytmprdata.tamperEvent_flags-1);
    // the lowermost 1 of tamperEvent_flags is now in eventCode.
    whichevent = temp_eventdata.eventCode;  // only the event code not whether tamper started or ended
    
    if((mytmprdata.tamperEvent_on & temp_eventdata.eventCode) != 0)
      temp_eventdata.eventCode |= 0x01;

  #ifdef BLOCK_TAMPERS    
      switch(whichevent)
      {
    #ifdef REVERSE_TAMPER_RECORDING
        case NEGEN_TAMPER:
          event_wrt_ptr=mytmprdata.reverse_event_wrt_ptr;
          event_base_address=REVERSE_EVENT_BASE_ADDRESS;
          event_end_address=REVERSE_EVENT_END_ADDRESS;
          break;
    #endif  

    #ifdef EARTH_TAMPER_RECORDING
        case EARTH_TAMPER:
          event_wrt_ptr = mytmprdata.earth_event_wrt_ptr;
          event_base_address=EARTH_EVENT_BASE_ADDRESS;
          event_end_address=EARTH_EVENT_END_ADDRESS;
          break;      
    #endif  
          
    #ifdef NEUTRAL_TAMPER_RECORDING
        case NEUTRAL_TAMPER:
          event_wrt_ptr = mytmprdata.neutral_event_wrt_ptr;
          event_base_address=NEUTRAL_EVENT_BASE_ADDRESS;
          event_end_address=NEUTRAL_EVENT_END_ADDRESS;
          break;      
    #endif  
          
    #ifdef NEUTRAL_DISTURBANCE_RECORDING
        case NEUTRAL_DISTURBANCE:
          event_wrt_ptr = mytmprdata.neutral_disturbance_event_wrt_ptr;
          event_base_address=NEUTRAL_DISTURBANCE_EVENT_BASE_ADDRESS;
          event_end_address=NEUTRAL_DISTURBANCE_EVENT_END_ADDRESS;
          break;      
    #endif
          
    #ifdef MAGNET_TAMPER_RECORDING
        case REED_TAMPER:
          event_wrt_ptr = mytmprdata.magnet_event_wrt_ptr;
          event_base_address=MAGNET_EVENT_BASE_ADDRESS;
          event_end_address=MAGNET_EVENT_END_ADDRESS;
          break;      
    #endif  
          
    #ifdef COVER_TAMPER_RECORDING
        case COVER_TAMPER:
          event_wrt_ptr = mytmprdata.cover_event_wrt_ptr;
          event_base_address=COVER_EVENT_BASE_ADDRESS;
          event_end_address=COVER_EVENT_END_ADDRESS;
          break;      
    #endif

    #ifdef POWER_EVENT_RECORDING
        case POWER_EVENT:
          event_wrt_ptr = mytmprdata.power_event_wrt_ptr;
          event_base_address=POWER_EVENT_BASE_ADDRESS;
          event_end_address=POWER_EVENT_END_ADDRESS;
          break;      
    #endif

    #ifdef SETTINGS_RECORDING
        case SETTINGS_CHANGED:
          event_wrt_ptr = mytmprdata.settings_event_wrt_ptr;
          event_base_address=SETTINGS_EVENT_BASE_ADDRESS;
          event_end_address=SETTINGS_EVENT_END_ADDRESS;
          break;      
    #endif

    #ifdef LOW_VOLTAGE_RECORDING
        case LOW_VOLTAGE_TAMPER:
          event_wrt_ptr = mytmprdata.lowvolt_event_wrt_ptr;
          event_base_address=LOWVOLT_EVENT_BASE_ADDRESS;
          event_end_address=LOWVOLT_EVENT_END_ADDRESS;
          break;      
    #endif
          
          
      }
  #endif  // BLOCK_TAMPERS
    
// create the new tamper information
// we should read the rtc here to get the latest time
    
#ifdef EVENT_RECORD_HAS_KWH
    temp_eventdata.kwh = myenergydata.kwh;
#endif

#ifdef SETTINGS_RECORDING // 15/May/2011
    if(whichevent!=SETTINGS_CHANGED)
    {
      temp_eventdata.voltage = mysysvar.instant_voltage;
      temp_eventdata.current = mysysvar.instant_current;
      temp_eventdata.pf = mysysvar.instant_pf;
    }
    else
    {// here this is a settings changed event
      temp_eventdata.pf = whichSetting; // this fellow will which setting has been changed - global variable
      if((whichSetting==BILL_DATE_CHANGED)||(whichSetting==MD_INTERVAL_CHANGED))
      {
        temp_eventdata.voltage = previous_setting;  // global variable
        temp_eventdata.current = current_setting; // global variable
      }
      else
      {
        temp_eventdata.voltage = 0;  // not used in case of tod
        temp_eventdata.current = 0; // not used in case of tod
      }
    }
#else // 15/May/2011
    temp_eventdata.voltage = mysysvar.instant_voltage;
    temp_eventdata.current = mysysvar.instant_current;
    temp_eventdata.pf = mysysvar.instant_pf;
#endif  // 15/May/2011    

// capture the time    
    __disable_interrupt();  
    rtc2=rtc1;
    __enable_interrupt();  
    
    if((whichevent==POWER_EVENT)&&((mytmprdata.tamperEvent_on&temp_eventdata.eventCode)==0))
    {// the power fail event should have the power off time i.e. ctl data time
      temp_eventdata.date = ctldata.date;
      temp_eventdata.month = ctldata.month;
      temp_eventdata.year = ctldata.year;
      temp_eventdata.hours = ctldata.hours;
      temp_eventdata.minutes = ctldata.minutes;
      temp_eventdata.seconds = 0;
    }
    else
    {   
      temp_eventdata.date = rtc2.date;
      temp_eventdata.month = rtc2.month;
      temp_eventdata.year = rtc2.year;
      temp_eventdata.hours = rtc2.hours;
      temp_eventdata.minutes = rtc2.minutes;
      temp_eventdata.seconds = rtc2.seconds;
    }

    
// any change here must be made in two other places - three in all (globalvariables.c,e2rom.c,analog.c 
#ifdef TAMPER_BACK_DATING_SUPPORT
  #ifdef SETTINGS_RECORDING
    if(whichevent!=SETTINGS_CHANGED)
    {   
  #endif
//      if(((temp_eventdata.eventCode&1)!=0)&&(whichevent!=POWER_EVENT))
      if((temp_eventdata.eventCode&1)!=0)
      {// here we have an occurrence event except POWER_EVENT
        if(whichevent!=POWER_EVENT)
          decAnyDate((char*)&temp_eventdata.date,(TAMPER_PERSISTENCE_ON/30));  // decrement only for reed tampers
      }
  #ifdef SETTINGS_RECORDING
    }
  #endif
#endif

  #ifdef BLOCK_TAMPERS
    write_properly(event_wrt_ptr, (char*)&temp_eventdata, sizeof(temp_eventdata));
    incptr(&event_wrt_ptr, event_base_address, event_end_address, EVENT_PTR_INC);
    
    if(event_wrt_ptr==event_base_address)
    {// we have an overflow condition here- all the tamper locations have been written into
      mytmprdata.tamperEvent_overflow|=whichevent;  // set this flag for this event - Block Tampers
    }
// we need to restore the pointer - since now there are separate wrt_ptrs

      switch(whichevent)
      {
    #ifdef REVERSE_TAMPER_RECORDING
        case NEGEN_TAMPER:
          mytmprdata.reverse_event_wrt_ptr=event_wrt_ptr;
          break;
    #endif  
          
    #ifdef EARTH_TAMPER_RECORDING
        case EARTH_TAMPER:
          mytmprdata.earth_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif  
          
    #ifdef NEUTRAL_TAMPER_RECORDING
        case NEUTRAL_TAMPER:
          mytmprdata.neutral_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif  
          
    #ifdef NEUTRAL_DISTURBANCE_RECORDING
        case NEUTRAL_DISTURBANCE:
          mytmprdata.neutral_disturbance_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif

    #ifdef MAGNET_TAMPER_RECORDING
        case REED_TAMPER:
          mytmprdata.magnet_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif  
          
    #ifdef COVER_TAMPER_RECORDING
        case COVER_TAMPER:
          mytmprdata.cover_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif  
          
    #ifdef POWER_EVENT_RECORDING
        case POWER_EVENT:
          mytmprdata.power_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif

    #ifdef SETTINGS_RECORDING
        case SETTINGS_CHANGED:
          mytmprdata.settings_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif

    #ifdef LOW_VOLTAGE_RECORDING
        case LOW_VOLTAGE_TAMPER:
          mytmprdata.lowvolt_event_wrt_ptr=event_wrt_ptr;
          break;      
    #endif
      }
  #else
    write_properly(mytmprdata.tamper_event_wrt_ptr, (char*)&temp_eventdata, sizeof(temp_eventdata));
    incptr(&mytmprdata.tamper_event_wrt_ptr, EVENT_BASE_ADDRESS, EVENT_END_ADDRESS, EVENT_PTR_INC);
    if(mytmprdata.tamper_event_wrt_ptr==EVENT_BASE_ADDRESS)
    {// we have an overflow condition here- all the tamper locations have been written into
      mytmprdata.tamperEvent_overflow|=whichevent;  // set this flag for this event - SINGLE STORAGE
    }

  #endif  // BLOCK_TAMPERS    

    mytmprdata.tamperEvent_flags &= ~whichevent;
// now that the pointers have been changed - we must save mytmprdata
    write_alternately(MYTMPRDATA_BASE_ADDRESS, ALTERNATE_MYTMPRDATA_BASE_ADDRESS, (char*)&mytmprdata, sizeof(mytmprdata));
    
    __disable_interrupt();
    if(mytmprdata.tamperEvent_flags == 0)
      e2write_flags &= ~E2_W_EVENTDATA;
    __enable_interrupt();
    return;
    
// in init_read it is sufficient to read mytmprdata    
  }
  
#endif  // PRODUCTION

  if(e2write_flags&E2_W_IMPDATA)		// important data.
  {	// saved during calib, meterno entry - 22 bytes
//    Store_Calib_Param();
    Calib_Data.code = 0xAA;
    write_alternately(CALIB_DATA_ADDRESS, ALTERNATE_CALIB_DATA_ADDRESS, (char*)&Calib_Data, sizeof(Calib_Data));
    e2write_flags &= ~E2_W_IMPDATA;
    return;
  }
}

void readMultiple_e2rom(char* buf, unsigned int addr, char nBytes, char device)
{

//char tbuf[32];  // this was 24
char redoe2rom;
//char i;

  do
  {
    redoe2rom = readMultiple_e2rom_internal(buf,addr,nBytes,device);

/*
    delay();
    redoe2rom |= readMultiple_e2rom_internal(tbuf,addr,nBytes,device);

    // redoe2rom is 1 if either of the 2 reads returns 1 (failure)
    if(device==RTC)
      i=1;
    else
      i=0;

    for(;i<nBytes && redoe2rom == 0;i++)
    {
      if(buf[i] != tbuf[i])
        redoe2rom = 1;
    }
*/
  }
  while(redoe2rom != 0);
}

char readMultiple_e2rom_internal(char* buf, unsigned int addr, char nBytes, char device)
{// returns 0 on success
  char i=0;
  char e2adhi = addr/256;
  char e2adlo = addr%256;
  char device_code=0xa0;
  char e2rom_ackf;

  clearwdt();
//  e2error = 0;
  e2rom_crtstrt();

  if(device == RTC)
  {
    #ifdef PHILIPS_RTC
      device_code=0xa2;	// PHILIPS RTC device code
    #else
      device_code=0xd0;
    #endif
  }

  e2rom_ackf = sende2rom(device_code);	
  
  while (e2rom_ackf == 0)
  {
    e2rom_crtstop();
    dely();
    e2rom_crtstrt();
  // first send a dummy write command to load the address
    e2rom_ackf = sende2rom(device_code);
  }

  if(device != RTC)
  {
    if(sende2rom(e2adhi)==0)
    {
      e2rom_crtstop();
      return 1;
    }
  }

  if(sende2rom(e2adlo)==0)
  {
    e2rom_crtstop();
    return 1;
  }

  e2rom_restrt();	// restart

  if(sende2rom(device_code|0x01)==0)
  {
    e2rom_crtstop();
    return 1;
  }

  buf[i] = e2rom_rcvbyt();
  i++;

  while (i!=nBytes)
  {
    e2rom_sndack();// send an acknowledgement
    buf[i]=e2rom_rcvbyt();
    i++;
  }

  e2rom_noack();
  e2rom_crtstop();

  return 0;		// success
}

void zap(unsigned int start_ad, unsigned int end_ad, char zapbyte)
{
//  char i;
  char check;
//  char zarr[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  char zarr[16];
  
  for(check=0;check<16;check++)
    zarr[check]=zapbyte;

  unsigned int addr=start_ad;
//  char checkresult=0;
  while(addr < end_ad)
  {
    clearwdt(); // this function must be before the loop
    do
    {
      waitForMS50FLG();
      moreflags&=~MS50FLG;
//      clearwdt(); // this is already being done in pageWriteE2rom
      check = pageWriteE2rom(addr,zarr,16,E2ROM);
    }
    while(check!=0);
    addr += 16;
//    lcd_clearTopLine();
//    lcd_display_int(addr);
  }
  
}




#if defined (PRODUCTION) || defined (TESTING)

unsigned int checke2rom(void)
{
// now check whether memory is fully cleared
  char zarr[32];
  unsigned int addr = 0x00;
  unsigned int checkresult=0;
  char i;

  lcd_clearTopLine();	
  
//krishna  
#ifdef MEM8K  
  while(addr != 0x2000)
#else
  while(addr != 0x1000)
#endif    
  {
    readMultiple_e2rom(zarr, addr, 32,E2ROM);

    for(i=0;i<32;i++)
    {  
#ifdef DELETE_ALL_EXCEPT_CALIBRATION
      if((addr>=CALIB_DATA_ADDRESS)&&(addr<(CALIB_DATA_ADDRESS+32)))
      {// do nothing
      }
      else
      {// here the addresses do not belong to IMPDATA
        if(zarr[i]!=0)
          checkresult++;
      }
#else      
      if(zarr[i]!=0)
        checkresult++;
#endif
    }
    addr += 32;
    lcd_clearTopLine();
    lcd_display_long(addr,5);
  }

  return checkresult; //0-success, non zero failure

}


void cleare2rom(void)
{
  char check;
  char zarr[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned int addr = 0x00;
#ifdef MEM8K  
  while(addr != 0x2000)
#else
  while(addr != 0x1000)
#endif    
  {
    clearwdt(); // this function must be before the loop
    do
    {
      waitForMS50FLG();
      moreflags&=~MS50FLG;
//      clearwdt();

#ifdef DELETE_ALL_EXCEPT_CALIBRATION
      if(addr!=CALIB_DATA_ADDRESS)  // this address must be on a 32 byte boundary
      {  
        check = pageWriteE2rom(addr,zarr,32,E2ROM);
      }
      else
      {// here the addr==CALIB_DATA_ADDRESS
        check = 0;
      }
#else
      check = pageWriteE2rom(addr,zarr,32,E2ROM);
#endif      
    }
    while(check!=0);
    addr += 32;
    lcd_clearTopLine();
    lcd_display_long(addr,5);
  }
  
  seconds=0;
  do
  {
    erase_rtc_func(); // this function is written for erase purposes only
    delay();
  }
  while (seconds < 2);

  
// now do the rtc bit
  
  rtc1.seconds=0x00;
  rtc1.minutes=0x00;
  rtc1.hours=0x00;
  rtc1.day=1;
  rtc1.date=10;
  rtc1.month=11;
  rtc1.year=11;
//  rtc1.Control=0x00;
  lcd_displayChar(D_LCD, 5);	

}

#endif


