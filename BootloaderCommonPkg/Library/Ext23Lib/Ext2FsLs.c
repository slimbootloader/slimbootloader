/** @file

  Copyright (c) 2019 - 2022, Intel Corporation. All rights reserved.<BR>
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
#include <Library/ConsoleOutLib.h>
#include "Ext2Fs.h"
#include "LibsaFsStand.h"

typedef struct ENTRY ENTRY;
struct ENTRY {
  ENTRY    *EntryNext;
  INODE32  EntryInode;
  UINT8    EntryType;
  CHAR8    EntryName[EXT2FS_MAXNAMLEN];
};

STATIC CONST CHAR8 *CONST mTypeStr[] = { "unknown", "REG",  "DIR",  "CHR",  "BLK",  "FIFO",  "SOCK",  "LNK" };

/**
  List directories or files and print them

  @param[in] File           pointer to an file private data
  @param[in] Pattern        not used for now

  @retval EFI_SUCCESS       list directories or files successfully
  @retval EFI_NOT_FOUND     not found specified dir or file
  @retval EFI_DEVICE_ERROR  an error while accessing filesystem
**/
EFI_STATUS
EFIAPI
Ext2fsLs (
  IN  OPEN_FILE         *File,
  IN  CONST CHAR8       *Pattern
  )
{
  EFI_STATUS      Status;
  FILE           *Fp;
  UINT32          BlockSize;
  CHAR8          *Buf;
  UINT32          BufSize;
  ENTRY          *Names;
  ENTRY          *New;
  ENTRY         **NextPtr;
  EXT2FS_DIRECT  *Dp;
  EXT2FS_DIRECT  *EdPtr;
  UINT32          FileSize;
  ENTRY          *PNames;
  CONST CHAR8    *Type;

  Status = EFI_SUCCESS;
  Fp = (FILE *)File->FileSystemSpecificData;

  if ((Fp->DiskInode.Ext2DInodeMode & EXT2_IFMT) == EXT2_IFREG) {
    CONSOLE_PRINT_UNICODE ((L"  %-16a %u\n", File->FileNameBuf, Fp->DiskInode.Ext2DInodeSize));
    return EFI_SUCCESS;
  } else if ((Fp->DiskInode.Ext2DInodeMode & EXT2_IFMT) != EXT2_IFDIR) {
    return EFI_NOT_FOUND;
  }

  BlockSize = Fp->SuperBlockPtr->Ext2FsBlockSize;
  Names = NULL;
  Fp->SeekPtr = 0;
  while (Fp->SeekPtr < (OFFSET)Fp->DiskInode.Ext2DInodeSize) {
    Status = BufReadFile (File, &Buf, &BufSize);
    if (RETURN_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto out;
    }
    if ((BufSize != BlockSize) || (BufSize == 0)) {
      Status = EFI_DEVICE_ERROR;
      goto out;
    }

    Dp = (EXT2FS_DIRECT *)Buf;
    EdPtr = (EXT2FS_DIRECT *)(Buf + BufSize);

    for (; Dp < EdPtr;  Dp = (VOID *) ((CHAR8 *)Dp + Dp->Ext2DirectRecLen)) {
      if (Dp->Ext2DirectRecLen <= 0) {
        Status = EFI_DEVICE_ERROR;
        goto out;
      }

      if (Dp->Ext2DirectInodeNumber == 0) {
        continue;
      }

      if (Dp->Ext2DirectType >= ARRAY_SIZE (mTypeStr)) {
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
        DEBUG ((DEBUG_INFO, "bad dir entry - %d\n", Dp->Ext2DirectType));
        Status = EFI_DEVICE_ERROR;
        goto out;
      }
      Type = mTypeStr[Dp->Ext2DirectType];

      New = AllocateZeroPool (sizeof * New + AsciiStrLen (Dp->Ext2DirectName));
      if (New == NULL) {
        DEBUG ((DEBUG_INFO, "%d: %s (%s)\n",
                Dp->Ext2DirectInodeNumber, Dp->Ext2DirectName, Type));
        continue;
      }

      New->EntryInode = Dp->Ext2DirectInodeNumber;
      New->EntryType  = Dp->Ext2DirectType;
      AsciiStrCpyS (New->EntryName, EXT2FS_MAXNAMLEN, Dp->Ext2DirectName);
      New->EntryName[Dp->Ext2DirectNameLen] = '\0';
      for (NextPtr = &Names; *NextPtr != NULL; NextPtr = & (*NextPtr)->EntryNext) {
          if (AsciiStrCmp (New->EntryName, (*NextPtr)->EntryName) < 0) {
          break;
        }
      }
      New->EntryNext = *NextPtr;
      *NextPtr = New;
    }
    Fp->SeekPtr += BufSize;
  }

  if (Names != NULL) {
    PNames = Names;

    do {
      New = PNames;
      if (New->EntryType == 2) {
        CONSOLE_PRINT_UNICODE ((L"  %a/\n", New->EntryName));
      } else if (New->EntryType == 1) {
        FileSize = 0;
        Status = ReadInode (New->EntryInode, File);
        if (!RETURN_ERROR (Status)) {
          Fp = (FILE *)File->FileSystemSpecificData;
          FileSize = Fp->DiskInode.Ext2DInodeSize;
        }
        Status = RETURN_SUCCESS;
        CONSOLE_PRINT_UNICODE ((L"  %-16a %u\n", New->EntryName, FileSize));
      } else if (New->EntryType == 7) {
        CONSOLE_PRINT_UNICODE ((L"  %a (symbolic link)\n", New->EntryName));
      }
      PNames = New->EntryNext;
    } while (PNames != NULL);
  }
out:
  if (Names != NULL) {
    do {
      New   = Names;
      Names = New->EntryNext;
      FreePool (New);
    } while (Names != NULL);
  }

  return Status;
}
