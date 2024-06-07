/** @file
  PCIe RP SoC library.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _MTL_SOC_PCIE_RP_LIB_H_
#define _MTL_SOC_PCIE_RP_LIB_H_

//#include <Library/P2SbSidebandAccessLib.h>
#include <Include/PcrDefine.h>

/**
  Translate RpIndex to controller index.

  @param[in] RpIndex  Root port index

  @return ControllerIndex
  @retval 0xFF Failed to lookup controller index.
**/
UINT32
MtlSocRpIndexToControllerIndex (
  IN UINTN  RpIndex
  );

/**
  Get the FID number of the PCIe root port.

  @param[in] RpIndex  Index of the root port

  @return FID number to use when sending SBI msg
**/
UINT16
MtlSocPcieRpGetSbiFid (
  IN UINTN  RpIndex
  );

/**
  Get MTL SoC Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
MtlSocGetPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINT16  *RpDev,
  OUT UINT16  *RpFun
  );

/**
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval P2SB_PID    Returns PID for SBI Access
**/
P2SB_PID
MtlSocGetPcieControllerSbiPid (
  IN UINT32  ControllerIndex
  );

/**
  This function returns PID according to Root Port Number

  @param[in] RpIndex     Root Port Index (0-based)

  @retval P2SB_PID   Returns PID for SBI Access
**/
P2SB_PID
MtlSocGetRpSbiPid (
  IN UINTN  RpIndex
  );

/**
  Checks whether RpIndex is located on IOE die.

  @param[in] RpIndex  Root port index.

  @return TRUE if root port is located on IOE die.
**/
BOOLEAN
MtlSocIsRpOnIoeDie (
  IN UINT32  RpIndex
  );

/**
  Checks if root port is decoded by IOC.

  @param[in] RpIndex  Root port index.

  @return TRUE if root port is IOC decoded.
**/
BOOLEAN
MtlSocIsRpIocDecoded (
  IN UINT32  RpIndex
  );

#endif
