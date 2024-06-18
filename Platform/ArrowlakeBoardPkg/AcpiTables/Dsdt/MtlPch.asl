/**@file
  MTL PCH ASL description

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
//
#define SERIAL_IO_I2C0_ADR 0x00150000
#define SERIAL_IO_I2C1_ADR 0x00150001
#define SERIAL_IO_I2C2_ADR 0x00150002
#define SERIAL_IO_I2C3_ADR 0x00150003
#define SERIAL_IO_I2C4_ADR 0x00190000
#define SERIAL_IO_I2C5_ADR 0x00190001

#define SERIAL_IO_SPI0_ADR 0x001E0002
#define SERIAL_IO_SPI1_ADR 0x001E0003
#define SERIAL_IO_SPI2_ADR 0x00120006
#define SERIAL_IO_SPI3_ADR 0x00130000

#define SERIAL_IO_UART0_ADR 0x001E0000
#define SERIAL_IO_UART1_ADR 0x001E0001
#define SERIAL_IO_UART2_ADR 0x00190002
#define SERIAL_IO_UART3_ADR 0x00130001

Name (MNVB, 0xFFFF0000)
Name (MNVL, 0xAA55)

Scope (\_SB.PC02) {
  Include ("MtlPchNvs.asl")
  Include ("MtlPchP2Sb.asl")
  Include ("MtlPchPmc.asl")
  Include ("MtlPchSata.asl")
  Include ("MtlPchXhci.asl")
  Include ("MtlPchGbe.asl")
  Include ("MtlPchHda.asl")
  Include ("MtlPchPcie.asl")
  Include ("PchXdci.asl")
  Include ("PchHeci.asl")
  //
  // Serial Io Block
  //
  Include ("SerialIoCommon.asl")
  Include ("SerialIoI2cCommon.asl")
  Include ("SerialIoI2cMtlPch.asl")
  Include ("SerialIoI3cMtlPch.asl")
  Include ("SerialIoSpiCommon.asl")
  Include ("SerialIoSpiMtlPch.asl")
  Include ("SerialIoUartCommon.asl")
  Include ("MtlPchIsh.asl")
}
Include ("SerialIoUartMtlPch.asl")
Include ("MtlPchCnvi.asl")
Include ("MtlPchThc.asl")
Include ("GpioMtlPch.asl")
Include ("IsClkMtlPch.asl")

