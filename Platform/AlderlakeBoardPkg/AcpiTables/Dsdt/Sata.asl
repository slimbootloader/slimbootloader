/** @file
  Integrated SATA controller ACPI methods

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Include this file in SATA device scope. This file requires following variables to be defined
// \SPPR - sata port present status - contains bitmask of SATA ports that are present on the system
//

//
// Platform hooks for SATA _DSM method.
// SATC - check if platform code supports given _DSM GUID
// SATD - execute _DSM method
//
External(SATC, MethodObj)
External(SATD, MethodObj)

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

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
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

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
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

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
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

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
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

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
  Method(PRES) {
    If(And(SPPR, 0x20)) {
      Return(1)
    } Else {
      Return(0)
    }
  }
}

Device(PRT6)
{
  Name(_ADR,0x0006FFFF)

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
  Method(PRES) {
    If(And(SPPR, 0x40)) {
      Return(1)
    } Else {
      Return(0)
    }
  }
}

Device(PRT7)
{
  Name(_ADR,0x0007FFFF)

  //
  // Check if drive is connected to the port.
  //
  // @retval 1  Drive is connected to the port
  // @retval 0  Drive is not connected to the port
  //
  Method(PRES) {
    If(And(SPPR, 0x80)) {
      Return(1)
    } Else {
      Return(0)
    }
  }
}

