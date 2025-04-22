/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope(\_SB) {
  Name(PR00, Package(){
// Host Bridge
    Package(){0x0000FFFF, 2, LNKA, 0 },   // NPK
    Package(){0x0002FFFF, 0, LNKD, 0 },   // Mobile IGFX
    Package(){0x0003FFFF, 0, LNKF, 0 },   // IUNIT
    Package(){0x000AFFFF, 0, LNKE, 0 },   // ISH
    Package(){0x000BFFFF, 0, LNKF, 0 },   // PUNIT
    Package(){0x000CFFFF, 0, LNKH, 0 },   // GMM

    Package(){0x000EFFFF, 0, LNKA, 0 },   // D14: Low Power Audio Engine
    Package(){0x000FFFFF, 0, LNKE, 0 },   // CSE
    Package(){0x0012FFFF, 0, LNKD, 0 },   // D18: SATA Controller

    Package(){0x0013FFFF, 0, LNKG, 0 },   // D19: PCI Express Port 3-6
    Package(){0x0013FFFF, 1, LNKH, 0 },   // D19: PCI Express Port 3-6
    Package(){0x0013FFFF, 2, LNKE, 0 },   // D19: PCI Express Port 3-6
    Package(){0x0013FFFF, 3, LNKF, 0 },   // D19: PCI Express Port 3-6

    Package(){0x0014FFFF, 0, LNKG, 0 },   // D20: PCI Express Port 1-2
    Package(){0x0014FFFF, 1, LNKH, 0 },   // D20: PCI Express Port 1-2

    Package(){0x0015FFFF, 0, LNKB, 0 },   // D21: xHCI Host, xDCI
    Package(){0x0015FFFF, 1, LNKC, 0 },

    Package(){0x001FFFFF, 0, LNKE, 0 },   // SMBus
  })

  Name(AR00, Package() {

// Fields: Address,
//         PCI Pin,
//         Source (0 is global interrupt pool),
//         Source Index (IRQ if Source=0)

    Package(){0x0000FFFF, 0, 0, 16 },  // NPK Device
    Package(){0x0000FFFF, 1, 0, 24 },  // PUNIT Device (INTB -> 24)
    Package(){0x0002FFFF, 0, 0, 19 },  // GEN (Intel GFX)
    Package(){0x0003FFFF, 0, 0, 21 },  // IUNIT
    Package(){0x000DFFFF, 1, 0, 40 },  // PMC
    Package(){0x000EFFFF, 0, 0, 25 },  // HD-Audio
    Package(){0x000FFFFF, 0, 0, 20 },  // CSE
    Package(){0x0011FFFF, 0, 0, 26 },  // ISH
    Package(){0x0012FFFF, 0, 0, 19 },  // SATA

// D19: PCI Express Port 3-6
    Package(){0x0013FFFF, 0, 0, 22 },  // PCIE0
    Package(){0x0013FFFF, 1, 0, 23 },  // PCIE0
    Package(){0x0013FFFF, 2, 0, 20 },  // PCIE0
    Package(){0x0013FFFF, 3, 0, 21 },  // PCIE0

// D20: PCI Express Port 1-2
    Package(){0x0014FFFF, 0, 0, 22 },  // PCIE1
    Package(){0x0014FFFF, 1, 0, 23 },  // PCIE1

    Package(){0x0015FFFF, 0, 0, 17 },  // xHCI
    Package(){0x0015FFFF, 1, 0, 13 },  // xDCI

    Package(){0x0016FFFF, 0, 0, 27 },  // I2C0
    Package(){0x0016FFFF, 1, 0, 28 },  // I2C1
    Package(){0x0016FFFF, 2, 0, 29 },  // I2C2
    Package(){0x0016FFFF, 3, 0, 30 },  // I2C3   // usage note: taking this line as an example, device 0x16, function 3 will use apic irq 30

    Package(){0x0017FFFF, 0, 0, 31 },  // I2C4
    Package(){0x0017FFFF, 1, 0, 32 },  // I2C5
    Package(){0x0017FFFF, 2, 0, 33 },  // I2C6
    Package(){0x0017FFFF, 3, 0, 34 },  // I2C7

    Package(){0x0018FFFF, 0, 0, 4 },   // UART1
    Package(){0x0018FFFF, 1, 0, 5 },   // UART2
    Package(){0x0018FFFF, 2, 0, 6 },   // UART3
    Package(){0x0018FFFF, 3, 0, 7 },   // UART4

    Package(){0x0019FFFF, 0, 0, 35 },  // SPI1
    Package(){0x0019FFFF, 1, 0, 36 },  // SPI2
    Package(){0x0019FFFF, 2, 0, 37 },  // SPI3

    Package(){0x001BFFFF, 0, 0, 3 },   // SDCard
    Package(){0x001CFFFF, 0, 0, 39 },  // eMMC
    Package(){0x001EFFFF, 0, 0, 42 },  // SDIO

    Package(){0x001FFFFF, 0, 0, 20 }   // SMBus
  })

  Name(PR04, Package(){
// PCIE Port #1 Slot
    Package(){0x0000FFFF, 0, LNKG, 0 },
    Package(){0x0000FFFF, 1, LNKH, 0 },
    Package(){0x0000FFFF, 2, LNKE, 0 },
    Package(){0x0000FFFF, 3, LNKF, 0 },
  })

  Name(AR04, Package(){
// PCIE Port #1 Slot
    Package(){0x0000FFFF, 0, 0, 22 },
    Package(){0x0000FFFF, 1, 0, 23 },
    Package(){0x0000FFFF, 2, 0, 20 },
    Package(){0x0000FFFF, 3, 0, 21 },
  })

  Name(PR05, Package(){
// PCIE Port #2 Slot
    Package(){0x0000FFFF, 0, LNKH, 0 },
    Package(){0x0000FFFF, 1, LNKE, 0 },
    Package(){0x0000FFFF, 2, LNKF, 0 },
    Package(){0x0000FFFF, 3, LNKG, 0 },
  })

  Name(AR05, Package(){
// PCIE Port #2 Slot
    Package(){0x0000FFFF, 0, 0, 23 },
    Package(){0x0000FFFF, 1, 0, 20 },
    Package(){0x0000FFFF, 2, 0, 21 },
    Package(){0x0000FFFF, 3, 0, 22 },
  })

  Name(PR06, Package(){
// PCIE Port #3 Slot
    Package(){0x0000FFFF, 0, LNKE, 0 },
    Package(){0x0000FFFF, 1, LNKF, 0 },
    Package(){0x0000FFFF, 2, LNKG, 0 },
    Package(){0x0000FFFF, 3, LNKH, 0 },
  })

  Name(AR06, Package(){
// PCIE Port #3 Slot
    Package(){0x0000FFFF, 0, 0, 20 },
    Package(){0x0000FFFF, 1, 0, 21 },
    Package(){0x0000FFFF, 2, 0, 22 },
    Package(){0x0000FFFF, 3, 0, 23 },
  })

  Name(PR07, Package(){
// PCIE Port #4 Slot
    Package(){0x0000FFFF, 0, LNKF, 0 },
    Package(){0x0000FFFF, 1, LNKG, 0 },
    Package(){0x0000FFFF, 2, LNKH, 0 },
    Package(){0x0000FFFF, 3, LNKE, 0 },
  })

  Name(AR07, Package(){
// PCIE Port #4 Slot
    Package(){0x0000FFFF, 0, 0, 21 },
    Package(){0x0000FFFF, 1, 0, 22 },
    Package(){0x0000FFFF, 2, 0, 23 },
    Package(){0x0000FFFF, 3, 0, 20 },
  })

//---------------------------------------------------------------------------
// List of IRQ resource buffers compatible with _PRS return format.
//---------------------------------------------------------------------------
// Naming legend:
// RSxy, PRSy - name of the IRQ resource buffer to be returned by _PRS, "xy" - last two characters of IRQ Link name.
// Note. PRSy name is generated if IRQ Link name starts from "LNK".
// HLxy , LLxy - reference names, can be used to access bit mask of available IRQs. HL and LL stand for active High(Low) Level triggered Irq model.
//---------------------------------------------------------------------------
  Name(PRSA, ResourceTemplate(){  // Link name: LNKA
    IRQ(Level, ActiveLow, Shared, LLKA) {3,4,5,6,10,11,12,14,15}
  })
  Alias(PRSA,PRSB)  // Link name: LNKB
  Alias(PRSA,PRSC)  // Link name: LNKC
  Alias(PRSA,PRSD)  // Link name: LNKD
  Alias(PRSA,PRSE)  // Link name: LNKE
  Alias(PRSA,PRSF)  // Link name: LNKF
  Alias(PRSA,PRSG)  // Link name: LNKG
  Alias(PRSA,PRSH)  // Link name: LNKH
//---------------------------------------------------------------------------
// Begin PCI tree object scope
//---------------------------------------------------------------------------

  Device(PCI0) { // PCI Bridge "Host Bridge"
    Name(_HID, EISAID("PNP0A08"))  // Indicates PCI Express/PCI-X Mode2 host hierarchy
    Name(_CID, EISAID("PNP0A03"))  // To support legacy OS that doesn't understand the new HID
    Name(_ADR, 0x00000000)
    Method(^BN00, 0) { return(0x0000) }  // Returns default Bus number for Peer PCI buses. Name can be overriden with control method placed directly under Device scope
    Method(_BBN, 0) { return(BN00()) }   // Bus number, optional for the Root PCI Bus
    Name(_UID, 0x0000)  // Unique Bus ID, optional

    Method(_PRT,0) {
      If(PICM) {Return(AR00)} // APIC mode
      Return (PR00)           // PIC Mode
    } // end _PRT
    include("HostBus.asl")
  } // end PCI0 Bridge "Host Bridge"
} // end _SB scope
