/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "owndefs.h"
#include "owncp.h"

#include "pcpngrsa.h"
#include "pcphash.h"
#include "pcptool.h"
#include "ippcp.h"
#include <Library/CryptoLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/DebugLib.h>

/**
  Performs AES encryption using Intel IPP library.

  @param[in]   PlainText       Pointer to the plaintext data to be encrypted.
  @param[in]   PlainTextSize   Size of the plaintext data in bytes.
  @param[in]   Key             Pointer to the AES encryption key.
  @param[in]   KeySize         Size of the encryption key in bytes (16, 24, or 32).
  @param[in]   Iv              Pointer to the initialization vector (16 bytes for AES).
  @param[out]  CipherText      Pointer to buffer to receive encrypted data.
  @param[in]   CipherTextSize  Size of the cipher text buffer in bytes.

  @retval  RETURN_SUCCESS             Encryption completed successfully.
  @retval  RETURN_INVALID_PARAMETER   Invalid input parameters.
  @retval  RETURN_BUFFER_TOO_SMALL    Output buffer too small.
  @retval  RETURN_SECURITY_VIOLATION  Encryption failed.
**/
RETURN_STATUS
EFIAPI
AesCbcEncrypt (
  IN  CONST UINT8     *PlainText,
  IN        UINT32     PlainTextSize,
  IN  CONST UINT8     *Key,
  IN        UINT32     KeySize,
  IN  CONST UINT8     *Iv,
  OUT       UINT8     *CipherText,
  IN        UINT32     CipherTextSize
  ) {
    IppStatus   status;
    IppsAESSpec *pAES = NULL;
    int         ctxSize = 0;
    UINT8       *pIV = NULL;
    UINT8       tempIV[16];
    DEBUG ((DEBUG_INFO, "AesEncrypt: PlainTextSize=%u, KeySize=%u, , CipherTextSize=%u\n",
        PlainTextSize, KeySize, CipherTextSize));

    // Validate input parameters
    if (PlainText == NULL || Key == NULL || Iv == NULL || CipherText == NULL) {
        DEBUG ((DEBUG_ERROR, "AesEncrypt: Invalid input parameters\n"));
      return RETURN_INVALID_PARAMETER;
    }

    if (PlainTextSize == 0 || CipherTextSize < PlainTextSize) {
        DEBUG ((DEBUG_ERROR, "AesEncrypt: Output buffer too small or input size is zero\n"));
      return RETURN_INVALID_PARAMETER;
    }

    if (KeySize != 16 && KeySize != 24 && KeySize != 32) {
        DEBUG ((DEBUG_ERROR, "AesEncrypt: Invalid KeySize %u\n", KeySize));
      return RETURN_INVALID_PARAMETER;
    }

    // Check if plaintext size is multiple of 16 bytes (AES block size)
    if (PlainTextSize % 16 != 0) {
      DEBUG ((DEBUG_ERROR, "AesEncrypt: PlainTextSize is not a multiple of 16 bytes\n"));
      return RETURN_INVALID_PARAMETER;
    }

    if (CipherTextSize < PlainTextSize) {
      DEBUG ((DEBUG_ERROR, "AesEncrypt: Output buffer too small\n"));
      return RETURN_BUFFER_TOO_SMALL;
    }

    // Get required context size
    status = ippsAESGetSize(&ctxSize);
    if (status != ippStsNoErr) {
      DEBUG ((DEBUG_ERROR, "AesEncrypt: ippsAESGetSize failed with status %d\n", status));
      return RETURN_SECURITY_VIOLATION;
    }

    // Allocate AES context
    pAES = (IppsAESSpec *) AllocateTemporaryMemory(ctxSize);
    if (pAES == NULL) {
      DEBUG ((DEBUG_ERROR, "AesEncrypt: Failed to allocate AES context\n"));
      return RETURN_OUT_OF_RESOURCES;
    }

    // Initialize AES context
    status = ippsAESInit(Key, KeySize, pAES, ctxSize);
    if (status != ippStsNoErr) {
      DEBUG ((DEBUG_ERROR, "AesEncrypt: ippsAESInit failed with status %d\n", status));
      return RETURN_SECURITY_VIOLATION;
    }

    // Copy IV to temporary buffer (IPP may modify it)
    CopyMem(tempIV, Iv, 16);
    pIV = tempIV;

    // Perform AES-CBC encryption
    status = ippsAESEncryptCBC(PlainText, CipherText, PlainTextSize, pAES, pIV);

    // Clean up
    FreeTemporaryMemory(pAES);

    if (status != ippStsNoErr) {
      DEBUG ((DEBUG_ERROR, "AesEncrypt: ippsAESEncryptCBC failed with status %d\n", status));
      return RETURN_SECURITY_VIOLATION;
    }
    DEBUG ((DEBUG_INFO, "AesEncrypt: Encryption successful\n"));

    return RETURN_SUCCESS;
  }