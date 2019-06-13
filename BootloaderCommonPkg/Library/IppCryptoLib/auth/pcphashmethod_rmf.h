/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*
//  Purpose:
//     Cryptography Primitive.
//     Security Hash Standard
//     Internal Definitions and Internal Functions Prototypes
//
*/
#if !defined(_PCP_HASH_METHOD_RMF_H)
#define _PCP_HASH_METHOD_RMF_H

/* hash alg methods */
typedef void (*hashInitF)(void* pHash);
typedef void (*hashUpdateF)(void* pHash, const Ipp8u* pMsg, int msgLen);
typedef void (*hashOctStrF)(Ipp8u* pDst, void* pHash);
typedef void (*msgLenRepF)(Ipp8u* pDst, Ipp64u lenLo, Ipp64u lenHi);

typedef struct _cpHashMethod_rmf {
   IppHashAlgId   hashAlgId;     /* algorithm ID */
   int            hashLen;       /* hash length in bytes */
   int            msgBlkSize;    /* message blkock size in bytes */
   int            msgLenRepSize; /* length of processed msg length representation in bytes */
   hashInitF      hashInit;      /* set initial hash value */
   hashUpdateF    hashUpdate;    /* hash compressor */
   hashOctStrF    hashOctStr;    /* convert hash into oct string */
   msgLenRepF     msgLenRep;     /* processed mgs length representation */
} cpHashMethod_rmf;

#endif /* _PCP_HASH_METHOD_RMF_H */
