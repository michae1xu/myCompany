/**
 * @brief       : 
 *
 * @file        : api_crypto.h
 * @version     : v0.0.1
 * @date        : 2017.11.10

 */
#ifndef __API_BIOS_H__
#define __API_BIOS_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    RCP_OTP_WRITE = 0x0,
    RCP_OTP_READ = 0x1,
} rcp_bios_otp_operate;

typedef enum
{
    F_ALGO_AES = 0x0,
    F_ALGO_SM4 = 0x1,
    F_ALGO_3DES = 0x2,
} rcp_bios_crypto_algo;

typedef enum
{
	F_CRYPTO_ECB = 0x0,
	F_CRYPTO_CBC = 0x1,
} rcp_bios_crypto_mode;

typedef enum
{
	F_DECRYPT = 0x0,
	F_ENCRYPT = 0x1,
} rcp_bios_crypto_operate;

typedef enum
{
    F_ALGO_SHA256 = 0x0,
    F_ALGO_SM3 = 0x1,
    F_ALGO_SHA1 = 0x2,
    F_ALGO_SHA384 = 0x3,
    F_ALGO_SHA512 = 0x4,
    F_ALGO_MD5 = 0x5,
    F_ALGO_MD4 = 0x6,
    F_ALGO_SHA224 = 0x7,
} rcp_bios_hash_algo;

typedef enum {
	BIOS_HASH_ONETIME = 0x0,
	BIOS_HASH_FINAL = 0x1,
	BIOS_HASH_INITIAL = 0x2,
	BIOS_HASH_UPDATE = 0x3,
} rcp_bios_hash_mode; 

typedef enum
{
    BIOS_HMAC_SHA1 = 0x0,
	BIOS_HMAC_SHA224 = 0x1,
	BIOS_HMAC_SHA256 = 0x2,
	BIOS_HMAC_MD5 = 0x3,
} rcp_bios_hmac_algo;

typedef enum
{
	BIOS_SM2_TEXT_VERIFY = 0x0,
	BIOS_SM2_DIGEST_VERIFY = 0x1,
	BIOS_RSA_TEXT_VERIFY = 0x2,
	BIOS_RSA_DIGEST_VERIFY = 0x3,
	BIOS_SM2_TEXT_SIGN = 0x4,
	BIOS_SM2_DIGEST_SIGN = 0x5,
	BIOS_RSA_TEXT_SIGN = 0x6,
	BIOS_RSA_DIGEST_SIGN = 0x7,
	BIOS_ECC_TEXT_SIGN = 0x8,
	BIOS_ECC_TEXT_VERIFY = 0x9,
} rcp_bios_rsa_operate;

int32_t rcp_bios_ddr_remap(void);

int32_t rcp_bios_otp(uint32_t rw, uint32_t *data);

int32_t rcp_bios_crypto(uint8_t algo, uint8_t mode, const uint8_t *in, size_t inlen,
	uint8_t *out, size_t outlen, const uint8_t *key, size_t keylen, uint8_t *iv, uint32_t enc);

int32_t rcp_bios_hash(rcp_bios_hash_algo algo, rcp_bios_hash_mode mode,
	const uint8_t *data, size_t size, uint8_t *hash, size_t hash_len);

int32_t rcp_bios_hmac(rcp_bios_hmac_algo algo, rcp_bios_hash_mode mode,
	const uint8_t *data, size_t size, const uint8_t *key, size_t key_len, uint8_t *hash, size_t hash_len);

int32_t rcp_bios_rsa_set_private_key(const uint8_t *n, uint32_t nsize,
	const uint8_t *d, uint32_t dsize);
int32_t rcp_bios_rsa_set_public_key(const uint8_t *n, uint32_t nsize,
	const uint8_t *e, uint32_t esize);
int32_t rcp_bios_rsa_sign_verify(uint8_t algo,
	uint8_t *msg,
	uint32_t msglen,
	uint8_t *sig,
	uint32_t siglen);

int32_t rcp_bios_sm2_sign_verify(uint8_t algo,
	uint8_t *key,
	uint32_t keylen,
	uint8_t *msg,
	uint32_t msglen,
	uint8_t *sig,
	uint32_t siglen);

#ifdef __cplusplus
} // extern "c"
#endif

#endif /* __API_FIXED_H__ */
