/** @file
  This file defines the hob structure for the OS config data support.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __OS_CONFIG_DATA_HOB_GUID_H__
#define __OS_CONFIG_DATA_HOB_GUID_H__

///
/// Os config data GUID
///
extern EFI_GUID gOsConfigDataGuid;

typedef struct {
  ///
  /// Currently set Revision to 1.
  ///
  UINT8     Revision;

  UINT8     Reserved[2];

  ///
  /// Comes from config data to indicate enable/disable crash mode
  ///
  UINT8     EnableCrashMode;

  ///
  /// Comes from config data to indicate reserved memory size in bytes
  ///
  UINT32    OsCrashMemorySize;
} OS_CONFIG_DATA_HOB;

#endif
