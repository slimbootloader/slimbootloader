/** @file
  This file defines the hob structure used for paylod.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PAYLOAD_KEY_HASH_GUID_H__
#define __PAYLOAD_KEY_HASH_GUID_H__

///
/// The pre-process public key GUID used for payload to verify image.
///
extern EFI_GUID gPayloadKeyHashGuid;

#ifndef    SHA256_DIGEST_SIZE
#define    SHA256_DIGEST_SIZE   32
#endif
#define    MAX_KEY_DIGEST_COUNT 3

typedef struct {
  UINT32               ComponentType;
  UINT8                Digest[SHA256_DIGEST_SIZE];
} KEY_HASH_ITEM;

typedef struct {
  UINT8                Revision;
  UINT8                Reserved0[3];
  UINT32               DigestCount;
  KEY_HASH_ITEM        KeyHash[0];
} PAYLOAD_KEY_HASH;

#endif
