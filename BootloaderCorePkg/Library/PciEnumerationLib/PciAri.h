/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_ARI_H__
#define __PCI_ARI_H__

/**
  Initialize PciExpress Capability Offset

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

**/
VOID
EFIAPI
InitializePciExpCapability (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice
  );

/**
  Initialize ARI(Alternative Routing-ID Interpretation) devices

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

**/
VOID
EFIAPI
InitializeAri (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice
  );

#endif // __PCI_ARI_H__
