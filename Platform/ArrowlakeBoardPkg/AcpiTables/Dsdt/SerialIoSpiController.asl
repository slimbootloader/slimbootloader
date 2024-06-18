/**@file

  Serial IO SPI Controller ACPI definitions

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

    If (LEqual (SPI_DEVICE_MODE, SERIAL_IO_SPI_PCI)) {
      Method (_DSM,4,Serialized) {if (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }; Return (Buffer () {0})}
      Method (_PS3) { SOD3 (SPI_DEVICE_PCI_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
      Method (_PS0) {  }
    }
    Method (_ADR) { Return (SPI_DEVICE_ADR) }
