/** @file
  Config Bridge - config data load/save via FindConfigDataByTag + Variables.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "ConfigBridge.h"
#include "FormEngine.h"
#include <Library/BootloaderCommonLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/HobLib.h>
#include <Guid/LoaderFspInfoGuid.h>
#include <Guid/FspHeaderFile.h>
#include <ConfigDataStruct.h>
#include <FspEas/FspApi.h>
#include <FspUpdCfgRegion.h>

STATIC BOOLEAN   mDirty;
STATIC UI_CFG_DELTA_ENTRY mDeltaEntries[UI_CFG_DELTA_MAX_ENTRIES];
STATIC UINT8              mDeltaEntryCount;
STATIC UI_CFG_DELTA_ENTRY mUnsavedEntries[UI_CFG_DELTA_MAX_ENTRIES];
STATIC UINT8              mUnsavedEntryCount;
STATIC UINT8             *mDefaultCfgData;
STATIC UINT32             mDefaultCfgDataSize;
STATIC BOOLEAN            mFspsUpdPtrMissingLogged;
STATIC BOOLEAN            mFspsUpdPtrFoundLogged;

STATIC
LOADER_FSP_INFO *
GetLoaderFspInfo (
  VOID
  )
{
  return (LOADER_FSP_INFO *)GetGuidHobData (NULL, NULL, &gLoaderFspInfoGuid);
}

/**
  Write bits to a byte buffer.
**/
STATIC
VOID
WriteBits (
  IN UINT8   *Data,
  IN UINT16   BitOffset,
  IN UINT16   BitLength,
  IN UINT32   Value
  )
{
  UINT32  ByteOff;
  UINT32  BitOff;
  UINT32  Idx;

  for (Idx = 0; Idx < BitLength && Idx < 32; Idx++) {
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
  Load existing delta from variable store and apply to current config.
**/
STATIC
EFI_STATUS
ApplyDeltaFromVars (
  VOID
  )
{
  EFI_STATUS          Status;
  UINT8               DeltaBuf[8 + UI_CFG_DELTA_MAX_ENTRIES * sizeof(UI_CFG_DELTA_ENTRY)];
  UINTN               DataSize;
  UINTN               ExpectedSize;
  UI_CFG_DELTA_HDR   *DeltaHdr;
  UI_CFG_DELTA_ENTRY *Entry;
  UINT8               Idx;
  VOID               *TagData;

  DataSize = sizeof (DeltaBuf);
  Status = GetVariable (UI_CFG_DELTA_VAR_NAME, &gUiSetupCfgDeltaGuid, NULL, &DataSize, DeltaBuf);
  if (EFI_ERROR (Status)) {
    mDeltaEntryCount = 0;
    return Status;
  }

  if (DataSize < sizeof (UI_CFG_DELTA_HDR)) {
    mDeltaEntryCount = 0;
    return EFI_COMPROMISED_DATA;
  }

  DeltaHdr = (UI_CFG_DELTA_HDR *)DeltaBuf;
  if (DeltaHdr->Signature != UI_CFG_DELTA_SIGNATURE ||
      DeltaHdr->Version   != UI_CFG_DELTA_VERSION) {
    mDeltaEntryCount = 0;
    return EFI_COMPROMISED_DATA;
  }

  ExpectedSize = sizeof (UI_CFG_DELTA_HDR) + ((UINTN)DeltaHdr->EntryCount * sizeof (UI_CFG_DELTA_ENTRY));
  if (DataSize < ExpectedSize) {
    mDeltaEntryCount = 0;
    return EFI_COMPROMISED_DATA;
  }

  mDeltaEntryCount = DeltaHdr->EntryCount;
  if (mDeltaEntryCount > UI_CFG_DELTA_MAX_ENTRIES) {
    mDeltaEntryCount = UI_CFG_DELTA_MAX_ENTRIES;
  }
  if (mDeltaEntryCount > 0) {
    CopyMem (mDeltaEntries, DeltaHdr->Entries, mDeltaEntryCount * sizeof (UI_CFG_DELTA_ENTRY));
  }

  for (Idx = 0; Idx < mDeltaEntryCount; Idx++) {
    Entry = &DeltaHdr->Entries[Idx];
    TagData = CfgGetTagData ((UINT16)Entry->TagId);
    if (TagData != NULL) {
      WriteBits ((UINT8 *)TagData, Entry->BitOffset, Entry->BitLength, Entry->Value);
    }
  }

  return EFI_SUCCESS;
}

/**
  Load config data.
**/
EFI_STATUS
CfgLoad (
  VOID
  )
{
  VOID    *CfgDataPtr;
  UINT32   CfgDataLen;

  //
  // Config data is already loaded by SBL boot flow via BuildConfigData().
  // We just apply any previously saved delta on top.
  //
  mDirty = FALSE;
  mDeltaEntryCount = 0;
  mUnsavedEntryCount = 0;

  // Cache the default config snapshot once per UiSetup run.
  CfgDataPtr = GetConfigDataPtr ();
  CfgDataLen = GetConfigDataSize ();
  if ((CfgDataPtr != NULL) && (CfgDataLen > 0)) {
    if ((mDefaultCfgData == NULL) || (mDefaultCfgDataSize != CfgDataLen)) {
      if (mDefaultCfgData != NULL) {
        FreePool (mDefaultCfgData);
        mDefaultCfgData = NULL;
        mDefaultCfgDataSize = 0;
      }

      mDefaultCfgData = AllocatePool (CfgDataLen);
      if (mDefaultCfgData != NULL) {
        mDefaultCfgDataSize = CfgDataLen;
      }
    }

    if ((mDefaultCfgData != NULL) && (mDefaultCfgDataSize == CfgDataLen)) {
      CopyMem (mDefaultCfgData, CfgDataPtr, CfgDataLen);
    }
  }

  ApplyDeltaFromVars ();
  return EFI_SUCCESS;
}

/**
  Save delta to variable store.
  Walks descriptor fields and compares to the original config data.
  For simplicity, we save all fields that have options (Combo type) or numeric
  where the user may have modified values.
**/
EFI_STATUS
CfgSave (
  VOID
  )
{
  EFI_STATUS          Status;
  UINT8               DeltaBuf[8 + UI_CFG_DELTA_MAX_ENTRIES * sizeof(UI_CFG_DELTA_ENTRY)];
  UI_CFG_DELTA_HDR   *DeltaHdr;
  UINTN               PayloadSize;

  if (!mDirty) {
    return EFI_SUCCESS;
  }

  SetMem (DeltaBuf, sizeof (DeltaBuf), 0);
  DeltaHdr = (UI_CFG_DELTA_HDR *)DeltaBuf;
  DeltaHdr->Signature  = UI_CFG_DELTA_SIGNATURE;
  DeltaHdr->Version    = UI_CFG_DELTA_VERSION;
  DeltaHdr->Reserved   = 0;
  DeltaHdr->EntryCount = mDeltaEntryCount;
  if (mDeltaEntryCount > 0) {
    CopyMem (DeltaHdr->Entries, mDeltaEntries, mDeltaEntryCount * sizeof (UI_CFG_DELTA_ENTRY));
  }

  // Use a compact payload to minimize flash writes and avoid reclaim pressure.
  PayloadSize = sizeof (UI_CFG_DELTA_HDR) + mDeltaEntryCount * sizeof (UI_CFG_DELTA_ENTRY);
  Status = SetVariable (
             UI_CFG_DELTA_VAR_NAME,
             &gUiSetupCfgDeltaGuid,
             0,
             PayloadSize,
             DeltaBuf
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UiSetup: CfgSave SetVariable=%r\n", Status));
  }

  if (!EFI_ERROR (Status)) {
    mDirty = FALSE;
    mUnsavedEntryCount = 0;
  }

  return Status;
}

/**
  Reload defaults by deleting the delta variable.
**/
EFI_STATUS
CfgReloadDefaults (
  VOID
  )
{
  UINT8             DeltaBuf[sizeof (UI_CFG_DELTA_HDR)];
  UI_CFG_DELTA_HDR *DeltaHdr;
  EFI_STATUS        Status;
  EFI_STATUS        RestoreStatus;
  VOID             *CfgDataPtr;
  UINT32            CfgDataLen;

  //
  // Reset delta content to an empty header instead of deleting the variable.
  // This avoids hitting the delete path in variable services.
  //
  SetMem (DeltaBuf, sizeof (DeltaBuf), 0);
  DeltaHdr = (UI_CFG_DELTA_HDR *)DeltaBuf;
  DeltaHdr->Signature  = UI_CFG_DELTA_SIGNATURE;
  DeltaHdr->Version    = UI_CFG_DELTA_VERSION;
  DeltaHdr->Reserved   = 0;
  DeltaHdr->EntryCount = 0;

  Status = SetVariable (
             UI_CFG_DELTA_VAR_NAME,
             &gUiSetupCfgDeltaGuid,
             0,
             sizeof (DeltaBuf),
             DeltaBuf
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "UiSetup: CfgReloadDefaults SetVariable=%r\n", Status));
  }

  // Restore live config blob from cached defaults so the UI updates immediately.
  RestoreStatus = EFI_NOT_READY;
  CfgDataPtr = GetConfigDataPtr ();
  CfgDataLen = GetConfigDataSize ();
  if ((CfgDataPtr != NULL) && (CfgDataLen > 0) &&
      (mDefaultCfgData != NULL) && (mDefaultCfgDataSize == CfgDataLen)) {
    CopyMem (CfgDataPtr, mDefaultCfgData, CfgDataLen);
    RestoreStatus = EFI_SUCCESS;
  }
  if (EFI_ERROR (RestoreStatus)) {
    DEBUG ((DEBUG_ERROR, "UiSetup: CfgReloadDefaults RestoreDefaults=%r\n", RestoreStatus));
  }

  mDeltaEntryCount = 0;
  mUnsavedEntryCount = 0;
  mDirty = FALSE;

  if (EFI_ERROR (RestoreStatus)) {
    return RestoreStatus;
  }

  return Status;
}

/**
  Record/overwrite one field change into the in-memory delta list.
**/
VOID
CfgRecordFieldChange (
  IN UI_FIELD_ENTRY  *Field,
  IN UINT32           Value
  )
{
  UINT8    Idx;
  BOOLEAN  Found;

  if (Field == NULL) {
    return;
  }

  Found = FALSE;
  for (Idx = 0; Idx < mDeltaEntryCount; Idx++) {
    if ((mDeltaEntries[Idx].TagId     == Field->TagId) &&
        (mDeltaEntries[Idx].BitOffset == Field->BitOffset) &&
        (mDeltaEntries[Idx].BitLength == Field->BitLength)) {
      mDeltaEntries[Idx].Value = Value;
      Found = TRUE;
      break;
    }
  }

  if (!Found) {
    if (mDeltaEntryCount >= UI_CFG_DELTA_MAX_ENTRIES) {
      // Replace the last entry if we run out of room.
      mDeltaEntries[mDeltaEntryCount - 1].TagId     = Field->TagId;
      mDeltaEntries[mDeltaEntryCount - 1].BitOffset = Field->BitOffset;
      mDeltaEntries[mDeltaEntryCount - 1].BitLength = Field->BitLength;
      mDeltaEntries[mDeltaEntryCount - 1].Reserved  = 0;
      mDeltaEntries[mDeltaEntryCount - 1].Value     = Value;
    } else {
      mDeltaEntries[mDeltaEntryCount].TagId     = Field->TagId;
      mDeltaEntries[mDeltaEntryCount].BitOffset = Field->BitOffset;
      mDeltaEntries[mDeltaEntryCount].BitLength = Field->BitLength;
      mDeltaEntries[mDeltaEntryCount].Reserved  = 0;
      mDeltaEntries[mDeltaEntryCount].Value     = Value;
      mDeltaEntryCount++;
    }
  }

  Found = FALSE;
  for (Idx = 0; Idx < mUnsavedEntryCount; Idx++) {
    if ((mUnsavedEntries[Idx].TagId     == Field->TagId) &&
        (mUnsavedEntries[Idx].BitOffset == Field->BitOffset) &&
        (mUnsavedEntries[Idx].BitLength == Field->BitLength)) {
      mUnsavedEntries[Idx].Value = Value;
      Found = TRUE;
      break;
    }
  }

  if (!Found) {
    if (mUnsavedEntryCount >= UI_CFG_DELTA_MAX_ENTRIES) {
      mUnsavedEntries[mUnsavedEntryCount - 1].TagId     = Field->TagId;
      mUnsavedEntries[mUnsavedEntryCount - 1].BitOffset = Field->BitOffset;
      mUnsavedEntries[mUnsavedEntryCount - 1].BitLength = Field->BitLength;
      mUnsavedEntries[mUnsavedEntryCount - 1].Reserved  = 0;
      mUnsavedEntries[mUnsavedEntryCount - 1].Value     = Value;
    } else {
      mUnsavedEntries[mUnsavedEntryCount].TagId     = Field->TagId;
      mUnsavedEntries[mUnsavedEntryCount].BitOffset = Field->BitOffset;
      mUnsavedEntries[mUnsavedEntryCount].BitLength = Field->BitLength;
      mUnsavedEntries[mUnsavedEntryCount].Reserved  = 0;
      mUnsavedEntries[mUnsavedEntryCount].Value     = Value;
      mUnsavedEntryCount++;
    }
  }
}

/**
  Check whether a field has unsaved changes in current UiSetup session.
**/
BOOLEAN
CfgIsFieldUnsaved (
  IN UI_FIELD_ENTRY  *Field
  )
{
  UINT8  Idx;

  if ((Field == NULL) || !mDirty) {
    return FALSE;
  }

  for (Idx = 0; Idx < mUnsavedEntryCount; Idx++) {
    if ((mUnsavedEntries[Idx].TagId     == Field->TagId) &&
        (mUnsavedEntries[Idx].BitOffset == Field->BitOffset) &&
        (mUnsavedEntries[Idx].BitLength == Field->BitLength)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Get working buffer (not used - we use FindConfigDataByTag directly).
**/
UINT8 *
CfgGetBuffer (
  VOID
  )
{
  return NULL;
}

/**
  Get tag data.
**/
UINT8 *
CfgGetTagData (
  IN UINT16  TagId
  )
{
  UINT8            *TagData;
  UINT8            *FspUpd;
  LOADER_FSP_INFO  *LoaderFspInfo;
  FSP_INFO_HEADER  *FspsHeader;

  TagData = (UINT8 *)FindConfigDataByTag ((UINT32)TagId);
  if (TagData != NULL) {
    return TagData;
  }

  LoaderFspInfo = GetLoaderFspInfo ();
  if (LoaderFspInfo == NULL) {
    return NULL;
  }

  FspsHeader = (FSP_INFO_HEADER *)(UINTN)(LoaderFspInfo->FspsBase + FSP_INFO_HEADER_OFF);

  // FSP UPD tags are not part of the runtime CFGDATA blob. Read from live UPD pointers.
  if (TagId == CDATA_FSPM_UPD_TAG) {
    FspUpd = (UINT8 *)(UINTN)LoaderFspInfo->FspmUpdPtr;
    if (FspUpd != NULL) {
      return FspUpd + GetFspmUpdCfgRegionOffset (FspsHeader);
    }
  } else if (TagId == CDATA_FSPS_UPD_TAG) {
    FspUpd = (UINT8 *)(UINTN)LoaderFspInfo->FspsUpdPtr;
    if (FspUpd != NULL) {
      UINT8 *Cfg = FspUpd + GetFspsUpdCfgRegionOffset (FspsHeader);
      if (!mFspsUpdPtrFoundLogged) {
        DEBUG ((DEBUG_INFO, "UiSetup: FSPS data available via LoaderFspInfo HOB cfg=%p\n", Cfg));
        mFspsUpdPtrFoundLogged = TRUE;
      }
      return Cfg;
    }

    if (!mFspsUpdPtrMissingLogged) {
      DEBUG ((DEBUG_WARN, "UiSetup: FSPS tag 0x%03X has no data (FindConfigDataByTag miss, LoaderFspInfo/FspsUpdPtr is NULL)\n", TagId));
      mFspsUpdPtrMissingLogged = TRUE;
    }
  }

  return NULL;
}

/**
  Check if dirty.
**/
BOOLEAN
CfgIsDirty (
  VOID
  )
{
  return mDirty;
}

/**
  Mark dirty.
**/
VOID
CfgSetDirty (
  VOID
  )
{
  mDirty = TRUE;
}

