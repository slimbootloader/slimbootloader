/** @file
  ACPI EC SSDT table for Ambient Light Sensor.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (ALSD)
{
  External (\ALSE, IntObj)
  External (\LHIH, IntObj)
  External (\LLOW, IntObj)

  Name (_HID, "ACPI0008")

  Method (_STA, 0)
  {
    If (LEqual (\ALSE, 2))
    {
      Return (0x000B)    // ALS Enabled.  Don't show it in UI.
    }

    Return (0x0000)      // ALS Disabled.  Hide it.
  }

  Method (_ALI)
  {
    Return (Or (ShiftLeft (\LHIH, 8), \LLOW))
  }

  Name (_ALR, Package ()
  {
    Package () {70,  0},
    Package () {73,  10},
    Package () {85,  80},
    Package () {100, 300},
    Package () {150, 1000}
  })

}
