/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __HASH_STORE_H__
#define __HASH_STORE_H__

#define  HASH_STORE_SIGNATURE                SIGNATURE_32('_', 'H', 'S', '_')
#define  HASH_STORE_DIGEST_LENGTH            32

#define  HASH_INDEX_MAX_NUM                  8

#pragma pack(1)
typedef struct {
  UINT8  Data[HASH_STORE_DIGEST_LENGTH];
} HASH_STORE_DATA;

typedef struct {
  UINT32             Signature;
  UINT32             Valid;
  HASH_STORE_DATA    Data[HASH_INDEX_MAX_NUM];
} HASH_STORE_TABLE;
#pragma pack()

#endif /* __HASH_STORE_H__ */