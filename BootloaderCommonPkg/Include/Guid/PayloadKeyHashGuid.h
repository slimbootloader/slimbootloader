/** @file
  This file defines the hob structure used for paylod.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PAYLOAD_KEY_HASH_GUID_H__
#define __PAYLOAD_KEY_HASH_GUID_H__

///
/// The pre-process public key GUID used for payload to verify image.
///
extern EFI_GUID gPayloadKeyHashGuid;


#pragma pack(1)
typedef struct {
  UINT32           Usage;
  UINT8            HashAlg;
  UINT8            Reserved;
  UINT16           DigestLen;
  UINT8            Digest[0];
} HASH_STORE_DATA;

typedef struct {
  UINT32             Signature;
  UINT8              Revision;
  UINT8              Reserved[3];
  UINT32             UsedLength;
  UINT32             TotalLength;
  UINT8              Data[0];
} HASH_STORE_TABLE;
#pragma pack()
#endif
