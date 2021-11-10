/** @file
  RLE Decompress Library header.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
