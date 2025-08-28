/** @file
  SMBIOS Device Information Binary Support for SMBIOS Initialization

  This header defines a structure for storing device-specific SMBIOS strings,
  allowing platforms to override default SMBIOS values during initialization.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SMBIOS_DEVICE_OVERRIDE_H__
#define __SMBIOS_DEVICE_OVERRIDE_H__

#include <Base.h>
#include <Library/BaseLib.h>

//
// Device Info Binary Structure
// Used to override default SMBIOS strings with device-specific information
// This structure is standardized across all platforms
//
#pragma pack(push, 1)
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
#pragma pack(pop)


/**
  Get SMBIOS string value with fallback logic

  @param[in] DeviceInfoValue    Value from device info binary (null-terminated)
  @param[in] DefaultValue       Default platform value

  @retval    Device info value if available and non-empty, otherwise default value
**/
CHAR8*
GetSmbiosStringValue (
  IN CHAR8    *DeviceInfoValue,
  IN CHAR8    *DefaultValue
);


#endif // __SMBIOS_DEVICE_OVERRIDE_H__
