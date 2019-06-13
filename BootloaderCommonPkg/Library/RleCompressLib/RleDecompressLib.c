/** @file
  UEFI Decompress Library implementation refer to UEFI specification.

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

/**
  Decompress data blob using RLE.

  @param  Data             Source data buffer pointer.
  @param  Length           Source data size.
  @param  Buffer           Destination data buffer.
                           If NULL, no data will be written.
                           The caller needs to ensure the buffer size is big enough.

  @retval Decompressed data length.

**/
UINTN
RleDecompressData (
  IN      UINT8       *Data,
  IN      UINTN        Length,
  IN OUT  UINT8       *Buffer
  )
{
  UINTN  Index;
  UINT16 LastChar;
  UINTN  LastCharCount;
  UINT8  CurrentChar;
  UINT8  Repeat;
  UINTN  DeCompressedIndex;

  LastChar = (UINT16) -1;
  LastCharCount = 0;
  DeCompressedIndex = 0;
  for (Index = 0; Index < Length; Index++) {
    CurrentChar = Data[Index];
    if (LastCharCount == 2) {
      LastCharCount = 0;
      Repeat = CurrentChar;
      if (Buffer != NULL) {
        SetMem (Buffer, Repeat, (UINT8)LastChar);
        Buffer  += Repeat;
      }
      DeCompressedIndex += Repeat;
      LastChar = (UINT16) -1;
    } else {
      if (LastChar != CurrentChar) {
        LastCharCount = 0;
      }
      LastCharCount++;
      if (Buffer != NULL) {
        *Buffer++ = CurrentChar;
      }
      LastChar  = CurrentChar;
      DeCompressedIndex++;
    }
  }

  return DeCompressedIndex;
}

