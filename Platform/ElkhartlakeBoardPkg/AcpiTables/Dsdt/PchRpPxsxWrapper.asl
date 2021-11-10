/** @file
  Intel ACPI Reference Code for Pch Rootport Endpoint Wrapper

 Copyright (c) 2013 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

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
