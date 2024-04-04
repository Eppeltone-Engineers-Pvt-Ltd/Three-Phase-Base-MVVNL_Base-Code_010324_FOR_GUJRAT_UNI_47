#include "iodefine.h"
#include "typedef.h"            // located right at top of tree below iodefine.h
#include "r_cg_wdt.h"
#include "wrp_mcu.h"            // for MCU_Delay function
#include "eeprom.h"
#include "e2rom.h"
#include "memoryOps.h"

#define _BV(i)	(1<<i)


void EPR_Write_Helper(uint32_t addr, uint8_t* buf, uint16_t size);	// private function added on 13/Jul/2020
void EPR_Read_Helper(uint32_t addr, uint8_t* buf, uint16_t size);	// private function added on 13/Jul/2020


void dely(uint8_t mcu_freq);
void e2rom_crtstop(void);
void e2rom_crtstrt(void);
void e2rom_restrt(void);
void e2rom_sndack(void);
void e2rom_noack(void);
uint8_t e2rom_rcvbyt(void);
uint8_t sende2rom(uint8_t a);

uint8_t pageWriteE2rom(uint32_t e2address, uint8_t e2buf[], uint16_t nBytes);// address is now a long
//uint8_t pageWriteE2rom(uint16_t e2address, uint8_t e2buf[], uint8_t nBytes);
//void write_properly(uint16_t ad, uint8_t* arr, uint8_t rec_size);
void readMultiple_e2rom(uint32_t address, uint8_t* buf, uint16_t nBytes);
uint8_t readMultiple_e2rom_internal(uint32_t addr, uint8_t* buf, uint16_t nBytes);


static uint8_t g_is_send_end    = 0;
static uint8_t g_is_receive_end = 0;


void EPR_Init(void)
{// make the SCLK pin high and make it an output
  SCLK_e2rom_HI;
  SCLK_e2rom_MAKEOUT;
  
	g_is_send_end    = 0;
	g_is_receive_end = 0;

}


/*
void disable_eeprom_port(void)
{// make the SCLK pin an input - and rely on the pullups
  SCLK_e2rom_MAKEINP;
}

void enable_eeprom_port(void)
{// make the SCLK pin high and make it an output
  SCLK_e2rom_HI;
  SCLK_e2rom_MAKEOUT;
}

void external_eeprom_init(void)
{// make the SCLK pin high and make it an output
  SCLK_e2rom_HI;
  SCLK_e2rom_MAKEOUT;
}
*/

void dely(uint8_t mcu_freq)
{
  volatile uint8_t i=0;
// frequency dependent

//  i = mcu_freq*2; // this was 2 earlier
  i = mcu_freq*4; // this was 2 earlier
  do
  {
    i--;
  }while (i!=0);
}

void e2rom_crtstop(void)
{
  SDA_e2rom_MAKEOUT;
  SDA_e2rom_LOW;
  dely(16);
  SCLK_e2rom_HI;
  
  dely(16);
  // after this i have to change direction of SDA_e2rom
  SDA_e2rom_HI;
  dely(16);
  SDA_e2rom_MAKEINP;
}



void e2rom_crtstrt(void)
{
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  SDA_e2rom_LOW;
  dely(16);
  SCLK_e2rom_LOW;  //lcall sclklo
  dely(16);
}



void e2rom_restrt(void)
{
  SDA_e2rom_HI;
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  dely(16);
  SCLK_e2rom_HI;
  dely(16);
  SDA_e2rom_LOW;
  dely(16);
  SCLK_e2rom_LOW;
  dely(16);
}

void e2rom_sndack(void)
{// this fellow is called during multiple reads
// the rcvbyt function keeps SDA_LOW and SDA_OUT
// hence all we need to do is to clock it.
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  SDA_e2rom_LOW;
  dely(16);
  SCLK_e2rom_HI;
  dely(16);
  
  SCLK_e2rom_LOW;
  dely(16);
  SDA_e2rom_MAKEINP;
  SDA_e2rom_HI;
}

void e2rom_noack(void)
{
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin
  SDA_e2rom_HI;
  dely(16);
  SCLK_e2rom_HI;
  dely(16);
  
  SCLK_e2rom_LOW;
  dely(16);
  SDA_e2rom_LOW;
}


uint8_t e2rom_rcvbyt(void)
{

  uint8_t i;
  uint8_t retByte=0;

  SDA_e2rom_HI;
  SDA_e2rom_MAKEINP;
  //msb first since   rlc A
  i=8;
  i--;

  dely(16);
  
  SCLK_e2rom_HI;
  
  if(MY_SDA_e2rom!=0)
    retByte |= _BV(i);

  dely(16);
  
  do
  {
    SCLK_e2rom_LOW;
    dely(16);
    
    SCLK_e2rom_HI;
    i--;
    if(MY_SDA_e2rom!=0)
      retByte |= _BV(i);
    dely(16);
  }while(i!=0);

  SCLK_e2rom_LOW;
  dely(16);

  return retByte;
}



uint8_t sende2rom(uint8_t a)
{// this fellow will return e2rom_ackflag which is now a local variable.

uint8_t mask=0x80;
uint8_t e2rom_ackflag;  

//  e2rom_sndbyt(a);
// The following code was initially in e2rom_sndbyt(a)
//----------------------------------------------------
//check direction of MY_SDA_e2rom
// sda is output when this function is called
  SDA_e2rom_MAKEOUT; // make SDA_e2rom output pin

  if( (a & mask) ==0)
    SDA_e2rom_LOW;
  else SDA_e2rom_HI; 

  dely(16);
  
  SCLK_e2rom_HI;
  dely(16);

  SCLK_e2rom_LOW;
  
  do
  {	
    dely(16);
    mask = mask/2;
    if( (a & mask) ==0)
      SDA_e2rom_LOW;
    else 
      SDA_e2rom_HI; 
    dely(16);
    
    SCLK_e2rom_HI;
    dely(16);
    SCLK_e2rom_LOW;
  }while(mask!=1);

  SDA_e2rom_MAKEINP;
  SDA_e2rom_HI;
  dely(16);

// The following code was initially in e2rom_rcvack()
//---------------------------------------------  
  e2rom_ackflag=1;
  SCLK_e2rom_HI;

  dely(16);
  if(MY_SDA_e2rom!=0)
    e2rom_ackflag=0;
  SCLK_e2rom_LOW;
  dely(16);
//---------------------------------------------  
  return e2rom_ackflag;
}

uint8_t pageWriteE2rom(uint32_t addr, uint8_t e2buf[], uint16_t nBytes)
// write nBytes in e2buf at e2address
// return: 0 for success 1 for failure
{
  uint16_t i=0;
  uint8_t e2adhi = (uint8_t)((addr>>8)&0xFF);// upper byte of 16 bit address
  uint8_t e2adlo = (uint8_t)(addr&0xFF);	// lower byte of 16 bit address
// the bit locations for bit 16 and 17 appear to be different from Microchip and ST devices
// the following line must be corrected accordingly - to be done at two places
//	uint8_t device_code=0xa0 |(uint8_t)((addr >> 15) & 0x02);	// get bit 16 into correct position
	uint8_t device_code=0xa0 |(uint8_t)((addr >> 15) & 0x0e);	// get A2,A1,A0 bits into correct position (Bits 3,2,1)
	uint8_t byt;
	
	e2rom_crtstrt();

	if(sende2rom(device_code)==0)
	{
		e2rom_crtstop();
		return 1;
	}

	if(sende2rom(e2adhi)==0)
	{
		e2rom_crtstop();
		return 1;
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
	
  MCU_Delay(5000);	// this function generates delay of 5 ms
//  MCU_Delay(10000);	// this function generates delay of 10 ms
	
//	moreflags&=~MS50FLG;		// signifying that e2rom is now busy for 50 ms

	return 0;
}

/*
void EPR_Write_Helper_TEST(uint32_t addr, uint8_t* buf, uint16_t size)
{
	uint16_t page_size;
	uint8_t checkmain;
	
//  if (size == 0)
//    return;
	
	while(size > 0)
	{
    page_size = EPR_DEVICE_PAGESIZE - (uint16_t)(addr % EPR_DEVICE_PAGESIZE);
    
		if (size < page_size)
      page_size = size;
		
		R_WDT_Restart();
	  do
		{
			MCU_Delay(100);	// this function generates delay of 100 microseconds
//			checkmain = pageWriteE2rom(addr, buf, page_size);
			checkmain=0;
		}while(checkmain!=0);
		
		addr += page_size;
		buf  += page_size;
		size -= page_size;
	}
}
*/

// this new version now allows data to be written across chip boundaries
uint8_t EPR_Write(uint32_t addr, uint8_t* buf, uint16_t size)
{
	uint16_t bytesInThisChip;
	uint16_t bytesInNextChip;
	uint8_t startingChip;
	uint8_t endingChip;
	
  if (buf == NULL)
    return EPR_ERROR;
  
  if (size == 0 || ((addr + size) > EPR_DEVICE_SIZE))
    return EPR_ERROR_SIZE;

	startingChip = addr/(int32_t)CHIP_SIZE;
	endingChip = (addr+(int32_t)size)/(int32_t)CHIP_SIZE;
	
	if(startingChip==endingChip)
	{// both addresses are in the same chip - no problem at all
//		EPR_Write_Helper_TEST(addr, buf, size);
		EPR_Write_Helper(addr, buf, size);
	}
	else
	{// here the two chips are different - split the writes
		bytesInThisChip = ((int32_t)(startingChip+1))*(int32_t)CHIP_SIZE - (int32_t)addr;
		bytesInNextChip = size - bytesInThisChip;

		// write the bytes for this chip
//		EPR_Write_Helper_TEST(addr, buf, bytesInThisChip);
		EPR_Write_Helper(addr, buf, bytesInThisChip);
		
		addr+=(int32_t)bytesInThisChip;
		buf+=bytesInThisChip;
		
		// write the bytes for the next chip
//		EPR_Write_Helper_TEST(addr, buf, bytesInNextChip);
		EPR_Write_Helper(addr, buf, bytesInNextChip);
	}
				
	return EPR_OK;
}

void EPR_Write_Helper(uint32_t addr, uint8_t* buf, uint16_t size)
{
	uint16_t page_size;
	uint8_t checkmain;

	uint8_t tryCount;
	
// the following may be required	
  if (size == 0)
    return;
	
	while(size > 0)
	{
    page_size = EPR_DEVICE_PAGESIZE - (uint16_t)(addr % EPR_DEVICE_PAGESIZE);
    
		if (size < page_size)
      page_size = size;
		
		tryCount=0;	
		R_WDT_Restart();
	  do
		{
			MCU_Delay(100);	// this function generates delay of 100 microseconds
			checkmain = pageWriteE2rom(addr, buf, page_size);
			tryCount++;
			if(tryCount>=10)
				checkmain=0;	// forcibly exit
		}while(checkmain!=0);
		
		addr += page_size;
		buf  += page_size;
		size -= page_size;
	}
}


uint8_t EPR_Read(uint32_t addr, uint8_t* buf, uint16_t size)
{
	
	uint16_t bytesInThisChip;
	uint16_t bytesInNextChip;
	uint8_t startingChip;
	uint8_t endingChip;

	uint8_t redoe2rom;

  if (buf == NULL)
	  return EPR_ERROR;
  
  if (size == 0 || (addr + size) > EPR_DEVICE_SIZE)
    return EPR_ERROR_SIZE;
	
	startingChip = addr/(int32_t)CHIP_SIZE;
	endingChip = (addr+(int32_t)size)/(int32_t)CHIP_SIZE;
	
	if(startingChip==endingChip)
	{// both addresses are in the same chip - no problem at all
		EPR_Read_Helper(addr, buf, size);
	}
	else
	{// here the two chips are different - split the reads
		bytesInThisChip = ((int32_t)(startingChip+1))*(int32_t)CHIP_SIZE - (int32_t)addr;
		bytesInNextChip = size - bytesInThisChip;

		// read the bytes from this chip
		EPR_Read_Helper(addr, buf, bytesInThisChip);
		
		addr+=(int32_t)bytesInThisChip;
		buf+=bytesInThisChip;
		
		// read the bytes from the next chip
		EPR_Read_Helper(addr, buf, bytesInNextChip);
	}
  return EPR_OK;
}

void EPR_Read_Helper(uint32_t addr, uint8_t* buf, uint16_t size)
{
	uint8_t redoe2rom;

	R_WDT_Restart();
	
// the following may be required	
  if (size == 0)
    return;
	
  do
  {
    redoe2rom = readMultiple_e2rom_internal(addr,buf,size);
  }
  while(redoe2rom != 0);
	
// the function readMultiple_e2rom_internal may return a 0 value if it fails for more than 10 tries - 25/Jul/2023	
// however in such a case the buffer will be filled with 0's
}


//--------------------the following functions are superseded ---------------------------------
/*
// this function commented on 13/Jul/2020
//void write_properly(uint16_t ad, uint8_t* arr, uint8_t rec_size)
// The following routine has been modified so that now we can write any number of bytes from any address
// All page boundaries will be accounted for - a single call to this function may result in multiple writes
uint8_t EPR_Write(uint32_t addr, uint8_t* buf, uint16_t size)
{
	uint16_t page_size;
	uint8_t checkmain;
	
  if (buf == NULL)
    return EPR_ERROR;
  
  if (size == 0 || ((addr + size) > EPR_DEVICE_SIZE))
    return EPR_ERROR_SIZE;

// Make WP low to enable writing		
//	P6.2=0;	
	while(size > 0)
	{
    page_size = EPR_DEVICE_PAGESIZE - (uint16_t)(addr % EPR_DEVICE_PAGESIZE);
    
		if (size < page_size)
      page_size = size;
		
		R_WDT_Restart();
	  do
		{
			MCU_Delay(100);	// this function generates delay of 100 microseconds
			checkmain = pageWriteE2rom(addr, buf, page_size);
		}while(checkmain!=0);
		
		addr += page_size;
		buf  += page_size;
		size -= page_size;
	}
	
// Make WP high to disable writing
//	P6.2=1;
	return EPR_OK;
}

uint8_t EPR_Read(uint32_t addr, uint8_t* buf, uint16_t size)
{
	uint8_t redoe2rom;

  if (buf == NULL)
	  return EPR_ERROR;
  
  if (size == 0 || (addr + size) > EPR_DEVICE_SIZE)
    return EPR_ERROR_SIZE;
	
	
	R_WDT_Restart();
	
  do
  {
    redoe2rom = readMultiple_e2rom_internal(addr,buf,size);
  }
  while(redoe2rom != 0);
  return EPR_OK;
}
*/

uint8_t readMultiple_e2rom_internal(uint32_t addr, uint8_t* buf, uint16_t nBytes)
{// returns 0 on success
  uint16_t i=0;
  uint8_t e2adhi = (uint8_t)((addr>>8)&0xFF);// upper byte of 16 bit address
  uint8_t e2adlo = (uint8_t)(addr&0xFF);	// lower byte of 16 bit address
// the bit locations for bit 16 and 17 appear to be different from Microchip and ST devices
// the following line must be corrected accordingly - to be done at two places
//	uint8_t device_code=0xa0 |(uint8_t)((addr >> 15) & 0x02);	// get bit 16 into correct position
	uint8_t device_code=0xa0 |(uint8_t)((addr >> 15) & 0x0e);	// get A2,A1,A0 bits into correct position (Bits 3,2,1)
	
	
  uint8_t e2rom_ackf;
	uint8_t j,tryCount;

//  e2error = 0;
  e2rom_crtstrt();

// for e2roms where more than 64K data may be stored, then bit 16 and 17
// for addresses > 64K, 128K are send alongwith the device_code
	
  e2rom_ackf = sende2rom(device_code);	

	tryCount=0;
	R_WDT_Restart();
	
  while (e2rom_ackf == 0)
  {
    e2rom_crtstop();
    dely(16);
    e2rom_crtstrt();
  // first send a dummy write command to load the address
    e2rom_ackf = sende2rom(device_code);
		tryCount++;
		if(tryCount>=10)
		{// as a safety measure return with error and make the entire buffer 0
			for(j=0;j<nBytes;j++)
				buf[j]=0;
			return 0;
		}
  }

	if(sende2rom(e2adhi)==0)
	{
	  e2rom_crtstop();
	  return 1;
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




