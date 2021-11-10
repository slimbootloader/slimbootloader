/** @file
 This file defines PEI Firmware Performance GUID and structures.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

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
