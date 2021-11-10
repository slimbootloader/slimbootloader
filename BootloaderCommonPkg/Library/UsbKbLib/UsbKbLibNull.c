/** @file
  USB keyboard library implementation.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

/**
  Polls a keyboard to see if there is any data waiting to be read.

  @retval TRUE             Data is waiting to be read from the device.
  @retval FALSE            There is no data waiting to be read from the device.

**/
BOOLEAN
EFIAPI
KeyboardPoll (
  VOID
  )
{
  return FALSE;
}

/**
  Reads data from a USB keyboard into a buffer.
  This function will wait till specified number of bytes are filled.

  @param  Buffer           Pointer to the data buffer to store the data read from the device.
  @param  NumberOfBytes    Number of bytes to read from the device.

  @retval                  The number of bytes read from the device.

**/
UINTN
EFIAPI
KeyboardRead (
  OUT UINT8   *Buffer,
  IN  UINTN   NumberOfBytes
  )
{
  return 0;
}
