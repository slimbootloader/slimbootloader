/** @file
  BMP Image Decoding

  Copyright (c) 2004 - 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GraphicsLib.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Guid/GraphicsInfoHob.h>

/**
  Display a *.BMP graphics image to the frame buffer. If a NULL GopBlt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  GopBlt        Buffer for transferring BmpImage to the frame buffer.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  GfxInfoHob    Pointer to graphics info HOB.

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
DisplayBmpToFrameBuffer (
  IN     VOID      *BmpImage,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
  IN     EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob
  )
{
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  BOOLEAN                       IsAllocated;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         ImageIndex;
  UINTN                         OffX;
  UINTN                         OffY;
  UINTN                         PixelHeight;
  UINTN                         PixelWidth;
  UINT64                        BltBufferSize;
  UINT32                        DataSizePerLine;
  UINT32                        ColorMapNum;
  UINT32                        FrameBufferOffset;
  UINT32                        *FrameBufferPtr;
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  EFI_STATUS                    Status;

  BmpHeader = (BMP_IMAGE_HEADER *) BmpImage;

  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  //
  // Doesn't support compress.
  //
  if (BmpHeader->CompressionType != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Only support BITMAPINFOHEADER format.
  // BITMAPFILEHEADER + BITMAPINFOHEADER = BMP_IMAGE_HEADER
  //
  if (BmpHeader->HeaderSize != sizeof (BMP_IMAGE_HEADER) - OFFSET_OF (BMP_IMAGE_HEADER, HeaderSize)) {
    return EFI_UNSUPPORTED;
  }

  //
  // The data size in each line must be 4 byte alignment.
  //
  DataSizePerLine = ((BmpHeader->PixelWidth * BmpHeader->BitPerPixel + 31) >> 3) & (~0x3);
  BltBufferSize = MultU64x32 (DataSizePerLine, BmpHeader->PixelHeight);
  if (BltBufferSize > (UINT32) ~0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BmpHeader->Size < BmpHeader->ImageOffset) ||
      (BmpHeader->Size - BmpHeader->ImageOffset !=  BmpHeader->PixelHeight * DataSizePerLine)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = BmpImage;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));
  if (BmpHeader->ImageOffset < sizeof (BMP_IMAGE_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BmpHeader->ImageOffset > sizeof (BMP_IMAGE_HEADER)) {
    switch (BmpHeader->BitPerPixel) {
    case 1:
      ColorMapNum = 2;
      break;
    case 4:
      ColorMapNum = 16;
      break;
    case 8:
      ColorMapNum = 256;
      break;
    default:
      ColorMapNum = 0;
      break;
    }
    //
    // BMP file may has padding data between the bmp header section and the bmp data section.
    //
    if (BmpHeader->ImageOffset - sizeof (BMP_IMAGE_HEADER) < sizeof (BMP_COLOR_MAP) * ColorMapNum) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  PixelWidth   = BmpHeader->PixelWidth;
  PixelHeight  = BmpHeader->PixelHeight;

  // Center the BMP image on the display
  OffX = (GfxInfoHob->GraphicsMode.HorizontalResolution - PixelWidth) / 2;
  OffY = (GfxInfoHob->GraphicsMode.VerticalResolution - PixelHeight) / 2;

  // Check dimensions
  if (((PixelHeight + OffY) > GfxInfoHob->GraphicsMode.VerticalResolution)
      || ((PixelWidth + OffX) > GfxInfoHob->GraphicsMode.HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Calculate the BltBuffer size needed for one line of splashing at a time.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  IsAllocated   = FALSE;
  if (*GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    *GopBltSize = (UINTN) BltBufferSize;
    *GopBlt     = AllocateTemporaryMemory (*GopBltSize);
    IsAllocated = TRUE;
    if (*GopBlt == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    if (*GopBltSize < (UINTN) BltBufferSize) {
      *GopBltSize = (UINTN) BltBufferSize;
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = *GopBlt;

  FrameBufferPtr = (UINT32 *) (((UINTN) GfxInfoHob->FrameBufferBase));
  FrameBufferOffset = (UINT32)((OffY + PixelHeight) * GfxInfoHob->GraphicsMode.HorizontalResolution + OffX);

  Status = EFI_SUCCESS;

  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = &BltBuffer[PixelWidth - 1];
    for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {
      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && Width < BmpHeader->PixelWidth; Index++) {
          Blt->Red    = BmpColorMap[ ((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[ ((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[ ((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          Width++;
        }

        Blt--;
        Width--;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (Width < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          Width++;
          Index       = (*Image) & 0x0f;
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 24:
        //
        // It is 24-bit BMP.
        //
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      default:
        //
        // Other bit format BMP is not supported.
        //
        Status = EFI_UNSUPPORTED;
        goto End;
      }
    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }

    CopyMem (&FrameBufferPtr[FrameBufferOffset], &BltBuffer[PixelWidth - 1], Width * 4);
    FrameBufferOffset -= GfxInfoHob->GraphicsMode.HorizontalResolution;
  }

End:
  if (IsAllocated) {
    FreePool (*GopBlt);
    *GopBlt = NULL;
  }

  return Status;
}
