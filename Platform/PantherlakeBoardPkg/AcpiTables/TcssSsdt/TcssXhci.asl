/** @file
  This file contains the TCSS XHCI ASL code. It defines a Power Management for TCSS XHCI

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

  External (\_SB.TPLD, MethodObj)
  External (\_SB.TUPC, MethodObj)

  External (\_SB.E1PL, MethodObj)
  External (\_SB.E1UP, MethodObj)
  External (\_SB.E2PL, MethodObj)
  External (\_SB.E2UP, MethodObj)
  External (\_SB.E3PL, MethodObj)
  External (\_SB.E3UP, MethodObj)
  External (\_SB.E4PL, MethodObj)
  External (\_SB.E4UP, MethodObj)
  External (\E1CP, IntObj)
  External (\E2CP, IntObj)
  External (\E3CP, IntObj)
  External (\E4CP, IntObj)

  Method (BASE, 0)
  {
    Store (And (_ADR, 0x7), Local0) // Function Number
    Store (And (ShiftRight (_ADR, 16), 0x1F), Local1) // Device Number
    Add (ShiftLeft (Local0, 12), ShiftLeft (Local1, 15), Local2)
    Add (\_SB.PC00.GPCB(), Local2, Local3)
    ADBG (Concatenate ("TCSS XHCI PCIe MMIO Address", ToHexString (Local3)))
    Return (Local3)
  }

  Method (_PS0,0,Serialized)
  {
    ADBG ("TCSS XHCI _PS0 Start")
    If (LEqual (\_SB.PC00.TXHC.PMEE, 1)) {
      ADBG ("Clear PME_EN of TCSS XHCI")
      Store (0, \_SB.PC00.TXHC.PMEE)
    }
    ADBG ("TCSS XHCI _PS0 End")
  }

  Method (_PS3,0,Serialized)
  {
    ADBG ("TCSS XHCI _PS3 Start")
    If (LEqual (\_SB.PC00.TXHC.PMEE, 0)) {
      ADBG ("PME_EN was not set. Set PME_EN of TCSS XHCI")
      Store (1, \_SB.PC00.TXHC.PMEE)
    }
    ADBG ("TCSS XHCI _PS3 End")
  }

  //
  // The deepest D-state supported by this device in the S0 system sleeping state where the device can wake itself,
  // "4" represents D3cold.
  //
  Method (_S0W, 0x0, NotSerialized)
  {
    If (TRTD) {
      Return (0x4)
    } Else {
      Return (0x3)
    }
  }
  //
  // Variable to skip TCSS/TBT D3 cold; 1+: Skip D3CE, 0 - Enable D3CE
  // TCSS D3 Cold and TBT RTD3 is only available when system power state is in S0.
  //
  Name (SD3C, 0)
  //
  // Power Management routine for D3
  //
  If (TRTD) {
    // To evaluates to a list of power resources that are dependent on this device. For OSPM to "put
    // the device in" the D0 device state
    Method (_PR0) {
      Return (Package () {\_SB.PC00.D3C})
    }
    // To evaluates to a list of power resources upon which this device is dependent when it is in the D3hot state.
    // The PMSS is in D3H when this method is called. For devices that support both D3hot and D3 exposed to OSPM via _PR3,
    // device software/drivers must always assume OSPM will target D3 and must assume all device context will be lost
    // and the device will no longer be enumerable.
    Method (_PR3) {
      Return (Package () {\_SB.PC00.D3C})
    }
  }

  OperationRegion(XPRT,SystemMemory,BASE(),0x100)
  //
  // Byte access for PMCS field to avoid race condition on device D-state
  //
  Field (XPRT,ByteAcc,NoLock,Preserve)
  {
    VDID, 32,
    Offset (R_XHCI_CFG_PM_CS), // 0x74
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
  Method (_DSM,4,serialized)
  {
    ADBG ("XHCI _DSM")

    If (PCIC (Arg0)) { return (PCID (Arg0,Arg1,Arg2,Arg3)) }

    Return (Buffer () {0})
  }


  //
  // _SXD and _SXW methods
  //
  Method (_S3D, 0, NotSerialized)
  {
    Return (3)
  }
  Method (_S4D, 0, NotSerialized)
  {
    Return (3)
  }
  Method (_S3W, 0, NotSerialized)
  {
    Return (3)
  }
  Method (_S4W, 0, NotSerialized)
  {
    Return (3)
  }

  Method (_DSW, 3)
  {
    ADBG ("TCSS XHCI _DSW")
    ADBG (Concatenate ("Arg0 -", ToHexString (Arg0)))
    ADBG (Concatenate ("Arg1 -", ToHexString (Arg1)))
    ADBG (Concatenate ("Arg2 -", ToHexString (Arg2)))
    C2PM (Arg0, Arg1, Arg2, DCPM)
    Store (Arg1, SD3C)                                 // If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold.
    ADBG (Concatenate ("SD3C -", ToHexString (SD3C)))
  }

  //
  // xHCI Root Hub Device
  //
  Device (RHUB)
  {
    Name (_ADR, Zero)

    //
    // TCSS XHCI controller _DSM method
    //
    Method (_DSM,4,serialized)
    {
      If(LEqual(Arg0, ToUUID("ce2ee385-00e6-48cb-9f05-2edb927c4899"))) {
        ADBG ("[TCSS] XHCI RHUB _DSM")
        If (LEqual (Arg2, 0)) {
          ADBG("[TCSS] XHCI RHUB F0")
          Return (Buffer () {0x81})
        } // function 0: list of supported functions
        If (LEqual (Arg2, 7)) {
          ADBG("[TCSS] XHCI RHUB F7")
          Return (0x01)
        } // function 7: providing _UPC supported or NOT
      }
      Return (Buffer () {0})
    }

    //
    // High Speed Ports
    //
    Device (HS01)
    {
      Name (_ADR, 0x01)
      //
      // HS01 port is not functional
      // Report as not visible and not connectable
      //
      Method(_UPC, 0, Serialized) {
        Return (\_SB.TUPC (0, 0xFF, 0x00000000))
      }
      Method(_PLD, 0, Serialized) {
        Return (\_SB.TPLD (0, 0))
      }
    }

    //
    // Super Speed Ports
    //
    Device (SS01)
    {
      Name (_ADR, 0x02)

      If (CondRefOf (U4CM)) {
        If (LAnd (LEqual (And (U4CM, 0x07), 1), LEqual (DME0, 1))) {
          If (CondRefOf (E1CP)) {
            If (LEqual (And (E1CP, 0x10), 0x10)) { // Check bit4 which indicates USB4 support
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
      }

      Method (_UPC, 0) {
        If (CondRefOf (\_SB.E1UP)) {
          Return (\_SB.E1UP ())
        } Else {
          Return (Package (4) {0x0, 0xFF, 0x0, 0x0})
        }
      }
      Method (_PLD, 0) {
        If (CondRefOf (\_SB.E1PL)) {
          Return (\_SB.E1PL ())
        } Else {
          Return (Package (1) {Buffer (0x14) {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}})
        }
      }

      // USB Device-Specific Method
      Method (_DSM, 4, NotSerialized) {
        ADBG ("TCSS XHCI SS01 _DSM")
        If (LEqual (Arg0, ToUUID ("ce2ee385-00e6-48cb-9f05-2edb927c4899"))) {
          ADBG ("TCSS XHCI SS01 GUID")
          If (LEqual (Arg2, 0)) {
            ADBG ("TCSS XHCI SS01 F0")
            Return (Buffer () {0x21})
          }
          If (LEqual (Arg2, 5)) {                            // Disable U1 and U2 transitions for a port
            ADBG ("TCSS XHCI SS01 F5")
            If (LEqual (Arg1, 0)) {
              If (LEqual (And (RTBM, 0x01), 0x01)) {
                ADBG ("XHC SS01 _DSM U1U2 disable")
                Return (0x01)                                // U1 and U2 transitions will be disabled.
              }
              Return (0)
            }
          }
        }
        Return (Buffer () {0})
      }
    }

    Device (SS02)
    {
      Name (_ADR, 0x03)

      If (CondRefOf (U4CM)) {
        If (LAnd (LEqual (And (U4CM, 0x07), 1), LEqual (DME0, 1))) {
          If (CondRefOf (E2CP)) {
            If (LEqual (And (E2CP, 0x10), 0x10)) { // Check bit4 which indicates USB4 support
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
      }

      Method (_UPC, 0) {
        If (CondRefOf (\_SB.E2UP)) {
          Return (\_SB.E2UP ())
        } Else {
          Return (Package (4) {0x0, 0xFF, 0x0, 0x0})
        }
      }
      Method (_PLD, 0) {
        If (CondRefOf (\_SB.E2PL)) {
          Return (\_SB.E2PL ())
        } Else {
          Return (Package (1) {Buffer (0x14) {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}})
        }
      }

      // USB Device-Specific Method
      Method (_DSM, 4, NotSerialized) {
        ADBG ("TCSS XHCI SS02 _DSM")
        If (LEqual (Arg0, ToUUID ("ce2ee385-00e6-48cb-9f05-2edb927c4899"))) {
          ADBG ("TCSS XHCI SS02 GUID")
          If (LEqual (Arg2, 0)) {
            ADBG ("TCSS XHCI SS02 F0")
            Return (Buffer () {0x21})
          }
          If (LEqual (Arg2, 5)) {                            // Disable U1 and U2 transitions for a port
            ADBG ("TCSS XHCI SS02 F5")
            If (LEqual (Arg1, 0)) {
              If (LEqual (And (RTBM, 0x02), 0x02)) {
                ADBG ("XHC SS02 _DSM U1U2 disable")
                Return (0x01)                                // U1 and U2 transitions will be disabled.
              }
              Return (0)
            }
          }
        }
        Return (Buffer() {0})
      }
    }

    Device (SS03)
    {
      Name (_ADR, 0x04)

      If (CondRefOf (U4CM)) {
        If (LAnd (LEqual (And (U4CM, 0x07), 1), LEqual (DME1, 1))) {
          If (CondRefOf (E3CP)) {
            If (LEqual (And (E3CP, 0x10), 0x10)) { // Check bit4 which indicates USB4 support
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
      }

      Method (_UPC, 0) {
        If (CondRefOf (\_SB.E3UP)) {
          Return (\_SB.E3UP ())
        } Else {
          Return (Package (4) {0x0, 0xFF, 0x0, 0x0})
        }
      }
      Method (_PLD, 0) {
        If (CondRefOf (\_SB.E3PL)) {
          Return (\_SB.E3PL ())
        } Else {
          Return (Package (1) {Buffer (0x14) {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}})
        }
      }

      // USB Device-Specific Method
      Method (_DSM, 4, NotSerialized) {
        ADBG ("TCSS XHCI SS03 _DSM")
        If (LEqual (Arg0, ToUUID ("ce2ee385-00e6-48cb-9f05-2edb927c4899"))) {
          ADBG ("TCSS XHCI SS03 GUID")
          If (LEqual (Arg2, 0)) {
            ADBG ("TCSS XHCI SS03 F0")
            Return (Buffer () {0x21})
          }
          If (LEqual (Arg2, 5)) {                            // Disable U1 and U2 transitions for a port
            ADBG ("TCSS XHCI SS03 F5")
            If (LEqual (Arg1, 0)) {
              If (LEqual (And (RTBM, 0x04), 0x04)) {
                ADBG ("XHC SS03 _DSM U1U2 disable")
                Return (0x01)                                // U1 and U2 transitions will be disabled.
              }
              Return (0)
            }
          }
        }
        Return (Buffer () {0})
      }
    }

    Device (SS04)
    {
      Name (_ADR, 0x05)

      If (CondRefOf (U4CM)) {
        If (LAnd (LEqual (And (U4CM, 0x07), 1), LEqual (DME1, 1))) {
          If (CondRefOf (E4CP)) {
            If (LEqual (And (E4CP, 0x10), 0x10)) { // Check bit4 which indicates USB4 support
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
      }

      Method (_UPC, 0) {
        If (CondRefOf (\_SB.E4UP)) {
          Return (\_SB.E4UP ())
        } Else {
          Return (Package (4) {0x0, 0xFF, 0x0, 0x0})
        }
      }
      Method (_PLD, 0) {
        If (CondRefOf (\_SB.E4PL)) {
          Return (\_SB.E4PL ())
        } Else {
          Return (Package (1) {Buffer (0x14) {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}})
        }
      }

      // USB Device-Specific Method
      Method (_DSM, 4, NotSerialized) {
        ADBG ("TCSS XHCI SS04 _DSM")
        If (LEqual (Arg0, ToUUID ("ce2ee385-00e6-48cb-9f05-2edb927c4899"))) {
          ADBG ("TCSS XHCI SS04 GUID")
          If (LEqual (Arg2, 0)) {
            ADBG ("TCSS XHCI SS04 F0")
            Return (Buffer () {0x21})
          }
          If (LEqual (Arg2, 5)) {                            // Disable U1 and U2 transitions for a port
            ADBG ("TCSS XHCI SS04 F5")
            If (LEqual (Arg1, 0)) {
              If (LEqual (And (RTBM, 0x08), 0x08)) {
                ADBG ("XHC SS04 _DSM U1U2 disable")
                Return (0x01)                                // U1 and U2 transitions will be disabled.
              }
              Return (0)
            }
          }
        }
        Return (Buffer () {0})
      }
    }

    Method (_PS0,0,Serialized)
    {
      ADBG ("TCSS RHUB XHCI PS0")
    }

    Method (_PS2,0,Serialized)
    {
      ADBG ("TCSS RHUB XHCI PS2")
    }

    Method (_PS3,0,Serialized)
    {
      ADBG ("TCSS RHUB XHCI PS3")
    }

  } // End of Device (RHUB)