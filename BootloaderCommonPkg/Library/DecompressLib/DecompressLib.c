/** @file
  Decompress interfaces

  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/LzmaDecompressLib.h>
#include <Library/Lz4CompressLib.h>
#include <Library/DecompressLib.h>

/**
  Given a compressed source buffer, this function retrieves the size of
  the uncompressed buffer and the size of the scratch buffer required
  to decompress the compressed source buffer.

  Retrieves the size of the uncompressed buffer and the temporary scratch buffer
  required to decompress the buffer specified by Source and SourceSize.
  The size of the uncompressed buffer is returned in DestinationSize,
  the size of the scratch buffer is returned in ScratchSize, and RETURN_SUCCESS is returned.
  ScratchSize is specific to the decompression implementation.

  @param  Signature       The signature to indicate the decompression algorithm.
  @param  Source          The source buffer containing the compressed data.
  @param  SourceSize      The size, in bytes, of the source buffer.
  @param  DestinationSize A pointer to the size, in bytes, of the uncompressed buffer
                          that will be generated when the compressed buffer specified
                          by Source and SourceSize is decompressed.
  @param  ScratchSize     A pointer to the size, in bytes, of the scratch buffer that
                          is required to decompress the compressed buffer specified
                          by Source and SourceSize.

  @retval  RETURN_SUCCESS The size of the uncompressed data was returned
                          in DestinationSize and the size of the scratch
                          buffer was returned in ScratchSize.

  @retval  RETURN_UNSUPPORTED  The decompression is not supported.

**/
RETURN_STATUS
EFIAPI
DecompressGetInfo (
  IN UINT32        Signature,
  IN  CONST VOID  *Source,
  IN  UINT32       SourceSize,
  OUT UINT32      *DestinationSize,
  OUT UINT32      *ScratchSize
  )
{
  RETURN_STATUS  Status;

  Status = RETURN_UNSUPPORTED;

  if (Signature == LZ4_SIGNATURE) {
    Status = Lz4DecompressGetInfo (Source, SourceSize, DestinationSize, ScratchSize);
  } else if (Signature == LZDM_SIGNATURE) {
    if (DestinationSize != NULL) {
      *DestinationSize = SourceSize;
    }
    if (ScratchSize != NULL) {
      *ScratchSize = 0;
    }
    Status = RETURN_SUCCESS;
  } else if (!FeaturePcdGet (PcdMinDecompression)) {
    if (Signature == LZMA_SIGNATURE) {
      Status = LzmaUefiDecompressGetInfo (Source, SourceSize, DestinationSize, ScratchSize);
    }
  }

  return Status;
}

/**
  Decompresses a compressed source buffer.

  Extracts decompressed data to its original form.
  If the compressed source data specified by Source is successfully decompressed
  into Destination, then RETURN_SUCCESS is returned.  If the compressed source data
  specified by Source is not in a valid compressed data format,
  then RETURN_INVALID_PARAMETER is returned.

  @param  Signature   The signature to indicate the decompression algorithm.
  @param  Source      The source buffer containing the compressed data.
  @param  SourceSize  The size of source buffer.
  @param  Destination The destination buffer to store the decompressed data
  @param  Scratch     A temporary scratch buffer that is used to perform the decompression.
                      This is an optional parameter that may be NULL if the
                      required scratch buffer size is 0.

  @retval  RETURN_SUCCESS Decompression completed successfully, and
                          the uncompressed buffer is returned in Destination.
  @retval  RETURN_INVALID_PARAMETER
                          The source buffer specified by Source is corrupted
                          (not in a valid compressed format).
**/
RETURN_STATUS
EFIAPI
Decompress (
  IN UINT32       Signature,
  IN CONST VOID  *Source,
  IN UINTN        SourceSize,
  IN OUT VOID    *Destination,
  IN OUT VOID    *Scratch
  )
{
  RETURN_STATUS  Status;

  Status = RETURN_UNSUPPORTED;
  if (Signature == LZ4_SIGNATURE) {
    Status = Lz4Decompress (Source, SourceSize, Destination, Scratch);
  } else if (Signature == LZDM_SIGNATURE) {
    CopyMem (Destination, Source, SourceSize);
    Status = RETURN_SUCCESS;
  } else if (!FeaturePcdGet (PcdMinDecompression)) {
    if (Signature == LZMA_SIGNATURE) {
      Status = LzmaUefiDecompress (Source, SourceSize, Destination, Scratch);
    }
  }

  return Status;
}
