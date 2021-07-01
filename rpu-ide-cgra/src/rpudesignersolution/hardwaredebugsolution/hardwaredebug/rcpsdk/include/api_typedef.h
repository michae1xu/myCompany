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
#include <stdio.h>


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

typedef enum 
{
    /** Success */
    RCP_SUCCESS = 0,                    /** No error */

    /** Firmware related errors [-1 ... -31]: */
    RCP_INVALID_TOKEN = -1,             /** Invalid token */
    RCP_INVALID_PARAMETER = -2,         /** Invalid parameter */
    RCP_INVALID_KEYSIZE = -3,           /** Invalid key size */
    RCP_INVALID_LENGTH = -4,            /** Invalid length */
    RCP_INVALID_LOCATION = -5,          /** Invalid location */
    RCP_CLOCK_ERROR = -6,               /** Clock error */
    RCP_ACCESS_ERROR = -7,              /** Access error */
    RCP_UNWRAP_ERROR = -10,             /** Unwrap error */
    RCP_DATA_OVERRUN_ERROR = -11,       /** Data overrun error */
    RCP_ASSET_CHECKSUM_ERROR = -12,     /** Asset checksum error */
    RCP_INVALID_ASSET = -13,            /** Invalid Asset */
    RCP_FULL_ERROR = -14,               /** Full/Overflow error */
    RCP_INVALID_ADDRESS = -15,          /** Invalid address */
    RCP_INVALID_MODULUS = -17,          /** Invalid Modulus */
    RCP_VERIFY_ERROR = -18,             /** Verify error */
    RCP_INVALID_STATE = -19,            /** Invalid state */
    RCP_OTP_WRITE_ERROR = -20,          /** OTP write error */
    RCP_PANIC_ERROR = -31,              /** Panic error */

    /** DMA related errors are bus related [-32 ... -63] */

    /** TRNG related errors [-64 ... -95]: */
    RCP_TRNG_SHUTDOWN_ERROR = -65,      /** Too many FROs shutdown */
    RCP_DRBG_STUCK_ERROR = -66,         /** Stuck DRBG */

    /** VAL related errors [-128 ... -137]: */
    RCP_UNSUPPORTED = -128,             /** Not supported */
    RCP_NOT_INITIALIZED = -129,         /** Not initialized yet */
    RCP_BAD_ARGUMENT = -130,            /** Wrong use; not depending on configuration */
    RCP_INVALID_ALGORITHM = -131,       /** Invalid algorithm code */
    RCP_INVALID_MODE = -132,            /** Invalid mode code */
    RCP_BUFFER_TOO_SMALL = -133,        /** Provided buffer too small for intended use */
    RCP_NO_MEMORY = -134,               /** No memory */
    RCP_OPERATION_FAILED = -135,        /** Operation failed */
    RCP_TIMEOUT_ERROR = -136,           /** Token or data timeout error */
    RCP_INTERNAL_ERROR = -137,          /** Internal error */	

	RCP_DEVICE_NOT_FOUND = -192,
	RCP_TOKEN_EXCHANGE_ERROR = -193,
	RCP_RESULT_NOT_MATCH = -194,
	RCP_ACK_ERROR = -195,
} RCP_STATUS;

#endif
