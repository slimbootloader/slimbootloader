/** @file
  ACPI DSDT table

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.PC00.LPCB.H_EC.PG3E, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PG3X, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.RPWR, FieldUnitObj)
External (PGED, IntObj)


Device (ISCT) {
  Name (_HID, "INTC109B")         // ICT Tools for ADL+
  Name (_CID,EISAID ("PNP0C02"))  // Motherboard resource

  Method (_STA) {
    //
    // Checking PGED setup option status.
    //
    If (LEqual (PGED, 1)) {
      //
      // Checking PG3D status. PG3D only works with DC mode.
      //
      If (LEqual (\_SB.PC00.LPCB.H_EC.ECRD (RefOf (\_SB.PC00.LPCB.H_EC.RPWR)),0)) {
        //
        // PG3D only works with DC mode (battery connect).
        //
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    } else {
      Return (0x00)
      }
    }
  //
  // _DSM : Device Specific Method for PG3 data.
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
  Method (_DSM, 0x4, Serialized) {
    //
    // Compare passed in UUID to supported UUID.
    //
    If (LEqual (Arg0, ToUUID ("F9D5C712-7AF1-4F60-B9F9-66B921C952B9"))) {
      If (LEqual (1, ToInteger (Arg1))) {      // Revision 1.
        Switch (ToInteger (Arg2)) {            // Switch to Function Index.
          Case (1) {
            If (LAnd (LEqual (\_SB.PC00.LPCB.H_EC.ECRD (RefOf (\_SB.PC00.LPCB.H_EC.PG3E)), 1), LEqual (\_SB.PC00.LPCB.H_EC.ECRD (RefOf (\_SB.PC00.LPCB.H_EC.PG3X)), 1))) {
              Return (0x01)
            } else {
              Return (0x00)
            }
          }
        } // End Switch statement
      } // End Revision check
    } // End UUID check

    //
    // If the code falls through to this point, just return a buffer of 0.
    //
    Return (Buffer () {0x00})
  } // End _DSM Method
}
