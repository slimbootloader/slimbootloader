/** @file
  Shell command that deliberately violates memory to test AddressSanitizer.

  It lives in OsLoader rather than ShellLib because only OsLoader sources are
  built with -fsanitize=address.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"
#include <Library/AsanLib.h>

//
// Not a multiple of 8, so overflow by one byte lands in a partially valid shadow granule.
//
#define ASAN_TEST_BUFFER_SIZE  13

STATIC UINT8  mAsanTestGlobal[ASAN_TEST_BUFFER_SIZE];

/**
  Run an AddressSanitizer test.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandAsanFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

STATIC CONST SHELL_COMMAND  mShellCommandAsan = {
  L"asan",
  L"Deliberately violate memory to test AddressSanitizer",
  &ShellCommandAsanFunc
};

/**
  Show usage prompt to screen.

**/
STATIC
VOID
AsanTestUsage (
  VOID
  )
{
  ShellPrint (L"Usage: asan <test>\n");
  ShellPrint (L"  overflow   write 1 byte past the end of a %d byte pool buffer\n", ASAN_TEST_BUFFER_SIZE);
  ShellPrint (L"  underflow  read 1 byte before the start of a pool buffer\n");
  ShellPrint (L"  uaf        read a pool buffer after it is freed\n");
  ShellPrint (L"  dfree      free a pool buffer twice\n");
  ShellPrint (L"  stack      write 1 byte past the end of a %d byte local array\n", ASAN_TEST_BUFFER_SIZE);
  ShellPrint (L"  global     write 1 byte past the end of a %d byte global array\n", ASAN_TEST_BUFFER_SIZE);
  ShellPrint (L"A detected error prints an ==ASAN== report and halts the system.\n");
}

/**
  Run an AddressSanitizer test.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandAsanFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  // Volatile so the compiler keeps the bad accesses.
  volatile UINT8  *Buffer;
  volatile UINTN  Index;
  UINT8           Local[ASAN_TEST_BUFFER_SIZE];
  UINT8           Value;

  if (Argc != 2) {
    AsanTestUsage ();
    return EFI_INVALID_PARAMETER;
  }

  Index = ASAN_TEST_BUFFER_SIZE;
  if (StrCmp (Argv[1], L"stack") == 0) {
    ShellPrint (L"Writing 0x%p, 1 byte past the end of a local array...\n", &Local[Index]);
    Local[Index] = 0xA5;
    ShellPrint (L"ASAN did not detect the violation!\n");
    return EFI_SUCCESS;
  }

  if (StrCmp (Argv[1], L"global") == 0) {
    ShellPrint (L"Writing 0x%p, 1 byte past the end of a global array...\n", &mAsanTestGlobal[Index]);
    mAsanTestGlobal[Index] = 0xA5;
    ShellPrint (L"ASAN did not detect the violation!\n");
    return EFI_SUCCESS;
  }

  Buffer = AllocatePool (ASAN_TEST_BUFFER_SIZE);
  if (Buffer == NULL) {
    ShellPrint (L"Failed to allocate the test buffer\n");
    return EFI_OUT_OF_RESOURCES;
  }

  if (StrCmp (Argv[1], L"overflow") == 0) {
    ShellPrint (L"Writing 0x%p, 1 byte past the end of the buffer...\n", &Buffer[ASAN_TEST_BUFFER_SIZE]);
    Buffer[ASAN_TEST_BUFFER_SIZE] = 0xA5;
  } else if (StrCmp (Argv[1], L"underflow") == 0) {
    ShellPrint (L"Reading 0x%p, 1 byte before the buffer...\n", Buffer - 1);
    Value = *(Buffer - 1);
    ShellPrint (L"Read 0x%02x\n", Value);
  } else if (StrCmp (Argv[1], L"uaf") == 0) {
    FreePool ((VOID *)Buffer);
    ShellPrint (L"Reading 0x%p after it was freed...\n", Buffer);
    Value = Buffer[0];
    ShellPrint (L"Read 0x%02x\n", Value);
    return EFI_SUCCESS;
  } else if (StrCmp (Argv[1], L"dfree") == 0) {
    FreePool ((VOID *)Buffer);
    ShellPrint (L"Freeing 0x%p a second time...\n", Buffer);
    FreePool ((VOID *)Buffer);
    return EFI_SUCCESS;
  } else {
    FreePool ((VOID *)Buffer);
    AsanTestUsage ();
    return EFI_INVALID_PARAMETER;
  }

  // Reached only if the violation was missed; the buffer may be corrupted, so leak it.
  ShellPrint (L"ASAN did not detect the violation!\n");
  return EFI_SUCCESS;
}

/**
  Add the "asan" shell test command when AddressSanitizer is enabled.

**/
VOID
RegisterAsanTestCommand (
  VOID
  )
{
  STATIC BOOLEAN  Registered;

  // AsanLibNull reports no shadow, so the command is hidden in normal builds.
  if (Registered || (AsanGetShadowSize (EFI_PAGE_SIZE) == 0)) {
    return;
  }

  if (!EFI_ERROR (ShellAddCommand (&mShellCommandAsan))) {
    Registered = TRUE;
  }
}
