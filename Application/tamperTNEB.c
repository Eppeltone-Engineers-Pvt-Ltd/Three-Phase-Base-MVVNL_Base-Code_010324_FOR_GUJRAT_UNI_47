#include <stdio.h>
#include "typedef.h"
#include "emeter3_structs.h"
#include "tamperTNEB.h"
#include "e2rom.h"
#include "tampers.h"
#include "memoryOps.h"
#include "utilities.h"
#include "calendarFunctions.h"
//----------------------------
uint8_t g_ControlTable[64];
uint8_t g_controlTableIndex=255;

extern uint32_t g_Class07_Event_EntriesInUse[];

void clearRestoreArea(uint32_t *wrt_ptr);	// private function
void updateCumulativeTamperDuration(int32_t tamperDuration, uint8_t event_code_bit);

void loadControlTable(uint8_t whichGroup)
{
// the function calling this table must ensure that only 0,1 and 4 are called	
	uint32_t startAddr;

	if(g_controlTableIndex == whichGroup)
		return;	// already loaded
	
	if(whichGroup==0)
		startAddr = CONTROL_TABLE0_ADDRESS;
	else if(whichGroup==1)
		startAddr = CONTROL_TABLE1_ADDRESS;
	else
		startAddr = CONTROL_TABLE4_ADDRESS;

// read this table from e2rom
	EPR_Read(startAddr, (uint8_t*)&g_ControlTable, sizeof(g_ControlTable));
	g_controlTableIndex = whichGroup;
}

void createControlTable(uint32_t *wrt_ptr, uint32_t base_addr, uint32_t end_address, uint8_t whichGroup)
{
	uint32_t evenAddress;
	uint8_t otherTable[16];
	ext_rtc_t restoreTime;
	
	uint8_t i;	// for 60 events i goes from 0 to 59
	uint8_t t;	// running index for the table
	uint8_t k;	// running index for the Occurrences only table
	uint8_t ktemp;
	
	if((whichGroup==2)||(whichGroup==3)||(whichGroup==5))
		return;
		
	if((mytmprdata.tamperEvent_overflow&_BV(whichGroup))==0)
	{// there is no overflow - we need start from oldest record and scan upto finishAddr
		evenAddress = base_addr;	// initialise to oldest record
	}
	else
	{// there is overflow - we need to start from oldest record and scan upto finishAddr
		evenAddress = *wrt_ptr;	// initialise to oldest record
	}

// first step - go through all the records which could be 60 or less
// and copy the occurred and restored event pairs into table, and 
// copy the occurred but not restored pairs into otherTable
	t=0;
	k=0;	// other table index
	i=0;	// this index goes from 0 to g_Class07_Event_EntriesInUse[whichGroup] 

	do
	{
		// read the odd record
		EPR_Read((evenAddress + ((uint32_t)EVENT_PTR_INC/(uint32_t)2)), (uint8_t*)&restoreTime, sizeof(restoreTime));	// this is the odd half - restore portion
		if((restoreTime.date==0)&&(restoreTime.month==0)&&(restoreTime.year==0))
		{// this is an urestored event - copy this index to the otherTable
			otherTable[k] = i;
			k++;
		}
		else
		{// this is a restored event - copy two indices
			g_ControlTable[t++]=i;
			g_ControlTable[t++]=i+1;
		}
		
		incptr(&evenAddress, base_addr, end_address, EVENT_PTR_INC);
		i=i+2;
	}
	while(evenAddress != *wrt_ptr);
	
// second step - do not touch the index 't'
// copy all the entries from other table into this table
	ktemp = 0;
	for (ktemp=0; ktemp<k; ktemp++)
	{
		g_ControlTable[t] = otherTable[ktemp];
		t++;
	}
// done - we now have the table ready
// these tables are to be stored for whichGroup = 0,1 and 4 only
	if(whichGroup==0)
		evenAddress = CONTROL_TABLE0_ADDRESS;
	else if(whichGroup==1)
		evenAddress = CONTROL_TABLE1_ADDRESS;
	else
		evenAddress = CONTROL_TABLE4_ADDRESS;

// store this table in e2rom

	EPR_Write(evenAddress, (uint8_t*)&g_ControlTable, sizeof(g_ControlTable));
}


void updateCumulativeTamperDuration(int32_t tamperDuration, uint8_t event_code_bit)
{
	cumtamperdurn_t cumTamperDurn;
  if(read_alternately(CUM_TAMPER_DURN_ADDRESS, ALTERNATE_CUM_TAMPER_DURN_ADDRESS, (uint8_t*)&cumTamperDurn, sizeof(cumTamperDurn))==0)
	{// e2rom does not contain proper data
	  clear_structure((uint8_t*)&cumTamperDurn,sizeof(cumTamperDurn)); // this function fills zero bytes in the structure
	}	
	// now that we have read, we can modify and write
	cumTamperDurn.duration[event_code_bit] += tamperDuration;	// in seconds
  write_alternately(CUM_TAMPER_DURN_ADDRESS, ALTERNATE_CUM_TAMPER_DURN_ADDRESS, (uint8_t*)&cumTamperDurn, sizeof(cumTamperDurn));
}

void clearRestoreArea(uint32_t *wrt_ptr)
{
	ext_rtc_t restoreTime;
	
	restoreTime.date=0;
	restoreTime.month=0;
	restoreTime.year=0;

	EPR_Write((*wrt_ptr + (uint32_t)(EVENT_PTR_INC/2)), (uint8_t*)&restoreTime, sizeof(restoreTime));
}

void TNEB_Tamper_Write(uint32_t *wrt_ptr, uint32_t base_addr, uint32_t end_addr, eventdata_t* eventDataPtr, uint8_t whichGroup)
{
//	eventdata_t tempEventData;
	ext_rtc_t occurTime;
	ext_rtc_t restoreTime;
	int32_t tamperDuration;
	uint32_t address;
	uint32_t terminalAddress;
	uint8_t event_code_bit;
	uint8_t done;
	
	if(eventDataPtr->event_type!=0)
	{// this is an occurence
		if((mytmprdata.tamperEvent_overflow&_BV(whichGroup))==0)
		{// here there is no overflow, hence we can merrily write
	    EPR_Write(*wrt_ptr, (uint8_t*)eventDataPtr, sizeof(eventdata_t));
// since this is an occurence event, we must also null the restoration record
			clearRestoreArea(wrt_ptr);
// now we should increment the wrt_ptr
			incptr_maskless(wrt_ptr, base_addr, end_addr, EVENT_PTR_INC);
		}
		else
		{// here we have an overflow situation
// if this record is an occurence which has not been restored yet then we must skip this fellow
// to do that we will have to read the restoration record date
// for an event which has not been restored, the restoreTime will be all zeroes
			address = *wrt_ptr;	
			EPR_Read((address+ (uint32_t)(EVENT_PTR_INC/2)), (uint8_t*)&restoreTime, sizeof(restoreTime));
			if((restoreTime.date==0)&&(restoreTime.month==0)&&(restoreTime.year==0))
			{// this record has not be restored yet
				done = 0;
				terminalAddress = address;	// some reference to determine that all addresses have been scanned
				do
				{// first increment the address
					incptr_maskless(&address, base_addr, end_addr, EVENT_PTR_INC);
					if(address==terminalAddress)
					{// we have reached the starting point - giveUp
						done=1;
						break;
					}
					// else check this record
					EPR_Read((address+(uint32_t)(EVENT_PTR_INC/2)), (uint8_t*)&restoreTime, sizeof(restoreTime));
					if((restoreTime.date==0)&&(restoreTime.month==0)&&(restoreTime.year==0))
					{// this record is also an occurence which has not been restored
						continue;
					}
					else
					{// here the occurence has been restored - hence we can overwrite this fellow
						*wrt_ptr = address;
	    			EPR_Write(*wrt_ptr, (uint8_t*)eventDataPtr, sizeof(eventdata_t));
						// since this is an occurence event, we must also null the restoration record
						clearRestoreArea(wrt_ptr);
// now we should increment the wrt_ptr
						incptr_maskless(wrt_ptr, base_addr, end_addr, EVENT_PTR_INC);
					}
				}	// search for a restored event
				while(done==0);
			}	// occurence not restored record encountered
			else
			{// this occurrence has been restored encountered - we can safely overwrite this record
				*wrt_ptr = address;
  			EPR_Write(*wrt_ptr, (uint8_t*)eventDataPtr, sizeof(eventdata_t));
				// since this is an occurence event, we must also null the restoration record
				clearRestoreArea(wrt_ptr);
// now we should increment the wrt_ptr
				incptr_maskless(wrt_ptr, base_addr, end_addr, EVENT_PTR_INC);
			}
		}	// overflow situation
		mytmprdata.unRestoredTampersCount[whichGroup]++;
	}	// eventType !=0
	else
	{// this is a restoration event
		// the occurence record of this record has been written in the past
		// if no overflow has occured, we must search backwards till the base_addr
		// if overflow has occured, we must stop search once we read the startAddress
		if((mytmprdata.tamperEvent_overflow&_BV(whichGroup))==0)
			terminalAddress=base_addr;
		else
			terminalAddress = *wrt_ptr;

		done = 0;
		address = *wrt_ptr;
		decptr_maskless(&address, base_addr, end_addr, EVENT_PTR_INC); // begin with the previous record which is the newest record
		do
		{// we are looking for the unrestored occurence record which has the same event_code_bit as this restoration case
			EPR_Read(address, (uint8_t*)&occurTime, sizeof(occurTime));	// read the occurence time - we may need it
			EPR_Read((address+(uint32_t)EVENT_CODE_OFFSET), &event_code_bit, sizeof(event_code_bit));

//			EPR_Read(address, (uint8_t*)&tempEventData, sizeof(tempEventData));
//			if(tempEventData.event_code_bit==eventDataPtr->event_code_bit)
			if(event_code_bit==eventDataPtr->event_code_bit)
			{// we got the record we wanted
				// should we check if this record is restored or not - since this is the earliest encountered
				// occurence record, it must be the one which has not been restored
				// if for some reason it has been restored then it may be an error - let us be safe
				EPR_Read((address+(uint32_t)(EVENT_PTR_INC/2)), (uint8_t*)&restoreTime, sizeof(restoreTime));
				if((restoreTime.date==0)&&(restoreTime.month==0)&&(restoreTime.year==0))
				{// this record is has not been restored - hence we can write here
					EPR_Write((address+(uint32_t)(EVENT_PTR_INC/2)),(uint8_t*)eventDataPtr, sizeof(eventdata_t));
					restoreTime = eventDataPtr->datetime;
					tamperDuration = diffInSecondsBetween(restoreTime, occurTime);
					updateCumulativeTamperDuration(tamperDuration, event_code_bit); 
					done=1;// job is done
				}
			}
// here either event_code does not match or the record is already restored or done is 1					 
			if(done==0)
			{
				if(address==terminalAddress)
					done = 1;	// we were unsuccesful in finding a matching record
				else
				{// continue the search in the ulta direction
					decptr_maskless(&address, base_addr, end_addr, EVENT_PTR_INC);
				}
			}
		}
		while(done == 0);
		mytmprdata.unRestoredTampersCount[whichGroup]--;
	}// restoration event
}
//----------------------------

