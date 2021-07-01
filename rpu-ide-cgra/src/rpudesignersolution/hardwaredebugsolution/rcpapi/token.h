#ifndef __TOKEN_HEADER__
#define  __TOKEN_HEADER__

#include "types.h" // for u32/u8/u16

#ifdef __GNUC__
#define _PACKED_ __attribute__ ((packed,aligned(4)))
#else
#define _PACKED_
#pragma pack(push, 1)
#endif

/* FIXME : to  access bitfields struct member using 32bit instruction 
 *      add -fstrict-volatile-bitfields  to CFLAGS
*/


typedef enum {
    VAULTIP             = 0x0,
    
    RPU_START           = VAULTIP,    
    RPU_DMA             = 0x1,
    RPU_CRYPTO          = 0x2,
    RPU_HASH            = 0x3,
    RPU_MAC             = 0x4,
    RPU_DEBUG           = 0x5,
    RPU_END             = RPU_DEBUG,
    
    DCS_START           = RPU_END,
    DCS_TRNG            = 0x0E,
    DCS_ALGO            = 0x0F,
    DCS_END             = DCS_ALGO,
    
    MGR                 = 0x10,
    //SDV_TEST            = 0x11,
    //VIP_REG_OP          = 0x1c,
    FIXED_ALGO          = 0x1F,
    NUM_GLOBAL_OP
} enum_global_op; // size 4 bit


#define REPLAY_OFFSET (8*1024)
#define MAX_TOKEN_SIZE (64*4)
#define mid2offset(id) ( (id)*MAX_TOKEN_SIZE )
#define offset2mid(offset) ( (offset)/MAX_TOKEN_SIZE )
#define TRIGGER_VALUE(id)  (  ((id) << 16) | (mid2offset((id))&0xFFFF))
#define TRIGGER_VALUE_2_MID(value)  ( (value >> 16) &0xFFFF )
#define TRIGGER_VALUE_IS_VALID(value)  (mid2offset(TRIGGER_VALUE_2_MID((value))) == ((value)&0xFFFF))

typedef enum { 
    HASH_ONETIME    = 0x0,
    HASH_FINAL      = 0x1,   
    HASH_INITIAL    = 0x2,
    HASH_UPDATE     = 0x3,    
} enum_hash_mode; // size 2 bits, one time, intermediate, initial, final

typedef enum
{
    RCP_WRITE               = 0x0,
    RCP_READ                = 0x1,
} enum_rcp_r_w;

typedef enum {
    RESULT_NO_ERROR     = 0x00,
    INVALID_OPERATION   = 0x01,
    INVALID_SUB_OP      = 0x02,
    INVALID_CMD_LEN     = 0x03,    
    INVALID_DATA_LEN    = 0x04,    
    INVALID_DATA_ADDR   = 0x05,
    RX_QUEUE_FULL       = 0x1F,
} enum_firmware_result; // size 5 bit

typedef enum {
	SEE_VAULTIP = 0x0,
	RPU_SRC = 0x1,
	DCS_SRC = 0x2,
	MGR_SRC = 0x3,
	DEV_SRC = 0x4,
	RPU_DEBUG_SRC = 0x5,
	FIXED_ALGO_SRC = 0x6,
	FIRMWARE = 0x7,
} enum_global_resultsrc; // size 2 bit


#define TOKEN_PRIORITY_LOW      0
#define TOKEN_PRIORITY_MID_1    1
#define TOKEN_PRIORITY_MID_2    2
#define TOKEN_PRIORITY_HI       3

#ifdef __GNUC__
#define TOKEN2DEV_COMM_HDR  \
    u32 seqno       : 16; /*0-15*/  \
    u32 priority    : 2;  /*16-17*/  \
    u32 dma_cmd_id  : 1;  /*18-18*/  \
    u32 global_op   : 5;  /*19-23*/  \
    u32 vaultip_op  : 4;  /*24-27*/  \
    u32 sub_op      : 4;  /*28-31*/  \
    u32 command_len ;  /*0-13*/   
#else
#define TOKEN2DEV_COMM_HDR  \
    u32 command;            \
    u32 command_len
#endif

struct token2dev_hdr {
    TOKEN2DEV_COMM_HDR;
} _PACKED_; 


#define TOKEN_RESULT(error,src,result)  ((unsigned int)( (src)<<16 | ((result)&0x1f) << 19 | (error) << 31))
#define TOKEN_RESULT_ERROR(value)       (((unsigned int)(value)) >> 31)
#define VIP_TOKEN_RESULT(error,src,result) ((unsigned int)( (src)<<24 | ((result)&0b11111) << 29 | (error) << 31))

#define token_global_op(token)  ((((volatile unsigned int*)(token))[0] >> 19) & 0x1f)
#define token_sub_op(token)     ((((volatile unsigned int*)(token))[0] >> 28) & 0x0f)

#define token_hdr(ptr) ((struct token2dev_hdr*)(ptr))

#define tokenlen(token) ((u32*)(token))[1]

enum {
	VAULIP_RESULT_SRC_FIRMWARE = 0,
	VAULIP_RESULT_SRC_DMA,
	VAULIP_RESULT_SRC_TRNG,
	VAULIP_RESULT_SRC_PANIC,
};

#ifdef __GNUC__
#define TOKEN2XEON_COMM_HDR         \
    u32 seqno               : 16; /*0-15*/     \
    u32 global_resultsrc    : 3 ; /*16-18*/    \
    u32 global_result       : 5 ; /*19-23*/    \
    u32 vaultip_result      : 5 ; /*24-28*/    \
    u32 vaultip_resultsrc   : 2 ; /*29-30*/    \
    u32 error               : 1 ; /*31*/       \
    u32 command_len          ; /*0-13*/ 
#else
#define TOKEN2XEON_COMM_HDR         \
    u32 command;                    \
    u32 command_len
#endif

struct token2xeon_hdr
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_; // word 0 & 1

#define reply_token_stauts(token) *((volatile u32*)(token))
#define set_reply_token_len(token, len) (((volatile u32*)(token))[1] = (len))
#define token_seqno(req)                (((((volatile u32*)(req))[0]) >> 00) & 0xffff)
#define token_error(req)            (((((volatile u32*)(req))[0]) >> 31) & 0x1)
#define token_g_result(req)         (((((volatile u32*)(req))[0]) >> 19) & 0x1f)
#define token_g_result_src(req)     (((((volatile u32*)(req))[0]) >> 16) & 0x7)

#define WORD0(header) (((volatile u32*)header)[0])
#define WORD1(header) (((volatile u32*)header)[1])


//======= dma token======

struct dma2dev_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 dma_out_addr_low;
    u32 dma_out_addr_high;
    u32 dma_out_data_len;       // in bytes
} _PACKED_;

struct dma2dev_reply
{
    TOKEN2XEON_COMM_HDR;
}_PACKED_;

//==== RPU CRYPTO =======
struct crypto_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 dma_out_addr_low;
    u32 dma_out_addr_high;
    u32 dma_out_data_len;       // in bytes
#define ALGO_AES            0x0
#define ALGO_SM4            0x1
#define ALGO_MISTY          0x2
#define ALGO_TWOFISH        0x3
#define ALGO_SERPENT        0x4
#define ALGO_DES            0x5
#define ALGO_3DES           0x6
#define ALGO_CAMELLIA       0x7
#define ALGO_SHACAL2        0x8
#define ALGO_SEED           0x9
#define ALGO_CAST           0xA
#define ALGO_BLOWFISH       0xB
    // Stream
#define ALGO_ZUC            0xC
#define ALGO_SNOW3G         0xD
    u32 algo ;               // word 8
    u32 algo_mode;
    u32 key_len ;            // in bytes
    // HASH_FINAL | HASH_ONETIME| HASH_INITIAL | HASH_UPDATE
    u32 mode ;
#define DECRYPT            0x0
#define ENCRYPT            0x1
    u32 encrypt;            // 1 : encryption; 0 : decryption
    u32 iv[4];                  // 16 bytes
    u32 key[16];                // 64 bytes
} _PACKED_;

struct crypto_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 iv[4];                  // 16 bytes
} _PACKED_;

typedef enum {
    CRYPTO_WAIT_INITIAL,
    CRYPTO_WAIT_UPDATE,
    CRYPTO_WAIT_UPDATE_OR_FINAL,
} enum_crypto_mode_state;
    
enum SYN_MODE{
    ECB_MODE    = 0x0,
    CBC_MODE    = 0x1,
    CFB64_MODE  = 0x2,
    OFB64_MODE  = 0x3,
    CFB128_MODE = 0x4,
    OFB128_MODE = 0x5,
    CTR128_MODE = 0x6,
};

//==== RPU HASH =======
struct hash_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes

#define ALGO_MD5                0x0
#define ALGO_SHA256             0x1
#define ALGO_SM3                0x2
#define ALGO_MDC2               0x3
    u32 algo ;               // word 5
            // HASH_ONETIME | HASH_FINAL | HASH_INITIAL | HASH_UPDATE
    u32 mode ;               // one time, intermediate, initial, final
    u32 digest[16];             // 64 bytes
} _PACKED_;

struct hash_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 digest[16];             // 64 bytes
} _PACKED_;

typedef enum {
    HASH_WAIT_INITIAL,
    HASH_WAIT_UPDATE,
    HASH_WAIT_UPDATE_OR_FINAL,
} enum_hash_mode_state;

//==== RPU MAC =======
typedef enum
{
    RPU_RESULT_NO_ERROR = 0x00,
    INVALID_ALGO        = 0x01,    
    INVALID_MODE        = 0x02,    
    INVALID_KEY_LEN     = 0x03,
    INVALID_HASH_ORDER  = 0x04,
    INVALID_MAC_ORDER   = 0x05,
    INVALID_CRYPTO_ORDER= 0x06,
    RPU_HW_ERROR        = 0x07,
    RPU_ERR_SETKEY,
    RPU_MODE_NOT_SUPPORT
} enum_rpu_result; // size 5 bit

typedef struct rpu_mac_req {
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
#define    ALGO_HMAC_MD5        0x0
#define    ALGO_HMAC_SHA256     0x1
#define    ALGO_AES_CBC_MAC     0x2
    u32 algo;

            //HASH_ONETIME,HASH_FINAL, HASH_INITIAL, HASH_UPDATE
    u32 mode    ;
    u32 key_len ;        // in bytes
    u32 mac[16];                // 64 bytes
    u32 key[32];                // 128 bytes
} _PACKED_ rpu_mac_req_t;


typedef struct rpu_mac_reply{
    TOKEN2XEON_COMM_HDR;
    u32 mac[16];                // 64 bytes
} _PACKED_ rpu_mac_reply_t;

typedef enum {
    MAC_WAIT_INITIAL,
    MAC_WAIT_UPDATE,
    MAC_WAIT_UPDATE_OR_FINAL,
} enum_mac_mode_state;

//======== fixed algo: SIGN VERIFY ======
// FIXED_ALGO 
typedef enum { 
    FIXED_ALGO_SIGN_VERIFY              = 0x0,
    FIXED_ALGO_HASH                     = 0x1,
    FIXED_ALGO_RSA_SET_KEY              = 0x2,
    FIXED_ALGO_CRYPTO                   = 0x3,
    FIXED_ALGO_MAC                      = 0x4,
    FIXED_ALGO_TRNG                     = 0x5,
} enum_fixed_algo_subop;

typedef enum {
    FIXED_ALGO_NO_ERROR = 0x00,
    RSA_VERIFY_FAIL     = 0x01,
    SM2_VERIFY_FAIL     = 0x02,
    INVALID_F_ALGO      = 0x03,
    INVALID_F_MODE      = 0x04,
    INVALID_F_HASH_ORDER= 0x05,
    INVALID_F_MAC_ORDER = 0x06,
    SET_KEY_ERROR       = 0x07,
    F_HW_ERROR          = 0x08,
    F_NOT_SUPPORTED
} enum_fixed_algo_result; // size 5 bit

struct fixed_algo_sign_verify_reply {
    TOKEN2XEON_COMM_HDR;
};

struct fixed_algo_sign_verify_req {
    TOKEN2DEV_COMM_HDR; // 

    u32 data_in_addr_low;   // 16 bytes align
    u32 data_in_addr_high;
    u32 data_in_len;        // in bytes
    
    u32 sign_in_out_addr_low;   // 16bytes align
    u32 sign_in_out_addr_high;
    u32 sign_in_out_len;        // in bytes
    
    u32 cert_in_addr_low;
    u32 cert_in_addr_high;
    u32 cert_in_len;        // in bytes
    
#define SM2_TEXT_VERIFY     0x0
#define SM2_DIGEST_VERIFY   0x1
#define RSA_TEXT_VERIFY     0x2
#define RSA_DIGEST_VERIFY   0x3
#define SM2_TEXT_SIGN       0x4
#define SM2_DIGEST_SIGN     0x5
#define RSA_TEXT_SIGN       0x6
#define RSA_DIGEST_SIGN     0x7
#define ECC_TEXT_SIGN       0x8
#define ECC_TEXT_VERIFY     0x9
    u32 algo        : 5;    // word 11
    u32 digest_len  : 7;

#define CERT_FLASH          0x0
#define CERT_OTP            0x1
#define CERT_CMD            0x2
#define CERT_SET_KEY        0x3
    u32 cert_loc    : 2;    // certification location
    u32 reserved    : 2;

#define CERT_DER            0x0
#define CERT_PARAM          0x1
#define CERT_BIN            0x2
#define CERT_PEM            0x3
    u32 cert_type   : 8;

#define SIGN_SHA1           0x0
#define SIGN_SHA224         0x1
#define SIGN_SHA256         0x2
    u32 hash_type  : 4;
    u32 reserved1   : 4;          
    u32 digest[64/4];         // 64 bytes
} _PACKED_;

#ifdef __GNUC__
#define req_fixsv_algo(req)         ((req)->algo)
#define req_fixsv_digestlen(req)    ((req)->digest_len)
#define req_fixsv_cert_loc(req)     ((req)->cert_loc)
#define req_fixsv_cert_type(req)    ((req)->cert_type)
#define req_fixsv_hashtype(req)     ((req)->hash_type)
#else
#define req_fixsv_algo(req)         ((((req)->param)>>00) & 0x1f)
#define req_fixsv_digestlen(req)    ((((req)->param)>>05) & 0x7f)
#define req_fixsv_cert_loc(req)     ((((req)->param)>>12) & 0x03)
#define req_fixsv_cert_type(req)    ((((req)->param)>>16) & 0xff)
#define req_fixsv_hashtype(req)     ((((req)->param)>>28) & 0x0f)
#endif

//===== FIXALGO rsa-setkey ===
//
struct rsa_set_key_reply
{
    TOKEN2XEON_COMM_HDR;
};
struct rsa_set_key_req
{
    TOKEN2DEV_COMM_HDR;

    u32 key_in_addr_low;
    u32 key_in_addr_high;
    u32 key_in_len;        // in bytes

#define PUB_KEY             0x0
#define PRI_KEY             0x1
#define PUB_PRI_KEY         0x2
#define DEL_KEY             0x3
    u32 key_mode : 4;           // word 5
#define KEY_DER            0x0
#define KEY_RSA_PARAM      0x1
#define KEY_BIN            0x2
#define KEY_PEM            0x3
    u32 key_type : 4;
    //SIGN_SHA1 | SIGN_SHA224 | SIGN_SHA256
    u32 hash_type : 4;       // certification location
    u32 reserved : 20;
} _PACKED_;

#ifdef __GNUC__
#define req_fix_keymode(req)  ((req)->key_mode)
#define req_fix_keytype(req)  ((req)->key_type)
#define req_fix_hashtype(req) ((req)->hash_type)
#else
#define req_fix_keymode(req)  ((((req)->param)>>0) & 0xf)
#define req_fix_keytype(req)  ((((req)->param)>>4) & 0xf)
#define req_fix_hashtype(req) ((((req)->param)>>8) & 0xf)
#endif

typedef enum {
    RSA_KEY_N               = 0x0, ///< RSA public Modulus (N)
    RSA_KEY_E               = 0x1, ///< RSA Public exponent (e)
    RSA_KEY_D               = 0x2, ///< RSA Private exponent (d)
    RSA_KEY_P               = 0x3, ///< RSA secret prime factor of Modulus (p)
    RSA_KEY_Q               = 0x4, ///< RSA secret prime factor of Modules (q)
    RSA_KEY_Dp              = 0x5, ///< p's CRT exponent (== d mod (p - 1))
    RSA_KEY_Dq              = 0x6, ///< q's CRT exponent (== d mod (q - 1))
    RSA_KEY_QInv            = 0x7, ///< The CRT coefficient (== 1/q mod p)
} enum_rsa_key_tag;

//===== rpu fixalgo hash =======
struct fixalgo_hash_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
#define F_ALGO_SHA256             0x0
#define F_ALGO_SM3                0x1
#define F_ALGO_SHA1               0x2
#define F_ALGO_SHA384             0x3
#define F_ALGO_SHA512             0x4
#define F_ALGO_MD5                0x5
#define F_ALGO_MD4                0x6
#define F_ALGO_SHA224             0x7
    u32 algo : 5;               // word 5
    //HASH_ONETIME,HASH_FINAL, HASH_INITIAL, HASH_UPDATE
    u32 mode : 2;               // one time, intermediate, initial, final
    u32 reserved : 25;          
    u32 digest[16];             // 64 bytes
} _PACKED_;

struct fixalgo_hash_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 digest[16];             // 64 bytes
} _PACKED_;


//=== fixed algo mac ===
struct fixalgo_mac_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 key_in_addr_low;
    u32 key_in_addr_high;
    u32 key_in_data_len;        // in bytes

#define F_ALGO_HMAC_SHA1           0x0
#define F_ALGO_HMAC_SHA224         0x1
#define F_ALGO_HMAC_SHA256         0x2
#define F_ALGO_HMAC_MD5            0x3
    u32 algo : 5;               // word 8

    // HASH_ONETIME | HASH_FINAL | HASH_INITIAL | HASH_UPDATE
    u32 mode : 2;               // one time, intermediate, initial, final
    u32 reserved : 25;           
    u32 mac[16];                // 64 bytes
} _PACKED_;

#ifdef __GNUC__
#define req_fixsv_mode(req)         ((req)->mode)
#else
#define req_fixsv_mode(req)         ((((req)->param)>>05) & 0x3)
#endif
struct fixalgo_mac_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 mac[16];                // 64 bytes
} _PACKED_;


// fixalgo crypto ===
struct fixed_algo_crypto_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 dma_out_addr_low;
    u32 dma_out_addr_high;
    u32 dma_out_data_len;       // in bytes

#define F_ALGO_AES      0x0
#define F_ALGO_SM4      0x1
#define F_ALGO_3DES     0x2
            u32 algo : 5;               // word 8

#define F_CRYPTO_ECB    0x0
#define F_CRYPTO_CBC    0x1
            u32 algo_mode : 4;
            u32 key_len : 7;            // in bytes
    u32 reserved : 15;

#define F_DECRYPT       0x0
#define F_ENCRYPT       0x1
            u32 encrypt : 1;            // 1 : encryption; 0 : decryption
    u32 iv[4];                  // 16 bytes
    u32 key[16];                // 64 bytes
} _PACKED_;

#ifdef __GNUC__
#define req_fixsv_keylen(req)          ((req)->key_len)
#define req_fixsv_algo_mode(req)       ((req)->algo_mode)
#define req_fixsv_encrypt(req)         ((req)->encrypt)
#else
#define req_fixsv_keylen(req)          ((((req)->param)>>9) & 0x7f)
#define req_fixsv_algo_mode(req)       ((((req)->param)>>05) & 0xf)
#define req_fixsv_encrypt(req)         ((((req)->param)>>31) & 0x1)
#endif

struct fixed_algo_crypto_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 iv[4];                  // 16 bytes
} _PACKED_;

// fixalgo crypto ===
struct fixed_algo_trng_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_out_addr_low;
    u32 dma_out_addr_high;
    u32 dma_out_data_len;       // in bytes
} _PACKED_;

struct fixed_algo_trng_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;


//==== dcs crypto ===
typedef enum { 
    DCS_ALGO_SM3                        = 0x0,
    DCS_ALGO_SM4                        = 0x1,
    DCS_ALGO_SIGN_VERIFY                = 0x2,    
    DCS_ALGO_SIGN                       = 0x3,
} enum_dcs_algo_subop;


typedef enum {
    DCS_RESULT_NO_ERROR     = 0x00,
    INVALID_D_ALGO          = 0x01,    
    INVALID_D_MODE          = 0x02,    
    INVALID_D_KEY_LEN       = 0x03,
    INVALID_D_HASH_ORDER    = 0x04,
    INVALID_D_CERT          = 0x05,
    INVALID_D_CRYPTO_ORDER  = 0x06,
    DCS_HW_ERROR            = 0x07,
    D_SM2_HS_VERIFY_FAIL    = 0x08,
    D_SM2_LP_VERIFY_FAIL    = 0x09,
    DCS_NO_SUPPORTED        = 0x10,
} enum_dcs_algo_result; // size 5 bit

struct dcs_crypto_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 dma_out_addr_low;
    u32 dma_out_addr_high;
    u32 dma_out_data_len;       // in bytes

#define DCS_ALGO_SM4_0      0x0
#define DCS_ALGO_SM4_MASK   0x1
    u32 algo ;               // word 8
#define D_CRYPTO_ECB 0x0
#define D_CRYPTO_CBC 0x1
    u32 algo_mode ;
    u32 key_len ;            // in bytes

            //HASH_ONETIME,HASH_FINAL, HASH_INITIAL, HASH_UPDATE
    u32 mode ;

#define D_DECRYPT 0x0
#define D_ENCRYPT 0x1
    u32 encrypt;            // 1 : encryption; 0 : decryption
    u32 iv[4];                  // 16 bytes
    u32 key[16];                // 64 bytes
    u32 mask[16];               // 64 bytes
} _PACKED_;

#define req_dcs_hmode(req) ((req)->mode)

struct dcs_crypto_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 iv[4];                  // 16 bytes
} _PACKED_;

typedef enum {
    D_CRYPTO_WAIT_INITIAL,
    D_CRYPTO_WAIT_UPDATE,
    D_CRYPTO_WAIT_UPDATE_OR_FINAL,
} enum_dcs_crypto_mode_state;


// DCS HASH
struct dcs_hash_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes

#define DCS_ALGO_SM3_0 0x0
#define DCS_ALGO_SM3_2 0x1
    u32 algo ;               // word 5

            //HASH_ONETIME,HASH_FINAL, HASH_INITIAL, HASH_UPDATE
    u32 mode ;               // one time, intermediate, initial, final
    u32 digest[16];             // 64 bytes
} _PACKED_;

struct dcs_hash_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 digest[16];             // 64 bytes
} _PACKED_;

// ======== dcs sign verify ===
struct dcs_sign_verify_req
{
    TOKEN2DEV_COMM_HDR;

    u32 data_in_addr_low;
    u32 data_in_addr_high;
    u32 data_in_len;        // in bytes
    
    u32 sign_in_addr_low;
    u32 sign_in_addr_high;
    u32 sign_in_len;        // in bytes
    
    u32 cert_in_addr_low;
    u32 cert_in_addr_high;
    u32 cert_in_len;        // in bytes

#define D_SIGN_VERIFY_SM2_HS 0x0
#define D_SIGN_VERIFY_SM2_LP 0x1
    u32 algo ;           // word 11
#define D_SIGN_HASH_NONE 0x0
#define D_SIGN_HASH_SM3_0 0x1
#define D_SIGN_HASH_SM3_2 0x2
    u32 hash_type ;
#define D_NOT_DO_HASH 0x0
#define D_DO_HASH 0x1
    u32 do_hash ;
#define D_CERT_FLASH    0x0
#define D_CERT_OTP      0x1
#define D_CERT_CMD      0x2
    u32 cert_loc ;       // certification location

#define D_CERT_DER      0x0
#define D_CERT_BIN      0x1
    u32 cert_type ;
} _PACKED_;

struct dcs_sign_verify_reply
{
    TOKEN2XEON_COMM_HDR;
};

struct dcs_sign_reply
{
    TOKEN2XEON_COMM_HDR;
};
struct dcs_sign_req
{
    TOKEN2DEV_COMM_HDR;

    u32 data_in_addr_low;
    u32 data_in_addr_high;
    u32 data_in_len;        // in bytes
    
    u32 sign_out_addr_low;
    u32 sign_out_addr_high;
    u32 sign_out_len;        // in bytes
    
    u32 cert_in_addr_low;
    u32 cert_in_addr_high;
    u32 cert_in_len;        // in bytes

#define D_SIGN_SM2_HS 0x0
#define D_SIGN_SM2_LP 0x1
    u32 algo ;           // word 11
            // D_SIGN_HASH_NONE | D_SIGN_HASH_SM3_0 | D_SIGN_HASH_SM3_2
    u32 hash_type ;
            // D_NOT_DO_HASH | D_DO_HASH
    u32 do_hash ;
            // D_CERT_FLASH | D_CERT_OTP | D_CERT_CMD
    u32 cert_loc ;       // certification location
            // D_CERT_DER |  D_CERT_BIN
    u32 cert_type ;
} _PACKED_;

#define req_cert_type(req)  ((req)->cert_type)
#define req_cert_loc(req)   ((req)->cert_loc)
#define req_cert_type(req)  ((req)->cert_type)
#define req_dohash(req)     ((req)->do_hash)
#define req_hashtype(req)   ((req)->hash_type)
#define req_algo(req)       ((req)->algo)
#define req_hmode(req)      ((req)->mode)
#define req_doencrypt(req)  ((req)->encrypt)


//========= VAULITP NOP ============
typedef struct nop_req
{
    u32 _seqno       : 16;
    u32 _priority    : 2 ;
    u32 _dma_cmd_id  : 1 ;
    u32 _global_op   : 5 ;
    u32 _vaultip_op  : 4 ;
    u32 _sub_op      : 4 ;
    u32 identy          ;
    u32 datalen         ;
    u32 inputdata_addr_lo;
    u32 inputdata_addr_hi;
    //u32 inputdatalen: 21;
    //u32 rev         : 11;
    u32 inputdatalen;
    u32 outputdata_addr_lo;
    u32 outputdata_addr_hi;
    //u32 outputdatalen:21;
    //u32 rev2        : 11;
    u32 outputdatalen;
} _PACKED_ nop_req_t; 

typedef struct nop_reply {
    u32 _seqno               : 16;
    u32 _global_resultsrc    : 3 ;
    u32 _global_result       : 5 ;
    u32 _vaultip_result      : 5 ;
    u32 _vaultip_resultsrc   : 2 ;
    u32 _error               : 1 ;
}nop_reply_t;


//
//=========  manager ===
//
typedef enum { 
    MGR_FWUPD               = 0x0,
    MGR_BIOS_DDR_REMAP      = 0x1,/*for bios fixalgo ddr remap*/
    MGR_PCIE_INT_SET        = 0x2,
    MGR_RCP_DDR_REMAP       = 0x3,
    MGR_BIOS_OTP            = 0x4,
    MGR_SYS_CTRL            = 0x5,
} enum_mgr_subop;

typedef enum {
    MGR_RESULT_NO_ERROR  = 0x00,
    INVALID_FWUPD_MODE      ,
    INVALID_DATA_CRC        ,
    MGR_NOT_SUPPORTED       ,
    MGR_REMAP_ERROR         ,
    MGR_REMAP_LEN_ERR       ,
    MGR_REMAP_CNT_ERR       ,
    MGR_REMAP_CROSS_4GB     ,
    MGR_REMAP_OTHER_ERROR   ,
    BIOS_OTP_WRITE_FAIL     ,
} enum_mgr_result; // size 5 bit

#define RCP_REMAP_DO_MAP   0
#define RCP_REMAP_DO_UNMAP 1

struct ddr_bios_remap_req
{
    TOKEN2DEV_COMM_HDR;

    u32 unremap;
    u32 remap_addr_low;
    u32 remap_addr_high;
    u32 remap_size;        // in bytes
} _PACKED_;

struct ddr_bios_remap_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_ ;

struct mgr_int_set_req
{
    TOKEN2DEV_COMM_HDR;

    u32 msi_int_addr_low;
    u32 msi_int_addr_high;
    u32 msi_int_value;
} _PACKED_;

struct mgr_int_set_reply{
    TOKEN2XEON_COMM_HDR;
};

struct remap_addr_info {
    u32 remap_addr_low;
    u32 remap_addr_high;
    u32 remap_size;        // in bytes
};
struct ddr_rcp_remap_req
{
    TOKEN2DEV_COMM_HDR;

    u32 unremap;
    u32 segcnt;
    struct remap_addr_info seg[0];
} _PACKED_;

struct ddr_rcp_remap_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 ckaddr[0];
} _PACKED_ ;


struct mgr_fwupd_reply {
    TOKEN2XEON_COMM_HDR;
};

struct mgr_fwupd_req
{
    TOKEN2DEV_COMM_HDR;

    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    
#define MGR_FWUPD_ONETIME   0x0
#define MGR_FWUPD_FINAL     0x1
#define MGR_FWUPD_INITIAL   0x2
#define MGR_FWUPD_UPDATE    0x3
    u32 mode ;               // one time, intermediate, initial, final
    u32 in_data_crc;
} _PACKED_;

struct mgr_bios_otp_req
{
    TOKEN2DEV_COMM_HDR;
    u32 r_w;
    u32 otp_data_in[4];
} _PACKED_;

struct mgr_bios_otp_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 otp_data_out[4];
} _PACKED_;

/* System control request */
#define SYS_INFO_MAGIC 0x53694e66 /* SiNf */
#define SYS_FEATURE_VIP 0x00000001
#define SYS_FEATRUE_DCS 0x00000002
#define SYS_FEATRUE_RPU 0x00000004
typedef struct sys_info
{
	u32 magic;
	u32 features;
	u32 version;
	u32 commit[5];
	u32 checksum;
} _PACKED_ sys_info_t;

struct mgr_sysctrl_req
{
    TOKEN2DEV_COMM_HDR;
#define MGR_SYSCTRL_GETINFO 0xc0
    u32 ctrlcode;
} _PACKED_;

struct mgr_sysctrl_reply
{
    TOKEN2XEON_COMM_HDR;
    union
    {
      struct sys_info sys_info;
    } u ;
} _PACKED_;

//======= rpu debug======
typedef enum
{
    ENTER_DBG                       = 0x0,
    RESET_RPU                       = 0x1,
    INIT_CFG                        = 0x2,
    INIT_SBOX                       = 0x3,
    W_GPRF                          = 0x4,
    RW_FIFO                         = 0x5,
    START_RPU                       = 0x6,
    RUN_N_CYCLE                     = 0x7,
    RW_PE                           = 0x8,
    READ_BCU_PE                     = 0x9,
    READ_FIFO_STATUS                = 0xA,
    READ_RCA_STATUS                 = 0xB,
    EXIT_DBG                        = 0xC,
} enum_rpu_debug_subop;

struct rpu_common_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct rpu_asym_read_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 payload[4];
} _PACKED_;

typedef enum
{
    RPU_DEBUG_RESULT_NO_ERROR = 0x00,
    RPU_DEBUG_TIME_OUT        = 0X01,
} enum_rpu_debug_result; // size 5 bit

struct enter_dbg_req
{
    TOKEN2DEV_COMM_HDR;
} _PACKED_;

struct enter_dbg_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct reset_rpu_req
{
    TOKEN2DEV_COMM_HDR;
} _PACKED_;

struct reset_rpu_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct init_cfg_req
{
    TOKEN2DEV_COMM_HDR;
    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 cmd_bits_len;           // in bytes
} _PACKED_;

struct init_cfg_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct init_sbox_req
{
    TOKEN2DEV_COMM_HDR;
    u32 sbox_data_addr_low;
    u32 sbox_data_addr_high;
    u32 sbox_data_len;        // in bytes
    u32 sbox_index_addr_low;
    u32 sbox_index_addr_high;
    u32 sbox_index_len;        // in bytes
    u32 sbox_grp_ofs_addr_low;
    u32 sbox_grp_ofs_addr_high;
    u32 sbox_grp_ofs_len;        // in bytes
} _PACKED_;

struct init_sbox_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct w_gprf_req
{
    TOKEN2DEV_COMM_HDR;
    u32 dma_in_addr_low;
    u32 dma_in_addr_high;
    u32 dma_in_data_len;        // in bytes
    u32 gprf_mem_ofs;
    u32 gprf_mem_num;
} _PACKED_;

struct w_gprf_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct r_w_fifo_req
{
    TOKEN2DEV_COMM_HDR;
    u32 dma_in_out_addr_low;
    u32 dma_in_out_addr_high;
    u32 dma_in_out_data_len;        // in bytes
    u32 fifo_id;
    u32 r_w;
} _PACKED_;

struct r_w_fifo_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct start_rpu_req
{
    TOKEN2DEV_COMM_HDR;
    u32 execute_times;
    u32 cmd_start_num;
    u32 cmd_end_num;
} _PACKED_;

struct start_rpu_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct run_n_cycles_req
{
    TOKEN2DEV_COMM_HDR;
    u32 cycle_num;
} _PACKED_;

struct run_n_cycles_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct r_w_pe_req
{
    TOKEN2DEV_COMM_HDR;
    u32 bcu_id;
    u32 rcu_id;
    u32 pe_id;
    u32 r_w;
    u32 data_in[4];
} _PACKED_;

struct r_w_pe_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 data_out[4];
} _PACKED_;

struct read_bcu_pe_req
{
    TOKEN2DEV_COMM_HDR;
    u32 dma_out_addr_low;
    u32 dma_out_addr_high;
    u32 dma_out_data_len;        // in bytes
    u32 bcu_id;
} _PACKED_;

struct read_bcu_pe_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

struct read_fifo_status_req
{
    TOKEN2DEV_COMM_HDR;
} _PACKED_;

struct read_fifo_status_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 fifo_status;
} _PACKED_;

struct read_rca_status_req
{
    TOKEN2DEV_COMM_HDR;
} _PACKED_;

struct read_rca_status_reply
{
    TOKEN2XEON_COMM_HDR;
    u32 rca_status;
} _PACKED_;

struct exit_dbg_req
{
    TOKEN2DEV_COMM_HDR;
} _PACKED_;

struct exit_dbg_reply
{
    TOKEN2XEON_COMM_HDR;
} _PACKED_;

typedef enum
{
    RPU_DBG_WRITE               = 0x0,
    RPU_DBG_READ                = 0x1,
} enum_rpu_dbg_r_w;




#ifdef __GNUC__
#else
#pragma pack(pop)
#endif

#endif
