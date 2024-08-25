/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include "InternalPciEnumerationLib.h"
#include "PciCommand.h"

/**
  This function is used to program the Resizable BAR Register.

  @param PciIoDevice            A pointer to the PCI_IO_DEVICE.
  @param ResizableBarOp         PciResizableBarMax: Set BAR to max size
                                PciResizableBarMin: set BAR to min size.

  @retval EFI_SUCCESS           Successfully enumerated the host bridge.
  @retval other                 Some error occurred when enumerating the host bridge.

**/
STATIC
EFI_STATUS
PciProgramResizableBar (
  IN PCI_IO_DEVICE                *PciIoDevice,
  IN PCI_RESIZABLE_BAR_OPERATION  ResizableBarOp
  )
{
  UINT64                                                 Capabilities;
  UINT32                                                 Index;
  UINT32                                                 Offset;
  INTN                                                   Bit;
  UINTN                                                  ResizableBarNumber;
  PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_ENTRY  Entries[PCI_MAX_BAR];

  ASSERT (PciIoDevice->ResizableBarOffset != 0);

  DEBUG ((DEBUG_VERBOSE, "Programs Resizable BAR register, address %x offset: 0x%08x, number: %d\n",
                        PciIoDevice->Address,
                        PciIoDevice->ResizableBarOffset,
                        PciIoDevice->ResizableBarNumber));
  ResizableBarNumber = MIN (PciIoDevice->ResizableBarNumber, PCI_MAX_BAR);
  for (Index = 0; Index < ResizableBarNumber; Index++) {
    Offset = PciIoDevice->ResizableBarOffset + sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER) +
             sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_ENTRY) * Index;
    Entries[Index].ResizableBarCapability.Uint32 = PciExpressRead32 (PciIoDevice->Address + Offset);
    Offset += sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_CAPABILITY);
    Entries[Index].ResizableBarControl.Uint32 = PciExpressRead32 (PciIoDevice->Address + Offset);
  }

  for (Index = 0; Index < ResizableBarNumber; Index++) {
    //
    // When the bit of Capabilities Set, indicates that the Function supports
    // operating with the BAR sized to (2^Bit) MB.
    // Example:
    // Bit 0 is set: supports operating with the BAR sized to 1 MB
    // Bit 1 is set: supports operating with the BAR sized to 2 MB
    // Bit n is set: supports operating with the BAR sized to (2^n) MB
    //
    Capabilities = LShiftU64 (Entries[Index].ResizableBarControl.Bits.BarSizeCapability, 28)
                   | Entries[Index].ResizableBarCapability.Bits.BarSizeCapability;

    if (ResizableBarOp == PciResizableBarMax) {
      Bit = HighBitSet64 (Capabilities);
    } else {
      ASSERT (ResizableBarOp == PciResizableBarMin);
      Bit = LowBitSet64 (Capabilities);
    }

    ASSERT (Bit >= 0);

    Offset = PciIoDevice->ResizableBarOffset + sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER)
             + Index * sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_ENTRY)
             + OFFSET_OF (PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_ENTRY, ResizableBarControl);

    Entries[Index].ResizableBarControl.Bits.BarSize = (UINT32)Bit;
    DEBUG ((DEBUG_VERBOSE, "Resizable Bar: Offset = 0x%x, Bar Size Capability = 0x%016lx, New Bar Size = 0x%lx\n",
                            OFFSET_OF (PCI_TYPE00, Device.Bar[Entries[Index].ResizableBarControl.Bits.BarIndex]),
                            Capabilities,
                            LShiftU64 (SIZE_1MB, Bit)));
    PciExpressWrite32 (PciIoDevice->Address + Offset, Entries[Index].ResizableBarControl.Uint32);
  }
  return EFI_SUCCESS;
}

/**
  Initialize Resizable BAR

  @param[in,out]  PciIoDevice     Pointer to instance of PCI_IO_DEVICE.

**/
VOID
InitializeResizeBar (
  IN OUT  PCI_IO_DEVICE              *PciIoDevice
  )
{
  EFI_STATUS    Status;

  PciIoDevice->ResizableBarOffset = 0;
  Status = LocatePciExpressCapabilityRegBlock (
              PciIoDevice,
              PCI_EXPRESS_EXTENDED_CAPABILITY_RESIZABLE_BAR_ID,
              &PciIoDevice->ResizableBarOffset
              );
  if (!EFI_ERROR (Status)) {
    PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_CONTROL  ResizableBarControl;
    UINT32                                                   Offset;

    Offset = PciIoDevice->ResizableBarOffset + sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_HEADER) +
          sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_RESIZABLE_BAR_CAPABILITY);
    ResizableBarControl.Uint32 = PciExpressRead32(PciIoDevice->Address + Offset);
    PciIoDevice->ResizableBarNumber = ResizableBarControl.Bits.ResizableBarNumber;
    DEBUG ((DEBUG_VERBOSE, "PciResizableBarNumber %d ResizableBarControl %x \n", PciIoDevice->ResizableBarNumber, ResizableBarControl.Uint32));
    PciProgramResizableBar (PciIoDevice, PciResizableBarMax);
  }
}
