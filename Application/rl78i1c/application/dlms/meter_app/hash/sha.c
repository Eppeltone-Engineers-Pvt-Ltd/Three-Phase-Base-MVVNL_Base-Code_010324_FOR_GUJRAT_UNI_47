/******************************************************************************	
* File Name    : sha.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   : CCRL
* H/W Platform : Unified Energy Meter Platform
* Description  : SHA Library implemenation file.
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : DD.MM.2015 
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/


#include "r_stdint1.h"
#include "string.h"
#include "r_sha.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define CHUNK_SIZE_BYTE		(64)
#define CHUNK_SIZE_INT32	(16)
#define CHUNK_SIZE_BIT		(512)

#define NUM_OF_ROUND		(80)
#define DIGEST_SIZE_BYTE	(20)
#define DIGEST_SIZE_INT32	(5)
#define HASH_ELEMENT_NUM	(sizeof(st_SHA1_Digest_Data)/sizeof(uint32_t))

#define COPY_32_MSB_LSB(y, x)			{	(*((uint8_t *)&y    )) = (*((uint8_t *)&x + 3));\
											(*((uint8_t *)&y + 1)) = (*((uint8_t *)&x + 2));\
											(*((uint8_t *)&y + 2)) = (*((uint8_t *)&x + 1));\
											(*((uint8_t *)&y + 3)) = (*((uint8_t *)&x    ));\
}

#define ROTATE_LEFT_32(x,position)		( (x<<position) | (x>>(32-position)))



/******************************************************************************
Type definitions
******************************************************************************/
typedef struct tagSHA1Digest
{

	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	uint32_t E;
}st_SHA1_Digest;

typedef struct tagSHA1Chunk
{
	union tagSHA1ChunkData
	{
		uint8_t 	u8 [64];
		uint32_t 	u32[16];
	} data;
	uint8_t		pos;
	uint8_t		reserved;
} st_SHA1_Chunk;

typedef struct tagMd5MessageInfo
{
	uint16_t	len;
	uint16_t	blk;
} st_SHA1_MsgInfo;

typedef struct tagMd5Info
{
	st_SHA1_Digest		digest;	
	st_SHA1_Chunk 		chunk;
	st_SHA1_MsgInfo		msg;
} st_SHA1_Info;

/******************************************************************************
Private global variables and functions
******************************************************************************/
//static void R_SHA1_DigestChunk_C(st_SHA1_Info * sha1_info);
extern void R_SHA1_DigestChunk_Asm(st_SHA1_Info * sha1_info, uint16_t blk_cnt, uint8_t *dat_ptr);
/******************************************************************************
* Function Name	: int8_t R_Sha1_HashDigest(uint8_t *, uint8_t *, uint16_t, uint8_t, R_sha1 *);
* Description  	: Calculate MD5 from input sha1_info
*				: User need to input two elements:
*				:	1. sha1_info.msg_info.ptr: pointer to message for md5 calculation
*				:	2. sha1_info.msg_info.len: length of the input message
* Arguments    	: st_SHA1_Info *sha1_info, follow instruction above
*				: Calculated md5 is stored in sha1_info.digest
* Return Value 	: none
******************************************************************************/
int8_t R_Sha1_HashDigest(uint8_t *mdat, uint8_t *hdat, uint16_t mlength, uint8_t control, R_sha1 * work)
{
	st_SHA1_Info 		* sha1_info 	= (st_SHA1_Info *)work;
	uint8_t			local_chunk_pos;
	uint16_t		blk_cnt;
	uint32_t 		temp_len;
	uint16_t		i;
	
	/* Check hash and work pointer */
	if ((hdat == NULL) || (work == NULL))
	{
		return R_SHA_ERROR_POINTER;	
	}
	
	/* Check control flag, nopadding must used with finish */
	if ( ((control &  R_SHA_NOPADDING) != 0) &&
		((control &  R_SHA_FINISH) == 0) )
	{
		return R_SHA_ERROR_FLAG;	
	}
	
	/* Check message length validity */
	if ((mdat == NULL) && (mlength > 0))
	{
		return R_SHA_ERROR_LENGTH;	
	}	
	
	/* First, initialize the buffer, digest data, message position and length */
	if ( (control & R_SHA_INIT) != 0)
	{
		sha1_info->digest.A 	= 0x67452301;
		sha1_info->digest.B 	= 0xefcdab89;
		sha1_info->digest.C 	= 0x98badcfe;
		sha1_info->digest.D 	= 0x10325476;
		sha1_info->digest.E		= 0xc3d2e1f0;
		
		sha1_info->chunk.pos 	= 0;
		sha1_info->msg.len 		= 0;
	}
	
	/* Get the total message length */
	sha1_info->msg.len += mlength;
	
	/* Copy message */
	local_chunk_pos = sha1_info->chunk.pos;
	
	blk_cnt = mlength / 64;
	if (blk_cnt > 0)
	{
		R_SHA1_DigestChunk_Asm(sha1_info, blk_cnt, mdat);
		
	}

	for (i=(blk_cnt*64); i< mlength; i++)
	{
		sha1_info->chunk.data.u8[local_chunk_pos] = mdat[i];
		local_chunk_pos++;
		if ( (local_chunk_pos >= 64) )
		{
			local_chunk_pos = 0;
			R_SHA1_DigestChunk_Asm(sha1_info, 1, sha1_info->chunk.data.u8);
		}
	}
	
	/* Final round */
	if ( (control & R_SHA_FINISH) != 0)
	{
		/* If padding required */
		if ( (control & R_SHA_NOPADDING) == 0)
		{
			/* Special case when position >= 56, length not include until next chunk */
			sha1_info->chunk.data.u8[local_chunk_pos] = 0x80;
			local_chunk_pos++;
			
			if (local_chunk_pos >= 56)
			{
				while(local_chunk_pos < CHUNK_SIZE_BYTE)
				{
					sha1_info->chunk.data.u8[local_chunk_pos] = 0;
					local_chunk_pos++;	
				}
				R_SHA1_DigestChunk_Asm(sha1_info, 1, sha1_info->chunk.data.u8);
				memset((uint8_t *)sha1_info->chunk.data.u8, 0, 56);
				local_chunk_pos = 0;
			}
			else
			{
				while(local_chunk_pos < 56)
				{
					sha1_info->chunk.data.u8[local_chunk_pos] = 0;
					local_chunk_pos++;	
				}
			}

			temp_len = (uint32_t)(sha1_info->msg.len << 3);
			COPY_32_MSB_LSB(sha1_info->chunk.data.u32[CHUNK_SIZE_INT32-1], temp_len);
			temp_len = (uint32_t)(sha1_info->msg.len >> 13);
			COPY_32_MSB_LSB(sha1_info->chunk.data.u32[CHUNK_SIZE_INT32-2], temp_len);		
		
			R_SHA1_DigestChunk_Asm(sha1_info, 1, sha1_info->chunk.data.u8);
		}
		
		COPY_32_MSB_LSB(hdat[0], sha1_info->digest.A);
		COPY_32_MSB_LSB(hdat[4], sha1_info->digest.B);
		COPY_32_MSB_LSB(hdat[8], sha1_info->digest.C);
		COPY_32_MSB_LSB(hdat[12], sha1_info->digest.D);
		COPY_32_MSB_LSB(hdat[16], sha1_info->digest.E);	
	}
	
	sha1_info->chunk.pos = local_chunk_pos;
		
	return R_PROCESS_COMPLETE;
}

//static void R_SHA1_DigestChunk_C(st_SHA1_Info * sha1_info)
//{
//	uint32_t 	A,B,C,D,E;
//	uint32_t 	W[16];
//	uint32_t 	temp;
//	uint8_t  	i;
//	
//	/* Start calculating SHA1: two steps
//		1.  SHA1 require extend to 80 uint32_t chunk from 16 uint32_t chunk
//			Store the extend buffer on RAM require: 80x4 = 320 bytes
//			Method to reduce RAM size: 	16x4 = 64  bytes on RAM (compatibility with MD5)
//										64x4 = 256 bytes on Stack 
//			Do: initialize extend buffer from original buffer
//		2. 
//	*/
//	A = sha1_info->digest.A;
//	B = sha1_info->digest.B;
//	C = sha1_info->digest.C;
//	D = sha1_info->digest.D;
//	E = sha1_info->digest.E;	
//	
//	COPY_32_MSB_LSB(W[ 0], sha1_info->chunk.u32[ 0]);
//	COPY_32_MSB_LSB(W[ 1], sha1_info->chunk.u32[ 1]);
//	COPY_32_MSB_LSB(W[ 2], sha1_info->chunk.u32[ 2]);
//	COPY_32_MSB_LSB(W[ 3], sha1_info->chunk.u32[ 3]);
//	COPY_32_MSB_LSB(W[ 4], sha1_info->chunk.u32[ 4]);
//	COPY_32_MSB_LSB(W[ 5], sha1_info->chunk.u32[ 5]);
//	COPY_32_MSB_LSB(W[ 6], sha1_info->chunk.u32[ 6]);
//	COPY_32_MSB_LSB(W[ 7], sha1_info->chunk.u32[ 7]);
//	COPY_32_MSB_LSB(W[ 8], sha1_info->chunk.u32[ 8]);
//	COPY_32_MSB_LSB(W[ 9], sha1_info->chunk.u32[ 9]);
//	COPY_32_MSB_LSB(W[10], sha1_info->chunk.u32[10]);
//	COPY_32_MSB_LSB(W[11], sha1_info->chunk.u32[11]);
//	COPY_32_MSB_LSB(W[12], sha1_info->chunk.u32[12]);
//	COPY_32_MSB_LSB(W[13], sha1_info->chunk.u32[13]);
//	COPY_32_MSB_LSB(W[14], sha1_info->chunk.u32[14]);
//	COPY_32_MSB_LSB(W[15], sha1_info->chunk.u32[15]);
//	
//	for (i=0; i<80; i++)
//	{
//		if (i>15)
//		{
//			uint32_t temp1,temp2,temp3,temp4;
//			temp1 = W[(i - 3) & 0x0F];
//			temp2 = W[(i - 8) & 0x0F];
//			temp3 = W[(i - 14) & 0x0F];
//			temp4 = W[ i      & 0x0F];
//			W[i&0x0F] = (W[(i - 3) & 0x0F] ^ W[(i - 8) & 0x0F] ^W[(i - 14) & 0x0F] ^ W[ i      & 0x0F]);
//			W[i&0x0F] = ROTATE_LEFT_32(W[i&0x0F],1);
//		}
//		
//		if (i <=19)
//		{
//	        temp = ROTATE_LEFT_32(A,5) + (D ^ (B & (C ^ D))) + E + W[i&0x0F] + 0x5a827999;
//		}
//		else if (i <=39)
//		{
//            temp = ROTATE_LEFT_32(A,5) + (B ^ C ^ D) + E + W[i&0x0F] + 0x6ed9eba1;
//		}
//		else if (i <= 59)
//		{
//			temp = (B&C) ^ (B&D) ^ (C&D);
//			temp = ROTATE_LEFT_32(A,5) + ((B&C) ^ (B&D) ^ (C&D)) + E + W[i&0x0F] + 0x8f1bbcdc;
//		}
//		else //i<=79
//		{
//			temp = ROTATE_LEFT_32(A,5) + (B ^ C ^ D) + E + W[i&0x0F] + 0xca62c1d6;		
//		}
//		E = D;
//        D = C;
//		C = ROTATE_LEFT_32(B, 30);
//		B = A;
//		A = temp;		
//	}
//	
//	sha1_info->digest.A +=  A;
//	sha1_info->digest.B +=  B;
//	sha1_info->digest.C +=  C;
//	sha1_info->digest.D +=  D;
//	sha1_info->digest.E +=  E;	
//}
