/** @file
  Header file for P2SB SoC library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _P2SB_SOC_LIB_H_
#define _P2SB_SOC_LIB_H_

#include <P2SbController.h>

/**
  Get P2SB instance for SOC

  @param[in, out] P2SbController        P2SB controller pointer

  @retval     EFI_SUCCESS           - Completed successfully
              EFI_INVALID_PARAMETER - P2SbController NULL
**/
EFI_STATUS
MtlSocGetP2SbController (
  IN OUT P2SB_CONTROLLER         *P2SbController
  );

/**
  Get P2SB instance for IOE

  @param[in, out] P2SbController        P2SB controller pointer

  @retval     EFI_SUCCESS           - Completed successfully
              EFI_INVALID_PARAMETER - P2SbController NULL
**/
EFI_STATUS
MtlIoeGetP2SbController (
  IN OUT P2SB_CONTROLLER         *P2SbController
  );

/**
  Checks if IOE P2SB is hidden.

  @return TRUE if P2SB is hidden.
**/
BOOLEAN
MtlIoeIsP2SbHidden (
  VOID
  );

#endif // _P2SB_SOC_LIB_H_
