/** @file
  ACPI EC SSDT table for Convertible Indicator.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (CIND)  // Convertible Indicators.
{
  External (\OSYS, IntObj)

  Name (_HID, "INT33D3")
  Name (_CID, "PNP0C60")

  Method (_STA, 0, Serialized)
  {
    If (LAnd (And (IUCE, 1), LGreaterEqual (\OSYS, 2012)))
    {
      Return (0x000F)
    }
    Return (0x00)
  }
}
