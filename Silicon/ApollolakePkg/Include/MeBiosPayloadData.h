/** @file
  Interface definition details for MBP during DXE phase.

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _MBP_DATA_H_
#define _MBP_DATA_H_

#include <MeBiosPayloadDataCommon.h>

#define MBP_APP_ID_KERNEL                     1
#define MBP_ITEM_ID_FW_VER_NAME               1

#define MBP_APP_ID_NVM                        8
#define MBP_ITEM_ID_CURRENT_BOOT_MEDIA        1

#define MBP_APP_ABL_SIG                       9
#define MBP_ITEM_ID_IAFW_IBB_SIG              1

typedef struct {
  UINT32  MKHIHeader;
  UINT8   Length;
  UINT8   ItemsNum;
  UINT8   Flags;
  UINT8   Reserved;
} MBP_CMD_RESP_DATA;

typedef struct {
  UINT32  BPDTOffset;
  UINT32  PhysicalData;
  UINT32  PhysicalDeviceArea;
  UINT32  LogicalData;
} MBP_CURRENT_BOOT_MEDIA;

#endif // _MBP_DATA_H_
