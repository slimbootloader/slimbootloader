/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/* 
// 
//  Purpose:
//     Cryptography Primitive.
//     Hash Message Authentication Code
//     Internal Definitions and Internal Functions Prototypes
// 
// 
*/

#if !defined(_PCP_HMAC_H)
#define _PCP_HMAC_H

#include "pcphash.h"

/*
// HMAC context
*/
struct _cpHMAC {
   IppCtxId idCtx;               /* HMAC identifier   */
   Ipp8u ipadKey[MBS_HASH_MAX];  /* inner padding key */
   Ipp8u opadKey[MBS_HASH_MAX];  /* outer padding key */
   IppsHashState hashCtx;        /* hash context      */
};

/* accessors */
#define HMAC_CTX_ID(stt)   ((stt)->idCtx)
#define HASH_CTX(stt)      ((stt)->hashCtx)
#define HMAC_VALID_ID(stt) (HMAC_CTX_ID((stt))==idCtxHMAC)

#define IPAD            (0x36)   /* inner padding value */
#define OPAD            (0x5C)   /* outer padding value */

#endif /* _PCP_HMAC_H */
