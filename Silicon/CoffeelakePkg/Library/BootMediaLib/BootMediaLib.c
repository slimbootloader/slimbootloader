/** @file
  Boot Media Library.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BootMediaLib.h>
#include <RegAccess.h>
#include <Library/IoLib.h>

/**
  Determines the boot media that the platform firmware is booting from

  @param[out] BootMediaType   The Boot Media the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootMediaType (
  OUT BOOT_MEDIA_TYPE            *BootMediaType
  )
{

  *BootMediaType   = BootMediaSpi;

  return EFI_SUCCESS;
}


/**
  Determines the boot partition that the platform firmware is booting from

  @param[out] BootPartition   The Boot partition the platform is booting from

  @retval     EFI_SUCCESS     The operation completed successfully.
**/
EFI_STATUS
EFIAPI
GetBootPartition (
  OUT BOOT_PARTITION_SELECT      *BootPartition
  )
{
  UINT32                           P2sbBase;
  UINT32                           P2sbBar;
  UINT16                           RegOffset;
  UINT8                            RtcPortId;
  UINT32                           Data32;
  BOOLEAN                          P2sbIsHidden;

  //
  //
  // get Top swap register Bit0 in PCH Private Configuration Space.
  //
  RegOffset  = 0x3414;               // RTC Backed Up Control (BUC) offset
  RtcPortId  = 0xC3;                 // RTC port ID
  P2sbBase   = MmPciBase (0, 31, 1); // P2SB device base

  P2sbIsHidden = FALSE;
  if (MmioRead16 (P2sbBase) == 0xFFFF) {
    MmioWrite8 (P2sbBase + 0xE1, 0); // unhide H2SB
    P2sbIsHidden = TRUE;
    DEBUG ((DEBUG_VERBOSE, "P2sb is hidden, unhide it\n"));
  }

  P2sbBar   = MmioRead32 (P2sbBase + 0x10);
  P2sbBar  &= 0xFFFFFFF0;
  ASSERT (P2sbBar != 0xFFFFFFF0);

  Data32    = MmioRead32 (P2sbBar | ((RtcPortId) << 16) | (UINT16)(RegOffset));
  DEBUG ((DEBUG_VERBOSE, "--P2sbBar=0x%x, Data32=0x%x\n", P2sbBar, Data32));

  if (P2sbIsHidden) {
    MmioWrite8 (P2sbBase + 0xE1, BIT0); //Hide P2SB
    DEBUG ((DEBUG_VERBOSE, "Hide p2sb again.\n"));
  }

  if ((Data32 & BIT0) == 0) {
    DEBUG ((DEBUG_VERBOSE, "Boot from partition 1\n"));
    *BootPartition = BootPartition1;
  } else {
    DEBUG ((DEBUG_VERBOSE, "Boot from partition 2\n"));
    *BootPartition = BootPartition2;
  }

  return EFI_SUCCESS;
}


/**
  Determines if the platform firmware is booting from SPI or not

  @retval TRUE        Platform firmware is booting from SPI
  @retval FALSE       Platform firmware is booting from a non-SPI device (eMMC, UFS, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsSpi (
  VOID
  )
{
  return TRUE;
}

/**
  Determines if the platform firmware is booting from eMMC or not

  @retval TRUE        Platform firmware is booting from eMMC
  @retval FALSE       Platform firmware is booting from a non-eMMC device (SPI, UFS, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsEmmc (
  VOID
  )
{
  return FALSE;
}

/**
  Determines if the platform firmware is booting from UFS or not

  @retval TRUE        Platform firmware is booting from UFS
  @retval FALSE       Platform firmware is booting from a non-UFS device (eMMC, SPI, etc.)
**/
BOOLEAN
EFIAPI
BootMediaIsUfs (
  VOID
  )
{
  return FALSE;
}
