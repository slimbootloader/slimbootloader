/** @file
  Header file for PchPcieRpLib.

@copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2021 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_PCIERP_LIB_H_
#define _PCH_PCIERP_LIB_H_

#include <Uefi.h>
#include <PcrDefine.h>

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
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval P2SB_PID    Returns PID for SBI Access
**/
P2SB_PID
PchGetPcieControllerSbiPid (
  IN UINT32  ControllerIndex
  );

/**
  This function returns PID according to Root Port Number

  @param[in] RpIndex     Root Port Index (0-based)

  @retval P2SB_PID    Returns PID for SBI Access
**/
P2SB_PID
GetRpSbiPid (
  IN UINTN  RpIndex
  );

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

/**
  Get Root Port physical Number by Pch Pcie Root Port Device and Function Number

  @param[in]  RpDev                 Root port device number.
  @param[in]  RpFun                 Root port function number.
  @param[out] RpNumber              Return corresponding physical Root Port index (0-based)

  @retval     EFI_SUCCESS           Physical root port is retrieved
  @retval     EFI_INVALID_PARAMETER RpDev and/or RpFun are invalid
  @retval     EFI_UNSUPPORTED       Root port device and function is not assigned to any physical root port
**/
EFI_STATUS
EFIAPI
GetPchPcieRpNumber (
  IN  UINTN   RpDev,
  IN  UINTN   RpFun,
  OUT UINTN   *RpNumber
  );

/**
  Gets pci segment base address of PCIe root port.

  @param RpIndex    Root Port Index (0 based)
  @return PCIe port base address.
**/
UINT64
PchPcieBase (
  IN  UINT32   RpIndex
  );

/**
  Checks which CLK_REQ signal is assigned to given CLK_SRC

  @param ClkSrc    number of CLK_SRC, 0-based
  @retval          number of CLK_REQ, 0-based
**/
UINT8
GetClkReqForClkSrc (
  IN UINT8 ClkSrc
  );

/**
  Determines whether L0s is supported on current stepping.

  @return TRUE if L0s is supported, FALSE otherwise
**/
BOOLEAN
PchIsPcieL0sSupported (
  VOID
  );

/**
  Some early PCH steppings require Native ASPM to be disabled due to hardware issues:
   - RxL0s exit causes recovery
   - Disabling PCIe L0s capability disables L1
  Use this function to determine affected steppings.

  @return TRUE if Native ASPM is supported, FALSE otherwise
**/
BOOLEAN
PchIsPcieNativeAspmSupported (
  VOID
  );
#endif // _PCH_PCIERP_LIB_H_
