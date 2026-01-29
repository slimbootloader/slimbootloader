 /**@file
  This file defines the device objects for the TSE device

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
External (\PCHS, IntObj)


Scope (\_SB.PC00) {
  //
  // TSE Device
  //
  Device (TSE) {

    Name (_HID, "INTC10DF")                 // PTL
    If (Lequal (PCHS, PCHW)) {
      Store ("INTC10E9", _HID)              // WCL
    }                                       // Intel Specific Name
    Name (_UID, 0)                          // 1st instance of the controller

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual(TSEE, 1)) {
        Return(0x0F)
      }
      Return(0x00)
    } // End of Method (_STA)
  } // End of Device (TSE)
} // End of Scope for TSE
