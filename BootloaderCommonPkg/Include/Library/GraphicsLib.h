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

#ifndef __GRAPHICSLIB_H__
#define __GRAPHICSLIB_H__

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/FrameBufferInfoGuid.h>
#include <Protocol/GraphicsOutput.h>

//
// Define the maximum message length that this library supports
//
#define MAX_MESSAGE_LENGTH  0x100

enum EFI_COLORS {
  BLACK,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  LIGHTGRAY,
  DARKGRAY,
  LIGHTBLUE,
  LIGHTGREEN,
  LIGHTCYAN,
  LIGHTRED,
  LIGHTMAGENTA,
  YELLOW,
  WHITE,
  EFI_COLORS_MAX
};

extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL Colors[EFI_COLORS_MAX];

#define GLYPH_WIDTH  8
#define GLYPH_HEIGHT 19

typedef struct {
  CHAR16  UnicodeWeight;
  UINT8   Attributes;
  UINT8   GlyphCol1[GLYPH_HEIGHT];
} EFI_NARROW_GLYPH;

extern EFI_NARROW_GLYPH gUsStdNarrowGlyphData[];
extern UINT32 mNarrowFontSize;

typedef struct {
  FRAME_BUFFER_INFO *FrameBuffer;
  UINTN             OffX, OffY;
  UINTN             Width, Height;
  UINTN             Rows, Cols;
  UINTN             CursorX, CursorY;
} FRAME_BUFFER_CONSOLE;

/**
  Convert a *.BMP graphics image to a GOP blt buffer. If a NULL Blt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  GopBlt        Buffer containing GOP version of BmpImage.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  PixelHeight   Height of GopBlt/BmpImage in pixels
  @param  PixelWidth    Width of GopBlt/BmpImage in pixels

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
ConvertBmpToGopBlt (
  IN     VOID      *BmpImage,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
  OUT UINTN     *PixelHeight,
  OUT UINTN     *PixelWidth
  );

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
  );

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
  );

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
  );

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
  );

/**
 Write a string to the frame buffer console.

 @param[in] Console Console instance
 @param[in] Str                 ASCII string to write
 @param[in] Length              Length of the string
 @param[in] ForegroundColor     Fore ground color parameter
 @param[in] BackgroundColor     Back ground color parameter
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
  );

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
  );

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
  );

#endif
