/** @file
  Renderer - Screen rendering via DrawFrameBuffer.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Renderer.h"
#include "ConfigBridge.h"
#include <Library/BootloaderCommonLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/ConsoleOutLib.h>
#include <Library/SerialPortLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Library/PcdLib.h>

#if defined(_MSC_VER)
#pragma optimize("", off)
#endif

//
// Screen buffer: 2 bytes per character (low = ASCII, high = attribute)
// DrawFrameBuffer format: bit[7:0]=char, bit[11:8]=FG, bit[15:12]=BG
//
STATIC UINT8   *mScreenBuf;
STATIC UINT8   *mFrameBufferScreenBuf;
STATIC UINT8   *mSerialScreenBuf;
STATIC UINT8   *mSerialLastFlushBuf;
STATIC UINT16   mCols;
STATIC UINT16   mRows;
STATIC UINT16   mFrameBufferCols;
STATIC UINT16   mFrameBufferRows;
STATIC UINT16   mSerialCols;
STATIC UINT16   mSerialRows;
STATIC BOOLEAN  mRenderToFrameBuffer;
STATIC BOOLEAN  mRenderToSerial;
STATIC BOOLEAN  mSerialScreenCleared;

//
// Dynamic glyph sizes based on display resolution
// Default: 8x19 (QEMU), Large: 12x24 (NVL and other high-res displays)
//
STATIC UINT32   mGlyphWidth  = 8;
STATIC UINT32   mGlyphHeight = 19;

#define UI_SERIAL_COLS  100
#define UI_SERIAL_ROWS  50

STATIC
VOID
SetActiveLayout (
  IN UI_RENDER_LAYOUT  Layout
  )
{
  if ((Layout == UiRenderLayoutFrameBuffer) && mRenderToFrameBuffer && (mFrameBufferScreenBuf != NULL)) {
    mScreenBuf = mFrameBufferScreenBuf;
    mCols      = mFrameBufferCols;
    mRows      = mFrameBufferRows;
  } else if ((Layout == UiRenderLayoutSerial) && mRenderToSerial && (mSerialScreenBuf != NULL)) {
    mScreenBuf = mSerialScreenBuf;
    mCols      = mSerialCols;
    mRows      = mSerialRows;
  }
}

/**
  Put a character + attribute at (row, col) in the screen buffer.
**/
STATIC
VOID
PutChar (
  IN UINT16  Row,
  IN UINT16  Col,
  IN UINT8   Ch,
  IN UINT8   Attr
  )
{
  UINT32  Offset;

  if (Row >= mRows || Col >= mCols) {
    return;
  }

  Offset = ((UINT32)Row * mCols + Col) * 2;
  mScreenBuf[Offset]     = Ch;
  mScreenBuf[Offset + 1] = Attr;
}

/**
  Draw a string at (row, col) with given attribute. Truncates at screen edge.
**/
STATIC
VOID
PutString (
  IN UINT16       Row,
  IN UINT16       Col,
  IN CONST CHAR8 *Str,
  IN UINT8        Attr,
  IN UINT16       MaxLen
  )
{
  UINT16  Idx;

  for (Idx = 0; Idx < MaxLen && Str[Idx] != '\0'; Idx++) {
    PutChar (Row, Col + Idx, (UINT8)Str[Idx], Attr);
  }
}

/**
  Fill a rectangular region with a character and attribute.
**/
STATIC
VOID
FillRect (
  IN UINT16  Row,
  IN UINT16  Col,
  IN UINT16  Height,
  IN UINT16  Width,
  IN UINT8   Ch,
  IN UINT8   Attr
  )
{
  UINT16  R;
  UINT16  C;

  for (R = Row; R < Row + Height && R < mRows; R++) {
    for (C = Col; C < Col + Width && C < mCols; C++) {
      PutChar (R, C, Ch, Attr);
    }
  }
}

/**
  Draw a horizontal line.
**/
STATIC
VOID
DrawHLine (
  IN UINT16  Row,
  IN UINT16  Col,
  IN UINT16  Width,
  IN UINT8   Ch,
  IN UINT8   Attr
  )
{
  UINT16  C;

  for (C = Col; C < Col + Width && C < mCols; C++) {
    PutChar (Row, C, Ch, Attr);
  }
}

/**
  Draw a vertical line.
**/
STATIC
VOID
DrawVLine (
  IN UINT16  Row,
  IN UINT16  Col,
  IN UINT16  Height,
  IN UINT8   Ch,
  IN UINT8   Attr
  )
{
  UINT16  R;

  for (R = Row; R < Row + Height && R < mRows; R++) {
    PutChar (R, Col, Ch, Attr);
  }
}

STATIC
UINT64
ReadBitField64Local (
  IN UINT8   *Data,
  IN UINT16   BitOffset,
  IN UINT16   BitLength
  )
{
  UINT64  Value;
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if ((Data == NULL) || (BitLength == 0) || (BitLength > 64)) {
    return 0;
  }

  Value = 0;
  for (Idx = 0; Idx < BitLength; Idx++) {
    ByteOff = (BitOffset + Idx) / 8;
    BitOff  = (BitOffset + Idx) % 8;
    if (Data[ByteOff] & (1U << BitOff)) {
      Value |= LShiftU64 (1ULL, Idx);
    }
  }

  return Value;
}

/**
  Get child page by display order under a specific parent.
**/
STATIC
UI_PAGE_ENTRY *
GetChildPageByOrder (
  IN UINT16   ParentPageId,
  IN UINT16   ChildOrder,
  OUT UINT16 *PageIdx
  )
{
  UINT16         Idx;
  UINT16         Count;
  UINT16         CurOrder;
  UI_PAGE_ENTRY *Page;

  Count = FormGetPageCount ();
  CurOrder = 0;

  for (Idx = 0; Idx < Count; Idx++) {
    Page = FormGetPage (Idx);
    if ((Page != NULL) && (Page->ParentPageId == ParentPageId)) {
      if (CurOrder == ChildOrder) {
        if (PageIdx != NULL) {
          *PageIdx = Idx;
        }
        return Page;
      }
      CurOrder++;
    }
  }

  if (PageIdx != NULL) {
    *PageIdx = 0xFFFF;
  }
  return NULL;
}

/**
  Count direct children under a specific parent.
**/
STATIC
UINT16
CountChildPages (
  IN UINT16  ParentPageId
  )
{
  UINT16         Idx;
  UINT16         Count;
  UINT16         PageCount;
  UI_PAGE_ENTRY *Page;

  Count = 0;
  PageCount = FormGetPageCount ();
  for (Idx = 0; Idx < PageCount; Idx++) {
    Page = FormGetPage (Idx);
    if ((Page != NULL) && (Page->ParentPageId == ParentPageId)) {
      Count++;
    }
  }

  return Count;
}

/**
  Initialize the renderer.
**/
EFI_STATUS
RendererInit (
  VOID
  )
{
  EFI_STATUS  Status;
  INT32       ProbeRow;
  INT32       ProbeCol;
  UINT32      Width;
  UINT32      Height;
  UINT32      OffX;
  UINT32      OffY;
  UINT32      ConsoleCols;
  UINT32      ConsoleRows;
  EFI_PEI_GRAPHICS_INFO_HOB  *GfxInfoHob;
  UINT32                      ConsoleOutMask;

  mScreenBuf = NULL;
  mFrameBufferScreenBuf = NULL;
  mSerialScreenBuf = NULL;
  mSerialLastFlushBuf = NULL;
  mFrameBufferCols = UI_MAX_COLS;
  mFrameBufferRows = UI_MAX_ROWS;
  mSerialCols = UI_SERIAL_COLS;
  mSerialRows = UI_SERIAL_ROWS;
  mCols = UI_MAX_COLS;
  mRows = UI_MAX_ROWS;
  mRenderToFrameBuffer = FALSE;
  mRenderToSerial = FALSE;
  mSerialScreenCleared = FALSE;

  ConsoleOutMask = PcdGet32 (PcdConsoleOutDeviceMask);
  mRenderToSerial = ((ConsoleOutMask & ConsoleOutSerialPort) != 0) ? TRUE : FALSE;
  mRenderToFrameBuffer = ((ConsoleOutMask & ConsoleOutFrameBuffer) != 0) ? TRUE : FALSE;

  GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
  if (mRenderToFrameBuffer && (GfxInfoHob != NULL)) {
    Width  = GfxInfoHob->GraphicsMode.HorizontalResolution;
    Height = GfxInfoHob->GraphicsMode.VerticalResolution;
    if ((PcdGet32 (PcdFrameBufferMaxConsoleWidth) > 0) && (Width > PcdGet32 (PcdFrameBufferMaxConsoleWidth))) {
      Width = PcdGet32 (PcdFrameBufferMaxConsoleWidth);
    }
    if ((PcdGet32 (PcdFrameBufferMaxConsoleHeight) > 0) && (Height > PcdGet32 (PcdFrameBufferMaxConsoleHeight))) {
      Height = PcdGet32 (PcdFrameBufferMaxConsoleHeight);
    }

    //
    // Dynamically select font size based on display resolution.
    // For high-res displays (> 1920x1080), use larger font (12x24) for better readability.
    // For standard/lower-res displays, use standard font (8x19).
    //
    if ((Width > 1920) || (Height > 1080)) {
      mGlyphWidth  = 12;
      mGlyphHeight = 24;
      DEBUG ((DEBUG_INFO, "Using large font (12x24) for high-resolution display (%ux%u)\n", Width, Height));
    } else {
      mGlyphWidth  = 8;
      mGlyphHeight = 19;
      DEBUG ((DEBUG_INFO, "Using standard font (8x19) for display (%ux%u)\n", Width, Height));
    }

    ConsoleCols = Width / mGlyphWidth;
    ConsoleRows = Height / mGlyphHeight;
    if (ConsoleCols > 0) {
      mFrameBufferCols = (UINT16)MIN ((UINT32)UI_MAX_COLS, ConsoleCols);
    }
    if (ConsoleRows > 0) {
      mFrameBufferRows = (UINT16)MIN ((UINT32)UI_MAX_ROWS, ConsoleRows);
    }
  }

  if (mRenderToFrameBuffer) {
    mFrameBufferScreenBuf = AllocateZeroPool ((UINT32)mFrameBufferCols * mFrameBufferRows * 2);
    if (mFrameBufferScreenBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    SetActiveLayout (UiRenderLayoutFrameBuffer);
  }

  if (mRenderToSerial) {
    mSerialScreenBuf = AllocateZeroPool ((UINT32)mSerialCols * mSerialRows * 2);
    if (mSerialScreenBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    mSerialLastFlushBuf = AllocateZeroPool ((UINT32)mSerialCols * mSerialRows * 2);
  }

  //
  // DrawFrameBuffer rejects sizes larger than the active framebuffer console.
  // Probe down to find a supported rows/cols geometry (for example 80x25).
  //
  if (mRenderToFrameBuffer) {
    Status = DrawFrameBuffer (mRows, mCols, mScreenBuf);
  } else {
    Status = EFI_SUCCESS;
  }

  if (mRenderToFrameBuffer && (Status == EFI_NOT_READY)) {
    if (GfxInfoHob != NULL) {
      Width  = GfxInfoHob->GraphicsMode.HorizontalResolution;
      Height = GfxInfoHob->GraphicsMode.VerticalResolution;
      if ((PcdGet32 (PcdFrameBufferMaxConsoleWidth) > 0) && (Width > PcdGet32 (PcdFrameBufferMaxConsoleWidth))) {
        Width = PcdGet32 (PcdFrameBufferMaxConsoleWidth);
      }
      if ((PcdGet32 (PcdFrameBufferMaxConsoleHeight) > 0) && (Height > PcdGet32 (PcdFrameBufferMaxConsoleHeight))) {
        Height = PcdGet32 (PcdFrameBufferMaxConsoleHeight);
      }
      OffX = (GfxInfoHob->GraphicsMode.HorizontalResolution - Width) / 2;
      OffY = (GfxInfoHob->GraphicsMode.VerticalResolution - Height) / 2;
      Status = InitFrameBufferConsole (GfxInfoHob, Width, Height, OffX, OffY);
      Status = DrawFrameBuffer (mRows, mCols, mScreenBuf);
    }
  }

  if (mRenderToFrameBuffer && EFI_ERROR (Status)) {
    for (ProbeRow = mRows; ProbeRow >= 1; ProbeRow--) {
      Status = DrawFrameBuffer ((UINT32)ProbeRow, mCols, mScreenBuf);
      if (!EFI_ERROR (Status)) {
        mRows = (UINT16)ProbeRow;
        break;
      }
    }

    if (EFI_ERROR (Status)) {
      for (ProbeCol = mCols; ProbeCol >= 1; ProbeCol--) {
        Status = DrawFrameBuffer (mRows, (UINT32)ProbeCol, mScreenBuf);
        if (!EFI_ERROR (Status)) {
          mCols = (UINT16)ProbeCol;
          break;
        }
      }
    }

    if (EFI_ERROR (Status)) {
      if (mRenderToSerial) {
        // Continue with serial text rendering when framebuffer is unavailable.
        mRenderToFrameBuffer = FALSE;
        mFrameBufferCols = 0;
        mFrameBufferRows = 0;
      } else {
        return Status;
      }
    }
  }

  if (mRenderToSerial) {
    SetActiveLayout (UiRenderLayoutSerial);
  } else if (mRenderToFrameBuffer) {
    SetActiveLayout (UiRenderLayoutFrameBuffer);
  }

  return EFI_SUCCESS;
}

VOID
RendererSetLayout (
  IN UI_RENDER_LAYOUT  Layout
  )
{
  SetActiveLayout (Layout);
}


UINT16
RendererGetCols (
  VOID
  )
{
  return mCols;
}

UINT16
RendererGetRows (
  VOID
  )
{
  return mRows;
}

BOOLEAN
RendererHasSerial (
  VOID
  )
{
  return mRenderToSerial;
}

BOOLEAN
RendererHasFrameBuffer (
  VOID
  )
{
  return mRenderToFrameBuffer;
}

/**
  Clear the entire screen buffer.
**/
VOID
RendererClear (
  VOID
  )
{
  FillRect (0, 0, mRows, mCols, ' ', ATTR_FIELD_NAME);
}

/**
  Draw the static frame.
**/
VOID
RenderFrame (
  IN CHAR8  *Title
  )
{
  UINT16  LastCol;
  UINT16  LastRow;
  UINT16  HelpStartRow;
  UINT16  LegendRow;
  CHAR8  *AlignPtr;
  UINT16  LeftMaxLen;
  UINT16  RightLen;
  UINT16  RightCol;

  LastCol = mCols - 1;
  LastRow = mRows - 1;
  HelpStartRow = mRows - UI_HELP_ROWS - UI_LEGEND_ROWS;
  LegendRow    = mRows - UI_LEGEND_ROWS;

  //
  // Top border
  //
  PutChar (0, 0, BOX_ULCORNER, ATTR_BORDER);
  DrawHLine (0, 1, mCols - 2, BOX_HLINE, ATTR_BORDER);
  PutChar (0, LastCol, BOX_URCORNER, ATTR_BORDER);

  //
  // Title bar (Row 1)
  //
  PutChar (1, 0, BOX_VLINE, ATTR_BORDER);
  FillRect (1, 1, 1, mCols - 2, ' ', ATTR_TITLE);

  //
  // Support "left|right" format so the right segment can be right-aligned
  // regardless of current console width (serial or graphics).
  //
  AlignPtr = AsciiStrStr (Title, "|");
  if (AlignPtr != NULL) {
    LeftMaxLen = (UINT16)(AlignPtr - Title);
    PutString (1, 2, Title, ATTR_TITLE, LeftMaxLen);

    RightLen = (UINT16)AsciiStrLen (AlignPtr + 1);
    if (RightLen < (mCols - 4)) {
      RightCol = (UINT16)(LastCol - 1 - RightLen);
      PutString (1, RightCol, AlignPtr + 1, ATTR_TITLE, RightLen);
    }
  } else {
    PutString (1, 2, Title, ATTR_TITLE, mCols - 4);
  }

  PutChar (1, LastCol, BOX_VLINE, ATTR_BORDER);

  //
  // Separator after title (Row 2)
  //
  PutChar (2, 0, BOX_LTEE, ATTR_BORDER);
  DrawHLine (2, 1, UI_PAGE_PANEL_WIDTH, BOX_HLINE, ATTR_BORDER);
  PutChar (2, UI_PAGE_PANEL_WIDTH + 1, BOX_TTEE, ATTR_BORDER);
  DrawHLine (2, UI_PAGE_PANEL_WIDTH + 2, mCols - UI_PAGE_PANEL_WIDTH - 3, BOX_HLINE, ATTR_BORDER);
  PutChar (2, LastCol, BOX_RTEE, ATTR_BORDER);

  //
  // Side borders for content area (Row 3 to HelpStartRow-1)
  //
  DrawVLine (3, 0, HelpStartRow - 3, BOX_VLINE, ATTR_BORDER);
  DrawVLine (3, UI_PAGE_PANEL_WIDTH + 1, HelpStartRow - 3, BOX_VLINE, ATTR_BORDER);
  DrawVLine (3, LastCol, HelpStartRow - 3, BOX_VLINE, ATTR_BORDER);

  //
  // Separator before help area
  //
  PutChar (HelpStartRow, 0, BOX_LTEE, ATTR_BORDER);
  DrawHLine (HelpStartRow, 1, mCols - 2, BOX_HLINE, ATTR_BORDER);
  PutChar (HelpStartRow, LastCol, BOX_RTEE, ATTR_BORDER);

  //
  // Help area borders
  //
  DrawVLine (HelpStartRow + 1, 0, UI_HELP_ROWS - 1, BOX_VLINE, ATTR_BORDER);
  DrawVLine (HelpStartRow + 1, LastCol, UI_HELP_ROWS - 1, BOX_VLINE, ATTR_BORDER);

  //
  // Separator before legend
  //
  PutChar (LegendRow, 0, BOX_LTEE, ATTR_BORDER);
  DrawHLine (LegendRow, 1, mCols - 2, BOX_HLINE, ATTR_BORDER);
  PutChar (LegendRow, LastCol, BOX_RTEE, ATTR_BORDER);

  //
  // Legend row
  //
  PutChar (LegendRow + 1, 0, BOX_VLINE, ATTR_BORDER);
  FillRect (LegendRow + 1, 1, 1, mCols - 2, ' ', ATTR_LEGEND);
  PutString (LegendRow + 1, 2, "I/K/J/L=Nav  Enter=Edit  /=Find  X/Esc=Exit  Q/F9=Defaults  S/F10=Save  H=Help",
             ATTR_LEGEND, mCols - 4);
  PutChar (LegendRow + 1, LastCol, BOX_VLINE, ATTR_BORDER);

  //
  // Bottom border
  //
  PutChar (LastRow, 0, BOX_LLCORNER, ATTR_BORDER);
  DrawHLine (LastRow, 1, mCols - 2, BOX_HLINE, ATTR_BORDER);
  PutChar (LastRow, LastCol, BOX_LRCORNER, ATTR_BORDER);
}

/**
  Render the page list in the left panel.
**/
VOID
RenderPageList (
  IN UINT16  ParentPageId,
  IN UINT16  SelectedPos,
  IN UINT16  ScrollOffset,
  IN BOOLEAN IsFocused
  )
{
  UINT16         Idx;
  UINT16         ChildPos;
  UINT16         ChildCount;
  UINT16         MaxRows;
  UINT16         HelpStartRow;
  UI_PAGE_ENTRY *Page;
  CHAR8         *Name;
  CHAR8          FallbackName[24];
  UINT8          Attr;
  BOOLEAN        HasChildren;
  UINT16         Row;
  CHAR8          LineBuf[UI_PAGE_PANEL_WIDTH + 1];
  UINT16         MaxNameLen;
  UINT16         DrawRows;

  HelpStartRow = mRows - UI_HELP_ROWS - UI_LEGEND_ROWS;
  MaxRows = HelpStartRow - 3;
  ChildCount = CountChildPages (ParentPageId);
  DrawRows = (ChildCount < MaxRows) ? ChildCount : MaxRows;

  FillRect (3, 1, MaxRows, UI_PAGE_PANEL_WIDTH, ' ', ATTR_PAGE);

  if (ChildCount == 0) {
    PutString (3, 2, "(No sub-pages)", ATTR_PAGE, UI_PAGE_PANEL_WIDTH - 2);
    return;
  }

  for (Idx = 0; Idx < DrawRows; Idx++) {
    ChildPos = ScrollOffset + Idx;
    Page = GetChildPageByOrder (ParentPageId, ChildPos, NULL);
    if (Page == NULL) {
      continue;
    }

    Row    = 3 + Idx;
    MaxNameLen = UI_PAGE_PANEL_WIDTH - 4;
    Name = FormGetString (Page->NameStrOffset);
    if ((Name == NULL) || (Name[0] == '\0') || ((UINT8)Name[0] < ' ') || (Name[0] == 0x7F)) {
      AsciiSPrint (FallbackName, sizeof (FallbackName), "Item %u", (UINT32)(ChildPos + 1));
      Name = FallbackName;
    }

    HasChildren = (CountChildPages (Page->PageId) > 0) ? TRUE : FALSE;
    if (HasChildren) {
      if (ChildPos == SelectedPos) {
        Attr = IsFocused ? ATTR_PAGE_PARENT_SEL : ATTR_PAGE_PARENT_SEL_INACTIVE;
      } else {
        Attr = ATTR_PAGE_PARENT;
      }
    } else {
      if (ChildPos == SelectedPos) {
        Attr = IsFocused ? ATTR_PAGE_LEAF_SEL : ATTR_PAGE_LEAF_SEL_INACTIVE;
      } else {
        Attr = ATTR_PAGE_LEAF;
      }
    }

    //
    // Fill line background
    //
    FillRect (Row, 1, 1, UI_PAGE_PANEL_WIDTH, ' ', Attr);

    //
    // Format: ">+ PageName" for page nodes, ">- PageName" for leaf pages.
    //
    {
      UINT16 Pos;
      for (Pos = 0; Pos < sizeof (LineBuf); Pos++) {
        LineBuf[Pos] = '\0';
      }
    }
    if (ChildPos == SelectedPos) {
      LineBuf[0] = '>';
    } else {
      LineBuf[0] = ' ';
    }

    LineBuf[1] = HasChildren ? '+' : '-';
    LineBuf[2] = ' ';

    {
      UINT16 DispLen;
      DispLen = 0;
      while (Name[DispLen] != '\0' && DispLen < MaxNameLen) {
        LineBuf[3 + DispLen] = Name[DispLen];
        DispLen++;
      }
    }

    PutString (Row, 1, LineBuf, Attr, UI_PAGE_PANEL_WIDTH);
  }

  if (ChildCount > MaxRows) {
    if (ScrollOffset > 0) {
      PutChar (3, UI_PAGE_PANEL_WIDTH - 1, '^', ATTR_PAGE);
    }
    if ((ScrollOffset + MaxRows) < ChildCount) {
      PutChar ((UINT16)(3 + MaxRows - 1), UI_PAGE_PANEL_WIDTH - 1, 'v', ATTR_PAGE);
    }
  }
}

/**
  Render child page names for a container page that has no direct fields.
**/
STATIC
VOID
RenderChildPagesHint (
  IN UI_PAGE_ENTRY  *Page,
  IN UINT16          FieldAreaStart,
  IN UINT16          FieldAreaWidth,
  IN UINT16          MaxVisibleRows
  )
{
  UINT16         Idx;
  UINT16         PageCount;
  UINT16         Row;
  UI_PAGE_ENTRY *Child;
  CHAR8         *Name;
  CHAR8          LineBuf[64];

  PutString (3, FieldAreaStart + 1, "(No fields in this page)", ATTR_FIELD_VAL, FieldAreaWidth - 2);

  PageCount = FormGetPageCount ();
  Row       = 5;

  for (Idx = 0; Idx < PageCount && (Row - 3) < MaxVisibleRows; Idx++) {
    Child = FormGetPage (Idx);
    if ((Child == NULL) || (Child->ParentPageId != Page->PageId)) {
      continue;
    }

    Name = FormGetString (Child->NameStrOffset);
    AsciiSPrint (LineBuf, sizeof (LineBuf), "- %a", Name);
    PutString (Row, FieldAreaStart + 1, LineBuf, ATTR_FIELD_NAME, FieldAreaWidth - 2);
    Row++;
  }
}

/**
  Render the fields for current page.
**/
VOID
RenderFields (
  IN UI_PAGE_ENTRY  *Page,
  IN UINT16          Cursor,
  IN UINT16          ScrollOffset,
  IN UINT8          *CfgBuf,
  IN BOOLEAN         IsFocused
  )
{
  UINT16          FieldAreaStart;
  UINT16          FieldAreaWidth;
  UINT16          MaxVisibleRows;
  UINT16          Idx;
  UINT16          VisIdx;
  UI_FIELD_ENTRY *Field;
  CHAR8          *Name;
  UINT32          Value;
  UINT64          Value64;
  UINT8           Attr;
  UINT16          Row;
  CHAR8           ValBuf[24];
  UI_OPTION_ENTRY *Opts;
  UINT16          OptCount;
  UINT16          OptIdx;
  VOID           *TagData;
  UINT16          NameWidth;
  UINT16          HelpStartRow;
  BOOLEAN         HasVisibleFields;
  UINT16          HexDigits;
  BOOLEAN         IsUnsaved;
  STATIC UINT16   LastMissingPageId;
  STATIC UINT16   LastMissingTagId;
  STATIC UINT16   LastMissingBitOffset;

  FieldAreaStart = UI_PAGE_PANEL_WIDTH + 2;
  FieldAreaWidth = mCols - FieldAreaStart - 1;
  HelpStartRow   = mRows - UI_HELP_ROWS - UI_LEGEND_ROWS;
  MaxVisibleRows = HelpStartRow - 3;
  NameWidth      = FieldAreaWidth - UI_FIELD_VALUE_WIDTH - 2;

  //
  // Clear field area
  //
  FillRect (3, FieldAreaStart, MaxVisibleRows, FieldAreaWidth, ' ', ATTR_FIELD_NAME);

  if (Page == NULL) {
    return;
  }

  VisIdx = 0;
  HasVisibleFields = FALSE;

  for (Idx = 0; Idx < Page->FieldCount; Idx++) {
    Field = FormGetField (Page->FieldStartIdx + Idx);
    if (Field == NULL) {
      continue;
    }

    //
    // Skip reserved fields
    //
    if (Field->FieldType == UI_FIELD_TYPE_RESERVED) {
      continue;
    }

    //
    // Evaluate condition
    //
    if (!FormEvalCondition (Field->ConditionIdx, CfgBuf)) {
      continue;
    }

    //
    // Skip if before scroll window
    //
    if (VisIdx < ScrollOffset) {
      VisIdx++;
      continue;
    }

    //
    // Check if beyond visible area
    //
    if ((VisIdx - ScrollOffset) >= MaxVisibleRows) {
      break;
    }

    Row = 3 + (VisIdx - ScrollOffset);
    Attr = ((VisIdx - ScrollOffset) == Cursor) ? (IsFocused ? ATTR_FIELD_SEL : ATTR_FIELD_SEL_INACTIVE) : ATTR_FIELD_NAME;
    IsUnsaved = CfgIsFieldUnsaved (Field);
    HasVisibleFields = TRUE;

    //
    // Field name
    //
    Name = FormGetString (Field->NameStrOffset);
    PutString (
      Row,
      FieldAreaStart + 1,
      Name,
      (((VisIdx - ScrollOffset) == Cursor) || !IsUnsaved) ? Attr : ATTR_MODIFIED,
      NameWidth
      );

    //
    // Field value
    //
    TagData = CfgGetTagData (Field->TagId);
    if (TagData != NULL) {
      Value = FormGetFieldValue (Field, (UINT8 *)TagData);
      Value64 = 0;

      if (Field->FieldType == UI_FIELD_TYPE_COMBO) {
        //
        // Show option label
        //
        Opts = FormGetOptions (Field, &OptCount);
        ValBuf[0] = '[';
        ValBuf[1] = '\0';
        for (OptIdx = 0; OptIdx < OptCount; OptIdx++) {
          if (Opts[OptIdx].Value == Value) {
            CHAR8 *Label = FormGetString (Opts[OptIdx].LabelStrOffset);
            UINT16 Len = 0;
            while (Label[Len] && Len < (UI_FIELD_VALUE_WIDTH - 3)) {
              ValBuf[1 + Len] = Label[Len];
              Len++;
            }
            ValBuf[1 + Len] = '\0';
            break;
          }
        }
        //
        // If no match found, show raw value
        //
        if (ValBuf[1] == '\0') {
          AsciiSPrint (ValBuf + 1, sizeof(ValBuf) - 1, "0x%x", Value);
        }
        //
        // Pad and close bracket
        //
        {
          UINT16 Len = (UINT16)AsciiStrLen(ValBuf);
          while (Len < UI_FIELD_VALUE_WIDTH - 1) {
            ValBuf[Len++] = ' ';
          }
          ValBuf[Len - 1] = ']';
          ValBuf[Len] = '\0';
        }
      } else if (Field->FieldType == UI_FIELD_TYPE_TABLE) {
        UINT64 ElemValue;
        UINT32 BitIdx;
        UINT32 ByteOff;
        UINT32 BitOff;
        UINT16 ShowDigits;
        UINT16 ElemBits;
        UINT16 ElemCount;

        ElemBits  = Field->OptionStartIdx;
        ElemCount = Field->OptionCount;
        if ((ElemBits == 0) || (ElemCount == 0)) {
          AsciiSPrint (ValBuf, sizeof(ValBuf), "[T %ux%u]", ElemCount, ElemBits);
        } else if (ElemBits <= 64) {
          ElemValue = 0;
          for (BitIdx = 0; BitIdx < ElemBits; BitIdx++) {
            ByteOff = (Field->BitOffset + BitIdx) / 8;
            BitOff  = (Field->BitOffset + BitIdx) % 8;
            if (((UINT8 *)TagData)[ByteOff] & (1U << BitOff)) {
              ElemValue |= LShiftU64 (1ULL, BitIdx);
            }
          }

          ShowDigits = (UINT16)((ElemBits + 3) / 4);
          if (ShowDigits < 2) {
            ShowDigits = 2;
          }
          if (ShowDigits > 8) {
            ShowDigits = 8;
          }

          AsciiSPrint (ValBuf, sizeof(ValBuf), "[0/%u 0x%0*llX]", ElemCount - 1, ShowDigits, ElemValue);
        } else {
          AsciiSPrint (ValBuf, sizeof(ValBuf), "[T %ux%u]", ElemCount, ElemBits);
        }
      } else {
        //
        // Numeric: show hex value
        //
        HexDigits = (UINT16)((Field->BitLength + 3) / 4);
        if (HexDigits < 2) {
          HexDigits = 2;
        }
        if ((Field->BitLength > 32) && (Field->BitLength <= 64)) {
          UINT32 BitIdx;
          UINT32 ByteOff;
          UINT32 BitOff;

          if (HexDigits > 16) {
            HexDigits = 16;
          }

          for (BitIdx = 0; BitIdx < Field->BitLength; BitIdx++) {
            ByteOff = (Field->BitOffset + BitIdx) / 8;
            BitOff  = (Field->BitOffset + BitIdx) % 8;
            if (((UINT8 *)TagData)[ByteOff] & (1U << BitOff)) {
              Value64 |= LShiftU64 (1ULL, BitIdx);
            }
          }

          AsciiSPrint (ValBuf, sizeof(ValBuf), "[0x%0*llX]", HexDigits, Value64);
        } else {
          if (HexDigits > 8) {
            HexDigits = 8;
          }
          AsciiSPrint (ValBuf, sizeof(ValBuf), "[0x%0*X]", HexDigits, Value);
        }
      }

      PutString (
        Row,
        FieldAreaStart + NameWidth + 1,
        ValBuf,
        ((VisIdx - ScrollOffset) == Cursor) ?
          (IsFocused ? ATTR_FIELD_SEL : ATTR_FIELD_SEL_INACTIVE) :
          (IsUnsaved ? ATTR_MODIFIED : ATTR_FIELD_VAL),
        UI_FIELD_VALUE_WIDTH
        );
    } else if (IsFocused && ((VisIdx - ScrollOffset) == Cursor)) {
      if ((LastMissingPageId != Page->PageId) ||
          (LastMissingTagId != Field->TagId) ||
          (LastMissingBitOffset != Field->BitOffset)) {
        DEBUG ((
          DEBUG_ERROR,
          "UiSetup: Missing selected field data page='%a'(id=%u) field='%a' path='%a' tag=0x%03X bit=%u len=%u\n",
          FormGetString (Page->NameStrOffset),
          Page->PageId,
          Name,
          FormGetFieldPath (Field),
          Field->TagId,
          Field->BitOffset,
          Field->BitLength
          ));

        if (Field->TagId == CDATA_FSPS_UPD_TAG) {
          DEBUG ((
            DEBUG_ERROR,
              "UiSetup: FSPS selected field has no backing data. Check LoaderFspInfo/FspsUpdPtr handoff timing.\n"
            ));
        }

        LastMissingPageId   = Page->PageId;
        LastMissingTagId    = Field->TagId;
        LastMissingBitOffset = Field->BitOffset;
      }
    }

    VisIdx++;
  }

  if (!HasVisibleFields) {
    RenderChildPagesHint (Page, FieldAreaStart, FieldAreaWidth, MaxVisibleRows);
  }
}

/**
  Render help text.
**/
VOID
RenderHelp (
  IN CHAR8  *HelpText
  )
{
  UINT16  HelpStartRow;
  UINT16  MaxWidth;
  UINT16  Row;
  UINT16  TextIdx;
  UINT16  LineLen;
  UINT16  WrapAt;
  UINT16  TextRows;

  TextRows     = UI_HELP_ROWS - 1;   // rows available for text (last row is border)
  HelpStartRow = mRows - UI_HELP_ROWS - UI_LEGEND_ROWS + 1;
  MaxWidth     = mCols - 4;

  //
  // Clear help area
  //
  FillRect (HelpStartRow, 1, TextRows, mCols - 2, ' ', ATTR_HELP);

  if (HelpText == NULL || HelpText[0] == '\0') {
    return;
  }

  TextIdx = 0;
  for (Row = HelpStartRow; Row < HelpStartRow + TextRows; Row++) {
    if (HelpText[TextIdx] == '\0') {
      break;
    }

    //
    // Measure how many chars fit on this row
    //
    LineLen = 0;
    while (HelpText[TextIdx + LineLen] != '\0' && LineLen < MaxWidth) {
      LineLen++;
    }

    if (HelpText[TextIdx + LineLen] != '\0') {
      //
      // More text remains — try to break at last space so we don't cut words
      //
      WrapAt = LineLen;
      while (WrapAt > 0 && HelpText[TextIdx + WrapAt - 1] != ' ') {
        WrapAt--;
      }
      if (WrapAt > 0) {
        LineLen = WrapAt;   // break before the space
      }
    }

    if (LineLen > 0) {
      PutString (Row, 2, HelpText + TextIdx, ATTR_HELP, LineLen);
      TextIdx += LineLen;
      // Skip leading spaces on next line
      while (HelpText[TextIdx] == ' ') {
        TextIdx++;
      }
    }
  }
}

/**
  Get one wrapped line from a text blob by wrapped line index.

  @retval TRUE   Target wrapped line exists and LineBuf is filled.
  @retval FALSE  Target wrapped line does not exist.
**/
STATIC
BOOLEAN
GetWrappedLine (
  IN  CHAR8   *Text,
  IN  UINT16   MaxWidth,
  IN  UINT16   TargetLine,
  OUT CHAR8   *LineBuf,
  IN  UINT16   LineBufSize,
  OUT UINT16  *TotalLines
  )
{
  UINT16  CurLine;
  UINT16  LineLen;
  UINT16  WrapAt;
  UINTN   TextIdx;
  UINT16  CopyLen;

  if ((Text == NULL) || (MaxWidth == 0) || (LineBuf == NULL) || (LineBufSize == 0)) {
    if (TotalLines != NULL) {
      *TotalLines = 0;
    }
    return FALSE;
  }

  LineBuf[0] = '\0';
  CurLine = 0;
  TextIdx = 0;

  while (Text[TextIdx] != '\0') {
    while (Text[TextIdx] == ' ') {
      TextIdx++;
    }

    if (Text[TextIdx] == '\0') {
      break;
    }

    LineLen = 0;
    while ((Text[TextIdx + LineLen] != '\0') && (LineLen < MaxWidth)) {
      LineLen++;
    }

    if (Text[TextIdx + LineLen] != '\0') {
      WrapAt = LineLen;
      while ((WrapAt > 0) && (Text[TextIdx + WrapAt - 1] != ' ')) {
        WrapAt--;
      }
      if (WrapAt > 0) {
        LineLen = WrapAt;
      }
    }

    if (CurLine == TargetLine) {
      CopyLen = LineLen;
      if (CopyLen >= LineBufSize) {
        CopyLen = (UINT16)(LineBufSize - 1);
      }
      if (CopyLen > 0) {
        CopyMem (LineBuf, Text + TextIdx, CopyLen);
      }
      LineBuf[CopyLen] = '\0';
    }

    TextIdx += LineLen;
    while (Text[TextIdx] == ' ') {
      TextIdx++;
    }

    CurLine++;
  }

  if (TotalLines != NULL) {
    *TotalLines = CurLine;
  }

  return (TargetLine < CurLine);
}

/**
  Render a scrollable full-help popup.
**/
VOID
RenderHelpPopup (
  IN  CHAR8   *HelpText,
  IN  UINT16   StartLine,
  OUT UINT16  *TotalLines,
  OUT UINT16  *VisibleLines
  )
{
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;
  UINT16  TextRows;
  UINT16  Row;
  UINT16  LineTotal;
  UINT16  Shown;
  CHAR8   LineBuf[192];
  CHAR8   FooterBuf[64];
  UINT16  LastCol;

  PopupWidth = (mCols > 12) ? (mCols - 8) : mCols;
  if (PopupWidth > (UINT16)(mCols - 2)) {
    PopupWidth = (UINT16)(mCols - 2);
  }
  if (PopupWidth < 36) {
    PopupWidth = (mCols >= 36) ? 36 : mCols;
  }

  PopupHeight = (mRows > 10) ? (mRows - 6) : mRows;
  if (PopupHeight > (UINT16)(mRows - 2)) {
    PopupHeight = (UINT16)(mRows - 2);
  }
  if (PopupHeight < 8) {
    PopupHeight = (mRows >= 8) ? 8 : mRows;
  }

  StartRow = (mRows - PopupHeight) / 2;
  StartCol = (mCols - PopupWidth) / 2;
  LastCol  = (UINT16)(StartCol + PopupWidth - 1);
  TextRows = PopupHeight - 4;

  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, LastCol, BOX_S_URCORNER, ATTR_POPUP);

  PutChar ((UINT16)(StartRow + PopupHeight - 1), StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine ((UINT16)(StartRow + PopupHeight - 1), StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar ((UINT16)(StartRow + PopupHeight - 1), LastCol, BOX_S_LRCORNER, ATTR_POPUP);

  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, LastCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  PutString (StartRow + 1, StartCol + 2, "Full Help (I/K/PgUp/PgDn scroll, X/Esc close)", ATTR_POPUP_SEL, PopupWidth - 4);

  LineTotal = 0;
  Shown = 0;
  for (Row = 0; Row < TextRows; Row++) {
    if (GetWrappedLine (HelpText, (UINT16)(PopupWidth - 4), (UINT16)(StartLine + Row), LineBuf, sizeof (LineBuf), &LineTotal)) {
      PutString ((UINT16)(StartRow + 2 + Row), StartCol + 2, LineBuf, ATTR_POPUP, PopupWidth - 4);
      Shown++;
    }
  }

  if (LineTotal > TextRows) {
    if (StartLine > 0) {
      PutChar (StartRow + 2, LastCol - 1, '^', ATTR_POPUP_SEL);
    }
    if ((UINT16)(StartLine + TextRows) < LineTotal) {
      PutChar ((UINT16)(StartRow + PopupHeight - 2), LastCol - 1, 'v', ATTR_POPUP_SEL);
    }
  }

  AsciiSPrint (
    FooterBuf,
    sizeof (FooterBuf),
    "Lines %u-%u/%u",
    (LineTotal == 0) ? 0 : (UINT32)(StartLine + 1),
    (LineTotal == 0) ? 0 : (UINT32)(StartLine + Shown),
    (UINT32)LineTotal
    );
  PutString ((UINT16)(StartRow + PopupHeight - 2), StartCol + 2, FooterBuf, ATTR_POPUP_SEL, PopupWidth - 4);

  if (TotalLines != NULL) {
    *TotalLines = LineTotal;
  }
  if (VisibleLines != NULL) {
    *VisibleLines = TextRows;
  }
}

/**
  Render a combo popup centered on screen.
**/
VOID
RenderComboPopup (
  IN UI_OPTION_ENTRY  *Options,
  IN UINT16            Count,
  IN UINT16            Selected
  )
{
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;
  UINT16  Idx;
  UINT16  Row;
  CHAR8  *Label;
  UINT8   Attr;

  PopupWidth  = 40;
  PopupHeight = Count + 4;  // border + title + options + border

  if (PopupHeight > mRows - 4) {
    PopupHeight = mRows - 4;
  }

  StartRow = (mRows - PopupHeight) / 2;
  StartCol = (mCols - PopupWidth) / 2;

  //
  // Draw popup box
  //
  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  // Top border
  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, StartCol + PopupWidth - 1, BOX_S_URCORNER, ATTR_POPUP);

  // Bottom border
  PutChar (StartRow + PopupHeight - 1, StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine (StartRow + PopupHeight - 1, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol + PopupWidth - 1, BOX_S_LRCORNER, ATTR_POPUP);

  // Side borders
  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol + PopupWidth - 1, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  // Title
  PutString (StartRow + 1, StartCol + 2, "Select Option:", ATTR_POPUP, PopupWidth - 4);

  //
  // Options
  //
  for (Idx = 0; Idx < Count && Idx < PopupHeight - 4; Idx++) {
    Row = StartRow + 2 + Idx;
    Attr = (Idx == Selected) ? ATTR_POPUP_SEL : ATTR_POPUP;
    FillRect (Row, StartCol + 1, 1, PopupWidth - 2, ' ', Attr);
    Label = FormGetString (Options[Idx].LabelStrOffset);
    PutString (Row, StartCol + 3, Label, Attr, PopupWidth - 6);
  }
}

/**
  Render numeric edit popup.
**/
VOID
RenderEditPopup (
  IN CHAR8    *FieldName,
  IN UINT64    CurrentValue,
  IN BOOLEAN   IsHex
  )
{
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;
  CHAR8   ValStr[24];

  PopupWidth  = 40;
  PopupHeight = 6;
  StartRow = (mRows - PopupHeight) / 2;
  StartCol = (mCols - PopupWidth) / 2;

  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  // Border
  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, StartCol + PopupWidth - 1, BOX_S_URCORNER, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine (StartRow + PopupHeight - 1, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol + PopupWidth - 1, BOX_S_LRCORNER, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol + PopupWidth - 1, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  // Field name
  PutString (StartRow + 1, StartCol + 2, FieldName, ATTR_POPUP, PopupWidth - 4);

  // Value
  if (IsHex) {
    AsciiSPrint (ValStr, sizeof(ValStr), "0x%llX", CurrentValue);
  } else {
    AsciiSPrint (ValStr, sizeof(ValStr), "%llu", CurrentValue);
  }
  PutString (StartRow + 3, StartCol + 2, "Value: ", ATTR_POPUP, 7);
  PutString (StartRow + 3, StartCol + 9, ValStr, ATTR_POPUP_SEL, PopupWidth - 12);

  // Instructions
  PutString (StartRow + 4, StartCol + 2, "Enter=OK  Esc=Cancel", ATTR_POPUP, PopupWidth - 4);
}

/**
  Render one-line find popup.
**/
VOID
RenderFindPopup (
  IN CHAR8    *Query,
  IN BOOLEAN   IsActive
  )
{
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;
  UINT16  ContentWidth;
  CHAR8   Title[32];
  CHAR8   QueryBuf[80];

  PopupWidth  = 52;
  PopupHeight = 6;
  if (PopupWidth > (UINT16)(mCols - 4)) {
    PopupWidth = (mCols > 8) ? (UINT16)(mCols - 4) : mCols;
  }

  StartRow = (mRows - PopupHeight) / 2;
  StartCol = (mCols - PopupWidth) / 2;

  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, StartCol + PopupWidth - 1, BOX_S_URCORNER, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine (StartRow + PopupHeight - 1, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol + PopupWidth - 1, BOX_S_LRCORNER, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol + PopupWidth - 1, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  ContentWidth = 0;
  if (PopupWidth > 4) {
    ContentWidth = PopupWidth;
    ContentWidth -= 4;
  }

  AsciiSPrint (Title, sizeof (Title), IsActive ? "Find (update)" : "Find");
  PutString (StartRow + 1, StartCol + 2, Title, ATTR_POPUP, ContentWidth);

  AsciiSPrint (QueryBuf, sizeof (QueryBuf), "/%a_", (Query != NULL) ? Query : "");
  PutString (StartRow + 3, StartCol + 2, QueryBuf, ATTR_POPUP_SEL, ContentWidth);
  PutString (StartRow + 4, StartCol + 2, "Enter=Search  Esc=Cancel  Bksp=Delete", ATTR_POPUP, ContentWidth);
}

/**
  Render selectable find-results popup.
**/
VOID
RenderFindResultsPopup (
  IN CHAR8    *Query,
  IN CHAR8   **Results,
  IN UINT16    Count,
  IN UINT16    Total,
  IN UINT16    Selected,
  IN UINT16    TopIndex,
  IN BOOLEAN   Truncated
  )
{
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;
  UINT16  VisibleRows;
  UINT16  Row;
  UINT16  ResultIdx;
  UINT8   Attr;
  CHAR8   Title[96];
  CHAR8   Footer[96];

  PopupWidth  = (mCols > 72) ? 68 : (UINT16)(mCols - 4);
  PopupHeight = (mRows > 20) ? 16 : (UINT16)(mRows - 4);
  if (PopupHeight < 8) {
    PopupHeight = 8;
  }

  StartRow = (mRows > PopupHeight) ? (mRows - PopupHeight) / 2 : 0;
  StartCol = (mCols - PopupWidth) / 2;

  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, StartCol + PopupWidth - 1, BOX_S_URCORNER, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine (StartRow + PopupHeight - 1, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol + PopupWidth - 1, BOX_S_LRCORNER, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol + PopupWidth - 1, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  if (Total > 0) {
    AsciiSPrint (Title, sizeof (Title), "Find Results: /%a (%u)", (Query != NULL) ? Query : "", Total);
  } else {
    AsciiSPrint (Title, sizeof (Title), "Find Results: /%a", (Query != NULL) ? Query : "");
  }
  PutString (StartRow + 1, StartCol + 2, Title, ATTR_POPUP, PopupWidth - 4);

  VisibleRows = PopupHeight - 5;
  if (Count == 0) {
    PutString (StartRow + 3, StartCol + 2, "No matches found.", ATTR_POPUP_SEL, PopupWidth - 4);
  } else {
    for (Row = 0; Row < VisibleRows; Row++) {
      ResultIdx = (UINT16)(TopIndex + Row);
      if ((Results == NULL) || (ResultIdx >= Count)) {
        continue;
      }

      Attr = (ResultIdx == Selected) ? ATTR_POPUP_SEL : ATTR_POPUP;
      PutString (StartRow + 2 + Row, StartCol + 2, Results[ResultIdx], Attr, PopupWidth - 4);
    }
  }

  if (Count == 0) {
    PutString (StartRow + PopupHeight - 2, StartCol + 2, "Enter/Esc=Close", ATTR_POPUP, PopupWidth - 4);
  } else if (Truncated) {
    AsciiSPrint (Footer, sizeof (Footer), "I/K=Move  Enter=Jump  Esc=Close  Showing %u of %u", Count, Total);
    PutString (StartRow + PopupHeight - 2, StartCol + 2, Footer, ATTR_POPUP, PopupWidth - 4);
  } else {
    PutString (StartRow + PopupHeight - 2, StartCol + 2, "I/K=Move  Enter=Jump  Esc=Close", ATTR_POPUP, PopupWidth - 4);
  }
}

/**
  Render a table edit popup with a scrollable element list.
**/
VOID
RenderTablePopup (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN UINT16           Selected,
  IN UINT16           TopIndex,
  IN BOOLEAN          DumpMode
  )
{
  UINT16  ElemBits;
  UINT16  ElemCount;
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;
  UINT16  VisibleRows;
  UINT16  Idx;
  UINT16  ElemIdx;
  UINT8   Attr;
  UINT64  ElemVal;
  CHAR8  *FieldName;
  CHAR8   Line[96];
  UINT16  Digits;
  UINT16  BytesPerRow;
  UINT16  BaseIdx;
  UINT16  ColIdx;
  UINT16  ByteIdx;
  UINT16  SelColIdx;
  UINT16  SelTextPos;
  UINTN   Pos;

  if ((Field == NULL) || (TagData == NULL)) {
    return;
  }

  ElemBits = Field->OptionStartIdx;
  ElemCount = Field->OptionCount;
  if ((ElemBits == 0) || (ElemCount == 0) || (ElemBits > 64)) {
    return;
  }

  PopupWidth  = (mCols > 72) ? 68 : (UINT16)(mCols - 4);
  PopupHeight = (mRows > 20) ? 16 : (UINT16)(mRows - 4);
  if (PopupHeight < 8) {
    PopupHeight = 8;
  }

  StartRow = (mRows > PopupHeight) ? (mRows - PopupHeight) / 2 : 0;
  StartCol = (mCols - PopupWidth) / 2;
  VisibleRows = (UINT16)(PopupHeight - 5);

  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, StartCol + PopupWidth - 1, BOX_S_URCORNER, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine (StartRow + PopupHeight - 1, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol + PopupWidth - 1, BOX_S_LRCORNER, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol + PopupWidth - 1, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  FieldName = FormGetString (Field->NameStrOffset);
  AsciiSPrint (Line, sizeof (Line), "%a (%u x %u-bit)", FieldName, ElemCount, ElemBits);
  PutString (StartRow + 1, StartCol + 2, Line, ATTR_POPUP, PopupWidth - 4);

  Digits = (UINT16)((ElemBits + 3) / 4);
  if (Digits < 2) {
    Digits = 2;
  }

  if (DumpMode && (ElemBits == 8)) {
    BytesPerRow = 16;
    for (Idx = 0; Idx < VisibleRows; Idx++) {
      BaseIdx = (UINT16)(TopIndex + Idx * BytesPerRow);
      if (BaseIdx >= ElemCount) {
        break;
      }

      FillRect ((UINT16)(StartRow + 2 + Idx), (UINT16)(StartCol + 1), 1, (UINT16)(PopupWidth - 2), ' ', ATTR_POPUP);

      Pos = AsciiSPrint (Line, sizeof (Line), "%03X: ", BaseIdx);
      for (ColIdx = 0; ColIdx < BytesPerRow; ColIdx++) {
        ByteIdx = (UINT16)(BaseIdx + ColIdx);
        if (ByteIdx < ElemCount) {
          ElemVal = ReadBitField64Local ((UINT8 *)TagData, (UINT16)(Field->BitOffset + ByteIdx * ElemBits), ElemBits);
          Pos += AsciiSPrint (&Line[Pos], (UINTN)(sizeof (Line) - Pos), "%02X ", (UINT8)ElemVal);
        } else {
          Pos += AsciiSPrint (&Line[Pos], (UINTN)(sizeof (Line) - Pos), "   ");
        }
      }
      Line[Pos] = '\0';

      PutString ((UINT16)(StartRow + 2 + Idx), (UINT16)(StartCol + 2), Line, ATTR_POPUP, (UINT16)(PopupWidth - 4));

      if ((Selected >= BaseIdx) && (Selected < (UINT16)(BaseIdx + BytesPerRow))) {
        SelColIdx = (UINT16)(Selected - BaseIdx);
        SelTextPos = (UINT16)(5 + SelColIdx * 3);
        if ((UINT16)(BaseIdx + SelColIdx) < ElemCount) {
          PutString (
            (UINT16)(StartRow + 2 + Idx),
            (UINT16)(StartCol + 2 + SelTextPos),
            &Line[SelTextPos],
            ATTR_POPUP_SEL,
            2
            );
        }
      }
    }

    PutString (
      (UINT16)(StartRow + PopupHeight - 2),
      (UINT16)(StartCol + 2),
      "Hex=Edit Arrows=Move Enter=Popup V=List Esc=Back",
      ATTR_POPUP,
      (UINT16)(PopupWidth - 4)
      );
  } else {
    for (Idx = 0; Idx < VisibleRows; Idx++) {
      ElemIdx = (UINT16)(TopIndex + Idx);
      if (ElemIdx >= ElemCount) {
        break;
      }

      Attr = (ElemIdx == Selected) ? ATTR_POPUP_SEL : ATTR_POPUP;
      FillRect ((UINT16)(StartRow + 2 + Idx), (UINT16)(StartCol + 1), 1, (UINT16)(PopupWidth - 2), ' ', Attr);

      ElemVal = ReadBitField64Local ((UINT8 *)TagData, (UINT16)(Field->BitOffset + ElemIdx * ElemBits), ElemBits);
      AsciiSPrint (Line, sizeof (Line), "[%03X]  0x%0*llX", ElemIdx, Digits, ElemVal);
      PutString ((UINT16)(StartRow + 2 + Idx), (UINT16)(StartCol + 2), Line, Attr, (UINT16)(PopupWidth - 4));
    }

    PutString (
      (UINT16)(StartRow + PopupHeight - 2),
      (UINT16)(StartCol + 2),
      (ElemBits == 8) ?
      "Up/Down=Select Enter=Edit V=Dump Esc=Back" :
      "Up/Down=Select Enter=Edit Esc=Back",
      ATTR_POPUP,
      (UINT16)(PopupWidth - 4)
      );
  }
}

/**
  Render a confirmation dialog.
**/
VOID
RenderConfirm (
  IN CHAR8    *Message,
  IN BOOLEAN   YesSelected
  )
{
  UINT16  PopupWidth;
  UINT16  PopupHeight;
  UINT16  StartRow;
  UINT16  StartCol;

  PopupWidth  = 44;
  PopupHeight = 6;
  StartRow = (mRows - PopupHeight) / 2;
  StartCol = (mCols - PopupWidth) / 2;

  FillRect (StartRow, StartCol, PopupHeight, PopupWidth, ' ', ATTR_POPUP);

  // Border
  PutChar (StartRow, StartCol, BOX_S_ULCORNER, ATTR_POPUP);
  DrawHLine (StartRow, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow, StartCol + PopupWidth - 1, BOX_S_URCORNER, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol, BOX_S_LLCORNER, ATTR_POPUP);
  DrawHLine (StartRow + PopupHeight - 1, StartCol + 1, PopupWidth - 2, BOX_S_HLINE, ATTR_POPUP);
  PutChar (StartRow + PopupHeight - 1, StartCol + PopupWidth - 1, BOX_S_LRCORNER, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);
  DrawVLine (StartRow + 1, StartCol + PopupWidth - 1, PopupHeight - 2, BOX_S_VLINE, ATTR_POPUP);

  // Message
  PutString (StartRow + 2, StartCol + 2, Message, ATTR_POPUP, PopupWidth - 4);

  // Yes/No buttons
  PutString (StartRow + 4, StartCol + 10, "[ Yes ]", YesSelected ? ATTR_POPUP_SEL : ATTR_POPUP, 7);
  PutString (StartRow + 4, StartCol + 24, "[ No  ]", YesSelected ? ATTR_POPUP : ATTR_POPUP_SEL, 7);
}

/**
  Flush screen buffer to framebuffer via DrawFrameBuffer.
**/
VOID
ScreenFlush (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT8   Attr;
  UINT8   CurStyle;
  UINT8   NewStyle;
  UINT16  Row;
  UINT16  Col;
  UINT16  SegStart;
  UINT16  SerialRows;
  UINT16  SerialCols;
  UINT16  HelpTextStart;
  UINT16  HelpTextEnd;
  UINT16  FirstCol;
  UINT16  LastCol;
  UINT8   Ch;
  UINT32  Off;
  UINT32  RowOff;
  BOOLEAN ForceFull;
  BOOLEAN RowChanged;
  CHAR8   Line[UI_SERIAL_COLS + 2];
  CHAR8   CursorPos[20];

  //
  // Flush framebuffer output first when enabled.
  // Keep serial output active as well when both outputs are selected.
  //
  if (mRenderToFrameBuffer && (mScreenBuf == mFrameBufferScreenBuf)) {
    Status = DrawFrameBuffer (mRows, mCols, mScreenBuf);
    if (EFI_ERROR (Status)) {
      return;
    }
  }

  //
  // Serial rendering path: send fixed viewport to serial terminal.
  //
  if (!mRenderToSerial || (mScreenBuf != mSerialScreenBuf)) {
    return;
  }

  SerialCols = (mCols > UI_SERIAL_COLS) ? UI_SERIAL_COLS : mCols;
  SerialRows = (mRows > UI_SERIAL_ROWS) ? UI_SERIAL_ROWS : mRows;
  HelpTextStart = mRows - UI_HELP_ROWS - UI_LEGEND_ROWS + 1;
  HelpTextEnd   = HelpTextStart + UI_HELP_ROWS - 2;

  // Hide cursor during full-screen ANSI render to prevent the visible cursor
  // sweep from top to bottom in the terminal (e.g. PuTTY) on every refresh.
  SerialPortWrite ((UINT8 *)"\x1b[?25l", 6);

  ForceFull = FALSE;
  if (!mSerialScreenCleared) {
    SerialPortWrite ((UINT8 *)"\x1b[2J\x1b[H", 7);
    mSerialScreenCleared = TRUE;
    ForceFull = TRUE;
  } else {
    SerialPortWrite ((UINT8 *)"\x1b[H", 3);
  }

  if (mSerialLastFlushBuf == NULL) {
    ForceFull = TRUE;
  }

  for (Row = 0; Row < SerialRows; Row++) {
    RowOff = (UINT32)Row * mCols * 2;
    FirstCol = 0;
    LastCol  = (SerialCols == 0) ? 0 : (UINT16)(SerialCols - 1);
    if (!ForceFull && (mSerialLastFlushBuf != NULL)) {
      RowChanged = FALSE;
      FirstCol = SerialCols;
      LastCol  = 0;
      for (Col = 0; Col < SerialCols; Col++) {
        Off = RowOff + (UINT32)Col * 2;
        if ((mSerialLastFlushBuf[Off] != mScreenBuf[Off]) ||
            (mSerialLastFlushBuf[Off + 1] != mScreenBuf[Off + 1])) {
          if (!RowChanged) {
            FirstCol = Col;
          }
          LastCol = Col;
          RowChanged = TRUE;
        }
      }

      if (!RowChanged) {
        continue;
      }
    }

    AsciiSPrint (CursorPos, sizeof (CursorPos), "\x1b[%u;%uH", (UINT32)(Row + 1), (UINT32)(FirstCol + 1));
    SerialPortWrite ((UINT8 *)CursorPos, AsciiStrLen (CursorPos));

    CurStyle = 0;
    SegStart = FirstCol;

    for (Col = FirstCol; Col <= LastCol; Col++) {
      Off = ((UINT32)Row * mCols + Col) * 2;
      Ch = mScreenBuf[Off];
      Attr = mScreenBuf[Off + 1];
      if ((Ch == BOX_HLINE) || (Ch == BOX_S_HLINE)) {
        Ch = '-';
      } else if ((Ch == BOX_VLINE) || (Ch == BOX_S_VLINE)) {
        Ch = '|';
      } else if ((Ch < 0x20) || (Ch > 0x7E)) {
        Ch = ' ';
      }

      if ((Attr == ATTR_BORDER) || (Attr == ATTR_LEGEND)) {
        NewStyle = 9;
      } else if ((Attr == ATTR_PAGE_SEL) || (Attr == ATTR_PAGE_PARENT_SEL) || (Attr == ATTR_PAGE_LEAF_SEL)) {
        NewStyle = 1;
      } else if (Attr == ATTR_PAGE_PARENT_SEL_INACTIVE) {
        NewStyle = 4;
      } else if (Attr == ATTR_PAGE_LEAF_SEL_INACTIVE) {
        NewStyle = 10;
      } else if (Attr == ATTR_PAGE_PARENT) {
        NewStyle = 11;
      } else if (Attr == ATTR_PAGE_LEAF) {
        NewStyle = 12;
      } else if (Attr == ATTR_PAGE) {
        NewStyle = 6;
      } else if (Attr == ATTR_FIELD_SEL) {
        NewStyle = 2;
      } else if (Attr == ATTR_FIELD_SEL_INACTIVE) {
        NewStyle = 5;
      } else if (Attr == ATTR_POPUP_SEL) {
        NewStyle = 3;
      } else if ((Row >= HelpTextStart) && (Row <= HelpTextEnd)) {
        NewStyle = 8;
      } else if (Col <= UI_PAGE_PANEL_WIDTH + 1) {
        NewStyle = 6;
      } else {
        NewStyle = 7;
      }

      if (Col == FirstCol) {
        CurStyle = NewStyle;
        if (CurStyle == 1) {
          SerialPortWrite ((UINT8 *)"\x1b[97;44m", 8);
        } else if (CurStyle == 2) {
          SerialPortWrite ((UINT8 *)"\x1b[30;46m", 8);
        } else if (CurStyle == 3) {
          SerialPortWrite ((UINT8 *)"\x1b[30;45m", 8);
        } else if (CurStyle == 4) {
          SerialPortWrite ((UINT8 *)"\x1b[96;100m", 9);
        } else if (CurStyle == 5) {
          SerialPortWrite ((UINT8 *)"\x1b[37;100m", 9);
        } else if (CurStyle == 6) {
          SerialPortWrite ((UINT8 *)"\x1b[97;100m", 9);
        } else if (CurStyle == 7) {
          SerialPortWrite ((UINT8 *)"\x1b[96;100m", 9);
        } else if (CurStyle == 8) {
          SerialPortWrite ((UINT8 *)"\x1b[93;100m", 9);
        } else if (CurStyle == 9) {
          SerialPortWrite ((UINT8 *)"\x1b[96;40m", 8);
        } else if (CurStyle == 10) {
          SerialPortWrite ((UINT8 *)"\x1b[96;100m", 9);
        } else if (CurStyle == 11) {
          SerialPortWrite ((UINT8 *)"\x1b[93;100m", 9);
        } else if (CurStyle == 12) {
          SerialPortWrite ((UINT8 *)"\x1b[97;100m", 9);
        }
      } else if (NewStyle != CurStyle) {
        SerialPortWrite ((UINT8 *)&Line[SegStart], Col - SegStart);
        if (CurStyle != 0) {
          SerialPortWrite ((UINT8 *)"\x1b[0m", 4);
        }
        if (NewStyle == 1) {
          SerialPortWrite ((UINT8 *)"\x1b[97;44m", 8);
        } else if (NewStyle == 2) {
          SerialPortWrite ((UINT8 *)"\x1b[30;46m", 8);
        } else if (NewStyle == 3) {
          SerialPortWrite ((UINT8 *)"\x1b[30;45m", 8);
        } else if (NewStyle == 4) {
          SerialPortWrite ((UINT8 *)"\x1b[96;100m", 9);
        } else if (NewStyle == 5) {
          SerialPortWrite ((UINT8 *)"\x1b[37;100m", 9);
        } else if (NewStyle == 6) {
          SerialPortWrite ((UINT8 *)"\x1b[97;100m", 9);
        } else if (NewStyle == 7) {
          SerialPortWrite ((UINT8 *)"\x1b[96;100m", 9);
        } else if (NewStyle == 8) {
          SerialPortWrite ((UINT8 *)"\x1b[93;100m", 9);
        } else if (NewStyle == 9) {
          SerialPortWrite ((UINT8 *)"\x1b[96;40m", 8);
        } else if (NewStyle == 10) {
          SerialPortWrite ((UINT8 *)"\x1b[96;100m", 9);
        } else if (NewStyle == 11) {
          SerialPortWrite ((UINT8 *)"\x1b[93;100m", 9);
        } else if (NewStyle == 12) {
          SerialPortWrite ((UINT8 *)"\x1b[97;100m", 9);
        }
        SegStart = Col;
        CurStyle = NewStyle;
      }

      Line[Col] = (CHAR8)Ch;
    }

    if (SerialCols > 0) {
      SerialPortWrite ((UINT8 *)&Line[SegStart], (UINTN)(LastCol - SegStart + 1));
    }

    if (CurStyle != 0) {
      SerialPortWrite ((UINT8 *)"\x1b[0m", 4);
    }

    if (mSerialLastFlushBuf != NULL) {
      CopyMem (&mSerialLastFlushBuf[RowOff], &mScreenBuf[RowOff], (UINTN)SerialCols * 2);
    }
  }

  // Restore cursor after render is complete.
  SerialPortWrite ((UINT8 *)"\x1b[?25h", 6);
}

