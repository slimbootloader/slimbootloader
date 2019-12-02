/** @file
  Provide Console output library functions.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Library/GraphicsLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/HobLib.h>
#include <Library/ConsoleOutLib.h>

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
  )
{
  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutSerialPort) {
    SerialPortWrite ((UINT8 *)Buffer, NumberOfBytes);
  }

  if (PcdGet32 (PcdConsoleOutDeviceMask) & ConsoleOutFrameBuffer) {
    FrameBufferWrite ((UINT8 *)Buffer, NumberOfBytes);
  }

  return NumberOfBytes;
}
