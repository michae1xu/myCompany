/**
 * @brief       : 
 *
 * @file        : api_crypto.h
 * @version     : v0.0.1
 * @date        : 2017.11.10

 */
#ifndef __API_CRYPTO_H__
#define __API_CRYPTO_H__

#include "api_typedef.h"

#ifdef __cplusplus
extern "c" {
#endif

typedef void * rcp_sym_ctx;

typedef enum
{
	RCP_NONE = 0,              // Not Used/Initialized

	// Hash algorithms
	RCP_HASH = 10,             // Begin marker
	RCP_HASH_SHA1,             // SHA-1
	RCP_HASH_SHA224,           // SHA-224
	RCP_HASH_SHA256,           // SHA-256
	RCP_HASH_MAX,              // must be last

	// MAC algorithms
	RCP_MAC = 20,              // Begin marker
	RCP_MAC_HMAC_SHA1,         // HMAC-SHA-1
	RCP_MAC_HMAC_SHA224,       // HMAC-SHA-224
	RCP_MAC_HMAC_SHA256,       // HMAC-SHA-256
	RCP_MAC_AES_CMAC = 26,     // AES-CMAC
	RCP_MAC_AES_CBC_MAC,       // AES-CBC-MAC
	RCP_MAC_POLY1305,          // Poly1305
	RCP_MAC_MAX,               // must be last

	// (Block)Cipher algorithms
	RCP_CIPHER = 30,           // Begin marker
	RCP_CIPHER_AES,            // AES
	RCP_CIPHER_DES,            // DES
	RCP_CIPHER_TRIPLE_DES,     // 3DES
	RCP_CIPHER_MAX,            // must be last
} rcp_sym_algo;
    
typedef enum
{
	RCP_MODE_NONE = 0,              // Not Used/Initialized

	// (Block)Cipher modes
	RCP_MODE = 30,           // Begin marker
	RCP_MODE_ECB,            // ECB
	RCP_MODE_CBC,            // CBC
	RCP_MODE_CTR,            // CTR
	RCP_MODE_ICM,            // ICM
	RCP_MODE_CCM = 36,       // CCM
	RCP_MODE_XTS,            // XTS
	RCP_MODE_MAX,            // must be last
} rcp_sym_mode;
    
typedef enum
{
    RPU_AES                = 0x0,
    RPU_SM4                = 0x1,
    RPU_MISTY              = 0x2,
    RPU_TWOFISH            = 0x3,
    RPU_SERPENT            = 0x4,
    RPU_DES                = 0x5,
    RPU_3DES               = 0x6,
    RPU_CAMELLIA           = 0x7,
    RPU_SHACAL2            = 0x8,
    RPU_SEED               = 0x9,
    RPU_CAST               = 0xA,
    RPU_BLOWFISH           = 0xB,

    // Stream
    RPU_ZUC                = 0xC,
    RPU_SNOW3G             = 0xD,
} enum_crypto_algo; // size 4 bits

typedef enum
{
    RPU_DECRYPT                = 0x0,
    RPU_ENCRYPT                = 0x1,
} enum_crypto_encrypt; // size 1 bit

enum rpu_syn_mode{
    RPU_ECB_MODE    = 0x0,
    RPU_CBC_MODE    = 0x1,
    RPU_CFB64_MODE  = 0x2,
    RPU_OFB64_MODE  = 0x3,
    RPU_CFB128_MODE = 0x4,
    RPU_OFB128_MODE = 0x5,
    RPU_CTR128_MODE = 0x6,
};

typedef struct
{
	uint8_t d[32];
	uint32_t dlen;
	uint8_t q[64];
	uint32_t qlen;
} rcp_sm2_ctx;

typedef enum
{
	SM4_ECB = 0x0,
	SM4_CBC = 0x1,
} enum_sm4_mode;

typedef struct
{
	uint8_t *iv;
	uint32_t ivlen;
	uint8_t *key;
	uint32_t keylen;
	uint8_t mode;
	uint8_t encrypt;
} rcp_sm4_ctx;

typedef struct
{
	uint32_t key_asset_id;      // AssetId of the private/public key
	uint32_t domain_asset_id;   // AssetId of the domain parameters
	size_t modulus_bits;        // (Prime) Modulus size in bits
	rcp_sym_algo hash_algo;     // Hash Algorithm to use
} rcp_asym_ctx;

typedef struct
{
	uint8_t *data;
	uint32_t len;
} rcp_big_init;

typedef struct
{
	rcp_big_init x;
	rcp_big_init y;
} rcp_ecc_point;

typedef struct
{
	size_t curve_bits;        // Modulus size in bits
	rcp_big_init p;             // Curve modulus (p)
	rcp_big_init a;             // Curve constant a
	rcp_big_init b;             // Curve constant b
	rcp_big_init n;             // Curve order (n)
	rcp_ecc_point base_point;   // Curve base point (G)
	uint8_t cofactor;           // Curve cofactor (h)
								// - Preferably 1, 0 == 1
} rcp_ecp_group;

typedef struct
{
	rcp_big_init r;             // Signature component r
	rcp_big_init s;             // Signature component s
} rcp_ecdsa_sig;

int32_t rcp_crypto_open(void);
int32_t rcp_crypto_close(void);

int32_t rcp_random_reseed(void);
int32_t rcp_random_data(uint8_t *data, size_t length);

int32_t rcp_sym_init(rcp_sym_algo algo, rcp_sym_mode mode, rcp_sym_ctx * ctx);
int32_t rcp_sym_free(rcp_sym_ctx ctx);

int32_t rcp_sym_set_key(rcp_sym_ctx ctx, const uint8_t *key, size_t keylen);
int32_t rcp_sym_set_iv(rcp_sym_ctx ctx, const uint8_t *iv, size_t ivlen);

int32_t rcp_sym_hash_update(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen);
int32_t rcp_sym_hash_final(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen, uint8_t *digest, size_t *digestlen);

int32_t rcp_sym_mac_update(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen);
int32_t rcp_sym_mac_generate(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen, uint8_t *mac, size_t *maclen);
int32_t rcp_sym_mac_verify(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen, uint8_t *mac, size_t maclen);

int32_t rcp_sym_cipher_init_enc(rcp_sym_ctx ctx);
int32_t rcp_sym_cipher_update(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen, uint8_t *output, size_t *outputlen);
int32_t rcp_sym_cipher_final(rcp_sym_ctx ctx, const uint8_t *input, size_t ilen, uint8_t *output, size_t *olen);

int32_t rcp_rsa_set_pubkey(rcp_asym_ctx *ctx, const uint8_t *n, size_t nlen, const uint8_t *e, size_t elen, rcp_sym_algo hash_algo);
int32_t rcp_rsa_set_prikey(rcp_asym_ctx *ctx, const uint8_t *n, size_t nlen, const uint8_t *d, size_t dlen, rcp_sym_algo hash_algo);
int32_t rcp_rsa_pkcs1v1_5_sign(rcp_asym_ctx *ctx, const uint8_t *hash, size_t hashlen, uint8_t *sig, uint32_t *siglen);
int32_t rcp_rsa_pkcs1v1_5_verify(rcp_asym_ctx *ctx, const uint8_t *hash, size_t hashlen, const uint8_t *sig, uint32_t siglen);

int32_t rcp_ecc_set_grp(rcp_asym_ctx *ctx, rcp_ecp_group *ecp_grp);
int32_t rcp_ecdsa_set_prikey(rcp_asym_ctx *ctx, size_t curve_bits, const rcp_big_init *d, rcp_sym_algo hash_algo);
int32_t rcp_ecdsa_set_pubkey(rcp_asym_ctx *ctx, size_t curve_bits, const rcp_ecc_point *q, rcp_sym_algo hash_algo);
int32_t rcp_ecdsa_sign(rcp_asym_ctx *ctx, const uint8_t *hash, size_t hashlen, rcp_ecdsa_sig *sig);
int32_t rcp_ecdsa_verify(rcp_asym_ctx *ctx, const uint8_t *hash, size_t hashlen, const rcp_ecdsa_sig *sig);

int32_t rcp_sm2_set_public_key(rcp_sm2_ctx *ctx, const uint8_t *pubkey, uint32_t size);
int32_t rcp_sm2_set_private_key(rcp_sm2_ctx *ctx, const uint8_t *prikey, uint32_t size);
int32_t rcp_sm2_sign(rcp_sm2_ctx *ctx, const uint8_t *msg, uint32_t msglen, uint8_t *sig, uint32_t siglen);
int32_t rcp_sm2_verify(rcp_sm2_ctx *ctx, const uint8_t *sig, uint32_t siglen, const uint8_t *msg, uint32_t msglen);

int32_t rcp_sm3(uint8_t *data, uint64_t len, uint8_t hash[32]);
int32_t rcp_sm4(rcp_sm4_ctx *ctx, const uint8_t *in, uint8_t *out, uint32_t len);

#ifdef __cplusplus
} // extern "c"
#endif

#endif
