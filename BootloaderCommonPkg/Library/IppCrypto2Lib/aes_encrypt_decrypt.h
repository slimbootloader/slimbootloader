/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __AES_ENCRYPT_DECRYPT_H__
#define __AES_ENCRYPT_DECRYPT_H__

#include <Uefi/UefiBaseType.h>

/**
  Performs AES CBC encryption using Intel IPP library.

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
  );

  /**
    Performs AES CBC decryption on input data using specified parameters.

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
  );

  #endif // __AES_ENCRYPT_DECRYPT_H__