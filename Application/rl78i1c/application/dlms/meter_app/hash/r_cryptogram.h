
/* $Id: //depot/cryptogram/include/r_cryptogram.h#2 $ */
/* $Date: 2010/02/28 $ */

#ifndef __r_cryptogram_h__
#define __r_cryptogram_h__

#if defined(MACOS) || defined(RX)
#include <stdint.h>
#else
#include "r_stdint.h"
//#include "r_stdint1.h"	// added for DLMS
#endif

#define FUNC_BIT_KEY(crypt,n)	R_##crypt##_##n##_Keysch
#define AES_KEY(n)	FUNC_BIT_KEY(Aes,n)
#define CAMELLIA_KEY(n)	FUNC_BIT_KEY(Camellia,n)

#define FUNC_BIT_ENC(crypt,n,mode)	R_##crypt##_##n##_##mode##enc
#define AES_ECBENC(n)	FUNC_BIT_ENC(Aes,n,Ecb)
#define AES_CBCENC(n)	FUNC_BIT_ENC(Aes,n,Cbc)
#define AES_OTFECBENC(n)	FUNC_BIT_ENC(Aes,n,OtfEcb)
#define AES_OTFCBCENC(n)	FUNC_BIT_ENC(Aes,n,OtfCbc)
#define CAMELLIA_ECBENC(n)	FUNC_BIT_ENC(Camellia,n,Ecb)
#define CAMELLIA_CBCENC(n)	FUNC_BIT_ENC(Camellia,n,Cbc)

#define FUNC_BIT_DEC(crypt,n,mode)	R_##crypt##_##n##_##mode##dec
#define AES_ECBDEC(n)	FUNC_BIT_DEC(Aes,n,Ecb)
#define AES_CBCDEC(n)	FUNC_BIT_DEC(Aes,n,Cbc)
#define AES_OTFECBDEC(n)	FUNC_BIT_DEC(Aes,n,OtfEcb)
#define AES_OTFCBCDEC(n)	FUNC_BIT_DEC(Aes,n,OtfCbc)
#define CAMELLIA_ECBDEC(n)	FUNC_BIT_DEC(Camellia,n,Ecb)
#define CAMELLIA_CBCDEC(n)	FUNC_BIT_DEC(Camellia,n,Cbc)
/*
 Define as follows and specify the length of key at compile time;
 
	void AES_KEY(KEY_LENGTH)(uint8_t *key,uint32_t *ekey)
	void AES_ECBENC(KEY_LENGTH)(uint8_t *pdat, uint8_t *cdat, uint32_t *ekey)
	void AES_CBCENC(KEY_LENGTH)(uint8_t *pdat, uint8_t *cdat, uint8_t *ivec, uint32_t *ekey)
	void AES_ECBDEC(KEY_LENGTH)(uint8_t *cdat, uint8_t *pdat, uint32_t *ekey)
	void AES_CBCDEC(KEY_LENGTH)(uint8_t *cdat, uint8_t *pdat, uint8_t *ivec, uint32_t *ekey)
 */
 
#if !defined(R8C) && !defined(M16C) && !defined(M32C)
#endif

void R_Des_Keysch(uint8_t near *,  uint32_t near *);
void R_Des_Ecbenc(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Des_Ecbdec(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Des_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Des_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);

void R_TDes2_Keysch(uint8_t near *,  uint32_t near *);
void R_TDes2_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_TDes2_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);

void R_TDes3_Keysch(uint8_t near *,  uint32_t near *);
void R_TDes3_Ecbenc(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_TDes3_Ecbdec(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_TDes3_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_TDes3_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);

void R_Aes_128_Keysch(uint8_t near *, uint32_t near *);
void R_Aes_128_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_128_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_128_Ecbenc(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_128_Ecbdec(uint8_t near *, uint8_t near *,  uint32_t near *, uint16_t near);

typedef struct {
	uint32_t work[60];
} R_ctr_mode;

void R_Aes_128_CtrSetKey(uint8_t *, R_ctr_mode *);
void R_Aes_128_CtrSetCounter(uint8_t *, R_ctr_mode *);
void R_Aes_128_CtrGetCounter(uint8_t *, R_ctr_mode *);
void R_Aes_128_CtrCrypto(uint8_t *, uint8_t *, uint16_t, R_ctr_mode *);

void R_Aes_128_OtfCbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Aes_128_OtfCbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Aes_128_OtfEcbenc(uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Aes_128_OtfEcbdec(uint8_t near *, uint8_t near *,  uint16_t near *, uint16_t near);
void R_Aes_256_OtfEcbenc(uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Aes_256_OtfEcbdec(uint8_t near *, uint8_t near *,  uint16_t near *, uint16_t near);

void R_Aes_192_Keysch(uint8_t near *, uint32_t near *);
void R_Aes_192_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_192_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_192_Ecbenc(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_192_Ecbdec(uint8_t near *, uint8_t near *,  uint32_t near *, uint16_t near);

void R_Aes_256_Keysch(uint8_t near *, uint32_t near *);
void R_Aes_256_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_256_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_256_Ecbenc(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Aes_256_Ecbdec(uint8_t near *, uint8_t near *,  uint32_t near *, uint16_t near);

void R_Misty1_Keysch(uint8_t near *, uint16_t near *);
void R_Misty1_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Misty1_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Misty1_Ecbenc(uint8_t near *, uint8_t near *, uint16_t near *, uint16_t near);
void R_Misty1_Ecbdec(uint8_t near *, uint8_t near *,  uint16_t near *, uint16_t near);

void R_Camellia_128_Keysch(uint8_t near *, uint32_t near *);
void R_Camellia_128_Cbcenc(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Camellia_128_Cbcdec(uint8_t near *, uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Camellia_128_Ecbenc(uint8_t near *, uint8_t near *, uint32_t near *, uint16_t near);
void R_Camellia_128_Ecbdec(uint8_t near *, uint8_t near *,  uint32_t near *, uint16_t near);

void R_Camellia_128_CtrSetKey(uint8_t *, R_ctr_mode *);
void R_Camellia_128_CtrSetCounter(uint8_t *, R_ctr_mode *);
void R_Camellia_128_CtrGetCounter(uint8_t *, R_ctr_mode *);
void R_Camellia_128_CtrCrypto(uint8_t *, uint8_t *, uint16_t, R_ctr_mode *);

#if defined(MD5)
#define FUNC_HASH(n)	R_##Md##n##_Hash
#else
#define FUNC_HASH(n)	R_##Sha##n##_Hash
#endif
#define HASH(n)	FUNC_HASH(n)

void R_Sha1_Hash(uint8_t near *, uint8_t near *, uint16_t near); 
void R_Sha256_Hash(uint8_t near *, uint8_t near *, uint16_t near); 
void R_Md5_Hash(uint8_t *, uint8_t *, uint16_t); 

#define R_SHA_ADD		(0)
#define R_SHA_INIT		(1)
#define R_SHA_FINISH	(2)
#define R_SHA_NOPADDING	(4)
#define R_SHA_FINISH_WITHOUT_PADDING	(R_SHA_FINISH | R_SHA_NOPADDING)

typedef struct {
	uint32_t work[24];
} R_md5;
typedef struct {
	uint32_t work[24];
} R_sha1;
typedef struct {
	uint32_t work[27];
} R_sha256;

#define	MD5_MD8_LEN			(16)
#define	MD5_MD32_LEN		( 4)
#define	MD5_BLOCK8_LEN		(64)
#define	MD5_BLOCK32_LEN		(16)
#define	SHA1_MD8_LEN		(20)
#define	SHA1_MD32_LEN		( 5)
#define	SHA1_BLOCK8_LEN		(64)
#define	SHA1_BLOCK32_LEN	(16)
#define	SHA256_MD8_LEN		(32)
#define	SHA256_MD32_LEN		( 8)
#define	SHA256_BLOCK8_LEN	(64)
#define	SHA256_BLOCK32_LEN	(16)

#if defined(MD5)
#define MD_SIZE	5
#define	HASH_MD8_LEN		MD5_MD8_LEN
#define	HASH_MD32_LEN		MD5_MD32_LEN
#define	HASH_BLOCK8_LEN		MD5_BLOCK8_LEN
#define	HASH_BLOCK32_LEN	MD5_BLOCK32_LEN
#elif defined(SHA1)
#define MD_SIZE	1
#define	HASH_MD8_LEN		SHA1_MD8_LEN
#define	HASH_MD32_LEN		SHA1_MD32_LEN
#define	HASH_BLOCK8_LEN		SHA1_BLOCK8_LEN
#define	HASH_BLOCK32_LEN	SHA1_BLOCK32_LEN
#elif defined(SHA256)
#define MD_SIZE	256
#define	HASH_MD8_LEN		SHA256_MD8_LEN
#define	HASH_MD32_LEN		SHA256_MD32_LEN
#define	HASH_BLOCK8_LEN		SHA256_BLOCK8_LEN
#define	HASH_BLOCK32_LEN	SHA256_BLOCK32_LEN
#endif

//int8_t R_Md5_HashDigest(uint8_t *, uint8_t *, uint16_t, uint8_t, R_md5 *);
int8_t R_Sha1_HashDigest(uint8_t *, uint8_t *, uint16_t, uint8_t, R_sha1 *);
int8_t R_Sha256_HashDigest(uint8_t *, uint8_t *, uint16_t, uint8_t, R_sha256 *);

void R_HMAC_md5(uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, void *);
void R_HMAC_sha1(uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, void *);
void R_HMAC_sha256(uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, void *);

/* lower functions' API for R_HMAC_md5/R_HMAC_sha1 */
void R_Md5_Init(void *);
void R_Md5_Update(uint8_t *, uint16_t, void *);
void R_Md5_Final(uint8_t *, void *);
void R_Sha1_Init(void *);
void R_Sha1_Update(uint8_t *, uint16_t, void *);
void R_Sha1_Final(uint8_t *, void *);

/* Remark: the following is just for combination tests */
void R_Sha1_Final_NoPadding(uint8_t *, void *);
void R_Md5_Final_NoPadding(uint8_t *, void *);
void R_HMAC_sha1_repeat(uint8_t *, uint16_t, uint32_t, uint8_t *, uint16_t, uint8_t *, void *);
void R_HMAC_md5_repeat(uint8_t *, uint16_t, uint32_t, uint8_t *, uint16_t, uint8_t *, void *);


#if defined(R8C) || defined(M16C) || defined(M32C)
#pragma PARAMETER R_Sha1_Hash(r1, r2, r3)
#pragma PARAMETER R_Misty1_Keysch(a0, a1)
#pragma PARAMETER R_Misty1_Cbcenc(a0, a1, r1, r2, r3)
#pragma PARAMETER R_Misty1_Cbcdec(a1, a0, r1, r2, r3)
#pragma PARAMETER R_Misty1_Ecbenc(a0, a1, r2, r3)
#pragma PARAMETER R_Misty1_Ecbdec(a1, a0, r2, r3)
#pragma PARAMETER R_Aes_128_OtfEcbenc(a0, a1, r2, r3)
#pragma PARAMETER R_Aes_128_OtfEcbdec(a1, a0, r2, r3)
#pragma PARAMETER R_Aes_128_OtfCbcenc(a0, a1, r1, r2, r3)
#pragma PARAMETER R_Aes_128_OtfCbcdec(a1, a0, r1, r2, r3)
#pragma PARAMETER R_Aes_256_OtfEcbenc(a0, a1, r2, r3)
#pragma PARAMETER R_Aes_256_OtfEcbdec(a1, a0, r2, r3)
#pragma PARAMETER R_Aes_256_OtfCbcenc(a0, a1, r1, r2, r3)
#pragma PARAMETER R_Aes_256_OtfCbcdec(a1, a0, r1, r2, r3)
#endif

/* Modulo ExponentiationÅic = a^b mod pÅj */
int32_t R_mod_exp(uint8_t *p, uint8_t *a, uint8_t *b, uint8_t *c, uint16_t *len);

#if 0
/* Hash Functions */
void P_Sha1_Hash(uint8_t *, uint8_t *, unsigned int);
#endif

/* RSA_PKCS */
int32_t rsa_pkcs_sign(uint8_t *privatekey, uint8_t *publickey, uint8_t *plainData, int32_t length[2], uint8_t *sign);
int32_t rsa_pkcs_verf(uint8_t *e, uint8_t *pk, uint8_t *pdata, int32_t len[2], uint8_t sign[256]);
int32_t rsa_pkcs_encode(uint8_t *plainData,int32_t length[2], uint8_t *EM);
int32_t rsaksp(uint8_t *d, uint8_t *n, uint8_t *EM, uint8_t *sign, uint8_t judg);


/* RSA_PSS sign */

int32_t rsa_pss_sign(uint8_t *prikey_d, uint8_t *pubkey_n, uint8_t *plainData, int32_t length[2], uint8_t *sign, uint8_t *salt);
int32_t rsa_pss_pad(uint8_t *plainData,int32_t length[2], uint8_t *EM, uint8_t *salt);
int32_t rsassp(uint8_t *prikey_d, uint8_t *pubkey_n, uint8_t *EM, uint8_t *sign);
int32_t rsa_pss_verf(uint8_t *pubkey_e, uint8_t *pubkey_n, uint8_t *pdata, int32_t len[2], uint8_t *sign);
int32_t emsa_pss_verify(uint8_t *pdata, uint8_t *em, int32_t len[2]);
void mgf(uint8_t* mgfSeed, uint8_t* out, int32_t maskLen);


#endif