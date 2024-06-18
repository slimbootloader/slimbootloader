/** @file
  ACPI EC SSDT table

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define ADBG(x)

DefinitionBlock (
  "ECSSDT.aml",
  "SSDT",
  0x02,
  "EcRef",
  "EcSsdt ",
  0x0
  )
{
  External (\_SB.PC00.LPCB, DeviceObj)
  External (\_SB.HIDD, DeviceObj)

  Include ("EcNvs.asl")

  Scope (\_SB.PC00.LPCB) {
    Include ("EcDevice.asl")
  }

  //
  // System Bus
  //
  Scope (\_SB) {
    Include ("SBMethod/PowerButtonMethod.asl")
    Include ("SBDevices/PowerButton.asl")
    Include ("SBDevices/ACDevice.asl")
    Include ("SBDevices/Pg3d.asl")
  }

  Scope (\_SB.HIDD) {
    Include ("HID/HidMethod.asl")
  }

  Include ("HIDWakeDSM.asl")
} // End EC SSDT
