/** @file
  ACPI DSDT table

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
External (\_SB.PC00.SAT0, DeviceObj)
External (\_SB.PC00.LPCB.H_EC, DeviceObj)

Device (\_SB.PC00.DOCK)
{

  Name (_HID, "ABCD0000")
  Name (_CID, EISAID ("PNP0C15"))
  Name (_UID,"VIRTUALDOCK")

  If (LEqual (\S0ID,0)) {
    Method (_EJ0, 1) {
      P8XH (0,0xED)
      //
      // @todo: Add necessary Power/Hot removal procedure here.
      //
    }
  }

  Method (_STA, 0) {
    ADBG ("Method _STA")
    If (LNot (CondRefOf (\_SB.PC00.LPCB.H_EC)))
    {
      Return (0x0)
    }
    If (\DSTS) {
      ADBG ("Virtual Dock Attached")
      Return (0x0F)
    } Else {
      ADBG ("Virtual Dock Unattahced")
      Return (0x0)
    }
  }


  If (LEqual (\S0ID,0)) {
    Method (_DCK, 1) {
      ADBG ("Method _DCK")
      If (Arg0) {
        Return (1)
      } Else {
        Store (0, \DSTS)
        Return (1)
      }
    }
  }

  If (CondRefOf (\_SB.PC00.SAT0)) { // Check if SATA Controller Supported
    Name (_EDL, Package ( ) {    // DOCK1 has several dependent devices
      \_SB.PC00.SAT0,
    })
  }

}
