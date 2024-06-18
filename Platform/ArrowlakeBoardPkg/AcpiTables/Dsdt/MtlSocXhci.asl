/**@file
  Integrated XHCI MTL SOC controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (XHCI) {

  Name (_ADR, 0x00140000)
  Name (DUWS, 0)
  Name (PU2C, 0)
  Name (PU3C, 0)
  Name (AHWA, 1) // Advertise HSII workaround to OS driver
  Name (PPID, 0x3A)
  Method (_PRW, 0) {
    If (CondRefOf (DUWS)) {
      If (CondRefOf (GP1E)){
        If (LNotEqual (GP1E, 0)) {
          Return (GPRW (GPE1_XHCI_PME_B0, DUWS))
        }
      }
    Return (GPRW (0x6D, DUWS)) // can wakeup from deepest USB Sleep Wake State
    } Else {
      If (CondRefOf (GP1E)){
        If (LNotEqual (GP1E, 0)) {
          Return (GPRW (GPE1_XHCI_PME_B0, 4))
        }
      }
      Return (GPRW (0x6D, 4)) // can wakeup from S4 state
    }
  }

  Store (\DUWS, DUWS)
  Store (\PU2C, PU2C)
  Store (\PU3C, PU3C)
  #define P2SB_DEV \_SB.PC00.SBSC
  Include ("PchXhci.asl")
  #undef P2SB_DEV
}
