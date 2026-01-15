/** @file
  This code provides a support function for Intel VT-d (Virtualization Technology for Directed I/O).

  Copyright (c) 1999 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/VTdLib.h>
#include <Library/PciLib.h>
#include <Register/IgdRegs.h>
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
  //
  // For Desktop SKU VT-d register space is 64KB, for other SKUs - 4KB
  //
#if FixedPcdGet8(PcdMtlSSupport) == 1
  return SIZE_64KB;
#else
  return SIZE_4KB;
#endif
}


/**
  Read VTD Engine Base Address from VTD BAR Offsets.

  @param[in]  VtdEngineNumber        - Engine number for which VTD Base Adderess is required.
                                        0 = IGD VT-d (VTD1_OFFSET)
                                        1 = IPU VT-d (VTD2_LOW_OFFSET) - may not exist on all SKUs
                                        2 = IOP VT-d (VTD3_OFFSET)

  @retval   VTD Engine Base Address
**/
UINT32
ReadVtdBaseAddress (
  IN UINT8        VtdEngineNumber
  )
{
  UINT64              McD0BaseAddress;
  UINTN               MchBar;

  McD0BaseAddress = PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, 0);
  MchBar          = PciRead32 ((UINTN)McD0BaseAddress + R_SA_MCHBAR) & (~BIT0);

  switch (VtdEngineNumber) {
    case 0:
      return (MmioRead32 (MchBar + 0x5400) & (~BIT0));
      break;
    case 1:
      return (MmioRead32 (MchBar + R_MCHBAR_VTD2_LOW_OFFSET) & (~BIT0));
      break;
    case 2:
      return (MmioRead32 (MchBar + 0x5410) & (~BIT0));
      break;
    default:
      return 0x0;
      break;
  }
}
