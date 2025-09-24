/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SMBIOS_INIT_LIB_H__
#define __SMBIOS_INIT_LIB_H__

#include <IndustryStandard/SmBios.h>


#pragma pack(1)

typedef struct {
  UINT8     Type;
  UINT8     Idx;
  CHAR8     *String;
} SMBIOS_TYPE_STRINGS;

typedef struct {
  // System Information (SMBIOS Type 1)
  CHAR8   SystemManufacturer[64];             // System manufacturer string
  CHAR8   SystemProductName[64];              // System product name string
  CHAR8   SystemVersion[32];                  // System version string
  CHAR8   SystemSerialNumber[32];             // System serial number string
  CHAR8   SystemUuid[37];                     // System UUID string (RFC 4122 format)
  CHAR8   SystemSku[32];                      // System SKU string
  CHAR8   SystemFamily[64];                   // System family string

  // Baseboard Information (SMBIOS Type 2)
  CHAR8   BaseboardManufacturer[64];          // Baseboard manufacturer string
  CHAR8   BaseboardProductName[64];           // Baseboard product name string
  CHAR8   BaseboardVersion[32];               // Baseboard version string
  CHAR8   BaseboardSerialNumber[32];          // Baseboard serial number string
  CHAR8   BaseboardAssetTag[32];              // Baseboard asset tag string

  // Chassis Information (SMBIOS Type 3)
  CHAR8   ChassisManufacturer[64];            // Chassis manufacturer string
  CHAR8   ChassisVersion[32];                 // Chassis version string
  CHAR8   ChassisSerialNumber[32];            // Chassis serial number string
  CHAR8   ChassisAssetTag[32];                // Chassis asset tag string

  // OEM Data (Platform-specific extensions)
  CHAR8   OemVersion[32];                     // OEM structure version
  CHAR8   OemCustomField1[64];                // OEM custom field 1
  CHAR8   OemCustomField2[64];                // OEM custom field 2
} DEVICE_INFO_DATA;
#pragma pack()

/**
  After adding and appending all Smbios Types, we need to do the following
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
  );

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
  );

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
  );

/**
  This function is called to initialize the SmbiosStringsPtr.
**/
VOID
EFIAPI
InitSmbiosStringPtr (
  VOID
  );

/**
  This function is called to initialize the SMBIOS tables.

  @retval       EFI_DEVICE_ERROR, if Smbios Entry is NULL
                Status after finalizing the Smbios init

**/
EFI_STATUS
EFIAPI
SmbiosInit (
  VOID
  );

/**
  Loads device information from the non-volatile region and applies overrides to SMBIOS strings.

  This function attempts to load a device info binary from the firmware image. If found,
  it updates the SMBIOS string table with values from the device info, overriding default
  platform strings such as manufacturer, product name, version, and serial number.

  @retval EFI_SUCCESS      Device info override applied successfully.
  @retval EFI_NOT_FOUND    Device info binary not found or no SMBIOS strings to override.
**/
EFI_STATUS
EFIAPI
ApplySmbiosDeviceInfoOverride (
  VOID
  );

#endif
