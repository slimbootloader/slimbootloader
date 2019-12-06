/** @file
  This file defines the hob structure used for paylod.

  Copyright (c) 2017-2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PAYLOAD_KEY_HASH_GUID_H__
#define __PAYLOAD_KEY_HASH_GUID_H__

///
/// The pre-process public key GUID used for payload to verify image.
///
extern EFI_GUID gPayloadKeyHashGuid;

#define  COMP_TYPE_STAGE_1B            0
#define  COMP_TYPE_STAGE_2             1
#define  COMP_TYPE_PAYLOAD             2
#define  COMP_TYPE_FIRMWARE_UPDATE     3
#define  COMP_TYPE_PUBKEY_CFG_DATA     4
#define  COMP_TYPE_PUBKEY_FWU          5
#define  COMP_TYPE_PUBKEY_OS           6
#define  COMP_TYPE_PAYLOAD_DYNAMIC     7
#define  COMP_TYPE_INVALID             8


//
// Hash Table Definition for Component and Public key usage
//

typedef UINT32 HASH_COMP_USAGE;

#define HASH_USAGE_STAGE_1B              BIT0
#define HASH_USAGE_STAGE_2               BIT1
#define HASH_USAGE_PAYLOAD               BIT2
#define HASH_USAGE_FIRMWARE_UPDATE       BIT3
#define HASH_USAGE_PUBKEY_CFG_DATA       BIT4
#define HASH_USAGE_PUBKEY_FWU            BIT5
#define HASH_USAGE_PUBKEY_OS             BIT6


#pragma pack(1)
typedef struct {
  //
  // Usage corresponds to components bit mask for hash is valid
  //
  HASH_COMP_USAGE    Usage;
  //
  // Hash algorithm used for digest
  //
  UINT8              HashAlg;
  UINT8              Reserved;
  //
  // Digest Length
  //
  UINT16             DigestLen;
  //
  // Hash of the component
  //
  UINT8              Digest[0];
} HASH_STORE_DATA;

typedef struct {
  UINT32             Signature;
  UINT8              Revision;
  UINT8              Reserved[3];
  //
  // Total valid  hash store data including the header
  //
  UINT32             UsedLength;
  //
  // The total space for hash store data allocated
  //
  UINT32             TotalLength;
  //
  // Hash store data chain with type HASH_STORE_DATA
  //
  UINT8              Data[0];
} HASH_STORE_TABLE;
#pragma pack()
#endif
