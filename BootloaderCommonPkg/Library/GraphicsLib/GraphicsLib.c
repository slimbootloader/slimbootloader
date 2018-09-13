/** @file
  Basic graphics rendering support

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/GraphicsLib.h>
#include <Library/PrintLib.h>

EFI_GRAPHICS_OUTPUT_BLT_PIXEL Colors[16] = {
  //
  // B     G     R
  //
  {0x00, 0x00, 0x00, 0x00},  // BLACK
  {0x98, 0x00, 0x00, 0x00},  // BLUE
  {0x00, 0x98, 0x00, 0x00},  // GREEN
  {0x98, 0x98, 0x00, 0x00},  // CYAN
  {0x00, 0x00, 0x98, 0x00},  // RED
  {0x98, 0x00, 0x98, 0x00},  // MAGENTA
  {0x00, 0x98, 0x98, 0x00},  // BROWN
  {0x98, 0x98, 0x98, 0x00},  // LIGHTGRAY
  {0x30, 0x30, 0x30, 0x00},  // DARKGRAY - BRIGHT BLACK
  {0xff, 0x00, 0x00, 0x00},  // LIGHTBLUE
  {0x00, 0xff, 0x00, 0x00},  // LIGHTGREEN
  {0xff, 0xff, 0x00, 0x00},  // LIGHTCYAN
  {0x00, 0x00, 0xff, 0x00},  // LIGHTRED
  {0xff, 0x00, 0xff, 0x00},  // LIGHTMAGENTA
  {0x00, 0xff, 0xff, 0x00},  // YELLOW
  {0xff, 0xff, 0xff, 0x00},  // WHITE
};

/**
  Copy image into frame buffer.

  @param[in] FrameBuffer         Frame buffer instance
  @param[in] GopBlt              The source image
  @param[in] Width               Width of the source image
  @param[in] Height              Height of the source image
  @param[in] OffX                Desired X offset in frame buffer
  @param[in] OffY                Desired Y offset in frame buffer

  @retval EFI_SUCCESS            Copy was successful
  @retval EFI_INVALID_PARAMETER  Image dimensions fall outside the framebuffer

**/
EFI_STATUS
EFIAPI
BltToFrameBuffer (
  IN FRAME_BUFFER_INFO *FrameBuffer,
  IN VOID              *GopBlt,
  IN UINTN             Width,
  IN UINTN             Height,
  IN UINTN             OffX,
  IN UINTN             OffY
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION *GopBltPixels;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL       Pixel;
  UINT32                              FrameBufferOffset;
  volatile UINT32                     *FrameBufferPtr;
  UINTN                               Row, Col;

  GopBltPixels = GopBlt;
  ASSERT (FrameBuffer != NULL);
  ASSERT (GopBlt != NULL);

  // Check dimensions
  if (((Height + OffY) > FrameBuffer->VerticalResolution)
      || ((Width + OffX) > FrameBuffer->HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  // Copy image into frame buffer
  FrameBufferPtr = (volatile UINT32 *) (UINTN) (FrameBuffer->LinearFrameBuffer);
  FrameBufferOffset = OffY * FrameBuffer->HorizontalResolution + OffX;
  for (Row = 0; Row < Height; Row++) {
    for (Col = 0; Col < Width; Col++) {
      Pixel = GopBltPixels[Row * Width + Col].Pixel;
      FrameBufferPtr[FrameBufferOffset + Col] = 0 \
          | ((Pixel.Red   & FrameBuffer->Red.Mask)   << FrameBuffer->Red.Position) \
          | ((Pixel.Green & FrameBuffer->Green.Mask) << FrameBuffer->Green.Position) \
          | ((Pixel.Blue  & FrameBuffer->Blue.Mask)  << FrameBuffer->Blue.Position);
    }
    FrameBufferOffset += FrameBuffer->HorizontalResolution;
  }

  return EFI_SUCCESS;
}

/**
  Draw a glyph into the frame buffer (ASCII only).

  @param[in] FrameBuffer         Frame buffer instance
  @param[in] Glyph               ASCII character to write
  @param[in] ForegroundColor     Foreground color to use
  @param[in] BackgroundColor     Background color to use
  @param[in] OffX                Desired X offset
  @param[in] OffY                Desired Y offset

  @retval EFI_SUCCESS            Success
  @retval EFI_INVALID_PARAMETER  Could not draw entire glyph in frame buffer

**/
EFI_STATUS
EFIAPI
BltGlyphToFrameBuffer (
  IN FRAME_BUFFER_INFO             *FrameBuffer,
  IN CHAR8                         Glyph,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor,
  IN UINTN                         OffX,
  IN UINTN                         OffY
  )
{
  UINT8                         *GlyphBitmap;
  CONST UINT8                   GlyphTableStart = 0x20;
  UINTN                         Width, Height;
  UINTN                         Row, Col;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL GopBlt[GLYPH_WIDTH * GLYPH_HEIGHT];

  Width = GLYPH_WIDTH;
  Height = GLYPH_HEIGHT;

  ASSERT (FrameBuffer != NULL);

  if ((Glyph < 0) || (Glyph > 0x7f)) {
    return EFI_INVALID_PARAMETER;
  }

  // Render ASCII characters 0-0x1f as whitespace
  if (Glyph < GlyphTableStart) {
    Glyph = 0x20;
  }

  // Glyph table maps to ASCII characters, index the table with the character
  GlyphBitmap = gUsStdNarrowGlyphData[ (UINTN)Glyph - GlyphTableStart].GlyphCol1;

  for (Row = 0; Row < Height; Row++) {
    for (Col = 0; Col < Width; Col++) {
      GopBlt[Row * Width + Col] = ((GlyphBitmap[Row] & (1 << (Width - Col - 1))) != 0) ? ForegroundColor : BackgroundColor;
    }
  }

  return BltToFrameBuffer (FrameBuffer, GopBlt, Width, Height, OffX, OffY);
}

/**
  Initialize the frame buffer console.

  @param[in] FrameBuffer         Frame buffer instance
  @param[in, out] Console             Console instance
  @param[in] Width               Width of the console (in pixels)
  @param[in] Height              Height of the console (in pixels)
  @param[in] OffX                Desired X offset of the console
  @param[in] OffY                Desired Y offset of the console

  @retval EFI_SUCCESS            Success
  @retval EFI_INVALID_PARAMETER  Could not fit entire console in frame buffer

**/
EFI_STATUS
EFIAPI
InitFrameBufferConsole (
  IN     FRAME_BUFFER_INFO    *FrameBuffer,
  IN OUT FRAME_BUFFER_CONSOLE *Console,
  IN     UINTN                Width,
  IN     UINTN                Height,
  IN     UINTN                OffX,
  IN     UINTN                OffY
  )
{
  ASSERT (FrameBuffer != NULL);
  ASSERT (Console != NULL);

  // Check dimensions
  if (((Height + OffY) > FrameBuffer->VerticalResolution)
      || ((Width + OffX) > FrameBuffer->HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  Console->FrameBuffer = FrameBuffer;
  Console->OffX        = OffX;
  Console->OffY        = OffY;
  Console->Width       = Width;
  Console->Height      = Height;
  Console->Rows        = Height / GLYPH_HEIGHT;
  Console->Cols        = Width / GLYPH_WIDTH;
  Console->CursorX     = 0;
  Console->CursorY     = 0;

  return EFI_SUCCESS;
}

/**
  Scroll the console area of the screen up.

  @param[in] Console      The console instance
  @param[in] ScrollAmount Amount (in pixels) to scroll

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
FrameBufferConsoleScroll (
  IN FRAME_BUFFER_CONSOLE *Console,
  IN UINTN                ScrollAmount
  )
{
  UINTN  Row;
  UINT32 Src, Dest;
  UINT32 *FrameBufferPtr;

  ASSERT (Console != NULL);

  // Copy each line to make space at the end
  FrameBufferPtr = (UINT32 *) (UINTN) (Console->FrameBuffer->LinearFrameBuffer);
  Dest = Console->OffY * Console->FrameBuffer->HorizontalResolution + Console->OffX;
  Src  = Dest + ScrollAmount * Console->FrameBuffer->HorizontalResolution;

  for (Row = ScrollAmount; Row < Console->Height; Row++) {
    CopyMem (FrameBufferPtr + Dest,
             FrameBufferPtr + Src,
             Console->Width * 4);
    Dest += Console->FrameBuffer->HorizontalResolution;
    Src  += Console->FrameBuffer->HorizontalResolution;
  }

  // Blank new area
  for (Row = Console->Height - ScrollAmount; Row < Console->Height; Row++) {
    ZeroMem (FrameBufferPtr + Dest, Console->Width * 4);
    Dest += Console->FrameBuffer->HorizontalResolution;
  }

  return EFI_SUCCESS;
}

/**
 Write a string to the frame buffer console.

 @param[in] Console Console instance
 @param[in] Str     ASCII string to write
 @param[in] Length  Length of the string
 @param[in] ForegroundColor Fore ground color parameter
 @param[in] BackgroundColor Back ground color parameter
 @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
FrameBufferConsoleWrite (
  IN FRAME_BUFFER_CONSOLE          *Console,
  IN UINT8                         *Str,
  IN UINTN                         Length,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor
  )
{
  EFI_STATUS Status;
  UINTN      Pos;

  ASSERT (Console != NULL);
  ASSERT (Str != NULL);

  for (Pos = 0; Pos < Length; Pos++) {
    // Continue on next line when cursor overflows columns
    if (Console->CursorX >= Console->Cols) {
      Console->CursorX = 0;
      Console->CursorY++;
    }

    // Create new line when cursor overflows rows
    if (Console->CursorY >= Console->Rows) {
      FrameBufferConsoleScroll (Console, GLYPH_HEIGHT);
      Console->CursorY = Console->Rows - 1;
      Console->CursorX = 0;
    }

    if (Str[Pos] == '\n') {
      // Newline
      Console->CursorX = 0;
      Console->CursorY++;
    } else if (Str[Pos] == '\r') {
      // Carriage Return
      Console->CursorX = 0;
    } else {
      Status = BltGlyphToFrameBuffer (Console->FrameBuffer, Str[Pos],
                                      ForegroundColor, BackgroundColor,
                                      Console->OffX + Console->CursorX * GLYPH_WIDTH,
                                      Console->OffY + Console->CursorY * GLYPH_HEIGHT);
      if (Status != EFI_SUCCESS) {
        return Status;
      }
      Console->CursorX++;
    }
  }

  return EFI_SUCCESS;
}

/**
  Write a formatted (printf style) message to the console.

  If Format is NULL, then ASSERT().

  @param[in]  Console         Console instance
  @param[in]  Format          Format string for the message to print.
  @param[in]  ...             Variable argument list whose contents are accessed
                              based on the format string specified by Format.

**/
EFI_STATUS
EFIAPI
FrameBufferConsolePrint (
  IN FRAME_BUFFER_CONSOLE *Console,
  IN CONST CHAR16         *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_MESSAGE_LENGTH];
  VA_LIST  Marker;

  ASSERT (Console != NULL);
  ASSERT (Format != NULL);

  VA_START (Marker, Format);
  AsciiVSPrintUnicodeFormat (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);
  return FrameBufferConsoleWrite (Console,
                                  (UINT8 *)Buffer, AsciiStrLen (Buffer),
                                  Colors[WHITE], Colors[BLACK]);
}

/**
  Write a formatted (printf style) message to the console.

  If Format is NULL, then ASSERT().

  @param[in]  Console         Console instance
  @param[in]  ForegroundColor Foreground color to use
  @param[in]  BackgroundColor Background color to use
  @param[in]  Format          Format string for the message to print.
  @param[in]  ...             Variable argument list whose contents are accessed
                              based on the format string specified by Format.

**/
EFI_STATUS
EFIAPI
FrameBufferConsolePrintEx (
  IN FRAME_BUFFER_CONSOLE          *Console,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor,
  IN CONST CHAR16                  *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_MESSAGE_LENGTH];
  VA_LIST  Marker;

  ASSERT (Console != NULL);
  ASSERT (Format != NULL);

  VA_START (Marker, Format);
  AsciiVSPrintUnicodeFormat (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);
  return FrameBufferConsoleWrite (Console,
                                  (UINT8 *)Buffer, AsciiStrLen (Buffer),
                                  ForegroundColor, BackgroundColor);
}
