/** @file
  Shell command `io` to read/write I/O ports.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ShellLib.h>
#include <Library/IoLib.h>


/**
  Read or write I/O ports.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandIoFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandIo = {
  L"io",
  L"Read or write I/O ports",
  &ShellCommandIoFunc
};

/**
  Read or write I/O ports.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandIoFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINTN   Addr, Width;
  BOOLEAN Write;
  UINT64  Value;

  Value = 0;

  if ((Argc == 2) || (Argc == 3)) {
    // Read 1 element
    Write = FALSE;
  } else if (Argc == 4) {
    // Write 1 element
    Write = TRUE;
    Value = StrHexToUint64 (Argv[3]);
  } else {
    // Invalid
    goto usage;
  }

  // Parse access width
  if (Argc > 2) {
    Width = StrHexToUintn (Argv[2]);
    if ((Width != 1) && (Width != 2) && (Width != 4)) {
      ShellPrint (L"Error: Width must be 1, 2, or 4\n");
      return EFI_INVALID_PARAMETER;
    }
  } else {
    Width = 1;
  }

  // Parse access address
  Addr = StrHexToUintn (Argv[1]);

  if (Write) {
    switch (Width) {
    case 1:
      IoWrite8 (Addr,  (UINT8)Value);
      break;
    case 2:
      IoWrite16 (Addr, (UINT16)Value);
      break;
    case 4:
      IoWrite32 (Addr, (UINT32)Value);
      break;
    default:
      break;
    }
  } else {
    ShellPrint (L"0x%x: ", Addr);
    switch (Width) {
    case 1:
      ShellPrint (L"0x%02x", IoRead8 (Addr));
      break;
    case 2:
      ShellPrint (L"0x%04x", IoRead16 (Addr));
      break;
    case 4:
      ShellPrint (L"0x%08x", IoRead32 (Addr));
      break;
    default:
      break;
    }
    ShellPrint (L"\n");
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s <addr> [<width> [<val>]]\n", Argv[0]);
  return EFI_ABORTED;
}
