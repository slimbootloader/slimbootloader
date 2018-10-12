/** @file
  Base PE/COFF loader supports loading any PE32/PE32+ or TE image, but
  only supports relocating IA32, x64, IPF, and EBC images.

  Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/PeImage.h>
#include <Library/LitePeCoffLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>

#define PE_COFF_IMAGE_ALIGN_SIZE        4

/**
  Check if the image is a TE or PE32 image.

  @param[in]       ImageBase    Memory address of an image
  @param[in, out]  Header       Pointer to EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION structure.
                                If it is TE/PE32 image, the structure will be updated.

  @retval TRUE                  Image is a TE/PE32 image
  @retval FALSE                 Not a TE/PE32 image

**/
BOOLEAN
EFIAPI
IsTePe32Image (
  IN      VOID                                   *ImageBase,
  IN  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION    *Header    OPTIONAL
  )
{
  EFI_IMAGE_DOS_HEADER                  *DosHdr;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION    Hdr;

  DosHdr = (EFI_IMAGE_DOS_HEADER *)ImageBase;
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present, so read the PE header after the DOS image header.
    //
    Hdr.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) ((UINTN) ImageBase + (UINTN) ((DosHdr->e_lfanew) & 0x0ffff));
  } else {
    //
    // DOS image header is not present, so PE header is at the image base.
    //
    Hdr.Pe32 = (EFI_IMAGE_NT_HEADERS32 *)ImageBase;
  }

  if ( (Hdr.Te->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) ||
       (Hdr.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) ) {
    if (Header != NULL) {
      *Header = Hdr;
    }
    return TRUE;
  }

  return FALSE;
}


/**
  Retrieves and returns a pointer to the entry point to a PE/COFF image that has been loaded
  into system memory with the PE/COFF Loader Library functions.

  Retrieves the entry point to the PE/COFF image specified by Pe32Data and returns this entry
  point in EntryPoint.  If the entry point could not be retrieved from the PE/COFF image, then
  return RETURN_INVALID_PARAMETER.  Otherwise return RETURN_SUCCESS.
  If Pe32Data is NULL, then ASSERT().
  If EntryPoint is NULL, then ASSERT().

  @param  Pe32Data                  The pointer to the PE/COFF image that is loaded in system memory.
  @param  EntryPoint                The pointer to entry point to the PE/COFF image to return.

  @retval RETURN_SUCCESS            EntryPoint was returned.
  @retval RETURN_INVALID_PARAMETER  The entry point could not be found in the PE/COFF image.

**/
RETURN_STATUS
EFIAPI
PeCoffLoaderGetEntryPoint (
  IN  VOID  *Pe32Data,
  OUT VOID  **EntryPoint
  )
{
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;

  ASSERT (Pe32Data   != NULL);
  ASSERT (EntryPoint != NULL);

  if (!IsTePe32Image (Pe32Data, &Hdr)) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Calculate the entry point relative to the start of the image.
  // AddressOfEntryPoint is common for PE32 & PE32+
  //
  if (Hdr.Te->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
    *EntryPoint = (VOID *) ((UINTN)Pe32Data + (UINTN) (Hdr.Te->AddressOfEntryPoint & 0x0ffffffff) + sizeof (
                              EFI_TE_IMAGE_HEADER) - Hdr.Te->StrippedSize);
    return RETURN_SUCCESS;
  } else if (Hdr.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
    *EntryPoint = (VOID *) ((UINTN)Pe32Data + (UINTN) (Hdr.Pe32->OptionalHeader.AddressOfEntryPoint & 0x0ffffffff));
    return RETURN_SUCCESS;
  }

  return RETURN_UNSUPPORTED;
}


/**
  Performs an specific relocation fpr PECOFF images. The caller needs to
  allocate enough buffer at the PreferedImageBase

  @param  ImageBase        Pointer to the current image base.

  @return Status code.

**/
RETURN_STATUS
PeCoffRelocateImage (
  IN UINT32   ImageBase
  )
{
  RETURN_STATUS                   Status;
  UINT32                          RelocSectionSize;
  UINT32                          RelocSectionOffset;
  UINT16                         *RelocDataPtr;
  UINT32                          FixupDelta;
  UINT32                          PageRva;
  UINT32                          BlockSize;
  UINTN                           Index;
  UINT8                           Type;
  UINT32                          Data;
  UINT16                          Offset;
  UINT16                          TypeOffset;
  UINT32                          ImgOffset;
  UINT32                          Adjust;
  EFI_TE_IMAGE_HEADER            *Te;
  EFI_IMAGE_NT_HEADERS32         *Pe32;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION    Hdr;
  PE_COFF_LOADER_IMAGE_CONTEXT    ImageContext;

  Status = RETURN_SUCCESS;

  if (!IsTePe32Image ((VOID *)ImageBase, &Hdr)) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Calculate the entry point relative to the start of the image.
  // AddressOfEntryPoint is common for PE32 & PE32+
  //
  Te   = NULL;
  Pe32 = NULL;
  if (Hdr.Te->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
    Te   = Hdr.Te;
  } else if (Hdr.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
    Pe32 = Hdr.Pe32;
  } else {
    return RETURN_UNSUPPORTED;
  }

  if (Te != NULL) {
    // Handle relocation data
    // Calculate the fixup delta.
    Adjust = Te->StrippedSize - sizeof (EFI_TE_IMAGE_HEADER);
    FixupDelta = ImageBase - ((UINT32)Te->ImageBase + Adjust);

    RelocSectionOffset = Te->DataDirectory[0].VirtualAddress - Adjust;
    RelocSectionSize   = Te->DataDirectory[0].Size;
  } else if (Pe32 != NULL) {
    if (Pe32->OptionalHeader.NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC) {
      return RETURN_UNSUPPORTED;
    }
    Adjust = 0;
    FixupDelta = ImageBase - Hdr.Pe32->OptionalHeader.ImageBase;
    RelocSectionOffset = Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    RelocSectionSize   = Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
  } else {
    return RETURN_UNSUPPORTED;
  }

  // This seems to be a bug in the way MS generates the reloc fixup blocks.
  // After we have gone thru all the fixup blocks in the .reloc section, the
  // variable RelocSectionSize should ideally go to zero. But I have found some orphan
  // data after all the fixup blocks that don't quite fit anywhere. So, I have
  // changed the check to a greater-than-eight. It should be at least eight
  // because the PageRva and the BlockSize together take eight bytes. If less
  // than 8 are remaining, then those are the orphans and we need to disregard them.
  RelocDataPtr =  (UINT16 *) (ImageBase + RelocSectionOffset);
  while (RelocSectionSize >= 8) {
    //DEBUG ((EFI_D_INFO, "RelocSectionSize = %08x\n", RelocSectionSize));

    // Read the Page RVA and Block Size for the current fixup block.
    PageRva   = * (UINT32 *) (RelocDataPtr + 0);
    BlockSize = * (UINT32 *) (RelocDataPtr + 2);
    RelocDataPtr += 4;

    if (BlockSize == 0) {
      //DEBUG ((EFI_D_INFO, "BlockSize is 0, done...\n"));
      break;
    }

    RelocSectionSize -= sizeof (UINT32) * 2;

    // Extract the correct number of Type/Offset entries. This is given by:
    // Loop count = Number of relocation items =
    // (Block Size - 4 bytes (Page RVA field) - 4 bytes (Block Size field)) divided
    // by 2 (each Type/Offset entry takes 2 bytes).
    // DEBUG ((EFI_D_INFO, "LoopCount = %04x\n", ((BlockSize - 2 * sizeof(UINT32)) / sizeof(UINT16))));
    for (Index = 0; Index < ((BlockSize - 2 * sizeof (UINT32)) / sizeof (UINT16)); Index++) {
      TypeOffset = *RelocDataPtr++;
      Type   = (UINT8) ((TypeOffset & 0xf000) >> 12);
      Offset = (UINT16) ((UINT16)TypeOffset & 0x0fff);
      RelocSectionSize -= sizeof (UINT16);
      ImgOffset = PageRva + Offset - Adjust;
      // DEBUG ((EFI_D_INFO, "%d: PageRva: %08x Offset: %04x Type: %x \n", Index, PageRva, ImgOffset, Type));
      Data = * (UINT32 *) (ImageBase + ImgOffset);
      switch (Type) {
      case 0:
        break;
      case 1:
        Data += (FixupDelta >> 16) & 0x0000ffff;
        break;
      case 2:
        Data += FixupDelta & 0x0000ffff;
        break;
      case 3:
        Data += FixupDelta;
        break;
      default:
        //DEBUG ((EFI_D_INFO, "Unknown Type!\n"));
        break;
      }
      * (UINT32 *) (ImageBase + ImgOffset) = Data;
    }
  }

  if (Te != NULL) {
    Te->ImageBase   += FixupDelta;
  } else {
    Pe32->OptionalHeader.ImageBase += FixupDelta;
  }

  ImageContext.ImageAddress = (PHYSICAL_ADDRESS)(UINTN) ImageBase;
  ImageContext.PdbPointer = (CHAR8 *) PeCoffLoaderGetPdbPointer ((VOID *)(UINTN) ImageContext.ImageAddress);
  PeCoffLoaderRelocateImageExtraAction (&ImageContext);

  return Status;
}

/**
  Returns PE/COFF image base is loaded in system memory where the input address is in.

  On DEBUG build, searches the PE/COFF image base forward the input address and
  returns it.

  @param  Address    Address located in one PE/COFF image.

  @retval 0          RELEASE build or cannot find the PE/COFF image base.
  @retval others     PE/COFF image base found.

**/
UINTN
EFIAPI
PeCoffSearchImageBase (
  IN UINTN    Address
  )
{
  UINTN                                Pe32Data;

  Pe32Data = 0;

  //
  // Find Image Base
  //
  Pe32Data = Address & ~(PE_COFF_IMAGE_ALIGN_SIZE - 1);
  while (Pe32Data != 0) {
    if (IsTePe32Image ((VOID *)Pe32Data, NULL) == TRUE) {
      break;
    }
    //
    // Not found the image base, check the previous aligned address
    //
    Pe32Data -= PE_COFF_IMAGE_ALIGN_SIZE;
  }

  return Pe32Data;
}

/**
  Returns a pointer to the PDB file name for a raw PE/COFF image that is not
  loaded into system memory with the PE/COFF Loader Library functions.

  Returns the PDB file name for the PE/COFF image specified by Pe32Data.  If
  the PE/COFF image specified by Pe32Data is not a valid, then NULL is
  returned.  If the PE/COFF image specified by Pe32Data does not contain a
  debug directory entry, then NULL is returned.  If the debug directory entry
  in the PE/COFF image specified by Pe32Data does not contain a PDB file name,
  then NULL is returned.
  If Pe32Data is NULL, then return NULL.

  @param  Pe32Data   Pointer to the PE/COFF image that is loaded in system
                     memory.

  @return The PDB file name for the PE/COFF image specified by Pe32Data or NULL
          if it cannot be retrieved.

**/
VOID *
EFIAPI
PeCoffLoaderGetPdbPointer (
  IN VOID  *Pe32Data
  )
{
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_DATA_DIRECTORY              *DirectoryEntry;
  EFI_IMAGE_DEBUG_DIRECTORY_ENTRY       *DebugEntry;
  UINTN                                 DirCount;
  VOID                                  *CodeViewEntryPointer;
  INTN                                  TEImageAdjust;
  UINT32                                NumberOfRvaAndSizes;
  UINT16                                Magic;
  EFI_IMAGE_SECTION_HEADER              *SectionHeader;
  UINT32                                Index, Index1;

  if (Pe32Data == NULL) {
    return NULL;
  }

  TEImageAdjust       = 0;
  DirectoryEntry      = NULL;
  DebugEntry          = NULL;
  NumberOfRvaAndSizes = 0;
  Index               = 0;
  Index1              = 0;
  SectionHeader       = NULL;

  if (!IsTePe32Image (Pe32Data, &Hdr)) {
    return NULL;
  }

  if (EFI_TE_IMAGE_HEADER_SIGNATURE == Hdr.Te->Signature) {
    if (Hdr.Te->DataDirectory[EFI_TE_IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress != 0) {
      DirectoryEntry  = &Hdr.Te->DataDirectory[EFI_TE_IMAGE_DIRECTORY_ENTRY_DEBUG];
      TEImageAdjust   = sizeof (EFI_TE_IMAGE_HEADER) - Hdr.Te->StrippedSize;

      //
      // Get the DebugEntry offset in the raw data image.
      //
      SectionHeader = (EFI_IMAGE_SECTION_HEADER *) (Hdr.Te + 1);
      Index = Hdr.Te->NumberOfSections;
      for (Index1 = 0; Index1 < Index; Index1 ++) {
        if ((DirectoryEntry->VirtualAddress >= SectionHeader[Index1].VirtualAddress) &&
           (DirectoryEntry->VirtualAddress < (SectionHeader[Index1].VirtualAddress + SectionHeader[Index1].Misc.VirtualSize))) {
          DebugEntry = (EFI_IMAGE_DEBUG_DIRECTORY_ENTRY *)((UINTN) Hdr.Te +
                        DirectoryEntry->VirtualAddress -
                        SectionHeader [Index1].VirtualAddress +
                        SectionHeader [Index1].PointerToRawData +
                        TEImageAdjust);
          break;
        }
      }
    }
  } else if (EFI_IMAGE_NT_SIGNATURE == Hdr.Pe32->Signature) {
    //
    // NOTE: We use Machine field to identify PE32/PE32+, instead of Magic.
    //       It is due to backward-compatibility, for some system might
    //       generate PE32+ image with PE32 Magic.
    //
    switch (Hdr.Pe32->FileHeader.Machine) {
    case EFI_IMAGE_MACHINE_IA32:
      //
      // Assume PE32 image with IA32 Machine field.
      //
      Magic = EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC;
      break;
    case EFI_IMAGE_MACHINE_X64:
      //
      // Assume PE32+ image with X64 or IPF Machine field
      //
      Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
      break;
    default:
      //
      // For unknow Machine field, use Magic in optional Header
      //
      Magic = Hdr.Pe32->OptionalHeader.Magic;
    }

    SectionHeader = (EFI_IMAGE_SECTION_HEADER *) (
                       (UINT8 *) Hdr.Pe32 +
                       sizeof (UINT32) +
                       sizeof (EFI_IMAGE_FILE_HEADER) +
                       Hdr.Pe32->FileHeader.SizeOfOptionalHeader
                       );
    Index = Hdr.Pe32->FileHeader.NumberOfSections;

    if (EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC == Magic) {
      //
      // Use PE32 offset get Debug Directory Entry
      //
      NumberOfRvaAndSizes = Hdr.Pe32->OptionalHeader.NumberOfRvaAndSizes;
      DirectoryEntry = (EFI_IMAGE_DATA_DIRECTORY *)&(Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_DEBUG]);
    } else if (Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      //
      // Use PE32+ offset get Debug Directory Entry
      //
      NumberOfRvaAndSizes = Hdr.Pe32Plus->OptionalHeader.NumberOfRvaAndSizes;
      DirectoryEntry = (EFI_IMAGE_DATA_DIRECTORY *)&(Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_DEBUG]);
    }

    if (NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_DEBUG || DirectoryEntry->VirtualAddress == 0) {
      DirectoryEntry = NULL;
      DebugEntry = NULL;
    } else {
      //
      // Get the DebugEntry offset in the raw data image.
      //
      for (Index1 = 0; Index1 < Index; Index1 ++) {
        if ((DirectoryEntry->VirtualAddress >= SectionHeader[Index1].VirtualAddress) &&
           (DirectoryEntry->VirtualAddress < (SectionHeader[Index1].VirtualAddress + SectionHeader[Index1].Misc.VirtualSize))) {
          DebugEntry = (EFI_IMAGE_DEBUG_DIRECTORY_ENTRY *) (
                       (UINTN) Pe32Data +
                       DirectoryEntry->VirtualAddress -
                       SectionHeader[Index1].VirtualAddress +
                       SectionHeader[Index1].PointerToRawData);
          break;
        }
      }
    }
  } else {
    return NULL;
  }

  if (NULL == DebugEntry || NULL == DirectoryEntry) {
    return NULL;
  }

  //
  // Scan the directory to find the debug entry.
  //
  for (DirCount = 0; DirCount < DirectoryEntry->Size; DirCount += sizeof (EFI_IMAGE_DEBUG_DIRECTORY_ENTRY), DebugEntry++) {
    if (EFI_IMAGE_DEBUG_TYPE_CODEVIEW == DebugEntry->Type) {
      if (DebugEntry->SizeOfData > 0) {
        //
        // Get the DebugEntry offset in the raw data image.
        //
        CodeViewEntryPointer = NULL;
        for (Index1 = 0; Index1 < Index; Index1 ++) {
          if ((DebugEntry->RVA >= SectionHeader[Index1].VirtualAddress) &&
             (DebugEntry->RVA < (SectionHeader[Index1].VirtualAddress + SectionHeader[Index1].Misc.VirtualSize))) {
            CodeViewEntryPointer = (VOID *) (
                                   ((UINTN)Pe32Data) +
                                   (UINTN) DebugEntry->RVA -
                                   SectionHeader[Index1].VirtualAddress +
                                   SectionHeader[Index1].PointerToRawData +
                                   (UINTN)TEImageAdjust);
            break;
          }
        }
        if (Index1 >= Index) {
          //
          // Can't find CodeViewEntryPointer in raw PE/COFF image.
          //
          continue;
        }
        switch (* (UINT32 *) CodeViewEntryPointer) {
        case CODEVIEW_SIGNATURE_NB10:
          return (VOID *) ((CHAR8 *)CodeViewEntryPointer + sizeof (EFI_IMAGE_DEBUG_CODEVIEW_NB10_ENTRY));
        case CODEVIEW_SIGNATURE_RSDS:
          return (VOID *) ((CHAR8 *)CodeViewEntryPointer + sizeof (EFI_IMAGE_DEBUG_CODEVIEW_RSDS_ENTRY));
        case CODEVIEW_SIGNATURE_MTOC:
          return (VOID *) ((CHAR8 *)CodeViewEntryPointer + sizeof (EFI_IMAGE_DEBUG_CODEVIEW_MTOC_ENTRY));
        default:
          break;
        }
      }
    }
  }

  return NULL;
}

/**
  Find and report image info to HOST.

  @param  ImageBase        Pointer to the current image base.

**/
VOID
PeCoffFindAndReportImageInfo (
  IN UINT32   ImageBase
  )
{
  UINTN                           Pe32Data;
  PE_COFF_LOADER_IMAGE_CONTEXT    ImageContext;

  Pe32Data = PeCoffSearchImageBase (ImageBase);
  if (Pe32Data != 0) {
    ImageContext.ImageAddress = Pe32Data;
    ImageContext.PdbPointer = (CHAR8 *) PeCoffLoaderGetPdbPointer ((VOID *)(UINTN) ImageContext.ImageAddress);
    PeCoffLoaderRelocateImageExtraAction (&ImageContext);
  }
}
