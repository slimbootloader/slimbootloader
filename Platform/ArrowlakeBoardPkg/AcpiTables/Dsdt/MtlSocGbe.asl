/**@file
  Integrated GbE MTL SOC controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

If (LNotEqual (GBES, 0)) {
  Device (GLAN) {
    Name (_ADR, 0x001F0006)

    Name (_S0W, 3) // Device can wake itself from D3 in S0

    Method (_PRW, 0) {
      If (CondRefOf (GP1E)) {
        If (LNotEqual (GP1E, 0)) {
          Return (GPRW (GPE1_GBE_PME_B0, 4))
        }
      }
      Return (GPRW (0x6D, 4)) // can wakeup from S4 state
    }

    Include ("Gbe.asl")
  }
}
