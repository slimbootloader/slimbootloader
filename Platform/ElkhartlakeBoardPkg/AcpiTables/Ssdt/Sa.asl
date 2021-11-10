/** @file
  This file contains the device definition of the System Agent
  ACPI reference code.
  Currently defines the device objects for the
  System Agent PCI Express* ports (PEG), iGfx and other devices.

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(\_SB.PC00, DeviceObj)
External(\_SB.PC00.GFX0, DeviceObj)
External(\_SB.PC00.IPU0, DeviceObj)
External(\_SB.PC00.B0D3, DeviceObj)
External(\_SB.PC00.PCIC, MethodObj)
External(\_SB.PC00.PCID, MethodObj)

Include ("SaNvs.asl")

///
/// I.G.D
///
Scope (\_SB.PC00.GFX0)
{
  include("Igfx.asl")
} // end I.G.D

///
/// IPU Device
///
include("Ipu.asl")
