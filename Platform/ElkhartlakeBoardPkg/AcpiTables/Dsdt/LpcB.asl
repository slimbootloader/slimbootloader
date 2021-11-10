/** @file
  ACPI DSDT table

 Copyright (c) 2011 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/


// LPC Bridge - Device 31, Function 0
scope (\_SB.PC00.LPCB) {
      Include ("LpcDev.asl")
}
