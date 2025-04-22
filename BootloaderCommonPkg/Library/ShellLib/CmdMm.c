/** @file
  Shell command `mm` to read/write system memory, PCI config space
  and IO ports.

  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>

/**
  Read or write memory, PCI config space, or IO ports.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandMmFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandMm = {
  L"mm",
  L"Read or write memory, PCI config space, or IO ports",
  &ShellCommandMmFunc
};

/**
  Write memory address given from mm command.

  @param[in]  Addr         Starting address to write to
  @param[in]  Width        Width of the value to write
  @param[in]  Value        Value to write to the memory address
  @param[in]  Count        Number of times past the start to write
  @param[in]  IsIoAddr     If the address given is an IO address

**/
VOID
EFIAPI
MmWrite (
  IN UINTN  Addr,
  IN UINT32 Width,
  IN UINT64 Value,
  IN UINT32 Count,
  IN BOOLEAN IsIoAddr
)
{
  UINTN Index;

  for (Index = 0; Index < Count; Index++, Addr+=Width) {
    switch (Width) {
    case 1:
      if (IsIoAddr) {
        IoWrite8 (Addr, (UINT8)Value);
      } else {
        MmioWrite8 (Addr, (UINT8)Value);
      }
      break;
    case 2:
      if (IsIoAddr) {
        IoWrite16 (Addr, (UINT16)Value);
      } else {
        MmioWrite16 (Addr, (UINT16)Value);
      }
      break;
    case 4:
      if (IsIoAddr) {
        IoWrite32 (Addr, (UINT32)Value);
      } else {
        MmioWrite32 (Addr, (UINT32)Value);
      }
      break;
    case 8:
      if (IsIoAddr) {
        IoWrite64 (Addr, (UINT64)Value);
      } else {
        MmioWrite64 (Addr, (UINT64)Value);
      }
      break;
    default:
      break;
    }
  }
}

/**
  Read memory address given from mm command.

  @param[in]  Addr         Starting address to read from
  @param[in]  Width        Width of the value to read
  @param[in]  Count        Number of times past the start to read
  @param[in]  IsIoAddr     If the address given is an IO address

**/
VOID
EFIAPI
MmRead (
  IN UINTN   Addr,
  IN UINT32  Width,
  IN UINT32  Count,
  IN BOOLEAN IsIoAddr
)
{
  UINT32 Index;
  UINT32 Start;
  UINT32 ItemsPerRow;

  // Pad backward to 16-byte boundary
  ItemsPerRow  = 16 / Width;
  if (Count == 1) {
    Start = 0;
  } else {
    Start = (Addr & 0xF) / Width;
  }
  Count  += Start;

  if (Start > 0) {
    ShellPrint (L"%08X: ", Addr & ~0xF);
    for (Index = 0; Index < Start * (Width * 2 + 1); Index++) {
      ShellPrint (L" ");
    }
  }

  for (Index = Start; Index < Count; Index++, Addr+=Width) {
    if ((Index % ItemsPerRow) == 0) {
      ShellPrint (L"%08X: ", Addr);
    }
    switch (Width) {
    case 1:
      ShellPrint (L"%02X ", IsIoAddr ? IoRead8 (Addr) : MmioRead8 (Addr));
      break;
    case 2:
      ShellPrint (L"%04X ", IsIoAddr ? IoRead16 (Addr) : MmioRead16 (Addr));
      break;
    case 4:
      ShellPrint (L"%08X ", IsIoAddr ? IoRead32 (Addr) : MmioRead32 (Addr));
      break;
    case 8:
      ShellPrint (L"%016llX ", IsIoAddr ? IoRead64 (Addr) : MmioRead64 (Addr));
      break;
    default:
      break;
    }
    if ((Index % ItemsPerRow) == (ItemsPerRow - 1)) {
      ShellPrint (L"\n");
    }
  }

  if ((Index % ItemsPerRow) != 0) {
    ShellPrint (L"\n");
  }
}

/**
  Read or write system memory.

  @param[in]  Shell        Shell instance
  @param[in]  Argc         Number of command line arguments
  @param[in]  Argv         Command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandMmFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS Status;
  UINTN   Addr;
  UINT32  Index;
  UINT32  Width;
  UINT64  Value;
  UINT32  Count;
  BOOLEAN Write;
  BOOLEAN IsPciAddr;
  BOOLEAN IsIoAddr;

  Status    = EFI_SUCCESS;
  IsPciAddr = FALSE;
  IsIoAddr  = FALSE;
  Write     = FALSE;
  Width = 4;
  Value = 0;
  Count = 1;

  if ((Argc > 1) && (Argc < 9)) {
    Addr = StrHexToUintn (Argv[1]);
    //
    // Parse optional arguments
    //
    for (Index = 2; Index < Argc; Index++) {
      if (StrCmp (Argv[Index], L"-w") == 0) {
        if ((Index + 1) < Argc) {
          Width = (UINT32)StrHexToUintn (Argv[Index+1]);
          Index++;
        } else {
          ShellPrint (L"Error, '-w' requires [1|2|4|8]\n");
        }
      } else if (StrCmp (Argv[Index], L"-c") == 0) {
        if ((Index + 1) < Argc) {
          Count = (UINT32)StrHexToUintn (Argv[Index+1]);
          Index++;
        } else {
          ShellPrint (L"Error, '-c' requires a value\n");
        }
      } else if (StrCmp (Argv[Index], L"-pci") == 0) {
        IsPciAddr = TRUE;
      } else if (StrCmp (Argv[Index], L"-io") == 0) {
        IsIoAddr = TRUE;
      } else if (!Write) {
        Value = StrHexToUintn (Argv[Index]);
        Write = TRUE;
      } else {
        ShellPrint (L"Invalid parameter '%s'", Argv[Index]);
        Status = EFI_INVALID_PARAMETER;
      }
    }
  } else if (Argc >= 9) {
    ShellPrint (L"Error, too many arguments\n");
    Status = EFI_INVALID_PARAMETER;
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  if (!EFI_ERROR (Status)) {
    if ((Width != 1) && (Width != 2) && (Width != 4) && (Width != 8)) {
      ShellPrint (L"Error, width %u not supported\n", Width);
      return EFI_INVALID_PARAMETER;
    }
    // Make sure access is aligned
    if ((Addr & (Width - 1)) != 0) {
      ShellPrint (L"Error, unaligned access\n");
      return EFI_INVALID_PARAMETER;
    }

    if (IsIoAddr && IsPciAddr) {
      ShellPrint (L"Error, cannot be both -pci & -io\n");
      return EFI_INVALID_PARAMETER;
    } else if (IsPciAddr) {
      Addr = (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) +
        PCI_EXPRESS_LIB_ADDRESS ((Addr >> 24) & 0xFF, (Addr >> 16) & 0xFF, (Addr >> 8) & 0xFF, Addr & 0xFF));
    }

    if (Write) {
      MmWrite (Addr, Width, Value, Count, IsIoAddr);
    } else {
      MmRead (Addr, Width, Count, IsIoAddr);
    }
  }

  if (EFI_ERROR(Status)) {
    ShellPrint (L"Usage: %s <addr> [value] [-w <1|2|4|8>] [-c count] [-io | -pci] \n", Argv[0]);
    ShellPrint (L"       All input numeric values are interpreted as hexadecimal numbers\n");
    ShellPrint (L"       <addr> must be starting address for read/write\n");
    ShellPrint (L"       w : Width of single data unit in bytes. (Default = 4)\n");
    ShellPrint (L"       c : Number of data units. (Default = 1)\n");
    ShellPrint (L"       examples:\n");
    ShellPrint (L"       # Read 1 dword at the address\n");
    ShellPrint (L"         mm c0000000\n");
    ShellPrint (L"       # Read PCI config space for 0.2:0[0]\n");
    ShellPrint (L"         mm 00020000 -w 1 -c 100 -pci\n");
    ShellPrint (L"       # Write 256 dwords of 0 starting at address\n");
    ShellPrint (L"         mm c0000000 0 -w 4 -c 100\n");
    ShellPrint (L"       # Read 1 byte from IO port 80h\n");
    ShellPrint (L"         mm 80 -w 1 -io\n");
  }

  return Status;
}
