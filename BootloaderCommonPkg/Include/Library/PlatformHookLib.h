/** @file
  Platform hook library. Platform can provide an implementation of this
  library class to provide hooks that may be required for some type of
  platform initialization.

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PLATFORM_HOOK_LIB__
#define __PLATFORM_HOOK_LIB__

/**
  Get serial port stride register size.

  @retval  The serial port register stride size.

**/
UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
  );

/**
  Get serial port register base address.

  @retval  The serial port register base address.

**/
UINT64
EFIAPI
GetSerialPortBase (
  VOID
  );


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
  );

#endif // __PLATFORM_HOOK_LIB__

