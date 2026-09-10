/** @file
  ACPI PEP deep standby entry/exit.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// TCSS device would only put under the SOC side.
//
External (\_SB.PC00.LPCB.NTIR, MethodObj)
External (\_SB.PC00.DPOF, FieldUnitObj)
External (\_SB.PC00.TXHC, DeviceObj)
External (\_SB.PC00.TDM0, DeviceObj)
External (\_SB.PC00.TDM0.SCME, IntObj)
External (\_SB.PC00.TDM1, DeviceObj)
External (\_SB.PC00.TDM1.SCME, IntObj)
External (\_SB.PC02.LPCB.NTIR, MethodObj)
External (\GPRV, MethodObj)

//
//  PEP deep standby entry/exit feature.
//
//  Arguments: (1)
//    Arg0    - 0 exit CS
//              1 entry CS
//  Return Value:
//    None.
//
Method (PESB, 1, Serialized)
{
  If (LEqual (S0ID, 1)) {
    \GUAM (Arg0)
  }

  // Notify EC of CS entry/exit
  If (CondRefOf (\_SB.PC00.LPCB.NTIR)) {
    \_SB.PC00.LPCB.NTIR (Arg0)
  }
  If (CondRefOf (\_SB.PC02.LPCB.NTIR)) {
    \_SB.PC02.LPCB.NTIR (Arg0)
  }

  //
  // All Monitor off bit setting,
  // DPOF = 1 - Give indication to IOM that all the display is OFF
  // DPOF = 0 - If system is running in SW CM mode or TBT controller not exist case
  //            can be skip to raise this bit
  //
  Store (0, Local0)
  If (CondRefOf (\_SB.PC00.DPOF)) {
    If (CondRefOf (\_SB.PC00.TXHC)) {
      If (CondRefOf (\_SB.PC00.TDM0)) {
        If (LEqual (\_SB.PC00.TDM0.SCME, 1)) {
          Store (Arg0, Local0)
        }
      }
      If (CondRefOf (\_SB.PC00.TDM1)) {  // check DMA1
        If (LEqual (\_SB.PC00.TDM1.SCME, 1)) {
          Store (Arg0, Local0)
        }
      }

      If (LEqual (Local0, 1)) {
        Store (1, \_SB.PC00.DPOF)
      }

      If (LEqual (Arg0, 0)) {
        Store (0, \_SB.PC00.DPOF)
      }
    }
  }

  //
  // call method specific to CS platforms when the system is in a
  // standby state with very limited SW activities
  //
  If (CondRefOf (\GPRV)) {
    If (LEqual (Arg0, 1)) {
      ADBG ("[PEP] allow PS_ON!")
      \GPRV (3, 0)
    } Else {
      ADBG ("[PEP] veto PS_ON!")
      \GPRV (3, 1)
    }
  }
}
