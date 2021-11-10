/** @file
  This file defines the hob structure for the serial port device info.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SERIAL_PORT_INFO_GUID_H__
#define __SERIAL_PORT_INFO_GUID_H__

///
/// Serial Port Information GUID
///
extern EFI_GUID gLoaderSerialPortInfoGuid;

typedef struct {
  UINT8      Revision;
  UINT8      Reserved0[3];
  UINT32     Type;
  UINT32     BaseAddr;
  UINT32     Baud;
  UINT32     RegWidth;
  UINT32     InputHertz;
  UINT32     UartPciAddr;
} SERIAL_PORT_INFO;

#endif
