/** @file
  This file defines the hob structure for the device table.

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DEVICE_TABLE_HOB_GUID_H__
#define __DEVICE_TABLE_HOB_GUID_H__

///
/// Device table Hob GUID
///
extern EFI_GUID gDeviceTableHobGuid;

typedef enum {
  PltDeviceSmbus = 0x20,
  PltDeviceIsh
} PLT_MEDIUM_MISC_TYPE;

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
