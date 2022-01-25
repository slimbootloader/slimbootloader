/** @file
  This file contains the TCSS XHCI ASL code.
  It defines a Power Management for TCSS XHCI

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  //
  // The deepest D-state supported by this device in the S0 system sleeping state where the device can wake itself,
  // "4" represents D3cold.
  //
  Method(_S0W, 0x0, NotSerialized)
  {
    If (TRTD) {
      Return(0x4)
    } Else {
      Return(0x3)
    }
  }
  //
  // Variable to skip TCSS/TBT D3 cold; 1+: Skip D3CE, 0 - Enable D3CE
  // TCSS D3 Cold and TBT RTD3 is only available when system power state is in S0.
  //
  Name(SD3C, 0)
  //
  // Power Management routine for D3
  //
  If (TRTD) {
    // To evaluates to a list of power resources that are dependent on this device. For OSPM to "put
    // the device in" the D0 device state
    Method(_PR0) {
      Return (Package(){\_SB.PC00.D3C})
    }
    // To evaluates to a list of power resources upon which this device is dependent when it is in the D3hot state.
    // The PMSS is in D3H when this method is called. For devices that support both D3hot and D3 exposed to OSPM via _PR3,
    // device software/drivers must always assume OSPM will target D3 and must assume all device context will be lost
    // and the device will no longer be enumerable.
    Method(_PR3) {
      Return (Package(){\_SB.PC00.D3C})
    }
  }

  OperationRegion(XPRT,PCI_Config,0x00,0x100)
  //
  // Byte access for PMCS field to avoid race condition on device D-state
  //
  Field(XPRT,ByteAcc,NoLock,Preserve)
  {
    VDID, 32,
    Offset(R_XHCI_CFG_PWR_CNTL_STS), // 0x74
    D0D3,  2,  // 0x74 BIT[1:0]
        ,  6,
    PMEE,  1,  // PME Enable
        ,  6,
    PMES,  1,  // PME Status
  }

  //
  // Variable to store the maximum D state supported in S0.
  //
  Name (XFLT, 0)

  //
  // XHCI controller _DSM method
  //
  Method(_DSM,4,serialized)
  {
    ADBG("XHCI _DSM")

    If(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }

    Return(Buffer() {0})
  }


  //
  // _SXD and _SXW methods
  //
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

  //
  // Additionally if the device is capable of waking up
  // the platform from a low power System state (S3 or S4), there is a _S0W and _PRW
  // associated with the device as well.
  //
  Method(_PRW, 0) {
    //
    // Report the deepest wakeable state is S4 always in order to supporting TCSS RTD3 capability is some OS builds. The actual Sx wakeable state is mapped and managed by
    // VCCST's preference to IOM for remaining or removing VNNAON rail in S4 state. BIOS is no longer required to report _PRW with S3 wake capability to OS even if
    // the platform wants to limit USBC Sx wake capability to S4 only, or when VCCST support is disabled.
    // The actual S4 wake capability will be limited by VCST setting instead of OS managment. Platform shall set VCCST setting to OFF when not supporting S4 wake over USBC.
    //
    Return (GPRW (0x6D, 4)) // can wakeup from S4 state
  }

  Method(_DSW, 3)
  {
    ADBG("TCSS XHCI _DSW")
    ADBG(Concatenate("Arg0 -", ToHexString(Arg0)))
    ADBG(Concatenate("Arg1 -", ToHexString(Arg1)))
    ADBG(Concatenate("Arg2 -", ToHexString(Arg2)))
    C2PM (Arg0, Arg1, Arg2, DCPM)
    Store(Arg1, SD3C)                                 // If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold.
    ADBG(Concatenate("SD3C -", ToHexString(SD3C)))
  }

  //
  // xHCI Root Hub Device
  //
  Device(RHUB)
  {
    Name(_ADR, Zero)

    //
    // Method for creating Type C _PLD buffers
    // _PLD contains lots of data, but for purpose of internal validation we care only about
    // ports visibility and pairing (this requires group position)
    // so these are the only 2 configurable parameters
    //
    Method(TPLD, 2, Serialized) { // For the port related with USB Type C.
      // Arg0:  Visible
      // Arg1:  Group position
      Name(PCKG, Package() { Buffer(0x10) {} } )
      CreateField(DerefOf(Index(PCKG,0)), 0, 7, REV)
      Store(1,REV)
      CreateField(DerefOf(Index(PCKG,0)), 64, 1, VISI)
      Store(Arg0, VISI)
      CreateField(DerefOf(Index(PCKG,0)), 87, 8, GPOS)
      Store(Arg1, GPOS)
      // For USB type C, Standerd values
      CreateField(DerefOf(Index(PCKG,0)), 74, 4, SHAP)  // Shape set to Oval
      Store(1, SHAP)
      CreateField(DerefOf(Index(PCKG,0)), 32, 16, WID)  // Width of the connector, 8.34mm
      Store(8, WID)
      CreateField(DerefOf(Index(PCKG,0)), 48, 16, HGT)  // Height of the connector, 2.56mm
      Store(3, HGT)
      return (PCKG)
    }

    //
    // Method for creating Type C _UPC buffers
    // Similar to _PLD, for internal testing we only care about 1 parameter
    //
    Method(TUPC, 2, Serialized) { // For the port related with USB Type C.
      // Arg0: Connectable
      // Arg1: Type
      // Type:
      //  0x08:     Type C connector - USB2-only
      //  0x09:     Type C connector - USB2 and SS with Switch
      //  0x0A:     Type C connector - USB2 and SS without Switch
      Name(PCKG, Package(4) { 1, 0x00, 0, 0 } )
      Store(Arg0,Index(PCKG,0))
      Store(Arg1,Index(PCKG,1))
      return (PCKG)
    }

    //
    // High Speed Ports
    //
    Device(HS01)
    {
      Name(_ADR, 0x01)
      // TODO: Check what to do about _UPC & _PLD
    }

    //
    // Super Speed Ports
    //
    Device(SS01)
    {
      Name(_ADR, 0x02)

      If(CondRefOf(U4SE))
      {
        If (LEqual (U4SE, 1))
        {
          Method (_DSD, 0) {
            ADBG ("XHC SS01 _DSD")
            Return (
              Package () {
                ToUUID ("DAFFD814-6EBA-4D8C-8A91-BC9BBF4AA301"),
                Package () {
                  Package (2) {"usb4-host-interface", \_SB.PC00.TDM0},
                  Package (2) {"usb4-port-number", 0}
                }
              }
            ) // End of Return ()
          }
        }
      }
    }

    Device(SS02)
    {
      Name(_ADR, 0x03)

      If(CondRefOf(U4SE))
      {
        If (LEqual (U4SE, 1))
        {
          Method (_DSD, 0) {
            ADBG ("XHC SS02 _DSD")
            Return (
              Package () {
                ToUUID ("DAFFD814-6EBA-4D8C-8A91-BC9BBF4AA301"),
                Package () {
                  Package (2) {"usb4-host-interface", \_SB.PC00.TDM0},
                  Package (2) {"usb4-port-number", 1}
                }
              }
            ) // End of Return ()
          }
        }
      }
    }

    Device(SS03)
    {
      Name(_ADR, 0x04)

      If(CondRefOf(U4SE))
      {
        If (LEqual (U4SE, 1))
        {
          Method (_DSD, 0) {
            ADBG ("XHC SS03 _DSD")
            Return (
              Package () {
                ToUUID ("DAFFD814-6EBA-4D8C-8A91-BC9BBF4AA301"),
                Package () {
                  Package (2) {"usb4-host-interface", \_SB.PC00.TDM1},
                  Package (2) {"usb4-port-number", 2}
                }
              }
            ) // End of Return ()
          }
        }
      }
    }

    Device(SS04)
    {
      Name(_ADR, 0x05)

      If(CondRefOf(U4SE))
      {
        If (LEqual (U4SE, 1))
        {
          Method (_DSD, 0) {
            ADBG ("XHC SS04 _DSD")
            Return (
              Package () {
                ToUUID ("DAFFD814-6EBA-4D8C-8A91-BC9BBF4AA301"),
                Package () {
                  Package (2) {"usb4-host-interface", \_SB.PC00.TDM1},
                  Package (2) {"usb4-port-number", 3}
                }
              }
            ) // End of Return ()
          }
        }
      }
    }

    Method(_PS0,0,Serialized)
    {
      ADBG("TCSS RHUB XHCI PS0")
    }

    Method(_PS2,0,Serialized)
    {
      ADBG("TCSS RHUB XHCI PS2")
    }

    Method(_PS3,0,Serialized)
    {
      ADBG("TCSS RHUB XHCI PS3")
    }

  } // End of Device(RHUB)
