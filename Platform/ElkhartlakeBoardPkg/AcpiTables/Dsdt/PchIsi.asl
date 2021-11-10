/**@file
  ACPI DSDT table for Intel Safety Island (ISI)

 Copyright (c) 2019 Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00) {
  //
  // Intel Safety Island (ISI) - Device 26 Function 3
  //
  Device(EISI) {
    Name(_ADR, 0x001A0003)
  } // Device(EISI)
}
