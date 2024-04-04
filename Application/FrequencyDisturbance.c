// this file computes the variance in frequency and the variance can be used to identify chopped ac in neutral tamper

#include "typedef.h"            // located right at top of tree below iodefine.h
#include "em_core.h"
#include "emeter3_structs.h"

#ifdef FREQ_DIST_CHECK
void addFreshValuesToArrays(void);
int32_t computeVariance_R(void);
int32_t computeVariance_Y(void);
int32_t computeVariance_B(void);


int16_t frequencyArr_R[16];
int16_t frequencyArr_Y[16];
int16_t frequencyArr_B[16];
int8_t freqArrIndex;


void clearFrequencyArray(void)
{
	freqArrIndex=0;	// this has the effect of clearing the frequencyArrays
}
void initFreqDisturbance(void)
{
	int8_t i;
	for(i=0;i<16;i++)
	{
		frequencyArr_R[i]=500;
		frequencyArr_Y[i]=500;
		frequencyArr_B[i]=500;
	}
	freqArrIndex=0;
}

int32_t computeVariance_R(void)
{
	int32_t accumulator;
	int32_t diffsquared;
	int16_t prevValue;
	int16_t diff;
	int8_t i;
	
	accumulator=0;
	
	prevValue = frequencyArr_R[15];
	for(i=0;i<16;i++)
	{
		diff = frequencyArr_R[i] - prevValue;
		diffsquared = (int32_t)diff*(int32_t)diff;
		accumulator +=diffsquared;
		prevValue = frequencyArr_R[i];
	}
	
	return (accumulator/(int32_t)i);
}

int32_t computeVariance_Y(void)
{
	int32_t accumulator;
	int32_t diffsquared;
	int16_t prevValue;
	int16_t diff;
	int8_t i;
	
	accumulator=0;
	
	prevValue = frequencyArr_Y[15];
	for(i=0;i<16;i++)
	{
		diff = frequencyArr_Y[i] - prevValue;
		diffsquared = (int32_t)diff*(int32_t)diff;
		accumulator +=diffsquared;
		prevValue = frequencyArr_Y[i];
	}
	
	return (accumulator/(int32_t)16);
}

int32_t computeVariance_B(void)
{
	int32_t accumulator;
	int32_t diffsquared;
	int16_t prevValue;
	int16_t diff;
	int8_t i;
	
	accumulator=0;
	
	prevValue = frequencyArr_B[15];
	for(i=0;i<16;i++)
	{
		diff = frequencyArr_B[i] - prevValue;
		diffsquared = (int32_t)diff*(int32_t)diff;
		accumulator +=diffsquared;
		prevValue = frequencyArr_B[i];
	}
	
	return (accumulator/(int32_t)16);
}

void addFreshValuesToArrays(void)
{
	frequencyArr_R[freqArrIndex]=EM_GetLineFrequency(LINE_PHASE_R)*10;
	frequencyArr_Y[freqArrIndex]=EM_GetLineFrequency(LINE_PHASE_Y)*10;
	frequencyArr_B[freqArrIndex]=EM_GetLineFrequency(LINE_PHASE_B)*10;

	freqArrIndex++;
	if(freqArrIndex>=16)
		freqArrIndex=0;
}
#endif

// This function may be called every second
int8_t checkFrequencyDisturbance(void)
{
	int8_t retval = 0;	// there is no frequency disturbance - 1 means frequency disturbance
#ifdef FREQ_DIST_CHECK	
	int32_t variance_R=0;
	int32_t variance_Y=0;
	int32_t variance_B=0;
	
	addFreshValuesToArrays();
	
	if(freqArrIndex==0)
	{
		variance_R = computeVariance_R();
		variance_Y = computeVariance_Y();
		variance_B = computeVariance_B();
	}
	
//	if((variance_R>200)||(variance_Y>200)||(variance_B>200))
	if((variance_R>50)||(variance_Y>50)||(variance_B>50))
		retval=1;
#endif	
	return retval;
}