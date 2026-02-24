/** @file
  Shell command `Encrypt` to perform AES encryption and decryption.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/CryptoLib.h>

// Test vectors for AES-256-CBC
static UINT8 test_key_256[] = {
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

static UINT8 test_iv[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};


#define BUFFER_SIZE 1024
#define KEY_SIZE_256 32

// 1K test data (BUFFER_SIZE bytes)
static UINT8 test_plaintext_1k[BUFFER_SIZE];


/**
  Initialize 1K test data with pattern.
**/
STATIC
VOID
InitializeTestData (
  VOID
  )
{
  UINTN i;
  UINT32 seed = 0x12345678;

  for (i = 0; i < BUFFER_SIZE; i++) {
    // Simple linear congruential generator for pseudo-random data
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    test_plaintext_1k[i] = (UINT8)(seed & 0xFF);
  }
}

/**
  Perform AES encryption and decryption test.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandEncryptFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandEncrypt = {
  L"Encrypt",
  L"Perform encryption",
  &ShellCommandEncryptFunc
};


void test_aes_encrypt_decrypt(void) {
  UINT8 ciphertext[BUFFER_SIZE];
  UINT8 plaintext[BUFFER_SIZE];
  RETURN_STATUS status;

  ShellPrint (L" Testing encryption....\n");

  // First encrypt the data
  status = AesEncrypt(
    test_plaintext_1k, BUFFER_SIZE,
    test_key_256, KEY_SIZE_256,
    test_iv,
    AES_MODE_CBC,
    ciphertext, BUFFER_SIZE
  );
  ASSERT(status == RETURN_SUCCESS);

  ShellPrint (L" Encryption completed successfully\n");

  ShellPrint (L" Testing decryption .\n");

  if (FeaturePcdGet (PcdEnableAesEncrypt) == 0) {
    ShellPrint (L" Encryption/Decryption is not enabled nor supported. Check platform board config file\n");
    return;
  } else {
    ShellPrint (L" Encryption/Decryption is enabled. Running decryption test...\n");
      // Then perform decryption
    status = AesDecrypt(
      ciphertext, BUFFER_SIZE,
      test_key_256, KEY_SIZE_256,
      test_iv,
      AES_MODE_CBC,
      plaintext, BUFFER_SIZE
    );
    ASSERT(status == RETURN_SUCCESS);
    ASSERT(CompareMem(plaintext, test_plaintext_1k, BUFFER_SIZE) == 0);
    ShellPrint (L" Decryption completed successfully\n");
  }
}


/**
  Test for AES-256-CBC encryption and decryption.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandEncryptFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  ShellPrint (L" Running AES-256 Encrypt/Decrypt Tests with 1K data...\n");

  // Initialize test data
  InitializeTestData();

  ShellPrint (L" Testing encryption and decryption...\n");

  test_aes_encrypt_decrypt();

  ShellPrint (L" All tests passed \n");

  return EFI_SUCCESS;
}