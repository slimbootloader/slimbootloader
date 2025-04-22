/** @file
  PCI command register operations supporting functions implementation for PCI Bus module.

  Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Library/BaseLib.h>

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include "InternalPciEnumerationLib.h"

/**
  Check the capability supporting by given device.

  @param[in]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

  @retval     TRUE            Capability supported.
  @retval     FALSE           Capability not supported.

**/
BOOLEAN
PciCapabilitySupport (
  IN PCI_IO_DEVICE  *PciIoDevice
  )
{
  if ((PciIoDevice->Pci.Hdr.Status & EFI_PCI_STATUS_CAPABILITY) != 0) {
    return TRUE;
  }

  return FALSE;
}

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
  )
{
  UINT8   CapabilityPtr;
  UINT16  CapabilityEntry;
  UINT8   CapabilityID;

  //
  // To check the capability of this device supports
  //
  if (!PciCapabilitySupport (PciIoDevice)) {
    return EFI_UNSUPPORTED;
  }

  if (*Offset != 0) {
    CapabilityPtr = *Offset;
  } else {
    if (IS_CARDBUS_BRIDGE (&PciIoDevice->Pci)) {
      CapabilityPtr = PciExpressRead8 (PciIoDevice->Address + EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR);
    } else {
      CapabilityPtr = PciExpressRead8 (PciIoDevice->Address + PCI_CAPBILITY_POINTER_OFFSET);
    }
  }

  while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
    CapabilityEntry = PciExpressRead16 (PciIoDevice->Address + CapabilityPtr);
    CapabilityID    = (UINT8) CapabilityEntry;

    if (CapabilityID == CapId) {
      *Offset = CapabilityPtr;
      return EFI_SUCCESS;
    }

    //
    // Certain PCI device may incorrectly have capability pointing to itself,
    // break to avoid dead loop.
    //
    if (CapabilityPtr == (UINT8) (CapabilityEntry >> 8)) {
      break;
    }

    CapabilityPtr = (UINT8) (CapabilityEntry >> 8);
  }

  return EFI_NOT_FOUND;
}

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
  )
{
  UINT32               CapabilityPtr;
  UINT32               CapabilityEntry;
  UINT16               CapabilityID;

  //
  // To check the capability of this device supports
  //
  if (!PciIoDevice->IsPciExp) {
    return EFI_UNSUPPORTED;
  }

  if (*Offset != 0) {
    CapabilityPtr = *Offset;
  } else {
    CapabilityPtr = EFI_PCIE_CAPABILITY_BASE_OFFSET;
  }

  while (CapabilityPtr != 0) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFFC;
    CapabilityEntry = PciExpressRead32 (PciIoDevice->Address + CapabilityPtr);
    if (CapabilityEntry == MAX_UINT32) {
      DEBUG ((DEBUG_WARN, "PCI Address 0x08X failed to access at offset 0x%X\n",
        PciIoDevice->Address, CapabilityPtr));
      break;
    }

    CapabilityID = (UINT16) CapabilityEntry;

    if (CapabilityID == CapId) {
      *Offset = CapabilityPtr;
      return EFI_SUCCESS;
    }

    CapabilityPtr = (CapabilityEntry >> 20) & 0xFFF;
  }

  return EFI_NOT_FOUND;
}
