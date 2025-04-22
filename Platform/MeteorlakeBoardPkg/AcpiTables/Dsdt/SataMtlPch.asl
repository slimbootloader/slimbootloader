/**@file
  Integrated SATA MTL PCH controller ACPI methods

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (SAT0) {
  Name (_ADR, 0x00170000)

  Method (_PS0,0,Serialized) {
    //
    // Clear LTR ignore bit for Sata on D0
    //
    Store(0, ISAT)
  }

  Method (_PS3, 0, Serialized) {}

  Include ("Sata.asl")
}