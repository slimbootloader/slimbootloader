/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/RegsUsb.h>

External(\_SB.PCI0.XHC.PS0X, MethodObj)
External(\_SB.PCI0.XHC.PS3X, MethodObj)
External(\_SB.PCI0.XHC.RHUB.PS0X, MethodObj)
External(\_SB.PCI0.XHC.RHUB.PS2X, MethodObj)
External(\_SB.PCI0.XHC.RHUB.PS3X, MethodObj)
External(\_SB.PCI0.XHC.RHUB.INIR, MethodObj)

Scope(\_SB_.PCI0) {
  Device(XHC) {

    Name(_ADR, 0x00140000)

    OperationRegion(XPRT,PCI_Config,0x00,0x100)
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
      ADBG("XHCI _DSM")

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
      Return(GPRW(0x6D, 4)) // can wakeup from S4 state
    }

    Method(_DSW, 3) {}

    Method(_INI) {
      If(CondRefOf(\_SB.PCI0.XHC.RHUB.INIR)) {      // _INI for RTD3
        \_SB.PCI0.XHC.RHUB.INIR()
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
      If(LEqual(PCHS, PCHH)) { // PCH-H
        Return (17)
      } Else {                 // PCH-LP
        Return (13)
      }
    }

    Method(_PS0,0,Serialized)
    {
      If(LEqual(^DVID,0xFFFF))
      {
        Return()
      }

      //
      // Call platform XHC PS0 method if present
      //
      If(CondRefOf(\_SB.PCI0.XHC.PS0X))
      {
        \_SB.PCI0.XHC.PS0X()
      }
    }

    Method(_PS3,0,Serialized)
    {
      If(LEqual(^DVID,0xFFFF))
      {
        Return()
      }

      //
      // Call platform XHC PS3 method if present
      //
      If(CondRefOf(\_SB.PCI0.XHC.PS3X))
      {
        \_SB.PCI0.XHC.PS3X()
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
        If(LEqual(\_SB.PCI0.XHC.DVID,0xFFFF))
        {
          Return()
        }

        //
        // Call platform XHC.RHUB PS0 method if present.
        //
        If(CondRefOf(\_SB.PCI0.XHC.RHUB.PS0X))
        {
          //
          // Don't Set/Clear MODPHY if RTD3 is disable.
          //
          \_SB.CSD0(MODPHY_SPD_GATING_XHCI)

          \_SB.PCI0.XHC.RHUB.PS0X()
        }
      }

      // PS2 Method for xHCI Root Hub
      Method(_PS2,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.XHC.DVID,0xFFFF))
        {
          Return()
        }
        //
        // Call platform XHC.RHUB PS2 method if present.
        //
        If(CondRefOf(\_SB.PCI0.XHC.RHUB.PS2X))
        {
          \_SB.PCI0.XHC.RHUB.PS2X()

          //
          // Don't Set/Clear MODPHY if RTD3 is disable.
          //
          \_SB.CSD3(MODPHY_SPD_GATING_XHCI)
        }
      }

      // PS3 Method for xHCI Root Hub
      Method(_PS3,0,Serialized)
      {
        If(LEqual(\_SB.PCI0.XHC.DVID,0xFFFF))
        {
          Return()
        }
        //
        // Call platform XHC.RHUB PS3 method if present.
        //
        If(CondRefOf(\_SB.PCI0.XHC.RHUB.PS3X))
        {
          \_SB.PCI0.XHC.RHUB.PS3X()

          //
          // Don't Set/Clear MODPHY if RTD3 is disable.
          //
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

      Device(HS09)
      {
        Name(_ADR, 0x09)
      }

      Device(HS10)
      {
        Name(_ADR, 0x0A)
      }

      //
      // USBR port will be known as USBR instead of HS11 / HS15
      //
      Device(USR1)
      {
        Method(_ADR) { Return (Add(USRA(),0)) }
      }

      //
      // USBR port 2 will be known as USBR instead of HS12 / HS16
      //
      Device(USR2)
      {
        Method(_ADR) { Return (Add(USRA(),1)) }
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

      Device(SS05)
      {
        Method(_ADR) { Return (Add(SSPA(),4)) }
      }

      Device(SS06)
      {
        Method(_ADR) { Return (Add(SSPA(),5)) }
      }

    } // device rhub
  } // device xhc
} //scope

//
// If PCH-H add additional ports
//
If(LEqual(PCHS,PCHH)) {
  Scope(\_SB_.PCI0.XHC.RHUB) {

    Device(HS11)
    {
      Name(_ADR, 0xB)
    }

    Device(HS12)
    {
      Name(_ADR, 0xC)
    }

    Device(HS13)
    {
      Name(_ADR, 0xD)
    }

    Device(HS14)
    {
      Name(_ADR, 0xE)
    }

    Device(SS07)
    {
      Method(_ADR) { Return (Add(SSPA(),6)) }
    }

    Device(SS08)
    {
      Method(_ADR) { Return (Add(SSPA(),7)) }
    }

    Device(SS09)
    {
      Method(_ADR) { Return (Add(SSPA(),8)) }
    }

    Device(SS10)
    {
      Method(_ADR) { Return (Add(SSPA(),9)) }
    }

  } //scope
} // if
