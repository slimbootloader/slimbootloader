/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/



// LPC Bridge - Device 31, Function 0
// Define the needed LPC registers used by ASL.

scope(\_SB) {
  OperationRegion(ILBR, SystemMemory, \IBAS, 0x8C)
  Field(ILBR, AnyAcc, NoLock, Preserve) {
    Offset(0x08), // 0x08
    PARC,   8,
    PBRC,   8,
    PCRC,   8,
    PDRC,   8,
    PERC,   8,
    PFRC,   8,
    PGRC,   8,
    PHRC,   8,
    Offset(0x88), // 0x88
    ,       4,
    UI4E,   1
  }

  Include ("Link98.asl")
}
// LPC Bridge - Device 31, Function 0
scope (\_SB.PCI0.LPCB) {

  Include ("LpcDev.asl")

} //end of SCOPE