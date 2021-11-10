/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCH_SPI_LIB_H__
#define __PCH_SPI_LIB_H__

/**
  Acquire SPI MMIO BAR

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval                         Return SPI BAR Address

**/
UINT32
AcquireSpiBar0 (
  IN  UINTN         PchSpiBase
  );

/**
  Release SPI MMIO BAR. Do nothing.

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval None

**/
VOID
ReleaseSpiBar0 (
  IN  UINTN         PchSpiBase
  );

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
  );

/**
  This function is a hook for Spi to enable BIOS Write Protect

  @param[in] PchSpiBase           PCH SPI PCI Base Address

  @retval None

**/
VOID
EFIAPI
EnableBiosWriteProtect (
  IN  UINTN         PchSpiBase
  );

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
  );

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
  );

#endif /* __PCH_SPI_LIB_H__ */
