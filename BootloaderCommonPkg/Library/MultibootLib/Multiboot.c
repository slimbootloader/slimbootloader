/** @file
  This file Multiboot specification (implementation).

  Copyright (c) 2014 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/MultibootLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Guid/GraphicsInfoHob.h>
#include "MultibootLibInternal.h"

#define SUPPORTED_FEATURES  (MULTIBOOT_HEADER_MODS_ALIGNED | MULTIBOOT_HEADER_WANT_MEMORY | MULTIBOOT_HEADER_HAS_VBE)
UINT8 mLoaderName[]        = "Slim BootLoader\0";

VOID DumpMbHeader (CONST MULTIBOOT_HEADER *Mh);


/**
  Get multiboot header address

  Get multiboot header following Multboot spec 0.6.98.

  @param[in]  ImageAddr    Memory address of an image

  @retval MultibootHeader  Pointer to Multiboot header if found.
  @retval NULL             No valid Multiboot header found or requesting
                           features not support.
**/
CONST MULTIBOOT_HEADER *
EFIAPI
GetMultibootHeader (
  IN  VOID                   *ImageAddr
  )
{
  UINT32                     AlignedAddress;
  MULTIBOOT_HEADER           *MbHeader;
  UINT32                     Offset;

  // Multiboot header must be 32-bit aligned.
  AlignedAddress = ((UINT32)(UINTN)ImageAddr + 3) & ~0x3;

  // Multiboot header must completely within the first 8192 bytes of the image.
  for (Offset = 0; Offset < 8192 - sizeof (MULTIBOOT_HEADER); Offset += 4) {
    MbHeader = (MULTIBOOT_HEADER *)(UINTN)(AlignedAddress + Offset);
    if ((MbHeader->Magic == MULTIBOOT_HEADER_MAGIC) &&
        (MbHeader->Magic + MbHeader->Flags + MbHeader->Checksum == 0)) {
      if ((MbHeader->Flags & (~SUPPORTED_FEATURES & 0xffff)) != 0) {
        // Some Multiboot features are not supported yet.
        DEBUG ((DEBUG_INFO, "Not supported Multiboot, flags=0x%x\n", MbHeader->Flags));
        return NULL;
      }

      return (CONST MULTIBOOT_HEADER *) MbHeader;
    }
  }

  return NULL;
}

/**
  Check if it is Multiboot image

  @param[in]  ImageAddr    Memory address of an image

  @retval TRUE             Image is Multiboot 0.6.98 compliant image
  @retval FALSE            Not multiboot image
**/
BOOLEAN
EFIAPI
IsMultiboot (
  IN  VOID                   *ImageAddr
  )
{
  CONST MULTIBOOT_HEADER     *MbHeader;

  MbHeader = GetMultibootHeader (ImageAddr);
  if (MbHeader != NULL) {
    DumpMbHeader (MbHeader);
    return TRUE;
  }
  return FALSE;
}


/**
  Get memory map information.

  @retval   Memory Map Info pointer
**/
STATIC
MEMORY_MAP_INFO *
EFIAPI
GetMemoryMapInfo (
  VOID
  )
{
  EFI_HOB_GUID_TYPE          *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }

  return (MEMORY_MAP_INFO *)GET_GUID_HOB_DATA (GuidHob);
}


/**
  Init Multiboot memory map.

  @param[out]  MbMmap         Multiboot memmap buffer
  @param[in]   MemoryMapInfo  Memmap buffer from boot loader
**/
VOID
InitMultibotMmap (
  OUT     MULTIBOOT_MMAP     *MbMmap,
  IN      MEMORY_MAP_INFO    *MemoryMapInfo
  )
{
  UINT32                     Index;
  MEMORY_MAP_ENTRY           *MmapEntry;

  MmapEntry = &MemoryMapInfo->Entry[0];
  for (Index = 0; Index < MemoryMapInfo->Count; Index++) {
    MbMmap[Index].Size     = sizeof (MULTIBOOT_MMAP) - sizeof (MbMmap[Index].Size);
    MbMmap[Index].BaseAddr = MmapEntry[Index].Base;
    MbMmap[Index].Length   = MmapEntry[Index].Size;
    MbMmap[Index].Type     = MmapEntry[Index].Type;
  }
}


/**
  Setup the Multiboot info for boot usage.

  @param[in,out]   MultiBoot  Point to loaded Multiboot image structure
**/
VOID
EFIAPI
SetupMultibootInfo (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  )
{
  UINT32                     MmapCount;
  MULTIBOOT_MMAP             *MbMmap;
  MEMORY_MAP_INFO            *MemoryMapInfo;
  MULTIBOOT_INFO             *MbInfo;
  EFI_HOB_GUID_TYPE          *GuidHob;
  EFI_PEI_GRAPHICS_INFO_HOB  *GfxInfoHob;

  MbInfo        = &MultiBoot->MbInfo;
  MemoryMapInfo = GetMemoryMapInfo();
  MmapCount     = MemoryMapInfo->Count;
  if (MmapCount > 0) {
    MbMmap = (MULTIBOOT_MMAP *) AllocatePool (sizeof (MULTIBOOT_MMAP) * MmapCount);
    if (MbMmap == NULL) {
      DEBUG ((DEBUG_INFO, "Multiboot MMap allocation Error\n"));
      ASSERT (MbMmap);
    }
    InitMultibotMmap (MbMmap, MemoryMapInfo);

    MbInfo->MmapAddr   = (UINT32 *) MbMmap;
    MbInfo->MmapLength = MmapCount * sizeof (MULTIBOOT_MMAP);
    MbInfo->Flags     |= MULTIBOOT_INFO_HAS_MMAP;

    // The amount of lower and upper memory size (in KB) will be updated
    // later since current memmap is not the final memmap.
  }

  // boot device [MULTIBOOT_INFO_HAS_BOOT_DEVICE]: not supported.

  if (MultiBoot->CmdFile.Size != 0) {
    MbInfo->Cmdline = MultiBoot->CmdFile.Addr;
    MbInfo->Flags  |= MULTIBOOT_INFO_HAS_CMDLINE;
  }

  // modules [MULTIBOOT_INFO_HAS_MODS]
  if (MultiBoot->MbModuleNumber > 0) {
    MbInfo->ModsAddr  = (V_ADDR)&MultiBoot->MbModule;
    MbInfo->ModsCount = MultiBoot->MbModuleNumber;
    MbInfo->Flags    |= MULTIBOOT_INFO_HAS_MODS;
  }

  // a.out symbol table [MULTIBOOT_INFO_HAS_AOUT_SYMS]: provided by a.out loader
  // ELF symbol table [MULTIBOOT_INFO_HAS_ELF_SYMS]: not supported,
  // if required, ElfLib and MultibootLib should implement additional APIs.

  // BIOS drive info [MULTIBOOT_INFO_HAS_DRIVES]: not supported
  // BIOS ROM configuration table [MULTIBOOT_INFO_HAS_CONFIG_TABLE]: not supported.

  // boot loader name:
  MbInfo->LoaderName = (UINT8 *) &mLoaderName;
  MbInfo->Flags     |= MULTIBOOT_INFO_HAS_LOADER_NAME;

  // BIOS APM table [MULTIBOOT_INFO_HAS_APM_TABLE]: not supported

  // BIOS VBE video mode information [MULTIBOOT_INFO_HAS_VBE]: not supported

  // BIOS FB framebuffer information [MULTIBOOT_INFO_HAS_FB]
  GuidHob = GetFirstGuidHob (&gEfiGraphicsInfoHobGuid);
  if (GuidHob != NULL) {
    GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GET_GUID_HOB_DATA (GuidHob);
    MbInfo = &MultiBoot->MbInfo;
    MbInfo->FramebufferAddr   = (UINT64)(UINTN)GfxInfoHob->FrameBufferBase;
    MbInfo->FramebufferPitch  = GfxInfoHob->GraphicsMode.PixelsPerScanLine * 4;
    MbInfo->FramebufferWidth  = GfxInfoHob->GraphicsMode.HorizontalResolution;
    MbInfo->FramebufferHeight = GfxInfoHob->GraphicsMode.VerticalResolution;
    MbInfo->FramebufferBpp    = 32;
    MbInfo->FramebufferType   = 1;
    MbInfo->FramebufferRedFieldPosition   = 0x10;
    MbInfo->FramebufferRedMaskSize        = 8;
    MbInfo->FramebufferGreenFieldPosition = 0x08;
    MbInfo->FramebufferGreenMaskSize      = 8;
    MbInfo->FramebufferBlueFieldPosition  = 0x00;
    MbInfo->FramebufferBlueMaskSize       = 8;
    MbInfo->Flags |= MULTIBOOT_INFO_HAS_FB;
  }

  // Arrange for passing this data to the image.
  MultiBoot->BootState.Eax = MULTIBOOT_INFO_MAGIC;
  MultiBoot->BootState.Ebx = (UINT32)(UINTN)MbInfo;
}


/**
  Align multiboot modules if required by spec.

  @param[in,out] MultiBoot   Point to loaded Multiboot image structure

  @retval  RETURN_SUCCESS     Align modules successfully
  @retval  Others             There is error when align image
**/
EFI_STATUS
EFIAPI
AlignMultibootModules (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  )
{
  MULTIBOOT_MODULE           *MbModule;
  VOID                       *AlignedAddr;
  UINT32                     ModuleSize;
  UINT32                     Index;

  for (Index = 0; Index < MultiBoot->MbModuleNumber; Index++) {
    MbModule   = &MultiBoot->MbModule[Index];
    ModuleSize = MbModule->End - MbModule->Start;
    if ((MbModule->Start & EFI_PAGE_MASK) != 0) {
      AlignedAddr = AllocatePages (EFI_SIZE_TO_PAGES (ModuleSize));
      if (AlignedAddr == NULL) {
        return RETURN_OUT_OF_RESOURCES;
      }
      DEBUG ((DEBUG_INFO, "Align Module[%d] from 0x%x to 0x%p\n",
              Index, MbModule->Start, AlignedAddr));
      CopyMem (AlignedAddr, (CONST VOID *)(UINTN)MbModule->Start, (UINTN)ModuleSize);
      MbModule->Start = (UINT32)(UINTN)AlignedAddr;
    }
  }

  return RETURN_SUCCESS;
}


/**
  Setup Multiboot image and its boot info.

  @param[in,out] MultiBoot   Point to loaded Multiboot image structure

  @retval  RETURN_SUCCESS    Setup Multiboot image successfully
  @retval  Others            There is error when setup image
**/
EFI_STATUS
EFIAPI
SetupMultibootImage (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  )
{
  EFI_STATUS                 Status;
  UINT8                      *HeaderAddr;
  UINT8                      *LoadAddr;
  UINT8                      *LoadEnd;
  UINT8                      *BssEnd;
  CONST MULTIBOOT_HEADER     *MbHeader;
  UINT8                      *CopyStart;
  UINT32                      ImgOffset;
  UINT32                      ImgLength;

  if (MultiBoot == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  MbHeader = GetMultibootHeader (MultiBoot->BootFile.Addr);
  if (MbHeader == NULL) {
    return RETURN_LOAD_ERROR;
  }

  if ((MbHeader->Flags & MULTIBOOT_HEADER_HAS_ADDR) == 0) {
    return RETURN_UNSUPPORTED;
  }

  if ((MbHeader->Flags & MULTIBOOT_HEADER_HAS_VBE) != 0) {
    // Just print the preferred graphics mode.
    DEBUG ((DEBUG_INFO, "Mb: ModeType=0x%x, Width=0x%x , Height=0x%x, Depth=0x%x\n", MbHeader->ModeType, MbHeader->Width, MbHeader->Height, MbHeader->Depth));
  }

  if ((MbHeader->Flags & MULTIBOOT_HEADER_MODS_ALIGNED) != 0) {
    // Other modules should be page (4KB) aligned
    Status = AlignMultibootModules (MultiBoot);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  HeaderAddr = MbHeader->HeaderAddr;
  LoadAddr   = MbHeader->LoadAddr;
  LoadEnd    = MbHeader->LoadEndAddr;
  BssEnd     = MbHeader->BssEndAddr;
  ImgOffset  = (UINT32)((UINT8 *)MbHeader - (UINT8 *)MultiBoot->BootFile.Addr - (HeaderAddr - LoadAddr));
  ImgLength  = (UINT32)((LoadEnd == NULL) ? MultiBoot->BootFile.Size - ImgOffset : LoadEnd - LoadAddr);
  if ((ImgOffset >= MultiBoot->BootFile.Size) || (ImgOffset + ImgLength > MultiBoot->BootFile.Size)) {
    return RETURN_LOAD_ERROR;
  }

  DEBUG ((DEBUG_INFO, "Mb: LoadAddr=0x%p, LoadEnd=0x%p , BssEnd=0x%p, Size=0x%x\n", LoadAddr, LoadEnd, BssEnd, ImgLength));
  CopyStart = (UINT8 *)MultiBoot->BootFile.Addr + ImgOffset;
  CopyMem (LoadAddr, CopyStart, ImgLength);
  if ((BssEnd != NULL) && (LoadEnd != NULL)) {
    if (BssEnd > LoadEnd) {
      ZeroMem ((VOID *) LoadEnd, BssEnd - LoadEnd);
    }
  }

  SetupMultibootInfo (MultiBoot);
  MultiBoot->BootState.EntryPoint = (UINT32)(UINTN)MbHeader->EntryAddr;
  return EFI_SUCCESS;
}


/**
  Print out the Multiboot header.

  @param[in]  Mh  The Multiboot header to be printed.
**/
VOID
DumpMbHeader (CONST MULTIBOOT_HEADER *Mh)
{
  DEBUG ((DEBUG_INFO, "\nDump MB header @%p:\n", Mh));

  DEBUG ((DEBUG_INFO, "- Magic:             %8x\n", Mh->Magic));
  DEBUG ((DEBUG_INFO, "- Flags:             %8x\n", Mh->Flags));
  DEBUG ((DEBUG_INFO, "- Checksum:          %8x\n", Mh->Checksum));

  /* Valid if mh_flags sets MULTIBOOT_HEADER_HAS_ADDR. */
  DEBUG ((DEBUG_INFO, "- HeaderAddr:        %8x\n", Mh->HeaderAddr));
  DEBUG ((DEBUG_INFO, "- LoadAddr:          %8x\n", Mh->LoadAddr));
  DEBUG ((DEBUG_INFO, "- LoadEndAddr:       %8x\n", Mh->LoadEndAddr));
  DEBUG ((DEBUG_INFO, "- BssEndAddr:        %8x\n", Mh->BssEndAddr));
  DEBUG ((DEBUG_INFO, "- EntryAddr:         %8x\n", Mh->EntryAddr));

  /* Valid if mh_flags sets MULTIBOOT_HEADER_HAS_VBE. */
  DEBUG ((DEBUG_INFO, "- ModeType:          %8x\n", Mh->ModeType));
  DEBUG ((DEBUG_INFO, "- Width:             %8x\n", Mh->Width));
  DEBUG ((DEBUG_INFO, "- Height:            %8x\n", Mh->Height));
  DEBUG ((DEBUG_INFO, "- Depth:             %8x\n", Mh->Depth));
}

/**
  Print out the Multiboot information block.

  @param[in]  Mi  The Multiboot information block to be printed.
**/
VOID
DumpMbInfo (
  IN  CONST MULTIBOOT_INFO   *Mi
  )
{
  UINT32                     Index;
  MULTIBOOT_MODULE           *Mod;
  CONST MULTIBOOT_MMAP       *Map;
  UINT32                     Offs;

  DEBUG ((DEBUG_INFO, "\nDump MB info @%p:\n", Mi));
  if (Mi == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO, "- Flags:             %8x\n", Mi->Flags));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_MEMORY.
  DEBUG ((DEBUG_INFO, "- MemLower:         %8x (%dK)\n", Mi->MemLower, Mi->MemLower));
  DEBUG ((DEBUG_INFO, "- MemUpper:         %8x (%dK)\n", Mi->MemUpper, Mi->MemUpper));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_BOOT_DEVICE.
  DEBUG ((DEBUG_INFO, "- BootDevicePart3: %8x\n", Mi->BootDevicePart3));
  DEBUG ((DEBUG_INFO, "- BootDevicePart2: %8x\n", Mi->BootDevicePart2));
  DEBUG ((DEBUG_INFO, "- BootDevicePart1: %8x\n", Mi->BootDevicePart1));
  DEBUG ((DEBUG_INFO, "- BootDeviceDrive: %8x\n", Mi->BootDeviceDrive));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_CMDLINE.
  DEBUG ((DEBUG_INFO, "- Cmdline addr:    %8p\n", Mi->Cmdline));
  if ((Mi->Flags & MULTIBOOT_INFO_HAS_CMDLINE) && Mi->Cmdline) {
    DEBUG ((DEBUG_INFO, "cmd = '%a'\n", Mi->Cmdline));
  }

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_MODS.
  DEBUG ((DEBUG_INFO, "- ModsCount:        %8x\n", Mi->ModsCount));
  DEBUG ((DEBUG_INFO, "- ModsAddr:         %8x\n", Mi->ModsAddr));
  if ((Mi->Flags & MULTIBOOT_INFO_HAS_MODS) && Mi->ModsCount > 0) {
    Mod = (MULTIBOOT_MODULE *)Mi->ModsAddr;
    for (Index = 0; Index < Mi->ModsCount; Index++) {
      DEBUG ((DEBUG_INFO, "- Mod[%d].Start:      %08x\n", Index, Mod[Index].Start));
      DEBUG ((DEBUG_INFO, "- Mod[%d].End:        %08x\n", Index, Mod[Index].End));
      DEBUG ((DEBUG_INFO, "- Mod[%d].String:     %08x\n", Index, (UINT32)(UINTN)Mod[Index].String));
    }
  }

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_{AOUT,ELF}_SYMS.
  DEBUG ((DEBUG_INFO, "- ElfshdrNum:       %8x\n", Mi->ElfshdrNum));
  DEBUG ((DEBUG_INFO, "- ElfshdrSize:      %8x\n", Mi->ElfshdrSize));
  DEBUG ((DEBUG_INFO, "- ElfshdrAddr:      %8x\n", Mi->ElfshdrAddr));
  DEBUG ((DEBUG_INFO, "- ElfshdrShndx:     %8x\n", Mi->ElfshdrShndx));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_MMAP.
  DEBUG ((DEBUG_INFO, "- MmapLength:       %8x\n", Mi->MmapLength));
  DEBUG ((DEBUG_INFO, "- MmapAddr:         %8x\n", Mi->MmapAddr));

  if ((Mi->Flags & MULTIBOOT_INFO_HAS_MMAP)) {
    for (Offs = 0; Offs < Mi->MmapLength; Offs += Map->Size + sizeof (Map->Size)) {
      Map = (VOID *)((UINTN)Mi->MmapAddr + Offs);
      DEBUG ((DEBUG_INFO, "%3x: ", Offs));
      DEBUG ((DEBUG_INFO, "%016lX", Map->BaseAddr));
      DEBUG ((DEBUG_INFO, "--%016lX", Map->Length));
      DEBUG ((DEBUG_INFO, "   %d\n", Map->Type));
    }
  }

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_DRIVES.
  DEBUG ((DEBUG_INFO, "- DrivesLength:     %8x\n", Mi->DrivesLength));
  DEBUG ((DEBUG_INFO, "- DrivesAddr:       %8x\n", Mi->DrivesAddr));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_CONFIG_TABLE.
  DEBUG ((DEBUG_INFO, "- ConfigTable:      %8p\n", Mi->UnusedConfigTable));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_LOADER_NAME.
  DEBUG ((DEBUG_INFO, "- LoaderName:       %8p\n", Mi->LoaderName));
  if ((Mi->Flags & MULTIBOOT_INFO_HAS_LOADER_NAME) && Mi->LoaderName) {
    DEBUG ((DEBUG_INFO, "  '%a'\n", Mi->LoaderName));
  }

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_APM.
  DEBUG ((DEBUG_INFO, "- ApmTable:         %8p\n", Mi->UnusedApmTable));

  // Valid if mi_flags sets MULTIBOOT_INFO_HAS_VBE.
  DEBUG ((DEBUG_INFO, "- VbeControlInfo:  %8p\n", Mi->UnusedVbeControlInfo));
  DEBUG ((DEBUG_INFO, "- VbeModeInfo:     %8p\n", Mi->UnusedVbeModeInfo));
  DEBUG ((DEBUG_INFO, "- VbeInterfaceSeg: %8x\n", Mi->UnusedVbeInterfaceSeg));
  DEBUG ((DEBUG_INFO, "- VbeInterfaceOff: %8x\n", Mi->UnusedVbeInterfaceOff));
  DEBUG ((DEBUG_INFO, "- VbeInterfaceLen: %8x\n\n", Mi->UnusedVbeInterfaceLen));
}

/**
  Print out the Multiboot boot state.

  @param[in]  BootState  The Multiboot boot state pointer.
**/
VOID
DumpMbBootState (
  IN  CONST IA32_BOOT_STATE  *BootState
  )
{
  DEBUG ((DEBUG_INFO, "Dump multiboot boot state:\n"));
  DEBUG ((DEBUG_INFO, "- EntryPoint: %4x\n", BootState->EntryPoint));
  DEBUG ((DEBUG_INFO, "-        Eax: %4x\n", BootState->Eax));
  DEBUG ((DEBUG_INFO, "-        Ebx: %4x\n", BootState->Ebx));
  DEBUG ((DEBUG_INFO, "-        Esi: %4x\n", BootState->Esi));
  DEBUG ((DEBUG_INFO, "-        Edi: %4x\n\n", BootState->Edi));
}
