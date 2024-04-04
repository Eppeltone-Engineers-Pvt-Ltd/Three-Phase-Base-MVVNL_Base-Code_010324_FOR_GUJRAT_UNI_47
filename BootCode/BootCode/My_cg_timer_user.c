#include "my_macrodriver.h"
#include "My_cg_timer.h"
#include "my_userdefine.h"

unsigned char counter ;

extern unsigned int flg1,flg,dly;
_Bool DelayTimerUnderFlowFlag;
 void  r_tau0_channel0_interrupt(void)
{
    DelayTimerUnderFlowFlag = 1;
    if(flg1==1)
    {
	    
    }
    else
    {
    R_TAU0_Channel0_Stop();
    }
    counter ++;
    
}
