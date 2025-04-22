/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/UsbRegs.h>

External(\_SB.PC00.XHCI.PS0X, MethodObj)
External(\_SB.PC00.XHCI.PS3X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.PS0X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.PS2X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.PS3X, MethodObj)
External(\_SB.PC00.XHCI.RHUB.INIR, MethodObj)
External(DUWS)

External(PU2C) // PCH USB2 Port count from NVS
External(PU3C) // PCH USB3 Port count from NVS

Scope(\_SB_.PC00) {
  Device(XHCI) {

    Name(_ADR, 0x00140000)

    OperationRegion(XPRT,PCI_Config,0x00,0x100)
    Field(XPRT,AnyAcc,NoLock,Preserve)
    {
      DVID, 16,
      Offset(0x10),
      XADL,  32,
      XADH,  32,
      Offset(R_XHCI_CFG_PWR_CNTL_STS), //0x74 PCI Power management control/status, PM_CS
      ,8,
      PMEE,1,
      ,6,
      PMES,1,
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
      If(CondRefOf(DUWS)) {
        Return(GPRW(0x6D, DUWS)) // can wakeup from deepest USB Sleep Wake State
      } Else {
        Return(GPRW(0x6D, 4)) // can wakeup from S4 state
      }
    }

    Method(GPEH) {
      If(LEqual(^DVID,0xFFFF))
      {
        Return
      }
      If(LEqual(PMES,1)) {
        Notify(\_SB.PC00.XHCI, 0x02)
      }
    }

    Method(_DSW, 3) {}

    Method(_INI) {
      If(CondRefOf(\_SB.PC00.XHCI.RHUB.INIR)) {      // _INI for RTD3
        \_SB.PC00.XHCI.RHUB.INIR()
      }
    }

    //
    // USRA (USbR port Address), method for calculating address of first USBR port in XHCI controller
    //  Arg0: Number of USB2 Port
    //
    Method(USRA,1,Serialized) {
      Return (Add (PU2C, Arg0))
    }

    //
    // SSPA (SuperSpeed Port Address), method for calculating address of first SS port in XHCI controller
    //  Arg0: Number of Super Speed Port
    //
    Method(SSPA,1,Serialized) {
      Return (Add (PU2C, Add (2, Arg0)))
    }

    Method(_PS0,0,Serialized)
    {
      ADBG("XHCI D0")

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
      ADBG("XHCI D3")

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
        ADBG("XHCI RHUB PS0")
        If(LEqual(\_SB.PC00.XHCI.DVID,0xFFFF))
        {
          Return()
        }
        //
        // Call platform XHCI.RHUB PS0 method if present.
        //
        If(CondRefOf(\_SB.PC00.XHCI.RHUB.PS0X))
        {
          ADBG("XHCI CSD0")
          \_SB.CSD0(MODPHY_SPD_GATING_XHCI)
          \_SB.PC00.XHCI.RHUB.PS0X()
        }
      }

      // PS2 Method for xHCI Root Hub
      Method(_PS2,0,Serialized)
      {
        ADBG("XHCI RHUB PS2")
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
          ADBG("XHCI CSD3")
          \_SB.CSD3(MODPHY_SPD_GATING_XHCI)
        }
      }

      // PS3 Method for xHCI Root Hub
      Method(_PS3,0,Serialized)
      {
        ADBG("XHCI RHUB PS3")
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
          ADBG("XHCI CSD3")
          \_SB.CSD3(MODPHY_SPD_GATING_XHCI)
        }
      }
    } // device rhub
  } // device xhc
} //scope

If(CondRefOf(PU2C)) {
  Scope(\_SB.PC00.XHCI.RHUB) {
    //
    // High Speed Ports (without USBR)
    //
    If (LLessEqual (1, PU2C)) {
      Device (HS01) {
        Name (_ADR, 1)
      }
    }

    If (LLessEqual (2, PU2C)) {
      Device (HS02) {
        Name (_ADR, 2)
      }
    }

    If (LLessEqual (3, PU2C)) {
      Device (HS03) {
        Name (_ADR, 3)
      }
    }

    If (LLessEqual (4, PU2C)) {
      Device (HS04) {
        Name (_ADR, 4)
      }
    }

    If (LLessEqual (5, PU2C)) {
      Device (HS05) {
        Name (_ADR, 5)
      }
    }

    If (LLessEqual (6, PU2C)) {
      Device (HS06) {
        Name (_ADR, 6)
      }
    }

    If (LLessEqual (7, PU2C)) {
      Device (HS07) {
        Name (_ADR, 7)
      }
    }

    If (LLessEqual (8, PU2C)) {
      Device (HS08) {
        Name (_ADR, 8)
      }
    }

    If (LLessEqual (9, PU2C)) {
      Device (HS09) {
        Name (_ADR, 9)
      }
    }

    If (LLessEqual (10, PU2C)) {
      Device (HS10) {
        Name (_ADR, 10)
      }
    }

    If (LLessEqual (11, PU2C)) {
      Device (HS11) {
        Name (_ADR, 11)
      }
    }

    If (LLessEqual (12, PU2C)) {
      Device (HS12) {
        Name (_ADR, 12)
      }
    }

    If (LLessEqual (13, PU2C)) {
      Device (HS13) {
        Name (_ADR, 13)
      }
    }

    If (LLessEqual (14, PU2C)) {
      Device (HS14) {
        Name (_ADR, 14)
      }
    }

    //
    // USBR ports definitions for supported PCH
    //
    Device(USR1) {
      Method(_ADR) { Return (USRA (1))}
    }

    Device(USR2) {
      Method(_ADR) { Return (USRA (2))}
    }
  }
}

If(CondRefOf(PU3C)) {
  Scope(\_SB.PC00.XHCI.RHUB) {
    //
    // Super Speed Ports
    //
    If (LLessEqual (1, PU3C)) {
      Device (SS01) {
        Method (_ADR) { Return (SSPA (1)) }
      }
    }

    If (LLessEqual (2, PU3C)) {
      Device (SS02) {
        Method (_ADR) { Return (SSPA (2)) }
      }
    }

    If (LLessEqual (3, PU3C)) {
      Device (SS03) {
        Method (_ADR) { Return (SSPA (3)) }
      }
    }

    If (LLessEqual (4, PU3C)) {
      Device (SS04) {
        Method (_ADR) { Return (SSPA (4)) }
      }
    }

    If (LLessEqual (5, PU3C)) {
      Device (SS05) {
        Method (_ADR) { Return (SSPA (5)) }
      }
    }

    If (LLessEqual (6, PU3C)) {
      Device (SS06) {
        Method (_ADR) { Return (SSPA (6)) }
      }
    }

    If (LLessEqual (7, PU3C)) {
      Device (SS07) {
        Method (_ADR) { Return (SSPA (7)) }
      }
    }

    If (LLessEqual (8, PU3C)) {
      Device (SS08) {
        Method (_ADR) { Return (SSPA (8)) }
      }
    }

    If (LLessEqual (9, PU3C)) {
      Device (SS09) {
        Method (_ADR) { Return (SSPA (9)) }
      }
    }

    If (LLessEqual (10, PU3C)) {
      Device (SS10) {
        Method (_ADR) { Return (SSPA (10)) }
      }
    }
  } // end RHUB scope
} // end if
