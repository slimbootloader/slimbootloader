/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are
  licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PayloadLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <Guid/OsConfigDataHobGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Library/BootOptionLib.h>
#include <Library/SblParameterLib.h>

#define PERF_DATA_SIGNATURE      SIGNATURE_32 ('P', 'E', 'R', 'F')


#pragma pack(1)
typedef struct {
  UINT32    Signature;
  UINT16    Count;
  UINT16    Flags;
  UINT32    Frequency;
  UINT64    TimeStamp[0];
} OS_PERF_DATA;

typedef struct {
  ///
  /// Boot medium type, Refer OS_BOOT_MEDIUM_TYPE
  ///
  UINT8                DevType;

  ///
  /// Zero-based hardware partition number
  ///
  UINT8                HwPart;

  ///
  /// For PCI device, its value is 0x00BBDDFF
  /// For other device, its value is MMIO address.
  ///
  UINT32               DevAddr;
} OS_BOOT_DEVICE;

typedef struct {
  UINT8          Revision;
  UINT8          BootDeviceCount;
  OS_BOOT_DEVICE BootDevice[0];
} OS_BOOT_DEVICE_LIST;

#pragma pack()

