/** @file
  ELF library

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ElfLibInternal.h"

STATIC
BOOLEAN
IsTextShdr (
  Elf32_Shdr *Shdr
  )
{
  return (BOOLEAN) ((Shdr->sh_flags & (SHF_WRITE | SHF_ALLOC)) == SHF_ALLOC);
}

STATIC
BOOLEAN
IsDataShdr (
  Elf32_Shdr *Shdr
  )
{
  return (BOOLEAN) (Shdr->sh_flags & (SHF_WRITE | SHF_ALLOC)) == (SHF_ALLOC | SHF_WRITE);
}

Elf32_Shdr *
GetElf32SectionByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  )
{
  Elf32_Ehdr        *Elf32Hdr;

  Elf32Hdr  = (Elf32_Ehdr *)ImageBase;
  if (Index >= Elf32Hdr->e_shnum) {
    return NULL;
  }

  return (Elf32_Shdr *)(ImageBase + Elf32Hdr->e_shoff + Index * Elf32Hdr->e_shentsize);
}

Elf32_Phdr *
GetElf32SegmentByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  )
{
  Elf32_Ehdr        *Elf32Hdr;

  Elf32Hdr  = (Elf32_Ehdr *)ImageBase;
  if (Index >= Elf32Hdr->e_phnum) {
    return NULL;
  }

  return (Elf32_Phdr *)(ImageBase + Elf32Hdr->e_phoff + Index * Elf32Hdr->e_phentsize);
}

EFI_STATUS
RelocateElf32Sections  (
  IN    ELF_IMAGE_CONTEXT      *ElfCt
  )
{
  Elf32_Ehdr      *Elf32Hdr;
  Elf32_Shdr      *Rel32Shdr;
  Elf32_Shdr      *Sec32Shdr;
  Elf32_Rel       *Rel32Entry;
  UINT8           *CurPtr;
  UINT32           Index;
  UINT32           RelIdx;
  UINT32          *Ptr32;
  UINT8            RelType;
  UINTN            Delta;

  Elf32Hdr  = (Elf32_Ehdr *)ElfCt->FileBase;
  if (Elf32Hdr->e_machine != EM_386) {
    return EFI_UNSUPPORTED;
  }

  Delta  = (UINTN) ElfCt->ImageAddress - (UINTN) ElfCt->PreferredImageAddress;
  CurPtr = ElfCt->FileBase + Elf32Hdr->e_shoff;
  for (Index = 0; Index < Elf32Hdr->e_shnum; Index++) {
    Rel32Shdr = (Elf32_Shdr *)CurPtr;
    CurPtr  = CurPtr + Elf32Hdr->e_shentsize;
    if ((Rel32Shdr->sh_type == SHT_REL) || (Rel32Shdr->sh_type == SHT_RELA)) {
      Sec32Shdr = GetElf32SectionByIndex (ElfCt->FileBase, Rel32Shdr->sh_info);
      if (Sec32Shdr == NULL) {
        continue;
      }
      if (!IsTextShdr(Sec32Shdr) && !IsDataShdr(Sec32Shdr)) {
        continue;
      }
      DEBUG ((DEBUG_INFO, "Relocate SEC %d\n", Rel32Shdr->sh_info));
      for (RelIdx = 0; RelIdx < Rel32Shdr->sh_size; RelIdx += Rel32Shdr->sh_entsize) {
        Rel32Entry = (Elf32_Rel *)((UINT8*)Elf32Hdr + Rel32Shdr->sh_offset + RelIdx);
        RelType = ELF32_R_TYPE(Rel32Entry->r_info);
        switch (RelType) {
          case R_386_NONE:
          case R_386_PC32:
            //
            // No fixup entry required.
            //
            break;
          case R_386_32:
            //
            // Creates a relative relocation entry from the absolute entry.
            //
            Ptr32 = (UINT32 *) (UINTN) (Rel32Entry->r_offset + Delta);
            *Ptr32 += (UINT32) Delta;
            break;
          default:
            DEBUG ((DEBUG_INFO, "Unsupported relocation type %02X\n", RelType));
        }
      }
    }
  }
  ElfCt->EntryPoint = (UINTN)(Elf32Hdr->e_entry + Delta);

  return EFI_SUCCESS;
}

/**
  Load ELF image which has 32-bit architecture.

  Caller should set Context.ImageAddress to a proper value, either pointing to
  a new allocated memory whose size equal to Context.ImageSize, or pointing
  to Context.PreferredImageAddress.

  @param[in]  ElfCt               ELF image context pointer.

  @retval EFI_SUCCESS         ELF binary is loaded successfully.
  @retval Others              Loading ELF binary fails.

**/
EFI_STATUS
LoadElf32Image (
  IN    ELF_IMAGE_CONTEXT    *ElfCt
  )
{
  Elf32_Ehdr    *Elf32Hdr;
  Elf32_Phdr    *ProgramHdr;
  Elf32_Phdr    *ProgramHdrBase;
  UINT16        Index;
  UINTN         Delta;

  ASSERT (ElfCt != NULL);

  //
  // Per the sprit of ELF, loading to memory only consumes info from program headers.
  //
  Elf32Hdr       = (Elf32_Ehdr *)ElfCt->FileBase;
  ProgramHdrBase = (Elf32_Phdr *)(ElfCt->FileBase + Elf32Hdr->e_phoff);
  for (Index = 0; Index < Elf32Hdr->e_phnum; Index++) {
    ProgramHdr = (Elf32_Phdr *)((UINT8 *)ProgramHdrBase + Index * Elf32Hdr->e_phentsize);

    //
    // Skip segments that don't require load (type tells, or size is 0)
    //
    if ((ProgramHdr->p_type != PT_LOAD) ||
        (ProgramHdr->p_memsz == 0)) {
      continue;
    }

    //
    // The memory offset of segment relative to the image base
    // Note: CopyMem() does nothing when the dst equals to src.
    //
    Delta = ProgramHdr->p_paddr - (UINT32) (UINTN) ElfCt->PreferredImageAddress;
    CopyMem (ElfCt->ImageAddress + Delta, ElfCt->FileBase + ProgramHdr->p_offset, ProgramHdr->p_filesz);
    ZeroMem (ElfCt->ImageAddress + Delta + ProgramHdr->p_filesz, ProgramHdr->p_memsz - ProgramHdr->p_filesz);
  }

  //
  // Relocate when new new image base is not the preferred image base.
  //
  if (ElfCt->ImageAddress != ElfCt->PreferredImageAddress) {
    RelocateElf32Sections (ElfCt);
  }

  return EFI_SUCCESS;
}
