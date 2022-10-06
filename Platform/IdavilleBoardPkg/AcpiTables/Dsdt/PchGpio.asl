/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.PC00) {

  Device (GPIO) {
    Name (_HID, "INTC3001")

    Name(BUF0,ResourceTemplate(){
      Memory32Fixed (ReadWrite, 0x00000000, 0x2000, COM0)  //GPIOW -> AddressBase: PCH_PCR_ADDRESS(PID_GPIOCOM0, 0)
      Memory32Fixed (ReadWrite, 0x00000000, 0x2000, COM1)  //GPIOE -> AddressBase: PCH_PCR_ADDRESS(PID_GPIOCOM1, 0) (#include "Register/PchRegsPcr.h")
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared) {0x15}
    })

    Method(_CRS,0,NotSerialized)
    {
      //Patch _BAS and _LEN for Memory32Fixed regions above
      CreateDWordField(BUF0, ^COM0._BAS, CM0B)
      Store(Or(SBRG, 0xC20000), CM0B)
      CreateDWordField(BUF0, ^COM1._BAS, CM1B)
      Store(Or(SBRG, 0xC50000), CM1B)

      Return(BUF0)
    }

    Method (_STA, 0, NotSerialized) {
      If (\_OSI ("Windows 2009")) {
             Return (0xB)
            }
      If (\_OSI ("Windows 2012")) {
             Return (0xB)
            }
      If (\_OSI ("Windows 2013")) {
             Return (0xB)
            }
      If (\_OSI ("Windows 2015")) {
             Return (0xB)
            }
      Else {
        Return (0xf)
      }
    }
  }
}
