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
      PcdSet64S(PcdFspResetStatus, Status);
      ResetSystem(EfiResetPlatformSpecific);
    }
    CpuDeadLoop ();
  }
}

