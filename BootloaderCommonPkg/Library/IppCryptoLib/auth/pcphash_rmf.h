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
//  Purpose:
//     Cryptography Primitive.
//     Security Hash Standard
//     Internal Definitions and Internal Functions Prototypes
//
*/

#if !defined(_PCP_HASH_RMF_H)
#define __PCP_HASH_RMF_H

#include "pcphash.h"
#include "pcphashmethod_rmf.h"


struct _cpHashCtx_rmf {
   IppCtxId    idCtx;                     /* hash identifier   */
   const cpHashMethod_rmf* pMethod;       /* hash methods      */
   int         msgBuffIdx;                /* buffer index      */
   Ipp8u       msgBuffer[MBS_HASH_MAX];   /* buffer            */
   Ipp64u      msgLenLo;                  /* processed message */
   Ipp64u      msgLenHi;                  /* length (bytes)    */
   cpHash      msgHash;                    /* hash value        */
};

/* accessors */
#define HASH_CTX_ID(stt)   ((stt)->idCtx)
#define HASH_METHOD(stt)   ((stt)->pMethod)
#define HAHS_BUFFIDX(stt)  ((stt)->msgBuffIdx)
#define HASH_BUFF(stt)     ((stt)->msgBuffer)
#define HASH_LENLO(stt)    ((stt)->msgLenLo)
#define HASH_LENHI(stt)    ((stt)->msgLenHi)
#define HASH_VALUE(stt)    ((stt)->msgHash)

#endif /* __PCP_HASH_RMF_H */
