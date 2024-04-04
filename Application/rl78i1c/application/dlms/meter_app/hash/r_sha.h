/*""FILE COMMENT""*******************************************************************
* File Name     : r_sha.h
* Version       : 1.00
* Device(s)     : -
* Tool-Chain    : -
* H/W Platform  : -
* Description   : SHA Library header file.
*************************************************************************************
* History       : DD.MM.YYYY Version Description
*""FILE COMMENT END""****************************************************************/

#ifndef __R_SHA_H__
#define __R_SHA_H__

#include "r_stdint1.h"

#define __R_SHA_VERSION__	"1.00"

#define R_SHA_ADD		(0)
#define R_SHA_INIT		(1)
#define R_SHA_FINISH	(2)
#define R_SHA_NOPADDING	(4)
#define R_SHA_FINISH_WITHOUT_PADDING	(R_SHA_FINISH | R_SHA_NOPADDING)

#define	R_PROCESS_COMPLETE	(0)
#define	R_SHA_ERROR_POINTER	(-1)
#define	R_SHA_ERROR_FLAG	(-2)
#define	R_SHA_ERROR_LENGTH	(-3)

typedef struct {
	uint32_t work[24];
} R_sha1;
typedef struct {
	uint32_t work[27];
} R_sha256;

int8_t R_Sha1_HashDigest(uint8_t *, uint8_t *, uint16_t, uint8_t, R_sha1 *);
int8_t R_Sha256_HashDigest(uint8_t *, uint8_t *, uint16_t, uint8_t, R_sha256 *);

#endif
