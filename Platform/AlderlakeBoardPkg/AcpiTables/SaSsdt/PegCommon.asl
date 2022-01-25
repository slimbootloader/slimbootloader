/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
External(\_SB.ISME, MethodObj)
External(\_SB.SHPO, MethodObj)
External(\_SB.CAGS, MethodObj)
External(\_SB.PC00.PEG0, DeviceObj)
External(\_SB.PC00.PEG1, DeviceObj)
External(\_SB.PC00.PEG2, DeviceObj)
External(\_SB.PC00.PEG3, DeviceObj)
External(\_SB.PC00.PEG1.DHDW, MethodObj)
External(\P0WK)
External(\P1WK)
External(\P2WK)
External(\P3WK)

//GPE Event handling - Start
Scope(\_GPE) {
  //
  // _L6F Method call for PEG0/1/2/3 ports to handle 2-tier RTD3 GPE events
  //
  Method(P0L6,0)
  {
    // PEG0 Device Wake Event
    If (\_SB.ISME(P0WK))
    {
      \_SB.SHPO(P0WK, 1)             // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
      Notify(\_SB.PC00.PEG0, 0x02)   // device wake
      \_SB.CAGS(P0WK)                // Clear GPE status bit for PEG0 WAKE
    }
  }

  Method(P1L6,0)
  {
    // PEG1 Device Wake Event
    If (\_SB.ISME(P1WK))
    {
      If (CondRefOf (\_SB.PC00.PEG1.DHDW)) { // DG support HPD as D3 Wake GPIO and DHDW exists
        \_SB.PC00.PEG1.DHDW () // Disable Discrete Graphics HPD SCI implementated As D3 Wake GPIO
        Notify(\_SB.PC00.PEG1, 0x00)   // Bus Check
      } Else {
        \_SB.SHPO(P1WK, 1)             // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.PEG1, 0x02)   // device wake
      }
      \_SB.CAGS(P1WK)                // Clear GPE status bit for PEG1 WAKE
    }
  }

  Method(P2L6,0)
  {
    // PEG2 Device Wake Event
    If (\_SB.ISME(P2WK))
    {
      \_SB.SHPO(P2WK, 1)             // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
      Notify(\_SB.PC00.PEG2, 0x02)   // device wake
      \_SB.CAGS(P2WK)                // Clear GPE status bit for PEG2 WAKE
    }
  }

  If (CondRefOf(\_SB.PC00.PEG3)) {
    Method(P3L6,0)
    {
      // PEG2 Device Wake Event
      If (\_SB.ISME(P3WK))
      {
        \_SB.SHPO(P3WK, 1)             // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        Notify(\_SB.PC00.PEG3, 0x02)   // device wake
        \_SB.CAGS(P3WK)                // Clear GPE status bit for PEG2 WAKE
      }
    }
  }
} //Scope(\_GPE)

