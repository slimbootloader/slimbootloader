/** @file
  ACPI uPEP Support

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/PmcRegs.h>
#include <Register/TcoRegs.h>
#include <Rtd3Defines.h>

External (\_SB.PC00.LPCB.NTIR, MethodObj)
External(\_SB.PC00.DPOF)
External(\_SB.PC00.LPCB.H_EC.ECNT, MethodObj)
External(\_SB.PC00.LPCB.HPET.TCN1)
External(\_SB.PC00.SPIF.SPIS)
External(SPCO,MethodObj)
External(\_SB.PC00.RP01.DL23, MethodObj)
External(\_SB.PC00.RP01.L23D, MethodObj)
External(\_SB.PC00.RP09.DL23, MethodObj)
External(\_SB.PC00.RP09.L23D, MethodObj)
External(\_SB.PC00.RP10.DL23, MethodObj)
External(\_SB.PC00.RP10.L23D, MethodObj)
External(\_SB.PC00.RP11.DL23, MethodObj)
External(\_SB.PC00.RP11.L23D, MethodObj)
External (TMCS, IntObj)

External(THCE) // TCSS XHCI Device Enable
External(TDCE) // TCSS XDCI Device Enable
External(TRE0) // TCSS ItbtPcie PCIE RP 0 Device Enable
External(TRE1) // TCSS ItbtPcie PCIE RP 1 Device Enable
External(TRE2) // TCSS ItbtPcie PCIE RP 2 Device Enable
External(TRE3) // TCSS ItbtPcie PCIE RP 3 Device Enable
External(DME0) // TCSS ITBT DMA0
External(DME1) // TCSS ITBT DMA1
External (\PSON) // Indicates if PS_ON is enabled
External(\_SB.PC00.XHCI.PSLI, MethodObj) // PD PS_ON Low Power S0 idle Method

External(\_SB.PC00.TCON, MethodObj)
External(\_SB.PC00.TDM0.SCME)
External(\_SB.PC00.TDM1.SCME)
External(\_SB.PC00.TDM0._STA)
External(\_SB.PC00.TDM1._STA)

Scope (\_SB.PC00.RP01.PXSX)
{
  Name (PERN, "RP01") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP02.PXSX)
{
  Name (PERN, "RP02") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP03.PXSX)
{
  Name (PERN, "RP03") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP04.PXSX)
{
  Name (PERN, "RP04") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP05.PXSX)
{
  Name (PERN, "RP05") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP06.PXSX)
{
  Name (PERN, "RP06") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP07.PXSX)
{
  Name (PERN, "RP07") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP08.PXSX)
{
  Name (PERN, "RP08") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP09.PXSX)
{
  Name (PERN, "RP09") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP10.PXSX)
{
  Name (PERN, "RP10") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP11.PXSX)
{
  Name (PERN, "RP11") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP12.PXSX)
{
  Name (PERN, "RP12") // PERN - PCIe  Root Port Name
  include("PepPcieEp.asl")
}

#define PEP_ENABLE_CONSTRAINT_AT_INDEX(ConstraintPackage, ConstraintIndex) Store (0x01, Index (DeRefOf(Index (ConstraintPackage, ConstraintIndex)), 1))
Scope(\_SB)
{
  Device (PEPD)
  {
    Name (_HID, "INT33A1")
    Name (_CID, EISAID ("PNP0D80"))
    Name (_UID, 0x1)
    Name (LBUF, Buffer(V_PMC_PWRM_LPM_REQ_DATA_LEN) {})

    Name (PPD0, Package () {"\\_SB.PC00.SAT0",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}})
    Name (PPD3, Package () {"\\_SB.PC00.SAT0",               0x0, Package () {0, Package () {0xFF, 3}}})
    Name (WWD3, Package () {"\\_SB.PC00.RP07",               0x0, Package () {0, Package () {0xFF, 3}}})

    Name (PKD0, Package () {0, Package () {0xFF, 0, 0x81}})
    Name (PKD3, Package () {0, Package () {0xFF, 3}})
    Name (DEVY, Package () // uPEP Device List
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
      Package () {"\\_SB.PR00",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  0 - SB.PR00
      Package () {"\\_SB.PR01",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  1 - SB.PR01
      Package () {"\\_SB.PR02",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  2 - SB.PR02
      Package () {"\\_SB.PR03",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  3 - SB.PR03
      Package () {"\\_SB.PR04",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  4 - SB.PR04
      Package () {"\\_SB.PR05",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  5 - SB.PR05
      Package () {"\\_SB.PR06",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  6 - SB.PR06
      Package () {"\\_SB.PR07",                    0x0, Package () {0, Package () {0xFF, 0}}},                   //  7 - SB.PR07
      Package () {"\\_SB.PC00.GFX0",               0x1, Package () {0, Package () {0xFF, 3}}},                   //  8 - Gfx
      Package () {"\\_SB.PC00.SAT0",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             //  9 - SATA
      Package () {"\\_SB.PC00.UA00",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 10 - UART0
      Package () {"\\_SB.PC00.UA01",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 11 - UART1
      Package () {"\\_SB.PC00.I2C0",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 12 - I2C0
      Package () {"\\_SB.PC00.I2C1",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 13 - I2C1
      Package () {"\\_SB.PC00.XHCI",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 14 - XHCI
      Package () {"\\_SB.PC00.HDAS",               0x1, Package () {0, Package () {0xFF, 0, 0x81}}},             // 15 - cAVS (HDAudio)
      Package () {"\\_SB.PC00.PEMC",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 16 - EMMC
      Package () {"\\_SB.PC00.PSDC",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 17 - SCS SD Card (PCI Mode)
      Package () {"\\_SB.PC00.I2C2",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 18 - I2C2
      Package () {"\\_SB.PC00.I2C3",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 19 - I2C3
      Package () {"\\_SB.PC00.I2C4",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 20 - I2C4
      Package () {"\\_SB.PC00.I2C5",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 21 - I2C5
      Package () {"\\_SB.PC00.UA02",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 22 - UART2
      Package () {"\\_SB.PC00.SPI0",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 23 - SPI0
      Package () {"\\_SB.PC00.SPI1",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 24 - SPI1
      Package () {"\\_SB.PC00.RP01",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 25 - RP01
      Package () {"\\_SB.PC00.RP02",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 26 - RP02
      Package () {"\\_SB.PC00.RP03",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 27 - RP03
      Package () {"\\_SB.PC00.RP04",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 28 - RP04
      Package () {"\\_SB.PC00.RP05",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 29 - RP05
      Package () {"\\_SB.PC00.RP06",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 30 - RP06
      Package () {"\\_SB.PC00.RP07",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 31 - RP07
      Package () {"\\_SB.PC00.RP08",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 32 - RP08
      Package () {"\\_SB.PC00.RP09",               0x0, Package () {0, Package () {0xFF, 3}}},                   // 33 - RP09
      Package () {"\\_SB.PC00.RP10",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 34 - RP10
      Package () {"\\_SB.PC00.RP11",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 35 - RP11
      Package () {"\\_SB.PC00.RP12",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 36 - RP12
      Package () {"\\_SB.PC00.RP13",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 37 - RP13
      Package () {"\\_SB.PC00.RP14",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 38 - RP14
      Package () {"\\_SB.PC00.RP15",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 39 - RP15
      Package () {"\\_SB.PC00.RP16",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 40 - RP16
      Package () {"\\_SB.PC00.RP17",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 41 - RP17
      Package () {"\\_SB.PC00.RP18",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 42 - RP18
      Package () {"\\_SB.PC00.RP19",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 43 - RP19
      Package () {"\\_SB.PC00.RP20",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 44 - RP20
      Package () {"\\_SB.PC00.SAT0.VOL0",          0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 45 - RAID
      Package () {"\\_SB.PR08",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 46 - SB.PR08
      Package () {"\\_SB.PR09",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 47 - SB.PR09
      Package () {"\\_SB.PR10",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 48 - SB.PR10
      Package () {"\\_SB.PR11",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 49 - SB.PR11
      Package () {"\\_SB.PR12",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 50 - SB.PR12
      Package () {"\\_SB.PR13",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 51 - SB.PR13
      Package () {"\\_SB.PR14",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 52 - SB.PR14
      Package () {"\\_SB.PR15",                    0x0, Package () {0, Package () {0xFF, 0}}},                   // 53 - SB.PR15
      Package () {"\\_SB.PC00.SPI2",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 54 - SPI2
      Package () {"\\_SB.PC00.CNVW",               0x0, Package () {0, Package () {0xFF, 0, 0x81}}},             // 55 - CNVi WiFi
      Package () {"\\_SB.PC00.IPU0",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 56 - IPU0
      Package () {"\\_SB.PC00.HECI",               0x1, Package () {0, Package () {0xFF, 0, 0x81}}},             // 57 - CSME
      Package () {"\\_SB.PC00.GLAN",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 58 - LAN (GBE)
      Package () {"\\_SB.PC00.THC0",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 59 - THC0
      Package () {"\\_SB.PC00.THC1",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 60 - THC1
      Package () {"Depreciate IDA",                0x0, Package () {0, Package () {0xFF, 3}}},                   // 61 - Reserved
      Package () {"USB\\VID_8087&PID_0AC9&MI*",    0x0, Package () {0, Package () {0xFF, 2}}},                   // 62 - CDC MBIM
      Package () {"\\_SB.PC00.I2C6",               0x1, Package () {0, Package () {0xFF, 3}}},                   // 63 - I2C6
      Package() {"\\_SB.PR16",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 64 - SB.PR16
      Package() {"\\_SB.PR17",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 65 - SB.PR17
      Package() {"\\_SB.PR18",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 66 - SB.PR18
      Package() {"\\_SB.PR19",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 67 - SB.PR19
      Package() {"\\_SB.PR20",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 68 - SB.PR20
      Package() {"\\_SB.PR21",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 69- SB.PR21
      Package() {"\\_SB.PR22",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 70 - SB.PR22
      Package() {"\\_SB.PR23",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 71 - SB.PR23
      Package() {"\\_SB.PC00.GNA0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 72 - GNA0
      Package() {"\\_SB.PC00.VMD0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 73 - VMD0
      Package() {"\\_SB.PC00.RP21",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 74 - RP21
      Package() {"\\_SB.PC00.RP22",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 75 - RP22
      Package() {"\\_SB.PC00.RP23",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 76 - RP23
      Package() {"\\_SB.PC00.RP24",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 77 - RP24
      Package() {"\\_SB.PC00.SAT0.PRT0",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 78 - PRT0
      Package() {"\\_SB.PC00.SAT0.PRT1",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 79 - PRT1
      Package() {"\\_SB.PC00.SAT0.PRT2",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 80 - PRT2
      Package() {"\\_SB.PC00.SAT0.PRT3",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 81 - PRT3
      Package() {"\\_SB.PC00.SAT0.PRT4",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 82 - PRT4
      Package() {"\\_SB.PC00.SAT0.PRT5",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 83 - PRT5
      Package() {"\\_SB.PC00.SAT0.PRT6",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 84 - PRT6
      Package() {"\\_SB.PC00.SAT0.PRT7",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 85 - PRT7
      Package() {"\\_SB.PC00.HEC3",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 86 - HEC3
      Package() {"\\_SB.PR24",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 87 - SB.PR24
      Package() {"\\_SB.PR25",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 88 - SB.PR25
      Package() {"\\_SB.PR26",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 89 - SB.PR26
      Package() {"\\_SB.PR27",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 90 - SB.PR27
      Package() {"\\_SB.PR28",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 91 - SB.PR28
      Package() {"\\_SB.PR29",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 92 - SB.PR29
      Package() {"\\_SB.PR30",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 93 - SB.PR30
      Package() {"\\_SB.PR31",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 94 - SB.PR31
      Package() {"\\_SB.PC00.PUF0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 95 - UFS0
      Package() {"\\_SB.PC00.PUF1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 96 - UFS1
      Package() {"\\_SB.PC00.I2C7",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 97 - I2C7
      Package() {"\\_SB.PC00.TXHC",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 98 - TCSS XHCI
      Package() {"\\_SB.PC00.TXDC",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 99 - TCSS XDCI
      Package() {"\\_SB.PC00.TRP0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 100 - TCSS ITBT PCIe RP0 on Segment0
      Package() {"\\_SB.PC00.TRP1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 101 - TCSS ITBT PCIe RP1 on Segment0
      Package() {"\\_SB.PC00.TRP2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 102 - TCSS ITBT PCIe RP2 on Segment0
      Package() {"\\_SB.PC00.TRP3",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 103 - TCSS ITBT PCIe RP3 on Segment0
      Package() {"\\_SB.PC01.TRP0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 104 - TCSS ITBT PCIe RP0 on Segment1
      Package() {"\\_SB.PC01.TRP1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 105 - TCSS ITBT PCIe RP1 on Segment1
      Package() {"\\_SB.PC01.TRP2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 106 - TCSS ITBT PCIe RP2 on Segment1
      Package() {"\\_SB.PC01.TRP3",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 107 - TCSS ITBT PCIe RP3 on Segment1
      Package() {"\\_SB.PC00.TDM0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 108 - TCSS DMA0
      Package() {"\\_SB.PC00.TDM1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 109 - TCSS DMA1
      Package() {"\\_SB.PC00.VPU0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 110 - VPU0
      Package() {"\\_SB.PR32",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 111 - SB.PR32
      Package() {"\\_SB.PR33",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 112 - SB.PR33
      Package() {"\\_SB.PR34",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 113 - SB.PR34
      Package() {"\\_SB.PR35",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 114 - SB.PR35
      Package() {"\\_SB.PR36",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 115 - SB.PR36
      Package() {"\\_SB.PR37",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 116 - SB.PR37
      Package() {"\\_SB.PR38",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 117 - SB.PR38
      Package() {"\\_SB.PR39",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 118 - SB.PR39
      Package() {"\\_SB.PR40",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 119 - SB.PR40
      Package() {"\\_SB.PR41",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 120 - SB.PR41
      Package() {"\\_SB.PR42",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 121 - SB.PR42
      Package() {"\\_SB.PR43",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 122 - SB.PR43
      Package() {"\\_SB.PR44",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 123 - SB.PR44
      Package() {"\\_SB.PR45",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 124 - SB.PR45
      Package() {"\\_SB.PR46",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 125 - SB.PR46
      Package() {"\\_SB.PR47",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 126 - SB.PR47
    })

    // Device Constraint Enable Disable(DCED)
    // Arg0 : String, device which we want to search (For Ex: \\_SB.PC00.TDM1 )
    // Arg1 : Constraint Enable/Disable
    Method(DCED,2, NotSerialized){
       Local0 = 0 // Index
       Local2 = SizeOf(DEVY)
       Local3 = 0
       While(Local0 < Local2){
         Store (DerefOf (Index (DeRefOf (Index(DEVY, Local0)),0)),Local1)
         if(LEqual(Arg0,Local1))
           {
             Store (Arg1, Index (DeRefOf (Index (DEVY, Local0)), 1))
             Local3 = 1
             Break
           }
         Increment ( Local0 )
       }
    }

    //
    // Get User Configured PEP Constraint
    // Arguments: (2)
    //    Arg0 - low bit index in PEPC
    //    Arg1 - high bit index in PEPC
    // Returns: 0 ->no constraint; 1->D0 F1; 3->D3
    //
    Method (GUCP, 2, Serialized) {
      If (LEqual (And (PEPC, Arg0), Arg0)) {
        If (LEqual (And (PEPC, Or (Arg1, Arg0)), Or (Arg1, Arg0))) {
          Return (3)
        } Else {
          Return (1)
        }
      } Else {
        Return (0)
      }
    }

    //
    // Get PEP Constraints of PCIe Root Port of specific EP type
    // Arguments: (1)
    //    Arg0 - PCIE EP Device type
    //         0->Other
    //         1->Storage
    //         2->LAN - PCH
    //         3->WLAN - PCH
    //         4->GFX - CPU, PCH
    //         5->DTBT - PCH
    //         6->WWAN - PCH
    //         99->Invalid (EP absent)
    // Returns: PeP constrant per EP type
    //         0 ->no constraint; 1->D0 F1; 3->D3
    //
    Method (GRPC, 1, Serialized) {
      If (LEqual (Arg0, 1)) {
        Return (GUCP (BIT0 << PEP_PCIE_STORAGE_CONS_INDEX, BIT1 << PEP_PCIE_STORAGE_CONS_INDEX))
      }

      If (LEqual (Arg0, 2)) {
        Return (GUCP (BIT0 << PEP_PCIE_LAN_CONS_INDEX, BIT1 << PEP_PCIE_LAN_CONS_INDEX))
      }

      If (LEqual (Arg0, 3)) {
        Return (GUCP (BIT0 << PEP_PCIE_WLAN_CONS_INDEX, BIT1 << PEP_PCIE_WLAN_CONS_INDEX))
      }

      If (LEqual (Arg0, 4)) {
        Return (GUCP (BIT0 << PEP_PCIE_GFX_CONS_INDEX, BIT1 << PEP_PCIE_GFX_CONS_INDEX))
      }

      If (LEqual (Arg0, 5)) {
        Return (GUCP (BIT0 << PEP_TCSS_CONS_INDEX, BIT0 << PEP_TCSS_CONS_INDEX))
      }

      If (LEqual (Arg0, 6)) {
        If (LEqual (WRTO, 3)) {
          Return (3)
        } Else {
          If (LEqual (WRTO, 1)) { // if WRTO is 1, then Put D2 constraint on CDC MBIM
            DSDF (WRTO, 62) // 62 - CDC MBIM
          }
          Return (0) // if WRTO is not 3, then PCIR RP Constraint should be disable
        }
      }

      If(LEqual (Arg0, 0)) {
        Return(GUCP(BIT0 << PEP_PCIE_OTHER_CONS_INDEX, BIT1 << PEP_PCIE_OTHER_CONS_INDEX))
      }
      Return (0)
    }

    //
    // Enable uPEP constraint and set a D-state Constraint for the specified offset DEVY entry
    //
    // Arguments: (2)
    //   Arg0 - Constraint to Apply(0->no constraint; 1->D0 F1 constraint; 3: D3 F0 constraint)
    //   Arg1 -  Index of entry in uPEP Device List
    // Return Value:
    //   None
    //
    Method(DSDF, 2, Serialized) {
      If (LNotEqual (Arg1, Ones)) {
        Store (Arg1, Local0)
        If (LEqual (Arg0, 0)) {
          Store (0x00, Index (DeRefOf(Index (\_SB.PEPD.DEVY, Local0)), 1)) // Disable the Entry PEP Constraint
        } Else {
          If (LEqual (Arg0, 3)) {
            Store (PKD3, Index (DeRefOf(Index (\_SB.PEPD.DEVY, Local0)), 2)) // PEP Constraint Set To D3
          }

          If (LEqual (Arg0, 1)) {
            Store (PKD0, Index (DeRefOf(Index (\_SB.PEPD.DEVY, Local0)), 2)) // PEP Constraint Set To D0/F1
          }

          Store (0x01, Index (DeRefOf(Index (\_SB.PEPD.DEVY, Local0)), 1)) // Enable the Entry PEP Constraint
        }
      }
    }

    //
    // Get the offset of the given object in package - \_SB.PEPD.DEVY
    //
    // Arguments: (1)
    //   Arg0 - The string looking for
    // Return: The offset of the Arg0 string object
    //            Ones return if Arg0 string not found
    //
    Method (PARS, 1, Serialized) {
      Store (SizeOf (\_SB.PEPD.DEVY), Local0)//how many entries to worry about?
      Store (0, Local1) //use this as index value
      While (LGreater(Local0, Local1)) {
        Store (Derefof(Index (\_SB.PEPD.DEVY, Local1)), Local2) //grab the first package
        Store (Derefof(Index (Local2, 0)), Local3) //grab the first object inside package
        If (LEqual(Local3, Arg0)) { //check to see if object matches string
          Return(Local1) //return the index of the object
        }
        Local1++
      }
      Return (Ones)
    }

    //
    // Configure pep Constraint of PCH RP01~RP28 and CPU PEG0~PEG2
    //
    // Arguments: (0)
    // Return Value:
    //   None
    //
    Method(RPPC, 0, Serialized) {
      DSDF(GRPC(\_SB.PC00.RP01.PXSX.GRPT()), PARS("\\_SB.PC00.RP01"))
      DSDF(GRPC(\_SB.PC00.RP02.PXSX.GRPT()), PARS("\\_SB.PC00.RP02"))
      DSDF(GRPC(\_SB.PC00.RP03.PXSX.GRPT()), PARS("\\_SB.PC00.RP03"))
      DSDF(GRPC(\_SB.PC00.RP04.PXSX.GRPT()), PARS("\\_SB.PC00.RP04"))
      DSDF(GRPC(\_SB.PC00.RP05.PXSX.GRPT()), PARS("\\_SB.PC00.RP05"))
      DSDF(GRPC(\_SB.PC00.RP06.PXSX.GRPT()), PARS("\\_SB.PC00.RP06"))
      DSDF(GRPC(\_SB.PC00.RP07.PXSX.GRPT()), PARS("\\_SB.PC00.RP07"))
      DSDF(GRPC(\_SB.PC00.RP08.PXSX.GRPT()), PARS("\\_SB.PC00.RP08"))
      DSDF(GRPC(\_SB.PC00.RP09.PXSX.GRPT()), PARS("\\_SB.PC00.RP09"))
      DSDF(GRPC(\_SB.PC00.RP10.PXSX.GRPT()), PARS("\\_SB.PC00.RP10"))
      DSDF(GRPC(\_SB.PC00.RP11.PXSX.GRPT()), PARS("\\_SB.PC00.RP11"))
      DSDF(GRPC(\_SB.PC00.RP12.PXSX.GRPT()), PARS("\\_SB.PC00.RP12"))
    }

    Method(SPDE, 0, Serialized) {
      External (\ESPC) // Indicates if constraints should be enumerated
      //
      // Below methods check for drive presence in SATA port
      //
      External (\_SB.PC00.SAT0.PRT0.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT1.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT2.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT3.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT4.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT5.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT6.PRES, MethodObj)
      External (\_SB.PC00.SAT0.PRT7.PRES, MethodObj)

      If (LNot(\ESPC)) {
        Return ()
      }

      If (CondRefOf (\_SB.PC00.SAT0)) {
        DCED("\\_SB.PC00.SAT0.PRT0", \_SB.PC00.SAT0.PRT0.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT1", \_SB.PC00.SAT0.PRT1.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT2", \_SB.PC00.SAT0.PRT2.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT3", \_SB.PC00.SAT0.PRT3.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT4", \_SB.PC00.SAT0.PRT4.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT5", \_SB.PC00.SAT0.PRT5.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT6", \_SB.PC00.SAT0.PRT6.PRES ())
        DCED("\\_SB.PC00.SAT0.PRT7", \_SB.PC00.SAT0.PRT7.PRES ())
      }
    }

    Name(BCCD, Package() // Bugcheck Critical Device(s)
    {  })

    //
    // _DSD to notify OSPM about CEC PS_ON support.
    //
    Name (_DSD, Package (0x02) { // Using Name object to be compatible for different OSPM
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package (0x01) {
        Package (0x02) {
          "intel-cec-pson-switching-enabled-in-s0",
          0 // to be updated in PSOP
        }
      }
    })

    Method (PSOP, 0, NotSerialized) { // PSON Pep Check
      //
      // Loop through _DSD object to find index for UUID of interest.
      // Stride length is 2 because list of properties comes right after the corresponding UUID.
      //
      For (Local0 = Zero, Local0 < SizeOf(_DSD), Local0 += 2) {
        If (LNotEqual(DerefOf(_DSD[Local0]), ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"))) {
          // skip if not the uuid of interest
          Continue
        }
        // get the package associated with this UUID
        Local1 = DerefOf (_DSD[Add(Local0, 1)])
        For (Local2 = Zero, Local2 < SizeOf(Local1), Increment(Local2)) {
          // get the key/value pair at the index
          Local3 = DerefOf(Local1[Local2])
          If (LAnd(LEqual("intel-cec-pson-switching-enabled-in-s0", DerefOf(Local3[Zero])), PSON)) {
            DerefOf(DerefOf(_DSD[Local0 + 1])[Local2])[One] = One
          }
        }
      }
      Return
    }

    Method(_STA, 0x0, NotSerialized)
    {
      If (LEqual (S0ID, 1))
      {
        PSOP () // Update _DSD
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

          // En/Dis BIT[0] UART0
          // En/Dis BIT[1] UART1
          // En/Dis BIT[2] UART2
          // En/Dis BIT[3] I2C0
          // En/Dis BIT[4] I2C1
          // En/Dis BIT[5] I2C2
          // En/Dis BIT[6] I2C3
          // En/Dis BIT[7] I2C4
          // En/Dis BIT[8] I2C5
          // En/Dis BIT[9] I2C6
          // En/Dis BIT[10] I2C7
          // En/Dis BIT[11] SPI0
          // En/Dis BIT[12] SPI1
          // En/Dis BIT[13] SPI2
          // En/Dis BIT[14] XHCI
          // En/Dis BIT[15] GFX
          // En/Dis BIT[16] CPU
          // En/Dis BIT[17] IPU0
          // En/Dis BIT[18] CSME
          // En/Dis BIT[19] LAN(GBE)
          // En/Dis BIT[20] THC0
          // En/Dis BIT[21] THC1
          // En/Dis BIT[22] TCSS
          // En/Dis BIT[23] GNA0
          // En/Dis BIT[24] VMD
          // En/Dis BIT[25] HECI3
          // BIT[26:27] HD Audio (includes ADSP) (0:No Constraint or 1: D0/F1 or 3:D3)
          // BIT[28:29] Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
          // BIT[30:31] PCIE Storage RP (0:No Constraint or 1: D0/F1 or 3:D3)
          // BIT[32:33] Pcie Lan (0:No Constraint or 1:D0/F1 or 3:D3)
          // BIT[34:35] Pcie Wlan (0:No Constraint or 1:D0/F1 or 3:D3)
          // BIT[36:37] Pcie Gfx (0:No Constraint or 1:D0/F1 or 3:D3)
          // BIT[38:39] Pcie Other (0:No Constraint or 1:D0/F1 or 3:D3)
          // En/Dis BIT[40] EMMC
          // En/Dis BIT[41] SDXC
          // Bit[42]  - En/Dis VPU0
          // Bit[43]  - En/Dis UFS0
          // Bit[44]  - En/Dis UFS1

          //
          // SATA port drive constraint enumeration
          //
          SPDE ()

          If (LEqual (And (PEPC, BIT0 << PEP_SATA_CONS_INDEX) >> PEP_SATA_CONS_INDEX, 1)) // Storage (1:Adapter D0/F1 or 3:Adapter D3)
          {
            // Constraint for SATA Controller (SAT0)
            If (LEqual (And (PEPC, Or (BIT1 << PEP_SATA_CONS_INDEX, BIT0 << PEP_SATA_CONS_INDEX)) >> PEP_SATA_CONS_INDEX, 1)) // Storage (1:Adapter D0/F1)
            {
              Store (PPD0, Index (DEVY, 9)) // 9 - SATA Adapter D0
            } ElseIf (LEqual (And (PEPC, Or (BIT1 << PEP_SATA_CONS_INDEX, BIT0 << PEP_SATA_CONS_INDEX)) >> PEP_SATA_CONS_INDEX, 3)) // Storage (3:Adapter D3)
            {
              Store (PPD3, Index (DEVY, 9)) // 9 - SATA Adapter D3
            }
            DCED("\\_SB.PC00.SAT0",0x01) // Set PEP enabled
          } ElseIf (LEqual (And (PEPC, Or (BIT1 << PEP_SATA_CONS_INDEX, BIT0 << PEP_SATA_CONS_INDEX)) >> PEP_SATA_CONS_INDEX, 2)) // Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 45)), 1)) // 45 - RAID
          }

          //Apply all the PCIe root ports constraint based on user choice
          RPPC()

          DCED("\\_SB.PC00.UA00",And (PEPC, BIT0 << PEP_UART0_CONS_INDEX) >> PEP_UART0_CONS_INDEX)
          DCED("\\_SB.PC00.UA01",And (PEPC, BIT0 << PEP_UART1_CONS_INDEX) >> PEP_UART1_CONS_INDEX)
          DCED("\\_SB.PC00.UA02",And (PEPC, BIT0 << PEP_UART2_CONS_INDEX) >> PEP_UART2_CONS_INDEX)
          DCED("\\_SB.PC00.XHCI",And (PEPC, BIT0 << PEP_XHCI_CONS_INDEX) >> PEP_XHCI_CONS_INDEX)
          DCED("\\_SB.PC00.I2C0",And (PEPC, BIT0 << PEP_I2C0_CONS_INDEX) >> PEP_I2C0_CONS_INDEX)
          DCED("\\_SB.PC00.I2C1",And (PEPC, BIT0 << PEP_I2C1_CONS_INDEX) >> PEP_I2C1_CONS_INDEX)
          DCED("\\_SB.PC00.I2C2",And (PEPC, BIT0 << PEP_I2C2_CONS_INDEX) >> PEP_I2C2_CONS_INDEX)
          DCED("\\_SB.PC00.I2C3",And (PEPC, BIT0 << PEP_I2C3_CONS_INDEX) >> PEP_I2C3_CONS_INDEX)
          DCED("\\_SB.PC00.I2C4",And (PEPC, BIT0 << PEP_I2C4_CONS_INDEX) >> PEP_I2C4_CONS_INDEX)
          DCED("\\_SB.PC00.I2C5",And (PEPC, BIT0 << PEP_I2C5_CONS_INDEX) >> PEP_I2C5_CONS_INDEX)
          DCED("\\_SB.PC00.I2C6",And (PEPC, BIT0 << PEP_I2C6_CONS_INDEX) >> PEP_I2C6_CONS_INDEX)
          DCED("\\_SB.PC00.I2C7",And (PEPC, BIT0 << PEP_I2C7_CONS_INDEX) >> PEP_I2C7_CONS_INDEX)
          DCED("\\_SB.PC00.SPI0",And (PEPC, BIT0 << PEP_SPI0_CONS_INDEX) >> PEP_SPI0_CONS_INDEX)
          DCED("\\_SB.PC00.SPI1",And (PEPC, BIT0 << PEP_SPI1_CONS_INDEX) >> PEP_SPI1_CONS_INDEX)
          DCED("\\_SB.PC00.SPI2",And (PEPC, BIT0 << PEP_SPI2_CONS_INDEX) >> PEP_SPI2_CONS_INDEX)
          DCED("\\_SB.PC00.VPU0",And (PEPC, BIT0 << PEP_VPU0_CONS_INDEX) >> PEP_VPU0_CONS_INDEX)
          DCED("\\_SB.PC00.PUF0",And (PEPC, BIT0 << PEP_UFS0_CONS_INDEX) >> PEP_UFS0_CONS_INDEX)
          DCED("\\_SB.PC00.PUF1",And (PEPC, BIT0 << PEP_UFS1_CONS_INDEX) >> PEP_UFS1_CONS_INDEX)
          DCED("\\_SB.PC00.PEMC",And (PEPC, BIT0 << PEP_EMMC_CONS_INDEX) >> PEP_EMMC_CONS_INDEX)
          DCED("\\_SB.PC00.PSDC",And (PEPC, BIT0 << PEP_SDXC_CONS_INDEX) >> PEP_SDXC_CONS_INDEX)
          DSDF(GUCP(BIT0 << PEP_AUDIO_CONS_INDEX, BIT1 << PEP_AUDIO_CONS_INDEX), PARS("\\_SB.PC00.HDAS")) //15 - HD Audio (includes ADSP)
          DCED("\\_SB.PC00.GFX0",And (PEPC, BIT0 << PEP_GFX_CONS_INDEX) >> PEP_GFX_CONS_INDEX)

          // Apply PEP constraints for CPU,
          // Based on number of threads
          For (Local0 = Zero, Local0 < TCNT, Local0 += 1) {
            If ( Local0 < 10){
              Concatenate ("\\_SB.PR0", Local0, Local1)
            }
            Else{
              Concatenate ("\\_SB.PR", ToDecimalString(Local0), Local1)
            }
              DCED(Local1, 0x01)
          }
          If (LEqual (And (PEPC, BIT0 << PEP_CPU_CONS_INDEX), 0)) // En/Dis CPU
          {
            // Disabled PEP PR
            For (Local0 = Zero, Local0 < TCNT, Local0 += 1) {
            If ( Local0 < 10){
              Concatenate ("\\_SB.PR0", Local0, Local1)
            }
            Else{
              Concatenate ("\\_SB.PR", ToDecimalString(Local0), Local1)
            }
              DCED(Local1, 0x00)
            }
          }

          DCED("\\_SB.PC00.IPU0",And (PEPC, BIT0 << PEP_IPU_CONS_INDEX) >> PEP_IPU_CONS_INDEX) // En/Dis IPU0

          DCED("\\_SB.PC00.HECI",And (PEPC, BIT0 << PEP_CSME_CONS_INDEX) >> PEP_CSME_CONS_INDEX) // En/Dis CSME

          If (LOr (LEqual (And (PEPC, BIT0 << PEP_GBE_CONS_INDEX), 0), LEqual (GBES,0))) { // En/Dis LAN (GBE)
            DCED("\\_SB.PC00.GLAN",0x00) // En/Dis LAN (GBE)
          }

          DCED("\\_SB.PC00.THC0",And (PEPC, BIT0 << PEP_THC0_CONS_INDEX) >> PEP_THC0_CONS_INDEX) // En/Dis THC0
          DCED("\\_SB.PC00.THC1",And (PEPC, BIT0 << PEP_THC1_CONS_INDEX) >> PEP_THC1_CONS_INDEX) // En/Dis THC1

          If (LNotEqual (And (PEPC, BIT0 << PEP_TCSS_CONS_INDEX), 0)) { // En/Dis TCSS IPs
            If (LEqual (THCE, 1)) {
              DCED("\\_SB.PC00.TXHC", 0x01) // TCSS XHCI
            }
            If(LEqual(TDCE, 1)) {
              DCED("\\_SB.PC00.TXDC", 0x01) // TCSS XDCI
            }
            DCED("\\_SB.PC00.TRP0", TRE0) // TCSS ITBT PCIe RP0 on Segment0
            DCED("\\_SB.PC00.TRP1", TRE1) // TCSS ITBT PCIe RP1 on Segment0
            DCED("\\_SB.PC00.TRP2", TRE2) // TCSS ITBT PCIe RP2 on Segment0
            DCED("\\_SB.PC00.TRP3", TRE3) // TCSS ITBT PCIe RP3 on Segment0
            DCED("\\_SB.PC00.TDM0", DME0) // TCSS DMA0
            DCED("\\_SB.PC00.TDM1", DME1) // TCSS DMA1
          }

          DCED("\\_SB.PC00.GNA0", And(PEPC, BIT0 << PEP_GNA_CONS_INDEX) >> PEP_GNA_CONS_INDEX) // En/Dis GNA0
          DCED("\\_SB.PC00.VMD0", And(PEPC, BIT0 << PEP_VMD_CONS_INDEX) >> PEP_VMD_CONS_INDEX) // En/Dis VMD0

          If (CondRefOf(\_SB.PC00.HEC3)) {
            DCED("\\_SB.PC00.HEC3", And(PEPC, BIT0 << PEP_HECI3_CONS_INDEX) >> PEP_HECI3_CONS_INDEX) // En/Dis HEC3
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
        Name(RSTG, Package() {0, 0})
        Name(PWRG, Package() {0, 0})
        Store (0, Local0) // Display All Monitor off flag
        If (LEqual (Arg2, 0x5))
        {
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \_SB.PC00.RP01.DL23()
#if FixedPcdGetBool(PcdMtlPSSupport) == 1
            Store(0x14040B, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x14080B, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
#else
            Store(0x141082, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x140805, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
#endif
            \PIN.ON(RSTG)
            \_SB.PSD3 (1)
            SPCO(0, 0)
            \PIN.OFF (PWRG)
            \_SB.SHPO (0, 0)

            \_SB.PC00.RP09.DL23()
            Store(0x14040D, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x141081, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
            \PIN.ON(RSTG)
            \_SB.PSD3 (1)
            SPCO(0, 0)
            \PIN.OFF (PWRG)
            \_SB.SHPO (0, 0)

            \_SB.PC00.RP10.DL23()
            Store(0x140894, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x140885, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
            \PIN.ON(RSTG)
            \_SB.PSD3 (1)
            SPCO(0, 0)
            \PIN.OFF (PWRG)
            \_SB.SHPO (0, 0)

            \_SB.PC00.RP11.DL23()
            Store(0x14040E, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x141086, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
            \PIN.ON(RSTG)
            \_SB.PSD3 (1)
            SPCO(0, 0)
            \PIN.OFF (PWRG)
            \_SB.SHPO (0, 0)
            \GUAM (1) // 0x01 - Power State Standby (CS Resiliency Entry)
          }

          // Call method to notify EC of Idle Resiliency entry
          \_SB.PC00.LPCB.NTIR (1)

          If (CondRefOf (\_SB.PC00.TXHC)) {
            // Local0 = 1 - Give indication to IOM that all the display is OFF
            // Local0 = 0
            // If system is running in FW CM mode, will need to raise DPOF bit
            If (CondRefOf (\_SB.PC00.TDM0)) {  // check DMA0
              If (LEqual (\_SB.PC00.TDM0.SCME, 0)) {
                Store (1, Local0)
              }
            } ElseIf (CondRefOf (\_SB.PC00.TDM1)) {  // check DMA1
              If (LEqual (\_SB.PC00.TDM1.SCME, 0)) {
                Store (1, Local0)
              }
            }
            If (LEqual (Local0, 1)) {
              Store (1, \_SB.PC00.DPOF)
            }
          }
        }

        // resiliency phase exit (deep standby exit)
        If (LEqual (Arg2, 0x6))
        {
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
#if FixedPcdGetBool(PcdMtlPSSupport) == 1
            Store(0x14040B, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x14080B, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
#else
            Store(0x141082, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x140805, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
#endif
            \_SB.SHPO (0, 1)
            \_SB.CAGS (0)
            \_SB.PSD0 (1)
            \PIN.ON (PWRG)
            Sleep (PEP0)
            SPCO(0, 1)
            \PIN.OFF (RSTG)
            \_SB.PC00.RP01.L23D()

            Store(0x14040D, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x141081, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
            \_SB.SHPO (0, 1)
            \_SB.CAGS (0)
            \_SB.PSD0 (1)
            \PIN.ON (PWRG)
            Sleep (PEP0)
            SPCO(0, 1)
            \PIN.OFF (RSTG)
            \_SB.PC00.RP09.L23D()


            Store(0x140894, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x140885, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
            \_SB.SHPO (0, 1)
            \_SB.CAGS (0)
            \_SB.PSD0 (1)
            \PIN.ON (PWRG)
            Sleep (PEP0)
            SPCO(0, 1)
            \PIN.OFF (RSTG)
            \_SB.PC00.RP10.L23D()


            Store(0x14040E, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x141086, Index(PWRG, 0))
            Store(0x1, Index(PWRG, 1))
            \_SB.SHPO (0, 1)
            \_SB.CAGS (0)
            \_SB.PSD0 (1)
            \PIN.ON (PWRG)
            Sleep (PEP0)
            SPCO(0, 1)
            \PIN.OFF (RSTG)
            \_SB.PC00.RP11.L23D()
            \GUAM (0) // 0x00 - Power State On (CS Resiliency Exit)
          }
          // Call method to notify EC of Idle Resiliency exit
          \_SB.PC00.LPCB.NTIR (0)
          If (CondRefOf (\_SB.PC00.TXHC)) {
            Store (0, \_SB.PC00.DPOF) //Clear bit17 to indicate IOM that all the display is not OFF.
          }

        }
      }// If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))

      Return (Buffer () {0})
    } // Method (_DSM)
  } //device (PEPD)
} // End Scope(\_SB)
