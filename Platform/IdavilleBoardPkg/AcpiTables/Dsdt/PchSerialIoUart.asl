/**@file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define SERIAL_IO_UART_PCI      1
#define SERIAL_IO_UART_HIDDEN   2
#define SERIAL_IO_UART_COM      3

#define UART0_MODE              \UM00
#define UART1_MODE              \UM01
#define UART2_MODE              \UM02

#define UART0_INT_PIN           \UI00
#define UART1_INT_PIN           \UI01
#define UART2_INT_PIN           \UI02

#define UART0_IO_PORT           \UP00
#define UART1_IO_PORT           \UP01
#define UART2_IO_PORT           \UP02

Scope(\_SB.PC00) {

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

  If (LEqual (UART0_MODE, SERIAL_IO_UART_COM)) {
    Device (IUR0)
    {
      Name (_HID, EISAID("PNP0501"))
      Name (_UID, "SerialIoUart0")
      Method (_STA)
      {
        Return (0xF)
      }
      Method (_CRS)
      {
        Return (UACR (UART0_IO_PORT, UART0_INT_PIN))
      }
    }
  }

  If (LEqual (UART1_MODE, SERIAL_IO_UART_COM)) {
    Device (IUR1)
    {
      Name (_HID, EISAID("PNP0501"))
      Name (_UID, "SerialIoUart1")
      Method (_STA)
      {
        Return (0xF)
      }
      Method (_CRS)
      {
        Return (UACR (UART1_IO_PORT, UART1_INT_PIN))
      }
    }
  }

  If (LEqual (UART2_MODE, SERIAL_IO_UART_COM)) {
    Device (IUR2)
    {
      Name (_HID, EISAID("PNP0501"))
      Name (_UID, "SerialIoUart2")
      Method (_STA)
      {
        Return (0xF)
      }
      Method(_CRS)
      {
        Return (UACR (UART2_IO_PORT, UART2_INT_PIN))
      }
    }
  }

}
