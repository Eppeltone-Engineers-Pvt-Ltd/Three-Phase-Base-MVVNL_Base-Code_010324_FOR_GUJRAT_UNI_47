// trig Functions implementation
// arcCos and cos implemented using lookup table
#include "typedef.h"            // located right at top of tree below iodefine.h

// private function

int8_t entryMatched(int16_t pf, int16_t index);

const int16_t cosTable[91]=
	{10000,
	9998,
	9993,
	9986,
	9975,
	9961,
	9945,
	9925,
	9902,
	9876,
	9848,
	9816,
	9781,
	9743,
	9702,
	9659,
	9612,
	9563,
	9510,
	9455,
	9396,
	9335,
	9271,
	9205,
	9135,
	9063,
	8987,
	8910,
	8829,
	8746,
	8660,
	8571,
	8480,
	8386,
	8290,
	8191,
	8090,
	7986,
	7880,
	7771,
	7660,
	7547,
	7431,
	7313,
	7193,
	7071,
	6946,
	6819,
	6691,
	6560,
	6427,
	6293,
	6156,
	6018,
	5877,
	5735,
	5591,
	5446,
	5299,
	5150,
	4999,
	4848,
	4694,
	4539,
	4383,
	4226,
	4067,
	3907,
	3746,
	3583,
	3420,
	3255,
	3090,
	2923,
	2756,
	2588,
	2419,
	2249,
	2079,
	1908,
	1736,
	1564,
	1391,
	1218,
	1045,
	871,
	697,
	523,
	348,
	174,
	0
	};
	

int8_t entryMatched(int16_t pf, int16_t index)
{
	int8_t retval;
	
	if((pf<cosTable[index-1])&&(pf>cosTable[index+1]))
		retval=1;	// matched
	else
		retval=0;	// not matched
	return retval;
}
	
int16_t findArcCos(int16_t signedPF, int16_t seedIndex)
{// the pf value is the float value*10000 - this function returns the angle
	int16_t searchIndex;
	int16_t pf;	// this is the absolute value
	int16_t bottom;
	int16_t top;
	int8_t done=0;
	
	if(signedPF<0)
		pf=-1*signedPF;
	else
		pf=signedPF;

	searchIndex = seedIndex;

	if(pf>9993)
	{
		searchIndex = 0;
		done=1;
	}
	else
	{
		if(pf<174)
		{
			searchIndex=90;
			done=1;
		}
	}
	
	bottom=1;	// this was 0 before
	top=90;

// we must terminate the search if search index becomes 1 or 90	
	while(done==0)	
	{
		if(1==entryMatched(pf, searchIndex))
			done=1;
		else
		{
			if(pf>cosTable[searchIndex])
			{// target is in the lower half
				top = searchIndex;	// bottom remains unchanged
				searchIndex = ((top+bottom)/2);
				if(searchIndex<=1)
				{
					searchIndex=1;	// this is to ensure that in entryMatched, we never go below [0] which is the first entry
					done=1;
				}	
			}
			else
			{// target is in the upper half
				bottom = searchIndex;	// top remains unchanged
				searchIndex = ((top+bottom)/2);
				if(searchIndex>=90)
				{
					searchIndex=90;	// this is to ensure that in entryMatched, we never go beyond [91] which is the last entry
					done=1;
				}	
			}
		}
	};

	
// we are	dealing in positive numbers only
//	if(signedPF<0)
//		searchIndex=180-searchIndex;
		
	return searchIndex;
}

int16_t findCos(int16_t angle)
{// angle must lie between 0 and 90
	int16_t pf;
	if(angle<0)
		angle*=-1;
		
	pf=cosTable[angle];
	return pf;
}