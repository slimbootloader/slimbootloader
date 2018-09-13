/** @file

Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/BaseMemoryLib.h>

/**
  Find configuration data header by its tag and platform ID.

  @param[in] PidMask     Platform ID mask.
  @param[in] Tag         Configuration TAG ID to find.
  @param[in] IsInternal  Search for internal data base only if it is non-zero.
  @param[in] Level       Nested call level.

  @retval             Configuration data header pointer.
                      NULL if the tag cannot be found.

**/
CDATA_HEADER *
FindConfigHdrByPidMaskTag (
  UINT32  PidMask,
  UINT32  Tag,
  UINT8   IsInternal,
  UINT32  Level
  )
{
  CDATA_BLOB          *CdataBlob;
  CDATA_HEADER        *CdataHdr;
  UINT8                Idx;
  REFERENCE_CFG_DATA  *Refer;
  UINT32               Offset;

  CdataBlob = (CDATA_BLOB *) GetConfigDataPtr ();
  Offset    = IsInternal > 0 ? (CdataBlob->InternalDataOffset * 4) : CdataBlob->HeaderLength;

  while (Offset < CdataBlob->UsedLength) {
    CdataHdr = (CDATA_HEADER *) ((UINT8 *)CdataBlob + Offset);
    if (CdataHdr->Tag == Tag) {
      for (Idx = 0; Idx < CdataHdr->ConditionNum; Idx++) {
        if ((PidMask & CdataHdr->Condition[Idx].Value) != 0) {
          // Found a match
          if ((CdataHdr->Flags & CDATA_FLAG_TYPE_MASK) == CDATA_FLAG_TYPE_REFER) {
            if (Level > 0) {
              // Prevent multiple level nesting
              return NULL;
            } else {
              Refer = (REFERENCE_CFG_DATA *) ((UINT8 *)CdataHdr + sizeof (CDATA_HEADER) + sizeof (
                                                CDATA_COND) * CdataHdr->ConditionNum);
              return FindConfigHdrByPidMaskTag (PID_TO_MASK (Refer->PlatformId), \
                                                Refer->Tag, (UINT8)Refer->IsInternal, 1);
            }
          } else {
            return (VOID *)CdataHdr;
          }
        }
      }
    }
    Offset += (CdataHdr->Length << 2);
  }
  return NULL;
}

/**
  Find configuration data header by its tag.

  @param[in] Tag     Configuration TAG ID to find.

  @retval            Configuration data header pointer.
                     NULL if the tag cannot be found.

**/
CDATA_HEADER *
EFIAPI
FindConfigHdrByTag (
  UINT32  Tag
  )
{
  UINT16               PlatformId;

  PlatformId = GetPlatformId ();
  return FindConfigHdrByPidMaskTag (PID_TO_MASK (PlatformId), Tag, 0, 0);
}

/**
  Find configuration data by its tag and PlatforID.

  @param[in] PlatformId     Platform ID.
  @param[in] Tag            Configuration TAG ID to find.

  @retval            Configuration data pointer.
                     NULL if the tag cannot be found.

**/
VOID *
EFIAPI
FindConfigDataByPidTag (
  UINT16  PlatformId,
  UINT32  Tag
  )
{
  CDATA_HEADER        *CdataHdr;
  VOID                *Cdata;

  CdataHdr = FindConfigHdrByPidMaskTag (PID_TO_MASK (PlatformId), Tag, 0, 0);
  if (CdataHdr == NULL) {
    Cdata = NULL;
  } else {
    Cdata = (VOID *) ((UINT8 *)CdataHdr + sizeof (CDATA_HEADER) + sizeof (CDATA_COND) * CdataHdr->ConditionNum);
  }

  return Cdata;
}

/**
  Find configuration data by its tag.

  @param[in] Tag     Configuration TAG ID to find.

  @retval            Configuration data pointer.
                     NULL if the tag cannot be found.

**/
VOID *
EFIAPI
FindConfigDataByTag (
  UINT32  Tag
  )
{
  CDATA_HEADER        *CdataHdr;
  VOID                *Cdata;

  CdataHdr = FindConfigHdrByTag (Tag);
  if (CdataHdr == NULL) {
    Cdata = NULL;
  } else {
    Cdata = (VOID *) ((UINT8 *)CdataHdr + sizeof (CDATA_HEADER) + sizeof (CDATA_COND) * CdataHdr->ConditionNum);
  }

  return Cdata;
}

/**
  Add new Config Data

  Ext/Built-In Config Data for a specific board
  is added to the Config Data Blob present at the
  CfgDataPtr in the LDR_GLOBAL_DATA. Exclude the
  CFG_BLOB header here as it is already there with CfgDataPtr.

  @param[in] CfgAddPtr              Address of the CfgBlob that is to be added

  @retval EFI_SUCCESS               if the CfgBlob is added successfully
  @retval EFI_UNSUPPORTED           if not a valid CfgBlob
  @retval EFI_OUT_OF_RESOURCES      if not enough memory available to add Config Data

**/
EFI_STATUS
EFIAPI
AddConfigData (
  IN  UINT8                *CfgAddPtr
  )
{
  CDATA_BLOB               *LdrCfgBlob;
  CDATA_BLOB               *CfgAddBlob;
  INT32                    CfgAddSize;

  LdrCfgBlob = (CDATA_BLOB *) GetConfigDataPtr ();
  CfgAddBlob = (CDATA_BLOB *) CfgAddPtr;
  if ((CfgAddBlob == NULL) || (CfgAddBlob->Signature != CFG_DATA_SIGNATURE)) {
    return EFI_UNSUPPORTED;
  }

  CfgAddSize = CfgAddBlob->UsedLength - (UINT32) (CfgAddBlob->HeaderLength);
  if (CfgAddSize < 0) {
    return EFI_UNSUPPORTED;
  } else if (CfgAddSize > (INT32) (LdrCfgBlob->TotalLength - LdrCfgBlob->UsedLength)) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (LdrCfgBlob->InternalDataOffset == 0) {
    // Append new config data before internal config data is available.
    CopyMem ((UINT8 *)LdrCfgBlob + LdrCfgBlob->UsedLength,
             (UINT8 *)CfgAddBlob + CfgAddBlob->HeaderLength,
             CfgAddSize);
  } else {
    //
    // Newly added config data has high priority
    // so move the the old config data firstly, 
    // then copy new config data just below header
    //
    CopyMem ((UINT8 *)LdrCfgBlob + LdrCfgBlob->HeaderLength + CfgAddSize,
             (UINT8 *)LdrCfgBlob + LdrCfgBlob->HeaderLength,
             LdrCfgBlob->UsedLength - LdrCfgBlob->HeaderLength);

    CopyMem ((UINT8 *)LdrCfgBlob + LdrCfgBlob->HeaderLength,
             (UINT8 *)CfgAddBlob + CfgAddBlob->HeaderLength,
             CfgAddSize);
    LdrCfgBlob->InternalDataOffset += (UINT16) (CfgAddSize >> 2);
  }
  LdrCfgBlob->UsedLength += CfgAddSize;

  return EFI_SUCCESS;
}

/**
  Get the source of the external configuration data to load.

  @param[in] Index              Index of the source

  @retval PDR                   if external data is loaded from SPI Flash PDR region
  @retval BIOS                  if external data is loaded from SPI Flash BIOS region
  @retval NULL                  if external data is loaded from niether

**/
CHAR8 *
GetCfgDataSource (
  IN  UINT8       Index
  )
{
  if (Index == 0) {
    return "PDR";
  } else if (Index == 1) {
    return "BIOS";
  } else {
    return "NULL";
  }
}
