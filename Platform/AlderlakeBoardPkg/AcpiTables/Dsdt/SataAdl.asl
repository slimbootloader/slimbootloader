/**@file
  Integrated SATA ADL controller ACPI methods

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.PC00) {
  Device (SAT0) {
    Name(_ADR, 0x00170000)

    Method(_PS0,0,Serialized) {
      //
      // Call CSD0 only if ST_FDIS_PMC == 0
      //
      If(LEqual(PCHS, PCHP)) {
        If(LNot(SCFD)){
          \_SB.CSD0(MODPHY_SPD_GATING_SATA)
        }
      }
      //
      // Clear LTR ignore bit for Sata on D0
      //
      Store(0, ISAT)
    }

    Method(_PS3,0,Serialized) {
      //
      // Set LTR ignore bit for Sata on D3
      //
      Store(1, ISAT)
      If(LEqual(PCHS, PCHP)) {
        \_SB.CSD3(MODPHY_SPD_GATING_SATA)
      }
    }

    Include ("Sata.asl")
  }
}
