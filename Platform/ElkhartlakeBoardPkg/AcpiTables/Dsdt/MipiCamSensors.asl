/** @file
  ACPI DSDT table

 Copyright (c) 2013 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00) {

  Method (PINR, 2, Serialized) { // Create GPIO resource template buffer
    // Arg0 - Group Pad Number
    // Arg1 - Group Number
    Name (GPIR, ResourceTemplate () { GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly, "\\_SB.GPI0", 0x00, ResourceConsumer, GPID,) { 0xFFFF } })
    CreateWordField (GPIR, GPID._PIN, PINF)
    Store(Add(GINF(Arg1,GPIO_DRIVER_PIN_BASE_NUMBER),Arg0), PINF)
    Return (GPIR)
  }

  Method (DSMI, 4, Serialized) { // Create information about I2C device used by sensor driver in _DSM method
    // Arg0 - I2C Device Function
    // Arg1 - I2C Slave Address
    // Arg2 - I2C Device Speed
    // Arg3 - I2C Bus
    ShiftLeft (Arg3, 24, Local3)
    ShiftLeft (Arg2, 16, Local2)
    ShiftLeft (Arg1, 8, Local1)
    Or(Local1, Arg0, Local0)
    Or(Local0, Local2, Local0)
    Or(Local0, Local3, Local0)
    Return (Local0)
  }

  Method (GPPI, 4, Serialized) { // Create information about GPIO pin used by sensor driver in _DSM method
    // Arg0 - GPIO Pin Function
    // Arg1 - GPIO Pin Number
    // Arg2 - GPIO Pin Initial Value
    // Arg3 - GPIO Pin Active Value
    ShiftLeft (Arg3, 24, Local3)
    ShiftLeft (Arg2, 16, Local2)
    ShiftLeft (Arg1, 8, Local1)
    Or(Local1, Arg0, Local0)
    Or(Local0, Local2, Local0)
    Or(Local0, Local3, Local0)
    Return (Local0)
  }

  Method (GRID, 1, Serialized) { // Generate ID (_HID or _CID) for user defined sensor models
    // Arg0 - Link Number
    Name(BUF, Buffer(9) {})
    Switch (ToInteger(Arg0)) {
      case(0) {
      Store(L0H0, Index(BUF,0))
      Store(L0H1, Index(BUF,1))
      Store(L0H2, Index(BUF,2))
      Store(L0H3, Index(BUF,3))
      Store(L0H4, Index(BUF,4))
      Store(L0H5, Index(BUF,5))
      Store(L0H6, Index(BUF,6))
      Store(L0H7, Index(BUF,7))
      Store(L0H8, Index(BUF,8))
      }
      case(1) {
      Store(L1H0, Index(BUF,0))
      Store(L1H1, Index(BUF,1))
      Store(L1H2, Index(BUF,2))
      Store(L1H3, Index(BUF,3))
      Store(L1H4, Index(BUF,4))
      Store(L1H5, Index(BUF,5))
      Store(L1H6, Index(BUF,6))
      Store(L1H7, Index(BUF,7))
      Store(L1H8, Index(BUF,8))
      }
      case(2) {
      Store(L2H0, Index(BUF,0))
      Store(L2H1, Index(BUF,1))
      Store(L2H2, Index(BUF,2))
      Store(L2H3, Index(BUF,3))
      Store(L2H4, Index(BUF,4))
      Store(L2H5, Index(BUF,5))
      Store(L2H6, Index(BUF,6))
      Store(L2H7, Index(BUF,7))
      Store(L2H8, Index(BUF,8))
      }
      default {
      Store(L3H0, Index(BUF,0))
      Store(L3H1, Index(BUF,1))
      Store(L3H2, Index(BUF,2))
      Store(L3H3, Index(BUF,3))
      Store(L3H4, Index(BUF,4))
      Store(L3H5, Index(BUF,5))
      Store(L3H6, Index(BUF,6))
      Store(L3H7, Index(BUF,7))
      Store(L3H8, Index(BUF,8))
      }
    }
    Return (ToString(BUF))
  }

  Method (HCID, 1, Serialized) { // return fixed or dynamic _HID or _CID
    // Arg0 - Link Number
    Switch (ToInteger(Arg0)) {
      case(0) { Store(L0SM, Local0) }
      case(1) { Store(L1SM, Local0) }
      case(2) { Store(L2SM, Local0) }
      case(3) { Store(L3SM, Local0) }
      case(4) { Store(L4SM, Local0) }
      default { Store(L5SM, Local0) }
    }
    Switch (ToInteger(Local0)) {
      case(0) { Return ("INT3471") } // IMX135
      case(1) { Return ("INT33BE") } // OV5693
      case(2) { Return ("INT3476") } // IMX179
      case(3) { Return ("INT3477") } // OV8858
      case(4) { Return ("INT3474") } // OV2740
      case(5) { Return ("INT3473") } // OV9728
      case(6) { Return ("INT3475") } // IMX188
      case(7) { Return ("INT3478") } // IMX208
      case(8) { Return ("INT3479") } // OV5670
      case(9) { Return ("INT347A") } // OV8865
      case(10) { Return ("INT347B") } // HM2051
      case(11) { Return ("OVTI2742") } // OV2742
      case(12) { Return ("OVTI9234") } // OV9234
      case(13) { Return ("OVTI8856") } // OV8856
      case(14) { Return ("OVTIF860") } // OV16860
      case(15) { Return ("SONY362A") } // SONY362A
      case(16) { Return ("SONY488A") } // SONY488A
      default { Return (GRID(Arg0)) }
    }
  }

  Method (CDEP, 1, Serialized) { // return dependency
    // Arg0 - LxCL Control Logic Device
    If (LEqual (Arg0, 0)) { // If Control Logic Device0 is to be used
      If (LEqual (C0TP, 1)) { // If Type of control logic is Discrete
        Switch (ToInteger(L0BS)) { // I2C bus
          case (0) { Return (Package() {\_SB.PC00.DSC0, \_SB.PC00.I2C0}) }
          case (1) { Return (Package() {\_SB.PC00.DSC0, \_SB.PC00.I2C1}) }
          case (2) { Return (Package() {\_SB.PC00.DSC0, \_SB.PC00.I2C2}) }
          case (3) { Return (Package() {\_SB.PC00.DSC0, \_SB.PC00.I2C3}) }
          case (4) { Return (Package() {\_SB.PC00.DSC0, \_SB.PC00.I2C4}) }
          case (5) { Return (Package() {\_SB.PC00.DSC0, \_SB.PC00.I2C5}) }
          default { Return (Package() {\_SB.PC00.DSC0}) }
        }
      }
      If (LGreater(C0TP, 1)) { Return (Package() {\_SB.PC00.CLP0}) } // when Control Logic Type is of type PMIC add dependency on CLP0
    }
    If (LEqual (Arg0, 1)) { // If Control Logic Device1 is to be used
      If (LEqual (C1TP, 1)) { // If Type of control logic is Discrete
        Switch (ToInteger(L1BS)) { // I2C bus
          case (0) { Return (Package() {\_SB.PC00.DSC1, \_SB.PC00.I2C0}) }
          case (1) { Return (Package() {\_SB.PC00.DSC1, \_SB.PC00.I2C1}) }
          case (2) { Return (Package() {\_SB.PC00.DSC1, \_SB.PC00.I2C2}) }
          case (3) { Return (Package() {\_SB.PC00.DSC1, \_SB.PC00.I2C3}) }
          case (4) { Return (Package() {\_SB.PC00.DSC1, \_SB.PC00.I2C4}) }
          case (5) { Return (Package() {\_SB.PC00.DSC1, \_SB.PC00.I2C5}) }
          default { Return (Package() {\_SB.PC00.DSC1}) }
        }
      }
      If (LGreater(C1TP, 1)) { Return (Package() {\_SB.PC00.CLP1}) } // when Control Logic Type is of type PMIC add dependency on CLP1
    }
    If (LEqual (Arg0, 2)) { // If Control Logic Device2 is to be used
      If (LEqual (C2TP, 1)) { // If Type of control logic is Discrete
        Switch (ToInteger(L2BS)) { // I2C bus
          case (0) { Return (Package() {\_SB.PC00.DSC2, \_SB.PC00.I2C0}) }
          case (1) { Return (Package() {\_SB.PC00.DSC2, \_SB.PC00.I2C1}) }
          case (2) { Return (Package() {\_SB.PC00.DSC2, \_SB.PC00.I2C2}) }
          case (3) { Return (Package() {\_SB.PC00.DSC2, \_SB.PC00.I2C3}) }
          case (4) { Return (Package() {\_SB.PC00.DSC2, \_SB.PC00.I2C4}) }
          case (5) { Return (Package() {\_SB.PC00.DSC2, \_SB.PC00.I2C5}) }
          default { Return (Package() {\_SB.PC00.DSC2}) }
        }
      }
      If (LGreater(C2TP, 1)) { Return (Package() {\_SB.PC00.CLP2}) } // when Control Logic Type is of type PMIC add dependency on CLP2
    }
    If (LEqual (Arg0, 3)) { // If Control Logic Device3 is to be used
      If (LEqual (C3TP, 1)) { // If Type of control logic is Discrete
        Switch (ToInteger(L3BS)) { // I2C bus
          case (0) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C0}) }
          case (1) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C1}) }
          case (2) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C2}) }
          case (3) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C3}) }
          case (4) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C4}) }
          case (5) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C5}) }
          default { Return (Package() {\_SB.PC00.DSC3}) }
        }
      }
      If (LGreater(C3TP, 1)) { Return (Package() {\_SB.PC00.CLP3}) } // when Control Logic Type is of type PMIC add dependency on CLP2
    }
    If (LEqual (Arg0, 4)) { // If Control Logic Device4 is to be used
      If (LEqual (C4TP, 1)) { // If Type of control logic is Discrete
        Switch (ToInteger(L4BS)) { // I2C bus
          case (0) { Return (Package() {\_SB.PC00.DSC4, \_SB.PC00.I2C0}) }
          case (1) { Return (Package() {\_SB.PC00.DSC4, \_SB.PC00.I2C1}) }
          case (2) { Return (Package() {\_SB.PC00.DSC4, \_SB.PC00.I2C2}) }
          case (3) { Return (Package() {\_SB.PC00.DSC3, \_SB.PC00.I2C3}) }
          case (4) { Return (Package() {\_SB.PC00.DSC4, \_SB.PC00.I2C4}) }
          case (5) { Return (Package() {\_SB.PC00.DSC4, \_SB.PC00.I2C5}) }
          default { Return (Package() {\_SB.PC00.DSC4}) }
        }
      }
      If (LGreater(C4TP, 1)) { Return (Package() {\_SB.PC00.CLP4}) } // when Control Logic Type is of type PMIC add dependency on CLP2
    }
    If (LEqual (Arg0, 5)) { // If Control Logic Device5 is to be used
      If (LEqual (C5TP, 1)) { // If Type of control logic is Discrete
        Switch (ToInteger(L5BS)) { // I2C bus
          case (0) { Return (Package() {\_SB.PC00.DSC5, \_SB.PC00.I2C0}) }
          case (1) { Return (Package() {\_SB.PC00.DSC5, \_SB.PC00.I2C1}) }
          case (2) { Return (Package() {\_SB.PC00.DSC5, \_SB.PC00.I2C2}) }
          case (3) { Return (Package() {\_SB.PC00.DSC5, \_SB.PC00.I2C3}) }
          case (4) { Return (Package() {\_SB.PC00.DSC5, \_SB.PC00.I2C4}) }
          case (5) { Return (Package() {\_SB.PC00.DSC5, \_SB.PC00.I2C5}) }
          default { Return (Package() {\_SB.PC00.DSC5}) }
        }
      }
      If (LGreater(C5TP, 1)) { Return (Package() {\_SB.PC00.CLP5}) } // when Control Logic Type is of type PMIC add dependency on CLP2
    }
    Return (Package() {\_SB.PC00})
  }

  Method (CDEG, 1, Serialized) {
    Switch (ToInteger(Arg0)) {
      case (0) { Return (0) }
      case (1) { Return (45) }
      case (2) { Return (90) }
      case (3) { Return (135) }
      case (4) { Return (180) }
      case (5) { Return (225) }
      case (6) { Return (270) }
      case (7) { Return (315) }
      default { Return (0) }
    }
  }

  Device (DSC0) { // Control Logic Device 0 when Discrete is selected
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG")
    Name(_UID, 0)
    Method(_CRS, 0x0, NotSerialized) {
      // Create GPIO resource template using as parameters data provided in the BIOS setup
      If (LGreater (C0GP, 0)) {
        Store (PINR(C0P0, C0G0), Local0)
      }
      If (LGreater (C0GP, 1)) {
        Store (PINR(C0P1, C0G1), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C0GP, 2)) {
        Store (PINR(C0P2, C0G2), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C0GP, 3)) {
        Store (PINR(C0P3, C0G3), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(_STA, 0, NotSerialized) {   // _STA: Status
      If (CL00) {
        If (LEqual(C0TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x00,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // PCH Clock source    (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C0VE, Index(PAR, 0))  // CLDB Version
      Store (C0TP, Index(PAR, 1))  // Type
      Store (C0CV, Index(PAR, 3))  // CRD version
      Store (C0IC, Index(PAR, 4))  // InputClock
      Store (C0SP, Index(PAR, 6))  // SubPlatformId
      Store (C0W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C0W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C0W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C0W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C0W4, Index(PAR, 12)) // WLED1 Type
      Store (C0W5, Index(PAR, 13)) // WLED2 Type
      Store (C0CS, Index(PAR, 14)) // PCH Clock source
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) { /*DsmGPIO*/
        // Function 0, Functions 1-5 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0x3F}) }
        // Function 1 (GPIO count)
        If (LEqual (Arg2, 1)) { Return (C0GP) }
        // Function 2 ( GPIO 0:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 2)) { Return (GPPI(C0F0, Add(Multiply(32, C0G0), C0P0), C0I0, C0A0)) }
        // Function 3 ( GPIO 1:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 3)) { Return (GPPI(C0F1, Add(Multiply(32, C0G1), C0P1), C0I1, C0A1)) }
        // Function 4 ( GPIO 2:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 4)) { Return (GPPI(C0F2, Add(Multiply(32, C0G2), C0P2), C0I2, C0A2)) }
        // Function 5 ( GPIO 3:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 5)) { Return (GPPI(C0F3, Add(Multiply(32, C0G3), C0P3), C0I3, C0A3)) }
        Return (Buffer() {0x00})
      }
      Return (Buffer() {0})
    }
  }

  Device (DSC1) { // Control Logic Device 1 when Discrete is selected
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG")
    Name(_UID, 1)
    Method(_CRS, 0x0, NotSerialized) {
      // Create GPIO resource template using as parameters data provided in the BIOS setup
      If (LGreater (C1GP, 0)) {
        Store (PINR(C1P0, C1G0), Local0)
      }
      If (LGreater (C1GP, 1)) {
        Store (PINR(C1P1, C1G1), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C1GP, 2)) {
        Store (PINR(C1P2, C1G2), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C1GP, 3)) {
        Store (PINR(C1P3, C1G3), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(_STA, 0, NotSerialized) {   // _STA: Status
      If (CL01) {
        If (LEqual(C1TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
     Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x01,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // PCH Clock source    (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C1VE, Index(PAR, 0))  // CLDB Version
      Store (C1TP, Index(PAR, 1))  // Type
      Store (C1CV, Index(PAR, 3))  // CRD version
      Store (C1IC, Index(PAR, 4))  // InputClock
      Store (C1SP, Index(PAR, 6))  // SubPlatformId
      Store (C1W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C1W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C1W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C1W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C1W4, Index(PAR, 12)) // WLED1 Type
      Store (C1W5, Index(PAR, 13)) // WLED2 Type
      Store (C1CS, Index(PAR, 14)) // PCH Clock source
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) { /*DsmGPIO*/
        // Function 0, Functions 1-5 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0x3F}) }
        // Function 1 (GPIO count)
        If (LEqual (Arg2, 1)) { Return (C1GP) }
        // Function 2 ( GPIO 0:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 2)) { Return (GPPI(C1F0, Add(Multiply(32, C1G0), C1P0), C1I0, C1A0)) }
        // Function 3 ( GPIO 1:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 3)) { Return (GPPI(C1F1, Add(Multiply(32, C1G1), C1P1), C1I1, C1A1)) }
        // Function 4 ( GPIO 2:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 4)) { Return (GPPI(C1F2, Add(Multiply(32, C1G2), C1P2), C1I2, C1A2)) }
        // Function 5 ( GPIO 3:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 5)) { Return (GPPI(C1F3, Add(Multiply(32, C1G3), C1P3), C1I3, C1A3)) }
      }
      Return (Buffer() {0})
    }
  }

  Device (DSC2) { // Control Logic Device 2 when Discrete is selected
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG")
    Name(_UID, 2)
    Method(_CRS, 0x0, NotSerialized) {
      // Create GPIO resource template using as parameters data provided in the BIOS setup
      If (LGreater (C2GP, 0)) {
        Store (PINR(C2P0, C2G0), Local0)
      }
      If (LGreater (C2GP, 1)) {
        Store (PINR(C2P1, C2G1), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C2GP, 2)) {
        Store (PINR(C2P2, C2G2), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C2GP, 3)) {
        Store (PINR(C2P3, C2G3), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(_STA, 0, NotSerialized) {   // _STA: Status
      If (CL02) {
        If (LEqual(C2TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x02,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // PCH Clock source    (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C2VE, Index(PAR, 0))  // CLDB Version
      Store (C2TP, Index(PAR, 1))  // Type
      Store (C2CV, Index(PAR, 3))  // CRD version
      Store (C2IC, Index(PAR, 4))  // InputClock
      Store (C2SP, Index(PAR, 6))  // SubPlatformId
      Store (C2W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C2W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C2W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C2W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C2W4, Index(PAR, 12)) // WLED1 Type
      Store (C2W5, Index(PAR, 13)) // WLED2 Type
      Store (C2CS, Index(PAR, 14)) // PCH Clock source
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) { /*DsmGPIO*/
        // Function 0 Functions 1-5 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0x3F}) }
        // Function 1 (GPIO count)
        If (LEqual (Arg2, 1)) { Return (C2GP) }
        // Function 2 ( GPIO 0:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 2)) { Return (GPPI(C2F0, Add(Multiply(32, C2G0), C2P0), C2I0, C2A0)) }
        // Function 3 ( GPIO 1:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 3)) { Return (GPPI(C2F1, Add(Multiply(32, C2G1), C2P1), C2I1, C2A1)) }
        // Function 4 ( GPIO 2:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 4)) { Return (GPPI(C2F2, Add(Multiply(32, C2G2), C2P2), C2I2, C2A2)) }
        // Function 5 ( GPIO 3:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 5)) { Return (GPPI(C2F3, Add(Multiply(32, C2G3), C2P3), C2I3, C2A3)) }
      }
      Return (Buffer() {0})
    }
  }

  Device (DSC3) { // Control Logic Device 3 when Discrete is selected
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG")
    Name(_UID, 3)
    Method(_CRS, 0x0, NotSerialized) {
      // Create GPIO resource template using as parameters data provided in the BIOS setup
      If (LGreater (C3GP, 0)) {
        Store (PINR(C3P0, C3G0), Local0)
      }
      If (LGreater (C3GP, 1)) {
        Store (PINR(C3P1, C3G1), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C3GP, 2)) {
        Store (PINR(C3P2, C3G2), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C3GP, 3)) {
        Store (PINR(C3P3, C3G3), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(_STA, 0, NotSerialized) {   // _STA: Status
      If (CL03) {
        If (LEqual(C3TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x03,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // PCH Clock source    (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C3VE, Index(PAR, 0))  // CLDB Version
      Store (C3TP, Index(PAR, 1))  // Type
      Store (C3CV, Index(PAR, 3))  // CRD version
      Store (C3IC, Index(PAR, 4))  // InputClock
      Store (C3SP, Index(PAR, 6))  // SubPlatformId
      Store (C3W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C3W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C3W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C3W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C3W4, Index(PAR, 12)) // WLED1 Type
      Store (C3W5, Index(PAR, 13)) // WLED2 Type
      Store (C3CS, Index(PAR, 14)) // PCH Clock source
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) { /*DsmGPIO*/
        // Function 0 Functions 1-5 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0x3F}) }
        // Function 1 (GPIO count)
        If (LEqual (Arg2, 1)) { Return (C3GP) }
        // Function 2 ( GPIO 0:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 2)) { Return (GPPI(C3F0, Add(Multiply(32, C3G0), C3P0), C3I0, C3A0)) }
        // Function 3 ( GPIO 1:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 3)) { Return (GPPI(C3F1, Add(Multiply(32, C3G1), C3P1), C3I1, C3A1)) }
        // Function 4 ( GPIO 2:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 4)) { Return (GPPI(C3F2, Add(Multiply(32, C3G2), C3P2), C3I2, C3A2)) }
        // Function 5 ( GPIO 3:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 5)) { Return (GPPI(C3F3, Add(Multiply(32, C3G3), C3P3), C3I3, C3A3)) }
      }
      Return (Buffer() {0})
    }
  }

  Device (DSC4) { // Control Logic Device 4 when Discrete is selected
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG")
    Name(_UID, 4)
    Method(_CRS, 0x0, NotSerialized) {
      // Create GPIO resource template using as parameters data provided in the BIOS setup
      If (LGreater (C4GP, 0)) {
        Store (PINR(C4P0, C4G0), Local0)
      }
      If (LGreater (C4GP, 1)) {
        Store (PINR(C4P1, C4G1), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C4GP, 2)) {
        Store (PINR(C4P2, C4G2), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C4GP, 3)) {
        Store (PINR(C4P3, C4G3), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(_STA, 0, NotSerialized) {   // _STA: Status
      If (CL04) {
        If (LEqual(C4TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x04,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // PCH Clock source    (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C4VE, Index(PAR, 0))  // CLDB Version
      Store (C4TP, Index(PAR, 1))  // Type
      Store (C4CV, Index(PAR, 3))  // CRD version
      Store (C4IC, Index(PAR, 4))  // InputClock
      Store (C4SP, Index(PAR, 6))  // SubPlatformId
      Store (C4W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C4W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C4W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C4W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C4W4, Index(PAR, 12)) // WLED1 Type
      Store (C4W5, Index(PAR, 13)) // WLED2 Type
      Store (C4CS, Index(PAR, 14)) // PCH Clock source
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) { /*DsmGPIO*/
        // Function 0 Functions 1-5 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0x3F}) }
        // Function 1 (GPIO count)
        If (LEqual (Arg2, 1)) { Return (C4GP) }
        // Function 2 ( GPIO 0:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 2)) { Return (GPPI(C4F0, Add(Multiply(32, C4G0), C4P0), C4I0, C4A0)) }
        // Function 3 ( GPIO 1:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 3)) { Return (GPPI(C4F1, Add(Multiply(32, C4G1), C4P1), C4I1, C4A1)) }
        // Function 4 ( GPIO 2:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 4)) { Return (GPPI(C4F2, Add(Multiply(32, C4G2), C4P2), C4I2, C4A2)) }
        // Function 5 ( GPIO 3:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 5)) { Return (GPPI(C4F3, Add(Multiply(32, C4G3), C4P3), C4I3, C4A3)) }
      }
      Return (Buffer() {0})
    }
  }

  Device (DSC5) { // Control Logic Device 5 when Discrete is selected
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG")
    Name(_UID, 5)
    Method(_CRS, 0x0, NotSerialized) {
      // Create GPIO resource template using as parameters data provided in the BIOS setup
      If (LGreater (C5GP, 0)) {
        Store (PINR(C5P0, C5G0), Local0)
      }
      If (LGreater (C5GP, 1)) {
        Store (PINR(C5P1, C5G1), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C5GP, 2)) {
        Store (PINR(C5P2, C5G2), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      If (LGreater (C5GP, 3)) {
        Store (PINR(C5P3, C5G3), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(_STA, 0, NotSerialized) {   // _STA: Status
      If (CL05) {
        If (LEqual(C5TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x05,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // PCH Clock source    (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C5VE, Index(PAR, 0))  // CLDB Version
      Store (C5TP, Index(PAR, 1))  // Type
      Store (C5CV, Index(PAR, 3))  // CRD version
      Store (C5IC, Index(PAR, 4))  // InputClock
      Store (C5SP, Index(PAR, 6))  // SubPlatformId
      Store (C5W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C5W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C5W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C5W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C5W4, Index(PAR, 12)) // WLED1 Type
      Store (C5W5, Index(PAR, 13)) // WLED2 Type
      Store (C5CS, Index(PAR, 14)) // PCH Clock source
      Return (PAR)
    }

    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("79234640-9E10-4FEA-A5C1-B5AA8B19756F"))) { /*DsmGPIO*/
        // Function 0 Functions 1-5 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0x3F}) }
        // Function 1 (GPIO count)
        If (LEqual (Arg2, 1)) { Return (C5GP) }
        // Function 2 ( GPIO 0:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 2)) { Return (GPPI(C5F0, Add(Multiply(32, C5G0), C5P0), C5I0, C5A0)) }
        // Function 3 ( GPIO 1:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 3)) { Return (GPPI(C5F1, Add(Multiply(32, C5G1), C5P1), C5I1, C5A1)) }
        // Function 4 ( GPIO 2:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 4)) { Return (GPPI(C5F2, Add(Multiply(32, C5G2), C5P2), C5I2, C5A2)) }
        // Function 5 ( GPIO 3:bit31-24:Active Value. 23-16:Initial Value. 15-8:PinNo. 0-7:Function )
        If (LEqual (Arg2, 5)) { Return (GPPI(C5F3, Add(Multiply(32, C5G3), C5P3), C5I3, C5A3)) }
      }
      Return (Buffer() {0})
    }
  }

  Device (CLP0) { // Control Logic Device 0 when PMIC is selected
    Name (_ADR, Zero)
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (CL00) { // If CLP0 device is to be enabled
        If (LEqual (C0IB, 2)) { // If PMIC needs I2C2 controller
          Return (Package() {\_SB.PC00.I2C2})
        }
        If (LEqual (C0IB, 3)) { // If PMIC needs I2C3 controller
          Return (Package() {\_SB.PC00.I2C3})
        }
      }
      Return (Package() {\_SB.PC00})
    }
    Name (_UID, 0)
    Name (_CID, "INT3472")
    Name (_DDN, "PMIC-CRDG")
    Method (_CRS, 0x0, NotSerialized) {
      //Store (Zero, Local0)
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      //ConcatenateResTemplate(Local0, IICB(C0IA, C0IB), Local2)
      //Store(Local2, Local0)
      Return (IICB(C0IA, C0IB))
    }
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (CL00) {
        If (LGreater(C0TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }
    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x00,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // Reserved            (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C0VE, Index(PAR, 0))  // CLDB Version
      Store (C0TP, Index(PAR, 1))  // Type
      Store (C0CV, Index(PAR, 3))  // CRD version
      Store (C0IC, Index(PAR, 4))  // InputClock
      Store (C0SP, Index(PAR, 6))  // SubPlatformId
      Store (C0W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C0W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C0W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C0W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C0W4, Index(PAR, 12)) // WLED1 Type
      Store (C0W5, Index(PAR, 13)) // WLED2 Type
      Return (PAR)
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x29, 0x0D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      Store (C0PL, BPOS)
      Return (PLDB)
    }
    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If (LEqual (Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x07})  // Functions 1-2 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (0x01) }
        // Function 2 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) { Return (DSMI(0x0B, C0IA, 0, C0IB)) }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { //DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          Store (0x01, I2CC)
          Store (DSMI(0x0B, C0IA, 0, C0IB), DEV0)
          Return (DSMB)
        }
      }
      Return (Buffer() {0})
    }
  } // Device (CLP0)


  Device (CLP1) { // Control Logic Device 1 when PMIC is selected
    Name (_ADR, Zero)
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (CL01) { // If CLP1 device is to be enabled
        If (LEqual (C1IB, 2)) { // If PMIC needs I2C2 controller
          Return (Package() {\_SB.PC00.I2C2})
        }
        If (LEqual (C1IB, 3)) { // If PMIC needs I2C3 controller
          Return (Package() {\_SB.PC00.I2C3})
        }
      }
      Return (Package() {\_SB.PC00})
    }
    Name (_CID, "INT3472")
    Name (_DDN, "PMIC-CRDG")
    Name (_UID, 1)
    Method (_CRS, 0x0, NotSerialized) {
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      Return (IICB(C1IA, C1IB))
    }
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (CL01) {
        If (LGreater(C1TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }

    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x01,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // Reserved            (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C1VE, Index(PAR, 0))  // CLDB Version
      Store (C1TP, Index(PAR, 1))  // Type
      Store (C1CV, Index(PAR, 3))  // CRD version
      Store (C1IC, Index(PAR, 4))  // InputClock
      Store (C1SP, Index(PAR, 6))  // SubPlatformId
      Store (C1W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C1W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C1W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C1W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C1W4, Index(PAR, 12)) // WLED1 Type
      Store (C1W5, Index(PAR, 13)) // WLED2 Type
      Return (PAR)
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x29, 0x0D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      Store (C1PL, BPOS)
      Return (PLDB)
    }
    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If (LEqual (Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x07})  // Functions 1-2 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (0x01) }
        // Function 2 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) { Return (DSMI(0x0B, C1IA, 0, C1IB)) }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          Store (0x01, I2CC)
          Store (DSMI(0x0B, C1IA, 0, C1IB), DEV0)
          Return (DSMB)
        }
      }
      Return (Buffer() {0})
    }
  } // Device (CLP1)

  Device (CLP2) { // Control Logic Device 2 when PMIC is selected
    Name (_ADR, Zero)
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (CL02) { // If CLP2 device is to be enabled
        If (LEqual (C2IB, 2)) { // If PMIC needs I2C2 controller
          Return (Package() {\_SB.PC00.I2C2})
        }
        If (LEqual (C2IB, 3)) { // If PMIC needs I2C3 controller
          Return (Package() {\_SB.PC00.I2C3})
        }
      }
      Return (Package() {\_SB.PC00})
    }
    Name (_CID, "INT3472")
    Name (_DDN, "PMIC-CRDG")
    Name (_UID, 2)
    Method (_CRS, 0x0, NotSerialized) {
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      Return (IICB(C2IA, C2IB))
    }
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (CL02) {
        If (LGreater(C2TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }

    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x02,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // Reserved            (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C2VE, Index(PAR, 0))  // CLDB Version
      Store (C2TP, Index(PAR, 1))  // Type
      Store (C2CV, Index(PAR, 3))  // CRD version
      Store (C2IC, Index(PAR, 4))  // InputClock
      Store (C2SP, Index(PAR, 6))  // SubPlatformId
      Store (C2W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C2W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C2W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C2W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C2W4, Index(PAR, 12)) // WLED1 Type
      Store (C2W5, Index(PAR, 13)) // WLED2 Type
      Return (PAR)
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x29, 0x0D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      Store (C2PL, BPOS)
      Return (PLDB)
    }
    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If (LEqual (Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x07})  // Functions 1-2 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (0x01) }
        // Function 2 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) { Return (DSMI(0x0B, C2IA, 0, C2IB)) }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          Store (0x01, I2CC)
          Store (DSMI(0x0B, C2IA, 0, C2IB), DEV0)
          Return (DSMB)
        }
      }
      Return (Buffer() {0})
    }
  } // Device (CLP2)

  Device (CLP3) { // Control Logic Device 3 when PMIC is selected
    Name (_ADR, Zero)
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (CL03) { // If CLP1 device is to be enabled
        If (LEqual (C3IB, 2)) { // If PMIC needs I2C2 controller
          Return (Package() {\_SB.PC00.I2C2})
        }
        If (LEqual (C3IB, 3)) { // If PMIC needs I2C3 controller
          Return (Package() {\_SB.PC00.I2C3})
        }
      }
      Return (Package() {\_SB.PC00})
    }
    Name (_CID, "INT3472")
    Name (_DDN, "PMIC-CRDG")
    Name (_UID, 3)
    Method (_CRS, 0x0, NotSerialized) {
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      Return (IICB(C2IA, C2IB))
    }
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (CL03) {
        If (LGreater(C3TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }

    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x03,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // Reserved            (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C3VE, Index(PAR, 0))  // CLDB Version
      Store (C3TP, Index(PAR, 1))  // Type
      Store (C3CV, Index(PAR, 3))  // CRD version
      Store (C3IC, Index(PAR, 4))  // InputClock
      Store (C3SP, Index(PAR, 6))  // SubPlatformId
      Store (C3W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C3W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C3W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C3W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C3W4, Index(PAR, 12)) // WLED1 Type
      Store (C3W5, Index(PAR, 13)) // WLED2 Type
      Return (PAR)
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x29, 0x0D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      Store (C3PL, BPOS)
      Return (PLDB)
    }
    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If (LEqual (Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x07})  // Functions 1-2 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (0x01) }
        // Function 2 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) { Return (DSMI(0x0B, C3IA, 0, C3IB)) }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          Store (0x01, I2CC)
          Store (DSMI(0x0B, C3IA, 0, C3IB), DEV0)
          Return (DSMB)
        }
      }
      Return (Buffer() {0})
    }
  } // Device (CLP3)

  Device (CLP4) { // Control Logic Device 4 when PMIC is selected
    Name (_ADR, Zero)
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (CL04) { // If CLP1 device is to be enabled
        If (LEqual (C4IB, 2)) { // If PMIC needs I2C2 controller
          Return (Package() {\_SB.PC00.I2C2})
        }
        If (LEqual (C4IB, 3)) { // If PMIC needs I2C3 controller
          Return (Package() {\_SB.PC00.I2C3})
        }
      }
      Return (Package() {\_SB.PC00})
    }
    Name (_CID, "INT3472")
    Name (_DDN, "PMIC-CRDG")
    Name (_UID, 4)
    Method (_CRS, 0x0, NotSerialized) {
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      Return (IICB(C2IA, C2IB))
    }
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (CL04) {
        If (LGreater(C4TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }

    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x04,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // Reserved            (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C4VE, Index(PAR, 0))  // CLDB Version
      Store (C4TP, Index(PAR, 1))  // Type
      Store (C4CV, Index(PAR, 3))  // CRD version
      Store (C4IC, Index(PAR, 4))  // InputClock
      Store (C4SP, Index(PAR, 6))  // SubPlatformId
      Store (C4W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C4W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C4W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C4W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C4W4, Index(PAR, 12)) // WLED1 Type
      Store (C4W5, Index(PAR, 13)) // WLED2 Type
      Return (PAR)
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x29, 0x0D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      Store (C4PL, BPOS)
      Return (PLDB)
    }
    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If (LEqual (Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x07})  // Functions 1-2 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (0x01) }
        // Function 2 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) { Return (DSMI(0x0B, C4IA, 0, C4IB)) }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          Store (0x01, I2CC)
          Store (DSMI(0x0B, C4IA, 0, C4IB), DEV0)
          Return (DSMB)
        }
      }
      Return (Buffer() {0})
    }
  } // Device (CLP4)

  Device (CLP5) { // Control Logic Device 5 when PMIC is selected
    Name (_ADR, Zero)
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (CL05) { // If CLP1 device is to be enabled
        If (LEqual (C5IB, 2)) { // If PMIC needs I2C2 controller
          Return (Package() {\_SB.PC00.I2C2})
        }
        If (LEqual (C5IB, 3)) { // If PMIC needs I2C3 controller
          Return (Package() {\_SB.PC00.I2C3})
        }
      }
      Return (Package() {\_SB.PC00})
    }
    Name (_CID, "INT3472")
    Name (_DDN, "PMIC-CRDG")
    Name (_UID, 5)
    Method (_CRS, 0x0, NotSerialized) {
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      Return (IICB(C2IA, C2IB))
    }
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (CL05) {
        If (LGreater(C5TP, 1)) { Return (0x0F) }
      }
      Return (0)
    }

    Method (CLDB, 0, Serialized) {
      Name (PAR, Buffer() {
        0x00,                 // CLDB Version        (Offset 0)
        0x00,                 // Control Logic Type  (Offset 1)
        0x05,                 // Control Logic Id    (Offset 2)
        0x00,                 // CRD version         (Offset 3)
        0x00,                 // InputClock          (Offset 4)
        0x0C,                 // Platform Id         (Offset 5)
        0x00,                 // SubPlatformId       (Offset 6)
        0x00,                 // Reserved            (Offset 7)
        0x00,                 // WLED1 Flash Max Current (Offset 8)
        0x00,                 // WLED1 Torch Max Current (Offset 9)
        0x00,                 // WLED2 Flash Max Current (Offset 10)
        0x00,                 // WLED2 Torch Max Current (Offset 11)
        0x00,                 // WLED1 Type          (Offset 12)
        0x00,                 // WLED2 Type          (Offset 13)
        0x00,                 // Reserved            (Offset 14)
        0x00,                 // Reserved            (Offset 15)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // Reserved  (Offset 16)
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   // Reserved  (Offset 24)
      })
      Store (C5VE, Index(PAR, 0))  // CLDB Version
      Store (C5TP, Index(PAR, 1))  // Type
      Store (C5CV, Index(PAR, 3))  // CRD version
      Store (C5IC, Index(PAR, 4))  // InputClock
      Store (C5SP, Index(PAR, 6))  // SubPlatformId
      Store (C5W0, Index(PAR, 8))  // WLED1 Flash Max Current
      Store (C5W1, Index(PAR, 9))  // WLED1 Torch Max Current
      Store (C5W2, Index(PAR, 10)) // WLED2 Flash Max Current
      Store (C5W3, Index(PAR, 11)) // WLED2 Torch Max Current
      Store (C5W4, Index(PAR, 12)) // WLED1 Type
      Store (C5W5, Index(PAR, 13)) // WLED2 Type
      Return (PAR)
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x29, 0x0D, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      Store (C5PL, BPOS)
      Return (PLDB)
    }
    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If (LEqual (Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x07})  // Functions 1-2 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (0x01) }
        // Function 2 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) { Return (DSMI(0x0B, C5IA, 0, C5IB)) }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          Store (0x01, I2CC)
          Store (DSMI(0x0B, C5IA, 0, C5IB), DEV0)
          Return (DSMB)
        }
      }
      Return (Buffer() {0})
    }
  } // Device (CLP5)

  Device (LNK0) {       // definition for Camera sensor module using Link0
    Name (_ADR, Zero)  // _ADR: Address
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (L0EN) { Return (0x0F) }
      Else { Return (0) }
    }
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (L0EN) { Return (CDEP(L0CL)) } // If LNK0 device is to be enabled
      Else { Return (Package() {\_SB.PC00}) }
    }
    Name (_UID, 0)  // _UID: Unique ID - Link Index

    Method (_CID, 0, NotSerialized) { // _CID: Compatible ID
      Return (HCID(0))
    }
    Method (_DDN, 0, Serialized) { // _DDN: DOS Device Name
      Name(BUF, Buffer(16) {})
      Store(L0M0, Index(BUF,0))
      Store(L0M1, Index(BUF,1))
      Store(L0M2, Index(BUF,2))
      Store(L0M3, Index(BUF,3))
      Store(L0M4, Index(BUF,4))
      Store(L0M5, Index(BUF,5))
      Store(L0M6, Index(BUF,6))
      Store(L0M7, Index(BUF,7))
      Store(L0M8, Index(BUF,8))
      Store(L0M9, Index(BUF,9))
      Store(L0MA, Index(BUF,10))
      Store(L0MB, Index(BUF,11))
      Store(L0MC, Index(BUF,12))
      Store(L0MD, Index(BUF,13))
      Store(L0ME, Index(BUF,14))
      Store(L0MF, Index(BUF,15))
      Return (ToString(BUF))
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x69, 0x0E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(L0PL, BPOS)
      Store(L0DG, RPOS)
      Return (PLDB)
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Store(Zero, Local0)  // empty resource buffer
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      If (LGreater (L0DI, 0)) {
        Store (IICB(L0A0, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 2 or more I2C devices were enabled
      If (LGreater(L0DI, 1)) {
        Store (IICB(L0A1, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 3 or more I2C devices were enabled
      If (LGreater(L0DI, 2)) {
        Store (IICB(L0A2, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 4 or more I2C devices were enabled
      If (LGreater(L0DI, 3)) {
        Store (IICB(L0A3, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 5 or more I2C devices were enabled
      If (LGreater(L0DI, 4)) {
        Store (IICB(L0A4, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 6 or more I2C devices were enabled
      If (LGreater(L0DI, 5)) {
        Store (IICB(L0A5, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 7 or more I2C devices were enabled
      If (LGreater(L0DI, 6)) {
        Store (IICB(L0A6, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 8 or more I2C devices were enabled
      If (LGreater(L0DI, 7)) {
        Store (IICB(L0A7, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 9 or more I2C devices were enabled
      If (LGreater(L0DI, 8)) {
        Store (IICB(L0A8, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 10 or more I2C devices were enabled
      If (LGreater(L0DI, 9)) {
        Store (IICB(L0A9, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 11 or more I2C devices were enabled
      If (LGreater(L0DI, 10)) {
        Store (IICB(L0AA, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 12 or more I2C devices were enabled
      If (LGreater(L0DI, 11)) {
        Store (IICB(L0AB, L0BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer() {
        0x00,                 // SSDB Version. Current version is 0x00
        0x00,                 // CRD version
          // CSI2DataStreamInterfaceGUID in CSI2_HOST_CONTROLLER,(offset 2)
          // 8A395669-11F7-4ea9-9C7D-20EE0AB5CA40,
        0x69,0x56,0x39,0x8A,  // 0x8a395669
        0xF7,0x11,            // 0x11F7
        0xA9,0x4E,            // 0x4EA9
        0x9C,0x7D,0x20,0xEE,  // 9C7D-20EE0AB5CA40
        0x0A,0xB5,0xCA,0x40,
          // - When the CIO2 device is configured in PCI mode then its B-D-F is 0-20-3
          //  (DevFunc = 0xA3, Bus = 0x00).
          // B-D-F of the CSI2 host controller to which the sensor connects.
        0xA3,                 // DevFunc DEV[5]:FUN[3]        (offset 18)
        0x00,                 // Bus BUS[8]                   (offset 19)
        0x00,0x00,0x00,0x00,  // G_DphyLinkEnFuses,           (offset 20)
        0x00,0x00,0x00,0x00,  // G_LanesClockDivision,        (offset 24)
        0x00,                 // LinkUsed,                    (offset 28)
        0x00,                 // LanesUsed,                   (offset 29)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_CLANE  (offset 30)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_CLANE  (offset 34)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE0 (offset 38)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE0 (offset 42)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE1 (offset 46)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE1 (offset 50)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE2 (offset 54)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE2 (offset 58)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE3 (offset 62)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE  (offset 66)
        0x00,0x00,0x00,0x00,  // MaxLaneSpeed[4]              (offset 70)
        0x00,                 // SensorCalibrationFileIndex   (offset 74)
        0x00,0x00,0x00,       // SensorCalibrationFileIndexMBZ  (offset 75)
        0x00,                 // Reserved: EEPROM Type (offset 78)
        0x00,                 // Reserved: VCM Type    (offset 79)
        0x0C,                 // Platform: SKC         (offset 80)
        0x00,                 // PlatformSub           (offset 81)
        0x00,                 // FlashSupport          (offset 82)
        0x00,                 // Privacy LED                  (offset 83)
        0x00,                 // Degree                (offset 84)
        0x01,                 // mipiDefine            (offset 85)
        0x00,0x00,0x00,0x00,  // MCLK                  (offset 86)
        0x00,                 // Control Logic         (offset 90)
        0x00,0x00,0x00, 0x00, // Reserved              (offset 91)
        0x00,                 // PMIC Position         (offset 95)
        0x00,                 // Voltage Rail          (offset 96)
        0x00,                 // PPR Value             (offset 97)
        0x00,                 // PPR Unit              (offset 98)
        0x00,0x00,            // Reserved              (offset 99)
        0x00,0x00,0x00,0x00,0x00, // Reserved          (offset 101)
        0x00,0x00             // Reserved              (offset 106)
      })
      Store(L0DV, Index(PAR, 0))  // SSDB Version
      Store(L0CV, Index(PAR, 1))  // CRD Version
      Store(CDIV, Index(PAR, 24)) // LanesClk Division
      Store(L0LU, Index(PAR, 28)) // LinkUsed
      Store(L0NL, Index(PAR, 29)) // LaneUsed
      Store(L0EE, Index(PAR, 78)) // EEPROM Type
      Store(L0VC, Index(PAR, 79)) // VCM Type
      Store(L0FS, Index(PAR, 82)) // Flash Support
      Store(L0LE, Index(PAR, 83)) // Privacy LED
      Store(CDEG(L0DG), Index(PAR, 84)) // Degree
      CreateDWordField(PAR, 86, DAT) // MCLK
      Store(L0CK, DAT)
      Store(L0CL, Index(PAR, 90)) // Control Logic
      Store(L0PP, Index(PAR, 95)) // PMIC Position
      Store(L0VR, Index(PAR, 96)) // Voltage Rail
      Store(L0PV, Index(PAR, 97)) // PL4/PMAX PPR Value
      Store(L0PU, Index(PAR, 98)) // PL4/PMAX PPR Unit
      Return(PAR)
    }

    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
        // Function 0
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1
        If (LEqual (Arg2, 1)) {
          Name(BUF, Buffer(16) {})
          Store(L0M0, Index(BUF,0))
          Store(L0M1, Index(BUF,1))
          Store(L0M2, Index(BUF,2))
          Store(L0M3, Index(BUF,3))
          Store(L0M4, Index(BUF,4))
          Store(L0M5, Index(BUF,5))
          Store(L0M6, Index(BUF,6))
          Store(L0M7, Index(BUF,7))
          Store(L0M8, Index(BUF,8))
          Store(L0M9, Index(BUF,9))
          Store(L0MA, Index(BUF,10))
          Store(L0MB, Index(BUF,11))
          Store(L0MC, Index(BUF,12))
          Store(L0MD, Index(BUF,13))
          Store(L0ME, Index(BUF,14))
          Store(L0MF, Index(BUF,15))
          Return (ToString(BUF))
        }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c - new implementation
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          CreateDWordField(DSMB, 8, DEV1)
          CreateDWordField(DSMB, 12, DEV2)
          CreateDWordField(DSMB, 16, DEV3)
          CreateDWordField(DSMB, 20, DEV4)
          CreateDWordField(DSMB, 24, DEV5)
          CreateDWordField(DSMB, 28, DEV6)
          CreateDWordField(DSMB, 32, DEV7)
          CreateDWordField(DSMB, 36, DEV8)
          CreateDWordField(DSMB, 40, DEV9)
          CreateDWordField(DSMB, 44, DEVA)
          CreateDWordField(DSMB, 48, DEVB)
          Store (L0DI, I2CC)
          Store (DSMI(L0D0, L0A0, 0, L0BS), DEV0)
          Store (DSMI(L0D1, L0A1, 0, L0BS), DEV1)
          Store (DSMI(L0D2, L0A2, 0, L0BS), DEV2)
          Store (DSMI(L0D3, L0A3, 0, L0BS), DEV3)
          Store (DSMI(L0D4, L0A4, 0, L0BS), DEV4)
          Store (DSMI(L0D5, L0A5, 0, L0BS), DEV5)
          Store (DSMI(L0D6, L0A6, 0, L0BS), DEV6)
          Store (DSMI(L0D7, L0A7, 0, L0BS), DEV7)
          Store (DSMI(L0D8, L0A8, 0, L0BS), DEV8)
          Store (DSMI(L0D9, L0A9, 0, L0BS), DEV9)
          Store (DSMI(L0DA, L0AA, 0, L0BS), DEVA)
          Store (DSMI(L0DB, L0AB, 0, L0BS), DEVB)
          Return (DSMB)
        }
        Return (Buffer() {0})
      }
      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        // Function 0 Functions 1-13 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0xFF, 0x1F}) }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (L0DI) }
        // Function 2 (I2C device 0 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) {
          If (LGreater(L0DI, 0)) { Return (DSMI(L0D0, L0A0, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 3 (I2C device 1 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 3)) {
          If (LGreater(L0DI, 1)) { Return (DSMI(L0D1, L0A1, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 4 (I2C device 2 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 4)) {
          If (LGreater(L0DI, 2)) { Return (DSMI(L0D2, L0A2, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 5 (I2C device 3 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 5)) {
          If (LGreater(L0DI, 3)) { Return (DSMI(L0D3, L0A3, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 6 (I2C device 4 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 6)) {
          If (LGreater(L0DI, 4)) { Return (DSMI(L0D4, L0A4, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 7 (I2C device 5 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 7)) {
          If (LGreater(L0DI, 5)) { Return (DSMI(L0D5, L0A5, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 8 (I2C device 6 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L0DI, 6)) { Return (DSMI(L0D6, L0A6, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 9 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L0DI, 7)) { Return (DSMI(L0D7, L0A7, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 10 (I2C device 8 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 9)) {
          If (LGreater(L0DI, 8)) { Return (DSMI(L0D8, L0A8, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 11 (I2C device 9 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 10)) {
          If (LGreater(L0DI, 9)) { Return (DSMI(L0D9, L0A9, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 12 (I2C device 10 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 11)) {
          If (LGreater(L0DI, 10)) { Return (DSMI(L0DA, L0AA, 0, L0BS)) }
          Else { Return (0) }
        }
        // Function 13 (I2C device 11 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 12)) {
          If (LGreater(L0DI, 11)) { Return (DSMI(L0DB, L0AB, 0, L0BS)) }
          Else { Return (0) }
        }
      }
      Return (Buffer() {0})
    }
  } // Device (LNK0)

  Device (LNK1) {       // definition for Camera sensor module using Link1
    Name (_ADR, Zero)  // _ADR: Address (of the channel on the PCI bus
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (L1EN) { Return (0x0F) }
      Else { Return (0) }
    }
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (L1EN) { Return (CDEP(L1CL)) } // If LNK1 device is to be enabled
      Else { Return (Package() {\_SB.PC00}) }
    }
    Name (_UID, 1)  // _UID: Unique ID - Link Index

    Method (_CID, 0, NotSerialized) { // _CID: Compatible ID
      Return (HCID(1))
    }
    Method (_DDN, 0, Serialized) { // _DDN: DOS Device Name
      Name(BUF, Buffer(16) {})
      Store(L1M0, Index(BUF,0))
      Store(L1M1, Index(BUF,1))
      Store(L1M2, Index(BUF,2))
      Store(L1M3, Index(BUF,3))
      Store(L1M4, Index(BUF,4))
      Store(L1M5, Index(BUF,5))
      Store(L1M6, Index(BUF,6))
      Store(L1M7, Index(BUF,7))
      Store(L1M8, Index(BUF,8))
      Store(L1M9, Index(BUF,9))
      Store(L1MA, Index(BUF,10))
      Store(L1MB, Index(BUF,11))
      Store(L1MC, Index(BUF,12))
      Store(L1MD, Index(BUF,13))
      Store(L1ME, Index(BUF,14))
      Store(L1MF, Index(BUF,15))
      Return (ToString(BUF))
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x69, 0x0E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(L1PL, BPOS)
      Store(L1DG, RPOS)
      Return (PLDB)
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Store(Zero, Local0)  // empty resource buffer
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      If (LGreater (L1DI, 0)) {
        Store (IICB(L1A0, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 2 or more I2C devices were enabled
      If (LGreater(L1DI, 1)) {
        Store (IICB(L1A1, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 3 or more I2C devices were enabled
      If (LGreater(L1DI, 2)) {
        Store (IICB(L1A2, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 4 or more I2C devices were enabled
      If (LGreater(L1DI, 3)) {
        Store (IICB(L1A3, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 5 or more I2C devices were enabled
      If (LGreater(L1DI, 4)) {
        Store (IICB(L1A4, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 6 or more I2C devices were enabled
      If (LGreater(L1DI, 5)) {
        Store (IICB(L1A5, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 7 or more I2C devices were enabled
      If (LGreater(L1DI, 6)) {
        Store (IICB(L1A6, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 8 or more I2C devices were enabled
      If (LGreater(L1DI, 7)) {
        Store (IICB(L1A7, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 9 or more I2C devices were enabled
      If (LGreater(L1DI, 8)) {
        Store (IICB(L1A8, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 10 or more I2C devices were enabled
      If (LGreater(L1DI, 9)) {
        Store (IICB(L1A9, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 11 or more I2C devices were enabled
      If (LGreater(L1DI, 10)) {
        Store (IICB(L1AA, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 12 or more I2C devices were enabled
      If (LGreater(L1DI, 11)) {
        Store (IICB(L1AB, L1BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer() {
        0x00,                 // SSDB Version. Current version is 0x00
        0x00,                 // CRD version
          // CSI2DataStreamInterfaceGUID in CSI2_HOST_CONTROLLER,(offset 2)
          // 8A395669-11F7-4ea9-9C7D-20EE0AB5CA40,
        0x69,0x56,0x39,0x8A,  // 0x8a395669
        0xF7,0x11,            // 0x11F7
        0xA9,0x4E,            // 0x4EA9
        0x9C,0x7D,0x20,0xEE,  // 9C7D-20EE0AB5CA40
        0x0A,0xB5,0xCA,0x40,
          // - When the CIO2 device is configured in PCI mode then its B-D-F is 0-20-3
          //  (DevFunc = 0xA3, Bus = 0x00).
          // B-D-F of the CSI2 host controller to which the sensor connects.
        0xA3,                 // DevFunc DEV[5]:FUN[3]        (offset 18)
        0x00,                 // Bus BUS[8]                   (offset 19)
        0x00,0x00,0x00,0x00,  // G_DphyLinkEnFuses,           (offset 20)
        0x00,0x00,0x00,0x00,  // G_LanesClockDivision,        (offset 24)
        0x00,                 // LinkUsed,                    (offset 28)
        0x00,                 // LanesUsed,                   (offset 29)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_CLANE  (offset 30)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_CLANE  (offset 34)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE0 (offset 38)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE0 (offset 42)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE1 (offset 46)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE1 (offset 50)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE2 (offset 54)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE2 (offset 58)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE3 (offset 62)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE  (offset 66)
        0x00,0x00,0x00,0x00,  // MaxLaneSpeed[4]              (offset 70)
        0x00,                 // SensorCalibrationFileIndex   (offset 74)
        0x00,0x00,0x00,       // SensorCalibrationFileIndexMBZ  (offset 75)
        0x00,                 // Reserved: EEPROM Type (offset 78)
        0x00,                 // Reserved: VCM Type    (offset 79)
        0x0C,                 // Platform: SKC         (offset 80)
        0x00,                 // PlatformSub           (offset 81)
        0x00,                 // FlashSupport          (offset 82)
        0x00,                 // Privacy LED                  (offset 83)
        0x00,                 // Degree                (offset 84)
        0x01,                 // mipiDefine            (offset 85)
        0x00,0x00,0x00,0x00,  // MCLK                  (offset 86)
        0x00,                 // Control Logic         (offset 90)
        0x00,0x00,0x00, 0x00, // Reserved              (offset 91)
        0x00,                 // PMIC Position         (offset 95)
        0x00,                 // Voltage Rail          (offset 96)
        0x00,                 // PPR Value             (offset 97)
        0x00,                 // PPR Unit              (offset 98)
        0x00,0x00,            // Reserved              (offset 99)
        0x00,0x00,0x00,0x00,0x00, // Reserved          (offset 101)
        0x00,0x00             // Reserved              (offset 106)
      })
      Store(L1DV, Index(PAR, 0))  // SSDB Version
      Store(L1CV, Index(PAR, 1))  // CRD Version
      Store(CDIV, Index(PAR, 24)) // LanesClk Division
      Store(L1LU, Index(PAR, 28)) // LinkUsed
      Store(L1NL, Index(PAR, 29)) // LaneUsed
      Store(L1EE, Index(PAR, 78)) // EEPROM Type
      Store(L1VC, Index(PAR, 79)) // VCM Type
      Store(L1FS, Index(PAR, 82)) // Flash Support
      Store(L1LE, Index(PAR, 83)) // Privacy LED
      Store(CDEG(L1DG), Index(PAR, 84)) // Degree
      CreateDWordField(PAR, 86, DAT) // MCLK
      Store(L1CK, DAT)
      Store(L1CL, Index(PAR, 90)) // Control Logic
      Store(L1PP, Index(PAR, 95)) // PMIC Position
      Store(L1VR, Index(PAR, 96)) // Voltage Rail
      Store(L1PV, Index(PAR, 97)) // PL4/PMAX PPR Value
      Store(L1PU, Index(PAR, 98)) // PL4/PMAX PPR Unit
      Return(PAR)
    }

    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
         // Function 0
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1
        If (LEqual (Arg2, 1)) {
          Name(BUF, Buffer(16) {})
          Store(L1M0, Index(BUF,0))
          Store(L1M1, Index(BUF,1))
          Store(L1M2, Index(BUF,2))
          Store(L1M3, Index(BUF,3))
          Store(L1M4, Index(BUF,4))
          Store(L1M5, Index(BUF,5))
          Store(L1M6, Index(BUF,6))
          Store(L1M7, Index(BUF,7))
          Store(L1M8, Index(BUF,8))
          Store(L1M9, Index(BUF,9))
          Store(L1MA, Index(BUF,10))
          Store(L1MB, Index(BUF,11))
          Store(L1MC, Index(BUF,12))
          Store(L1MD, Index(BUF,13))
          Store(L1ME, Index(BUF,14))
          Store(L1MF, Index(BUF,15))
          Return (ToString(BUF))
        }
      }

      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c - new implementation
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          CreateDWordField(DSMB, 8, DEV1)
          CreateDWordField(DSMB, 12, DEV2)
          CreateDWordField(DSMB, 16, DEV3)
          CreateDWordField(DSMB, 20, DEV4)
          CreateDWordField(DSMB, 24, DEV5)
          CreateDWordField(DSMB, 28, DEV6)
          CreateDWordField(DSMB, 32, DEV7)
          CreateDWordField(DSMB, 36, DEV8)
          CreateDWordField(DSMB, 40, DEV9)
          CreateDWordField(DSMB, 44, DEVA)
          CreateDWordField(DSMB, 48, DEVB)
          Store (L1DI, I2CC)
          Store (DSMI(L1D0, L1A0, 0, L1BS), DEV0)
          Store (DSMI(L1D1, L1A1, 0, L1BS), DEV1)
          Store (DSMI(L1D2, L1A2, 0, L1BS), DEV2)
          Store (DSMI(L1D3, L1A3, 0, L1BS), DEV3)
          Store (DSMI(L1D4, L1A4, 0, L1BS), DEV4)
          Store (DSMI(L1D5, L1A5, 0, L1BS), DEV5)
          Store (DSMI(L1D6, L1A6, 0, L1BS), DEV6)
          Store (DSMI(L1D7, L1A7, 0, L1BS), DEV7)
          Store (DSMI(L1D8, L1A8, 0, L1BS), DEV8)
          Store (DSMI(L1D9, L1A9, 0, L1BS), DEV9)
          Store (DSMI(L1DA, L1AA, 0, L1BS), DEVA)
          Store (DSMI(L1DB, L1AB, 0, L1BS), DEVB)
          Return (DSMB)
        }
        Return (Buffer() {0})
      }

      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        // Function 0 Functions 1-13 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0xFF, 0x1F}) }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (L1DI) }
        // Function 2 (I2C device 0 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) {
          If (LGreater(L1DI, 0)) { Return (DSMI(L1D0, L1A0, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 3 (I2C device 1 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 3)) {
          If (LGreater(L1DI, 1)) { Return (DSMI(L1D1, L1A1, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 4 (I2C device 2 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 4)) {
          If (LGreater(L1DI, 2)) { Return (DSMI(L1D2, L1A2, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 5 (I2C device 3 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 5)) {
          If (LGreater(L1DI, 3)) { Return (DSMI(L1D3, L1A3, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 6 (I2C device 4 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 6)) {
          If (LGreater(L1DI, 4)) { Return (DSMI(L1D4, L1A4, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 7 (I2C device 5 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 7)) {
          If (LGreater(L1DI, 5)) { Return (DSMI(L1D5, L1A5, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 8 (I2C device 6 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L1DI, 6)) { Return (DSMI(L1D6, L1A6, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 9 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L1DI, 7)) { Return (DSMI(L1D7, L1A7, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 10 (I2C device 8 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 9)) {
          If (LGreater(L1DI, 8)) { Return (DSMI(L1D8, L1A8, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 11 (I2C device 9 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 10)) {
          If (LGreater(L1DI, 9)) { Return (DSMI(L1D9, L1A9, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 12 (I2C device 10 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 11)) {
          If (LGreater(L1DI, 10)) { Return (DSMI(L1DA, L1AA, 0, L1BS)) }
          Else { Return (0) }
        }
        // Function 13 (I2C device 11 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 12)) {
          If (LGreater(L1DI, 11)) { Return (DSMI(L1DB, L1AB, 0, L1BS)) }
          Else { Return (0) }
        }
      }
      Return (Buffer() {0})
    }
  } // Device (LNK1)

  Device (LNK2) {       // definition for Camera sensor module using Link2
    Name (_ADR, Zero)  // _ADR: Address
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (L2EN) { Return (0x0F) }
      Else { Return (0) }
    }
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (L2EN) { Return (CDEP(L2CL)) } // If LNK2 device is to be enabled
      Else { Return (Package() {\_SB.PC00}) }
    }
    Name (_UID, 2)  // _UID: Unique ID - Link Index

    Method (_CID, 0, NotSerialized) { // _CID: Compatible ID
      Return (HCID(2))
    }
    Method (_DDN, 0, Serialized) { // _DDN: DOS Device Name
      Name(BUF, Buffer(16) {})
      Store(L2M0, Index(BUF,0))
      Store(L2M1, Index(BUF,1))
      Store(L2M2, Index(BUF,2))
      Store(L2M3, Index(BUF,3))
      Store(L2M4, Index(BUF,4))
      Store(L2M5, Index(BUF,5))
      Store(L2M6, Index(BUF,6))
      Store(L2M7, Index(BUF,7))
      Store(L2M8, Index(BUF,8))
      Store(L2M9, Index(BUF,9))
      Store(L2MA, Index(BUF,10))
      Store(L2MB, Index(BUF,11))
      Store(L2MC, Index(BUF,12))
      Store(L2MD, Index(BUF,13))
      Store(L2ME, Index(BUF,14))
      Store(L2MF, Index(BUF,15))
      Return (ToString(BUF))
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x69, 0x0E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(L2PL, BPOS)
      Store(L2DG, RPOS)
      Return (PLDB)
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Store(Zero, Local0)  // empty resource buffer
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      If (LGreater (L2DI, 0)) {
        Store (IICB(L2A0, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 2 or more I2C devices were enabled
      If (LGreater(L2DI, 1)) {
        Store (IICB(L2A1, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 3 or more I2C devices were enabled
      If (LGreater(L2DI, 2)) {
        Store (IICB(L2A2, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 4 or more I2C devices were enabled
      If (LGreater(L2DI, 3)) {
        Store (IICB(L2A3, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 5 or more I2C devices were enabled
      If (LGreater(L2DI, 4)) {
        Store (IICB(L2A4, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 6 or more I2C devices were enabled
      If (LGreater(L2DI, 5)) {
        Store (IICB(L2A5, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 7 or more I2C devices were enabled
      If (LGreater(L2DI, 6)) {
        Store (IICB(L2A6, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 8 or more I2C devices were enabled
      If (LGreater(L2DI, 7)) {
        Store (IICB(L2A7, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 9 or more I2C devices were enabled
      If (LGreater(L2DI, 8)) {
        Store (IICB(L2A8, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 10 or more I2C devices were enabled
      If (LGreater(L2DI, 9)) {
        Store (IICB(L2A9, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 11 or more I2C devices were enabled
      If (LGreater(L2DI, 10)) {
        Store (IICB(L2AA, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 12 or more I2C devices were enabled
      If (LGreater(L2DI, 11)) {
        Store (IICB(L2AB, L2BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer() {
        0x00,                 // SSDB Version. Current version is 0x00
        0x00,                 // CRD version
          // CSI2DataStreamInterfaceGUID in CSI2_HOST_CONTROLLER,(offset 2)
          // 8A395669-11F7-4ea9-9C7D-20EE0AB5CA40,
        0x69,0x56,0x39,0x8A,  // 0x8a395669
        0xF7,0x11,            // 0x11F7
        0xA9,0x4E,            // 0x4EA9
        0x9C,0x7D,0x20,0xEE,  // 9C7D-20EE0AB5CA40
        0x0A,0xB5,0xCA,0x40,
          // - When the CIO2 device is configured in PCI mode then its B-D-F is 0-20-3
          //  (DevFunc = 0xA3, Bus = 0x00).
          // B-D-F of the CSI2 host controller to which the sensor connects.
        0xA3,                 // DevFunc DEV[5]:FUN[3]        (offset 18)
        0x00,                 // Bus BUS[8]                   (offset 19)
        0x00,0x00,0x00,0x00,  // G_DphyLinkEnFuses,           (offset 20)
        0x00,0x00,0x00,0x00,  // G_LanesClockDivision,        (offset 24)
        0x00,                 // LinkUsed,                    (offset 28)
        0x00,                 // LanesUsed,                   (offset 29)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_CLANE  (offset 30)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_CLANE  (offset 34)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE0 (offset 38)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE0 (offset 42)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE1 (offset 46)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE1 (offset 50)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE2 (offset 54)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE2 (offset 58)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE3 (offset 62)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE  (offset 66)
        0x00,0x00,0x00,0x00,  // MaxLaneSpeed[4]              (offset 70)
        0x00,                 // SensorCalibrationFileIndex   (offset 74)
        0x00,0x00,0x00,       // SensorCalibrationFileIndexMBZ  (offset 75)
        0x00,                 // Reserved: EEPROM Type (offset 78)
        0x00,                 // Reserved: VCM Type    (offset 79)
        0x0C,                 // Platform: SKC         (offset 80)
        0x00,                 // PlatformSub           (offset 81)
        0x00,                 // FlashSupport          (offset 82)
        0x00,                 // Privacy LED           (offset 83)
        0x00,                 // Degree                (offset 84)
        0x01,                 // mipiDefine            (offset 85)
        0x00,0x00,0x00,0x00,  // MCLK                  (offset 86)
        0x00,                 // Control Logic         (offset 90)
        0x00,0x00,0x00,0x00,  // Reserved              (offset 91)
        0x00,                 // PMIC Position         (offset 95)
        0x00,                 // Voltage Rail          (offset 96)
        0x00,                 // PPR Value             (offset 97)
        0x00,                 // PPR Unit              (offset 98)
        0x00,0x00,            // Reserved              (offset 99)
        0x00,0x00,0x00,0x00,0x00, // Reserved          (offset 101)
        0x00,0x00             // Reserved              (offset 106)
      })
      Store(L2DV, Index(PAR, 0))  // SSDB Version
      Store(L2CV, Index(PAR, 1))  // CRD Version
      Store(CDIV, Index(PAR, 24)) // LanesClk Division
      Store(L2LU, Index(PAR, 28)) // LinkUsed
      Store(L2NL, Index(PAR, 29)) // LaneUsed
      Store(L2EE, Index(PAR, 78)) // EEPROM Type
      Store(L2VC, Index(PAR, 79)) // VCM Type
      Store(L2FS, Index(PAR, 82)) // Flash Support
      Store(L2LE, Index(PAR, 83)) // Privacy LED
      Store(CDEG(L2DG), Index(PAR, 84)) // Degree
      CreateDWordField(PAR, 86, DAT) // MCLK
      Store(L2CK, DAT)
      Store(L2CL, Index(PAR, 90)) // Control Logic
      Store(L2PP, Index(PAR, 95)) // PMIC Position
      Store(L2VR, Index(PAR, 96)) // Voltage Rail
      Store(L2PV, Index(PAR, 97)) // PL4/PMAX PPR Value
      Store(L2PU, Index(PAR, 98)) // PL4/PMAX PPR Unit
      Return(PAR)
    }

    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
         // Function 0
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1
        If (LEqual (Arg2, 1)) {
          Name(BUF, Buffer(16) {})
          Store(L2M0, Index(BUF,0))
          Store(L2M1, Index(BUF,1))
          Store(L2M2, Index(BUF,2))
          Store(L2M3, Index(BUF,3))
          Store(L2M4, Index(BUF,4))
          Store(L2M5, Index(BUF,5))
          Store(L2M6, Index(BUF,6))
          Store(L2M7, Index(BUF,7))
          Store(L2M8, Index(BUF,8))
          Store(L2M9, Index(BUF,9))
          Store(L2MA, Index(BUF,10))
          Store(L2MB, Index(BUF,11))
          Store(L2MC, Index(BUF,12))
          Store(L2MD, Index(BUF,13))
          Store(L2ME, Index(BUF,14))
          Store(L2MF, Index(BUF,15))
          Return (ToString(BUF))
        }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c - new implementation
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          CreateDWordField(DSMB, 8, DEV1)
          CreateDWordField(DSMB, 12, DEV2)
          CreateDWordField(DSMB, 16, DEV3)
          CreateDWordField(DSMB, 20, DEV4)
          CreateDWordField(DSMB, 24, DEV5)
          CreateDWordField(DSMB, 28, DEV6)
          CreateDWordField(DSMB, 32, DEV7)
          CreateDWordField(DSMB, 36, DEV8)
          CreateDWordField(DSMB, 40, DEV9)
          CreateDWordField(DSMB, 44, DEVA)
          CreateDWordField(DSMB, 48, DEVB)
          Store (L2DI, I2CC)
          Store (DSMI(L2D0, L2A0, 0, L2BS), DEV0)
          Store (DSMI(L2D1, L2A1, 0, L2BS), DEV1)
          Store (DSMI(L2D2, L2A2, 0, L2BS), DEV2)
          Store (DSMI(L2D3, L2A3, 0, L2BS), DEV3)
          Store (DSMI(L2D4, L2A4, 0, L2BS), DEV4)
          Store (DSMI(L2D5, L2A5, 0, L2BS), DEV5)
          Store (DSMI(L2D6, L2A6, 0, L2BS), DEV6)
          Store (DSMI(L2D7, L2A7, 0, L2BS), DEV7)
          Store (DSMI(L2D8, L2A8, 0, L2BS), DEV8)
          Store (DSMI(L2D9, L2A9, 0, L2BS), DEV9)
          Store (DSMI(L2DA, L2AA, 0, L2BS), DEVA)
          Store (DSMI(L2DB, L2AB, 0, L2BS), DEVB)
          Return (DSMB)
        }
        Return (Buffer() {0})
      }

      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        // Function 0 Functions 1-13 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0xFF, 0x1F}) }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (L2DI) }
        // Function 2 (I2C device 0 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) {
          If (LGreater(L2DI, 0)) { Return (DSMI(L2D0, L2A0, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 3 (I2C device 1 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 3)) {
          If (LGreater(L2DI, 1)) { Return (DSMI(L2D1, L2A1, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 4 (I2C device 2 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 4)) {
          If (LGreater(L2DI, 2)) { Return (DSMI(L2D2, L2A2, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 5 (I2C device 3 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 5)) {
          If (LGreater(L2DI, 3)) { Return (DSMI(L2D3, L2A3, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 6 (I2C device 4 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 6)) {
          If (LGreater(L2DI, 4)) { Return (DSMI(L2D4, L2A4, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 7 (I2C device 5 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 7)) {
          If (LGreater(L2DI, 5)) { Return (DSMI(L2D5, L2A5, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 8 (I2C device 6 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L2DI, 6)) { Return (DSMI(L2D6, L2A6, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 9 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L2DI, 7)) { Return (DSMI(L2D7, L2A7, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 10 (I2C device 8 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 9)) {
          If (LGreater(L2DI, 8)) { Return (DSMI(L2D8, L2A8, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 11 (I2C device 9 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 10)) {
          If (LGreater(L2DI, 9)) { Return (DSMI(L2D9, L2A9, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 12 (I2C device 10 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 11)) {
          If (LGreater(L2DI, 10)) { Return (DSMI(L2DA, L2AA, 0, L2BS)) }
          Else { Return (0) }
        }
        // Function 13 (I2C device 11 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 12)) {
          If (LGreater(L2DI, 11)) { Return (DSMI(L2DB, L2AB, 0, L2BS)) }
          Else { Return (0) }
        }
      }
      Return (Buffer() {0})
    }
  } // Device (LNK2)

  Device (LNK3) {       // definition for Camera sensor module using Link3
    Name (_ADR, Zero)  // _ADR: Address
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (L3EN) { Return (0x0F) }
      Else { Return (0) }
    }
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (L3EN) { Return (CDEP(L3CL)) } // If LNK3 device is to be enabled
      Else { Return (Package() {\_SB.PC00}) }
    }
    Name (_UID, 3)  // _UID: Unique ID - Link Index

    Method (_CID, 0, NotSerialized) { // _CID: Compatible ID
      Return (HCID(3))
    }
    Method (_DDN, 0, Serialized) { // _DDN: DOS Device Name
      Name(BUF, Buffer(16) {})
      Store(L3M0, Index(BUF,0))
      Store(L3M1, Index(BUF,1))
      Store(L3M2, Index(BUF,2))
      Store(L3M3, Index(BUF,3))
      Store(L3M4, Index(BUF,4))
      Store(L3M5, Index(BUF,5))
      Store(L3M6, Index(BUF,6))
      Store(L3M7, Index(BUF,7))
      Store(L3M8, Index(BUF,8))
      Store(L3M9, Index(BUF,9))
      Store(L3MA, Index(BUF,10))
      Store(L3MB, Index(BUF,11))
      Store(L3MC, Index(BUF,12))
      Store(L3MD, Index(BUF,13))
      Store(L3ME, Index(BUF,14))
      Store(L3MF, Index(BUF,15))
      Return (ToString(BUF))
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x69, 0x0E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(L3PL, BPOS)
      Store(L3DG, RPOS)
      Return (PLDB)
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Store(Zero, Local0)  // empty resource buffer
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      If (LGreater (L3DI, 0)) {
        Store (IICB(L3A0, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 2 or more I2C devices were enabled
      If (LGreater(L3DI, 1)) {
        Store (IICB(L3A1, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 3 or more I2C devices were enabled
      If (LGreater(L3DI, 2)) {
        Store (IICB(L3A2, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 4 or more I2C devices were enabled
      If (LGreater(L3DI, 3)) {
        Store (IICB(L3A3, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 5 or more I2C devices were enabled
      If (LGreater(L3DI, 4)) {
        Store (IICB(L3A4, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 6 or more I2C devices were enabled
      If (LGreater(L3DI, 5)) {
        Store (IICB(L3A5, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 7 or more I2C devices were enabled
      If (LGreater(L3DI, 6)) {
        Store (IICB(L3A6, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 8 or more I2C devices were enabled
      If (LGreater(L3DI, 7)) {
        Store (IICB(L3A7, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 9 or more I2C devices were enabled
      If (LGreater(L3DI, 8)) {
        Store (IICB(L3A8, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 10 or more I2C devices were enabled
      If (LGreater(L3DI, 9)) {
        Store (IICB(L3A9, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 11 or more I2C devices were enabled
      If (LGreater(L3DI, 10)) {
        Store (IICB(L3AA, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 12 or more I2C devices were enabled
      If (LGreater(L3DI, 11)) {
        Store (IICB(L3AB, L3BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer() {
        0x00,                 // SSDB Version. Current version is 0x00
        0x00,                 // CRD version
          // CSI2DataStreamInterfaceGUID in CSI2_HOST_CONTROLLER,(offset 2)
          // 8A395669-11F7-4ea9-9C7D-20EE0AB5CA40,
        0x69,0x56,0x39,0x8A,  // 0x8a395669
        0xF7,0x11,            // 0x11F7
        0xA9,0x4E,            // 0x4EA9
        0x9C,0x7D,0x20,0xEE,  // 9C7D-20EE0AB5CA40
        0x0A,0xB5,0xCA,0x40,
          // - When the CIO2 device is configured in PCI mode then its B-D-F is 0-20-3
          //  (DevFunc = 0xA3, Bus = 0x00).
          // B-D-F of the CSI2 host controller to which the sensor connects.
        0xA3,                 // DevFunc DEV[5]:FUN[3]        (offset 18)
        0x00,                 // Bus BUS[8]                   (offset 19)
        0x00,0x00,0x00,0x00,  // G_DphyLinkEnFuses,           (offset 20)
        0x00,0x00,0x00,0x00,  // G_LanesClockDivision,        (offset 24)
        0x00,                 // LinkUsed,                    (offset 28)
        0x00,                 // LanesUsed,                   (offset 29)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_CLANE  (offset 30)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_CLANE  (offset 34)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE0 (offset 38)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE0 (offset 42)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE1 (offset 46)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE1 (offset 50)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE2 (offset 54)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE2 (offset 58)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE3 (offset 62)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE  (offset 66)
        0x00,0x00,0x00,0x00,  // MaxLaneSpeed[4]              (offset 70)
        0x00,                 // SensorCalibrationFileIndex   (offset 74)
        0x00,0x00,0x00,       // SensorCalibrationFileIndexMBZ  (offset 75)
        0x00,                 // Reserved: EEPROM Type (offset 78)
        0x00,                 // Reserved: VCM Type    (offset 79)
        0x0C,                 // Platform: SKC         (offset 80)
        0x00,                  // PlatformSub           (offset 81)
        0x00,                 // FlashSupport          (offset 82)
        0x00,                 // Privacy LED                  (offset 83)
        0x00,                 // Degree                (offset 84)
        0x01,                 // mipiDefine            (offset 85)
        0x00,0x00,0x00,0x00,  // MCLK                  (offset 86)
        0x00,                 // Control Logic         (offset 90)
        0x00,0x00,0x00,0x00,  // Reserved              (offset 91)
        0x00,                 // PMIC Position         (offset 95)
        0x00,                 // Voltage Rail          (offset 96)
        0x00,                 // PPR Value             (offset 97)
        0x00,                 // PPR Unit              (offset 98)
        0x00,0x00,            // Reserved              (offset 99)
        0x00,0x00,0x00,0x00,0x00, // Reserved          (offset 101)
        0x00,0x00             // Reserved              (offset 106)
      })
      Store(L3DV, Index(PAR, 0))  // SSDB Version
      Store(L3CV, Index(PAR, 1))  // CRD Version
      Store(CDIV, Index(PAR, 24)) // LanesClk Division
      Store(L3LU, Index(PAR, 28)) // LinkUsed
      Store(L3NL, Index(PAR, 29)) // LaneUsed
      Store(L3EE, Index(PAR, 78)) // EEPROM Type
      Store(L3VC, Index(PAR, 79)) // VCM Type
      Store(L3FS, Index(PAR, 82)) // Flash Support
      Store(L3LE, Index(PAR, 83)) // Privacy LED
      Store(CDEG(L3DG), Index(PAR, 84)) // Degree
      CreateDWordField(PAR, 86, DAT) // MCLK
      Store(L3CK, DAT)
      Store(L3CL, Index(PAR, 90)) // Control Logic
      Store(L3PP, Index(PAR, 95)) // PMIC Position
      Store(L3VR, Index(PAR, 96)) // Voltage Rail
      Store(L3PV, Index(PAR, 97)) // PL4/PMAX PPR Value
      Store(L3PU, Index(PAR, 98)) // PL4/PMAX PPR Unit
      Return(PAR)
    }

    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
         // Function 0
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1
        If (LEqual (Arg2, 1)) {
          Name(BUF, Buffer(16) {})
          Store(L3M0, Index(BUF,0))
          Store(L3M1, Index(BUF,1))
          Store(L3M2, Index(BUF,2))
          Store(L3M3, Index(BUF,3))
          Store(L3M4, Index(BUF,4))
          Store(L3M5, Index(BUF,5))
          Store(L3M6, Index(BUF,6))
          Store(L3M7, Index(BUF,7))
          Store(L3M8, Index(BUF,8))
          Store(L3M9, Index(BUF,9))
          Store(L3MA, Index(BUF,10))
          Store(L3MB, Index(BUF,11))
          Store(L3MC, Index(BUF,12))
          Store(L3MD, Index(BUF,13))
          Store(L3ME, Index(BUF,14))
          Store(L3MF, Index(BUF,15))
          Return (ToString(BUF))
        }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c - new implementation
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          CreateDWordField(DSMB, 8, DEV1)
          CreateDWordField(DSMB, 12, DEV2)
          CreateDWordField(DSMB, 16, DEV3)
          CreateDWordField(DSMB, 20, DEV4)
          CreateDWordField(DSMB, 24, DEV5)
          CreateDWordField(DSMB, 28, DEV6)
          CreateDWordField(DSMB, 32, DEV7)
          CreateDWordField(DSMB, 36, DEV8)
          CreateDWordField(DSMB, 40, DEV9)
          CreateDWordField(DSMB, 44, DEVA)
          CreateDWordField(DSMB, 48, DEVB)
          Store (L3DI, I2CC)
          Store (DSMI(L3D0, L3A0, 0, L3BS), DEV0)
          Store (DSMI(L3D1, L3A1, 0, L3BS), DEV1)
          Store (DSMI(L3D2, L3A2, 0, L3BS), DEV2)
          Store (DSMI(L3D3, L3A3, 0, L3BS), DEV3)
          Store (DSMI(L3D4, L3A4, 0, L3BS), DEV4)
          Store (DSMI(L3D5, L3A5, 0, L3BS), DEV5)
          Store (DSMI(L3D6, L3A6, 0, L3BS), DEV6)
          Store (DSMI(L3D7, L3A7, 0, L3BS), DEV7)
          Store (DSMI(L3D8, L3A8, 0, L3BS), DEV8)
          Store (DSMI(L3D9, L3A9, 0, L3BS), DEV9)
          Store (DSMI(L3DA, L3AA, 0, L3BS), DEVA)
          Store (DSMI(L3DB, L3AB, 0, L3BS), DEVB)
          Return (DSMB)
        }
        Return (Buffer() {0})
      }

      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        // Function 0 Functions 1-13 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0xFF, 0x1F}) }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (L3DI) }
        // Function 2 (I2C device 0 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) {
          If (LGreater(L3DI, 0)) { Return (DSMI(L3D0, L3A0, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 3 (I2C device 1 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 3)) {
          If (LGreater(L3DI, 1)) { Return (DSMI(L3D1, L3A1, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 4 (I2C device 2 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 4)) {
          If (LGreater(L3DI, 2)) { Return (DSMI(L3D2, L3A2, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 5 (I2C device 3 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 5)) {
          If (LGreater(L3DI, 3)) { Return (DSMI(L3D3, L3A3, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 6 (I2C device 4 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 6)) {
          If (LGreater(L3DI, 4)) { Return (DSMI(L3D4, L3A4, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 7 (I2C device 5 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 7)) {
          If (LGreater(L3DI, 5)) { Return (DSMI(L3D5, L3A5, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 8 (I2C device 6 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L3DI, 6)) { Return (DSMI(L3D6, L3A6, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 9 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L3DI, 7)) { Return (DSMI(L3D7, L3A7, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 10 (I2C device 8 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 9)) {
          If (LGreater(L3DI, 8)) { Return (DSMI(L3D8, L3A8, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 11 (I2C device 9 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 10)) {
          If (LGreater(L3DI, 9)) { Return (DSMI(L3D9, L3A9, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 12 (I2C device 10 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 11)) {
          If (LGreater(L3DI, 10)) { Return (DSMI(L3DA, L3AA, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 13 (I2C device 11 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 12)) {
          If (LGreater(L3DI, 11)) { Return (DSMI(L3DB, L3AB, 0, L3BS)) }
          Else { Return (0) }
        }
      }
      Return (Buffer() {0})
    }
  } // Device (LNK3)

  Device (LNK4) {       // definition for Camera sensor module using Link4
    Name (_ADR, Zero)  // _ADR: Address
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (L4EN) { Return (0x0F) }
      Else { Return (0) }
    }
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (L4EN) { Return (CDEP(L4CL)) } // If LNK4 device is to be enabled
      Else { Return (Package() {\_SB.PC00}) }
    }
    Name (_UID, 4)  // _UID: Unique ID - Link Index

    Method (_CID, 0, NotSerialized) { // _CID: Compatible ID
      Return (HCID(4))
    }
    Method (_DDN, 0, Serialized) { // _DDN: DOS Device Name
      Name(BUF, Buffer(16) {})
      Store(L4M0, Index(BUF,0))
      Store(L4M1, Index(BUF,1))
      Store(L4M2, Index(BUF,2))
      Store(L4M3, Index(BUF,3))
      Store(L4M4, Index(BUF,4))
      Store(L4M5, Index(BUF,5))
      Store(L4M6, Index(BUF,6))
      Store(L4M7, Index(BUF,7))
      Store(L4M8, Index(BUF,8))
      Store(L4M9, Index(BUF,9))
      Store(L4MA, Index(BUF,10))
      Store(L4MB, Index(BUF,11))
      Store(L4MC, Index(BUF,12))
      Store(L4MD, Index(BUF,13))
      Store(L4ME, Index(BUF,14))
      Store(L4MF, Index(BUF,15))
      Return (ToString(BUF))
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x69, 0x0E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(L4PL, BPOS)
      Store(L4DG, RPOS)
      Return (PLDB)
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Store(Zero, Local0)  // empty resource buffer
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      If (LGreater (L4DI, 0)) {
        Store (IICB(L4A0, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 2 or more I2C devices were enabled
      If (LGreater(L4DI, 1)) {
        Store (IICB(L4A1, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 3 or more I2C devices were enabled
      If (LGreater(L4DI, 2)) {
        Store (IICB(L4A2, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 4 or more I2C devices were enabled
      If (LGreater(L4DI, 3)) {
        Store (IICB(L4A3, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 5 or more I2C devices were enabled
      If (LGreater(L4DI, 4)) {
        Store (IICB(L4A4, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 6 or more I2C devices were enabled
      If (LGreater(L4DI, 5)) {
        Store (IICB(L4A5, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 7 or more I2C devices were enabled
      If (LGreater(L4DI, 6)) {
        Store (IICB(L4A6, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 8 or more I2C devices were enabled
      If (LGreater(L4DI, 7)) {
        Store (IICB(L4A7, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 9 or more I2C devices were enabled
      If (LGreater(L4DI, 8)) {
        Store (IICB(L4A8, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 10 or more I2C devices were enabled
      If (LGreater(L4DI, 9)) {
        Store (IICB(L4A9, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 11 or more I2C devices were enabled
      If (LGreater(L4DI, 10)) {
        Store (IICB(L4AA, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 12 or more I2C devices were enabled
      If (LGreater(L4DI, 11)) {
        Store (IICB(L4AB, L4BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer() {
        0x00,                 // SSDB Version. Current version is 0x00
        0x00,                 // CRD version
          // CSI2DataStreamInterfaceGUID in CSI2_HOST_CONTROLLER,(offset 2)
          // 8A395669-11F7-4ea9-9C7D-20EE0AB5CA40,
        0x69,0x56,0x39,0x8A,  // 0x8a395669
        0xF7,0x11,            // 0x11F7
        0xA9,0x4E,            // 0x4EA9
        0x9C,0x7D,0x20,0xEE,  // 9C7D-20EE0AB5CA40
        0x0A,0xB5,0xCA,0x40,
          // - When the CIO2 device is configured in PCI mode then its B-D-F is 0-20-3
          //  (DevFunc = 0xA3, Bus = 0x00).
          // B-D-F of the CSI2 host controller to which the sensor connects.
        0xA3,                 // DevFunc DEV[5]:FUN[3]        (offset 18)
        0x00,                 // Bus BUS[8]                   (offset 19)
        0x00,0x00,0x00,0x00,  // G_DphyLinkEnFuses,           (offset 20)
        0x00,0x00,0x00,0x00,  // G_LanesClockDivision,        (offset 24)
        0x00,                 // LinkUsed,                    (offset 28)
        0x00,                 // LanesUsed,                   (offset 29)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_CLANE  (offset 30)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_CLANE  (offset 34)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE0 (offset 38)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE0 (offset 42)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE1 (offset 46)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE1 (offset 50)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE2 (offset 54)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE2 (offset 58)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE3 (offset 62)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE  (offset 66)
        0x00,0x00,0x00,0x00,  // MaxLaneSpeed[4]              (offset 70)
        0x00,                 // SensorCalibrationFileIndex   (offset 74)
        0x00,0x00,0x00,       // SensorCalibrationFileIndexMBZ  (offset 75)
        0x00,                 // Reserved: EEPROM Type (offset 78)
        0x00,                 // Reserved: VCM Type    (offset 79)
        0x0C,                 // Platform: SKC         (offset 80)
        0x00,                  // PlatformSub           (offset 81)
        0x00,                 // FlashSupport          (offset 82)
        0x00,                 // Privacy LED                  (offset 83)
        0x00,                 // Degree                (offset 84)
        0x01,                 // mipiDefine            (offset 85)
        0x00,0x00,0x00,0x00,  // MCLK                  (offset 86)
        0x00,                 // Control Logic         (offset 90)
        0x00,0x00,0x00,0x00,  // Reserved              (offset 91)
        0x00,                 // PMIC Position         (offset 95)
        0x00,                 // Voltage Rail          (offset 96)
        0x00,                 // PPR Value             (offset 97)
        0x00,                 // PPR Unit              (offset 98)
        0x00,0x00,            // Reserved              (offset 99)
        0x00,0x00,0x00,0x00,0x00, // Reserved          (offset 101)
        0x00,0x00             // Reserved              (offset 106)
      })
      Store(L4DV, Index(PAR, 0))  // SSDB Version
      Store(L4CV, Index(PAR, 1))  // CRD Version
      Store(CDIV, Index(PAR, 24)) // LanesClk Division
      Store(L4LU, Index(PAR, 28)) // LinkUsed
      Store(L4NL, Index(PAR, 29)) // LaneUsed
      Store(L4EE, Index(PAR, 78)) // EEPROM Type
      Store(L4VC, Index(PAR, 79)) // VCM Type
      Store(L4FS, Index(PAR, 82)) // Flash Support
      Store(L4LE, Index(PAR, 83)) // Privacy LED
      Store(CDEG(L4DG), Index(PAR, 84)) // Degree
      CreateDWordField(PAR, 86, DAT) // MCLK
      Store(L4CK, DAT)
      Store(L4CL, Index(PAR, 90)) // Control Logic
      Store(L4PP, Index(PAR, 95)) // PMIC Position
      Store(L4VR, Index(PAR, 96)) // Voltage Rail
      Store(L4PV, Index(PAR, 97)) // PL4/PMAX PPR Value
      Store(L4PU, Index(PAR, 98)) // PL4/PMAX PPR Unit
      Return(PAR)
    }

    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
         // Function 0
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1
        If (LEqual (Arg2, 1)) {
          Name(BUF, Buffer(16) {})
          Store(L4M0, Index(BUF,0))
          Store(L4M1, Index(BUF,1))
          Store(L4M2, Index(BUF,2))
          Store(L4M3, Index(BUF,3))
          Store(L4M4, Index(BUF,4))
          Store(L4M5, Index(BUF,5))
          Store(L4M6, Index(BUF,6))
          Store(L4M7, Index(BUF,7))
          Store(L4M8, Index(BUF,8))
          Store(L4M9, Index(BUF,9))
          Store(L4MA, Index(BUF,10))
          Store(L4MB, Index(BUF,11))
          Store(L4MC, Index(BUF,12))
          Store(L4MD, Index(BUF,13))
          Store(L4ME, Index(BUF,14))
          Store(L4MF, Index(BUF,15))
          Return (ToString(BUF))
        }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c - new implementation
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          CreateDWordField(DSMB, 8, DEV1)
          CreateDWordField(DSMB, 12, DEV2)
          CreateDWordField(DSMB, 16, DEV3)
          CreateDWordField(DSMB, 20, DEV4)
          CreateDWordField(DSMB, 24, DEV5)
          CreateDWordField(DSMB, 28, DEV6)
          CreateDWordField(DSMB, 32, DEV7)
          CreateDWordField(DSMB, 36, DEV8)
          CreateDWordField(DSMB, 40, DEV9)
          CreateDWordField(DSMB, 44, DEVA)
          CreateDWordField(DSMB, 48, DEVB)
          Store (L4DI, I2CC)
          Store (DSMI(L4D0, L4A0, 0, L4BS), DEV0)
          Store (DSMI(L4D1, L4A1, 0, L4BS), DEV1)
          Store (DSMI(L4D2, L4A2, 0, L4BS), DEV2)
          Store (DSMI(L4D3, L4A3, 0, L4BS), DEV3)
          Store (DSMI(L4D4, L4A4, 0, L4BS), DEV4)
          Store (DSMI(L4D5, L4A5, 0, L4BS), DEV5)
          Store (DSMI(L4D6, L4A6, 0, L4BS), DEV6)
          Store (DSMI(L4D7, L4A7, 0, L4BS), DEV7)
          Store (DSMI(L4D8, L4A8, 0, L4BS), DEV8)
          Store (DSMI(L4D9, L4A9, 0, L4BS), DEV9)
          Store (DSMI(L4DA, L4AA, 0, L4BS), DEVA)
          Store (DSMI(L4DB, L4AB, 0, L4BS), DEVB)
          Return (DSMB)
        }
        Return (Buffer() {0})
      }

      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        // Function 0 Functions 1-13 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0xFF, 0x1F}) }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (L4DI) }
        // Function 2 (I2C device 0 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) {
          If (LGreater(L4DI, 0)) { Return (DSMI(L4D0, L4A0, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 3 (I2C device 1 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 3)) {
          If (LGreater(L4DI, 1)) { Return (DSMI(L4D1, L4A1, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 4 (I2C device 2 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 4)) {
          If (LGreater(L4DI, 2)) { Return (DSMI(L4D2, L4A2, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 5 (I2C device 3 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 5)) {
          If (LGreater(L4DI, 3)) { Return (DSMI(L4D3, L4A3, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 6 (I2C device 4 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 6)) {
          If (LGreater(L4DI, 4)) { Return (DSMI(L4D4, L4A4, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 7 (I2C device 5 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 7)) {
          If (LGreater(L4DI, 5)) { Return (DSMI(L4D5, L4A5, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 8 (I2C device 6 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L4DI, 6)) { Return (DSMI(L4D6, L4A6, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 9 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L4DI, 7)) { Return (DSMI(L3D7, L3A7, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 10 (I2C device 8 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 9)) {
          If (LGreater(L4DI, 8)) { Return (DSMI(L3D8, L3A8, 0, L3BS)) }
          Else { Return (0) }
        }
        // Function 11 (I2C device 9 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 10)) {
          If (LGreater(L4DI, 9)) { Return (DSMI(L4D9, L4A9, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 12 (I2C device 10 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 11)) {
          If (LGreater(L4DI, 10)) { Return (DSMI(L4DA, L4AA, 0, L4BS)) }
          Else { Return (0) }
        }
        // Function 13 (I2C device 11 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 12)) {
          If (LGreater(L4DI, 11)) { Return (DSMI(L4DB, L4AB, 0, L4BS)) }
          Else { Return (0) }
        }
      }
      Return (Buffer() {0})
    }
  } // Device (LNK4)

  Device (LNK5) {       // definition for Camera sensor module using Link5
    Name (_ADR, Zero)  // _ADR: Address
    Method (_STA, 0, NotSerialized) {  // _STA: Status
      If (L5EN) { Return (0x0F) }
      Else { Return (0) }
    }
    Method (_DEP, 0, NotSerialized) {  // _DEP: Operation Region Dependencies
      If (L5EN) { Return (CDEP(L5CL)) } // If LNK5 device is to be enabled
      Else { Return (Package() {\_SB.PC00}) }
    }
    Name (_UID, 5)  // _UID: Unique ID - Link Index

    Method (_CID, 0, NotSerialized) { // _CID: Compatible ID
      Return (HCID(5))
    }
    Method (_DDN, 0, Serialized) { // _DDN: DOS Device Name
      Name(BUF, Buffer(16) {})
      Store(L5M0, Index(BUF,0))
      Store(L5M1, Index(BUF,1))
      Store(L5M2, Index(BUF,2))
      Store(L5M3, Index(BUF,3))
      Store(L5M4, Index(BUF,4))
      Store(L5M5, Index(BUF,5))
      Store(L5M6, Index(BUF,6))
      Store(L5M7, Index(BUF,7))
      Store(L5M8, Index(BUF,8))
      Store(L5M9, Index(BUF,9))
      Store(L5MA, Index(BUF,10))
      Store(L5MB, Index(BUF,11))
      Store(L5MC, Index(BUF,12))
      Store(L5MD, Index(BUF,13))
      Store(L5ME, Index(BUF,14))
      Store(L5MF, Index(BUF,15))
      Return (ToString(BUF))
    }
    Method(_PLD, 0, Serialized) { // _PLD: Physical Location of Device
      Name(PLDB, Package(1) {
        Buffer(0x14) {
          /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          /* 0008 */   0x69, 0x0E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
          /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
        }
      })
      CreateByteField(DerefOf(Index(PLDB,0)), 8, BPOS)
      CreateField(DerefOf(Index(PLDB,0)), 115,  4, RPOS) // Rotation field
      Store(L5PL, BPOS)
      Store(L5DG, RPOS)
      Return (PLDB)
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Store(Zero, Local0)  // empty resource buffer
      // Create I2C Bus resource template using as parameters data provided in the BIOS setup
      If (LGreater (L5DI, 0)) {
        Store (IICB(L5A0, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 2 or more I2C devices were enabled
      If (LGreater(L5DI, 1)) {
        Store (IICB(L5A1, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 3 or more I2C devices were enabled
      If (LGreater(L5DI, 2)) {
        Store (IICB(L5A2, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 4 or more I2C devices were enabled
      If (LGreater(L5DI, 3)) {
        Store (IICB(L5A3, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 5 or more I2C devices were enabled
      If (LGreater(L5DI, 4)) {
        Store (IICB(L5A4, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 6 or more I2C devices were enabled
      If (LGreater(L5DI, 5)) {
        Store (IICB(L5A5, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 7 or more I2C devices were enabled
      If (LGreater(L5DI, 6)) {
        Store (IICB(L5A6, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 8 or more I2C devices were enabled
      If (LGreater(L5DI, 7)) {
        Store (IICB(L5A7, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 9 or more I2C devices were enabled
      If (LGreater(L5DI, 8)) {
        Store (IICB(L5A8, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 10 or more I2C devices were enabled
      If (LGreater(L5DI, 9)) {
        Store (IICB(L5A9, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 11 or more I2C devices were enabled
      If (LGreater(L5DI, 10)) {
        Store (IICB(L5AA, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      // Add next resource buffer if 12 or more I2C devices were enabled
      If (LGreater(L5DI, 11)) {
        Store (IICB(L5AB, L5BS), Local1)
        ConcatenateResTemplate(Local0, Local1, Local2)
        Store(Local2, Local0)
      }
      Return (Local0)
    }
    Method(SSDB, 0, Serialized)
    {
      Name(PAR, Buffer() {
        0x00,                 // SSDB Version. Current version is 0x00
        0x00,                 // CRD version
          // CSI2DataStreamInterfaceGUID in CSI2_HOST_CONTROLLER,(offset 2)
          // 8A395669-11F7-4ea9-9C7D-20EE0AB5CA40,
        0x69,0x56,0x39,0x8A,  // 0x8a395669
        0xF7,0x11,            // 0x11F7
        0xA9,0x4E,            // 0x4EA9
        0x9C,0x7D,0x20,0xEE,  // 9C7D-20EE0AB5CA40
        0x0A,0xB5,0xCA,0x40,
          // - When the CIO2 device is configured in PCI mode then its B-D-F is 0-20-3
          //  (DevFunc = 0xA3, Bus = 0x00).
          // B-D-F of the CSI2 host controller to which the sensor connects.
        0xA3,                 // DevFunc DEV[5]:FUN[3]        (offset 18)
        0x00,                 // Bus BUS[8]                   (offset 19)
        0x00,0x00,0x00,0x00,  // G_DphyLinkEnFuses,           (offset 20)
        0x00,0x00,0x00,0x00,  // G_LanesClockDivision,        (offset 24)
        0x00,                 // LinkUsed,                    (offset 28)
        0x00,                 // LanesUsed,                   (offset 29)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_CLANE  (offset 30)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_CLANE  (offset 34)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE0 (offset 38)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE0 (offset 42)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE1 (offset 46)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE1 (offset 50)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE2 (offset 54)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE2 (offset 58)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_TERMEN_DLANE3 (offset 62)
        0x00,0x00,0x00,0x00,  // CSI_RX_DLY_CNT_SETTLE_DLANE  (offset 66)
        0x00,0x00,0x00,0x00,  // MaxLaneSpeed[4]              (offset 70)
        0x00,                 // SensorCalibrationFileIndex   (offset 74)
        0x00,0x00,0x00,       // SensorCalibrationFileIndexMBZ  (offset 75)
        0x00,                 // Reserved: EEPROM Type (offset 78)
        0x00,                 // Reserved: VCM Type    (offset 79)
        0x0C,                 // Platform: SKC         (offset 80)
        0x00,                  // PlatformSub           (offset 81)
        0x00,                 // FlashSupport          (offset 82)
        0x00,                 // Privacy LED                  (offset 83)
        0x00,                 // Degree                (offset 84)
        0x01,                 // mipiDefine            (offset 85)
        0x00,0x00,0x00,0x00,  // MCLK                  (offset 86)
        0x00,                 // Control Logic         (offset 90)
        0x00,0x00,0x00,0x00,  // Reserved              (offset 91)
        0x00,                 // PMIC Position         (offset 95)
        0x00,                 // Voltage Rail          (offset 96)
        0x00,                 // PPR Value             (offset 97)
        0x00,                 // PPR Unit              (offset 98)
        0x00,0x00,            // Reserved              (offset 99)
        0x00,0x00,0x00,0x00,0x00, // Reserved          (offset 101)
        0x00,0x00             // Reserved              (offset 106)
      })
      Store(L5DV, Index(PAR, 0))  // SSDB Version
      Store(L5CV, Index(PAR, 1))  // CRD Version
      Store(CDIV, Index(PAR, 24)) // LanesClk Division
      Store(L5LU, Index(PAR, 28)) // LinkUsed
      Store(L5NL, Index(PAR, 29)) // LaneUsed
      Store(L5EE, Index(PAR, 78)) // EEPROM Type
      Store(L5VC, Index(PAR, 79)) // VCM Type
      Store(L5FS, Index(PAR, 82)) // Flash Support
      Store(L5LE, Index(PAR, 83)) // Privacy LED
      Store(CDEG(L5DG), Index(PAR, 84)) // Degree
      CreateDWordField(PAR, 86, DAT) // MCLK
      Store(L5CK, DAT)
      Store(L5CL, Index(PAR, 90)) // Control Logic
      Store(L5PP, Index(PAR, 95)) // PMIC Position
      Store(L5VR, Index(PAR, 96)) // Voltage Rail
      Store(L5PV, Index(PAR, 97)) // PL4/PMAX PPR Value
      Store(L5PU, Index(PAR, 98)) // PL4/PMAX PPR Unit
      Return(PAR)
    }

    Method (_DSM, 4, Serialized) {  // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
         // Function 0
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1
        If (LEqual (Arg2, 1)) {
          Name(BUF, Buffer(16) {})
          Store(L5M0, Index(BUF,0))
          Store(L5M1, Index(BUF,1))
          Store(L5M2, Index(BUF,2))
          Store(L5M3, Index(BUF,3))
          Store(L5M4, Index(BUF,4))
          Store(L5M5, Index(BUF,5))
          Store(L5M6, Index(BUF,6))
          Store(L5M7, Index(BUF,7))
          Store(L5M8, Index(BUF,8))
          Store(L5M9, Index(BUF,9))
          Store(L5MA, Index(BUF,10))
          Store(L5MB, Index(BUF,11))
          Store(L5MC, Index(BUF,12))
          Store(L5MD, Index(BUF,13))
          Store(L5ME, Index(BUF,14))
          Store(L5MF, Index(BUF,15))
          Return (ToString(BUF))
        }
      }
      If (LEqual (Arg0,ToUUID("5815C5C8-C47D-477B-9A8D-76173176414B"))) { // DsmI2c - new implementation
        If (LEqual (Arg2, 0)) {
          If (LEqual (Arg1, 0)) {
            Return (Buffer() {0x03})  // Function 1 is supported for revision 0
          }
          Else { Return (Buffer() {0x01}) } // revisions higher than 0 are not supported
        }
        // Function 1 (I2C count + Buffer with I2C device information)
        If (LEqual (Arg2, 1)) {
          Name (DSMB, Buffer(13*4) {})
          CreateDWordField(DSMB, 0, I2CC)
          CreateDWordField(DSMB, 4, DEV0)
          CreateDWordField(DSMB, 8, DEV1)
          CreateDWordField(DSMB, 12, DEV2)
          CreateDWordField(DSMB, 16, DEV3)
          CreateDWordField(DSMB, 20, DEV4)
          CreateDWordField(DSMB, 24, DEV5)
          CreateDWordField(DSMB, 28, DEV6)
          CreateDWordField(DSMB, 32, DEV7)
          CreateDWordField(DSMB, 36, DEV8)
          CreateDWordField(DSMB, 40, DEV9)
          CreateDWordField(DSMB, 44, DEVA)
          CreateDWordField(DSMB, 48, DEVB)
          Store (L5DI, I2CC)
          Store (DSMI(L5D0, L5A0, 0, L5BS), DEV0)
          Store (DSMI(L5D1, L5A1, 0, L5BS), DEV1)
          Store (DSMI(L5D2, L5A2, 0, L5BS), DEV2)
          Store (DSMI(L5D3, L5A3, 0, L5BS), DEV3)
          Store (DSMI(L5D4, L5A4, 0, L5BS), DEV4)
          Store (DSMI(L5D5, L5A5, 0, L5BS), DEV5)
          Store (DSMI(L5D6, L5A6, 0, L5BS), DEV6)
          Store (DSMI(L5D7, L5A7, 0, L5BS), DEV7)
          Store (DSMI(L5D8, L5A8, 0, L5BS), DEV8)
          Store (DSMI(L5D9, L5A9, 0, L5BS), DEV9)
          Store (DSMI(L5DA, L5AA, 0, L5BS), DEVA)
          Store (DSMI(L5DB, L5AB, 0, L5BS), DEVB)
          Return (DSMB)
        }
        Return (Buffer() {0})
      }

      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        // Function 0 Functions 1-13 supported for revision 0
        If (LEqual (Arg2, 0)) { Return (Buffer() {0xFF, 0x1F}) }
        // Function 1 (I2C count)
        If (LEqual (Arg2, 1)) { Return (L5DI) }
        // Function 2 (I2C device 0 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 2)) {
          If (LGreater(L5DI, 0)) { Return (DSMI(L5D0, L5A0, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 3 (I2C device 1 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 3)) {
          If (LGreater(L5DI, 1)) { Return (DSMI(L5D1, L5A1, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 4 (I2C device 2 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 4)) {
          If (LGreater(L5DI, 2)) { Return (DSMI(L5D2, L5A2, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 5 (I2C device 3 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 5)) {
          If (LGreater(L5DI, 3)) { Return (DSMI(L5D3, L5A3, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 6 (I2C device 4 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 6)) {
          If (LGreater(L5DI, 4)) { Return (DSMI(L5D4, L5A4, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 7 (I2C device 5 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 7)) {
          If (LGreater(L5DI, 5)) { Return (DSMI(L5D5, L5A5, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 8 (I2C device 6 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L5DI, 6)) { Return (DSMI(L5D6, L5A6, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 9 (I2C device 7 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 8)) {
          If (LGreater(L5DI, 7)) { Return (DSMI(L5D7, L5A7, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 10 (I2C device 8 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 9)) {
          If (LGreater(L5DI, 8)) { Return (DSMI(L5D8, L5A8, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 11 (I2C device 9 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 10)) {
          If (LGreater(L5DI, 9)) { Return (DSMI(L5D9, L5A9, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 12 (I2C device 10 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 11)) {
          If (LGreater(L5DI, 10)) { Return (DSMI(L5DA, L5AA, 0, L5BS)) }
          Else { Return (0) }
        }
        // Function 13 (I2C device 11 information (bits.31-24:BUS.23-16:Speed.15-8:Addr.7-0:Function))
        If (LEqual (Arg2, 12)) {
          If (LGreater(L5DI, 11)) { Return (DSMI(L5DB, L5AB, 0, L5BS)) }
          Else { Return (0) }
        }
      }
      Return (Buffer() {0})
    }
  } // Device (LNK5)
}

Scope(\_SB.PC00.I2C2) {
  Device(CAM0) { //IMX135-CRDG2
    Name(_ADR, Zero)
    Name(_CID, "INT3471")
    Name(_DDN, "IMX135-CRDG2")
    Name(_UID, "0")
    Name (_DEP, Package () {  // _DEP: Dependencies
      \_SB.PC00.I2C2.PMIC
      })
    Name(_PLD, Package (0x01) {
      Buffer(0x14) {
        0x82,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x69,
        0x0E,
        0x00,
        0x00,
        0x03,
        0x00,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF
      }
    })
    Method(_CRS, 0x0, Serialized) {
      Name(SBUF, ResourceTemplate() {
        I2cSerialBus (0x10, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,) //sensor address 0x10
        I2cSerialBus (0x0E, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,) //VCM
        I2cSerialBus (0x50, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,) //EEPROM
        I2cSerialBus (0x51, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,) //EEPROM
        I2cSerialBus (0x52, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,) //EEPROM
        I2cSerialBus (0x53, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
           "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,)  //EEPROM
      })
      Return (SBUF)
    }

    Method(_STA, 0, NotSerialized) { // _STA: Status
      If (LEqual(SCSS,1))
      {
      Return (0x0F)
    }
    Else
    {
      Return (0x00)
    }
    }
    Method(SSDB, 0, Serialized) {
      Name(PAR, Buffer(0x6C) {
        0x00,                              //Version
        0x50,                              //SKU - CRD-G2
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              //GUID for CSI2 host controller
        0x00,                              //DevFunction
        0x00,                              //Bus
        0x00, 0x00, 0x00, 0x00,          //DphyLinkEnFuses
        0x00, 0x00, 0x00, 0x00,          //ClockDiv
        0x00,                              //LinkUsed
        0x04,                              //LaneUsed
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_TERMEN_CLANE
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_SETTLE_CLANE
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_TERMEN_DLANE0
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_SETTLE_DLANE0
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_TERMEN_DLANE1
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_SETTLE_DLANE1
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_TERMEN_DLANE2
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_SETTLE_DLANE2
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_TERMEN_DLANE3
        0x00, 0x00, 0x00, 0x00,          //CSI_RX_DLY_CNT_SETTLE_DLANE3
        0x00, 0x00, 0x00, 0x00,          //MaxLaneSpeed
        0x00,                              //SensorCalibrationFileIdx
        0x00, 0x00, 0x00,                  //SensorCalibrationFileIdxInMBZ
        0x08,                            //RomType: EEPROM CAT24C08
        0x03,                            //VcmType: AD5816G
        0x09,                            //Platform info
        0x00,                            //Platform sub info
        0x02,                            //Flash Disabled
        0x01,                            //Privacy LED supported
        0x00,                            //0 degree
        0x01,                            //MIPI link/lane number defined in ACPI
        0x00, 0x36, 0x6E, 0x01,          //MCLK: 24000000Hz
        0x00,                            //Control logic ID
        0x00, 0x00,                      //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,    //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,    //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,    //Reserved
      })
      Return (PAR)
    }
    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
        Return ("13P2BAD33")
      }
      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        If (LEqual (Arg2, One)) { /* I2C count */
          Return (0x06)
        }
        If (LEqual (Arg2, 0x02)) { /* I2C 0:bit31-24:BUS. 23-16:Speed.15-8:Addr. 0-7:Function*/
          Return (0x02001000) /*Sensor,I2C bus2, Addr:0x10 .Function:sensor*/
        }
        If (LEqual (Arg2, 0x03)) {
          Return (0x02000E01) /* AD5816 I2C bus2, Addr:0x0E.Function:VCM*/
        }
        If (LEqual (Arg2, 0x04)) {
          Return (0x02005002) /*I2C bus2 EEPROM:CAT24C08*/
        }
        If (LEqual (Arg2, 0x05)) {
          Return (0x02005103) /*I2C bus 2 EEPROM:CAT24C08*/
        }
        If (LEqual (Arg2, 0x06)) {
          Return (0x02005204) /*I2C bus 2 EEPROM:CAT24C08*/
        }
        If (LEqual (Arg2, 0x07)) {
          Return (0x02005305) /*I2C bus 2 EEPROM:CAT24C08*/
        }
      }
      Return (Buffer() {0})
    }
  }
}

Scope(\_SB.PC00.I2C4) {
  Device(CAM1) {     //OV2740-CRDG2
    Name(_ADR, Zero)
    Name(_CID, "INT3474")
    Name(_DDN, "OV2740-CRDG2")
    Name(_UID, "0")
    Name (_DEP, Package () { // _DEP: Dependencies
      \_SB.PC00.I2C2.PMIC
    })
    Name(_PLD, Package (0x01) {
      Buffer(0x14) {
        0x82,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x61,
        0x0E,
        0x00,
        0x00,
        0x03,
        0x00,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF
      }
    })
    Method(_CRS, 0x0, Serialized) {
      Name(SBUF, ResourceTemplate() {
        I2cSerialBus (0x36, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C4", 0x00, ResourceConsumer, ,)
      })
      Return (SBUF)
    }
    Method(_STA, 0, NotSerialized) { // _STA: Status
    If (LEqual(SCSS,1))
    {
        Return (0x0F)
    }
    Else
      {
      Return (0x00)
    }
    }
    Method(SSDB, 0, Serialized) {
      Name(PAR, Buffer(0x6C) {
        0x00,                               //Version
        0x50,                               //SKU: CRD-G2
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //GUID for CSI2 host controller
        0x00,                               //DevFunction
        0x00,                               //Bus
        0x00, 0x00, 0x00, 0x00,             //DphyLinkEnFuses
        0x00, 0x00, 0x00, 0x00,             //ClockDiv
        0x01,                               //LinkUsed
        0x02,                               //LaneUsed
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_TERMEN_CLANE
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_SETTLE_CLANE
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_TERMEN_DLANE0
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_SETTLE_DLANE0
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_TERMEN_DLANE1
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_SETTLE_DLANE1
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_TERMEN_DLANE2
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_SETTLE_DLANE2
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_TERMEN_DLANE3
        0x00, 0x00, 0x00, 0x00,             //CSI_RX_DLY_CNT_SETTLE_DLANE3
        0x00, 0x00, 0x00, 0x00,             //MaxLaneSpeed
        0x00,                               //SensorCalibrationFileIdx
        0x00, 0x00, 0x00,                   //SensorCalibrationFileIdxInMBZ
        0x00,                               // RomType: none
        0x00,                               // VcmType: NONE
        0x09,                               // Platform info
        0x00,                               // Platform sub info
        0x02,                               // Flash Disabled
        0x01,                               // Privacy LED supported
        0x00,                               // 0 degree -- Change to 90
        0x01,                               // MIPI link/lane defined in ACPI
        0x00, 0xF8, 0x24, 0x01,             // MCLK: 19200000Hz
        0x00,                               // Control logic ID
        0x00, 0x00,                         //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,       //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,       //Reserved
        0x00, 0x00, 0x00, 0x00, 0x00,       //Reserved
      })
      Return (PAR)
    }
    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("822ACE8F-2814-4174-A56B-5F029FE079EE"))) { /*DsmSnsrModule*/
        Return ("4SF259T2")
      }
      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        If (LEqual (Arg2, One)) { /* I2C count */
          Return (0x01)
        }
        If (LEqual (Arg2, 0x02)) { /* I2C 0:bit31-24:BUS. 23-16:Speed.15-8:Addr. 0-7:Function*/
          Return (0x04003600) /*OV2740 Sensor,I2C bus4, Addr:0x36.Function:sensor*/
        }
      }
      Return (Buffer() {0})
    }
  }
}

Scope(\_SB.PC00.I2C2) {
  Device(PMIC) {
    Name(_ADR, Zero)
    Name(_CID, "INT3472")
    Name(_DDN, "PMIC-CRDG2")
    Name(_UID, "0")
    Name(_PLD, Package (0x01) {

      Buffer(0x14) {
        0x82,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x21,
        0x0D,
        0x00,
        0x00,
        0x03,
        0x00,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF
      }
    })
    Method(_CRS, 0x0, Serialized) {
      Name(SBUF, ResourceTemplate() {
        I2cSerialBus (0x4C, ControllerInitiated, 0x00061A80, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, ,)
      })
      Return (SBUF)
    }
    Method(_STA, 0, NotSerialized) { // _STA: Status
      If (LEqual(SCSS,1))
      {
        Return (0x0F)
      }
      Else
      {
        Return (0x00)
      }
    }
    Method(CLDB, 0, Serialized) {
      Name(PAR, Buffer(0x20) {
        0x00,     //Version
        0x02,     //Control logic Type 0:  UNKNOWN  1: DISCRETE 2: PMIC TPS68470  3: PMIC uP6641
        0x00,     //Control logic ID: Control Logic 0
        0x50,     //CRD board Type, 0: UNKNOWN  0x20: CRD-D  0x30: CRD-G  0x40: PPV 0x50:  CRD-G2
        0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
      })
      Return (PAR)
    }
    Method (_DSM, 4, NotSerialized) { // _DSM: Device-Specific Method
      If(LEqual(Arg0,ToUUID("26257549-9271-4CA4-BB43-C4899D5A4881"))) { /*DsmI2c*/
        If (LEqual (Arg2, Zero)) { /* supported functions */
          Return (0x07) // Functions 1-2 supported
        }
        If (LEqual (Arg2, One)) { /* I2C count */
          Return (0x01)
        }
        If (LEqual (Arg2, 0x02)) { /* I2C 0:bit31-24:BUS. 23-16:Speed.15-8:Addr. 0-7:Function*/
          Return (0x02004C0B) /*TPS68470 PMIC,I2C bus2, Addr:0x4C.Function:pmic*/
        }
      }
      Return (Buffer() {0})
    }
  }
}
