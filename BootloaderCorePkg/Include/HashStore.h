/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __HASH_STORE_H__
#define __HASH_STORE_H__

#define  HASH_STORE_SIGNATURE                SIGNATURE_32('_', 'H', 'S', '_')
#define  HASH_STORE_DIGEST_LENGTH            32

#define  HASH_INDEX_STAGE_1B               0
#define  HASH_INDEX_STAGE_2                1
#define  HASH_INDEX_PAYLOAD                2
#define  HASH_INDEX_FIRMWARE_UPDATE        3
#define  HASH_INDEX_CFG_PUBKEY             4
#define  HASH_INDEX_FWU_PUBKEY             5
#define  HASH_INDEX_OS_PUBKEY              6
#define  HASH_INDEX_PAYLOAD_DYNAMIC        7
#define  HASH_INDEX_MAX_NUM                8


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