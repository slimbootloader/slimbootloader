/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include  "TrustyBoot.h"


/**
  Update OS memory map to reserve memory under 1MB

  @param[in,out] MbInfo       Multi boot info
  @param[in]     MemorySize   The memory size to reserve

  @retval        0            Failed to get reserved memory
  @retval        addr         Reserved memory address.
**/
UINT32
ReserveMemUnder1Mb (
  IN OUT MULTIBOOT_INFO      *MbInfo,
  IN     UINT32              MemorySize
  )
{
  UINT32                     Index;
  UINT32                     Count;
  MULTIBOOT_MMAP             *MbMmap;
  UINT32                     FreeMemoryIndex;

  MbMmap = (MULTIBOOT_MMAP *) MbInfo->MmapAddr;
  Count  = MbInfo->MmapLength / sizeof (MULTIBOOT_MMAP);
  // Find the first reserved region under 1MB.
  for (Index = 0; Index < Count; Index++) {
    if (MbMmap[Index].Type == MEM_MAP_TYPE_RESERVED) {
      break;
    }
  }

  if ((Index == 0) || (Index == Count)) {
    return 0;
  }

  if (MbMmap[Index].BaseAddr >= MB_ (1)) {
    return 0;
  }

  FreeMemoryIndex = Index - 1;
  if (MbMmap[FreeMemoryIndex].Length < MemorySize) {
    return 0;
  }

  MbMmap[FreeMemoryIndex].Length  -= MemorySize;
  MbMmap[Index].BaseAddr          -= MemorySize;
  MbMmap[Index].Length            += MemorySize;

  return (UINT32) MbMmap[Index].BaseAddr;
}

/**
  Get the VMM boot parameters information

  When performing Trusty boot there are memory map requirements
  to allocate some memory for the vmm to use during its execution.

  @param[in,out]  VmmImageData    The VMM image data pointer which manages reserved memories for VMM
  @param[in,out]  TrustyMbi       The primary Multiboot module's information.
  @param[in,out]  BootOsMbi       The secondary Multiboot module's information.

  @retval         EFI_SUCCESS     The VMM boot parameters were successfully setup.
  @retval         Others          The VMM boot parameters could not be allocated.
**/
EFI_STATUS
GetVmmBootParam (
  IN OUT  VMM_IMAGE_DATA    *VmmImageData,
  IN OUT  MULTIBOOT_INFO    *TrustyMbi,
  IN OUT  MULTIBOOT_INFO    *BootOsMbi
  )
{
  VMM_BOOT_PARAM *VmmBootParam;
  UINT32          Size;

  ZeroMem (VmmImageData, sizeof (VMM_IMAGE_DATA));

  //
  // Reserve Memory for Vmm Boot Params
  //
  Size = sizeof (VMM_BOOT_PARAM);
  VmmImageData->VmmBootParams.Addr = AllocateReservedPool (Size);
  if (VmmImageData->VmmBootParams.Addr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VmmImageData->VmmBootParams.Size = Size;

  //
  // Reserve Memory for Vmm Heap Memory
  //
  Size = EFI_SIZE_TO_PAGES (VMM_OS_HEAP_SIZE);
  VmmImageData->VmmHeapAddr.Addr = AllocateReservedPages (Size);
  if (VmmImageData->VmmHeapAddr.Addr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VmmImageData->VmmHeapAddr.Size = VMM_OS_HEAP_SIZE;
  VmmImageData->VmmHeapAddr.AllocType = ImageAllocateTypePage;

  //
  // Reserve Memory for Vmm Runtime Memory
  //
  Size = EFI_SIZE_TO_PAGES (VMM_OS_RUNTIME_MEM_SIZE);
  VmmImageData->VmmRuntimeAddr.Addr = AllocateReservedPages (Size);
  if (VmmImageData->VmmRuntimeAddr.Addr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VmmImageData->VmmRuntimeAddr.Size = VMM_OS_RUNTIME_MEM_SIZE;
  VmmImageData->VmmRuntimeAddr.AllocType = ImageAllocateTypePage;

  //
  // Update VmmBootParam Info
  //
  VmmBootParam = (VMM_BOOT_PARAM *)VmmImageData->VmmBootParams.Addr;
  VmmBootParam->SizeOfThisStruct = sizeof (VMM_BOOT_PARAM);

  // 4KB under 1M reserved in e820.
  // In most platforms, memory range 0xA0000 ~ 0xFFFFF are reserved already.
  // 0x9F000 ~ 0x9FFFF might be accessed by Linux kernel for MP table.
  // So reserve 4KB more here to avoid reserve 0x9F000 ~ 0x9FFFF range.
  VmmBootParam->SipiPage = ReserveMemUnder1Mb (TrustyMbi, KB_ (8));
  ASSERT (VmmBootParam->SipiPage != 0);
  // Update second OS memmap so that it knows the memory map changes.
  ReserveMemUnder1Mb (BootOsMbi, KB_ (8));

  // 64KB, SBL should reserve it in e820
  VmmBootParam->HeapAddr = (UINT32)(UINTN)VmmImageData->VmmHeapAddr.Addr;
  // 4MB, SBL should reserve it in e820
  VmmBootParam->VmmRuntimeAddr = (UINT32)(UINTN)VmmImageData->VmmRuntimeAddr.Addr;

  return EFI_SUCCESS;
}

/**
  Set the CPU state for the VMM to launch OS.

  When performing Trusty boot the primary Multiboot module will need
  the secondary Multiboot module's boot state information in order
  to launch it later.

  @param[in,out]  VmmOsBootState  The boot state information required to
                                  boot the secondary Multiboot module
  @param[in]      SecondaryState  The boot state information required to
                                  boot the secondary Multiboot module
**/
VOID
SetCpuState (
  IN OUT CPU_BOOT_STATE   *VmmOsBootState,
  IN     IA32_BOOT_STATE  *SecondaryState
  )
{
  VmmOsBootState->Eip = SecondaryState->EntryPoint;
  VmmOsBootState->Eax = SecondaryState->Eax;
  VmmOsBootState->Ebx = SecondaryState->Ebx;
  VmmOsBootState->Esi = SecondaryState->Esi;
  VmmOsBootState->Edi = SecondaryState->Edi;
  VmmOsBootState->Ecx = SecondaryState->EntryPoint;
}

/**
  The AttestationKeybox should be encrypted with a derivative of RPMB key

  The boot loader should also check if AttestationKeybox already exists in RPMB.
  If not get it from CSE and put it to RPMB. The AttestationKeybox should be
  encrypted with a derivative of RPMB key.

  @retval  RETURN_SUCCESS       The RPMB attestation keybox was successfully setup.
**/
RETURN_STATUS
SetAttestationKeyboxInRpmb (
  VOID
  )
{
  // TODO: needs to be implemented
  return RETURN_SUCCESS;
}

/**
  Update OS command line parameters

  When performing Trusty boot the OS module may need a pointer to the
  IMAGE_BOOT_PARAM structure containing all of the information required
  to perform Trusty boot. This is specified via the command line.

  @param[in,out] CmdFile        Pointer to IMAGE_DATA for Command file
  @param[in]     BootParams     Pointer to the IMAGE_BOOT_PARAM structure
                                that needs to be passed via command line
  @param[in]     CmdBufLen      Command buffer length.

  @retval  EFI_SUCCESS          Command line was succesffuly modified.
**/
EFI_STATUS
UpdateCmdLine (
  IN OUT IMAGE_DATA        *CmdFile,
  IN     IMAGE_BOOT_PARAM  *BootParams,
  IN     UINT32             CmdBufLen
  )
{
  CHAR8 ParamValue[32];

  AsciiSPrint (ParamValue, sizeof (ParamValue), " ImageBootParamsAddr=0x%x", BootParams);
  AsciiStrCatS ((CHAR8 *)CmdFile->Addr, CmdBufLen, ParamValue);
  CmdFile->Size = (UINT32)AsciiStrLen ((CHAR8 *)CmdFile->Addr);
  return EFI_SUCCESS;
}


/**
  Get Image Boot Parameter info.

  Pass this as a cmd line pointer.

  @param[in,out]  BootParamsData    The Boot Parameter data pointer which manages reserved memories for Boot Parameter

  @retval         EFI_SUCCESS       Boot Parameter structure was successfully built.
  @retval         Others            Boot Parameter structure could not be allocated.
**/
EFI_STATUS
GetImageBootParamInfo (
  IN OUT  BOOTPARAMS_IMAGE_DATA   *BootParamsData
  )
{
  IMAGE_BOOT_PARAM          *BootParams;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;
  SEED_LIST_INFO_HOB        *SeedListInfoHob;
  UINT32                     Length;

  if (BootParamsData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ZeroMem (BootParamsData, sizeof (BOOTPARAMS_IMAGE_DATA));
  BootParamsData->Base.Addr = AllocateReservedPool (sizeof (IMAGE_BOOT_PARAM));
  if (BootParamsData->Base.Addr == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }
  BootParamsData->Base.Size = sizeof (IMAGE_BOOT_PARAM);

  BootParams = (IMAGE_BOOT_PARAM *)BootParamsData->Base.Addr;
  BootParams->SizeOfThisStruct = sizeof (IMAGE_BOOT_PARAM);
  BootParams->Version = 0;

  // Update SEED List
  if (PcdGetBool (PcdSeedListEnabled)) {
    SeedListInfoHob = GetSeedListInfoHOB (&Length);
    if ((SeedListInfoHob == NULL) || (Length == 0)) {
      return EFI_NOT_FOUND;
    }
    // Allocate reserved memory for SeedList to pass to Trusty
    BootParamsData->SeedList.Addr = AllocateReservedPool (Length);
    if (BootParamsData->SeedList.Addr == NULL) {
      return RETURN_OUT_OF_RESOURCES;
    }
    BootParamsData->SeedList.Size = Length;

    ZeroMem (BootParamsData->SeedList.Addr, Length);
    CopyMem (BootParamsData->SeedList.Addr, SeedListInfoHob, Length);
  }

  // Update Platform Info
  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  if (LoaderPlatformInfo != NULL) {
    Length = sizeof (LOADER_PLATFORM_INFO);
    BootParamsData->PlatformInfo.Addr = (LOADER_PLATFORM_INFO *) AllocateReservedPool (Length);
    if (BootParamsData->PlatformInfo.Addr != NULL) {
      BootParamsData->PlatformInfo.Size = Length;
      ZeroMem (BootParamsData->PlatformInfo.Addr, Length);
      CopyMem (BootParamsData->PlatformInfo.Addr, LoaderPlatformInfo, Length);
    }
  }

  return EFI_SUCCESS;
}

/**
  Setup the dependencies for performing Trusty boot.

  @param[in,out]  TrustyImage     The trusty image pointer.
  @param[in,out]  BootOsImage     The normal OS image pointer.

  @retval  RETURN_SUCCESS         Trusty boot setup was completed.
  @retval  Others                 Encountered an error setting up for Trusty boot.
**/
EFI_STATUS
EFIAPI
SetupTrustyBoot (
  IN OUT  MULTIBOOT_IMAGE    *TrustyImage,
  IN OUT  MULTIBOOT_IMAGE    *BootOsImage
  )
{
  RETURN_STATUS              Status;
  IMAGE_BOOT_PARAM          *BootParams;
  VMM_BOOT_PARAM            *VmmBootParams;
  BOOTPARAMS_IMAGE_DATA     *TrustyBootParamsData;
  BOOTPARAMS_IMAGE_DATA     *NormalBootParamsData;
  VMM_IMAGE_DATA            *VmmImageData;

  //
  // Update Trusty OS command line parameter
  //
  TrustyBootParamsData = &TrustyImage->TrustyImageData.BootParamsData;
  Status = GetImageBootParamInfo (TrustyBootParamsData);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BootParams = (IMAGE_BOOT_PARAM *) TrustyBootParamsData->Base.Addr;

  //
  // Get VmmBootParams is required for trusty OS
  //
  VmmImageData = &TrustyImage->TrustyImageData.VmmImageData;
  Status = GetVmmBootParam (VmmImageData, &TrustyImage->MbInfo, &BootOsImage->MbInfo);
  if (!EFI_ERROR (Status)) {
    VmmBootParams = (VMM_BOOT_PARAM *)VmmImageData->VmmBootParams.Addr;
    SetCpuState (&VmmBootParams->CpuState, &BootOsImage->BootState);
    BootParams->VmmBootParamAddr = (UINT64) (UINTN)VmmBootParams;
  } else {
    return Status;
  }

  Status = UpdateCmdLine (&TrustyImage->CmdFile, BootParams, TrustyImage->CmdBufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetAttestationKeyboxInRpmb ();

  //
  // Update normal OS command line parameter
  //
  NormalBootParamsData = &BootOsImage->TrustyImageData.BootParamsData;
  Status = GetImageBootParamInfo (NormalBootParamsData);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  BootParams = (IMAGE_BOOT_PARAM *) NormalBootParamsData->Base.Addr;

  Status = UpdateCmdLine (&BootOsImage->CmdFile, BootParams, BootOsImage->CmdBufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
