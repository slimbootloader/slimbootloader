/** @file
  This file defines platform data to be passed to payload.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LOADER_PLATFORM_DATA_GUID_H__
#define __LOADER_PLATFORM_DATA_GUID_H__

///
/// Debug Log Buffer Information GUID
///
extern EFI_GUID gLoaderPlatformDataGuid;

#define  DEBUG_OUTPUT_DEVICE_LOG_BUFFER     BIT0
#define  DEBUG_OUTPUT_DEVICE_SERIAL_PORT    BIT1
#define  DEBUG_OUTPUT_DEVICE_CONSOLE        BIT2

typedef struct {
  UINT8                     Revision;
  UINT8                     Reserved0[3];
  VOID                     *DebugLogBuffer;
  VOID                     *ConfigDataPtr;
  VOID                     *ContainerList;
  VOID                     *DmaBufferPtr;
} LOADER_PLATFORM_DATA;

#endif
