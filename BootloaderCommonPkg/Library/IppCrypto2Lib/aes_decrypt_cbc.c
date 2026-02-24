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
  Performs AES decryption on input data using specified parameters.

  @param[in]   CipherText      Pointer to the ciphertext data to be decrypted.
  @param[in]   CipherTextSize  Size of the ciphertext data in bytes.
  @param[in]   Key             Pointer to the AES decryption key.
  @param[in]   KeySize         Size of the decryption key in bytes (16, 24, or 32).
  @param[in]   Iv              Pointer to the initialization vector (16 bytes for AES).
  @param[out]  PlainText       Pointer to buffer to receive decrypted data.
  @param[in]   PlainTextSize   Size of the plain text buffer in bytes.

  @retval  RETURN_SUCCESS             Decryption completed successfully.
  @retval  RETURN_INVALID_PARAMETER   Invalid input parameters.
  @retval  RETURN_BUFFER_TOO_SMALL    Output buffer too small.
  @retval  RETURN_SECURITY_VIOLATION  Decryption failed.
**/

RETURN_STATUS
EFIAPI
AesCbcDecrypt (
  IN  CONST UINT8     *CipherText,
  IN        UINT32     CipherTextSize,
  IN  CONST UINT8     *Key,
  IN        UINT32     KeySize,
  IN  CONST UINT8     *Iv,
  OUT       UINT8     *PlainText,
  IN        UINT32     PlainTextSize
  ) {
    IppStatus status;
    IppsAESSpec* pAES = NULL;
    int ctxSize = 0;
    Ipp8u* pIV = NULL;

    DEBUG ((DEBUG_INFO, "AesDecrypt: CipherTextSize=%u, KeySize=%u, PlainTextSize=%u\n",
        CipherTextSize, KeySize, PlainTextSize));

    // Validate input parameters
    if (!CipherText || !Key || !Iv || !PlainText) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Invalid input parameters\n"));
        return RETURN_INVALID_PARAMETER;
    }

    if (CipherTextSize == 0 || PlainTextSize < CipherTextSize) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Output buffer too small or input size is zero\n"));
        return RETURN_BUFFER_TOO_SMALL;
    }

    if (KeySize != 16 && KeySize != 24 && KeySize != 32) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Invalid KeySize %u\n", KeySize));
        return RETURN_INVALID_PARAMETER;
    }

    // Check if input size is multiple of AES block size (16 bytes)
    if (CipherTextSize % 16 != 0) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: CipherTextSize is not a multiple of AES block size\n"));
        return RETURN_INVALID_PARAMETER;
    }

    // Get AES context size
    status = ippsAESGetSize(&ctxSize);
    if (status != ippStsNoErr) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Failed to get AES context size, status %d\n", status));
        return RETURN_SECURITY_VIOLATION;
    }

    // Allocate memory for AES context
    pAES = (IppsAESSpec*)AllocateTemporaryMemory(ctxSize);
    if (!pAES) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Failed to allocate memory for AES context\n"));
        return RETURN_OUT_OF_RESOURCES;
    }

    // Initialize AES context
    status = ippsAESInit(Key, KeySize, pAES, ctxSize);
    if (status != ippStsNoErr) {
        FreeTemporaryMemory(pAES);
        DEBUG ((DEBUG_ERROR, "AesDecrypt: AES initialization failed with status %d\n", status));
        return RETURN_SECURITY_VIOLATION;
    }

    // Allocate and copy IV (as it will be modified)
    pIV = (Ipp8u*)AllocateTemporaryMemory(16);
    if (!pIV) {
        FreeTemporaryMemory(pAES);
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Failed to allocate memory for IV\n"));
        return RETURN_OUT_OF_RESOURCES;
    }
    CopyMem(pIV, Iv, 16);

    // Perform CBC decryption
    status = ippsAESDecryptCBC(CipherText, PlainText, CipherTextSize, pAES, pIV);

    // Clean up
    FreeTemporaryMemory(pIV);
    FreeTemporaryMemory(pAES);

    if (status != ippStsNoErr) {
        DEBUG ((DEBUG_ERROR, "AesDecrypt: Decryption failed with status %d\n", status));
        return RETURN_SECURITY_VIOLATION;
    }
    DEBUG ((DEBUG_INFO, "AesDecrypt: Decryption successful\n"));

    return RETURN_SUCCESS;
  }
