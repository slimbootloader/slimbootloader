/**@file
 ACPI definition for GPIO controller for ADL

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <GpioAcpiDefinesAdl.h>

Scope(\_SB) {
  //----------------------------
  //  GPIO Controller
  //----------------------------
  Device (GPI0)
  {
    Method (_HID) {
      // Return motherboard reserved resources HID when GPIO is hidden
      If (LEqual (GPHD, 1)) {
        Return ("PNP0C02")
      }
      Return (GPIO_VER4_S_ACPI_HID)
    }

    Name (LINK,"\\_SB.GPI0")

    Method (_CRS, 0x0, NotSerialized) {
      Name(RBFS,ResourceTemplate(){
        Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS0)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS1)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS3)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS4)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS5)
      })
      CreateDWordField(RBFS,IRQH._INT,INTH)
      Store(SGIR,INTH)
      CreateDWordField(RBFS,RBS0._BAS,CMH0)
      Store( Add(SBRG,PCH_GPIO_COM0), CMH0)
      CreateDWordField(RBFS,RBS1._BAS,CMH1)
      Store( Add(SBRG,PCH_GPIO_COM1), CMH1)
      CreateDWordField(RBFS,RBS3._BAS,CMH3)
      Store( Add(SBRG,PCH_GPIO_COM3), CMH3)
      CreateDWordField(RBFS,RBS4._BAS,CMH4)
      Store( Add(SBRG,PCH_GPIO_COM4), CMH4)
      CreateDWordField(RBFS,RBS5._BAS,CMH5)
      Store( Add(SBRG,PCH_GPIO_COM5), CMH5)
      Return (RBFS)
    }

    Method (_STA, 0x0, NotSerialized) {
      If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x8)
      }
      Return(0xF)
    }
  } // END Device(GPIO)
} // END Scope(\_SB.PC00)
