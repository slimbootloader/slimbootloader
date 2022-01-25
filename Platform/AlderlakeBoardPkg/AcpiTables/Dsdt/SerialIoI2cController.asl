/**@file

  Serial IO I2C Controller ACPI definitions

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

    If(LEqual(I2C_DEVICE_MODE, SERIAL_IO_I2C_HIDDEN)) {
      Method (_CRS) { Return (I2CH (I2C_DEVICE_PCI_BASE)) }
      Name (_STA, 0x8)
    }
    If(LEqual(I2C_DEVICE_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer(){0})}
      Method (_PS3) { SOD3 (I2C_DEVICE_PCI_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    If (LOr (LEqual (I2C_DEVICE_MODE, SERIAL_IO_I2C_PCI), LEqual (I2C_DEVICE_MODE, SERIAL_IO_I2C_DISABLED))) {
      Method (_ADR) { Return (I2C_DEVICE_ADR) }
    }
