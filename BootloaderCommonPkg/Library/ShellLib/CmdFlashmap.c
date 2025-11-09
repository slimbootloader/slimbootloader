/** @file
  Shell command `flashmap` to display flash memory map information.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Guid/FlashMapInfoGuid.h>

/**
  Display the flash memory map.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
ShellCommandFlashmapFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandFlashmap = {
  L"flashmap",
  L"Display flash memory map",
  &ShellCommandFlashmapFunc
};

/**
  Get flash map component description.

  @param[in] Signature    Component signature
  @retval Component description string
**/
STATIC
CONST CHAR8 *
GetComponentDescription (
  IN UINT32 Signature
  )
{
  switch (Signature) {
    case FLASH_MAP_SIG_STAGE1A:      return "Initial bootloader";
    case FLASH_MAP_SIG_STAGE1B:      return "Memory initialization";
    case FLASH_MAP_SIG_STAGE2:       return "Silicon initialization";
    case FLASH_MAP_SIG_ACM:          return "Authenticated Code Module";
    case FLASH_MAP_SIG_UCODE:        return "Microcode patches";
    case FLASH_MAP_SIG_MRCDATA:      return "Memory training data";
    case FLASH_MAP_SIG_VARIABLE:     return "Variable storage";
    case FLASH_MAP_SIG_UEFIVARIABLE: return "UEFI variable storage";
    case FLASH_MAP_SIG_PAYLOAD:      return "OsLoader payload";
    case FLASH_MAP_SIG_EPAYLOAD:     return "Extended payload";
    case FLASH_MAP_SIG_FWUPDATE:     return "Firmware update payload";
    case FLASH_MAP_SIG_CFGDATA:      return "Configuration data";
    case FLASH_MAP_SIG_BLRESERVED:   return "Bootloader reserved";
    case FLASH_MAP_SIG_BPM:          return "Boot Policy Manifest";
    case FLASH_MAP_SIG_EMPTY:        return "Empty region";
    default:                         return "Unknown component";
  }
}

/**
  Get flash map flags description.

  @param[in]  Flags       Flash map flags
  @param[out] FlagStr     Output string buffer (minimum 32 bytes)
**/
STATIC
VOID
GetFlagsDescription (
  IN  UINT32  Flags,
  OUT CHAR8   *FlagStr
  )
{
  UINTN Len = 0;
  FlagStr[0] = 0;

  if (Flags & FLASH_MAP_FLAGS_TOP_SWAP) {
    AsciiStrCatS (FlagStr, 32, "TS ");
    Len += 3;
  }
  if (Flags & FLASH_MAP_FLAGS_REDUNDANT_REGION) {
    AsciiStrCatS (FlagStr, 32, "RED ");
    Len += 4;
  }
  if (Flags & FLASH_MAP_FLAGS_NON_REDUNDANT_REGION) {
    AsciiStrCatS (FlagStr, 32, "NRED ");
    Len += 5;
  }
  if (Flags & FLASH_MAP_FLAGS_NON_VOLATILE_REGION) {
    AsciiStrCatS (FlagStr, 32, "NV ");
    Len += 3;
  }
  if (Flags & FLASH_MAP_FLAGS_COMPRESSED) {
    AsciiStrCatS (FlagStr, 32, "COMP ");
    Len += 5;
  }
  if (Flags & FLASH_MAP_FLAGS_BACKUP) {
    AsciiStrCatS (FlagStr, 32, "BAK ");
    Len += 4;
  }

  if (Len > 0) {
    FlagStr[Len - 1] = 0; // Remove trailing space
  } else {
    AsciiStrCpyS (FlagStr, 32, "-");
  }
}

/**
  Display the flash memory map.

  @param[in]  Shell        Shell instance
  @param[in]  Argc         Number of command line arguments
  @param[in]  Argv         Command line arguments

  @retval EFI_SUCCESS      Command completed successfully
  @retval EFI_ABORTED      Command was aborted
**/
EFI_STATUS
EFIAPI
ShellCommandFlashmapFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  FLASH_MAP               *FlashMap;
  FLASH_MAP_ENTRY_DESC    *FlashMapDesc;
  UINT32                  Index;
  UINT32                  EntryCount;
  CHAR8                   SigStr[5];
  CHAR8                   FlagStr[32];
  BOOLEAN                 ShowHelp;
  BOOLEAN                 Verbose;
  UINT32                  TotalSize;
  UINT32                  FlashBase;
  UINT32                  FlashAddress;

  ShowHelp = FALSE;
  Verbose = FALSE;

  // Parse command line arguments
  if (Argc > 1) {
    for (Index = 1; Index < Argc; Index++) {
      if ((StrCmp (Argv[Index], L"-h") == 0) || (StrCmp (Argv[Index], L"--help") == 0)) {
        ShowHelp = TRUE;
        break;
      } else if ((StrCmp (Argv[Index], L"-v") == 0) || (StrCmp (Argv[Index], L"--verbose") == 0)) {
        Verbose = TRUE;
      }
    }
  }

  if (ShowHelp) {
    ShellPrint (L"\nDisplay flash memory map information\n\n");
    ShellPrint (L"Usage: flashmap [OPTIONS]\n\n");
    ShellPrint (L"OPTIONS:\n");
    ShellPrint (L"  -h, --help     Display this help message\n");
    ShellPrint (L"  -v, --verbose  Display verbose information\n\n");
    ShellPrint (L"Flash Map Flags:\n");
    ShellPrint (L"  TS    - Top Swap region\n");
    ShellPrint (L"  RED   - Redundant region\n");
    ShellPrint (L"  NRED  - Non-redundant region\n");
    ShellPrint (L"  NV    - Non-volatile region\n");
    ShellPrint (L"  COMP  - Compressed\n");
     ShellPrint (L"  BAK   - Backup region\n\n");
    return EFI_SUCCESS;
  }

  // Find Flash Map HOB
  GuidHob = GetFirstGuidHob (&gFlashMapInfoGuid);
  if (GuidHob == NULL) {
    ShellPrint (L"Error: Flash Map information not found\n");
    return EFI_NOT_FOUND;
  }

  FlashMap = (FLASH_MAP *)GET_GUID_HOB_DATA (GuidHob);
  // Validate flash map header
  if (FlashMap->Signature != FLASH_MAP_SIG_HEADER) {
    ShellPrint (L"Error: Invalid Flash Map signature\n");
    return EFI_INVALID_PARAMETER;
  }

  // Calculate number of entries
  EntryCount = (FlashMap->Length - sizeof(FLASH_MAP)) / sizeof(FLASH_MAP_ENTRY_DESC);

  // Calculate flash base address (typically 0xFFFFFFFF + 1 - ROM Size)
  FlashBase = (UINT32)(0x100000000ULL - FlashMap->RomSize);

  // Display header information
  SigStr[4] = '\0';
  ShellPrint (L"\n=== Flash Memory Map ===\n");
  if (Verbose) {
    *(UINT32 *)SigStr = FlashMap->Signature;
    ShellPrint (L"Signature  : %a (0x%08X)\n", SigStr, FlashMap->Signature);
    ShellPrint (L"Version    : %d\n", FlashMap->Version);
    ShellPrint (L"Length     : %d bytes\n", FlashMap->Length);
    ShellPrint (L"Attributes : 0x%02X\n", FlashMap->Attributes);
    ShellPrint (L"BIOS Size  : 0x%08X (%d MB)\n", FlashMap->RomSize, FlashMap->RomSize / (1024 * 1024));
    ShellPrint (L"BIOS Base  : 0x%08X\n", FlashBase);
    ShellPrint (L"Entries    : %d\n\n", EntryCount);
  } else {
    ShellPrint (L"BIOS Size: 0x%08X (%d MB), BIOS Region Base: 0x%08X, Entries: %d\n\n",
                FlashMap->RomSize, FlashMap->RomSize / (1024 * 1024), FlashBase, EntryCount);
  }

  // Display table header - different format for verbose vs normal mode
  if (Verbose) {
    ShellPrint (L"Idx  Sign  Offset     Flash Addr Size       Flags        Description\n");
    ShellPrint (L"---- ----  ---------- ---------- ---------- ------------ -----------\n");
  } else {
    ShellPrint (L"Idx  Sign  Offset     Flash Addr Size       Flags\n");
    ShellPrint (L"---- ----  ---------- ---------- ---------- ----------\n");
  }

  // Display flash map entries
  FlashMapDesc = FlashMap->EntryDesc;
  TotalSize = 0;

  for (Index = 0; Index < EntryCount; Index++) {
    *(UINT32 *)SigStr = FlashMapDesc[Index].Signature;
    GetFlagsDescription (FlashMapDesc[Index].Flags, FlagStr);

    // Calculate actual flash address
    FlashAddress = FlashBase + FlashMapDesc[Index].Offset;

    if (Verbose) {
      ShellPrint (L"%4d %-4a  0x%08X 0x%08X 0x%08X %-12a %a\n",
                  Index,
                  SigStr,
                  FlashMapDesc[Index].Offset,
                  FlashAddress,
                  FlashMapDesc[Index].Size,
                  FlagStr,
                  GetComponentDescription (FlashMapDesc[Index].Signature));
    } else {
      ShellPrint (L"%4d %-4a  0x%08X 0x%08X 0x%08X %-10a\n",
                  Index,
                  SigStr,
                  FlashMapDesc[Index].Offset,
                  FlashAddress,
                  FlashMapDesc[Index].Size,
                  FlagStr);
    }
    TotalSize += FlashMapDesc[Index].Size;
  }

  if (Verbose) {
    ShellPrint (L"---- ----  ---------- ---------- ---------- ------------ -----------\n");
  } else {
    ShellPrint (L"---- ----  ---------- ---------- ---------- ----------\n");
  }
  ShellPrint (L"Total mapped size: 0x%08X (%d KB)\n", TotalSize, TotalSize / 1024);

  return EFI_SUCCESS;
}
