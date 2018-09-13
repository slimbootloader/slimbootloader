/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __VERIFIED_BOOT_LIB_H__
#define __VERIFIED_BOOT_LIB_H__

#define    COMP_TYPE_STAGE_1B            0
#define    COMP_TYPE_STAGE_2             1
#define    COMP_TYPE_PAYLOAD             2
#define    COMP_TYPE_FIRMWARE_UPDATE     3
#define    COMP_TYPE_PUBKEY_CFG_DATA     4
#define    COMP_TYPE_PUBKEY_FWU          5
#define    COMP_TYPE_PUBKEY_OS           6
#define    COMP_TYPE_INVALID             7

/**
  Halt CPU from execution.

  @param[in]  Message      Message to display before halt.

**/
VOID
CpuHalt (
  IN CHAR8  *Message
  );

/**
  Verify data block hash with the built-in one.

  @param[in]  Data            Data buffer pointer.
  @param[in]  Length          Data buffer size.
  @param[in]  ComponentType   Component type.

  @retval RETURN_SUCCESS             Hash verification succeeded.
  @retval RETURN_NOT_FOUND           Hash data for ComponentType is not found.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_SECURITY_VIOLATION  Hash verification failed.

**/
RETURN_STATUS
DoHashVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       UINT8            ComponentType
  );

/**
  Verifies the RSA signature with PKCS1-v1_5 encoding scheme defined in RSA PKCS#1.
  Also(optional), return the hash of the message to the caller.

  @param[in]  Data            Data buffer pointer.
  @param[in]  Length          Data buffer size.
  @param[in]  ComponentType   Component type.
  @param[in]  Signature       Signature for the data buffer.
  @param[in]  PubKey          Public Key data pointer.
  @param[out] OutHash         OutHash buffer pointer.

  @retval RETURN_SUCCESS             RSA verification succeeded.
  @retval RETURN_NOT_FOUND           Hash data for ComponentType is not found.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_SECURITY_VIOLATION  PubKey or Signature verification failed.

**/
RETURN_STATUS
DoRsaVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       UINT8            ComponentType,
  IN CONST UINT8           *Signature,
  IN       UINT8           *PubKey,
  OUT      UINT8           *OutHash
  );

typedef RETURN_STATUS (*RSA_VERIFY) (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       UINT8            ComponentType,
  IN CONST UINT8           *Signature,
  IN       UINT8           *PubKey,
  OUT      UINT8           *OutHash
  );

/**
  Generate RandomNumbers.

  @param[out] Data                   Array of random numbers generated
  @param[in] Size                    Size of the array to be generated.

  @retval EFI_SUCCESS                Random number generation successful.
  @retval EFI_UNSUPPORTED            Couldnt generate a random number.
**/
RETURN_STATUS
EFIAPI
GenerateRandomNumbers(
  OUT   CHAR8   *Data,
  IN    UINTN   Size
  );
#endif /* __VERIFIED_BOOT_LIB_H__ */
