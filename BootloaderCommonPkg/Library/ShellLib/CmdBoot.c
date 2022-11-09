/** @file
  Shell command `boot` to print or modify the OS boot option list.

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
  @param[in, out] Buffer       buffer for Shell user input
  @param[in]      BufferSize   size of the Buffer in bytes
  @param[out]     BootOption   update the boot entry with the device type,
                               address, and partition info
  @param[in]      CurrOption   the current boot entry value

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetBootDeviceInfo (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION      *BootOption,
  IN     OS_BOOT_OPTION      *CurrOption
  )
{
  EFI_STATUS                 Status;
  BOOLEAN                    IsHex;

  do {
    ShellPrint (L"Enter DevType (SATA 0x%X, SD 0x%X, eMMC 0x%X, UFS 0x%X, SPI 0x%X, USB 0x%X, NVMe 0x%X)\n",
                OsBootDeviceSata, OsBootDeviceSd, OsBootDeviceEmmc, OsBootDeviceUfs, OsBootDeviceSpi, OsBootDeviceUsb, OsBootDeviceNvme
                );
    ShellPrint (L"(default 0x%X) ", CurrOption->DevType);
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BootOption->DevType = (OS_BOOT_MEDIUM_TYPE) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));

    if (StrLen (Buffer) == 0) {
      BootOption->DevType = CurrOption->DevType;
      break;
    } else if (BootOption->DevType < OsBootDeviceMax) {
      break;
    }
    ShellPrint (L"Invalid DevType value '%s', please re-enter\n", Buffer);
  } while (1);

  ShellPrint (L"Enter DevInstance (uint)\n");
  ShellPrint (L"(default 0x%X) ", CurrOption->DevInstance);
  Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (StrLen (Buffer) == 0) {
    BootOption->DevInstance = CurrOption->DevInstance;
  } else {
    BootOption->DevInstance = (UINT8)((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
  }

  do {
    ShellPrint (L"Enter BootFlags (NORMAL 0x0, MISC 0x1, CRASH_OS 0x2, PREOS 0x4, EXTRA 0x8, MENDER 0x10)\n");
    ShellPrint (L"(default 0x%X) ", CurrOption->BootFlags);
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (StrLen (Buffer) == 0) {
      BootOption->BootFlags = CurrOption->BootFlags;
      break;
    }
    BootOption->BootFlags = (OS_FILE_SYSTEM_TYPE) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (((UINT8)BootOption->BootFlags) <= ((UINT8)BOOT_FLAGS_MISC | BOOT_FLAGS_CRASH_OS | BOOT_FLAGS_PREOS | BOOT_FLAGS_EXTRA | BOOT_FLAGS_MENDER)) {
      break;
    }
    ShellPrint (L"Invalid value '%s', please re-enter\n", Buffer);
  } while (1);

  ShellPrint (L"Enter HwPart (uint)\n");
  ShellPrint (L"(default 0x%X) ", CurrOption->HwPart);
  Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (StrLen (Buffer) == 0) {
    BootOption->HwPart = CurrOption->HwPart;
  } else {
    BootOption->HwPart = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
  }

  do {
    ShellPrint (L"Enter FsType (FAT (0x%X), EXT2/3/4 (0x%X), Auto (0x%X), RAW (0x%X))\n",
                EnumFileSystemTypeFat, EnumFileSystemTypeExt2, EnumFileSystemTypeAuto,
                EnumFileSystemMax);
    ShellPrint (L"(default 0x%X) ", CurrOption->FsType);
    Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if (StrLen (Buffer) == 0) {
      BootOption->FsType = CurrOption->FsType;
      break;
    }
    BootOption->FsType = (OS_FILE_SYSTEM_TYPE) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
    if (((UINT8)BootOption->FsType) <= ((UINT8)EnumFileSystemMax)) {
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
  ShellPrint (L"Enter SwPart (uint)\n");
  ShellPrint (L"(default 0x%X) ", CurrOption->SwPart);
  Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (StrLen (Buffer) == 0) {
    BootOption->SwPart = CurrOption->SwPart;
  } else {
    BootOption->SwPart = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
  }

  return EFI_SUCCESS;
}

/**
  Get the OS boot entry's file info.

  @param[in]      Shell        shell instance
  @param[in, out] Buffer       buffer for Shell user input
  @param[in]      BufferSize   size of the Buffer in bytes
  @param[out]     BootOption   update the boot entry with the file system type
                               and file path user values
  @param[in]      CurrOption   the current boot entry value

  @retval EFI_SUCCESS
  @retval EFI_NO_MAPPING       LoadImageType is not a valid Image index value

**/
EFI_STATUS
GetBootFileInfo (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION      *BootOption,
  IN     OS_BOOT_OPTION      *CurrOption,
  IN     UINT8               LoadImageType
  )
{
  EFI_STATUS                 Status;
  UINTN                      Length;

  if (LoadImageType >= LoadImageTypeMax) {
    ShellPrint (L"Invalid LoadImageType '0x%X'\n", LoadImageType);
    return EFI_NO_MAPPING;
  }

  do {
    if (LoadImageType == LoadImageTypeNormal){
      ShellPrint (L"Enter file path string (max length of %d)\n",
                  sizeof (BootOption->Image[LoadImageType].FileName) - 1
                  );
    } else if (LoadImageType == LoadImageTypePreOs){
        ShellPrint (L"Enter Pre-OS file path string (max length of %d)\n",
                    sizeof (BootOption->Image[LoadImageType].FileName) - 1
                    );
    } else if (LoadImageType == LoadImageTypeExtra0){
        ShellPrint (L"Enter Extra Image file path string (max length of %d)\n",
                    sizeof (BootOption->Image[LoadImageType].FileName) - 1
                    );
    }
    ShellPrint (L"(default '%a') ", CurrOption->Image[LoadImageType].FileName);
    Status = ShellReadLine (Shell, Buffer, BufferSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Length = StrLen (Buffer);
    if (Length == 0) {
      CopyMem (BootOption->Image[LoadImageType].FileName, CurrOption->Image[LoadImageType].FileName, sizeof (CurrOption->Image[LoadImageType].FileName));
      break;
    }
    if (Length < sizeof (BootOption->Image[LoadImageType].FileName)) {
      UnicodeStrToAsciiStrS (Buffer, (CHAR8 *)BootOption->Image[LoadImageType].FileName, sizeof (BootOption->Image[LoadImageType].FileName));
      break;
    }
    ShellPrint (L"Invalid, too long: '%s' len=%d, please re-enter\n", Buffer, Length);
  } while (1);

  return EFI_SUCCESS;
}

/**
  Get the OS boot entry's LBA info.

  @param[in]      Shell        shell instance
  @param[in, out] Buffer       buffer for Shell user input
  @param[in]      BufferSize   size of the Buffer in bytes
  @param[out]     BootOption   update the boot entry with the LBA user values
  @param[in]      CurrOption   the current boot entry value

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetBootLbaInfo (
  IN     SHELL               *Shell,
  IN OUT CHAR16              *Buffer,
  IN     UINTN               BufferSize,
  OUT    OS_BOOT_OPTION      *BootOption,
  IN     OS_BOOT_OPTION      *CurrOption
  )
{
  EFI_STATUS                 Status;
  BOOLEAN                    IsHex;

  ShellPrint (L"Enter SwPart (uint)\n");
  ShellPrint (L"(default 0x%X) ", CurrOption->Image[0].LbaImage.SwPart);
  Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (StrLen (Buffer) == 0) {
    BootOption->Image[0].LbaImage.SwPart = CurrOption->Image[0].LbaImage.SwPart;
  } else {
    BootOption->Image[0].LbaImage.SwPart = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
  }

  ShellPrint (L"Enter LBA Address (uint)\n");
  ShellPrint (L"(default 0x%X) ", CurrOption->Image[0].LbaImage.LbaAddr);
  Status = ShellReadUintn (Shell, Buffer, BufferSize, &IsHex);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (StrLen (Buffer) == 0) {
    BootOption->Image[0].LbaImage.LbaAddr = CurrOption->Image[0].LbaImage.LbaAddr;
  } else {
    BootOption->Image[0].LbaImage.LbaAddr = (UINT32) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
  }

  return EFI_SUCCESS;
}

/**
  Print Pre-OS or/and extra images.

  @param[in] BootOption     the boot options
  @param[in] Flags          the boot flags
  @param[in] ImageType      the image types

**/
VOID
PrintExtraImage (
OS_BOOT_OPTION *BootOption,
UINT8 Flags,
LOAD_IMAGE_TYPE ImageType
)
{
  if ((BootOption->BootFlags & Flags) != 0){
    if (BootOption->Image[ImageType].LbaImage.Valid == 1) {
      ShellPrint (L" [%x|0x%x]", ImageType, BootOption->Image[ImageType].LbaImage.SwPart,
      BootOption->Image[ImageType].LbaImage.LbaAddr);
    } else if (BootOption->Image[ImageType].FileName[0] != '\0') {
      ShellPrint (L" [%a]", BootOption->Image[ImageType].FileName);
    }
  }
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
      ShellPrint (L"%3x|%7x| %5a | %4x | %3x | %4x | %4a | %4x | %a", \
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
      ShellPrint (L"%3x|%7x| %5a | %4x | %3x | %4x | %4a | %4x | 0x%x", \
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
    //Print Pre-OS image filename
    PrintExtraImage (BootOption,BOOT_FLAGS_PREOS,LoadImageTypePreOs);
    //Print extra image filename
    PrintExtraImage (BootOption,BOOT_FLAGS_EXTRA,LoadImageTypeExtra0);

    if (Index == OsBootOptionList->CurrentBoot) {
      ShellPrint (L" *Current");
    }
    ShellPrint (L"\n");
  }
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
  OS_BOOT_OPTION       *CurrOption;
  BOOLEAN              SkipArgParse;
  BOOLEAN              ChangeCurrentIndex;
  BOOLEAN              IsHex;
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

  Swap = FALSE;
  ChangeCurrentIndex = FALSE;

  PrintBootOption (BootOptionList);
  do {
    ShellPrint (L"SubCommand:\n");
    ShellPrint (L"  q   -- quit boot option change\n");
    ShellPrint (L"  s   -- swap boot order by index\n");
    ShellPrint (L"  c   -- set the boot index\n");
    ShellPrint (L"  idx -- modify the boot option specified by idx (0");

    if (BootOptionList->OsBootOptionCount > 1) {
      ShellPrint (L" to %u", BootOptionList->OsBootOptionCount - 1);
    }
    ShellPrint (L")\n");

    Status = ShellReadLine (Shell, Buffer, sizeof (Buffer));
    if (EFI_ERROR (Status)) {
      goto ExitBootCmd;
    }

    Index = 0;
    if (StrCmp (Buffer, L"s") == 0) {
      Swap = TRUE;
      break;
    } else if (StrCmp (Buffer, L"c") == 0) {
      ChangeCurrentIndex = TRUE;
      break;
    } else if (StrCmp (Buffer, L"q") == 0) {
      goto ExitBootCmd;
    } else {
      Index = (UINT16)StrHexToUintn (Buffer);
      if ((StrLen (Buffer) > 0) && (Index < BootOptionList->OsBootOptionCount)) {
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
  } else if (ChangeCurrentIndex) {
    do {
      ShellPrint (L"Enter index to change to (0 to %u)\n",
                  BootOptionList->OsBootOptionCount - 1
                  );
      ShellPrint (L"(current index %u) ", BootOptionList->CurrentBoot);
      Status = ShellReadUintn (Shell, Buffer, sizeof (Buffer), &IsHex);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Index = (UINT8) ((IsHex) ? StrHexToUintn (Buffer) : StrDecimalToUintn (Buffer));
      if (StrLen (Buffer) == 0) {
        break;
      } else if (Index < BootOptionList->OsBootOptionCount) {
        BootOptionList->CurrentBoot = (UINT8) Index;
        BootOptionList->BootOptionReset = 0x1;
        break;
      }
      ShellPrint (L"Invalid index '%s', please re-enter\n", Buffer);
    } while (1);
  } else {
    CurrOption = &BootOptionList->OsBootOption[Index];
    ZeroMem (&BootOption, sizeof (BootOption));
    Status = GetBootDeviceInfo (Shell, Buffer, sizeof (Buffer), &BootOption, CurrOption);
    if (EFI_ERROR (Status)) {
      goto ExitBootCmd;
    }

    if (BootOption.FsType != EnumFileSystemMax) {
      Status = GetBootFileInfo (Shell, Buffer, sizeof (Buffer), &BootOption, CurrOption, LoadImageTypeNormal);
      if (EFI_ERROR (Status)) {
        goto ExitBootCmd;
      }
      if ((BootOption.BootFlags & BOOT_FLAGS_PREOS) != 0){
        Status = GetBootFileInfo (Shell, Buffer, sizeof (Buffer), &BootOption, CurrOption, LoadImageTypePreOs);
        if (EFI_ERROR (Status)) {
          goto ExitBootCmd;
        }
      }
      if ((BootOption.BootFlags & BOOT_FLAGS_EXTRA) != 0){
        Status = GetBootFileInfo (Shell, Buffer, sizeof (Buffer), &BootOption, CurrOption, LoadImageTypeExtra0);
        if (EFI_ERROR (Status)) {
          goto ExitBootCmd;
        }
      }
    } else {
      Status = GetBootLbaInfo (Shell, Buffer, sizeof (Buffer), &BootOption, CurrOption);
      if (EFI_ERROR (Status)) {
        goto ExitBootCmd;
      }
    }
    CopyMem (&BootOptionList->OsBootOption[Index], &BootOption, sizeof (BootOption));
  }
  ShellPrint (L"Updated the Boot Option List\n");
  PrintBootOption (BootOptionList);

ExitBootCmd:
  if (EFI_ERROR (Status)) {
    ShellPrint (L"ERROR, exiting command unsuccessfully\n");
  }
  return Status;
}
