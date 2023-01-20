/** @file
  The platform GPIO library header.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_LIB_H_
#define _GPIO_LIB_H_

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <GpioConfig.h>
#include <Include/GpioV2Config.h>

#define GPIO_NAME_LENGTH_MAX  32

/**
  Configure the GPIO pins, available as part of platform specific GPIO CFG DATA.
  If the pins are not part of GPIO CFG DATA, pass CDATA_NO_TAG for Tag param.

  @param    Tag         Tag ID of the Gpio Cfg data item
  @param    Entries     Number of entries in Gpio Table
  @param    DataBuffer  Pointer to the Gpio Table to be programmed

  @retval EFI_SUCCESS                   The function completed successfully
  @retval EFI_NOT_FOUND                 If Gpio Config Data cant be found
**/
EFI_STATUS
EFIAPI
ConfigureGpioV2 (
  IN  UINT16  Tag,
  IN  GPIOV2_INIT_CONFIG  *GpioTable,
  IN  UINT16  GpioTableCount
  );

#endif // _GPIO_LIB_H_

