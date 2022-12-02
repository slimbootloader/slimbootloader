/** @file

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright (c) 1997 Manuel Bouyer.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 -
  Copyright (c) 1993
    The Regents of the University of California.  All rights reserved.

  This code is derived from software contributed to Berkeley by
  The Mach Operating System project at Carnegie-Mellon University.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the Name of the University nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.


  Copyright (c) 1990, 1991 Carnegie Mellon University
  All Rights Reserved.

  Author: David Golub

  Permission to use, copy, modify and distribute this software and its
  documentation is hereby granted, provided that both the copyright
  notice and this permission notice appear in all copies of the
  software, derivative works or modified versions, and any portions
  thereof, and that both notices appear in supporting documentation.

  CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
  CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
  ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.

  Carnegie Mellon requests users of this software to return to

   Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
   School of Computer Science
   Carnegie Mellon University
   Pittsburgh PA 15213-3890

  any improvements or extensions that they make and grant Carnegie the
  rights to redistribute these changes.

    Stand-alone FILE reading package for Ext2 FILE system.
**/

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/MediaAccessLib.h>
#include "Ext2Fs.h"
#include "LibsaFsStand.h"

#define HOWMANY(x, y)  (((x)+((y)-1))/(y))

#if defined(LIBSA_FS_SINGLECOMPONENT) && !defined(LIBSA_NO_FS_SYMLINK)
#define LIBSA_NO_FS_SYMLINK
#endif

/**
  Given an offset in a FILE, find the disk block number that
  contains that block.

  @param[in]  File            pointer to an Open file.
  @param[in]  FileBlock       Block to find the file.
  @param[out] DiskBlockPtr    Pointer to the disk which contains block.

  @retval 0 if success
  @retval other if error.
**/
STATIC
RETURN_STATUS
BlockMap (
  IN  OPEN_FILE     *File,
  IN  INDPTR         FileBlock,
  OUT INDPTR        *DiskBlockPtr
  );

/**
  Search a directory for a Name and return its inode number.

  @param[in]      Name        Name to compare with
  @param[in]      Length      Length of the dir name
  @param[in/out]  File        Pointer to file private data
  @param[out]     INumPtr     pointer to Inode number.

  @retval 0 if success
  @retval other if error.
**/
STATIC
RETURN_STATUS
SearchDirectory (
  IN      CHAR8         *Name,
  IN      INT32          Length,
  IN OUT  OPEN_FILE     *File,
  OUT     INODE32       *INumPtr
  );

/**
  Gives the info of device block config.

  @param[in]    DevData     Device privete data.
  @param[in]    ReadWrite   Read or Write
  @param[in]    BlockNum    Block number to start
  @param[in]    Size        Size to read block.
  @param[out]   Buf         Buffer to read the block data.
  @param[out]   RSize       Actual read size

  @retval 0 if success
  @retval other if error.
**/
RETURN_STATUS
EFIAPI
BDevStrategy (
  IN  VOID       *DevData,
  IN  INT32       ReadWrite,
  IN  DADDRESS    BlockNum,
  IN  UINT32      Size,
  OUT VOID       *Buf,
  OUT UINT32     *RSize
  )
{
  RETURN_STATUS           Status;
  PEI_EXT_PRIVATE_DATA    *PrivateData;
  UINT64                  Startblockno;

  PrivateData = DevData;

  if (ReadWrite != F_READ && ReadWrite != F_WRITE) {
    return EFI_INVALID_PARAMETER;
  }
  if ((Size % PrivateData->BlockSize) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  Startblockno = BlockNum + PrivateData->StartBlock;
  if (ReadWrite == F_READ) {
    Status = MediaReadBlocks (PrivateData->PhysicalDevNo, (UINT32)Startblockno, Size, Buf);
    if (RETURN_ERROR (Status)) {
      return Status;
    }
  }

  *RSize = Size;
  return RETURN_SUCCESS;
}

/**
  Read a new inode into a FILE structure.

  @param[in]      INumber     inode number
  @param[in/out]  File        pointer to open file struct.

  @retval         0 if success
  @retval         other if error.
**/
RETURN_STATUS
EFIAPI
ReadInode (
  IN    INODE32      INumber,
  IN    OPEN_FILE   *File
  )
{
  FILE         *Fp;
  M_EXT2FS     *FileSystem;
  CHAR8        *Buf;
  UINT32        RSize;
  RETURN_STATUS Status;
  DADDRESS      InodeSector;
  EXT2GD       *Ext2FsGrpDes;
  EXTFS_DINODE *DInodePtr;

  Fp = (FILE *)File->FileSystemSpecificData;
  FileSystem = Fp->SuperBlockPtr;

  Ext2FsGrpDes = &FileSystem->Ext2FsGrpDes[INOTOCG(FileSystem, INumber)];

  InodeSector = (DADDRESS) (Ext2FsGrpDes->Ext2BGDInodeTables + DivU64x32 (ModU64x32 ((INumber - 1), FileSystem->Ext2Fs.Ext2FsINodesPerGroup), FileSystem->Ext2FsInodesPerBlock));

  if (FileSystem->Ext2FsGDSize > 32) {
    if (Ext2FsGrpDes->Ext2BGDInodeTablesHi !=0) {
      InodeSector |= LShiftU64 ((UINT64) (Ext2FsGrpDes->Ext2BGDInodeTablesHi), 32);
    }
  }
  InodeSector = FSBTODB (FileSystem, InodeSector);

  //
  // Read inode and save it.
  //
  Buf = Fp->Buffer;
  Status = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                    InodeSector, FileSystem->Ext2FsBlockSize, Buf, &RSize);
  if (RETURN_ERROR (Status)) {
    return Status;
  }
  if (RSize != (UINT32)FileSystem->Ext2FsBlockSize) {
    return EFI_DEVICE_ERROR;
  }

  DInodePtr = (EXTFS_DINODE *) (Buf +
                                EXT2_DINODE_SIZE (FileSystem) * INODETOFSBO (FileSystem, INumber));
  E2FSILOAD (DInodePtr, &Fp->DiskInode);

  //
  // Clear out the Old buffers
  //
  Fp->InodeCacheBlock = ~0;
  Fp->BufferBlockNum = -1;
  return Status;
}

/**
  Given an offset in a FILE, find the disk block number that
  contains that block.

  @param[in]  File            pointer to an Open file.
  @param[in]  FileBlock       Block to find the file.
  @param[out] DiskBlockPtr    Pointer to the disk which contains block.

  @retval 0 if success
  @retval other if error.
**/
STATIC
RETURN_STATUS
BlockMap (
  IN  OPEN_FILE     *File,
  IN  INDPTR         FileBlock,
  OUT INDPTR        *DiskBlockPtr
  )
{
  FILE     *Fp;
  M_EXT2FS *FileSystem;
  UINT32    Level;
  INDPTR    IndCache;
  INDPTR    IndBlockNum;
  UINT32    RSize;
  INDPTR   *Buf;
  UINT32    Index;
  UINT64    NextLevelNode;
  EXT4_EXTENT_TABLE *Etable;
  EXT4_EXTENT_INDEX *ExtIndex;
  EXT4_EXTENT       *Extent;
  RETURN_STATUS     Status;

  Fp = (FILE *)File->FileSystemSpecificData;
  FileSystem = Fp->SuperBlockPtr;
  Buf = (VOID *)Fp->Buffer;

  if ((Fp->DiskInode.Ext2DInodeStatusFlags & EXT4_EXTENTS) != 0) {
    Etable = (EXT4_EXTENT_TABLE*) &(Fp->DiskInode.Ext2DInodeBlocks);
    if (Etable->Eheader.EhMagic != EXT4_EXTENT_HEADER_MAGIC) {
        DEBUG ((DEBUG_ERROR, "EXT4 extent header magic mismatch 0x%X!\n", Etable->Eheader.EhMagic));
        return EFI_DEVICE_ERROR;
    }

    while (Etable->Eheader.EhDepth > 0) {
      ExtIndex = NULL;

      /* if only one entry exists, the first entry should be used */
      if (Etable->Eheader.EhEntries == 1) {
        ExtIndex = &(Etable->Enodes.Eindex[0]);
      }

      for (Index=1; Index < Etable->Eheader.EhEntries; Index++) {
        ExtIndex = &(Etable->Enodes.Eindex[Index]);
        if (((UINT32) FileBlock) < ExtIndex->EiBlk) {
          ExtIndex = &(Etable->Enodes.Eindex[Index-1]);
          break;
        }
        ExtIndex = NULL;
      }

      if (ExtIndex != NULL) {
        //
        // TODO: Need to support 48-bit block device addressing.
        // Throw an ASSERT if upper 16-bits are non-zero.
        //
        ASSERT (ExtIndex->EiLeafHi == 0);
        NextLevelNode = ExtIndex->EiLeafLo; //LShiftU64((UINT64)ExtIndex->EiLeafHi, 32) | ExtIndex->EiLeafLo;

        //
        // We need to read the next level node of the extent tree since the data was not in the current level.
        //
        Status = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                          FSBTODB (Fp->SuperBlockPtr, (DADDRESS) NextLevelNode), FileSystem->Ext2FsBlockSize,
                                          Buf, &RSize);
        if (RETURN_ERROR (Status)) {
          return Status;
        }
        if (RSize != (UINT32)FileSystem->Ext2FsBlockSize) {
          return EFI_DEVICE_ERROR;
        }

        Etable = (EXT4_EXTENT_TABLE*) Buf;
        if (Etable->Eheader.EhMagic != EXT4_EXTENT_HEADER_MAGIC) {
            DEBUG ((DEBUG_ERROR, "EXT4 extent header magic mismatch 0x%X!\n", Etable->Eheader.EhMagic));
            return EFI_DEVICE_ERROR;
        }
      } else {
        DEBUG ((DEBUG_ERROR, "Could not find FileBlock #%d in the index extent data!\n", FileBlock));
        return EFI_NO_MAPPING;
      }
    }

    Extent = NULL;
    for (Index=0; Index < Etable->Eheader.EhEntries; Index++) {
      Extent = &(Etable->Enodes.Extent[Index]);
      if ((((UINT32) FileBlock) >= Extent->Eblk) && (((UINT32) FileBlock) < (Extent->Eblk + Extent->Elen))) {
        break;
      }
      Extent = NULL;
    }

    if (Extent != NULL) {
      //
      // TODO: Need to support 48-bit block device addressing
      // Throw an ASSERT if upper 16-bits are non-zero.
      //
      ASSERT (Extent->EstartHi == 0);
      *DiskBlockPtr = Extent->EstartLo + (FileBlock - Extent->Eblk); // (LShiftU64((UINT64)Extent->EiLeafHi, 32) | Extent->EstartLo) + (FileBlock - Extent->Eblk);
    } else {
      *DiskBlockPtr = 0;
    }
  } else {
    if (FileBlock < NDADDR) {
      //
      // Direct block.
      //
      *DiskBlockPtr = Fp->DiskInode.Ext2DInodeBlocks[FileBlock];
      return 0;
    }

    FileBlock -= NDADDR;

    IndCache = FileBlock >> LN2_IND_CACHE_SZ;
    if (IndCache == Fp->InodeCacheBlock) {
      *DiskBlockPtr =
        Fp->InodeCache[FileBlock & IND_CACHE_MASK];
      return 0;
    }

    for (Level = 0;;) {
      Level += Fp->NiShift;
      if (FileBlock < (INDPTR)1 << Level) {
        break;
      }
      if (Level > NIADDR * Fp->NiShift)
        //
        // Block number too high
        //
      {
        return EFI_OUT_OF_RESOURCES;
      }
      FileBlock -= (INDPTR)1 << Level;
    }

    IndBlockNum =
      Fp->DiskInode.Ext2DInodeBlocks[NDADDR + (Level / Fp->NiShift - 1)];

    while (1) {
      Level -= Fp->NiShift;
      if (IndBlockNum == 0) {
        *DiskBlockPtr = 0;    // missing
        return 0;
      }

      //
      //  If we were feeling brave, we could work out the number
      //  of the disk sector and read a single disk sector instead
      //  of a filesystem block.
      //  However we don't do this very often anyway...
      //
      Status = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                        FSBTODB (Fp->SuperBlockPtr, IndBlockNum), FileSystem->Ext2FsBlockSize,
                                        Buf, &RSize);
      if (RETURN_ERROR (Status)) {
        return Status;
      }
      if (RSize != (UINT32)FileSystem->Ext2FsBlockSize) {
        return EFI_DEVICE_ERROR;
      }
      IndBlockNum = Buf[FileBlock >> Level];
      if (Level == 0) {
        break;
      }
      FileBlock &= (1 << Level) - 1;
    }
    //
    // Save the part of the block that contains this sector
    //
    CopyMem (Fp->InodeCache, &Buf[FileBlock & ~IND_CACHE_MASK],
             IND_CACHE_SZ * sizeof Fp->InodeCache[0]);
    Fp->InodeCacheBlock = IndCache;

    *DiskBlockPtr = IndBlockNum;
  }

  return RETURN_SUCCESS;
}

/**
  Read a portion of a FILE into an internal buffer.

  Return the location in the buffer and the amount in the buffer.

  @param[in]  File        Pointer to the open file.
  @param[out] BufferPtr   buffer corresponding to offset
  @param[out] SizePtr     Size of remainder of buffer.

  @retval     0 if success
  @retval     other if error.
**/
RETURN_STATUS
EFIAPI
BufReadFile (
  IN  OPEN_FILE     *File,
  OUT CHAR8        **BufferPtr,
  OUT UINT32        *SizePtr
  )
{
  FILE *Fp;
  M_EXT2FS *FileSystem;
  INT32 Off;
  INDPTR FileBlock;
  INDPTR DiskBlock;
  UINT32 BlockSize;
  RETURN_STATUS Rc;

  Fp = (FILE *)File->FileSystemSpecificData;
  FileSystem = Fp->SuperBlockPtr;
  DiskBlock  = 0;

  Off = BLOCKOFFSET (FileSystem, Fp->SeekPtr);
  FileBlock = LBLKNO (FileSystem, Fp->SeekPtr);
  BlockSize = FileSystem->Ext2FsBlockSize;    // no fragment

  if (FileBlock != Fp->BufferBlockNum) {
    Rc = BlockMap (File, FileBlock, &DiskBlock);
    if (Rc != 0) {
      return Rc;
    }

    if (DiskBlock == 0) {
      SetMem32 (Fp->Buffer, BlockSize, 0);
      Fp->BufferSize = BlockSize;
    } else {

      Rc = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                        FSBTODB (FileSystem, DiskBlock),
                                        BlockSize, Fp->Buffer, &Fp->BufferSize);
      if (Rc != 0) {
        return Rc;
      }
    }

    Fp->BufferBlockNum = FileBlock;
  }

  //
  //  Return address of byte in buffer corresponding to
  //  offset, and Size of remainder of buffer after that
  //  byte.
  //
  *BufferPtr = Fp->Buffer + Off;
  *SizePtr = BlockSize - Off;

  //
  //  But truncate buffer at end of FILE.
  //
  //  XXX should handle LARGEFILE
  //
  if (*SizePtr > Fp->DiskInode.Ext2DInodeSize - Fp->SeekPtr) {
    *SizePtr = Fp->DiskInode.Ext2DInodeSize - Fp->SeekPtr;
  }

  return 0;
}

/**
  Search a directory for a Name and return its inode number.

  @param[in]      Name        Name to compare with
  @param[in]      Length      Length of the dir name
  @param[in/out]  File        Pointer to file private data
  @param[out]     INumPtr     pointer to Inode number.

  @retval 0 if success
  @retval other if error.
**/
STATIC
RETURN_STATUS
SearchDirectory (
  IN      CHAR8         *Name,
  IN      INT32          Length,
  IN OUT  OPEN_FILE     *File,
  OUT     INODE32       *INumPtr
  )
{
  FILE *Fp;
  EXT2FS_DIRECT *Dp;
  EXT2FS_DIRECT *EdPtr;
  CHAR8 *Buf;
  UINT32 BufSize;
  INT32 NameLen;
  RETURN_STATUS Status;

  Fp = (FILE *)File->FileSystemSpecificData;

  Fp->SeekPtr = 0;
  //
  // XXX should handle LARGEFILE
  //
  while (Fp->SeekPtr < (OFFSET)Fp->DiskInode.Ext2DInodeSize) {
    Status = BufReadFile (File, &Buf, &BufSize);
    if (RETURN_ERROR (Status)) {
      return Status;
    }

    Dp = (EXT2FS_DIRECT *)Buf;
    EdPtr = (EXT2FS_DIRECT *) (Buf + BufSize);
    for (; Dp < EdPtr;
         Dp = (VOID *) ((CHAR8 *)Dp + Dp->Ext2DirectRecLen)) {
      if (Dp->Ext2DirectRecLen <= 0) {
        break;
      }
      if (Dp->Ext2DirectInodeNumber == (INODE32)0) {
        continue;
      }
      NameLen = Dp->Ext2DirectNameLen;
      if (NameLen == Length &&
          !CompareMem (Name, Dp->Ext2DirectName, Length)) {
        //
        // found entry
        //
        *INumPtr = Dp->Ext2DirectInodeNumber;
        AsciiStrCpyS (File->FileNameBuf, EXT2FS_MAXNAMLEN, Name);
        return 0;
      }
    }
    Fp->SeekPtr += BufSize;
  }
  return EFI_NOT_FOUND;
}

/**
  Validate EXT2 Superblock

  @param[in]      FsHandle      EXT file system handle.
  @param[in]      File          File for which super block needs to be read.
  @param[out]     RExt2Fs       EXT2FS meta data to retreive.

  @retval 0 if superblock validation is success
  @retval other if error.
**/
RETURN_STATUS
EFIAPI
Ext2SbValidate (
  IN CONST EFI_HANDLE  FsHandle,
  IN CONST OPEN_FILE   *File     OPTIONAL,
  OUT      EXT2FS      *RExt2Fs  OPTIONAL
  )
{
  PEI_EXT_PRIVATE_DATA *PrivateData;
  UINT8 *Buffer;
  EXT2FS *Ext2Fs;
  UINT32 BufSize;
  RETURN_STATUS Rc;
  UINT32 SbOffset;

  Rc = 0;
  Buffer = NULL;

  if (FsHandle == NULL) {
    Rc = RETURN_INVALID_PARAMETER;
    goto Exit;
  }

  PrivateData = (PEI_EXT_PRIVATE_DATA *)FsHandle;

  Buffer = AllocatePool ((PrivateData->BlockSize > SBSIZE) ? PrivateData->BlockSize : SBSIZE);
  if (Buffer == NULL) {
    Rc = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  if (File == NULL) {
    Rc = BDevStrategy (PrivateData, F_READ,
                       SBOFF / PrivateData->BlockSize, PrivateData->BlockSize, Buffer, &BufSize);
  } else {
    Rc = DEV_STRATEGY (File->DevPtr) (PrivateData, F_READ,
                                      SBOFF / PrivateData->BlockSize, PrivateData->BlockSize,
                                      Buffer, &BufSize);
  }

  if (Rc != 0) {
    goto Exit;
  }

  SbOffset = (SBOFF < PrivateData->BlockSize) ? SBOFF : 0;
  Ext2Fs = (EXT2FS *)(&Buffer[SbOffset]);
  if (Ext2Fs->Ext2FsMagic != E2FS_MAGIC) {
    Rc = EFI_UNSUPPORTED;
    goto Exit;
  }

  if (Ext2Fs->Ext2FsRev > E2FS_REV1 ||
      (Ext2Fs->Ext2FsRev == E2FS_REV1 &&
       (Ext2Fs->Ext2FsFirstInode != EXT2_FIRSTINO ||
        (Ext2Fs->Ext2FsInodeSize != 128 && Ext2Fs->Ext2FsInodeSize != 256) ||
        Ext2Fs->Ext2FsFeaturesIncompat & ~EXT2F_INCOMPAT_SUPP))) {
    Rc = EFI_UNSUPPORTED;
    goto Exit;
  }

  if (Ext2Fs->Ext2FsRev == E2FS_REV0) {
    Ext2Fs->Ext2FsFirstInode = 11;
    Ext2Fs->Ext2FsInodeSize  = 128;
  }

  if (RExt2Fs != NULL) {
    E2FS_SBLOAD ((VOID *)Ext2Fs, RExt2Fs);
  }

Exit:
  if (Buffer != NULL) {
    FreePool (Buffer);
  }

  return Rc;
}

/**
  Read Superblock of the file.

  @param[in]      File          File for which super block needs to be read.
  @param[in/out]  FileSystem    Fs on which super block is computed.

  @retval 0 if superblock compute is success
  @retval other if error.
**/
RETURN_STATUS
EFIAPI
ReadSBlock (
  IN      OPEN_FILE     *File,
  IN OUT  M_EXT2FS      *FileSystem
  )
{
  PEI_EXT_PRIVATE_DATA *PrivateData;
  RETURN_STATUS Rc;

  Rc = 0;

  PrivateData = (PEI_EXT_PRIVATE_DATA*) File->FileDevData;

  Rc = Ext2SbValidate ((EFI_HANDLE)PrivateData, File, &FileSystem->Ext2Fs);
  if (RETURN_ERROR (Rc)) {
    goto Exit;
  }

  //
  // compute in-memory m_ext2fs values
  //
  FileSystem->Ext2FsNumCylinder       =
    HOWMANY (FileSystem->Ext2Fs.Ext2FsBlockCount - FileSystem->Ext2Fs.Ext2FsFirstDataBlock,
             FileSystem->Ext2Fs.Ext2FsBlocksPerGroup);

  FileSystem->Ext2FsFsbtobd           = (INT32)(FileSystem->Ext2Fs.Ext2FsLogBlockSize + 10) - (INT32)HighBitSet32 (PrivateData->BlockSize);
  FileSystem->Ext2FsBlockSize         = MINBSIZE << FileSystem->Ext2Fs.Ext2FsLogBlockSize;
  FileSystem->Ext2FsLogicalBlock      = LOG_MINBSIZE + FileSystem->Ext2Fs.Ext2FsLogBlockSize;
  FileSystem->Ext2FsQuadBlockOffset   = FileSystem->Ext2FsBlockSize - 1;
  FileSystem->Ext2FsBlockOffset       = (UINT32)~FileSystem->Ext2FsQuadBlockOffset;
  FileSystem->Ext2FsGDSize            = 32;
  if (FileSystem->Ext2Fs.Ext2FsFeaturesIncompat & EXT2F_INCOMPAT_64BIT) {
    FileSystem->Ext2FsGDSize          = FileSystem->Ext2Fs.Ext2FsGDSize;
  }
  FileSystem->Ext2FsNumGrpDesBlock    =
    HOWMANY (FileSystem->Ext2FsNumCylinder, FileSystem->Ext2FsBlockSize / FileSystem->Ext2FsGDSize);
  FileSystem->Ext2FsInodesPerBlock    = FileSystem->Ext2FsBlockSize / FileSystem->Ext2Fs.Ext2FsInodeSize;
  FileSystem->Ext2FsInodesTablePerGrp = FileSystem->Ext2Fs.Ext2FsINodesPerGroup / FileSystem->Ext2FsInodesPerBlock;

Exit:
  return Rc;
}

/**
  Read group descriptor of the file.

  @param[in/out]  File          File for which group descriptor needs to be read.
  @param[in]      FileSystem    Fs on which super block is computed.

  @retval 0 if Group descriptor read is success
  @retval other if error.
**/
RETURN_STATUS
EFIAPI
ReadGDBlock (
  IN OUT  OPEN_FILE     *File,
  IN      M_EXT2FS      *FileSystem
  )
{
  FILE *Fp;
  UINT32 RSize;
  UINT32 gdpb;
  INT32 Index;
  INT32 Cnt;
  INT32 i;
  CHAR8 *Ptr;
  RETURN_STATUS Status;

  Fp = (FILE *)File->FileSystemSpecificData;

  gdpb = FileSystem->Ext2FsBlockSize / FileSystem->Ext2FsGDSize;

  for (Index = 0; Index < FileSystem->Ext2FsNumGrpDesBlock; Index++) {
    Status = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                      FSBTODB (FileSystem, FileSystem->Ext2Fs.Ext2FsFirstDataBlock +
                                          1 /* superblock */ + Index),
                                      FileSystem->Ext2FsBlockSize, Fp->Buffer, &RSize);
    if (RETURN_ERROR (Status)) {
      return Status;
    }
    if (RSize != (UINT32)FileSystem->Ext2FsBlockSize) {
      return EFI_DEVICE_ERROR;
    }

    /* Ext2FsGDSize may not be sizeof Ext2FsGrpDes */
    if (FileSystem->Ext2FsGDSize == sizeof(EXT2GD)) {
      E2FS_CGLOAD ((EXT2GD *)Fp->Buffer,
                 &FileSystem->Ext2FsGrpDes[Index * gdpb],
                 (Index == (FileSystem->Ext2FsNumGrpDesBlock - 1)) ?
                 (FileSystem->Ext2FsNumCylinder - gdpb * Index) * FileSystem->Ext2FsGDSize :
                 FileSystem->Ext2FsBlockSize);
    } else {
      Cnt = (Index == (FileSystem->Ext2FsNumGrpDesBlock - 1)) ?
            FileSystem->Ext2FsNumCylinder - gdpb * Index :
            gdpb;

      for (i = 0; i < Cnt; i++) {
        Ptr = Fp->Buffer + (i * FileSystem->Ext2FsGDSize);
        E2FS_CGLOAD (Ptr,
                     &FileSystem->Ext2FsGrpDes[Index * gdpb + i],
                     FileSystem->Ext2FsGDSize);
      }
    }
  }

  return RETURN_SUCCESS;
}

/**
  Open struct file.

  @param[in]      Path          Path to locate the file
  @param[in/out]  File          The struct having the device and file info

  @retval RETURN_SUCCESS if file open is success
  @retval other if error.
**/
RETURN_STATUS
EFIAPI
Ext2fsOpen (
  IN      CHAR8         *Path,
  IN OUT  OPEN_FILE     *File
  )
{
#ifndef LIBSA_FS_SINGLECOMPONENT
  CHAR8 *Cp;
  CHAR8 *Ncp;
  INT32 Component;
#endif
  INODE32 INumber;
  FILE *Fp;
  M_EXT2FS *FileSystem;
  RETURN_STATUS Status;
#ifndef LIBSA_NO_FS_SYMLINK
  INODE32 ParentInumber;
  INT32 Nlinks;
  CHAR8 NameBuf[MAXPATHLEN+1];
  CHAR8 *Buf;

  Nlinks = 0;
  CHAR8 SymFileNameBuf[EXT2FS_MAXNAMLEN];
#endif

  INDPTR mult;
  INT32  Length2;

  //
  // allocate struct file system specific data structure
  //
  Fp = AllocatePool (sizeof (FILE));
  if (Fp == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto out;
  }

  SetMem32 (Fp, sizeof (FILE), 0 );
  File->FileSystemSpecificData = (VOID *)Fp;
  //
  // allocate space and read super block
  //
  FileSystem = AllocatePool (sizeof (*FileSystem));
  if (FileSystem == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto out;
  }

  SetMem32 (FileSystem, sizeof (*FileSystem), 0);
  Fp->SuperBlockPtr = FileSystem;

  Status = ReadSBlock (File, FileSystem);
  if (RETURN_ERROR (Status)) {
    goto out;
  }

#ifdef EXT2FS_DEBUG
  DumpSBlock (FileSystem);
#endif
  //
  // alloc a block sized buffer used for all FileSystem transfers
  //
  Fp->Buffer = AllocatePool (FileSystem->Ext2FsBlockSize);
  //
  // read group descriptor blocks
  //
  FileSystem->Ext2FsGrpDes = AllocatePool (sizeof(EXT2GD) * FileSystem->Ext2FsNumCylinder);
  Status = ReadGDBlock (File, FileSystem);
  if (RETURN_ERROR (Status)) {
    goto out;
  }

#ifdef EXT2FS_DEBUG
  DumpGroupDesBlock (FileSystem);
#endif
  //
  //  Calculate indirect block levels.
  //
  //
  //  We note that the number of indirect blocks is always
  //  a power of 2.  This lets us use shifts and masks instead
  //  of divide and remainder and avoinds pulling in the
  //  64bit division routine into the boot code.
  //
  mult = NINDIR (FileSystem);
  for (Length2 = 0; mult != 1; Length2++) {
    mult >>= 1;
  }

  Fp->NiShift = Length2;

  INumber = EXT2_ROOTINO;
  Status = ReadInode (INumber, File);
  if (RETURN_ERROR (Status)) {
    goto out;
  }

#ifndef LIBSA_FS_SINGLECOMPONENT
  Cp = Path;
  while (*Cp != '\0') {
    //
    //  Remove extra separators
    //
    while (*Cp == '/') {
      Cp++;
    }
    if (*Cp == '\0') {
      break;
    }

    //
    //  Check that current node is a directory.
    //
    if ((Fp->DiskInode.Ext2DInodeMode & EXT2_IFMT) != EXT2_IFDIR) {
      Status = EFI_LOAD_ERROR;
      goto out;
    }

    //
    //  Get next component of Path Name.
    //
    Ncp = Cp;
    while ((Component = *Cp) != '\0' && Component != '/') {
      Cp++;
    }

    //
    //  Look up component in current directory.
    //  Save directory INumber in case we find a
    //  symbolic link.
    //
#ifndef LIBSA_NO_FS_SYMLINK
    ParentInumber = INumber;
#endif
    Status = SearchDirectory (Ncp, (INT32)(Cp - Ncp), File, &INumber);
    if (RETURN_ERROR (Status)) {
      goto out;
    }

    //
    //  Open next component.
    //
    Status = ReadInode (INumber, File);
    if (RETURN_ERROR (Status)) {
      goto out;
    }

#ifndef LIBSA_NO_FS_SYMLINK
    //
    //  Check for symbolic link.
    //
    if ((Fp->DiskInode.Ext2DInodeMode & EXT2_IFMT) == EXT2_IFLNK) {
      //
      // XXX should handle LARGEFILE
      //
      UINTN LinkLength;
      UINTN Len;

      LinkLength = Fp->DiskInode.Ext2DInodeSize;

      Len = AsciiStrLen (Cp);

      if (Nlinks == 0) {
        /* copy the top-most filename */
        AsciiStrCpyS (SymFileNameBuf, EXT2FS_MAXNAMLEN, Ncp);
      }

      if (((LinkLength + Len) > MAXPATHLEN) ||
          ((++Nlinks) > MAXSYMLINKS)) {
        Status = RETURN_LOAD_ERROR;
        goto out;
      }

      CopyMem (&NameBuf[LinkLength], Cp, Len + 1);

      if (LinkLength < EXT2_MAXSYMLINKLEN) {
        CopyMem (NameBuf, Fp->DiskInode.Ext2DInodeBlocks, LinkLength);
      } else {
        //
        //  Read FILE for symbolic link
        //
        UINT32 BufSize;
        INDPTR    DiskBlock;

        Buf = Fp->Buffer;
        Status = BlockMap (File, (INDPTR)0, &DiskBlock);
        if (RETURN_ERROR (Status)) {
          goto out;
        }

        Status = DEV_STRATEGY (File->DevPtr) (File->FileDevData,
                                          F_READ, FSBTODB (FileSystem, DiskBlock),
                                          FileSystem->Ext2FsBlockSize, Buf, &BufSize);
        if (RETURN_ERROR (Status)) {
          goto out;
        }

        CopyMem (NameBuf, Buf, LinkLength);
      }

      //
      //  If relative pathname, restart at parent directory.
      //  If absolute pathname, restart at root.
      //
      Cp = NameBuf;
      if (*Cp != '/') {
        INumber = ParentInumber;
      } else {
        INumber = (INODE32)EXT2_ROOTINO;
      }

      if (Nlinks == 1) {
        /* only show the dest name of the next link */
        AsciiStrCatS (SymFileNameBuf, EXT2FS_MAXNAMLEN, " -> ");
        AsciiStrCatS (SymFileNameBuf, EXT2FS_MAXNAMLEN, NameBuf);
      }

      Status = ReadInode (INumber, File);
      if (RETURN_ERROR (Status)) {
        goto out;
      }
    }
#endif    // !LIBSA_NO_FS_SYMLINK
  }

  //
  //  Found terminal component.
  //
  Status = RETURN_SUCCESS;

#else // !LIBSA_FS_SINGLECOMPONENT
  //
  // look up component in the current (root) directory
  //
  Status = SearchDirectory (Path, AsciiStrLen (Path), File, &INumber);
  if (RETURN_ERROR (Status)) {
    goto out;
  }
  //
  // open it
  //
  Status = ReadInode (INumber, File);

#endif // !LIBSA_FS_SINGLECOMPONENT

  Fp->SeekPtr = 0;        // reset seek pointer

#ifndef LIBSA_NO_FS_SYMLINK
  if (Nlinks > 0) {
    AsciiStrCpyS (File->FileNameBuf, EXT2FS_MAXNAMLEN, SymFileNameBuf);
  }
#endif

out:
  if (RETURN_ERROR (Status)) {
    Ext2fsClose (File);
  }
  return Status;
}

/**
  Close the opened file.

  @param[in/out]    File        File to be closed.

  @retval RETURN_SUCCESS regardless of success/fail condition
**/
RETURN_STATUS
EFIAPI
Ext2fsClose (
  IN OUT  OPEN_FILE     *File
  )
{
  FILE *Fp;

  Fp = (FILE *)File->FileSystemSpecificData;

  File->FileSystemSpecificData = NULL;
  if (Fp == NULL) {
    return RETURN_SUCCESS;
  }

  if (Fp->SuperBlockPtr->Ext2FsGrpDes) {
    FreePool (Fp->SuperBlockPtr->Ext2FsGrpDes);
  }
  if (Fp->Buffer) {
    FreePool (Fp->Buffer);
  }
  FreePool (Fp->SuperBlockPtr);
  FreePool (Fp);
  return RETURN_SUCCESS;
}

/**
  Gets the size of the file from descriptor.

  @param[in]    File      File to be closed.

  @retval size of the file from descriptor.
**/
UINT32
EFIAPI
Ext2fsFileSize (
  IN  OPEN_FILE     *File
  )
{
  FILE *Fp;
  Fp = (FILE *)File->FileSystemSpecificData;
  return (UINT32)Fp->DiskInode.Ext2DInodeSize;
}

/**
  Copy a portion of a FILE into a memory.
  Cross block boundaries when necessary

  @param[in/out]    File      File handle to be read
  @param[in]        Start     Start address of read buffer
  @param[in]        Size      Size to be read
  @param[out]       ResId     Actual read size

  @retval RETURN_SUCCESS if file read is success
  @retval other if error.
**/
RETURN_STATUS
EFIAPI
Ext2fsRead (
  IN OUT  OPEN_FILE     *File,
  IN      VOID          *Start,
  IN      UINT32         Size,
  OUT     UINT32        *ResId
  )
{
  FILE *Fp;
  UINT32 Csize;
  CHAR8 *Buf;
  UINT32 BufSize;
  CHAR8 *Address;
  RETURN_STATUS Status;

  Fp = (FILE *)File->FileSystemSpecificData;
  Status = RETURN_SUCCESS;
  Address = Start;

  while (Size != 0) {
    //
    // XXX should handle LARGEFILE
    //
    if (Fp->SeekPtr >= (OFFSET)Fp->DiskInode.Ext2DInodeSize) {
      break;
    }

    Status = BufReadFile (File, &Buf, &BufSize);
    if (RETURN_ERROR (Status)) {
      break;
    }

    Csize = Size;
    if (Csize > BufSize) {
      Csize = BufSize;
    }

    CopyMem (Address, Buf, Csize);

    Fp->SeekPtr += Csize;
    Address += Csize;
    Size -= Csize;
  }
  if (ResId != NULL) {
    *ResId = Size;
  }
  return Status;
}

#ifdef EXT2FS_DEBUG
/**
  Dump the file system super block info.

  @param[in]  FileSystem     pointer to filesystem.

  @retval     none
**/
VOID
EFIAPI
DumpSBlock (
  IN  M_EXT2FS  *FileSystem
  )
{

  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsBlockCount = %u\n", FileSystem->Ext2Fs.Ext2FsBlockCount));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsFirstDataBlock = %u\n", FileSystem->Ext2Fs.Ext2FsFirstDataBlock));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsLogBlockSize = %u\n", FileSystem->Ext2Fs.Ext2FsLogBlockSize));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsBlocksPerGroup = %u\n", FileSystem->Ext2Fs.Ext2FsBlocksPerGroup));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsINodesPerGroup = %u\n", FileSystem->Ext2Fs.Ext2FsINodesPerGroup));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsMagic = 0x%x\n", FileSystem->Ext2Fs.Ext2FsMagic));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsRev = %u\n", FileSystem->Ext2Fs.Ext2FsRev));

  if (FileSystem->Ext2Fs.Ext2FsRev == E2FS_REV1) {
    DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsFirstInode = %u\n",
            FileSystem->Ext2Fs.Ext2FsFirstInode));
    DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsInodeSize = %u\n",
            FileSystem->Ext2Fs.Ext2FsInodeSize));
    DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsFeaturesCompat = %u\n",
            FileSystem->Ext2Fs.Ext2FsFeaturesCompat));
    DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsFeaturesIncompat = %u\n",
            FileSystem->Ext2Fs.Ext2FsFeaturesIncompat));
    DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsFeaturesROCompat = %u\n",
            FileSystem->Ext2Fs.Ext2FsFeaturesROCompat));
    DEBUG ((DEBUG_INFO, "FileSystem->Ext2Fs.Ext2FsRsvdGDBlock = %u\n",
            FileSystem->Ext2Fs.Ext2FsRsvdGDBlock));
  }

  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsGDSize = %u\n", FileSystem->Ext2FsGDSize));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsBlockSize = %u\n", FileSystem->Ext2FsBlockSize));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsFsbtobd = %u\n", FileSystem->Ext2FsFsbtobd));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsNumCylinder = %u\n", FileSystem->Ext2FsNumCylinder));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsNumGrpDesBlock = %u\n", FileSystem->Ext2FsNumGrpDesBlock));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsInodesPerBlock = %u\n", FileSystem->Ext2FsInodesPerBlock));
  DEBUG ((DEBUG_INFO, "FileSystem->Ext2FsInodesTablePerGrp = %u\n", FileSystem->Ext2FsInodesTablePerGrp));
}

/**
  Dump the file group descriptor block info.

  @param[in]  FileSystem     pointer to filesystem.

  @retval     none
**/
VOID
EFIAPI
DumpGroupDesBlock (
  IN  M_EXT2FS  *FileSystem
  )
{
  INT32     Index;
  EXT2GD    *Ext2FsGrpDesEntry;

  for (Index=0; Index < FileSystem->Ext2FsNumCylinder; Index++) {
    Ext2FsGrpDesEntry = (EXT2GD*) ((UINT32) FileSystem->Ext2FsGrpDes + (Index * FileSystem->Ext2FsGDSize));
    DEBUG ((DEBUG_INFO, "Ext2FsGrpDes[Index=%u]\n", Index));
    DEBUG ((DEBUG_INFO, "  Ext2BGDBlockBitmap   %u\n", Ext2FsGrpDesEntry->Ext2BGDBlockBitmap));
    DEBUG ((DEBUG_INFO, "  Ext2BGDInodeBitmap   %u\n", Ext2FsGrpDesEntry->Ext2BGDInodeBitmap));
    DEBUG ((DEBUG_INFO, "  Ext2BGDInodeTables   %u\n", Ext2FsGrpDesEntry->Ext2BGDInodeTables));
    DEBUG ((DEBUG_INFO, "  Ext2BGDFreeBlocks    %u\n", Ext2FsGrpDesEntry->Ext2BGDFreeBlocks));
    DEBUG ((DEBUG_INFO, "  Ext2BGDFreeInodes    %u\n", Ext2FsGrpDesEntry->Ext2BGDFreeInodes));
    DEBUG ((DEBUG_INFO, "  Ext2BGDNumDir        %u\n", Ext2FsGrpDesEntry->Ext2BGDNumDir));
    if (FileSystem->Ext2FsGDSize > 32) {
      DEBUG ((DEBUG_INFO, "  Ext2BGDInodeTablesHi %u\n", Ext2FsGrpDesEntry->Ext2BGDInodeTablesHi));
    }
  }
}
#endif
