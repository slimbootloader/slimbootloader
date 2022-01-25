/** @file
  This file contains the device definition of the System Agent
  ACPI reference code.
  Currently defines the device objects for the
  System Agent PCI Express* ports (PEG), iGfx and other devices.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(\_SB.PC00, DeviceObj)
External(\_SB.PC00.GFX0, DeviceObj)
External(\_SB.PC00.IPU0, DeviceObj)
External(\_SB.PC00.B0D3, DeviceObj)
External(\_SB.PC00.PCIC, MethodObj)
External(\_SB.PC00.PCID, MethodObj)

///
/// CPU PCIe Root Port
///
include("CpuPcieRp.asl")
include("PegCommon.asl")
