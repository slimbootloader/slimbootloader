/**@file
  IPU IMG Clock Output Control
  Set of ACPI methods to enable, disable and select frequency of IMG Clock Output
  In Specs please refer to IMGCLKOUT_x where x is Clock Output index

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Register/PchRegsClk.h"
#define PTL_PCD_PID_ICLK      0x72

Scope (\_SB)
{
  Device (ICLK) {
    Name (_HID, EISAID ("PNP0C02"))
    Name (_UID, "SOCCLK")

    OperationRegion (CKOR, SystemMemory, Add(SBRG, Add(ShiftLeft(ICKP, 16), R_ISCLK_PCR_CAMERA1)), 0x40)
    Field(CKOR,AnyAcc,Lock,Preserve) {
      CLK0, 8,
      Offset(0x0C),
      CLK1, 8,
      Offset(0x18),
      CLK2, 8
    }

    //
    // Number Of Clocks
    //
    Method(NCLK, 0x0, NotSerialized)
    {
      If (LEqual (PCHS, PCHW)) {
        Return (6)
      }
      Return (3) // IMGCLKOUT_0, IMGCLKOUT_1, IMGCLKOUT_2
    }

    //
    // Clock Control
    //
    Method (CLKC, 0x2, NotSerialized)
    {
      //
      // Arg0 - Clock number (0:IMGCLKOUT_0, etc)
      // Arg1 - Desired state (0:Disable, 1:Enable)
      //
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLK0, Local0)
          Store(Or(And(Local0, Not(B_ISCLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK0)
        }
        Case (1) {
          Store(CLK1, Local0)
          Store(Or(And(Local0, Not(B_ISCLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK1)
        }
        Case (2) {
          Store(CLK2, Local0)
          Store(Or(And(Local0, Not(B_ISCLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK2)
        }
      }
    }

    //
    // Clock Frequency
    //
    Method (CLKF, 0x2, NotSerialized)
    {
      //
      // Arg0 - Clock number (0:IMGCLKOUT_0, etc)
      // Arg1 - Clock frequency:
      //    2'b00 - 19p2 XTAL
      //    2'b01 - 19p2 IMG
      //    2'b10 - 19p2 RTC
      //    2'b11 - 24 IMG
      If (LEqual (PCHS, PCHW)) {
        Return ()
      }
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLK0, Local0)
          Store(Or(And(Local0, Not(B_ISCLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ISCLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK0)
        }
        Case (1) {
          Store(CLK1, Local0)
          Store(Or(And(Local0, Not(B_ISCLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ISCLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK1)
        }
        Case (2) {
          Store(CLK2, Local0)
          Store(Or(And(Local0, Not(B_ISCLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ISCLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK2)
        }
      }
    }

    Method (_STA)
    {
      // Device is present, enabled and decoding it's resources
      // and should not be shown in UI
      Return (0x3)
    }

    Name (PID, PTL_PCD_PID_ICLK)
    #define P2SB_DEV \_SB.PC00.SBSC
    Include ("IsClk.asl")
    #undef P2SB_DEV
  }
}

//
// PCIe clock runtime control methods.
//
Scope (\) {

  //
  // Configure PCIe ClkReq Override
  // Arg0: Clock number
  // Arg1: Enable(1)/Disable(0) Clock
  //
  Method (SPCO, 2, Serialized) {
    ADBG ("Calling SPCO method to configure PCIe ClkReq Override")
    If (LEqual (Arg1,1)) {
      \_SB.ICLK.CLKE (Arg0)
    } Else {
      \_SB.ICLK.CLKD (Arg0)
    }
  }
}