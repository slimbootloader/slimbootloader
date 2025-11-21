/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define UART0_MODE        UM00
#define UART1_MODE        UM01
#define UART2_MODE        UM02

#define UART0_PCIE_BASE   UC00
#define UART1_PCIE_BASE   UC01
#define UART2_PCIE_BASE   UC02

#define UART0_DMA         UD00
#define UART1_DMA         UD01
#define UART2_DMA         UD02

#define UART0_IRQ         UI00
#define UART1_IRQ         UI01
#define UART2_IRQ         UI02

#define UART0_PG          UP00
#define UART1_PG          UP01
#define UART2_PG          UP02

#define UART_UID_BASE     0x50

Scope (\_SB.URSC) {
    Method (_CRS, 0x0, Serialized) {
      Store (UARB (UART0_MODE, UART0_PCIE_BASE), Local0)
      ConcatenateResTemplate (Local0, UARB (UART1_MODE, UART1_PCIE_BASE), Local1)
      Store (Local1, Local0)
      ConcatenateResTemplate (Local0, UARB (UART2_MODE, UART2_PCIE_BASE), Local1)
      Store (Local1, Local0)
      Return (Local0)
    }
} // END Device (URSC)


#define UART_DEVICE_ADR       SERIAL_IO_UART0_ADR
#define UART_DEVICE_MODE      UART0_MODE
#define UART_DEVICE_PCI_BASE  UART0_PCIE_BASE
#define UART_DEVICE_DMA       UART0_DMA
#define UART_DEVICE_HIDDEN    UAH0
#define UART_DEVICE_DDN       "SerialIoUart0"
#define UART_DEVICE_UID       UART_UID_BASE
#define UART_DEVICE_IRQ       UART0_IRQ
#define UART_DEVICE_PG        UART0_PG
Scope (\_SB.PC00) {
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
#undef UART_DEVICE_UID
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART1_ADR
#define UART_DEVICE_MODE      UART1_MODE
#define UART_DEVICE_PCI_BASE  UART1_PCIE_BASE
#define UART_DEVICE_DMA       UART1_DMA
#define UART_DEVICE_HIDDEN    UAH1
#define UART_DEVICE_DDN       "SerialIoUart1"
#define UART_DEVICE_UID       UART_UID_BASE + 1
#define UART_DEVICE_IRQ       UART1_IRQ
#define UART_DEVICE_PG        UART1_PG
Scope (\_SB.PC00) {
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
#undef UART_DEVICE_UID
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG
#define UART_DEVICE_ADR       SERIAL_IO_UART2_ADR
#define UART_DEVICE_MODE      UART2_MODE
#define UART_DEVICE_PCI_BASE  UART2_PCIE_BASE
#define UART_DEVICE_DMA       UART2_DMA
#define UART_DEVICE_HIDDEN    UAH2
#define UART_DEVICE_DDN       "SerialIoUart2"
#define UART_DEVICE_UID       UART_UID_BASE + 2
#define UART_DEVICE_IRQ       UART2_IRQ
#define UART_DEVICE_PG        UART2_PG
Scope (\_SB.PC00) {
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
#undef UART_DEVICE_UID
#undef UART_DEVICE_IRQ
#undef UART_DEVICE_PG

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
