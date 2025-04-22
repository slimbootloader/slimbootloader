/** @file
  Basic graphics rendering support

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/GraphicsLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>

#define  ANSI_ESCAPE_SEQ_CLEAR_SCREEN    (UINT8 *)"\x1b[2J"

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

  @param[in] GfxInfoHob          Pointer to graphics info HOB
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
  IN EFI_PEI_GRAPHICS_INFO_HOB        *GfxInfoHob,
  IN VOID                             *GopBlt,
  IN UINTN                            Width,
  IN UINTN                            Height,
  IN UINTN                            OffX,
  IN UINTN                            OffY
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION *GopBltPixels;
  UINT32                               FrameBufferOffset;
  UINT32                              *FrameBufferPtr;
  UINTN                                Row;

  GopBltPixels = GopBlt;
  ASSERT (GfxInfoHob != NULL);
  ASSERT (GopBlt != NULL);

  // Check dimensions
  if (((Height + OffY) > GfxInfoHob->GraphicsMode.VerticalResolution)
      || ((Width + OffX) > GfxInfoHob->GraphicsMode.HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  // Copy image into frame buffer
  FrameBufferPtr = (UINT32 *) (UINTN) (GfxInfoHob->FrameBufferBase);
  FrameBufferOffset = (UINT32)(OffY * GfxInfoHob->GraphicsMode.HorizontalResolution + OffX);
  for (Row = 0; Row < Height; Row++) {
    CopyMem (&FrameBufferPtr[FrameBufferOffset], &GopBltPixels[Row * Width], Width * 4);
    FrameBufferOffset += GfxInfoHob->GraphicsMode.HorizontalResolution;
  }

  return EFI_SUCCESS;
}

/**
  Draw a glyph into the frame buffer (ASCII only).

  @param[in] GfxInfoHob          Pointer to graphics info HOB
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
  IN EFI_PEI_GRAPHICS_INFO_HOB     *GfxInfoHob,
  IN CHAR8                         Glyph,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor,
  IN UINTN                         OffX,
  IN UINTN                         OffY
  )
{
  UINT8                            *GlyphBitmap;
  UINTN                            Width, Height;
  UINTN                            Row;
  UINTN                            Col;
  UINTN                            Code;
  UINTN                            Base;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    GopBlt[GLYPH_WIDTH * GLYPH_HEIGHT];

  if (GfxInfoHob == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Width = GLYPH_WIDTH;
  Height = GLYPH_HEIGHT;

  // Glyph table maps to ASCII characters, index the table with the character
  Code = (UINTN)(Glyph & 0xFF);
  Base = 0xAF;
  if ((Code >= Base) && (Code <= 0xF2)) {
    Code = (0x80 - 0x20) + (Code - Base);
  } else if ((Code >= 0x20) && (Code <= 0x7F)) {
    Code = Code - 0x20;
  } else {
    Code = 0;
  }

  // Glyph table maps to ASCII characters, index the table with the character
  GlyphBitmap = gUsStdNarrowGlyphData[Code].GlyphCol1;

  for (Row = 0; Row < Height; Row++) {
    for (Col = 0; Col < Width; Col++) {
      GopBlt[Row * Width + Col] = ((GlyphBitmap[Row] & (1 << (Width - Col - 1))) != 0) ? ForegroundColor : BackgroundColor;
    }
  }

  return BltToFrameBuffer (GfxInfoHob, GopBlt, Width, Height, OffX, OffY);
}

/**
  Initialize the frame buffer console.

  @param[in] GfxInfoHob          Pointer to graphics info HOB
  @param[in] Width               Width of the console (in pixels)
  @param[in] Height              Height of the console (in pixels)
  @param[in] OffX                Desired X offset of the console
  @param[in] OffY                Desired Y offset of the console

  @retval EFI_SUCCESS            Success
  @retval EFI_ALREADY_STARTED    Framebuffer has been initialized already
  @retval EFI_INVALID_PARAMETER  Could not fit entire console in frame buffer

**/
EFI_STATUS
EFIAPI
InitFrameBufferConsole (
  IN EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob,
  IN     UINTN                 Width,
  IN     UINTN                 Height,
  IN     UINTN                 OffX,
  IN     UINTN                 OffY
  )
{
  FRAME_BUFFER_CONSOLE  *Console;
  BOOLEAN                ClearScreen;

  Console = &mFbConsole;
  if (Console->GfxInfoHob != NULL) {
    return EFI_ALREADY_STARTED;
  }

  if (GfxInfoHob == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // Check dimensions
  if (((Height + OffY) > GfxInfoHob->GraphicsMode.VerticalResolution)
      || ((Width + OffX) > GfxInfoHob->GraphicsMode.HorizontalResolution)) {
    return EFI_INVALID_PARAMETER;
  }

  // Don't clear screen for the 1st time to enter Shell
  // so that the logo will stay on screen.
  ClearScreen = (mFbConsole.TextDisplayBuf != NULL) ? TRUE : FALSE;

  Console->GfxInfoHob  = GfxInfoHob;
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
  Console->TextDrawBuf = AllocateZeroPool (Console->Rows * Console->Cols * 2);
  ASSERT (Console->TextDrawBuf != NULL);

  if (ClearScreen) {
    // Clear screen using standard ANSI Escape Sequences 'ESC[2J'
    FrameBufferWrite (ANSI_ESCAPE_SEQ_CLEAR_SCREEN, 4);
  }

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
  IN UINTN               ScrollAmount
  )
{
  FRAME_BUFFER_CONSOLE   *Console;
  UINTN                  BufX;
  UINTN                  BufY;
  UINTN                  BufPos;
  UINTN                  ScreenX;
  UINTN                  ScreenY;

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
        BltGlyphToFrameBuffer (Console->GfxInfoHob, Console->TextSwapBuf[BufPos],
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
  UINTN                 Length;
  EFI_PEI_GRAPHICS_INFO_HOB  *GfxInfoHob;

  if (Buffer == NULL) {
    return 0;
  }

  Console = &mFbConsole;
  if (Console->Height == 0) {
    return 0;
  }

  // Handle clear screen
  if ((NumberOfBytes == 4) && (CompareMem (ANSI_ESCAPE_SEQ_CLEAR_SCREEN, Buffer, 4)) == 0) {
    // Clear screen
    SetMem (Console->TextDisplayBuf, Console->Rows * Console->Cols, 0);
    // Zero framebuffer
    GfxInfoHob = Console->GfxInfoHob;
    Length = ((UINTN)GfxInfoHob->GraphicsMode.HorizontalResolution * GfxInfoHob->GraphicsMode.PixelsPerScanLine) * 4;
    SetMem64 ((UINT32 *) (UINTN)(GfxInfoHob->FrameBufferBase), Length, 0);
    Console->CursorX = 0;
    Console->CursorY = 0;
    return NumberOfBytes;
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
      Status = BltGlyphToFrameBuffer (Console->GfxInfoHob, Buffer[Pos],
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

/**
  Draw frame buffer from a given text buffer.
  The text buffer needs to be compatible with format below:
  Each text char takes 16 bits:
    BIT   7:0  ASCII char
    BIT  11:8  Foreground color
    BIT 15:12  Background color

  @param[in]  Row      Row number for the text buffer.
  @param[in]  Col      Column number for the text buffer.
  @param[in]  Buffer   The pointer to the text buffer.

  @retval EFI_NOT_READY          Frame buffer console has not been initialized yet.
  @retval EFI_INVALID_PARAMETER  Invalid parameters.
  @retval EFI_SUCCESS            Text buffer was drawn successfully.

**/
EFI_STATUS
EFIAPI
DrawFrameBuffer (
  IN UINT32     Row,
  IN UINT32     Col,
  IN UINT8     *Buffer
  )
{
  FRAME_BUFFER_CONSOLE *Console;
  UINTN                 Pos;
  UINT32                PosX;
  UINT32                PosY;
  UINT32                OffX;
  UINT32                OffY;
  UINT16                Value;
  UINT16               *Ptr;

  Console = &mFbConsole;
  if (Console->GfxInfoHob == NULL) {
    return EFI_NOT_READY;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Col > Console->Cols) {
    return EFI_INVALID_PARAMETER;
  }

  if (Row > Console->Rows) {
    return EFI_INVALID_PARAMETER;
  }

  Pos  = 0;
  OffX = ((UINT32)Console->Cols - Col) >> 1;
  OffY = ((UINT32)Console->Rows - Row) >> 1;
  for (PosY = 0; PosY < Row; PosY++) {
    for (PosX = 0; PosX < Col; PosX++) {
      Value = *(UINT16 *)(Buffer + Pos);
      Ptr   = (UINT16 *)(Console->TextDrawBuf + Pos);
      if (*Ptr != Value) {
        *Ptr = Value;
        BltGlyphToFrameBuffer (
          Console->GfxInfoHob, Buffer[Pos],
          mColors[(Value >>  8) & 0x0F],
          mColors[(Value >> 12) & 0x0F],
          Console->OffX + (PosX + OffX) * GLYPH_WIDTH,
          Console->OffY + (PosY + OffY) * GLYPH_HEIGHT);
      }
      Pos += 2;
    }
  }

  return EFI_SUCCESS;
}

