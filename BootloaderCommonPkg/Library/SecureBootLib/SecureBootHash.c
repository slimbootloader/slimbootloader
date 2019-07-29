/** @file
  Secure boot library routines to provide hash verification.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CryptoLib.h>
#include <Library/SecureBootLib.h>
#include <Library/BootloaderCommonLib.h>

/**
  Verify data block hash with the built-in one.

  @param[in]  Data           Data buffer pointer.
  @param[in]  Length         Data buffer size.
  @param[in]  HashAlg        Specify hash algrothsm.
  @param[in]  ComponentType  Component type.
  @param[in,out]  Hash       On input,  expected hash value when ComponentType is not used.
                             On output, calculated hash value when verification succeeds.

  @retval RETURN_SUCCESS             Hash verification succeeded.
  @retval RETRUN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_NOT_FOUND           Hash data for ComponentType is not found.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_SECURITY_VIOLATION  Hash verification failed.

**/
RETURN_STATUS
DoHashVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       UINT8            HashAlg,
  IN       UINT8            ComponentType,
  IN OUT   UINT8           *Hash
  )
{
  RETURN_STATUS        Status;
  UINT8                Digest[SHA256_DIGEST_SIZE];
  CONST UINT8          *HashData;

  if (HashAlg != HASH_TYPE_SHA256) {
    return RETURN_UNSUPPORTED;
  }

  // Get expected hash to compare with
  if (ComponentType >= COMP_TYPE_INVALID) {
    HashData = Hash;
  } else {
    Status = GetComponentHash (ComponentType, &HashData);
    if (RETURN_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Warning: Component (%d) verification is bypassed.\n", ComponentType));
      return Status;
    }
  }

  if (HashData == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  Sha256 (Data, Length, Digest);
  if (CompareMem (HashData, (VOID *)Digest, SHA256_DIGEST_SIZE)) {
    Status = RETURN_SECURITY_VIOLATION;

    DEBUG ((DEBUG_ERROR, "Hash check fail for component type (%d)\n", ComponentType));

    DEBUG_CODE_BEGIN();

    DEBUG ((DEBUG_INFO, "First 32Bytes Input Data\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)Data);

    DEBUG ((DEBUG_INFO, "Last 32Bytes Input Data\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *) (Data + Length - 32));

    DEBUG ((DEBUG_INFO, "Image Digest\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)Digest);

    DEBUG ((DEBUG_INFO, "HashStore Digest\n"));
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)HashData);

    DEBUG_CODE_END();

  } else {
    if ((Hash != NULL) && (HashData != Hash)) {
      CopyMem (Hash, Digest, sizeof(Digest));
    }
    Status = RETURN_SUCCESS;
    DEBUG ((DEBUG_INFO, "HASH Verification Success! Component Type (%d)\n", ComponentType));
  }

  return Status;
}
