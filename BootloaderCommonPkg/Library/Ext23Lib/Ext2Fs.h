/** @file
  Copyright (c) 1982, 1986, 1989, 1993
  The Regents of the University of California.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the name of the University nor the names of its contributors
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

   @file fs.h 8.10 (Berkeley) 10/27/94
   Modified for ext2fs by Manuel Bouyer.

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

   @file fs.h 8.10 (Berkeley) 10/27/94
   Modified for ext2fs by Manuel Bouyer.
**/

#ifndef _UFS_EXT2FS_EXT2FS_H_
#define _UFS_EXT2FS_EXT2FS_H_

#include <PiPei.h>
#include <Ppi/BlockIo.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PartitionLib.h>
#include "Ext2FsDiNode.h"
#include "Ext2FsDir.h"
#include "LibsaFsStand.h"
/**
  Each disk drive contains some number of file systems.
  A file system consists of a number of cylinder groups.
  Each cylinder group has inodes and data.

  A file system is described by its super-block, which in turn
  describes the cylinder groups.  The super-block is critical
  data and is replicated in each cylinder group to protect against
  catastrophic loss.  This is done at `newfs' time and the critical
  super-block data does not change, so the copies need not be
  referenced further unless disaster strikes.

  The first boot and super blocks are given in absolute disk addresses.
  The byte-offset forms are preferred, as they don't imply a sector size.
**/
#define BBSIZE      1024
#define SBSIZE      1024
#define BBOFF       ((OFFSET)(0))
#define SBOFF       ((OFFSET)(BBOFF + BBSIZE))
#define BBLOCK      ((DADDRESS)(0))
#define SBLOCK      ((DADDRESS)(BBLOCK + BBSIZE / DEV_BSIZE))

#define BDEV_BLOCKSIZE    512

#undef  EXT2FS_DEBUG

#define LIBSA_FS_SINGLECOMPONENT
#define LIBSA_FS_SINGLE_DEVICE
#define LIBSA_FS_SINGLE_FILESYSTEM
#define LIBSA_NO_FS_SYMLINK
#define LIBSA_NO_TWIDDLE
#undef  LIBSA_ENABLE_LS_OP
#define LIBSA_NO_FS_WRITE

//
// An (open) file:
//
typedef struct {
  OPEN_FILE Openfile;
} _FILE;

/**
  Addresses stored in inodes are capable of addressing blocks
  XXX

  MINBSIZE is the smallest allowable block size.
  MINBSIZE must be big enough to hold a cylinder group block,
  thus changes to (struct cg) must keep its size within MINBSIZE.
  Note that super blocks are always of size SBSIZE,
  and that both SBSIZE and MAXBSIZE must be >= MINBSIZE.
**/
#define LOG_MINBSIZE    10
#define MINBSIZE        (1 << LOG_MINBSIZE)

/**
  The path name on which the file system is mounted is maintained
  in fs_fsmnt. MAXMNTLEN defines the amount of space allocated in
  the super block for this name.
**/
#define MAXMNTLEN    512

/**
  MINFREE gives the minimum acceptable percentage of file system
  blocks which may be free. If the freelist drops below this level
  only the superuser may continue to allocate blocks. This may
  be set to 0 if no reserve of free blocks is deemed necessary,
  however throughput drops by fifty percent if the file system
  is run at between 95% and 100% full; thus the minimum default
  value of fs_minfree is 5%. However, to get good clustering
  performance, 10% is a better choice. hence we use 10% as our
  default value. With 10% free space, fragmentation is not a
  problem, so we choose to optimize for time.
**/
#define MINFREE    5

//
// Super block for an ext2fs file system.
//
typedef struct {
  UINT32  Ext2FsINodeCount;         // Inode count
  UINT32  Ext2FsBlockCount;         // blocks count
  UINT32  Ext2FsRsvdBlockCount;     // reserved blocks count
  UINT32  Ext2FsFreeBlockCount;     // free blocks count
  UINT32  Ext2FsFreeINodeCount;     // free inodes count
  UINT32  Ext2FsFirstDataBlock;     // first data block
  UINT32  Ext2FsLogBlockSize;       // block size = 1024*(2^Ext2FsLogBlockSize)
  UINT32  Ext2FsFragmentSize;       // fragment size
  UINT32  Ext2FsBlocksPerGroup;     // blocks per group
  UINT32  Ext2FsFragsPerGroup;      // frags per group
  UINT32  Ext2FsINodesPerGroup;     // inodes per group
  UINT32  Ext2FsMountTime;          // mount time
  UINT32  Ext2FsWriteTime;          // write time
  UINT16  Ext2FsMountCount;         // mount count
  UINT16  Ext2FsMaxMountCount;      // max mount count
  UINT16  Ext2FsMagic;              // magic number
  UINT16  Ext2FsState;              // file system state
  UINT16  Ext2FsBehavior;           // behavior on errors
  UINT16  Ext2FsMinorRev;           // minor revision level
  UINT32  Ext2FsLastFsck;           // time of last fsck
  UINT32  Ext2FsFsckInterval;       // max time between fscks
  UINT32  Ext2FsCreator;            // creator OS
  UINT32  Ext2FsRev;                // revision level
  UINT16  Ext2FsRsvdUid;            // default uid for reserved blocks
  UINT16  Ext2FsRsvdGid;            // default gid for reserved blocks
  //
  // EXT2_DYNAMIC_REV superblocks
  //
  UINT32  Ext2FsFirstInode;         /* first non-reserved inode */
  UINT16  Ext2FsInodeSize;          /* size of inode structure */
  UINT16  Ext2FsBlockGrpNum;        /* block grp number of this sblk*/
  UINT32  Ext2FsFeaturesCompat;     /*  compatible feature set */
  UINT32  Ext2FsFeaturesIncompat;   /* incompatible feature set */
  UINT32  Ext2FsFeaturesROCompat;   /* RO-compatible feature set */
  UINT8   Ext2FsUuid[16];           /* 128-bit uuid for volume */
  CHAR8   Ext2FsVolumeName[16];     /* volume name */
  CHAR8   Ext2FsFSMnt[64];          /* name mounted on */
  UINT32  Ext2FsAlgorithm;          /* For compression */
  UINT8   Ext2FsPreAlloc;           /* # of blocks to preallocate */
  UINT8   Ext2FsDirPreAlloc;        /* # of blocks to preallocate for dir */
  UINT16  Ext2FsRsvdGDBlock;        /* # of reserved gd blocks for resize */
  UINT32  Rsvd2[204];
} EXT2FS;

//
// Ext2 file system block group descriptor
//
typedef struct {
  UINT32 Ext2BGDBlockBitmap;    /* blocks bitmap block */
  UINT32 Ext2BGDInodeBitmap;    /* inodes bitmap block */
  UINT32 Ext2BGDInodeTables;    /* inodes table block  */
  UINT16 Ext2BGDFreeBlocks;     /* number of free blocks */
  UINT16 Ext2BGDFreeInodes;     /* number of free inodes */
  UINT16 Ext2BGDNumDir;         /* number of directories */
  UINT16 Rsvd;
  UINT32 Rsvd2[3];
} EXT2GD;

//
// In-memory data for ext2fs
//
typedef struct {
  EXT2FS   Ext2Fs;
  UINT8    Ext2FsFSMnt[MAXMNTLEN];   // name mounted on
  CHAR8    Ext2FsReadOnly;           // mounted read-only flag
  CHAR8    Ext2FsModified;           // super block modified flag
  INT32    Ext2FsBlockSize;          // block size
  INT32    Ext2FsLogicalBlock;       // ``lblkno'' calc of logical blkno
  INT32    Ext2FsBlockOffset;        // ``blkoff'' calc of blk offsets
  INT64    Ext2FsQuadBlockOffset;    // ~fs_bmask - for use with quad size
  INT32    Ext2FsFsbtobd;            // FSBTODB and DBTOFSB shift constant
  INT32    Ext2FsNumCylinder;        // number of cylinder groups
  INT32    Ext2FsNumGrpDesBlock;     // number of group descriptor block
  INT32    Ext2FsInodesPerBlock;     // number of inodes per block
  INT32    Ext2FsInodesTablePerGrp;  // number of inode table per group
  EXT2GD  *Ext2FsGrpDes;             // group descripors
} M_EXT2FS;



//
//  Filesystem identification
//
#define E2FS_MAGIC   0xef53 /* the ext2fs magic number */
#define E2FS_REV0    0      /* GOOD_OLD revision */
#define E2FS_REV1    1      /* Support compat/incompat features */
//
// Compatible/incompatible features
//
#define EXT2F_COMPAT_PREALLOC       0x0001
#define EXT2F_COMPAT_HASJOURNAL     0x0004
#define EXT2F_COMPAT_RESIZE         0x0010

#define EXT2F_ROCOMPAT_SPARSESUPER  0x0001
#define EXT2F_ROCOMPAT_LARGEFILE    0x0002
#define EXT2F_ROCOMPAT_BTREE_DIR    0x0004

#define EXT2F_INCOMPAT_COMP         0x0001
#define EXT2F_INCOMPAT_FTYPE        0x0002

/**
  Features supported in this implementation

  We support the following REV1 features:
  - EXT2F_ROCOMPAT_SPARSESUPER
     superblock backups stored only in cg_has_sb(bno) groups
  - EXT2F_ROCOMPAT_LARGEFILE
     use e2di_dacl in EXTFS_DINODE to store
     upper 32bit of size for >2GB files
  - EXT2F_INCOMPAT_FTYPE
     store file type to e2d_type in EXT2FS_direct
     (on REV0 e2d_namlen is UINT16 and no e2d_type, like ffs)
**/
#define EXT2F_COMPAT_SUPP        0x0000
#define EXT2F_ROCOMPAT_SUPP      (EXT2F_ROCOMPAT_SPARSESUPER \
                                 | EXT2F_ROCOMPAT_LARGEFILE)
#define EXT2F_INCOMPAT_SUPP      EXT2F_INCOMPAT_FTYPE

//
//  Definitions of behavior on errors
//
#define E2FS_BEH_CONTINUE   1   /* continue operation */
#define E2FS_BEH_READONLY   2   /* remount fs read only */
#define E2FS_BEH_PANIC      3   /* cause panic */
#define E2FS_BEH_DEFAULT    E2FS_BEH_CONTINUE

//
//  OS identification
//
#define E2FS_OS_LINUX   0
#define E2FS_OS_HURD    1
#define E2FS_OS_MASIX   2
#define E2FS_OS_FREEBSD 3
#define E2FS_OS_LITES   4

//
//  Filesystem clean flags
//
#define E2FS_ISCLEAN 0x01
#define E2FS_ERRORS  0x02

/**
  The cache size (IND_CACHE_SZ) must be smaller or equal the number of pointers
  in the indirect blocks: NINDIR = Ext2FsBlockSize / sizeof(UINT32).
  For ext2fs minimal block size is 1kB and block pointer size is 4 (UINT32)
  so LN2_IND_CACHE_SZ <= 8 (cache size IND_CACHE_SZ=2^8=256)
  Optimal for file system speed is the biggest cache size possible.
**/
#define LN2_IND_CACHE_SZ    8
#define IND_CACHE_SZ        (1 << LN2_IND_CACHE_SZ)
#define IND_CACHE_MASK      (IND_CACHE_SZ - 1)

#define INDPTR      INT32

typedef UINT32 INODE32;

//
//  In-core open file.
//
typedef struct {
  OFFSET            SeekPtr;                  // seek pointer
  M_EXT2FS          *SuperBlockPtr;           // pointer to super-block
  EXTFS_DINODE      DiskInode;                // copy of on-disk inode
  UINT32            NiShift;                  // for blocks in indirect block
  INDPTR            InodeCacheBlock;
  INDPTR            InodeCache[IND_CACHE_SZ];
  CHAR8             *Buffer;                  // buffer for data block
  UINT32            BufferSize;               // size of data block
  DADDRESS          BufferBlockNum;           // block number of data block
} FILE;


/**
  EXT2FS metadatas are stored in little-endian byte order. These macros
  helps reading theses metadatas
**/

#if BYTE_ORDER == LITTLE_ENDIAN
#define H2FS16(x) (x)
#define H2FS32(x) (x)
#define H2FS64(x) (x)
#define FS2H16(x) (x)
#define FS2H32(x) (x)
#define FS2H64(x) (x)
#define E2FS_SBLOAD(old, new) CopyMem((new), (old), SBSIZE);
#define E2FS_CGLOAD(old, new, size) CopyMem((new), (old), (size));
#define E2FS_SBSAVE(old, new) CopyMem((new), (old), SBSIZE);
#define E2FS_CGSAVE(old, new, size) CopyMem((new), (old), (size));
#else
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
  EXT2FS *Old,
  EXT2FS *New
  );

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
  EXT2GD *Old,
  EXT2GD *New,
  INT32 Size
  );

#define H2FS16(x) bswap16(x)
#define H2FS32(x) bswap32(x)
#define H2FS64(x) bswap64(x)
#define FS2H16(x) bswap16(x)
#define FS2H32(x) bswap32(x)
#define FS2H64(x) bswap64(x)
#define E2FS_SBLOAD(old, new) E2fsSBByteSwap((old), (new))
#define E2FS_CGLOAD(old, new, size) E2fsCGByteSwap((old), (new), (size));
#define E2FS_SBSAVE(old, new) E2fsSBByteSwap((old), (new))
#define E2FS_CGSAVE(old, new, size) E2fsCGByteSwap((old), (new), (size));
#endif

/**
  Turn file system block numbers into disk block addresses.
  This maps file system blocks to device size blocks.
**/
#define FSBTODB(fs, b)    ((b) << (fs)->Ext2FsFsbtobd)
#define DBTOFSB(fs, b)    ((b) >> (fs)->Ext2FsFsbtobd)

/**
  Macros for handling inode numbers:
      inode number to file system block offset.
      inode number to cylinder group number.
      inode number to file system block address.
**/
#define INOTOCG(fs, x)  (((x) - 1) / (fs)->Ext2Fs.Ext2FsINodesPerGroup)
#define INODETOFSBA(fs, x)  \
    ((fs)->Ext2FsGrpDes[INOTOCG((fs), (x))].Ext2BGDInodeTables + \
    (((x) - 1) % (fs)->Ext2Fs.Ext2FsINodesPerGroup) / (fs)->Ext2FsInodesPerBlock)
#define INODETOFSBO(fs, x)  (((x) - 1) % (fs)->Ext2FsInodesPerBlock)

/**
  Give cylinder group number for a file system block.
  Give cylinder group block number for a file system block.
**/
#define DTOG(fs, d) (((d) - (fs)->Ext2Fs.Ext2FsFirstDataBlock) / (fs)->Ext2Fs.Ext2FsFragsPerGroup)
#define DTOGD(fs, d) \
    (((d) - (fs)->Ext2Fs.Ext2FsFirstDataBlock) % (fs)->Ext2Fs.Ext2FsFragsPerGroup)

/**
  The following macros optimize certain frequently calculated
  quantities by using shifts and masks in place of divisions
  modulos and multiplications.
**/
#define BLOCKOFFSET(fs, loc)     /* calculates (loc % fs->Ext2FsBlockSize) */ \
    ((loc) & (fs)->Ext2FsQuadBlockOffset)
#define LBLKTOSIZE(fs, blk)      /* calculates (blk * fs->Ext2FsBlockSize) */ \
    ((blk) << (fs)->Ext2FsLogicalBlock)
#define LBLKNO(fs, loc)          /* calculates (loc / fs->Ext2FsBlockSize) */ \
    ((loc) >> (fs)->Ext2FsLogicalBlock)
#define BLKROUNDUP(fs, size)     /* calculates roundup(size, fs->Ext2FsBlockSize) */ \
    (((size) + (fs)->Ext2FsQuadBlockOffset) & (fs)->Ext2FsBlockOffset)
#define FRAGROUNDUP(fs, size)    /* calculates roundup(size, fs->Ext2FsBlockSize) */ \
    (((size) + (fs)->Ext2FsQuadBlockOffset) & (fs)->Ext2FsBlockOffset)
//
//  Determine the number of available frags given a
//  percentage to hold in reserve.
//
#define FREESPACE(fs) \
   ((fs)->Ext2Fs.Ext2FsFreeBlockCount - (fs)->Ext2Fs.Ext2FsRsvdBlockCount)

//
//  Number of indirects in a file system block.
//
#define NINDIR(fs) ((fs)->Ext2FsBlockSize / sizeof(UINT32))

typedef
EFI_STATUS
( *MEDIA_READ_BLOCKS) (
  IN  UINTN         DeviceIndex,
  IN  EFI_PEI_LBA   StartLBA,
  IN  UINTN         BufferSize,
  OUT VOID         *Buffer
  );

typedef struct {
  UINTN                Signature;
  UINT64               StartBlock;
  UINT64               LastBlock;
  UINT32               BlockSize;
  UINT8                PhysicalDevNo;
} PEI_EXT_PRIVATE_DATA;


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
  );
#define    DEV_STRATEGY(d)    BDevStrategy

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
  );

/**
  Close the file.
  @param File  File to be closed.

  @retval 0 if Group descriptor read is success
**/
INT32
Ext2fsClose (
  OPEN_FILE     *File
  );


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
  );

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
  );

#if defined(LIBSA_ENABLE_LS_OP)
/**
  Update the mode and size from descriptor to stat Block.
  contains that block.
  @param File       pointer to an file private data
  @param Pattern    pointer to Pattern
**/
VOID
Ext2fsLs (
  OPEN_FILE *File,
  const CHAR8 *Pattern
  );
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
  );

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
  );

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
  );


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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  Gets the size of the file from descriptor.

  @param File  File to be closed.
  @retval size of the file from descriptor.
**/
UINT32
Ext2fsFileSize (
  OPEN_FILE     *File
  );

#ifdef EXT2FS_DEBUG
/**
  Dump the file system super block info.

  @param FileSystem     pointer to filesystem.
**/
VOID
DumpSBlock (
  M_EXT2FS  *FileSystem
  );
#endif

//
// FILE operation structure - to be passed to generic FileSystem core
//
const struct fs_ops ext2fs_ops = {
  .open  = Ext2fsOpen,
  .close = Ext2fsClose,
  .read  = Ext2fsRead,
  .stat  = Ext2fsStat,
#if defined(LIBSA_ENABLE_LS_OP)
  .ls    = Ext2fsLs,
#endif
  .disk_blocks = Ext2fsDiskBlocks,
  .lookup_file = Ext2fsLookUpFile,
  .seek  = Ext2fsSeek,
};

#endif  // !_UFS_EXT2FS_EXT2FS_H_
