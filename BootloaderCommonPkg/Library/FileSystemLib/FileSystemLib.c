/** @file
  File system level API library interface

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

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
  EFI_STATUS   Status;
  UINT32      *Signature;

  Signature = (UINT32 *)FsHandle;

  Status = EFI_INVALID_PARAMETER;
  if (Signature != NULL) {
    if (*Signature == FS_FAT_SIGNATURE) {
      Status = FatGetFileByName (FsHandle, FileName, FileBuffer, FileSize);
    } else if (*Signature == FS_EXT_SIGNATURE) {
      Status = ExtGetFileByName (FsHandle, FileName, FileBuffer, FileSize);
    }
  }

  return Status;
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

  Status = EFI_INVALID_PARAMETER;
  if ((FsType == EnumFileSystemTypeFat) || (FsType == EnumFileSystemTypeAuto)) {
    Status = FatInitFileSystem (SwPart, PartHandle, FsHandle);
  }

  if (EFI_ERROR (Status)) {
    if ((FsType == EnumFileSystemTypeExt2) || (FsType == EnumFileSystemTypeAuto)) {
      Status = ExtInitFileSystem (SwPart, PartHandle, FsHandle);
    }
  }

  return Status;
}