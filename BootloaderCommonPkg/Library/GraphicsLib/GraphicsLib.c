/** @file
  Basic graphics rendering support

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/GraphicsLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>

CONST EFI_GRAPHICS_OUTPUT_BLT_PIXEL mColors[16] = {
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

STATIC FRAME_BUFFER_CONSOLE mFbConsole;

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
  UINT32                               FrameBufferOffset;
  UINT32                              *FrameBufferPtr;
  UINTN                                Row;

  GopBltPixels = GopBlt;
  ASSERT (FrameBuffer != NULL);
  ASSERT (GopBlt != NULL);

  // Check dimensions
  if (((Height + OffY) > FrameBuffer->VerticalResolution)
      || ((Width + OffX) > FrameBuffer->HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  // Copy image into frame buffer
  FrameBufferPtr = (UINT32 *) (UINTN) (FrameBuffer->LinearFrameBuffer);
  FrameBufferOffset = OffY * FrameBuffer->HorizontalResolution + OffX;
  for (Row = 0; Row < Height; Row++) {
    CopyMem (&FrameBufferPtr[FrameBufferOffset], &GopBltPixels[Row * Width], Width * 4);
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

  if (FrameBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Glyph < 0) || (Glyph > 0x7f)) {
    return EFI_INVALID_PARAMETER;
  }

  Width = GLYPH_WIDTH;
  Height = GLYPH_HEIGHT;

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
  IN     UINTN                Width,
  IN     UINTN                Height,
  IN     UINTN                OffX,
  IN     UINTN                OffY
  )
{
  FRAME_BUFFER_CONSOLE  *Console;

  if (FrameBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Check dimensions
  if (((Height + OffY) > FrameBuffer->VerticalResolution)
      || ((Width + OffX) > FrameBuffer->HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  Console = &mFbConsole;
  Console->FrameBuffer = FrameBuffer;
  Console->OffX        = OffX;
  Console->OffY        = OffY;
  Console->Width       = Width;
  Console->Height      = Height;
  Console->Rows        = Height / GLYPH_HEIGHT;
  Console->Cols        = Width / GLYPH_WIDTH;
  Console->CursorX     = 0;
  Console->CursorY     = 0;
  Console->ForegroundColor = mColors[7];
  Console->BackgroundColor = mColors[0];
  Console->TextDisplayBuf = AllocateZeroPool (Console->Rows * Console->Cols);
  ASSERT (Console->TextDisplayBuf != NULL);
  Console->TextSwapBuf = AllocateZeroPool (Console->Rows * Console->Cols);
  ASSERT (Console->TextSwapBuf != NULL);

  return EFI_SUCCESS;
}

/**
  Scroll the console area of the screen up.

  @param[in] ScrollAmount Amount (in rows) to scroll

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
FrameBufferConsoleScroll (
  IN UINTN ScrollAmount
  )
{
  FRAME_BUFFER_CONSOLE *Console;
  UINTN BufX, BufY, BufPos;
  UINTN ScreenX, ScreenY;

  Console = &mFbConsole;
  if (Console->Height == 0) {
    return EFI_UNSUPPORTED;
  }

  if (ScrollAmount > Console->Rows) {
    ScrollAmount = Console->Rows;
  }

  if (ScrollAmount < Console->Rows) {
    // Move all lines in text buffer up
    CopyMem (&Console->TextSwapBuf[0],
             &Console->TextDisplayBuf[Console->Cols * ScrollAmount],
             Console->Cols * (Console->Rows - ScrollAmount));
  }

  // Blank remaining lines
  ZeroMem (&Console->TextSwapBuf[Console->Cols * (Console->Rows - ScrollAmount)],
           Console->Cols * ScrollAmount);

  // Write text buffer to screen
  //
  // Note: At this point, TextDisplayBuf contains what is currently being
  // displayed and TextSwapBuf contains what *should* be displayed. Step through
  // every character of the buffer and update the framebuffer with any
  // differences.
  BufPos = 0;
  ScreenY = Console->OffY;
  for (BufY = 0; BufY < Console->Rows; BufY++) {
    ScreenX = Console->OffX;
    for (BufX = 0; BufX < Console->Cols; BufX++) {
      if (Console->TextSwapBuf[BufPos] != Console->TextDisplayBuf[BufPos]) {
        Console->TextDisplayBuf[BufPos] = Console->TextSwapBuf[BufPos];
        BltGlyphToFrameBuffer (Console->FrameBuffer, Console->TextSwapBuf[BufPos],
                               Console->ForegroundColor, Console->BackgroundColor,
                               ScreenX, ScreenY);
      }
      BufPos++;
      ScreenX += GLYPH_WIDTH;
    }
    ScreenY += GLYPH_HEIGHT;
  }

  return EFI_SUCCESS;
}

/**
  Write data from buffer to graphics framebuffer.

  Writes NumberOfBytes data bytes from Buffer to the framebuffer device.
  The number of bytes actually written to the framebuffer is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written.
                           If this value is less than NumberOfBytes, then the write operation failed.

**/
UINTN
EFIAPI
FrameBufferWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  )
{
  FRAME_BUFFER_CONSOLE *Console;
  EFI_STATUS            Status;
  UINTN                 Pos;

  if (Buffer == NULL) {
    return 0;
  }

  Console = &mFbConsole;
  if (Console->Height == 0) {
    return 0;
  }

  for (Pos = 0; Pos < NumberOfBytes; Pos++) {
    // Continue on next line when cursor overflows columns
    if (Console->CursorX >= Console->Cols) {
      Console->CursorX = 0;
      Console->CursorY++;
    }

    // Create new line when cursor overflows rows
    if (Console->CursorY >= Console->Rows) {
      FrameBufferConsoleScroll (1);
      Console->CursorY = Console->Rows - 1;
      Console->CursorX = 0;
    }

    if (Buffer[Pos] == '\x00') {
      break;
    } else if (Buffer[Pos] == '\b') {
      if (Console->CursorX > 0) {
        // Move to previous column
        Console->CursorX--;
      } else {
        if (Console->CursorY != 0) {
          // Move to previous row
          Console->CursorY--;
          Console->CursorX = Console->Cols - 1;
        }
      }
    } else if (Buffer[Pos] == '\n') {
      // Newline
      Console->CursorX = 0;
      Console->CursorY++;
    } else if (Buffer[Pos] == '\r') {
      // Carriage Return
      Console->CursorX = 0;
    } else {
      Console->TextDisplayBuf[Console->CursorY * Console->Cols + Console->CursorX] = Buffer[Pos];
      Status = BltGlyphToFrameBuffer (Console->FrameBuffer, Buffer[Pos],
                                      Console->ForegroundColor, Console->BackgroundColor,
                                      Console->OffX + Console->CursorX * GLYPH_WIDTH,
                                      Console->OffY + Console->CursorY * GLYPH_HEIGHT);
      if (Status != EFI_SUCCESS) {
        break;
      }
      Console->CursorX++;
    }
  }

  return Pos;
}
