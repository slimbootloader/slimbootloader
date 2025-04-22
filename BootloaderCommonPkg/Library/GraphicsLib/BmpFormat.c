/** @file
  BMP Image Decoding

  Copyright (c) 2004 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GraphicsLib.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Guid/GraphicsInfoHob.h>


/**
  Verify a BMP image header and determine its centralized display location
  on screen.

  @param  BmpImage      Pointer to BMP file
  @param  OffX          Pointer to receive X offset for BMP display position.
  @param  OffY          Pointer to receive Y offset for BMP display position.
  @param  GfxInfoHob    Pointer to graphics info HOB.

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
**/
EFI_STATUS
EFIAPI
GetBmpDisplayPos (
  IN     VOID    *BmpImage,
  OUT  UINT32    *OffX,          OPTIONAL
  OUT  UINT32    *OffY,          OPTIONAL
  IN   EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob
)
{
  BMP_IMAGE_HEADER              *BmpHeader;
  UINTN                         PosX;
  UINTN                         PosY;
  UINTN                         PixelHeight;
  UINTN                         PixelWidth;
  UINT64                        BltBufferSize;
  UINT32                        DataSizePerLine;
  UINT32                        ColorMapNum;

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
  PixelWidth   = BmpHeader->PixelWidth;
  PixelHeight  = BmpHeader->PixelHeight;

  // Center the BMP image on the display
  PosX = (GfxInfoHob->GraphicsMode.HorizontalResolution - PixelWidth) / 2;
  PosY = (GfxInfoHob->GraphicsMode.VerticalResolution - PixelHeight) / 2;

  // Check dimensions
  if (((PixelHeight + PosY) > GfxInfoHob->GraphicsMode.VerticalResolution)
      || ((PixelWidth + PosX) > GfxInfoHob->GraphicsMode.HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  if (OffX != NULL) {
    *OffX = (UINT32)PosX;
  }

  if (OffY != NULL) {
    *OffY = (UINT32)PosY;
  }

  return EFI_SUCCESS;
}

/**
  Display a *.BMP graphics image to the frame buffer or BLT buffer. If a NULL
  GopBlt buffer is passed in, the BMP image will be displayed into BLT memory
  buffer. Otherwise, it will be dispalyed into the actual frame buffer.

  @param  BmpImage      Pointer to BMP file
  @param  BmpImageSize  Size of BMP file
  @param  GopBlt        Buffer for transferring BmpImage to the BLT memory buffer.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  GfxInfoHob    Pointer to graphics info HOB.

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
EFIAPI
DisplayBmpToFrameBuffer (
  IN     VOID      *BmpImage,
  IN     UINT32    BmpImageSize,
  IN     VOID      *GopBlt,
  IN     UINTN     GopBltSize,
  IN     EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob
  )
{
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltLineBuf;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  BOOLEAN                       IsAllocated;
  UINTN                         Index;
  UINTN                         Height;
  UINTN                         Width;
  UINTN                         ImageIndex;
  UINTN                         PixelHeight;
  UINTN                         PixelWidth;
  UINT32                        OffX;
  UINT32                        OffY;
  UINT64                        DataSize;
  UINT64                        DataSizePerLine;
  UINT64                        LineBufferSize;
  UINT64                        BltBufferSize;
  UINT32                        FrameBufferOffset;
  UINT32                        *FrameBufferPtr;
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  EFI_STATUS                    Status;

  if (BmpImage == NULL || BmpImageSize == MAX_UINT32) {
    return EFI_INVALID_PARAMETER;
  }

  if (BmpImageSize < sizeof (BMP_IMAGE_HEADER)) {
    DEBUG ((DEBUG_ERROR, "DisplayBmpToFrameBuffer: BmpImageSize too small\n"));
    return EFI_UNSUPPORTED;
  }

  BmpHeader    = (BMP_IMAGE_HEADER *) BmpImage;
  PixelWidth   = BmpHeader->PixelWidth;
  PixelHeight  = BmpHeader->PixelHeight;
  // DataSizePerLine in bits
  DataSizePerLine = MultU64x32(PixelWidth, BmpHeader->BitPerPixel);
  // Convert to bytes and round to 4byte alignment
  DataSizePerLine = ((DataSizePerLine+31) >> 3) &(~0x3);
  // Calculate total Data Size we expect
  DataSize = MultU64x32(DataSizePerLine, (UINT32)PixelHeight);

  if ((PixelHeight == 0) || (PixelWidth == 0)) {
    DEBUG ((DEBUG_ERROR, "DisplayBmpToFrameBuffer: BmpHeader->PixelHeight or BmpHeader->PixelWidth is 0.\n"));
    return EFI_UNSUPPORTED;
  }

  // Make sure BMP header doesn't describe any pixels which would fall
  // outside BMP pixel data buffer.
  if ((BmpHeader->Size != BmpImageSize) ||
      (BmpHeader->Size < BmpHeader->ImageOffset) ||
      (BmpHeader->Size - BmpHeader->ImageOffset != DataSize))
  {
    DEBUG ((DEBUG_ERROR, "DisplayBmpToFrameBuffer: invalid BmpImage... \n"));
    DEBUG ((DEBUG_ERROR, "   BmpHeader->Size: 0x%x\n", BmpHeader->Size));
    DEBUG ((DEBUG_ERROR, "   BmpHeader->ImageOffset: 0x%x\n", BmpHeader->ImageOffset));
    DEBUG ((DEBUG_ERROR, "   BmpImageSize: 0x%x\n", BmpImageSize));
    DEBUG ((DEBUG_ERROR, "   DataSize: 0x%lx\n", DataSize));

    return EFI_UNSUPPORTED;
  }

  Status = GetBmpDisplayPos (BmpImage, &OffX, &OffY, GfxInfoHob);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  BmpColorMap  = (BMP_COLOR_MAP *) &BmpHeader[1];
  Image        = ((UINT8 *) BmpImage) + BmpHeader->ImageOffset;
  ImageHeader  = Image;

  //
  // Calculate the BltBuffer size needed for one line of splashing at a time.
  //
  LineBufferSize = MultU64x32 ((UINT64)PixelWidth, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  IsAllocated   = FALSE;
  if (GopBlt == NULL) {
    //
    // GopBlt is not allocated by caller.
    //
    BltLineBuf  = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) AllocateTemporaryMemory ((UINTN)LineBufferSize);
    if (BltLineBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    IsAllocated = TRUE;
    FrameBufferPtr    = (UINT32 *) (((UINTN) GfxInfoHob->FrameBufferBase));
    FrameBufferOffset = (UINT32)((OffY + PixelHeight) * GfxInfoHob->GraphicsMode.HorizontalResolution + OffX);
  } else {
    //
    // GopBlt has been allocated by caller.
    //
    BltBufferSize = MultU64x32 (LineBufferSize, (UINT32)PixelHeight);
    if (GopBltSize < (UINTN) BltBufferSize) {
      return EFI_BUFFER_TOO_SMALL;
    }
    BltLineBuf = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)GopBlt;
  }

  //
  // Convert image from BMP to Blt buffer format
  //
  Status = EFI_SUCCESS;
  for (Height = 0; Height < BmpHeader->PixelHeight; Height++) {
    Blt = BltLineBuf;
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
    if (GopBlt == NULL) {
      CopyMem (&FrameBufferPtr[FrameBufferOffset], BltLineBuf, Width * 4);
      FrameBufferOffset -= GfxInfoHob->GraphicsMode.HorizontalResolution;
    } else {
      BltLineBuf += PixelWidth;
    }
  }

End:
  if (IsAllocated) {
    FreePool (BltLineBuf);
  }

  return Status;
}
