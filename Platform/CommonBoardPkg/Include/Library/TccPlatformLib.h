/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TCC_PLATFORM_LIB_H_
#define _TCC_PLATFORM_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Library/TccLib.h>
#include <FspmUpd.h>

/**
  This procedure will retrieve Pre-Mem TCC BIOS settings table and configure FSP-M settings
**/
EFI_STATUS
EFIAPI
TccSetFspmConfig (
  IN FSPM_UPD          *FspmUpd,
  IN BIOS_SETTINGS     *PreMemBiosSettings,
  IN TCC_BINARIES_BASE *PreMemTccBinariesBase
  );

/**
  This procedure will retrieve Pre-Mem TCC BIOS settings table and configure FSP-M settings
**/
EFI_STATUS
TccSetFspsConfig (
  IN BIOS_SETTINGS*     BiosSettings,
  IN TCC_BINARIES_BASE* TccBinariesBase
  );

/**
  Get GT COS of specific policy

  @param[in] CosPolicy              GT COS Policy to get ways
  @param[out] CosWaysMask           Cos Ways Mask

  @retval EFI_SUCCESS               Got GT COS successful
  @retval EFI_INVALID_PARAMETER     Invalid COS policy
  @retval EFI_UNSUPPORTED           Unsupported MSR
**/
EFI_STATUS
GetGtCos (
  IN UINT8     GtCosIndex,
  OUT UINT32   *CosWaysMask
  );

/**
  Get WRC COS of specific policy

  @param[out] CosWaysMask  Cos Ways Mask

  @retval EFI_SUCCESS           get WRC COS successful
  @retval EFI_INVALID_PARAMETER invalid COS policy
  @retval EFI_UNSUPPORTED       Unsupported MSR
**/
EFI_STATUS
MsrGetWrcCos (
  OUT UINT32   *CosWaysMask
  );

/*
  Get Max IA L2 Cache COS

  @retval UINT32    Max IA L2 Cache COS
*/
UINT32
GetMaxIaL2Cos (
  VOID
  );

#endif // _TCC_PLATFORM_LIB_H_
