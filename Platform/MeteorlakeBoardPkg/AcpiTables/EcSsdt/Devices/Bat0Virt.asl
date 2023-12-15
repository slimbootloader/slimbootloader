/** @file
  ACPI EC SSDT table for Virtual Battery 0.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Define the Virtual Battery 0 Control Method.
//
Device (BAT0)
{
  External (\PWRS, FieldUnitObj)

  Name (_HID, EISAID ("PNP0C0A"))

  Name (_UID, 0)

  Method (_STA, 0)
  {
    If (And (BATS, BIT1))   // Virtual Battery is supported.
    {
      If (And (BNUM, 3))   // Real Battery 1 or 2 present?
      {
        Return (0x000B)  // Yes.  Hide Virtual.
      } Else {
        Return (0x001F)    // No.  Show Virtual.
      }
    }
    Return (0)
  }

  Method (_BIF, 0)
  {
    Return (Package () {
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

  Method (_BST, 0, Serialized)
  {
    Name (PKG0, Package () {
      0xFFFFFFFF, // Battery State.
      0xFFFFFFFF, // Battery Present Rate.
      0xFFFFFFFF, // Battery Remaining Capacity.
      0xFFFFFFFF  // Battery Present Voltage.
    })

    // Virtual AC/Battery Check.

    If (\PWRS)
    {
      // Return Charging.

      Store (2, Index (PKG0, 0))
    }
    Else
    {
      // Return Discharging.

      Store (1, Index (PKG0, 0))
    }
    Store (0x7FFFFFFF, Index (PKG0, 1))
    Store (Multiply (B0SC, 100), Index (PKG0, 2))
    Return (PKG0)
  }

  // Return that everything runs off Battery.

  Method (_PCL, 0)
  {
    Return (
      Package () { _SB }
    )
  }
}
