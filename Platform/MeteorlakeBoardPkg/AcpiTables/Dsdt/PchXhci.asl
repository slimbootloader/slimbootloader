/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/UsbHostRegs.h>

  OperationRegion (XPRT,PCI_Config,0x00,0x100)
  Field (XPRT,AnyAcc,NoLock,Preserve)
  {
    DVID, 16,
    Offset (0x10),
    XADL,  32,
    XADH,  32,
    Offset (R_XHCI_CFG_PCE_REG), // 0xA2
        ,  2,
    D3HE,  1,  // D3_hot_en
  }

    //
    // XHCI controller _DSM method
    //
    Method(_DSM,4,serialized){
      If (PCIC (Arg0)) { return (PCID (Arg0,Arg1,Arg2,Arg3)) }

      If (LEqual (Arg0, ToUUID ("203B203B-1F13-409A-9AAE-1A0347525584"))) {
        Switch (ToInteger (Arg2)) {
          Case (0) {
            // Function 0: return Bit map to indicate Function 0,1 is supported
            Return (Buffer () {0x3})
          }
          Case (1) {
            If (CondRefOf (AHWA)) {
              Return (LAnd (AHWA, 1))
            } Else {
              Return (0)
            }
          }
        }
      }
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

  Method (_S0W, 0x0, NotSerialized)
  {
    Return (0x3)
  }

    Method(_DSW, 3) {}

  Method (_INI) {
    External (^RHUB.INIR, MethodObj)

    If (CondRefOf (^RHUB.INIR)) {      // _INI for RTD3
      ^RHUB.INIR ()
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

  Method (_PS0,0,Serialized)
  {
    External (PS0X, MethodObj)

    If(LEqual(^DVID,0xFFFF))
    {
      Return()
    }

    //
    // Call platform XHCI PS0 method if present
    //
    If (CondRefOf (PS0X))
    {
      PS0X ()
    }
  }

  Method (_PS3,0,Serialized)
  {
    External (PS3X, MethodObj)

    If(LEqual(^DVID,0xFFFF))
    {
      Return()
    }

    //
    // Call platform XHCI PS3 method if present
    //
    If (CondRefOf (PS3X))
    {
      PS3X ()
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

  Device (RHUB)
  {
    Name (_ADR, Zero)
    External (PS0X, MethodObj)
    External (PS2X, MethodObj)
    External (PS3X, MethodObj)

    // PS0 Method for xHCI Root Hub
    Method (_PS0,0,Serialized)
    {
      If (LEqual (^^DVID,0xFFFF))
      {
        Return ()
      }
      //
      // Call platform XHCI.RHUB PS0 method if present.
      //
      If (CondRefOf (PS0X))
      {
        \_SB.CSD0 (MODPHY_SPD_GATING_XHCI)
        PS0X ()
      }
    }

    // PS2 Method for xHCI Root Hub
    Method (_PS2,0,Serialized)
    {
      If (LEqual (^^DVID,0xFFFF))
      {
        Return ()
      }
      //
      // Call platform XHCI.RHUB PS2 method if present.
      //
      If (CondRefOf (PS2X))
      {
        PS2X ()
        \_SB.CSD3 (MODPHY_SPD_GATING_XHCI)
      }
    }

    // PS3 Method for xHCI Root Hub
    Method (_PS3,0,Serialized)
    {
      If (LEqual (^^DVID,0xFFFF))
      {
        Return ()
      }
      //
      // Call platform XHCI.RHUB PS3 method if present.
      //
      If (CondRefOf (PS3X))
      {
        PS3X ()
        \_SB.CSD3 (MODPHY_SPD_GATING_XHCI)
      }
    }

    If (CondRefOf (PU2C)) {
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

          External (VTCM, IntObj)
          If (LAnd (CondRefOf (VTCM), LEqual (VTCM, 1))) { // This entry will expose only when Expose "XHCI SDEV Entry" is enabled
            Name (_UPC, Package() {
              0xFF, // Port is connectible
              0xFF, // Connector type(N/A for non-visible ports)
              0x00000000, // reserved must be zero
              0x00000000}) // reserved must be zero

            Name (_PLD, Package (1) {
              Buffer (0x14) {
                0x82, 0x00, 0x00, 0x00, // Revision 2, Ignore color width and height is not required as this is an embedded connector
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x80, 0x80, 0x00, // Not user visible
                0x00, 0x00, 0x00, 0x00,
                0xFF, 0xFF, 0xFF, 0xFF}}) // Not ejectable

            Device (CIR) {
              Method (_ADR)
              {
                Return (Add (5, SXI1)) // Address = Port + Interface
              }
              Name (_PLD, Package (1) {
                Buffer (0x14) {
                  0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x24, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0xc8, 0x00, 0xa0, 0x00 }}) // Not ejectable
            }
          }
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

      Device (USR2) {
        Method (_ADR) { Return (USRA (2))}
      }
    }

    If (CondRefOf (PU3C)) {
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
    } // end if
  } // device rhub
