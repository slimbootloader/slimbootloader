/** @file
  MTL PCH isCLK device

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include "Register/PchRegsClk.h"
#define MTL_PCH_PID_ICLK      0xAD

Scope(\_SB) {
  Device (PCLK) {
    Name (_HID, EISAID ("PNP0C02"))
    Name (_UID, "PCHCLK")

    Name (PID, MTL_PCH_PID_ICLK)
    #define P2SB_DEV \_SB.PC02.SBPC
    //
    // PCIe(100MHz) clock disable
    // Arg0 - clock index
    //
    Method (CLKD, 1) {
      If(LGreater(Arg0, 6)) {
        Add(Arg0, 1, Local1)
        Subtract(Local1, 7, Local1)
        P2SB_DEV.PCRA(PID, R_ICLK_PCR_BIOS_BUFFEN_H, Not(ShiftLeft(1, Local1)))
      } Else {
        Add(Arg0, 2, Local1)
        P2SB_DEV.PCRA(PID, R_ICLK_PCR_BIOS_BUFFEN, Not(ShiftLeft(1, Local1)))
      }
    }

    //
    // PCIe(100MHz) clock enable
    // Arg0 - clock index
    //
    Method (CLKE, 1) {
      If(LGreater(Arg0, 6)) {
        Add(Arg0,1, Local1)
        Subtract(Local1, 7, Local1)
        P2SB_DEV.PCRO(PID, R_ICLK_PCR_BIOS_BUFFEN_H,(ShiftLeft(1, Local1)))
      } Else {
        Add(Arg0,2, Local1)
        P2SB_DEV.PCRO(PID, R_ICLK_PCR_BIOS_BUFFEN,(ShiftLeft(1, Local1)))
      }
    }
    #undef P2SB_DEV

    Method (SPCO, 2) {
      ADBG ("Calling SPCO method to configure PCH PCIe ClkReq Override")
      If (LEqual (Arg1,1)) {
          \_SB.PCLK.CLKE (Arg0)
      } Else {
          \_SB.PCLK.CLKD (Arg0)
      }
    }
  }
}