/**@file
  IPU IMG Clock Output Control
  Set of ACPI methods to enable, disable and select frequency of IMG Clock Output
  In Specs please refer to IMGCLKOUT_x where x is Clock Output index

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Register/PchRegsClk.h"
#define MTL_SOC_PID_ICLK      0x64

Scope (\_SB)
{
  Device (ICLK) {
    Name (_HID, EISAID ("PNP0C02"))
    Name (_UID, "SOCCLK")

    OperationRegion (CKOR, SystemMemory, Add(SBRG, Add(ShiftLeft(ICKP, 16), R_ICLK_PCR_CAMERA1)), 0x40)
    Field(CKOR,AnyAcc,Lock,Preserve) {
      CLK0, 8,
      Offset(0x0C),
      CLK1, 8,
      Offset(0x18),
      CLK2, 8,
      Offset(0x24),
      CLK3, 8,
      Offset(0x30),
      CLK4, 8,
      Offset(0x3C),
      CLK5, 8,
    }

    //
    // Number Of Clocks
    //
    Method(NCLK, 0x0, NotSerialized)
    {
      Return (6) // IMGCLKOUT_0, IMGCLKOUT_1, IMGCLKOUT_2, IMGCLKOUT_3, IMGCLKOUT_4, IMGCLKOUT_5
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
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK0)
        }
        Case (1) {
          Store(CLK1, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK1)
        }
        Case (2) {
          Store(CLK2, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK2)
        }
        Case (3) {
          Store(CLK3, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK3)
        }
        Case (4) {
          Store(CLK4, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK4)
        }
        Case (5) {
          Store(CLK5, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_CLK_EN)), ShiftLeft(Arg1, 2)), CLK5)
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
      Switch(ToInteger(Arg0)) {
        Case (0) {
          Store(CLK0, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK0)
        }
        Case (1) {
          Store(CLK1, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK1)
        }
        Case (2) {
          Store(CLK2, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK2)
        }
        Case (3) {
          Store(CLK3, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK3)
        }
        Case (4) {
          Store(CLK4, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK4)
        }
        Case (5) {
          Store(CLK5, Local0)
          Store(Or(And(Local0, Not(B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), And(Arg1, B_ICLK_PCR_CAMERA1_CAM1_FREQ_SEL_1_0)), CLK5)
        }
      }
    }

    Method (_STA)
    {
      // Device is present, enabled and decoding it's resources
      // and should not be shown in UI
      Return (0x3)
    }

    Name (PID, MTL_SOC_PID_ICLK)
    #define P2SB_DEV \_SB.PC00.SBSC
    Include ("IsClk.asl")
    #undef P2SB_DEV
  }

  //
  // MTL IOE die isCLK
  //
  Device (ECLK) {
    Name (_HID, EISAID ("PNP0C02"))
    Name (_UID, "IOECLK")
    Method (_STA)
    {
      // Device is present, enabled and decoding it's resources
      // and should not be shown in UI
      Return (0x3)
    }

    Name (PID, MTL_SOC_PID_ICLK)
    #define P2SB_DEV \_SB.PC00.SBIE
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
    If (LEqual (Arg1,1)) {
      If (LGreater (Arg0, 5)) {
        //
        // Simics model doesn't yet support isCLK on IOE die.
        //
        \_SB.ECLK.CLKE (Subtract (Arg0, 6))
      } Else {
        \_SB.ICLK.CLKE (Arg0)
      }
    } Else {
      If (LGreater (Arg0, 5)) {
        //
        // Simics model doesn't yet support isCLK on IOE die.
        //
        \_SB.ECLK.CLKD (Subtract (Arg0, 6))
      } Else {
        \_SB.ICLK.CLKD (Arg0)
      }
    }
  }

  //
  // WA to Mask/UnMask Hybrid Partner CLKREQ using IPC message
  // Arg0: Hybrid device partner port number
  // Arg1: UnMask(1)/Mask(0) Clock
  //
  Method (HBCM, 2, Serialized) {
    // Unmapping CLKREQ in FIA is not needed on MTL SoC.
  }
}
