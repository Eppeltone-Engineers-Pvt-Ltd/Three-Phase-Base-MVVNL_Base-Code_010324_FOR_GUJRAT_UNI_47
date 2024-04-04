#include "my_macrodriver.h"
#include "My_iodefine.h" 

#include "command.h"

#include "my_userdefine.h"
#include "fsl.h"
#include "fsl_types.h"
#include "my_uartf.h"							

extern unsigned char rx_data;	
extern unsigned char GetByte ( unsigned long timeout );
extern unsigned char GetDelayTimerStatus (void);
extern _Bool DelayTimerUnderFlowFlag;
void Blank_Check_Error(unsigned char );
void RunCommandHandler (void);
extern unsigned char XmodemDownloadAndProgramFlash (unsigned long FlashAddress);

void R_TAU0_Create(void);
void R_TAU0_Channel0_Start(void);
void R_TAU0_Channel0_Stop(void);

void decryptEED(unsigned char* bufPtr,unsigned char bufLength);


unsigned char first_user_flash_address[]="0002000";
unsigned char str_tx[35]="Waiting for byte, Timeout is 5 sec";
unsigned char str1[]="RL78/I1B Bootloader Main Menu v1.0";
unsigned char str2[]="----------------------------------";
unsigned char str3[]="1......Blank Check User Area";
unsigned char str4[]="2......Erase User Area";
//unsigned char str5[]="3......Sending Hex file";
unsigned char str6[]="3......Program Flash via XModem Download";
unsigned char str7[]="5......Start User Program MMG FILE";
unsigned char str8[]=">";
	
unsigned char c1_str1[]="Blank checking user area...";
unsigned char c1_str2[]="Block: ";
unsigned char c1_str3[]=": NOT blank";
unsigned char c1_str4[]=": BLANK";

unsigned char c2_str1[]="Really erase ALL user blocks (Y/N)?";
unsigned char c2_str2[]="Timed out waiting for response";
unsigned char c2_str3[]="Comms error";
unsigned char c2_str4[]="All blocks erase cancelled";
unsigned char c2_str5[]="Erasing block ";
unsigned char c2_str6[]=" >> ";
unsigned char c2_str7[]=": Erasing success!!";
unsigned char c2_str8[]="Erasing of block: ";
unsigned char c2_str9[]=": FAILED";
unsigned char c2_str10[]="All user blocks erased";

unsigned char c3_str1[]="Enter block no to be erased as 2-digit number e.g. 08";
unsigned char c3_str2[]=">D'";
unsigned char c3_str3[]="Timed out waiting for block number...";
unsigned char c3_str4[]="Invalid block number";
unsigned char c3_str5[]="Erase block (Y/N)?";
unsigned char c3_str6[]="Timed out waiting for confirmation";
unsigned char c3_str7[]="Erasing of block ";
unsigned char c3_str8[]="PASSED ";
unsigned char c3_str9[]=" FAILED ";
unsigned char c3_str10[]="Block erase cancelled";

unsigned char c4_str1[]="Download to address 0x";
unsigned char c4_str2[]=" (Y/N)?";
unsigned char c4_str3[]="Timed out waiting for confirmation...";
unsigned char c4_str4[]="Program Flash (Y/N)?";
unsigned char c4_str5[]="Timed out waiting for response";
unsigned char c4_str6[]="Comms error";
unsigned char c4_str7[]="Flash programming cancelled";
unsigned char c4_str8[]="Start XModem download...";
unsigned char c4_str9[]="Xmodem Executing...";
unsigned char c4_str10[]="Xmodem Execution end!!!";
unsigned char c4_str11[]="Timeout";
unsigned char c4_str12[]="Download OK";
unsigned char c4_str13[]="Flash program FAIL";
unsigned char c4_str14[]="NOT WORKING";

unsigned char c5_str1[]="Run user program (Y/N)?";
unsigned char c5_str2[]="Timed out waiting for response";
unsigned char c5_str3[]="Comms error";
unsigned char c5_str4[]="Run user program request cancelled";
unsigned char c5_str5[]="Running user program...";
unsigned char c5_str6[]="Invalid user reset address.  Staying put.";
//pt2FunctionErase fp;
void JumpToApplication(void);
unsigned char RunPreCheck(void);

unsigned char rxBuffer[32];
unsigned char rxCount = 0;


void decryptEED(unsigned char* bufPtr,unsigned char bufLength)
{// bufLength number of bytes have been received in bufPtr 
// Each byte in buffer, is xored with firstkey, complemented and stored back in the buffer
// After each operation the firstkey is incremented
	unsigned char i;
	unsigned char firstkey = 0x55;
	unsigned char temp;
	
	for(i=0;i<bufLength;i++)
	{
		temp=bufPtr[i];
		temp=temp^firstkey;
		bufPtr[i]=~temp;
		firstkey++;
	}

}

unsigned char RunPreCheck(void)
{
	unsigned char retval;	// 0 - success
	unsigned char tick_count;
	unsigned char done;
	unsigned char byteRecd;
	unsigned char msgHeaderRecd;
	
	rxBuffer[0]=0;
	rxBuffer[1]=0;
	rxBuffer[2]=0;
	rxBuffer[3]=0;
	rxBuffer[4]=0;
	rxBuffer[5]=0;
	rxBuffer[6]=0;
	rxBuffer[7]=0;
	
	// send an initial string 
	SendString(str8);
	SendString(str8);
	SendString(str8);
	
	// we should collect as many bytes received in 1 second
	
	
	WDTE=0xAC;
	tick_count = 2;	// 2 seconds
	SRIF3 = 0;
	R_TAU0_Create();
	R_TAU0_Channel0_Start();
	EI();
	done=0;
	msgHeaderRecd=0;
	
	// wait for timeout to be completed or at least six bytes to be received
	do
	{
		if(0==GetDelayTimerStatus())
		{	// time is not over yet
			if(SRIF3 == 1)
			{// a byte has been received - could be an error too
				byteRecd = RXD3;
				SRIF3 = 0;

				if(msgHeaderRecd==0)
				{//header byte has not been received yet
					if(byteRecd==':')
					{
						msgHeaderRecd=1;
						rxCount=0;
					}
				}
				else
				{// here ':' has been received
					rxBuffer[rxCount++] = byteRecd;
					if(rxCount==5)
						done=1;
				}
			}
			NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
		}
		else
		{// here one timer interrupt has been received
			R_TAU0_Channel0_Stop();
			DI();
			tick_count--;
			if(0!=tick_count)
			{
				R_TAU0_Channel0_Start();
				EI();
			}
			else
				done=1;
		}
	}
	while(done==0);
	
	if(rxCount>=5)
	{// we have received the password which is encrypted
		decryptEED((unsigned char *)&rxBuffer,5);
		if((rxBuffer[0]=='E')&&(rxBuffer[1]=='E')&&(rxBuffer[2]=='O')&&(rxBuffer[3]=='B')&&(rxBuffer[4]=='L'))
		{// password is correct
			retval=0;	// success
		}
		else
			retval=1;	// failure
	}
	else
	{// we have not received the full password within the specified time limit
		retval=1;	// failure
	}
	return retval;
}







//
// functionality of this handler will be dependent on the communication hardware interface
//
/**************************************/
/*    RunCommandHandler function      */
/**************************************/

void RunCommandHandler (void)
{
	unsigned char rx_status;
	
	here:	
	{
		SendString(str_tx);
		WDTE=0xAC;
		rx_status = GetByte (5);
	}
	
	if ( rx_status == TIMEOUT )
	{
		WDTE=0xAC;	
		JumpToApplication();
		WDTE=0xAC;
		
	}
	else if(rx_data=='H')//if H recd , enter bootloader code
	{		
 		ShowMenu();			
		while (1)
		{
			WDTE=0xAC;
			rx_status = GetByte (60);
			WDTE=0xAC;
			
			if ( rx_status == OK )
			{
				switch ( rx_data )
				{
					case '1':
						__DI();
						Command_1();		// blank check
						__EI();
						break;
					
					case '2':
						__DI();
						Command_2();		// erase all user flash blocks
						__EI();
						break;
					
					case '3':
						Command_4();		// xmodem download to flash memory
						break;

					case '5':
						Command_5();		// run target application
						break;

					default:
						rx_data=0;
						ShowMenu();
						break;
				}
			}
			else 
			{
				if ( rx_status == TIMEOUT )
				{
					WDTE=0xAC;
					JumpToApplication();
				}
			}
		}
	}
	else
		goto here;
}

/**************************************/
/*    ShowMenu function               */
/**************************************/
void ShowMenu (void)
{
	WDTE=0xAC;
	SendLFCR();
	SendLFCR();
	SendString(str1);//"RL78G1x Bootloader Main Menu v1.0" );
	SendLFCR();
	SendString(str2);// "----------------------------------"  );
	SendLFCR();
	SendLFCR();
	SendString(str3);// "1......Blank Check User Area" );
	SendLFCR();
	SendLFCR();
	SendString(str4);// "2......Erase User Area" );
	SendLFCR();
	SendLFCR();
	SendString(str6);// "4......Program Flash via XModem Download" );
	SendLFCR();
	SendLFCR();
	SendString(str7);// "5......Start User Program" );
	SendLFCR();
	SendLFCR();
//	SendString(str8);// '>' );// deliberately commented since we are using >>> as a prompt in RunPreCheck
}

/**************************************/
/*    Command_1 function              */
/**************************************/
unsigned char uc,tmp1,tmp2;
unsigned int blk;
void Command_1 (void)
{
	// blank check the user flash area
	// the user flash area is specified as the range of addresses between
	// 'FIRST_USER_FLASH_ADDR' and 'LAST_USER_FLASH_ADDR' inclusive
	// these values are specified in 'command.h'
	
//	read_datum *address;
//	unsigned char tmp;
	fsl_u08 my_fsl_status;
	WDTE=0xAC;
	SendLFCR();
	SendString(c1_str1);// "Blank checking user area..." );
	
	FSL_Open();    	
	FSL_PrepareFunctions();	
	uc=0;
	for(blk=Start_Block;blk<=No_Of_Blocks;blk++){
		uc=blk;
		SendLFCR();
		SendString(c1_str2);// "Block: " );
		if(uc < 9){
			SendByte(uc+48);
		}else{
			tmp1 = uc / 100;
			SendByte(tmp1+48);
			tmp1 = uc % 100;
			
			tmp1 = tmp1 / 10;
			SendByte(tmp1+48);
			tmp1 = uc % 10;
			SendByte(tmp1+48);
		}
		my_fsl_status = FSL_BlankCheck(uc);			//Blank Check if Device is not blank issue Erase Command
		if(my_fsl_status != FSL_OK){
			SendString(c1_str3);// ": NOT blank" ); 
			//Blank_Check_Error(blk);
			MyErrorHandler();
		}else{
//[CcnvCA78K0R] 			NOP();	
			__nop();	
			SendString(c1_str4);// ": BLANK" );
		}
	}
	
	SendLFCR();	
	FSL_Close();
	WDTE=0xAC;
}

/**************************************/
/*    Command_2 function              */
/**************************************/
void Command_2 (void)
{
	// erase all user flash blocks
	// the first flash block to be erased is 'FIRST_USER_FLASH_BLK' and the last
	// flash block to be erased is 'NO_OF_FLASH_BLOCKS' - 1
	// these values are defined in 'command.h' and 'flash_header.h'	
	
	unsigned char tmp1;
	fsl_u08 my_fsl_status;
	
	WDTE=0xAC;
	SendLFCR();
	FSL_Open();    	
	FSL_PrepareFunctions();	
	uc=0;
	for(blk=Start_Block;blk<=No_Of_Blocks;blk++){
		uc=blk;
		SendLFCR();
		SendString(c2_str5);// "Erasing block " );
		if(uc < 9){
			SendByte(uc+48);
		}else{
			tmp1 = uc / 100;
			SendByte(tmp1+48);
			tmp1 = uc % 100;
			
			tmp1 = tmp1 / 10;
			SendByte(tmp1+48);
			tmp1 = uc % 10;
			SendByte(tmp1+48);
			
		}		
		SendString (c2_str6);// " >> " );
		// call the erase function with the block to be erased and a dummy value (0)
		if(uc==No_Of_Blocks)
		{
		  NOP();NOP();NOP();NOP();NOP();NOP();
		  NOP();NOP();NOP();NOP();NOP();NOP();
		  NOP();NOP();NOP();NOP();NOP();NOP();
		}
		WDTE=0xAC;
		my_fsl_status = FSL_Erase(uc);
		WDTE=0xAC;
		while(my_fsl_status == FSL_BUSY)
		{
			WDTE=0xAC;
			my_fsl_status = FSL_StatusCheck();
		}
				
		WDTE=0xAC;
		if(my_fsl_status == FSL_OK){
			SendString(c2_str7);//": Erasing success!!");
		}else if(my_fsl_status != FSL_OK){	
			SendString(c2_str8);// "Erasing of block: " );
			SendString(c2_str9);// ": FAILED" );
			SendLFCR();
			MyErrorHandler();
			return;
		}
	}
	WDTE=0xAC;
	FSL_Close();
	WDTE=0xAC;
	SendLFCR();
	SendString(c2_str10);// "All user blocks erased" );
	WDTE=0xAC;
  NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();
  NOP();NOP();NOP();NOP();NOP();NOP();
}


/**************************************/
/*    Command_4 function              */
/**************************************/
void Command_4 (void)
{
    // download data using xmodem and program into flash
//  unsigned char Status, Shift;
  unsigned char Status;
	volatile unsigned long Address, ul;
//	unsigned char rxstatus;
	// suggest download address as the first user flash address as usually this will be the case
	SendLFCR();
	SendString(c4_str1);// "Download to address 0x" );
	SendString(first_user_flash_address);// FIRST_USER_FL_ADDR_TXT );
	SendLFCR();
	SendString(c4_str8);// "Start XModem download..." );
	SendLFCR();
	Address = FIRST_USER_FLASH_ADDR;
	SendString(c4_str9);// "Xmodem Executing..." );
	// start xmodem download and program the flash with the downloaded data
	Status = XmodemDownloadAndProgramFlash( Address );
	SendLFCR();
	SendString(c4_str10);// "Xmodem Execution end!!!" );	
	if ( Status == XM_TIMEOUT )
		SendString(c4_str11);// "Timeout" );
	else if ( Status == XM_OK )
	{
		SendString(c4_str12);// "Download OK" );
	}
	else if ( Status == XM_PROG_FAIL )
		SendString(c4_str13);// "Flash program FAIL" );
	else SendString(c4_str14);//"NOT WORKING");
	
	if(Status != XM_OK)// Partially written , erase the code 
	{	
		__DI();	
		Command_2();
		WDTE=0xAC;
		__EI();
	}
	
	SendLFCR();
	FSL_Close();
}

/**************************************/
/*    Command_5 function              */
/**************************************/
void Command_5 (void)
{
//	unsigned char rxstatus;
	__far unsigned int *ptr;
	pt2FunctionErase fp;
	ptr = (__far unsigned int *)( FIRST_USER_FLASH_ADDR );

	if ( *ptr != 0xffff )
	{
		SendString(c5_str5);// "Running user program..." );
		__DI();

		R_UART3B_Stop();
    SAU1EN = 0U;    // stop supply SAU1 clock

		R_TAU0_Channel0_Stop();		
    TAU0EN = 0U;    /* enables input clock supply */
		
		fp = (pt2FunctionErase)ptr;
		fp( 0, 0);
	}
	else
	{
		// invalid user reset address
		SendString(c5_str6);// "Invalid user reset address.  Staying put." );
		return;
	}
}

void JumpToApplication(void)
{
//	unsigned char rxstatus;
	__far unsigned int *ptr;
	pt2FunctionErase fp;
	ptr = (__far unsigned int *)( FIRST_USER_FLASH_ADDR );
	if ( *ptr != 0xffff )
	{
		SendString(c5_str5);// "Running user program..." );
		__DI();

		R_UART3B_Stop();
    SAU1EN = 0U;    // stop supply SAU1 clock

		R_TAU0_Channel0_Stop();		
    TAU0EN = 0U;    /* enables input clock supply */
 
		fp = (pt2FunctionErase) ptr;
		fp( 0, 0);
	}
	else
	{
		RunCommandHandler();
	}	
}