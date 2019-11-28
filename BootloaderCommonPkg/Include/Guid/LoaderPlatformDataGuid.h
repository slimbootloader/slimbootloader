/** @file
  This file defines platform data to be passed to payload.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LOADER_PLATFORM_DATA_GUID_H__
#define __LOADER_PLATFORM_DATA_GUID_H__
#include <Uefi/UefiBaseType.h>

///
/// Debug Log Buffer Information GUID
///
extern EFI_GUID gLoaderPlatformDataGuid;

#define  DEBUG_LOG_BUFFER_SIGNATURE         SIGNATURE_32 ('D', 'L', 'O', 'G')

//
// Debug Interfaces
//
#define  DEBUG_DEVICE_RAM                   BIT0
#define  DEBUG_DEVICE_ISA_SERIAL            BIT1
#define  DEBUG_DEVICE_USB                   BIT3
#define  DEBUG_DEVICE_SERIALIO              BIT4
#define  DEBUG_DEVICE_TRACEHUB              BIT5

typedef struct {
  UINT32  Signature;
  UINT8   HeaderLength;
  UINT8   Attribute;
  UINT8   Reserved[2];
  UINT32  UsedLength;
  UINT32  TotalLength;
  UINT8   Buffer[0];
} DEBUG_LOG_BUFFER_HEADER;

typedef struct {
  UINT8                     Revision;
  UINT8                     Reserved0[3];
  DEBUG_LOG_BUFFER_HEADER  *DebugLogBuffer;
  VOID                     *ConfigDataPtr;
  VOID                     *ContainerList;
} LOADER_PLATFORM_DATA;

#endif
