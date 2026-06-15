/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define UART0_MODE        \UM00
#define UART0_IO_PORT     \UC00
#define UART0_IRQ         \UI00

#define UART_DISABLED 0
#define UART_PCI      1
#define UART_HIDDEN   2
#define UART_COM      3
#define UART_SKIP     4

Scope (\_SB)
{
  //
  //  UART Resource allocation
  //  Returns resource buffer for UART in Legacy COM mode
  //
  //  @param[in]  Arg0    IO Port
  //  @param[in]  Arg1    Interrupt Pin Number
  //
  //  @retval             Resource buffer
  //
  Method (UACR, 2, Serialized) {
    Name (IOBF, ResourceTemplate () { IO (Decode16, 0x0FFF, 0x0FFF, 0x01, 0x08, UAIO) })
    Name (INBF, ResourceTemplate () { Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , UAIN) {20} })

    CreateWordField (IOBF, UAIO._MIN, UMIN)
    CreateWordField (IOBF, UAIO._MAX, UMAX)
    CreateDWordField (INBF, UAIN._INT, INTP)
    Store (Arg0, UMIN)
    Store (Arg0, UMAX)
    Store (Arg1, INTP)

    ConcatenateResTemplate (IOBF, INBF, Local1)

    Return (Local1)
  }

  Device (IUR0) {
    Name (_HID, EISAID("PNP0501"))
    Name (_UID, "IblUart0")

    Method (_STA, 0, NotSerialized) {
      If (LOr (LEqual(UART0_MODE, UART_HIDDEN), LEqual (UART0_MODE, UART_COM))) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }

    Method (_CRS, 0x0, Serialized) {
      Return (UACR (UART0_IO_PORT, UART0_IRQ))
    }
  }
}
