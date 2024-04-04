

/**
* \addtogroup mlsCIEng
* @{
*/

/**
* @file mlsCIEng.cpp
* @brief mlsCIEng
*
* This port is purposely to debug the CI framework in windows os
*
*/

/*----------------------------------------------------------------------------*/


#include "mlsCIFConfig.h"
#include "format.h"
#include <string.h>
/* Create Dummy Data For Test */

#if (MLS_CIENG_USE_DUMMY_DATA == 1)

#include "mlsDummy.h"

void mlsDummyFill(Byte * buff, UInt8 len)
{
    UInt8 idx;
    for (idx = 0; idx < len; idx++)
    {
        *(buff+idx) = idx;
    }
}
#endif

void mlsDummyFillEnergyLogged(void)
{
//  int blockid,i = 0;
//  unsigned long *ptr32;
//  ONE_MONTH_ENERGY_DATA_LOG buff;
//  uint8_t buff8[sizeof(ONE_MONTH_ENERGY_DATA_LOG)];
    
//  unsigned short addr;
//  unsigned short size;
    
//  /* Max demand: max value of uint64_t decrease down */
//  #if 0
//  ptr32 = &buff.present_max_demand.active.l32;
//  *ptr32 = 0xffffffff;
//  ptr32 = &buff.present_max_demand.reactive.l32;
//  *ptr32 = 0xffffffff;
//  ptr32 = &buff.present_max_demand.apparent.l32;
//  *ptr32 = 0xffffffff;
//  ptr32 = &buff.present_max_demand.active.h32;
//  *ptr32 = 0xffffffff;
//  ptr32 = &buff.present_max_demand.reactive.h32;
//  *ptr32 = 0xffffffff;
//  ptr32 = &buff.present_max_demand.apparent.h32;
//  *ptr32 = 0xffffffff;
//  #endif
//  /* Energy total: Max subnormal double decrease down
//  ptr32 = (unsigned long *)&buff.present_energy_total.active.l32;
//  *ptr32 = 0x00000000;
//  ptr32 = (unsigned long *)&buff.present_energy_total.reactive.l32;
//  *ptr32 = 0x00000000;
//  ptr32 = (unsigned long *)&buff.present_energy_total.apparent.l32;
//  *ptr32 = 0x00000000;
//  ptr32 = (unsigned long *)&buff.present_energy_total.active.h32;
//  *ptr32 = 0x40508000;
//  ptr32 = (unsigned long *)&buff.present_energy_total.reactive.h32;
//  *ptr32 = 0x40508000;
//  ptr32 = (unsigned long *)&buff.present_energy_total.apparent.h32;
//  *ptr32 = 0x40508000;    
//  */
//  /* Energy tariff: Max value of double decrease down*/
//  for (i=0; i<6; i++)
//  {   
//      /* Start value 66 
//      ptr32 = (unsigned long *)&buff.present_energy_tariff[i].active.l32;
//      *ptr32 = 0x00000000;
//      ptr32 = (unsigned long *)&buff.present_energy_tariff[i].reactive.l32;
//      *ptr32 = 0x00000000;
//      ptr32 = (unsigned long *)&buff.present_energy_tariff[i].apparent.l32;
//      *ptr32 = 0x00000000;
//      ptr32 = (unsigned long *)&buff.present_energy_tariff[i].active.h32;
//      *ptr32 = 0x40508000;
//      ptr32 = (unsigned long *)&buff.present_energy_tariff[i].reactive.h32;
//      *ptr32 = 0x40508000;
//      ptr32 = (unsigned long *)&buff.present_energy_tariff[i].apparent.h32;
//      *ptr32 = 0x40508000;
//      */
//  }
//  buff.present_rtc_log.Sec = 00;
//  buff.present_rtc_log.Min = 55;
//  buff.present_rtc_log.Hour = 4;
//  buff.present_rtc_log.Day = 27;
//  buff.present_rtc_log.Week = 1;
//  buff.present_rtc_log.Year = 14;
//  buff.present_rtc_log.Month = 12;
    
//  for (blockid=0; blockid< 12; blockid++)
//  {
//      memcpy(buff8, &buff, sizeof(ONE_MONTH_ENERGY_DATA_LOG));
//      addr = CONFIG_STORAGE_ENERGY_DATA_LOG_ADDR + blockid*sizeof(ONE_MONTH_ENERGY_DATA_LOG);
//      size = sizeof(ONE_MONTH_ENERGY_DATA_LOG);
//      if (EPR_Write(
//              addr,
//              buff8,
//              size
//          ) != EPR_OK)
//      {
//          NOP();
//      }       
//      /* Max demand: max value of uint64_t decrease down */
//      ptr32 = (unsigned long *)&buff.present_max_demand.active.l32;
//      *ptr32 = *ptr32 - 1;
//      ptr32 = (unsigned long *)&buff.present_max_demand.reactive.l32;
//      *ptr32 = *ptr32 - 1;
//      ptr32 = (unsigned long *)&buff.present_max_demand.apparent.l32;
//      *ptr32 = *ptr32 - 1;

//      /* Energy total: Max subnormal double decrease down
//      ptr32 = (unsigned long *)&buff.present_energy_total.active.h32;
//      *ptr32 = *ptr32 - 0x4000;
//      ptr32 = (unsigned long *)&buff.present_energy_total.reactive.h32;
//      *ptr32 = *ptr32 - 0x4000;
//      ptr32 = (unsigned long *)&buff.present_energy_total.apparent.h32;
//      *ptr32 = *ptr32 - 0x4000;
//*/
//      /* Energy tariff: Max value of double decrease down
//      for (i=0;i<6;i++)
//      {
//          ptr32 = (unsigned long *)&buff.present_energy_tariff[i].active.h32;
//          *ptr32 = *ptr32 - 0x4000;
//          ptr32 = (unsigned long *)&buff.present_energy_tariff[i].reactive.h32;
//          *ptr32 = *ptr32 - 0x4000;
//          ptr32 = (unsigned long *)&buff.present_energy_tariff[i].apparent.h32;
//          *ptr32 = *ptr32 - 0x4000;
//      }
//      buff.present_rtc_log.Month = buff.present_rtc_log.Month - 1;
//      */
//  }
}

void mlsDummyFillTamperLogged(void)
{
//  unsigned int blockid,i = 0;
//  ONE_TAMPER_DATA_LOG buff;
//  uint8_t buff8[sizeof(ONE_TAMPER_DATA_LOG) * 20];
//  uint8_t *ptr8 = buff8;
    
//  unsigned short addr;
//  unsigned short size;

//  buff.tamper_rtc_time.Sec = 00;
//  buff.tamper_rtc_time.Min = 55;
//  buff.tamper_rtc_time.Hour = 4;
//  buff.tamper_rtc_time.Day = 28;
//  buff.tamper_rtc_time.Week = 1;
//  buff.tamper_rtc_time.Year = 14;
//  buff.tamper_rtc_time.Month = 10;
    
    
//  for (i=0; i<200; i++)
//  {
//      buff.tamper_type.reverse_current = 0;
//      buff.tamper_type.earth_connected = 0;
//      buff.tamper_type.neutral_missing = 0;
//      buff.tamper_type.case_open = 0;
//      buff.tamper_type.magnet = 0;
//      buff.tamper_type.power_failed = 0;
        
//      if(i==1999)
//      {
//          NOP();  
//      }
        
//      if ( (i%6)==0 )
//      {
//          buff.tamper_type.magnet = 1;
//      }
//      else if ((i%6)==1)
//      {
//          buff.tamper_type.case_open = 1;         
//      }
//      else if ((i%6)==2)
//      {
//          buff.tamper_type.neutral_missing = 1;           
//      }
//      else if ((i%6)==3)
//      {
//          buff.tamper_type.earth_connected = 1;           
//      }
//      else if ((i%6)==4)
//      {
//          buff.tamper_type.reverse_current = 1;           
//      }
//      else if ((i%6)==5)
//      {
//          buff.tamper_type.power_failed  = 1;         
//      }
//      buff.tamper_rtc_time.Year = buff.tamper_rtc_time.Year + 1;
        
//      if ( ((i+1) % 20) != 0)
//      {
//          memcpy(ptr8, &buff, sizeof(ONE_TAMPER_DATA_LOG));
//          ptr8 += sizeof(ONE_TAMPER_DATA_LOG);
//      }
//      else
//      {
//          memcpy(ptr8, &buff, sizeof(ONE_TAMPER_DATA_LOG));           
//          addr = CONFIG_STORAGE_TAMPER_DATA_LOG_ADDR + (((i+1)/20)-1) * (sizeof(ONE_TAMPER_DATA_LOG)*20);
//          size = sizeof(ONE_TAMPER_DATA_LOG) * 20;
//          if (EPR_Write(
//                  addr,
//                  buff8,
//                  size
//              ) != EPR_OK)
//          {
//              NOP();
//          }       
//          ptr8 = buff8;
//      }
//  }
}


