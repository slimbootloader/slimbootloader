/**@file

 Copyright (c) 2010 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/UsbRegs.h>

External(\_SB.PC00.XHCI.PS0X, MethodObj)
External(\_SB.PC00.XHCI.PS3X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.PS0X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.PS2X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.PS3X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.INIR, MethodObj)

Scope(\_SB_.PC00) {
  Device(XHCI) {

    Name(_ADR, 0x00140000)
    OperationRegion(XPRT, SystemMemory, 0xC00A0000, 0x100)
    Field(XPRT,AnyAcc,NoLock,Preserve)
    {
      DVID, 16,
      Offset(0x10),
      XADL,  32,
      XADH,  32,
      Offset(R_XHCI_CFG_PCE), // 0xA2
          ,  2,
      D3HE,  1,  // D3_hot_en
    }

    //
    // XHCI controller _DSM method
    //
    Method(_DSM,4,serialized){
      If(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }

      Return(Buffer() {0})
    }

    Method(_S3D, 0, NotSerialized)
    {
      Return(3)
    }
    Method(_S4D, 0, NotSerialized)
    {
      Return(3)
    }
    Method(_S3W, 0, NotSerialized)
    {
      Return(3)
    }
    Method(_S4W, 0, NotSerialized)
    {
      Return(3)
    }

    Method(_S0W, 0x0, NotSerialized)
    {
      Return(0x3)
    }

    Method(_PRW, 0) {
      Return(GPRW(0x6D, 3)) // can wakeup from S3 state
    }

    Method(_DSW, 3) {}

    Method(_INI) {
      If(CondRefOf(\_SB.PC00.XHCI.RHUB.INIR)) {      // _INI for RTD3
        \_SB.PC00.XHCI.RHUB.INIR()
      }
    }

    //
    // USRA (USbR port Address), method for calculating address of first USBR port in XHCI controller
    //
    Method(USRA,0,Serialized) {
      If(LEqual(PCHS, PCHH)) { // PCH-H
        Return (15)
      } Else {                 // PCH-LP
        Return (11)
      }
    }

    //
    // SSPA (SuperSpeed Port Address), method for calculating address of first SS port in XHCI controller
    //
    Method(SSPA,0,Serialized) {
      If(LEqual(PCHS, PCHH)) {        // PCH-H
        Return (17)
      } ElseIf(LEqual(PCHS, PCHL)) {  // PCH-LP
        Return (13)
      } Else {
        Return (9)                    // PCH-N
      }
      Return (0)
    }

    Method(_PS0,0,Serialized)
    {
      If(LEqual(^DVID,0xFFFF))
      {
        Return()
      }

      //
      // Call platform XHCI PS0 method if present
      //
      If(CondRefOf(\_SB.PC00.XHCI.PS0X))
      {
        \_SB.PC00.XHCI.PS0X()
      }
    }

    Method(_PS3,0,Serialized)
    {
      If(LEqual(^DVID,0xFFFF))
      {
        Return()
      }

      //
      // Call platform XHCI PS3 method if present
      //
      If(CondRefOf(\_SB.PC00.XHCI.PS3X))
      {
        \_SB.PC00.XHCI.PS3X()
      }
    }

    //
    //
    // Check for XHCI switch UUID
    //
    // Arguments:
    //  Arg0 (Buffer) : UUID
    //
    // Returns:
    //  1: It's valid UUID
    //  0: Invalid UUID
    //
    Method(CUID,1,Serialized) {
      If(LEqual(Arg0,ToUUID("7c9512a9-1705-4cb4-af7d-506a2423ab71"))) {
        Return(1)
      }
      Return(0)
    }

    Device(RHUB)
    {
      Name(_ADR, Zero)

      // PS0 Method for xHCI Root Hub
      Method(_PS0,0,Serialized)
      {
        If(LEqual(\_SB.PC00.XHCI.DVID,0xFFFF))
        {
          Return()
        }
        //
        // Call platform XHCI.RHUB PS0 method if present.
        //
        If(CondRefOf(\_SB.PC00.XHCI.RHUB.PS0X))
        {
          \_SB.CSD0(MODPHY_SPD_GATING_XHCI)
          \_SB.PC00.XHCI.RHUB.PS0X()
        }
      }

      // PS2 Method for xHCI Root Hub
      Method(_PS2,0,Serialized)
      {
        If(LEqual(\_SB.PC00.XHCI.DVID,0xFFFF))
        {
          Return()
        }
        //
        // Call platform XHCI.RHUB PS2 method if present.
        //
        If(CondRefOf(\_SB.PC00.XHCI.RHUB.PS2X))
        {
          \_SB.PC00.XHCI.RHUB.PS2X()
          \_SB.CSD3(MODPHY_SPD_GATING_XHCI)
        }
      }

      // PS3 Method for xHCI Root Hub
      Method(_PS3,0,Serialized)
      {
        If(LEqual(\_SB.PC00.XHCI.DVID,0xFFFF))
        {
          Return()
        }
        //
        // Call platform XHCI.RHUB PS3 method if present.
        //
        If(CondRefOf(\_SB.PC00.XHCI.RHUB.PS3X))
        {
          \_SB.PC00.XHCI.RHUB.PS3X()
          \_SB.CSD3(MODPHY_SPD_GATING_XHCI)
        }
      }

      //
      // High Speed Ports (without USBR)
      //
      Device(HS01)
      {
        Name(_ADR, 0x01)
      }

      Device(HS02)
      {
        Name(_ADR, 0x02)
      }

      Device(HS03)
      {
        Name(_ADR, 0x03)
      }

      Device(HS04)
      {
        Name(_ADR, 0x04)
      }

    Device(HS05)
      {
     Name(_ADR, 0x05)
      }

    Device(HS06)
      {
     Name(_ADR, 0x06)
      }

    Device(HS07)
      {
     Name(_ADR, 0x07)
      }

    Device(HS08)
      {
     Name(_ADR, 0x08)
      }

      //
      // Super Speed Ports
      //
      Device(SS01)
    {
        Method(_ADR) { Return (Add(SSPA(),0)) }
    }

      Device(SS02)
    {
        Method(_ADR) { Return (Add(SSPA(),1)) }
    }

    Device(SS03)
    {
      Method(_ADR) { Return (Add(SSPA(),2)) }
    }

    Device(SS04)
    {
      Method(_ADR) { Return (Add(SSPA(),3)) }
    }
    } // device rhub
  } // device xhc
} //scope

//
// Additional ports for PCH-LP and PCH-H
//
If(LOr(LEqual(PCHS, PCHL), LEqual(PCHS, PCHH))) {
  Scope(\_SB.PC00.XHCI.RHUB) {
    Device(HS09)
    {
      Name(_ADR, 0x09)
    }

    Device(HS10)
    {
      Name(_ADR, 0x0A)
    }

    Device(HS11)
    {
      Name(_ADR, 0x0B)
    }

    Device(HS12)
    {
      Name(_ADR, 0x0C)
    }
  } //scope
} // if
