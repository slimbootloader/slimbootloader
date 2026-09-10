/** @file
  ACPI EC SSDT table to summary all the devices defined under H_EC scope.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Device (H_EC) {
  Name (ECAV, Zero)    // OS Bug Checks if EC OpRegion accessed before Embedded Controller Driver loaded
  Name (B0SC, 100)     // Battery 0 Stored Capacity. Used in VBHK and BAT0 device.
  Name (BNUM, 0)       // Number Of Batteries Present
  Name (ECUP, 1)      // EC State indicator: 1- Normal Mode 0- Low Power Mode

  Include ("EcRam.asl")
  Include ("EcBase.asl")
  Include ("EcBaseMethod.asl")
  Include ("EcExternalMethod.asl")
}

