/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include "InternalPciEnumerationLib.h"
#include "PciCommand.h"

/**
  Initialize PciExpress Capability Offset

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

**/
VOID
EFIAPI
InitializePciExpCapability (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice
  )
{
  EFI_STATUS            Status;

  Status = LocateCapabilityRegBlock (
           PciIoDevice,
           EFI_PCI_CAPABILITY_ID_PCIEXP,
           &PciIoDevice->PciExpressCapabilityOffset
           );

  if (!EFI_ERROR (Status)) {
    PciIoDevice->IsPciExp = TRUE;
  }
}

/**
  Initialize ARI(Alternative Routing-ID Interpretation) devices

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

**/
VOID
EFIAPI
InitializeAri (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice
  )
{
  PCI_IO_DEVICE        *Bridge;
  EFI_STATUS            Status;
  UINT32                Data32;

  if ((PciIoDevice == NULL) || (PciIoDevice->Parent == NULL)) {
    return;
  }

  //
  // Check if the device is an ARI device.
  //
  Status = LocatePciExpressCapabilityRegBlock (
             PciIoDevice,
             EFI_PCIE_CAPABILITY_ID_ARI,
             &PciIoDevice->AriCapabilityOffset
             );

  if (!EFI_ERROR (Status)) {
    Bridge = PciIoDevice->Parent;

    //
    // Check if its parent supports ARI forwarding.
    //
    Data32 = PciExpressRead32 (Bridge->Address +
                               Bridge->PciExpressCapabilityOffset +
                               EFI_PCIE_CAPABILITY_DEVICE_CAPABILITIES_2_OFFSET);

    if ((Data32 & EFI_PCIE_CAPABILITY_DEVICE_CAPABILITIES_2_ARI_FORWARDING) != 0) {
      //
      // ARI forward support in bridge, so enable it.
      //
      Data32 = PciExpressRead32 (Bridge->Address +
                                 Bridge->PciExpressCapabilityOffset +
                                 EFI_PCIE_CAPABILITY_DEVICE_CONTROL_2_OFFSET);

      if ((Data32 & EFI_PCIE_CAPABILITY_DEVICE_CONTROL_2_ARI_FORWARDING) == 0) {
        Data32 |= EFI_PCIE_CAPABILITY_DEVICE_CONTROL_2_ARI_FORWARDING;
        PciExpressWrite32 (Bridge->Address +
                           Bridge->PciExpressCapabilityOffset +
                           EFI_PCIE_CAPABILITY_DEVICE_CONTROL_2_OFFSET,
                           Data32);
      }
    }
  }
}
