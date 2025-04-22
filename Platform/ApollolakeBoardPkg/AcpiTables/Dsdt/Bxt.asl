/** @file
  ACPI DSDT table

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.PCI0) {
  //
  // Integrated Sensor Hub - Device 17, Function 0
  //
  Device(ISH0) {
    Name(_ADR, 0x00110000)
    Method (_STA, 0, NotSerialized) {
      Return (0x0F)
    }

    //--------------------
    //  Intel Proprietary Wake up Event solution
    //--------------------
    Method(_DSM, 0x4, Serialized)
    {
      If(LEqual(Arg0, ToUUID("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37"))) {
        // Function 0 : Query Function
        If(LEqual(Arg2, Zero)) {
          // Revision 0
          If(LEqual(Arg1, Zero)) {
            Return(Buffer(One) { 0x03 }) // There are 1 function defined other than Query.
          } Else {
            Return(0) // Revision mismatch
          }
        }
        // Function 1 :
        Return(0)

      } Else {
        Return(Buffer(One) { 0x00 }) // Guid mismatch
      }
    }

    Method (_PS0, 0, NotSerialized) { // _PS0: Power State 0
    }

    Method (_PS3, 0, NotSerialized) { // _PS3: Power State 3
    }

  }

}//end scope
