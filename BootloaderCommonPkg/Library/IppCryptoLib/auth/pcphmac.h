/*******************************************************************************
* Copyright 2002-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

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
