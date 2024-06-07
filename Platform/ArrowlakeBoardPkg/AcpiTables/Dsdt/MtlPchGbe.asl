/**@file
  Integrated GbE MTL PCH controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  Device (GLAN) {
    Name (_ADR, 0x001F0006)

    Name (_S0W, 3) // Device can wake itself from D3 in S0

    Method (_PRW, 0) {
      Return (GPRW (GPE_PME, 4)) // can wakeup from S4 state
    }

    Include ("Gbe.asl")
  }
