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

#define  HASH_STORE_SIGNATURE          SIGNATURE_32('_', 'H', 'S', '_')

#define  COMP_TYPE_STAGE_1B            0
#define  COMP_TYPE_STAGE_2             1
#define  COMP_TYPE_PAYLOAD             2
#define  COMP_TYPE_PAYLOAD_FWU         3
#define  COMP_TYPE_INVALID             4

//
// Hash Table Definition for Component and Public key usage
//

typedef UINT32 HASH_COMP_USAGE;

#define HASH_USAGE_STAGE_1B              (1 << COMP_TYPE_STAGE_1B)
#define HASH_USAGE_STAGE_2               (1 << COMP_TYPE_STAGE_2)
#define HASH_USAGE_PAYLOAD               (1 << COMP_TYPE_PAYLOAD)
#define HASH_USAGE_FIRMWARE_UPDATE       (1 << COMP_TYPE_PAYLOAD_FWU)

#define HASH_USAGE_PUBKEY_MASTER         BIT8
#define HASH_USAGE_PUBKEY_CFG_DATA       BIT9
#define HASH_USAGE_PUBKEY_FWU            BIT10
#define HASH_USAGE_PUBKEY_OS             BIT11
#define HASH_USAGE_PUBKEY_CONTAINER_DEF  BIT12

#define HASH_USAGE_PUBKEY_OEM_0          BIT24
#define HASH_USAGE_PUBKEY_OEM_1          BIT25
#define HASH_USAGE_PUBKEY_OEM_2          BIT26
#define HASH_USAGE_PUBKEY_OEM_3          BIT27
#define HASH_USAGE_PUBKEY_OEM_4          BIT28
#define HASH_USAGE_PUBKEY_OEM_5          BIT29
#define HASH_USAGE_PUBKEY_OEM_6          BIT30
#define HASH_USAGE_PUBKEY_OEM_7          BIT31
#define HASH_USAGE_PUBKEY_OEM(x)         (HASH_USAGE_PUBKEY_OEM_0 << (x))

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
  UINT8              HeaderLength;
  UINT8              Reserved[2];
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
