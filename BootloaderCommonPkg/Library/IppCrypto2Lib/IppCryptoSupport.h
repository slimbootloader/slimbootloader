/** @file
  Root include file of C runtime library to support building the Intel
  IPPCrypto library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef IPP_CRYPTO_LIB_SUPPORT_H_
#define IPP_CRYPTO_LIB_SUPPORT_H_

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

typedef UINT8    uint8_t;
typedef UINT16   uint16_t;
typedef INT32    int32_t;
typedef UINT32   uint32_t;
typedef UINT64   uint64_t;
typedef UINTN    uintptr_t;
typedef UINTN    size_t;
typedef BOOLEAN  bool;

#define true   (1 == 1)
#define false  (1 == 0)

//
// Definitions for global constants used by library routines
//
#define INT_MAX     0x7FFFFFFF           /* Maximum (signed) int value */
#define INT32_MAX   0x7FFFFFFF           /* Maximum (signed) int32 value */
#define UINT32_MAX  0xFFFFFFFF           /* Maximum unsigned int32 value */

//
// Function prototypes of Library routines
//
void *
memset     (
  void *,
  int,
  size_t
  );

void *
memcpy    (
  void *,
  const void *,
  size_t
  );


//
// Macros that directly map functions to BaseLib functions
//

#define _byteswap_ushort(x)                 SwapBytes16(x)
#define _byteswap_ulong(x)                  SwapBytes32(x)
#define _byteswap_uint64(x)                 SwapBytes64(x)
#define _lrotl(x, nBits)                    LRotU32((x), (nBits))
#define _lrotr(x, nBits)                    RRotU32((x), (nBits))

#endif /* IPP_CRYPTO_LIB_SUPPORT_H_ */
