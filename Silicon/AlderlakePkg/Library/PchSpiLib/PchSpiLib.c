/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Register/RegsSpi.h>
#include <RegAccess.h>

/**
  Acquire SPI MMIO BAR

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval                         Return SPI BAR Address

**/
UINT32
AcquireSpiBar0 (
  IN  UINTN         PchSpiBase
  )
{
  UINTN     SpiBase;

  SpiBase = MmioRead32 (PchSpiBase + R_PCH_LPC_RCBA) & ~BIT0;

  return (SpiBase + R_PCH_RCRB_SPI_BASE);
}

/**
  Release SPI MMIO BAR. Do nothing.

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval None

**/
VOID
ReleaseSpiBar0 (
  IN  UINTN         PchSpiBase
  )
{
}

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @retval EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase

**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  IN  UINTN         PchSpiBase
  )
{
  if ((MmioRead8 (PchSpiBase + R_SPI_CFG_BC) & B_SPI_CFG_BC_EISS) != 0) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Enable the access to the BIOS space for both read and write cycles
  //
  MmioOr8 (PchSpiBase + R_SPI_CFG_BC, B_SPI_CFG_BC_WPD);

  return EFI_SUCCESS;
}

/**
  This function is a hook for Spi to enable BIOS Write Protect

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval None

**/
VOID
EFIAPI
EnableBiosWriteProtect (
  IN  UINTN         PchSpiBase
  )
{
  //
  // Disable the access to the BIOS space for write cycles
  //
  MmioAnd8 (PchSpiBase + R_SPI_CFG_BC, (UINT8) (~B_SPI_CFG_BC_WPD));
}

/**
  This function disables SPI Prefetching and caching,
  and returns previous BIOS Control Register value before disabling.

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval                         Previous BIOS Control Register value

**/
UINT8
EFIAPI
SaveAndDisableSpiPrefetchCache (
  IN  UINTN         PchSpiBase
  )
{
  UINT8           BiosCtlSave;

  BiosCtlSave = MmioRead8 (PchSpiBase + R_SPI_BCR) & B_SPI_BCR_SRC;

  MmioAndThenOr32 (PchSpiBase + R_SPI_BCR, \
    (UINT32) (~B_SPI_BCR_SRC), \
    (UINT32) (V_SPI_BCR_SRC_PREF_DIS_CACHE_DIS <<  B_SPI_BCR_SRC));

  return BiosCtlSave;
}

/**
  This function updates BIOS Control Register with the given value.

  @param[in] PchSpiBase           PCH SPI PCI Base Address
  @param[in] BiosCtlValue         BIOS Control Register Value to be updated

  @retval None

**/
VOID
EFIAPI
SetSpiBiosControlRegister (
  IN  UINTN         PchSpiBase,
  IN  UINT8         BiosCtlValue
  )
{
  MmioAndThenOr8 (PchSpiBase + R_SPI_BCR, (UINT8) ~B_SPI_BCR_SRC, BiosCtlValue);
}
