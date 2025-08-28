/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage2BoardInitLib.h"
#include <Library/SmbiosOverride.h>
#include <Library/PrintLib.h>

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
  CHAR8                 TempStrBuf[SMBIOS_STRING_MAX_LENGTH];
  UINT16                Index;
  UINTN                 Length;
  SMBIOS_TYPE_STRINGS  *TempSmbiosStrTbl;
  BOOT_LOADER_VERSION  *VerInfoTbl;
  VOID                 *SmbiosStringsPtr;
  UINT8                TempName[9];

  // Default string values
  CHAR8                *SystemManufacturer;
  CHAR8                *SystemProductName;
  CHAR8                *SystemVersion;
  CHAR8                *SystemSerialNumber;
  CHAR8                *SystemSku;
  CHAR8                *SystemFamily;
  CHAR8                *BaseboardManufacturer;
  CHAR8                *BaseboardProductName;
  CHAR8                *BaseboardVersion;
  CHAR8                *BaseboardSerialNumber;

  if (FeaturePcdGet (PcdSmbiosEnabled)) {
    Index         = 0;
    TempSmbiosStrTbl  = (SMBIOS_TYPE_STRINGS *) AllocateTemporaryMemory (0);
    if (TempSmbiosStrTbl == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    VerInfoTbl  = GetVerInfoPtr ();
    TempName[8] = 0;

    // Set SMBIOS default values
    SystemManufacturer = "Intel Corporation";
    SystemVersion = "0.0";
    SystemSerialNumber = "System Serial Number";
    SystemSku = "System SKU Number";
    SystemFamily = GetCpuName();

    BaseboardManufacturer = "Intel Corporation";
    BaseboardVersion = "1";
    BaseboardSerialNumber = "No Board Serial Number";

    CHAR8 SystemProductStrBuf[SMBIOS_STRING_MAX_LENGTH];
    CopyMem (TempName, GetPlatformName(), sizeof (UINT64));
    AsciiSPrint (SystemProductStrBuf, sizeof (TempStrBuf), "%a (CPU:%a)", TempName, GetCpuName ());
    SystemProductName = SystemProductStrBuf;

    CHAR8 BaseboardStrBuf[SMBIOS_STRING_MAX_LENGTH];
    AsciiSPrint (BaseboardStrBuf, sizeof (BaseboardStrBuf), "%a (ID:%02X)", TempName, GetPlatformId ());
    BaseboardProductName = BaseboardStrBuf;

    // Load Device Info binary and Override Smbios value
    if (FeaturePcdGet (PcdSmbiosOverride)) {
      DEVICE_INFO_DATA     *DeviceInfo = NULL;
      UINT32               DeviceInfoSize = 0;
      EFI_STATUS           Status;

      Status = LoadComponent (SIGNATURE_32 ('D', 'E', 'V', 'I'), SIGNATURE_32 ('D', 'I', 'N', 'F'),
                            (VOID **)&DeviceInfo, &DeviceInfoSize);
      if (!EFI_ERROR(Status) && DeviceInfo != NULL) {
        DEBUG((DEBUG_INFO, "Device info binary loaded successfully, size: %d\n", DeviceInfoSize));

        // Override defaults with device info binary data
        SystemManufacturer = GetSmbiosStringValue(DeviceInfo->SystemManufacturer, SystemManufacturer);
        SystemProductName = GetSmbiosStringValue(DeviceInfo->SystemProductName, SystemProductName);
        SystemVersion = GetSmbiosStringValue(DeviceInfo->SystemVersion, SystemVersion);
        SystemSerialNumber = GetSmbiosStringValue(DeviceInfo->SystemSerialNumber, SystemSerialNumber);
        SystemSku = GetSmbiosStringValue(DeviceInfo->SystemSku, SystemSku);
        SystemFamily = GetSmbiosStringValue(DeviceInfo->SystemFamily, SystemFamily);

        BaseboardManufacturer = GetSmbiosStringValue(DeviceInfo->BaseboardManufacturer, BaseboardManufacturer);
        BaseboardProductName = GetSmbiosStringValue(DeviceInfo->BaseboardProductName, BaseboardProductName);
        BaseboardVersion = GetSmbiosStringValue(DeviceInfo->BaseboardVersion, BaseboardVersion);
        BaseboardSerialNumber = GetSmbiosStringValue(DeviceInfo->BaseboardSerialNumber, BaseboardSerialNumber);

      } else {
        DEBUG((DEBUG_INFO, "Device info binary not found, using defaults: %r\n", Status));
      }
    }

    //
    // SMBIOS_TYPE_BIOS_INFORMATION
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
      1, "Intel Corporation");
  if (VerInfoTbl != NULL) {
    CopyMem (TempName, &VerInfoTbl->ImageId, sizeof (UINT64));
    for (Length = 7; Length != 0; Length--) {
      if (TempName[Length] != 0x20) break;
      TempName[Length] = 0;
    }
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf),
      "%a.%02d.%02d.%02d.%02d.%02d.%03d.%c-%08X%a\0",
      TempName,
      VerInfoTbl->ImageVersion.SecureVerNum,
      VerInfoTbl->ImageVersion.CoreMajorVersion,
      VerInfoTbl->ImageVersion.CoreMinorVersion,
      VerInfoTbl->ImageVersion.ProjMajorVersion,
      VerInfoTbl->ImageVersion.ProjMinorVersion,
      VerInfoTbl->ImageVersion.BuildNumber,
      VerInfoTbl->ImageVersion.BldDebug ? 'D' : 'R',
      *((UINT32*)&VerInfoTbl->SourceVersion + 1),
      VerInfoTbl->ImageVersion.Dirty ? "-dirty" : "");
  } else {
    AsciiSPrint (TempStrBuf, sizeof (TempStrBuf), "%a\0", "Unknown");
  }
  AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
    2, TempStrBuf);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BIOS_INFORMATION,
      3, __DATE__);

    //
    // SMBIOS_TYPE_SYSTEM_INFORMATION
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      1, SystemManufacturer);

    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      2, SystemProductName);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      3, SystemVersion);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      4, SystemSerialNumber);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      5, SystemSku);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_SYSTEM_INFORMATION,
      6, SystemFamily);

    //
    // SMBIOS_TYPE_BASEBOARD_INFORMATION
    //
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      1, BaseboardManufacturer);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      2, BaseboardProductName);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      3, BaseboardVersion);
    AddSmbiosTypeString (&TempSmbiosStrTbl[Index++], SMBIOS_TYPE_BASEBOARD_INFORMATION,
      4, BaseboardSerialNumber);

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
