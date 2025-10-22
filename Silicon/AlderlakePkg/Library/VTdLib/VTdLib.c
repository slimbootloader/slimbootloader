/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/VTdLib.h>
#include <Register/IgdRegs.h>
#include <Library/PciLib.h>
#include <Register/SaRegsHostBridge.h>
#include <Register/VtdRegs.h>

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.

  @retval   VTD Engine Base Address
**/
UINT32
EFIAPI
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  )
{
  UINT64              McD0BaseAddress;
  UINTN               MchBar;
  UINT32              BaseAddress;

  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = PciRead32 ((UINTN)McD0BaseAddress + R_SA_MCHBAR) & (~BIT0);

  BaseAddress = 0;
  switch (VtdEngineNumber) {
    case 0:
      BaseAddress = MmioRead32 (MchBar + R_MCHBAR_VTD1_OFFSET) & (~BIT0);
      break;
    case 1:
      BaseAddress = MmioRead32 (MchBar + R_MCHBAR_VTD2_LOW_OFFSET) & (~BIT0);
      break;
    case 2:
      BaseAddress = MmioRead32 (MchBar + R_MCHBAR_VTD3_OFFSET) & (~BIT0);
      break;
    default:
      break;
  }
  return BaseAddress;
}
