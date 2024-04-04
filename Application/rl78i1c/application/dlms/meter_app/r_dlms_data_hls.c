/******************************************************************************
* File Name    : r_dlms_data_hls.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   :
* H/W Platform : Unified Energy Meter Platform
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12.03.2014
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Drivers */

/* DLMS */
#include "r_dlms_data.h"				/* DLMS Data Definitions */
#include "r_dlms_obis.h"				/* DLMS OBIS Definitions */
#include "r_dlms_obis_ic.h"				/* DLMS OBIS IC Definitions */
#include "DLMS_User.h"					/* DLMS User Definitions */

/* HLS */
#include "r_dlms_data_hls.h"			/* DLMS Data High level security Definitions */

/* Start user code */
#if AES_ENABLE

#include "r_aes.h"						/* AES 128-bit (ECB Mode) */

#endif
/* End user code*/


/* Standard Lib */
#include <string.h>						/* String Standard Lib */

/******************************************************************************
Macro definitions
******************************************************************************/
#define AES_MAX_BLOCK					(4)	/* 4 block (4 x 16 bytes) = 64 bytes */
/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
#if AES_ENABLE 
///******************************************************************************
//Private global variables and functions
//******************************************************************************/
//uint32_t ekey128[AES_EKEY128_LEN];
//
//uint8_t key128[AES_KEY128_LEN];
//
//uint8_t pdat128[AES_MAX_BLOCK * AES_BLOCK_SIZE];
//
///******************************************************************************
//* Function Name : R_OBIS_Aes_Keysch
//* Interface     : void R_OBIS_Aes_Keysch(
//*               :     Unsigned8              *in_key,
//*               : );
//* Description   : I/F wrapper for AES 128-bit Key Schedule
//* Arguments     : Unsigned8            *in_key,			[In]  key area
//* Function Calls: TBD
//* Return Value  : void
//******************************************************************************/
//void R_OBIS_Aes_Keysch(Unsigned8 *in_key)
//{
//	Unsigned16	length = 0;
//	/* TODO: put the custom method HERE */
//
//	/* Init key128 all 0 */
//	memset(key128, 0, AES_KEY128_LEN);
//
//	/* Update key128 */
//	length = strlen((void *)in_key);
//	length = (length > AES_KEY128_LEN) ? AES_KEY128_LEN : length;
//	memcpy(key128, in_key, length);
//
//	/* Update to ekey128 */
//	//R_Aes_128_Keysch(key128, ekey128);
//}
//
///******************************************************************************
//* Function Name : R_OBIS_Aes_Challenge_generate
//* Interface     : void R_OBIS_Aes_Challenge_generate(
//*               :     Unsigned8            *in_ptext,
//*               : );
//* Description   : I/F wrapper to generate Challenge for AES
//* Arguments     : Unsigned8                 *in_ptext,			[In]  plain text area
//* Function Calls: TBD
//* Return Value  : Unsigned16: number of block
//******************************************************************************/
//Unsigned16 R_OBIS_Aes_Challenge_generate(Unsigned8 *in_ptext)
//{
//	Unsigned16	length = 0;
//	Unsigned8	i, j;
//	/* TODO: put the custom method HERE */
//
//	
//
//	//length = strlen((void *)in_ptext);
//	//for India is 16
//	length= 16;
//
//	/* Max support is AES_MAX_BLOCK * AES_BLOCK_SIZE  */
//	length = (length > (AES_MAX_BLOCK * AES_BLOCK_SIZE)) ? (AES_MAX_BLOCK * AES_BLOCK_SIZE) : length;
//	i = (Unsigned8)(length % AES_BLOCK_SIZE);
//	j = (i == 0) ? 0 : (AES_BLOCK_SIZE - i);
//
//	/* Update to pdat128 */
//	/* TODO: put the custom method HERE */
//	/* Example: use padding mode PKCS7 */
//	memset(pdat128, j, AES_MAX_BLOCK * AES_BLOCK_SIZE);
//	memcpy(pdat128, in_ptext, length);
//
//	return (length + j);
//}

/******************************************************************************
* Function Name : R_OBIS_Aes_Ecbenc
* Interface     : Unsigned16 R_OBIS_Aes_Ecbenc(
*               :     Unsigned8            *in_ptext,
*               :     Unsigned8           *out_ctext,
*               :     Unsigned8              *in_key,
*               : );
* Description   : I/F wrapper for AES 128-bit Encryption Function (ECB Mode)
* Arguments     : Unsigned8            *in_ptext,			[In]  plain text area
*               : Unsigned8           *out_ctext,			[Out] cipher text area
*               : Unsigned8              *in_key,			[In]  expanded key area
* Function Calls: TBD
* Return Value  : Unsigned16: size of cipher text
******************************************************************************/
Unsigned16 R_OBIS_Aes_Ecbenc(Unsigned8 *in_ptext, Unsigned8 *out_ctext, Unsigned8 *in_key)
{
	Unsigned16	length = 16;
	
	AES128_ECB_encrypt((uint8_t near*)in_ptext, (uint8_t near*)in_key,(uint8_t near*)out_ctext);
//	Unsigned16	length = 0;
//	/******************************************************************************
//	 * Generate cipher key from in_key 
//	******************************************************************************/
//	R_OBIS_Aes_Keysch(in_key);
//
//	/******************************************************************************
//	 * Generate challenge from in_ptext 
//	******************************************************************************/
//	length = R_OBIS_Aes_Challenge_generate(in_ptext);
//
//	/******************************************************************************
//	 * TODO: put the custom method to encrypt data HERE
//	******************************************************************************/
//	R_Aes_128_Ecbenc((uint8_t near*)pdat128, (uint8_t near*)out_ctext, ekey128, (length/AES_BLOCK_SIZE));

	return length;
}
#endif