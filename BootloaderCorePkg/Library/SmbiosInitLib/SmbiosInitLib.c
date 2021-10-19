/** @file

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
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
#include <Library/FspSupportLib.h>
#include "SmbiosTables.h"

VOID      *mType127Ptr            =   NULL;

#define SMBIOS_STRING_UNKNOWN              "Unknown"
#define SMBIOS_STRING_UNKNOWN_VERSION      "XXXX.XXX.XXX.XXX"
#define SMBIOS_STRING_VENDOR               "Intel Corporation"
#define SMBIOS_STRING_PLATFORM             "Client Platform"

//
// Add more platform specific strings
// in the following format
//
CONST SMBIOS_TYPE_STRINGS  mDefaultSmbiosStrings[] = {
  // Type                               StrId   String
  // Type 0
  {  SMBIOS_TYPE_BIOS_INFORMATION ,        1,  SMBIOS_STRING_VENDOR           },  // Vendor
  {  SMBIOS_TYPE_BIOS_INFORMATION ,        2,  SMBIOS_STRING_UNKNOWN_VERSION  },  // BIOS Version
  {  SMBIOS_TYPE_BIOS_INFORMATION ,        3,  SMBIOS_STRING_UNKNOWN          },  // BIOS Release Date
  // Type 1
  {  SMBIOS_TYPE_SYSTEM_INFORMATION ,      1,  SMBIOS_STRING_VENDOR           },  // Manufacturer
  {  SMBIOS_TYPE_SYSTEM_INFORMATION ,      2,  SMBIOS_STRING_UNKNOWN          },  // Product Name
  {  SMBIOS_TYPE_SYSTEM_INFORMATION ,      3,  SMBIOS_STRING_UNKNOWN_VERSION  },  // Version
  {  SMBIOS_TYPE_SYSTEM_INFORMATION ,      4,  SMBIOS_STRING_UNKNOWN          },  // Serial Number
  {  SMBIOS_TYPE_SYSTEM_INFORMATION ,      5,  SMBIOS_STRING_UNKNOWN          },  // SKU Variant
  {  SMBIOS_TYPE_SYSTEM_INFORMATION ,      6,  SMBIOS_STRING_PLATFORM         },  // Serial Number
  // Type 2
  {  SMBIOS_TYPE_BASEBOARD_INFORMATION ,   1,  SMBIOS_STRING_UNKNOWN          },  // Manufacturer
  {  SMBIOS_TYPE_BASEBOARD_INFORMATION ,   2,  SMBIOS_STRING_UNKNOWN          },  // Product Name
  {  SMBIOS_TYPE_BASEBOARD_INFORMATION ,   3,  SMBIOS_STRING_UNKNOWN_VERSION  },  // Version
  {  SMBIOS_TYPE_BASEBOARD_INFORMATION ,   4,  SMBIOS_STRING_UNKNOWN          },  // Serial Number
  // Type 127 - End of strings
  {  SMBIOS_TYPE_END_OF_TABLE,             0,  ""                             }
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
  IN  UINT16    NewLength
  )
{
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntry;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL) {
    return EFI_DEVICE_ERROR;
  }

  if ( (UINT32)(SmbiosEntry->EntryPointLength + sizeof (UINT8) + SmbiosEntry->TableLength + NewLength) > (UINT32)PcdGet16(PcdSmbiosTablesSize) ) {
    return EFI_BUFFER_TOO_SMALL;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Return the pointer to the Smbios table spcified by 'Type'

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
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntry;
  UINT8                         *StringPtr;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL) {
    return NULL;
  }

  TypeHdr = (SMBIOS_STRUCTURE *)(UINTN)SmbiosEntry->TableAddress;
  for (; TypeHdr->Type != SMBIOS_TYPE_END_OF_TABLE;) {
    if ( (UINT32)(UINTN)TypeHdr > (PcdGet32 (PcdSmbiosTablesBase) + (UINT32)PcdGet16 (PcdSmbiosTablesSize)) ) {
      return NULL;
    }
    if (TypeHdr->Type == Type) {
      return TypeHdr;
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

  return NULL;
}

/**
  After adding new Smbios Types, we need to do the following:
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
  IN  UINT16    NewMaxStructSize
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
  Status = CheckSmbiosOverflow (sizeof (SMBIOS_STRUCTURE) + TYPE_TERMINATOR_SIZE);
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
  mType127Ptr = (VOID *) Type127;

  SmbiosEntry->TableLength += sizeof (SMBIOS_STRUCTURE) + TYPE_TERMINATOR_SIZE;

  //
  // Patch entry point struct with new data
  //
  if (NewMaxStructSize > SmbiosEntry->MaxStructureSize) {
    SmbiosEntry->MaxStructureSize = NewMaxStructSize;
  }
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
  Append an Smbios Type to an existing set of types
  Different from AddSmbiosType, which gets called during initial Smbios setup

  @param[in]  TypeData    pointer to the Type Data including strings
  @param[in]  TypeLength  Size of type data including strings and end terminator (0000)

  @retval                         EFI_DEVICE_ERROR          , if Smbios Entry is NULL
                                  EFI_ALREADY_STARTED       , is type is already present
                                  Overflow status for adding
                                  this type or Type 127     , otherwise

**/
EFI_STATUS
EFIAPI
AppendSmbiosType (
  IN  VOID      *TypeData,
  IN  UINT16    TypeLength
  )
{
  SMBIOS_TABLE_ENTRY_POINT    *SmbiosEntry;
  EFI_STATUS                  Status;
  SMBIOS_STRUCTURE            *TypeHdr;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);
  if (SmbiosEntry == NULL || mType127Ptr == NULL || TypeLength > SMBIOS_TABLE_MAX_LENGTH) {
    return EFI_DEVICE_ERROR;
  }

  TypeHdr = (SMBIOS_STRUCTURE *) TypeData;

  //
  // Check if the type is already added during Smbios init
  //
  if (TypeHdr->Type == SMBIOS_TYPE_BIOS_INFORMATION   ||
      TypeHdr->Type == SMBIOS_TYPE_SYSTEM_INFORMATION ||
      TypeHdr->Type == SMBIOS_TYPE_BASEBOARD_INFORMATION) {
    DEBUG ((DEBUG_INFO, "Type already present, returning\n"));
    return EFI_ALREADY_STARTED;
  }

  //
  // Check for overflows before adding new type
  //
  Status = CheckSmbiosOverflow (TypeLength);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Find current Type127 and replace it with new Type Data
  //
  SmbiosEntry->TableLength -= ( ((SMBIOS_STRUCTURE *)mType127Ptr)->Length + TYPE_TERMINATOR_SIZE);
  TypeHdr->Handle = ((SMBIOS_STRUCTURE *)mType127Ptr)->Handle;
  CopyMem (mType127Ptr, TypeData, TypeLength);
  SmbiosEntry->TableLength += TypeLength;

  //
  // After appending, update with Typ127 and patch entry point
  //
  Status = FinalizeSmbios (TypeLength);
  return Status;
}

/**
  Get the value a particular string in a Type
  from data structure pointed to by PcdSmbiosStringsPtr

  @param[in]  Type      Get the string for a Type
  @param[in]  StringId  String Num

  @retval               CHAR8 * to the string, if found
                        "Unknown"            , otherwise

**/
CHAR8 *
GetSmbiosString (
  IN  UINT8   Type,
  IN  UINT8   StringId
  )
{
  SMBIOS_TYPE_STRINGS       *SmbiosStrings;
  UINT16                    Index;

  SmbiosStrings = (SMBIOS_TYPE_STRINGS *)(UINTN)PcdGet32 (PcdSmbiosStringsPtr);
  if (SmbiosStrings == NULL) {
    return "Unknown";
  }

  for (Index = 0; Index < PcdGet16 (PcdSmbiosStringsCnt); ++Index) {
    if (SmbiosStrings[Index].Type == SMBIOS_TYPE_END_OF_TABLE) {
      return "Unknown";
    }
    if (Type == SmbiosStrings[Index].Type && StringId == SmbiosStrings[Index].Idx) {
      if (SmbiosStrings[Index].String != NULL && AsciiStrLen (SmbiosStrings[Index].String) != 0) {
        return SmbiosStrings[Index].String;
      }
      break;
    }
  }

  return "Unknown";
}

/**
  Add the string to an Smbios type

  @param[in]  Destination     Address where the string gets added inside a type
  @param[in]  Source          CHAR8 * to the actual string

  @retval                     Destination for the next string to be added

**/
CHAR8 *
AddSmbiosString (
  IN  CHAR8   *Destination,
  IN  CHAR8   *Source
  )
{
  if (Source == NULL) {
    return Destination;
  }

  CopyMem ((CHAR8 *)Destination, Source, AsciiStrSize (Source));
  Destination += AsciiStrSize (Source); // every string terminates with a 00

  return Destination;
}


/**
  Get system memory size from FSP HOB.

  @retval   Total system memory size

**/
UINT64
GetSystemMemorySize (
  VOID
  )
{
  UINT32                        Tolm;
  UINT64                        Tohm;
  VOID                         *FspHob;

  // Build memory array mapped address information.
  FspHob = GetFspHobListPtr ();
  if (FspHob != NULL) {
    GetSystemTopOfMemeory (FspHob, &Tohm);
  } else {
    Tohm = SIZE_4GB;
  }
  Tolm = GetUsableMemoryTop ();
  return Tolm + (Tohm - SIZE_4GB);
}

/**
  Add a particular Smbios type to the Smbios allocated region.
  Different from AppenSmbiosType which can be called at a later stage

  @param[in]  Type        Type Id being added
  @param[in]  TypeAddr    Address where the type is begin added

  @retval                 Address where the next type will be added

**/
VOID *
EFIAPI
AddSmbiosType (
  IN  UINT8       Type,
  IN  VOID        *TypeAddr,
  IN  UINT16      *MaxLength
  )
{
  EFI_STATUS                    Status;
  SMBIOS_TABLE_ENTRY_POINT      *SmbiosEntry;
  SMBIOS_STRUCTURE_POINTER      TypePtr;
  UINT16                        TypeLength;
  VOID                          *HdrInfo;
  UINT8                         HdrLen;
  CHAR8*                        StringPtr;
  UINT8                         StrIdx;
  UINT8                         NumStr;

  SmbiosEntry = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN)PcdGet32 (PcdSmbiosTablesBase);

  //
  // Prepare type info
  //
  TypePtr.Raw = (UINT8 *) TypeAddr;
  switch (Type) {
  case SMBIOS_TYPE_BIOS_INFORMATION:
    HdrLen = sizeof (SMBIOS_TABLE_TYPE0);
    HdrInfo = (VOID *)&mBiosInfo;
    NumStr = 3;
    break;
  case SMBIOS_TYPE_SYSTEM_INFORMATION:
    HdrLen = sizeof (SMBIOS_TABLE_TYPE1);
    HdrInfo = (VOID *)&mSystemInfo;
    NumStr = 6;
    break;
  case SMBIOS_TYPE_BASEBOARD_INFORMATION:
    HdrLen = sizeof (SMBIOS_TABLE_TYPE2);
    HdrInfo = (VOID *)&mBaseBoardInfo;
    NumStr = 6;
    break;
  case SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS:
    HdrLen  = sizeof (SMBIOS_TABLE_TYPE19);
    mMemArrayMappedAddr.ExtendedEndingAddress = GetSystemMemorySize () - 1;
    HdrInfo = (VOID *)&mMemArrayMappedAddr;
    NumStr  = 0;
    break;
  }

  //
  // Check for overflow before adding the Type
  //
  if (NumStr > 0) {
    TypeLength = HdrLen + (SMBIOS_STRING_MAX_LENGTH + 1) * NumStr + sizeof (CHAR8);
  } else {
    TypeLength = HdrLen + 2 * sizeof (CHAR8);
  }

  Status = CheckSmbiosOverflow (TypeLength);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "Not enough memory to add Type[%d]\n", Type));
    return TypeAddr;
  }

  //
  // Copy header info into memory allocated
  // and add strings for this type
  //
  CopyMem (TypePtr.Raw, HdrInfo, HdrLen);
  StringPtr = (CHAR8 *) (TypePtr.Raw + HdrLen);
  if (NumStr > 0) {
    for (StrIdx = 1; StrIdx <= NumStr; ++StrIdx) {
      StringPtr = AddSmbiosString (StringPtr, GetSmbiosString (Type, StrIdx));
    }
  } else {
    // Add string terminator
    *StringPtr++ = 0;
  }
  *StringPtr++ = 0;
  TypeAddr = StringPtr;       // last string is terminated with a 0000

  //
  // Update TypeLength, header length, Max Length
  // and TableLength(in entry point)
  //
  TypeLength = (UINT16)((UINTN)(TypeAddr) - (UINTN)TypePtr.Raw);
  TypePtr.Hdr->Length = HdrLen;
  if (TypeLength > *MaxLength) {
    *MaxLength = TypeLength;
  }
  SmbiosEntry->TableLength += TypeLength;

  //
  // According to SMBIOS spec, Handle field is used to
  // get a particular smbios type structure.
  //
  TypePtr.Hdr->Handle = SmbiosEntry->NumberOfSmbiosStructures++;

  return TypeAddr;

}


/**
  This function is called to initialize the SmbiosStringsPtr.
**/
VOID
EFIAPI
InitSmbiosStringPtr (
  VOID
  )
{

  EFI_STATUS            Status;
  UINT16                Length;
  VOID                 *SmbiosStringsPtr;

  //
  // Allocate the memmory for SMBIOS String Ptr,
  //

  SmbiosStringsPtr = AllocateZeroPool (PcdGet16(PcdSmbiosStringsCnt) * sizeof (SMBIOS_TYPE_STRINGS));
  if (SmbiosStringsPtr == NULL) {
    DEBUG ((DEBUG_INFO, "SmbiosStringsPtr Memory allocation failed"));
    ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
  }

  //
  // Copy contents from Default String Array
  //

  Length = sizeof (mDefaultSmbiosStrings);
  if(Length <= (PcdGet16(PcdSmbiosStringsCnt) * sizeof (SMBIOS_TYPE_STRINGS))) {
    CopyMem (SmbiosStringsPtr, mDefaultSmbiosStrings, Length);
  } else {
    DEBUG ((DEBUG_INFO, "SmbiosStringsPtr Not Sufficient 0x%x", Length));
    ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
  }
  //
  // Initialize SMBIOS String Ptr, Update Length
  //
  Status = PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStringsPtr);
  ASSERT_EFI_ERROR (Status);
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
  VOID                          *TypeAddr;
  UINT16                        MaxLength = 0;
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
  SmbiosEntryPoint->MajorVersion                              = 2;
  SmbiosEntryPoint->MinorVersion                              = 7;
  SmbiosEntryPoint->TableLength                               = 0;
  *((UINT32 *)&(SmbiosEntryPoint->IntermediateAnchorString))  = SIGNATURE_32('_', 'D', 'M', 'I');
  SmbiosEntryPoint->IntermediateAnchorString[4]               = '_';
  SmbiosEntryPoint->TableAddress                              = (UINT32)(UINTN)SmbiosEntryPoint + sizeof (SMBIOS_TABLE_ENTRY_POINT) + sizeof (UINT8);

  //
  // Add common SMBIOS Types' information.
  // Types start at 16 byte boundary
  //
  TypeAddr = (VOID *)(UINTN)SmbiosEntryPoint->TableAddress;
  TypeAddr = AddSmbiosType (SMBIOS_TYPE_BIOS_INFORMATION,      TypeAddr, &MaxLength);
  TypeAddr = AddSmbiosType (SMBIOS_TYPE_SYSTEM_INFORMATION,    TypeAddr, &MaxLength);
  TypeAddr = AddSmbiosType (SMBIOS_TYPE_BASEBOARD_INFORMATION,       TypeAddr, &MaxLength);
  TypeAddr = AddSmbiosType (SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS, TypeAddr, &MaxLength);

  //
  // Add Type 0x7F, and patch the entry point structure
  //
  Status = FinalizeSmbios (MaxLength);

  return Status;
}
