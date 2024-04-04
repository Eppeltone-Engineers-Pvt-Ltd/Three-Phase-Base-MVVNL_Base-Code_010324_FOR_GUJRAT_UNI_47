#ifndef IHEXPARSER_H
#define IHEXPARSER_H

/*
	function will return success(1) / failure(0) if checksum is OK/NOT OK
	srcBuf points to the buffer to be analysed - need not be terminated with <CR><LF>
	*addr and *opCode will be used to return the address & opCode encountered in the srcBuf
	eg Hex Srtings
	:10C0000000001100230034004600570069007A0048
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
uint8_t iHexParse(uint8_t *srcBuf, uint16_t *addr, uint8_t *opCode, uint8_t *dataCount, uint8_t *outBuf);
uint8_t handleDisplayProgrammingCommands(uint8_t *srcBuf);
void updateDisplayProgrammingCommands(void);



#endif