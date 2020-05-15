/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <FspApiLibInternal.h>
#include <Library/BoardInitLib.h>


/**
  This FSP API is called after TempRamExit API.
  FspMemoryInit, TempRamExit and FspSiliconInit APIs provide an alternate method to complete the
  silicon initialization.

  @retval EFI_SUCCESS                 FSP execution environment was initialized successfully.
  @retval EFI_INVALID_PARAMETER       Input parameters are invalid.
  @retval EFI_UNSUPPORTED             The FSP calling conditions were not met.
  @retval EFI_DEVICE_ERROR            FSP initialization failed.
  @retval FSP_STATUS_RESET_REQUIREDx  A reset is reuired. These status codes will not be returned during S3.
**/
EFI_STATUS
EFIAPI
CallFspSiliconInit (
  VOID
  )
{
  UINT8                       FspsUpd[FixedPcdGet32 (PcdFSPSUpdSize)];
  UINT8                      *DefaultSiliconInitUpd;
  FSP_INFO_HEADER            *FspHeader;
  FSP_SILICON_INIT            FspSiliconInit;
  EFI_STATUS                  Status;

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(PcdGet32 (PcdFSPSBase) + FSP_INFO_HEADER_OFF);

  ASSERT (FspHeader->Signature == FSP_INFO_HEADER_SIGNATURE);
  ASSERT (FspHeader->ImageBase == PcdGet32 (PcdFSPSBase));

  // Copy default UPD data
  DefaultSiliconInitUpd = (UINT8 *)(UINTN)(FspHeader->ImageBase + FspHeader->CfgRegionOffset);
  CopyMem (&FspsUpd, DefaultSiliconInitUpd, FspHeader->CfgRegionSize);

  /* Update architectural UPD fields */
  UpdateFspConfig (FspsUpd);

  ASSERT (FspHeader->FspSiliconInitEntryOffset != 0);
  FspSiliconInit = (FSP_SILICON_INIT)(UINTN)(FspHeader->ImageBase + \
                                             FspHeader->FspSiliconInitEntryOffset);

  DEBUG ((DEBUG_INFO, "Call FspSiliconInit ... \n"));
  if (IS_X64) {
    Status = Execute32BitCode ((UINTN)FspSiliconInit, (UINTN)FspsUpd, (UINTN)0, FALSE);
    Status = (UINTN)LShiftU64 (Status & ((UINTN)MAX_INT32 + 1), 32) | (Status & MAX_INT32);
  } else {
    Status = FspSiliconInit (&FspsUpd);
  }
  DEBUG ((DEBUG_INFO, "%r\n", Status));

  return Status;
}
