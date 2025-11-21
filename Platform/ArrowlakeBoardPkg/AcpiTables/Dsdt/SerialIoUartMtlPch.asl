/**@file

  Serial IO UART Controllers ACPI definitions

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define UART0_MODE        \_SB.PC02.UM00
#define UART1_MODE        \_SB.PC02.UM01
#define UART2_MODE        \_SB.PC02.UM02
#define UART3_MODE        \_SB.PC02.UM03

#define UART0_PCIE_BASE   \_SB.PC02.UC00
#define UART1_PCIE_BASE   \_SB.PC02.UC01
#define UART2_PCIE_BASE   \_SB.PC02.UC02
#define UART3_PCIE_BASE   \_SB.PC02.UC03

#define UART0_DMA         \_SB.PC02.UD00
#define UART1_DMA         \_SB.PC02.UD01
#define UART2_DMA         \_SB.PC02.UD02
#define UART3_DMA         \_SB.PC02.UD03

#define UART0_IRQ         \_SB.PC02.UI00
#define UART1_IRQ         \_SB.PC02.UI01
#define UART2_IRQ         \_SB.PC02.UI02
#define UART3_IRQ         \_SB.PC02.UI03

#define UART0_PG          \_SB.PC02.UP00
#define UART1_PG          \_SB.PC02.UP01
#define UART2_PG          \_SB.PC02.UP02
#define UART3_PG          \_SB.PC02.UP03

#define UART_DEVICE_ADR       SERIAL_IO_UART0_ADR
#define UART_DEVICE_MODE      UART0_MODE
#define UART_DEVICE_PCI_BASE  UART0_PCIE_BASE
#define UART_DEVICE_DMA       UART0_DMA
#define UART_DEVICE_HIDDEN    UAH0
#define UART_DEVICE_IRQ       UART0_IRQ
#define UART_DEVICE_PG        UART0_PG
If (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_PCI)) {
  Scope (\_SB.PC02) {
    Device (UA00) {
    Include ("SerialIoUartPciController.asl")
  }
}


Scope (\_SB) {
  If (LOr (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_COM))) {
    Device (PUA0) {
      Name (_DDN, "MtlPchSerialIoUart0")
      Name (_UID, 0x60)
      Include ("SerialIoUartHiddenDevice.asl")
    }
  }
}

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART1_ADR
#define UART_DEVICE_MODE      UART1_MODE
#define UART_DEVICE_PCI_BASE  UART1_PCIE_BASE
#define UART_DEVICE_DMA       UART1_DMA
#define UART_DEVICE_HIDDEN    UAH1
#define UART_DEVICE_IRQ       UART1_IRQ
#define UART_DEVICE_PG        UART1_PG
If (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_PCI)) {
  Scope (\_SB.PC02) {
    Device (UA01) {
    Include ("SerialIoUartPciController.asl")
  }
}

Scope (\_SB) {
  If (LOr (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_COM))) {
    Device (PUA1) {
      Name (_DDN, "MtlPchSerialIoUart1")
      Name (_UID, 0x61)
      Include ("SerialIoUartHiddenDevice.asl")
    }
  }
}

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART2_ADR
#define UART_DEVICE_MODE      UART2_MODE
#define UART_DEVICE_PCI_BASE  UART2_PCIE_BASE
#define UART_DEVICE_DMA       UART2_DMA
#define UART_DEVICE_HIDDEN    UAH2
#define UART_DEVICE_IRQ       UART2_IRQ
#define UART_DEVICE_PG        UART2_PG
If (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_PCI)) {
  Scope (\_SB.PC02) {
    Device (UA02) {
    Include ("SerialIoUartPciController.asl")
  }
}

Scope (\_SB) {
  If (LOr (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_COM))) {
    Device (PUA2) {
      Name (_DDN, "MtlPchSerialIoUart2")
      Name (_UID, 0x62)
      Include ("SerialIoUartHiddenDevice.asl")
    }
  }
}

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART3_ADR
#define UART_DEVICE_MODE      UART3_MODE
#define UART_DEVICE_PCI_BASE  UART3_PCIE_BASE
#define UART_DEVICE_DMA       UART3_DMA
#define UART_DEVICE_HIDDEN    UAH3
#define UART_DEVICE_IRQ       UART3_IRQ
#define UART_DEVICE_PG        UART3_PG
If (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_PCI)) {
  Scope (\_SB.PC02) {
    Device (UA03) {
    Include ("SerialIoUartPciController.asl")
  }
}

Scope (\_SB) {
  If (LOr (LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_HIDDEN), LEqual (UART_DEVICE_MODE, SERIAL_IO_UART_COM))) {
    Device (PUA3) {
      Name (_DDN, "MtlPchSerialIoUart3")
      Name (_UID, 0x63)
      Include ("SerialIoUartHiddenDevice.asl")
    }
  }
}

#undef UART0_MODE
#undef UART1_MODE
#undef UART2_MODE
#undef UART0_PCIE_BASE
#undef UART1_PCIE_BASE
#undef UART2_PCIE_BASE
#undef UART0_DMA
#undef UART1_DMA
#undef UART2_DMA
#undef UART0_IRQ
#undef UART1_IRQ
#undef UART2_IRQ
#undef UART0_PG
#undef UART1_PG
#undef UART2_PG
