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
//#include "r_calib.h"
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

#define PROD_KWH_VALUE	2000

#define IGNORECHECKSUM

#define CR	(0x0dU)
#define LF	(0x0aU)

#define ACRRCDF				(0x01U)
#define ALL_IS_WELL_FLAG 	(0x02U)

#define AMSGBSIZE (64U)

//extern int32_t testCtr;


extern Unsigned16	g_Class07_Event_CurrentEntry[];
extern Unsigned32 g_Class07_Event_EntriesInUse[];

uint8_t amsgbuf[64];
static uint8_t UartSendbuff[266];	// this has to be 266 - nothing else works

uint8_t amsgbctr;
uint8_t arbtail;

uint8_t comflags;
uint8_t checksum;
uint8_t gCalFlag;

uint8_t calswitch;

volatile uint8_t gWhichPort;	// global variable
volatile uint8_t tx_in_progress;

void doReadState(void);
uint8_t doAutoCal(uint8_t line);
void send_done_msg(void);
void send_error_msg(void);
void restoreMeterDisplay(void);
void performModeChangeActions(uint8_t i);
extern void EM_SetVoltageThreshold(uint16_t vlow);


extern uint32_t g_Class07_Blockload_MaxEntries;



void PVK_Comm_Init(void)
{// we only need to let the receive uart2 interrupt function know the address of amsgbuf and the amsgbsize

// PRAVAK Comm will use only the OPTICAL PORT at present at 4800 baud
//#ifdef OPTICAL_PORT
//    WRP_UART2_Init();	// waste of time - does nothing
    WRP_UART2_Start();
//#else
//    WRP_UART3_Init();	// waste of time - does nothing
    WRP_UART3_Start();
//#endif	

// when starting this fellow ought to be zero and default port to be 3 - added on 27/05/2019
	tx_in_progress=0;	
	gWhichPort=3;	
}

void PVK_Comm_Start(void)
{

}

//--------------------- rs232 routines -----------------------
// Receive
void PVK_ReceiveByte(uint8_t g_received_byte, uint8_t whichPort)
{// whenever a byte is received this function will get called by wrp_user_uart.WRP_UART_ReceiveEndCallback
//  powermgmt_mode_t power_mode;
	
	gWhichPort = whichPort;
	amsgbuf[amsgbctr]=g_received_byte;
  amsgbctr++;
  
  if(amsgbctr==AMSGBSIZE)
    amsgbctr=0;
	
	sleepCommModeTimer=0;
//	comm_mode=1;
//	commModeTimer=0;
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
				temp=comflags&ALL_IS_WELL_FLAG;
				if(temp==0)
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
	sleepCommModeTimer=0;	// added on 17/Jan/2023
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

//------------------------------------------------------------



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

void doReadState(void)
{
	uint8_t status;
	uint32_t eeprom_addr;
	uint16_t statesize, copied_size;    
	uint8_t  page_buffer[16];

//	eeprom_addr = STORAGE_EEPROM_SYSTEM_STATE_ADDR; /* Point to EEPROM System State Location */
	copied_size = 0;
	statesize   = 0;

	do
	{
	    status = EM_GetState(page_buffer, 16, &statesize);
	    
	    if (status == EM_OK)
	    {
			asendHexMultiple(page_buffer, statesize-copied_size);
	        copied_size = statesize;    /* Backup all */
	    }
	    else if (status == EM_COPYING)
	    {
			asendHexMultiple(page_buffer, 16);

	        eeprom_addr += 16;
	        copied_size += 16;
	    }
	}
	while (status == EM_COPYING && copied_size < 613);

}

typedef struct pvkCalCmdData
{
	float32_t volt;		// 0
	float32_t current;	// 4
	float32_t pf;		// 8
	float32_t ext_temp;	// 12
	float32_t frequency;// 16
	float32_t imax;		// 20
	float32_t dummy;	// 24
	uint8_t pw[24];		// password string
}pvkCalCmdData_t;


uint8_t doAutoCal(uint8_t line)
{
// line will be 0(R),1(Y),2(B),3(Neutral)	
	
    st_service_param service_params;
	
	pvkCalCmdData_t CalCmdData;
	
	uint8_t id_list[4];
	
	char *ptr;
	
	char tbuf[96];
	char smallbuf[16];
	
	
	uint8_t length_in;
	uint8_t length_out;
	uint8_t total_block;
	
	
// initialize the data
	switch(line)
	{
		case 0:
			id_list[0]=0x21;	// step id for r phase
			break;
		case 1:
			id_list[0]=0x22;	// step id for y phase
			break;
		case 2:
			id_list[0]=0x23;	// step id for b phase
			break;
		case 3:
			id_list[0]=0x24;	// step id for neutral
			break;
		case 4:
			id_list[0]=0x02;	// step id for save - in this case first byte of input buffer must be 1
			break;
	}
	
	
	CalCmdData.volt = 240.0;
	CalCmdData.current = 10.0;
	CalCmdData.pf = 1.0;
	CalCmdData.ext_temp = 25.0;
	CalCmdData.frequency = 50.0;
	
//	CalCmdData.imax = 60.0;
	CalCmdData.imax = CAL_IMAX_CURRENT;	
	
//	CalCmdData.imax = 90.0;
	CalCmdData.dummy = 0.0;
	sprintf((char*)&CalCmdData.pw,"%s","SPEMCALIB");
	
	
	service_params.p_id_list = (uint8_t*)&id_list;
    service_params.p_buff_in = (uint8_t*)&CalCmdData;
    service_params.p_buff_out = (uint8_t*)&CalCmdData;
    service_params.p_len_out = &length_out;
    
	if(id_list[0]==0x02)
		*service_params.p_buff_in=1;
	
    if (SERVICE_DoAutoCalibration(&service_params) != SERVICE_OK)
    {
			return 1;	// error
    }
    else
    {// we can attempt to print the values returned in CalCmdData
			return 0; // No error
    }
}

//------------------------------------------------------------

void send_done_msg(void)
{
	asendbare((uint8_t *)"Done\r\n");
}

void send_error_msg(void)
{
	asendbare((uint8_t *)"Error\r\n");
}

// this function should not be used - use the DLMS class08 set feature
void write_rtc_stuff(uint8_t c);
void write_rtc_stuff(uint8_t c)
{
  uint8_t* ptr;
	rtc_calendarcounter_value_t ren_rtc;	// this structure is used to read the renesas rtc
  uint8_t check;
  uint8_t i;
  
  if(c==0)
  {// time is being set - the time value is already in bcd
	// for the renesas meter we can get the calendar counter value and 
   // modify the desired parameters and the set the calendar counter value
		R_RTC_Get_CalendarCounterValue(&ren_rtc);	// this way we get to read the RTC - careful about BCD data
// update the seconds, minutes and hours variables - leaving others intact		
		ren_rtc.rseccnt = rtc.second;
		ren_rtc.rmincnt = rtc.minute;
		ren_rtc.rhrcnt = rtc.hour;
		R_RTC_Set_CalendarCounterValue(ren_rtc);	// now set the calendar counter - time will get changed
  }
  else
  {// here it is a DATE command - hence leave date,month,year as it is and convert 
   // seconds, minutes and hours into packed bcd  
		R_RTC_Get_CalendarCounterValue(&ren_rtc);	// this way we get to read the RTC - careful about BCD data
// update the date, month and year (careful about the year) variables - leaving others intact
		ren_rtc.rdaycnt=rtc.date;
		ren_rtc.rmoncnt=rtc.month;
		ren_rtc.ryrcnt= ((int16_t)rtc.year)&0x00FF;
		R_RTC_Set_CalendarCounterValue(ren_rtc);	// now set the calendar counter - time will get changed
  }

// after performing this operation the affected rtc values would be in bcd - for our use they should be in binary
// the simplest thing would be to call readRTC
	readRTC();

// the following fix is not needed anymore as the EM is being stopped and started in R_RTC_Set_CalendarCounterValue
// which should hopefully fix the -66% error
// the following is a temproary fix to make sure that calibration is not upset after setting date/time
//	sprintf((char*)&amsgbuf[0],"%s","GETCAL");
//	aanalmsg();
// we must do the survey and log_updates  - which are commented for the time being 
	
 
	do_initial_things();	// survey update is called in do initial things
}

//#define BANKTEST	// only for testing the working of three memory banks


#ifdef TN3P_BOARD
void restoreMeterDisplay(void)
{
	uint8_t i;
	
	if(Common_Data.meterType==0)
	{// default is bidirectional meter
		for(i=0;i<DSP_ARRAY_SIZE;i++)
			dspArr[i]=factoryDispPgmBidir[i];
	}
	else
	{
		for(i=0;i<DSP_ARRAY_SIZE;i++)
			dspArr[i]=factoryDispPgmUnidir[i];
	}
		
	Common_Data.displayProgrammed=0;
	write_common_data();
}
#endif

#ifdef BANKTEST
	uint32_t page_offset;
	uint8_t page_no;
#endif

uint32_t extractMeterNumber(uint8_t arr[]);

uint32_t extractMeterNumber(uint8_t arr[])
{// the buffer contains a null terminated string aaaannnnnnn
// there are alphanumeric characters followed by a number

	uint32_t retval;
	int8_t i;
	int8_t tempchar;

	retval=0;
	
	for(i=0;i<15;i++)
	{
		tempchar = arr[i];
		if((tempchar>=0x30)&&(tempchar<=0x39))
		{// we have found the first numeric digit
			retval = atol((char*)&arr[i]);
			break;
		}
	}
	return retval;
}

void aanalmsg(void)
{
  eventdata_t temp_eventdata;
	uint8_t *ptr;
	uint32_t tempLong;
	uint8_t mfgcode[4];
	uint8_t channel;
	uint8_t tbuf[8];
	uint8_t buf[64];
	uint8_t i;
	uint8_t indx;
	uint8_t currchar;
	char tempChar;
	uint8_t retval;
	
//	#ifdef EPPELTONE
	mfgcode[0]='E';
	mfgcode[1]='E';
	mfgcode[2]='O';
//	#endif

#ifdef TN3P_BOARD
	if(nowcheck(&amsgbuf[0],(uint8_t *)"SETDSPSTART")==1)
	{
		g_dsp_progFlag=1;
		for(i=0;i<DSP_ARRAY_SIZE;i++)
			dspArr[i]=-1;	// initialize & fill the array with -1
		sendok();
		return;
	}
	
	if(amsgbuf[0]==':')
	{
		if(g_dsp_progFlag!=0)
		{
			if(0==handleDisplayProgrammingCommands(amsgbuf))
				sendERROR();	// returning 0 is error
			else
				sendok();	// returning 1 is good
		}
		else
		{
			asendbare((uint8_t *)"Please Send Start Cmd");    
			asendcrlf();
		}
		return;
	}
	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"SETDSPEND")==1)
	{
		updateDisplayProgrammingCommands();
		asendbare((uint8_t *)"Done");    
		asendcrlf();
		return;
	}

	if(nowcheck(&amsgbuf[0],(uint8_t *)"RESTOREDSP")==1)
	{// this command may be used to restore the factory programmed dspArr
		restoreMeterDisplay();
		asendbare((uint8_t *)"Restored");    
		asendcrlf();
		return;
	}
#endif

#ifdef CTL_FAULT_DEBUG
	if(nowcheck(&amsgbuf[0],(uint8_t *)"CORRUPT")==1)
	{// this will corrupt the ctldata checksum
		ctldata.overflow|=FAULT_FLAG;
		fill_structure((uint8_t*)&ctldata,sizeof(ctldata),1);
		EPR_Write(CTLDATA_BASE_ADDRESS, (uint8_t*)&ctldata, sizeof(ctldata));
		EPR_Write(ALTERNATE_CTLDATA_BASE_ADDRESS, (uint8_t*)&ctldata, sizeof(ctldata));
		sendok();
		while(1); // force a reset
		return;
	}
#endif

	if(nowcheck(&amsgbuf[0],(uint8_t *)"SETLED")==1)
	{// SETLED n - 0:kvar, 1-kva
		i = (uint8_t)(atoi((char *)&amsgbuf[6]));
		if(i<2)
		{
			if(i!=Common_Data.led_function)
			{
				Common_Data.led_function = i;
				write_common_data();
			}
		}
		sendok();
		return;
	}
	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"MDRESET")==1)
	{// MDRESET n - 0:NO, 1-YES
		i = (uint8_t)(atoi((char *)&amsgbuf[7]));
		if(i<2)
		{
			if(i!=Common_Data.manualResetPermitted)
			{
				Common_Data.manualResetPermitted = i;
				write_common_data();
			}
		}
		sendok();
		return;
	}
	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"MDTYPE")==1)
	{// MDTYPE n - 0:kw, 1-kva
		i = (uint8_t)(atoi((char *)&amsgbuf[6]));
		if(i<2)
		{
			if(i!=Common_Data.mdType)
			{
				Common_Data.mdType = i;
				write_common_data();
			}
		}
		sendok();
		return;
	}

	if(nowcheck(&amsgbuf[0],(uint8_t *)"OK")==1)
	{
		sendok2();
//		getInstantaneousParams(IPFS,LINE_PHASE_R);
		return;
	}

	if(nowcheck(&amsgbuf[0],(uint8_t *)"MEMTEST")==1)
	{// we need to check the locations 0x0ffff, 0x1ffff and 0x2ffff
//		if(CALSW==0)	// for testing only
		if(calswitch!=0)	// for actual use
		{// PROD mode only
			gCalFlag=1;
  		LCD_ClearAll();
	  	LCD_DisplayDigit(1, LCD_CHAR_M);
	  	LCD_DisplayDigit(2, LCD_CHAR_E);
	  	LCD_DisplayDigit(3, LCD_CHAR_M);

			buf[0]=0x55;
			buf[1]=0xAA;
			indx=2;	// this is the number of bytes
			
// check the first bank	
			retval = checkMemory(0x0fffe,(uint8_t*)buf, (uint8_t*)tbuf, indx);
			if(retval==0)
	  		LCD_DisplayDigit(5, LCD_CHAR_P);	// pass
			else
	  		LCD_DisplayDigit(5, LCD_CHAR_F);
		
// check the second bank
			retval = checkMemory(0x1fffe,(uint8_t*)buf, (uint8_t*)tbuf, indx);
			if(retval==0)
	  		LCD_DisplayDigit(6, LCD_CHAR_P);	// pass
			else
	  		LCD_DisplayDigit(6, LCD_CHAR_F);

// check the third bank
			retval = checkMemory(0x2fffe,(uint8_t*)buf, (uint8_t*)tbuf, indx);
			if(retval==0)
	  		LCD_DisplayDigit(7, LCD_CHAR_P);	// pass
			else
	  		LCD_DisplayDigit(7, LCD_CHAR_F);
		}
		return;
	}
	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"CREEP")==1)
	{
		if(calswitch!=0)
		{// PROD mode only
			gCalFlag=1;
			if(total_active_power==0)
			{
				lcd_display_pass();
				sendok();
			}
			else
			{
				lcd_display_fail();
				sendERROR();
			}
		}
		return;
	}

	if(nowcheck(&amsgbuf[0],(uint8_t *)"STARTING")==1)
	{
		if(calswitch!=0)
		{// PROD mode only
			gCalFlag=1;
			if((total_active_power>11)&&(total_active_power<18))
			{
				lcd_display_pass();
				sendok();
			}
			else
			{
				lcd_display_fail();
				sendERROR();
			}
		}
		return;
	}

	if(nowcheck(&amsgbuf[0],(uint8_t *)"DISPRESET")==1)
	{
		if(calswitch!=0)
		{// PROD mode only
			gCalFlag=0;	// release the display
			disp_state=0;
		}
		return;
	}
		
	if(nowcheck(&amsgbuf[0],(uint8_t *)"CAL")==1)
	{// Auto Calibrate : 0-R, 1-Y, 2-B, 3-N
		channel = atoi((char *)&amsgbuf[3]);
		
		if(calswitch!=0)
		{// PROD mode only
			gCalFlag=1;
			lcd_display_Cal_Status();
			
			if(channel==3)
			{
//				retval = checkNeutralCurrent();
				retval = 0;			
				if(retval!=0)
				{// there is error
					lcd_display_error();
				}
				else
				{// neutral current is within limits
					retval=doAutoCal(3);	// calibrate neutral
					if(retval!=0)
					{
						lcd_display_error();
					}
					else
					{
						LCD_DisplayDigit(5, LCD_CHAR_N);
						Common_Data.meter_calibrated|=_BV(4);
					}
					
					retval=doAutoCal(4);	// save
					if(retval!=0)
					{
						lcd_display_error();
					}
					else
					{
						LCD_DisplayDigit(6, LCD_CHAR_D);
						Common_Data.meter_calibrated|=_BV(5);
						retval=0;
						write_common_data();
						
					  do
						{
					    R_WDT_Restart();
					  	MCU_Delay(50000);	// this function generates delay of 50 ms
							retval++;
						}
						while(retval<100);
						
						gCalFlag=0;	// release the display 
					}
				}
			}
			else
			{
				retval = checkPhaseVoltagesCurrents();
				if(retval!=0)
				{// there is error
					lcd_display_error();
					return;
				}
				else
				{// here all three phase voltages and currents are within calibrateable limits
					asendbare((uint8_t *)"Start_R");    
					asendcrlf();

					retval=doAutoCal(0);	// calibrate phase R
					if(retval!=0)
					{
						lcd_display_error();
						return;
					}
					else
					{
						LCD_DisplayDigit(1, LCD_CHAR_R);
						Common_Data.meter_calibrated|=_BV(0);
					}
					
					longDelay(15);
					
					asendbare((uint8_t *)"Start_Y");    
					asendcrlf();
					
					retval=doAutoCal(1);	// calibrate phase Y
					if(retval!=0)
					{
						lcd_display_error();
						return;
					}
					else
					{
						LCD_DisplayDigit(2, LCD_CHAR_Y);
						Common_Data.meter_calibrated|=_BV(1);
					}
					
					longDelay(15);
					
					asendbare((uint8_t *)"Start_B");    
					asendcrlf();
					retval=doAutoCal(2);	// calibrate phase B
					if(retval!=0)
					{
						lcd_display_error();
						return;
					}
					else
					{
						LCD_DisplayDigit(3, LCD_CHAR_B);
						Common_Data.meter_calibrated|=_BV(2);
					}
					
					longDelay(15);
					
					asendbare((uint8_t *)"Save");    
					asendcrlf();
					retval=doAutoCal(4);	// save
					if(retval!=0)
					{
						lcd_display_error();
						return;
					}
					else
					{
						LCD_DisplayDigit(4, 5);
						Common_Data.meter_calibrated|=_BV(3);
						write_common_data();
					}
				}
			}

			asendbare((uint8_t *)"Done");    
			asendcrlf();
			longDelay(5);	// delay of 5 seconds
		}
		return;
	}

	if(nowcheck(&amsgbuf[0],(uint8_t *)"ZAP")==1)
	{// erase e2rom and fill energies with properly checksummed records
	//this function all causes a reset
//		if(calswitch!=0)
		{// production mode
			clearE2Rom();
			clearRAMStructures();		
	// we are zapping the Common_data structure also and in the init_read_common_data, we will update the meterStartDate		
	// hence the following two lines are being commented
	//		Common_Data.meterStartDate =  rtc;
	//  	write_alternately(COMMON_DATA_ADDRESS, ALTERNATE_COMMON_DATA_ADDRESS, (uint8_t*)&Common_Data, sizeof(Common_Data));
			
			if(0==verifyEnergyAreaInit())
				send_done_msg();
			else
				send_error_msg();

			while(1); // force a reset
		}
		return;
	}

	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"ZPCL")==1)
	{// erase e2rom and fill energies with properly checksummed records
	//this function all causes a reset
		if(calswitch!=0)
		{
			clearE2RomCalibration();
			while(1); // force a reset
		}
		return;
	}


	if(nowcheck(&amsgbuf[0],(uint8_t *)"CLRCVR")==1)
	{// erase the cover tamper - we need to put some check here - say kwh or kwh_export
//		if(calswitch!=0)
//		{
			sendok();
	
			if((mytmprdata.tamperEvent_image&COVER_OPEN_MASK)!=0)
			{
//		    mytmprdata.tamperCount--;		// since this is not incremented for cover open, it should not be decremented either
				g_powermgmt_flag.is_cover_int=0;
				g_Class07_Event_CurrentEntry[5]=0;
				g_Class07_Event_EntriesInUse[5]=0;
				mytmprdata.tamperEvent_flags &= ~COVER_OPEN_MASK;
				mytmprdata.tamperEvent_image &= ~COVER_OPEN_MASK;
				mytmprdata.tamperEvent_on &= ~COVER_OPEN_MASK;
				mytmprdata.tamperEvent_overflow &= ~(_BV(5));
				mytmprdata.tamper_events5_wrt_ptr=EVENTS5_BASE_ADDRESS;
			  
				clear_structure((uint8_t*)&temp_eventdata,sizeof(temp_eventdata));
		    EPR_Write(mytmprdata.tamper_events5_wrt_ptr, (uint8_t*)&temp_eventdata, sizeof(temp_eventdata));

				write_alternately(MYTMPRDATA_BASE_ADDRESS, ALTERNATE_MYTMPRDATA_BASE_ADDRESS, (uint8_t*)&mytmprdata, sizeof(mytmprdata));
				g_Class01_EventCode[5]=0;
		    write_alternately(LATEST_EVENT_BASE_ADDRESS,ALTERNATE_LATEST_EVENT_BASE_ADDRESS,(uint8_t*)&g_Class01_EventCode,14);
				disp_timer=disp_autoscroll_interval;	// this is so that the C-Open message is cleared right away
			}
//		}
		return;
	}


/*	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"LOAD")==1)
	{
		testCtr = (int32_t)(atoi((char *)&amsgbuf[4]));
		sendok();
		return;
	}
*/	
	
#ifdef TESTING	
	if(nowcheck(&amsgbuf[0],(uint8_t *)"LOAD")==1)
	{
		load = (int32_t)(atoi((char *)&amsgbuf[4]));
		sendok();
		return;
	}
#endif



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
//		asendbare((uint8_t *)"SW Ver: 2.19");    
		asendbare((uint8_t *)buf);    
		asendcrlf();
		return;
	}

  if(nowcheck((uint8_t*)&amsgbuf[indx+2],(uint8_t *)"DATE")==1)
  {// MSC00DATEddmmyy - we are not checking the checksum - but command must include 00
    sendok();
    
    rtc.date = (amsgbuf[4+indx+2]-'0')*16+(amsgbuf[4+indx+3]-'0');// account for the two check sum bytes
    rtc.month = (amsgbuf[6+indx+2]-'0')*16+(amsgbuf[6+indx+3]-'0');// account for the two check sum bytes
    rtc.year = (amsgbuf[8+indx+2]-'0')*16+(amsgbuf[8+indx+3]-'0');// account for the two check sum bytes
    write_rtc_stuff(1);
    return;
  }
		
  if(nowcheck((uint8_t*)&amsgbuf[indx+2],(uint8_t *)"TIME")==1)
  {// MSC00TIMEssmmhh
    sendok();
    ptr = &rtc.second;
    for(i=4;i<10;i+=2)  //14 bytes earlier -  now 6 bytes
    {// this one is different for internal rtc
      *ptr = (amsgbuf[i+indx+2]-'0')*16+(amsgbuf[i+indx+3]-'0');// account for the two check sum bytes
      ptr++;
    }

    write_rtc_stuff(0);
    return;
  }
	
  if(nowcheck((uint8_t*)&amsgbuf[indx+2],(uint8_t *)"METER")==1)
  {// MSC00METERaaaannnnnnnn	// alphanumeric
		if(calswitch==0)
		{	// meter number can only be set in USER mode
	    sendok();
			strcpy((char*)buf,(char *)&amsgbuf[indx+2+5]);
	//		Common_Data.meterNumber=atol((char *)&amsgbuf[indx+2+5]);
			Common_Data.meterNumber=extractMeterNumber(buf);
			write_common_data();
	    write_alternately(METER_NUMBER_BASE_ADDRESS, ALTERNATE_METER_NUMBER_BASE_ADDRESS, (uint8_t*)buf, 16);
		}
		else
			sendERROR();
    return;
  }

#ifdef RECOVERENERGIES
  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"RECOVER")==1)
  {// MSCRECOVER
    sendok();
		R_WDT_Restart();
		recoverEnergies();
		update_saved_energies();
    return;
  }
#endif	
	
  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"RST")==1)
  {// MSCRST - EEORST	// reset the meter  and enter bootloader
		while(1); // force a reset
    return;
  }

  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"BAT")==1)
  {// MSCBAT
		if(Common_Data.rtc_failed==0)
			asendbare((uint8_t *)"Good");
		else
			asendbare((uint8_t *)"Bad");
			
		asendcrlf();
    return;
  }

	
  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"BM")==1)
  {// EEOBM  - batch month
		retval = (uint8_t)(atoi((char *)&amsgbuf[indx+2]));
		if((retval>=1)&&(retval<13))
		{
			Common_Data.batchmonth = retval;
			e2write_flags|=E2_W_IMPDATA;
    	sendok();
		}
		else
			sendERROR();
			
		asendcrlf();
    return;
  }
	
  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"BY")==1)
  {// EEOBY  - batch year
		retval = (uint8_t)(atoi((char *)&amsgbuf[indx+2]));
		if((retval>=1)&&(retval<100))
		{
			Common_Data.batchyear = retval;
			e2write_flags|=E2_W_IMPDATA;
    	sendok();
		}
		else
			sendERROR();
			
		asendcrlf();
    return;
  }

  if(nowcheck((uint8_t*)&amsgbuf[indx],(uint8_t *)"SPOTBILLH")==1)
  {// EEOSPOTBILLH : we need to send 48 bytes
	// compose the buffer
		buf[0]='$';	// header
		buf[1]=44;	// number of data bytes =44
		getFullMeterNumber(&buf[2]);	// get the meter number in ASCII and copy into buffer
// now to fill buf[18] onwards - current date time
		buf[18]=rtc.date;
		buf[19]=rtc.month;
		buf[20]=rtc.year;
		buf[21]=rtc.hour;
		buf[22]=rtc.minute;
		buf[23]=rtc.second;
// present kwh
		buf[24]=(uint8_t)myenergydata.kwh;
		buf[25]=(uint8_t)(myenergydata.kwh>>8);
		buf[26]=(uint8_t)(myenergydata.kwh>>16);
		buf[27]=(uint8_t)(myenergydata.kwh>>24);
// present md kw - mymddata.mdkw
//		tempLong = getInstantaneousParams(ACT_POWER,LINE_TOTAL);
		tempLong = (uint32_t)mymddata.mdkw;
		buf[28]=(uint8_t)tempLong;
		buf[29]=(uint8_t)(tempLong>>8);
		buf[30]=(uint8_t)(tempLong>>16);
		buf[31]=(uint8_t)(tempLong>>24);
// billingdate
    retval = decrementMonth(1);
    EPR_Read(BILLDATA_BASE_ADDRESS+(retval-1)*BILL_PTR_INC,(uint8_t*)tbuf,8);	// get 8 bytes of billing date into tbuf
		buf[32]=tbuf[3];	// date
		buf[33]=tbuf[2];	// month
		buf[34]=((((int32_t)tbuf[0]*(int32_t)256) + (int32_t)tbuf[1])-(int32_t)2000);	// year iwthout the century
		buf[35]=tbuf[5];	// hour
		buf[36]=tbuf[6];	// minute
		buf[37]=tbuf[7];	// second
// previous kwh
    EPR_Read(BILLDATA_BASE_ADDRESS+(retval-1)*BILL_PTR_INC+16,(uint8_t*)&tempLong,4);
		buf[38]=(uint8_t)tempLong;
		buf[39]=(uint8_t)(tempLong>>8);
		buf[40]=(uint8_t)(tempLong>>16);
		buf[41]=(uint8_t)(tempLong>>24);
// previous kw - this is obviously demand		
    EPR_Read(BILLDATA_BASE_ADDRESS+(retval-1)*BILL_PTR_INC+96,(uint8_t*)&tempLong,4);
		buf[42]=(uint8_t)tempLong;
		buf[43]=(uint8_t)(tempLong>>8);
		buf[44]=(uint8_t)(tempLong>>16);
		buf[45]=(uint8_t)(tempLong>>24);
		buf[46]=0x0d;
		buf[47]=0x0a;
		asendHexMultiple(buf, 48);
		
//		asendcrlf();
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
