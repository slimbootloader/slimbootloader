/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/*-
 * Copyright (c) 1998 Michael Smith <msmith@freebsd.org>
 * Copyright (c) 1998 Peter Wemm <peter@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ElfLib.h>
#include "Elf32.h"

#define IS_BOOTABLE_I386_ELF(h) \
 ( IS_ELF (h)                             && \
  (h->E_Ident[EI_CLASS]   == ELFCLASS32)  && \
  (h->E_Ident[EI_DATA]    == ELFDATA2LSB) && \
  (h->E_Ident[EI_VERSION] == EV_CURRENT)  && \
 ((h->E_Type              == ET_EXEC)     || (h->E_Type == ET_DYN)) && \
  (h->E_Machine           == EM_386)      && \
  (h->E_Version           == EV_CURRENT))

/**
  load ELF image symbol table

  @param[in]      ImageBase   Memory address of ELF image
  @param[in]      Ehdr        ELF file header
  @param[in,out]  Eaddr       Start address for ELF symbols

  @retval         End address of loaded sysbols table
**/
UINT32
LoadElfSymtab (
  IN  VOID   *ImageBase,
  IN  UINT32  Eaddr
  );

/**
  Check if the image is a bootable ELF image.

  @param[in]  ImageAddress      Memory address of an image

  @retval     TRUE              Image is a bootable ELF image
  @retval     FALSE             Not a bootable ELF image
**/
BOOLEAN
EFIAPI
IsElfImage (
  IN  VOID  *ImageBase
  )
{
  ELF32_EHDR *Ehdr;

  Ehdr = (ELF32_EHDR *) ImageBase;

  return (BOOLEAN) IS_BOOTABLE_I386_ELF (Ehdr);
}

/**
  load ELF image symbol table

  @param[in]      ImageBase   Memory address of ELF image
  @param[in]      Ehdr        ELF file header
  @param[in,out]  Eaddr       Start address for ELF symbols

  @retval         End address of loaded sysbols table
**/
UINT32
LoadElfSymtab (
  IN  VOID   *ImageBase,
  IN  UINT32  Eaddr
  )
{
  ELF32_EHDR                 *Ehdr;
  ELF32_SHDR                 *Shdr;
  UINT32                      Addr;
  UINT32                      Size;
  UINT32                      Index;

  // Copy ELF section headers.
  Ehdr = (ELF32_EHDR *) (UINTN) ImageBase;
  if (Ehdr->E_Shnum == 0 || Ehdr->E_Shentsize) {
    return Eaddr;
  }

  Shdr = (ELF32_SHDR *) (UINTN) Eaddr;
  Size = Ehdr->E_Shnum * Ehdr->E_Shentsize;
  CopyMem (Shdr, (VOID *) ((UINT32)ImageBase + Ehdr->E_Shoff), Size);
  Eaddr += Size;

  // Copy sections that are part of the symbol table.
  for (Index = 0 ; Index < Ehdr->E_Shnum ; Index += 1) {
    if (Shdr[Index].Sh_Type != SHT_SYMTAB) {
      continue;
    }
    Addr = ALIGN_UP (Eaddr, Shdr[Index].Sh_Addralign);
    Size = ALIGN_UP (Shdr[Index].Sh_Size, 4);

    CopyMem ((VOID *)Addr, (VOID *) ((UINT32)ImageBase + Shdr[Index].Sh_Offset), Size);
    Shdr[Index].Sh_Addr = Addr;
    Eaddr = Addr + Size;
  }

  return Eaddr;
}

/**
  Load the ELF image to specified address in ELF header.

  This function load ELF image section by section into memory address specified
  in ELF program header, and also load ELF symbols.

  @param[in]  ImageBase      Memory address of an image

  @retval Image entry point  The entry point of ELF image if load image success
  @retval NULL               Error with loading ELF image
**/
UINT32 *
LoadElfImage (
  IN  VOID  *ImageBase
  )
{
  ELF32_EHDR                 *Ehdr;
  ELF32_PHDR                 *Phdr;
  UINT32                      Eaddr;
  UINT32                      Index;
  UINT32                      Msize;
  UINT32                      Fsize;
  UINT32                      Mpos;
  UINT32                      Ipos;

  //
  // TODO: Check if image, load region overlap; if yes, move image away.
  //

  //
  // Get ELF header and Program header
  //
  Ehdr = (ELF32_EHDR *) ImageBase;
  Phdr = (ELF32_PHDR *) ((UINT32)ImageBase + Ehdr->E_Phoff);

  //
  //  Iterate over the program headers and load all necessary segments.
  //  Keep track of the highest load address used in <eaddr>
  //
  Eaddr = 0;
  for (Index = 0; Index < Ehdr->E_Phnum ; Index ++) {
    if (Phdr[Index].P_Type != PT_LOAD) {
      continue;
    }

    Msize = Phdr[Index].P_Memsz;            // target (in-memory) segment size
    Fsize = Phdr[Index].P_Filesz;           // source (image) segment size
    Mpos  = Phdr[Index].P_Paddr;            // target segment address
    Ipos  = Phdr[Index].P_Offset;           // location of segment in image

    if ((Mpos <= (UINT32)ImageBase) && (Mpos + Msize > (UINT32)ImageBase)) {
      DEBUG ((DEBUG_INFO, "** IMAGE overwrite, out of range **\n"));
      return NULL;
    }

    if (Fsize != 0) {
      CopyMem ((VOID *) Mpos, (VOID *) ((UINT32)ImageBase + Ipos), Fsize);
    }

    if (Msize > Fsize) {
      ZeroMem ((VOID *) (Mpos + Fsize), Msize - Fsize);
    }

    Eaddr = MAX (Eaddr, Mpos + Msize);
  }
  ASSERT (Eaddr != 0);

  //
  // Return Entry Point
  //
  return (UINT32 *) Ehdr->E_Entry;
}
