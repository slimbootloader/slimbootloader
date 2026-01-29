/** @file
  This file contains the TCSS PCIe Root Port configuration

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // Additionally if the device is capable of waking up
  // the platform from a low power System state (S3 or S4), there is a _S0W and _PRW
  // associated with the device as well.
  //
  Method (_PRW, 0) {

    //
    // Report the deepest wakeable state is S4 always in order to supporting TCSS RTD3 capability is some OS builds. The actual Sx wakeable state is mapped and managed by
    // VCCST's preference to IOM for remaining or removing VNNAON rail in S4 state. BIOS is no longer required to report _PRW with S3 wake capability to OS even if
    // the platform wants to limit USBC Sx wake capability to S4 only, or when VCCST support is disabled.
    // The actual S4 wake capability will be limited by VCST setting instead of OS managment. Platform shall set VCCST setting to OFF when not supporting S4 wake over USBC.
    //

    // GP1E enables GPE1
    If (CondRefOf (GP1E)) {
      If (LNotEqual (GP1E, 0)) {
        Return (GPRW (GPE1_EN, 4)) // can wakeup from S4 state
      }
    }
    Return (GPRW (GPE0_EN, 4)) // can wakeup from S4 state
  }