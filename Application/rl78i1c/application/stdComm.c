#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// #include <time.h> // this function does not exist in renesas

#include "r_cg_macrodriver.h"
#include "r_cg_rtc.h"
#include "r_cg_wdt.h"

#include "wrp_user_uart.h"
#include "em_errcode.h"
#include "em_operation.h"
#include "service.h"

#include "r_dlms_user_interface.h"		// this is for secret backup and restore function test
#include "em_display.h"
#include "r_lcd_display.h"
#include "wrp_mcu.h"
#include "emeter3_structs.h"
#include "pravakComm.h"
#include "memoryOps.h"
#include "e2rom.h"	// pravak implementation
#include "utilities.h"
#include "tampers.h"
#include "survey.h"
#include "emeter3_app.h"
#include "powermgmt.h"
#include "iHexParser.h"

#define IGNORECHECKSUM

#define CR	(0x0dU)
#define LF	(0x0aU)
#define ACRRCDF				(0x01U)

#define AMSGBSIZE (64U)

uint8_t amsgbuf[64];
static uint8_t UartSendbuff[266];	// this has to be 266 - nothing else works

uint8_t amsgbctr;
uint8_t arbtail;

uint8_t comflags;
uint8_t checksum;
uint8_t gCalFlag;

volatile uint8_t gWhichPort;
volatile uint8_t tx_in_progress;

void doReadState(void);
void send_done_msg(void);
void send_error_msg(void);


void PVK_Comm_Init(void)
{
    WRP_UART2_Start();
    WRP_UART3_Start();

// when starting this fellow ought to be zero and default port to be 3
	tx_in_progress=0;	
	gWhichPort=3;	
}

void PVK_Comm_Start(void)
{

}

//--------------------- rs232 routines -----------------------
// Receive
void PVK_ReceiveByte(uint8_t g_received_byte, uint8_t whichPort)
{// whenever a byte is received on any port, this function will get called by wrp_user_uart.WRP_UART_ReceiveEndCallback
	gWhichPort = whichPort;
	amsgbuf[amsgbctr]=g_received_byte;
  amsgbctr++;
  
  if(amsgbctr==AMSGBSIZE)
    amsgbctr=0;
}

void agetmsg(void)
{
	uint8_t amsgbyte;
	uint8_t temp;
	
	if(amsgbctr==arbtail)
		return;
  
  amsgbyte=amsgbuf[arbtail];
  arbtail++;
  if(arbtail==AMSGBSIZE)
		arbtail=0;
 
  if(amsgbyte==LF)
  {
		temp=comflags&ACRRCDF;
		if(temp!=0)
		{
			comflags&=~ACRRCDF;
	    if(amsgbctr!=0)
	    {
				amsgbuf[amsgbctr-2]=0x00;
				aanalmsg();
				amsgbctr=0;
				arbtail=0;
	    }
		}
		else
		{
			amsgbctr=0;
			arbtail=0;
			comflags&=~ACRRCDF;
		}
  }
  else
  {
		temp=comflags&ACRRCDF;
		if(temp!=0)
		{
			amsgbctr=0;
			arbtail=0;
			comflags&=~ACRRCDF;
		}
		else
		{
			if(amsgbyte==CR)//msgbyte== <cr>
				comflags|=ACRRCDF;
		}
  }
}

void PVK_TX_Complete(void)
{
	tx_in_progress=0;
}

uint8_t asendHexMultiple(uint8_t* e2buf, uint16_t nBytes)
{
	while(tx_in_progress!=0);	// wait till previous transmission is not completed
	tx_in_progress=1;
	
    memcpy(UartSendbuff, e2buf, nBytes);     // copy to the other buffer for avoid conflicting between the last and current sending

	if(gWhichPort==2)
		WRP_UART2_SendData(UartSendbuff, nBytes);
	else if	(gWhichPort==3)
		WRP_UART3_SendData(UartSendbuff, nBytes);
	
	return 0;
}

// avoid this function - use onlyin development for testing
/*
uint8_t asendHexBufferInAscii(uint8_t* e2buf, uint16_t nBytes)
{
	uint8_t i;
	uint8_t dighi;
	uint8_t diglo;
	
	while(tx_in_progress!=0);	// wait till previous transmission is not completed
	tx_in_progress=1;
	
	for(i=0;i<nBytes;i++)
	{
		byt2digs(e2buf[i],&dighi,&diglo);
		UartSendbuff[i*3]=dighi;
		UartSendbuff[i*3+1]=diglo;
		UartSendbuff[i*3+2]=' ';
	}
	
	if(gWhichPort==2)
		WRP_UART2_SendData(UartSendbuff, nBytes);
	else if	(gWhichPort==3)
		WRP_UART3_SendData(UartSendbuff, nBytes);

	return 0;
}
*/


void asendbare(uint8_t* e2buf)
//sends null terminated string in a[]
// does NOT append <CR><LF>
{
	uint8_t nBytes = (uint8_t)strlen((char *)e2buf);
	
	      R_WDT_Restart();

	while(tx_in_progress!=0);	// wait till previous transmission is not completed
	tx_in_progress=1;

  memcpy(UartSendbuff, e2buf, nBytes);     // copy to the other buffer for avoid conflicting between the last and current sending

	if(gWhichPort==2)
		WRP_UART2_SendData(UartSendbuff, nBytes);
	else if	(gWhichPort==3)
		WRP_UART3_SendData(UartSendbuff, nBytes);


}

void asendcrlf(void)
{
	asendbare((uint8_t *)"\r\n");
}

char calcChksum(char buf[])
{
  char i;
  char checkchksum=0;
  i=0;
  while(buf[i] !='\0')
  {
    checkchksum += buf[i];
    i++;
  }
  return checkchksum;
}

void sendERROR(void)
{
	asendbare((uint8_t *)"ERROR");
	asendcrlf();
}

void sendok(void)
{
	asendbare((uint8_t *)"OK");
	asendcrlf();
}

void sendok2(void)
{
	asendbare((uint8_t *)"OK2");
	asendcrlf();
}


void send_done_msg(void)
{
	asendbare((uint8_t *)"Done\r\n");
}

void send_error_msg(void)
{
	asendbare((uint8_t *)"Error\r\n");
}

void aanalmsg(void)
{
	uint8_t *ptr;
	uint32_t temp_ptr;
	uint8_t mfgcode[4];
	uint8_t tbuf[8];
	uint8_t buf[64];
	uint8_t i;
	uint8_t indx;
	uint8_t currchar;
	char tempChar;
	uint8_t retval;
	
	mfgcode[0]='E';
	mfgcode[1]='E';
	mfgcode[2]='O';

	if(nowcheck(&amsgbuf[0],(uint8_t *)"OK")==1)
	{
		sendok2();
		return;
	}

	indx=0;
	currchar=amsgbuf[indx];

	while((currchar!=mfgcode[0]) && (indx < AMSGBSIZE))
	{// if it does not receive the first character it will get stuck here.
		indx++;
		currchar=amsgbuf[indx];
	}

	if(currchar!=mfgcode[0])
		return;
	
	indx++;
	if (amsgbuf[indx]!=mfgcode[1])
		return;

	indx++;
	if (amsgbuf[indx]!=mfgcode[2])
		return;

	indx++;	//indx points to the first byte of the actual message

// -----------------------------------------------------
// In the following area the commands do not use the checksum - hence indx now points to the command itself
	if(nowcheck(&amsgbuf[indx],(uint8_t *)"VER")==1)
	{
		buf[0] = (BOARD_ID/10)+0x30;
		buf[1] = (BOARD_ID%10) + 0x30;;
		
		buf[2] = (VERNO/100) + 0x30;
		buf[3] = ((VERNO - ((VERNO/100)*100))/10) + 0x30;
		buf[4] = (VERNO%10)+0x30;
		
		#ifdef BIDIR_METER
			buf[5] =	'B';
		#else
			buf[5] =	'U';
		#endif
		
		buf[6]=METER_CLASS + 0x30;
		buf[7]=0x00;
		asendbare((uint8_t *)buf);    
		asendcrlf();
		return;
	}

  if(nowcheck((uint8_t*)&amsgbuf[indx+2],(uint8_t *)"METER")==1)
  {// MSC00METERaaaannnnnnnn	// alphanumeric
    sendok();
		strcpy((char*)buf,(char *)&amsgbuf[indx+2+5]);
    return;
  }

  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"RST")==1)
  {// MSCRST - EEORST	// reset the meter  and enter bootloader
		while(1); // force a reset
    return;
  }
}

uint8_t nowcheck(uint8_t* mbuf, uint8_t* conststr)
{
	uint8_t resultf=1;
	uint8_t constbyte;
	uint8_t msgbyte;
	uint8_t i=0;

	constbyte=conststr[i];
	msgbyte=mbuf[i];

	while(constbyte !=0x00 && resultf==1)
	{
		if(constbyte!=msgbyte)
			resultf=0;
		else
		{
			i++;
			constbyte=conststr[i];
			msgbyte=mbuf[i];
		}
	}
	return resultf;
}
