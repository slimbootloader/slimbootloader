/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <FspApiLibInternal.h>
#include <Library/BoardInitLib.h>
#include <Library/BootloaderCoreLib.h>

/**
  This FSP API is called after TempRamInit and initializes the memory.
  This FSP API accepts a pointer to a data structure that will be platform dependent
  and defined for each FSP binary. This will be documented in Integration guide with
  each FSP release.
  After FspMemInit completes its execution, it passes the pointer to the HobList and
  returns to the boot loader from where it was called. BootLoader is responsible to
  migrate it's stack and data to Memory.
  FspMemoryInit, TempRamExit and FspSiliconInit APIs provide an alternate method to
  complete the silicon initialization and provides bootloader an opportunity to get
  control after system memory is available and before the temporary RAM is torn down.

  @param[in] FspmBase                 The base address of FSPM.
  @param[out] HobListPtr              Pointer to receive the address of the HOB list.

  @retval EFI_SUCCESS                 FSP execution environment was initialized successfully.
  @retval EFI_INVALID_PARAMETER       Input parameters are invalid.
  @retval EFI_UNSUPPORTED             The FSP calling conditions were not met.
  @retval EFI_DEVICE_ERROR            FSP initialization failed.
  @retval EFI_OUT_OF_RESOURCES        Stack range requested by FSP is not met.
  @retval FSP_STATUS_RESET_REQUIREDx  A reset is reuired. These status codes will not be returned during S3.
**/
EFI_STATUS
EFIAPI
CallFspMemoryInit (
  UINT32                     FspmBase,
  VOID                       **HobList
  )
{
  UINT8                       FspmUpd[FixedPcdGet32 (PcdFSPMUpdSize)];
  UINT8                      *DefaultMemoryInitUpd;
  FSP_INFO_HEADER            *FspHeader;
  FSP_MEMORY_INIT             FspMemoryInit;
  FSPM_UPD_COMMON            *FspmUpdCommon;
  EFI_STATUS                  Status;
  UINTN                       NewStack;

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(FspmBase + FSP_INFO_HEADER_OFF);

  ASSERT (FspHeader->Signature == FSP_INFO_HEADER_SIGNATURE);
  ASSERT (FspHeader->ImageBase == FspmBase);

  // Copy default UPD data
  DefaultMemoryInitUpd = (UINT8 *)(UINTN)(FspHeader->ImageBase + FspHeader->CfgRegionOffset);
  CopyMem (&FspmUpd, DefaultMemoryInitUpd, FspHeader->CfgRegionSize);

  /* Update architectural UPD fields */
  FspmUpdCommon = (FSPM_UPD_COMMON *)FspmUpd;
  FspmUpdCommon->FspmArchUpd.BootLoaderTolumSize = 0;
  FspmUpdCommon->FspmArchUpd.BootMode            = (UINT32)GetBootMode();
  FspmUpdCommon->FspmArchUpd.NvsBufferPtr        = (UINT32)(UINTN)FindNvsData();

  UpdateFspConfig (FspmUpd);

  ASSERT (FspHeader->FspMemoryInitEntryOffset != 0);
  FspMemoryInit = (FSP_MEMORY_INIT)(UINTN)(FspHeader->ImageBase + \
                                           FspHeader->FspMemoryInitEntryOffset);

  DEBUG ((DEBUG_INFO, "Call FspMemoryInit ... "));

  NewStack = PcdGet32 (PcdFSPMStackTop);
  if (NewStack == 0xFFFFFFFF) {
    NewStack = FspmUpdCommon->FspmArchUpd.StackBase + FspmUpdCommon->FspmArchUpd.StackSize;
  }
  if (IS_X64) {
    if (NewStack != 0) {
      Status = FspmSwitchStack ((VOID *)(UINTN)FspMemoryInit, (VOID *)FspmUpd, (VOID *)HobList, (VOID *)NewStack);
    } else {
      Status = Execute32BitCode ((UINTN)FspMemoryInit, (UINTN)FspmUpd, (UINTN)HobList, FALSE);
    }
    Status = (UINTN)LShiftU64 (Status & ((UINTN)MAX_INT32 + 1), 32) | (Status & MAX_INT32);
  } else {
    if (NewStack != 0) {
      Status = FspmSwitchStack ((VOID *)(UINTN)FspMemoryInit, (VOID *)&FspmUpd, (VOID *)HobList, (VOID *)NewStack);
    } else {
      Status = FspMemoryInit (&FspmUpd, HobList);
    }
  }
  DEBUG ((DEBUG_INFO, "%r\n", Status));

  return Status;
}
