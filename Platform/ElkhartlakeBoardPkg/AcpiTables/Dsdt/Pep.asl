/** @file
  ACPI uPEP Support

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(\_SB.PC00.DPOF)
External(\_SB.PC00.SPIF.SPIS)
External(\_SB.PC00.LPCB.HPET.TCN1)
External(\_SB.PSD0, MethodObj)
External(\_SB.PSD3, MethodObj)
External(SPCO,MethodObj)

If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
  Scope(\_SB.PC00.GFX0) {
    Method(_DEP) {
      Return(Package() {\_SB.PEPD})
    }
  }
  If(LNotEqual(And(PEPC, 0x03), 0)) {
    Scope(\_SB.PC00.SAT0) {
      Method(_DEP) {
      Return(Package() {\_SB.PEPD})
      }
    }
  }
  Scope(\_SB.PC00.I2C0) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C3) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C4) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C5) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C6) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.I2C7) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.SPI0) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.SPI1) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.SPI2) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.UA00) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.UA01) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.UA02) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.HECI) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PC00.XHCI) {
    Method(_DEP) {
      Return(Package() {\_SB.PEPD})
    }
  }
  Scope(\_SB.PC00.HDAS) {
    Method(_DEP) {
      Return(Package() {\_SB.PEPD})
    }
  }
}

Scope (\_SB.PC00.RP01.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PC00.RP02.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PC00.RP03.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PC00.RP04.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PC00.RP05.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PC00.RP06.PXSX)
{
  include("PepPcieSsd.asl")
}

Scope (\_SB.PC00.RP07.PXSX)
{
  include("PepPcieSsd.asl")
}

If(LOr(LEqual(S0ID, 1),LGreaterEqual(OSYS, 2015))){
  Scope(\_SB.PR00) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PR01) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PR02) { Name(_DEP, Package(){\_SB.PEPD}) }
  Scope(\_SB.PR03) { Name(_DEP, Package(){\_SB.PEPD}) }
}

Method(PCID,1){
  Name(RSTG, Package() {0, 0})
  Name(PWRG, Package() {0, 0})
  Store(0x0B08000B, Index(RSTG, 0))
  Store(0, Index(RSTG, 1))
  Store(0, Index(PWRG, 0))
  Store(0, Index(PWRG, 1))
  If(LEqual(Arg0, 0x1)){
    If (LEqual (\_SB.PC00.RP01.VDID, 0xFFFFFFFF)) {
      Return()
    }
    \_SB.PSD0(1)
    \PIN.ON(PWRG)
    Sleep(PEP0)
    SPCO(0, 1)
    \PIN.OFF(RSTG)
    \_SB.PC00.RP01.L23D()
  }
  ElseIf(LEqual(Arg0, 0x0)){
    If (LEqual (\_SB.PC00.RP01.VDID, 0xFFFFFFFF)) {
      Return()
    }
    \_SB.PC00.RP01.DL23()
    \PIN.ON(RSTG)
    \_SB.PSD3(1)
    SPCO(0, 0)
    \PIN.OFF(PWRG)
    \_SB.SHPO(0x0B100002, 0)
  }
}
Scope(\_SB)
{
  Device (PEPD)
  {
    Name (_HID, "INT33A1")
    Name (_CID, EISAID ("PNP0D80"))
    Name (_UID, 0x1)

    Name(PPD0, Package() {"\\_SB.PC00.SAT0",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}})
    Name(PPD3, Package() {"\\_SB.PC00.SAT0",               0x0, Package() {0, Package() {0xFF, 3}}})
    Name(DEVY, Package() // uPEP Device List
    {
      //
      // 1: ACPI Device Descriptor: Fully Qualified name-string
      // 2: Enabled/Disabled Field
      //      0 = This device is disabled and applies no constraints
      //     1+ = This device is enabled and applies constraints
      // 3: Constraint Package: entry per LPI state in LPIT
      //     a. Associated LPI State UID
      //         ID == 0xFF: same constraints apply to all states in LPIT
      //     b: minimum Dx state as pre-condition
      //     c: (optional) OEM specific OEM may provide an additional encoding
      //         which further defines the D-state Constraint
      //            0x0-0x7F - Reserved
      //            0x80-0xFF - OEM defined
      //
      Package() {"\\_SB.PR00",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  0 - SB.PR00
      Package() {"\\_SB.PR01",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  1 - SB.PR01
      Package() {"\\_SB.PR02",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  2 - SB.PR02
      Package() {"\\_SB.PR03",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  3 - SB.PR03
      Package() {"\\_SB.PR04",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  4 - SB.PR04
      Package() {"\\_SB.PR05",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  5 - SB.PR05
      Package() {"\\_SB.PR06",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  6 - SB.PR06
      Package() {"\\_SB.PR07",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  7 - SB.PR07
      Package() {"\\_SB.PR08",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  8 - SB.PR08
      Package() {"\\_SB.PR09",                    0x0, Package() {0, Package() {0xFF, 0}}},                   //  9 - SB.PR09
      Package() {"\\_SB.PR10",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 10 - SB.PR10
      Package() {"\\_SB.PR11",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 11 - SB.PR11
      Package() {"\\_SB.PR12",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 12 - SB.PR12
      Package() {"\\_SB.PR13",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 13 - SB.PR13
      Package() {"\\_SB.PR14",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 14 - SB.PR14
      Package() {"\\_SB.PR15",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 15 - SB.PR15
      Package() {"\\_SB.PC00.GFX0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 16 - Gfx
      Package() {"\\_SB.PC00.SAT0",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 17 - SATA
      Package() {"\\_SB.PC00.HECI",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},             // 18 - CSME
      Package() {"\\_SB.PC00.XHCI",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 19 - XHCI
      Package() {"\\_SB.PC00.HDAS",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},             // 20 - cAVS (HDAudio)
      Package() {"\\_SB.PC00.UA00",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 21 - UART0
      Package() {"\\_SB.PC00.UA01",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 22 - UART1
      Package() {"\\_SB.PC00.UA02",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 23 - UART2
      Package() {"\\_SB.PC00.I2C0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 24 - I2C0
      Package() {"\\_SB.PC00.I2C1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 25 - I2C1
      Package() {"\\_SB.PC00.I2C2",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 26 - I2C2
      Package() {"\\_SB.PC00.I2C3",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 27 - I2C3
      Package() {"\\_SB.PC00.I2C4",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 28 - I2C4
      Package() {"\\_SB.PC00.I2C5",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 29 - I2C5
      Package() {"\\_SB.PC00.I2C6",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 30 - I2C6
      Package() {"\\_SB.PC00.I2C7",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 31 - I2C7
      Package() {"\\_SB.PC00.PEMC",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 32 - EMMC
      Package() {"\\_SB.PC00.PSDC",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 33 - SCS SD Card (PCI Mode)
      Package() {"\\_SB.PC00.SPI0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 34 - SPI0
      Package() {"\\_SB.PC00.SPI1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 35 - SPI1 //No Constraint for SPI1, as it is used for runtime postcodes
      Package() {"\\_SB.PC00.SPI2",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 36 - SPI2
      Package() {"\\_SB.PC00.RP01.PXSX",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 37 - RP01
      Package() {"\\_SB.PC00.RP02.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 38 - RP02
      Package() {"\\_SB.PC00.RP03.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 39 - RP03
      Package() {"\\_SB.PC00.RP04.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 40 - RP04
      Package() {"\\_SB.PC00.RP05.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 41 - RP05
      Package() {"\\_SB.PC00.RP06.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 42 - RP06
      Package() {"\\_SB.PC00.RP07.PXSX",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 43 - RP07
      Package() {"\\_SB.PC00.SAT0.VOL0",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 44 - RAID
      Package() {"\\_SB.PC00.CNVW",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 45 - CNVi WiFi
      Package() {"\\_SB.PC00.THC0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 46 - THC0
      Package() {"\\_SB.PC00.THC1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 47 - THC1
      Package() {"\\_SB.PC00.HDAS.IDA",           0x1, Package() {0, Package() {0xFF, 3}}},                   // 48 - IDA
      Package() {"\\_SB.PC00.OSED",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 49 - PSE LH2OSE IPC
      Package() {"\\_SB.PC00.OTN0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 50 - PSE TSN-0
      Package() {"\\_SB.PC00.OTN1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 51 - PSE TSN-1
      Package() {"\\_SB.PC00.DMA0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 52 - PSE DMA0
      Package() {"\\_SB.PC00.DMA1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 53 - PSE DMA1
      Package() {"\\_SB.PC00.DMA2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 54 - PSE DMA2
      Package() {"\\_SB.PC00.OPWM",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 55 - PSE PWM
      Package() {"\\_SB.PC00.OADC",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 56 - PSE ADC
      Package() {"\\_SB.PC00.OSP0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 57 - PSE SPI-0
      Package() {"\\_SB.PC00.OSP1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 58 - PSE SPI-1
      Package() {"\\_SB.PC00.OSP2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 59 - PSE SPI-2
      Package() {"\\_SB.PC00.OSP3",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 60 - PSE SPI-3
      Package() {"\\_SB.PC00.OIC0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 61 - PSE I2C-0
      Package() {"\\_SB.PC00.OIC1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 62 - PSE I2C-1
      Package() {"\\_SB.PC00.OIC2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 63 - PSE I2C-2
      Package() {"\\_SB.PC00.OIC3",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 64 - PSE I2C-3
      Package() {"\\_SB.PC00.OIC4",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 65 - PSE I2C-4
      Package() {"\\_SB.PC00.OIC5",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 66 - PSE I2C-5
      Package() {"\\_SB.PC00.OIC6",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 67 - PSE I2C-6
      Package() {"\\_SB.PC00.OIC7",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 68 - PSE I2C-7
      Package() {"\\_SB.PC00.OUA0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 69 - PSE UART-0
      Package() {"\\_SB.PC00.OUA1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 70 - PSE UART-1
      Package() {"\\_SB.PC00.OUA2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 71 - PSE UART-2
      Package() {"\\_SB.PC00.OUA3",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 72 - PSE UART-3
      Package() {"\\_SB.PC00.OUA4",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 73 - PSE UART-4
      Package() {"\\_SB.PC00.OUA5",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 74 - PSE UART-5
      Package() {"\\_SB.PC00.OIS0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 75 - PSE I2S-0
      Package() {"\\_SB.PC00.OIS1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 76 - PSE I2S-1
      Package() {"\\_SB.PC00.GP00",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 77 - PSE GPIO-0
      Package() {"\\_SB.PC00.GP01",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 78 - PSE GPIO-1
      Package() {"\\_SB.PC00.CAN0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 79 - PSE CAN-0
      Package() {"\\_SB.PC00.CAN1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 80 - PSE CAN-1
      Package() {"\\_SB.PC00.QEP0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 81 - PSE QEP-0
      Package() {"\\_SB.PC00.QEP1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 82 - PSE QEP-1
      Package() {"\\_SB.PC00.QEP2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 83 - PSE QEP-2
      Package() {"\\_SB.PC00.QEP3",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 84 - PSE QEP-3
      Package() {"\\_SB.PC00.EISI",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 85 - ISI
      Package() {"\\_SB.PC00.GTSN",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 86 - GBETSN
      Package() {"\\_SB.PC00.PUF0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 87 - UFS0
      Package() {"\\_SB.PC00.PUF1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 88 - UFS1
      Package() {"\\_SB.PC00.RP01",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 89 - RP01
      Package() {"\\_SB.PC00.RP07",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 90 - RP07
})

    Name(BCCD, Package() // Bugcheck Critical Device(s)
    {
      //
      // 1: ACPI Device Descriptor: Fully Qualified name string
      // 2: Package of packages: 1 or more specific commands to power up critical device
      //  2a: Package: GAS-structure describing location of PEP accessible power control
      //    Refer to ACPI 5.0 spec section 5.2.3.1 for details
      //    a: Address Space ID (0 = System Memory)
      //       NOTE: A GAS Address Space of 0x7F (FFH) indicates remaining package
      //             elements are Intel defined
      //    b: Register bit width (32 = DWORD)
      //    c: Register bit offset
      //    d: Access size (3 = DWORD Access)
      //    e: Address (for System Memory = 64-bit physical address)
      //  2b: Package containing:
      //    a: AND mask !V not applicable for all Trigger Types
      //    b: Value (bits required to power up the critical device)
      //    c: Trigger Type:
      //         0 = Read
      //         1 = Write
      //         2 = Write followed by Read
      //         3 = Read Modify Write
      //         4 = Read Modify Write followed by Read
      //  2c: Power up delay: Time delay before next operation in uSec
      //
      Package() {"\\_SB.PC00.SAT0", Package() {
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PC00.SAT0.PRT0", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PC00.SAT0.PRT1", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PC00.SAT0.PRT2", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PC00.SAT0.PRT3", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   16000}                        // Power up delay = 16ms
        }
      },
      Package() {"\\_SB.PC00.RP01.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PC00.RP02.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PC00.RP03.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PC00.RP04.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PC00.RP05.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PC00.RP06.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
      Package() {"\\_SB.PC00.RP07.PXSX", Package(){
        Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                   Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                   100000}                       // Power up delay = 100ms
        }
      },
    })

    Method(_STA, 0x0, NotSerialized)
    {
      If(LOr(LGreaterEqual(OSYS,2015), LAnd(LGreaterEqual(OSYS,2012),LEqual(S0ID, 1))))
      {
        Return(0xf)
      }
      Return(0)
    }

    Method(_DSM, 0x4, Serialized)
    {
      If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      {
        // Number of Functions (including this one)
        If(LEqual(Arg2, Zero))
        {
          Return(Buffer(One){0x7F})
        }
        // Device Constraints Enumeration
        If(LEqual(Arg2, One))
        {
          If (LEqual(S0ID, 0)) {
            Return(Package() {})
          }

          // Update uPEP device list based on PEPC (Low Power S0 Constraint)
          // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
          // Bit[2]   - En/Dis UART0
          // Bit[3]   - En/Dis UART1
          // Bit[4]   - En/Dis PCIe NVMe/AHCI
          // Bit[5]   - En/Dis I2C0
          // Bit[6]   - En/Dis I2C1
          // Bit[7]   - En/Dis XHCI
          // Bit[8]   - En/Dis HD Audio (includes ADSP)
          // Bit[9]   - En/Dis Gfx
          // Bit[10]  - En/Dis PR0
          // Bit[11]  - En/Dis EMMC
          // Bit[12]  - En/Dis SDXC
          // Bit[13]  - En/Dis I2C2
          // Bit[14]  - En/Dis I2C3
          // Bit[15]  - En/Dis I2C4
          // Bit[16]  - En/Dis I2C5
          // Bit[17]  - En/Dis UART2
          // Bit[18]  - En/Dis SPI0
          // Bit[19]  - En/Dis SPI1
          // Bit[20]  - En/Dis SPI2
          // Bit[21]  - En/Dis IDA
          // Bit[22]  - En/Dis CSME
          // Bit[23]  - En/Dis I2C6
          // Bit[24]  - En/Dis I2C7
          // Bit[25]  - En/Dis THC0
          // Bit[26]  - En/Dis THC1
          // Bit[27]  - En/Dis ISI
          // Bit[28]  - En/Dis TSN GBE
          // Bit[29]  - En/Dis PSE
          // BIT[30]  - En/Dis UFS0
          // BIT[31]  - En/Dis UFS1

          // Update uPEP device list based on PEPS (Low Power S0 Constraint)
          // Bit[0]   - En/Dis PSE TSN0
          // Bit[1]   - En/Dis PSE TSN1
          // Bit[2]   - En/Dis PSE PWM
          // Bit[3]   - En/Dis PSE ADC
          // Bit[4]   - En/Dis PSE SPI0
          // Bit[5]   - En/Dis PSE SPI1
          // Bit[6]   - En/Dis PSE SPI2
          // Bit[7]   - En/Dis PSE SPI3
          // Bit[8]   - En/Dis PSE I2C0
          // Bit[9]   - En/Dis PSE I2C1
          // Bit[10]  - En/Dis PSE I2C2
          // Bit[11]  - En/Dis PSE I2C3
          // Bit[12]  - En/Dis PSE I2C4
          // Bit[13]  - En/Dis PSE I2C5
          // Bit[14]  - En/Dis PSE I2C6
          // Bit[15]  - En/Dis PSE I2C7
          // Bit[16]  - En/Dis PSE UART0
          // Bit[17]  - En/Dis PSE UART1
          // Bit[18]  - En/Dis PSE UART2
          // Bit[19]  - En/Dis PSE UART3
          // Bit[20]  - En/Dis PSE UART4
          // Bit[21]  - En/Dis PSE UART5
          // Bit[22]  - En/Dis PSE I2S0
          // Bit[23]  - En/Dis PSE I2S1
          // Bit[24]  - En/Dis PSE GPIO 0
          // Bit[25]  - En/Dis PSE GPIO 1
          // Bit[26]  - En/Dis PSE CAN 0
          // Bit[27]  - En/Dis PSE CAN 1
          // Bit[28]  - En/Dis PSE QEP 0
          // Bit[29]  - En/Dis PSE QEP 1
          // Bit[30]  - En/Dis PSE QEP 2
          // Bit[31]  - En/Dis PSE QEP 3


          If(LEqual(And(PEPC, BIT0), 1)) // PEPC Bit[1:0] - Storage (1:Adapter D0/F1 or 3:Adapter D3)
          {
            // Constraint for SATA Controller (SAT0)
            If(LEqual(And(PEPC, Or(BIT1, BIT0)), 1)) // PEPC Bit[1:0] - Storage (1:Adapter D0/F1)
            {
              Store (PPD0, Index (DEVY, 17)) // 17 - SATA Adapter D0
            } ElseIf(LEqual(And(PEPC, Or(BIT1, BIT0)), 3)) // PEPC Bit[1:0] - Storage (3:Adapter D3)
            {
              Store (PPD3, Index (DEVY, 17)) // 17 - SATA Adapter D3
            }
            Store (0x01, Index (DeRefOf(Index (DEVY, 17)), 1)) // Set PEP enabled
          } ElseIf(LEqual(And(PEPC, Or(BIT1, BIT0)), 2)) // PEPC Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 44)), 1)) // 44 - RAID
          }

          If(LEqual(And(PEPC, BIT2), 0)) // PEPC Bit[2] - En/Dis UART0
          {
            // Disabled UA00
            Store (0x00, Index (DeRefOf(Index (DEVY, 21)), 1)) // 21 - UART0
          }

          If(LEqual(And(PEPC, BIT3), 0)) // PEPC Bit[3] - En/Dis UART1
          {
            // Disabled UA01
            Store (0x00, Index (DeRefOf(Index (DEVY, 22)), 1)) // 22 - UART1
          }

          If(LEqual(And(PEPC, BIT4), BIT4)) // PEPC Bit[4] - En/Dis PCIe NVMe/AHCI
          {
            //
            // Only enable PCIe Root Port for PEP if AHCI Controller or NVMe Class Code is connected. _DEP for PCIe would be reported based on OS
            //
            If(LOr(\_SB.PC00.RP01.PXSX.PAHC(), \_SB.PC00.RP01.PXSX.PNVM())){
              //Store (0x00, Index (DeRefOf(Index (DEVY, 37)), 1)) // 37 - RP01.PXSX
              Store (0x01, Index (DeRefOf(Index (DEVY, 89)), 1)) // 89 - RP01
            }
            If(LOr(\_SB.PC00.RP02.PXSX.PAHC(), \_SB.PC00.RP02.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 38)), 1)) // 38 - RP02
            }
            If(LOr(\_SB.PC00.RP03.PXSX.PAHC(), \_SB.PC00.RP03.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 39)), 1)) // 39 - RP03
            }
            If(LOr(\_SB.PC00.RP04.PXSX.PAHC(), \_SB.PC00.RP04.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 40)), 1)) // 40 - RP04
            }
            If(LOr(\_SB.PC00.RP05.PXSX.PAHC(), \_SB.PC00.RP05.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 41)), 1)) // 41 - RP05
            }
            If(LOr(\_SB.PC00.RP06.PXSX.PAHC(), \_SB.PC00.RP06.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 42)), 1)) // 42 - RP06
            }
            If(LOr(\_SB.PC00.RP07.PXSX.PAHC(), \_SB.PC00.RP07.PXSX.PNVM())){
              Store (0x01, Index (DeRefOf(Index (DEVY, 43)), 1)) // 43 - RP07.PXSX

              Store (0x01, Index (DeRefOf(Index (DEVY, 90)), 1)) // 90 - RP07
            }
          }

          If(LEqual(And(PEPC, BIT5), 0)) // PEPC Bit[5] - En/Dis I2C0
          {
            // Disabled I2C0
            Store (0x00, Index (DeRefOf(Index (DEVY, 24)), 1)) // 24 - I2C0
          }

          If(LEqual(And(PEPC, BIT6), 0))  // PEPC Bit[6] - En/Dis I2C1
          {
            // Disabled I2C1
            Store (0x00, Index (DeRefOf(Index (DEVY, 25)), 1)) // 25 - I2C1
          }

          If(LEqual(And(PEPC, BIT7), 0))  // PEPC Bit[7] - En/Dis XHCI
          {
            // Disabled XHCI
            Store (0x00, Index (DeRefOf(Index (DEVY, 19)), 1)) // 19 - XHCI
          }

          If(LEqual(And(PEPC, BIT8), 0)) // PEPC Bit[8] - En/Dis Azalia
          {
            // Disabled Azalia
            Store (0x00, Index (DeRefOf(Index (DEVY, 20)), 1)) // 20 - HD Audio (includes ADSP)
          }

          If(LEqual(And(PEPC, BIT9), 0)) // PEPC Bit[9] - En/Dis Gfx
          {
            // Disabled Gfx
            Store (0x00, Index (DeRefOf(Index (DEVY, 16)), 1)) // 16 - Gfx
          }

          // Apply PEP constraints for CPU,
          // Based on number of threads
          If (LGreaterEqual(TCNT, 1))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 0)), 1))  // 0 - PR00
          }
          If (LGreaterEqual(TCNT, 2))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 1)), 1))  // 1 - PR01
          }
          If (LGreaterEqual(TCNT, 3))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 2)), 1))  // 2 - PR02
          }
          If (LGreaterEqual(TCNT, 4))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 3)), 1))  // 3 - PR03
          }
          If (LGreaterEqual(TCNT, 5))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 4)), 1))  // 4 - PR04
          }
          If (LGreaterEqual(TCNT, 6))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 5)), 1))  // 5 - PR05
          }
          If (LGreaterEqual(TCNT, 7))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 6)), 1))  // 6 - PR06
          }
          If (LGreaterEqual(TCNT, 8))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 7)), 1))  // 7 - PR07
          }
          If (LGreaterEqual(TCNT, 9))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 8)), 1))  // 8 - PR08
          }
          If (LGreaterEqual(TCNT, 10))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 9)), 1))  // 9 - PR09
          }
          If (LGreaterEqual(TCNT, 11))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 10)), 1))  // 10 - PR10
          }
          If (LGreaterEqual(TCNT, 12))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 11)), 1))  // 11 - PR11
          }
          If (LGreaterEqual(TCNT, 13))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 12)), 1))  // 12 - PR12
          }
          If (LGreaterEqual(TCNT, 14))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 13)), 1))  // 13 - PR13
          }
          If (LGreaterEqual(TCNT, 15))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 14)), 1))  // 14 - PR14
          }
          If (LGreaterEqual(TCNT, 16))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 15)), 1))  // 15 - PR15
          }

          If(LEqual(And(PEPC, BIT10), 0)) // PEPC Bit[10] - En/Dis CPU
          {
            // Disabled PEP PR0
            Store (0x00, Index (DeRefOf(Index (DEVY, 0)), 1))   // 0 -  PR00
            Store (0x00, Index (DeRefOf(Index (DEVY, 1)), 1))   // 1 -  PR01
            Store (0x00, Index (DeRefOf(Index (DEVY, 2)), 1))   // 2 -  PR02
            Store (0x00, Index (DeRefOf(Index (DEVY, 3)), 1))   // 3 -  PR03
            Store (0x00, Index (DeRefOf(Index (DEVY, 4)), 1))   // 4 -  PR04
            Store (0x00, Index (DeRefOf(Index (DEVY, 5)), 1))   // 5 -  PR05
            Store (0x00, Index (DeRefOf(Index (DEVY, 6)), 1))   // 6 -  PR06
            Store (0x00, Index (DeRefOf(Index (DEVY, 7)), 1))   // 7 -  PR07
            Store (0x00, Index (DeRefOf(Index (DEVY, 8)), 1))   // 8 -  PR08
            Store (0x00, Index (DeRefOf(Index (DEVY, 9)), 1))   // 9 -  PR09
            Store (0x00, Index (DeRefOf(Index (DEVY, 10)), 1))  // 10 - PR10
            Store (0x00, Index (DeRefOf(Index (DEVY, 11)), 1))  // 11 - PR11
            Store (0x00, Index (DeRefOf(Index (DEVY, 12)), 1))  // 12 - PR12
            Store (0x00, Index (DeRefOf(Index (DEVY, 13)), 1))  // 13 - PR13
            Store (0x00, Index (DeRefOf(Index (DEVY, 14)), 1))  // 14 - PR14
            Store (0x00, Index (DeRefOf(Index (DEVY, 15)), 1))  // 15 - PR15
          }

          If(LOr(LEqual(And(PEPC, BIT11), 0), LEqual(EMCE,0))) // PEPC Bit[11] - En/Dis eMMC
          {
            // Disabled eMMC
            Store (0x00, Index (DeRefOf(Index (DEVY, 32)), 1)) // 32 - EMMC
          }

          If(LOr(LEqual(And(PEPC, BIT12), 0), LEqual(SDCE,0))) // PEPC Bit[12] - En/Dis SDXC
          {
            // Disabled SDXC
            Store (0x00, Index (DeRefOf(Index (DEVY, 33)), 1)) // 33 - SCS SD Card (PCI Mode)
          }

          If(LEqual(And(PEPC, BIT13), 0)) { // PEPC Bit[13] - En/Dis I2C2
            Store (0x00, Index (DeRefOf(Index (DEVY, 26)), 1)) // 26 - I2C2
          }

          If(LEqual(And(PEPC, BIT14),0)) { // PEPC Bit[14] - En/Dis I2C3
            Store (0x00, Index (DeRefOf(Index (DEVY, 27)), 1)) // 27 - I2C3
          }

          If(LEqual(And(PEPC, BIT15), 0)) { // PEPC Bit[15] - En/Dis I2C4
            Store (0x00, Index (DeRefOf(Index (DEVY, 28)), 1)) // 28 - I2C4
          }

          If(LEqual(And(PEPC, BIT16), 0)) { // PEPC Bit[16] - En/Dis I2C5
            Store (0x00, Index (DeRefOf(Index (DEVY, 29)), 1)) // 29 - I2C5
          }

          If(LEqual(And(PEPC, BIT17), 0)) { // PEPC Bit[17] - En/Dis UART2
            Store (0x00, Index (DeRefOf(Index (DEVY, 23)), 1)) // 23 - UART2
          }

          If(LEqual(And(PEPC, BIT18), 0)) { // PEPC Bit[18] - En/Dis SPI0
            Store (0x00, Index (DeRefOf(Index (DEVY, 34)), 1)) // 34 - SPI0
          }

          If(LEqual(And(PEPC, BIT19), 0)) { // PEPC Bit[19] - En/Dis SPI1
            Store (0x00, Index (DeRefOf(Index (DEVY, 35)), 1)) // 35 - SPI1
          }

          If(LEqual(And(PEPC, BIT20), 0)) { // PEPC Bit[20] - En/Dis SPI2
            Store (0x00, Index (DeRefOf(Index (DEVY, 36)), 1)) // 36 - SPI2
          }

          If(LEqual(And(PEPC, BIT21), 0)) { // PEPC Bit[21] - En/Dis IDA
            Store (0x00, Index (DeRefOf(Index (DEVY, 48)), 1)) // 48 - IDA
          }

          If(LEqual(And(PEPC, BIT22), 0)) { // PEPC Bit[22] - En/Dis CSME
            Store (0x00, Index (DeRefOf(Index (DEVY, 18)), 1)) // 18 - CSME
          }

          If(LEqual(And(PEPC, BIT23), 0)) { // PEPC Bit[23] - En/Dis I2C6
            Store (0x00, Index (DeRefOf(Index (DEVY, 30)), 1)) // 30 - I2C6
          }

          If(LEqual(And(PEPC, BIT24), 0)) { // PEPC Bit[24] - En/Dis I2C7
            Store (0x00, Index (DeRefOf(Index (DEVY, 31)), 1)) // 31 - I2C7
          }

          If(LEqual(And(PEPC, BIT25), 0)) { // PEPC Bit[25] - En/Dis THC0
            Store (0x00, Index (DeRefOf(Index (DEVY, 46)), 1)) // 46 - THC0
          }

          If(LEqual(And(PEPC, BIT26), 0)) { // PEPC Bit[26] - En/Dis THC1
            Store (0x00, Index (DeRefOf(Index (DEVY, 47)), 1)) // 47 - THC1
          }

          If(LEqual(And(PEPC, BIT27), 0)) { // PEPC Bit[27] - En/Dis ISI
            Store (0x00, Index (DeRefOf(Index (DEVY, 85)), 1)) // 85 - ISI
          }

          If(LEqual(And(PEPC, BIT28), 0)) { // PEPC Bit[28] - En/Dis GBETSN
            Store (0x00, Index (DeRefOf(Index (DEVY, 86)), 1)) // 86 - GBETSN
          }

          If(LEqual(And(PEPC, BIT29), 0)) { // PEPC Bit[29] - En/Dis PSE
            Store (0x00, Index (DeRefOf(Index (DEVY, 49)), 1)) // 49 - PSE
          }

          If(LEqual(And(PEPC, BIT30), 0)) { // PEPC Bit[30] - En/Dis UFS0
            Store (0x00, Index (DeRefOf(Index (DEVY, 87)), 1)) // 87 - UFS0
          }

          If(LEqual(And(PEPC, BIT31), 0)) { // PEPC Bit[31] - En/Dis UFS1
            Store (0x00, Index (DeRefOf(Index (DEVY, 88)), 1)) // 88 - UFS1
          }
          If(LEqual(And(PEPS, BIT0), 0)) { // PEPS Bit[0] - En/Dis PSE TSN 0
            Store (0x00, Index (DeRefOf(Index (DEVY, 50)), 1)) // 50 - PSE TSN0
          }
          If(LEqual(And(PEPS, BIT1), 0)) { // PEPS Bit[1] - En/Dis PSE TSN1
            Store (0x00, Index (DeRefOf(Index (DEVY, 51)), 1)) // 51 - PSE TSN1
          }

          If(LEqual(And(PEPS, BIT2), 0)) { // PEPS Bit[2] - En/Dis PSE PWM
            Store (0x00, Index (DeRefOf(Index (DEVY, 55)), 1)) // 55 - PSE PWM
          }

          If(LEqual(And(PEPS, BIT3), 0)) { // PEPS Bit[3] - En/Dis PSE ADC
            Store (0x00, Index (DeRefOf(Index (DEVY, 56)), 1)) // 56 - PSE ADC
          }

          If(LEqual(And(PEPS, BIT4), 0)) { // PEPS Bit[4] - En/Dis PSE SPI0
            Store (0x00, Index (DeRefOf(Index (DEVY, 57)), 1)) // 57 - PSE SPI0
          }

          If(LEqual(And(PEPS, BIT5), 0)) { // PEPS Bit[5] - En/Dis PSE SPI1
            Store (0x00, Index (DeRefOf(Index (DEVY, 58)), 1)) // 58 - PSE SPI1
          }

          If(LEqual(And(PEPS, BIT6), 0)) { // PEPS Bit[6] - En/Dis PSE SPI2
            Store (0x00, Index (DeRefOf(Index (DEVY, 59)), 1)) // 59 - PSE SPI2
          }

          If(LEqual(And(PEPS, BIT7), 0)) { // PEPS Bit[7] - En/Dis PSE SPI3
            Store (0x00, Index (DeRefOf(Index (DEVY, 60)), 1)) // 60 - PSE SPI3
          }

          If(LEqual(And(PEPS, BIT8), 0)) { // PEPS Bit[8] - En/Dis PSE I2C0
            Store (0x00, Index (DeRefOf(Index (DEVY, 61)), 1)) // 61 - PSE I2C0
          }

          If(LEqual(And(PEPS, BIT9), 0)) { // PEPS Bit[9] - En/Dis PSE I2C1
            Store (0x00, Index (DeRefOf(Index (DEVY, 62)), 1)) // 62 - PSE I2C1
          }

          If(LEqual(And(PEPS, BIT10), 0)) { // PEPS Bit[10] - En/Dis PSE I2C2
            Store (0x00, Index (DeRefOf(Index (DEVY, 63)), 1)) // 63 - PSE I2c2
          }

          If(LEqual(And(PEPS, BIT11), 0)) { // PEPS Bit[11] - En/Dis PSE I2C3
            Store (0x00, Index (DeRefOf(Index (DEVY, 64)), 1)) // 64 - PSE I2C3
          }

          If(LEqual(And(PEPS, BIT12), 0)) { // PEPS Bit[12] - En/Dis PSE I2C4
            Store (0x00, Index (DeRefOf(Index (DEVY, 65)), 1)) // 65 - PSE I2C4
          }

          If(LEqual(And(PEPS, BIT13), 0)) { // PEPS Bit[13] - En/Dis PSE I2C5
            Store (0x00, Index (DeRefOf(Index (DEVY, 66)), 1)) // 66 - PSE I2c5
          }

          If(LEqual(And(PEPS, BIT14), 0)) { // PEPS Bit[14] - En/Dis PSE I2C6
            Store (0x00, Index (DeRefOf(Index (DEVY, 67)), 1)) // 67 - PSE I2C6
          }

          If(LEqual(And(PEPS, BIT15), 0)) { // PEPS Bit[15] - En/Dis PSE I2C7
            Store (0x00, Index (DeRefOf(Index (DEVY, 68)), 1)) // 68 - PSE I2C7
          }

          If(LEqual(And(PEPS, BIT16), 0)) { // PEPS Bit[16] - En/Dis PSE UART0
            Store (0x00, Index (DeRefOf(Index (DEVY, 69)), 1)) // 69 - PSE UART0
          }

          If(LEqual(And(PEPS, BIT17), 0)) { // PEPS Bit[17] - En/Dis PSE UART1
            Store (0x00, Index (DeRefOf(Index (DEVY, 70)), 1)) // 70 - PSE UART1
          }

          If(LEqual(And(PEPS, BIT18), 0)) { // PEPS Bit[18] - En/Dis PSE UART2
            Store (0x00, Index (DeRefOf(Index (DEVY, 71)), 1)) // 71 - PSE UART2
          }

          If(LEqual(And(PEPS, BIT19), 0)) { // PEPS Bit[19] - En/Dis PSE UART3
            Store (0x00, Index (DeRefOf(Index (DEVY, 72)), 1)) // 72 - PSE UART3
          }

          If(LEqual(And(PEPS, BIT20), 0)) { // PEPS Bit[20] - En/Dis PSE UART4
            Store (0x00, Index (DeRefOf(Index (DEVY, 73)), 1)) // 73 - PSE UART4
          }

          If(LEqual(And(PEPS, BIT21), 0)) { // PEPS Bit[21] - En/Dis PSE UART5
            Store (0x00, Index (DeRefOf(Index (DEVY, 74)), 1)) // 74 - PSE UART5
          }

          If(LEqual(And(PEPS, BIT22), 0)) { // PEPS Bit[22] - En/Dis PSE I2S0
            Store (0x00, Index (DeRefOf(Index (DEVY, 75)), 1)) // 75 - PSE I2S0
          }

          If(LEqual(And(PEPS, BIT23), 0)) { // PEPS Bit[23] - En/Dis PSE I2S1
            Store (0x00, Index (DeRefOf(Index (DEVY, 76)), 1)) // 76 - PSE I2S1
          }

          If(LEqual(And(PEPS, BIT24), 0)) { // PEPS Bit[24] - En/Dis PSE GPIO0
            Store (0x00, Index (DeRefOf(Index (DEVY, 77)), 1)) // 77 - PSE GPIO0
          }

          If(LEqual(And(PEPS, BIT25), 0)) { // PEPS Bit[25] - En/Dis PSE GPIO1
            Store (0x00, Index (DeRefOf(Index (DEVY, 78)), 1)) // 78 - PSE GPIO1
          }

          If(LEqual(And(PEPS, BIT26), 0)) { // PEPS Bit[26] - En/Dis PSE CAN0
            Store (0x00, Index (DeRefOf(Index (DEVY, 79)), 1)) // 79 - PSE CAN0
          }

          If(LEqual(And(PEPS, BIT27), 0)) { // PEPS Bit[27] - En/Dis PSE CAN1
            Store (0x00, Index (DeRefOf(Index (DEVY, 80)), 1)) // 80 - PSE CAN1
          }

          If(LEqual(And(PEPS, BIT28), 0)) { // PEPS Bit[28] - En/Dis PSE QEP0
            Store (0x00, Index (DeRefOf(Index (DEVY, 81)), 1)) // 81 - PSE QEP0
          }

          If(LEqual(And(PEPS, BIT29), 0)) { // PEPS Bit[29] - En/Dis PSE QEP1
            Store (0x00, Index (DeRefOf(Index (DEVY, 82)), 1)) // 82 - PSE QEP1
          }

          If(LEqual(And(PEPS, BIT30), 0)) { // PEPS Bit[30] - En/Dis PSE QEP2
            Store (0x00, Index (DeRefOf(Index (DEVY, 83)), 1)) // 83 - PSE QEP2
          }

          If(LEqual(And(PEPS, BIT31), 0)) { // PEPS Bit[31] - En/Dis PSE QEP3
            Store (0x00, Index (DeRefOf(Index (DEVY, 84)), 1)) // 84 - PSE QEP3
          }

          If(LGreaterEqual(OSYS,2015)){ // Add D3 uPEP constraint for PCIe-based WIFI on Win. Threshold
            If(\_SB.PC00.RP01.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 37)), 2)), 1)), 1)) // 37 - RP01
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 37)), 2)), 1)), 2)) // 37 - RP01
              Store (0x01, Index (DeRefOf(Index (DEVY, 37)), 1)) // 37 - RP01
            }
            If(\_SB.PC00.RP02.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 38)), 2)), 1)), 1)) // 38 - RP02
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 38)), 2)), 1)), 2)) // 38 - RP02
              Store (0x01, Index (DeRefOf(Index (DEVY, 38)), 1)) // 38 - RP02
            }
            If(\_SB.PC00.RP03.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 39)), 2)), 1)), 1)) // 39 - RP03
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 39)), 2)), 1)), 2)) // 39 - RP03
              Store (0x01, Index (DeRefOf(Index (DEVY, 39)), 1)) // 39 - RP03
            }
            If(\_SB.PC00.RP04.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 40)), 2)), 1)), 1)) // 40 - RP04
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 40)), 2)), 1)), 2)) // 40 - RP04
              Store (0x01, Index (DeRefOf(Index (DEVY, 40)), 1)) // 40 - RP04
            }
            If(\_SB.PC00.RP05.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 41)), 2)), 1)), 1)) // 41 - RP05
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 41)), 2)), 1)), 2)) // 41 - RP05
              Store (0x01, Index (DeRefOf(Index (DEVY, 41)), 1)) // 41 - RP05
            }
            If(\_SB.PC00.RP06.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 42)), 2)), 1)), 1)) // 42 - RP06
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 42)), 2)), 1)), 2)) // 42 - RP06
              Store (0x01, Index (DeRefOf(Index (DEVY, 42)), 1)) // 42 - RP06
            }
            If(\_SB.PC00.RP07.PXSX.WIST()){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 43)), 2)), 1)), 1)) // 43 - RP07
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 43)), 2)), 1)), 2)) // 43 - RP07
              Store (0x01, Index (DeRefOf(Index (DEVY, 43)), 1)) // 43 - RP07.PXSX
              //Store (0x00, Index (DeRefOf(Index (DEVY, 90)), 1)) // 90 - RP07
            }
            If(LNotEqual(\_SB.PC00.CNVW.VDID, 0xFFFFFFFF)){
              Store (3, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 45)), 2)), 1)), 1)) // 45 - CNVi- WiFI
              Store (0, Index(DeRefOf(Index(DeRefOf(Index (DeRefOf(Index (DEVY, 45)), 2)), 1)), 2)) // 45 - CNVi- WiFI
              Store (0x01, Index (DeRefOf(Index (DEVY, 45)), 1)) // 45 - CNVi- WiFI
            }
          }

          Return(DEVY)
        }

        // BCCD
        If(LEqual(Arg2, 2))
        {
          Return(BCCD)
        }

        // Screen off notification
        If(LEqual(Arg2, 0x3))
        {
          // no action for screen off
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            D8XH (0, 0xC5)
            D8XH (1, 0)
          }
        }

        // Screen on notification
        If(LEqual(Arg2, 0x4))
        {
          // no action for screen on
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            D8XH (0, 0xC5)
            D8XH (1, 0xAB)
          }
        }

        // resiliency phase entry (deep standby entry)
        If(LEqual(Arg2, 0x5))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \_SB.PC00.SPIF.SPIS() // Clear SPI Synchronous SMI Status bit
            Store(0x0000000000000000, \_SB.PC00.LPCB.HPET.TCN1)
            PCID(0)
            \GUAM(1) // 0x01 - Power State Standby (CS Resiliency Entry)
          }
        }

        // resiliency phase exit (deep standby exit)
        If(LEqual(Arg2, 0x6))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            PCID(1)
            \GUAM(0) // 0x00 - Power State On (CS Resiliency Exit)
          }
        }
      }// If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      Return (Buffer() {0})
    } // Method(_DSM)
  } //device (PEPD)
} // End Scope(\_SB)
