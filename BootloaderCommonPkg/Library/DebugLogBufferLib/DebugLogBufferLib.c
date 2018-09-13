/** @file
  Provide Log Buffer Library functions.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/LoaderPlatformDataGuid.h>

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
  )
{
  DEBUG_LOG_BUFFER_HEADER  *LogBufHdr;

  // This function will be called by DEBUG or ASSERT macro.
  // So please DON'T use DEBUG/ASSERT macro inside this function,
  // to avoid recursion.
  LogBufHdr = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
  if (LogBufHdr == NULL) {
    return 0;
  }

  if (LogBufHdr->UsedLength + NumberOfBytes > LogBufHdr->TotalLength) {
    NumberOfBytes = LogBufHdr->TotalLength - LogBufHdr->UsedLength;
  }

  if (NumberOfBytes > 0) {
    CopyMem ((UINT8 *)LogBufHdr + LogBufHdr->UsedLength, Buffer, NumberOfBytes);
    LogBufHdr->UsedLength += NumberOfBytes;
  }

  return NumberOfBytes;
}
