/** @file

  Copyright (c) 2017 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <IndustryStandard/SmBios.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/SmbiosInitLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/TimeStampLib.h>
#include <Library/MpInitLib.h>
#include <Library/ContainerLib.h>
#include <Library/PrintLib.h>
#include <Guid/BootLoaderVersionGuid.h>
#include "SmbiosTables.h"


GLOBAL_REMOVE_IF_UNREFERENCED PROCESSOR_FAMILY_FIELD mProcessorFamilyField[] = {
  { "Core(TM) i9",  ProcessorFamilyIntelCoreI9 },
  { "Core(TM) i7",  ProcessorFamilyIntelCoreI7 },
  { "Core(TM) i5",  ProcessorFamilyIntelCoreI5 },
  { "Core(TM) i3",  ProcessorFamilyIntelCoreI3 },
  { "Core(TM) m3",  ProcessorFamilyIntelCorem3 },
  { "Core(TM) m5",  ProcessorFamilyIntelCorem5 },
  { "Core(TM) m7",  ProcessorFamilyIntelCorem7 },
  { "Core(TM) M",   ProcessorFamilyIntelCoreM },
  { "Pentium(R)",   ProcessorFamilyPentium },
  { "Celeron(R)",   ProcessorFamilyCeleron },
  { "Atom(TM)",     ProcessorFamilyIntelAtom },
  { "Xeon(R)",      ProcessorFamilyIntelXeon },
};


/**
  Check if the Smbios types (including the entry point struct)
  have crossed the statically allocated size for Smbios init

  @param[in]  NewLength   New TypeLength to be checked for overflow if added

  @retval                 EFI_DEVICE_ERROR    , if Smbios Entry is NULL
                          EFI_BUFFER_TOO_SMALL, if new taype to be added exceeds allocated memory
                          EFI_SUCCESS         , if new type can be added without mem overflow

**/
EFI_STATUS
EFIAPI
CheckSmbiosOverflow (
  IN  UINT16    HdrLen,
  IN  UINT8     NumStr
  )
{
  SMBIOS_TABLE_ENTRY_POINT     *SmbiosEntry;
  UINT16                        NewLength;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL) {
    return EFI_DEVICE_ERROR;
  }

  if (NumStr > 0) {
    NewLength = HdrLen + (SMBIOS_STRING_MAX_LENGTH + 1) * NumStr + sizeof (CHAR8);
  } else {
    NewLength = HdrLen + 2 * sizeof (CHAR8);
  }

  if ( (UINT32)(SmbiosEntry->EntryPointLength + sizeof (UINT8) + SmbiosEntry->TableLength + NewLength) > (UINT32)PcdGet16(PcdSmbiosTablesSize) ) {
    return EFI_BUFFER_TOO_SMALL;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Return the pointer to the last Smbios table spcified by 'Type'

  @param[in]  Type    Smbios type requested

  @retval             Pointer to the starting of the requested Smbios type
                      NULL, otherwise

**/
VOID *
EFIAPI
FindSmbiosType (
  IN  UINT8   Type
  )
{
  SMBIOS_STRUCTURE              *TypeHdr;
  SMBIOS_STRUCTURE              *LastTypeHdr;
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntry;
  UINT8                         *StringPtr;
  UINT32                         CurLimit;

  LastTypeHdr = NULL;
  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL) {
    return NULL;
  }

  TypeHdr  = (SMBIOS_STRUCTURE *) (UINTN) SmbiosEntry->TableAddress;
  CurLimit = (UINT32) (UINTN) ((UINT8 *) SmbiosEntry + SmbiosEntry->EntryPointLength + sizeof (UINT8) + SmbiosEntry->TableLength);

  while ( (UINT32)(UINTN)TypeHdr < CurLimit ) {
    if (TypeHdr->Type == Type) {
      LastTypeHdr = TypeHdr;
    }
    //
    // Go to the end of strings to find next Type header
    //
    StringPtr = (UINT8 *) TypeHdr + TypeHdr->Length;
    while ( !(StringPtr[0] == 0 && StringPtr[1] == 0) ) {
      StringPtr++;
    }
    TypeHdr = (SMBIOS_STRUCTURE *)(StringPtr + TYPE_TERMINATOR_SIZE);
  }

  return LastTypeHdr;
}

/**
  After adding and appending all Smbios Types, we need to do the following:
    1. Check for the table overflow
    2. Add Type 127
    3. Adjust the TotalLength in entry point struct
    4. Update entry point & intermediate checksum

  @param[in]  NewMaxStructSize    Maximum type size of all the types added    , if called from SmbiosInit
                                  Type Length of the current table appended   , if called from AppendSmbiosType

  @retval                         EFI_DEVICE_ERROR, if Smbios Entry is NULL
                                  Overflow status , otherwise

**/
EFI_STATUS
FinalizeSmbios (
  VOID
  )
{
  SMBIOS_TABLE_TYPE127          *Type127;
  EFI_STATUS                    Status;
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntry;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Check for overflows before adding Type127
  //
  Status = CheckSmbiosOverflow (sizeof (SMBIOS_STRUCTURE), 0);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Create Type127 and update global Type127 for the next append
  //
  Type127 = (SMBIOS_TABLE_TYPE127 *)(UINTN)(SmbiosEntry->TableAddress + SmbiosEntry->TableLength);
  Type127->Hdr.Type = SMBIOS_TYPE_END_OF_TABLE;
  Type127->Hdr.Length = sizeof (SMBIOS_STRUCTURE);
  Type127->Hdr.Handle = SmbiosEntry->NumberOfSmbiosStructures++;

  //
  // Patch entry point struct with new data
  //
  SmbiosEntry->TableLength                 += sizeof (SMBIOS_STRUCTURE) + TYPE_TERMINATOR_SIZE;
  SmbiosEntry->IntermediateChecksum         = 0;
  SmbiosEntry->EntryPointStructureChecksum  = 0;
  SmbiosEntry->IntermediateChecksum         = CalculateCheckSum8 ((UINT8 *)SmbiosEntry + 0x10, sizeof (SMBIOS_TABLE_ENTRY_POINT) - 0x10);
  SmbiosEntry->EntryPointStructureChecksum  = CalculateCheckSum8 ((UINT8 *)SmbiosEntry       , sizeof (SMBIOS_TABLE_ENTRY_POINT)       );

  //
  // Keep a copy in legacy F segment so that non-UEFI can locate it
  //
  if (FeaturePcdGet (PcdLegacyEfSegmentEnabled)) {
    CopyMem ((VOID *)0xFFF60, SmbiosEntry, sizeof (SMBIOS_TABLE_ENTRY_POINT));
  }

  return Status;
}

/**
  Calculate number of strings in a type.

  @param[in]  Type      Get the num of strings for a Type

  @retval               Number of strings for a Type, if given via PCd structure
                                            assume 0, otherwise
**/
UINT8
CalculateNumStrInType (
  IN  UINT8   Type
)
{
  UINT8                   Index;
  UINT8                   Count;
  SMBIOS_TYPE_STRINGS    *SmbiosStrings;
  CHAR8                  *StringPtr;
  UINT8                   UniqueIdx[32] = {0};

  Count = 0;
  SmbiosStrings = (SMBIOS_TYPE_STRINGS *) (UINTN) PcdGet32 (PcdSmbiosStringsPtr);
  if (SmbiosStrings != NULL) {
    for (Index = 0; Index < PcdGet16 (PcdSmbiosStringsCnt); Index++) {
      StringPtr = (CHAR8 *)SmbiosStrings + sizeof(SmbiosStrings->Type) + sizeof (SmbiosStrings->Idx);
      if (SmbiosStrings->Type == Type && SmbiosStrings->Idx != 0) {
        if (UniqueIdx[SmbiosStrings->Idx] == 0) {
          Count++;
        }
      }
      // Move to next entry
      SmbiosStrings = (SMBIOS_TYPE_STRINGS *)(StringPtr + AsciiStrSize (StringPtr));
      if (SmbiosStrings->Type == SMBIOS_TYPE_END_OF_TABLE) {
        break;
      }
    }
  }

  return Count;
}


/**
  Dump all SMBIOS strings from the SMBIOS string table.

  This function iterates through the smbios table pointed to by PcdSmbiosStringsPtr,
  and prints each entry's type, string ID, and string value for debugging purposes.
  The function stops when it encounters the SMBIOS_TYPE_END_OF_TABLE marker.
**/
VOID
DumpSmbiosStrings (
  VOID
)
{
  UINT8                   Index;
  SMBIOS_TYPE_STRINGS    *SmbiosStrings;
  CHAR8                  *StringPtr;

  DEBUG ((DEBUG_INFO, "Print the SMBIOS strings list to update SMBIOS table\n"));
  SmbiosStrings = (SMBIOS_TYPE_STRINGS *) (UINTN) PcdGet32 (PcdSmbiosStringsPtr);
  if (SmbiosStrings != NULL) {
    for (Index = 0; Index < PcdGet16 (PcdSmbiosStringsCnt); Index++) {
      StringPtr = (CHAR8 *)SmbiosStrings + sizeof(SmbiosStrings->Type) + sizeof (SmbiosStrings->Idx);
      DEBUG ((DEBUG_INFO, "%02d: Type: %d,Index:0x%d, String = %a\n", Index, SmbiosStrings->Type, SmbiosStrings->Idx, StringPtr));
      SmbiosStrings = (SMBIOS_TYPE_STRINGS *)(StringPtr + AsciiStrSize (StringPtr));
      if (SmbiosStrings->Type == SMBIOS_TYPE_END_OF_TABLE) {
        break;
      }
    }
  }
}

/**
  Dump all SMBIOS structures and their associated strings from the SMBIOS table.

  This function iterates through the SMBIOS table pointed to by PcdSmbiosTablesBase,
  printing each structure's type, length, and all associated strings for debugging purposes.
  It advances through the table by skipping the formatted section and traversing the string area
  until the double-null terminator is found, then moves to the next structure.
**/
VOID
DumpSmbiosTable (
  VOID
)
{
  SMBIOS_STRUCTURE              *TypeHdr;
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntry;
  CHAR8                         *StringPtr;
  UINT32                         CurLimit;
  UINT32                         Idx;
  UINT8                          StringIdx;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL) {
    return;
  }

  TypeHdr  = (SMBIOS_STRUCTURE *) (UINTN) SmbiosEntry->TableAddress;
  CurLimit = (UINT32) (UINTN) (SmbiosEntry->TableAddress + SmbiosEntry->TableLength);
  DEBUG ((DEBUG_INFO, "\n Dumping SMBIOS table strings, base=0x%p, len=0x%x, CurLimit=0x%x\n", TypeHdr, SmbiosEntry->TableLength, CurLimit));

  Idx = 0;
  while ((UINT32)(UINTN)TypeHdr < CurLimit) {
    DEBUG ((DEBUG_INFO, "Checking table 0x%p: Type: %d, Length: 0x%x\n", TypeHdr, TypeHdr->Type, TypeHdr->Length));

    // Dump all strings for this SMBIOS structure
    StringPtr = (CHAR8 *)TypeHdr + TypeHdr->Length;
    StringIdx = 1;
    while (!(StringPtr[0] == 0 && StringPtr[1] == 0)) {
      if (StringPtr[0] != 0) {
        DEBUG ((DEBUG_INFO, "  0x%p: String %d: %a\n", StringPtr, StringIdx, StringPtr));
        StringIdx++;
      }
      // Move to next string (null-terminated)
      while (*StringPtr != 0) {
        StringPtr++;
      }

      if (StringPtr[1] == 0) break;
      StringPtr++; // Skip null terminator
    }

    // Advance to next SMBIOS structure
    TypeHdr = (SMBIOS_STRUCTURE *)(StringPtr + TYPE_TERMINATOR_SIZE); // Skip double null
    Idx++;
  }
}

/**
  Get the value a particular string in a Type
  from data structure pointed to by PcdSmbiosStringsPtr

  @param[in]  Type      Get the string for a Type
  @param[in]  StringId  String Num

  @retval               CHAR8 * to the string, if found
                        SMBIOS_STRING_UNKNOWN            , otherwise

**/
CHAR8 *
GetSmbiosString (
  IN  UINT8   Type,
  IN  UINT8   StringId
  )
{
  SMBIOS_TYPE_STRINGS       *SmbiosStrings;
  UINT16                    Index;
  CHAR8                     *StringPtr;
  CHAR8                     *StringFound;

  if ((SMBIOS_TYPE_STRINGS *)(UINTN)PcdGet32 (PcdSmbiosStringsPtr) == NULL) {
    return SMBIOS_STRING_UNKNOWN;
  }

  // Search PcdSmbiosStringsPtr, if multiple strings are found, use the last occurrence
  StringFound   = SMBIOS_STRING_UNKNOWN;
  SmbiosStrings = (SMBIOS_TYPE_STRINGS *)(UINTN)PcdGet32 (PcdSmbiosStringsPtr);

  for (Index = 0; Index < PcdGet16 (PcdSmbiosStringsCnt); Index++) {
    StringPtr = (CHAR8 *)SmbiosStrings + sizeof(SmbiosStrings->Type) + sizeof (SmbiosStrings->Idx);
    if (Type == SmbiosStrings->Type && StringId == SmbiosStrings->Idx) {
      StringFound = StringPtr;
    }
    SmbiosStrings = (SMBIOS_TYPE_STRINGS *)(StringPtr + AsciiStrSize (StringPtr));
    if (SmbiosStrings->Type == SMBIOS_TYPE_END_OF_TABLE) {
      break;
    }
  }

  return StringFound;
}

/**
  Add the string to an Smbios type

  @param[in]  Destination     Address where the string gets added inside a type
  @param[in]  Source          CHAR8 * to the actual string

  @retval                     Destination for the next string to be added

**/
CHAR8 *
CopySmbiosString (
  IN  CHAR8   *Destination,
  IN  CHAR8   *Source
  )
{
  if (Source == NULL) {
    return Destination;
  }

  CopyMem ((CHAR8 *)Destination, Source, AsciiStrLen (Source));
  Destination += AsciiStrSize (Source); // every string terminates with a 00

  return Destination;
}

/**
  Append a string to an Smbios type header

  @param[in]  Type      Type to which a string is appended
  @param[in]  String    String literal to be appended

  @retval               EFI_SUCCESS, if string is appended successfully
                        EFI_ERROR,    otherwise

**/
EFI_STATUS
EFIAPI
AddSmbiosString (
  IN  UINT8     Type,
  IN  CHAR8    *String
  )
{
  SMBIOS_TABLE_ENTRY_POINT     *SmbiosEntry;
  SMBIOS_STRUCTURE             *TypeHdr;
  CHAR8                        *StringPtr;
  UINTN                         StrLen;
  UINT16                        TypeLength;
  EFI_STATUS                    Status;
  BOOLEAN                       StrPresent;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *) (UINTN) PcdGet32 (PcdSmbiosTablesBase);
  StrPresent  = FALSE;

  //
  // Find the header to append a string
  //
  TypeHdr = (SMBIOS_STRUCTURE *) FindSmbiosType (Type);
  if (TypeHdr == NULL) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Check for potential overflow if string is added
  //
  Status = CheckSmbiosOverflow (0, 1);
  if (EFI_ERROR(Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Add 'Unknown' if empty string
  //
  if (*String == '\0') {
    String = SMBIOS_STRING_UNKNOWN;
  }

  StringPtr = (CHAR8 *) TypeHdr + TypeHdr->Length;
  // Find end of an existing string
  while ( !(StringPtr[0] == 0 && StringPtr[1] == 0) ) {
    StrPresent = TRUE;
    StringPtr++;
  }
  if (StrPresent == TRUE) {
    *StringPtr++ = 0; // Leave a 00 between strings
  }
  StringPtr = CopySmbiosString (StringPtr, String);
  *StringPtr++ = 0; // add another 00 to the string

  //
  // Update TypeLength, TableLength(in entry point), Max Length
  //
  StrLen = AsciiStrLen (String);
  if (StrPresent == TRUE) {
    SmbiosEntry->TableLength += (UINT16) (StrLen + 1);
  } else {
    SmbiosEntry->TableLength += (UINT16) StrLen;
  }
  TypeLength = (UINT16)((UINTN)(StringPtr) - (UINTN)TypeHdr);
  if (TypeLength > SmbiosEntry->MaxStructureSize) {
    SmbiosEntry->MaxStructureSize = TypeLength;
  }

  return EFI_SUCCESS;
}

/**
  Add a particular Smbios type to the Smbios allocated region.

  @param[in]  HdrInfo     Address of the type being added

  @retval                 EFI_SUCCESS, if Type added successfully,
                          EFI_ERROR,   otherwise

**/
EFI_STATUS
EFIAPI
AddSmbiosType (
  IN  VOID   *HdrInfo
  )
{
  SMBIOS_TABLE_ENTRY_POINT     *SmbiosEntry;
  UINT16                        TypeLength;
  CHAR8                        *StringPtr;
  UINT8                         StrIdx;
  UINT8                         NumStr;
  EFI_STATUS                    Status;
  SMBIOS_STRUCTURE             *TypeHdr;
  UINT16                        HdrLen;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *) (UINTN) PcdGet32 (PcdSmbiosTablesBase);
  TypeHdr     = (SMBIOS_STRUCTURE *) HdrInfo;

  if (TypeHdr == NULL) {
    return EFI_DEVICE_ERROR;
  } else {
    HdrLen = TypeHdr->Length;
    if (HdrLen == 0) {
      return EFI_DEVICE_ERROR;
    }
  }

  NumStr = CalculateNumStrInType (TypeHdr->Type);

  //
  // Check for overflow before adding the Type
  //
  Status = CheckSmbiosOverflow (HdrLen, NumStr);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (FindSmbiosType (TypeHdr->Type) != NULL) {
    // A Previous instance of same type is already existing.
    // Dont add those strings here. Expect new ones to be added
    // via AddSmbiosString ()
    NumStr = 0;
  }

  //
  // Copy header, init string ptr, add strings
  //
  TypeHdr = (VOID *) (UINTN) (SmbiosEntry->TableAddress + SmbiosEntry->TableLength);
  CopyMem (TypeHdr, HdrInfo, HdrLen);
  StringPtr = (CHAR8 *) TypeHdr + HdrLen;
  if (NumStr > 0) {
    for (StrIdx = 1; StrIdx <= NumStr; ++StrIdx) {
      StringPtr = CopySmbiosString (StringPtr, GetSmbiosString (TypeHdr->Type, StrIdx));
    }
  } else {
    *StringPtr++ = 0; // Add string terminator
  }
  *StringPtr++ = 0; // Add string terminator

  //
  // Update TypeLength, TableLength(in entry point), Max Length
  //
  TypeLength = (UINT16)((UINTN)(StringPtr) - (UINTN)TypeHdr);
  SmbiosEntry->TableLength += TypeLength;
  if (TypeLength > SmbiosEntry->MaxStructureSize) {
    SmbiosEntry->MaxStructureSize = TypeLength;
  }

  //
  // According to SMBIOS spec, Handle field is used to
  // get a particular smbios type structure.
  //
  TypeHdr->Length = (UINT8) HdrLen;
  TypeHdr->Handle = SmbiosEntry->NumberOfSmbiosStructures++;

  return Status;
}


/**
  This function builds required processor info SMBIOS type.
**/
EFI_STATUS
BuildProcessorInfo (
  VOID
  )
{
  SMBIOS_TABLE_TYPE4              SmbiosRecord;
  UINT32                          Eax01;
  UINT32                          Ebx01;
  UINT32                          Ecx01;
  UINT32                          Edx01;
  PROCESSOR_ID_DATA               ProcessorId;
  UINT32                          ProcessorVoltage;
  UINTN                           NumberOfThreads;
  UINTN                           NumberOfCores;
  EFI_STATUS                      Status;
  SYS_CPU_INFO                   *SysCpuInfo;
  UINTN                           Idx;
  UINTN                           BufIdx;
  CHAR8                           CpuBrandBuf[64];
  BOOLEAN                         HyperThreadEnable;
  UINT16                          Family;
  UINT16                          BclkFrequency;

  for (Idx = 0, BufIdx = 0; Idx < 3; Idx++) {
    AsmCpuid (
              (UINT32)(CPUID_BRAND_STRING1 + Idx),
              (UINT32 *)&CpuBrandBuf[BufIdx],
              (UINT32 *)&CpuBrandBuf[BufIdx+4],
              (UINT32 *)&CpuBrandBuf[BufIdx+8],
              (UINT32 *)&CpuBrandBuf[BufIdx+12]
             );
    BufIdx += 16;
  }
  if (CpuBrandBuf[0] == 0) {
    AsciiStrCpyS (CpuBrandBuf, sizeof(CpuBrandBuf), BRAND_STRING_UNSUPPORTED);
  }
  CpuBrandBuf[48] = 0;

  Family = ProcessorFamilyIntelCoreM;
  for (Idx = 0; Idx < ARRAY_SIZE(mProcessorFamilyField); Idx++) {
    if (AsciiStrStr (CpuBrandBuf, mProcessorFamilyField[Idx].String) != NULL) {
      Family = mProcessorFamilyField[Idx].Family;
      break;
    }
  }

  ZeroMem (&SmbiosRecord, sizeof(SmbiosRecord));
  SmbiosRecord.Hdr.Type   = SMBIOS_TYPE_PROCESSOR_INFORMATION;
  SmbiosRecord.Hdr.Length = sizeof (SMBIOS_TABLE_TYPE4);

  // Fill string index
  SmbiosRecord.Socket = SMBIOS_STRING_INDEX_1;
  SmbiosRecord.ProcessorManufacturer = SMBIOS_STRING_INDEX_2;
  SmbiosRecord.ProcessorVersion = SMBIOS_STRING_INDEX_3;
  SmbiosRecord.SerialNumber = SMBIOS_STRING_INDEX_4;
  SmbiosRecord.AssetTag = SMBIOS_STRING_INDEX_5;
  SmbiosRecord.PartNumber = SMBIOS_STRING_INDEX_6;

  // Processor Type, Family, ID
  BclkFrequency = 100;
  SmbiosRecord.ProcessorType = EfiCentralProcessor;
  SmbiosRecord.ProcessorFamily = (Family > 0xFD) ? 0xFE : (UINT8)Family;
  SmbiosRecord.ExternalClock   = BclkFrequency;
  ZeroMem (&ProcessorId, sizeof(ProcessorId));
  AsmCpuid(CPUID_VERSION_INFO, &Eax01, &Ebx01, &Ecx01, &Edx01);
  ProcessorId.Signature = *(PROCESSOR_SIGNATURE *)&Eax01;
  ProcessorId.FeatureFlags = *(PROCESSOR_FEATURE_FLAGS *)&Edx01;
  CopyMem (&SmbiosRecord.ProcessorId, &ProcessorId, sizeof(PROCESSOR_ID_DATA));

  // Processor Voltage
  ProcessorVoltage = (BIT7 | 9);
  SmbiosRecord.Voltage = *((PROCESSOR_VOLTAGE *)&ProcessorVoltage);

  // Status, assume all good
  SmbiosRecord.Status = 0x41;

  // Processor Upgrade
  SmbiosRecord.ProcessorUpgrade = 0x08;

  // Processor Family 2
  SmbiosRecord.ProcessorFamily2 = Family;

  // Processor speed in MHz
  SmbiosRecord.CurrentSpeed = (UINT16) DivU64x32 (GetTimeStampFrequency () + 999, 1000);
  SmbiosRecord.MaxSpeed = SmbiosRecord.CurrentSpeed;

  HyperThreadEnable = FALSE;
  SysCpuInfo = MpGetInfo ();
  NumberOfThreads = SysCpuInfo->CpuCount;
  for (Idx = 0; Idx < NumberOfThreads; Idx++) {
    if ((SysCpuInfo->CpuInfo[Idx].ApicId & BIT0) != 0) {
      HyperThreadEnable = TRUE;
      break;
    }
  }
  NumberOfCores = HyperThreadEnable ? NumberOfThreads >> 1 : NumberOfThreads;

  SmbiosRecord.CoreCount  = (NumberOfCores > 255) ? 0xFF : (UINT8)NumberOfCores;
  SmbiosRecord.CoreCount2 = (UINT16)NumberOfCores;
  SmbiosRecord.EnabledCoreCount  = (NumberOfCores > 255) ? 0xFF : (UINT8)NumberOfCores;
  SmbiosRecord.EnabledCoreCount2 = (UINT16)NumberOfCores;
  SmbiosRecord.ThreadCount  = (NumberOfThreads > 255) ? 0xFF : (UINT8)NumberOfThreads;
  SmbiosRecord.ThreadCount2 = (UINT16)NumberOfThreads;
  // Unknown
  SmbiosRecord.ProcessorCharacteristics = 0x2;

  //
  // Updating Cache Handle Information
  //
  SmbiosRecord.L1CacheHandle  = 0xFFFF;
  SmbiosRecord.L2CacheHandle  = 0xFFFF;
  SmbiosRecord.L3CacheHandle  = 0xFFFF;

  // Add SMBIOS type
  Status = AddSmbiosType (&SmbiosRecord);

  //
  // SMBIOS_TYPE_PROCESSOR_INFORMATION
  // Socket
  AddSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION, "Socket0");
  // ProcessorManufacturer
  AddSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION, "Intel");
  // ProcessorVersion
  AddSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION, CpuBrandBuf);
  // SerialNumber
  AddSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION,
                  GetSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION, SMBIOS_STRING_INDEX_4));
  // AssetTag
  AddSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION,
                  GetSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION, SMBIOS_STRING_INDEX_5));
  // PartNumber
  AddSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION,
                  GetSmbiosString (SMBIOS_TYPE_PROCESSOR_INFORMATION, SMBIOS_STRING_INDEX_6));

  return Status;
}


/**
  Add a SMBIOS string entry to the destination buffer.

  @param[out]     Dest   Pointer to the destination buffer where the entry will be added.
  @param[in]      Type   SMBIOS type for the entry.
  @param[in]      Index  String index for the entry.
  @param[in]      String Null-terminated ASCII string to store.

  @retval Pointer to the next available position in the buffer.
**/
CHAR8 *
EFIAPI
AddSmbiosTypeString (
  OUT    CHAR8                 *Dest,
  IN     UINT8                 Type,
  IN     UINT8                 Index,
  IN     CHAR8                 *String
  )
{
  SMBIOS_TYPE_STRINGS  *SmbiosStrings;
  UINTN                Len;

  SmbiosStrings       = (SMBIOS_TYPE_STRINGS *)Dest;
  SmbiosStrings->Type = Type;
  SmbiosStrings->Idx  = Index;
  if (String != NULL) {
    Len = AsciiStrSize (String);
    CopyMem (Dest + OFFSET_OF(SMBIOS_TYPE_STRINGS, String), String, Len);
  } else {
    ASSERT (SmbiosStrings->Type == SMBIOS_TYPE_END_OF_TABLE);
    (Dest + OFFSET_OF(SMBIOS_TYPE_STRINGS, String))[0] = 0;
    (Dest + OFFSET_OF(SMBIOS_TYPE_STRINGS, String))[1] = 0;
    Len = TYPE_TERMINATOR_SIZE;
  }

  return Dest + sizeof (SmbiosStrings->Type) + sizeof (SmbiosStrings->Idx) + Len;
}


/**
  Load and initialize customized SMBIOS string data.

  This function allocates a buffer for SMBIOS strings, populates it with BIOS and baseboard information,
  and loads additional SMBIOS string data from a specified component. The buffer pointer is stored in a PCD
  for later use. The function ensures that the buffer is not overrun when copying additional data.

  @param[in]  ContainerSig   Signature identifying the container to load SMBIOS data from.
  @param[in]  ComponentName  Name of the component containing SMBIOS string data.

  @retval EFI_SUCCESS            The SMBIOS string data was loaded and initialized successfully.
  @retval EFI_OUT_OF_RESOURCES   Memory allocation for the SMBIOS string table failed.
  @retval Other                  Error returned by LoadComponent if loading SMBIOS data fails.
**/
EFI_STATUS
EFIAPI
LoadSmbiosStringsFromComponent (
  IN  UINT32    ContainerSig,
  IN  UINT32    ComponentName
  )
{
  EFI_STATUS            Status;
  CHAR8                 *SmbiosStrTbl;
  CHAR8                 *SmbiosData;
  UINT32                SmbiosDataSize;
  BOOT_LOADER_VERSION   *VerInfoTbl;
  CHAR8                 TempStrBuf[SMBIOS_STRING_MAX_LENGTH];

  SmbiosStrTbl = AllocateZeroPool (SIZE_4KB);
  if (SmbiosStrTbl == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  (VOID) PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStrTbl);

  VerInfoTbl = GetVerInfoPtr ();
  if (VerInfoTbl != NULL) {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf),
      "SBL:%03d.%03d.%03d.%03d.%03d.%05d.%c-%016lX%a\0",
      VerInfoTbl->ImageVersion.SecureVerNum,
      VerInfoTbl->ImageVersion.CoreMajorVersion,
      VerInfoTbl->ImageVersion.CoreMinorVersion,
      VerInfoTbl->ImageVersion.ProjMajorVersion,
      VerInfoTbl->ImageVersion.ProjMinorVersion,
      VerInfoTbl->ImageVersion.BuildNumber,
      VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R',
      VerInfoTbl->SourceVersion,
      VerInfoTbl->ImageVersion.Dirty ? "-dirty" : "");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "SBL version Unknown");
  }
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 2, TempStrBuf);

  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BIOS_INFORMATION, 3, __DATE__);

  AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%8a (ID:%02X)", GetPlatformName(), GetPlatformId ());
  SmbiosStrTbl = AddSmbiosTypeString (SmbiosStrTbl, SMBIOS_TYPE_BASEBOARD_INFORMATION, 2, TempStrBuf);

  SmbiosData     = NULL;
  SmbiosDataSize = 0;
  Status = LoadComponent (ContainerSig, ComponentName, (VOID **)&SmbiosData, &SmbiosDataSize);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if ((((UINT32)(UINTN)SmbiosStrTbl - PcdGet32(PcdSmbiosStringsPtr)) + SmbiosDataSize) < SIZE_4KB) {
    CopyMem (SmbiosStrTbl, SmbiosData, SmbiosDataSize);
  }

  return EFI_SUCCESS;
}


/**
  This function is called to initialize the SMBIOS tables.

  @retval       EFI_DEVICE_ERROR, if Smbios Entry is NULL
                Status after finalizing the Smbios init

**/
EFI_STATUS
EFIAPI
SmbiosInit (
  VOID
  )
{
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntryPoint;
  EFI_STATUS                    Status;

  //
  // Create Entry Point structure
  //
  SmbiosEntryPoint = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntryPoint == NULL) {
    DEBUG ((DEBUG_INFO, "Memory not allocated for SMBIOS tables"));
    return EFI_DEVICE_ERROR;
  }

  *((UINT32 *)&(SmbiosEntryPoint->AnchorString))              = SIGNATURE_32('_', 'S', 'M', '_');
  SmbiosEntryPoint->EntryPointLength                          = sizeof (SMBIOS_TABLE_ENTRY_POINT);
  SmbiosEntryPoint->MajorVersion                              = 3;
  SmbiosEntryPoint->MinorVersion                              = 3;
  SmbiosEntryPoint->MaxStructureSize                          = 0;
  *((UINT32 *)&(SmbiosEntryPoint->IntermediateAnchorString))  = SIGNATURE_32('_', 'D', 'M', 'I');
  SmbiosEntryPoint->IntermediateAnchorString[4]               = '_';
  SmbiosEntryPoint->TableLength                               = 0;
  SmbiosEntryPoint->TableAddress                              = (UINT32)(UINTN)SmbiosEntryPoint + sizeof (SMBIOS_TABLE_ENTRY_POINT) + sizeof (UINT8);
  SmbiosEntryPoint->NumberOfSmbiosStructures                  = 0;

  //
  // Patch common Type headers if necessary
  //
  mMemArrayMappedAddr.ExtendedEndingAddress = GetMemoryInfo (EnumMemInfoTom) - 1;

  //
  // Add common SMBIOS Types' information.
  // Types start at 16 byte boundary
  //
  Status  = AddSmbiosType (&mBiosInfo);
  Status |= AddSmbiosType (&mSystemInfo);
  Status |= AddSmbiosType (&mBaseBoardInfo);
  Status |= AddSmbiosType (&mDefaultChasisInfo);
  Status |= BuildProcessorInfo ();
  Status |= AddSmbiosType (&mMemArrayMappedAddr);

  if (Status != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}
