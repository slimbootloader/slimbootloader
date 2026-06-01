/** @file
  Renderer header - screen rendering via DrawFrameBuffer.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Include/UiCommon.h"
#include "FormEngine.h"

typedef enum {
  UiRenderLayoutFrameBuffer = 0,
  UiRenderLayoutSerial
} UI_RENDER_LAYOUT;

/**
  Initialize the renderer and allocate the screen buffer.

  @retval EFI_SUCCESS    Renderer initialized.
  @retval Others         Failure (no framebuffer, etc.)
**/
EFI_STATUS
RendererInit (
  VOID
  );

VOID
RendererSetLayout (
  IN UI_RENDER_LAYOUT  Layout
  );

/**
  Get usable screen columns.

  @return Number of columns.
**/
UINT16
RendererGetCols (
  VOID
  );

/**
  Get usable screen rows.

  @return Number of rows.
**/
UINT16
RendererGetRows (
  VOID
  );

/**
  Clear the entire screen buffer.
**/
VOID
RendererClear (
  VOID
  );

/**
  Draw the static frame (borders, title, key legend).

  @param[in]  Title   Title string to display.
**/
VOID
RenderFrame (
  IN CHAR8  *Title
  );

/**
  Render the page list (left panel).

  @param[in]  ParentPageId  Parent page id for current menu level (0 = root).
  @param[in]  SelectedPos   Currently selected child position in current level.
  @param[in]  ScrollOffset  First visible child position in current level.
**/
VOID
RenderPageList (
  IN UINT16  ParentPageId,
  IN UINT16  SelectedPos,
  IN UINT16  ScrollOffset,
  IN BOOLEAN IsFocused
  );

/**
  Render the fields for the current page (right panel).

  @param[in]  Page          Current page entry.
  @param[in]  Cursor        Currently highlighted field (relative to visible list).
  @param[in]  ScrollOffset  First visible field index.
  @param[in]  CfgBuf        Config data buffer (for reading values).
**/
VOID
RenderFields (
  IN UI_PAGE_ENTRY  *Page,
  IN UINT16          Cursor,
  IN UINT16          ScrollOffset,
  IN UINT8          *CfgBuf,
  IN BOOLEAN         IsFocused
  );

/**
  Render the help text area (bottom panel).

  @param[in]  HelpText   Help string to display.
**/
VOID
RenderHelp (
  IN CHAR8  *HelpText
  );

/**
  Render a scrollable full-help popup.

  @param[in]  HelpText      Full help text.
  @param[in]  StartLine     First wrapped line to display.
  @param[out] TotalLines    Total wrapped line count.
  @param[out] VisibleLines  Number of wrapped lines visible in popup.
**/
VOID
RenderHelpPopup (
  IN  CHAR8   *HelpText,
  IN  UINT16   StartLine,
  OUT UINT16  *TotalLines,
  OUT UINT16  *VisibleLines
  );

/**
  Render a combo-box popup for option selection.

  @param[in]  Options    Option entries.
  @param[in]  Count      Number of options.
  @param[in]  Selected   Currently selected option index.
**/
VOID
RenderComboPopup (
  IN UI_OPTION_ENTRY  *Options,
  IN UINT16            Count,
  IN UINT16            Selected
  );

/**
  Render a numeric edit popup.

  @param[in]  FieldName     Name of the field being edited.
  @param[in]  CurrentValue  Current/editing value.
  @param[in]  IsHex         TRUE for hex display, FALSE for decimal.
**/
VOID
RenderEditPopup (
  IN CHAR8    *FieldName,
  IN UINT64    CurrentValue,
  IN BOOLEAN   IsHex
  );

/**
  Render a one-line find popup.

  @param[in]  Query      Current query text.
  @param[in]  IsActive   TRUE when an existing search is active.
**/
VOID
RenderFindPopup (
  IN CHAR8    *Query,
  IN BOOLEAN   IsActive
  );

/**
  Render a selectable find results popup.

  @param[in]  Query       Query text used for search.
  @param[in]  Results     Array of result lines.
  @param[in]  Count       Number of visible result lines.
  @param[in]  Total       Total number of matches.
  @param[in]  Selected    Selected line index in Results.
  @param[in]  TopIndex    First visible line index.
  @param[in]  Truncated   TRUE when more matches exist than shown.
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
  );

/**
  Render a table edit popup with a scrollable element list.

  @param[in]  Field      Table field descriptor.
  @param[in]  TagData    Backing tag data.
  @param[in]  Selected   Currently selected element index.
  @param[in]  TopIndex   First visible element index.
**/
VOID
RenderTablePopup (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN UINT16           Selected,
  IN UINT16           TopIndex,
  IN BOOLEAN          DumpMode
  );

/**
  Render a confirmation dialog.

  @param[in]  Message      Message to display.
  @param[in]  YesSelected  TRUE if Yes is highlighted.
**/
VOID
RenderConfirm (
  IN CHAR8    *Message,
  IN BOOLEAN   YesSelected
  );

/**
  Flush the screen buffer to the frame buffer via DrawFrameBuffer.
**/
VOID
ScreenFlush (
  VOID
  );

/**
  Return TRUE if serial output is active (used to decide whether a periodic
  idle redraw is needed for late-joining serial clients).

  @return TRUE if serial rendering is enabled.
**/
BOOLEAN
RendererHasSerial (
  VOID
  );

BOOLEAN
RendererHasFrameBuffer (
  VOID
  );

#endif // _RENDERER_H_
