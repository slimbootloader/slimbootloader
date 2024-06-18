/**@file
  CNVi MTL PCH controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Register/CnviRegs.h>

External(\_SB.BTRK, MethodObj)
External(\_SB.GBTR, MethodObj)
External(\CNMT)

Scope (\_SB.PC02) {
  If (LEqual (CRFP, 1)) {
    If (LEqual (CWFC, 1)) {
      Device (CNVW) {

        Name (_ADR, 0x00140003)
        Name (PU2C, 0)
        Store (^PU2C, PU2C)

        Method (BASE, 0) {
          Store (\_SB.PC00.PC2M (_ADR, PBSB), Local0)
          Return (Local0)    // Return PCIE Base address
        }

        Method(_PS0, 0, Serialized) {
          ADBG("CNVW Ctrlr D0")
        }

        Method(_PS3, 0, Serialized) {
          ADBG("CNVW Ctrlr D3")
        }

        Method (_PRW, 0) {
          Return (GPRW (GPE_PME, 4)) // can wakeup from S4 state
        }

        #define R_CNVI_ACPI_PLRB R_CNVI_VER4_PCR_CNVI_PLDR_ABORT
        #define P2SB_DEV \_SB.PC02.SBPC
        Include ("CnviWifi.asl")
        #undef R_CNVI_ACPI_PLRB
        #undef P2SB_DEV
      }
    }
  }
}

