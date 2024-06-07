/**@file

  Serial IO I2C Controller ACPI definitions for Embedded devices

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

    If (LEqual (I2C_DEVICE_MODE, SERIAL_IO_I2C_PCI)) {
      Method (_DSM,4,Serialized) {if (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer () {0})}
      Method (_PS3) { SOD3 (I2C_DEVICE_PCI_BASE, SERIAL_IO_D3, 0) } // In Embedded, some I2c do not need to have BAR address reset.
      Method (_PS0) {  }
    }
    Method (_ADR) { Return (I2C_DEVICE_ADR) }
