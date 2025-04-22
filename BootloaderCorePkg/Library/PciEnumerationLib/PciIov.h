/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_IOV_H__
#define __PCI_IOV_H__

/**
  Parse PCI IOV VF bar information and fill them into PCI device instance.

  @param PciIoDevice  Pci device instance.
  @param Offset       Bar offset.
  @param BarIndex     Bar index.

  @return Next bar offset.

**/
UINTN
PciIovParseVfBar (
  IN PCI_IO_DEVICE  *PciIoDevice,
  IN UINTN          Offset,
  IN UINTN          BarIndex
  );

/**
  Initialize SR-IOV (Single Root IO Virtualization)

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.
  @param[in]      Bus             Device Bus NO.
  @param[in]      Device          Device device NO.
  @param[in]      Func            Device func NO.

**/
VOID
EFIAPI
InitializeSrIov (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice,
  IN      UINT8                       Bus,
  IN      UINT8                       Device,
  IN      UINT8                       Func
  );

#endif // __PCI_IOV_H__
