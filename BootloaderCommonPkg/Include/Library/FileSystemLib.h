/** @file
  File system level API library interface prototypes

Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FILE_SYSTEM_LIB_H_
#define _FILE_SYSTEM_LIB_H_

#include <Library/PartitionLib.h>
#include <Guid/OsBootOptionGuid.h>

/**
  Initialize file systems.

  @param[in]     SwPart           Software partition index.
  @param[in]     PartHandle       Partition handle.
  @param[out]    FsHandle         file system handle.

  @retval EFI_SUCCESS             The file system was initialized successfully.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_NOT_FOUND           file system was not detected on this partition.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
typedef
EFI_STATUS
(EFIAPI *FS_INIT_FILE_SYSTEM) (
  IN  UINT32                                       SwPart,
  IN  EFI_HANDLE                                   PartHandle,
  OUT EFI_HANDLE                                  *FsHandle
  );

/**
  Clean-up allocated memory/etc. for file system

  @param[in]     FsHandle         File system handle to clean-up.

  @retval                         none

**/
typedef
VOID
(EFIAPI *FS_CLOSE_FILE_SYSTEM) (
  IN  EFI_HANDLE                                  FsHandle
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
typedef
EFI_STATUS
(EFIAPI *FS_OPEN_FILE) (
  IN  EFI_HANDLE                                  FsHandle,
  IN  CHAR16                                     *FileName,
  OUT EFI_HANDLE                                 *FileHandle
  );

/**
  Get file size by opened file handle.

  @param[in]     FileHandle       file handle
  @param[out]    FileSize         Pointer to file buffer size.

  @retval EFI_SUCCESS             The file was loaded correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.

**/
typedef
EFI_STATUS
(EFIAPI *FS_GET_FILE_SIZE) (
  IN  EFI_HANDLE                                  FileHandle,
  OUT UINTN                                      *FileSize
  );

/**
  Read file into memory by opened file handle.

  @param[in]     FsHandle         file system handle.
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
typedef
EFI_STATUS
(EFIAPI *FS_READ_FILE) (
  IN  EFI_HANDLE                                  FsHandle,
  IN  EFI_HANDLE                                  FileHandle,
  OUT VOID                                      **FileBuffer,
  OUT UINTN                                      *FileSize
  );

/**
  Close a file by opened file handle

  @param[in]     FsHandle         file system handle.
  @param[in]     FileHandle       file handle

  @retval                         none

**/
typedef
VOID
(EFIAPI *FS_CLOSE_FILE) (
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
typedef
EFI_STATUS
(EFIAPI *FS_LIST_DIR) (
  IN  EFI_HANDLE                                  FsHandle,
  IN  CHAR16                                     *DirFilePath
  );

/**
  Get SW partition no. of detected file system

  @param[in]     FsHandle               file system handle.
  @param[out]    SwPartNo               sw part no.

  @retval        EFI_SUCCESS            found sw part no.
  @retval        EFI_INVALID_PARAMETER  Invalid FsHandle

**/
EFI_STATUS
EFIAPI
GetFileSystemCurrentPartNo (
  IN  EFI_HANDLE                                    FsHandle,
  OUT UINT32                                       *SwPartNo
  );

/**
  Get detected file system type

  @param[in]     FsHandle             file system handle.

  @retval        OS_FILE_SYSTEM_TYPE  file system type enum

**/
OS_FILE_SYSTEM_TYPE
EFIAPI
GetFileSystemType (
  IN  EFI_HANDLE                                    FsHandle
  );

/**
  Initialize file systems.

  @param[in]     SwPart           Software partition index.
  @param[in]     FsType           Filesystem type.
  @param[in]     PartHandle       Partition handle.
  @param[out]    FsHandle         file system handle.

  @retval EFI_SUCCESS             The file system was initialized successfully.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.
  @retval EFI_NOT_FOUND           file system was not detected on this partition.
  @retval EFI_OUT_OF_RESOURCES    Insufficant memory resource pool.

**/
EFI_STATUS
EFIAPI
InitFileSystem (
  IN  UINT32                                       SwPart,
  IN  UINT32                                       FsType,
  IN  EFI_HANDLE                                   PartHandle,
  OUT EFI_HANDLE                                  *FsHandle
  );

/**
  Clean-up allocated memory/etc. for file system

  @param[in]     FsHandle         File system handle to clean-up.

  @retval                         none

**/
VOID
EFIAPI
CloseFileSystem (
  IN  EFI_HANDLE                                    FsHandle
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
OpenFile (
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
GetFileSize (
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
ReadFile (
  IN  EFI_HANDLE                                  FileHandle,
  OUT VOID                                      **FileBuffer,
  OUT UINTN                                      *FileSize
  );

/**
  Close a file by opened file handle

  @param[in]     FileHandle       file handle

  @retval                         none

**/
VOID
EFIAPI
CloseFile (
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
ListDir (
  IN  EFI_HANDLE                                  FsHandle,
  IN  CHAR16                                     *DirFilePath
  );

typedef struct {
  FS_INIT_FILE_SYSTEM                 InitFileSystem;
  FS_CLOSE_FILE_SYSTEM                CloseFileSystem;
  FS_OPEN_FILE                        OpenFile;
  FS_GET_FILE_SIZE                    GetFileSize;
  FS_READ_FILE                        ReadFile;
  FS_CLOSE_FILE                       CloseFile;
  FS_LIST_DIR                         ListDir;
} FILE_SYSTEM_FUNC;

#endif // _FAT_PEIM_H_
