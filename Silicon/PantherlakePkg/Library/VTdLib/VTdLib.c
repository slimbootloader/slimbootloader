/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/PtlVTdLib.h>
#include <Library/PciLib.h>
#include <Register/SaRegsHostBridge.h>
#include <Register/VtdRegs.h>

/**
  Get VT-d BAR size.

  @return     VT-d BAR size
**/
UINT32
GetVtdBarSize (
  )
{
  return SIZE_64KB;
}

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
EFIAPI
ReadVtdBaseAddress (
  IN UINT8            VtdEngineNumber
  )
{
  UINT64              McD0BaseAddress;
  UINTN               MchBar;

  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = PciRead32 ((UINTN)McD0BaseAddress + R_SA_MCHBAR) & (~BIT0);

  if (VtdEngineNumber < VTD_ENGINE_NUMBER) {
      return ((MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET) & (~(BIT0|BIT1|BIT2))) + (VtdEngineNumber * GetVtdBarSize()));
  } else {
      return 0x0;
  }
}
