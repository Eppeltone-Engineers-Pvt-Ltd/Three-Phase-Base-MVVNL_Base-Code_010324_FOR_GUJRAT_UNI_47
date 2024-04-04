#include "My_iodefine.h" 

#include <string.h>
#include "my_userdefine.h"
#include "my_macrodriver.h"
#include "fsl.h"
#include "fsl_types.h"
#include "Command.h"

volatile union {
	unsigned char uc[ 128 + 4 + 1 ];
	read_datum ur[ (128 + 4 + 1) / sizeof (read_datum) ];
} RxByteBuffer;
	
unsigned char XmodemDownloadAndProgramFlash (unsigned long FlashAddress);
extern unsigned char rx_data;
extern unsigned char PurgeComms ( unsigned long timeout );
extern unsigned char GetByte ( unsigned long timeout );
extern unsigned char GetDelayTimerStatus (void);
/***********************************************/
/*    XmodemDownloadAndProgramFlash function   */
/***********************************************/
unsigned char rxstatus,iverify_blk;
unsigned char ExpectedBlkNum;
unsigned char RetryCounter;
unsigned char RxByteCount;
unsigned char RxByteBufferIndex;
unsigned char Status;
unsigned char checksum;
unsigned char StartCondition;
unsigned long Address;
unsigned int flg1,flg=0,dly;
fsl_u08 my_fsl_status;
fsl_write_t my_fsl_write_str;
extern unsigned char counter ;
	 
unsigned char XmodemDownloadAndProgramFlash (unsigned long FlashAddress)
{
/*
XmodemDownloadAndProgramFlash() takes a memory address as the base address to
which data downloaded is programmed.  The data is downloaded using the XModem
protocol developed in 1982 by Ward Christensen.
The routine detects errors due to timeouts, comms errors or invalid checksums.
The routine reports the following to the caller:
-Success
-Invalid address
-Comms error
-Timeout error
-Failure to program flash


Expects:	
--------
FlashAddress:
32-bit address located in Flash memory space starting on a 32-byte boundary

Returns:
--------
XM_OK				-	Download and Flash programming performed ok
XM_ADDRESS_ERROR	-	Address was either not on a 128-bit boundary or not in valid Flash
XM_COMMS_ERROR		-	Comms parity, framing or overrun error
XM_TIMEOUT			-	Transmitter did not respond to this receiver
XM_PROG_FAIL		-	Falied to program one or more bytes of the Flash memory
*/

//	volatile union {
//		unsigned char uc[ 128 + 4 + 1 ];
//		read_datum ur[ (128 + 4 + 1) / sizeof (read_datum) ];
//	} RxByteBuffer; 
	// To ensure that data is stored starting on the correct size boundary for the Flash
	// an extra byte is needed as this buffer must store the xmodem protocol bytes
	// and the data bytes.	This means that a padding byte is added
	// at the beginning of the buffer.

	// first xmodem block number is 1
	ExpectedBlkNum = 1;
	
	StartCondition = 0;
	iverify_blk = 4;
	Address = FlashAddress;
	
	// if required, copy the RAM based program function to RAM
	// in this version the copy procedure has already been done in 'InitCommandHandler' in 'command.c'
	
	// flush the comms rx buffer with a delay of 1 sec
	// function will return when no data has been received for 1 sec

	rxstatus = PurgeComms( 10 );
	while(1)
	{
		NOP();
		RetryCounter = 10;
	
		rxstatus = TIMEOUT;
		while ( (RetryCounter > 0) && (rxstatus == TIMEOUT) )
		{
			if (StartCondition == 0)
			{
				//	if this is the start of the xmodem frame
				//	send a NAK to the transmitter
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();				
//				SendByte( (__near unsigned char)NAK );
				SendByte( (unsigned char)NAK );
				rxstatus = GetByte( 5);
			}							  
			else
			{
				rxstatus = GetByte( 5 );
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();
			}
			RetryCounter--;
		}
			
		StartCondition = 1;
		NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
		NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	
		//	if timed out after 10 attempts or comms error
		//	return relevant error state to caller
		if ( rxstatus == TIMEOUT )
		{
			NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
			NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
			return ( XM_TIMEOUT ); 
		}
		else if ( rxstatus == ERROR )
		{
			// loop back to (1)
			// do nothing
			NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
			NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
			return ( XM_COMMS_ERROR );
		}
		else			
		{
			// if first received byte is 'end of frame'
			// return ACK to sender and exit
			if ( rx_data == EOT )
			{
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				SendByte( (unsigned char)ACK );
				return( XM_OK );
			}
			else
			{				
				//	initialise counter for incoming Rx bytes
				// start of header + block num + (255 - block num) + 128 data bytes + checksum
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();				
				RxByteCount = 128 + 4;
				// RxByteBufferIndex is initiales to 1 to ensure correct boundary for the data
				RxByteBufferIndex = 1;
				rxstatus = XM_OK;
					
				// store the byte we have just received
				RxByteBuffer.uc[ RxByteBufferIndex++ ] = rx_data;
				RxByteCount--;
					
				while( RxByteCount > 0 )
				{
					//	get Rx byte with 1 second timeout
	        rxstatus= GetByte( 5 );

					//	if timed out or comms error
					if ( (rxstatus == TIMEOUT) || (rxstatus == ERROR) )
					{
						NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
						NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
						rxstatus = XM_TIMEOUT;
						//	timed out so purge incoming data
						// send NAK and return loop back start of while loop
						SendByte( (unsigned char)NAK );
						RxByteCount = 0;
					}
					else
					{
						// no timeout or comms error
						// store Rx byte
						NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
						NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
						RxByteBuffer.uc[ RxByteBufferIndex++ ] = rx_data;
						RxByteCount--;
					}
				}
					
				if (rxstatus == XM_TIMEOUT)
				{
						// loop back to (1)
						// do nothing
				}
				else
				{
					// data Rx ok
					// calculate the checksum of the data bytes only
					NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
					NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
					NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
					checksum = 0;
					for (RxByteBufferIndex=0; RxByteBufferIndex<128; RxByteBufferIndex++)
					{
						checksum += RxByteBuffer.uc[ RxByteBufferIndex + 3 + 1 ];
					}

					//	if SOH, BLK#, 255-BLK# or checksum not valid
					//	(BLK# is valid if the same as expected blk counter or is 1 less
					if ( !( (RxByteBuffer.uc[0 + 1] == SOH) && ((RxByteBuffer.uc[1 + 1] == ExpectedBlkNum) || (RxByteBuffer.uc[1 + 1] == ExpectedBlkNum - 1) ) && (RxByteBuffer.uc[2 + 1] + RxByteBuffer.uc[1 + 1] == 255 ) && (RxByteBuffer.uc[131 + 1] == checksum) ) )
					{
							//	send NAK and loop back to (1)
						NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
						NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
						SendByte( (unsigned char)NAK );
					}
					else
					{
					//	if blk# is expected blk num
						if ( RxByteBuffer.uc[1 + 1] == ExpectedBlkNum )
						{
							FSL_Open();    	
							FSL_PrepareFunctions();	
							WDTE=0xAC;
							my_fsl_write_str.fsl_data_buffer_p_u08 = (fsl_u08 *)&RxByteBuffer.uc[3 + 1];
							my_fsl_write_str.fsl_word_count_u08 = 0x20;
							my_fsl_write_str.fsl_destination_address_u32 = Address;

							my_fsl_status = FSL_Write((__near fsl_write_t*)&my_fsl_write_str);

							if(my_fsl_status != FSL_OK) 
							{
								MyErrorHandler();
							}
							WDTE=0xAC;
							if(my_fsl_status == FSL_OK) {
								Status = PROG_PASS;
								NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
								NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
							}else{
								NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
								NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
									
								MyErrorHandler();
								Status = PROG_FAIL;
							}
							//Status = PROG_PASS;
							if( Status == PROG_PASS )
							{

								Address += 128;
								ExpectedBlkNum++;
								SendByte( (unsigned char)ACK );
							}
							else
							{
								// prog fail
								SendByte( (unsigned char)NAK );
								// cancel xmodem download
								SendByte( (unsigned char)CAN );
									
								return( XM_PROG_FAIL );
							}
						}
						else
						{
						//	block number is valid but this data block has already been received
						//	send ACK and loop to (1)
							SendByte( (unsigned char)ACK );
						}
					}
				}
			}
		}
	}		
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();								
	NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();								

	return(XM_OK);		//	Processing doesn't come here. 
}
