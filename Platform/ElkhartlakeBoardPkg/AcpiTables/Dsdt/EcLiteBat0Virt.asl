/** @file
  ACPI DSDT table

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope (\_SB)
{
  Name(B0SC, 90) // Battery 0 Stored Capacity
  Name(ECPR, 0) // Eclite Present or not

  // Battery Test Method:
  //    This method updates the Virtual Battery percentages and
  //    notifies the OS of these changes.
  //
  //  Arguments:
  //    Arg0: The desired Virtual Battery Percentage.
  //
  //  Return Value:
  //    None
  Method(BTTM,1,Serialized)
  {
    If(PWRS)
    {
      If(LGreaterEqual(Arg0,B0SC))
      {
        Store(Arg0,B0SC)
        Notify(BAT0,0x80)
      }
    }
    Else
    {
      If(LLessEqual(Arg0,B0SC))
      {
        Store(Arg0,B0SC)
        Notify(BAT0,0x80)
      }
    }
  }

  //
  // Method to Handle AC/DC Virtual Switch
  //
  Method (ADCS, 1, Serialized) {
    Store (0xF, Local0)
    //
    // Read AC/DC Switch GPIO RX State
    // Expecting either value of 0 or 1.
    //
    If (CondRefOf (\_SB.GGIV)) {
      Store (\_SB.GGIV(\VBSG), Local0)
    }

    If (LEqual (Local0, 0xF)) {
      Return ()
    }

    If (LAnd(ECPR, LEqual(\_SB.ECLT.BNUM, 0))) {
      If (LNot (Local0)) {
        Store(ECLITE_DC_PRESENT,PWRS)
      } Else {
        Store(ECLITE_AC_PRESENT,PWRS)
      }

      If(Arg0) {
        // Perform needed ACPI Notifications.
        Notify(BAT0,0x80)
      }
    }
    Return ()
  }

  // Define the Virtual Battery 0 Control Method.
  Device(BAT0)
  {
    Name(_HID,EISAID("PNP0C0A"))
    Name(_UID,0)

    Method(_STA,0)
    {
      If (CondRefOf (\_SB.ECLT))
      {
        Store(1, ECPR)
      } Else {
        Store(0, ECPR)
      }

      If (And(BATP, BIT1)) {          // Virtual Battery is supported.
        If(LNotEqual(VBSG,0)) {
          ADCS(0)
        }

        If (LAnd(ECPR, And(\_SB.ECLT.BNUM, 3))) { // Real Battery 1 or 2 present?
          Return (0x0000)           // Yes.  Disable Virtual.
        } Else {
          Return (0x001F)           // No.  Show Virtual.
        }
      }
      Return (0)
    }

    Method(_BIF,0)
    {
      Return(Package() {
        0x00000000, // Power Unit = mWh.
        0x00002710, // 10000 mWh Design Capacity.
        0x00002710, // 10000 mWh Last Full Charge.
        0x00000001, // Secondary Battery Technology.
        0xFFFFFFFF, // Unknown Design Voltage.
        0x000003E8, // 10% Warning Level.
        0x00000190, // 4% Low Level.
        0x00000064, // 1% Granularity Level 1.
        0x00000064, // 1% Granularity Level 2.
        "CRB Battery 0",
        "Battery 0",
        "Fake",
        "-Virtual Battery 0-"})
    }

    Method(_BST,0, Serialized)
    {
      Name(PKG0,Package() {
        0xFFFFFFFF, // Battery State.
        0xFFFFFFFF, // Battery Present Rate.
        0xFFFFFFFF, // Battery Remaining Capacity.
        0xFFFFFFFF  // Battery Present Voltage.
      })

      // Virtual AC/Battery Check.
      If(PWRS)
      {
        //Return charging.
        Store(2,Index(PKG0,0))
      }
      Else
      {
        // Return Charging.
        Store(1,Index(PKG0,0))
      }
      Store(650,Index(PKG0,1))
      Store(Multiply(B0SC,100),Index(PKG0,2))
      Return(PKG0)
    }

    // Return that everything runs off Battery.
    Method(_PCL,0)
    {
      Return (
        Package() { _SB }
      )
    }
  }
}

