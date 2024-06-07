/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // This file contains methods for UART in hidden and COM mode. Include it into a hidden/com mode UART device.
  // Required defines:
  // UART_DEVICE_MODE - points to a ASL variable holding the mode of the UART
  // UART_DEVICE_PCI_BASE - points to a ASL variable holding the SystemMemory address of UART PCI config space
  // UART_DEVICE_IRQ - IRQ number used by the UART controller. It will be reported to the OS in _CRS
  //


  Method (_HID) {
    Return (UHID (UART_DEVICE_MODE))
  }

  //
  //  Returns UART HID
  //
  //  @param[in]  Arg0      UART mode
  //
  //  @retval               HID value
  //
  Method (UHID, 1, Serialized) {
    If (LEqual (Arg0, SERIAL_IO_UART_COM)) { Return (EISAID ("PNP0501")) }
    Return (EISAID ("PNP0C02"))
  }

  Method (_CRS) {
    Return (UARH (UART_DEVICE_PCI_BASE, UART_DEVICE_IRQ))
  }

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
    OperationRegion (UACF, SystemMemory, Arg0, Add (R_SERIAL_IO_CFG_BAR, 8))
    Field (UACF, AnyAcc, NoLock, Preserve) {
      Offset (R_SERIAL_IO_CFG_BAR),
      BAR0, 64
    }
    Name (BUF0, ResourceTemplate () { Memory32Fixed (ReadWrite, 0, 8, BFR0) }) // UAHx allocates only first 8 bytes
    Name (IBUF, ResourceTemplate () { Interrupt ( ResourceConsumer, Level, ActiveLow, Shared, , , SIRQ) {20}})

    CreateDWordField (BUF0, BFR0._BAS, ADR0)
    CreateDWordField (IBUF, SIRQ._INT, IRQN)

    Store (And (BAR0, 0xFFFFFFFFFFFFF000), Local0)
    Store (Local0, ADR0) // BAR0 first 8 bytes
    Store (Arg1, IRQN)   // Interrupt Number
    ConcatenateResTemplate (BUF0, IBUF, Local1)
    Return (Local1)
  }

  Method (_STA) {
    If (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_COM)) {
      Return (0xF)
    }
    Return (0x8)
  }

    //
  //  Uart _PS3 handler
  //
  //  @param[in]  Arg0      UART PG mode
  //  @param[in]  Arg1      UART Pci Config Base
  //
  Method (UPS3, 2, Serialized) {
    Name (REGS, Package () {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00000000})
    If (LEqual (Arg0, UART_PG_ON)) {
      OperationRegion (UACF, SystemMemory, Arg1, Add (R_SERIAL_IO_CFG_PMECTRLSTATUS, 4))
      Field (UACF, AnyAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_CFG_BAR),
        BAR0, 64
      }
      Field (UACF, ByteAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_CFG_PMECTRLSTATUS),
        PMEC, 8
      }
      OperationRegion (UAB0, SystemMemory, And (BAR0, 0xFFFFFFFFFFFFF000), Add (R_SERIAL_IO_UART_MEM_RESETS_8_BIT, 4))
      Field (UAB0, ByteAcc, Lock, Preserve) {
        DLL, 8,
        DLH, 8,
        FCR, 8,
        LCR, 8,
        MCR, 8,
        Offset (R_SERIAL_IO_UART_MEM_RESETS_8_BIT),
        PPRR, 8
      }
      Field (UAB0, DWordAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_UART_MEM_CLOCKS_8_BIT),
        PCLK, 32
      }
      Store (LCR, Local1)
      // Open Divisor Latch
      Or (LCR, 0x80, LCR) // LCR.DLAB = 1
      Store (DLL, Index (REGS, 0))
      Store (DLH, Index (REGS, 1))
      // Close Divisor Latch
      Store (Local1, LCR)
      Store (FCR, Index (REGS, 2))
      Store (Local1, Index (REGS, 3)) // LCR
      Store (MCR, Index (REGS, 4))
      Store (PCLK, Index (REGS, 5))
      // Check if data is valid
      If (LAnd (LNotEqual (DeRefOf (Index (REGS, 5)), 0), LNotEqual (DeRefOf (Index (REGS, 5)), 0xFFFFFFFF))) {
        URSA (And (BAR0, 0xFFFFFFFFFFFFF000), REGS)
      }
      Store (0x0, PPRR)  // Place In reset state
      Or (PPRR, 0, PPRR)
      Store (0x3, PMEC)
      Or (PMEC, 0, PMEC) // perform a read to avoid ordering and noncoherency problems
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
      OperationRegion (UACF, SystemMemory, Arg1, Add (R_SERIAL_IO_CFG_PMECTRLSTATUS, 4))
      Field (UACF, AnyAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_CFG_BAR),
        BAR0, 64
      }
      Field (UACF, ByteAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_CFG_PMECTRLSTATUS),
        PMEC, 8
      }
      OperationRegion (UAB0, SystemMemory, And (BAR0, 0xFFFFFFFFFFFFF000), Add (R_SERIAL_IO_UART_MEM_RESETS_8_BIT, 4))
      Field (UAB0, ByteAcc, Lock, Preserve) {
        DLL, 8,
        DLH, 8,
        FCR, 8,
        LCR, 8,
        MCR, 8,
        Offset (R_SERIAL_IO_UART_MEM_RESETS_8_BIT),
        PPRR, 8
      }
      Field (UAB0, DWordAcc, NoLock, Preserve) {
        Offset (R_SERIAL_IO_UART_MEM_CLOCKS_8_BIT),
        PCLK, 32
      }
      Store (0x0, PMEC)
      Or (PMEC, 0, PMEC) // perform a read to avoid ordering and noncoherency problems
      Store(0x7, PPRR)  // Out of reset

      Store (URRS (And (BAR0, 0xFFFFFFFFFFFFF000)), Local0)
      //
      // Check if M/N values are 0x0 or FFs
      //
      If (LAnd (LNotEqual (DeRefOf (Index (Local0, 5)), 0), LNotEqual (DeRefOf (Index (Local0, 5)), 0xFFFFFFFF))) {
        // Open Divisor Latch
        Or (LCR, 0x80, LCR) // LCR.DLAB = 1
        Store (DeRefOf (Index (Local0, 0)), DLL)
        Store (DeRefOf (Index (Local0, 1)), DLH)
        // Close Divisor Latch & restore LCR
        Store (DeRefOf (Index (Local0, 3)), LCR)
        Store (DeRefOf (Index (Local0, 2)), FCR)
        Store (DeRefOf (Index (Local0, 4)), MCR)
        // Update Clock
        // CLK_EN =1 (BIT0)
        // CLK_UPDATE = 1 (BIT31)
        Or (DeRefOf (Index (Local0, 5)), 0x80000001, Local2)
        Store (Local2, PCLK)
        // Disable Clock update
        Or (DeRefOf (Index (Local0, 5)), 0x00000001, Local2)
        Store (Local2, PCLK)
      }
    }
    //UART_PG_OFF - no action
  }

  //
  //  Uart _PSC handler
  //
  //  @param[in]  Arg0      UART Pci Config Base
  //
  Method (UPSC, 1, Serialized) {
    OperationRegion (UACF, SystemMemory, Add (Arg0, R_SERIAL_IO_CFG_PMECTRLSTATUS), 4)
    Field (UACF, ByteAcc, NoLock, Preserve) {
      PMEC, 8
    }
    Return (And (PMEC, 0x3))
  }

  If (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_COM)) {
    Method (_PSC) { Return (UPSC (UART_DEVICE_PCI_BASE)) }
    Method (_PS3) { UPS3 (UART_DEVICE_PG, UART_DEVICE_PCI_BASE) }
    Method (_PS0) { UPS0 (UART_DEVICE_PG, UART_DEVICE_PCI_BASE) }
  }