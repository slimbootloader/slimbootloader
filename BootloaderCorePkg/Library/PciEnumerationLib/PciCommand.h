/** @file
  PCI command register operations supporting functions declaration for PCI Bus module.

  Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCI_COMMAND_H__
#define __PCI_COMMAND_H__

/**
  Check the capability supporting by given device.

  @param[in]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

  @retval     TRUE            Capability supported.
  @retval     FALSE           Capability not supported.

**/
BOOLEAN
PciCapabilitySupport (
  IN PCI_IO_DEVICE  *PciIoDevice
  );

/**
  Locate capability register block per capability ID.

  @param[in]      PciIoDevice       A pointer to the PCI_IO_DEVICE.
  @param[in]      CapId             The capability ID.
  @param[in, out] Offset            A pointer to the offset returned.

  @retval EFI_SUCCESS      Successfully located capability register block.
  @retval EFI_UNSUPPORTED  Pci device does not support capability.
  @retval EFI_NOT_FOUND    Pci device support but can not find register block.

**/
EFI_STATUS
LocateCapabilityRegBlock (
  IN PCI_IO_DEVICE  *PciIoDevice,
  IN UINT8          CapId,
  IN OUT UINT8      *Offset
  );

/**
  Locate PciExpress capability register block per capability ID.

  @param[in]      PciIoDevice       A pointer to the PCI_IO_DEVICE.
  @param[in]      CapId             The capability ID.
  @param[in, out] Offset            A pointer to the offset returned.

  @retval EFI_SUCCESS      Successfully located capability register block.
  @retval EFI_UNSUPPORTED  Pci device does not support capability.
  @retval EFI_NOT_FOUND    Pci device support but can not find register block.

**/
EFI_STATUS
LocatePciExpressCapabilityRegBlock (
  IN     PCI_IO_DEVICE *PciIoDevice,
  IN     UINT16        CapId,
  IN OUT UINT32        *Offset
  );

#endif // __PCI_COMMAND_H__
