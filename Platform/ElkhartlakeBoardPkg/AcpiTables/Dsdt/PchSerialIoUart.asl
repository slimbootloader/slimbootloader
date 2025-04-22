/**@file

  Serial IO UART Controllers ACPI definitions

  Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "Register/PchRegsSerialIoUart.h"

#define SERIAL_IO_UART_DISABLED 0
#define SERIAL_IO_UART_PCI      1
#define SERIAL_IO_UART_HIDDEN   2
#define SERIAL_IO_UART_COM      3
#define SERIAL_IO_UART_SKIP     4

#define UART_PG_OFF             0
#define UART_PG_ON              1
#define UART_PG_AUTO            2

Scope(\_SB.PC00) {

  //
  //  Checks if UART specific DMA check _DSM is called
  //
  //  @param[in]  Arg0      UART mode
  //
  //  @retval               1 if true, 0 otherwise
  //
  Method (UDSM, 1 ,Serialized) {
    If (LEqual(Arg0, ToUUID ("F7AF8347-A966-49FE-9022-7A9DEEEBDB27"))) {
      return (1)
    }
    return (0)
  }

  //
  //  Uart DMA check _DSM
  //
  //  @param[in]  Arg0      Function Index
  //  @param[in]  Arg1      DMA/PIO support
  //
  //  @retval    case 0     Buffer with coresponding data
  //             case 1     Integer [0:PIO; 1:DMA]
  //             other      Empty Buffer
  //
  Method (UDMA, 2, Serialized) {
    Switch (ToInteger(Arg0)) {
      Case (0) {
        Return (ToBuffer(0x3)) // Supports Function 1.
      }
      // Function 1, Return UART operating mode [0:PIO; 1:DMA]
      Case (1) {
        Return (ToInteger(Arg1))
      }
    } // End Switch statement
    Return (ToBuffer(0x0))
  }

  Device (UA00) {
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (UDSM (Arg0)) { Return (UDMA (Arg2, UART0_DMA)) }
      Return (Buffer(){0})
    }
    Method (_ADR) { Return (SERIAL_IO_UART0_ADR) }
    Method (_PS3) { SOD3 (UART0_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
    Method (_PS0) {  }
  }

  Device (UA01) {
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (UDSM (Arg0)) { Return (UDMA (Arg2, UART1_DMA)) }
      Return (Buffer(){0})
    }
    Method (_ADR) { Return (SERIAL_IO_UART1_ADR) }
    Method (_PS3) { SOD3 (UART1_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
    Method (_PS0) {  }
  }

  Device (UA02) {
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (UDSM (Arg0)) { Return (UDMA (Arg2, UART2_DMA)) }
      Return (Buffer(){0})
    }
    Method (_ADR) { Return (SERIAL_IO_UART2_ADR) }
    Method (_PS3) { SOD3 (UART2_PCIE_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
    Method (_PS0) {  }
  }

} //End Scope(\_SB.PC00)

Scope(\_SB) {
  //
  // Uart Resources
  //
  Device (URSC) {
    Name (_HID, EISAID("PNP0C02"))
    Name (_STA, 0x3)
    Name (_UID, 5)
    //
    //  Allocates remaining memory for Hidden/Com UART devices
    //
    //  @param[in]  Arg0      Mode
    //  @param[in]  Arg1      PciCfgBase
    //
    //  @retval               Resource buffer with memory ranges
    //
    Method (UARB, 2, Serialized) {
      If (LOr (LEqual (Arg0, SERIAL_IO_UART_HIDDEN), LEqual (Arg0, SERIAL_IO_UART_COM))) {
        OperationRegion (UACF, SystemMemory, Arg1, Add (R_SERIAL_IO_CFG_BAR0_LOW, 16))
        Field (UACF, AnyAcc, NoLock, Preserve) {
          Offset(R_SERIAL_IO_CFG_BAR0_LOW),
          BAR0, 64,
          BAR1, 64
        }
        Name (BUF1, ResourceTemplate () { Memory32Fixed (ReadWrite, 0, 0xFF8, BFR1) }) //4096 - 8 bytes, allocate remaining 4088 for BAR0 (MMIO BAR0)
        Name (BUF2, ResourceTemplate () { Memory32Fixed (ReadWrite, 0, 0x1000, BFR2) }) // allocate additional 4096 for BAR1 (PCI CFG SPACE)

        CreateDWordField (BUF1, BFR1._BAS, ADR1)
        CreateDWordField (BUF2, BFR2._BAS, ADR2)

        Store (And (BAR0, 0xFFFFFFFFFFFFF000), Local1)
        Store (Add (Local1, 8), ADR1)                // Add 8 bytes to the address offset. First 8 bytes are allocated by the UAHx _CRS
        Store (And (BAR1, 0xFFFFFFFFFFFFF000), ADR2) // BAR1 (PCI CFG SPACE) Address

        ConcatenateResTemplate (BUF1, BUF2, Local0)
        Return (Local0)
      }
      Return (Buffer(){0x79, 0x00})
    }

    Method (_CRS, 0x0, Serialized) {
      Store (UARB (UART0_MODE, UART0_PCIE_BASE), Local0)
      ConcatenateResTemplate (Local0, UARB (UART0_MODE, UART0_PCIE_BASE), Local1)
      Store (Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART1_MODE, UART1_PCIE_BASE), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART2_MODE, UART2_PCIE_BASE), Local1)
      Store(Local1, Local0)
      Return (Local0)
    }
  } // END Device(URSC)
  //
  //  UART Hidden Resource allocation
  //  Returns resource buffer with memory ranges used but not explicitely claimed by the device
  //
  //  @param[in]  Arg0      Pci Config Base
  //  @param[in]  Arg1      Interrupt number
  //
  //  @retval               Resource buffer with memory ranges
  //
  Method (UARH, 2, Serialized) {
    OperationRegion (UACF, SystemMemory, Arg0, Add(R_SERIAL_IO_CFG_BAR0_LOW, 8))
    Field (UACF, AnyAcc, NoLock, Preserve) {
      Offset (R_SERIAL_IO_CFG_BAR0_LOW),
      BAR0, 64
    }
    Name (BUF0, ResourceTemplate () { Memory32Fixed (ReadWrite, 0, 8, BFR0) }) // UAHx allocates only first 8 bytes
    Name (IBUF, ResourceTemplate () { Interrupt( ResourceConsumer, Level, ActiveLow, Shared, , , SIRQ) {20}})

    CreateDWordField (BUF0, BFR0._BAS, ADR0)
    CreateDWordField (IBUF, SIRQ._INT, IRQN)

    Store (And (BAR0, 0xFFFFFFFFFFFFF000), Local0)
    Store (Local0, ADR0) // BAR0 first 8 bytes
    Store (Arg1, IRQN)   // Interrupt Number
    ConcatenateResTemplate (BUF0, IBUF, Local1)
    Return (Local1)
  }

  //
  //  Uart Power Gating Detection
  //  - If UART is in PCI mode PG will be set to autonomous with _PS0/PS3 support
  //  - Other modes:
  //      If UART_PG_AUTO is applied function checks if LCR is programmed
  //      Value different then 0 means that UART was used by the OS/SMM and PG should be OFF
  //      If it is 0 PG is ON
  //
  //  @param[in]  Arg0      UART mode
  //  @param[in]  Arg1      UART PG mode
  //  @param[in]  Arg2      Pci Config Base
  //
  //  @retval               Final PG mode
  //
  Method (UAPG, 3, Serialized) {
    If (LNotEqual (Arg1, UART_PG_AUTO)) {
      Return (Arg1)
    }
    OperationRegion (UACF, SystemMemory, Arg2, Add (R_SERIAL_IO_CFG_BAR0_LOW, 8))
    Field (UACF, AnyAcc, NoLock, Preserve) {
      Offset(R_SERIAL_IO_CFG_BAR0_LOW),
      BAR0, 64
    }
    OperationRegion (UAB0, SystemMemory, And (BAR0, 0xFFFFFFFFFFFFF000), 0x10)
    Field (UAB0, ByteAcc, Lock, Preserve) {
      DLL, 8,
      DLH, 8,
      FCR, 8,
      LCR, 8
    }
    Store (LCR, Local0)
    If (LAnd (Local0, 0x7F)) {
      Return (UART_PG_OFF) // Disable D0 and D3 transitions
    }
    Return (UART_PG_ON)  // Enable D0 and D3 transitions
  }

  //
  //  Returns UART HID
  //
  //  @param[in]  Arg0      UART mode
  //
  //  @retval               HID value
  //
  Method (UHID, 1, Serialized) {
    If (LEqual (Arg0, SERIAL_IO_UART_COM)) { Return (EISAID("PNP0501")) }
    Return (EISAID("PNP0C02"))
  }

  //
  //  Uart _PS3 handler
  //
  //  @param[in]  Arg0      UART PG mode
  //  @param[in]  Arg1      UART Pci Config Base
  //
  Method (UPS3, 2, Serialized) {
    If (LEqual (Arg0, UART_PG_ON)) {
      OperationRegion (UACF, SystemMemory, Arg1, Add (R_SERIAL_IO_CFG_PME_CTRL_STS, 4))
      Field (UACF, AnyAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_BAR0_LOW),
        BAR0, 64
      }
      Field (UACF, ByteAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_PME_CTRL_STS),
        PMEC, 8
      }
      OperationRegion (UAB0, SystemMemory,  And (BAR0, 0xFFFFFFFFFFFFF000), Add (R_SERIAL_IO_MEM_PPR_RESETS_8BIT, 1))
      Field (UAB0, ByteAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_MEM_PPR_RESETS_8BIT),
        PPRR, 8
      }
      Store (0x0, PPRR)  // Place In reset state
      Store (0x3, PMEC)
    }
    //UART_PG_OFF - no action
  }

  //
  //  Uart _PS0 handler
  //
  //  @param[in]  Arg0      UART PG mode
  //  @param[in]  Arg1      UART Pci Config Base
  //
  Method (UPS0, 2, Serialized) {
    If (LEqual (Arg0, UART_PG_ON)) {
      OperationRegion (UACF, SystemMemory, Arg1, Add (R_SERIAL_IO_CFG_PME_CTRL_STS, 4))
      Field (UACF, AnyAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_BAR0_LOW),
        BAR0, 64
      }
      Field (UACF, ByteAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_PME_CTRL_STS),
        PMEC, 8
      }
      Store (0x0, PMEC)
      OperationRegion (UAB0, SystemMemory,  And (BAR0, 0xFFFFFFFFFFFFF000), Add (R_SERIAL_IO_MEM_PPR_RESETS_8BIT, 1))
      Field (UAB0, ByteAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_MEM_PPR_RESETS_8BIT),
        PPRR, 8
      }
      Store(0x7, PPRR)  // Out of reset
    }
    //UART_PG_OFF - no action
  }

  //
  //  Uart _PSC handler
  //
  //  @param[in]  Arg0      UART Pci Config Base
  //
  Method (UPSC, 1, Serialized) {
    OperationRegion (UACF, SystemMemory, Add (Arg0, R_SERIAL_IO_CFG_PME_CTRL_STS), 4)
    Field (UACF, ByteAcc, NoLock, Preserve) {
      PMEC, 8
    }
    Return (And (PMEC, 0x3))
  }

  If (LOr (LEqual(UART0_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART0_MODE, SERIAL_IO_UART_COM))) {
    Device (UAH0) {
      Name (_DDN, "SerialIoUart0")
      Name (_UID, "SerialIoUart0")
      Method (_HID) { Return (UHID (UART0_MODE)) }
      Method (_CRS) { Return (UARH (UART0_PCIE_BASE, UART0_IRQ)) }
      Method (_STA) {
        If (LEqual (UART0_MODE, SERIAL_IO_UART_COM)) {
          If (LEqual (UART0_PG, UART_PG_AUTO)) {
            Store (UAPG (UART0_MODE, UART0_PG, UART0_PCIE_BASE), UART0_PG)
          }
          Return (0xF)
        }
        Return (0x3)
      }
      If (LEqual (UART0_MODE, SERIAL_IO_UART_COM)) {
        Method (_PSC) { Return (UPSC (UART0_PCIE_BASE)) }
        Method (_PS3) { UPS3 (UART0_PG, UART0_PCIE_BASE) }
        Method (_PS0) { UPS0 (UART0_PG, UART0_PCIE_BASE) }
      }
    }
  }

  If (LOr (LEqual(UART1_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART1_MODE, SERIAL_IO_UART_COM))) {
    Device (UAH1) {
      Name (_DDN, "SerialIoUart1")
      Name (_UID, "SerialIoUart1")
      Method (_HID) { Return (UHID (UART1_MODE)) }
      Method (_CRS) { Return (UARH (UART1_PCIE_BASE, UART1_IRQ)) }
      Method (_STA) {
        If (LEqual (UART1_MODE, SERIAL_IO_UART_COM)) {
          If (LEqual (UART1_PG, UART_PG_AUTO)) {
            Store (UAPG (UART1_MODE, UART1_PG, UART1_PCIE_BASE), UART1_PG)
          }
          Return (0xF)
        }
        Return (0x3)
      }
      If (LEqual (UART1_MODE, SERIAL_IO_UART_COM)) {
        Method (_PSC) { Return (UPSC (UART1_PCIE_BASE)) }
        Method (_PS3) { UPS3 (UART1_PG, UART1_PCIE_BASE) }
        Method (_PS0) { UPS0 (UART1_PG, UART1_PCIE_BASE) }
      }
    }
  }

  If (LOr (LEqual(UART2_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART2_MODE, SERIAL_IO_UART_COM))) {
    Device (UAH2) {
      Name (_DDN, "SerialIoUart2")
      Name (_UID, "SerialIoUart2")
      Method (_HID) { Return (UHID (UART2_MODE)) }
      Method (_CRS) { Return (UARH (UART2_PCIE_BASE, UART2_IRQ)) }
      Method (_STA) {
        If (LEqual (UART2_MODE, SERIAL_IO_UART_COM)) {
          If (LEqual (UART2_PG, UART_PG_AUTO)) {
            Store (UAPG (UART2_MODE, UART2_PG, UART2_PCIE_BASE), UART2_PG)
          }
          Return (0xF)
        }
        Return (0x3)
      }
      If (LEqual (UART2_MODE, SERIAL_IO_UART_COM)) {
        Method (_PSC) { Return (UPSC (UART2_PCIE_BASE)) }
        Method (_PS3) { UPS3 (UART2_PG, UART2_PCIE_BASE) }
        Method (_PS0) { UPS0 (UART2_PG, UART2_PCIE_BASE) }
      }
    }
  }
} //End Scope (\_SB)
