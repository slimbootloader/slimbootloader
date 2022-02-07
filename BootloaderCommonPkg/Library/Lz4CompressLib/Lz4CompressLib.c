/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011-present, Yann Collet.
   Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
    - LZ4 homepage : http://www.lz4.org
    - LZ4 source repository : https://github.com/lz4/lz4

*/

#include "Lz4.h"

#define  CTX_ALIGN_SIZE   8

/**
  Given a LZ4 source buffer, this function retrieves the required
  size of the compressed buffer size in the worst case and the
  required scratch buffer size.

  @param[in]  Source          The source buffer containing the data to be compressed.
  @param[in]  SourceSize      The size, in bytes, of the source buffer.
  @param[out] DestinationSize A pointer to the size, in bytes, of the required data
                              buffer size for decompression.
  @param      ScratchSize     A pointer to the size, in bytes, of the required scratch
                              buffer size for decompression.

  @retval  RETURN_SUCCESS     The size of the uncompressed data was returned
                              in DestinationSize and the size of the scratch
                              buffer was returned in ScratchSize.

**/
RETURN_STATUS
EFIAPI
Lz4CompressGetInfo (
  IN  CONST VOID  *Source,
  IN  UINT32       SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  )
{
  UINT32  OutputSize;
  UINT32  ReqScratchSize;

  if (DestinationSize) {
    OutputSize = LZ4_compressBound (SourceSize);
    if (DestinationSize != NULL) {
      *DestinationSize = OutputSize + sizeof(UINT32);
    }
  }

  if (ScratchSize) {
    ReqScratchSize = (UINT32) LZ4_get_ctx_size ();
    // Request more for 8-byte alignment
    *ScratchSize = ReqScratchSize + CTX_ALIGN_SIZE;
  }

  return RETURN_SUCCESS;
}

/**
  Compresses a source buffer with LZ4.

  Extracts decompressed data to its original form. The caller should first call
  Lz4CompressGetInfo() to retrieve the required decompression buffer
  size and scratch size, and then use this function to compress the data.

  @param[in]  Source      The source buffer containing the compressed data.
  @param[in]  SourceSize  The size of source buffer.
  @param[in]  Destination The destination buffer to store the decompressed data.
                          The size of the destination buffer must be equal or larger than
                          the size returned by Lz4CompressGetInfo ().
  @param[out] DestinationSize
                          A pointer to receive the size, in bytes, of the decompressed data
                          buffer size.
  @param[in]  Scratch     A temporary scratch buffer that is used to perform the decompression.
                          The size of the scratch buffer must be equal or larger than the size
                          returned by Lz4CompressGetInfo ().

  @retval  RETURN_SUCCESS Decompression completed successfully, and
                          the uncompressed buffer is returned in Destination.
  @retval  RETURN_INVALID_PARAMETER
                          The source buffer specified by Source is corrupted
                          (not in a valid compressed format).
**/
RETURN_STATUS
EFIAPI
Lz4Compress (
  IN CONST VOID  *Source,
  IN UINT32       SourceSize,
  IN OUT VOID    *Destination,
  OUT UINT32     *DestinationSize,
  IN OUT VOID    *Scratch
  )
{
  INT32        Size;
  CHAR8       *DstPtr;

  if ((Source == NULL) || (Destination == NULL) || (Scratch == NULL)) {
    return RETURN_INVALID_PARAMETER;
  }

  DstPtr  = (CHAR8 *)Destination + sizeof(UINT32);
  Scratch = ALIGN_POINTER (Scratch, CTX_ALIGN_SIZE);
  Size = (UINT32)LZ4_compressHC_withStateHC (Scratch, Source, DstPtr, SourceSize);
  if (Size <= 0) {
    return RETURN_INVALID_PARAMETER;
  } else {
    if (DestinationSize != NULL) {
      // Insert the uncompressed data length at the beginning
      *DestinationSize = Size + sizeof(UINT32);
      *(UINT32 *)Destination = SourceSize;
    }
    return RETURN_SUCCESS;
  }
}
