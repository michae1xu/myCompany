/**
 * @brief       : 
 *
 * @file        : api_typedef.h
 * @version     : v0.0.1
 * @date        : 2017.09.06

 */
#ifndef __API_TYPEDEF_H__
#define __API_TYPEDEF_H__

#include <stdint.h>

#ifndef EXPORT
#if defined(RCP_MSWINDOWS)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif

#define RCP_DEBUG   1

#if RCP_DEBUG
#include <stdio.h>
#define RCP_DEBUG_LOG printf
#else
#define RCP_DEBUG_LOG(...)
#endif

typedef int RCP_STATUS;

typedef enum 
{
    /** [1 .. 127]: The reserved for propagating the VaultIP firmware warnings */

    /** TRNG related warnings [64 ... 95]:\n
     * - Aprop fail\n
     * - Repcount fail\n
     * - Stuck NRBG\n
     * - Stuck output */

    /** DMA related warnings [32 ... 63]: None expected */

    /** Firmware related warnings [1 ... 31]: */
    RCP_VAL_WARNING_Z1USED = 16,            /** Z=1 is used */

    /** Success */
    RCP_VAL_SUCCESS = 0,                    /** No error */

    /** [-1 .. -137]: The reserved for propagating the VaultIP firmware error */
    /** Firmware related errors [-1 ... -31]: */
    RCP_VAL_INVALID_TOKEN = -1,             /** Invalid token */
    RCP_VAL_INVALID_PARAMETER = -2,         /** Invalid parameter */
    RCP_VAL_INVALID_KEYSIZE = -3,           /** Invalid key size */
    RCP_VAL_INVALID_LENGTH = -4,            /** Invalid length */
    RCP_VAL_INVALID_LOCATION = -5,          /** Invalid location */
    RCP_VAL_CLOCK_ERROR = -6,               /** Clock error */
    RCP_VAL_ACCESS_ERROR = -7,              /** Access error */
    RCP_VAL_UNWRAP_ERROR = -10,             /** Unwrap error */
    RCP_VAL_DATA_OVERRUN_ERROR = -11,       /** Data overrun error */
    RCP_VAL_ASSET_CHECKSUM_ERROR = -12,     /** Asset checksum error */
    RCP_VAL_INVALID_ASSET = -13,            /** Invalid Asset */
    RCP_VAL_FULL_ERROR = -14,               /** Full/Overflow error */
    RCP_VAL_INVALID_ADDRESS = -15,          /** Invalid address */
    RCP_VAL_INVALID_MODULUS = -17,          /** Invalid Modulus */
    RCP_VAL_VERIFY_ERROR = -18,             /** Verify error */
    RCP_VAL_INVALID_STATE = -19,            /** Invalid state */
    RCP_VAL_OTP_WRITE_ERROR = -20,          /** OTP write error */
    RCP_VAL_PANIC_ERROR = -31,              /** Panic error */

    /** DMA related errors are bus related [-32 ... -63] */

    /** TRNG related errors [-64 ... -95]: */
    RCP_VAL_TRNG_SHUTDOWN_ERROR = -65,      /** Too many FROs shutdown */
    RCP_VAL_DRBG_STUCK_ERROR = -66,         /** Stuck DRBG */

    /** VAL related errors [-128 ... -137]: */
    RCP_VAL_UNSUPPORTED = -128,             /** Not supported */
    RCP_VAL_NOT_INITIALIZED = -129,         /** Not initialized yet */
    RCP_VAL_BAD_ARGUMENT = -130,            /** Wrong use; not depending on configuration */
    RCP_VAL_INVALID_ALGORITHM = -131,       /** Invalid algorithm code */
    RCP_VAL_INVALID_MODE = -132,            /** Invalid mode code */
    RCP_VAL_BUFFER_TOO_SMALL = -133,        /** Provided buffer too small for intended use */
    RCP_VAL_NO_MEMORY = -134,               /** No memory */
    RCP_VAL_OPERATION_FAILED = -135,        /** Operation failed */
    RCP_VAL_TIMEOUT_ERROR = -136,           /** Token or data timeout error */
    RCP_VAL_INTERNAL_ERROR = -137,          /** Internal error */	
} VAL_CODE;

typedef enum
{
    API_SUCCESS = 0x200, 
    API_FAILED, 
    API_NULL_PARAM,
    API_PARAM_ERR,
    API_STATUS_TIMEOUT,
    API_DEVICE_NOT_FOUND,
    // ...
} API_CODE;

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif
