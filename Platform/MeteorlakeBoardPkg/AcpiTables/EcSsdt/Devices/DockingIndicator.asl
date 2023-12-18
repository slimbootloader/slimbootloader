/** @file
  ACPI EC SSDT table for Docking Indicator.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (DIND)  // Docking Indicators.
{
  External (\OSYS)
  External (\IUDE, IntObj)

  Name (_HID, "INT33D4")
  Name (_CID, "PNP0C70")
  Method (_STA, 0, Serialized)
  {
    If (LAnd (And (IUDE, 1), LGreaterEqual (\OSYS, 2012)))
    {
      Return (0x000F)
    }
    Return (0x00)
  }
}
