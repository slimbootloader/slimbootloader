/** @file
  Measure the SMBIOS tables into TPM PCR[1].

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/TpmLib.h>
#include <Library/SmbiosInitLib.h>
#include "SmbiosTables.h"

#define SMBIOS_HANDOFF_TABLE_DESC        "SmbiosTable"
#define SMBIOS_MEASURE_PCR_INDEX         1
#define FILTER_FLAG_IS_STRING            BIT0
#define SMBIOS_FIELD_SIZE(TYPE, Field)   sizeof (((TYPE *)0)->Field)
#define SMBIOS_EXCLUDED_STRING_CHAR      ' '
#define SMBIOS_STRING_BITMAP_SIZE        (256 / 8)

typedef struct {
  UINT8     Type;
  UINTN     Offset;
  UINTN     Size;
  UINT32    Flags;
} SMBIOS_FILTER_ENTRY;

#pragma pack(1)
typedef struct {
  UINT8       TableDescriptionSize;
  UINT8       TableDescription[sizeof (SMBIOS_HANDOFF_TABLE_DESC)];
  UINT64      NumberOfTables;
  EFI_GUID    VendorGuid;
  UINTN       VendorTable;
} SMBIOS_HANDOFF_TABLE_POINTERS2;
#pragma pack()

// Per-unit or per-boot fields excluded from the measurement (TCG PFP PCR[1] guidance).
STATIC CONST SMBIOS_FILTER_ENTRY  mSmbiosMeasureFilter[] = {
  { SMBIOS_TYPE_SYSTEM_INFORMATION,    OFFSET_OF (SMBIOS_TABLE_TYPE1,  SerialNumber),      SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE1,  SerialNumber),      FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_SYSTEM_INFORMATION,    OFFSET_OF (SMBIOS_TABLE_TYPE1,  Uuid),              SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE1,  Uuid),              0                     },
  { SMBIOS_TYPE_SYSTEM_INFORMATION,    OFFSET_OF (SMBIOS_TABLE_TYPE1,  WakeUpType),        SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE1,  WakeUpType),        0                     },
  { SMBIOS_TYPE_BASEBOARD_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE2,  SerialNumber),      SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE2,  SerialNumber),      FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_BASEBOARD_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE2,  AssetTag),          SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE2,  AssetTag),          FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_BASEBOARD_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE2,  LocationInChassis), SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE2,  LocationInChassis), FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_SYSTEM_ENCLOSURE,      OFFSET_OF (SMBIOS_TABLE_TYPE3,  SerialNumber),      SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE3,  SerialNumber),      FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_SYSTEM_ENCLOSURE,      OFFSET_OF (SMBIOS_TABLE_TYPE3,  AssetTag),          SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE3,  AssetTag),          FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  SerialNumber),      SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  SerialNumber),      FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  AssetTag),          SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  AssetTag),          FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  PartNumber),        SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  PartNumber),        FILTER_FLAG_IS_STRING },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  Voltage),           SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  Voltage),           0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  CoreCount),         SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  CoreCount),         0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  EnabledCoreCount),  SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  EnabledCoreCount),  0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  ThreadCount),       SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  ThreadCount),       0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  CoreCount2),        SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  CoreCount2),        0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  EnabledCoreCount2), SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  EnabledCoreCount2), 0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  ThreadCount2),      SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  ThreadCount2),      0                     },
  { SMBIOS_TYPE_PROCESSOR_INFORMATION, OFFSET_OF (SMBIOS_TABLE_TYPE4,  ThreadEnabled),     SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE4,  ThreadEnabled),     0                     },
  { SMBIOS_TYPE_TEMPERATURE_PROBE,     OFFSET_OF (SMBIOS_TABLE_TYPE28, NominalValue),      SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE28, NominalValue),      0                     },
};

/**
  Build the measurement image of the SMBIOS structure table: filtered fields
  are zeroed and each excluded string is replaced by a fixed one-character
  placeholder, so the image does not depend on excluded string values or lengths.

  The image is never longer than the source table.

  @param[in]   Table        SMBIOS structure table.
  @param[in]   TableLength  Length of the table in bytes.
  @param[out]  Normalized   Buffer of at least TableLength bytes.

  @retval      Length of the normalized image in bytes.
**/
STATIC
UINT32
NormalizeSmbiosTable (
  IN   CONST UINT8  *Table,
  IN   UINT32        TableLength,
  OUT  UINT8        *Normalized
  )
{
  CONST UINT8        *Limit;
  CONST UINT8        *Src;
  UINT8              *Dst;
  SMBIOS_STRUCTURE   *TypeHdr;
  UINT8              *Field;
  UINT8               Excluded[SMBIOS_STRING_BITMAP_SIZE];
  UINTN               StringLen;
  UINTN               Index;
  UINT8               StringId;
  UINT8               Type;

  Limit = Table + TableLength;
  Src   = Table;
  Dst   = Normalized;

  while ((Src + sizeof (SMBIOS_STRUCTURE) <= Limit) &&
         (((CONST SMBIOS_STRUCTURE *)Src)->Length >= sizeof (SMBIOS_STRUCTURE)) &&
         (Src + ((CONST SMBIOS_STRUCTURE *)Src)->Length <= Limit)) {
    TypeHdr = (SMBIOS_STRUCTURE *)Dst;
    CopyMem (Dst, Src, ((CONST SMBIOS_STRUCTURE *)Src)->Length);
    Type = TypeHdr->Type;
    ZeroMem (Excluded, sizeof (Excluded));

    if ((Type == SMBIOS_TYPE_TEMPERATURE_PROBE) &&
        (OFFSET_OF (SMBIOS_TABLE_TYPE28, LocationAndStatus) +
         SMBIOS_FIELD_SIZE (SMBIOS_TABLE_TYPE28, LocationAndStatus) <= TypeHdr->Length)) {
      ((SMBIOS_TABLE_TYPE28 *)TypeHdr)->LocationAndStatus.TemperatureProbeStatus = 0;
    }

    for (Index = 0; Index < ARRAY_SIZE (mSmbiosMeasureFilter); Index++) {
      if ((mSmbiosMeasureFilter[Index].Type != Type) ||
          (mSmbiosMeasureFilter[Index].Offset + mSmbiosMeasureFilter[Index].Size > TypeHdr->Length)) {
        continue;
      }

      Field = Dst + mSmbiosMeasureFilter[Index].Offset;
      if (((mSmbiosMeasureFilter[Index].Flags & FILTER_FLAG_IS_STRING) != 0) && (*Field != 0)) {
        Excluded[*Field >> 3] |= (UINT8)(1 << (*Field & 7));
      }
      ZeroMem (Field, mSmbiosMeasureFilter[Index].Size);
    }

    Src += TypeHdr->Length;
    Dst += TypeHdr->Length;

    if ((Src + 1 < Limit) && (Src[0] == 0) && (Src[1] == 0)) {
      // Structure without strings: keep the double-NUL terminator.
      Dst[0] = 0;
      Dst[1] = 0;
      Src   += 2;
      Dst   += 2;
    } else {
      StringId = 0;
      while ((Src < Limit) && (*Src != 0)) {
        StringLen = AsciiStrnLenS ((CONST CHAR8 *)Src, (UINTN)(Limit - Src));
        if (Src + StringLen >= Limit) {
          // Unterminated string: stop at the last complete structure data.
          return (UINT32)(Dst - Normalized);
        }

        StringId++;
        if ((Excluded[StringId >> 3] & (1 << (StringId & 7))) != 0) {
          *Dst++ = SMBIOS_EXCLUDED_STRING_CHAR;
        } else {
          CopyMem (Dst, Src, StringLen);
          Dst += StringLen;
        }
        *Dst++ = 0;
        Src   += StringLen + 1;
      }

      if (Src >= Limit) {
        break;
      }
      *Dst++ = 0;
      Src++;
    }

    if (Type == SMBIOS_TYPE_END_OF_TABLE) {
      break;
    }
  }

  return (UINT32)(Dst - Normalized);
}

/**
  Measure a filtered copy of the SMBIOS structure table into TPM PCR[1]
  and log it as an EV_EFI_HANDOFF_TABLES2 event.

  @retval EFI_SUCCESS            SMBIOS table measured and logged.
  @retval EFI_NOT_FOUND          SMBIOS table is not available.
  @retval EFI_OUT_OF_RESOURCES   Memory allocation failed.
  @retval Others                 TPM extend failed.
**/
EFI_STATUS
EFIAPI
MeasureSmbiosTables (
  VOID
  )
{
  SMBIOS_TABLE_ENTRY_POINT         *SmbiosEntry;
  SMBIOS_HANDOFF_TABLE_POINTERS2    HandoffTables;
  UINT8                            *TableCopy;
  UINT32                            TableCopyLength;
  EFI_STATUS                        Status;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if ((SmbiosEntry == NULL) || (SmbiosEntry->TableAddress == 0) || (SmbiosEntry->TableLength == 0)) {
    return EFI_NOT_FOUND;
  }

  TableCopy = AllocateTemporaryMemory (SmbiosEntry->TableLength);
  if (TableCopy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TableCopyLength = NormalizeSmbiosTable (
                      (CONST UINT8 *)(UINTN)SmbiosEntry->TableAddress,
                      SmbiosEntry->TableLength,
                      TableCopy
                      );
  if (TableCopyLength == 0) {
    FreeTemporaryMemory (TableCopy);
    return EFI_NOT_FOUND;
  }

  ZeroMem (&HandoffTables, sizeof (HandoffTables));
  HandoffTables.TableDescriptionSize = (UINT8)sizeof (SMBIOS_HANDOFF_TABLE_DESC);
  CopyMem (HandoffTables.TableDescription, SMBIOS_HANDOFF_TABLE_DESC, sizeof (SMBIOS_HANDOFF_TABLE_DESC));
  HandoffTables.NumberOfTables = 1;
  // Avoid taking the address of a packed (unaligned) member.
  CopyMem ((UINT8 *)&HandoffTables + OFFSET_OF (SMBIOS_HANDOFF_TABLE_POINTERS2, VendorGuid), &gEfiSmbiosTableGuid, sizeof (EFI_GUID));
  HandoffTables.VendorTable = (UINT64)(UINTN)SmbiosEntry;

  Status = TpmHashAndExtendPcrEventLog (
             SMBIOS_MEASURE_PCR_INDEX,
             TableCopy,
             TableCopyLength,
             EV_EFI_HANDOFF_TABLES2,
             sizeof (HandoffTables),
             (UINT8 *)&HandoffTables
             );

  FreeTemporaryMemory (TableCopy);

  return Status;
}
