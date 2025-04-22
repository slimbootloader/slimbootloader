/** @file
  This file defines the hob structure for performance data.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PERFORMANCE_INFO_GUID_H__
#define __PERFORMANCE_INFO_GUID_H__

extern EFI_GUID gLoaderPerformanceInfoGuid;

#pragma pack(1)

typedef struct {
  UINT8     Revision;
  UINT8     Reserved0[3];
  UINT16    Count;
  UINT16    Flags;
  UINT32    Frequency;
  UINT64    TimeStamp[0];
} PERFORMANCE_INFO;

#pragma pack()

#endif
