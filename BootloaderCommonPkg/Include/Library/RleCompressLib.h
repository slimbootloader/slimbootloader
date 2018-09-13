/** @file
  UEFI Decompress Library implementation refer to UEFI specification.

  Copyright (c) 2006 - 2015, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _LRE_COMPRESS_LIB_H_
#define _LRE_COMPRESS_LIB_H_

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
  );

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
  );

#endif