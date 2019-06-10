/** @file
  This file contains the device definition of the System Agent
  ACPI reference code.
  Currently defines the device objects for the
  System Agent PCI Express* ports (PEG), iGfx and other devices.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

External(\_SB.PCI0, DeviceObj)
External(\_SB.PCI0.GFX0, DeviceObj)
External(\_SB.PCI0.IPU0, DeviceObj)
External(\_SB.PCI0.B0D3, DeviceObj)
External(\_SB.PCI0.PCIC, MethodObj)
External(\_SB.PCI0.PCID, MethodObj)

///
/// I.G.D
///
Scope (\_SB.PCI0.GFX0)
{
  include("Igfx.asl")
} // end I.G.D
