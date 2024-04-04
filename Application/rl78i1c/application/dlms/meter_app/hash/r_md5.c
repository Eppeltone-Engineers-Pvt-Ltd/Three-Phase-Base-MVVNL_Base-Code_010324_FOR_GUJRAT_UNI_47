/******************************************************************************	
* File Name    : r_md5.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : 
* H/W Platform : Unified Energy Meter Platform
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 08.09.2013 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_cryptogram.h"	/* MD5 Transform */
#include "r_md5.h"			/* MD5 API definition */

#include <string.h>			/* Standard String Lib */

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name : R_Md5_HashDigest
* Interface     : void R_Md5_HashDigest(uint8_t *msg, uint8_t *hash, uint16_t msg_length)
* Description   : Create MD5 hash disgest value (16 bytes)
* Arguments     : uint8_t * msg      : Area that store input message
*               : uint8_t * hash     : Area that store hash value
*               : uint16_t msg_length: Length of the message
* Return Value  : None
******************************************************************************/
void R_Md5_HashDigest(uint8_t *msg, uint8_t *hash, uint16_t msg_length)
{
	uint8_t		last_block[64];	
	uint16_t	length      = msg_length;
	uint32_t 	*p_hash_u32 = (uint32_t *)hash;
	
	/* Params check */
	if (msg == NULL ||
		msg_length == 0 ||
		hash == NULL)
	{
		return;
	}
 
	/* Initialize variables - simple count in nibbles */
	*(p_hash_u32+0) = 0x67452301;
	*(p_hash_u32+1) = 0xefcdab89;
	*(p_hash_u32+2) = 0x98badcfe;
	*(p_hash_u32+3) = 0x10325476;
 
	/* hash all ahead block */
	while (length >= 64)
	{
		R_Md5_Hash(msg, hash, 1);
		
		msg    += 64;
		length -= 64;
	}

	/*
	 * Pre-processing:
	 * append "1" bit to message	
	 * append "0" bits until message length in bits = 448 (mod 512)
	 * append length mod (2^64) to message
	 */
	
	/* hash last blocks */
	memset(last_block, 0, 64);
	if (length > 0)
	{
		memcpy(last_block, msg, length);
	}
	last_block[length] = 0x80;	/* append the "1" bit; most significant bit is "first" */
	if (length >= 56)
	{
		/* hash 2nd last block without the size first */
		R_Md5_Hash(last_block, hash, 1);
		memset(last_block, 0, 64);	
	}
	/*
	 * hash last block (that store the size)
	 *
	 * append the len in bits at the end of the buffer.
	 * because the msg_length limited in uint16_t, then
	 * just need 4 bytes encoding for the length.
	 */
	p_hash_u32  = (uint32_t *)((uint8_t *)last_block + 56);
	*p_hash_u32 = ((uint32_t)msg_length) * 8;

	/* hash & update the last block */
	R_Md5_Hash(last_block, hash, 1);
}
