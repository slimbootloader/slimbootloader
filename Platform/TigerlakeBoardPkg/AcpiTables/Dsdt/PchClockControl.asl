/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Register/PchRegsClk.h"

Scope(\_SB)
{
  // IsCLK PCH register for clock settings
#ifndef CPU_ICL
  OperationRegion (ICLK, SystemMemory, Add(SBRG, Add(ShiftLeft(ICKP, 16), R_ICLK_PCR_CAMERA1)), 0x82)
  Field(ICLK,AnyAcc,Lock,Preserve)
  {
    CLK1, 8,
    Offset(0x80),
    CLK2, 8,
  }
#else
  OperationRegion (ICLK, SystemMemory, Add(SBRG, Add(ShiftLeft(ICKP, 16), R_ICLK_PCR_CAMERA1)), 0x40)
  Field(ICLK,AnyAcc,Lock,Preserve)
  {
    CLK1, 8,
    Offset(0x0C),
    CLK2, 8,
    Offset(0x18),
    CLK3, 8,
    Offset(0x24),
    CLK4, 8,
    Offset(0x30),
    CLK5, 8,
    Offset(0x3C),
    CLK6, 8,
  }
#endif

  //
  // Number Of Clocks
  //
  Method(NCLK, 0x0, NotSerialized)
  {
#ifndef CPU_ICL
    Return (2) // IMGCLKOUT_0, IMGCLKOUT_1
#else
#ifdef CPU_TGL
    Return (6) // IMGCLKOUT_0, IMGCLKOUT_1, IMGCLKOUT_2, IMGCLKOUT_3, IMGCLKOUT_4, IMGCLKOUT_5
#else
    Return (5) // IMGCLKOUT_0, IMGCLKOUT_1, IMGCLKOUT_2, IMGCLKOUT_3, IMGCLKOUT_4
#endif
#endif
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
    Switch(ToInteger(Arg0))
    {
      Case (0)
      {
        Store(CLK1, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK1)
      }
      Case (1)
      {
        Store(CLK2, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK2)
      }
#ifdef CPU_ICL
#ifdef CPU_TGL
      Case (2)
      {
        Store(CLK3, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK3)
      }
      Case (3)
      {
        Store(CLK4, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK4)
      }
      Case (4)
      {
        Store(CLK5, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK5)
      }
      Case (5)
      {
        Store(CLK6, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK6)
      }
#else
      Case (2)
      {
        Store(CLK3, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK3)
      }
      Case (3)
      {
        Store(CLK4, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK4)
      }
      Case (4)
      {
        Store(CLK5, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_REQUEST)), ShiftLeft(Arg1, 1)), CLK5)
      }
#endif
#endif
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
    Switch(ToInteger(Arg0))
    {
      Case (0)
      {
        Store(CLK1, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK1)
      }
      Case (1)
      {
        Store(CLK2, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK2)
      }
#ifdef CPU_ICL
#ifdef CPU_TGL
      Case (2)
      {
        Store(CLK3, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK3)
      }
      Case (3)
      {
        Store(CLK4, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK4)
      }
      Case (4)
      {
        Store(CLK5, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK5)
      }
      Case (5)
      {
        Store(CLK6, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK6)
      }
#else
      Case (2)
      {
        Store(CLK3, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK3)
      }
      Case (3)
      {
        Store(CLK4, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK4)
      }
      Case (4)
      {
        Store(CLK5, Local0)
        Store(Or(And(Local0, Not(B_ICLK_PCR_FREQUENCY)), Arg1), CLK5)
      }
#endif
#endif
    }
  }
} // \_SB
