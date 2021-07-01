/**
 * @brief       : 
 *
 * @file        : dsc.h
 * @version     : v0.0.1
 * @date        : 2017.11.10

 */
#ifndef __DSC_H__
#define __DSC_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   return code
 */
typedef enum 
{
    /** Success */
    DSC_SUCCESS = 0,                    /*!< No error */

    /** Firmware related errors [-1 ... -31]: */
    DSC_INVALID_TOKEN = -1,             /*!< Invalid token */
    DSC_INVALID_PARAMETER = -2,         /*!< Invalid parameter */
    DSC_INVALID_KEYSIZE = -3,           /*!< Invalid key size */
    DSC_INVALID_LENGTH = -4,            /*!< Invalid length */
    DSC_INVALID_LOCATION = -5,          /*!< Invalid location */
    DSC_CLOCK_ERROR = -6,               /*!< Clock error */
    DSC_ACCESS_ERROR = -7,              /*!< Access error */
    DSC_UNWRAP_ERROR = -10,             /*!< Unwrap error */
    DSC_DATA_OVERRUN_ERROR = -11,       /*!< Data overrun error */
    DSC_ASSET_CHECKSUM_ERROR = -12,     /*!< Asset checksum error */
    DSC_INVALID_ASSET = -13,            /*!< Invalid Asset */
    DSC_FULL_ERROR = -14,               /*!< Full/Overflow error */
    DSC_INVALID_ADDRESS = -15,          /*!< Invalid address */
    DSC_INVALID_MODULUS = -17,          /*!< Invalid Modulus */
    DSC_VERIFY_ERROR = -18,             /*!< Verify error */
    DSC_INVALID_STATE = -19,            /*!< Invalid state */
    DSC_OTP_WRITE_ERROR = -20,          /*!< OTP write error */
    DSC_PANIC_ERROR = -31,              /*!< Panic error */

    /** DMA related errors are bus related [-32 ... -63] */

    /** TRNG related errors [-64 ... -95]: */
    DSC_TRNG_SHUTDOWN_ERROR = -65,      /*!< Too many FROs shutdown */
    DSC_DRBG_STUCK_ERROR = -66,         /*!< Stuck DRBG */

    /** VAL related errors [-128 ... -137]: */
    DSC_UNSUPPORTED = -128,             /*!< Not supported */
    DSC_NOT_INITIALIZED = -129,         /*!< Not initialized yet */
    DSC_BAD_ARGUMENT = -130,            /*!< Wrong use; not depending on configuration */
    DSC_INVALID_ALGORITHM = -131,       /*!< Invalid algorithm code */
    DSC_INVALID_MODE = -132,            /*!< Invalid mode code */
    DSC_BUFFER_TOO_SMALL = -133,        /*!< Provided buffer too small for intended use */
    DSC_NO_MEMORY = -134,               /*!< No memory */
    DSC_OPERATION_FAILED = -135,        /*!< Operation failed */
    DSC_TIMEOUT_ERROR = -136,           /*!< Token or data timeout error */
    DSC_INTERNAL_ERROR = -137,          /*!< Internal error */  

    DSC_DEVICE_NOT_FOUND = -192,
    DSC_TOKEN_EXCHANGE_ERROR = -193,
    DSC_RESULT_NOT_MATCH = -194,
    DSC_ACK_ERROR = -195,
	DSC_DEVICE_NOT_OPENED = -196,
    DSC_DEVICE_BUSY = -197,
} DSC_STATUS;

/**
 * \brief   symmetric cryptographic algorithm type
 */
typedef enum
{
    DSC_NONE = 0,              /*!< Not Used/Initialized */

    // Hash algorithms
    DSC_HASH = 10,             /*!< Begin marker */
    DSC_HASH_SHA1,             /*!< SHA-1 */
    DSC_HASH_SHA224,           /*!< SHA-224 */
    DSC_HASH_SHA256,           /*!< SHA-256 */
    DSC_HASH_SM3,              /*!< SM3 */
    DSC_HASH_MD5,              /*!< MD5 */
    DSC_HASH_MAX,              /*!< must be last */

    // MAC algorithms
    DSC_MAC = 20,              /*!< Begin marker */
    DSC_MAC_HMAC_SHA1,         /*!< HMAC-SHA-1 */
    DSC_MAC_HMAC_SHA224,       /*!< HMAC-SHA-224 */
    DSC_MAC_HMAC_SHA256,       /*!< HMAC-SHA-256 */
    DSC_MAC_HMAC_MD5,          /*!< HMAC-MD5 */
    DSC_MAC_AES_CMAC = 26,     /*!< AES-CMAC */
    DSC_MAC_AES_CBC_MAC,       /*!< AES-CBC-MAC */
    DSC_MAC_MAX,               /*!< must be last */

    // (Block)Cipher algorithms
    DSC_CIPHER = 30,           /*!< Begin marker */
    DSC_CIPHER_AES,            /*!< AES */
    DSC_CIPHER_DES,            /*!< DES */
    DSC_CIPHER_TDES,           /*!< 3DES */
    DSC_CIPHER_SM4,
    DSC_CIPHER_TWOFISH,
    DSC_CIPHER_CAMELLIA,
    DSC_CIPHER_SEED,
    DSC_CIPHER_BLOWFISH,
    DSC_CIPHER_ZUC,
    DSC_CIPHER_SNOW3G,
    DSC_CIPHER_CIPHER_MAX,      /*!< must be last */
} dsc_sym_algo;

/**
 * \brief   symmetric cryptographic algorithm mode
 */
typedef enum
{
    DSC_MODE_NONE = 0,       /*!< Not Used/Initialized */

    // (Block)Cipher modes
    DSC_MODE = 30,           /*!< Begin marker */
    DSC_MODE_ECB,            /*!< ECB */
    DSC_MODE_CBC,            /*!< CBC */
    DSC_MODE_CTR,            /*!< CTR */
    DSC_MODE_ICM,            /*!< ICM */
    DSC_MODE_CCM = 36,       /*!< CCM */
    DSC_MODE_XTS,            /*!< XTS */
    DSC_MODE_CFB,            /*!< CFB */
    DSC_MODE_OFB,            /*!< OFB */
    DSC_MODE_STREAM,         /*!< STREAM */
    DSC_MODE_MAX,            /*!< must be last */
} dsc_sym_mode;

/**
 * \brief   symmetric cryptographic encrypt mode
 */
typedef enum
{
    DSC_DECRYPT = 0,
    DSC_ENCRYPT = 1,
} dsc_sym_encrypt;

/**
 * \brief   symmetric cryptographic algorithm structure.
 */
typedef struct
{
    void *contex;
    uint32_t session;
    uint32_t reserved;
} dsc_sym_ctx;

/**
 * \brief          open crypto device
 *
 * \return         see DSC_STATUS enum
 */
int32_t dsc_crypto_open(void);

/**
 * \brief          close crypto device
 *
 * \return         see DSC_STATUS enum
 */
int32_t dsc_crypto_close(void);

/**
 * \brief   random context structure.
 */
typedef struct 
{
    uint8_t reseed;     /*!<  reseed the trng   */
    uint8_t *data;      /*!<  buffer holding the  data */
    size_t len;         /*!<  length of the random data */
    uint32_t reserve1;  /*!<  reserve1          */
    uint32_t reserve2;  /*!<  reserve2          */
} dsc_random_ctx;

/**
 * \brief          get random data
 *
 * \param ctx      random context
 *
 * \return         see DSC_STATUS enum
 */
int32_t dsc_random_data(dsc_random_ctx *ctx);

/**
 * \brief          Initialize symmetric cryptographic structure
 *
 * \param algo     see dsc_sym_algo enum
 * \param mode     see dsc_sym_mode enum
 * \param key      buffer holding the key
 * \param keylen   length of the key  
 * \param iv       buffer holding the iv
 * \param ivlen    length of the iv  
 * \param ctx      sym context to be initialized
 *
 * \return         see DSC_STATUS enum
 */
int32_t dsc_sym_init(dsc_sym_ctx *ctx, 
                     dsc_sym_algo algo, 
                     uint8_t encrypt, 
					 dsc_sym_mode mode,
                     const uint8_t *key, 
					 uint32_t keylen, 
					 const uint8_t *iv, 
					 uint32_t ivlen);

/**
 * \brief          Clear symmetric cryptographic structure
 *
 * \param ctx      sym context to be clear
 *
 * \return         see DSC_STATUS enum
 */
int32_t dsc_sym_free(dsc_sym_ctx *ctx);

/**
 * \brief          hash algorithm process buffer
 *
 * \param ctx      sym context 
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 *
 * \return         see DSC_STATUS enum
 */
int32_t dsc_sym_hash_update(dsc_sym_ctx *ctx, 
                            const uint8_t *input, 
							size_t ilen);

/**
 * \brief               hash algorithm final digest
 *
 * \param ctx           sym context 
 * \param input         buffer holding the data
 * \param ilen          length of the input data 
 * \param digest        buffer holding the digest
 * \param digestlen     length of the digest 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_sym_hash_final(dsc_sym_ctx *ctx, uint8_t *digest, size_t *digestlen);

/**
 * \brief               cipher algorithm process buffer
 *
 * \param ctx           sym context 
 * \param input         buffer holding the data
 * \param ilen          length of the input data 
 * \param output        buffer holding the result
 * \param olen          length of the result 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_sym_cipher_update(dsc_sym_ctx *ctx, const uint8_t *input, size_t ilen, uint8_t *output, size_t *olen);

/**
 * \brief               cipher algorithm final result
 *
 * \param ctx           sym context 
 * \param input         buffer holding the data
 * \param ilen          length of the input data 
 * \param output        buffer holding the result
 * \param olen          length of the result 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_sym_cipher_final(dsc_sym_ctx *ctx);

/**
 * \brief               mac algorithm process buffer
 *
 * \param ctx           sym context 
 * \param input         buffer holding the data
 * \param ilen          length of the input data 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_sym_mac_update(dsc_sym_ctx *ctx, const uint8_t *input, size_t ilen);

/**
 * \brief               mac algorithm generate mac
 *
 * \param ctx           sym context 
 * \param input         buffer holding the data
 * \param ilen          length of the input data 
 * \param mac           buffer holding the mac
 * \param maclen        length of the mac 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_sym_mac_generate(dsc_sym_ctx *ctx, uint8_t *mac, size_t *maclen);

/**
 * \brief               mac algorithm verify mac
 *
 * \param ctx           sym context 
 * \param input         buffer holding the data
 * \param ilen          length of the input data 
 * \param mac           buffer holding the mac
 * \param maclen        length of the mac 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_sym_mac_verify(dsc_sym_ctx *ctx, uint8_t *mac, size_t maclen);

/**
 * \brief   Symmetric cryptographic algorithm context structure.
 */
typedef struct 
{
    void *contex;
    uint8_t initialized;
    uint32_t session;
    uint32_t reserve;
} dsc_asym_ctx;

/**
 * \brief   big data context structure.
 */
typedef struct
{
    uint8_t *data;
    uint32_t len;
} dsc_big_int;

/**
 * \brief   ecc point context structure.
 */
typedef struct
{
    dsc_big_int x;
    dsc_big_int y;
} dsc_ecc_point;

/**
 * \brief   ecc group context structure.
 */
typedef struct
{
    size_t curve_bits;          /*!< Modulus size in bits */
    dsc_big_int p;             /*!< Curve modulus (p) */
    dsc_big_int a;             /*!< Curve constant a */
    dsc_big_int b;             /*!< Curve constant b */
    dsc_big_int n;             /*!< Curve order (n) */
    dsc_ecc_point base_point;   /*!< Curve base point (G) */
    uint8_t cofactor;           /*!< Curve cofactor (h) */
                                /*!< - Preferably 1, 0 == 1 */
} dsc_ecp_group;

/**
 * \brief   ecdsa sign result context structure.
 */
typedef struct
{
    dsc_big_int r;             /*!< Signature component r */
    dsc_big_int s;             /*!< Signature component s */
} dsc_ecdsa_sig;

/**
 * \brief               Initialize Symmetric cryptographic algorithm context structure.
 *
 * \param ctx           asym context 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_init(dsc_asym_ctx *ctx);

/**
 * \brief               Clear Symmetric cryptographic algorithm context structure.
 *
 * \param ctx           asym context 
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_free(dsc_asym_ctx *ctx);

/**
 * \brief               rsa set public key
 *
 * \param ctx           asym context 
 * \param n             module 
 * \param nlen          length of module 
 * \param e             public key
 * \param elen          length of public key 
 * \param hash_algo     hash algorithm
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_rsa_set_pubkey(dsc_asym_ctx *ctx, const uint8_t *n, size_t nlen, const uint8_t *e, size_t elen, dsc_sym_algo hash_algo);

/**
 * \brief               rsa set private key
 *
 * \param ctx           asym context 
 * \param n             module 
 * \param nlen          length of module 
 * \param d             private key
 * \param dlen          length of the private key 
 * \param hash_algo     hash algorithm
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_rsa_set_prikey(dsc_asym_ctx *ctx, const uint8_t *n, size_t nlen, const uint8_t *d, size_t dlen, dsc_sym_algo hash_algo);

/**
 * \brief               rsa signature
 *
 * \param ctx           asym context 
 * \param msg          buffer holding the msg
 * \param msglen       length of msg 
 * \param sig           buffer holding the signature
 * \param siglen        length of the signature
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_rsa_pkcs1v1_5_sign(dsc_asym_ctx *ctx, const uint8_t *msg, size_t msglen, uint8_t *sig, uint32_t *siglen);

/**
 * \brief               rsa verify
 *
 * \param ctx           asym context 
 * \param msg          buffer holding the msg
 * \param msglen       length of msg 
 * \param sig           buffer holding the signature
 * \param siglen        length of the signature
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_rsa_pkcs1v1_5_verify(dsc_asym_ctx *ctx, const uint8_t *msg, size_t msglen, const uint8_t *sig, uint32_t siglen);

/**
 * \brief               sm2 set public key
 *
 * \param ctx           asym context 
 * \param pubkey        buffer holding the public key
 * \param size          length of the public key
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_sm2_set_public_key(dsc_asym_ctx *ctx, const uint8_t *pubkey, uint32_t size);

/**
 * \brief               sm2 set private key
 *
 * \param ctx           asym context 
 * \param prikey        buffer holding the private key
 * \param size          length of the private key
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_sm2_set_private_key(dsc_asym_ctx *ctx, const uint8_t *prikey, uint32_t size);

/**
 * \brief               sm2 signature
 *
 * \param ctx           asym context 
 * \param hash           buffer holding the hash
 * \param hashlen        length of the hash
 * \param sig           buffer holding the signature
 * \param siglen        length of the signature
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_sm2_sign(dsc_asym_ctx *ctx, const uint8_t *hash, uint32_t hashlen, uint8_t *sig, uint32_t *siglen);

/**
 * \brief               sm2 verify
 *
 * \param ctx           asym context 
 * \param hash          buffer holding the hash
 * \param hashlen        length of the hash
 * \param sig           buffer holding the signature
 * \param siglen        length of the signature
 *
 * \return              see DSC_STATUS enum
 */
int32_t dsc_asym_sm2_verify(dsc_asym_ctx *ctx, const uint8_t *hash, uint32_t hashlen, const uint8_t *sig, uint32_t siglen);

#ifdef __cplusplus
} // extern "c"
#endif

#endif /* __DSC_H__ */
