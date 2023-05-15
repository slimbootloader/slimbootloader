/** @file
  Pch access library for GSPI controller.

  This library is provided as an example for FuSa proof of concept
  communication, so only SPI write is implemented.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCH_GSPI_LIB_H__
#define __PCH_GSPI_LIB_H__

#include <Base.h>

/**
  Initialize GSPI controller instance for polled PIO operation.

 */
EFI_STATUS
EFIAPI
PchGspiInit(
  UINT8 Controller
  );

/**
  Write Buffer to SPI controller indicated. Configure HW
  chip select setting first.

  @param Controller
  @param ChipSelect
  @param Data
  @param DataSize
  @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
PchGspiWrite(
  UINT8 Controller,
  UINT8 ChipSelect,
  UINT8 *Data,
  UINTN DataSize
  );

#endif //__PCH_GSPI_LIB_H__
