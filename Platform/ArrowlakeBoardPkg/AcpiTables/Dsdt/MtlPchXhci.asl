/**@file
  Integrated XHCI MTL PCH controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (XHCI) {

  Name (_ADR, 0x00140000)
  Name (DUWS, 0)
  Name (PU2C, 0)
  Name (PU3C, 0)
  Name (AHWA, 1) // Advertise HSII workaround to OS driver
  Name (PPID, 0x4F)
  Store (\DUWS, DUWS)
  Store (^PU2C, PU2C)
  Store (^PU3C, PU3C)

  Method (_PRW, 0) {
    If (CondRefOf (DUWS)) {
      Return (GPRW (GPE_PME, DUWS)) // can wakeup from deepest USB Sleep Wake State
    } Else {
      Return (GPRW (GPE_PME, 4)) // can wakeup from S4 state
    }
  }
  #define P2SB_DEV \_SB.PC02.SBPC
  Include ("PchXhci.asl")
  #undef P2SB_DEV
}
