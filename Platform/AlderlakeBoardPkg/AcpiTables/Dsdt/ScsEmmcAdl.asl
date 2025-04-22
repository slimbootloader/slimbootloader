/**@file
  ACPI DSDT table for SCS EMMC Controller

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(EMCE)

#define PCI_EMMC_ADR                0x001A0000
#define PEMC                        0xA1

Scope(\_SB.PC00) {
  If (LEqual (EMCE, 1)) {
    //
    // SCS EMMC (PCI Mode)
    //
    Device(EMMC) {
      Name(_ADR, PCI_EMMC_ADR)
      Name (_DDN, "Intel(R) eMMC Controller")
      Include ("ScsEmmc.asl")
    }
  } //EMCE
}
