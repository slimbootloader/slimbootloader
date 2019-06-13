/** @file
  Provide Console output library functions.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CONSOLE_OUT_LIB_H__
#define __CONSOLE_OUT_LIB_H__

#include <PiPei.h>
#include <Library/BaseLib.h>

typedef enum {
  ConsoleOutSerialPort  = BIT0,
  ConsoleOutFrameBuffer = BIT1,
  ConsoleOutAll         = 0xFFFFFFFF,
} CONSOLE_OUT_DEVICE_TYPE;

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
ConsoleWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  );

#endif
