/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BoardInitLib.h>
#include <Library/DecompressLib.h>

/**
  Load Stage1B raw image to destination address.

  The destination could be either memory address or flash address.

  @param[in]  Dst    The destination address for image loading.
  @param[in]  Src    The source address for image loading.
  @param[in]  Len    The size for destination buffer.

  @retval     EFI_SUCCESS  Always return success.

**/
EFI_STATUS
LoadStage1B (
  IN UINT32   Dst,
  IN UINT32   Src,
  IN UINT32   Len
  )
{
  if (Dst != Src) {
    CopyMem ((VOID *)Dst, (VOID *)Src, Len);
  }
  return EFI_SUCCESS;
}

/**
  Load Stage2 raw image to destination address.

  The destination could be either memory address or flash address.

  @param[in]  Dst    The destination address for image loading.
  @param[in]  Src    The source address for image loading.
  @param[in]  Len    The size for destination buffer.

  @retval     EFI_SUCCESS  Always return success.

 **/
EFI_STATUS
LoadStage2 (
  IN UINT32   Dst,
  IN UINT32   Src,
  IN UINT32   Len
  )
{
  if (Dst != Src) {
    CopyMem ((VOID *)Dst, (VOID *)Src, Len);
  }
  return EFI_SUCCESS;
}

/**
  Load payload raw image to destination address.

  The destination could be either memory address or flash address.

  @param[in]  Dst    The destination address for image loading.
  @param[in]  Src    The source address for image loading.
  @param[in]  Len    The size for destination buffer.

  @retval   EFI_SUCCESS           Always return success.
  @retval   EFI_NOT_FOUND         Could not find the payload image.
  @retval   EFI_BUFFER_TOO_SMALL  Dstination buffer is too small.
  @retval   Others                Failed to load firmware.
 **/
EFI_STATUS
EFIAPI
LoadPayload (
  UINT32                         Dst,
  UINT32                         Src,
  UINT32                         Size
  )
{
  UINT32                          ActualSize;
  LOADER_COMPRESSED_HEADER       *Hdr;

  Hdr = (LOADER_COMPRESSED_HEADER *)Src;
  if (IS_COMPRESSED (Hdr)) {
    ActualSize = sizeof(LOADER_COMPRESSED_HEADER) + Hdr->CompressedSize;
  } else {
    ActualSize = Size;
  }

  if (Dst != Src) {
    if (Size < ActualSize) {
      return EFI_BUFFER_TOO_SMALL;
    }
    CopyMem ((VOID *)Dst, (VOID *)Src, ActualSize);
  }

  return EFI_SUCCESS;
}
