/** @file
  This file defines the hob structure used for Platform information.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LOADER_PLATFORM_INFO_GUID_H__
#define __LOADER_PLATFORM_INFO_GUID_H__

#define MAX_SERIAL_NUMBER_LENGTH    16

///
/// Boot loader Platform Information GUID
///
extern EFI_GUID gLoaderPlatformInfoGuid;

#pragma pack(1)

//Definition for LOADER_PLATFORM_INFO.LdrFeatures

#define        FEATURE_ACPI                     BIT0
#define        FEATURE_MEASURED_BOOT            BIT1
#define        FEATURE_MMC_TUNING               BIT2
#define        FEATURE_MMC_FORCE_TUNING         BIT3
#define        FEATURE_VERIFIED_BOOT            BIT4

//
//Definition for LOADER_PLATFORM_INFO.HwState
//

/// Verified boot state (0- disabled, 1 - enabled), based on boot guard profile.
#define        HWSTATE_VERIFIED_BOOT            BIT0

// Measured boot state (0- disabled, 1 - enabled), based on boot guard profile.
#define        HWSTATE_MEASURED_BOOT            BIT1

// Manufacturing state (0 - ready for production; 1 - not ready for production)
//  (This comes from CSE. When this bit is 1, it is still in manufacturing mode. Host
//  can use this bit to inform user that platform is NOT ready for production yet.
//  when this bit is 0, then it is ready for production)
#define        HWSTATE_IN_MANUFACTURING_MODE     BIT2

//Secure debug (0 - disabled, 1 - enabled)
#define        HWSTATE_SECURE_DEBUG              BIT3

//Definition for LOADER_PLATFORM_INFO.Flags

//
// When writing flash in SMM mode, some platform need disable SMM write
// protection using following method:
//  -- AsmWriteMsr32 (0x1FE, MmioRead32 (0xFED30880) | BIT0);
//
#define   FLAGS_SPI_DISABLE_SMM_WRITE_PROTECT    BIT0

typedef struct {
  UINT8          Revision;
  UINT8          Reserved[3];
  UINT8          BootPartition;
  UINT8          BootMode;
  UINT16         PlatformId;
  UINT32         CpuCount;
  UINT16         HwState;
  UINT16         Flags;
  UINT32         LdrFeatures;
  CHAR8          SerialNumber[MAX_SERIAL_NUMBER_LENGTH];
} LOADER_PLATFORM_INFO;

#pragma pack()

#endif
