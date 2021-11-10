/** @file
  UEFI Decompress Library implementation.

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

/**
  Compress data blob using RLE.

  @param  Data             Source data buffer pointer.
  @param  Length           Source data size.
  @param  Buffer           Destination data buffer.
                           If NULL, no data will be written.
                           The caller needs to ensure the buffer size is big enough.

  @retval Compressed data length.

**/
UINTN
RleCompressData (
  IN      UINT8             *Data,
  IN      UINTN              Length,
  IN  OUT UINT8             *Buffer
  )
{
  UINTN                 Index;
  UINT8                 LastChar;
  UINT8                 LastCharCount;
  UINT8                 CurrentChar;
  UINTN                 CompressedIndex;

  LastChar      = Data[0] + 1; // Just ensure it's different from the first byte.
  LastCharCount = 0;
  CompressedIndex = 0;

  for (Index = 0; Index <= Length; Index++) {
    if ((LastCharCount == 255) || (Index == Length)) {
      CurrentChar = (UINT8) LastChar + 1; // just ensure it's different from LastChar
    } else {
      CurrentChar = Data[Index];
    }

    if (LastChar != CurrentChar) {
      if (LastCharCount == 1) {
        if (Buffer != NULL) {
          *Buffer++ = LastChar;
        }
        CompressedIndex += 1;
      } else if (LastCharCount >= 2) {
        CompressedIndex += 3;
        if (Buffer != NULL) {
          *Buffer++ = LastChar;
          *Buffer++ = LastChar;
        }
        if (LastCharCount == 255) {
          CurrentChar = Data[Index];
          LastChar    = CurrentChar + 1;
        }
        LastCharCount -= 2;
        if (Buffer != NULL) {
          *Buffer++ = LastCharCount;
        }
      }
      LastCharCount = 0;
    }
    LastCharCount++;
    LastChar = CurrentChar;
  }
  return CompressedIndex;
}
