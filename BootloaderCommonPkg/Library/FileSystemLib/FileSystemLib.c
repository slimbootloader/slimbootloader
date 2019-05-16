/** @file
  File system level API library interface

Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>

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
#include <Library/MemoryAllocationLib.h>
#include <Library/FatLib.h>
#include <Library/Ext23Lib.h>

#define FILE_SYSTEM_CB_SIGNATURE  SIGNATURE_32( 'F', 'S', 'C', 'B' )
#define FILE_CB_SIGNATURE         SIGNATURE_32( 'F', 'I', 'C', 'B' )

typedef struct {
  UINTN                 Signature;
  EFI_HANDLE            FsHandle;
  LIST_ENTRY            OpenFiles;
} FILE_SYSTEM_CONTROL_BLOCK;

typedef struct {
  LIST_ENTRY            Link;
  UINTN                 Signature;
  EFI_HANDLE            FileHandle;
  EFI_HANDLE            FileSystemControlBlock;
} FILE_CONTROL_BLOCK;

STATIC  BOOLEAN                 mFileSystemRegistered = FALSE;
STATIC  FILE_SYSTEM_FUNC        mFileSystemFuncs[EnumFileSystemTypeAuto];

STATIC
VOID
RegisterFileSystems (
  VOID
)
{
  OS_FILE_SYSTEM_TYPE FsType;

  if (mFileSystemRegistered == FALSE) {
    ZeroMem (&mFileSystemFuncs, sizeof (mFileSystemFuncs));

    FsType = EnumFileSystemTypeFat;
    if (FixedPcdGet32 (PcdSupportedFileSystemMask) & (1 << FsType)) {
      mFileSystemFuncs[FsType].InitFileSystem   = FatInitFileSystem;
      mFileSystemFuncs[FsType].CloseFileSystem  = FatCloseFileSystem;
      mFileSystemFuncs[FsType].OpenFile         = FatFsOpenFile;
      mFileSystemFuncs[FsType].GetFileSize      = FatFsGetFileSize;
      mFileSystemFuncs[FsType].ReadFile         = FatFsReadFile;
      mFileSystemFuncs[FsType].CloseFile        = FatFsCloseFile;
    }

    FsType = EnumFileSystemTypeExt2;
    if (FixedPcdGet32 (PcdSupportedFileSystemMask) & (1 << FsType)) {
      mFileSystemFuncs[FsType].InitFileSystem   = ExtInitFileSystem;
      mFileSystemFuncs[FsType].CloseFileSystem  = ExtCloseFileSystem;
      mFileSystemFuncs[FsType].OpenFile         = ExtFsOpenFile;
      mFileSystemFuncs[FsType].GetFileSize      = ExtFsGetFileSize;
      mFileSystemFuncs[FsType].ReadFile         = ExtFsReadFile;
      mFileSystemFuncs[FsType].CloseFile        = ExtFsCloseFile;
    }
    mFileSystemRegistered = TRUE;
  }
}

STATIC
OS_FILE_SYSTEM_TYPE
EFIAPI
GetFileSystemType (
  IN  EFI_HANDLE                                    FsHandle
  )
{
  UINTN   Signature;
  OS_FILE_SYSTEM_TYPE FsType;

  FsType = EnumFileSystemMax;
  if (FsHandle != NULL) {
    Signature = *(UINTN *)FsHandle;
    if (Signature == FS_FAT_SIGNATURE) {
      FsType = EnumFileSystemTypeFat;
    } else if (Signature == FS_EXT_SIGNATURE) {
      FsType = EnumFileSystemTypeExt2;
    }
  }
  return FsType;
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
  EFI_STATUS                  Status;
  UINT32                      Type;
  EFI_HANDLE                  Handle;
  FILE_SYSTEM_CONTROL_BLOCK  *FileSystemControlBlock;

  Status = EFI_INVALID_PARAMETER;

  if (FsType >= EnumFileSystemMax) {
    return Status;
  }

  RegisterFileSystems ();

  for (Type = EnumFileSystemTypeFat; Type < EnumFileSystemTypeAuto; Type++) {
    if (mFileSystemFuncs[Type].InitFileSystem == NULL) {
      continue;
    }
    if ((FsType == EnumFileSystemTypeAuto) || (FsType == Type)) {
      Handle = NULL;
      Status = mFileSystemFuncs[Type].InitFileSystem (SwPart, PartHandle, &Handle);
      if (!EFI_ERROR (Status)) {
        break;
      }

      if (Handle != NULL) {
        mFileSystemFuncs[Type].CloseFileSystem (Handle);
      }
    }
  }

  if (!EFI_ERROR (Status)) {
    FileSystemControlBlock = (FILE_SYSTEM_CONTROL_BLOCK *) AllocatePool (sizeof (FILE_SYSTEM_CONTROL_BLOCK));
    FileSystemControlBlock->Signature = FILE_SYSTEM_CB_SIGNATURE;
    FileSystemControlBlock->FsHandle  = Handle;
    InitializeListHead (&FileSystemControlBlock->OpenFiles);

    *FsHandle = (EFI_HANDLE)FileSystemControlBlock;
  }

  return Status;
}

/**
  Clean-up allocated memory/etc. for file system

  @param[in]     FsHandle         File system handle to clean-up.

  @retval                         none

**/
VOID
EFIAPI
CloseFileSystem (
  IN  EFI_HANDLE                                   FsHandle
  )
{
  OS_FILE_SYSTEM_TYPE         FsType;
  FILE_SYSTEM_CONTROL_BLOCK  *FileSystemControlBlock;
  FILE_CONTROL_BLOCK         *FileControlBlock;
  LIST_ENTRY                 *Node;

  FileSystemControlBlock = (FILE_SYSTEM_CONTROL_BLOCK *)FsHandle;
  if (FileSystemControlBlock == NULL) {
    return;
  }

  FsType = GetFileSystemType (FileSystemControlBlock->FsHandle);
  if (FsType >= EnumFileSystemTypeAuto) {
    return;
  }

  Node = GetFirstNode (&FileSystemControlBlock->OpenFiles);
  while (!IsNull (&FileSystemControlBlock->OpenFiles, Node)) {
    FileControlBlock = (FILE_CONTROL_BLOCK *)Node;
    if (FileControlBlock->Signature == FILE_CB_SIGNATURE) {
      DEBUG ((DEBUG_INFO, "  Close file handle 0x%p\n", FileControlBlock->FileHandle));
      CloseFile (FileControlBlock);
    }
    Node = GetNextNode (&FileSystemControlBlock->OpenFiles, Node);
  }

  if (mFileSystemFuncs[FsType].CloseFileSystem == NULL) {
    return;
  }

  mFileSystemFuncs[FsType].CloseFileSystem (FileSystemControlBlock->FsHandle);
  FreePool (FileSystemControlBlock);
  DEBUG ((DEBUG_INFO, "CloseFileSystem: FsType %d\n", FsType));
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
OpenFile (
  IN  EFI_HANDLE                                    FsHandle,
  IN  CHAR16                                       *FileName,
  OUT EFI_HANDLE                                   *FileHandle
  )
{
  EFI_STATUS                  Status;
  OS_FILE_SYSTEM_TYPE         FsType;
  EFI_HANDLE                  FsFileHandle;
  FILE_SYSTEM_CONTROL_BLOCK  *FileSystemControlBlock;
  FILE_CONTROL_BLOCK         *FileControlBlock;

  FileSystemControlBlock = (FILE_SYSTEM_CONTROL_BLOCK *)FsHandle;
  ASSERT (FileSystemControlBlock != NULL);
  if (FileSystemControlBlock == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FsType = GetFileSystemType (FileSystemControlBlock->FsHandle);
  if (FsType >= EnumFileSystemTypeAuto) {
    return EFI_NOT_READY;
  }

  if (mFileSystemFuncs[FsType].OpenFile == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = mFileSystemFuncs[FsType].OpenFile (FileSystemControlBlock->FsHandle, FileName, &FsFileHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FileControlBlock = (FILE_CONTROL_BLOCK *) AllocatePool (sizeof (FILE_CONTROL_BLOCK));
  if (FileControlBlock == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  FileControlBlock->Signature   = FILE_CB_SIGNATURE;
  FileControlBlock->FileHandle  = FsFileHandle;
  FileControlBlock->FileSystemControlBlock  = FileSystemControlBlock;
  InsertTailList (&FileSystemControlBlock->OpenFiles, &FileControlBlock->Link);

  *FileHandle = (EFI_HANDLE) FileControlBlock;

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
GetFileSize (
  IN  EFI_HANDLE                                  FileHandle,
  OUT UINTN                                      *FileSize
  )
{
  OS_FILE_SYSTEM_TYPE         FsType;
  FILE_SYSTEM_CONTROL_BLOCK  *FileSystemControlBlock;
  FILE_CONTROL_BLOCK         *FileControlBlock;

  FileControlBlock = (FILE_CONTROL_BLOCK *)FileHandle;
  ASSERT (FileControlBlock != NULL);
  if (FileControlBlock == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ASSERT (FileControlBlock->Signature == FILE_CB_SIGNATURE);

  FileSystemControlBlock = (FILE_SYSTEM_CONTROL_BLOCK *)FileControlBlock->FileSystemControlBlock;
  ASSERT (FileSystemControlBlock->Signature == FILE_SYSTEM_CB_SIGNATURE);

  FsType = GetFileSystemType (FileSystemControlBlock->FsHandle);
  if (FsType >= EnumFileSystemTypeAuto) {
    return EFI_NOT_READY;
  }

  if (mFileSystemFuncs[FsType].GetFileSize == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mFileSystemFuncs[FsType].GetFileSize (FileControlBlock->FileHandle, FileSize);
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
ReadFile (
  IN  EFI_HANDLE                                  FileHandle,
  OUT VOID                                      **FileBuffer,
  OUT UINTN                                      *FileSize
  )
{
  OS_FILE_SYSTEM_TYPE         FsType;
  FILE_SYSTEM_CONTROL_BLOCK  *FileSystemControlBlock;
  FILE_CONTROL_BLOCK         *FileControlBlock;

  ASSERT (FileHandle != NULL);
  if (FileHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ASSERT (*FileBuffer != NULL);
  if (*FileBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FileControlBlock = (FILE_CONTROL_BLOCK *)FileHandle;
  ASSERT (FileControlBlock->Signature == FILE_CB_SIGNATURE);

  FileSystemControlBlock = (FILE_SYSTEM_CONTROL_BLOCK *)FileControlBlock->FileSystemControlBlock;
  ASSERT (FileSystemControlBlock->Signature == FILE_SYSTEM_CB_SIGNATURE);

  FsType = GetFileSystemType (FileSystemControlBlock->FsHandle);
  if (FsType >= EnumFileSystemTypeAuto) {
    return EFI_NOT_READY;
  }

  if (mFileSystemFuncs[FsType].ReadFile == NULL) {
    return EFI_UNSUPPORTED;
  }

  return mFileSystemFuncs[FsType].ReadFile (FileSystemControlBlock->FsHandle, FileControlBlock->FileHandle, FileBuffer, FileSize);
}

/**
  Close a file by opened file handle

  @param[in]     FileHandle       file handle

  @retval                         none

**/
VOID
EFIAPI
CloseFile (
  IN  EFI_HANDLE                                  FileHandle
  )
{
  OS_FILE_SYSTEM_TYPE         FsType;
  FILE_SYSTEM_CONTROL_BLOCK  *FileSystemControlBlock;
  FILE_CONTROL_BLOCK         *FileControlBlock;

  FileControlBlock = (FILE_CONTROL_BLOCK *)FileHandle;
  ASSERT (FileControlBlock != NULL);
  ASSERT (FileControlBlock->Signature == FILE_CB_SIGNATURE);
  if (FileControlBlock == NULL || FileControlBlock->Signature != FILE_CB_SIGNATURE) {
    return;
  }

  FileSystemControlBlock = (FILE_SYSTEM_CONTROL_BLOCK *)FileControlBlock->FileSystemControlBlock;
  ASSERT (FileSystemControlBlock != NULL);
  ASSERT (FileSystemControlBlock->Signature == FILE_SYSTEM_CB_SIGNATURE);
  if (FileSystemControlBlock == NULL) {
    return;
  }

  FsType = GetFileSystemType (FileSystemControlBlock->FsHandle);
  if (FsType >= EnumFileSystemTypeAuto) {
    return;
  }

  if (mFileSystemFuncs[FsType].CloseFile == NULL) {
    return;
  }

  RemoveEntryList (&FileControlBlock->Link);
  mFileSystemFuncs[FsType].CloseFile (FileControlBlock->FileHandle);
  FreePool (FileControlBlock);
}
