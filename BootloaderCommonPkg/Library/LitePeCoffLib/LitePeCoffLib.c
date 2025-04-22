/** @file
  Base PE/COFF loader supports loading any PE32/PE32+ or TE image, but
  only supports relocating IA32, x64, IPF, and EBC images.

  Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/PeImage.h>
#include <Library/LitePeCoffLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugAgentLib.h>

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

  if (ImageBase == NULL) {
    return FALSE;
  }

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

  if (EntryPoint == NULL || Pe32Data == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

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
EFIAPI
PeCoffRelocateImage (
  IN UINT32   ImageBase
  )
{
  RETURN_STATUS                   Status;
  UINT32                          RelocSectionSize;
  UINT32                          RelocSectionOffset;
  UINT16                         *RelocDataPtr;
  UINT64                          FixupDelta;
  UINT32                          PageRva;
  UINT32                          BlockSize;
  UINTN                           Index;
  UINT8                           Type;
  UINT32                         *DataPtr;
  UINT16                          Offset;
  UINT16                          TypeOffset;
  UINT32                          ImgOffset;
  UINT32                          Adjust;
  EFI_TE_IMAGE_HEADER            *Te;
  EFI_IMAGE_NT_HEADERS32         *Pe32;
  EFI_IMAGE_NT_HEADERS64         *Pe64;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION    Hdr;

  Status = RETURN_SUCCESS;

  if (!IsTePe32Image ((VOID *)(UINTN)ImageBase, &Hdr)) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Calculate the entry point relative to the start of the image.
  // AddressOfEntryPoint is common for PE32 & PE32+
  //
  Te   = NULL;
  Pe32 = NULL;
  Pe64 = NULL;
  if (Hdr.Te->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
    Te   = Hdr.Te;
  } else if (Hdr.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
    if (Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      Pe32 = Hdr.Pe32;
    } else if (Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      Pe64 = Hdr.Pe32Plus;
    }
  } else {
    return RETURN_UNSUPPORTED;
  }

  if (Te != NULL) {
    // Handle relocation data
    // Calculate the fixup delta.
    Adjust = Te->StrippedSize - sizeof (EFI_TE_IMAGE_HEADER);
    FixupDelta = (UINT64)ImageBase - (Te->ImageBase + Adjust);

    RelocSectionOffset = Te->DataDirectory[0].VirtualAddress - Adjust;
    RelocSectionSize   = Te->DataDirectory[0].Size;
  } else if (Pe32 != NULL) {
    if (Pe32->OptionalHeader.NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC) {
      return RETURN_UNSUPPORTED;
    }
    Adjust = 0;
    FixupDelta = (UINT64)ImageBase - Pe32->OptionalHeader.ImageBase;
    RelocSectionOffset = Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    RelocSectionSize   = Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
  } else if (Pe64 != NULL) {
    if (Pe64->OptionalHeader.NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC) {
      return RETURN_UNSUPPORTED;
    }
    Adjust = 0;
    FixupDelta = (UINT64)ImageBase - Pe64->OptionalHeader.ImageBase;
    RelocSectionOffset = Pe64->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    RelocSectionSize   = Pe64->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
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
  RelocDataPtr =  (UINT16 *)(UINTN)(ImageBase + RelocSectionOffset);
  while (RelocSectionSize >= 8) {
    // Read the Page RVA and Block Size for the current fixup block.
    PageRva   = * (UINT32 *) (RelocDataPtr + 0);
    BlockSize = * (UINT32 *) (RelocDataPtr + 2);
    RelocDataPtr += 4;

    if (BlockSize == 0) {
      break;
    }

    RelocSectionSize -= sizeof (UINT32) * 2;

    // Extract the correct number of Type/Offset entries. This is given by:
    // Loop count = Number of relocation items =
    // (Block Size - 4 bytes (Page RVA field) - 4 bytes (Block Size field)) divided
    // by 2 (each Type/Offset entry takes 2 bytes).
    DEBUG ((DEBUG_VERBOSE, "LoopCount = %04x\n", ((BlockSize - 2 * sizeof(UINT32)) / sizeof(UINT16))));
    for (Index = 0; Index < ((BlockSize - 2 * sizeof (UINT32)) / sizeof (UINT16)); Index++) {
      TypeOffset = *RelocDataPtr++;
      Type   = (UINT8) ((TypeOffset & 0xf000) >> 12);
      Offset = (UINT16) ((UINT16)TypeOffset & 0x0fff);
      RelocSectionSize -= sizeof (UINT16);
      ImgOffset = PageRva + Offset - Adjust;
      DEBUG ((DEBUG_VERBOSE, "%d: PageRva: %08x Offset: %04x Type: %x \n", Index, PageRva, ImgOffset, Type));
      DataPtr = (UINT32 *)(UINTN)(ImageBase + ImgOffset);
      switch (Type) {
      case 0:
        break;
      case 1:
        *DataPtr += (((UINT32)FixupDelta >> 16) & 0x0000ffff);
        break;
      case 2:
        *DataPtr += ((UINT32)FixupDelta & 0x0000ffff);
        break;
      case 3:
        *DataPtr += (UINT32)FixupDelta;
        break;
      case 10:
        *(UINT64 *)DataPtr += FixupDelta;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unknown RELOC type: %d\n", Type));
        break;
      }
    }
  }

  if (Te != NULL) {
    Te->ImageBase   += FixupDelta;
  } else if (Pe32 != NULL) {
    Pe32->OptionalHeader.ImageBase += (UINT32)FixupDelta;
  } else {
    Pe64->OptionalHeader.ImageBase += FixupDelta;
  }

  PeCoffFindAndReportImageInfo (ImageBase);

  return Status;
}

/**
  Get PE/TE image preferred image base address

  Based on current image location Pe32Data, check if it is same with expected location to run the
  image. If it is not same, it means the image need relocate to expected location. This function
  will return the preferred image base.

  @param[in]   Pe32Data             The pointer to the PE/COFF image that is loaded in system memory.
  @param[out]  Base                 The pointer to receive image base.

  @retval RETURN_SUCCESS            Base was returned successfully.
  @retval RETURN_UNSUPPORTED        It is not PE/COFF image.
**/
RETURN_STATUS
EFIAPI
PeCoffGetPreferredBase (
  IN  VOID                             *Pe32Data,
  OUT UINT32                           *Base      OPTIONAL
  )
{
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  UINT32                                ImageBase;

  if (!IsTePe32Image (Pe32Data, &Hdr)) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Common for PE32 & PE32+
  //
  if (Hdr.Te->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
    ImageBase = (UINT32)Hdr.Te->ImageBase + Hdr.Te->StrippedSize - sizeof (EFI_TE_IMAGE_HEADER);
  } else if (Hdr.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
    if (Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      ImageBase = Hdr.Pe32->OptionalHeader.ImageBase;
    } else if (Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      ImageBase = (UINT32)Hdr.Pe32Plus->OptionalHeader.ImageBase;
    } else {
      return RETURN_UNSUPPORTED;
    }
  } else {
    return RETURN_UNSUPPORTED;
  }

  if (Base != NULL) {
    *Base = ImageBase;
  }

  return RETURN_SUCCESS;
}

/**
  Extract and return the machine type from the PE/COFF image.

  @param  Pe32Data                  The pointer to the PE/COFF image that is loaded in system memory.
  @param  MachinePtr                The pointer to machine type to return.

  @retval RETURN_SUCCESS            Machine was returned successfully.
  @retval RETURN_UNSUPPORTED        Unsupported image format.
  @retval RETURN_INVALID_PARAMETER  The Pe32Data pointer is NULL.

**/
RETURN_STATUS
EFIAPI
PeCoffLoaderGetMachine (
  IN  VOID     *Pe32Data,
  OUT UINT16   *MachinePtr      OPTIONAL
  )
{
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  UINT16                                Machine;

  if (Pe32Data == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  Machine = 0;
  if (IsTePe32Image (Pe32Data, &Hdr)) {
    if (Hdr.Te->Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
      Machine = Hdr.Te->Machine;
    }

    if (Hdr.Te->Signature == EFI_IMAGE_NT_SIGNATURE) {
      Machine = Hdr.Pe32->FileHeader.Machine;
    }
  }

  if (Machine == 0) {
    return RETURN_UNSUPPORTED;
  }

  if (MachinePtr != NULL) {
    *MachinePtr = Machine;
  }

  return RETURN_SUCCESS;
}
