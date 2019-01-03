/** @file
  File system level API library interface

Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials are licensed and made available
under the terms and conditions of the BSD License which accompanies this
distribution. The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FileSystemLib.h>
#include <Library/PcdLib.h>

OS_FILE_SYSTEM_TYPE     mCurrentFsType = EnumFileSystemMax;
FILE_SYSTEM_FUNC        mFileSystemFuncs[EnumFileSystemTypeAuto];

/**
Loads file into memory by its name.

  @param[in]     FsHandle         FAT file system handle.
  @param[in]     FileName         The file name to get.
  @param[out]    FileBufferPtr    Allocated file buffer pointer.
  @param[out]    FileSize         Pointer to file buffer size.

  @retval EFI_SUCCESS             The file was loaded correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_DEVICE_ERROR        A device error occurred.
  @retval EFI_NOT_FOUND           A requested file cannot be found.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.
  @retval EFI_BUFFER_TOO_SMALL    Buffer size is too small.

**/
EFI_STATUS
EFIAPI
GetFileByName (
  IN EFI_HANDLE                                    FsHandle,
  IN CHAR16                                       *FileName,
  OUT VOID                                       **FileBuffer,
  OUT UINTN                                       *FileSize
  )
{

  if (mCurrentFsType >= EnumFileSystemTypeAuto) {
    return EFI_NOT_READY;
  }

  if (mFileSystemFuncs[mCurrentFsType].GetFileByName == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mFileSystemFuncs[mCurrentFsType].GetFileByName (FsHandle, FileName, FileBuffer, FileSize);

}

/**
Initialize file systems.

  @param[in]     SwPart           Software partition index.
  @param[in]     FsType           Filesystem type.
  @param[in]     PartHandle       Partition handle.
  @param[out]    FsHandle         FAT file system handle.

  @retval EFI_SUCCESS             The file system was initialized successfully.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_NOT_FOUND           FAT file system was not detected on this partition.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
InitFileSystem (
  IN  UINT32                                       SwPart,
  IN  UINT32                                       FsType,
  IN  EFI_HANDLE                                   PartHandle,
  OUT EFI_HANDLE                                  *FsHandle
  )
{
  EFI_STATUS   Status;
  UINT32       Type;

  Status = EFI_INVALID_PARAMETER;

  if (FsType >= EnumFileSystemMax) {
    return Status;
  }

  if (mCurrentFsType == EnumFileSystemMax) {
    Type = EnumFileSystemTypeFat;
    if (FixedPcdGet32 (PcdSupportedFileSystemMask) & (1 << Type)) {
      mFileSystemFuncs[Type].InitFileSystem     = FatInitFileSystem;
      mFileSystemFuncs[Type].GetFileByName      = FatGetFileByName;
    }

    Type = EnumFileSystemTypeExt2;
    if (FixedPcdGet32 (PcdSupportedFileSystemMask) & (1 << Type)) {
      mFileSystemFuncs[Type].InitFileSystem     = ExtInitFileSystem;
      mFileSystemFuncs[Type].GetFileByName      = ExtGetFileByName;
    }
    mCurrentFsType = EnumFileSystemTypeAuto;
  }

  for (Type = EnumFileSystemTypeFat; Type < EnumFileSystemTypeAuto; Type++) {
    if (mFileSystemFuncs[Type].InitFileSystem == NULL) {
      continue;
    }
    if ((FsType == EnumFileSystemTypeAuto) || (FsType == Type)) {
      Status = mFileSystemFuncs[Type].InitFileSystem (SwPart, PartHandle, FsHandle);
      if (!EFI_ERROR (Status)) {
        mCurrentFsType = Type;
        break;
      }
    }
  }

  return Status;
}
