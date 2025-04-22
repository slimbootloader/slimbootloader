/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name(B0SC, 100) // Battery 0 Stored Capacity

Method(_Q53)
{
    VBSE()    // Virtual Battery Switch Event, remove if not needed.
}


// Virtual Battery Switch Event:
//    This method handles the Virtual Battery switch on the CRB.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(VBSE,0,Serialized) // called from EC.asl
{
    // Check for Virtual Battery Mode.  If the CRB is
    // running on real batteries, skip this whole section.

    If(LEqual(\_SB.PC00.LPCB.H_EC.BNUM,0))
    {
      // Check that the current and NVS Power States
      // are different.  The 2 may be out of sync due to
      // Virtual Battery hot-key support.

      If(LNotEqual(ECRD(RefOf(VPWR)),PWRS))
      {
        // Update NVS Power State.

        Store(ECRD(RefOf(VPWR)),PWRS)

        // Perform needed ACPI Notifications.

        PNOT()
      }
    }
}


//
// called from EC.asl _Q56 method
//
// Virtual Battery Hot Keys:
//    This method passes the hot key scan code to the BTTM method.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(VBHK,0,Serialized)
{
      // 1/! Scan Code = 0x02

      Store(ECRD(RefOf(SCCK)), Local0)

      If(LEqual(Local0,0x02)) //  Battery = 10%.
      {
        BTTM(10)
      }

      // 2/@ Scan Code = 0x03

      If(LEqual(Local0,0x03)) //  Battery = 20%.
      {
        BTTM(20)
      }

      // 3/# Scan Code = 0x04

      If(LEqual(Local0,0x04)) //  Battery = 30%.
      {
        BTTM(30)
      }

      // 4/$ Scan Code = 0x05

      If(LEqual(Local0,0x05)) //  Battery = 40%.
      {
        BTTM(40)
      }

      // 5/% Scan Code = 0x06

      If(LEqual(Local0,0x06)) //  Battery = 50%.
      {
        BTTM(50)
      }

      // 6/^ Scan Code = 0x07

      If(LEqual(Local0,0x07)) //  Battery = 60%.
      {
        BTTM(60)
      }

      // 7/& Scan Code = 0x08

      If(LEqual(Local0,0x08)) //  Battery = 70%.
      {
        BTTM(70)
      }

      // 8/* Scan Code = 0x09

      If(LEqual(Local0,0x09)) //  Battery = 80%.
      {
        BTTM(80)
      }

      // 9/( Scan Code = 0x0A

      If(LEqual(Local0,0x0A)) //  Battery = 90%.
      {
        BTTM(90)
      }

      // 0/) Scan Code = 0x0B

      If(LEqual(Local0,0x0B)) // Battery = 100%.
      {
        BTTM(100)
      }

      // _/- Scan Code = 0x0C

      If(LEqual(Local0,0x0C)) // Battery = Battery - 2%.
      {
        If(LGreaterEqual(B0SC,2))
        {
          BTTM(Subtract(B0SC,2))
        }
      }

      // +/= Scan Code = 0x0D

      If(LEqual(Local0,0x0D)) // Battery = Battery + 2%.
      {
        If(LLessEqual(B0SC,98))
        {
          BTTM(Add(B0SC,2))
        }
      }

      // F5 Scan Code = 0x3F

      If(LEqual(Local0,0x3F))   // Virtual Power State Change.
      {
      // Check for Virtual Battery Mode.  If the CRB is
      // running on real batteries, skip this whole section.

        If(LEqual(\_SB.PC00.LPCB.H_EC.BNUM,0))
        {
          // Toggle the Virtual Power State.
          Xor(PWRS,1,PWRS)

          // Perform needed ACPI Notifications.
          PNOT()
        }
      }
}


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

  // Define the Virtual Battery 0 Control Method.

  Device(BAT0)
  {
    Name(_HID,EISAID("PNP0C0A"))

    Name(_UID,0)

    Method(_STA,0)
    {
      If (And(BATP, BIT1))   // Virtual Battery is supported.
      {
        If (And(\_SB.PC00.LPCB.H_EC.BNUM, 3))   // Real Battery 1 or 2 present?
        {
          Return (0x000B)  // Yes.  Hide Virtual.
        } Else {
          Return (0x001F)    // No.  Show Virtual.
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
        // Return Charging.

        Store(2,Index(PKG0,0))
      }
      Else
      {
        // Return Discharging.

        Store(1,Index(PKG0,0))
      }
      Store(0x7FFFFFFF,Index(PKG0,1))
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

