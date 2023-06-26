/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Include ("IocRootBridges.asl")

Scope(\_SB) {
  Name(PD00, Package(){
// WARNING: That setting should align with platform policy
// D31
    Package(){0x001FFFFF, 0, 0, 11 },
    Package(){0x001FFFFF, 1, 0, 10 },
    Package(){0x001FFFFF, 2, 0, 11 },
    Package(){0x001FFFFF, 3, 0, 11 },
// D29
    Package(){0x001DFFFF, 0, 0, 11 },
    Package(){0x001DFFFF, 1, 0, 10 },
    Package(){0x001DFFFF, 2, 0, 11 },
    Package(){0x001DFFFF, 3, 0, 11 },
// D28
    Package(){0x001CFFFF, 0, 0, 11 },
    Package(){0x001CFFFF, 1, 0, 10 },
    Package(){0x001CFFFF, 2, 0, 11 },
    Package(){0x001CFFFF, 3, 0, 11 },
// D27
    Package(){0x001BFFFF, 0, 0, 11 },
    Package(){0x001BFFFF, 1, 0, 10 },
    Package(){0x001BFFFF, 2, 0, 11 },
    Package(){0x001BFFFF, 3, 0, 11 },
// D23
    Package(){0x0017FFFF, 0, 0, 11 },
// D22
    Package(){0x0016FFFF, 0, 0, 11 },
    Package(){0x0016FFFF, 1, 0, 10 },
    Package(){0x0016FFFF, 2, 0, 11 },
    Package(){0x0016FFFF, 3, 0, 11 },
// D20
    Package(){0x0014FFFF, 0, 0, 11 },
    Package(){0x0014FFFF, 1, 0, 10 },
    Package(){0x0014FFFF, 2, 0, 11 },
    Package(){0x0014FFFF, 3, 0, 11 },
// Host Bridge
// P.E.G. Root Port D6F0
    Package(){0x0006FFFF, 0, 0, 11 },
    Package(){0x0006FFFF, 1, 0, 10 },
    Package(){0x0006FFFF, 2, 0, 11 },
    Package(){0x0006FFFF, 3, 0, 11 },
// P.E.G. Root Port D1F0
    Package(){0x0001FFFF, 0, 0, 11 },
    Package(){0x0001FFFF, 1, 0, 10 },
    Package(){0x0001FFFF, 2, 0, 11 },
    Package(){0x0001FFFF, 3, 0, 11 },
// P.E.G. Root Port D1F1
// P.E.G. Root Port D1F2
// SA IGFX Device
    Package(){0x0002FFFF, 0, 0, 11 },
// SA Thermal Device
    Package(){0x0004FFFF, 0, 0, 11 },
// SA IPU Device
    Package(){0x0005FFFF, 0, 0, 11 },
// SA GNA Device
    Package(){0x0008FFFF, 0, 0, 11 },
// SA XHCI XDCI Device
    Package(){0x000DFFFF, 0, 0, 11 },
// ITBT PCIE Root Port
    Package(){0x0007FFFF, 0, 0, 11 },
    Package(){0x0007FFFF, 1, 0, 10 },
    Package(){0x0007FFFF, 2, 0, 11 },
    Package(){0x0007FFFF, 3, 0, 11 },
  })
  Name(AR00, Package(){
// PCI Bridge
// D31
    Package(){0x001FFFFF, 0, 0, 16 },
    Package(){0x001FFFFF, 1, 0, 17 },
    Package(){0x001FFFFF, 2, 0, 18 },
    Package(){0x001FFFFF, 3, 0, 19 },
// D30
    Package(){0x001EFFFF, 0, 0, 16 },
    Package(){0x001EFFFF, 1, 0, 17 },
    Package () {0x001EFFFF, 2, 0, 27 },
    Package () {0x001EFFFF, 3, 0, 28 },
// D29 (unused)
// D28
    Package(){0x001CFFFF, 0, 0, 16 },
    Package(){0x001CFFFF, 1, 0, 17 },
    Package(){0x001CFFFF, 2, 0, 18 },
    Package(){0x001CFFFF, 3, 0, 19 },
// D27 (unused)
// D26 (unused)
// D25
    Package () {0x0019FFFF, 0, 0, 29 },
    Package () {0x0019FFFF, 1, 0, 30 },
    Package () {0x0019FFFF, 2, 0, 31 },
// D24 (reserved for RST)
// D23
    Package(){0x0017FFFF, 0, 0, 16 },
// D22
    Package(){0x0016FFFF, 0, 0, 16 },
    Package(){0x0016FFFF, 1, 0, 17 },
    Package(){0x0016FFFF, 2, 0, 18 },
    Package(){0x0016FFFF, 3, 0, 19 },
// D21
    Package () {0x0015FFFF, 0, 0, 32 },
    Package () {0x0015FFFF, 1, 0, 33 },
    Package () {0x0015FFFF, 2, 0, 34 },
    Package () {0x0015FFFF, 3, 0, 35 },
// D20
    Package(){0x0014FFFF, 0, 0, 16 },
    Package(){0x0014FFFF, 1, 0, 17 },
    Package(){0x0014FFFF, 2, 0, 18 },
// D19 (unused)
// D18
    Package(){0x0012FFFF, 0, 0, 26 },
    Package () {0x0012FFFF, 1, 0, 37 },
    Package () {0x0012FFFF, 2, 0, 25 },
// D17 (unused)
// D16
    Package(){0x0010FFFF, 0, 0, 23 },
    Package(){0x0010FFFF, 1, 0, 22 },
// Host Bridge
// D6
    Package(){0x0006FFFF, 0, 0, 16 },
    Package(){0x0006FFFF, 1, 0, 17 },
    Package(){0x0006FFFF, 2, 0, 18 },
    Package(){0x0006FFFF, 3, 0, 19 },
// D1
    Package(){0x0001FFFF, 0, 0, 16 },
    Package(){0x0001FFFF, 1, 0, 17 },
    Package(){0x0001FFFF, 2, 0, 18 },
    Package(){0x0001FFFF, 3, 0, 19 },

// SA IGFX Device
    Package(){0x0002FFFF, 0, 0, 16 },
// SA Thermal Device
    Package(){0x0004FFFF, 0, 0, 16 },
// SA IPU Device
    Package(){0x0005FFFF, 0, 0, 16 },
// SA GNA Device
    Package(){0x0008FFFF, 0, 0, 16 },
// SA XHCI XDCI Device
    Package(){0x000DFFFF, 0, 0, 16 },
    Package(){0x000DFFFF, 1, 0, 17 },
// ITBT PCIE Root Port
    Package(){0x0007FFFF, 0, 0, 16 },
    Package(){0x0007FFFF, 1, 0, 17 },
    Package(){0x0007FFFF, 2, 0, 18 },
    Package(){0x0007FFFF, 3, 0, 19 },
  })
  Name(PD04, Package(){
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 10 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR04, Package(){
    Package(){0x0000FFFF, 0, 0, 16 },
    Package(){0x0000FFFF, 1, 0, 17 },
    Package(){0x0000FFFF, 2, 0, 18 },
    Package(){0x0000FFFF, 3, 0, 19 },
  })
  Name(PD05, Package(){
    Package(){0x0000FFFF, 0, 0, 10 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR05, Package(){
    Package(){0x0000FFFF, 0, 0, 17 },
    Package(){0x0000FFFF, 1, 0, 18 },
    Package(){0x0000FFFF, 2, 0, 19 },
    Package(){0x0000FFFF, 3, 0, 16 },
  })
  Name(PD06, Package(){
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 10 },
  })
  Name(AR06, Package(){
    Package(){0x0000FFFF, 0, 0, 18 },
    Package(){0x0000FFFF, 1, 0, 19 },
    Package(){0x0000FFFF, 2, 0, 16 },
    Package(){0x0000FFFF, 3, 0, 17 },
  })
  Name(PD07, Package(){
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 10 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR07, Package(){
    Package(){0x0000FFFF, 0, 0, 19 },
    Package(){0x0000FFFF, 1, 0, 16 },
    Package(){0x0000FFFF, 2, 0, 17 },
    Package(){0x0000FFFF, 3, 0, 18 },
  })
  Name(PD08, Package(){
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 10 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR08, Package(){
    Package(){0x0000FFFF, 0, 0, 16 },
    Package(){0x0000FFFF, 1, 0, 17 },
    Package(){0x0000FFFF, 2, 0, 18 },
    Package(){0x0000FFFF, 3, 0, 19 },
  })
  Name(PD09, Package(){
    Package(){0x0000FFFF, 0, 0, 10 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR09, Package(){
    Package(){0x0000FFFF, 0, 0, 17 },
    Package(){0x0000FFFF, 1, 0, 18 },
    Package(){0x0000FFFF, 2, 0, 19 },
    Package(){0x0000FFFF, 3, 0, 16 },
  })
  Name(PD0E, Package(){
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 10 },
  })
  Name(AR0E, Package(){
    Package(){0x0000FFFF, 0, 0, 18 },
    Package(){0x0000FFFF, 1, 0, 19 },
    Package(){0x0000FFFF, 2, 0, 16 },
    Package(){0x0000FFFF, 3, 0, 17 },
  })
  Name(PD0F, Package(){
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 10 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR0F, Package(){
    Package(){0x0000FFFF, 0, 0, 19 },
    Package(){0x0000FFFF, 1, 0, 16 },
    Package(){0x0000FFFF, 2, 0, 17 },
    Package(){0x0000FFFF, 3, 0, 18 },
  })
  Name(PD02, Package(){
// P.E.G. Port Slot x4
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 10 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR02, Package(){
// P.E.G. Port Slot x4
    Package(){0x0000FFFF, 0, 0, 16 },
    Package(){0x0000FFFF, 1, 0, 17 },
    Package(){0x0000FFFF, 2, 0, 18 },
    Package(){0x0000FFFF, 3, 0, 19 },
  })
  Name(PD0A, Package(){
// P.E.G. Port Slot x16
    Package(){0x0000FFFF, 0, 0, 10 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR0A, Package(){
// P.E.G. Port Slot x16
    Package(){0x0000FFFF, 0, 0, 17 },
    Package(){0x0000FFFF, 1, 0, 18 },
    Package(){0x0000FFFF, 2, 0, 19 },
    Package(){0x0000FFFF, 3, 0, 16 },
  })
  Name(PD0B, Package(){
// P.E.G. Port Slot x8
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 11 },
    Package(){0x0000FFFF, 3, 0, 10 },
  })
  Name(AR0B, Package(){
// P.E.G. Port Slot x8
    Package(){0x0000FFFF, 0, 0, 18 },
    Package(){0x0000FFFF, 1, 0, 19 },
    Package(){0x0000FFFF, 2, 0, 16 },
    Package(){0x0000FFFF, 3, 0, 17 },
  })
  Name(PD0C, Package(){
// P.E.G. Port Slot x4
    Package(){0x0000FFFF, 0, 0, 11 },
    Package(){0x0000FFFF, 1, 0, 11 },
    Package(){0x0000FFFF, 2, 0, 10 },
    Package(){0x0000FFFF, 3, 0, 11 },
  })
  Name(AR0C, Package(){
// P.E.G. Port Slot x4
    Package(){0x0000FFFF, 0, 0, 19 },
    Package(){0x0000FFFF, 1, 0, 16 },
    Package(){0x0000FFFF, 2, 0, 17 },
    Package(){0x0000FFFF, 3, 0, 18 },
  })

  Scope (PC00) {
    Method (_PRT,0) {
      If (PICM) {Return (AR00)} // APIC mode
      Return (PD00) // PIC Mode
    } // end _PRT
  } // end PC00 Bridge "Host Bridge"

  If (CondRefOf (\_SB.PC02)) {
    Scope (PC02) {
      // PCH interrupts routing
      Name (ARP0, Package() {
        // D31:F0  LPC Controller/eSPI Controller
        // D31:F1  P2SB
        // D31:F2  PMC
        // D31:F3  ACE (Audio Context Engine)
        // D31:F4  SMBus
        // D31:F5  SPI (flash) Controller
        // D31:F6  GbE Controller
        // D31:F7  North Peak
        Package () {0x001FFFFF, 0, 0, 16 },
        Package () {0x001FFFFF, 1, 0, 17 },
        Package () {0x001FFFFF, 2, 0, 18 },
        Package () {0x001FFFFF, 3, 0, 19 },
        // D30:F0  LPSS: UART #0
        // D30:F1  LPSS: UART #1
        // D30:F2  LPSS: SPI #0
        // D30:F3  LPSS: SPI #1
        // D30:F4  TSN GbE Controller
        // D30:F5  TSN GbE Controller 2
        Package () {0x001EFFFF, 0, 0, 16 },
        Package () {0x001EFFFF, 1, 0, 17 },
        Package () {0x001EFFFF, 2, 0, 27 },
        Package () {0x001EFFFF, 3, 0, 28 },
        // D29:F0  PCI Express Root Port #9
        // D29:F1  PCI Express Root Port #10
        // D29:F2  PCI Express Root Port #11
        // D29:F3  PCI Express Root Port #12
        // D29:F4  PCI Express Root Port #13
        // D29:F5  PCI Express Root Port #14
        // D29:F6  PCI Express Root Port #15
        // D29:F7  PCI Express Root Port #16
        Package () {0x001DFFFF, 0, 0, 16 },
        Package () {0x001DFFFF, 1, 0, 17 },
        Package () {0x001DFFFF, 2, 0, 18 },
        Package () {0x001DFFFF, 3, 0, 19 },
        // D28:F0  PCI Express Root Port #1
        // D28:F1  PCI Express Root Port #2
        // D28:F2  PCI Express Root Port #3
        // D28:F3  PCI Express Root Port #4
        // D28:F4  PCI Express Root Port #5
        // D28:F5  PCI Express Root Port #6
        // D28:F6  PCI Express Root Port #7
        // D28:F7  PCI Express Root Port #8
        Package () {0x001CFFFF, 0, 0, 16 },
        Package () {0x001CFFFF, 1, 0, 17 },
        Package () {0x001CFFFF, 2, 0, 18 },
        Package () {0x001CFFFF, 3, 0, 19 },
        // D27:F0  PCI Express Root Port #17
        // D27:F1  PCI Express Root Port #18
        // D27:F2  PCI Express Root Port #19
        // D27:F3  PCI Express Root Port #20
        // D27:F4  PCI Express Root Port #21
        // D27:F5  PCI Express Root Port #22
        // D27:F6  PCI Express Root Port #23
        // D27:F7  PCI Express Root Port #24
        Package () {0x001BFFFF, 0, 0, 16 },
        Package () {0x001BFFFF, 1, 0, 17 },
        Package () {0x001BFFFF, 2, 0, 18 },
        Package () {0x001BFFFF, 3, 0, 19 },
        // D26 (unused)
        // D25:F0  LPSS: I2C Controller #4
        // D25:F1  LPSS: I2C Controller #5
        // D25:F2  LPSS: UART #2
        Package () {0x0019FFFF, 0, 0, 29 },
        Package () {0x0019FFFF, 1, 0, 30 },
        Package () {0x0019FFFF, 2, 0, 31 },
        // D24:F0  ESE HECI #1
        // D24:F1  ESE HECI #2
        // D24:F2  ESE HECI #3
        // D23:F0  SATA Controller (AHCI)
        Package () {0x0017FFFF, 0, 0, 16 },
        // D22:F0  HECI #1
        // D22:F1  HECI #2
        // D22:F2  IDE Redirection (IDER-R)
        // D22:F3  Keyboard and Text (KT) Redirection
        // D22:F4  HECI #3
        // D22:F5  HECI #4
        // D22:F7 CSME WLAN
        Package () {0x0016FFFF, 0, 0, 16 },
        Package () {0x0016FFFF, 1, 0, 17 },
        Package () {0x0016FFFF, 2, 0, 18 },
        Package () {0x0016FFFF, 3, 0, 19 },
        // D21:F0  LPSS: I2C Controller #0
        // D21:F1  LPSS: I2C Controller #1
        // D21:F2  LPSS: I2C Controller #2
        // D21:F3  LPSS: I2C Controller #3
        // D21:F4  LPSS: I3C Controller
        Package () {0x0015FFFF, 0, 0, 32 },
        Package () {0x0015FFFF, 1, 0, 33 },
        Package () {0x0015FFFF, 2, 0, 34 },
        Package () {0x0015FFFF, 3, 0, 35 },
        // D20:F0  USB 3.1 xHCI HC
        // D20:F1  USB Device Controller (OTG) (xDCI)
        // D20:F2  Shared SRAM
        // D20:F3  CNVi: WiFi
        // D20:F5  Error Aggregation Handler (EAH)
        Package () {0x0014FFFF, 0, 0, 16 },
        Package () {0x0014FFFF, 1, 0, 17 },
        Package () {0x0014FFFF, 2, 0, 18 },
        Package () {0x0014FFFF, 3, 0, 19 },
        // D19:F0  LPSS: SPI #3
        // D19:F1  LPSS: UART #3
        Package () {0x0013FFFF, 0, 0, 36 },
        Package () {0x0013FFFF, 1, 0, 37 },
        // D18:F0  Integrated Sensor Hub
        // D18:F6  LPSS: SPI #2
        Package () {0x0012FFFF, 0, 0, 26 },
        Package () {0x0012FFFF, 1, 0, 38 },
        Package () {0x0012FFFF, 2, 0, 25 },
        // D17 (unused)
        // D16:F0  THC #0 (Touch Host Controller)
        // D16:F1  THC #1 (Touch Host Controller)
        Package () {0x0010FFFF, 0, 0, 23 },
        Package () {0x0010FFFF, 1, 0, 22 },
      })

      Method (_PRT, 0) {
        If (PICM) {Return (ARP0)} // APIC mode
        Return (PD00) // PIC Mode
      }
    }
  }
} // end _SB scope

