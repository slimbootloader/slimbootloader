/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

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
    Package(){0x001EFFFF, 2, 0, 36 },
    Package(){0x001EFFFF, 3, 0, 37 },
// D29
    Package(){0x001DFFFF, 0, 0, 16 },
    Package(){0x001DFFFF, 1, 0, 17 },
    Package(){0x001DFFFF, 2, 0, 18 },
    Package(){0x001DFFFF, 3, 0, 19 },
// D28
    Package(){0x001CFFFF, 0, 0, 16 },
    Package(){0x001CFFFF, 1, 0, 17 },
    Package(){0x001CFFFF, 2, 0, 18 },
    Package(){0x001CFFFF, 3, 0, 19 },
// D27
    Package(){0x001BFFFF, 0, 0, 16 },
    Package(){0x001BFFFF, 1, 0, 17 },
    Package(){0x001BFFFF, 2, 0, 18 },
    Package(){0x001BFFFF, 3, 0, 19 },
// D26
    Package(){0x001AFFFF, 0, 0, 16 },
    Package(){0x001AFFFF, 1, 0, 17 },
    Package(){0x001AFFFF, 2, 0, 18 },
    Package(){0x001AFFFF, 3, 0, 19 },
// D25
    Package(){0x0019FFFF, 0, 0, 31 },
    Package(){0x0019FFFF, 1, 0, 32 },
    Package(){0x0019FFFF, 2, 0, 33 },
// D23
    Package(){0x0017FFFF, 0, 0, 16 },
// D22
    Package(){0x0016FFFF, 0, 0, 16 },
    Package(){0x0016FFFF, 1, 0, 17 },
    Package(){0x0016FFFF, 2, 0, 18 },
    Package(){0x0016FFFF, 3, 0, 19 },
// D21
    Package(){0x0015FFFF, 0, 0, 27 },
    Package(){0x0015FFFF, 1, 0, 40 },
    Package(){0x0015FFFF, 2, 0, 29 },
    Package(){0x0015FFFF, 3, 0, 30 },
// D20
    Package(){0x0014FFFF, 0, 0, 16 },
    Package(){0x0014FFFF, 1, 0, 17 },
    Package(){0x0014FFFF, 2, 0, 18 },
//  Package(){0x0014FFFF, 3, 0, 19 }, not used
// D19
    Package(){0x0013FFFF, 0, 0, 20 },
    Package(){0x0013FFFF, 1, 0, 21 },
    Package(){0x0013FFFF, 2, 0, 24 },
    Package(){0x0013FFFF, 3, 0, 38 },
// D18
    Package(){0x0012FFFF, 0, 0, 26 },
    Package(){0x0012FFFF, 1, 0, 39 },
    Package(){0x0012FFFF, 2, 0, 18 },
//  Package(){0x0012FFFF, 3, 0, 19 },
// D17
    Package(){0x0011FFFF, 0, 0, 25 },
    Package(){0x0011FFFF, 1, 0, 36 },
    Package(){0x0011FFFF, 2, 0, 28 },
    Package(){0x0011FFFF, 3, 0, 34 },
// D16
    Package(){0x0010FFFF, 2, 0, 18 },
    Package(){0x0010FFFF, 3, 0, 19 },
    Package(){0x0010FFFF, 0, 0, 23 },
    Package(){0x0010FFFF, 1, 0, 22 },
// Host Bridge
// P.E.G. Root Port D6F0
    Package(){0x0006FFFF, 0, 0, 16 },
    Package(){0x0006FFFF, 1, 0, 17 },
    Package(){0x0006FFFF, 2, 0, 18 },
    Package(){0x0006FFFF, 3, 0, 19 },
// P.E.G. Root Port D1F0
    Package(){0x0001FFFF, 0, 0, 16 },
    Package(){0x0001FFFF, 1, 0, 17 },
    Package(){0x0001FFFF, 2, 0, 18 },
    Package(){0x0001FFFF, 3, 0, 19 },
// P.E.G. Root Port D1F1
// P.E.G. Root Port D1F2
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
//---------------------------------------------------------------------------
// Begin PCI tree object scope
//---------------------------------------------------------------------------
  Device(PC00) { // PCI Bridge "Host Bridge"
    Name(_HID, EISAID("PNP0A08")) // Indicates PCI Express/PCI-X Mode2 host hierarchy
    Name(_CID, EISAID("PNP0A03")) // To support legacy OS that doesn't understand the new HID
    Name(_SEG, 0)
    Method(^BN00, 0){ return(0x0000) }  // Returns default Bus number for Peer PCI busses. Name can be overriden with control method placed directly under Device scope
    Method(_BBN, 0){ return(BN00()) } // Bus number, optional for the Root PCI Bus
    Name(_UID, 0x0000)  // Unique Bus ID, optional
    Method(_PRT,0) {
      If(PICM) {Return(AR00)} // APIC mode
      Return (PD00) // PIC Mode
    } // end _PRT

  Include("HostBus.asl")
  } // end PC00 Bridge "Host Bridge"
} // end _SB scope
