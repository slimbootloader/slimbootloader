/** @file
  This code provides a initialization of Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/VTdLib.h>
#include <Library/PciLib.h>
#include <RegAccess.h>

#define R_SA_MCHBAR_VTD1_OFFSET               0x6C88  ///< DMA Remapping HW UNIT1 for IGD
#define R_SA_MCHBAR_VTD2_OFFSET               0x6C80  ///< DMA Remapping HW UNIT2 for all other - PEG, USB, SATA etc

/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Address is required.
                                      0 = Graphics VTD Engine
                                      1 = General Purpose VTD Engine

  @retval   VTD Engine Base Address
**/
UINT32
EFIAPI
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  )
{
  UINTN               McD0BaseAddress;
  UINTN               MchBar;
  UINT32              BaseAddress;

  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = PciRead32 (McD0BaseAddress + R_SA_MCHBAR_REG) & (~BIT0);
  BaseAddress = 0;
  switch (VtdEngineNumber) {
    case 0:
      BaseAddress = MmioRead32 (MchBar + R_SA_MCHBAR_VTD1_OFFSET) & (~3);
      break;
    case 1:
      BaseAddress = MmioRead32 (MchBar + R_SA_MCHBAR_VTD2_OFFSET) & (~3);
      break;
    default:
      break;
  }
  return BaseAddress;
}