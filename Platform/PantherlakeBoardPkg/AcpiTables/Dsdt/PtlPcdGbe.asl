/**@file
  Integrated GbE PTL SOC controller ACPI methods

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

If (LNotEqual (GBES, 0)) {
  Device (GLAN) {
    Name (_ADR, 0x001F0006)

    Name (_S0W, 3) // Device can wake itself from D3 in S0

    Method (_PRW, 0) {
      Return (GPRW (GPE1_GBE_PME_B0, 4)) // can wakeup from S4 state
    }

    Method (_PS3, 0, NotSerialized)
    {
      ADBG("GBE _PS3")
    }

    Method (_PS0, 0, NotSerialized)
    {
      ADBG("GBE _PS0")
    }

    Include ("Gbe.asl")
  }
}
