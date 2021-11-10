/** @file
  PE/Coff Extra Action library instances.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PeCoffExtraActionLib.h>

/**
  Check if the hardware breakpoint in Drx is enabled by checking the Lx and Gx bit in Dr7.

  It assumes that DebugAgent will set both Lx and Gx bit when setting up the hardware breakpoint.


  @param  RegisterIndex  Index of Dr register. The value range is from 0 to 3.
  @param  Dr7            Value of Dr7 register.

  @return TRUE   The hardware breakpoint specified in the Drx is enabled.
  @return FALSE  The hardware breakpoint specified in the Drx is disabled.

**/
BOOLEAN
IsDrxEnabled (
  IN  UINT8  RegisterIndex,
  IN  UINTN  Dr7
  )
{
  return (BOOLEAN) (((Dr7 >> (RegisterIndex * 2)) & (BIT0 | BIT1)) == (BIT0 | BIT1));
}

/**
  Common routine to report the PE/COFF image loading/relocating or unloading event.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image.
  @param  Signature     IMAGE_LOAD_SIGNATURE or IMAGE_UNLOAD_SIGNATURE.

**/
VOID
PeCoffLoaderExtraActionCommon (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext,
  IN     UINTN                         Signature
  )
{
  BOOLEAN                    InterruptState;
  UINTN                      Dr0;
  UINTN                      Dr1;
  UINTN                      Dr2;
  UINTN                      Dr3;
  UINTN                      Dr7;
  UINTN                      Cr4;
  UINTN                      NewDr7;
  UINT8                      LoadImageMethod;
  UINT8                      DebugAgentStatus;
  IA32_DESCRIPTOR            IdtDescriptor;
  IA32_IDT_GATE_DESCRIPTOR   OriginalIdtEntry;
  BOOLEAN                    IdtEntryHooked;
  UINT32                     RegEdx;

  ASSERT (ImageContext != NULL);

  if (ImageContext->PdbPointer != NULL) {
    DEBUG ((DEBUG_ERROR, "    PDB = %a\n", ImageContext->PdbPointer));
  }

  //
  // Disable interrupts and save the current interrupt state
  //
  InterruptState = SaveAndDisableInterrupts ();

  IdtEntryHooked  = FALSE;
  LoadImageMethod = PcdGet8 (PcdDebugLoadImageMethod);
  if (LoadImageMethod == DEBUG_LOAD_IMAGE_METHOD_IO_HW_BREAKPOINT) {
    //
    // If the CPU does not support Debug Extensions(CPUID:01 EDX:BIT2)
    // then force use of DEBUG_LOAD_IMAGE_METHOD_SOFT_INT3
    //
    AsmCpuid (1, NULL, NULL, NULL, &RegEdx);
    if ((RegEdx & BIT2) == 0) {
      LoadImageMethod = DEBUG_LOAD_IMAGE_METHOD_SOFT_INT3;
    }
  }
  AsmReadIdtr (&IdtDescriptor);
  if (LoadImageMethod == DEBUG_LOAD_IMAGE_METHOD_SOFT_INT3) {
    if (!CheckDebugAgentHandler (&IdtDescriptor, SOFT_INT_VECTOR_NUM)) {
      //
      // Do not trigger INT3 if Debug Agent did not setup IDT entries.
      //
      return;
    }
  } else {
    if (!CheckDebugAgentHandler (&IdtDescriptor, IO_HW_BREAKPOINT_VECTOR_NUM)) {
      //
      // Save and update IDT entry for INT1
      //
      SaveAndUpdateIdtEntry1 (&IdtDescriptor, &OriginalIdtEntry);
      IdtEntryHooked = TRUE;
    }
  }

  //
  // Save Debug Register State
  //
  Dr0 = AsmReadDr0 ();
  Dr1 = AsmReadDr1 ();
  Dr2 = AsmReadDr2 ();
  Dr3 = AsmReadDr3 ();
  Dr7 = AsmReadDr7 () | BIT10; // H/w sets bit 10, some simulators don't
  Cr4 = AsmReadCr4 ();

  //
  // DR0 = Signature
  // DR1 = The address of the Null-terminated ASCII string for the PE/COFF image's PDB file name
  // DR2 = The pointer to the ImageContext structure
  // DR3 = IO_PORT_BREAKPOINT_ADDRESS
  // DR7 = Disables all HW breakpoints except for DR3 I/O port access of length 1 byte
  // CR4 = Make sure DE(BIT3) is set
  //
  AsmWriteDr7 (BIT10);
  AsmWriteDr0 (Signature);
  AsmWriteDr1 ((UINTN) ImageContext->PdbPointer);
  AsmWriteDr2 ((UINTN) ImageContext);
  AsmWriteDr3 (IO_PORT_BREAKPOINT_ADDRESS);

  if (LoadImageMethod == DEBUG_LOAD_IMAGE_METHOD_IO_HW_BREAKPOINT) {
    AsmWriteDr7 (0x20000480);
    AsmWriteCr4 (Cr4 | BIT3);
    //
    // Do an IN from IO_PORT_BREAKPOINT_ADDRESS to generate a HW breakpoint until the port
    // returns a read value other than DEBUG_AGENT_IMAGE_WAIT
    //
    do {
      DebugAgentStatus = IoRead8 (IO_PORT_BREAKPOINT_ADDRESS);
    } while (DebugAgentStatus == DEBUG_AGENT_IMAGE_WAIT);

  } else if (LoadImageMethod == DEBUG_LOAD_IMAGE_METHOD_SOFT_INT3) {
    //
    // Generate a software break point.
    //
    CpuBreakpoint ();
  }

  //
  // Restore Debug Register State only when Host didn't change it inside exception handler.
  // E.g.: User halts the target and sets the HW breakpoint while target is
  //       in the above exception handler
  //
  NewDr7 = AsmReadDr7 () | BIT10; // H/w sets bit 10, some simulators don't
  if (!IsDrxEnabled (0, NewDr7) && (AsmReadDr0 () == 0 || AsmReadDr0 () == Signature)) {
    //
    // If user changed Dr3 (by setting HW bp in the above exception handler,
    // we will not set Dr0 to 0 in GO/STEP handler because the break cause is not IMAGE_LOAD/_UNLOAD.
    //
    AsmWriteDr0 (Dr0);
  }
  if (!IsDrxEnabled (1, NewDr7) && (AsmReadDr1 () == (UINTN) ImageContext->PdbPointer)) {
    AsmWriteDr1 (Dr1);
  }
  if (!IsDrxEnabled (2, NewDr7) && (AsmReadDr2 () == (UINTN) ImageContext)) {
    AsmWriteDr2 (Dr2);
  }
  if (!IsDrxEnabled (3, NewDr7) && (AsmReadDr3 () == IO_PORT_BREAKPOINT_ADDRESS)) {
    AsmWriteDr3 (Dr3);
  }
  if (LoadImageMethod == DEBUG_LOAD_IMAGE_METHOD_IO_HW_BREAKPOINT) {
    if (AsmReadCr4 () == (Cr4 | BIT3)) {
      AsmWriteCr4 (Cr4);
    }
    if (NewDr7 == 0x20000480) {
      AsmWriteDr7 (Dr7);
    }
  } else if (LoadImageMethod == DEBUG_LOAD_IMAGE_METHOD_SOFT_INT3) {
    if (NewDr7 == BIT10) {
      AsmWriteDr7 (Dr7);
    }
  }
  //
  // Restore original IDT entry for INT1 if it was hooked.
  //
  if (IdtEntryHooked) {
    RestoreIdtEntry1 (&IdtDescriptor, &OriginalIdtEntry);
  }
  //
  // Restore the interrupt state
  //
  SetInterruptState (InterruptState);
}

/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
VOID
EFIAPI
PeCoffLoaderRelocateImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  PeCoffLoaderExtraActionCommon (ImageContext, IMAGE_LOAD_SIGNATURE);
}

/**
  Performs additional actions just before a PE/COFF image is unloaded.  Any resources
  that were allocated by PeCoffLoaderRelocateImageExtraAction() must be freed.

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that is being unloaded.

**/
VOID
EFIAPI
PeCoffLoaderUnloadImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  PeCoffLoaderExtraActionCommon (ImageContext, IMAGE_UNLOAD_SIGNATURE);
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

