/** @file
  TSE ACPI

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


DefinitionBlock (
  "TseSsdt.aml",
  "SSDT",
  2,
  "INTEL ",
  "TseSsdt",
  0x1000
  )
{
  External (TSEE) // TSEE Device Enable
  External (\_SB.PC00, DeviceObj)
  External (\_SB.PC00.GMHB, MethodObj)        // external method to access MCHBAR

  Include ("Tse.asl")
} // End of definition block
