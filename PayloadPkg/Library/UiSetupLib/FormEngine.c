/** @file
  Form Engine - UI descriptor parser and field value access.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "FormEngine.h"
#include "ConfigBridge.h"
#include <Library/PcdLib.h>


STATIC UI_DESC_HEADER      *mDescHdr;
STATIC UI_PAGE_ENTRY       *mPages;
STATIC UI_FIELD_ENTRY      *mFields;
STATIC UI_OPTION_ENTRY     *mOptions;
STATIC UI_CONDITION_ENTRY  *mConditions;
STATIC CHAR8               *mStringTable;

/**
  Initialize the form engine with a UI descriptor binary.
**/
EFI_STATUS
FormInit (
  IN CONST UINT8  *DescBuf,
  IN UINT32        DescLen
  )
{
  UI_DESC_HEADER  *Hdr;
  UINTN            TotalSize;
  UINTN            PageEnd;
  UINTN            FieldEnd;
  UINTN            OptionEnd;
  UINTN            ConditionEnd;

  if (DescBuf == NULL || DescLen < sizeof (UI_DESC_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  Hdr = (UI_DESC_HEADER *)DescBuf;
  if (Hdr->Signature != UI_DESC_SIGNATURE || Hdr->Version != UI_DESC_VERSION) {
    return EFI_INCOMPATIBLE_VERSION;
  }

  if (Hdr->TotalSize > DescLen) {
    return EFI_INVALID_PARAMETER;
  }

  TotalSize = (UINTN)Hdr->TotalSize;
  PageEnd = sizeof (UI_DESC_HEADER) + ((UINTN)Hdr->PageCount * sizeof (UI_PAGE_ENTRY));
  if (PageEnd > TotalSize) {
    return EFI_INVALID_PARAMETER;
  }

  FieldEnd = PageEnd + ((UINTN)Hdr->FieldCount * sizeof (UI_FIELD_ENTRY));
  if (FieldEnd > TotalSize) {
    return EFI_INVALID_PARAMETER;
  }

  OptionEnd = FieldEnd + ((UINTN)Hdr->OptionCount * sizeof (UI_OPTION_ENTRY));
  if (OptionEnd > TotalSize) {
    return EFI_INVALID_PARAMETER;
  }

  ConditionEnd = OptionEnd + ((UINTN)Hdr->ConditionCount * sizeof (UI_CONDITION_ENTRY));
  if (ConditionEnd > TotalSize) {
    return EFI_INVALID_PARAMETER;
  }

  if (((UINTN)Hdr->StringTableOffset < ConditionEnd) || ((UINTN)Hdr->StringTableOffset > TotalSize)) {
    return EFI_INVALID_PARAMETER;
  }

  mDescHdr = Hdr;

  //
  // Set up pointers to each section
  //
  mPages      = (UI_PAGE_ENTRY *)((UINT8 *)Hdr + sizeof (UI_DESC_HEADER));
  mFields     = (UI_FIELD_ENTRY *)((UINT8 *)mPages + Hdr->PageCount * sizeof (UI_PAGE_ENTRY));
  mOptions    = (UI_OPTION_ENTRY *)((UINT8 *)mFields + Hdr->FieldCount * sizeof (UI_FIELD_ENTRY));
  mConditions = (UI_CONDITION_ENTRY *)((UINT8 *)mOptions + Hdr->OptionCount * sizeof (UI_OPTION_ENTRY));
  mStringTable = (CHAR8 *)((UINT8 *)Hdr + Hdr->StringTableOffset);

  return EFI_SUCCESS;
}

/**
  Get the number of pages.
**/
UINT16
FormGetPageCount (
  VOID
  )
{
  if (mDescHdr == NULL) {
    return 0;
  }
  return mDescHdr->PageCount;
}

/**
  Get the number of fields.
**/
UINT16
FormGetFieldCount (
  VOID
  )
{
  if (mDescHdr == NULL) {
    return 0;
  }
  return mDescHdr->FieldCount;
}

/**
  Get a page entry by index.
**/
UI_PAGE_ENTRY *
FormGetPage (
  IN UINT16  PageIdx
  )
{
  if (mDescHdr == NULL || PageIdx >= mDescHdr->PageCount) {
    return NULL;
  }
  return &mPages[PageIdx];
}

/**
  Get a field entry by index.
**/
UI_FIELD_ENTRY *
FormGetField (
  IN UINT16  FieldIdx
  )
{
  if (mDescHdr == NULL || FieldIdx >= mDescHdr->FieldCount) {
    return NULL;
  }
  return &mFields[FieldIdx];
}

/**
  Get string at offset.
**/
CHAR8 *
FormGetString (
  IN UINT32  StrOffset
  )
{
  UINTN  StrTableSize;

  if (mDescHdr == NULL) {
    return "";
  }

  if ((UINTN)mDescHdr->StringTableOffset > (UINTN)mDescHdr->TotalSize) {
    return "";
  }

  StrTableSize = (UINTN)mDescHdr->TotalSize - (UINTN)mDescHdr->StringTableOffset;
  if ((UINTN)StrOffset >= StrTableSize) {
    return "";
  }

  return mStringTable + StrOffset;
}

/**
  Get the canonical path string for a field.
**/
CHAR8 *
FormGetFieldPath (
  IN UI_FIELD_ENTRY  *Field
  )
{
  if (Field == NULL) {
    return "";
  }
  return FormGetString (Field->PathStrOffset);
}

/**
  Find a field by canonical path.
**/
UI_FIELD_ENTRY *
FormFindFieldByPath (
  IN CONST CHAR8  *FieldPath
  )
{
  UINT16          FieldIdx;
  UI_FIELD_ENTRY *Field;

  if ((mDescHdr == NULL) || (FieldPath == NULL) || (*FieldPath == '\0')) {
    return NULL;
  }

  for (FieldIdx = 0; FieldIdx < mDescHdr->FieldCount; FieldIdx++) {
    Field = &mFields[FieldIdx];
    if (AsciiStriCmp (FormGetFieldPath (Field), FieldPath) == 0) {
      return Field;
    }
  }

  return NULL;
}

/**
  Read bits from a byte buffer.
**/
STATIC
UINT32
ReadBitField (
  IN UINT8   *Data,
  IN UINT16   BitOffset,
  IN UINT16   BitLength
  )
{
  UINT32  Value;
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if (BitLength == 0 || BitLength > 32) {
    return 0;
  }

  Value = 0;
  for (Idx = 0; Idx < BitLength; Idx++) {
    ByteOff = (BitOffset + Idx) / 8;
    BitOff  = (BitOffset + Idx) % 8;
    if (Data[ByteOff] & (1 << BitOff)) {
      Value |= (1 << Idx);
    }
  }

  return Value;
}

/**
  Write bits to a byte buffer.
**/
STATIC
VOID
WriteBitField (
  IN UINT8   *Data,
  IN UINT16   BitOffset,
  IN UINT16   BitLength,
  IN UINT32   Value
  )
{
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  if (BitLength == 0 || BitLength > 32) {
    return;
  }

  for (Idx = 0; Idx < BitLength; Idx++) {
    ByteOff = (BitOffset + Idx) / 8;
    BitOff  = (BitOffset + Idx) % 8;
    if (Value & (1 << Idx)) {
      Data[ByteOff] |= (UINT8)(1 << BitOff);
    } else {
      Data[ByteOff] &= (UINT8)~(1 << BitOff);
    }
  }
}

/**
  Read a field value from the config buffer.
**/
UINT32
FormGetFieldValue (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  )
{
  if (Field == NULL || TagData == NULL) {
    return 0;
  }
  return ReadBitField (TagData, Field->BitOffset, Field->BitLength);
}

/**
  Write a field value into the config buffer.
**/
VOID
FormSetFieldValue (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN UINT32           Value
  )
{
  if (Field == NULL || TagData == NULL) {
    return;
  }
  WriteBitField (TagData, Field->BitOffset, Field->BitLength, Value);
}

/**
  Evaluate a condition.
**/
BOOLEAN
FormEvalCondition (
  IN UINT8   CondIdx,
  IN UINT8  *CfgBuf
  )
{
  UI_CONDITION_ENTRY  *Cond;
  VOID                *TagData;
  UINT32               FieldVal;

  //
  // 0xFF means unconditional (always visible)
  //
  if (CondIdx == 0xFF) {
    return TRUE;
  }

  // currently unused.
  (VOID)CfgBuf;

  if (mDescHdr == NULL || CondIdx >= mDescHdr->ConditionCount) {
    return TRUE;
  }

  Cond = &mConditions[CondIdx];

  //
  // Find the tag data in the config buffer
  //
  TagData = CfgGetTagData (Cond->TagId);
  if (TagData == NULL) {
    return TRUE;
  }

  FieldVal = ReadBitField ((UINT8 *)TagData, Cond->BitOffset, Cond->BitLength);

  switch (Cond->Operator) {
    case UI_COND_OP_EQUAL:
      return (FieldVal == Cond->CompareValue);
    case UI_COND_OP_NOT_EQUAL:
      return (FieldVal != Cond->CompareValue);
    case UI_COND_OP_GREATER:
      return (FieldVal > Cond->CompareValue);
    case UI_COND_OP_LESS:
      return (FieldVal < Cond->CompareValue);
    default:
      return TRUE;
  }
}

/**
  Get options for a field.
**/
UI_OPTION_ENTRY *
FormGetOptions (
  IN  UI_FIELD_ENTRY  *Field,
  OUT UINT16          *Count
  )
{
  if (Field == NULL || Count == NULL) {
    if (Count != NULL) {
      *Count = 0;
    }
    return NULL;
  }

  *Count = Field->OptionCount;
  if (Field->OptionCount == 0) {
    return NULL;
  }

  return &mOptions[Field->OptionStartIdx];
}

