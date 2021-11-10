/**@file

 Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock (
    "UsbPortXhciEhlLp4xCrb.aml",
    "SSDT",
    0x02, // SHAD revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
    "INTEL", // OEM ID (6 byte string)
    "xh_elcrb", // OEM table ID  (8 byte string) BoardIdEhlLp4xType3Crb
    0x0 // OEM version of DSDT table (4 byte Integer)
    )
{
  External(\_SB.PC00.XHCI.RHUB.HS01, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS02, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS03, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS04, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS05, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS06, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS07, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS08, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS09, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS11, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.HS12, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS01, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS02, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS03, DeviceObj)
  External(\_SB.PC00.XHCI.RHUB.SS04, DeviceObj)

  External(\_SB.PC00.RP07.PXSX.WIST, MethodObj)

  External(SXI1,IntObj) // SdevXhciInterfaceNumber1
  External(SXI2,IntObj) // SdevXhciInterfaceNumber2
  External(SXP1,IntObj) // SdevXhciRootPortNumber1
  External(SXP2,IntObj) // SdevXhciRootPortNumber2

  External(ATDV)
  External(BTSE)
  External(BTBR)
  External(BED2)
  External(BED3)
  External(BTLE)
  External(BTL2)
  External(BTLL)

  Method(GPLD, 2, Serialized) {
    //
    // Arg0:  Visible
    // Arg1:  Group position
    //
    Name(PCKG, Package() { Buffer(0x10) {} })
    CreateField(DerefOf(Index(PCKG,0)), 0, 7, REV)
    Store(2,REV)
    CreateField(DerefOf(Index(PCKG,0)), 7, 1, RGB)
    Store(1,RGB)
    CreateField(DerefOf(Index(PCKG,0)), 64, 1, VISI) // Arg 0
    Store(Arg0, VISI)
    CreateField(DerefOf(Index(PCKG,0)), 87, 8, GPOS) // Arg 1
    Store(Arg1, GPOS)
    return (PCKG)
  }

  Method(GUPC, 2, Serialized) {
    /*
    Arg0: Connectable
    Non-Zero value: Connectable

    Arg1: Type
    0x00: Type A connector
    0x01: Mini-AB connector
    0x02: ExpressCard
    0x03: USB 3 Standard-A connector
    0x04: USB 3 Standard-B connector
    0x05: USB 3 Micro-B connector
    0x06: USB 3 Micro-AB connector
    0x07: USB 3 Power-B connector
    0x08: Type C connector - USB2-only
    0x09: Type C connector - USB2 and SS with Switch
    0x0A: Type C connector - USB2 and SS without Switch
    0x0B - 0xFE: Reserved
    0xFF: Proprietary connector
    */
    Name(PCKG, Package(4) { 0xFF, 0xFF, 0, 0 } )
    Store(Arg0,Index(PCKG,0))
    Store(Arg1,Index(PCKG,1))
    return (PCKG)
  }
  //
  // PCH XHCI Ports
  //
  Scope (\_SB.PC00.XHCI.RHUB.HS01) {
    // Maps to Type C connector - USB2 and SS with Switch
    Method(_UPC) {
      Return (GUPC(1, 0x09))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x01))
    }
    If (LAnd(CondRefOf(SXI1),CondRefOf(SXP1))) {
      If(LAnd(LGreater(SXI1,0),LEqual(SXP1,1))) {
        Device (CIR) {
          ///
          /// Address is Port + Interface number
          ///
          Method (_ADR, 0) {
            Return (Add(1,SXI1))
          }
        }
      }
    }
    If (LAnd(CondRefOf(SXI2),CondRefOf(SXP2))) {
      If(LAnd(LGreater(SXI2,0),LEqual(SXP2,1))) {
        Device (CIR2) {
          ///
          /// Address is Port + Interface number
          ///
          Method (_ADR, 0) {
            Return (Add(1,SXI2))
          }
        }
      }
    }
  }
  Scope (\_SB.PC00.XHCI.RHUB.HS02) {
    // Maps to USB3/2 Type A connector
    Method(_UPC) {
      Return (GUPC(1, 0x03))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x02))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS03) {
    // Maps to USB3/2 Type A connector
    Method(_UPC) {
      Return (GUPC(1, 0x03))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x03))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS04) {
    // Maps to USB3/2 Type A connector
    Method(_UPC) {
      Return (GUPC(1, 0x03))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x04))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS05) {
    // Maps to M2
    Method(_UPC) {
      Return (GUPC(0, 0xFF))
    }
    Method(_PLD) {
      Return (GPLD(0x00, 0x00))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS06) {
    // Unused
    Method(_UPC) {
      Return (GUPC(0,0xFF))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS07) {
    // Unused
    Method(_UPC) {
      Return (GUPC(0,0xFF))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS08) {
    // Unused
    Method(_UPC) {
      Return (GUPC(0,0xFF))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS09) {
    // Unused
    Method(_UPC) {
      Return (GUPC(0,0xFF))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS10) {
    // Maps to M2
    Method(_UPC) {
      Return (GUPC(0, 0xFF))
    }
    Method(_PLD) {
      Return (GPLD(0x00, 0x00))
    }
    If (LAnd(CondRefOf(SXI1),CondRefOf(SXP1))) {
      If(LAnd(LGreater(SXI1,0),LEqual(SXP1,10))) {
        Device (CIR) {
          ///
          /// Address is Port + Interface number
          ///
          Method (_ADR, 0) {
            Return (Add(10,SXI1))
          }
        }
      }
    }
    If (LAnd(CondRefOf(SXI2),CondRefOf(SXP2))) {
      If(LAnd(LGreater(SXI2,0),LEqual(SXP2,10))) {
        Device (CIR2) {
          ///
          /// Address is Port + Interface number
          ///
          Method (_ADR, 0) {
            Return (Add(10,SXI2))
          }
        }
      }
    }
  }
  If (\_SB.PC00.RP07.PXSX.WIST()) {
    Scope (\_SB.PC00.XHCI.RHUB.HS10) {
      Include("AntennaDiversity.asl")
      Include("BtRegulatory.asl")
      Include("GeneralPurposeConfig.asl")
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS11) {
    // Unused
    Method(_UPC) {
      Return (GUPC(0,0xFF))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.HS12) {
    // Unused
    Method(_UPC) {
      Return (GUPC(0,0xFF))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS01) {
    // Maps to Type C connector - USB2 and SS with Switch
    Method(_UPC) {
      Return (GUPC(1, 0x09))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x01))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS02) {
    // Maps to USB3/2 Type A connector
    Method(_UPC) {
      Return (GUPC(1,0x03))
    }
    Method(_PLD) {
      Return (GPLD(1,0x02))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS03) {
    // Maps to USB3/2 Type A connector
    Method(_UPC) {
      Return (GUPC(1, 0x03))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x03))
    }
  }

  Scope (\_SB.PC00.XHCI.RHUB.SS04) {
    // Maps to USB3/2 Type A connector
    Method(_UPC) {
      Return (GUPC(1, 0x03))
    }
    Method(_PLD) {
      Return (GPLD(1, 0x04))
    }
  }
}
