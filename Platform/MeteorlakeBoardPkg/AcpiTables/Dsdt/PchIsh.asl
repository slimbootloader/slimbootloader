/**@file
  ACPI DSDT table for ISH

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
Scope (\_SB.PC00) {
  //
  // Integrated Sensor Hub (PCI Mode)
  //
  Device (ISHD) {
    Name (_ADR, 0x00120000)
  } // Device (ISHD)
}