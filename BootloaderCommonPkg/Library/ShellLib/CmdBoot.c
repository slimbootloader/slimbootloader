/** @file
  Shell command `boot` to print or modify the OS boot option list.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FileSystemLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/BootOptionLib.h>
#include <Guid/OsBootOptionGuid.h>
#include "Shell.h"


/**
  Print or modify the OS boot option list.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandBootFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandBoot = {
  L"boot",
  L"Print or modify the OS boot option list",
  &ShellCommandBootFunc
};


/**
  Swap two of the OS boot entries.

  @param[in]      Shell          shell instance
  @param[in, out] Buffer         buffer for Shell user input
  @param[in]      BufferSize     size of the Buffer in bytes
  @param[out]     BootOptionList update the OS boot option HOB
                                 to swap the user specified boot entries

  @retval EFI_SUCCESS

**/
EFI_STATUS
SwapBootEntries (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION_LIST *BootOptionList
  )
{
  EFI_STATUS                 Status;
  BOOLEAN                    IsHex;
  UINT8                      Index1;
  UINT8                      Index2;
  OS_BOOT_OPTION             BootOption;

  if (BootOptionList->OsBootOptionCount < 2) {
    ShellPrint (L"Only 1 index available, cannot perform swap\n");
    return EFI_SUCCESS;
  } else if (BootOptionList->OsBootOptionCount == 2) {
    Index1 = 0;
    Index2 = 1;
  } else {
    do {
      ShellPrint (L"Enter first index to swap (0x0 to 0x%X)\n",
                  BootOptionList->OsBootOptionCount - 1
                  );
      Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Index1 = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
      if (Index1 < BootOptionList->OsBootOptionCount && (StrLen (Buffer) > 0)) {
        break;
      }
      ShellPrint (L"Invalid index '%s', please re-enter\n", Buffer);
    } while (1);

    do {
      ShellPrint (L"Enter second index to swap (0x0 to 0x%X)\n",
                  BootOptionList->OsBootOptionCount - 1
                  );
      Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Index2 = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
      if ((Index2 < BootOptionList->OsBootOptionCount) && (Index1 != Index2) && (StrLen (Buffer) > 0)) {
        break;
      }
      ShellPrint (L"Invalid or duplicate index '%s', please re-enter\n", Buffer);
    } while (1);
  }
  CopyMem (&BootOption, &BootOptionList->OsBootOption[Index1], sizeof (BootOption));
  CopyMem (&BootOptionList->OsBootOption[Index1], &BootOptionList->OsBootOption[Index2], sizeof (BootOption));
  CopyMem (&BootOptionList->OsBootOption[Index2], &BootOption, sizeof (BootOption));
  return EFI_SUCCESS;
}

/**
  Get the OS boot entry's file info.

  @param[in]      Shell        shell instance
  @param[in, out]  Buffer       buffer for Shell user input
  @param[in]      BufferSize   size of the Buffer in bytes
  @param[out]     BootOption   update the boot entry with the device type,
                               address, and partition info

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetBootDeviceInfo (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION      *BootOption
  )
{
  EFI_STATUS                 Status;
  BOOLEAN                    IsHex;

  do {
    ShellPrint (L"Enter DevType (SATA 0x%X, SD 0x%X, eMMC 0x%X, UFS 0x%X, SPI 0x%X, USB 0x%X, NVMe 0x%X)\n",
                OsBootDeviceSata, OsBootDeviceSd, OsBootDeviceEmmc, OsBootDeviceUfs, OsBootDeviceSpi, OsBootDeviceUsb, OsBootDeviceNvme
                );
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->DevType = (OS_BOOT_MEDIUM_TYPE) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));

    if ((BootOption->DevType < OsBootDeviceMax) && (StrLen (Buffer) > 0)) {
      break;
    }
    ShellPrint (L"Invalid DevType value '%s', please re-enter\n", Buffer);
  } while (1);

  do {
    ShellPrint (L"Enter DevInstance (uint)\n");
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->DevInstance = (UINT8)((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (StrLen (Buffer) > 0) {
      break;
    }
    ShellPrint (L"Invalid DevInstance value '%s', please re-enter\n", Buffer);
  } while (1);

  do {
    ShellPrint (L"Enter HwPart (uint)\n");
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->HwPart = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (StrLen (Buffer) > 0) {
      break;
    }
    ShellPrint (L"Invalid HwPart value '%s', please re-enter\n", Buffer);
  } while (1);

  do {
    ShellPrint (L"Enter FsType (FAT (0x%X), EXT2/3 (0x%X), Auto (0x%X), RAW (0x%X)\n",
                EnumFileSystemTypeFat, EnumFileSystemTypeExt2, EnumFileSystemTypeAuto,
                EnumFileSystemMax);
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->FsType = (OS_FILE_SYSTEM_TYPE) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if ((((UINT8)BootOption->FsType) <= ((UINT8)EnumFileSystemMax)) && (StrLen (Buffer) > 0)) {
      break;
    }
    ShellPrint (L"Invalid value '%s', please re-enter\n", Buffer);
  } while (1);

  if (BootOption->FsType == EnumFileSystemMax) {
    return EFI_SUCCESS;
  }

  //
  // Get software partition for file system
  //
  do {
    ShellPrint (L"Enter SwPart (uint)\n");
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->SwPart = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (StrLen (Buffer) > 0) {
      break;
    }
    ShellPrint (L"Invalid SwPart value '%s', please re-enter\n", Buffer);
  } while (1);

  return EFI_SUCCESS;
}

/**
  Get the OS boot entry's file info.

  @param[in]      Shell        shell instance
  @param[in, out] Buffer       buffer for Shell user input
  @param[in]      BufferSize   size of the Buffer in bytes
  @param[out]     BootOption   update the boot entry with the file system type
                               and file path user values

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetBootFileInfo (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION      *BootOption
  )
{
  EFI_STATUS                 Status;
  UINTN                      Length;

  do {
    ShellPrint (L"Enter file path string (max length of %d)\n",
                sizeof (BootOption->Image[0].FileName) - 1
                );
    Status = ShellReadLine (Shell, Buffer, BufferSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Length = StrLen (Buffer);
    if ((Length < sizeof (BootOption->Image[0].FileName)) && (Length > 0)) {
      UnicodeStrToAsciiStrS (Buffer, (CHAR8 *)BootOption->Image[0].FileName, sizeof (BootOption->Image[0].FileName));
      break;
    }
    ShellPrint (L"Invalid, too long/short: '%s' len=%d, please re-enter\n", Buffer, Length);
  } while (1);

  return EFI_SUCCESS;
}

/**
  Get the OS boot entry's LBA info.

  @param[in]      Shell        shell instance
  @param[in, out] Buffer       buffer for Shell user input
  @param[in]      BufferSize   size of the Buffer in bytes
  @param[out]     BootOption   update the boot entry with the LBA user values

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetBootLbaInfo (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION      *BootOption
  )
{
  EFI_STATUS                 Status;
  BOOLEAN                    IsHex;

  do {
    ShellPrint (L"Enter SwPart (uint)\n");
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->Image[0].LbaImage.SwPart = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (StrLen (Buffer) > 0) {
      break;
    }
    ShellPrint (L"Invalid SwPart value '%s', please re-enter\n", Buffer);
  } while (1);

  do {
    ShellPrint (L"Enter LBA Address (uint)\n");
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->Image[0].LbaImage.LbaAddr = (UINT32) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (StrLen (Buffer) > 0) {
      break;
    }
    ShellPrint (L"Invalid LBA Address value '%s', please re-enter\n", Buffer);
  } while (1);

  return EFI_SUCCESS;
}

/**
  Print the OS boot option list.

  @param[in]  OsBootOptionList    the OS boot option list

**/
VOID
PrintBootOption (
  OS_BOOT_OPTION_LIST        *OsBootOptionList
  )
{
  UINT32                     Index;
  OS_BOOT_OPTION             *BootOption;

  ShellPrint (L"Boot options (in HEX):\n\n");

  ShellPrint (L"Idx|ImgType|DevType|DevNum|Flags|HwPart|FsType|SwPart|File/Lbaoffset\n");
  for (Index = 0; Index < OsBootOptionList->OsBootOptionCount; Index++) {
    BootOption = &OsBootOptionList->OsBootOption[Index];
    if (BootOption->FsType < EnumFileSystemMax) {
      ShellPrint (L"%3x|%7x| %5a | %4x | %3x | %4x | %4a | %4x | %a\n", \
                 Index, \
                 BootOption->ImageType, \
                 GetBootDeviceNameString(BootOption->DevType), \
                 BootOption->DevInstance, \
                 BootOption->BootFlags, \
                 BootOption->HwPart,  \
                 GetFsTypeString (BootOption->FsType), \
                 BootOption->SwPart,  \
                 BootOption->Image[0].FileName \
                 );
    } else {
      ShellPrint (L"%3x|%7x| %5a | %4x | %3x | %4x | %4a | %4x | 0x%x\n", \
                 Index, \
                 BootOption->ImageType, \
                 GetBootDeviceNameString(BootOption->DevType), \
                 BootOption->DevInstance, \
                 BootOption->BootFlags, \
                 BootOption->HwPart,  \
                 GetFsTypeString (BootOption->FsType), \
                 BootOption->Image[0].LbaImage.SwPart, \
                 BootOption->Image[0].LbaImage.LbaAddr \
                 );
    }
  }

  ShellPrint (L"\n");
}

/**
  Print the the command usage.

  @param[in]  Argv    command line arguments

**/
VOID
PrintUsage (
  IN CHAR16 *Argv[]
  )
{
  ShellPrint (L"Usage: %s [-h|-p]\n", Argv[0]);
  ShellPrint (L"  -h: print the usage\n");
  ShellPrint (L"  -p: print the Boot Option List\n");
}

/**
  Print or modify the OS boot option list.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandBootFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINT16               Index;
  CHAR16               Buffer[0x100];
  OS_BOOT_OPTION_LIST  *BootOptionList;
  OS_BOOT_OPTION       BootOption;
  BOOLEAN              ResetAll;
  BOOLEAN              SkipArgParse;
  BOOLEAN              Swap;
  EFI_STATUS           Status;

  SkipArgParse = FALSE;
  Status = EFI_SUCCESS;

  if (Argc > 2) {
    PrintUsage (Argv);
    return Status;
  } else if (Argc == 1) {
    SkipArgParse = TRUE;
  }

  BootOptionList = GetBootOptionList ();
  if (BootOptionList == NULL || BootOptionList->OsBootOptionCount == 0) {
    ShellPrint (L"No Boot Option List found!\n");
    Status = EFI_ABORTED;
    goto ExitBootCmd;
  }

  if (!SkipArgParse) {
    if (StrCmp (Argv[1], L"-h") == 0) {
      PrintUsage (Argv);
      goto ExitBootCmd;
    } else if (StrCmp (Argv[1], L"-p") == 0) {
      PrintBootOption (BootOptionList);
      goto ExitBootCmd;
    } else {
      PrintUsage (Argv);
      goto ExitBootCmd;
    }
  }

  ResetAll = FALSE;
  Swap = FALSE;

  PrintBootOption (BootOptionList);
  do {
    ShellPrint (L"SubCommand:\n");
    ShellPrint (L"  s   -- swap boot order by index\n");
    ShellPrint (L"  a   -- modify all boot options one by one\n");
    ShellPrint (L"  q   -- quit boot option change\n");
    ShellPrint (L"  idx -- modify the boot option specified by idx (0");

    if (BootOptionList->OsBootOptionCount > 1) {
      ShellPrint (L" to 0x%X", BootOptionList->OsBootOptionCount - 1);
    }
    ShellPrint (L")\n");

    Status = ShellReadLine (Shell, Buffer, sizeof (Buffer));
    if (EFI_ERROR (Status)) {
      goto ExitBootCmd;
    }

    Index = 0;
    if (StrCmp (Buffer, L"a") == 0) {
      ResetAll = TRUE;
      break;
    } else if (StrCmp (Buffer, L"s") == 0) {
      Swap = TRUE;
      break;
    } else if (StrCmp (Buffer, L"q") == 0) {
      goto ExitBootCmd;
    } else {
      Index = (UINT16)StrHexToUintn (Buffer);
      if (Index < BootOptionList->OsBootOptionCount) {
        break;
      }
      ShellPrint (L"Invalid value '%s', please re-enter\n", Buffer);
    }
  } while (1);

  if (Swap) {
    Status = SwapBootEntries (Shell, Buffer, sizeof (Buffer), BootOptionList);
    if (EFI_ERROR (Status)) {
      goto ExitBootCmd;
    }
  } else {
    do {
      ZeroMem (&BootOption, sizeof (BootOption));
      Status = GetBootDeviceInfo (Shell, Buffer, sizeof (Buffer), &BootOption);
      if (EFI_ERROR (Status)) {
        goto ExitBootCmd;
      }

      if (BootOption.FsType != EnumFileSystemMax) {
        Status = GetBootFileInfo (Shell, Buffer, sizeof (Buffer), &BootOption);
        if (EFI_ERROR (Status)) {
          goto ExitBootCmd;
        }
      } else {
        Status = GetBootLbaInfo (Shell, Buffer, sizeof (Buffer), &BootOption);
        if (EFI_ERROR (Status)) {
          goto ExitBootCmd;
        }
      }
      CopyMem (&BootOptionList->OsBootOption[Index], &BootOption, sizeof (BootOption));
      if (!ResetAll) {
        break;
      }
      Index++;
    } while (Index < (BootOptionList->OsBootOptionCount - 1));
  }
  ShellPrint (L"Updated the Boot Option List\n");
  PrintBootOption (BootOptionList);

ExitBootCmd:
  if (EFI_ERROR (Status)) {
    ShellPrint (L"ERROR, exiting comamnd unsuccesfully\n");
  } else {
    BootOptionList->BootOptionReset = 1;
    BootOptionList->CurrentBoot     = 0;
  }
  return Status;
}
