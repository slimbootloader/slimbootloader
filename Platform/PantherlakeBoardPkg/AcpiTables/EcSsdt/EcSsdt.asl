/** @file
  ACPI EC SSDT table

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef PARENT_OF_LPCB
#define PARENT_OF_LPCB PC00
#endif

DefinitionBlock (
  "ECSSDT.aml",
  "SSDT",
  0x02,
  "EcRef",
  "EcSsdt ",
  0x0
  )
{
  External (ADBG, MethodObj)
  #define ADBG(x) \
  If (CondRefOf(ADBG)) {\
    ADBG(x) \
  }
  External (\_SB.ASRT, MethodObj)
  ADBG ("[EC SSDT][AcpiTableEntry]")
  External (\_SB.PARENT_OF_LPCB.LPCB, DeviceObj)
  External (\_SB.HIDD, DeviceObj)

  Include ("EcNvs.asl")

  Scope (\_SB.PARENT_OF_LPCB.LPCB) {
    Include ("EcDevice.asl")
  }
} // End EC SSDT
