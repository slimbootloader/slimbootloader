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
  Calculate hash API.

  @param[in]  Data           Data buffer pointer.
  @param[in]  Length         Data buffer size.
  @param[in]  HashAlg        Specify hash algrothsm.
  @param[in,out]  OutHash    Hash of Data buffer


  @retval RETURN_SUCCESS             Hash Calculation succeeded.
  @retval RETRUN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_UNSUPPORTED         Hash Alg type is not supported.

**/
RETURN_STATUS
CalculateHash  (
  IN CONST UINT8          *Data,
  IN       UINT32          Length,
  IN       UINT8           HashAlg,
  IN OUT   UINT8          *OutHash
  )
{
  UINT8 Digest[HASH_DIGEST_MAX];
  UINT8 DigestSize;
  UINT8 *HashRetVal;

  if(Digest != NULL){
    if (HashAlg == HASH_TYPE_SHA256) {
      HashRetVal = Sha256 (Data, Length, Digest);
      DigestSize = SHA256_DIGEST_SIZE;
    } else if (HashAlg == HASH_TYPE_SHA384) {
      HashRetVal = Sha384 (Data, Length, Digest);
      DigestSize = SHA384_DIGEST_SIZE;
    } else if (HashAlg == HASH_TYPE_SM3) {
      HashRetVal = Sm3 (Data, Length, Digest);
      DigestSize = SM3_DIGEST_SIZE;
    } else {
      return RETURN_UNSUPPORTED;
    }

    if (HashRetVal == NULL){
      return RETURN_UNSUPPORTED;
    } else{
      CopyMem (OutHash, Digest, DigestSize);
    }
  }
  return RETURN_SUCCESS;
}


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
  UINT8                Digest[HASH_DIGEST_MAX];
  UINT8                DigestSize;


  if ((Data == NULL) || (HashAlg != HASH_TYPE_SHA256)) {
    return RETURN_INVALID_PARAMETER;
  }

  if ((Usage == 0) && (HashData == NULL)) {
    return RETURN_INVALID_PARAMETER;
  }

  if (HashAlg == HASH_TYPE_SHA256) {
    DigestSize = SHA256_DIGEST_SIZE;
  } else if (HashAlg == HASH_TYPE_SHA384) {
    DigestSize = SHA384_DIGEST_SIZE;
  } else {
    return RETURN_INVALID_PARAMETER;
  }

  Status = CalculateHash (Data, Length, HashAlg, Digest);
  if (EFI_ERROR(Status)) {
    return RETURN_UNSUPPORTED;
  }

  Status = RETURN_SECURITY_VIOLATION;
  if (Usage == 0) {
    // Compare hash with the buffer passed in
    if (CompareMem (HashData, (VOID *)Digest, DigestSize) == 0) {
      Status = RETURN_SUCCESS;
    }
  } else {
    // Compare hash with the the one stored in hash store
    Status2 = MatchHashInStore (Usage, HashAlg, Digest);
    if (!EFI_ERROR(Status2)) {
      if (HashData != NULL) {
        CopyMem (HashData, Digest, DigestSize);
      }
      Status = RETURN_SUCCESS;
    }
  }

  DEBUG ((DEBUG_INFO, "HASH verification for usage (0x%08X): %r\n", Usage, Status));
  if (EFI_ERROR(Status)) {
    DEBUG_CODE_BEGIN();

    DEBUG ((DEBUG_INFO, "First %d Bytes Input Data\n", DigestSize));
    DumpHex (2, 0, DigestSize, (VOID *)Data);

    DEBUG ((DEBUG_INFO, "Last %d Bytes Input Data\n", DigestSize));
    DumpHex (2, 0, DigestSize, (VOID *) (Data + Length - DigestSize));

    DEBUG ((DEBUG_INFO, "Image Digest\n"));
    DumpHex (2, 0, DigestSize, (VOID *)Digest);

    DEBUG ((DEBUG_INFO, "HashStore Digest\n"));
    DumpHex (2, 0, DigestSize, (VOID *)HashData);

    DEBUG_CODE_END();
  }

  return Status;
}
