/** @file
  Config Bridge header - config data load/save via BuildConfigData + Variables.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _CONFIG_BRIDGE_H_
#define _CONFIG_BRIDGE_H_

#include "Include/UiCommon.h"

/**
  Load config data: call BuildConfigData and apply any existing delta from VARS.

  @retval EFI_SUCCESS   Config loaded.
  @retval Others        Failure.
**/
EFI_STATUS
CfgLoad (
  VOID
  );

/**
  Save modified config data as a delta to the variable store.

  @retval EFI_SUCCESS   Delta saved.
  @retval Others        Failure.
**/
EFI_STATUS
CfgSave (
  VOID
  );

/**
  Reload defaults: delete delta variable and re-load from BuildConfigData.

  @retval EFI_SUCCESS   Defaults loaded.
  @retval Others        Failure.
**/
EFI_STATUS
CfgReloadDefaults (
  VOID
  );

/**
  Get the working config buffer pointer.

  @return Pointer to config data buffer.
**/
UINT8 *
CfgGetBuffer (
  VOID
  );

/**
  Get tag data pointer within the working buffer.

  @param[in]  TagId   Tag identifier.

  @return Pointer to the tag's data, or NULL.
**/
UINT8 *
CfgGetTagData (
  IN UINT16  TagId
  );

/**
  Check if any changes are unsaved.

  @return TRUE if there are unsaved changes.
**/
BOOLEAN
CfgIsDirty (
  VOID
  );

/**
  Mark the config as dirty (modified).
**/
VOID
CfgSetDirty (
  VOID
  );

/**
  Record a changed field/value pair into the in-memory delta list.

  @param[in]  Field   Field descriptor.
  @param[in]  Value   New field value.
**/
VOID
CfgRecordFieldChange (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT32           Value
  );

/**
  Check whether a field has unsaved changes in current UiSetup session.

  @param[in]  Field   Field descriptor.

  @return TRUE if field is changed and not yet saved.
**/
BOOLEAN
CfgIsFieldUnsaved (
  IN UI_FIELD_ENTRY  *Field
  );

#endif // _CONFIG_BRIDGE_H_
