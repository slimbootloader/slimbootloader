/**@file

  Serial IO I3C ACPI definitions

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
// @todo - add NVS variable to check for disable/enable
Device (I3C0) {
  Name (_ADR, 0x00150004)
  Method (_S0W, 0) {
    // PMEs can be generated from D3(hot)
    Return (3)
  }
  // IBI Wake Support
  Method (_PRW, 0) {
    Return (GPRW (GPE_PME, 4)) // can wakeup from S4 state
  }
} // end Device I3C0

