/** @file
  Flash descriptor library header.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FLASH_DESCRIPTOR_H__
#define __FLASH_DESCRIPTOR_H__

typedef enum {
  Descriptor = 0,
  BIOS,
  CSME,
  Gbe,
  PlatformData,
  DeviceExpansion1,
  SecondaryBIOS,
  uCodePatch,
  EC,
  DeviceExpansion2,
  IE_Innovation_Engine,
  Gbe_A,
  Gbe_B,
  Spare1,
  Spare2,
  Spare3,
  FlRegMax = 0xff
} FLASH_DESCRIPTOR_REGION_SECTION;

/**
  Return SPI linear Base address of descriptor region section

  @param[in] RegNum                  FLREG number of region section defined in the descriptor

  @retval UINT32                        Base address of the FLREG
**/
UINT32
GetSpiFlashRegionBase (
  IN UINTN        RegNum
  );

/**
  Return SPI linear region limit of BIOS region

  @retval UINTN                         Region Limit address of the BIOS region
**/
UINT32
GetSpiFlashRegionLimit (
  IN UINTN        RegNum
  );



#endif
