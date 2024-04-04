#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_vbatt.h"
#include "r_cg_userdefine.h"
#include "emeter3_structs.h"
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

void R_PORT_Create(void)
{
	/*  3P4W PROTOTYPE METER PINS MAPPING
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *              7               6               5               4               3               2               1               0
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P0  MCU     TXD0            RXD0            PORT_IO         PORT_O          PORT_O          PORT_PW                         
     *      NAME    NC  	   				NC   						FEA_LED0	    	FEA_LED1	    	VCC-1	    			FEA_SW1
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P1  MCU     SEG11           SEG10           SEG9            SEG8            SEG7            SEG6            SEG5            SEG4
     *      NAME    SEG11           SEG10           SEG9            SEG8            SEG7            SEG6            SEG5            SEG4
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P2  MCU                                     ANI5            ANI4            ANI3            ANI2/EXLVD      AVREFM          AVREFP
     *      NAME                                    ADC2            ADC1            ADC0            EXLVD           NC              NC
     *--------------------------------------------------------------------------------------------------------------------------------------------      
     *  P3  MCU     PORT            PORT_PW         PORT_O          PORT_O          PORT_PW         PORT_O          PORT_O          PORT_O
     *      NAME    VCC-3           NC	            WP1        	    WP2		    			WP3             BT-CHK		    	NC	            NC
     *--------------------------------------------------------------------------------------------------------------------------------------------      
     *  P4  MCU                                                                     PORT_PW         PORT            PORT            E1
     *      NAME                                                                    NC					    NC              NC              TOOL0*
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P5  MCU     PORT_PW         TXD2            RXD2            PORT            PORT_PW         PORT_O          PORT_IO         PORT
     *      NAME    VCC-2           FEA_IR_TXD      FEA_IR_RXD      HALLSENSOR2     NC              NC              NC  						HALLSENSOR4
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P6  MCU                                                                                     PORT_O          SDAA0           SCLA0
     *      NAME                                                                                    SPARE           FEA_IIC0_SDA    FEA_IIC0_SCL
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P7  MCU     SEG23           SEG22           SEG21           SEG20           SEG19           SEG18           SEG17           SEG16
     *      NAME    SEG23           SEG22           SEG21           SEG20           SEG19           SEG18           SEG17           SEG16
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P8  MCU                                     PORT_PW         PORT            SEG15           SEG14           SEG13           SEG12
     *      NAME                                    TXD3            RXD3            SEG15           SEG14           SEG13           SEG12   
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P12 MCU     CAPH            CAPL            PORT_I          XT2             XT1             X2/EXCLK        X1
     *      NAME    CAPH            CAPL            HALL_SENSOR1    XT2             XT1             Pulled Down/NA  Pulled Down/NA
     *--------------------------------------------------------------------------------------------------------------------------------------------      
     *  P13 MCU     INTP0
     *      NAME    FEA_SW3
     *--------------------------------------------------------------------------------------------------------------------------------------------
     *  P15 MCU                                                                                     RTCIC2          RTCIC1          RTCOUT
     *      NAME                                                                                    FEA_SW2         FEA_CASE        MCU_1HZ/Pulled Down
     *--------------------------------------------------------------------------------------------------------------------------------------------
    */
		
#ifdef PCB_2_5
	PIOR0=0;	// No I/O redirection RTCOUT is at P150
#else
	PIOR0|=0x08;	// PIOR03 = 1 : RTCOUT is redirected to P62
#endif
		
    /* Setting for not used segments EVB */
    PFSEG0 = _80_PFSEG07_SEG | _40_PFSEG06_SEG | _20_PFSEG05_SEG | _10_PFSEG04_SEG;
    PFSEG1 = _80_PFSEG15_SEG | _40_PFSEG14_SEG | _20_PFSEG13_SEG | _10_PFSEG12_SEG | _08_PFSEG11_SEG | 
             _04_PFSEG10_SEG | _02_PFSEG09_SEG | _01_PFSEG08_SEG;
	
    PFSEG2 = _80_PFSEG23_SEG | _40_PFSEG22_SEG | _20_PFSEG21_SEG | _10_PFSEG20_SEG |
             _08_PFSEG19_SEG | _04_PFSEG18_SEG | _02_PFSEG17_SEG | _01_PFSEG16_SEG; 
    PFSEG3 = _00_PFSEG31_PORT | _00_PFSEG30_PORT | _00_PFSEG29_PORT | _00_PFSEG28_PORT |
             _00_PFSEG27_PORT | _00_PFSEG26_PORT | _00_PFSEG25_PORT | _00_PFSEG24_PORT; 
    PFSEG4 = _00_PFSEG39_PORT | _00_PFSEG38_PORT | _00_PFSEG37_PORT | _00_PFSEG36_PORT |
             _00_PFSEG35_PORT | _00_PFSEG34_PORT | _00_PFSEG33_PORT | _00_PFSEG32_PORT;
    PFSEG5 = _00_PFSEG41_PORT | _00_PFSEG40_PORT;

    /* Set port value */
#ifdef PCB_2_5
		P0 = _00_Pn7_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _20_Pn5_OUTPUT_1 | _10_Pn4_OUTPUT_1 | 
         _08_Pn3_OUTPUT_1 | _00_Pn2_OUTPUT_0; // P02 is connected to switch which is input hence no output value required.
				 
    P3 = _80_Pn7_OUTPUT_1 | _00_Pn6_OUTPUT_0 | _00_Pn5_OUTPUT_0 | _00_Pn4_OUTPUT_0 | 
         _00_Pn3_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;	 
				 
    P4 = _00_Pn3_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0;	// P4 has only three usable bits 1,2,3

    P5 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1 | _00_Pn5_OUTPUT_0 | _00_Pn4_OUTPUT_0 | 
         _08_Pn3_OUTPUT_1 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;
				 
    P6 = 0x01;	// P62 =0,SDA=0,SCL=1  - P62=0 (not used) - 0000 0001
		
#else
		P0 = _80_Pn7_OUTPUT_1 | _00_Pn6_OUTPUT_0 | _20_Pn5_OUTPUT_1 | _10_Pn4_OUTPUT_1 | 
         _08_Pn3_OUTPUT_1 | _00_Pn2_OUTPUT_0; // P02 is connected to switch which is input hence no output value required.
				 
    P3 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1 | _00_Pn5_OUTPUT_0 | _00_Pn4_OUTPUT_0 | 
         _00_Pn3_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;	 
				 
    P4 = _00_Pn3_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0;	// P4 has only three usable bits 1,2,3
		
    P5 = _80_Pn7_OUTPUT_1 | _40_Pn6_OUTPUT_1 | _00_Pn5_OUTPUT_0 | _00_Pn4_OUTPUT_0 | 
         _00_Pn3_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn0_OUTPUT_0;
				 
    P6 = 0x01;	// TP10 =0,SDA=0,SCL=1 
		
#endif

		
				 
    P8 = _20_Pn5_OUTPUT_1;
		
#ifndef RTC_OUT_DISABLED		
	#ifdef PCB_2_5		
			P15 = 0x00;	// P15.0 is used as RTCOUT 
	#else
			P15 = 0x01;	// P15.0 is to be made high - RTC-BKUP
// in this version RTCOUT is P62			
	#endif
#endif

	ISCLCD = _02_ISCVL3_INPUT_EFFECTIVE;	// tis make P127 digital valid

#ifdef PCB_2_5
		P12 = 0x20;	// P12.5 = 1; Battery supply	0010 0000
#else
		P12 = 0x00;
#endif




    /* Set port input/output mode */

    ADPC = _00_ADPC_DI_OFF;
    
    ISCLCD = _02_ISCVL3_INPUT_EFFECTIVE | _00_ISCCAP_INPUT_INEFFECTIVE;	// this looks ok
    
    PM0 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_00_PMn5_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT |
          _00_PMn3_MODE_OUTPUT | _04_PMn2_MODE_INPUT;
					
    PM1 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_00_PMn5_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT |
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;

//    PM3 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_00_PMn5_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT |
//          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;
// P30 is now used as digital input (PROD mode selector)
    PM3 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_00_PMn5_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT |
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _01_PMn0_MODE_INPUT;
					
#ifdef PCB_2_5
    PM4 = _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _01_PMn0_MODE_INPUT;
#else
    PM4 = _08_PMn3_MODE_INPUT | _04_PMn2_MODE_INPUT | _00_PMn1_MODE_OUTPUT | _01_PMn0_MODE_INPUT;
#endif

#ifdef PCB_2_5
    PM5 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_20_PMn5_MODE_INPUT | _10_PMn4_MODE_INPUT |
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _01_PMn0_MODE_INPUT;
#else
    PM5 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_20_PMn5_MODE_INPUT | _10_PMn4_MODE_INPUT |
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _01_PMn0_MODE_INPUT;
#endif
					
    PM6 = 0xFA;	// PM62=Output,61=Input,60=Output - 1111 1010
    
    PM7 = _00_PMn7_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT |_00_PMn5_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT |
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;
    PM8 = _00_PMn5_MODE_OUTPUT | _10_PMn4_MODE_INPUT | 
          _00_PMn3_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn0_MODE_OUTPUT;
#ifdef PCB_2_5
    PM12 = 0x1E;		// P12.5(Battery Backup Supply is output),XT2,XT1,X2,X1 are inputs - 0001 1110
#else
    PM12 = 0x3E;		// P12.5(Hall sensor),XT2,XT1,X2,X1 are inputs - 0011 1110
#endif

    PM15 = _04_PMn2_MODE_INPUT | _02_PMn1_MODE_INPUT;	// P15.0 is output in both versions of PCB
    /* Set port pull-up */
    PU15 = _00_PUn2_PULLUP_OFF | _00_PUn1_PULLUP_OFF | _00_PUn0_PULLUP_OFF;
		
#ifdef PCB_2_5
		PU4 = _00_PUn3_PULLUP_OFF | _00_PUn2_PULLUP_OFF;
		PU5 = _10_PUn4_PULLUP_ON | _01_PUn0_PULLUP_ON;
		PU12 = _00_PUn5_PULLUP_OFF;
#else
		PU4 = _08_PUn3_PULLUP_ON | _04_PUn2_PULLUP_ON;
		PU5 = _10_PUn4_PULLUP_ON | _01_PUn0_PULLUP_ON;
		PU12 = _20_PUn5_PULLUP_ON; 
#endif		
}

/* Start user code for adding. Do not edit comment generated here */
/***********************************************************************************************************************
* Function Name: R_PORT_StopCreate
* Description  : This function turn off unnesccesary port pin before stopping to conserve power
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_StopCreate(void)
{
    /*  Turn off un necessary port before sleeping 
    *   Only keep below items:
    *       1. AUX_OUT (for Switch and Case Open)
    *       2. 
    *   Magnetics not keep due to high current consumption:
    */
	
    P0 = 0x00;	// P03 is also off - cal leds power
    P1 = 0x00;
    P2 = 0x00;
		P3 = 0x80;	// 1000 0000 : P37(VCC3) high and P36(Not used) Low and P31(Not used) Low
//    P3 = 0x00;	// 0000 0000 : P37(VCC3) high and P36(VCC4) Low
		
    P4 = 0x00;
//    P5 = 0xE8;	// P57 (VCC2),IR_TXD(P56),IR_RXD(P55) to optical port are High   -  1110 1000, P53 is high (AUX_OUT) this is for switches

#ifdef PCB_2_5
		P5 = 0x00;	// P53 (VCC5)=0
#else
		P5 = 0x08;	// P57 (VCC2)=0,IR_TXD(P56)=0,IR_RXD(P55)=0,HallSensor,AUX_OUT(P53)=1  -  0000 1000, (AUX_OUT)is for switches
//    P5 = 0x00;	// P57 (VCC2)=0,IR_TXD(P56)=0,IR_RXD(P55)=0,HallSensor,AUX_OUT(P53)=0  -  0000 0000, (AUX_OUT)is OFF for switches
#endif

//		P6 = 0x05;	// 0000 0101 : WP P6.2=1,SDA P6.1=0,SCL P6.0=1 - This matches normal startup

#ifdef PCB_2_5
		P6 = 0x00;	// 0000 0000 : P6.2=0(not used),SDA P6.1=0,SCL P6.0=0
#else
		P6 = 0x00;	// 0000 0000 : WP P6.2=0,SDA P6.1=0,SCL P6.0=0
#endif
		
		
    P7 = 0x00;
//    P8 = 0x30;	// P8.5(TXD3) high and P8.4(RXD3) high - 0011 0000 
    P8 = 0x00;	// P8.5(TXD3) - 0 and P8.4(RXD3) - 0 : 0000 0000 
		
	ISCLCD = _02_ISCVL3_INPUT_EFFECTIVE;	// tis make P127 digital valid
#ifdef PCB_2_5
		P12 = 0x20;	// P12.5 = 1; Battery supply
#else
		P12 = 0x00;
#endif

#ifndef RTC_OUT_DISABLED		
	#ifdef PCB_2_5		
		P15=0x00;	// P15.0 is used for RTCOUT
	#else
		P15=0x01;	// P15.0 (used as RTC-BKUP) will be made 0 only during rtc battery test
	#endif
#endif

    PM0 = 0x04;	// P0.6(Output - Not used) P0.2(SW1) is input - 0000 0100
//    PM1 = 0x08;	// P1.3 is input - 0000 1000
    PM1 = 0x00;
    PM2 = 0x00;
//    PM3 = 0x00;
    PM3 = 0x01;	// P3.0 is input
		
#ifdef PCB_2_5		
    PM4 = 0x01;	// p4.0 is tool0 - input -, P4.3 (Not used output) 0000 0001 
#else
    PM4 = 0x0D;	// p4.0 is tool0 - input -, P4.3 and P4.2 are inputs 0000 1101 
#endif

#ifdef PCB_2_5		
    PM5 |= 0x31;	// p5.5(IR_rxd and P5.4 - Hall sensor2 are input - 0011 0001
#else
    PM5 |= 0x31;	// p5.5(IR_rxd and P5.4,P5.0 - Hall sensor2 are input - 0011 0001
#endif
//    PM6 = 0xFA;		// WP P6.2=Output,SDA P6.1=Input,SCL P6.0=Output  - 1111 1010
		PM6 = 0xFB;	// 1111 1011 - P6.2  is Output(Not used), SDA(Inp), SCL(Inp)

		PM7 = 0x00;
    PM8 |= 0x10;		// p8.4(RXD3 is input) - 0001 0000
		
#ifdef PCB_2_5		
    PM12 = 0x1E;		// P12.5(Battery Backup Supply is output),XT2,XT1,X2,X1 are inputs - 0001 1110
#else
    PM12 = 0x3E;		// P12.5(Hall sensor),XT2,XT1,X2,X1 are inputs - 0011 1110
#endif

    PM15 = 0x06;	// P15.1 and P15.2 are input - 0000 0110

		PU4 = _00_PUn3_PULLUP_OFF | _00_PUn2_PULLUP_OFF;
		PU5 = _00_PUn4_PULLUP_OFF | _00_PUn0_PULLUP_OFF;
		PU12 = _00_PUn5_PULLUP_OFF; 
		
}
/* End user code. Do not edit comment generated here */
