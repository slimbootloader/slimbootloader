/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  Copyright (c) 1982, 1986, 1989, 1993
  The Regents of the University of California.  All rights reserved.
  (c) UNIX System Laboratories, Inc.
  All or some portions of this file are derived from material licensed
  to the University of California by American Telephone and Telegraph
  Co. or Unix System Laboratories, Inc. and are reproduced herein with
  the permission of UNIX System Laboratories, Inc.

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

   @file dinode.h    8.6 (Berkeley) 9/13/94
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

   @file dinode.h    8.6 (Berkeley) 9/13/94
   Modified for ext2fs by Manuel Bouyer.
 **/

#ifndef _UFS_EXT2FS_EXT2FS_DINODE_H_
#define _UFS_EXT2FS_EXT2FS_DINODE_H_

/**
  The root inode is the root of the file system.  Inode 0 can't be used for
  normal purposes and bad blocks are normally linked to inode 1, thus
  the root inode is 2.
  Inode 3 to 10 are reserved in ext2fs.
**/
#define    EXT2_BADBLKINO      ((INODE)1)
#define    EXT2_ROOTINO        ((INODE)2)
#define    EXT2_ACLIDXINO      ((INODE)3)
#define    EXT2_ACLDATAINO     ((INODE)4)
#define    EXT2_BOOTLOADERINO  ((INODE)5)
#define    EXT2_UNDELDIRINO    ((INODE)6)
#define    EXT2_RESIZEINO      ((INODE)7)
#define    EXT2_JOURNALINO     ((INODE)8)
#define    EXT2_FIRSTINO       ((INODE)11)

/**
  A dinode contains all the meta-data associated with a UFS file.
  This structure defines the on-disk format of a dinode. Since
  this structure describes an on-disk structure, all its fields
  are defined by types with precise widths.
**/

#define NDADDR  12          // Direct addresses in inode.
#define NIADDR  3           // Indirect addresses in inode.

#define EXT2_MAXSYMLINKLEN ((NDADDR+NIADDR) * sizeof (UINT32))

typedef struct {
  UINT16    Ext2DInodeMode;                     //   0: IFMT, permissions; see below.
  UINT16    Ext2DInodeUid;                      //   2: Owner UID
  UINT32    Ext2DInodeSize;                     //   4: Size (in bytes)
  UINT32    Ext2DInodeAcessTime;                //   8: Acces time
  UINT32    Ext2DInodeCreatTime;                //  12: Create time
  UINT32    Ext2DInodeModificationTime;         //  16: Modification time
  UINT32    Ext2DInodeDeletionTime;             //  20: Deletion time
  UINT16    Ext2DInodeGid;                      //  24: Owner GID
  UINT16    Ext2DInodeLinkcount;                //  26: File link count
  UINT32    Ext2DInodeBlockcount;               //  28: Blocks count
  UINT32    Ext2DInodeStatusFlags;              //  32: Status flags (chflags)
  UINT32    Ext2DInodeLinuxRsvd1;               // 36
  UINT32    Ext2DInodeBlocks[NDADDR + NIADDR];  // 40: disk blocks
  UINT32    Ext2DInodeGen;                      // 100: generation number
  UINT32    Ext2DInodeFileAcl;                  // 104: file ACL (not implemented)
  UINT32    Ext2DInodeDirAcl;                   // 108: dir ACL (not implemented)
  UINT32    Ext2DInodeFragmentAddr;             // 112: fragment address
  UINT8     Ext2DInodeFragmentNum;              // 116: fragment number
  UINT8     Ext2DInodeFragmentSize;             // 117: fragment size
  UINT16    Ext2DInodeLinuxRsvd2;               // 118
  UINT16    Ext2DInodeUidHigh;                  // 120: Owner UID top 16 bits
  UINT16    Ext2DInodeGidHigh;                  // 122: Owner GID top 16 bits
  UINT32    Ext2DInodeLinuxRsvd3;               // 124
} EXTFS_DINODE;

#define EXT4_MAX_HEADER_EXTENT_ENTRIES  4
#define EXT4_EXTENT_HEADER_MAGIC        0xF30A

typedef struct {
  UINT16    EhMagic;      // magic number: 0xF30A
  UINT16    EhEntries;    // number of valid entries
  UINT16    EhMax;        // capacity of store in entries
  UINT16    EhDepth;      // the depth of extent tree
  UINT32    EhGen;        // generation of extent tree
} EXT4_EXTENT_HEADER;

typedef struct {
  UINT32    EiBlk;        // indexes logical blocks
  UINT32    EiLeafLo;     // points to physical block of the next level
  UINT16    EiLeafHi;     // high 16 bits of physical block
  UINT16    EiUnused;
} EXT4_EXTENT_INDEX;

typedef struct {
  UINT32    Eblk;         // first logical block
  UINT16    Elen;         // number of blocks
  UINT16    EstartHi;     // high 16 bits of physical block
  UINT32    EstartLo;     // low 32 bits of physical block
} EXT4_EXTENT;

typedef struct {
  EXT4_EXTENT_HEADER Eheader;
  union {
    EXT4_EXTENT_INDEX  Eindex[EXT4_MAX_HEADER_EXTENT_ENTRIES];
    EXT4_EXTENT        Extent[EXT4_MAX_HEADER_EXTENT_ENTRIES];
  } Enodes;
} EXT4_EXTENT_TABLE;

#define    E2MAXSYMLINKLEN    ((NDADDR + NIADDR) * sizeof(UINT32))
//
// File permissions.
//
#define    EXT2_IEXEC      0000100        // Executable.
#define    EXT2_IWRITE     0000200        // Writable.
#define    EXT2_IREAD      0000400        // Readable.
#define    EXT2_ISVTX      0001000        // Sticky bit.
#define    EXT2_ISGID      0002000        // Set-gid.
#define    EXT2_ISUID      0004000        // Set-uid.
//
// File types.
//
#define    EXT2_IFMT       0170000     // Mask of file type.
#define    EXT2_IFIFO      0010000     // Named pipe (fifo).
#define    EXT2_IFCHR      0020000     // Character device.
#define    EXT2_IFDIR      0040000     // Directory file.
#define    EXT2_IFBLK      0060000     // Block device.
#define    EXT2_IFREG      0100000     // Regular file.
#define    EXT2_IFLNK      0120000     // Symbolic link.
#define    EXT2_IFSOCK     0140000     // UNIX domain socket.
//
// file flags
//
#define EXT2_SECRM      0x00000001      // Secure deletion
#define EXT2_UNRM       0x00000002      // Undelete
#define EXT2_COMPR      0x00000004      // Compress file
#define EXT2_SYNC       0x00000008      // Synchronous updates
#define EXT2_IMMUTABLE  0x00000010      // Immutable file
#define EXT2_APPEND     0x00000020      // writes to file may only append
#define EXT2_NODUMP     0x00000040      // do not dump file
#define EXT4_EXTENTS    0x00080000      // Inode uses extents

//
// Size of on-disk inode.
//
#define EXT2_REV0_DINODE_SIZE   sizeof(EXTFS_DINODE)
#define EXT2_DINODE_SIZE(fs)    ((fs)->Ext2Fs.Ext2FsRev > E2FS_REV0 ?    \
                    (fs)->Ext2Fs.Ext2FsInodeSize :    \
                    EXT2_REV0_DINODE_SIZE)

/**
 * The Ext2DInodeBlocks fields may be overlaid with other information for
 * file types that do not have associated disk storage. Block
 * and character devices overlay the first data block with their
 * dev_t value. Short symbolic links place their path in the
 * di_db area.
**/

//
// Ext2Fs needs byte swapping on big-endian systems
//
#define E2FSILOAD(OLD, NEW)    \
        CopyMem((NEW),(OLD),sizeof(EXTFS_DINODE))
#define E2FSISAVE(OLD, NEW)    \
        CopyMem((NEW),(OLD),sizeof(EXTFS_DINODE))


#endif // !_UFS_EXT2FS_EXT2FS_DINODE_H_
