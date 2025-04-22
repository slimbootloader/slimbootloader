/** @file
  ACPI EC SSDT table for AC device.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Define an AC Device for ULT, ULX and Halo Platforms.
//
Device (ADP1)
{
  External (\PWRS, FieldUnitObj)
  External (\ADAS, IntObj)

  Name (_HID,"ACPI0003")

  Method (_STA)
  {
    If (LEqual (ADAS, 0)){
      Return (0x00)
    }
    Return (0x0F)
  }

  // Return the value that determines if running
  // from AC or not.

  Method (_PSR, 0)
  {
    Return (\PWRS)
  }

  // Return that everything runs off of AC.

  Method (_PCL, 0)
  {
    Return (
      Package () { _SB }
    )
  }
}
