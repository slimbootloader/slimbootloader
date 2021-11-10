/** @file
  Function prototypes for FAT library

Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FAT_LIB_H_
#define _FAT_LIB_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PartitionLib.h>
#include <Library/FileSystemLib.h>

#define FS_FAT_SIGNATURE    SIGNATURE_32 ('p', 'f', 'a', 't')

/**
Initialize FAT file system volumes.

  @param[in]     SwPart           Software partition index.
  @param[in]     PartHandle       Partition handle.
  @param[out]    FsHandle         FAT file system handle.

  @retval EFI_SUCCESS             The file system was initialized successfully.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_NOT_FOUND           FAT file system was not detected on this partition.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
FatInitFileSystem (
  IN  UINT32                                       SwPart,
  IN  EFI_HANDLE                                   PartHandle,
  OUT EFI_HANDLE                                  *FsHandle
  );

/**
  Clean-up allocated memory/etc. for FAT file system

  @param[in]     FsHandle         FAT file system handle.

  @retval                         none

**/
VOID
EFIAPI
FatCloseFileSystem (
  IN  EFI_HANDLE                                   FsHandle
  );

/**
  Open a file by its name and return its file handle.

  @param[in]     FsHandle         file system handle.
  @param[in]     FileName         The file name to get.
  @param[out]    FileHandle       file handle

  @retval EFI_SUCCESS             The file opened correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_DEVICE_ERROR        A device error occurred.
  @retval EFI_NOT_FOUND           A requested file cannot be found.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
FatFsOpenFile (
  IN  EFI_HANDLE                                    FsHandle,
  IN  CHAR16                                       *FileName,
  OUT EFI_HANDLE                                   *FileHandle
  );

/**
  Get file size by opened file handle.

  @param[in]     FileHandle       file handle
  @param[out]    FileSize         Pointer to file buffer size.

  @retval EFI_SUCCESS             The file was loaded correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.

**/
EFI_STATUS
EFIAPI
FatFsGetFileSize (
  IN  EFI_HANDLE                                  FileHandle,
  OUT UINTN                                      *FileSize
  );

/**
  Read file into memory by opened file handle.

  @param[in]     FileHandle       file handle
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
FatFsReadFile (
  IN  EFI_HANDLE                                  FsHandle,
  IN  EFI_HANDLE                                  FileHandle,
  OUT VOID                                      **FileBufferPtr,
  OUT UINTN                                      *FileSize
  );

/**
  Close a file by opened file handle

  @param[in]     FileHandle       file handle

  @retval                         none

**/
VOID
EFIAPI
FatFsCloseFile (
  IN  EFI_HANDLE                                  FileHandle
  );

/**
  List directories or files

  @param[in]     FsHandle         file system handle.
  @param[in]     DirFilePath      directory or file path

  @retval EFI_SUCCESS             list directories of files successfully
  @retval EFI_UNSUPPORTED         this api is not supported
  @retval Others                  an error occurs

**/
EFI_STATUS
EFIAPI
FatFsListDir (
  IN  EFI_HANDLE                                  FsHandle,
  IN  CHAR16                                     *DirFilePath
  );

#endif // _FAT_LIB_H_
