#include "my_macrodriver.h"
#include "my_cgc.h"
#include "string.h"
#include "fsl.h"
#include "fsl_types.h"
#include "my_uartf.h"
#include "My_cg_timer.h"
#include "command.h"
#include "MY_ram_vector.h"

#include "my_userdefine.h"

unsigned int i;
_Bool rx_flag;
_Bool tx_flag;
unsigned char rec_val;
unsigned char intro_str[]="RL78/G14 UART (19.2 Kbps) BOOT LOADER Programming...";
unsigned char tmpr;
unsigned char rx_data;	
unsigned long tick_count;
unsigned char err_flag;
unsigned char status;
extern _Bool DelayTimerUnderFlowFlag;
  
unsigned char Data_Clear[9] = {27, 91, 50, 74, 0, 27, 91, 72, 0}; 
unsigned char my_data[8] = "MY F S L";
unsigned char ReadByte(void);
void SendByte(unsigned char b);
unsigned char RxByteWaiting(void);
unsigned char TxByteEmpty (void);
unsigned char GetByte (unsigned long);
unsigned char GetDelayTimerStatus (void);
unsigned char PurgeComms (unsigned long);
void SendString ( unsigned  char *str );
void lvd_isr(void);
void HOCOFC_InterruptISR(void);

extern unsigned char RunPreCheck(void);

void main(void)
{
	fsl_descriptor_t  Flash_Init_Info;	
	fsl_u08 my_fsl_status;
	unsigned char us_c;
	
	R_SAU1B_Create();

	DI();
	RAM_INTTM00_ISR = &r_tau0_channel0_interrupt;
	{// if these brackets are not used then the char x=200; generates an error
		char x=200;
		OSMC = 0x80;
		CMC = 0x10;	//32768 Hz Connected, X1 as I/O, LMP Oscillation
		CSC = 0x80; //On Chip Oscillator ON, 32768 Hz On HS Crystal OFF
		while(x--); //CPU use Chip Oscillator as fmain  
		HOCODIV = 0x02;// for 6 MHz on Chip Main Clock. 
	//		HOCODIV = 0x01;// for 24 MHz on Chip Main Clock. 
		CSS = 0x00;
		HIOSTOP = 0x00;
	}

   //FSL Initilization				
	Flash_Init_Info.fsl_flash_voltage_u08 	= 0x00; //Full speed-mode
	Flash_Init_Info.fsl_frequency_u08 		= 24;	//24MHz
	Flash_Init_Info.fsl_auto_status_check_u08	= 0x01;	//internal mode	

	my_fsl_status=FSL_Init((const __far fsl_descriptor_t*)&Flash_Init_Info);

	if(my_fsl_status != FSL_OK) 
		MyErrorHandler();
	
	PFSEG4=0;
  PFSEG5=0;
  P5 = 0xC0;
  P8 = 0x20;
	PM5 = 0x31;
	PM8 = 0x10;
	
	R_UART3B_Start();
	
	us_c=RunPreCheck();
	if(0==us_c)	
	{// success
		RunCommandHandler(); 
	}
	else
	{// password was incorrect
		Command_5();		// run target application
	}
	
  while(1)
  {
    WDTE =0xACU;
    SendByte(ReadByte());
  }
}

void MyErrorHandler(void)
{
	WDTE=0xAC;
	NOP();
}

unsigned char ReadByte(void)
{      
	while(SRIF3 == 0);			//Wait until a byte is received
	SRIF3 = 0;
	WDTE=0xAC;
	return (RXD3);
}

void SendByte (__near unsigned char b )
{      
	SRIF3 = 0;
	TXD3	= b;
	while(STIF3 == 0);			//Wait until buffer becomes empty
	STIF3 = 0;	
	WDTE=0xAC;
}

unsigned char RxByteWaiting(void)
{
	if(TMIF00 == 1){
		DelayTimerUnderFlowFlag = 1;
		R_TAU0_Channel0_Stop();
	}
	if(SRIF3 == 1){
		tmpr = 1;
		NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	}else {
		tmpr = 0;	
		NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
	}
	
  NOP();
	WDTE=0xAC;
	return(tmpr);
}

unsigned char TxByteEmpty ( void )
{
	unsigned char tmp;	
	//WDTE=0xAC;
	if( STIF3 == 0 )
	{
		tmp = 0;
	}
	else
	{
		tmp = 1;
	}
	return tmp;
}

unsigned char GetByte ( unsigned long timeout )
{
	// the CPU clock selected is 6 MHz
	// The clock to the timer unit is 600000/128 = 46875
	// The timer count register is 46874 - hence it will take 1 second to generate an interrupt
	// When the timer is started the variable DelayTimerUnderFlowFlag is cleared to 0
	// When the tiemr interrupt comes the variable DelayTimerUnderFlowFlag is set to 1
	// The function GetDelayTimerStatus returns the value of the DelayTimerUnderFlowFlag 
	unsigned long timeout1;
	timeout1 = timeout;
	WDTE=0xAC;
//	tick_count = (__near unsigned long) timeout1 / MS_PER_TIMER_COMPARE_MATCH;
	tick_count = timeout1;
	SRIF3 = 0;
	R_TAU0_Create();
	R_TAU0_Channel0_Start();
	EI();
	// wait for a byte to arrive
	while ( ( RxByteWaiting() == 0 ) && (tick_count) )
	{
		if ( GetDelayTimerStatus() )
		{
			R_TAU0_Channel0_Stop();
			DI();
			if ( --tick_count )
			{
				EI();
				R_TAU0_Channel0_Start();
			}
		}
	}
	
	if ( RxByteWaiting() == 1 )
	{
	// read error flags and data
		volatile unsigned char err_type;
		err_flag = SSR13L;
    
		// check for errors
		// Rx error		
		if((err_flag>=1)&&(err_flag<=7)){	//Error
			rx_data = RXD3;
			err_type = (uint8_t)(SSR13 & 0x0007U);
			SIR13 = (uint16_t)err_type;
			status = ERROR;
		}
		else
		{
			// no Rx error
			rx_data = RXD3;
			status = OK;
		}		
	}
	else
	{
		status = TIMEOUT;
	}
	//WDTE=0xAC;
	return (status);
}

unsigned char GetDelayTimerStatus (void)
{
	WDTE=0xAC;
	if ( DelayTimerUnderFlowFlag == 1 )
	{
		__nop();
		return (1);
	}			   
	else
	{
		NOP();
		return (0);
	}
}

unsigned char PurgeComms(unsigned long timeout)
{
	unsigned char err_flag;
	WDTE=0xAC;
	do {
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
//				err_flag = GetByte((__near unsigned long)5);;
				err_flag = GetByte((unsigned long)5);;
		return(err_flag);
	} while( (err_flag != TIMEOUT) && (err_flag != ERROR) );
	//WDTE=0xAC;
}

void SendLFCR(void)
{
	WDTE=0xAC;
	SendByte(LINEFEED);
	SendByte(CARRIAGE_RETURN);
}

unsigned char us,us1;
void SendString( unsigned char *str)
{
	
	WDTE=0xAC;
	us = 0;
	while ( str[us] != 0 )
//	while ( us<  10 )
	{
		// Display_LCD(LCD_LINE1, (int8_t *)" Async   ");
		us1=str[us];
		//SendByte (str[us] );
//		SendByte (  ( __near unsigned char )us1 );
		SendByte (us1);
		us++;
		WDTE=0xAC;
	}
}

void lvd_isr(void)
{
	NOP();
}

void HOCOFC_InterruptISR(void)
{
  NOP();NOP();	
//	HOCOFC = 0x01;
	NOP();NOP();		
}

