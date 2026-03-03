/** @file
  ELF library

  Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ElfLibInternal.h"

STATIC
BOOLEAN
IsTextShdr (
  Elf64_Shdr *Shdr
  )
{
  return (BOOLEAN) ((Shdr->sh_flags & (SHF_WRITE | SHF_ALLOC)) == SHF_ALLOC);
}

STATIC
BOOLEAN
IsDataShdr (
  Elf64_Shdr *Shdr
  )
{
  return (BOOLEAN) (Shdr->sh_flags & (SHF_WRITE | SHF_ALLOC)) == (SHF_ALLOC | SHF_WRITE);
}

Elf64_Shdr *
GetElf64SectionByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
  )
{
  Elf64_Ehdr        *Elf64Hdr;

  Elf64Hdr  = (Elf64_Ehdr *)ImageBase;
  if (Index >= Elf64Hdr->e_shnum) {
    return NULL;
  }

  return (Elf64_Shdr *)(ImageBase + Elf64Hdr->e_shoff + Index * Elf64Hdr->e_shentsize);
}

Elf64_Phdr *
GetElf64SegmentByIndex (
  IN  UINT8                 *ImageBase,
  IN  UINT32                Index
)
{
  Elf64_Ehdr        *Elf64Hdr;

  Elf64Hdr  = (Elf64_Ehdr *)ImageBase;
  if (Index >= Elf64Hdr->e_phnum) {
    return NULL;
  }

  return (Elf64_Phdr *)(ImageBase + Elf64Hdr->e_phoff + Index * Elf64Hdr->e_phentsize);
}

EFI_STATUS
RelocateElf64Sections  (
  IN    ELF_IMAGE_CONTEXT      *ElfCt
  )
{
  Elf64_Ehdr       *Elf64Hdr;
  Elf64_Shdr       *Rel64Shdr;
  Elf64_Shdr       *Sec64Shdr;
  Elf64_Rel        *Rel64Entry;
  UINT8            *CurPtr;
  UINT32           Index;
  UINT64           RelIdx;
  UINT32           *Ptr32;
  UINT64           *Ptr64;
  UINT32           RelType;
  UINTN            Delta;
  UINT64           Remainder;

  Elf64Hdr  = (Elf64_Ehdr *)ElfCt->FileBase;
  if (Elf64Hdr->e_machine != EM_X86_64) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check against section header table overflow
  //
  if ((UINTN)Elf64Hdr->e_shoff + (UINTN)Elf64Hdr->e_shnum * (UINTN)Elf64Hdr->e_shentsize > ElfCt->FileSize) {
    return EFI_INVALID_PARAMETER;
  }

  Delta  = (UINTN) ElfCt->ImageAddress - (UINTN) ElfCt->PreferredImageAddress;
  CurPtr = ElfCt->FileBase + Elf64Hdr->e_shoff;
  ASSERT(Elf64Hdr->e_shnum < MAX_ELF_SHNUM);
  for (Index = 0; Index < Elf64Hdr->e_shnum; Index++) {
    Rel64Shdr = (Elf64_Shdr *)CurPtr;
    CurPtr  = CurPtr + Elf64Hdr->e_shentsize;
    if ((Rel64Shdr->sh_type == SHT_REL) || (Rel64Shdr->sh_type == SHT_RELA)) {
      Sec64Shdr = GetElf64SectionByIndex (ElfCt->FileBase, Rel64Shdr->sh_info);
      if (Sec64Shdr == NULL) {
        continue;
      }
      if (!IsTextShdr(Sec64Shdr) && !IsDataShdr(Sec64Shdr)) {
        continue;
      }

      ASSERT (Rel64Shdr->sh_size < MAX_ELF_RELOC_SECT_SIZE);

      //
      // Check against malformed or truncated relocation tables where the last entry might be incomplete,
      // Use DivU64x64Remainder to avoid intrinsic call to __umoddi3
      //
      if(Rel64Shdr->sh_entsize == 0) {
        return EFI_INVALID_PARAMETER;
      }

      DivU64x64Remainder (Rel64Shdr->sh_size, Rel64Shdr->sh_entsize, &Remainder);
      if ((Rel64Shdr->sh_entsize < sizeof (Elf64_Rel)) || (Remainder != 0)) {
        return EFI_INVALID_PARAMETER;
      }

      //
      // Check relocation section bounds
      //
      if ((UINTN)Rel64Shdr->sh_offset + (UINTN)Rel64Shdr->sh_size > ElfCt->FileSize) {
        return EFI_INVALID_PARAMETER;
      }

      for (RelIdx = 0; RelIdx < Rel64Shdr->sh_size; RelIdx += Rel64Shdr->sh_entsize) {
        Rel64Entry = (Elf64_Rel *)((UINT8*)Elf64Hdr + Rel64Shdr->sh_offset + RelIdx);
        RelType = ELF64_R_TYPE(Rel64Entry->r_info);
        switch (RelType) {
          case R_X86_64_NONE:
          case R_X86_64_PC32:
          case R_X86_64_PLT32:
          case R_X86_64_GOTPCREL:
          case R_X86_64_GOTPCRELX:
          case R_X86_64_REX_GOTPCRELX:
            break;
          case R_X86_64_64:
            Ptr64   = (UINT64 *)(UINTN)(Rel64Entry->r_offset + Delta);

            //
            // Sanity check for the relocation address
            //
            if (((UINTN)Ptr64 < (UINTN)ElfCt->ImageAddress) || \
                ((UINTN)Ptr64 + sizeof(UINT64) > (UINTN)ElfCt->ImageAddress + ElfCt->ImageSize)) {
               DEBUG ((DEBUG_ERROR, "Relocation target out of bounds: 0x%p\n", Ptr64));
               return EFI_LOAD_ERROR;
            }

            *Ptr64 += Delta;
            break;
          case R_X86_64_32:
            Ptr32   = (UINT32 *)(UINTN)(Rel64Entry->r_offset + Delta);

            //
            // Sanity check for the relocation address
            //
            if (((UINTN)Ptr32 < (UINTN)ElfCt->ImageAddress) || \
                ((UINTN)Ptr32 + sizeof(UINT32) > (UINTN)ElfCt->ImageAddress + ElfCt->ImageSize)) {
               DEBUG ((DEBUG_ERROR, "Relocation target out of bounds: 0x%p\n", Ptr32));
               return EFI_LOAD_ERROR;
            }

            *Ptr32 += (UINT32)Delta;
            break;
          default:
            DEBUG ((DEBUG_INFO, "Unsupported relocation type %02X\n", RelType));
        }
      }
    }
  }
  ElfCt->EntryPoint = (UINTN)(Elf64Hdr->e_entry + Delta);

  return EFI_SUCCESS;
}

/**
  Load ELF image which has 64-bit architecture.

  Caller should set Context.ImageAddress to a proper value, either pointing to
  a new allocated memory whose size equal to Context.ImageSize, or pointing
  to Context.PreferredImageAddress.

  @param[in]  ElfCt               ELF image context pointer.

  @retval EFI_SUCCESS         ELF binary is loaded successfully.
  @retval Others              Loading ELF binary fails.

**/
EFI_STATUS
LoadElf64Image (
  IN    ELF_IMAGE_CONTEXT    *ElfCt
  )
{
  Elf64_Ehdr    *Elf64Hdr;
  Elf64_Phdr    *ProgramHdr;
  Elf64_Phdr    *ProgramHdrBase;
  UINT32        Index;
  UINTN         Delta;

  ASSERT (ElfCt != NULL);

  //
  // Per the sprit of ELF, loading to memory only consumes info from program headers.
  //
  Elf64Hdr       = (Elf64_Ehdr *)ElfCt->FileBase;

  //
  // Check against program header table overflow
  //
  if ((UINTN)Elf64Hdr->e_phoff + (UINTN)Elf64Hdr->e_phnum * (UINTN)Elf64Hdr->e_phentsize > ElfCt->FileSize) {
    return EFI_INVALID_PARAMETER;
  }

  ProgramHdrBase = (Elf64_Phdr *)(ElfCt->FileBase + Elf64Hdr->e_phoff);
  ASSERT(Elf64Hdr->e_phnum < MAX_ELF_PHNUM);
  for (Index = 0; Index < Elf64Hdr->e_phnum; Index++) {
    ProgramHdr = (Elf64_Phdr *)((UINT8 *)ProgramHdrBase + Index * Elf64Hdr->e_phentsize);

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
    Delta = (UINTN) ProgramHdr->p_paddr - (UINTN) ElfCt->PreferredImageAddress;

    //
    // Check bounds for file read and memory write
    //
    if ((UINTN)ProgramHdr->p_offset + (UINTN)ProgramHdr->p_filesz > ElfCt->FileSize) {
       return EFI_INVALID_PARAMETER;
    }

    if (Delta + (UINTN)ProgramHdr->p_memsz > ElfCt->ImageSize) {
       return EFI_INVALID_PARAMETER;
    }

    CopyMem (ElfCt->ImageAddress + Delta, ElfCt->FileBase + (UINTN) ProgramHdr->p_offset, (UINTN) ProgramHdr->p_filesz);
    ZeroMem (ElfCt->ImageAddress + Delta + (UINTN) ProgramHdr->p_filesz, (UINTN) (ProgramHdr->p_memsz - ProgramHdr->p_filesz));
  }

  //
  // Relocate when new image base is not the preferred image base.
  //
  if ((UINTN)ElfCt->ImageAddress != (UINTN)ElfCt->PreferredImageAddress) {
    if (EFI_ERROR (RelocateElf64Sections (ElfCt))) {
      return EFI_LOAD_ERROR;
    }
  }

  return EFI_SUCCESS;
}
