/** @file
  Flash descriptor library.

  Copyright (c) 2017-2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Pci30.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <RegAccess.h>


/**
  Return SPI linear Base address of descriptor region section

  @param[in] RegNum              FLREG number of region section defined in the descriptor

  @retval UINT32                 Base address of the FLREG
**/
UINT32
GetSpiFlashRegionBase (
  IN UINTN                       RegNum
  )
{
  UINTN                          SpiPciBase;
  UINT32                         SpiBar0;
  UINT32                         FlashRegBase;

  SpiPciBase = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiPciBase = TO_MM_PCI_ADDRESS (SpiPciBase);
  SpiBar0 = MmioRead32 (SpiPciBase + R_SPI_BASE) & B_SPI_BASE_BAR;
  DEBUG((DEBUG_INFO, "SpiBar0 = 0x%x\n", SpiBar0));

  FlashRegBase = MmioRead32 (SpiBar0 + R_SPI_FREG0_FLASHD + RegNum * 4) & B_SPI_FREG0_BASE_MASK;
  DEBUG((DEBUG_INFO, "FlashRegBase = 0x%x\n", FlashRegBase));

  if (FlashRegBase == V_SPI_FLREG_DISABLED) {
    FlashRegBase = 0;
    DEBUG((DEBUG_ERROR, "SPI FLREG%d is disabled!!!\n", RegNum));
  }
  FlashRegBase <<= N_SPI_FREG0_BASE;
  DEBUG((DEBUG_INFO, "SPI FLREG%d base = 0x%x\n", RegNum, FlashRegBase));

  return FlashRegBase;
}


/**
  Return SPI linear region limit of BIOS region

  @retval UINTN                  Region Limit address of the BIOS region
**/
UINT32
GetSpiFlashRegionLimit (
  IN UINTN                       RegNum
  )
{
  UINTN                          SpiPciBase;
  UINT32                         SpiBar0;
  UINT32                         FlashRegLimit;

  SpiPciBase = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiPciBase = TO_MM_PCI_ADDRESS (SpiPciBase);

  SpiBar0    = MmioRead32 (SpiPciBase + R_SPI_BASE) & B_SPI_BASE_BAR;

  FlashRegLimit   = MmioRead32 (SpiBar0 + R_SPI_FREG0_FLASHD + RegNum * 4) & B_SPI_FREG0_LIMIT_MASK;
  FlashRegLimit >>= N_SPI_FREG1_LIMIT;
  FlashRegLimit  |= 0xFFF;

  DEBUG ((DEBUG_INFO, "SPI FLREG%d limit = 0x%x\n", RegNum, FlashRegLimit));

  return FlashRegLimit;
}


