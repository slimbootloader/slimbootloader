/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"

/**
  Add a Smbios type string into a buffer

**/
STATIC
EFI_STATUS
AddSmbiosTypeString (
  SMBIOS_TYPE_STRINGS  *Dest,
  UINT8                 Type,
  UINT8                 Index,
  CHAR8                *String
  )
{
  UINTN   Length;

  Dest->Type    = Type;
  Dest->Idx     = Index;
  if (String != NULL) {
    Length = AsciiStrLen (String);

    Dest->String  = (CHAR8 *)AllocateZeroPool (Length + 1);
    if (Dest->String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (Dest->String, String, Length);
  }

  return EFI_SUCCESS;
}

/**
  Initialize necessary information for Smbios

  @retval EFI_SUCCESS             Initialized necessary information successfully
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory for Smbios info

**/
EFI_STATUS
InitializeSmbiosInfo (
  VOID
  )
{
  UINT16                Index;
  UINTN                 Length;
  SMBIOS_TYPE_STRINGS  *TempSmbiosStrTbl;
  VOID                 *SmbiosStringsPtr;

  if (FeaturePcdGet (PcdSmbiosEnabled)) {
    Index         = 0;
    TempSmbiosStrTbl  = (SMBIOS_TYPE_STRINGS *) AllocateTemporaryMemory (0);

    //
    // SMBIOS_TYPE_BIOS_INFORMATION
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
      1, "Intel Corporation");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
      2, "0.0.0 Version");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
      3, __DATE__);

    //
    // SMBIOS_TYPE_SYSTEM_INFORMATION
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      1, "Intel Corporation");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      2, "AlderLake");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      3, "0.0");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      4, "System Serial Number");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      5, "System SKU Number");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      6, "AlderLake");

    //
    // SMBIOS_TYPE_BASEBOARD_INFORMATION
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      1, "Intel Corporation");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      2, "AlderLake");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      3, "1");
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      4, "No Board Serial Number");

    //
    // SMBIOS_TYPE_PROCESSOR_INFORMATION : TBD
    //

    //
    // SMBIOS_TYPE_END_OF_TABLE
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_END_OF_TABLE,
      0, NULL);

    Length = sizeof (SMBIOS_TYPE_STRINGS) * Index;
    SmbiosStringsPtr = AllocatePool (Length);
    if (SmbiosStringsPtr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (SmbiosStringsPtr, TempSmbiosStrTbl, Length);
    (VOID) PcdSet32S (PcdSmbiosStringsPtr, (UINT32)(UINTN)SmbiosStringsPtr);
    (VOID) PcdSet16S (PcdSmbiosStringsCnt, Index);
  }

  return EFI_SUCCESS;
}
