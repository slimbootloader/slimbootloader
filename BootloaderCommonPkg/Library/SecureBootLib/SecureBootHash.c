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
  Get hash to extend a firmware stage component
  Hash calculation to extend would be in either of ways
  Retrieve Hash from Component hash table or
  Calculate Hash using source buf and length provided

  @param[in] ComponentType             Stage whose measurement need to be extended.
  @param[in] HashType                  Hash type required
  @param[in] Src                       Buffer Address
  @param[in] Length                    Data Len
  @param[out] HashData                 Hash Data buf addr

  @retval RETURN_SUCCESS      Operation completed successfully.
  @retval Others              Unable to calcuate hash.
**/
RETURN_STATUS
EFIAPI
GetHashToExtend (
  IN       UINT8            ComponentType,
  IN       HASH_ALG_TYPE    HashType,
  IN       UINT8           *Src,
  IN       UINT32           Length,
  OUT      UINT8           *HashData
  )
{
  RETURN_STATUS        Status;
  HASH_ALG_TYPE        CompHashAlg;
  CONST UINT8         *Digest;
  UINT8                DigestSize;

  if (HashData == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  if (HashType == HASH_TYPE_SHA256) {
    DigestSize = SHA256_DIGEST_SIZE;
  } else if (HashType == HASH_TYPE_SHA384) {
    DigestSize = SHA384_DIGEST_SIZE;
  }  else if (HashType == HASH_TYPE_SM3) {
    DigestSize = SM3_DIGEST_SIZE;
  } else {
    return RETURN_INVALID_PARAMETER;
  }

  // Hash can be calcluated in one of the two ways
  // Get componenet hash from hash store based on Componen Id and return if hash is valid
  // Incase component hash is not avilable calculate hash from src buf and HashType provided.

  // Get componenet hash from hash store based on Componen Id
  if ((ComponentType >= COMP_TYPE_STAGE_1B) && (ComponentType < COMP_TYPE_INVALID)) {
    Status = GetComponentHash (ComponentType, &Digest, &CompHashAlg);
    if((Status == EFI_SUCCESS) && (CompHashAlg == HashType)) {
      CopyMem (HashData, Digest, DigestSize);
      return RETURN_SUCCESS;
    }
  }

  // Calculate hash for a ComponentType if hash is not retrieved from GetComponentHash
  if ((Src != NULL) && (Length > 0)) {
    DEBUG ((DEBUG_INFO, "Calculate Hash for component Type 0x%x as its not available in Component hash table \n", ComponentType));
    Status = CalculateHash ((UINT8 *)Src, Length, HashType, (UINT8 *) HashData);
  } else{
    return RETURN_INVALID_PARAMETER;
  }

  return Status;
}

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
EFIAPI
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
      DEBUG((DEBUG_INFO, "Hash Calculation is invalid!!\n"));
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
  @param[in]  Usage          Hash component usage.
  @param[in]  HashAlg        Specify hash algorithm.
  @param[in,out]  Hash       On input,  expected hash value when hash component usage is 0.
                             On output, calculated hash value when verification succeeds.

  @retval RETURN_SUCCESS             Hash verification succeeded.
  @retval RETURN_INVALID_PARAMETER   Hash parameter is not valid.
  @retval RETURN_NOT_FOUND           Hash data for hash component usage is not found.
  @retval RETURN_UNSUPPORTED         HashAlg not supported.
  @retval RETURN_SECURITY_VIOLATION  Hash verification failed.

**/
RETURN_STATUS
EFIAPI
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


  if ((Data == NULL) ||
      ((HashAlg != HASH_TYPE_SHA256) && (HashAlg != HASH_TYPE_SHA384))) {
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

  DEBUG ((DEBUG_INFO, "HASH verification for usage (0x%08X) with Hash Alg (0x%x): %r\n", Usage, HashAlg, Status));
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
