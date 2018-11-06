/** @file
  This file defines the hob structure for the device table.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __DEVICE_TABLE_HOB_GUID_H__
#define __DEVICE_TABLE_HOB_GUID_H__

///
/// Device table Hob GUID
///
extern EFI_GUID gDeviceTableHobGuid;

typedef struct {
  UINT32        PciFunctionNumber:8;
  UINT32        PciDeviceNumber:8;
  UINT32        PciBusNumber:8;
  UINT32        IsMmioDevice:8;
} PLT_PCI_DEVICE;

typedef struct {
  union {
    UINT32          DevAddr;
    PLT_PCI_DEVICE  PciDev;
  } Dev;
  UINT8             Type;
  UINT8             Instance;
  UINT16            Reserved;
} PLT_DEVICE;

typedef struct {
  UINT16          DeviceNumber;
  UINT16          Reserved;
  PLT_DEVICE      Device[0];
} PLT_DEVICE_TABLE;

#endif
