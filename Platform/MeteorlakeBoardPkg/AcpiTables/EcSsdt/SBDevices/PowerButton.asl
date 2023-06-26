/** @file
  ACPI EC SSDT table for Power Button.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (PWRB)
{
  External (\GGPE, MethodObj)
  External (\PPBG, IntObj)
  External (\_SB.PC00.LPCB.PRBL, FieldUnitObj)

  Name (_HID, EISAID ("PNP0C0C"))

  // The PRW isn't working when
  // placed in any of the logical locations ( PS2K, PS2M,
  // H_EC ), so a Power Button Device was created specifically
  // for the WAKETIME_SCI PRW.

  //
  // Power button status flag used to communicate H_EC.VBDL
  //
  Name (PBST, 1)

  //
  // Up Press Register flag. Set when OS register to recieve the up press of the power button
  //
  Name (UPPS, 0)

  //
  // Status of Power Button Level when EC is in mode where SCI is sent for both press and release of power button
  //
  Name (PBLV, 0)

  Method (PKG2, 2, Serialized) {
    Name (PKG, Package (2) {0, 0})
    Store (Arg0, Index (PKG, 0))
    Store (Arg1, Index (PKG, 1))
    Return (PKG)
  }

  Method (_PRW, 0) {
    Name (GPEB, 0)   // GPE status bit #

    // Get GPIO status bit location from PCH lib
    If (LNotEqual (PPBG, 0))
    {
      Store (GGPE (PPBG), GPEB)   // PM_PWRBTN is GPD, Pad 3
      Return (PKG2 (GPEB, 4))
    }
    Return (Package (){30, 4})       //GPI14 = GPE30 = Waketime SCI for Haswell Traditional boards
  }

  Method (_STA, 0)
  {
    If (LEqual (PBST, 1)){
      Return (0x0F)
    }
    Return (0x00)
  }

  Method (PBUP, 0) {
    If (UPPS) {
      Notify (\_SB.PWRB, 0xC0) // Send release notification to Power Button device
    }
  }

  //
  // _DSM : Device Specific Method for the Power Button.
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
  Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
    // Compare passed in UUID to supported UUID.
    If (LEqual (Arg0, ToUUID ("9C355BCB-35FA-44f7-8A67-447359C36A03")))
    {
      If (LEqual (0, ToInteger (Arg1)))        // Revision 0.
      {
        Switch (ToInteger (Arg2)) {           // Switch to Function Index.
          //
          // Function 0, Query of supported functions.
          //
          Case (0) {
            // Need check for platfroms that support this
            Return (Buffer () {0x07}) // Supports Function 1, 2.
          }

          //
          // Function 1, Register for Power Button Release Notify.
          //
          Case (1) {
            Store (0x1, UPPS) // OS has registered to recieve notify on release of power button
            //
            // Place EC into mode where SCI is sent for both press and release of power button
            //
            \_SB.PC00.LPCB.H_EC.DPBN ()
            Return (0x00)
          }

          //
          // Function 2, Power Button Level.
          //
          Case (2) {
            If (UPPS) { // If OS has registered to recieve notify on release of power button
              If (LNot (PBLV)) {
                return (0x00) // Power button not pressed
              } else {
                return (0x01) // Power button pressed
              }
            } else {
              If (\_SB.PC00.LPCB.PRBL) {
                return (0x00) // Power button not pressed
              } else {
                return (0x01) // Power button pressed
              }
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


}//end device PWRB
