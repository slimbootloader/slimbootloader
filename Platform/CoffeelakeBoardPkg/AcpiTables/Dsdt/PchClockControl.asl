/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Register/PchRegsClk.h"

Scope(\_SB)
{
  // IsCLK PCH register for clock settings
  OperationRegion (ICLK, SystemMemory, Add(SBRG, Add(ShiftLeft(PID_ICLK, 16), R_ICLK_PCR_CAMERA1)), 0x82)
  Field(ICLK,AnyAcc,Lock,Preserve)
  {
    CLK1, 8,
    Offset(0x80),
    CLK2, 8,
  }

  //
  // Number Of Clocks
  //
  Method(NCLK, 0x0, NotSerialized)
  {
    Return (2) // IMGCLKOUT_0, IMGCLKOUT_1
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
    Switch(Arg0)
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
    Switch(Arg0)
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
    }
  }
} // \_SB
