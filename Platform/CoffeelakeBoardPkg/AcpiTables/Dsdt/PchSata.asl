/**@file
  Integrated SATA controller ACPI methods

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Platform hooks for SATA _DSM method.
// SATC - check if platform code supports given _DSM GUID
// SATD - execute _DSM method
//
External(\_SB.PCI0.SAT0.SATC, MethodObj)
External(\_SB.PCI0.SAT0.SATD, MethodObj)

// PCH SATA Controller
Device (SAT0) {
  //Bus 0x00, Device 0x17, Function 0x00
  Name(_ADR, 0x00170000)

  Method(_DSM,4,serialized){
    If(PCIC(Arg0)) {
      Return(PCID(Arg0,Arg1,Arg2,Arg3))
    // RST method to check if D3 on adapter is required.
    } ElseIf(LAnd(CondRefOf(SATC), CondRefOf(SATD))) {
      If(SATC(Arg0)) {
        Return(SATD(Arg0, Arg1, Arg2, Arg3))
      }
    } Else {
      Return(Buffer() {0})
    }
  }

  Method(_PS0,0,Serialized) {
    ADBG("SATA0 Ctrlr D0")
    //
    // Call CSD0 only if ST_FDIS_PMC == 0
    //
    If(LEqual(PCHS, PCHL)) {
      If(LNot(SCFD)){
        \_SB.CSD0(MODPHY_SPD_GATING_SATA)
      }
    }
  }

  Method(_PS3,0,Serialized) {
    ADBG("SATA0 Ctrlr D3")
    If(LEqual(PCHS, PCHL)) {
      \_SB.CSD3(MODPHY_SPD_GATING_SATA)
    }
  }

  Method(_S0W) {
    Return(3)
  }

  Device(PRT0)
  {
    Name(_ADR,0x0000FFFF)

    //
    // Check if drive is connected to the port.
    //
    // @retval 1  Drive is connected to the port
    // @retval 0  Drive is not connected to the port
    //
    Method(PRES) {
      If(And(SPPR, 0x1)) {
        Return(1)
      } Else {
        Return(0)
      }
    }
  }

  Device(PRT1)
  {
    Name(_ADR,0x0001FFFF)
    Method(PRES) {
      If(And(SPPR, 0x2)) {
        Return(1)
      } Else {
        Return(0)
      }
    }
  }

  Device(PRT2)
  {
    Name(_ADR,0x0002FFFF)
    Method(PRES) {
      If(And(SPPR, 0x4)) {
        Return(1)
      } Else {
        Return(0)
      }
    }
  }

  Device(PRT3)
  {
    Name(_ADR,0x0003FFFF)
    Method(PRES) {
      If(And(SPPR, 0x8)) {
        Return(1)
      } Else {
        Return(0)
      }
    }
  }

  Device(PRT4)
  {
    Name(_ADR,0x0004FFFF)
    Method(PRES) {
      If(And(SPPR, 0x10)) {
        Return(1)
      } Else {
        Return(0)
      }
    }
  }

  Device(PRT5)
  {
    Name(_ADR,0x0005FFFF)
    Method(PRES) {
      If(And(SPPR, 0x20)) {
        Return(1)
      } Else {
        Return(0)
      }
    }
  }
}
