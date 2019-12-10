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
  @param[in]  Usage          Hash usage.
  @param[in]  ComponentType  Component type.
  @param[in,out]  Hash       On input,  expected hash value when ComponentType is not used.
                             On output, calculated hash value when verification succeeds.

  @retval RETURN_SUCCESS             Hash verification succeeded.
  @retval RETURN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_NOT_FOUND           Hash data for ComponentType is not found.
  @retval RETURN_UNSUPPORTED         Hash component type is not supported.
  @retval RETURN_SECURITY_VIOLATION  Hash verification failed.

**/
RETURN_STATUS
DoHashVerify (
  IN CONST UINT8           *Data,
  IN       UINT32           Length,
  IN       HASH_COMP_USAGE  Usage,
  IN       UINT8            HashAlg,
  IN OUT   UINT8           *HashData
  )
{
  RETURN_STATUS        Status;
  RETURN_STATUS        Status2;
  UINT8                Digest[SHA256_DIGEST_SIZE];

  if ((Data == NULL) || (HashAlg != HASH_TYPE_SHA256)) {
    return RETURN_INVALID_PARAMETER;
  }

  if ((Usage == 0) && (HashData == NULL)) {
    return RETURN_INVALID_PARAMETER;
  }

  Sha256 (Data, Length, Digest);

  Status = RETURN_SECURITY_VIOLATION;
  if (Usage == 0) {
    // Compare hash with the buffer passed in
    if (CompareMem (HashData, (VOID *)Digest, sizeof(Digest)) == 0) {
      Status = RETURN_SUCCESS;
    }
  } else {
    // Compare hash with the the one stored in hash store
    Status2 = MatchHashInStore (Usage, HashAlg, Digest);
    if (!EFI_ERROR(Status2)) {
      if (HashData != NULL) {
        CopyMem (HashData, Digest, sizeof(Digest));
      }
      Status = RETURN_SUCCESS;
    }
  }

  DEBUG ((DEBUG_INFO, "HASH verification for usage (0x%08X): %r\n", Usage, Status));
  if (EFI_ERROR(Status)) {
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
  }

  return Status;
}
