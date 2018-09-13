/** @file
  This file defines the hob structure used for Platform information.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LOADER_PLATFORM_INFO_GUID_H__
#define __LOADER_PLATFORM_INFO_GUID_H__

#define MAX_SERIAL_NUMBER_LENGTH    16

///
/// Boot loader Platform Information GUID
///
extern EFI_GUID gLoaderPlatformInfoGuid;

#pragma pack(1)

typedef struct {
  UINT8          Revision;
  UINT8          Reserved[3];
  UINT8          BootPartition;
  UINT8          BootMode;
  UINT16         PlatformId;
  UINT32         CpuCount;
  UINT16         SecurityFlags; // Bit 0: Vb (0- disabled, 1 - enabled)
                                // Bit 1: Mb (0- disabled, 1 - enabled)
                                // Bit 2: Manufacturing state (0 - ready for production; 1 - not ready for production)
                                // (This comes from CSE. When this bit is 1, it is still in manufacturing mode. Host 
                                // can use this bit to inform user that platform is NOT ready for production yet.
                                // when this bit is 0, then it is ready for production)
                                // Bit 3: Secure debug enabled/disabled
  UINT8           Reserved1[2];
  UINT32          LdrFeatures;
  CHAR8           SerialNumber[MAX_SERIAL_NUMBER_LENGTH];
} LOADER_PLATFORM_INFO;

#pragma pack()

#endif
