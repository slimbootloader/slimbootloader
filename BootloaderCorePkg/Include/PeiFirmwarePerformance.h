/** @file
 This file defines PEI Firmware Performance GUID and structures.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PEI_FIRMWARE_PERFORMANCE_DATA_H__
#define __PEI_FIRMWARE_PERFORMANCE_DATA_H__

#pragma pack(1)
typedef struct {
  UINT16    ProgressID;
  EFI_GUID  Guid;
  UINT32    ApicID;
  UINT64    Timestamp;
} PEI_GUID_EVENT_RECORD;
#pragma pack()

typedef struct {
  UINT32                 NumberOfEntries;
  UINT32                 Reserved;
  PEI_GUID_EVENT_RECORD  GuidEventRecord[1];
} PEI_FIRMWARE_PERFORMANCE_HOB;

extern EFI_GUID gPeiFirmwarePerformanceGuid;

#endif
