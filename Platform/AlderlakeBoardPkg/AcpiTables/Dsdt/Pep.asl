/** @file
  ACPI uPEP Support

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/PmcRegs.h>
#include <Register/TcoRegs.h>

External(\_SB.PC00.DPOF)
External(\_SB.PC00.LPCB.H_EC.ECNT, MethodObj)
External(\_SB.PC00.LPCB.HPET.TCN1)
External(\_SB.PC00.SPIF.SPIS)
External(SPCO,MethodObj)
External(\_SB.PC00.RP08.DL23, MethodObj)
External(\_SB.PC00.RP08.L23D, MethodObj)
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
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP02.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP03.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP04.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP05.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP06.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP07.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP08.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP09.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP10.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP11.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP12.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP13.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP14.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP15.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP16.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP17.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP18.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP19.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP20.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP21.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP22.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP23.PXSX)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.RP24.PXSX)
{
  include("PepPcieEp.asl")
}

If (LEqual (PCHS, PCHX)) {
  Scope (\_SB.PC00.RP25.PXSX)
  {
    include("PepPcieEp.asl")
  }

  Scope (\_SB.PC00.RP26.PXSX)
  {
    include("PepPcieEp.asl")
  }

  Scope (\_SB.PC00.RP27.PXSX)
  {
    include("PepPcieEp.asl")
  }

  Scope (\_SB.PC00.RP28.PXSX)
  {
    include("PepPcieEp.asl")
  }
}

Scope (\_SB.PC00.PEG0.PEGP)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.PEG1.PEGP)
{
  include("PepPcieEp.asl")
}

Scope (\_SB.PC00.PEG2.PEGP)
{
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

    Name(PPD0, Package() {"\\_SB.PC00.SAT0",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}})
    Name(PPD3, Package() {"\\_SB.PC00.SAT0",               0x0, Package() {0, Package() {0xFF, 3}}})
    Name(WWD3, Package() {"\\_SB.PC00.RP04",               0x0, Package() {0, Package() {0xFF, 3}}})
    Name(PKD0, Package() {0, Package() {0xFF, 0, 0x81}})
    Name(PKD3, Package() {0, Package() {0xFF, 3}})
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
      Package() {"\\_SB.PC00.GFX0",               0x1, Package() {0, Package() {0xFF, 3}}},                   //  8 - Gfx
      Package() {"\\_SB.PC00.SAT0",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             //  9 - SATA
      Package() {"\\_SB.PC00.UA00",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 10 - UART0
      Package() {"\\_SB.PC00.UA01",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 11 - UART1
      Package() {"\\_SB.PC00.I2C0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 12 - I2C0
      Package() {"\\_SB.PC00.I2C1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 13 - I2C1
      Package() {"\\_SB.PC00.XHCI",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 14 - XHCI
      Package() {"\\_SB.PC00.HDAS",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},             // 15 - cAVS (HDAudio)
      Package() {"\\_SB.PC00.PEMC",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 16 - EMMC
      Package() {"\\_SB.PC00.PSDC",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 17 - SCS SD Card (PCI Mode)
      Package() {"\\_SB.PC00.I2C2",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 18 - I2C2
      Package() {"\\_SB.PC00.I2C3",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 19 - I2C3
      Package() {"\\_SB.PC00.I2C4",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 20 - I2C4
      Package() {"\\_SB.PC00.I2C5",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 21 - I2C5
      Package() {"\\_SB.PC00.UA02",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 22 - UART2
      Package() {"\\_SB.PC00.SPI0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 23 - SPI0
      Package() {"\\_SB.PC00.SPI1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 24 - SPI1
      Package() {"\\_SB.PC00.RP01",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 25 - RP01
      Package() {"\\_SB.PC00.RP02",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 26 - RP02
      Package() {"\\_SB.PC00.RP03",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 27 - RP03
      Package() {"\\_SB.PC00.RP04",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 28 - RP04
      Package() {"\\_SB.PC00.RP05",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 29 - RP05
      Package() {"\\_SB.PC00.RP06",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 30 - RP06
      Package() {"\\_SB.PC00.RP07",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 31 - RP07
      Package() {"\\_SB.PC00.RP08",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 32 - RP08
      Package() {"\\_SB.PC00.RP09",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 33 - RP09
      Package() {"\\_SB.PC00.RP10",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 34 - RP10
      Package() {"\\_SB.PC00.RP11",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 35 - RP11
      Package() {"\\_SB.PC00.RP12",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 36 - RP12
      Package() {"\\_SB.PC00.RP13",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 37 - RP13
      Package() {"\\_SB.PC00.RP14",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 38 - RP14
      Package() {"\\_SB.PC00.RP15",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 39 - RP15
      Package() {"\\_SB.PC00.RP16",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 40 - RP16
      Package() {"\\_SB.PC00.RP17",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 41 - RP17
      Package() {"\\_SB.PC00.RP18",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 42 - RP18
      Package() {"\\_SB.PC00.RP19",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 43 - RP19
      Package() {"\\_SB.PC00.RP20",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 44 - RP20
      Package() {"\\_SB.PC00.SAT0.VOL0",          0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 45 - RAID
      Package() {"\\_SB.PR08",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 46 - SB.PR08
      Package() {"\\_SB.PR09",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 47 - SB.PR09
      Package() {"\\_SB.PR10",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 48 - SB.PR10
      Package() {"\\_SB.PR11",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 49 - SB.PR11
      Package() {"\\_SB.PR12",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 50 - SB.PR12
      Package() {"\\_SB.PR13",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 51 - SB.PR13
      Package() {"\\_SB.PR14",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 52 - SB.PR14
      Package() {"\\_SB.PR15",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 53 - SB.PR15
      Package() {"\\_SB.PC00.SPI2",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 54 - SPI2
      Package() {"\\_SB.PC00.CNVW",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 55 - CNVi WiFi
      Package() {"\\_SB.PC00.IPU0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 56 - IPU0
      Package() {"\\_SB.PC00.HECI",               0x1, Package() {0, Package() {0xFF, 0, 0x81}}},             // 57 - CSME
      Package() {"\\_SB.PC00.GLAN",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 58 - LAN(GBE)
      Package() {"\\_SB.PC00.PEG0",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 59 - PEG0
      Package() {"\\_SB.PC00.THC0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 60 - THC0
      Package() {"\\_SB.PC00.THC1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 61 - THC1
      Package() {"Depreciate IDA",                0x0, Package() {0, Package() {0xFF, 3}}},                   // 62 - Reserved
      Package() {"USB\\VID_8087&PID_0AC9&MI*",    0x0, Package() {0, Package() {0xFF, 2}}},                   // 63 - CDC MBIM
      Package() {"\\_SB.PC00.I2C6",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 64 - I2C6
      Package() {"\\_SB.PC00.PEG1",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 65 - PEG1
      Package() {"\\_SB.PR16",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 66 - SB.PR16
      Package() {"\\_SB.PR17",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 67 - SB.PR17
      Package() {"\\_SB.PR18",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 68 - SB.PR18
      Package() {"\\_SB.PR19",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 69 - SB.PR19
      Package() {"\\_SB.PR20",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 70 - SB.PR20
      Package() {"\\_SB.PR21",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 71 - SB.PR21
      Package() {"\\_SB.PR22",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 72 - SB.PR22
      Package() {"\\_SB.PR23",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 73 - SB.PR23
      Package() {"\\_SB.PC00.GNA0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 74 - GNA0
      Package() {"\\_SB.PC00.VMD0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 75 - VMD0
      Package() {"\\_SB.PC00.RP21",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 76 - RP21
      Package() {"\\_SB.PC00.RP22",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 77 - RP22
      Package() {"\\_SB.PC00.RP23",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 78 - RP23
      Package() {"\\_SB.PC00.RP24",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 79 - RP24
      Package() {"Reserved For DTBT RP0",         0x0, Package() {0, Package() {0xFF, 3}}},                   // 80 - DTBT RP 00
      Package() {"Reserved For DTBT RP1",         0x0, Package() {0, Package() {0xFF, 3}}},                   // 81 - DTBT RP 01
      Package() {"\\_SB.PC00.RP25",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 82 - RP25
      Package() {"\\_SB.PC00.RP26",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 83 - RP26
      Package() {"\\_SB.PC00.RP27",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 84 - RP27
      Package() {"\\_SB.PC00.RP28",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 85 - RP28
      Package() {"\\_SB.PC00.SAT0.PRT0",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 86 - PRT0
      Package() {"\\_SB.PC00.SAT0.PRT1",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 87 - PRT1
      Package() {"\\_SB.PC00.SAT0.PRT2",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 88 - PRT2
      Package() {"\\_SB.PC00.SAT0.PRT3",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 89 - PRT3
      Package() {"\\_SB.PC00.SAT0.PRT4",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 90 - PRT4
      Package() {"\\_SB.PC00.SAT0.PRT5",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 91 - PRT5
      Package() {"\\_SB.PC00.SAT0.PRT6",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 92 - PRT6
      Package() {"\\_SB.PC00.SAT0.PRT7",          0x0, Package() {0, Package() {0xFF, 3}}},                   // 93 - PRT7
      Package() {"\\_SB.PC00.HEC3",               0x0, Package() {0, Package() {0xFF, 0, 0x81}}},             // 94 - HEC3
      Package() {"\\_SB.PC00.PEG2",               0x0, Package() {0, Package() {0xFF, 3}}},                   // 95 - PEG2
      Package() {"\\_SB.PR24",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 96 - SB.PR24
      Package() {"\\_SB.PR25",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 97 - SB.PR25
      Package() {"\\_SB.PR26",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 98 - SB.PR26
      Package() {"\\_SB.PR27",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 99 - SB.PR27
      Package() {"\\_SB.PR28",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 100 - SB.PR28
      Package() {"\\_SB.PR29",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 101 - SB.PR29
      Package() {"\\_SB.PR30",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 102 - SB.PR30
      Package() {"\\_SB.PR31",                    0x0, Package() {0, Package() {0xFF, 0}}},                   // 103 - SB.PR31
      Package() {"\\_SB.PC00.PUF0",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 104 - UFS0
      Package() {"\\_SB.PC00.PUF1",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 105 - UFS1
      Package() {"\\_SB.PC00.I2C7",               0x1, Package() {0, Package() {0xFF, 3}}},                   // 106 - I2C7
      //
      // Important: Below are TCSS IPs list, please keep it always
      // at the bottom.
      // All new IPs must be added before the below list and modify
      // below indexs accordingly.
      //
      //
      // Important: Do not add any new IPs here.
      // Add any new IPs before the above TCSS IP list.
      //
    })

    //
    // Get User Configured PEP Constraint
    // Arguments: (2)
    //    Arg0 - low bit index in PEPC
    //    Arg1 - high bit index in PEPC
    // Returns: 0 ->no constraint; 1->D0 F1; 3->D3
    //
    Method(GUCP, 2, Serialized) {
      If (LEqual(And(PEPC, Arg0), Arg0)) {
        If (LEqual(And(PEPC, Or(Arg1, Arg0)), Or(Arg1, Arg0))) {
          Return(3)
        } Else {
          Return(1)
        }
      } Else {
        Return(0)
      }
    }

    //
    // Get PEP Constraints of PCH/CPU PCIe Root Port of specific EP type
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
        Return (GUCP (BIT33, BIT34))
      }

      If (LEqual (Arg0, 2)) {
        Return (GUCP (BIT35, BIT36))
      }

      If (LEqual (Arg0, 3)) {
        If (LGreaterEqual (OSYS,2015)){
          Return (GUCP (BIT37, BIT38))
        }
        Return(0)
      }

      If (LEqual (Arg0, 4)) {
        Return (GUCP (BIT39, BIT40))
      }

      If (LEqual (Arg0, 5)) {
        Return (GUCP (BIT29, BIT29))
      }

      If (LEqual (Arg0, 6)) {
        If (LEqual (WRTO, 3)) {
          Return (3)
        } Else {
          If (LEqual (WRTO, 1)) { // if WRTO is 1, then Put D2 constraint on CDC MBIM
            DSDF (WRTO, 63) // 63 - CDC MBIM
          }
          Return (0) // if WRTO is not 3, then PCIR RP Constraint should be disable
        }
      }

      If(LEqual (Arg0, 0)) {
        Return(GUCP(BIT41, BIT42))
      }

      Return(0)
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
      Return(Ones)
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
      DSDF(GRPC(\_SB.PC00.RP13.PXSX.GRPT()), PARS("\\_SB.PC00.RP13"))
      DSDF(GRPC(\_SB.PC00.RP14.PXSX.GRPT()), PARS("\\_SB.PC00.RP14"))
      DSDF(GRPC(\_SB.PC00.RP15.PXSX.GRPT()), PARS("\\_SB.PC00.RP15"))
      DSDF(GRPC(\_SB.PC00.RP16.PXSX.GRPT()), PARS("\\_SB.PC00.RP16"))
      DSDF(GRPC(\_SB.PC00.RP17.PXSX.GRPT()), PARS("\\_SB.PC00.RP17"))
      DSDF(GRPC(\_SB.PC00.RP18.PXSX.GRPT()), PARS("\\_SB.PC00.RP18"))
      DSDF(GRPC(\_SB.PC00.RP19.PXSX.GRPT()), PARS("\\_SB.PC00.RP19"))
      DSDF(GRPC(\_SB.PC00.RP20.PXSX.GRPT()), PARS("\\_SB.PC00.RP20"))
      DSDF(GRPC(\_SB.PC00.RP21.PXSX.GRPT()), PARS("\\_SB.PC00.RP21"))
      DSDF(GRPC(\_SB.PC00.RP22.PXSX.GRPT()), PARS("\\_SB.PC00.RP22"))
      DSDF(GRPC(\_SB.PC00.RP23.PXSX.GRPT()), PARS("\\_SB.PC00.RP23"))
      DSDF(GRPC(\_SB.PC00.RP24.PXSX.GRPT()), PARS("\\_SB.PC00.RP24"))
      If (LEqual (PCHS, PCHX)) {
        DSDF(GRPC(\_SB.PC00.RP25.PXSX.GRPT()), PARS("\\_SB.PC00.RP25"))
        DSDF(GRPC(\_SB.PC00.RP26.PXSX.GRPT()), PARS("\\_SB.PC00.RP26"))
        DSDF(GRPC(\_SB.PC00.RP27.PXSX.GRPT()), PARS("\\_SB.PC00.RP27"))
        DSDF(GRPC(\_SB.PC00.RP28.PXSX.GRPT()), PARS("\\_SB.PC00.RP28"))
      }
      DSDF(GRPC(\_SB.PC00.PEG0.PEGP.GRPT()), PARS("\\_SB.PC00.PEG0"))
      DSDF(GRPC(\_SB.PC00.PEG1.PEGP.GRPT()), PARS("\\_SB.PC00.PEG1"))
      DSDF(GRPC(\_SB.PC00.PEG2.PEGP.GRPT()), PARS("\\_SB.PC00.PEG2"))
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

      If (\_SB.PC00.SAT0.PRT0.PRES()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 86)
      }
      If (\_SB.PC00.SAT0.PRT1.PRES()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 87)
      }
      If (\_SB.PC00.SAT0.PRT2.PRES()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 88)
      }
      If (\_SB.PC00.SAT0.PRT3.PRES()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 89)
      }
      If (\_SB.PC00.SAT0.PRT4.PRES()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 90)
      }
      If (\_SB.PC00.SAT0.PRT5.PRES ()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 91)
      }
      If (\_SB.PC00.SAT0.PRT6.PRES()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 92)
      }
      If (\_SB.PC00.SAT0.PRT7.PRES ()) {
        PEP_ENABLE_CONSTRAINT_AT_INDEX (DEVY, 93)
      }
    }

    Name(BCCD, Package() // Bugcheck Critical Device(s)
    {  })

    OperationRegion (LS0I, SystemMemory, LPMR, V_PMC_PWRM_LPM_REQ_DATA_LEN)
    Field (LS0I, ByteAcc, Lock, Preserve)
    {
      //
      // Low Power Mode Required Register
      //
      L0IX, V_PMC_PWRM_LPM_REQ_BITS_DATA_LEN,
    }

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
      If(LEqual(S0ID, 1))
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

          //
          // SATA port drive constraint enumeration
          //
          SPDE()

          // Update uPEP device list based on PEPC (Low Power S0 Constraint)
          // Bit[1:0] - Storage (0:None, 1:Storage Controller, 2:Raid)
          // Bit[2]   - En/Dis UART0
          // Bit[3]   - En/Dis UART1
          // Bit[4]   - En/Dis I2C0
          // Bit[5]   - En/Dis I2C1
          // Bit[6]   - En/Dis XHCI
          // Bit[8:7] - HD Audio (includes ADSP) (0:None, 1:D0F1, 3:D3)
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
          // Bit[21]  - En/Dis IPU0
          // Bit[22]  - En/Dis CSME
          // Bit[23]  - En/Dis LAN(GBE)
          // Bit[24]  - Reserved
          // Bit[25]  - En/Dis THC0
          // Bit[26]  - En/Dis THC1
          // Bit[27]  - Reserved
          // Bit[28]  - En/Dis I2C6
          // Bit[29]  - En/Dis TCSS IPs/DTBT
          // Bit[30]  - En/Dis GNA0
          // Bit[31]  - En/Dis VMD0
          // Bit[32]  - En/Dis HECI3
          // Bit[34:33] - PCIe Storage(0:None, 1:D0F1, 3:D3)
          // Bit[36:35] - PCIe Lan(0:None, 1:D0F1, 3:D3)
          // Bit[38:37] - PCIe Wlan(0:None, 1:D0F1, 3:D3)
          // Bit[40:39] - PCIe Gfx(0:None, 1:D0F1, 3:D3)
          // Bit[42:41] - PCIe Other(0:None, 1:D0F1, 3:D3)
          // Bit[43]  - En/Dis DG on x8 PEG port (PEG1)
          // BIT[44]  - En/Dis UFS0
          // BIT[45]  - En/Dis UFS1
          // Bit[46]  - En/Dis I2C7

          If(LEqual(And(PEPC, BIT0), 1)) // PEPC Bit[1:0] - Storage (1:Adapter D0/F1 or 3:Adapter D3)
          {
            // Constraint for SATA Controller (SAT0)
            If(LEqual(And(PEPC, Or(BIT1, BIT0)), 1)) // PEPC Bit[1:0] - Storage (1:Adapter D0/F1)
            {
              Store (PPD0, Index (DEVY, 9)) // 9 - SATA Adapter D0
            } ElseIf(LEqual(And(PEPC, Or(BIT1, BIT0)), 3)) // PEPC Bit[1:0] - Storage (3:Adapter D3)
            {
              Store (PPD3, Index (DEVY, 9)) // 9 - SATA Adapter D3
            }
            Store (0x01, Index (DeRefOf(Index (DEVY, 9)), 1)) // Set PEP enabled
          } ElseIf(LEqual(And(PEPC, Or(BIT1, BIT0)), 2)) // PEPC Bit[1:0] - Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 45)), 1)) // 45 - RAID
          }

          //Apply all the PCIe root ports constraint based on user choice
          RPPC()

          If(LEqual(And(PEPC, BIT2), 0)) // PEPC Bit[2] - En/Dis UART0
          {
            // Disabled UA00
            Store (0x00, Index (DeRefOf(Index (DEVY, 10)), 1)) // 10 - UART0
          }

          If(LEqual(And(PEPC, BIT3), 0)) // PEPC Bit[3] - En/Dis UART1
          {
            // Disabled UA01
            Store (0x00, Index (DeRefOf(Index (DEVY, 11)), 1)) // 11 - UART1
          }

          If(LEqual(And(PEPC, BIT4), 0)) // PEPC Bit[4] - En/Dis I2C0
          {
            // Disabled I2C0
            Store (0x00, Index (DeRefOf(Index (DEVY, 12)), 1)) // 12 - I2C0
          }

          If(LEqual(And(PEPC, BIT5), 0))  // PEPC Bit[5] - En/Dis I2C1
          {
            // Disabled I2C1
            Store (0x00, Index (DeRefOf(Index (DEVY, 13)), 1)) // 13 - I2C1
          }

          If(LEqual(And(PEPC, BIT6), 0))  // PEPC Bit[6] - En/Dis XHCI
          {
            // Disabled XHCI
            Store (0x00, Index (DeRefOf(Index (DEVY, 14)), 1)) // 14 - XHCI
          }

          DSDF(GUCP(BIT7, BIT8), PARS("\\_SB.PC00.HDAS")) //15 - HD Audio (includes ADSP)

          If(LEqual(And(PEPC, BIT9), 0)) // PEPC Bit[9] - En/Dis Gfx
          {
            // Disabled Gfx
            Store (0x00, Index (DeRefOf(Index (DEVY, 8)), 1)) // 8 - Gfx
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
            Store (0x01, Index (DeRefOf(Index (DEVY, 46)), 1))  // 46 - PR08
          }
          If (LGreaterEqual(TCNT, 10))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 47)), 1))  // 47 - PR09
          }
          If (LGreaterEqual(TCNT, 11))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 48)), 1))  // 48 - PR10
          }
          If (LGreaterEqual(TCNT, 12))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 49)), 1))  // 49 - PR11
          }
          If (LGreaterEqual(TCNT, 13))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 50)), 1))  // 50 - PR12
          }
          If (LGreaterEqual(TCNT, 14))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 51)), 1))  // 51 - PR13
          }
          If (LGreaterEqual(TCNT, 15))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 52)), 1))  // 52 - PR14
          }
          If (LGreaterEqual(TCNT, 16))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 53)), 1))  // 53 - PR15
          }
          If (LGreaterEqual(TCNT, 17))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 66)), 1))  // 66 - PR16
          }
          If (LGreaterEqual(TCNT, 18))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 67)), 1))  // 67 - PR17
          }
          If (LGreaterEqual(TCNT, 19))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 68)), 1))  // 68 - PR18
          }
          If (LGreaterEqual(TCNT, 20))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 69)), 1))  // 69 - PR19
          }
          If (LGreaterEqual(TCNT, 21))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 70)), 1))  // 70 - PR20
          }
          If (LGreaterEqual(TCNT, 22))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 71)), 1))  // 71 - PR21
          }
          If (LGreaterEqual(TCNT, 23))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 72)), 1))  // 72 - PR22
          }
          If (LGreaterEqual(TCNT, 24))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 73)), 1))  // 73 - PR23
          }
          If (LGreaterEqual(TCNT, 25))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 96)), 1))  // 96 - PR24
          }
          If (LGreaterEqual(TCNT, 26))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 97)), 1))  // 97 - PR25
          }
          If (LGreaterEqual(TCNT, 27))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 98)), 1))  // 98 - PR26
          }
          If (LGreaterEqual(TCNT, 28))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 99)), 1))  // 99 - PR27
          }
          If (LGreaterEqual(TCNT, 29))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 100)), 1))  // 100 - PR28
          }
          If (LGreaterEqual(TCNT, 30))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 101)), 1))  // 101 - PR29
          }
          If (LGreaterEqual(TCNT, 31))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 102)), 1))  // 102 - PR30
          }
          If (LGreaterEqual(TCNT, 32))
          {
            Store (0x01, Index (DeRefOf(Index (DEVY, 103)), 1))  // 103 - PR31
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
            Store (0x00, Index (DeRefOf(Index (DEVY, 46)), 1))  // 46 - PR08
            Store (0x00, Index (DeRefOf(Index (DEVY, 47)), 1))  // 47 - PR09
            Store (0x00, Index (DeRefOf(Index (DEVY, 48)), 1))  // 48 - PR10
            Store (0x00, Index (DeRefOf(Index (DEVY, 49)), 1))  // 49 - PR11
            Store (0x00, Index (DeRefOf(Index (DEVY, 50)), 1))  // 50 - PR12
            Store (0x00, Index (DeRefOf(Index (DEVY, 51)), 1))  // 51 - PR13
            Store (0x00, Index (DeRefOf(Index (DEVY, 52)), 1))  // 52 - PR14
            Store (0x00, Index (DeRefOf(Index (DEVY, 53)), 1))  // 53 - PR15
            Store (0x00, Index (DeRefOf(Index (DEVY, 66)), 1))  // 66 - PR16
            Store (0x00, Index (DeRefOf(Index (DEVY, 67)), 1))  // 67 - PR17
            Store (0x00, Index (DeRefOf(Index (DEVY, 68)), 1))  // 68 - PR18
            Store (0x00, Index (DeRefOf(Index (DEVY, 69)), 1))  // 69 - PR19
            Store (0x00, Index (DeRefOf(Index (DEVY, 70)), 1))  // 70 - PR20
            Store (0x00, Index (DeRefOf(Index (DEVY, 71)), 1))  // 71 - PR21
            Store (0x00, Index (DeRefOf(Index (DEVY, 72)), 1))  // 72 - PR22
            Store (0x00, Index (DeRefOf(Index (DEVY, 73)), 1))  // 73 - PR23
            Store (0x00, Index (DeRefOf(Index (DEVY, 96)), 1))  // 96 - PR24
            Store (0x00, Index (DeRefOf(Index (DEVY, 97)), 1))  // 97 - PR25
            Store (0x00, Index (DeRefOf(Index (DEVY, 98)), 1))  // 98 - PR26
            Store (0x00, Index (DeRefOf(Index (DEVY, 99)), 1))  // 99 - PR27
            Store (0x00, Index (DeRefOf(Index (DEVY, 100)), 1)) // 100 - PR28
            Store (0x00, Index (DeRefOf(Index (DEVY, 101)), 1)) // 101 - PR29
            Store (0x00, Index (DeRefOf(Index (DEVY, 102)), 1)) // 102 - PR30
            Store (0x00, Index (DeRefOf(Index (DEVY, 103)), 1)) // 103 - PR31
          }

          If(LOr(LEqual(And(PEPC, BIT11), 0), LEqual(EMCE,0))) // PEPC Bit[11] - En/Dis eMMC
          {
            // Disabled eMMC
            Store (0x00, Index (DeRefOf(Index (DEVY, 16)), 1)) // 16 - EMMC
          }

          If(LOr(LEqual(And(PEPC, BIT12), 0), LEqual(SDCE,0))) // PEPC Bit[12] - En/Dis SDXC
          {
            // Disabled SDXC
            Store (0x00, Index (DeRefOf(Index (DEVY, 17)), 1)) // 17 - SCS SD Card (PCI Mode)
          }

          If(LEqual(And(PEPC, BIT13), 0)) { // PEPC Bit[13] - En/Dis I2C2
            Store (0x00, Index (DeRefOf(Index (DEVY, 18)), 1)) // 18 - I2C2
          }

          If(LEqual(And(PEPC, BIT14),0)) { // PEPC Bit[14] - En/Dis I2C3
            Store (0x00, Index (DeRefOf(Index (DEVY, 19)), 1)) // 19 - I2C3
          }

          If(LEqual(And(PEPC, BIT15), 0)) { // PEPC Bit[15] - En/Dis I2C4
            Store (0x00, Index (DeRefOf(Index (DEVY, 20)), 1)) // 20 - I2C4
          }

          If(LEqual(And(PEPC, BIT16), 0)) { // PEPC Bit[16] - En/Dis I2C5
            Store (0x00, Index (DeRefOf(Index (DEVY, 21)), 1)) // 21 - I2C5
          }

          If(LEqual(And(PEPC, BIT46), 0)) { // PEPC Bit[46] - En/Dis I2C7
            Store (0x00, Index (DeRefOf(Index (DEVY, 106)), 1)) // 106 - I2C7
          }

          If(LEqual(And(PEPC, BIT17), 0)) { // PEPC Bit[17] - En/Dis UART2
            Store (0x00, Index (DeRefOf(Index (DEVY, 22)), 1)) // 22 - UART2
          }

          If(LEqual(And(PEPC, BIT18), 0)) { // PEPC Bit[18] - En/Dis SPI0
            Store (0x00, Index (DeRefOf(Index (DEVY, 23)), 1)) // 23 - SPI0
          }

          If(LEqual(And(PEPC, BIT19), 0)) { // PEPC Bit[19] - En/Dis SPI1
            Store (0x00, Index (DeRefOf(Index (DEVY, 24)), 1)) // 24 - SPI1
          }

          If(LEqual(And(PEPC, BIT20), 0)) { // PEPC Bit[20] - En/Dis SPI2
            Store (0x00, Index (DeRefOf(Index (DEVY, 54)), 1)) // 54 - SPI2
          }
          If(LEqual(And(PEPC, BIT21), 0)) { // PEPC Bit[21] - En/Dis IPU0
            Store (0x00, Index (DeRefOf(Index (DEVY, 56)), 1)) // 56 - IPU0
          }

          If(LEqual(And(PEPC, BIT22), 0)) { // PEPC Bit[22] - En/Dis CSME
            Store (0x00, Index (DeRefOf(Index (DEVY, 57)), 1)) // 57 - CSME
          }

          If(LOr(LEqual(And(PEPC, BIT23), 0), LEqual(GBES,0))) { // PEPC Bit[23] - En/Dis LAN(GBE)
            Store (0x00, Index (DeRefOf(Index (DEVY, 58)), 1)) // 58 - LAN(GBE)
          }

          If(LEqual(And(PEPC, BIT25), 0)) { // PEPC Bit[25] - En/Dis THC0
            Store (0x00, Index (DeRefOf(Index (DEVY, 60)), 1)) // 60 - THC0
          }

          If(LEqual(And(PEPC, BIT26), 0)) { // PEPC Bit[26] - En/Dis THC1
            Store (0x00, Index (DeRefOf(Index (DEVY, 61)), 1)) // 61 - THC1
          }

          If(LEqual(And(PEPC, BIT28), 0)) { // PEPC Bit[28] - En/Dis I2C6
            Store (0x00, Index (DeRefOf(Index (DEVY, 64)), 1)) // 64 - I2C6
          }

          If(LNotEqual(And(PEPC, BIT43), 0)) { // PEPC Bit[43] - En/Dis DG on PEG1 (x8 PEG Port)
            Store (0x01, Index (DeRefOf(Index (DEVY, 65)), 1)) // 65 - DG
          }
          If(LEqual(And(PEPC, BIT44), 0)) { // PEPC Bit[44] - En/Dis UFS0
            Store (0x00, Index (DeRefOf(Index (DEVY, 104)), 1)) // 104 - UFS0
          }
          If(LEqual(And(PEPC, BIT45), 0)) { // PEPC Bit[45] - En/Dis UFS1
            Store (0x00, Index (DeRefOf(Index (DEVY, 105)), 1)) // 105 - UFS1
          }
          If(LEqual(And(PEPC, BIT30), 0)) { // PEPC Bit[30] - En/Dis GNA0
            Store (0x00, Index (DeRefOf(Index (DEVY, 74)), 1)) // 74 - GNA0
          }

          If(LEqual(And(PEPC, BIT31), 0)) { // PEPC Bit[31] - En/Dis VMD0
            Store (0x00, Index (DeRefOf(Index (DEVY, 75)), 1)) // 75 - VMD0
          }

          If (CondRefOf(\_SB.PC00.HEC3)) {
            If(LNotEqual(And(PEPC, BIT32), 0)) { // PEPC Bit[32] - En/Dis HECI3
              Store (0x01, Index (DeRefOf(Index (DEVY, 94)), 1)) // 94 - HECI3
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

        Name(RSTG, Package() {0, 0})
        Name(PWRG, Package() {0, 0})

        // resiliency phase entry (deep standby entry)
        Store (1, Local0) // Display All Monitor off flag
        ADBG (Concatenate ("All Monitor off flag(default): ", ToHexString (Local0)))
        If(LEqual(Arg2, 0x5))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \_SB.PC00.SPIF.SPIS() // Clear SPI Synchronous SMI Status bit
            Store(0x0000000000000000, \_SB.PC00.LPCB.HPET.TCN1)
            \_SB.PC00.RP08.DL23()
            Store(0x90C000A, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x9020016, Index(PWRG, 0))
            Store(0x0, Index(PWRG, 1))
            \PIN.ON(RSTG)
            \_SB.PSD3 (1)
            If(CondRefOf(TMCS)) {
              SPCX(7, 0, TMCS)
            } Else {
              SPCO(7, 0)
            }
            \PIN.OFF (PWRG)
            \_SB.SHPO (0, 0)
            \GUAM(1) // 0x01 - Power State Standby (CS Resiliency Entry)
          }

          If (\ECON)
          {
            \_SB.PC00.LPCB.H_EC.ECNT(1) // 0x01 - Call method to notify EC of Idle Resiliency entry
          }
          If (CondRefOf(\_SB.PC00.TXHC)) {
            If (LEqual(\_SB.PC00.TDM0._STA, 0x0F)) {  // check DMA0
              If (LEqual (\_SB.PC00.TDM0.SCME, 1)) {
                Store (0, Local0)
                ADBG (Concatenate ("TDM0 set DPOF to  ", ToHexString (Local0)))
              }
            } ElseIf (LEqual(\_SB.PC00.TDM1._STA, 0x0F)) {  // check DMA1
              If (LEqual (\_SB.PC00.TDM1.SCME, 1)) {
                Store (0, Local0)
                ADBG (Concatenate ("TDM1 set DPOF to  ", ToHexString (Local0)))
              }
            }
            // Local0 = 1
            // Give indication to IOM that all the display is OFF
            // Local0 = 0
            // The system is running in SW CM mode
            // Skip DPOF setting
            ADBG (Concatenate ("All Monitor off flag: ", ToHexString (Local0)))
            If (LEqual (Local0, 1)) {
              Store (1, \_SB.PC00.DPOF)
            }
          }
          If (CondRefOf (\_SB.PC00.XHCI.PSLI)) {
            \_SB.PC00.XHCI.PSLI (5)
          } else {
          }
        }

        // resiliency phase exit (deep standby exit)
        If(LEqual(Arg2, 0x6))
        {
          If(LEqual(S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            Store(0x90C000A, Index(RSTG, 0))
            Store(0x0, Index(RSTG, 1))
            Store(0x9020016, Index(PWRG, 0))
            Store(0x0, Index(PWRG, 1))
            \_SB.SHPO (0, 1)
            \_SB.CAGS (0)
            \_SB.PSD0 (1)
            \PIN.ON (PWRG)
            Sleep (PEP0)
            If(CondRefOf(TMCS)) {
              SPCX(7, 1, TMCS)
            } Else {
              SPCO(7, 1)
            }
            \PIN.OFF (RSTG)
            \_SB.PC00.RP08.L23D()
            \GUAM(0) // 0x00 - Power State On (CS Resiliency Exit)
          }
          If (\ECON) {
            \_SB.PC00.LPCB.H_EC.ECNT(0) // 0x00 - Call method to notify EC of Idle Resiliency exit
          }

          If (CondRefOf(\_SB.PC00.TXHC)) {
            Store(0, \_SB.PC00.DPOF) //Clear bit17 to indicate IOM that all the display is not OFF.
          }

          If (CondRefOf (\_SB.PC00.XHCI.PSLI)) {
            \_SB.PC00.XHCI.PSLI (6)
          } else {
          }

        }
      }// If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))

      If ((Arg0 == ToUUID ("57a6512e-3979-4e9d-9708-ff13b2508972")))
      {
        If ((Arg2 == Zero)) {
          Return (Buffer(One) { 0x03 })
        }
        If ((Arg2 == 0x01))
        {
          Store (L0IX, LBUF)
          Return (LBUF)
        }
      } // If(LEqual(Arg0,ToUUID("57a6512e-3979-4e9d-9708-ff13b2508972")))

      Return (Buffer() {0})
    } // Method(_DSM)
  } //device (PEPD)
} // End Scope(\_SB)
