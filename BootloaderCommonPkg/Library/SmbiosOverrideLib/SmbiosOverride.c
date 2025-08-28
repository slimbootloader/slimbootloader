/** @file
  SMBIOS Device Information Binary Support Implementation

  This library provides common smbios device info binary support that can be
  used by any platform's SMBIOS initialization.

  Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include <Library/SmbiosOverride.h>

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
  )
{
  // Use device info value if available and non-empty, otherwise use default
  if (DeviceInfoValue != NULL && DeviceInfoValue[0] != '\0') {
    return DeviceInfoValue;
  }
  return DefaultValue;
}


