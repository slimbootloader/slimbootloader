/** @file
  Header file for PchPcieRpLib.

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_PCIERP_LIB_H_
#define _PCH_PCIERP_LIB_H_

/**
  PCIe controller bifurcation configuration.
**/
typedef enum {
  PcieBifurcationDefault = 0,
  PcieBifurcation4x1,
  PcieBifurcation1x2_2x1,
  PcieBifurcation2x2,
  PcieBifurcation1x4,
  PcieBifurcation4x2,
  PcieBifurcation1x4_2x2,
  PcieBifurcation2x2_1x4,
  PcieBifurcation2x4,
  PcieBifurcation1x8,
  PcieBifurcationUnknown,
  PcieBifurcationMax
} PCIE_BIFURCATION_CONFIG;

/**
  Get Pch Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber            Root port physical number. (0-based)
  @param[out] RpDev               Return corresponding root port device number.
  @param[out] RpFun               Return corresponding root port function number.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
GetPchPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  );
#endif // _PCH_PCIERP_LIB_H_