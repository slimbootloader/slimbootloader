/**@file

  Serial IO SPI Controllers ACPI definitions

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define SERIAL_IO_SPI_DISABLED 0
#define SERIAL_IO_SPI_PCI      1
#define SERIAL_IO_SPI_HIDDEN   2

Scope(\_SB.PC00) {
  //
  //  SPI Hidden Resource allocation
  //  Returns resource buffer with memory ranges used but not explicitely claimed by the device
  //
  //  @param[in]  Arg0      Pci Config Base
  //
  //  @retval               Resource buffer with memory ranges
  //
  Method (SPIH, 1, Serialized) {
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
  } // End SPIH

//-------------------------------------------
//  Serial IO SPI Controller 0 Configuration
//-------------------------------------------
  Device (SPI0) {
    If(LEqual(SPI0_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI0_PCIE_BASE)) }
      Name (_STA, 0xB)
    }
    If(LEqual(SPI0_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer(){0})}
      Method (_PS3) { SOD3 (SPI0_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI0_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI0_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) { Return (SERIAL_IO_SPI0_ADR) }
    }
  }
//------------------------------------------
//  Serial IO SPI Controller 1 Configuration
//------------------------------------------
  Device (SPI1) {
    If (LEqual(SPI1_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI1_PCIE_BASE)) }
      Name (_STA, 0xB)
    }
    If (LEqual(SPI1_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer(){0})}
      Method (_PS3) { SOD3 (SPI1_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI1_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI1_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) { Return (SERIAL_IO_SPI1_ADR) }
    }
  }
//------------------------------------------
//  Serial IO SPI Controller 2 Configuration
//------------------------------------------
  Device (SPI2) {
    If (LEqual (SPI2_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI2_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (SPI2_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (SPI2_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI2_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI2_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_SPI2_ADR)
      }
    }
  }
#ifdef PCH_TGL
//------------------------------------------
//  Serial IO SPI Controller 3 Configuration
//------------------------------------------
  Device (SPI3) {
    If (LEqual (SPI3_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI3_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (SPI3_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (SPI3_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI3_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI3_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_SPI3_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO SPI Controller 4 Configuration
//------------------------------------------
  Device (SPI4) {
    If (LEqual (SPI4_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI4_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (SPI4_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (SPI4_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI4_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI4_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_SPI4_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO SPI Controller 5 Configuration
//------------------------------------------
  Device (SPI5) {
    If (LEqual (SPI5_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI5_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (SPI5_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (SPI5_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI5_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI5_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_SPI5_ADR)
      }
    }
  }
//------------------------------------------
//  Serial IO SPI Controller 6 Configuration
//------------------------------------------
  Device (SPI6) {
    If (LEqual (SPI6_MODE, SERIAL_IO_SPI_HIDDEN)) {
      Method (_CRS) { Return (SPIH (SPI6_PCIE_BASE)) }
      Name(_STA, 0xB)
    }
    If (LEqual (SPI6_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
      Method (_PS3) { SOD3 (SPI6_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (SPI6_MODE, SERIAL_IO_SPI_PCI), LEqual (SPI6_MODE, SERIAL_IO_SPI_DISABLED))) {
      Method (_ADR) {
        Return (SERIAL_IO_SPI6_ADR)
      }
    }
  }
#endif //PCH_TGL
} //End Scope(\_SB.PC00)

