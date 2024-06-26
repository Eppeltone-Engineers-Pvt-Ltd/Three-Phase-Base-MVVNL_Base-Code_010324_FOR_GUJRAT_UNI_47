/***********************************************************************************************************************
* File Name    : r_dscl_iirbiquad_config.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : ADC Wrapper for RL78/I1C Platform
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "iodefine.h"
#include "r_dscl_iirbiquad_config.h"   /* Wrapper Shared Configuration */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
__near const int32_t iir32_coeff_1[] = {
    /* Fs: 3906Hz coefficients */
    138568530   , -230224206    , 138568530     , -(-1737342189)   , -(710513863 ),
    667196326   , -1316792950   , 667196326     , -(-2087467922)   , -(1033245651),
    618640647   , -1207002848   , 618641184     , -(-1965432869)   , -(918993079 ),
    
};

__near const int32_t iir32_coeff_2[] = {
    /* Fs: 1302Hz coefficients */
    165914587   , -16728898     , 165914587     , -(-1052127401)   , -(293485853), 
    653532961   , -1147722636   , 653532961     , -(-1853493137)   , -(956499954), 
    566624298   , -888403248    , 566624298     , -(-1526538751)   , -(673536771), 
};

/******************************************************************************
Private global variables and functions
******************************************************************************/
