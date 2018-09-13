/** @file
  This file defines the hob structure for performance data.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
