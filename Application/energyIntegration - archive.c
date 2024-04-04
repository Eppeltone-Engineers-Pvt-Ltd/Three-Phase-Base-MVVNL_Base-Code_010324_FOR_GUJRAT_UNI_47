// energyIncrementation.c


#include "r_cg_macrodriver.h"	// located in driver/mcu
#include "typedef.h"            // located right at top of tree below iodefine.h
//#include "r_cg_wdt.h"
//#include "r_cg_rtc.h"
#include "em_type.h"

#include "em_core.h"                    /* EM Core APIs */
#include "em_maths.h"

/* Application layer */
//#include "platform.h"
#include "r_lcd_display.h"

#include "emeter3_structs.h"
#include "energyIntegration.h"
#include "tampers.h"
#include "utilities.h"
#include "em_display.h"

extern void asendbare(uint8_t* e2buf);


extern float32_t getInstantaneousParams(uint8_t param, EM_LINE line);
extern float32_t computeTwoWirePower(EM_LINE line);
extern uint8_t checkForVoltageHarmonics(EM_LINE line);
extern uint8_t checkForCurrentHarmonics(EM_LINE line);
extern void update_energies_stored(void);

int32_t getPower(uint8_t param, EM_LINE line);
int8_t isSameSign(int32_t power1, int32_t power2);

int16_t systemPF;	// this is a signed number between 1000 and -1000
int8_t mixedPowers;	// this is 0 if all active energies are of the same sign, else this is 1

int32_t getPower(uint8_t param, EM_LINE line)
{// this function returns power with appropriate number of decimals
	int32_t temp_power;
	int32_t abs_power;
#ifdef SIX_DECIMALS
		temp_power = (int32_t)(getInstantaneousParams(param,line)*1000);
#else
		temp_power = (int32_t)(getInstantaneousParams(param,line)*100);
#endif
	
	abs_power=temp_power;
	if(temp_power<0)
		abs_power*=-1;
	
#ifdef SIX_DECIMALS
	if(abs_power>24000000)	//(240*100*1000))
		temp_power=0;

	#ifdef CLASS1_METER		
	if(abs_power<=2300)	// 2.3 watts
		temp_power=0;
	#else
	if(abs_power<=600)	// 0.6 watts
		temp_power=0;
	#endif
	
#else
	if(abs_power>2400000)		//(240*100*100))
		temp_power=0;
		
	#ifdef CLASS1_METER		
	if(abs_power<=230)	// 2.3 watts
		temp_power=0;
	#else
	if(abs_power<=60)	// 0.6 watts
		temp_power=0;
	#endif	
#endif	
		
	return temp_power;
}

int8_t isSameSign(int32_t power1, int32_t power2)
{
	int8_t retval=0;	// False
	
	if(power1>=0)
	{
		if(power2>=0)
			retval=1;
	}
	else
	{// here power1 is -ve
		if(power2<0)
			retval=1;
	}
	return retval;	// if same sign then 1 ,else 0	
}


void energy_incrementation(void)
{
// we do not seem to be performing any checks on whether we are in the correct operating mode or not.
// we ought to

// power is reported in watts - hence if only three places of decimal are required in energy, we can
// simply convert the SDK value into an int32

	EM_PF_SIGN power_factor_sign;
	uint8_t outOfPhaseHarmonics;
	int32_t sigmaPower;
	int32_t temp_power;
	int32_t temp_total_active_power;
	int32_t temp_total_apparent_power;
	int32_t temp_total_reactive_power;
	int32_t tempLong;
	int32_t phase_power_active;
	int32_t phase_power_reactive;	
	
	float32_t factor;
	float32_t measured_pf;
	float32_t measured_current;
	int16_t timeInMinutes;
	int8_t i;
//	float32_t saved_temp_total_active_power;
//	uint8_t tbuf[16];
	
//	char tbuf[16];	
//	exportMode = 0; // assume import mode as default
//-----------------------kwh----------------------------------------------------  
// reverse status is being done in tampers.c where individual active power sign is being considered

	temp_total_active_power=0;	// to begin with
//	temp_total_reactive_power_lag =0;
//	temp_total_reactive_power_lead =0;
	temp_total_reactive_power = 0;
	temp_total_apparent_power = 0;
	sigmaPower = 0;
	
	for(i=0;i<3;i++)
	{
		phase_power_active = getPower(ACT_POWER,(EM_LINE)i);
#ifdef BIDIR_METER		
		if(Common_Data.meterType!=0)
		{// here meter type is Unidirectional
			if(phase_power_active<0)
				phase_power_active*=-1;	// -ve powers represent reverse tamper - NOT Export mode
		}
#endif

#ifdef BIDIR_METER		
		if(phase_power_active<0)
		{// this phase is running in export mode
			temp_power=-1*phase_power_active;
		// if there is a requirement to account for Harmonic distortion in voltage / current do it here per phase
			total_consumed_active_energy_export+=temp_power;
			temp_total_active_power-=temp_power;	// here temp_power is being subtracted
			sigmaPower+=temp_power;	//temp_power is +ve and is being added
			
			timeInMinutes = rtc.hour*60 +rtc.minute;
			tempLong = (total_consumed_active_energy_export/(int32_t)(KWH_DIVIDER));

			if(((timeInMinutes>1200)||(timeInMinutes<300))&&(tempLong>0))
			{// there seems to be export in noSolar time period < 05:00 and > 20:00
			 // this energy is to be added to nonSolar export
				myenergydata.kwh_nonSolar+= tempLong;
			}
			
//			if(tempLong>0)
			if(total_consumed_active_energy_export>0)
			{
				myenergydata.kwh_export += tempLong;
				myenergydata.zone_kwh_export += tempLong;  // add the kwh consumption for tod
				total_consumed_active_energy_export = total_consumed_active_energy_export%(int32_t)(KWH_DIVIDER);
				high_resolution_kwh_value_export = (total_consumed_active_energy_export/(int32_t)HIGH_RES_DIVIDER);
			}
			
			// now for reactive power
			
			phase_power_reactive = getPower(REACT_POWER_SIGNED,(EM_LINE)i);
			power_factor_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,(EM_LINE)i);
			temp_total_reactive_power+=phase_power_reactive;
			if(power_factor_sign==PF_SIGN_LEAD_C)
			{
				temp_power=-phase_power_reactive;
				
				total_consumed_reactive_energy_lead_export+=temp_power;
				tempLong = (total_consumed_reactive_energy_lead_export/(int32_t)(KWH_DIVIDER));

//				if(tempLong>0)
				if(total_consumed_reactive_energy_lead_export>0)
				{
					myenergydata.kvarh_lag_export += tempLong;
				  myenergydata.zone_kvarh_lag_export += tempLong;  // add the kvah consumption for tod
					total_consumed_reactive_energy_lead_export = total_consumed_reactive_energy_lead_export%(int32_t)(KWH_DIVIDER);
//					high_resolution_kvarh_lag_value_export = (total_consumed_reactive_energy_lead_export/(int32_t)HIGH_RES_DIVIDER);
				}
			}
			
			if(power_factor_sign==PF_SIGN_LAG_L)
			{	
				temp_power=phase_power_reactive;
				total_consumed_reactive_energy_lag_export+=temp_power;
				tempLong = (total_consumed_reactive_energy_lag_export/(int32_t)(KWH_DIVIDER));
				
//				if(tempLong>0)
				if(total_consumed_reactive_energy_lag_export>0)
				{
					myenergydata.kvarh_lead_export += tempLong;
				  myenergydata.zone_kvarh_lead_export += tempLong;  // add the kvah consumption for tod
					total_consumed_reactive_energy_lag_export = total_consumed_reactive_energy_lag_export%(int32_t)(KWH_DIVIDER);
//					high_resolution_kvarh_lead_value_export = (total_consumed_reactive_energy_lag_export/(int32_t)HIGH_RES_DIVIDER);
				}
			}
			
			// now for apparent power which is done differently by using sigma(active_power) and sigma(reactive_power)
			// this has to go out of this loop
			// if active and reactive powers have opposite signs then apparent power = active power (CBIP) - Lag only Tariff
			
			if(isSameSign(phase_power_active,phase_power_reactive)==1)
			{// only for quadrant 1 and 3 - signs are same
				temp_power = EM_sqrt(EM_pow((float32_t)phase_power_active,2) + EM_pow((float32_t)phase_power_reactive,2));
			}
			else
			{// signs are different
				temp_power = ABS(phase_power_active);
			}
			// the above computation ensures that apparent power >= active power			
			temp_total_apparent_power+=temp_power;				
			total_consumed_apparent_energy_export+=temp_power;
			tempLong = (total_consumed_apparent_energy_export/(int32_t)(KWH_DIVIDER));
			
//			if(tempLong>0)
			if(total_consumed_apparent_energy_export>0)
			{
				myenergydata.kvah_export += tempLong;
				myenergydata.zone_kvah_export += tempLong;  // add the kvah consumption for tod
				total_consumed_apparent_energy_export = total_consumed_apparent_energy_export%(int32_t)(KWH_DIVIDER);
//				high_resolution_kvah_value_export = (total_consumed_apparent_energy_export/(int32_t)HIGH_RES_DIVIDER);
			}
		}
		else
		{// this phase is running in import mode
			temp_power=phase_power_active;	// phase_power_active is positive 
#endif		
		// if there is a requirement to account for Harmonic distortion in voltage / current do it here per phase
#ifndef BIDIR_METER			
			if(phase_power_active<0)
				temp_power=-1*phase_power_active;	// temp_power is always +ve
			else
				temp_power=phase_power_active;

// fundamental power computation
			measured_pf = EM_GetPowerFactor(LINE_TOTAL);
			if(measured_pf<0)
				measured_pf*=-1;

			if(0!=checkForVoltageHarmonics((EM_LINE)i))
			{// voltage harmonics > 7%
				if(0!=checkForCurrentHarmonics((EM_LINE)i))
				{// current harmonics > 37%
					if(measured_pf<0.93)
					{
						temp_power =  (int32_t)((float32_t)temp_power * 1.0815);
					}
				}
			}
			sigmaPower+=phase_power_active;	// phase_power_active and hence sigmaPower can be both positive or negative
#endif			
			temp_total_active_power+=temp_power;	// temp_power is +ve and is being added
			
#ifdef BIDIR_METER			
			sigmaPower+=temp_power;	//temp_power is +ve and is being added - for BIDIR case sigmaPower is always positive whereas temp_total_active_power can be positive or negative
#endif
			total_consumed_active_energy+=temp_power;
			tempLong = (total_consumed_active_energy/(int32_t)(KWH_DIVIDER));
			
//			if(tempLong>0)
			if(total_consumed_active_energy>0)
			{					
				myenergydata.kwh += tempLong;
				myenergydata.zone_kwh += tempLong;  // add the kwh consumption for tod
				total_consumed_active_energy = total_consumed_active_energy%(int32_t)(KWH_DIVIDER);
				high_resolution_kwh_value = (total_consumed_active_energy/(int32_t)HIGH_RES_DIVIDER);
			}

#ifdef SPLIT_ENERGIES_REQUIRED
			power_factor_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,(EM_LINE)i);
			if(power_factor_sign==PF_SIGN_LEAD_C)
			{
				total_consumed_active_energy_lead+=temp_power;
				tempLong = (total_consumed_active_energy_lead/(int32_t)(KWH_DIVIDER));
				if(tempLong>0)
				{					
					myenergydata.kwh_lead += tempLong;
					total_consumed_active_energy_lead = total_consumed_active_energy_lead%(int32_t)(KWH_DIVIDER);
				}
			}
			else
			{
				if(power_factor_sign==PF_SIGN_LAG_L)
				{
					total_consumed_active_energy_lag+=temp_power;
					tempLong = (total_consumed_active_energy_lag/(int32_t)(KWH_DIVIDER));
					if(tempLong>0)
					{					
						myenergydata.kwh_lag += tempLong;
						total_consumed_active_energy_lag = total_consumed_active_energy_lag%(int32_t)(KWH_DIVIDER);
					}
				}
			}
#endif
			
			
#ifdef FUNDAMENTAL_ENERGY_REQUIRED
// we should do this only when harmonics are present - not always
			factor = 1.0;
			if(0!=checkForVoltageHarmonics((EM_LINE)i))
			{// voltage harmonics > 7%
				if(0!=checkForCurrentHarmonics((EM_LINE)i))
				{// current harmonics > 37%
					factor = 1.0/1.04;
				}
			}
//			else
//				factor = 1.0;
				
			total_consumed_fundamental_active_energy+=((float32_t)temp_power*factor);
			
//		  LCD_ClearAll();	
//			LCD_DisplayIntWithPos(total_consumed_fundamental_active_energy, LCD_LAST_POS_DIGIT);
			
			tempLong = (total_consumed_fundamental_active_energy/(int32_t)(KWH_DIVIDER));
			
//			if(tempLong>0)
			if(total_consumed_fundamental_active_energy>0)
			{					
				myenergydata.kwh_fundamental += tempLong;
				total_consumed_fundamental_active_energy = total_consumed_fundamental_active_energy%(int32_t)(KWH_DIVIDER);
				high_resolution_kwh_fundamental_value = (total_consumed_fundamental_active_energy/(int32_t)HIGH_RES_DIVIDER);
			}			
#endif			
			// now for reactive power
			
			phase_power_reactive = getPower(REACT_POWER_SIGNED,(EM_LINE)i);
			power_factor_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,(EM_LINE)i);
			temp_total_reactive_power+=phase_power_reactive;
			
			if(power_factor_sign==PF_SIGN_LEAD_C)
			{
#ifndef BIDIR_METER
				if(phase_power_active>0)
					temp_power=-phase_power_reactive;
				else
					temp_power=phase_power_reactive;
#else
				temp_power=-phase_power_reactive;
#endif
				
				total_consumed_reactive_energy_lead+=temp_power;
				tempLong = (total_consumed_reactive_energy_lead/(int32_t)(KWH_DIVIDER));
				
//				if(tempLong>0)
				if(total_consumed_reactive_energy_lead>0)
				{
					myenergydata.kvarh_lead += tempLong;
#ifdef TOD_HAS_REACTIVE_ENERGIES					
				  myenergydata.zone_kvarh_lead += tempLong;  // add the kvah consumption for tod
#endif					
					total_consumed_reactive_energy_lead = total_consumed_reactive_energy_lead%(int32_t)(KWH_DIVIDER);
					high_resolution_kvarh_lead_value = (total_consumed_reactive_energy_lead/(int32_t)HIGH_RES_DIVIDER);
				}
			}
			
			if(power_factor_sign==PF_SIGN_LAG_L)
			{
#ifndef BIDIR_METER				
				if(phase_power_active>0)
					temp_power=phase_power_reactive;
				else
					temp_power=-phase_power_reactive;
#else
				temp_power=phase_power_reactive;
#endif
				total_consumed_reactive_energy_lag+=temp_power;
				tempLong = (total_consumed_reactive_energy_lag/(int32_t)(KWH_DIVIDER));
				
//				if(tempLong>0)
				if(total_consumed_reactive_energy_lag>0)
				{
					myenergydata.kvarh_lag += tempLong;
#ifdef TOD_HAS_REACTIVE_ENERGIES
					myenergydata.zone_kvarh_lag += tempLong;  // add the kvah consumption for tod
#endif					
					total_consumed_reactive_energy_lag = total_consumed_reactive_energy_lag%(int32_t)(KWH_DIVIDER);
					high_resolution_kvarh_lag_value = (total_consumed_reactive_energy_lag/(int32_t)HIGH_RES_DIVIDER);
				}
			}
			
			// now for apparent power which is done differently by using sigma(active_power) and sigma(reactive_power)
			// this has to go out of this loop
			// if active and reactive powers have opposite signs then apparent power = active power (CBIP) - Lag only Tariff

#ifdef LAG_ONLY_TARIFF			
	#ifdef BIDIR_METER			
			if(isSameSign(phase_power_active,phase_power_reactive)==1)
			{// only for quadrant 1 and 3 - signs are same
				temp_power = EM_sqrt(EM_pow((float32_t)phase_power_active,2) + EM_pow((float32_t)phase_power_reactive,2));
			}
			else
			{// signs are different
				temp_power = ABS(phase_power_active);
			}
			// the above computation ensures that apparent power >=active power
	#else
// if the reactive power for this phase is lead - then apparent power must equal active power

			if(power_factor_sign==PF_SIGN_LAG_L)
				temp_power = EM_sqrt(EM_pow((float32_t)phase_power_active,2) + EM_pow((float32_t)phase_power_reactive,2));
			else
				temp_power = ABS(phase_power_active);
	#endif
#else
			temp_power = EM_sqrt(EM_pow((float32_t)phase_power_active,2) + EM_pow((float32_t)phase_power_reactive,2));
#endif

			temp_total_apparent_power+=temp_power;				
			total_consumed_apparent_energy+=temp_power;
			tempLong = (total_consumed_apparent_energy/(int32_t)(KWH_DIVIDER));
			
//			if(tempLong>0)
			if(total_consumed_apparent_energy>0)
			{
				myenergydata.kvah += tempLong;
				myenergydata.zone_kvah += tempLong;  // add the kvah consumption for tod
				total_consumed_apparent_energy = total_consumed_apparent_energy%(int32_t)(KWH_DIVIDER);
				high_resolution_kvah_value = (total_consumed_apparent_energy/(int32_t)HIGH_RES_DIVIDER);
			}
			
#ifdef SPLIT_ENERGIES_REQUIRED
			power_factor_sign = (EM_PF_SIGN)getInstantaneousParams(PFSIGNVAL,(EM_LINE)i);
			if(power_factor_sign==PF_SIGN_LEAD_C)
			{
				total_consumed_apparent_energy_lead+=temp_power;
				tempLong = (total_consumed_apparent_energy_lead/(int32_t)(KWH_DIVIDER));
				if(tempLong>0)
				{					
					myenergydata.kvah_lead += tempLong;
					total_consumed_apparent_energy_lead = total_consumed_apparent_energy_lead%(int32_t)(KWH_DIVIDER);
				}
			}
			else
			{
				if(power_factor_sign==PF_SIGN_LAG_L)
				{
					total_consumed_apparent_energy_lag+=temp_power;
					tempLong = (total_consumed_apparent_energy_lag/(int32_t)(KWH_DIVIDER));
					if(tempLong>0)
					{					
						myenergydata.kvah_lag += tempLong;
						total_consumed_apparent_energy_lag = total_consumed_apparent_energy_lag%(int32_t)(KWH_DIVIDER);
					}
				}
			}
#endif
			
#ifdef BIDIR_METER			
		}
#endif		
	}				

// above computation is correct for apparent energy incrementation	
	#ifdef SIX_DECIMALS
		total_active_power = temp_total_active_power/(int32_t)1000;	// power is now in watts
		total_apparent_power = temp_total_apparent_power/(int32_t)1000;
		total_reactive_power = temp_total_reactive_power/(int32_t)1000;
		
		total_active_power_pulsing = temp_total_active_power/(int32_t)100;	// power is now in deci watts
		total_apparent_power_pulsing = temp_total_apparent_power/(int32_t)100;
		total_reactive_power_pulsing = temp_total_reactive_power/(int32_t)100;
		
	#else
		total_active_power = temp_total_active_power/(int32_t)100;	// power is now in watts
		total_apparent_power = temp_total_apparent_power/(int32_t)100;
		total_reactive_power = temp_total_reactive_power/(int32_t)100;
		
		total_active_power_pulsing = temp_total_active_power/(int32_t)10;	// power is now in deci watts
		total_apparent_power_pulsing = temp_total_apparent_power/(int32_t)10;
		total_reactive_power_pulsing = temp_total_reactive_power/(int32_t)10;
	
	#endif


// For Bidir meter sigmaPower is the absolute sum and temp_total_active_power is the algebraic sum with sign 
// For Unidir meter temp_total_active_power is the absolute sum and sigmaPower is the algebraic sum with sign 
#ifdef BIDIR_METER	
	if(ABS(temp_total_active_power)==sigmaPower)
	{// all three phase active energies have the same sign
		mixedPowers=0;
	}
	else
	{// things are mixed up
		mixedPowers=1;
	}
#else
	if(ABS(sigmaPower)==temp_total_active_power)
	{// all three phase active energies have the same sign
		mixedPowers=0;
	}
	else
	{// things are mixed up
		mixedPowers=1;
	}
#endif

	myenergydata.chksum=calcChecksum((uint8_t*)&myenergydata,sizeof(myenergydata)-1);

// system pf computation - start ------------------------------------------------------	
// somehow for system signed pf a different computation is done	


// being commented on 03/Sep/2020
// For Kerala Unidir meter - reactive power signs must not be used for computing total apparent power
// Above, total apparent power has been computed per phase already
// temp_total_active_power and temp_total_reactive_power contain the algebraic sums
#ifndef KERALA_UNIDIR
	temp_total_apparent_power = EM_sqrt(EM_pow((float32_t)temp_total_active_power,2) + EM_pow((float32_t)temp_total_reactive_power,2));
#endif	
	if(ABS(total_active_power)< 2)
	{// total_active_power is in watts
		systemPF=0;
	}
	else
	{
		systemPF = (temp_total_active_power)/(temp_total_apparent_power/1000);
//		systemPF = (temp_total_active_power*(int32_t)1000)/temp_total_apparent_power;	// this causes overflow from 40A onwards 
		if(systemPF>1000)
			systemPF=1000;
			
		if(isSameSign(temp_total_active_power,temp_total_reactive_power)==0)
		{// signs are different
			systemPF*=-1;	// make the sign -ve
		}
	}
	
// system pf computation - end ------------------------------------------------------	

//	if(ABS(total_active_power)!=0)
	if(ABS(total_active_power)<3)
	{// here there is no power (less than 3 watts) - being treated as zero power
#ifdef BIDIR_METER	
		if((myenergydata.kwh!=total_energy_lastSaved)||(myenergydata.kwh_export!=total_energy_lastSaved_export))
			update_energies_stored();
#else
		if(myenergydata.kwh!=total_energy_lastSaved)
			update_energies_stored();
#endif
	}
	else
	{// here total active power is >3 i.e there is some load on the meter
#ifdef BIDIR_METER	
		if(total_active_power>0)
		{// meter is in import mode
			if((myenergydata.kwh-total_energy_lastSaved)>=KWH_INCREMENT)
				update_energies_stored();
		}
		else
		{// meter is in export mode
			if((myenergydata.kwh_export-total_energy_lastSaved_export)>=KWH_INCREMENT)
				update_energies_stored();
		}
#else
// this is for unidirectional meters
		if((myenergydata.kwh-total_energy_lastSaved)>=KWH_INCREMENT)
			update_energies_stored();
#endif
	}
/*
	if(total_active_power>0)
	{	
		if(total_active_power!=0){
			if((myenergydata.kwh-total_energy_lastSaved)>=KWH_INCREMENT)
				update_energies_stored();
		}
		else{// here total active power is 0
			if(myenergydata.kwh!=total_energy_lastSaved)
				update_energies_stored();
		}
	}
#ifdef BIDIR_METER	
	else
	{// here total active power is -ve
		if(ABS(total_active_power)!=0){
			if((myenergydata.kwh_export-total_energy_lastSaved_export)>=KWH_INCREMENT)
				update_energies_stored();
		}
		else{// here total active power is 0
			if(myenergydata.kwh_export!=total_energy_lastSaved_export)
				update_energies_stored();
		}
	}
#endif
*/
}
