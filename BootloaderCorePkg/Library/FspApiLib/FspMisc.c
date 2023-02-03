/** @file

  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <FspApiLibInternal.h>
#include <Library/ResetSystemLib.h>
#include <Library/LitePeCoffLib.h>
#include <Library/PcdLib.h>

/**
  This function will handle FSP reset request.

  @param[in] Status     FSP API return status.

**/
VOID
EFIAPI
FspResetHandler (
  IN  EFI_STATUS   Status
  )
{
  if ((Status >= FSP_STATUS_RESET_REQUIRED_COLD) && (Status <= FSP_STATUS_RESET_REQUIRED_8)) {
    DEBUG ((DEBUG_INIT, "FSP Requested Reboot ...\n\n"));
    if (Status == FSP_STATUS_RESET_REQUIRED_WARM) {
      ResetSystem(EfiResetWarm);
    } else if (Status == FSP_STATUS_RESET_REQUIRED_COLD){
      ResetSystem(EfiResetCold);
    } else {
      PcdSet32S(PcdFspResetStatus, (UINT32)Status);
      ResetSystem(EfiResetPlatformSpecific);
    }
    CpuDeadLoop ();
  }
}

/**
  This FSP API is used to notify the FSP about the different phases in the boot process.
  This allows the FSP to take appropriate actions as needed during different initialization
  phases. The phases will be platform dependent and will be documented with the FSP
  release. The current FSP supports following notify phases:
    Post PCI enumeration
    Ready To Boot
    End of firmware

  @param[in] Delta              The delta between the new and old base.
                                Delta = NewBase - OldBase

  @retval EFI_SUCCESS           The rebasing was handled successfully.

**/
EFI_STATUS
EFIAPI
RebaseFspComponent (
  UINT32   Delta
  )
{
  FSP_INFO_HEADER   *FspHeader;

  FspHeader = (FSP_INFO_HEADER *)(UINTN)(PcdGet32 (PcdFSPSBase) + Delta + FSP_INFO_HEADER_OFF);
  FspHeader->ImageBase += Delta;

  return EFI_SUCCESS;
}
