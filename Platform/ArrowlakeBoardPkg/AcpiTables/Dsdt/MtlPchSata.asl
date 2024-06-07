/**@file
  Integrated SATA MTL PCH controller ACPI methods

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (SAT0) {
  Name (_ADR, 0x00170000)

  Method (_PS0,0,Serialized) {
    //
    // Clear LTR ignore bit for Sata on D0
    //
    Store(0, \_SB.PC02.PPMC.ISAT)
    ADBG (Concatenate ("SATA PS0 LTR ignore bit status = ", ToHexString (\_SB.PC02.PPMC.ISAT)))
  }

  Method (_PS3, 0, Serialized) {
    //
    // Clear LTR ignore bit for Sata on D0
    //

     Store(1, \_SB.PC02.PPMC.ISAT)
     ADBG (Concatenate ("SATA PS3 LTR ignore bit status = ", ToHexString (\_SB.PC02.PPMC.ISAT)))
  }

  Include ("Sata.asl")
}