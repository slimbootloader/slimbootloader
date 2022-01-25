/** @file
  ACPI DSDT table

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (\_SB.PC00.DOCK)
{

  Name(_HID, "ABCD0000")
  Name(_CID, EISAID("PNP0C15"))
  Name(_UID,"SADDLESTRING")

  If(LEqual(\S0ID,0)) {
    Method(_EJ0, 1) {
      P8XH(0,0xED)
      //
      // @todo: Add necessary Power/Hot removal procedure here.
      //
    }
  }

  Method(_STA, 0) {
    If(LEqual(ECON,0))
    {
      Return(0x0)
    }
    If(\DSTS) {
      Return(0x0F)
    } Else {
      Return(0x0)
    }
  }


 If(LEqual(\S0ID,0)) {
   Method(_DCK, 1) {
     If (Arg0) {
       Return (1)
     } Else {
       Store(0, \DSTS)
       Return (1)
     }
   }
 }

  Name(_EDL, Package( ) {   // DOCK1 has several dependent devices
    \_SB.PC00.RP07.PXSX,
    \_SB.PC00.RP08.PXSX,
    \_SB.PC00.SAT0,
    })

}
