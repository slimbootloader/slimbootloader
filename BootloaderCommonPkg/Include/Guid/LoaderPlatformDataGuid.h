/** @file
  This file defines platform data to be passed to payload.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __LOADER_PLATFORM_DATA_GUID_H__
#define __LOADER_PLATFORM_DATA_GUID_H__

///
/// Debug Log Buffer Information GUID
///
extern EFI_GUID gLoaderPlatformDataGuid;

#define  DEBUG_LOG_BUFFER_SIGNATURE         SIGNATURE_32 ('D', 'L', 'O', 'G')

#define  DEBUG_OUTPUT_DEVICE_LOG_BUFFER     BIT0
#define  DEBUG_OUTPUT_DEVICE_SERIAL_PORT    BIT1
#define  DEBUG_OUTPUT_DEVICE_CONSOLE        BIT2

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
} LOADER_PLATFORM_DATA;

#endif
