/**@file
  Integrated SATA controller ACPI methods

 Copyright (c) 2013 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Platform hooks for SATA _DSM method.
// SATC - check if platform code supports given _DSM GUID
// SATD - execute _DSM method
//
External(\_SB.PC00.SAT0.SATC, MethodObj)
External(\_SB.PC00.SAT0.SATD, MethodObj)

// PCH SATA Controller
Device (SAT0) {
  //Bus 0x00, Device 0x17, Function 0x00
  Name(_ADR, 0x00170000)

  //
  // Windows defined _DSD that informs the OS
  // that it should support D3 on this storage device.
  //
  Name (_DSD, Package () {
    ToUUID("5025030F-842F-4AB4-A561-99A5189762D0"),
    Package () {
        Package (2) {"StorageD3Enable", 1}
    }
  })

  Method(_DSM,4,serialized) {
    If(PCIC(Arg0)) {
      Return(PCID(Arg0,Arg1,Arg2,Arg3))
    }
    // If _DSM GUID doesn't match any of the GUIDs supported by reference code
    // check if platform code supports it by calling SATC method. If platform code supports
    // given GUID call SATD which contains _DSM implementation.
    If(LAnd(CondRefOf(SATC), CondRefOf(SATD))) {
      If(SATC(Arg0)) {
        Return(SATD(Arg0, Arg1, Arg2, Arg3))
      }
    }

    Return(Buffer() {0})
  }

  Method(_PS0,0,Serialized) {
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
  }

  Device(PRT1)
  {
    Name(_ADR,0x0001FFFF)
  }

  Device(PRT2)
  {
    Name(_ADR,0x0002FFFF)
  }

  Device(PRT3)
  {
    Name(_ADR,0x0003FFFF)
  }

  Device(PRT4)
  {
    Name(_ADR,0x0004FFFF)
  }

  Device(PRT5)
  {
    Name(_ADR,0x0005FFFF)
  }
}
