/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>
#include <RegAccess.h>

/**
  Get serial port stride register size.

  @retval  The serial port register stride size.

**/
UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
)
{
  return 1;
}

/**
  Get serial port register base address.

  @retval  The serial port register base address.

**/
UINT64
EFIAPI
GetSerialPortBase (
  VOID
  )
{
  return 0x3F8;
}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  return RETURN_SUCCESS;
}
