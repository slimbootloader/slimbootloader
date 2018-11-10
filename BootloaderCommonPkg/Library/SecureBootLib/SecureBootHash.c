/** @file
  Secure boot library routines to provide hash verification.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CryptoLib.h>
#include <Library/SecureBootLib.h>
#include <Library/BootloaderCommonLib.h>

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
  )
{
  RETURN_STATUS        Status;
  UINT8                Digest[SHA256_DIGEST_SIZE];
  CONST UINT8          *PubKeyHash;

  // Get public key hash
  Status = GetComponentHash (ComponentType, &PubKeyHash);
  if (RETURN_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Warning: Component (%d) verification is bypassed.\n", ComponentType));
    return Status;
  }

  Sha256 (Data, Length, Digest);
  if (CompareMem (PubKeyHash, (VOID *)Digest, SHA256_DIGEST_SIZE)) {
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
    DumpHex (2, 0, SHA256_DIGEST_SIZE, (VOID *)PubKeyHash);

    DEBUG_CODE_END();

  } else {
    Status = RETURN_SUCCESS;
    DEBUG ((DEBUG_INFO, "HASH Verification Success! Component Type (%d)\n", ComponentType));
  }

  return Status;
}
