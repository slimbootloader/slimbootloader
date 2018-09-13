/** @file
  Shell command `mem` to read/write system memory.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ShellLib.h>

/**
  Read or write system memory.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
ShellCommandMemFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMem = {
  L"mem",
  L"Read or write system memory",
  &ShellCommandMemFunc
};

/**
  Read or write system memory.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
ShellCommandMemFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINTN   Index, Start, ItemsPerRow, Addr, Count, Width;
  BOOLEAN Write;
  UINT64  Value;

  Width = 4;
  Write = FALSE;
  Value = 0;

  if ((Argc == 2) || (Argc == 3)) {
    // Read 1 element
    Count = 1;
  } else if (Argc == 4) {
    // Read multiple elements
    Count = StrHexToUintn (Argv[3]);
  } else if ((Argc == 5) && (StrCmp (Argv[3], L"=") == 0)) {
    // Write 1 element
    Write = TRUE;
    Value = StrHexToUint64 (Argv[4]);
  } else {
    // Invalid
    goto usage;
  }

  // Parse access width
  if (Argc > 2) {
    Width = StrHexToUintn (Argv[2]);
    if ((Width != 1) && (Width != 2) && (Width != 4) && (Width != 8)) {
      ShellPrint (L"Error: Width must be 1, 2, 4, or 8\n");
      return EFI_INVALID_PARAMETER;
    }
  }

  // Parse access address
  Addr = StrHexToUintn (Argv[1]);

  // Make sure access is aligned
  if ((Addr & (Width - 1)) != 0) {
    ShellPrint (L"Error: Unaligned access\n");
    return EFI_INVALID_PARAMETER;
  }

  if (Write) {
    switch (Width) {
    case 1:
      * (volatile UINT8 *)Addr = (UINT8 )Value;
      break;
    case 2:
      * (volatile UINT16 *)Addr = (UINT16)Value;
      break;
    case 4:
      * (volatile UINT32 *)Addr = (UINT32)Value;
      break;
    case 8:
      * (volatile UINT64 *)Addr = (UINT64)Value;
      break;
    default:
      break;
    }
  } else {
    // Pad backward to 16-byte boundary
    ItemsPerRow  = 16 / Width;
    Start        = (Addr & 0xf) / Width;
    Count       += Start;

    if (Start > 0) {
      ShellPrint (L"%08x: ", Addr & ~0xf);
      for (Index = 0; Index < Start * (Width * 2 + 1); Index++) {
        ShellPrint (L" ");
      }
    }

    for (Index = Start; Index < Count; Index++) {
      if ((Index % ItemsPerRow) == 0) {
        ShellPrint (L"%08x: ", Addr);
      }

      switch (Width) {
      case 1:
        ShellPrint (L"%02x ",    * (volatile UINT8 *)Addr);
        break;
      case 2:
        ShellPrint (L"%04x ",    * (volatile UINT16 *)Addr);
        break;
      case 4:
        ShellPrint (L"%08x ",    * (volatile UINT32 *)Addr);
        break;
      case 8:
        ShellPrint (L"%016llx ", * (volatile UINT64 *)Addr);
        break;
      default:
        break;
      }

      if ((Index % ItemsPerRow) == (ItemsPerRow - 1)) {
        ShellPrint (L"\n");
      }

      Addr += Width;
    }

    if ((Index % ItemsPerRow) != 0) {
      ShellPrint (L"\n");
    }
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s <addr> [<width> [<count> | = <val>]]\n", Argv[0]);
  return EFI_ABORTED;
}
