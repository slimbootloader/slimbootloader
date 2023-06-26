/**@file
  isCLK ASL code.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Register/PchRegsClk.h"

//
// Variables and definitions required by this file
// PID - variable holding isCLK sideband PID
// P2SB_DEV - define holding path to sideband device this isCLK can be accessed from.
//

//
// PCIe(100MHz) clock disable
// Arg0 - clock index
//
Method (CLKD, 1) {
  If (LGreater (Arg0, 6)) {
    ADD (Arg0, 1, Local1)
    Subtract (Local1, 7, Local1)
    P2SB_DEV.PCRA (PID, R_ICLK_PCR_BIOS_BUFFEN_H, Not (ShiftLeft (1, Local1)))
  } Else {
    ADD (Arg0, 2, Local1)
    P2SB_DEV.PCRA (PID, R_ICLK_PCR_BIOS_BUFFEN, Not (ShiftLeft (1, Local1)))
  }
}

//
// PCIe(100MHz) clock enable
// Arg0 - clock index
//
Method (CLKE, 1) {
  If (LGreater (Arg0, 6)) {
    ADD (Arg0,1, Local1)
    Subtract (Local1, 7, Local1)
    P2SB_DEV.PCRO (PID, R_ICLK_PCR_BIOS_BUFFEN_H, (ShiftLeft (1, Local1)))
  } Else {
    ADD (Arg0,2, Local1)
    P2SB_DEV.PCRO (PID, R_ICLK_PCR_BIOS_BUFFEN, (ShiftLeft (1, Local1)))
  }
}