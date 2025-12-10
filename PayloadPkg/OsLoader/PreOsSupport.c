/** @file

  Copyright (c) 2019 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/DebugLib.h>
#include <Library/ContainerLib.h>
#include <Library/ElfLib.h>
#include <Library/LinuxLib.h>
#include <Library/MultibootLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include "OsLoader.h"


/**
  Get required information from loaded OS image.

  @param[in]  LoadedImage         Loaded OS image information.
  @param[out] PreOsParams         Pointer of PRE_OS_PARAM

  @retval EFI_INVALID_PARAMETER   Invalid parameter is given or loaded image flag is not set.
  @retval EFI_SUCCESS             Get required information success.

**/
EFI_STATUS
EFIAPI
GetNormalOsInfo (
  IN   LOADED_IMAGE           *LoadedImage,
  OUT  PRE_OS_PARAM           *PreOsParams
  )
{
  BOOT_PARAMS                *OsBootParam;
  IA32_BOOT_STATE            *BootState;

  if (PreOsParams == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (PreOsParams, sizeof (PRE_OS_PARAM));

  PreOsParams->Version     = 0x1;
  PreOsParams->HeapSize    = EFI_SIZE_TO_PAGES (0);
  PreOsParams->HeapAddr    = (UINT32)(UINTN)AllocatePages (PreOsParams->HeapSize);
  PreOsParams->HobListPtr  = PcdGet32 (PcdPayloadHobList);

  if ((LoadedImage->Flags & LOADED_IMAGE_LINUX) != 0) {
    OsBootParam = GetLinuxBootParams();
    UpdateLinuxBootParams (OsBootParam);
    PreOsParams->OsBootState.Esi     = (UINT32)(UINTN)OsBootParam;
    PreOsParams->OsBootState.Eip     = OsBootParam->Hdr.Code32Start;
  } else if ((LoadedImage->Flags & (LOADED_IMAGE_ELF | LOADED_IMAGE_MULTIBOOT)) != 0) {
    DEBUG((DEBUG_INFO, "POSC to boot %a\n", (LoadedImage->Flags & LOADED_IMAGE_ELF)?"elf":"mb"));
    BootState = &LoadedImage->Image.MultiBoot.BootState;
    PreOsParams->OsBootState.Eax     = BootState->Eax;
    PreOsParams->OsBootState.Ebx     = BootState->Ebx;
    PreOsParams->OsBootState.Esi     = BootState->Esi;
    PreOsParams->OsBootState.Edi     = BootState->Edi;
    PreOsParams->OsBootState.Eip     = BootState->EntryPoint;
  } else if ((LoadedImage->Flags & LOADED_IMAGE_PE) != 0) {
    BootState = &LoadedImage->Image.MultiBoot.BootState;
    PreOsParams->OsBootState.Eip     = BootState->EntryPoint;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  PreOsParams->OsBootState.Eflags  = 0;

  return EFI_SUCCESS;
}


/**
  Start preOS boot image

  This function will call into preOS entry point with OS information as parameter.

  @param[in]  LoadedPreOsImage  Loaded PreOS image information.
  @param[in]  LoadedImage       Loaded OS image information.

  @retval  RETURN_SUCCESS       boot image is return after boot
  @retval  Others               There is error when checking boot image
**/
EFI_STATUS
StartPreOsBooting (
  IN LOADED_IMAGE            *LoadedPreOsImage,
  IN LOADED_IMAGE            *LoadedImage
  )
{
  EFI_STATUS                 Status;
  PRE_OS_PARAM               PreOsParams;
  IA32_BOOT_STATE            *BootState;
  PRE_OS_ENTRYPOINT          EntryPoint;

  DEBUG ((DEBUG_INFO, "StartPreOsBooting...\n"));

  DEBUG_CODE_BEGIN();
  PrintStackHeapInfo ();
  DEBUG_CODE_END();

  DEBUG ((DEBUG_INFO, "GetNormalOsInfo...\n"));

  Status = GetNormalOsInfo (LoadedImage, &PreOsParams);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  DEBUG ((DEBUG_INFO, "GetNormalOsInfo...%r\n", Status));

  BootState  = &LoadedPreOsImage->Image.MultiBoot.BootState;
  EntryPoint = (PRE_OS_ENTRYPOINT)(UINTN)BootState->EntryPoint;
  DEBUG ((DEBUG_INFO, "EntryPoint =...0x%p\n", EntryPoint));

  EntryPoint (&PreOsParams);

  //
  // Should not reach here
  //
  CpuHalt ("Failed to launch Pre-OS image!");

  return EFI_DEVICE_ERROR;
}

