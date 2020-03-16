/** @file
  VT-d Protected Memory library inteerface definitions.

  Copyright (c) 2010 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __VTD_PMR_LIB_H__
#define __VTD_PMR_LIB_H__

#include <IndustryStandard/Vtd.h>

#define  VTD_ENGINE_ALL                   ((UINT64)(-1))

typedef struct {
  UINT64                                  EngineMask;
  UINT8                                   HostAddressWidth;
  UINTN                                   VTdEngineCount;
  UINT64                                  VTdEngineAddress[0];
} VTD_INFO;

/**
  Set DMA protected region.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.
  @param LowMemoryBase      The protected low memory region base.
  @param LowMemoryLength    The protected low memory region length.
  @param HighMemoryBase     The protected high memory region base.
  @param HighMemoryLength   The protected high memory region length.

  @retval EFI_SUCCESS      The DMA protection is set.
  @retval EFI_UNSUPPORTED  The DMA protection is not set.
**/
EFI_STATUS
SetDmaProtectedRange (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask,
  IN UINT32        LowMemoryBase,
  IN UINT32        LowMemoryLength,
  IN UINT64        HighMemoryBase,
  IN UINT64        HighMemoryLength
  );

/**
  Diable DMA protection.

  @param VTdInfo            The VTd engine context information.
  @param EngineMask         The mask of the VTd engine to be accessed.

  @retval EFI_SUCCESS DMA protection is disabled.
**/
EFI_STATUS
DisableDmaProtection (
  IN VTD_INFO      *VTdInfo,
  IN UINT64        EngineMask
  );

#endif

