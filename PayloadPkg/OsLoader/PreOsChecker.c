/** @file

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
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

#define SIGNATURE_IPFW    SIGNATURE_32 ('I', 'P', 'F', 'W')
#define SIGNATURE_POSC    SIGNATURE_32 ('P', 'O', 'S', 'C')

typedef struct {
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;
  UINT32 Esi;
  UINT32 Edi;
  UINT32 Ebp;
  UINT32 Eip;
  UINT32 Eflags;
} OS_BOOT_STATE;

typedef struct {
  UINT32        Version;
  UINT32        HeapSize;
  UINT32        HeapAddr;
  OS_BOOT_STATE OsBootState;
  UINT32        HobListPtr;
} PRE_OS_PAYLOAD_PARAM;

typedef VOID (*PRE_OS_CHECKER_ENTRY) (VOID *Params);

STATIC VOID *mPreOsCheckerEntry = NULL;

/**
  To see whether PreOsChecker is loaded properly

  @retval           TRUE if successfully loaded, otherwise FALSE

**/
BOOLEAN
EFIAPI
IsPreOsCheckerLoaded (
  IN  VOID
  )
{
  return ((mPreOsCheckerEntry == NULL) ? FALSE : TRUE);
}

/**
  Search for and load pre-OS checker for execution.

  This function will search for a pre-OS checker binary that might
  be loaded as part of the payload, if found get the entry point for
  execution later instead of jumping into the OS directly.

  @retval  EFI_LOAD_ERROR         Pre-OS checker is not loaded successfully.
  @retval  EFI_INVALID_PARAMETER  Invalid parameter is used at loading PreOsChecker image
  @retval  EFI_UNSUPPORTED        Pre-OS checker image is not supported

**/
EFI_STATUS
EFIAPI
LoadPreOsChecker (
  IN  VOID
  )
{
  EFI_STATUS         Status;
  UINT32             Length;
  VOID              *Buffer;

  //
  // TBD: Set to NULL to re-load PreOsChecker in case of restarting Payload for now.
  //
  mPreOsCheckerEntry = NULL;
  //if (mPreOsCheckerEntry != NULL) {
  //  DEBUG ((DEBUG_INFO, "Pre-OS checker is already loaded at entry @ 0x%p\n", mPreOsCheckerEntry));
  //  return EFI_STATUS;
  //}

  //
  // Check if there is a pre-OS checker that needs to be executed
  //
  Buffer = NULL;
  Length = 0;
  Status = LoadComponent (SIGNATURE_IPFW, SIGNATURE_POSC, &Buffer, &Length);

  if (EFI_ERROR (Status) || (Buffer == NULL) || (Length == 0)) {
    return EFI_NOT_FOUND;
  }

  Status = LoadElfImage (Buffer, &mPreOsCheckerEntry);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Pre-OS checker entry @ 0x%p\n", mPreOsCheckerEntry));
  }

  return Status;
}

/**
  Start PreOsChecker with OS information

  @param[in] BootParam            Pointer of OS BOOT_PARAMS or IA32_BOOT_STATE
  @param[in] ImageFlags           Flags of the loaded image

  @retval EFI_NOT_FOUND           PreOsChecker not found
  @retval EFI_INVALID_PARAMETER   Invalid OS BOOT_PARAMS found
  @retval none                    No EFI_SUCCESS if PreOsChecker is executed properly

**/
EFI_STATUS
EFIAPI
StartPreOsChecker (
  IN  VOID   *BootParam,
  IN  UINT16  ImageFlags
  )
{
  PRE_OS_PAYLOAD_PARAM       PreOsParams;
  PRE_OS_CHECKER_ENTRY       EntryPoint;
  BOOT_PARAMS                *OsBootParam;
  IA32_BOOT_STATE            *BootState;

  if (mPreOsCheckerEntry == NULL) {
    return EFI_NOT_FOUND;
  }

  if (BootParam == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem ((VOID *)&PreOsParams, (UINTN)sizeof (PreOsParams));

  PreOsParams.Version     = 0x1;
  PreOsParams.HeapSize    = EFI_SIZE_TO_PAGES (0);
  PreOsParams.HeapAddr    = (UINT32)(UINTN)AllocatePages (PreOsParams.HeapSize);
  PreOsParams.HobListPtr  = PcdGet32 (PcdPayloadHobList);

  if ((ImageFlags & LOADED_IMAGE_LINUX) != 0) {
    OsBootParam = (BOOT_PARAMS*)BootParam;
    UpdateLinuxBootParams (OsBootParam);
    PreOsParams.OsBootState.Esi     = (UINT32)(UINTN)OsBootParam;
    PreOsParams.OsBootState.Eip     = OsBootParam->Hdr.Code32Start;
  } else if ((ImageFlags & LOADED_IMAGE_MULTIBOOT) != 0) {
    BootState = (IA32_BOOT_STATE*)BootParam;
    PreOsParams.OsBootState.Eax     = BootState->Eax;
    PreOsParams.OsBootState.Ebx     = BootState->Ebx;
    PreOsParams.OsBootState.Esi     = BootState->Esi;
    PreOsParams.OsBootState.Edi     = BootState->Edi;
    PreOsParams.OsBootState.Eip     = BootState->EntryPoint;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  PreOsParams.OsBootState.Eflags  = 0;

  EntryPoint = (PRE_OS_CHECKER_ENTRY)(UINTN)mPreOsCheckerEntry;
  EntryPoint (&PreOsParams);

  //
  // Not reach here
  //
  CpuHalt ("Failed to launch Pre-OS Checker!");

  return EFI_DEVICE_ERROR;
}
