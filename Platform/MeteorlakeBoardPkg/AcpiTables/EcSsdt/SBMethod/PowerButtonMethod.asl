/** @file
  ACPI EC SSDT table

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\OSYS)
External (\_SB.HIDD, DeviceObj)
External (\_SB.HIDD.BTLD)
External (\_SB.PC00.GFX0.IUER)

Method (PWPR, 0, Serialized) // Power Button Press Helper Method
{
  If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Power Button Press
    Notify (\_SB.HIDD, 0xCE) // Notify HID driver that Power button is pressed.
  } Else {
    If (LEqual (\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
      Notify (\_SB.PWRB, 0x80) // Send press notification to Power Button device
      Store (1, \_SB.PWRB.PBLV) // Keep track of Power Button Level
    }

    If (CondRefOf (\_SB.PC00.GFX0.IUER))
    {
      Store (\_SB.PC00.GFX0.IUER, Local0)
      And (Local0, 0xC0, \_SB.PC00.GFX0.IUER)  // Clear 4:0 button events on entry.
      Store (\_SB.PC00.GFX0.IUER, Local0)
      Or (Local0, 0x01, \_SB.PC00.GFX0.IUER)   // Set Power Button Status = pressed.
    }

  } //If Win10
} //End Method

Method (PWRR, 0, Serialized) // Power Button Release Helper Method
{
  If (LAnd (LGreaterEqual (\OSYS, 2015), \_SB.HIDD.BTLD)) { //Win10 and Button Driver loaded: Power Button Release
    Notify (\_SB.HIDD, 0xCF) // Notify HID driver that Power button is released.
  } Else {
    If (LEqual (\_SB.PWRB.PBST, 0x1)) { // Using Control Method Power Button
      \_SB.PWRB.PBUP () // Send release notification to Power Button device if requested
      Store (0, \_SB.PWRB.PBLV) //// Keep track of Power Button Level
    }

    If (CondRefOf (\_SB.PC00.GFX0.IUER))
    {
      Store (\_SB.PC00.GFX0.IUER, Local0)
      And (Local0, 0xC0, \_SB.PC00.GFX0.IUER)  // Clear 4:0 button events on entry.
    }

  } //If Win10
} //End Method
