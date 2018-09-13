/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  UINTN                      Index;
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
  Get the Platform info parameters.

  Pass this as a cmd line pointer to all OSes.

  @retval  PlatformInfo           The platform info data was successfully setup.
  @retval  NULL                   The platform info data could not be allocated.

**/
LOADER_PLATFORM_INFO *
GetPlatformInfo (
  VOID
  )
{
  LOADER_PLATFORM_INFO    *LoaderPlatformInfo;
  LOADER_PLATFORM_INFO    *PlatformInfo;

  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  if (LoaderPlatformInfo == NULL) {
    return NULL;
  }

  PlatformInfo = (LOADER_PLATFORM_INFO *) AllocateReservedPool (sizeof (LOADER_PLATFORM_INFO));
  if (PlatformInfo == NULL) {
    return NULL;
  }
  ZeroMem (PlatformInfo, sizeof (LOADER_PLATFORM_INFO));
  CopyMem (PlatformInfo, LoaderPlatformInfo, sizeof(LOADER_PLATFORM_INFO));

  return PlatformInfo;
}

/**
  Get SeedList info.

  Pass this as a cmd line pointer.

  @retval  SeedList           SeedList data was successfully setup.
  @retval  NULL               SeedList data could not be allocated.

**/
SEED_LIST_INFO_HOB *
GetSeedListInfo (
  VOID
  )
{
  SEED_LIST_INFO_HOB      *SeedList;
  SEED_LIST_INFO_HOB      *SeedListInfoHob;
  UINT32                  SeedListLen;

  SeedList = NULL;
  if (PcdGetBool (PcdSeedListEnabled)) {
    // Get Seed List HOB from Stage2
    SeedListInfoHob = GetSeedListInfoHOB(&SeedListLen);
    if ((SeedListInfoHob == NULL) || (SeedListLen == 0)) {
      return NULL;
    }

    // Allocate reserved memory for SeedList to pass to Trusty
    SeedList = AllocateReservedPool (SeedListLen);
    if (SeedList == NULL) {
      return NULL;
    }
    ZeroMem (SeedList, SeedListLen);
    CopyMem (SeedList, SeedListInfoHob, SeedListLen);
  }
  return SeedList;
}


/**
  Get the VMM boot parameters information

  When performing Trusty boot there are memory map requirements
  to allocate some memory for the vmm to use during its execution.

  @param[in,out]  TrustyMbi       The primary Multiboot module's information.
  @param[in,out]  BootOsMbi       The secondary Multiboot module's information.

  @retval  VmmBootParam           The VMM boot parameters were successfully setup.
  @retval  NULL                   The VMM boot parameters could not be allocated.
**/
VMM_BOOT_PARAM *
GetVmmBootParam (
  IN OUT  MULTIBOOT_INFO    *TrustyMbi,
  IN OUT  MULTIBOOT_INFO    *BootOsMbi
  )
{
  VMM_BOOT_PARAM *VmmBootParam;

  VmmBootParam = (VMM_BOOT_PARAM *) AllocateReservedPool (sizeof (VMM_BOOT_PARAM));
  if (VmmBootParam == NULL) {
    return NULL;
  }
  ZeroMem (VmmBootParam, sizeof (VMM_BOOT_PARAM));

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
  VmmBootParam->HeapAddr = (UINT32) AllocateReservedPages (EFI_SIZE_TO_PAGES (VMM_OS_HEAP_SIZE));
  // 4MB, SBL should reserve it in e820
  VmmBootParam->VmmRuntimeAddr = (UINT32) AllocateReservedPages (EFI_SIZE_TO_PAGES (VMM_OS_RUNTIME_MEM_SIZE));

  if ((VmmBootParam->HeapAddr == 0) || (VmmBootParam->VmmRuntimeAddr == 0)) {
    return NULL;
  }
  return VmmBootParam;
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

  @retval  EFI_SUCCESS          Command line was succesffuly modified.
**/
EFI_STATUS
UpdateCmdLine (
  IN OUT IMAGE_DATA        *CmdFile,
  IN     IMAGE_BOOT_PARAM  *BootParams
  )
{
  CHAR8 ParamValue[32];

  AsciiSPrint (ParamValue, sizeof (ParamValue), " ImageBootParamsAddr=0x%x", BootParams);
  AsciiStrCat ((CHAR8 *)CmdFile->Addr, ParamValue);
  CmdFile->Size = AsciiStrLen ((CHAR8 *)CmdFile->Addr);
  return EFI_SUCCESS;
}


/**
  Get Image Boot Parameter info.

  Pass this as a cmd line pointer.

  @retval  BootParams        Boot Parameter structure was successfully returned.
  @retval  NULL              Boot Parameter structure could not be allocated.
**/
IMAGE_BOOT_PARAM *
GetImageBootParamInfo (
  VOID
  )
{
  IMAGE_BOOT_PARAM          *BootParams;

  BootParams = (IMAGE_BOOT_PARAM *) AllocateReservedPool (sizeof (IMAGE_BOOT_PARAM));
  if (BootParams == NULL) {
    return NULL;
  }
  BootParams->SizeOfThisStruct = sizeof (IMAGE_BOOT_PARAM);
  BootParams->Version = 0;

  if (PcdGetBool (PcdSeedListEnabled)) {
    BootParams->SeedListInfoAddr = (UINT64) (UINTN)GetSeedListInfo ();
  }
  BootParams->PlatformInfoAddr  = (UINT64) (UINTN)GetPlatformInfo ();

  // VmmBootParams is only applicable for Trusty. Set it to zero by default
  // and update this in TrustyBoot function
  BootParams->VmmBootParamAddr = 0;

  return BootParams;
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
  IMAGE_BOOT_PARAM          *BootParams;
  VMM_BOOT_PARAM            *VmmBootParams;
  RETURN_STATUS             Status;

  //
  // Update Trusty OS command line parameter
  //
  BootParams = GetImageBootParamInfo();
  if (BootParams == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  // Get VmmBootParams is required for trusty OS
  VmmBootParams = GetVmmBootParam (&TrustyImage->MbInfo, &BootOsImage->MbInfo);
  if (VmmBootParams != NULL) {
    SetCpuState (& (VmmBootParams->CpuState), &BootOsImage->BootState);
    BootParams->VmmBootParamAddr  = (UINT64) (UINTN)VmmBootParams;
  }
  Status = UpdateCmdLine (&TrustyImage->CmdFile, BootParams);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetAttestationKeyboxInRpmb ();

  //
  // Update normal OS command line parameter
  //
  BootParams = GetImageBootParamInfo();
  if (BootParams == NULL) {
    return RETURN_OUT_OF_RESOURCES;
  }

  Status = UpdateCmdLine (&BootOsImage->CmdFile, BootParams);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

