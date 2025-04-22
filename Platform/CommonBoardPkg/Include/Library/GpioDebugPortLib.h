/** @file
  Provide GPIO debug port library functions.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __GPIO_DEBUG_PORT_LIB_H__
#define __GPIO_DEBUG_PORT_LIB_H__


/**
  Write data from buffer to the console.

  Writes NumberOfBytes data bytes from Buffer to the virtual console.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to write to the console.

  @retval >0               The number of bytes written to the console.
**/
UINTN
EFIAPI
GpioDebugPortWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  );

#endif
