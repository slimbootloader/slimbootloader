/** @file
  Log buffer library

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DEBUG_LOG_BUFFER_LIB_H__
#define __DEBUG_LOG_BUFFER_LIB_H__

#define  DEBUG_LOG_BUFFER_SIGNATURE         SIGNATURE_32 ('D', 'L', 'O', 'G')

#define  DEBUG_LOG_BUFFER_ATTRIBUTE_FULL    BIT0

typedef struct {
  UINT32  Signature;
  UINT8   HeaderLength;
  UINT8   Attribute;
  UINT8   Reserved[2];
  UINT32  UsedLength;
  UINT32  TotalLength;
  UINT8   Buffer[0];
} DEBUG_LOG_BUFFER_HEADER;

/**
  Write data from buffer to console buffer.

  Writes NumberOfBytes data bytes from Buffer to the serial device.
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.
                           If this value is less than NumberOfBytes, then the write operation failed.

**/
UINTN
EFIAPI
DebugLogBufferWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  );

#endif

