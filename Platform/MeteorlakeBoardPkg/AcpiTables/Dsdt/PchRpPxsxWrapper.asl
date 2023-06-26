/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
External (PBNU, IntObj)
//
// PCIe endpoint PCI config space to MMIO address
//
// Arg0 - Bus number of the root port
// Arg1 - _ADR of the target root port controller.
// Arg2 - _ADR of the target endpoint device.
//
Method (GMIO, 3, Serialized) { // Get Memory Mapped I/O address
  // Bus number will be grabbed from the root port. (under the scope where this method exists)
  // Root port PCI config space definition
  OperationRegion (PXCS, SystemMemory, \_SB.PC00.PC2M (Arg1, Arg0), 0x20) // PCI eXpress Configuration Space
  Field (PXCS, AnyAcc, NoLock, Preserve) {
    Offset (0x18), // Bus Number register
    PBUS, 8,      // Primary Bus Number
    SBUS, 8,      // Subordinate Bus Number
  }
  Store (\_SB.PC00.GPCB (), Local0) // MMIO Base address
  Add (Local0, ShiftRight (And (Arg2,0x001F0000), 1), Local0)  // Device no
  Add (Local0, ShiftLeft (And (Arg2, 0x00000007), 12), Local0) // Function no
  Add (Local0, ShiftLeft (SBUS, 20), Local0)                   // Bus no

  Return (Local0)
}

Scope (\_SB.PC00.RP01.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP01
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP02.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP02
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP03.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP03
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP04.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP04
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP05.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP05
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP06.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP06
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP07.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP07
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP08.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP08
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP09.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP09
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP10.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP10
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP11.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP11
  Include ("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP12.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP12
  Include ("DiscreteConnectivity.asl")
}