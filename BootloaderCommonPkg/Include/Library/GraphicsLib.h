/** @file
  Basic graphics rendering support

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __GRAPHICSLIB_H__
#define __GRAPHICSLIB_H__

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/GraphicsInfoHob.h>
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
  EFI_PEI_GRAPHICS_INFO_HOB     *GfxInfoHob;
  CHAR8                         *TextDisplayBuf;
  CHAR8                         *TextSwapBuf;
  CHAR8                         *TextDrawBuf;
  UINTN                         OffX;
  UINTN                         OffY;
  UINTN                         Width;
  UINTN                         Height;
  UINTN                         Rows;
  UINTN                         Cols;
  UINTN                         CursorX;
  UINTN                         CursorY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor;
} FRAME_BUFFER_CONSOLE;


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
);


/**
  Display a *.BMP graphics image to the frame buffer or BLT buffer. If a NULL
  GopBlt buffer is passed in, the BMP image will be displayed into BLT memory
  buffer. Otherwise, it will be dispalyed into the actual frame buffer.

  @param  BmpImage      Pointer to BMP file
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
  IN  VOID      *BmpImage,
  IN  VOID      *GopBlt,
  IN  UINTN      GopBltSize,
  IN  EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob
  );

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
  IN EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob,
  IN VOID                      *GopBlt,
  IN UINTN                     Width,
  IN UINTN                     Height,
  IN UINTN                     OffX,
  IN UINTN                     OffY
  );

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
  );

/**
  Initialize the frame buffer console.

  @param[in] GfxInfoHob          Pointer to graphics info HOB
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
  IN EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob,
  IN     UINTN                 Width,
  IN     UINTN                 Height,
  IN     UINTN                 OffX,
  IN     UINTN                 OffY
  );

/**
  Scroll the console area of the screen up.

  @param[in] ScrollAmount Amount (in pixels) to scroll

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
FrameBufferConsoleScroll (
  IN UINTN                ScrollAmount
  );

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
  );

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
  );

#endif
