/** @file
  VT-d Protected Memory library inteerface definitions.

  Copyright (c) 2010 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __VTD_PMR_LIB_H__
#define __VTD_PMR_LIB_H__

#include <IndustryStandard/Vtd.h>
#include <Library/PcdLib.h>

#define  VTD_ENGINE_ALL                   ((UINT64)(-1))

typedef struct {
  UINT64                                  EngineMask;
  UINT8                                   HostAddressWidth;
  UINTN                                   VTdEngineCount;
  UINT64                                  VTdEngineAddress[8];
} VTD_INFO;

/**
  Enable or disable DMA protection using VTD PMR.

  This function will enable/disable DMA protection.

  @param[in] VtdInfo            VT-d info structure pointer.
  @param[in] Enable             Enable/Disable DMA protection.

  @retval     EFI_UNSUPPORTED   DMA protection is not supported.
  @retval     EFI_SUCCESS       DMA protection is enabled or disabled successfully.

**/
EFI_STATUS
EFIAPI
SetDmaProtection (
  IN  VTD_INFO     *VtdInfo,
  IN  BOOLEAN       Enable
);

#endif

