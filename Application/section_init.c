/***********************************************************************************************************************
* File Name    : section_init.c
* Version      : 
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* Description  : Section initialization routine
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct 
{
    char __near *ram_sectop;        /*Start address of section                  */
    char __near *ram_secend;        /*End address + 1 of section                */
} bsec_t;

typedef struct 
{
    char __far *rom_sectop;         /*Start address of copy source section      */
    char __far *rom_secend;         /*End address + 1 of copy source section    */
    char __near *ram_sectop;        /*Start address of copy destination section */
} dsec_t;

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/* Not use the bsec_table: since all RAM area is init by cstart before */
#if 0
const bsec_t bsec_table[] = 
{
    {
        (char __near *)__sectop(".bss"),
        (char __near *)__secend(".bss")
    },
    {
        (char __near *)__sectop(".sbss"),
        (char __near *)__secend(".sbss")
    },
    {
        (char __near *)__sectop("ADC_Data_n"),
        (char __near *)__secend("ADC_Data_n")
    },
#ifdef __DEBUG    
    {
        (char __near *)__sectop("EMMetroBss_n"),
        (char __near *)__secend("EMMetroBss_n")
    },
    {
        (char __near *)__sectop("WrpAdcBss_n"),
        (char __near *)__secend("WrpAdcBss_n")
    }
#endif
};
#define BSEC_MAX                        (sizeof(bsec_table) / sizeof(bsec_t))   /*Number of BSS sections to be initialized to 0*/
#endif

const dsec_t dsec_table[] = 
{
    {
        __sectop(".data"),
        __secend(".data"),
        (char __near *)__sectop(".dataR")
    },
    {
        __sectop(".sdata"),
        __secend(".sdata"),
        (char __near *)__sectop(".sdataR")
    },
#ifdef __DEBUG 
    {
        __sectop("EMMetroData_n"),
        __secend("EMMetroData_n"),
        (char __near *)__sectop("EMMetroDataR_n")
    },
    {
        __sectop("WrpAdcData_n"),
        __secend("WrpAdcData_n"),
        (char __near *)__sectop("WrpAdcDataR_n")
    },
#endif
};

#define DSEC_MAX                        (sizeof(dsec_table) / sizeof(dsec_t))   /*Number of DATA sections to be copied*/

/***********************************************************************************************************************
* Function Name: void section_init(void)
* Description  : Initialize memory sections:
*              : . ROMization
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void section_init(void)
{
    unsigned int i;
    char __far *rom_p;
    char __near *ram_p;
        
    /* Data Section Init */
    for (i = 0; i < DSEC_MAX; i++) 
    {
        rom_p = dsec_table[i].rom_sectop;
        ram_p = dsec_table[i].ram_sectop;
        for ( ; rom_p != dsec_table[i].rom_secend; rom_p++, ram_p++) 
        {
            *ram_p = *rom_p;
        }
    }
}
