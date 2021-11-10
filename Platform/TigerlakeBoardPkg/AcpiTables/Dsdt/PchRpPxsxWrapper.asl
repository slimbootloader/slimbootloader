/** @file
  Intel ACPI Reference Code for Pch Rootport Endpoint Wrapper

  Copyright (c) 2013 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// PCIe endpoint PCI config space to MMIO address
//
// Arg0 - _ADR of the target root port controller.
// Arg1 - _ADR of the target endpoint device.
//
Method (GMIO, 2, Serialized) { // Get Memory Mapped I/O address
  // Bus number will be grabbed from the root port. (under the scope where this method exists)
  // Root port PCI config space definition
  OperationRegion (PXCS, SystemMemory, \_SB.PC00.PC2M(Arg0), 0x20) // PCI eXpress Configuration Space
  Field (PXCS, AnyAcc, NoLock, Preserve) {
    Offset(0x18), // Bus Number register
    PBUS, 8,      // Primary Bus Number
    SBUS, 8,      // Subordinate Bus Number
  }
  Store (\_SB.PC00.GPCB(), Local0) // MMIO Base address
  Add (Local0, ShiftRight(And(Arg1,0x001F0000), 1), Local0)  // Device no
  Add (Local0, ShiftLeft(And(Arg1, 0x00000007), 12), Local0) // Function no
  Add (Local0, ShiftLeft(SBUS, 20), Local0)                  // Bus no

  Return (Local0)
}

Scope (\_SB.PC00.RP01.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP01
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP02.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP02
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP03.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP03
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP04.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP04
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP05.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP05
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP06.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP06
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP07.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP07
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP08.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP08
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP09.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP09
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP10.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP10
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP11.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP11
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP12.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP12
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP13.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP13
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP14.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP14
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP15.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP15
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP16.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP16
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP17.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP17
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP18.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP18
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP19.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP19
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP20.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP20
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP21.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP21
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP22.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP22
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP23.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP23
  Include("DiscreteConnectivity.asl")
}

Scope (\_SB.PC00.RP24.PXSX)
{
  #undef PCIE_ROOT_PORT
  #define PCIE_ROOT_PORT \_SB.PC00.RP24
  Include("DiscreteConnectivity.asl")
}
