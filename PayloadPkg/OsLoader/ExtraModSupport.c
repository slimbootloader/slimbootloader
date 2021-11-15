/** @ Provide API to call extra modules

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "OsLoader.h"

#define  RTCM_HEAP_SIZE       0x10000
#define  RTCM_RSVD_SIZE       FixedPcdGet32 (PcdRtcmRsvdSize)
#define  MOD_DEF_HEAP_SIZE    0x10000
#define  MOD_DEF_RSVD_SIZE    0x04000

/**
  Call into an extra image entrypoint.

  Detect and call into the image entrypoint. If required, handle thunk call
  as well.

  @param[in]  ModSignature      Module signature.
  @param[in]  LoadedImage       Loaded Image information, expected to be PE32 format.

  @retval  EFI_SUCCESS          Image returns successfully
  @retval  Others               There is error during the module call.

**/
EFI_STATUS
CallExtraModule (
  IN   UINT32           ModSignature,
  IN   LOADED_IMAGE    *LoadedImage
  )
{
  EFI_STATUS           Status;
  UINT16               PldMachine;
  PLD_MODULE_ENTRY     PldEntry;
  VOID                *HobList;
  PLD_EXTRA_MOD_ARGS   ModArgs;
  VOID                *ExtraImageBase;

  ExtraImageBase = LoadedImage->Image.Common.BootFile.Addr;

  // Prepare memory for extra modules
  ZeroMem (&ModArgs, sizeof(ModArgs));
  ModArgs.Signature  = ModSignature;
  if (ModSignature == PLD_EXTRA_MOD_RTCM) {
    ModArgs.RsvdSize   = RTCM_RSVD_SIZE;
    ModArgs.HeapSize   = RTCM_HEAP_SIZE;
  } else {
    ModArgs.RsvdSize   = MOD_DEF_RSVD_SIZE;
    ModArgs.HeapSize   = MOD_DEF_HEAP_SIZE;
  }
  ModArgs.RsvdBase   = (UINT32)(UINTN) AllocateReservedPages (EFI_SIZE_TO_PAGES (ModArgs.RsvdSize));
  ModArgs.HeapBase   = (UINT32)(UINTN) AllocatePages (EFI_SIZE_TO_PAGES (ModArgs.HeapSize));
  ASSERT (ModArgs.HeapBase != 0);
  ASSERT (ModArgs.RsvdBase != 0);

  // Get entrypoint and call into module with thunk support
  Status = PeCoffLoaderGetMachine (ExtraImageBase, &PldMachine);
  if (!EFI_ERROR(Status)) {
    Status = PeCoffLoaderGetEntryPoint (ExtraImageBase, (VOID *)&PldEntry);
  }

  if (!EFI_ERROR(Status)) {
    if (IS_X64) {
      if (PldMachine == IMAGE_FILE_MACHINE_I386) {
        DEBUG ((DEBUG_INFO, "Switch to x86 Mode\n"));
      }
    } else {
      if (PldMachine == IMAGE_FILE_MACHINE_X64) {
        DEBUG ((DEBUG_INFO, "Switch to x64 Mode\n"));
      }
    }
    DEBUG ((DEBUG_INIT, "Call Extra Module Entry @ 0x%p \n", PldEntry));
    HobList = (VOID *)(UINTN) PcdGet32 (PcdPayloadHobList);
    if (PldMachine == IMAGE_FILE_MACHINE_X64) {
      Status = Execute64BitCode ((UINT64)(UINTN)PldEntry, (UINT64)(UINTN)HobList,
                                 (UINT64)(UINTN)&ModArgs, FALSE);
    } else {

      Status = Execute32BitCode ((UINT64)(UINTN)PldEntry, (UINT64)(UINTN)HobList,
                                 (UINT64)(UINTN)&ModArgs, FALSE);
    }
    DEBUG ((DEBUG_INFO, "Extra Module Returned: %r\n", Status));
  }

  // Deallocate heap memory, but keep reserved memory
  if (ModArgs.HeapBase != 0) {
    FreePages ((VOID *)(UINTN)ModArgs.HeapBase, EFI_SIZE_TO_PAGES (ModArgs.HeapSize));
  }

  return Status;
}
