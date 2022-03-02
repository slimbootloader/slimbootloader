/** @file
  Shell command `fs` to view partition information

  Copyright (c) 2019 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BootloaderCommonLib.h>
#include <Library/ShellLib.h>
#include <Library/PartitionLib.h>
#include <Library/FileSystemLib.h>
#include <Library/MediaAccessLib.h>
#include <Library/ConsoleInLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/BootOptionLib.h>
#include <Guid/DeviceTableHobGuid.h>

STATIC  OS_BOOT_MEDIUM_TYPE mDeviceType     = OsBootDeviceMax;
STATIC  UINT8               mDeviceInstance = 0;
STATIC  EFI_HANDLE          mHwPartHandle   = NULL;
STATIC  EFI_HANDLE          mFsHandle       = NULL;

/**
  Show media information

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandFsFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandFs = {
  L"fs",
  L"filesystem access command",
  &ShellCommandFsFunc
};

/**
  Print available platform devices

  @retval     none

**/
STATIC
VOID
PrintPlatformDevices (
  IN  VOID
  )
{
  PLT_DEVICE_TABLE   *DeviceTable;
  UINT16              Count;
  UINT16              Index;
  OS_BOOT_MEDIUM_TYPE DeviceType;
  UINT8               DeviceInstance;

  Count = 0;
  DeviceTable = (PLT_DEVICE_TABLE *)GetDeviceTable ();
  if (DeviceTable != NULL) {
    Count = DeviceTable->DeviceNumber;
    for (Index = 0; Index < Count; Index++) {
      DeviceType = (OS_BOOT_MEDIUM_TYPE)DeviceTable->Device[Index].Type;
      if (DeviceType >= OsBootDeviceMax)
        continue;

      DeviceInstance = DeviceTable->Device[Index].Instance;
      ShellPrint (L"  %d:%d %a", DeviceType, DeviceInstance, GetBootDeviceNameString (DeviceType));
      if (DeviceTable->Device[Index].Dev.PciDev.IsMmioDevice != 0) {
        ShellPrint (L"(MEM 0x%x)\n", DeviceTable->Device[Index].Dev.DevAddr);
      } else {
        ShellPrint (L"(PCI 0x%x:0x%x:0x%x)\n",
          DeviceTable->Device[Index].Dev.PciDev.PciBusNumber,
          DeviceTable->Device[Index].Dev.PciDev.PciDeviceNumber,
          DeviceTable->Device[Index].Dev.PciDev.PciFunctionNumber);
      }
    }
  }
}

/**
  Close file system

  @retval     EFI_SUCCESS

**/
STATIC
EFI_STATUS
CmdFsClose (
  IN  VOID
  )
{
  if (mFsHandle != NULL) {
    CloseFileSystem (mFsHandle);
    mFsHandle = NULL;
  }
  if (mHwPartHandle != NULL) {
    ClosePartitions (mHwPartHandle);
    mHwPartHandle = NULL;
  }

  if (mDeviceType != OsBootDeviceMax) {
    // De-init the current media device
    // If USB keyboard console is used, don't DeInit USB yet.
    if (!((mDeviceType == OsBootDeviceUsb) &&
        ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInUsbKeyboard) != 0))) {
      MediaInitialize (0, DevDeinit);
    }
    mDeviceType = OsBootDeviceMax;
    mDeviceInstance = 0;
  }

  return EFI_SUCCESS;
}

/**
  Initialize file system

  @param[in]  DeviceType      Platform Device Index
  @param[in]  DeviceInstance  The device instance number starting from 0.
  @param[in]  HwPartNo        HW Partition Number
  @param[in]  SwPartNo        SW Partition Number

  @retval     EFI_SUCCESS     FS init success
  @retval     Others          Error

**/
STATIC
EFI_STATUS
CmdFsInit (
  IN  OS_BOOT_MEDIUM_TYPE   DeviceType,
  IN  UINT8                 DeviceInstance,
  IN  UINT32                HwPartNo,
  IN  UINT32                SwPartNo
  )
{
  EFI_STATUS          Status;
  OS_FILE_SYSTEM_TYPE FsType;
  UINTN               BaseAddress;

  // Get device bar
  BaseAddress = GetDeviceAddr (DeviceType, DeviceInstance);
  if (BaseAddress == 0) {
    ShellPrint (L"Device no. %d (%a) is not in platform devices. <device no.> - ",
      DeviceType, GetBootDeviceNameString (DeviceType));
    PrintPlatformDevices ();
    return EFI_ABORTED;
  } else if (!(BaseAddress & 0xFF000000)) {
    BaseAddress = TO_MM_PCI_ADDRESS (BaseAddress);
  }

  if (mDeviceType != OsBootDeviceMax) {
    CmdFsClose ();
  }

  // Set media interface
  Status = MediaSetInterfaceType (DeviceType);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = MediaInitialize (BaseAddress, DevInitAll);
  ShellPrint (L"Media(%a) Init ", GetBootDeviceNameString (DeviceType));
  mDeviceType = DeviceType;
  mDeviceInstance = DeviceInstance;
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"Success!\n");
  } else {
    ShellPrint (L"Fail!\n");
    goto Error;
  }

  // Find a partition
  ShellPrint (L"Find a Partition\n");
  Status = FindPartitions (HwPartNo, &mHwPartHandle);
  ShellPrint (L"Partition Detect (device:%d, hwpart:%d) ", mDeviceType, HwPartNo);
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"Success!\n");
  } else {
    ShellPrint (L"Fail!\n");
    goto Error;
  }

  ShellPrint (L"Init FileSystem\n");
  Status = InitFileSystem (SwPartNo, EnumFileSystemTypeAuto, mHwPartHandle, &mFsHandle);
  ShellPrint (L"FileSystem Detect (device:%d, hwpart:%d, swpart:%d) ",
    mDeviceType, HwPartNo, SwPartNo);
  if (!EFI_ERROR (Status)) {
    FsType = GetFileSystemType (mFsHandle);
    ShellPrint (L"Success! (%a)\n", GetFsTypeString (FsType));
  } else {
    ShellPrint (L"Fail!\n");
    goto Error;
  }

  return EFI_SUCCESS;

Error:
  CmdFsClose ();
  return Status;
}

/**
  Print directory or file list

  @retval EFI_SUCCESS
  @retval EFI_ABORTED

**/
STATIC
EFI_STATUS
CmdFsFsListDir (
  IN  CHAR16      *DirFilePath
  )
{
  EFI_STATUS      Status;

  if (mFsHandle == NULL) {
    ShellPrint (L"FileSystem is not initialized!\n");
    return EFI_ABORTED;
  }

  Status = ListDir (mFsHandle, DirFilePath);
  if (Status == EFI_NOT_FOUND) {
    ShellPrint (L"Not found!\n");
  } else if (Status == EFI_UNSUPPORTED) {
    ShellPrint (L"This command has been disabled.\n");
  }
  return EFI_SUCCESS;
}

/**
  Load a file into memory.

  @param[in]  File path to load.
  @param[in]  LoadAddr   Memory address to load.

  @retval EFI_SUCCESS   File was loaded successfully.
  @retval EFI_ABORTED   File system is not initialized.
  @retval EFI_OUT_OF_RESOURCES  Insufficant memory.
  @retval EFI_BAD_BUFFER_SIZE   File size is 0.

**/
STATIC
EFI_STATUS
CmdFsLoad (
  IN  CHAR16      *FilePath,
  IN  UINTN        LoadAddr
  )
{
  EFI_STATUS      Status;
  UINTN           ImageSize;
  VOID           *Image;
  EFI_HANDLE      FsHandle;
  EFI_HANDLE      FileHandle;

  FsHandle = mFsHandle;
  if (FsHandle == NULL) {
    ShellPrint (L"FileSystem is not initialized!\n");
    return EFI_ABORTED;
  }

  FileHandle = NULL;
  Status = OpenFile (FsHandle, FilePath, &FileHandle);
  if (EFI_ERROR (Status)) {
    goto FileLoadErrOut;
  }

  Status = GetFileSize (FileHandle, &ImageSize);
  if (EFI_ERROR (Status)) {
    goto FileLoadErrOut;
  }

  if (ImageSize == 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    goto FileLoadErrOut;
  }

  if (LoadAddr == 0) {
    // Allocate space for loading
    Image = AllocatePages (EFI_SIZE_TO_PAGES (ImageSize));
    if (Image == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto FileLoadErrOut;
    }
  } else {
    Image = (VOID *)LoadAddr;
  }

  Status = ReadFile (FileHandle, &Image, &ImageSize);
  if (EFI_ERROR (Status)) {
    if (Image != NULL) {
      FreePool (Image);
    }
    goto FileLoadErrOut;
  }

FileLoadErrOut:
  if (EFI_ERROR (Status)) {
    ShellPrint (L"Failed to load file: %r\n", Status);
  } else {
    ShellPrint (L"File was loaded at 0x%p with length 0x%X!\n", Image, ImageSize);
  }

  if (FileHandle != NULL) {
    CloseFile (FileHandle);
  }

  return Status;
}

/**
  Display current filesystem info

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
CmdFsInfo (
  IN  VOID
  )
{
  EFI_STATUS            Status;
  UINT32                HwPartNo;
  UINT32                SwPartNo;
  OS_FILE_SYSTEM_TYPE   FsType;

  ShellPrint (L"Current DeviceType: %a\n", (mDeviceType != OsBootDeviceMax) ?
    GetBootDeviceNameString (mDeviceType) : "Not Initialized");

  ShellPrint (L"Current DeviceInstance: %d\n", mDeviceInstance);

  ShellPrint (L"Current HwPart: ");
  Status = GetPartitionCurrentPartNo (mHwPartHandle, &HwPartNo);
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"%d\n", HwPartNo);
  } else {
    ShellPrint (L"Not Initialized\n");
  }

  ShellPrint (L"Current SwPart: ");
  Status = GetFileSystemCurrentPartNo (mFsHandle, &SwPartNo);
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"%d\n", SwPartNo);
  } else {
    ShellPrint (L"Not Initialized\n");
  }

  FsType = GetFileSystemType (mFsHandle);
  ShellPrint (L"Current FileSystem: %a\n", (FsType != EnumFileSystemMax) ?
    GetFsTypeString (FsType) : "Not Detected");

  return EFI_SUCCESS;
}

/**
  Basic file system test commands

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandFsFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS          Status;
  OS_BOOT_MEDIUM_TYPE DeviceType;
  UINT8               DeviceInstance;
  UINT32              HwPartNo;
  UINT32              SwPartNo;
  CHAR16             *SubCmd;
  CHAR16             *String;
  UINTN               Result;
  UINTN               LoadAddr;

  if (Argc < 2) {
    goto Usage;
  }

  SubCmd = Argv[1];
  if (StrCmp (SubCmd, L"init") == 0) {
    if (Argc < 3) {
      DeviceType = 0;
      DeviceInstance = 0;
    } else {
      Status = StrHexToUintnS (Argv[2], &String, &Result);
      if (EFI_ERROR (Status)) {
        goto Usage;
      }
      DeviceType = (OS_BOOT_MEDIUM_TYPE)Result;

      Result = 0;
      if (String[0] == L':') {
        String++;
        Status = StrHexToUintnS (String, NULL, &Result);
        if (EFI_ERROR (Status)) {
          goto Usage;
        }
      }
      DeviceInstance = (UINT8)Result;
    }

    HwPartNo = (Argc < 4) ? 0 : (UINT32)StrHexToUintn (Argv[3]);
    SwPartNo = (Argc < 5) ? 0 : (UINT32)StrHexToUintn (Argv[4]);
    Status = CmdFsInit (DeviceType, DeviceInstance, HwPartNo, SwPartNo);
  } else if (StrCmp (SubCmd, L"close") == 0) {
    Status = CmdFsClose ();
  } else if (StrCmp (SubCmd, L"ls") == 0) {
    Status = CmdFsFsListDir ((Argc < 3) ? L"/" : Argv[2]);
  } else if (StrCmp (SubCmd, L"info") == 0) {
    Status = CmdFsInfo ();
  } else if (StrCmp (SubCmd, L"load") == 0) {
    LoadAddr = (Argc < 4) ? 0 : StrHexToUintn (Argv[3]);
    Status = CmdFsLoad ((Argc < 3) ? L"/" : Argv[2], LoadAddr);
  } else {
    goto Usage;
  }
  return Status;

Usage:
  ShellPrint (L"Usage: %s init [DevType[:DevInstance]] [HwPart] [SwPart]\n", Argv[0]);
  ShellPrint (L"       %s close\n", Argv[0]);
  ShellPrint (L"       %s info\n", Argv[0]);
  ShellPrint (L"       %s ls [dir or file path]\n", Argv[0]);
  ShellPrint (L"       %s load [dir or file path] [Address]\n", Argv[0]);

  ShellPrint (L"\nDevType:DevInstance - Media type and instance number in the same media type\n");
  PrintPlatformDevices ();
  ShellPrint (L"HwPart - HW partition or port number\n");
  ShellPrint (L"SwPart - Logical partition number from MBR or GPT\n");

  return EFI_ABORTED;
}
