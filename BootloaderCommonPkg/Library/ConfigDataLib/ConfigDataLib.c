/** @file

Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

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
  Offset    = IsInternal > 0 ? (CdataBlob->ExtraInfo.InternalDataOffset * 4) : CdataBlob->HeaderLength;

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

  if (LdrCfgBlob->ExtraInfo.InternalDataOffset == 0) {
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
    LdrCfgBlob->ExtraInfo.InternalDataOffset += (UINT16) (CfgAddSize >> 2);
  }
  LdrCfgBlob->UsedLength += CfgAddSize;

  return EFI_SUCCESS;
}

/**
  Extract array entry ID from the item data.

  @param[in] ArrayCfgHdr        Array CFGDATA header pointer.
  @param[in] ItemData           Array item data pointer.

  @retval    The item ID.

**/
STATIC
UINT32
GetArrayItemId (
  IN  ARRAY_CFG_HDR   *ArrayCfgHdr,
  IN  UINT8           *ItemData
)
{
  UINT32  Id;
  UINT32  Offset;

  Offset = ArrayCfgHdr->ItemIdBitOff >> 3;
  Id     = *(UINT32 *)(ItemData + Offset);
  Id   >>= (ArrayCfgHdr->ItemIdBitOff & 0x07);
  Id    &= ((1 << ArrayCfgHdr->ItemIdBitLen) - 1);
  return Id;
}

/**
  Get a full CFGDATA set length.

  @retval   Length of a full CFGDATA set.
            0 indicates no CFGDATA exists.

**/
UINT32
EFIAPI
GetConfigDataSize (
  VOID
)
{
  UINT32               Start;
  UINT32               Offset;
  UINT32               CfgBlobHdrLen;
  UINT32               PidMask;
  UINT32               CondVal;
  CDATA_BLOB          *LdrCfgBlob;
  CDATA_HEADER        *CdataHdr;

  LdrCfgBlob  = (CDATA_BLOB *) GetConfigDataPtr ();
  if (LdrCfgBlob == NULL) {
    return 0;
  }

  CfgBlobHdrLen = LdrCfgBlob->HeaderLength;
  if (LdrCfgBlob->ExtraInfo.InternalDataOffset == 0) {
    // No  internal CFGDATA
    Start   = CfgBlobHdrLen;
    PidMask = (1 << GetPlatformId ());
  } else {
    // Has internal CFGDATA
    Start   = LdrCfgBlob->ExtraInfo.InternalDataOffset * 4;
    PidMask = BIT0;
  }
  Offset = Start;
  while (Offset < LdrCfgBlob->UsedLength) {
    CdataHdr = (CDATA_HEADER *) ((UINT8 *)LdrCfgBlob + Offset);
    CondVal  = CdataHdr->Condition[0].Value;
    if ((CondVal != 0) && ((CondVal & PidMask) == 0)) {
      break;
    }
    Offset += (CdataHdr->Length << 2);
  }

  return Offset - Start + CfgBlobHdrLen;
}

/**
  Build a full set of CFGDATA for current platform.

  @param[in] Buffer             Buffer pointer to store the full CFGDATA set.
  @param[in] Length             Buffer length.

  @retval EFI_BUFFER_TOO_SMALL  The buffer size is too small to store the CFGDATA.
  @retval EFI_INVALID_PARAMETER The buffer pointer is NULL.
  @retval EFI_NOT_FOUND         Could not find some CFGDATA tag.
  @retval EFI_SUCCESS           The full set of CFGDATA was built successfully.

**/
EFI_STATUS
EFIAPI
BuildConfigData (
  IN  UINT8      *Buffer,
  IN  UINT32      BufLen
  )
{
  CDATA_BLOB          *LdrCfgBlob;
  CDATA_BLOB          *CdataBlob;
  CDATA_HEADER        *CdataHdr;
  CDATA_HEADER        *CdataHdrCurr;
  ARRAY_CFG_HDR       *ArrayHdr;
  ARRAY_CFG_HDR       *ArrayHdrCurr;
  UINT8               *Cdata;
  UINT8               *CdataCurr;
  UINT8               *Ptr;
  UINT32               Index;
  UINT32               Index2;
  UINT32               Length;
  UINT32               Offset;
  UINT32               Offset1;
  UINT32               Offset2;
  UINT32               HdrLen;
  UINT32               ItemId;
  UINT32               BitMaskLen;
  UINT32               GpioTableDataOffset;
  UINT32               CfgBlobHdrLen;
  PLATFORMID_CFG_DATA *PidCfg;

  Length = GetConfigDataSize ();
  if (Length > BufLen) {
    return EFI_BUFFER_TOO_SMALL;
  }

  CdataBlob     = (CDATA_BLOB *) Buffer;
  LdrCfgBlob    = (CDATA_BLOB *) GetConfigDataPtr ();
  CfgBlobHdrLen = LdrCfgBlob->HeaderLength;
  if (LdrCfgBlob->ExtraInfo.InternalDataOffset == 0) {
    // No internal CFGDATA
    Offset = CfgBlobHdrLen;
  } else {
    // Has internal CFGDATA
    Offset = LdrCfgBlob->ExtraInfo.InternalDataOffset * 4;
  }

  // Copy CFGDATA blob header first and then CfgData default
  CopyMem (CdataBlob, LdrCfgBlob, CfgBlobHdrLen);
  CopyMem ((UINT8 *)CdataBlob + CfgBlobHdrLen, (UINT8 *)LdrCfgBlob + Offset, Length - CfgBlobHdrLen);
  CdataBlob->ExtraInfo.InternalDataOffset = 0;
  CdataBlob->UsedLength  = Length;
  CdataBlob->TotalLength = Length;

  // Build each CFGDATA tag
  Offset = CdataBlob->HeaderLength;
  while (Offset < CdataBlob->UsedLength) {
    CdataHdr = (CDATA_HEADER *) ((UINT8 *)CdataBlob + Offset);
    CdataHdrCurr = FindConfigHdrByTag (CdataHdr->Tag);
    if (CdataHdrCurr != NULL) {
      // Set platform mask
      if (CdataHdr->Tag == CDATA_PLATFORMID_TAG) {
        CdataHdr->Condition[0].Value = 0xFFFFFFFF;
      } else {
        CdataHdr->Condition[0].Value = (1 << GetPlatformId ());
      }

      CdataCurr = ((UINT8 *)CdataHdrCurr + sizeof (CDATA_HEADER) + sizeof (CDATA_COND) * CdataHdrCurr->ConditionNum);
      HdrLen = sizeof (CDATA_HEADER) + sizeof (CDATA_COND) * CdataHdr->ConditionNum;
      Cdata  = (UINT8 *)CdataHdr + HdrLen;
      if ((CdataHdr->Flags & CDATA_FLAG_TYPE_ARRAY) != 0) {
        // Handle array item specially
        ArrayHdr     = (ARRAY_CFG_HDR *)Cdata;
        ArrayHdrCurr = (ARRAY_CFG_HDR *)CdataCurr;
        BitMaskLen   = (CdataHdr->Length << 2) - HdrLen - (UINT32)OFFSET_OF(ARRAY_CFG_HDR, BaseTableBitMask) \
                       - (ArrayHdr->ItemSize * ArrayHdr->ItemCount);

        // Update BaseTableBitMask
        CopyMem (ArrayHdr->BaseTableBitMask, ArrayHdrCurr->BaseTableBitMask, BitMaskLen);
        ArrayHdr->BaseTableId = 0x80;

        // Update skip bit accordingly in the entry based on BaseTableBitMask
        GpioTableDataOffset = (UINT32)OFFSET_OF(ARRAY_CFG_HDR, BaseTableBitMask) + BitMaskLen;
        for (Index2 = 0; Index2 < ArrayHdr->ItemCount; Index2++) {
          if ((ArrayHdr->BaseTableBitMask[Index2 >> 3] & (1 << (Index2 & 7))) == 0) {
            // Skip one entry
            Offset2 = Index2 * ArrayHdr->ItemSize;
            Ptr   = Cdata + GpioTableDataOffset + Offset2;
            Index = ArrayHdr->ItemValidBitOff;
            Ptr[Index >> 3] |= (1 << (Index & 7));
          }
        }

        // Update extra entries
        for (Index = 0; Index  < ArrayHdrCurr->ItemCount; Index++) {
          Offset1 = Index * ArrayHdrCurr->ItemSize;
          ItemId  = GetArrayItemId (ArrayHdrCurr, CdataCurr + GpioTableDataOffset + Offset1);
          for (Index2 = 0; Index2 < ArrayHdr->ItemCount; Index2++) {
            Offset2 = Index2 * ArrayHdr->ItemSize;
            if (GetArrayItemId (ArrayHdr, Cdata + GpioTableDataOffset + Offset2) == ItemId) {
              // Set item as valid in BaseTableBitMask
              ArrayHdr->BaseTableBitMask[Index >> 3] |= (1 << (Index & 7));
              CopyMem (Cdata + GpioTableDataOffset + Offset2, CdataCurr + GpioTableDataOffset + Offset1,  ArrayHdr->ItemSize);
              break;
            }
          }
        }
      } else {
        // Copy full CFGDATA tag data
        CopyMem (Cdata, CdataCurr, (CdataHdr->Length << 2) - HdrLen);
      }
    } else {
      // Set platform mask
      if (CdataHdr->Tag == CDATA_PLATFORMID_TAG) {
        PidCfg = (PLATFORMID_CFG_DATA *)((UINT8 *)CdataHdr + sizeof (CDATA_HEADER) + \
                 sizeof (CDATA_COND) * CdataHdr->ConditionNum);
        PidCfg->PlatformId = GetPlatformId ();
        DEBUG ((DEBUG_INFO, "PID: %d\n", GetPlatformId ()));
      } else {
        return EFI_NOT_FOUND;
      }
    }

    Offset += (CdataHdr->Length << 2);
  }

  return EFI_SUCCESS;
}
