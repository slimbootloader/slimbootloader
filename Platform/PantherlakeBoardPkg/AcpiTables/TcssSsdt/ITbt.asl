/** @file
 Thunderbolt ACPI methods

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
External (\_SB.PC00.GFX0, DeviceObj)
Name (ITNB, 0xFFFF0000)  // ITBT NVS Base address
Name (ITNL, 0xAA55)      // ITBT NVS Length
Include ("ITbtNvs.asl")