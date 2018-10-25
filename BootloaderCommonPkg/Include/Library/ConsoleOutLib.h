/** @file
  Provide Console output library functions.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
