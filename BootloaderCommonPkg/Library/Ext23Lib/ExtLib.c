/** @file
  ExtLib APIs

Copyright (c) 2006 - 2021, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/Ext23Lib.h>
#include "Ext2Fs.h"

/**
Initialize the EXT2/3 FileSystem API structure.

@param  SwPart                 The partition number of EXT2/3 FileSystem located
@param  PartHandle             The Partition handle
@param  FsHandle               The EXT2/3 FILE system handle

@retval EFI_INVALID_PARAMETER  The Partition handle is not for EXT2/3, or
                               partition number exceeds the maxium number in Partition handle
@retval EFI_OUT_OF_RESOURCES   Can't allocate memory resource

**/
EFI_STATUS
EFIAPI
ExtInitFileSystem (
  IN  UINT32        SwPart,
  IN  EFI_HANDLE    PartHandle,
  OUT EFI_HANDLE    *FsHandle
  )
{
  PEI_EXT_PRIVATE_DATA      *PrivateData;
  PART_BLOCK_DEVICE         *PartBlockDev;
  EFI_STATUS                Status;

  // Valid parameters
  PartBlockDev = (PART_BLOCK_DEVICE *)PartHandle;
  if ((FsHandle == NULL) || (PartBlockDev == NULL) || \
      (PartBlockDev->Signature != PART_INFO_SIGNATURE)) {
    return EFI_INVALID_PARAMETER;
  }

  if (SwPart >= PartBlockDev->BlockDeviceCount) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = (PEI_EXT_PRIVATE_DATA *)AllocateZeroPool (sizeof (PEI_EXT_PRIVATE_DATA));
  if (PrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData->Signature     = FS_EXT_SIGNATURE;
  PrivateData->PhysicalDevNo = (UINT8)PartBlockDev->HarewareDevice;
  PrivateData->StartBlock    = PartBlockDev->BlockDevice[SwPart].StartBlock;
  PrivateData->LastBlock     = PartBlockDev->BlockDevice[SwPart].LastBlock;
  PrivateData->BlockSize     = PartBlockDev->BlockInfo.BlockSize;

  // validate Ext2 superblock
  Status = Ext2SbValidate ((EFI_HANDLE)PrivateData, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_VERBOSE, "No valid EXT superblock on StartBlock %d Part %d\n", PrivateData->StartBlock, SwPart));
    goto Error;
  }

  DEBUG ((DEBUG_INFO, "Detected EXT on StartBlock %d Part %d\n", PrivateData->StartBlock, SwPart));

  *FsHandle = (EFI_HANDLE)PrivateData;

  return EFI_SUCCESS;

Error:
  if (PrivateData != NULL) {
    FreePool (PrivateData);
  }
  return Status;
}

/**
  Clean-up allocated memory/etc. for EXT file system

  @param[in]     FsHandle         EXT file system handle.

  @retval                         none

**/
VOID
EFIAPI
ExtCloseFileSystem (
  IN  EFI_HANDLE                                   FsHandle
  )
{
  PEI_EXT_PRIVATE_DATA      *PrivateData;

  PrivateData = NULL;
  if (FsHandle != NULL) {
    PrivateData = (PEI_EXT_PRIVATE_DATA *)FsHandle;
    ASSERT (PrivateData->Signature == FS_EXT_SIGNATURE);
  }

  if (PrivateData != NULL && PrivateData->Signature == FS_EXT_SIGNATURE) {
    FreePool (PrivateData);
  }
}

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
ExtFsOpenFile (
  IN  EFI_HANDLE                                    FsHandle,
  IN  CHAR16                                       *FileName,
  OUT EFI_HANDLE                                   *FileHandle
  )
{
  EFI_STATUS              Status;
  PEI_EXT_PRIVATE_DATA   *PrivateData;
  OPEN_FILE              *OpenFile;
  CHAR8                  *NameBuffer;
  UINTN                   NameSize;

  PrivateData = (PEI_EXT_PRIVATE_DATA *)FsHandle;
  if (PrivateData == NULL || PrivateData->Signature != FS_EXT_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  NameSize = StrSize (FileName);
  NameBuffer = AllocatePool (NameSize);
  Status = UnicodeStrToAsciiStrS (FileName, NameBuffer, NameSize);
  if (EFI_ERROR(Status)) {
    FreePool (NameBuffer);
    return Status;
  }

  OpenFile = (OPEN_FILE *)AllocateZeroPool (sizeof (OPEN_FILE));
  if (OpenFile == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  OpenFile->FileDevData = PrivateData;
  Status = Ext2fsOpen (NameBuffer, OpenFile);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  FreePool (NameBuffer);
  *FileHandle = (EFI_HANDLE)OpenFile;

  return EFI_SUCCESS;

Error:
  if (NameBuffer != NULL) {
    FreePool (NameBuffer);
  }
  if (OpenFile != NULL) {
    FreePool (OpenFile);
  }
  return Status;
}

/**
  Get file size by opened file handle.

  @param[in]     FileHandle       file handle
  @param[out]    FileSize         Pointer to file buffer size.

  @retval EFI_SUCCESS             The file was loaded correctly.
  @retval EFI_INVALID_PARAMETER   Parameter is not valid.

**/
EFI_STATUS
EFIAPI
ExtFsGetFileSize (
  IN  EFI_HANDLE                                  FileHandle,
  OUT UINTN                                      *FileSize
  )
{
  OPEN_FILE              *OpenFile;

  OpenFile = (OPEN_FILE *)FileHandle;
  ASSERT (OpenFile != NULL);
  if (OpenFile == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *FileSize = Ext2fsFileSize (OpenFile);
  return EFI_SUCCESS;
}

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
ExtFsReadFile (
  IN  EFI_HANDLE                                  FsHandle,
  IN  EFI_HANDLE                                  FileHandle,
  OUT VOID                                      **FileBufferPtr,
  OUT UINTN                                      *FileSizePtr
  )
{
  OPEN_FILE              *OpenFile;
  VOID                   *FileBuffer;
  UINT32                  FileSize;
  UINT32                  Residual;
  EFI_STATUS              Status;

  OpenFile = (OPEN_FILE *)FileHandle;
  ASSERT (OpenFile != NULL);
  if (OpenFile == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FileSize = Ext2fsFileSize (OpenFile);
  if (FileSize == 0) {
    return EFI_SUCCESS;
  }

  ASSERT (FileBufferPtr != NULL);
  if (FileBufferPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FileBuffer = *FileBufferPtr;
  Residual = 0;
  Status = Ext2fsRead (OpenFile, FileBuffer, FileSize, &Residual);
  if (EFI_ERROR (Status) || (Residual != 0)) {
    return EFI_LOAD_ERROR;
  } else {
    *FileSizePtr = FileSize;
  }

  return EFI_SUCCESS;
}

/**
  Close a file by opened file handle

  @param[in]     FileHandle       file handle

  @retval                         none

**/
VOID
EFIAPI
ExtFsCloseFile (
  IN  EFI_HANDLE                                  FileHandle
  )
{
  OPEN_FILE              *OpenFile;

  OpenFile = (OPEN_FILE *)FileHandle;
  if (OpenFile == NULL) {
    return;
  }
  Ext2fsClose (OpenFile);
  FreePool (OpenFile);
}

/**
  List directories or files

  @param[in]     FsHandle         file system handle.
  @param[in]     DirFilePath      directory or file path
  @param[in]     ConsoleOutFunc   redirect output message to a console

  @retval EFI_SUCCESS             list directories of files successfully
  @retval EFI_UNSUPPORTED         this api is not supported
  @retval Others                  an error occurs

**/
EFI_STATUS
EFIAPI
ExtFsListDir (
  IN  EFI_HANDLE                                  FsHandle,
  IN  CHAR16                                     *DirFilePath
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              FileHandle;

  Status = EFI_UNSUPPORTED;

DEBUG_CODE_BEGIN ();
  FileHandle = NULL;
  Status = ExtFsOpenFile (FsHandle, DirFilePath, &FileHandle);
  if (!EFI_ERROR (Status)) {
    Status = Ext2fsLs ((OPEN_FILE *)FileHandle, NULL);
  }
  if (FileHandle != NULL) {
    ExtFsCloseFile (FileHandle);
  }
DEBUG_CODE_END ();

  return Status;
}
