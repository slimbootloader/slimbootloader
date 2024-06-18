/** @file
  ACPI EC SSDT table for WDT device.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (WDT0)  // WDT Device Resource Consumption
{
  Name (_HID, EISAID ("PNP0C02"))

  Name (_UID, 3)

  Name (_CRS, ResourceTemplate ()
  {
    IO (Decode16, 0x6A4, 0x6A4, 0x1, 0x1)  // 1 Byte EC Prv Intfc.
    IO (Decode16, 0x6A0, 0x6A0, 0x1, 0x1)  // 1 Byte EC Prv Intfc.
  })
}
