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
  UINT64      VendorTable;
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
  Locate a string in the string-set of an SMBIOS structure.

  @param[in]  TypeHdr   SMBIOS structure header.
  @param[in]  StringId  1-based string index.
  @param[in]  Limit     First byte past the end of the table.

  @retval     Pointer to the string, or NULL if not found within Limit.
**/
STATIC
CHAR8 *
GetSmbiosStringById (
  IN  SMBIOS_STRUCTURE  *TypeHdr,
  IN  UINT8              StringId,
  IN  CONST UINT8       *Limit
  )
{
  CHAR8   *String;
  UINT8    Index;

  String = (CHAR8 *)TypeHdr + TypeHdr->Length;
  for (Index = 1; ((UINT8 *)String < Limit) && (*String != 0); Index++) {
    if (Index == StringId) {
      return String;
    }
    while (((UINT8 *)String < Limit) && (*String != 0)) {
      String++;
    }
    String++;
  }

  return NULL;
}

/**
  Blank the filtered fields in a copy of the SMBIOS structure table.

  @param[in, out]  Table        Copy of the SMBIOS structure table.
  @param[in]       TableLength  Length of the table in bytes.
**/
STATIC
VOID
FilterSmbiosTable (
  IN OUT  UINT8    *Table,
  IN      UINT32    TableLength
  )
{
  SMBIOS_STRUCTURE   *TypeHdr;
  CONST UINT8        *Limit;
  UINT8              *Field;
  UINT8              *StringPtr;
  CHAR8              *String;
  UINTN               Index;

  Limit   = Table + TableLength;
  TypeHdr = (SMBIOS_STRUCTURE *)Table;

  while (((UINT8 *)TypeHdr + sizeof (SMBIOS_STRUCTURE) <= Limit) &&
         ((UINT8 *)TypeHdr + TypeHdr->Length <= Limit)) {
    for (Index = 0; Index < ARRAY_SIZE (mSmbiosMeasureFilter); Index++) {
      if ((mSmbiosMeasureFilter[Index].Type != TypeHdr->Type) ||
          (mSmbiosMeasureFilter[Index].Offset + mSmbiosMeasureFilter[Index].Size > TypeHdr->Length)) {
        continue;
      }

      Field = (UINT8 *)TypeHdr + mSmbiosMeasureFilter[Index].Offset;
      if (((mSmbiosMeasureFilter[Index].Flags & FILTER_FLAG_IS_STRING) != 0) && (*Field != 0)) {
        String = GetSmbiosStringById (TypeHdr, *Field, Limit);
        if (String != NULL) {
          SetMem (String, AsciiStrnLenS (String, (UINTN)(Limit - (UINT8 *)String)), ' ');
        }
      }
      ZeroMem (Field, mSmbiosMeasureFilter[Index].Size);
    }

    if (TypeHdr->Type == SMBIOS_TYPE_END_OF_TABLE) {
      break;
    }

    StringPtr = (UINT8 *)TypeHdr + TypeHdr->Length;
    while ((StringPtr + 1 < Limit) && !((StringPtr[0] == 0) && (StringPtr[1] == 0))) {
      StringPtr++;
    }
    TypeHdr = (SMBIOS_STRUCTURE *)(StringPtr + TYPE_TERMINATOR_SIZE);
  }
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
  EFI_STATUS                        Status;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if ((SmbiosEntry == NULL) || (SmbiosEntry->TableAddress == 0) || (SmbiosEntry->TableLength == 0)) {
    return EFI_NOT_FOUND;
  }

  TableCopy = AllocateTemporaryMemory (SmbiosEntry->TableLength);
  if (TableCopy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (TableCopy, (VOID *)(UINTN)SmbiosEntry->TableAddress, SmbiosEntry->TableLength);
  FilterSmbiosTable (TableCopy, SmbiosEntry->TableLength);

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
             SmbiosEntry->TableLength,
             EV_EFI_HANDOFF_TABLES2,
             sizeof (HandoffTables),
             (UINT8 *)&HandoffTables
             );

  FreeTemporaryMemory (TableCopy);

  return Status;
}
