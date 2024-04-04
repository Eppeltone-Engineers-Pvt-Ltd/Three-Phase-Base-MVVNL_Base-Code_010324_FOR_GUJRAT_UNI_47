// iHexParser.c
#include "typedef.h"            // located right at top of tree below iodefine.h
#include "emeter3_structs.h"
#include "iHexParser.h"
#include "utilities.h"
#include "memoryOps.h"
#include "e2rom.h"

/*
	function will return success(1) / failure(0) if checksum is OK/NOT OK
	srcBuf points to the buffer to be analysed - need not be terminated with <CR><LF>
	*addr and *opCode will be used to return the address & opCode encountered in the srcBuf
	eg Hex Srtings
	The checksum is the 2's complement of all hex bytes excluding ':' and the checksum itself
	Alternately adding all the bytes excluding ':' but including the checksum should yield 00
	
	The first byte after : is the number of data elements
	The non-counted elements are 
		two bytes of address
		one byte of dataType/opCode
		one byte of checksum
	Hence checksum loop should be carried out over (no_of_data_elements + 4)		
	
	:10 C0 00 00 00 00 11 00 23 00 34 00 46 00 57 00 69 00 7A 00 48
	:10C010008B009C00AE00BF00D000E100F2000301E5
	:10C02000140124013501460156016601770187019B
	:00C24801F5
	:00000001FF
	
	
// SETDSPSTART
// SETDSPEND

:nnaaaattdddddd...nntimes cc
tt
00 - autoscroll
01 - pushbutton
02 - high resolution
	
	
*/


//	  00 00 00 00 01 11	11 11 11 12 22 22 22 22 23 33 33 33 33 34 44
//		12 34 56 78 90 12	34 56 78 90	12 34 56 78 90 12 34 56 78 90 12
//	: 10 C0 00 00 00 00 11 00 23 00 34 00 46 00 57 00 69 00 7A 00 48

uint8_t iHexParse(uint8_t *srcBuf, uint16_t *addr, uint8_t *opCode, uint8_t *dataCount, uint8_t *outBuf)
{
	uint8_t retval;
	uint8_t i;
	uint8_t j;
	uint8_t addrHi;
	uint8_t addrLo;
	uint8_t chksum;
	
	retval = 0;	// failure
	
	if(srcBuf[0]!=':')
		return retval;	// first character must be ':'

	i=1;
	*dataCount = hexchars2byte(srcBuf[i],srcBuf[i+1]);	// 1,2
	
// in addition to dataCount bytes of data, there will be four more bytes
// these are address(2) + opCode(1) + checksum(1) 
	chksum = *dataCount;	// dataCount is to be included in the chksum
	
// first get the address	
	i=i+2;
	addrHi = hexchars2byte(srcBuf[i],srcBuf[i+1]); // 3,4
	chksum+=addrHi;
	
	i=i+2;
	addrLo = hexchars2byte(srcBuf[i],srcBuf[i+1]); // 5,6
	chksum+=addrLo;
	
	*addr = addrHi*256 + addrLo;

// now get the opCode
	i=i+2;
	*opCode = hexchars2byte(srcBuf[i],srcBuf[i+1]); // 7,8
	chksum+= *opCode;
	
// now get dataCount number of dataBytes	
	for (j=0;j<(*dataCount);j++)
	{
		i=i+2;
		outBuf[j] = hexchars2byte(srcBuf[i],srcBuf[i+1]);
		chksum+= outBuf[j];
	}
	
// now for the last byte which is the checksum		
	i=i+2;
	chksum += hexchars2byte(srcBuf[i],srcBuf[i+1]); // 41,42 - for the example
	
	if(chksum==0)
		retval =1;	// success
// address, opCode and databytes are already filled into the appropriate locations	
	return retval;
}


//uint8_t iHexParse(uint8_t *srcBuf, uint16_t *addr, uint8_t *opCode, uint8_t *dataCount, uint8_t *outBuf)

uint8_t handleDisplayProgrammingCommands(uint8_t *srcBuf)
{// this function is called only when g_dsp_progFlag = 1

	uint8_t outBuf[32];
	uint16_t addr;
	uint16_t pgmMaxSize;
	uint8_t opCode;
	uint8_t retval;
	int8_t pgmStartIndex;
	uint8_t dataCount;
	uint8_t i;
	
	if(0==iHexParse(srcBuf, &addr, &opCode, &dataCount, outBuf))	// returns 1 on success
	{
		return 0; // error
	}
	
// here the dsp programming command line has been received properly
	retval = 1; // all good
	switch(opCode)
	{
		case 0:
			pgmMaxSize = AUTOSCROLL_SIZE;
			pgmStartIndex = AUTOSCROLL_OFFSET;
			break;
			
		case 1:
			pgmMaxSize = PUSHBUTTON_SIZE;
			pgmStartIndex = PUSHBUTTON_OFFSET;
			break;
			
		case 2:
			pgmMaxSize = HIGHRES_SIZE;
			pgmStartIndex = HIGHRES_OFFSET;
			break;
			
		default:
			pgmMaxSize = 0;
			break;
			
	}
	
	if(pgmMaxSize==0)
		return 0;	// opCode is not good
		
	if((addr + dataCount)>=pgmMaxSize)
		return 0;	// dps_pgm_length is greater than can be accomodated

// here message recd properly & opCode is good & program length is within limits		
	for(i=0;i<dataCount;i++)
	{
		dspArr[pgmStartIndex + addr + i] = outBuf[i];
	}
	return 1;	// success
}

void updateDisplayProgrammingCommands(void)
{
#ifdef TN3P_BOARD
// all we have to do is to store the dspArr into E2rom
	EPR_Write(DSP_PROG_ADDRESS, (uint8_t*)dspArr, DSP_ARRAY_SIZE);
	Common_Data.displayProgrammed=1;
  write_alternately(COMMON_DATA_ADDRESS, ALTERNATE_COMMON_DATA_ADDRESS, (uint8_t*)&Common_Data, sizeof(Common_Data));	
#endif	
	g_dsp_progFlag=0;
}