/** @file
  Form Engine header - descriptor parser and field value access.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _FORM_ENGINE_H_
#define _FORM_ENGINE_H_

#include "Include/UiCommon.h"

/**
  Initialize the form engine with a UI descriptor binary.

  @param[in]  DescBuf   Pointer to the UI descriptor binary.
  @param[in]  DescLen   Length of the descriptor binary.

  @retval EFI_SUCCESS   Form engine initialized.
  @retval Others        Invalid descriptor.
**/
EFI_STATUS
FormInit (
  IN CONST UINT8  *DescBuf,
  IN UINT32        DescLen
  );

/**
  Get the number of pages in the descriptor.

  @return Number of pages.
**/
UINT16
FormGetPageCount (
  VOID
  );

/**
  Get the number of fields in the descriptor.

  @return Number of fields.
**/
UINT16
FormGetFieldCount (
  VOID
  );

/**
  Get a page entry by index.

  @param[in]  PageIdx   Page index (0-based).

  @return Pointer to the page entry, or NULL if out of range.
**/
UI_PAGE_ENTRY *
FormGetPage (
  IN UINT16  PageIdx
  );

/**
  Get a field entry by index.

  @param[in]  FieldIdx  Field index (0-based).

  @return Pointer to the field entry, or NULL if out of range.
**/
UI_FIELD_ENTRY *
FormGetField (
  IN UINT16  FieldIdx
  );

/**
  Get the string at a given offset in the string table.

  @param[in]  StrOffset  Offset into the string table.

  @return Pointer to the null-terminated string.
**/
CHAR8 *
FormGetString (
  IN UINT32  StrOffset
  );

/**
  Get the canonical field path for a field.

  @param[in]  Field  The field entry.

  @return Pointer to the null-terminated path string.
**/
CHAR8 *
FormGetFieldPath (
  IN UI_FIELD_ENTRY  *Field
  );

/**
  Find a field by canonical path.

  @param[in]  FieldPath  Canonical field path.

  @return Pointer to the field entry, or NULL if not found.
**/
UI_FIELD_ENTRY *
FormFindFieldByPath (
  IN CONST CHAR8  *FieldPath
  );

/**
  Read a field value from the config buffer.

  @param[in]  Field    The field entry.
  @param[in]  TagData  Pointer to the tag's data in the config buffer.

  @return The field value (up to 32 bits).
**/
UINT32
FormGetFieldValue (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData
  );

/**
  Write a field value into the config buffer.

  @param[in]  Field    The field entry.
  @param[in]  TagData  Pointer to the tag's data in the config buffer.
  @param[in]  Value    Value to write.
**/
VOID
FormSetFieldValue (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT8           *TagData,
  IN UINT32           Value
  );

/**
  Evaluate a condition entry against current config data.

  @param[in]  CondIdx   Index into condition table (0xFF = always true).
  @param[in]  CfgBuf    Full config data buffer.

  @return TRUE if condition is met (field should be visible).
**/
BOOLEAN
FormEvalCondition (
  IN UINT8   CondIdx,
  IN UINT8  *CfgBuf
  );

/**
  Get options for a field.

  @param[in]   Field   The field entry.
  @param[out]  Count   Number of options returned.

  @return Pointer to the first option entry.
**/
UI_OPTION_ENTRY *
FormGetOptions (
  IN  UI_FIELD_ENTRY  *Field,
  OUT UINT16          *Count
  );

#endif // _FORM_ENGINE_H_
