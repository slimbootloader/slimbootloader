/** @file
  D3 Wake support for ISH

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope (\_SB.PC00.ISHD) {
  Method (_S0W, 0) {
    ADBG ("ISHD _S0W")
    Return (0x3)
  }
  Method (_PRW, 0) {
    ADBG ("ISHD _PRW")
    Return (GPRW (0x68, 0x04))
  }
}
