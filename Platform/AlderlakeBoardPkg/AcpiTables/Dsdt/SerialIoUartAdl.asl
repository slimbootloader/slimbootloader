/**@file

  Serial IO UART Controllers ACPI definitions

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define UART0_MODE        UM00
#define UART1_MODE        UM01
#define UART2_MODE        UM02
#define UART3_MODE        UM03
#define UART4_MODE        UM04
#define UART5_MODE        UM05
#define UART6_MODE        UM06

#define UART0_PCIE_BASE   UC00
#define UART1_PCIE_BASE   UC01
#define UART2_PCIE_BASE   UC02
#define UART3_PCIE_BASE   UC03
#define UART4_PCIE_BASE   UC04
#define UART5_PCIE_BASE   UC05
#define UART6_PCIE_BASE   UC06

#define UART0_DMA         UD00
#define UART1_DMA         UD01
#define UART2_DMA         UD02
#define UART3_DMA         UD03
#define UART4_DMA         UD04
#define UART5_DMA         UD05
#define UART6_DMA         UD06

#define UART0_IRQ         UI00
#define UART1_IRQ         UI01
#define UART2_IRQ         UI02
#define UART3_IRQ         UI03
#define UART4_IRQ         UI04
#define UART5_IRQ         UI05
#define UART6_IRQ         UI06

#define UART0_PG          UP00
#define UART1_PG          UP01
#define UART2_PG          UP02
#define UART3_PG          UP03
#define UART4_PG          UP04
#define UART5_PG          UP05
#define UART6_PG          UP06

Scope(\_SB.URSC) {
    Method (_CRS, 0x0, Serialized) {
      Store (UARB (UART0_MODE, UART0_PCIE_BASE), Local0)
      ConcatenateResTemplate(Local0, UARB (UART1_MODE, UART1_PCIE_BASE), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART2_MODE, UART2_PCIE_BASE), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART3_MODE, UART3_PCIE_BASE), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART4_MODE, UART4_PCIE_BASE), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART5_MODE, UART5_PCIE_BASE), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, UARB (UART6_MODE, UART6_PCIE_BASE), Local1)
      Store(Local1, Local0)
      Return (Local0)
    }
} // END Device(URSC)


#define UART_DEVICE_ADR       SERIAL_IO_UART0_ADR
#define UART_DEVICE_MODE      UART0_MODE
#define UART_DEVICE_PCI_BASE  UART0_PCIE_BASE
#define UART_DEVICE_DMA       UART0_DMA
#define UART_DEVICE_HIDDEN    UAH0
#define UART_DEVICE_DDN       "SerialIoUart0"
#define UART_DEVICE_IRQ       UART0_IRQ
#define UART_DEVICE_PG        UART0_PG
Scope(\_SB.PC00) {
  Device (UA00) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_DDN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART1_ADR
#define UART_DEVICE_MODE      UART1_MODE
#define UART_DEVICE_PCI_BASE  UART1_PCIE_BASE
#define UART_DEVICE_DMA       UART1_DMA
#define UART_DEVICE_HIDDEN    UAH1
#define UART_DEVICE_DDN       "SerialIoUart1"
#define UART_DEVICE_IRQ       UART1_IRQ
#define UART_DEVICE_PG        UART1_PG
Scope(\_SB.PC00) {
  Device (UA01) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_DDN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART2_ADR
#define UART_DEVICE_MODE      UART2_MODE
#define UART_DEVICE_PCI_BASE  UART2_PCIE_BASE
#define UART_DEVICE_DMA       UART2_DMA
#define UART_DEVICE_HIDDEN    UAH2
#define UART_DEVICE_DDN       "SerialIoUart2"
#define UART_DEVICE_IRQ       UART2_IRQ
#define UART_DEVICE_PG        UART2_PG
Scope(\_SB.PC00) {
  Device (UA02) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_DDN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART3_ADR
#define UART_DEVICE_MODE      UART3_MODE
#define UART_DEVICE_PCI_BASE  UART3_PCIE_BASE
#define UART_DEVICE_DMA       UART3_DMA
#define UART_DEVICE_HIDDEN    UAH3
#define UART_DEVICE_DDN       "SerialIoUart3"
#define UART_DEVICE_IRQ       UART3_IRQ
#define UART_DEVICE_PG        UART3_PG
Scope(\_SB.PC00) {
  Device (UA03) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_DDN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART4_ADR
#define UART_DEVICE_MODE      UART4_MODE
#define UART_DEVICE_PCI_BASE  UART4_PCIE_BASE
#define UART_DEVICE_DMA       UART4_DMA
#define UART_DEVICE_HIDDEN    UAH4
#define UART_DEVICE_DDN       "SerialIoUart4"
#define UART_DEVICE_IRQ       UART4_IRQ
#define UART_DEVICE_PG        UART4_PG
Scope(\_SB.PC00) {
  Device (UA04) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_DDN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART5_ADR
#define UART_DEVICE_MODE      UART5_MODE
#define UART_DEVICE_PCI_BASE  UART5_PCIE_BASE
#define UART_DEVICE_DMA       UART5_DMA
#define UART_DEVICE_HIDDEN    UAH5
#define UART_DEVICE_DDN       "SerialIoUart5"
#define UART_DEVICE_IRQ       UART5_IRQ
#define UART_DEVICE_PG        UART5_PG
Scope(\_SB.PC00) {
  Device (UA05) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")

#undef UART_DEVICE_ADR
#undef UART_DEVICE_MODE
#undef UART_DEVICE_PCI_BASE
#undef UART_DEVICE_DMA
#undef UART_DEVICE_HIDDEN
#undef UART_DEVICE_DDN
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART6_ADR
#define UART_DEVICE_MODE      UART6_MODE
#define UART_DEVICE_PCI_BASE  UART6_PCIE_BASE
#define UART_DEVICE_DMA       UART6_DMA
#define UART_DEVICE_HIDDEN    UAH6
#define UART_DEVICE_DDN       "SerialIoUart6"
#define UART_DEVICE_IRQ       UART6_IRQ
#define UART_DEVICE_PG        UART6_PG
Scope(\_SB.PC00) {
  Device (UA06) {
Include ("SerialIoUartPciController.asl")
}
Include ("SerialIoUartHiddenController.asl")
