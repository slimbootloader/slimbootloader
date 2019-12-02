/** @file
  Linux image load library

  Copyright (c) 2011 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/LinuxLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/MemoryMapInfoGuid.h>

/**
  Jump to kernel entry point.

  @param[in] KernelStart       Pointer to kernel entry point.
  @param[in] KernelBootParams  Pointer to boot parameter structure.
 **/
VOID
EFIAPI
JumpToKernel (
  IN VOID   *KernelStart,
  IN VOID   *KernelBootParams
  );

/**
  Return the memory map info HOB data.

  @retval   Pointer to the memory map info hob.
            NULL if HOB is not found.

**/
STATIC
MEMORY_MAP_INFO *
EFIAPI
GetMemoryMapInfo (
  VOID
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }
  return (MEMORY_MAP_INFO *)GET_GUID_HOB_DATA (GuidHob);
}

/**
  Dump kernel boot parameters.

  @param[in] Bp    Pointer to boot parameter structure.
 **/
VOID
DumpLinuxBootParams (
  IN BOOT_PARAMS             *Bp
  )
{
  IN SETUP_HEADER            *Header;

  if (Bp == NULL) {
    return;
  }

  Header = &Bp->Hdr;
  DEBUG ((DEBUG_INFO, "\n\n============ KERNEL SETUP ============\n"));
  DEBUG ((DEBUG_INFO, "SetupSectorss: 0x%x\n", Header->SetupSectorss));
  DEBUG ((DEBUG_INFO, "RootFlags: 0x%x\n", Header->RootFlags));
  DEBUG ((DEBUG_INFO, "SysSize: 0x%x\n", Header->SysSize));
  DEBUG ((DEBUG_INFO, "RamSize: 0x%x\n", Header->RamSize));
  DEBUG ((DEBUG_INFO, "VideoMode: 0x%x\n", Header->VideoMode));
  DEBUG ((DEBUG_INFO, "RootDev: 0x%x\n", Header->RootDev));
  DEBUG ((DEBUG_INFO, "Signature: 0x%x\n", Header->Signature));
  DEBUG ((DEBUG_INFO, "Jump: 0x%x\n", Header->Jump));
  DEBUG ((DEBUG_INFO, "Header: 0x%x\n", Header->Header));
  DEBUG ((DEBUG_INFO, "Version: 0x%x\n", Header->Version));
  DEBUG ((DEBUG_INFO, "SuSwitch: 0x%x\n", Header->SuSwitch));
  DEBUG ((DEBUG_INFO, "SetupSeg: 0x%x\n", Header->SetupSeg));
  DEBUG ((DEBUG_INFO, "StartSys: 0x%x\n", Header->StartSys));
  DEBUG ((DEBUG_INFO, "KernalVer: 0x%x\n", Header->KernalVer));
  DEBUG ((DEBUG_INFO, "LoaderId: 0x%x\n", Header->LoaderId));
  DEBUG ((DEBUG_INFO, "LoadFlags: 0x%x\n", Header->LoadFlags));
  DEBUG ((DEBUG_INFO, "MoveSize: 0x%x\n", Header->MoveSize));
  DEBUG ((DEBUG_INFO, "Code32Start: 0x%x\n", Header->Code32Start));

  DEBUG ((DEBUG_INFO, "BootsectorKludge: 0x%x\n", Header->BootsectorKludge));
  DEBUG ((DEBUG_INFO, "HeapEnd: 0x%x\n", Header->HeapEnd));
  DEBUG ((DEBUG_INFO, "ExtLoaderVer: 0x%x\n", Header->ExtLoaderVer));
  DEBUG ((DEBUG_INFO, "ExtLoaderType: 0x%x\n", Header->ExtLoaderType));

  DEBUG ((DEBUG_INFO, "CmdlineSize: 0x%x\n", Header->CmdlineSize));
  DEBUG ((DEBUG_INFO, "CmdLinePtr: 0x%x\n", Header->CmdLinePtr));
  DEBUG ((DEBUG_INFO, "Cmd Args: %a\n", (CHAR8 *)Header->CmdLinePtr));

  DEBUG ((DEBUG_INFO, "RamDiskStart: 0x%x\n", Header->RamDiskStart));
  DEBUG ((DEBUG_INFO, "RamDisklen: 0x%x\n", Header->RamDisklen));

  DEBUG ((DEBUG_INFO, "RamDiskMax: 0x%x\n", Header->RamDiskMax));
  DEBUG ((DEBUG_INFO, "KernelAlignment: 0x%x\n", Header->KernelAlignment));
  DEBUG ((DEBUG_INFO, "RelocatableKernel: 0x%x\n", Header->RelocatableKernel));
  DEBUG ((DEBUG_INFO, "MinAlignment: 0x%x\n", Header->MinAlignment));
  DEBUG ((DEBUG_INFO, "XloadFlags: 0x%x\n", Header->XloadFlags));
  DEBUG ((DEBUG_INFO, "HardwareSubarch: 0x%x\n", Header->HardwareSubarch));
  DEBUG ((DEBUG_INFO, "HardwareSubarchData: 0x%x\n", Header->HardwareSubarchData));
  DEBUG ((DEBUG_INFO, "PayloadOffset: 0x%x\n", Header->PayloadOffset));
  DEBUG ((DEBUG_INFO, "PayloadLength: 0x%x\n", Header->PayloadLength));
  DEBUG ((DEBUG_INFO, "SetupData: 0x%x\n", Header->SetupData));
  DEBUG ((DEBUG_INFO, "PrefAddress: 0x%x\n", Header->PrefAddress));
  DEBUG ((DEBUG_INFO, "InitSize: 0x%x\n", Header->InitSize));
  DEBUG ((DEBUG_INFO, "HandoverOffset: 0x%x\n\n", Header->HandoverOffset));
}

/**
  Return kernel boot parameters.

  @retval    Pointer to boot parameter structure.
 **/
BOOT_PARAMS *
EFIAPI
GetLinuxBootParams (
  VOID
  )
{
  BOOT_PARAMS *Bp;

  Bp = (BOOT_PARAMS *) (UINTN)BOOT_PARAMS_BASE;
  return Bp;
}

/**
  Check if the image is a bootable Linux image.

  @param[in]  ImageBase      Memory address of an image

  @retval     TRUE           Image is a bootable kernel image
  @retval     FALSE          Not a bootable kernel image
**/
BOOLEAN
EFIAPI
IsBzImage (
  IN  CONST VOID             *ImageBase
  )
{
  BOOT_PARAMS                *Bp;

  Bp = (BOOT_PARAMS *) ImageBase;
  if (Bp == NULL) {
    return FALSE;
  }

  // Check boot sector Signature
  if ((Bp->Hdr.Signature != 0xAA55) || (Bp->Hdr.Header != SETUP_HDR)) {
    DEBUG ((DEBUG_ERROR, "This image is not in bzimage format\n"));
    return FALSE;
  }

  DEBUG ((DEBUG_INFO, "Found bzimage Signature\n"));
  return TRUE;
}

/**
  Load linux kernel image to specified address and setup boot parameters.

  @param[in]  KernelBase     Memory address of an kernel image.
  @param[in]  InitRdBase     Memory address of an InitRd image.
  @param[in]  InitRdLen      InitRd image size.
  @param[in]  CmdLineBase    Memory address of command line buffer.
  @param[in]  CmdLineLen     Command line buffer size.

  @retval EFI_INVALID_PARAMETER   Input parameters are not valid.
  @retval EFI_UNSUPPORTED         Unsupported binary type.
  @retval EFI_SUCCESS             Kernel is loaded successfully.
**/
EFI_STATUS
EFIAPI
LoadBzImage (
  IN  CONST VOID                  *KernelBase,
  IN  CONST VOID                  *InitRdBase,
  IN      UINT32                   InitRdLen,
  IN  CONST VOID                  *CmdLineBase,
  IN      UINT32                   CmdLineLen
  )
{
  BOOT_PARAMS                *Bp;
  BOOT_PARAMS                *BaseBp;
  VOID                       *KernelBuf;
  UINT32                      BootParamSize;
  UINTN                       KernelSize;
  VOID CONST                 *ImageBase;

  ImageBase = KernelBase;
  if (ImageBase == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsBzImage (ImageBase)) {
    return EFI_UNSUPPORTED;
  }

  BaseBp = (BOOT_PARAMS *) ImageBase;
  Bp = GetLinuxBootParams ();
  ZeroMem ((VOID *)Bp, sizeof (BOOT_PARAMS));
  Bp->Hdr = BaseBp->Hdr;

  if (Bp->Hdr.SetupSectorss != 0) {
    BootParamSize = (Bp->Hdr.SetupSectorss + 1) * 512;
  } else {
    BootParamSize = 5 * 512;
  }

  KernelBuf  = (VOID *) (UINTN)LINUX_KERNEL_BASE;
  KernelSize = Bp->Hdr.SysSize * 16;
  CopyMem (KernelBuf, (UINT8 *)ImageBase + BootParamSize, KernelSize);

  //
  // Update boot params
  //
  Bp->Hdr.LoaderId     = 0xff;
  Bp->Hdr.CmdLinePtr   = (UINT32)CmdLineBase;
  Bp->Hdr.CmdlineSize  = CmdLineLen;
  Bp->Hdr.RamDiskStart = (UINT32)InitRdBase;
  Bp->Hdr.RamDisklen   = InitRdLen;

  return EFI_SUCCESS;
}

/**
  Update linux kernel boot parameters.

  @retval EFI_SUCCESS        Linux boot parameters were updated successfully.
**/
VOID
EFIAPI
UpdateLinuxBootParams (
  VOID
  )
{
  BOOT_PARAMS                *Bp;
  EFI_HOB_GUID_TYPE          *GuidHob;
  EFI_PEI_GRAPHICS_INFO_HOB  *GfxInfoHob;
  UINTN                       MemoryMapSize;
  E820_ENTRY                 *E820Entry;
  MEMORY_MAP_INFO            *MapInfo;
  UINTN                       Index;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GfxMode;

  Bp = GetLinuxBootParams ();

  //
  // Get graphics data
  //
  GuidHob = GetFirstGuidHob (&gEfiGraphicsInfoHobGuid);
  if (GuidHob != NULL) {
    GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GET_GUID_HOB_DATA (GuidHob);
    ZeroMem (&Bp->ScreenInfo, sizeof (Bp->ScreenInfo));
    GfxMode = &GfxInfoHob->GraphicsMode;
    if (GfxMode->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {
      Bp->ScreenInfo.RedPos   = 0;
      Bp->ScreenInfo.BluePos  = 16;
    } else if (GfxMode->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
      Bp->ScreenInfo.RedPos   = 16;
      Bp->ScreenInfo.BluePos  = 0;
    } else {
      // Unsupported format
      GfxMode = NULL;
    }
    if (GfxMode != NULL) {
      Bp->ScreenInfo.OrigVideoIsVGA  = VIDEO_TYPE_EFI;
      Bp->ScreenInfo.LfbLinelength   = (UINT16) (GfxMode->PixelsPerScanLine * 4);
      Bp->ScreenInfo.LfbDepth        = 32;
      Bp->ScreenInfo.LfbBase         = (UINT32)(UINTN)GfxInfoHob->FrameBufferBase;
      Bp->ScreenInfo.LfbWidth        = (UINT16)GfxMode->HorizontalResolution;
      Bp->ScreenInfo.LfbHeight       = (UINT16)GfxMode->VerticalResolution;
      Bp->ScreenInfo.Pages           = 1;
      Bp->ScreenInfo.RedSize         = 8;
      Bp->ScreenInfo.GreenSize       = 8;
      Bp->ScreenInfo.BlueSize        = 8;
      Bp->ScreenInfo.RsvdSize        = 8;
      Bp->ScreenInfo.GreenPos        = 8;
      Bp->ScreenInfo.RsvdPos         = 24;
    }
  }

  // Get memory map
  E820Entry = &Bp->E820Map[0];
  MemoryMapSize = (UINTN)ARRAY_SIZE (Bp->E820Map);
  MapInfo = GetMemoryMapInfo();
  for (Index = 0; Index < MapInfo->Count && Index < MemoryMapSize; Index++) {
    E820Entry->Type = (UINT32)MapInfo->Entry[Index].Type;
    E820Entry->Addr = MapInfo->Entry[Index].Base;
    E820Entry->Size = MapInfo->Entry[Index].Size;
    E820Entry++;
  }
  Bp->E820Entries = (UINT8)MapInfo->Count;

}

/**
  Load linux kernel image to specified address and setup boot parameters.

  @param[in]  HobList    HOB list pointer. Not used for now.
  @param[in]  Params     Extra parameters. Not used for now.
**/
VOID
EFIAPI
LinuxBoot (
  IN VOID   *HobList,
  IN VOID   *Params
  )
{
  BOOT_PARAMS   *Bp;

  UpdateLinuxBootParams ();
  Bp = GetLinuxBootParams ();
  JumpToKernel ((VOID *)Bp->Hdr.Code32Start, Bp);
}