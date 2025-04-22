/**@file
  CNVi MTL controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <PcieRegs.h>
#include <Register/CnviRegs.h>

External(\_SB.BTRK, MethodObj)
External(\_SB.GBTR, MethodObj)
External(\CNMT)
External(\PCRR, MethodObj)

Scope (\_SB.PC00) {
  If (LEqual (CRFP, 1)) {
    If (LEqual (CWFC, 1)) {
      Device (CNVW) {

        Name (_ADR, 0x00140003)
        Name (PU2C, 0)
        Store (\PU2C, PU2C)

        Method (BASE, 0) {
          Store (\_SB.PC00.PC2M (_ADR, 0), Local0)
          Return (Local0)    // Return PCIE Base address
        }

        Method (_PS0, 0, Serialized) {
          ADBG ("CNVW Ctrlr D0")
          // S0i2/3 dynamic switching for CNVi WiFi (Arg0 = 2), active (Arg1 = 1)
          \_SB.S023 (2, 1)
        }

        Method (_PS3, 0, Serialized) {
          ADBG ("CNVW Ctrlr D3")
          //
          // Check if:
          //  CNVI.PCR.STAT[5:0](CNVI_STATE) = 0xC
          //  CNVI.PCR.STAT[6](CNVI_STATE_VAL) = 1
          //
          Store (PCRR (\PCNV, R_CNVI_PCR_STAT), Local0)
          If (LEqual (And(Local0, 0x7F), 0x4C)) {
            // S0i2/3 dynamic switching for CNVi WiFi (Arg0 = 2), inactive (Arg1 = 0)
            \_SB.S023 (2, 0)
          }
        }

        Method (_PRW, 0) {
          If (CondRefOf (GP1E)) {
            If (LNotEqual (GP1E, 0)) {
              Return (GPRW (GPE1_CNVI_PME_B0, 4))
            }
          }
          Return (GPRW (0x6D, 4)) // can wakeup from S4 state
        }

        #define R_CNVI_ACPI_PLRB R_CNVI_VER2_PCR_CNVI_PLDR_ABORT
        #define P2SB_DEV \_SB.PC00.SBSC
        Include ("CnviWifi.asl")
        #undef R_CNVI_ACPI_PLRB
        #undef P2SB_DEV
      }
    }
  }
}

