/** @file
  Shell command `fs` to view partition information

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BootloaderCommonLib.h>
#include <Library/ShellLib.h>
#include <Library/PartitionLib.h>
#include <Library/FileSystemLib.h>
#include <Library/MediaAccessLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Library/BootOptionLib.h>
#include <Guid/DeviceTableHobGuid.h>

STATIC  OS_BOOT_MEDIUM_TYPE mDeviceType   = OsBootDeviceMax;
STATIC  EFI_HANDLE          mHwPartHandle = NULL;
STATIC  EFI_HANDLE          mFsHandle     = NULL;

/**
  Show media information

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
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

  Count = 0;
  DeviceTable = (PLT_DEVICE_TABLE *)GetDeviceTable ();
  if (DeviceTable != NULL) {
    Count = DeviceTable->DeviceNumber;
    for (Index = 0; Index < Count; Index++) {
      DeviceType = (OS_BOOT_MEDIUM_TYPE)DeviceTable->Device[Index].Type;
      if (DeviceType >= OsBootDeviceMax)
        continue;

      ShellPrint (L"%d:%a ", DeviceType, GetBootDeviceNameString (DeviceType));
    }
  }
  ShellPrint (L"\n");
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
    mDeviceType = OsBootDeviceMax;
  }
  return EFI_SUCCESS;
}

/**
  Initialize file system

  @param[in]  DeviceType      Platform Device Index
  @param[in]  HwPartNo        HW Partition Number
  @param[in]  SwPartNo        SW Partition Number

  @retval     EFI_SUCCESS     FS init success
  @retval     Others          Error

**/
STATIC
EFI_STATUS
CmdFsInit (
  IN  OS_BOOT_MEDIUM_TYPE   DeviceType,
  IN  UINT32                HwPartNo,
  IN  UINT32                SwPartNo
  )
{
  EFI_STATUS          Status;
  OS_FILE_SYSTEM_TYPE FsType;
  UINTN               BaseAddress;

  // Get device bar
  BaseAddress = GetDeviceAddr (DeviceType, 0);
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
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"Success!\n");
    mDeviceType = DeviceType;
  } else {
    ShellPrint (L"Fail!\n");
    return EFI_ABORTED;
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

  Status = ListDir (mFsHandle, DirFilePath, ShellPrint);
  if (Status == EFI_NOT_FOUND) {
    ShellPrint (L"Not found!\n");
  } else if (Status == EFI_UNSUPPORTED) {
    ShellPrint (L"This command has been disabled.\n");
  }
  return EFI_SUCCESS;
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

  ShellPrint (L"Current Device: %a\n", (mDeviceType != OsBootDeviceMax) ?
    GetBootDeviceNameString (mDeviceType) : "Not Initialized");

  Status = GetPartitionCurrentPartNo (mHwPartHandle, &HwPartNo);
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"Current HwPart: %d\n", HwPartNo);
  } else {
    ShellPrint (L"Current HwPart: Not Initialized\n");
  }

  Status = GetFileSystemCurrentPartNo (mFsHandle, &SwPartNo);
  if (!EFI_ERROR (Status)) {
    ShellPrint (L"Current SwPart: %d\n", SwPartNo);
  } else {
    ShellPrint (L"Current SwPart: Not Initialized\n");
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
STATIC
EFI_STATUS
ShellCommandFsFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS          Status;
  OS_BOOT_MEDIUM_TYPE DeviceType;
  UINT32              HwPartNo;
  UINT32              SwPartNo;
  CHAR16             *SubCmd;

  if (Argc < 2) {
    goto Usage;
  }

  SubCmd = Argv[1];
  if (StrCmp (SubCmd, L"init") == 0) {
    DeviceType = (Argc < 3) ? 0 : (OS_BOOT_MEDIUM_TYPE)StrHexToUintn (Argv[2]);
    HwPartNo = (Argc < 4) ? 0 : StrHexToUintn (Argv[3]);
    SwPartNo = (Argc < 5) ? 0 : StrHexToUintn (Argv[4]);
    Status = CmdFsInit (DeviceType, HwPartNo, SwPartNo);
  } else if (StrCmp (SubCmd, L"close") == 0) {
    Status = CmdFsClose ();
  } else if (StrCmp (SubCmd, L"ls") == 0) {
    Status = CmdFsFsListDir ((Argc < 3) ? L"/" : Argv[2]);
  } else if (StrCmp (SubCmd, L"info") == 0) {
    Status = CmdFsInfo ();
  } else {
    goto Usage;
  }
  return Status;

Usage:
  ShellPrint (L"Usage: %s init [device no.] [hwpart no.] [swpart no.]\n", Argv[0]);
  ShellPrint (L"       %s close\n", Argv[0]);
  ShellPrint (L"       %s info\n", Argv[0]);
  ShellPrint (L"       %s ls [dir or file path]\n", Argv[0]);

  ShellPrint (L"\n  - [device no.] ");
  PrintPlatformDevices ();
  ShellPrint (L"  - [hwpart no.] hw partition or port number\n");
  ShellPrint (L"  - [swpart no.] logical partition index from mbr or gpt\n");

  return EFI_ABORTED;
}
