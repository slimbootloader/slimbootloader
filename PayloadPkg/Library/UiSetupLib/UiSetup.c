/** @file
  UI Setup - Main entry point and state machine.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "FormEngine.h"
#include "Renderer.h"
#include "InputHandler.h"
#include "ConfigBridge.h"
#include <Library/PcdLib.h>
#include <Library/PayloadLib.h>


//
// Include the auto-generated UI descriptor binary
//
#if defined(__has_include)
#if __has_include("UiDescData.h")
#include "UiDescData.h"
#else
STATIC CONST UINT8 mUiDescBin[] = { 0 };
#endif
#else
#include "UiDescData.h"
#endif

#define UI_CLEAR_SCREEN_SEQ  ((UINT8 *)"\x1b[2J")
#define UI_MENU_MAX_DEPTH    12
#define UI_SEARCH_TEXT_MAX   64
#define UI_FIND_MAX_RESULTS  128
#define UI_FIND_RESULT_TEXT_MAX  88

STATIC
UI_FIELD_ENTRY *
GetVisibleField (
  IN UI_PAGE_ENTRY  *Page,
  IN UINT16          VisibleIdx,
  IN UINT8          *CfgBuf
  );

STATIC
UINT16
GetUiVisibleRows (
  VOID
  )
{
  UINT16  Rows;

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
  } else if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
  }

  Rows = RendererGetRows ();
  if (Rows <= (UI_TITLE_ROWS + UI_HELP_ROWS + UI_LEGEND_ROWS + 3)) {
    return 1;
  }

  return (UINT16)(Rows - UI_TITLE_ROWS - UI_HELP_ROWS - UI_LEGEND_ROWS - 3);
}

STATIC
VOID
RenderMainView (
  IN UINT16          CurParentPageId,
  IN UINT16          SelPagePos,
  IN UINT16          PageScrollOffset,
  IN UI_PAGE_ENTRY  *CurPage,
  IN UINT16          SelField,
  IN UINT16          ScrollOffset,
  IN UINT8          *CfgBuf,
  IN UI_STATE        State,
  IN BOOLEAN         FlushScreen
  )
{
  UI_FIELD_ENTRY *CurField;
  CHAR8          *HelpText;
  CHAR8           FullTitle[120];

  RendererClear ();

  //
  // Use '|' as a delimiter: left side is title, right side is right-aligned in RenderFrame().
  //
  AsciiSPrint (FullTitle, sizeof (FullTitle), "Slim Bootloader Setup|Platform ID: [%d]", GetPlatformId ());
  RenderFrame (FullTitle);
  RenderPageList (CurParentPageId, SelPagePos, PageScrollOffset, (State == UI_STATE_MAIN_MENU));
  RenderFields (CurPage, SelField, ScrollOffset, CfgBuf, (State == UI_STATE_PAGE));

  CurField = GetVisibleField (CurPage, ScrollOffset + SelField, CfgBuf);
  if (CurField != NULL) {
    HelpText = FormGetString (CurField->HelpStrOffset);
  } else {
    HelpText = "";
  }

  RenderHelp (HelpText);
  if (FlushScreen) {
    ScreenFlush ();
  }
}

STATIC
VOID
RenderMainViewAll (
  IN UINT16          CurParentPageId,
  IN UINT16          SelPagePos,
  IN UINT16          PageScrollOffset,
  IN UI_PAGE_ENTRY  *CurPage,
  IN UINT16          SelField,
  IN UINT16          ScrollOffset,
  IN UINT8          *CfgBuf,
  IN UI_STATE        State,
  IN BOOLEAN         FlushScreen
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderMainView (CurParentPageId, SelPagePos, PageScrollOffset, CurPage, SelField, ScrollOffset, CfgBuf, State, FlushScreen);
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderMainView (CurParentPageId, SelPagePos, PageScrollOffset, CurPage, SelField, ScrollOffset, CfgBuf, State, FlushScreen);
  }
}

STATIC
VOID
RenderComboPopupAll (
  IN UI_OPTION_ENTRY  *Options,
  IN UINT16            Count,
  IN UINT16            Selected
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderComboPopup (Options, Count, Selected);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderComboPopup (Options, Count, Selected);
    ScreenFlush ();
  }
}

STATIC
VOID
RenderEditPopupAll (
  IN CHAR8    *FieldName,
  IN UINT64    CurrentValue,
  IN BOOLEAN   IsHex
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderEditPopup (FieldName, CurrentValue, IsHex);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderEditPopup (FieldName, CurrentValue, IsHex);
    ScreenFlush ();
  }
}

STATIC
VOID
RenderFindPopupAll (
  IN CHAR8    *Query,
  IN BOOLEAN   IsActive
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderFindPopup (Query, IsActive);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderFindPopup (Query, IsActive);
    ScreenFlush ();
  }
}

STATIC
VOID
RenderFindResultsPopupAll (
  IN CHAR8    *Query,
  IN CHAR8   **Results,
  IN UINT16    Count,
  IN UINT16    Total,
  IN UINT16    Selected,
  IN UINT16    TopIndex,
  IN BOOLEAN   Truncated
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderFindResultsPopup (Query, Results, Count, Total, Selected, TopIndex, Truncated);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderFindResultsPopup (Query, Results, Count, Total, Selected, TopIndex, Truncated);
    ScreenFlush ();
  }
}

STATIC
VOID
RenderTablePopupAll (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN UINT16           Selected,
  IN UINT16           TopIndex,
  IN BOOLEAN          DumpMode
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderTablePopup (Field, TagData, Selected, TopIndex, DumpMode);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderTablePopup (Field, TagData, Selected, TopIndex, DumpMode);
    ScreenFlush ();
  }
}

STATIC
UINT64
ReadFieldValue64 (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  )
{
  UINT64  Value;
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if ((Field == NULL) || (TagData == NULL)) {
    return 0;
  }

  if (Field->BitLength <= 32) {
    return FormGetFieldValue (Field, TagData);
  }

  if (Field->BitLength > 64) {
    return 0;
  }

  Value = 0;
  for (Idx = 0; Idx < Field->BitLength; Idx++) {
    ByteOff = (Field->BitOffset + Idx) / 8;
    BitOff  = (Field->BitOffset + Idx) % 8;
    if (TagData[ByteOff] & (1U << BitOff)) {
      Value |= LShiftU64 (1ULL, Idx);
    }
  }

  return Value;
}

STATIC
VOID
WriteFieldValue64 (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN UINT64           Value
  )
{
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if ((Field == NULL) || (TagData == NULL)) {
    return;
  }

  if (Field->BitLength <= 32) {
    FormSetFieldValue (Field, TagData, (UINT32)Value);
    return;
  }

  if (Field->BitLength > 64) {
    return;
  }

  for (Idx = 0; Idx < Field->BitLength; Idx++) {
    ByteOff = (Field->BitOffset + Idx) / 8;
    BitOff  = (Field->BitOffset + Idx) % 8;
    if (Value & LShiftU64 (1ULL, Idx)) {
      TagData[ByteOff] |= (UINT8)(1U << BitOff);
    } else {
      TagData[ByteOff] &= (UINT8)~(1U << BitOff);
    }
  }
}

STATIC
VOID
RecordFieldChange64 (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT64           Value
  )
{
  UI_FIELD_ENTRY  PartField;
  UINT16          RemainingBits;
  UINT16          ChunkBits;
  UINT16          OffsetBits;
  UINT8           Shift;
  UINT64          Mask;
  UINT32          ChunkValue;

  if (Field == NULL) {
    return;
  }

  if (Field->BitLength <= 32) {
    CfgRecordFieldChange (Field, (UINT32)Value);
    return;
  }

  PartField = *Field;
  RemainingBits = Field->BitLength;
  OffsetBits = 0;
  Shift = 0;
  while (RemainingBits > 0) {
    ChunkBits = (RemainingBits > 32) ? 32 : RemainingBits;
    PartField.BitOffset = (UINT16)(Field->BitOffset + OffsetBits);
    PartField.BitLength = ChunkBits;

    if (ChunkBits == 32) {
      Mask = 0xFFFFFFFFULL;
    } else {
      Mask = LShiftU64 (1ULL, ChunkBits) - 1ULL;
    }

    ChunkValue = (UINT32)RShiftU64 (Value, Shift);
    ChunkValue &= (UINT32)Mask;
    CfgRecordFieldChange (&PartField, ChunkValue);

    OffsetBits = (UINT16)(OffsetBits + ChunkBits);
    RemainingBits = (UINT16)(RemainingBits - ChunkBits);
    Shift = (UINT8)(Shift + ChunkBits);
  }
}

STATIC
VOID
RenderConfirmAll (
  IN CHAR8    *Message,
  IN BOOLEAN   YesSelected
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderConfirm (Message, YesSelected);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderConfirm (Message, YesSelected);
    ScreenFlush ();
  }
}

STATIC
VOID
RenderHelpPopupAll (
  IN  CHAR8   *HelpText,
  IN  UINT16   StartLine,
  OUT UINT16  *TotalLines,
  OUT UINT16  *VisibleLines
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RenderHelpPopup (HelpText, StartLine, TotalLines, VisibleLines);
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RenderHelpPopup (HelpText, StartLine, TotalLines, VisibleLines);
    ScreenFlush ();
  }
}

STATIC
VOID
ClearAllOutputs (
  VOID
  )
{
  if (RendererHasFrameBuffer ()) {
    RendererSetLayout (UiRenderLayoutFrameBuffer);
    RendererClear ();
    ScreenFlush ();
  }

  if (RendererHasSerial ()) {
    RendererSetLayout (UiRenderLayoutSerial);
    RendererClear ();
    ScreenFlush ();
  }
}

/**
  Count direct child pages under a parent page id.
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
  Get child page index by order under a parent page id.
**/
STATIC
UINT16
GetChildPageIndexByOrder (
  IN UINT16  ParentPageId,
  IN UINT16  ChildOrder
  )
{
  UINT16         Idx;
  UINT16         CurOrder;
  UINT16         PageCount;
  UI_PAGE_ENTRY *Page;

  CurOrder = 0;
  PageCount = FormGetPageCount ();
  for (Idx = 0; Idx < PageCount; Idx++) {
    Page = FormGetPage (Idx);
    if ((Page != NULL) && (Page->ParentPageId == ParentPageId)) {
      if (CurOrder == ChildOrder) {
        return Idx;
      }
      CurOrder++;
    }
  }

  return 0xFFFF;
}

STATIC
BOOLEAN
GetChildPageOrderByIndex (
  IN  UINT16  ParentPageId,
  IN  UINT16  PageIdx,
  OUT UINT16 *ChildOrder
  )
{
  UINT16  CurOrder;
  UINT16  CurIdx;
  UINT16  Count;

  if (ChildOrder == NULL) {
    return FALSE;
  }

  Count = CountChildPages (ParentPageId);
  for (CurOrder = 0; CurOrder < Count; CurOrder++) {
    CurIdx = GetChildPageIndexByOrder (ParentPageId, CurOrder);
    if (CurIdx == PageIdx) {
      *ChildOrder = CurOrder;
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check if page has child pages.
**/
STATIC
BOOLEAN
PageHasChildren (
  IN UINT16  PageIdx
  )
{
  UI_PAGE_ENTRY *Page;

  Page = FormGetPage (PageIdx);
  if (Page == NULL) {
    return FALSE;
  }

  return (CountChildPages (Page->PageId) > 0);
}

/**
  Count visible (non-reserved, condition-passing) fields for a page.
**/
STATIC
UINT16
CountVisibleFields (
  IN UI_PAGE_ENTRY  *Page,
  IN UINT8          *CfgBuf
  )
{
  UINT16          Count;
  UINT16          Idx;
  UI_FIELD_ENTRY *Field;

  if (Page == NULL) {
    return 0;
  }

  Count = 0;
  for (Idx = 0; Idx < Page->FieldCount; Idx++) {
    Field = FormGetField (Page->FieldStartIdx + Idx);
    if (Field == NULL || Field->FieldType == UI_FIELD_TYPE_RESERVED) {
      continue;
    }
    if (!FormEvalCondition (Field->ConditionIdx, CfgBuf)) {
      continue;
    }
    Count++;
  }
  return Count;
}

/**
  Get the Nth visible field for a page.
**/
STATIC
UI_FIELD_ENTRY *
GetVisibleField (
  IN UI_PAGE_ENTRY  *Page,
  IN UINT16          VisIdx,
  IN UINT8          *CfgBuf
  )
{
  UINT16          Count;
  UINT16          Idx;
  UI_FIELD_ENTRY *Field;

  if (Page == NULL) {
    return NULL;
  }

  Count = 0;
  for (Idx = 0; Idx < Page->FieldCount; Idx++) {
    Field = FormGetField (Page->FieldStartIdx + Idx);
    if (Field == NULL || Field->FieldType == UI_FIELD_TYPE_RESERVED) {
      continue;
    }
    if (!FormEvalCondition (Field->ConditionIdx, CfgBuf)) {
      continue;
    }
    if (Count == VisIdx) {
      return Field;
    }
    Count++;
  }
  return NULL;
}

STATIC
BOOLEAN
StrContainsIgnoreCase (
  IN CONST CHAR8  *Hay,
  IN CONST CHAR8  *Needle
  )
{
  UINTN  HayIdx;
  UINTN  NeedleLen;
  UINTN  Cur;
  CHAR8  HayCh;
  CHAR8  NeedCh;

  if ((Hay == NULL) || (Needle == NULL)) {
    return FALSE;
  }

  NeedleLen = AsciiStrLen (Needle);
  if (NeedleLen == 0) {
    return TRUE;
  }

  for (HayIdx = 0; Hay[HayIdx] != '\0'; HayIdx++) {
    for (Cur = 0; Cur < NeedleLen; Cur++) {
      HayCh = Hay[HayIdx + Cur];
      NeedCh = Needle[Cur];
      if (HayCh == '\0') {
        return FALSE;
      }

      if (HayCh >= 'A' && HayCh <= 'Z') {
        HayCh = (CHAR8)(HayCh + ('a' - 'A'));
      }

      if (NeedCh >= 'A' && NeedCh <= 'Z') {
        NeedCh = (CHAR8)(NeedCh + ('a' - 'A'));
      }

      if (HayCh != NeedCh) {
        break;
      }
    }

    if (Cur == NeedleLen) {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
BOOLEAN
FieldMatchesSearch (
  IN UI_PAGE_ENTRY   *Page,
  IN UI_FIELD_ENTRY  *Field,
  IN CONST CHAR8     *SearchText
  )
{
  CHAR8  *FieldName;
  CHAR8  *PageName;

  if ((Field == NULL) || (SearchText == NULL) || (SearchText[0] == '\0')) {
    return FALSE;
  }

  FieldName = FormGetString (Field->NameStrOffset);
  if (StrContainsIgnoreCase (FieldName, SearchText)) {
    return TRUE;
  }

  if (Page != NULL) {
    PageName = FormGetString (Page->NameStrOffset);
    if (StrContainsIgnoreCase (PageName, SearchText)) {
      return TRUE;
    }
  }

  return FALSE;
}

STATIC
VOID
BuildAllFieldSearchResults (
  IN  UINT8          *CfgBuf,
  IN  CONST CHAR8    *SearchText,
  OUT CHAR8           ResultText[UI_FIND_MAX_RESULTS][UI_FIND_RESULT_TEXT_MAX],
  OUT CHAR8          *ResultLines[UI_FIND_MAX_RESULTS],
  OUT UINT16          ResultParentPageId[UI_FIND_MAX_RESULTS],
  OUT UINT16          ResultPagePos[UI_FIND_MAX_RESULTS],
  OUT UINT16          ResultVisIdx[UI_FIND_MAX_RESULTS],
  OUT UINT16         *ResultCount,
  OUT UINT16         *ResultTotal
  )
{
  UINT16          PageIdx;
  UINT16          PageCount;
  UINT16          PagePos;
  UINT16          Idx;
  UINT16          VisIdx;
  UINT16          Stored;
  UINT16          Total;
  UI_PAGE_ENTRY  *Page;
  UI_FIELD_ENTRY *Field;
  CHAR8          *PageName;
  CHAR8          *FieldName;

  if (ResultCount != NULL) {
    *ResultCount = 0;
  }

  if (ResultTotal != NULL) {
    *ResultTotal = 0;
  }

  if ((SearchText == NULL) || (SearchText[0] == '\0')) {
    return;
  }

  PageCount = FormGetPageCount ();
  Stored = 0;
  Total = 0;
  for (PageIdx = 0; PageIdx < PageCount; PageIdx++) {
    Page = FormGetPage (PageIdx);
    if (Page == NULL) {
      continue;
    }

    if (!GetChildPageOrderByIndex (Page->ParentPageId, PageIdx, &PagePos)) {
      continue;
    }

    PageName = FormGetString (Page->NameStrOffset);
    VisIdx = 0;
    for (Idx = 0; Idx < Page->FieldCount; Idx++) {
      Field = FormGetField (Page->FieldStartIdx + Idx);
      if ((Field == NULL) || (Field->FieldType == UI_FIELD_TYPE_RESERVED)) {
        continue;
      }

      if (!FormEvalCondition (Field->ConditionIdx, CfgBuf)) {
        continue;
      }

      if (FieldMatchesSearch (Page, Field, SearchText)) {
        if (Stored < UI_FIND_MAX_RESULTS) {
          FieldName = FormGetString (Field->NameStrOffset);
          AsciiSPrint (
            ResultText[Stored],
            UI_FIND_RESULT_TEXT_MAX,
            "%a > %a",
            (PageName != NULL) ? PageName : "",
            (FieldName != NULL) ? FieldName : ""
            );
          ResultLines[Stored] = ResultText[Stored];
          ResultParentPageId[Stored] = Page->ParentPageId;
          ResultPagePos[Stored] = PagePos;
          ResultVisIdx[Stored] = VisIdx;
          Stored++;
        }

        Total++;
      }

      VisIdx++;
    }
  }

  if (ResultCount != NULL) {
    *ResultCount = Stored;
  }

  if (ResultTotal != NULL) {
    *ResultTotal = Total;
  }
}

/**
  Handle combo popup editing.

  @return TRUE if value was changed.
**/
STATIC
BOOLEAN
HandleComboEdit (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  )
{
  UI_OPTION_ENTRY *Options;
  UINT16           Count;
  UINT16           Selected;
  UINT16           Idx;
  UINT32           CurValue;
  UINT16           Key;

  Options = FormGetOptions (Field, &Count);
  if (Options == NULL || Count == 0) {
    return FALSE;
  }

  CurValue = FormGetFieldValue (Field, TagData);

  //
  // Find current selection
  //
  Selected = 0;
  for (Idx = 0; Idx < Count; Idx++) {
    if (Options[Idx].Value == CurValue) {
      Selected = Idx;
      break;
    }
  }

  //
  // Popup loop
  //
  while (TRUE) {
    RenderComboPopupAll (Options, Count, Selected);

    Key = InputGetKey ();
    switch (Key) {
      case UI_KEY_UP:
        if (Selected > 0) {
          Selected--;
        }
        break;
      case UI_KEY_DOWN:
        if (Selected < Count - 1) {
          Selected++;
        }
        break;
      case UI_KEY_ENTER:
        FormSetFieldValue (Field, TagData, Options[Selected].Value);
        CfgRecordFieldChange (Field, Options[Selected].Value);
        CfgSetDirty ();
        return TRUE;
      case UI_KEY_ESC:
        return FALSE;
      default:
        break;
    }
  }
}

/**
  Handle numeric value editing.

  @return TRUE if value was changed.
**/
STATIC
BOOLEAN
HandleNumericEdit (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  )
{
  UINT64    Value;
  UINT64    MaxVal;
  UINT64    Digit;
  UINT16    Key;
  CHAR8    *FieldName;
  BOOLEAN   TypedNewValue;

  if (Field->BitLength > 64) {
    return FALSE;
  }

  Value = ReadFieldValue64 (Field, TagData);
  MaxVal = (Field->BitLength >= 64) ? MAX_UINT64 : (LShiftU64 (1ULL, Field->BitLength) - 1ULL);
  FieldName = FormGetString (Field->NameStrOffset);
  TypedNewValue = FALSE;

  while (TRUE) {
    RenderEditPopupAll (FieldName, Value, TRUE);

    Key = InputGetKey ();
    switch (Key) {
      case UI_KEY_ENTER:
        WriteFieldValue64 (Field, TagData, Value);
        RecordFieldChange64 (Field, Value);
        CfgSetDirty ();
        return TRUE;
      case UI_KEY_ESC:
        return FALSE;
      case UI_KEY_BKSP:
        if (TypedNewValue) {
          Value = RShiftU64 (Value, 4);
        }
        break;
      default:
        //
        // Allow hex digit input
        //
        if (Key >= '0' && Key <= '9') {
          Digit = (UINT32)(Key - '0');
          Value = TypedNewValue ? (LShiftU64 (Value, 4) | Digit) : Digit;
          Value &= MaxVal;
          TypedNewValue = TRUE;
        } else if (Key >= 'a' && Key <= 'f') {
          Digit = (UINT32)(Key - 'a' + 10);
          Value = TypedNewValue ? (LShiftU64 (Value, 4) | Digit) : Digit;
          Value &= MaxVal;
          TypedNewValue = TRUE;
        } else if (Key >= 'A' && Key <= 'F') {
          Digit = (UINT32)(Key - 'A' + 10);
          Value = TypedNewValue ? (LShiftU64 (Value, 4) | Digit) : Digit;
          Value &= MaxVal;
          TypedNewValue = TRUE;
        }
        break;
    }
  }
}

/**
  Handle table value editing using element metadata stored in descriptor.

  @return TRUE if value was changed.
**/
STATIC
BOOLEAN
HandleTableEdit (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  )
{
  UI_FIELD_ENTRY  ElemField;
  UINT16          ElemBits;
  UINT16          ElemCount;
  UINT16          ElemIdx;
  UINT16          TopIdx;
  UINT16          VisibleRows;
  UINT16          Key;
  UINT64          Value;
  UINT64          MaxVal;
  UINT64          Digit;
  BOOLEAN         Changed;
  BOOLEAN         TypedNewValue;
  BOOLEAN         DumpMode;
  CHAR8          *BaseName;
  CHAR8           FieldName[96];
  UINT16          BytesPerRow;
  UINT16          NewTop;
  BOOLEAN         DumpHighNibblePending;
  UINT8           DumpHighNibble;
  UINT8           ByteVal8;

  if ((Field == NULL) || (TagData == NULL)) {
    return FALSE;
  }

  ElemBits  = Field->OptionStartIdx;
  ElemCount = Field->OptionCount;
  if ((ElemBits == 0) || (ElemCount == 0) || (ElemBits > 64)) {
    return FALSE;
  }

  ElemIdx = 0;
  TopIdx = 0;
  VisibleRows = (GetUiVisibleRows () > 4) ? (UINT16)(GetUiVisibleRows () - 1) : 4;
  if (VisibleRows > 10) {
    VisibleRows = 10;
  }
  Changed = FALSE;
  DumpMode = FALSE;
  BaseName = FormGetString (Field->NameStrOffset);
  MaxVal = (ElemBits >= 64) ? MAX_UINT64 : (LShiftU64 (1ULL, ElemBits) - 1ULL);
  BytesPerRow = 16;
  DumpHighNibblePending = FALSE;
  DumpHighNibble = 0;

  ElemField = *Field;
  ElemField.BitLength = ElemBits;
  ElemField.BitOffset = (UINT16)(Field->BitOffset + (ElemIdx * ElemBits));

  while (TRUE) {
    RenderTablePopupAll (Field, TagData, ElemIdx, TopIdx, DumpMode);

    Key = InputGetKey ();
    switch (Key) {
      case UI_KEY_UP:
        DumpHighNibblePending = FALSE;
        if (DumpMode && (ElemBits == 8)) {
          if (ElemIdx >= BytesPerRow) {
            ElemIdx = (UINT16)(ElemIdx - BytesPerRow);
          } else {
            ElemIdx = 0;
          }
          NewTop = (UINT16)((ElemIdx / BytesPerRow) * BytesPerRow);
          if (ElemIdx < TopIdx) {
            TopIdx = NewTop;
          }
        } else {
          if (ElemIdx > 0) {
            ElemIdx--;
            if (ElemIdx < TopIdx) {
              TopIdx = ElemIdx;
            }
          }
        }
        break;

      case UI_KEY_DOWN:
        DumpHighNibblePending = FALSE;
        if (DumpMode && (ElemBits == 8)) {
          if ((UINT16)(ElemIdx + BytesPerRow) < ElemCount) {
            ElemIdx = (UINT16)(ElemIdx + BytesPerRow);
          }
          if (ElemIdx >= (UINT16)(TopIdx + BytesPerRow * VisibleRows)) {
            TopIdx = (UINT16)(((ElemIdx / BytesPerRow) - VisibleRows + 1) * BytesPerRow);
          }
        } else {
          if ((UINT16)(ElemIdx + 1) < ElemCount) {
            ElemIdx++;
            if (ElemIdx >= (UINT16)(TopIdx + VisibleRows)) {
              TopIdx = (UINT16)(ElemIdx - VisibleRows + 1);
            }
          }
        }
        break;

      case UI_KEY_LEFT:
        DumpHighNibblePending = FALSE;
        if (DumpMode && (ElemBits == 8) && (ElemIdx > 0)) {
          ElemIdx--;
          NewTop = (UINT16)((ElemIdx / BytesPerRow) * BytesPerRow);
          if (ElemIdx < TopIdx) {
            TopIdx = NewTop;
          }
        }
        break;

      case UI_KEY_RIGHT:
        DumpHighNibblePending = FALSE;
        if (DumpMode && (ElemBits == 8) && ((UINT16)(ElemIdx + 1) < ElemCount)) {
          ElemIdx++;
          if (ElemIdx >= (UINT16)(TopIdx + BytesPerRow * VisibleRows)) {
            TopIdx = (UINT16)(((ElemIdx / BytesPerRow) - VisibleRows + 1) * BytesPerRow);
          }
        }
        break;

      case 'v':
      case 'V':
        if (ElemBits == 8) {
          DumpHighNibblePending = FALSE;
          DumpMode = (BOOLEAN)!DumpMode;
          if (DumpMode) {
            TopIdx = (UINT16)((ElemIdx / BytesPerRow) * BytesPerRow);
          } else {
            TopIdx = ElemIdx;
          }
        }
        break;

      case UI_KEY_ENTER:
        DumpHighNibblePending = FALSE;
        ElemField.BitOffset = (UINT16)(Field->BitOffset + (ElemIdx * ElemBits));
        Value = ReadFieldValue64 (&ElemField, TagData);
        TypedNewValue = FALSE;
        while (TRUE) {
          AsciiSPrint (FieldName, sizeof (FieldName), "%a[%03X]", BaseName, ElemIdx);
          RenderEditPopupAll (FieldName, Value, TRUE);
          Key = InputGetKey ();
          switch (Key) {
            case UI_KEY_BKSP:
              if (TypedNewValue) {
                Value = RShiftU64 (Value, 4);
              }
              break;
            case UI_KEY_ENTER:
              WriteFieldValue64 (&ElemField, TagData, Value);
              RecordFieldChange64 (&ElemField, Value);
              CfgSetDirty ();
              Changed = TRUE;
              goto DoneElementEdit;
            case UI_KEY_ESC:
              goto DoneElementEdit;
            default:
              if (Key >= '0' && Key <= '9') {
                Digit = (UINT64)(Key - '0');
                Value = TypedNewValue ? (LShiftU64 (Value, 4) | Digit) : Digit;
                Value &= MaxVal;
                TypedNewValue = TRUE;
              } else if (Key >= 'a' && Key <= 'f') {
                Digit = (UINT64)(Key - 'a' + 10);
                Value = TypedNewValue ? (LShiftU64 (Value, 4) | Digit) : Digit;
                Value &= MaxVal;
                TypedNewValue = TRUE;
              } else if (Key >= 'A' && Key <= 'F') {
                Digit = (UINT64)(Key - 'A' + 10);
                Value = TypedNewValue ? (LShiftU64 (Value, 4) | Digit) : Digit;
                Value &= MaxVal;
                TypedNewValue = TRUE;
              }
              break;
          }
        }
DoneElementEdit:
        break;

      case UI_KEY_ESC:
        return Changed;

      case UI_KEY_BKSP:
        if (DumpMode && (ElemBits == 8)) {
          if (DumpHighNibblePending) {
            DumpHighNibblePending = FALSE;
          } else if (ElemIdx > 0) {
            ElemIdx--;
            NewTop = (UINT16)((ElemIdx / BytesPerRow) * BytesPerRow);
            if (ElemIdx < TopIdx) {
              TopIdx = NewTop;
            }
          }
        }
        break;

      default:
        if (DumpMode && (ElemBits == 8)) {
          if (Key >= '0' && Key <= '9') {
            Digit = (UINT64)(Key - '0');
          } else if (Key >= 'a' && Key <= 'f') {
            Digit = (UINT64)(Key - 'a' + 10);
          } else if (Key >= 'A' && Key <= 'F') {
            Digit = (UINT64)(Key - 'A' + 10);
          } else {
            break;
          }

          ElemField.BitOffset = (UINT16)(Field->BitOffset + (ElemIdx * ElemBits));
          ByteVal8 = (UINT8)ReadFieldValue64 (&ElemField, TagData);
          if (!DumpHighNibblePending) {
            DumpHighNibble = (UINT8)Digit;
            ByteVal8 = (UINT8)(((UINT8)Digit << 4) | (ByteVal8 & 0x0F));
            DumpHighNibblePending = TRUE;
          } else {
            ByteVal8 = (UINT8)((DumpHighNibble << 4) | (UINT8)Digit);
            DumpHighNibblePending = FALSE;
          }

          WriteFieldValue64 (&ElemField, TagData, ByteVal8);
          RecordFieldChange64 (&ElemField, ByteVal8);
          CfgSetDirty ();
          Changed = TRUE;

          if (!DumpHighNibblePending && ((UINT16)(ElemIdx + 1) < ElemCount)) {
            ElemIdx++;
            if (ElemIdx >= (UINT16)(TopIdx + BytesPerRow * VisibleRows)) {
              TopIdx = (UINT16)(((ElemIdx / BytesPerRow) - VisibleRows + 1) * BytesPerRow);
            }
          }
          break;
        }
        break;
    }
  }
}

/**
  Handle save confirmation.

  @return TRUE if user confirmed save.
**/
STATIC
BOOLEAN
HandleSaveConfirm (
  VOID
  )
{
  BOOLEAN  YesSelected;
  UINT16   Key;

  YesSelected = TRUE;

  while (TRUE) {
    RenderConfirmAll ("Save configuration changes?", YesSelected);

    Key = InputGetKey ();
    switch (Key) {
      case UI_KEY_LEFT:
      case UI_KEY_RIGHT:
      case UI_KEY_TAB:
        YesSelected = !YesSelected;
        break;
      case UI_KEY_ENTER:
        return YesSelected;
      case UI_KEY_ESC:
        return FALSE;
      default:
        break;
    }
  }
}

/**
  Save current setup changes with immediate user-visible feedback.

  @retval TRUE   Save succeeded.
  @retval FALSE  Save failed.
**/
STATIC
BOOLEAN
SaveChangesWithFeedback (
  VOID
  )
{
  EFI_STATUS  Status;

  RenderConfirmAll ("Saving configuration... Please wait.", TRUE);

  Status = CfgSave ();
  if (EFI_ERROR (Status)) {
    RenderConfirmAll ("Save failed. Press Enter to continue.", TRUE);
    InputGetKey ();
    return FALSE;
  }

  return TRUE;
}

/**
  Handle load-defaults confirmation.

  @return TRUE if user confirmed.
**/
STATIC
BOOLEAN
HandleDefaultsConfirm (
  VOID
  )
{
  BOOLEAN  YesSelected;
  UINT16   Key;

  YesSelected = TRUE;

  while (TRUE) {
    RenderConfirmAll ("Load defaults? All current values will be reset.", YesSelected);

    Key = InputGetKey ();
    switch (Key) {
      case UI_KEY_LEFT:
      case UI_KEY_RIGHT:
      case UI_KEY_TAB:
        YesSelected = !YesSelected;
        break;
      case UI_KEY_ENTER:
        return YesSelected;
      case UI_KEY_ESC:
        return FALSE;
      default:
        break;
    }
  }
}

/**
  Display full help text in a scrollable popup.
**/
STATIC
VOID
ShowFullHelp (
  IN CHAR8  *HelpText
  )
{
  UINT16  Key;
  UINT16  StartLine;
  UINT16  TotalLines;
  UINT16  VisibleLines;

  if ((HelpText == NULL) || (HelpText[0] == '\0')) {
    return;
  }

  StartLine = 0;
  TotalLines = 0;
  VisibleLines = 1;

  while (TRUE) {
    RenderHelpPopupAll (HelpText, StartLine, &TotalLines, &VisibleLines);

    Key = InputGetKey ();
    switch (Key) {
      case UI_KEY_UP:
        if (StartLine > 0) {
          StartLine--;
        }
        break;
      case UI_KEY_DOWN:
        if ((StartLine + VisibleLines) < TotalLines) {
          StartLine++;
        }
        break;
      case UI_KEY_PGUP:
        if (StartLine > VisibleLines) {
          StartLine = (UINT16)(StartLine - VisibleLines);
        } else {
          StartLine = 0;
        }
        break;
      case UI_KEY_PGDN:
        if ((StartLine + VisibleLines) < TotalLines) {
          StartLine = (UINT16)(StartLine + VisibleLines);
          if ((StartLine + VisibleLines) > TotalLines) {
            StartLine = (UINT16)(TotalLines - VisibleLines);
          }
        }
        break;
      case UI_KEY_ESC:
      case UI_KEY_ENTER:
      case 'x':
      case 'X':
      case 'h':
      case 'H':
        return;
      default:
        break;
    }
  }
}

STATIC
BOOLEAN
HandleSearchResultModeKey (
  IN     UINT16    Key,
  IN     UINT16    MaxVisRows,
  IN OUT UI_STATE *State,
  IN OUT UINT16   *CurParentPageId,
  IN OUT UINT16   *SelPagePos,
  IN OUT UINT16   *PageScrollOffset,
  IN OUT UINT16   *ScrollOffset,
  IN OUT UINT16   *SelField,
  IN OUT BOOLEAN  *SearchResultMode,
  IN     BOOLEAN   SearchResultForField,
  IN     UINT16    SearchResultCount,
  IN OUT UINT16   *SearchResultSel,
  IN OUT UINT16   *SearchResultTop,
  IN     UINT16    SearchResultParentPageId[UI_FIND_MAX_RESULTS],
  IN     UINT16    SearchResultPagePos[UI_FIND_MAX_RESULTS],
  IN     UINT16    SearchResultFieldIdx[UI_FIND_MAX_RESULTS]
  )
{
  UINT16  SearchResultVisRows;
  UINT16  FoundIdx;

  if (RendererGetRows () > 20) {
    SearchResultVisRows = 11;
  } else if (RendererGetRows () > 12) {
    SearchResultVisRows = (UINT16)(RendererGetRows () - 9);
  } else {
    SearchResultVisRows = 1;
  }

  switch (Key) {
    case UI_KEY_ESC:
      *SearchResultMode = FALSE;
      break;
    case UI_KEY_UP:
      if (*SearchResultSel > 0) {
        (*SearchResultSel)--;
        if (*SearchResultSel < *SearchResultTop) {
          *SearchResultTop = *SearchResultSel;
        }
      }
      break;
    case UI_KEY_DOWN:
      if ((*SearchResultSel + 1) < SearchResultCount) {
        (*SearchResultSel)++;
        if (*SearchResultSel >= (UINT16)(*SearchResultTop + SearchResultVisRows)) {
          *SearchResultTop = (UINT16)(*SearchResultSel - SearchResultVisRows + 1);
        }
      }
      break;
    case UI_KEY_ENTER:
      *SearchResultMode = FALSE;
      if (SearchResultCount == 0) {
        break;
      }

      if (SearchResultForField) {
        *CurParentPageId = SearchResultParentPageId[*SearchResultSel];
        *SelPagePos = SearchResultPagePos[*SearchResultSel];
        *PageScrollOffset = (UINT16)(*SelPagePos / MaxVisRows * MaxVisRows);
        *State = UI_STATE_PAGE;
        FoundIdx = SearchResultFieldIdx[*SearchResultSel];
        *ScrollOffset = (UINT16)(FoundIdx / MaxVisRows * MaxVisRows);
        *SelField = (UINT16)(FoundIdx - *ScrollOffset);
      } else {
        *SelPagePos = SearchResultPagePos[*SearchResultSel];
        if (*SelPagePos < *PageScrollOffset) {
          *PageScrollOffset = *SelPagePos;
        } else if (*SelPagePos >= (UINT16)(*PageScrollOffset + MaxVisRows)) {
          *PageScrollOffset = (UINT16)(*SelPagePos - MaxVisRows + 1);
        }
      }
      break;
    default:
      break;
  }

  return TRUE;
}

STATIC
BOOLEAN
HandleSearchEditModeKey (
  IN     UINT16    Key,
  IN     UINT8    *CfgBuf,
  IN OUT BOOLEAN  *SearchEditMode,
  IN OUT BOOLEAN  *SearchActive,
  IN OUT BOOLEAN  *SearchResultMode,
  IN OUT CHAR8     SearchText[UI_SEARCH_TEXT_MAX],
  IN OUT UINTN    *SearchLen,
  IN OUT BOOLEAN  *SearchResultForField,
  IN OUT BOOLEAN  *SearchResultsTruncated,
  IN OUT UINT16   *SearchResultCount,
  IN OUT UINT16   *SearchResultTotal,
  IN OUT UINT16   *SearchResultSel,
  IN OUT UINT16   *SearchResultTop,
  IN OUT CHAR8     SearchResultText[UI_FIND_MAX_RESULTS][UI_FIND_RESULT_TEXT_MAX],
  IN OUT CHAR8    *SearchResultLines[UI_FIND_MAX_RESULTS],
  IN OUT UINT16    SearchResultParentPageId[UI_FIND_MAX_RESULTS],
  IN OUT UINT16    SearchResultPagePos[UI_FIND_MAX_RESULTS],
  IN OUT UINT16    SearchResultFieldIdx[UI_FIND_MAX_RESULTS]
  )
{
  //
  // Reverse-map navigation shortcut keys back to their original ASCII
  // characters so they can be typed inside the find query.
  // InputGetKey() maps i/k/j/l -> UP/DOWN/LEFT/RIGHT, s -> F10, q -> F9.
  // x/X -> ESC stays as-is (it cancels the popup, same as the real ESC).
  //
  switch (Key) {
    case UI_KEY_UP:    Key = 'i'; break;
    case UI_KEY_DOWN:  Key = 'k'; break;
    case UI_KEY_LEFT:  Key = 'j'; break;
    case UI_KEY_RIGHT: Key = 'l'; break;
    case UI_KEY_F9:    Key = 'q'; break;
    case UI_KEY_F10:   Key = 's'; break;
    default:           break;
  }

  if (Key == UI_KEY_ESC) {
    *SearchEditMode = FALSE;
    return TRUE;
  }

  if (Key == UI_KEY_ENTER) {
    *SearchEditMode = FALSE;
    if (*SearchLen > 0) {
      *SearchActive = TRUE;
      BuildAllFieldSearchResults (
        CfgBuf,
        SearchText,
        SearchResultText,
        SearchResultLines,
        SearchResultParentPageId,
        SearchResultPagePos,
        SearchResultFieldIdx,
        SearchResultCount,
        SearchResultTotal
        );
      *SearchResultForField = TRUE;
      *SearchResultsTruncated = (BOOLEAN)(*SearchResultTotal > *SearchResultCount);
      *SearchResultSel = 0;
      *SearchResultTop = 0;
      *SearchResultMode = TRUE;
    } else {
      *SearchActive = FALSE;
      *SearchResultMode = FALSE;
      *SearchResultCount = 0;
      *SearchResultTotal = 0;
    }

    return TRUE;
  }

  if (Key == UI_KEY_BKSP) {
    if (*SearchLen > 0) {
      (*SearchLen)--;
      SearchText[*SearchLen] = '\0';
    }

    return TRUE;
  }

  if ((Key >= 0x20) && (Key <= 0x7E) && (*SearchLen < (UI_SEARCH_TEXT_MAX - 1))) {
    SearchText[(*SearchLen)++] = (CHAR8)Key;
    SearchText[*SearchLen] = '\0';
  }

  return TRUE;
}

/**
  Main UiSetup entry point. Runs the interactive state machine.

  @retval EFI_SUCCESS   User exited setup normally.
  @retval Others        Setup init failed.
**/
EFI_STATUS
EFIAPI
RunUiSetup (
  VOID
  )
{
  EFI_STATUS      Status;
  UI_STATE        State;
  UINT16          SelField;
  UINT16          ScrollOffset;
  UINT16          PageCount;
  UINT16          VisFieldCount;
  UINT16          Key;
  UI_PAGE_ENTRY  *CurPage;
  UI_FIELD_ENTRY *CurField;
  UINT16          CurPageIdx;
  UINT16          CurParentPageId;
  UINT16          CurChildCount;
  UINT16          SelPagePos;
  UINT16          PageScrollOffset;
  UINT16          MenuDepth;
  UINT16          ParentStack[UI_MENU_MAX_DEPTH];
  UINT16          SelPosStack[UI_MENU_MAX_DEPTH];
  UINT16          ScrollStack[UI_MENU_MAX_DEPTH];
  UINT8          *CfgBuf;
  CHAR8          *HelpText;
  VOID           *TagData;
  UINT16          MaxVisRows;
  BOOLEAN         FirstLoop;
  CHAR8           SearchText[UI_SEARCH_TEXT_MAX];
  UINTN           SearchLen;
  BOOLEAN         SearchActive;
  BOOLEAN         SearchEditMode;
  BOOLEAN         SearchResultMode;
  BOOLEAN         SearchResultForField;
  BOOLEAN         SearchResultsTruncated;
  CHAR8           SearchResultText[UI_FIND_MAX_RESULTS][UI_FIND_RESULT_TEXT_MAX];
  CHAR8          *SearchResultLines[UI_FIND_MAX_RESULTS];
  UINT16          SearchResultParentPageId[UI_FIND_MAX_RESULTS];
  UINT16          SearchResultFieldIdx[UI_FIND_MAX_RESULTS];
  UINT16          SearchResultPagePos[UI_FIND_MAX_RESULTS];
  UINT16          SearchResultCount;
  UINT16          SearchResultTotal;
  UINT16          SearchResultSel;
  UINT16          SearchResultTop;
  UINT16          Idx;

  //
  // Initialize subsystems
  //
  if (!FeaturePcdGet (PcdUiSetupEnabled)) {
    return EFI_UNSUPPORTED;
  }

  Status = FormInit (mUiDescBin, sizeof (mUiDescBin));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UiSetup: FormInit failed - %r\n", Status));
    return Status;
  }

  Status = RendererInit ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UiSetup: RendererInit failed - %r\n", Status));
    return Status;
  }

  // Clear the full framebuffer so pre-setup prompt text does not bleed into UI.
  FrameBufferWrite (UI_CLEAR_SCREEN_SEQ, 4);

  Status = InputInit ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UiSetup: InputInit failed - %r\n", Status));
    return Status;
  }

  Status = CfgLoad ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_WARN, "UiSetup: CfgLoad returned %r\n", Status));
  }

  //
  // Setup state machine initial values
  //
  State        = UI_STATE_MAIN_MENU;
  SelField     = 0;
  ScrollOffset = 0;
  CurParentPageId = 0;
  SelPagePos = 0;
  PageScrollOffset = 0;
  MenuDepth = 0;
  PageCount    = FormGetPageCount ();
  CfgBuf       = NULL;  // We use FindConfigDataByTag directly
  MaxVisRows   = GetUiVisibleRows ();

  if (PageCount == 0) {
    DEBUG ((DEBUG_ERROR, "UiSetup: No pages in descriptor\n"));
    return EFI_NOT_FOUND;
  }

  FirstLoop = TRUE;
  SearchText[0] = '\0';
  SearchLen = 0;
  SearchActive = FALSE;
  SearchEditMode = FALSE;
  SearchResultMode = FALSE;
  SearchResultForField = FALSE;
  SearchResultsTruncated = FALSE;
  SearchResultCount = 0;
  SearchResultTotal = 0;
  SearchResultSel = 0;
  SearchResultTop = 0;

  //
  // Main loop
  //
  while (State != UI_STATE_EXIT) {
    CurChildCount = CountChildPages (CurParentPageId);
    if (CurChildCount == 0) {
      CurPageIdx = 0xFFFF;
      CurPage = NULL;
    } else {
      if (SelPagePos >= CurChildCount) {
        SelPagePos = (UINT16)(CurChildCount - 1);
      }
      CurPageIdx = GetChildPageIndexByOrder (CurParentPageId, SelPagePos);
      CurPage = FormGetPage (CurPageIdx);
    }

    //
    // Render current state
    //
    VisFieldCount = CountVisibleFields (CurPage, CfgBuf);

    //
    // Show help for currently selected field
    //
    CurField = GetVisibleField (CurPage, ScrollOffset + SelField, CfgBuf);
    if (CurField != NULL) {
      HelpText = FormGetString (CurField->HelpStrOffset);
    } else {
      HelpText = "";
    }

    {
      BOOLEAN  HasOverlay;
      HasOverlay = (BOOLEAN)(SearchEditMode || SearchResultMode);
      RenderMainViewAll (CurParentPageId, SelPagePos, PageScrollOffset, CurPage, SelField, ScrollOffset, CfgBuf, State, !HasOverlay);
      if (SearchEditMode) {
        RenderFindPopupAll (SearchText, SearchActive);
      } else if (SearchResultMode) {
        RenderFindResultsPopupAll (SearchText, SearchResultLines, SearchResultCount, SearchResultTotal, SearchResultSel, SearchResultTop, SearchResultsTruncated);
      }
    }

    if (FirstLoop) {
      FirstLoop = FALSE;
    }

    //
    // Wait for input with periodic serial refresh.
    //
    // When a serial client (e.g. PuTTY) connects after the initial render
    // the already-transmitted bytes are lost.  Poll for ~3 s idle; if no key
    // arrives re-render the screen so a late-joining serial terminal gets the
    // current state without requiring the user to press a key first.
    //
    // Only activate idle refresh when serial output is in use - on a pure
    // framebuffer display there is no late-connect scenario and the extra
    // redraws are unnecessary.
    //
    // Each spin iteration is ~10 µs under QEMU TCG, so 300 000 ≈ 3 s.
    //
    if (RendererHasSerial ()) {
      UINTN  IdleSpins;
      for (IdleSpins = 300000; IdleSpins > 0; IdleSpins--) {
        if (InputKeyReady ()) {
          break;
        }
      }
      if (IdleSpins == 0) {
        // Idle timeout - re-render without consuming a key, then restart wait.
        continue;
      }
    }
    Key = InputGetKey ();

    if (SearchResultMode) {
      HandleSearchResultModeKey (
        Key,
        MaxVisRows,
        &State,
        &CurParentPageId,
        &SelPagePos,
        &PageScrollOffset,
        &ScrollOffset,
        &SelField,
        &SearchResultMode,
        SearchResultForField,
        SearchResultCount,
        &SearchResultSel,
        &SearchResultTop,
        SearchResultParentPageId,
        SearchResultPagePos,
        SearchResultFieldIdx
        );

      continue;
    }

    if (SearchEditMode) {
      HandleSearchEditModeKey (
        Key,
        CfgBuf,
        &SearchEditMode,
        &SearchActive,
        &SearchResultMode,
        SearchText,
        &SearchLen,
        &SearchResultForField,
        &SearchResultsTruncated,
        &SearchResultCount,
        &SearchResultTotal,
        &SearchResultSel,
        &SearchResultTop,
        SearchResultText,
        SearchResultLines,
        SearchResultParentPageId,
        SearchResultPagePos,
        SearchResultFieldIdx
        );

      continue;
    }

    // Allow 'x'/'X' as an ESC alias (only outside search edit mode).
    if ((Key == 'x') || (Key == 'X')) {
      Key = UI_KEY_ESC;
    }

    if ((Key == 'h') || (Key == 'H')) {
      ShowFullHelp (HelpText);
      continue;
    }

    if (Key == '/') {
      SearchEditMode = TRUE;
      SearchActive = FALSE;
      SearchResultMode = FALSE;
      SearchText[0] = '\0';
      SearchLen = 0;
      for (Idx = 0; Idx < UI_FIND_MAX_RESULTS; Idx++) {
        SearchResultLines[Idx] = SearchResultText[Idx];
      }
      continue;
    }

    switch (State) {
      case UI_STATE_MAIN_MENU:
        //
        // Navigate between pages (left panel focused)
        //
        switch (Key) {
          case UI_KEY_UP:
            if (SelPagePos > 0) {
              SelPagePos--;
              if (SelPagePos < PageScrollOffset) {
                PageScrollOffset = SelPagePos;
              }
            }
            break;
          case UI_KEY_DOWN:
            if ((SelPagePos + 1) < CurChildCount) {
              SelPagePos++;
              if (SelPagePos >= (UINT16)(PageScrollOffset + MaxVisRows)) {
                PageScrollOffset++;
              }
            }
            break;
          case UI_KEY_ENTER:
          case UI_KEY_RIGHT:
            if (CurPage == NULL) {
              break;
            }

            if (PageHasChildren (CurPageIdx)) {
              if (MenuDepth < UI_MENU_MAX_DEPTH) {
                ParentStack[MenuDepth] = CurParentPageId;
                SelPosStack[MenuDepth] = SelPagePos;
                ScrollStack[MenuDepth] = PageScrollOffset;
                MenuDepth++;
              }
              CurParentPageId = CurPage->PageId;
              SelPagePos = 0;
              PageScrollOffset = 0;
            } else {
              State = UI_STATE_PAGE;
              SelField = 0;
              ScrollOffset = 0;
            }
            break;
          case UI_KEY_TAB:
            if (CurPage != NULL) {
              State = UI_STATE_PAGE;
              SelField = 0;
              ScrollOffset = 0;
            }
            break;
          case UI_KEY_QUIT:
            if (CfgIsDirty ()) {
              if (HandleSaveConfirm ()) {
                if (SaveChangesWithFeedback ()) {
                  State = UI_STATE_EXIT;
                }
              } else {
                State = UI_STATE_EXIT;
              }
            } else {
              State = UI_STATE_EXIT;
            }
            break;
          case UI_KEY_LEFT:
            if (MenuDepth > 0) {
              MenuDepth--;
              CurParentPageId = ParentStack[MenuDepth];
              SelPagePos = SelPosStack[MenuDepth];
              PageScrollOffset = ScrollStack[MenuDepth];
            }
            break;
          case UI_KEY_ESC:
            if (MenuDepth > 0) {
              MenuDepth--;
              CurParentPageId = ParentStack[MenuDepth];
              SelPagePos = SelPosStack[MenuDepth];
              PageScrollOffset = ScrollStack[MenuDepth];
            } else {
              if (CfgIsDirty ()) {
                if (HandleSaveConfirm ()) {
                  if (SaveChangesWithFeedback ()) {
                    State = UI_STATE_EXIT;
                  }
                } else {
                  State = UI_STATE_EXIT;
                }
              } else {
                State = UI_STATE_EXIT;
              }
            }
            break;
          case UI_KEY_F9:
            if (HandleDefaultsConfirm ()) {
              CfgReloadDefaults ();
            }
            break;
          case UI_KEY_F10:
            if (HandleSaveConfirm ()) {
              SaveChangesWithFeedback ();
            }
            break;
          default:
            break;
        }
        break;

      case UI_STATE_PAGE:
        //
        // Navigate fields within a page
        //
        switch (Key) {
          case UI_KEY_UP:
            if (SelField > 0) {
              SelField--;
            } else if (ScrollOffset > 0) {
              ScrollOffset--;
            }
            break;
          case UI_KEY_DOWN:
            if (SelField < MaxVisRows - 1 && (ScrollOffset + SelField) < VisFieldCount - 1) {
              SelField++;
            } else if ((ScrollOffset + SelField) < VisFieldCount - 1) {
              ScrollOffset++;
            }
            break;
          case UI_KEY_PGUP:
            if (ScrollOffset >= MaxVisRows) {
              ScrollOffset -= MaxVisRows;
            } else {
              ScrollOffset = 0;
              SelField = 0;
            }
            break;
          case UI_KEY_PGDN:
            if (ScrollOffset + MaxVisRows < VisFieldCount) {
              ScrollOffset += MaxVisRows;
              if (ScrollOffset + SelField >= VisFieldCount) {
                SelField = VisFieldCount - ScrollOffset - 1;
              }
            }
            break;
          case UI_KEY_ENTER:
            //
            // Edit the selected field
            //
            CurField = GetVisibleField (CurPage, ScrollOffset + SelField, CfgBuf);
            if (CurField != NULL) {
              TagData = CfgGetTagData (CurField->TagId);
              if (TagData != NULL) {
                if (CurField->FieldType == UI_FIELD_TYPE_COMBO) {
                  HandleComboEdit (CurField, (UINT8 *)TagData);
                } else if (CurField->FieldType == UI_FIELD_TYPE_TABLE) {
                  HandleTableEdit (CurField, (UINT8 *)TagData);
                } else if (CurField->FieldType == UI_FIELD_TYPE_EDITNUM) {
                  HandleNumericEdit (CurField, (UINT8 *)TagData);
                }
              }
            }
            break;
          case UI_KEY_QUIT:
            //
            // Direct exit from field navigation (same as from main menu)
            //
            if (CfgIsDirty ()) {
              if (HandleSaveConfirm ()) {
                if (SaveChangesWithFeedback ()) {
                  State = UI_STATE_EXIT;
                }
              } else {
                State = UI_STATE_EXIT;
              }
            } else {
              State = UI_STATE_EXIT;
            }
            break;
          case UI_KEY_ESC:
          case UI_KEY_LEFT:
            //
            // Go back to page list
            //
            State = UI_STATE_MAIN_MENU;
            break;
          case UI_KEY_F9:
            if (HandleDefaultsConfirm ()) {
              CfgReloadDefaults ();
            }
            break;
          case UI_KEY_F10:
            if (HandleSaveConfirm ()) {
              SaveChangesWithFeedback ();
            }
            break;
          default:
            break;
        }
        break;

      default:
        State = UI_STATE_EXIT;
        break;
    }
  }

  // Clear the UiSetup framebuffer content before handing control back to shell.
  ClearAllOutputs ();

  return EFI_SUCCESS;
}

