/** @file
Header file for console input library

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CONSOLE_IN_H__
#define __CONSOLE_IN_H__

typedef enum {
  ConsoleInSerialPort  = BIT0,
  ConsoleInUsbKeyboard = BIT1,
  ConsoleInDebugPort   = BIT2,
  ConsoleInAll         = 0xFFFFFFFF,
} CONSOLE_IN_DEVICE_TYPE;


/**
  Poll a console to see if there is any data waiting to be read.

  @retval FALSE                There is no data waiting to be read from the console.
  @retval TRUE                 Data is waiting to be read from the console.

**/
BOOLEAN
EFIAPI
ConsolePoll (
  VOID
  );

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
  );

#endif
