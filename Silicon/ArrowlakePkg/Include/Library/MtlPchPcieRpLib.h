/** @file
  MTL PCH PCIe RP library header.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MTL_PCH_PCIE_RP_LIB_
#define _MTL_PCH_PCIE_RP_LIB_


/**
  Get MTL SoC Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
MtlPchGetPcieRpDevFun (
  IN  UINT32                 RpNumber,
  OUT UINT16                 *RpDev,
  OUT UINT16                 *RpFun
  );


#endif