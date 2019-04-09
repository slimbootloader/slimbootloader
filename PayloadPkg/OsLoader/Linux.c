/** @file

  Copyright (c) 2011 - 2019, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "OsLoader.h"

/**
  Populates the memory map info from HOB on to Boot param.

  @param[out]  Bp   Pointer to the boot parameter structure.

 **/
VOID
GetLinuxMemMap (
  OUT BOOT_PARAMS            *Bp
  )
{
  UINTN                      MemoryMapSize;
  E820_ENTRY                 *E820Entry;
  MEMORY_MAP_INFO            *MapInfo;
  UINTN                      Index;

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
  Dumps kernel setup values.

  @param[in] Header  Pointer to setup Header structure.
 **/
VOID
DumpSetupHeader (
  IN SETUP_HEADER            *Header
  )
{
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
  Dumps kernel boot parameters.

  @param[in] Bp  Pointer to boot parameter structure.
 **/
VOID
DumpBootParameters (
  IN BOOT_PARAMS             *Bp
  )
{
  DumpSetupHeader (&Bp->Hdr);
}


/**
  Dumps the content of the buffer starting from Address.

  @param[in]  Address   Start address of buffer to dump
  @param[in]  Length    Size of the buffer to dump

 **/
VOID
DumpBuffer (
  IN CHAR8                   *Address,
  IN UINT32                  Length
  )
{
  UINT32                     Index;

  DEBUG ((DEBUG_INFO, "Dump buffer from:0x%p, length %x\n", Address, Length));
  for (Index = 0; Index < Length; Index++) {
    if ( (Index % 16) == 0) {
      DEBUG ((DEBUG_INFO, "\n0x%08x:", Index));
    }
    DEBUG ((DEBUG_INFO, " %02x ", Address[Index] & 0xFF));
  }
  DEBUG ((DEBUG_INFO, "\n\n"));
}

/**
  Checks weather the given image is bzimage.

  @param  KernelSetup               Pointer to the boot parameter structure.

  @retval EFI_SUCCESS               Found bzimage
  @retval EFI_UNSUPPORTED           if image is not bzimage
  @retval EFI_INVALID_PARAMETER     if KernelSetup is NULL
 **/
EFI_STATUS
EFIAPI
LoadLinuxCheckKernelSetup (
  IN VOID        *KernelSetup
  )
{
  BOOT_PARAMS *Bp;

  Bp = KernelSetup;

  if (Bp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Check boot sector Signature
  if ((Bp->Hdr.Signature != 0xAA55) || (Bp->Hdr.Header != SETUP_HDR)) {
    DEBUG ((DEBUG_ERROR, "This image is not in bzimage format\n"));
    return EFI_UNSUPPORTED;
  } else {
    DEBUG ((DEBUG_INFO, "Found bzimage Signature\n"));
    return EFI_SUCCESS;
  }
}


/**
  Setup linux kernel and boot parameter table

  This function will check if boot image is bzImage firstly.
  If yes, will copy kernel to address 0x100000, and copy boot parameters
  to 0x90000. And also update boot parametere fields.

  @param[in]  LinuxImage        Loaded Linux image information.

  @retval  RETURN_SUCCESS       Setup bzImage kernel and boot parameter successfully
  @retval  Others               There is error when setup bzImage and boot parameters
**/
EFI_STATUS
SetupBzImage (
  IN LINUX_IMAGE             *LinuxImage
  )
{
  EFI_STATUS                 Status;
  VOID                       *KernelBuf;
  UINTN                      KernelSize;
  BOOT_PARAMS                *Bp;
  BOOT_PARAMS                *BaseBp;
  UINT32                      BootParamSize;
  EFI_HOB_GUID_TYPE          *GuidHob;
  EFI_PEI_GRAPHICS_INFO_HOB  *GfxInfoHob;
  UINT8                      *Buffer;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GfxMode;

  DEBUG ((DEBUG_INFO, "Setup bzImage boot parameters ...\n"));
  Buffer = LinuxImage->BootFile.Addr;
  BaseBp = (BOOT_PARAMS *) Buffer;
  Status = LoadLinuxCheckKernelSetup ((VOID *) BaseBp);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "LoadLinuxCheckKernelSetup Error...%r\n", Status));
    return EFI_INVALID_PARAMETER;
  }

  Bp = (BOOT_PARAMS *) (UINTN)BOOT_PARAMS_BASE;
  LinuxImage->BootParams = Bp;
  ZeroMem ((VOID *)Bp, sizeof (BOOT_PARAMS));
  Bp->Hdr = BaseBp->Hdr;

  if (Bp->Hdr.SetupSectorss != 0) {
    BootParamSize = (Bp->Hdr.SetupSectorss + 1) * 512;
  } else {
    BootParamSize = 5 * 512;
  }

  KernelBuf  = (VOID *) (UINTN)LINUX_KERNEL_BASE;
  KernelSize = Bp->Hdr.SysSize * 16;
  DEBUG ((DEBUG_INFO, "Src=0x%p Dest=0x%p KernelSize=%d\n", Buffer + BootParamSize, KernelBuf, KernelSize));
  if (KernelSize > IMAGE_MAX_SIZE) {
    DEBUG ((DEBUG_ERROR, "Linux kernel too big!\n"));
    Status = EFI_LOAD_ERROR;
    return Status;
  }
  CopyMem (KernelBuf, Buffer + BootParamSize, KernelSize);

  //
  // Update boot params
  //
  Bp->Hdr.LoaderId     = 0xff;
  Bp->Hdr.CmdLinePtr   = (UINT32) (UINTN)LinuxImage->CmdFile.Addr;
  Bp->Hdr.CmdlineSize  = LinuxImage->CmdFile.Size;
  Bp->Hdr.RamDiskStart = (UINT32) (UINTN)LinuxImage->InitrdFile.Addr;
  Bp->Hdr.RamDisklen   = LinuxImage->InitrdFile.Size;

  //
  // Get graphics data
  //
  GuidHob = GetFirstGuidHob (&gEfiGraphicsInfoHobGuid);
  if (GuidHob != NULL) {
    GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GET_GUID_HOB_DATA (GuidHob);
    ZeroMem (&Bp->ScreenInfo, sizeof (Bp->ScreenInfo));
    GfxMode = &GfxInfoHob->GraphicsMode;
    Bp->ScreenInfo.OrigVideoIsVGA  = VIDEO_TYPE_EFI;
    Bp->ScreenInfo.LfbLinelength   = (UINT16) (GfxMode->PixelsPerScanLine * 4);
    Bp->ScreenInfo.LfbDepth        = 32;
    Bp->ScreenInfo.LfbBase         = (UINT32)(UINTN)GfxInfoHob->FrameBufferBase;
    Bp->ScreenInfo.LfbWidth        = (UINT16)GfxMode->HorizontalResolution;
    Bp->ScreenInfo.LfbHeight       = (UINT16)GfxMode->VerticalResolution;
    Bp->ScreenInfo.Pages           = 1;
    Bp->ScreenInfo.BlueSize        = 8;
    Bp->ScreenInfo.BluePos         = GET_POS_FROM_MASK (GfxMode->PixelInformation.BlueMask);
    Bp->ScreenInfo.GreenSize       = 8;
    Bp->ScreenInfo.GreenPos        = GET_POS_FROM_MASK (GfxMode->PixelInformation.GreenMask);
    Bp->ScreenInfo.RedSize         = 8;
    Bp->ScreenInfo.RedPos          = GET_POS_FROM_MASK (GfxMode->PixelInformation.RedMask);
    Bp->ScreenInfo.RsvdSize        = 8;
    Bp->ScreenInfo.RsvdPos         = GET_POS_FROM_MASK (GfxMode->PixelInformation.ReservedMask);
  }

  if (Bp->Hdr.Version < 0x205 || (!Bp->Hdr.RelocatableKernel && (Bp->Hdr.Code32Start != LINUX_KERNEL_BASE))) {
    //
    // Only support relocatable kernels or non-relocatable kernel built with base LINUX_KERNEL_BASE
    //
    DEBUG ((DEBUG_ERROR, "Only support relocatable kernels  and Kernel Version >= 0x205"));
    return EFI_UNSUPPORTED;
  }

  Bp->Hdr.Code32Start = (UINT32) (UINTN) KernelBuf;

  //
  // Get memory map
  //
  GetLinuxMemMap (Bp);
  return Status;
}



