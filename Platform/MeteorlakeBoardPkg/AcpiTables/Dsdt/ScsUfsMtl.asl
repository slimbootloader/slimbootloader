/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
External(UF0E)

#define PCI_UFS0_ADR                0x00120007

Scope(\_SB.PC00) {

  If (LEqual (UF0E, 1)) {
    //
    // SCS UFS0 (PCI Mode)
    //
    Device(PUF0) {
      Name(_ADR, PCI_UFS0_ADR)
      Name (_DDN, "Intel(R) UFS Controller")

    }
  } //UF0E
}
