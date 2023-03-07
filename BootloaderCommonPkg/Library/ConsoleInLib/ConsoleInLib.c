/** @file
Implementation for console input library

Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/UsbKbLib.h>
#include <Library/SerialPortLib.h>
#include <Library/ConsoleInLib.h>
#include <Library/DebugPortLib.h>

/**
  Poll a console to see if there is any data waiting to be read.

  @retval FALSE                There is no data waiting to be read from the console.
  @retval TRUE                 Data is waiting to be read from the console.

**/
BOOLEAN
EFIAPI
ConsolePoll (
  VOID
  )
{

  if (FeaturePcdGet (PcdSourceDebugEnabled) != TRUE) {
    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInSerialPort) != 0) {
      if (SerialPortPoll ()) {
        return TRUE;
      }
    }
  }

  if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInUsbKeyboard) != 0) {
    if (KeyboardPoll ()) {
      return TRUE;
    }
  }

  if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInDebugPort) != 0) {
    if (DebugPortPoll ()) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Read data from a console device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the console device.
  @param  NumberOfBytes    Number of bytes to read from the console device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the console device.
                           If this value is less than NumberOfBytes, then the read operation failed.
**/
UINTN
EFIAPI
ConsoleRead (
  OUT UINT8 *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  volatile UINTN    Count;
  UINTN             ReadCount;

  Count = 0;
  while (Count < NumberOfBytes) {
    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInSerialPort) != 0) {
      if (FeaturePcdGet (PcdSourceDebugEnabled) != TRUE) {
        if (SerialPortPoll ()) {
          ReadCount = SerialPortRead (Buffer, 1);
          Buffer += ReadCount;
          Count  += ReadCount;
        }
      }
      // If only serial enabled, and Source Debug enabled, return
      // error here to prevent infinite loop and build error.
      else if (PcdGet32 (PcdConsoleInDeviceMask) == ConsoleInSerialPort) {
        return 0;
      }
    }

    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInUsbKeyboard) != 0) {
      if (KeyboardPoll () && (Count < NumberOfBytes)) {
        ReadCount = KeyboardRead (Buffer, 1);
        Buffer += ReadCount;
        Count  += ReadCount;
      }
    }

    if ((PcdGet32 (PcdConsoleInDeviceMask) & ConsoleInDebugPort) != 0) {
      if (DebugPortPoll () && (Count < NumberOfBytes)) {
        ReadCount = DebugPortRead (Buffer, 1);
        Buffer += ReadCount;
        Count  += ReadCount;
      }
    }
  }

  return NumberOfBytes;
}
