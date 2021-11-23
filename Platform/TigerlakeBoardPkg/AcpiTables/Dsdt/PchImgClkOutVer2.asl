/**@file
  IPU IMG Clock Output Control
  Set of ACPI methods to enable, disable and select frequency of IMG Clock Output
  In Specs please refer to IMGCLKOUT_x where x is Clock Output index

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Register/PchRegsClk.h"

Scope(\_SB)
{
  OperationRegion (ICLK, SystemMemory, Add(SBRG, Add(ShiftLeft(ICKP, 16), R_ICLK_PCR_CAMERA1)), 0x40)
  If(LEqual(PCHS, PCHL)) {
    Field(ICLK,AnyAcc,Lock,Preserve) {
      CLL0, 8,
      Offset(0x0C),
      CLL1, 8,
      Offset(0x18),
      CLL2, 8,
      Offset(0x24),
      CLL3, 8,
      Offset(0x30),
      CLL4, 8,
      Offset(0x3C),
      CLL5, 8,
    }
  } Else {
    Field(ICLK,AnyAcc,Lock,Preserve) {
      CLH0, 8,
      Offset(0x0C),
      CLH1, 8,
    }
  }

  //
  // Number Of Clocks
  //
  Method(NCLK, 0x0, NotSerialized)
  {
    If(LEqual(PCHS, PCHL)) {
      Return (6) // IMGCLKOUT_0, IMGCLKOUT_1, IMGCLKOUT_2, IMGCLKOUT_3, IMGCLKOUT_4, IMGCLKOUT_5
    } Else {
      Return (2) // IMGCLKOUT_0, IMGCLKOUT_1
    }
  }

  //
  // Clock Control
  //
  Method(CLKC, 0x2, NotSerialized)
  {
    //
    // Arg0 - Clock number (0:IMGCLKOUT_0, etc)
    // Arg1 - Desired state (0:Disable, 1:Enable)
    //
    If(LEqual(PCHS, PCHL)) {
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLL0, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLL0)
        }
        Case (1) {
          Store(CLL1, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLL1)
        }
        Case (2) {
          Store(CLL2, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLL2)
        }
        Case (3) {
          Store(CLL3, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLL3)
        }
        Case (4) {
          Store(CLL4, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLL4)
        }
        Case (5) {
          Store(CLL5, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLL5)
        }
      }
    } Else {
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLH0, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLH0)
        }
        Case (1) {
          Store(CLH1, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLH1)
        }
      }
    }
  }

  //
  // Clock Frequency
  //
  Method(CLKF, 0x2, NotSerialized)
  {
    //
    // Arg0 - Clock number (0:IMGCLKOUT_0, etc)
    // Arg1 - Clock frequency (0:24MHz, 1:19.2MHz)
    //
    If(LEqual(PCHS, PCHL)) {
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLL0, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLL0)
        }
        Case (1) {
          Store(CLL1, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLL1)
        }
        Case (2) {
          Store(CLL2, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLL2)
        }
        Case (3) {
          Store(CLL3, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLL3)
        }
        Case (4) {
          Store(CLL4, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLL4)
        }
        Case (5) {
          Store(CLL5, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLL5)
        }
      }
    } Else {
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLH0, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLH0)
        }
        Case (1) {
          Store(CLH1, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLH1)
        }
      }
    }
  }
} // \_SB
