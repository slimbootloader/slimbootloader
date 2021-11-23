/**@file

  Serial IO I2C Controllers ACPI definitions

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define SERIAL_IO_I2C_DISABLED 0
#define SERIAL_IO_I2C_PCI      1
#define SERIAL_IO_I2C_HIDDEN   2

Scope(\_SB.PC00) {
  //
  //  I2C Hidden Resource allocation
  //  Returns resource buffer with memory ranges used but not explicitely claimed by the device
  //
  //  @param[in]  Arg0      Pci Config Base
  //
  //  @retval               Resource buffer with memory ranges
  //
  Method (I2CH, 1, Serialized) {
    OperationRegion (ICB1, SystemMemory, Arg0, Add(R_SERIAL_IO_CFG_BAR0_LOW, 16))
    Field (ICB1, AnyAcc, NoLock, Preserve) {
      Offset(R_SERIAL_IO_CFG_BAR0_LOW),
      BAR0, 64,
      BAR1, 64
    }
    Name (BUF0, ResourceTemplate () { Memory32Fixed (ReadWrite, 0, 0x1000, BFR0) })
    Name (BUF1, ResourceTemplate () { Memory32Fixed (ReadWrite, 0, 0x1000, BFR1) })
    CreateDWordField (BUF0, BFR0._BAS, ADR0)
    CreateDWordField (BUF1, BFR1._BAS, ADR1)
    Store (And (BAR0, 0xFFFFFFFFFFFFF000), ADR0) // BAR0
    Store (And (BAR1, 0xFFFFFFFFFFFFF000), ADR1) // BAR1 - PCI CFG SPACE
    ConcatenateResTemplate (BUF0, BUF1, Local0)
    Return (Local0)
  } // End I2CH

//-------------------------------------------
//  Serial IO I2C Controller 0 Configuration
//-------------------------------------------
  Device (I2C0) {
    If(LEqual(I2C0_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C0_PCIE_BASE)) }
      Name (_STA, 0xB)
    }
    If(LEqual(I2C0_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer(){0})}
      Method (_PS3) { SOD3 (I2C0_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C0_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C0_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) { Return (SERIAL_IO_I2C0_ADR) }
    }
  }
//------------------------------------------
//  Serial IO I2C Controller 1 Configuration
//------------------------------------------
  Device (I2C1) {
    If (LEqual(I2C1_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C1_PCIE_BASE)) }
      Name (_STA, 0xB)
    }
    If (LEqual(I2C1_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer(){0})}
      Method (_PS3) { SOD3 (I2C1_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C1_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C1_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) { Return (SERIAL_IO_I2C1_ADR) }
    }
  }
//------------------------------------------
//  Serial IO I2C Controller 2 Configuration
//------------------------------------------
  Device (I2C2) {
    If (LEqual (I2C2_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C2_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (I2C2_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (I2C2_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C2_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C2_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_I2C2_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO I2C Controller 3 Configuration
//------------------------------------------
  Device (I2C3) {
    If (LEqual (I2C3_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C3_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (I2C3_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (I2C3_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C3_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C3_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_I2C3_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO I2C Controller 4 Configuration
//------------------------------------------
  Device (I2C4) {
    If (LEqual (I2C4_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C4_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (I2C4_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (I2C4_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C4_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C4_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_I2C4_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO I2C Controller 5 Configuration
//------------------------------------------
  Device (I2C5) {
    If (LEqual (I2C5_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C5_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (I2C5_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (I2C5_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C5_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C5_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_I2C5_ADR)
      }
    }
  }
#ifdef PCH_TGL
//------------------------------------------
//  Serial IO I2C Controller 6 Configuration
//------------------------------------------
  Device (I2C6) {
    If (LEqual (I2C6_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C6_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (I2C6_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (I2C6_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C6_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C6_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_I2C6_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO I2C Controller 7 Configuration
//------------------------------------------
  Device (I2C7) {
    If (LEqual (I2C7_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C7_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (I2C7_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (I2C7_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C7_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C7_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_I2C7_ADR)
      }
    }
  }
#endif //PCH_TGL
} //End Scope(\_SB.PC00)

