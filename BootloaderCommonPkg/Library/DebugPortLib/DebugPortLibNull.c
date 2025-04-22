/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

/**
  Write data from buffer to debug port.

  Writes NumberOfBytes data bytes from Buffer to the debug port.
  The number of bytes actually written to the debug port is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the debug port.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the debug port.
**/
UINTN
EFIAPI
DebugPortWrite (
  IN UINT8    *Buffer,
  IN UINTN    NumberOfBytes
  )
{
  return 0;
}

/**
  Reads data from a debug port into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the debug port.
  @param  NumberOfBytes    Number of bytes to read from the debug port.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the debug port.
**/
UINTN
EFIAPI
DebugPortRead (
  OUT UINT8 *Buffer,
  IN UINTN  NumberOfBytes
  )
{
  return 0;
}

/**
  Polls a debug port to see if there is any data waiting to be read.

  Polls adebug port to see if there is any data waiting to be read.
  If there is data waiting to be read from the debug port, then TRUE is returned.
  If there is no data waiting to be read from the debug port, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the debug port.
  @retval FALSE            There is no data waiting to be read from the debug port.
**/
BOOLEAN
EFIAPI
DebugPortPoll (
  VOID
  )
{
  return FALSE;
}
