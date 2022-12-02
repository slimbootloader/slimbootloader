/** @file
 Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent

 Copyright (c) 1999 Christopher G. Demetriou.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement:
      This product includes software developed by Christopher G. Demetriou
   for the NetBSD Project.
 4. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission

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

 Copyright (c) 1993
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

   @file stand.h     8.1 (Berkeley) 6/11/93

 libsa_fs_stand.h

 Definitions (file-system related) for/from the NetBSD libsa library.
**/

#ifndef _LIBSA_FS_STAND_H_
#define _LIBSA_FS_STAND_H_

#define LIBSA_SINGLE_DEVICE    bdev

enum seek {
  SEEK_SET = 0,
  SEEK_CUR,
  SEEK_END
};

//
// File information (stats); returned by the fs_readdir() function:
//
typedef struct {
  UINT32 StatMode;   // file type and protection (access permissions)
  UINT32
  StatSize;          // file size (in  bytes)
} STAT;

//
// <sys/types.h>
//
typedef long int DADDRESS;
typedef long int OFFSET;
typedef unsigned long ULONG;
typedef unsigned long INODE;

typedef struct {
  INT32       FileFlags;                      // see F_* below
#if !defined(LIBSA_SINGLE_DEVICE)
  DEVSW       *DevPtr;                        // pointer to device operations
#endif
  VOID        *FileDevData;                   // device specific data
  VOID        *FileSystemSpecificData;        // file system specific data
#if !defined(LIBSA_NO_RAW_ACCESS)
  OFFSET      FileOffset;                     // current file offset (F_RAW)
#endif
  CHAR8       FileNameBuf[EXT2FS_MAXNAMLEN];  // buffer to store file name (not always the actual name, e.g., symbolic link)
} OPEN_FILE;

#if ! defined(LIBSA_SINGLE_DEVICE)

typedef const struct {
  CHAR8    *Devname;
  INT32 (*DevStrategy) (VOID *, INT32, DADDRESS, UINT32, VOID *, UINT32 *);
  INT32 (*DevOpen) (OPEN_FILE *, ...);
  INT32 (*DevClose) (OPEN_FILE *);
  INT32 (*DevIoctl) (OPEN_FILE *, ULONG, VOID *);
} DEVSW;

#define DEV_NAME(d)         ((d)->Devname)
#define DEV_STRATEGY(d)     ((d)->DevStrategy)

#else

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
RETURN_STATUS
EFIAPI
BDevStrategy (
  VOID     *DevData,
  INT32     ReadWrite,
  DADDRESS  BlockNum,
  UINT32    Size,
  VOID     *Buf,
  UINT32   *RSize
  );

#endif

//
// f_flags values
//
#define F_READ  0x0001    // file opened for reading
#define F_WRITE 0x0002    // file opened for writing
#define F_RAW   0x0004    // raw device open - no file system
#define F_NODEV 0x0008    // network open - no device
//
// dosfs_stat() needs them
//
#define S_IFMT   00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#endif
