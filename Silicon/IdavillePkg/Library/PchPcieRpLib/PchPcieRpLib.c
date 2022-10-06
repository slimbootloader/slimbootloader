/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcieRpInfo.h>
#include <Register/PchRegsPcr.h>

typedef struct {
  UINT8 DevNum;
  UINT8 FunNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCH_PCIE_CONTROLLER_INFO;

GLOBAL_REMOVE_IF_UNREFERENCED CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[] = {
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1,  PID_PCIE0_P0, 0  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2,  PID_PCIE0_P1, 1  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3,  PID_PCIE0_P2, 2  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_4,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4,  PID_PCIE0_P3, 3  },

  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_5,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5,  PID_PCIE1_P0, 4  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_6,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6,  PID_PCIE1_P1, 5  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_7,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7,  PID_PCIE1_P2, 6  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_8,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8,  PID_PCIE1_P3, 7  },

  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_9,  PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9,  PID_PCIE2_P0, 8  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_10, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10, PID_PCIE2_P1, 9  },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_11, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11, PID_PCIE2_P2, 10 },
  { PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_12, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12, PID_PCIE2_P3, 11 }
};

/**
  Get Pch Pcie Root Port Device and Function Number by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER RpNumber is invalid
**/
EFI_STATUS
EFIAPI
GetPchPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  )
{
  if (RpNumber >= GetPchMaxPciePortNum ()) {
    DEBUG ((DEBUG_ERROR, "GetPchPcieRpDevFun invalid RpNumber %x", RpNumber));
    return EFI_INVALID_PARAMETER;
  }
  *RpDev = mPchPcieControllerInfo[RpNumber].DevNum;
  *RpFun = 0;

  return EFI_SUCCESS;
}
