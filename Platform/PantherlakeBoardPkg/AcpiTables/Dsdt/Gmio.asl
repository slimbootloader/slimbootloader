/**@file
  Intel ACPI Sample Code for GMIO method Implementation

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.PC00.PC2M, MethodObj)

//
// PCIe endpoint PCI config space to MMIO address
//
// Arg0 - Bus number of a root port.
// Arg1 - _ADR of the target root port controller.
// Arg2 - _ADR of the target endpoint device.
//
Method (GMIO, 3, Serialized) { // Get Memory Mapped I/O address
  // Bus number will be grabbed from the root port. (under the scope where this method exists)
  // Root port PCI config space definition
  If (CondRefOf (\_SB.PC00.PC2M)) {
    OperationRegion (PXCS, SystemMemory, \_SB.PC00.PC2M (Arg1, Arg0), 0x20) // PCI eXpress Configuration Space
    Field (PXCS, AnyAcc, NoLock, Preserve) {
      Offset (0x18), // Bus Number register
      PBUS, 8,       // Primary Bus Number
      SBUS, 8,       // Subordinate Bus Number
    }
  }
  If (CondRefOf (\_SB.PC00.GPCB)) {
    Store (\_SB.PC00.GPCB (), Local0) // MMIO Base address
  }
  Add (Local0, ShiftRight (And (Arg2, 0x001F0000), 1), Local0) // Device no
  Add (Local0, ShiftLeft (And (Arg2, 0x00000007), 12), Local0) // Function no
  Add (Local0, ShiftLeft (SBUS, 20), Local0)                   // Bus no

  Return (Local0)
}