/** @file
  Header file for P2SB SoC library.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
PtlPcdGetP2SbController (
  IN OUT P2SB_CONTROLLER         *P2SbController
  );

#endif // _P2SB_SOC_LIB_H_
