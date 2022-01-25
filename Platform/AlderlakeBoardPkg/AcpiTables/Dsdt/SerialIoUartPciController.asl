/**@file

  Serial IO UART Pci Controllers ACPI definitions

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If (PCIC (Arg0)) { Return (PCID (Arg0,Arg1,Arg2,Arg3)) }
      If (UDSM (Arg0)) { Return (UDMA (Arg2, UART_DEVICE_DMA)) }
      Return (Buffer(){0})
    }
    Method (_ADR) { Return (UART_DEVICE_ADR) }
    Method (_PS3) { SOD3 (UART_DEVICE_PCI_BASE, SERIAL_IO_D3, SERIAL_IO_BAR_RESET) }
    Method (_PS0) {  }
  }
