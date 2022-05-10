/** @file
  This file defines the hob structure for csme performance data.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CSME_PERFORMANCE_INFO_GUID_H__
#define __CSME_PERFORMANCE_INFO_GUID_H__

extern EFI_GUID gCsmePerformanceInfoGuid;

#pragma pack(1)

typedef struct {
  UINT8     Revision;
  UINT8     Reserved0[3];
  UINT32    BootDataVersion;
  UINT32    BootDataLength;
  UINT32    BootPerformanceData[];
} CSME_PERFORMANCE_INFO;

#pragma pack()

#endif
