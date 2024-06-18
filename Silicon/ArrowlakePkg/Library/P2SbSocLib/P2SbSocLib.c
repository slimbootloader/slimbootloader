/** @file
  P2SB SoC library.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/P2SbSocLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/PcdLib.h>
#include <Register/PchRegs.h>
#include <PchReservedResources.h>
#include <Register/PchBdfAssignment.h>
#include <PcieRegs.h>

/**
  Get P2SB instance for SOC

  @param[in, out] P2SbController        P2SB controller pointer

  @retval     EFI_SUCCESS           - Completed successfully
              EFI_INVALID_PARAMETER - P2SbController NULL
**/
EFI_STATUS
MtlSocGetP2SbController (
  IN OUT P2SB_CONTROLLER         *P2SbController
  )
{
  UINT8   Dev;
  UINT8   Fun;

  if (P2SbController == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Dev = PCI_DEVICE_NUMBER_PCH_P2SB;
  Fun = PCI_FUNCTION_NUMBER_PCH_P2SB;

  ZeroMem (P2SbController, sizeof (P2SB_CONTROLLER));
  P2SbController->PciCfgBaseAddr = PCI_SEGMENT_LIB_ADDRESS (DEFAULT_PCI_SEGMENT_NUMBER_PCH, DEFAULT_PCI_BUS_NUMBER_PCH, Dev, Fun, 0);
  P2SbController->Mmio           = PCH_PCR_BASE_ADDRESS;
  P2SbController->HpetMmio       = 0xFED00000;

  return EFI_SUCCESS;
}

/**
  Get P2SB instance for IOE

  @param[in, out] P2SbController        P2SB controller pointer

  @retval     EFI_SUCCESS           - Completed successfully
              EFI_INVALID_PARAMETER - P2SbController NULL
**/
EFI_STATUS
MtlIoeGetP2SbController (
  IN OUT P2SB_CONTROLLER         *P2SbController
  )
{
  UINT8   Dev;
  UINT8   Fun;

  if (P2SbController == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Dev = PCI_DEVICE_NUMBER_IOE_P2SB;
  Fun = PCI_FUNCTION_NUMBER_IOE_P2SB;

  ZeroMem (P2SbController, sizeof (P2SB_CONTROLLER));
  P2SbController->PciCfgBaseAddr = PCI_SEGMENT_LIB_ADDRESS (DEFAULT_PCI_SEGMENT_NUMBER_PCH, DEFAULT_PCI_BUS_NUMBER_PCH, Dev, Fun, 0);
  P2SbController->Mmio           = IOE_PCR_ABOVE_4G_BASE_ADDRESS;
  P2SbController->HpetMmio       = 0;

  return EFI_SUCCESS;
}

/**
  Checks if IOE P2SB is hidden.

  @return TRUE if P2SB is hidden.
**/
BOOLEAN
MtlIoeIsP2SbHidden (
  VOID
  )
{
  UINT64  PciAddress;

  PciAddress = PCI_SEGMENT_LIB_ADDRESS (DEFAULT_PCI_SEGMENT_NUMBER_PCH, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_IOE_P2SB, PCI_FUNCTION_NUMBER_IOE_P2SB, 0);
  if (PciSegmentRead16 (PciAddress + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    return TRUE;
  } else {
    return FALSE;
  }
}
