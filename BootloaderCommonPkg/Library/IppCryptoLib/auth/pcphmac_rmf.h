/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
//  Purpose:
//     Cryptography Primitive.
//     Hash Message Authentication Code
//     Internal Definitions and Internal Functions Prototypes
*/

#if !defined(_PCP_HMAC_RMF_H)
#define _PCP_HMAC_RMF_H

#include "pcphash_rmf.h"

/*
// HMAC context
*/
struct _cpHMAC_rmf {
   IppCtxId idCtx;               /* HMAC identifier   */
   Ipp8u ipadKey[MBS_HASH_MAX];  /* inner padding key */
   Ipp8u opadKey[MBS_HASH_MAX];  /* outer padding key */
   IppsHashState_rmf hashCtx;    /* hash context      */
};


#endif /* _PCP_HMAC_RMF_H */
