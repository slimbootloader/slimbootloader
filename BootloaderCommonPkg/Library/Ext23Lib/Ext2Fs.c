/** @file
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

#include "Ext2Fs.h"

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/Ext23Lib.h>
#include <Library/MediaAccessLib.h>
#include "LibsaFsStand.h"

#define    LITTLE_ENDIAN  1234              // LSB First: i386, vax
#define    BIG_ENDIAN     4321              // MSB First: 68000, ibm, net
#define    PDP_ENDIAN     3412              // LSB First in word, MSW First in long

#define BYTE_ORDER LITTLE_ENDIAN
#define HOWMANY(x, y)  (((x)+((y)-1))/(y))
#define DEV_BSHIFT  9                       // log2(DEV_BSIZE)
#define DEV_BSIZE   (1 << DEV_BSHIFT)


#if defined(LIBSA_FS_SINGLECOMPONENT) && !defined(LIBSA_NO_FS_SYMLINK)
#define LIBSA_NO_FS_SYMLINK
#endif

#if defined(LIBSA_NO_TWIDDLE)
#define TWIDDLE()
#endif

#ifndef FSBTODB
#define FSBTODB(FileSystem, indp) FSBTODB(FileSystem, indp)
#endif

#if defined(LIBSA_ENABLE_LS_OP)

#include "cli.h"

#define NELEM(x) (sizeof (x) / sizeof(*x))

typedef struct ENTRY ENTRY;
struct ENTRY {
  ENTRY    *EntryNext;
  INODE32  EntryInode;
  UINT8    EntryType;
  CHAR8    EntryName[1];
};

STATIC CONST CHAR8 *CONST mTypeStr[] = { "unknown", "REG",  "DIR",  "CHR",  "BLK",  "FIFO",  "SOCK",  "LNK" };

#endif  // LIBSA_ENABLE_LS_OP


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

  DEBUG ((DEBUG_INFO, "Detected EXT on StartBlock %d Part %d\n", PrivateData->StartBlock, SwPart));

  *FsHandle = (EFI_HANDLE)PrivateData;

  return EFI_SUCCESS;
}

/**
Read the FILE by Name for EXT2/3 FileSystem

@param  PrivateData          The EXT2/3 FILE system handle
@param  FileName             The FILE Name to read
@param  FileBufferPtr        The address of FILE read in memory
@param  FileSizePtr          The address point for FILE Size

@retval EFI_SUCCESS       The function completed successfully.
@retval !EFI_SUCCESS      Something error while read FILE.

**/
EFI_STATUS
EFIAPI
ExtGetFileByName (
  IN    EFI_HANDLE      PrivateData,
  IN    CHAR16         *FileName,
  OUT   VOID          **FileBufferPtr,
  OUT   UINTN          *FileSizePtr
  )
{
  OPEN_FILE   OpenFile;
  INT32       Ret;
  UINT32      FileSize, Residual, NameSize;
  CHAR8      *FileBuffer;
  CHAR8      *NameBuffer;

  if (((PEI_EXT_PRIVATE_DATA *)PrivateData)->Signature != FS_EXT_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  OpenFile.FileDevData = PrivateData;

  NameSize = StrSize (FileName);
  NameBuffer = AllocatePool (NameSize);
  NameBuffer = UnicodeStrToAsciiStr (FileName, NameBuffer);

  Ret = Ext2fsOpen (NameBuffer, &OpenFile);
  if (Ret != 0) {
    DEBUG ((EFI_D_INFO, "** Openning FAILED\n"));
    return Ret;
  }

  FileSize = Ext2fsFileSize (&OpenFile);
  DEBUG ((EFI_D_INFO, "** Openning SUCCEEDED, struct file Size == 0x%X\n", FileSize));

  FileBuffer = AllocatePages (EFI_SIZE_TO_PAGES (FileSize));
  ASSERT (FileBuffer != NULL);
  Residual = 0;
  Ret = Ext2fsRead (&OpenFile, FileBuffer, FileSize, &Residual);
  ASSERT (Ret == 0);
  ASSERT (Residual == 0);

  *FileBufferPtr = FileBuffer;
  *FileSizePtr = FileSize;

  return Ret;
}


/**
Gives the info of device block config.

@param  DevData     Device privete data.
@param  ReadWrite   Read write
@param  BlockNum    Block number to start
@param  Size        Size to read block.
@param  Buf         Buffer to read the block data.
@param  RSize

@retval EFI_SUCCESS       The function completed successfully.
@retval !EFI_SUCCESS      Something error while read FILE.
**/
INT32
BDevStrategy (
  VOID     *DevData,
  INT32     ReadWrite,
  DADDRESS  BlockNum,
  UINT32    Size,
  VOID     *Buf,
  UINT32   *RSize
  )
{
  INT32                    Res;
  PEI_EXT_PRIVATE_DATA    *PrivateData;
  UINT64                  Startblockno;

  PrivateData = DevData;

  if (ReadWrite != F_READ && ReadWrite != F_WRITE) {
    return EFI_INVALID_PARAMETER;
  }
  if ((Size % BDEV_BLOCKSIZE) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  Startblockno = BlockNum + PrivateData->StartBlock;
  if (ReadWrite == F_READ) {
    Res = MediaReadBlocks (0, (UINT32)Startblockno, Size, Buf);
    if (Res != 0) {
      return Res;
    }
  }

  *RSize = Size;
  return 0;
}

/**
 Find the length of the string.
 @param [in] Str string to find length
 @retval length of the string.
**/
UINT32
Strlen (
  CONST CHAR8 *Str
  )
{
  CONST CHAR8 *Ptr;

  for (Ptr = Str ; *Ptr != 0 ; Ptr += 1)
    ;

  return (Ptr - Str);
}

/**
 Read a new inode into a FILE structure.
 @param [in] INumber inode number
 @param [in] File pointer to open file struct.
 @retval
**/
STATIC
INT32
ReadInode (
  IN    INODE32      INumber,
  IN    OPEN_FILE   *File
  )
{
  FILE         *Fp;
  M_EXT2FS     *FileSystem;
  CHAR8        *Buf;
  UINT32        RSize;
  INT32         Rc;
  DADDRESS      InodeSector;
  EXTFS_DINODE *DInodePtr;

  Fp = (FILE *)File->FileSystemSpecificData;
  FileSystem = Fp->SuperBlockPtr;

  InodeSector = FSBTODB (FileSystem, INODETOFSBA (FileSystem, INumber));

  //
  // Read inode and save it.
  //
  Buf = Fp->Buffer;
  TWIDDLE();
  Rc = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                    InodeSector, FileSystem->Ext2FsBlockSize, Buf, &RSize);
  if (Rc != 0) {
    return Rc;
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
  return Rc;
}

/**
  Given an offset in a FILE, find the disk block number that
  contains that block.
  @param File           pointer to an Open file.
  @param FileBlock      Block to find the file.
  @param DiskBlockPtr   Pointer to the disk which contains block.
  @retval 0 if success
  @retval other if error.
**/
STATIC
INT32
BlockMap (
  OPEN_FILE     *File,
  INDPTR         FileBlock,
  INDPTR        *DiskBlockPtr
  )
{
  FILE     *Fp;
  M_EXT2FS *FileSystem;
  UINT32    Level;
  INDPTR    IndCache;
  INDPTR    IndBlockNum;
  UINT32    RSize;
  INT32     Rc;
  INDPTR   *Buf;

  Fp = (FILE *)File->FileSystemSpecificData;
  FileSystem = Fp->SuperBlockPtr;
  Buf = (VOID *)Fp->Buffer;

  if (FileBlock < NDADDR) {
    //
    // Direct block.
    //
    *DiskBlockPtr = FS2H32 (Fp->DiskInode.Ext2DInodeBlocks[FileBlock]);
    return 0;
  }

  FileBlock -= NDADDR;

  IndCache = FileBlock >> LN2_IND_CACHE_SZ;
  if (IndCache == Fp->InodeCacheBlock) {
    *DiskBlockPtr =
      FS2H32 (Fp->InodeCache[FileBlock & IND_CACHE_MASK]);
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
    FS2H32 (Fp->DiskInode.Ext2DInodeBlocks[NDADDR + (Level / Fp->NiShift - 1)]);

  for (;;) {
    Level -= Fp->NiShift;
    if (IndBlockNum == 0) {
      *DiskBlockPtr = 0;    // missing
      return 0;
    }

    TWIDDLE();
    //
    //  If we were feeling brave, we could work out the number
    //  of the disk sector and read a single disk sector instead
    //  of a filesystem block.
    //  However we don't do this very often anyway...
    //
    Rc = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                      FSBTODB (Fp->SuperBlockPtr, IndBlockNum), FileSystem->Ext2FsBlockSize,
                                      Buf, &RSize);
    if (Rc != 0) {
      return Rc;
    }
    if (RSize != (UINT32)FileSystem->Ext2FsBlockSize) {
      return EFI_DEVICE_ERROR;
    }
    IndBlockNum = FS2H32 (Buf[FileBlock >> Level]);
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

  return 0;
}

/**
  Read a portion of a FILE into an internal buffer.
  Return the location in the buffer and the amount in the buffer.
  @param File       Pointer to the open file.
  @param BufferPtr  buffer corresponding to offset
  @param SizePtr    Size of remainder of buffer.
**/
STATIC
INT32
BufReadFile (
  OPEN_FILE     *File,
  CHAR8        **BufferPtr,
  UINT32        *SizePtr
  )
{
  FILE *Fp;
  M_EXT2FS *FileSystem;
  INT32 Off;
  INDPTR FileBlock;
  INDPTR DiskBlock;
  UINT32 BlockSize;
  INT32 Rc;

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

      TWIDDLE();
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
  Search a directory for a Name and return its
  inode number.
  @param Name       Name to compare with
  @param Length     Length of the dir name
  @param File       Pointer to file private data
  @param INumPtr    pointer to Inode number.
**/
STATIC
INT32
SearchDirectory (
  CHAR8         *Name,
  INT32          Length,
  OPEN_FILE     *File,
  INODE32       *INumPtr
  )
{
  FILE *Fp;
  EXT2FS_DIRECT *Dp;
  EXT2FS_DIRECT *EdPtr;
  CHAR8 *Buf;
  UINT32 BufSize;
  INT32 NameLen;
  INT32 Rc;

  Fp = (FILE *)File->FileSystemSpecificData;

  Fp->SeekPtr = 0;
  //
  // XXX should handle LARGEFILE
  //
  while (Fp->SeekPtr < (OFFSET)Fp->DiskInode.Ext2DInodeSize) {
    Rc = BufReadFile (File, &Buf, &BufSize);
    if (Rc != 0) {
      return Rc;
    }

    Dp = (EXT2FS_DIRECT *)Buf;
    EdPtr = (EXT2FS_DIRECT *) (Buf + BufSize);
    for (; Dp < EdPtr;
         Dp = (VOID *) ((CHAR8 *)Dp + FS2H16 (Dp->Ext2DirectRecLen))) {
      if (FS2H16 (Dp->Ext2DirectRecLen) <= 0) {
        break;
      }
      if (FS2H32 (Dp->Ext2DirectInodeNumber) == (INODE32)0) {
        continue;
      }
      NameLen = Dp->Ext2DirectNameLen;
      if (NameLen == Length &&
          !CompareMem (Name, Dp->Ext2DirectName, Length)) {
        //
        // found entry
        //
        *INumPtr = FS2H32 (Dp->Ext2DirectInodeNumber);
        return 0;
      }
    }
    Fp->SeekPtr += BufSize;
  }
  return EFI_UNSUPPORTED;
}

/**
  Read Superblock of the file.
  @param File       File for which super block needs to be read.
  @param FileSystem Fs on which super block is computed.

  @retval 0 if superblock compute is success
  @retval other if error.
**/
INT32
ReadSBlock (
  OPEN_FILE     *File,
  M_EXT2FS      *FileSystem
  )
{
  STATIC UINT8 SbBuf[SBSIZE];
  EXT2FS Ext2Fs;
  UINT32 BufSize;
  INT32 Rc;

  Rc = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                    SBOFF / DEV_BSIZE, SBSIZE, SbBuf, &BufSize);
  if (Rc != 0) {
    return Rc;
  }

  if (BufSize != SBSIZE) {
    return EFI_DEVICE_ERROR;
  }

  E2FS_SBLOAD ((VOID *)SbBuf, &Ext2Fs);
  if (Ext2Fs.Ext2FsMagic != E2FS_MAGIC) {
    return EFI_INVALID_PARAMETER;
  }
  if (Ext2Fs.Ext2FsRev > E2FS_REV1 ||
      (Ext2Fs.Ext2FsRev == E2FS_REV1 &&
       (Ext2Fs.Ext2FsFirstInode != EXT2_FIRSTINO ||
        (Ext2Fs.Ext2FsInodeSize != 128 && Ext2Fs.Ext2FsInodeSize != 256) ||
        Ext2Fs.Ext2FsFeaturesIncompat & ~EXT2F_INCOMPAT_SUPP))) {
    return EFI_UNSUPPORTED;
  }

  E2FS_SBLOAD ((VOID *)SbBuf, &FileSystem->Ext2Fs);
  //
  // compute in-memory m_ext2fs values
  //
  FileSystem->Ext2FsNumCylinder       =
    HOWMANY (FileSystem->Ext2Fs.Ext2FsBlockCount - FileSystem->Ext2Fs.Ext2FsFirstDataBlock,
             FileSystem->Ext2Fs.Ext2FsBlocksPerGroup);
  //
  // XXX assume hw bsize = 512
  //
  FileSystem->Ext2FsFsbtobd           = FileSystem->Ext2Fs.Ext2FsLogBlockSize + 1;
  FileSystem->Ext2FsBlockSize         = MINBSIZE << FileSystem->Ext2Fs.Ext2FsLogBlockSize;
  FileSystem->Ext2FsLogicalBlock      = LOG_MINBSIZE + FileSystem->Ext2Fs.Ext2FsLogBlockSize;
  FileSystem->Ext2FsQuadBlockOffset   = FileSystem->Ext2FsBlockSize - 1;
  FileSystem->Ext2FsBlockOffset       = (UINT32)~FileSystem->Ext2FsQuadBlockOffset;
  FileSystem->Ext2FsNumGrpDesBlock    =
    HOWMANY (FileSystem->Ext2FsNumCylinder, FileSystem->Ext2FsBlockSize / sizeof (EXT2GD));
  FileSystem->Ext2FsInodesPerBlock    = FileSystem->Ext2FsBlockSize / Ext2Fs.Ext2FsInodeSize;
  FileSystem->Ext2FsInodesTablePerGrp = FileSystem->Ext2Fs.Ext2FsINodesPerGroup / FileSystem->Ext2FsInodesPerBlock;

  return 0;
}

/**
  Read group descriptor of the file.
  @param File       File for which group descriptor needs to be read.
  @param FileSystem Fs on which super block is computed.

  @retval 0 if Group descriptor read is success
  @retval other if error.
**/
INT32
ReadGDBlock (
  OPEN_FILE     *File,
  M_EXT2FS      *FileSystem
  )
{
  FILE *Fp;
  UINT32 RSize;
  UINT32 gdpb;
  INT32 Index, Rc;

  Fp = (FILE *)File->FileSystemSpecificData;

  gdpb = FileSystem->Ext2FsBlockSize / sizeof (EXT2GD);

  for (Index = 0; Index < FileSystem->Ext2FsNumGrpDesBlock; Index++) {
    Rc = DEV_STRATEGY (File->DevPtr) (File->FileDevData, F_READ,
                                      FSBTODB (FileSystem, FileSystem->Ext2Fs.Ext2FsFirstDataBlock +
                                          1 /* superblock */ + Index),
                                      FileSystem->Ext2FsBlockSize, Fp->Buffer, &RSize);
    if (Rc != 0) {
      return Rc;
    }
    if (RSize != (UINT32)FileSystem->Ext2FsBlockSize) {
      return EFI_DEVICE_ERROR;
    }

    E2FS_CGLOAD ((EXT2GD *)Fp->Buffer,
                 &FileSystem->Ext2FsGrpDes[Index * gdpb],
                 (Index == (FileSystem->Ext2FsNumGrpDesBlock - 1)) ?
                 (FileSystem->Ext2FsNumCylinder - gdpb * Index) * sizeof (EXT2GD) :
                 FileSystem->Ext2FsBlockSize);
  }

  return 0;
}


/**
  Open struct file.

  @param  Path          path to locate the file
  @param  File          The struct having the device and file info

  @retval 0 if Group descriptor read is success
  @retval other if error.
**/
INT32
Ext2fsOpen (
  CHAR8         *Path,
  OPEN_FILE     *File
  )
{
#ifndef LIBSA_FS_SINGLECOMPONENT
  CONST CHAR8 *Cp;
  CONST CHAR8 *Ncp;
  INT32 Component;
#endif
  INODE32 INumber;
  FILE *Fp;
  M_EXT2FS *FileSystem;
  INT32 Rc;
#ifndef LIBSA_NO_FS_SYMLINK
  INODE32 ParentInumber;
  INT32 Nlinks;
  CHAR8 NameBuf[MAXPATHLEN + 1];
  CHAR8 *Buf;

  Nlinks = 0;
#endif

  INDPTR mult;
  INT32  Length2;

  //
  // allocate struct file system specific data structure
  //
  Fp = AllocatePool (sizeof (FILE));
  SetMem32 (Fp, sizeof (FILE), 0 );
  File->FileSystemSpecificData = (VOID *)Fp;
  //
  // allocate space and read super block
  //
  FileSystem = AllocatePool (sizeof (*FileSystem));
  SetMem32 (FileSystem, sizeof (*FileSystem), 0);
  Fp->SuperBlockPtr = FileSystem;
  TWIDDLE();

  Rc = ReadSBlock (File, FileSystem);
  if (Rc != 0) {
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
  FileSystem->Ext2FsGrpDes = AllocatePool (sizeof (EXT2GD) * FileSystem->Ext2FsNumCylinder);
  Rc = ReadGDBlock (File, FileSystem);
  if (Rc != 0) {
    goto out;
  }

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
  if ((Rc = ReadInode (INumber, File)) != 0) {
    goto out;
  }

#ifndef LIBSA_FS_SINGLECOMPONENT
  Cp = Path;
  while (*Cp != NULL) {

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
      Rc = ENOTDIR;
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
    Rc = SearchDirectory (Ncp, Cp - Ncp, File, &INumber);
    if (Rc != 0) {
      goto out;
    }

    //
    //  Open next component.
    //
    if ((Rc = ReadInode (INumber, File)) != 0) {
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
      INT32 LinkLength;
      INT32 Len;

      LinkLength = Fp->DiskInode.Ext2DInodeSize;

      Len = Strlen (Cp);

      if (LinkLength + Len > MAXPATHLEN ||
          ++Nlinks > MAXSYMLINKS) {
        Rc = ENOENT;
        goto out;
      }

      memmove (&NameBuf[LinkLength], Cp, Len + 1);

      if (LinkLength < EXT2_MAXSYMLINKLEN) {
        CopyMem (NameBuf, Fp->DiskInode.Ext2DInodeBlocks, LinkLength);
      } else {
        //
        //  Read FILE for symbolic link
        //
        UINT32 BufSize;
        INDPTR    DiskBlock;

        Buf = Fp->Buffer;
        Rc = BlockMap (File, (INDPTR)0, &DiskBlock);
        if (Rc != 0) {
          goto out;
        }

        TWIDDLE();
        Rc = DEV_STRATEGY (File->DevPtr) (File->FileDevData,
                                          F_READ, FSBTODB (FileSystem, DiskBlock),
                                          FileSystem->Ext2FsBlockSize, Buf, &BufSize);
        if (Rc != 0) {
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

      if ((Rc = ReadInode (INumber, File)) != 0) {
        goto out;
      }
    }
#endif    // !LIBSA_NO_FS_SYMLINK
  }

  //
  //  Found terminal component.
  //
  Rc = 0;

#else // !LIBSA_FS_SINGLECOMPONENT
  //
  // look up component in the current (root) directory
  //
  Rc = SearchDirectory (Path, Strlen (Path), File, &INumber);
  if (Rc != 0) {
    goto out;
  }
  //
  // open it
  //
  Rc = ReadInode (INumber, File);

#endif // !LIBSA_FS_SINGLECOMPONENT

  Fp->SeekPtr = 0;        // reset seek pointer

out:
  if (Rc != 0) {
    Ext2fsClose (File);
  }
#if 0
  else {
    fsmod = "Ext2Fs";
  }
#endif
  return Rc;
}

/**
  Close the file.
  @param File  File to be closed.

  @retval 0 if Group descriptor read is success
**/
INT32
Ext2fsClose (
  OPEN_FILE     *File
  )
{
  FILE *Fp;

  Fp = (FILE *)File->FileSystemSpecificData;

  File->FileSystemSpecificData = NULL;
  if (Fp == NULL) {
    return 0;
  }

  if (Fp->SuperBlockPtr->Ext2FsGrpDes) {
    FreePool (Fp->SuperBlockPtr->Ext2FsGrpDes);
  }
  if (Fp->Buffer) {
    FreePool (Fp->Buffer);
  }
  FreePool (Fp->SuperBlockPtr);
  FreePool (Fp);
  return 0;
}

/**
  Gets the size of the file from descriptor.

  @param File  File to be closed.
  @retval size of the file from descriptor.
**/
UINT32
Ext2fsFileSize (
  OPEN_FILE     *File
  )
{
  FILE *Fp;
  Fp = (FILE *)File->FileSystemSpecificData;
  return (UINT32)Fp->DiskInode.Ext2DInodeSize;
}

/**
  Copy a portion of a FILE into kernel memory.
  Cross block boundaries when necessary
  @param File
  @param Start
  @param Size
  @param ResId
**/
INT32
Ext2fsRead (
  OPEN_FILE     *File,
  VOID          *Start,
  UINT32         Size,
  UINT32        *ResId
  )
{
  FILE *Fp;
  UINT32 Csize;
  CHAR8 *Buf;
  UINT32 BufSize;
  INT32 Rc;
  CHAR8 *Address;

  Fp = (FILE *)File->FileSystemSpecificData;
  Rc = 0;
  Address = Start;

  while (Size != 0) {
    //
    // XXX should handle LARGEFILE
    //
    if (Fp->SeekPtr >= (OFFSET)Fp->DiskInode.Ext2DInodeSize) {
      break;
    }

    Rc = BufReadFile (File, &Buf, &BufSize);
    if (Rc != 0) {
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
  return Rc;
}

/**
  Updates the seek ptr of file based on seek point.
  @param File       pointer to an Open file.
  @param Offset     Offset to update the seekptr.
  @param Where      Seek point where it needs to be update.

**/
OFFSET
Ext2fsSeek (
  OPEN_FILE    *File,
  OFFSET        Offset,
  INT32         Where
  )
{
  FILE *Fp;
  Fp = (FILE *)File->FileSystemSpecificData;

  switch (Where) {
  case SEEK_SET:
    Fp->SeekPtr = Offset;
    break;
  case SEEK_CUR:
    Fp->SeekPtr += Offset;
    break;
  case SEEK_END:
    //
    // XXX should handle LARGEFILE
    //
    Fp->SeekPtr = Fp->DiskInode.Ext2DInodeSize - Offset;
    break;
  default:
    return -1;
  }
  return Fp->SeekPtr;
}

/**
  Update the mode and size from descriptor to stat Block.
  contains that block.
  @param File       pointer to an file private data.
  @param StatBlock  pointer to File information (stats).
  @retval 0 if success
**/
INT32
Ext2fsStat (
  OPEN_FILE     *File,
  STAT          *StatBlock
  )
{
  FILE *Fp;

  Fp = (FILE *)File->FileSystemSpecificData;
  //
  // only important stuff
  //
  SetMem32 (StatBlock, sizeof * StatBlock, 0);
  StatBlock->StatMode = Fp->DiskInode.Ext2DInodeMode;
  //
  // XXX should handle LARGEFILE
  //
  StatBlock->StatSize = Fp->DiskInode.Ext2DInodeSize;
  return 0;
}

#if defined(LIBSA_ENABLE_LS_OP)
/**
  Update the mode and size from descriptor to stat Block.
  contains that block.
  @param File       pointer to an file private data
  @param Pattern    pointer to Pattern
**/
VOID
Ext2fsLs (
  OPEN_FILE     *File,
  CONST CHAR8   *Pattern
  )
{
  FILE    *Fp;
  UINT32   BlockSize;
  CHAR8   *Buf;
  UINT32   BufSize;
  ENTRY   *Names;
  ENTRY   *New;
  ENTRY  **NextPtr;

  Fp = (FILE *)File->FileSystemSpecificData;
  BlockSize = Fp->SuperBlockPtr->Ext2FsBlockSize;
  Names = NULL;

  Fp->SeekPtr = 0;
  while (Fp->SeekPtr < (OFFSET)Fp->DiskInode.Ext2DInodeSize) {
    EXT2FS_DIRECT  *Dp;
    EXT2FS_DIRECT  *EdPtr;
    INT32 Rc;
    Rc = BufReadFile (File, &Buf, &BufSize);
    if (Rc != 0) {
      goto out;
    }
    if (BufSize != BlockSize || BufSize == 0) {
      goto out;
    }

    Dp = (EXT2FS_DIRECT *)Buf;
    EdPtr = (EXT2FS_DIRECT *) (Buf + BufSize);

    for (; Dp < EdPtr;
         Dp = (VOID *) ((CHAR8 *)Dp + FS2H16 (Dp->Ext2DirectRecLen))) {
      CONST CHAR8 *Type;

      if (FS2H16 (Dp->Ext2DirectRecLen) <= 0) {
        goto out;
      }

      if (FS2H32 (Dp->Ext2DirectInodeNumber) == 0) {
        continue;
      }

      if (Dp->Ext2DirectType >= NELEM (mTypeStr) ||
          ! (Type = mTypeStr[Dp->Ext2DirectType])) {
        //
        //  This does not handle "Old"
        //  filesystems properly. On little
        //  endian machines, we get a bogus
        //  type Name if the NameLen matches a
        //  valid type identifier. We could
        //  check if we read NameLen "0" and
        //  handle this case specially, if
        //  there were a pressing need...
        //
        DEBUG ((DEBUG_INFO, "bad dir entry\n"));
        goto out;
      }
#if 0
      if ((Pattern != NULL) && !fnmatch (Dp->Ext2DirectName, Pattern)) {
        continue;
      }
#endif
      New = AllocatePool (sizeof * New + Strlen (Dp->Ext2DirectName));
      if (New == NULL) {
        DEBUG ((DEBUG_INFO, "%d: %s (%s)\n",
                FS2H32 (Dp->Ext2DirectInodeNumber), Dp->Ext2DirectName, Type));
        continue;
      }
      New->EntryInode = FS2H32 (Dp->Ext2DirectInodeNumber);
      New->EntryType  = Dp->Ext2DirectType;
      strcpy (New->EntryName, Dp->Ext2DirectName);
      for (NextPtr = &Names; *NextPtr != NULL; NextPtr = & (*NextPtr)->EntryNext) {
        if (strcmp (New->EntryName, (*NextPtr)->EntryName) < 0) {
          break;
        }
      }
      New->EntryNext = *NextPtr;
      *NextPtr = New;
    }
    Fp->SeekPtr += BufSize;
  }

  if (Names != NULL) {
    ENTRY *PNames;
    PNames = Names;
    do {
      New = PNames;
      DEBUG ((DEBUG_INFO, "%d: %s (%s)\n",
              New->EntryInode, New->EntryName, mTypeStr[New->EntryType]));
      PNames = New->EntryNext;
    } while (PNames != NULL);
  } else {
    DEBUG ((DEBUG_INFO, "not found\n"));
  }
out:
  if (Names != NULL) {
    do {
      New   = Names;
      Names = New->EntryNext;
      FreePool (New);
    } while (Names != NULL);
  }
  return;
}
#endif


#if BYTE_ORDER == BIG_ENDIAN
/**
  Byte swap functions for big endian machines.
  (Ext2Fs is always little endian)

  XXX: We should use src/sys/ufs/Ext2Fs/ext2fs_bswap.c

 These functions are only needed if native byte order is not big endian

 @param Old pointer to old filesystem field
 @param New pointer to new filesystem field
**/
VOID
E2fsSBByteSwap (
  EXT2FS    *Old,
  EXT2FS    *New
  )
{
  //
  // preserve unused fields
  //
  CopyMem (New, Old, sizeof (EXT2FS));
  New->Ext2FsINodeCount         =   bswap32 (Old->Ext2FsINodeCount);
  New->Ext2FsBlockCount         =   bswap32 (Old->Ext2FsBlockCount);
  New->Ext2FsRsvdBlockCount     =   bswap32 (Old->Ext2FsRsvdBlockCount);
  New->Ext2FsFreeBlockCount     =   bswap32 (Old->Ext2FsFreeBlockCount);
  New->Ext2FsFreeINodeCount     =   bswap32 (Old->Ext2FsFreeINodeCount);
  New->Ext2FsFirstDataBlock     =   bswap32 (Old->Ext2FsFirstDataBlock);
  New->Ext2FsLogBlockSize       =   bswap32 (Old->Ext2FsLogBlockSize);
  New->Ext2FsFragmentSize       =   bswap32 (Old->Ext2FsFragmentSize);
  New->Ext2FsBlocksPerGroup     =   bswap32 (Old->Ext2FsBlocksPerGroup);
  New->Ext2FsFragsPerGroup      =   bswap32 (Old->Ext2FsFragsPerGroup);
  New->Ext2FsINodesPerGroup     =   bswap32 (Old->Ext2FsINodesPerGroup);
  New->Ext2FsMountTime          =   bswap32 (Old->Ext2FsMountTime);
  New->Ext2FsWriteTime          =   bswap32 (Old->Ext2FsWriteTime);
  New->Ext2FsMountCount         =   bswap16 (Old->Ext2FsMountCount);
  New->Ext2FsMaxMountCount      =   bswap16 (Old->Ext2FsMaxMountCount);
  New->Ext2FsMagic              =   bswap16 (Old->Ext2FsMagic);
  New->Ext2FsState              =   bswap16 (Old->Ext2FsState);
  New->Ext2FsBehavior           =   bswap16 (Old->Ext2FsBehavior);
  New->Ext2FsMinorRev           =   bswap16 (Old->Ext2FsMinorRev);
  New->Ext2FsLastFsck           =   bswap32 (Old->Ext2FsLastFsck);
  New->Ext2FsFsckInterval       =   bswap32 (Old->Ext2FsFsckInterval);
  New->Ext2FsCreator            =   bswap32 (Old->Ext2FsCreator);
  New->Ext2FsRev                =   bswap32 (Old->Ext2FsRev);
  New->Ext2FsRsvdUid            =   bswap16 (Old->Ext2FsRsvdUid);
  New->Ext2FsRsvdGid            =   bswap16 (Old->Ext2FsRsvdGid);
  New->Ext2FsFirstInode         =   bswap32 (Old->Ext2FsFirstInode);
  New->Ext2FsInodeSize          =   bswap16 (Old->Ext2FsInodeSize);
  New->Ext2FsBlockGrpNum        =   bswap16 (Old->Ext2FsBlockGrpNum);
  New->Ext2FsFeaturesCompat     =   bswap32 (Old->Ext2FsFeaturesCompat);
  New->Ext2FsFeaturesIncompat   =   bswap32 (Old->Ext2FsFeaturesIncompat);
  New->Ext2FsFeaturesROCompat   =   bswap32 (Old->Ext2FsFeaturesROCompat);
  New->Ext2FsAlgorithm          =   bswap32 (Old->Ext2FsAlgorithm);
  New->Ext2FsRsvdGDBlock        =   bswap16 (Old->Ext2FsRsvdGDBlock);
}

/**
  Byte swap functions for big endian machines.
  (Ext2Fs is always little endian)
  XXX: We should use src/sys/ufs/Ext2Fs/ext2fs_bswap.c
  These functions are only needed if native byte order is not big endian

  @param Old     pointer to old filesystem field
  @param New     pointer to new filesystem field
  @param Size    Size to swap
**/
VOID
E2fsCGByteSwap (
  EXT2GD    *Old,
  EXT2GD    *New,
  INT32      Size
  )
{
  INT32 Index;

  for (Index = 0; Index < (Size / sizeof (EXT2GD)); Index++) {
    New[Index].Ext2BGDBlockBitmap  = bswap32 (Old[Index].Ext2BGDBlockBitmap);
    New[Index].Ext2BGDInodeBitmap  = bswap32 (Old[Index].Ext2BGDInodeBitmap);
    New[Index].Ext2BGDInodeTables  = bswap32 (Old[Index].Ext2BGDInodeTables);
    New[Index].Ext2BGDFreeBlocks   = bswap16 (Old[Index].Ext2BGDFreeBlocks);
    New[Index].Ext2BGDFreeInodes   = bswap16 (Old[Index].Ext2BGDFreeInodes);
    New[Index].Ext2BGDNumDir       = bswap16 (Old[Index].Ext2BGDNumDir);
  }
}

/**
  Byte swap functions for big endian machines.
  (Ext2Fs is always little endian)

  XXX: We should use src/sys/ufs/Ext2Fs/ext2fs_bswap.c

  These functions are only needed if native byte order is not big endian

  @param Old     pointer to old filesystem field
  @param New     pointer to new filesystem field
**/
VOID
E2fsIByteSwap (
  EXTFS_DINODE  *Old,
  EXTFS_DINODE  *New
  )
{

  New->Ext2DInodeMode             =   bswap16 (Old->Ext2DInodeMode);
  New->Ext2DInodeUid              =   bswap16 (Old->Ext2DInodeUid);
  New->Ext2DInodeGid              =   bswap16 (Old->Ext2DInodeGid);
  New->Ext2DInodeLinkcount        =   bswap16 (Old->Ext2DInodeLinkcount);
  New->Ext2DInodeSize             =   bswap32 (Old->Ext2DInodeSize);
  New->Ext2DInodeAcessTime        =   bswap32 (Old->Ext2DInodeAcessTime);
  New->Ext2DInodeCreatTime        =   bswap32 (Old->Ext2DInodeCreatTime);
  New->Ext2DInodeModificationTime =   bswap32 (Old->Ext2DInodeModificationTime);
  New->Ext2DInodeDeletionTime     =   bswap32 (Old->Ext2DInodeDeletionTime);
  New->Ext2DInodeBlockcount       =   bswap32 (Old->Ext2DInodeBlockcount);
  New->Ext2DInodeStatusFlags      =   bswap32 (Old->Ext2DInodeStatusFlags);
  New->Ext2DInodeGen              =   bswap32 (Old->Ext2DInodeGen);
  New->Ext2DInodeFileAcl          =   bswap32 (Old->Ext2DInodeFileAcl);
  New->Ext2DInodeDirAcl           =   bswap32 (Old->Ext2DInodeDirAcl);
  New->Ext2DInodeFragmentAddr     =   bswap32 (Old->Ext2DInodeFragmentAddr);

  CopyMem (&New->Ext2DInodeBlocks[0], &Old->Ext2DInodeBlocks[0],
           (NDADDR + NIADDR) * sizeof (UINT32));
}
#endif

#ifdef EXT2FS_DEBUG
/**
  Dump the file system super block info.

  @param FileSystem     pointer to filesystem.
**/
VOID
DumpSBlock (
  M_EXT2FS  *FileSystem
  )
{

  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsBlockCount = %u\n", FileSystem->Ext2Fs.Ext2FsBlockCount));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsFirstDataBlock = %u\n", FileSystem->Ext2Fs.Ext2FsFirstDataBlock));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsLogBlockSize = %u\n", FileSystem->Ext2Fs.Ext2FsLogBlockSize));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsBlocksPerGroup = %u\n", FileSystem->Ext2Fs.Ext2FsBlocksPerGroup));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsINodesPerGroup = %u\n", FileSystem->Ext2Fs.Ext2FsINodesPerGroup));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsMagic = 0x%x\n", FileSystem->Ext2Fs.Ext2FsMagic));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsRev = %u\n", FileSystem->Ext2Fs.Ext2FsRev));

  if (FileSystem->Ext2Fs.Ext2FsRev == E2FS_REV1) {
    DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsFirstInode = %u\n",
            FileSystem->Ext2Fs.Ext2FsFirstInode));
    DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsInodeSize = %u\n",
            FileSystem->Ext2Fs.Ext2FsInodeSize));
    DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsFeaturesCompat = %u\n",
            FileSystem->Ext2Fs.Ext2FsFeaturesCompat));
    DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsFeaturesIncompat = %u\n",
            FileSystem->Ext2Fs.Ext2FsFeaturesIncompat));
    DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsFeaturesROCompat = %u\n",
            FileSystem->Ext2Fs.Ext2FsFeaturesROCompat));
    DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2Fs.Ext2FsRsvdGDBlock = %u\n",
            FileSystem->Ext2Fs.Ext2FsRsvdGDBlock));
  }

  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2FsBlockSize = %u\n", FileSystem->Ext2FsBlockSize));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2FsFsbtobd = %u\n", FileSystem->Ext2FsFsbtobd));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2FsNumCylinder = %u\n", FileSystem->Ext2FsNumCylinder));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2FsNumGrpDesBlock = %u\n", FileSystem->Ext2FsNumGrpDesBlock));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2FsInodesPerBlock = %u\n", FileSystem->Ext2FsInodesPerBlock));
  DEBUG ((DEBUG_VERBOSE, "FileSystem->Ext2FsInodesTablePerGrp = %u\n", FileSystem->Ext2FsInodesTablePerGrp));
}
#endif

/**

  Determine the disk block(s) that contains data for FILE <File>,
  starting at Offset <FilePosition> and extending for up to <Len> bytes.
  @param File           Pointer to an file private data.
  @param FilePosition   offset address of the file position.
  @param Len            Number of bytes
  @param NBlocks        Number of consecutive blocks

  @retval  First disk block, number of consecutive blocks thru *<nblock>.
**/
INT32
Ext2fsDiskBlocks (
  OPEN_FILE    *File,
  UINT32        FilePosition,
  UINT32        Len,
  UINT32       *NBlocks
  )
{
  FILE      *Fp;
  M_EXT2FS  *FileSystem;
  UINT32    BlockSize;
  UINT32    First;
  UINT32    Num;
  INDPTR    FileBlock;
  INDPTR    DiskBlock;
  INT32     Rc;

  First = 0;
  Num   = 0;

  Fp         = (FILE *) File->FileSystemSpecificData;
  FileSystem = Fp->SuperBlockPtr;
  BlockSize  = FileSystem->Ext2FsBlockSize;

  *NBlocks = 0;
  while (Len >= BlockSize) {
    FileBlock = LBLKNO (FileSystem, FilePosition);
    DiskBlock = 0;

    Rc = BlockMap (File, FileBlock, &DiskBlock);
    if (Rc != 0) {
      if (Num == 0) {
        return 0;
      } else {
        break;
      }
    }

    if (Num == 0) {
      First = DiskBlock;
      Num  += 1;
    } else if (First + Num == (UINT32)DiskBlock) {
      Num += 1;
    } else {
      break;
    }

    FilePosition += BlockSize;
    Len -= BlockSize;
  }

  *NBlocks = Num;
  return FSBTODB (FileSystem, First);
}

/**
  Given an offset in a FILE, find the disk block number that
  contains that block.
  @param OpenFile       pointer to an Open file.
  @param Stat           Block to find the file.
  @param Name           Pointer to the disk which contains block.
  @param NamLen
  @retval 0 if success
  @retval other if error.
**/
INT32
Ext2fsLookUpFile (
  OPEN_FILE     *OpenFile,
  STAT          *Stat,
  CHAR8         *Name,
  UINT32         NamLen
  )
{
  FILE      *Fp;
  EXT2FS_DIRECT *Dp;
  CHAR8  *Buf;
  UINT32 BufSize;
  UINT32 Ext2DirectInodeNumber;
  INT32      Ext2DirectRecLen;
  UINT32 Ext2DirectNameLen;
  INT32 Rc;

  Fp = OpenFile->FileSystemSpecificData;

  do {
    if ((UINT32)Fp->SeekPtr >= Fp->DiskInode.Ext2DInodeSize) {
      return 0;
    }

    if ((Rc = BufReadFile (OpenFile, &Buf, &BufSize)) != 0) {
      return (Rc < 0) ? Rc : -Rc;
    }

    if (BufSize < sizeof (EXT2FS_DIRECT) - EXT2FS_MAXNAMLEN + 1) {
      return -1;  // XXX: corrupt entry.
    }
    Dp = (EXT2FS_DIRECT *) Buf;

    Ext2DirectInodeNumber   =   FS2H32 (Dp->Ext2DirectInodeNumber);
    Ext2DirectRecLen        =   FS2H16 (Dp->Ext2DirectRecLen);
    Ext2DirectNameLen       =   Dp->Ext2DirectNameLen;

    Dp->Ext2DirectName[Ext2DirectNameLen] = '\0';

    if (Ext2DirectRecLen <= 0) {
      return 0;
    }

    Fp->SeekPtr += Ext2DirectRecLen;
  } while (Ext2DirectInodeNumber == 0 || Ext2DirectNameLen == 0);

  CopyMem (Name, Dp->Ext2DirectName, MIN (NamLen, Ext2DirectNameLen));

  _FILE Tf;
  SetMem32 (&Tf, sizeof (Tf), 0);
  if ((Rc = Ext2fsOpen (Dp->Ext2DirectName, &Tf.Openfile)) != 0) {
    return (Rc < 0) ? Rc : -Rc;
  }

  Ext2fsStat (&Tf.Openfile, Stat);
  Ext2fsClose (&Tf.Openfile);

  return Ext2DirectNameLen;
}

#if defined(LIBSA_ENABLE_LS_OP)

/**
  CMD: edir -- FILE system tests / exercisers.
  @param ac   Number of arguments.
  @param av   argument vector.
  @param args argument string.
**/
STATIC
INT32
CmdEdir (
  INT32                 ac,
  CONST CHAR8          *av[],
  CONST cmd_args_t     *args
  )
{
  UINT32    OptInd;
  CONST CHAR8 *Path;
  FILE File;
  INT32 Error;

  OptInd = args->optind;
  Path   = (ac > OptInd) ? av[OptInd] : ".";

  if (ac - OptInd > 1) {
    return CMD_ERROR_ARGS;
  }

  if ((Error = fs_open (Path, &File, 0, 0)) != 0) {
    DEBUG ((DEBUG_INFO, "open '%s' failed (%d)\n", Path, Error));
    return -9;
  }
  Ext2fsLs (&File->Openfile, 0);
  if ((Error = fs_close (File)) != 0) {
    DEBUG ((DEBUG_INFO, "close '%s' failed (%d)\n", Path, Error));
    return -9;
  }

  return CMD_RESULT_OK;
}

CLI_COMMAND (edir, "List directory (EXT2-specific)", "",
             CmdEdir,
             0);

#endif
