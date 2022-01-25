/**@file
 ACPI definition for GPIO controller

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <GpioAcpiDefinesTgl.h>

Scope(\_SB) {
  //----------------------------
  //  GPIO Controller
  //----------------------------
  Device (GPI0)
  {
    Method (_HID) {
      // Return motherboard reserved resources HID when GPIO is hidden
      if(LEqual(GPHD, 1)) {
        Return("PNP0C02")
      }

      Return(GPIO_VER2_P_ACPI_HID)
    }

    Name (LINK,"\\_SB.GPI0")

    Method (_CRS, 0x0, NotSerialized) {
      Name(RBFL,ResourceTemplate(){
        Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQL) { 14 } //Interrupt IRQ_EN
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL0)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL1)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL2)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL3)
      })

      CreateDWordField(RBFL,IRQL._INT,INTL)
      Store(SGIR,INTL)
      CreateDWordField(RBFL,RBL0._BAS,CML0)
      Store( Add(SBRG,PCH_GPIO_COM0), CML0)
      CreateDWordField(RBFL,RBL1._BAS,CML1)
      Store( Add(SBRG,PCH_GPIO_COM1), CML1)
      CreateDWordField(RBFL,RBL2._BAS,CML4)
      Store( Add(SBRG,PCH_GPIO_COM4), CML4)
      CreateDWordField(RBFL,RBL3._BAS,CML5)
      Store( Add(SBRG,PCH_GPIO_COM5), CML5)

      Return(RBFL)
    }

    Method (_STA, 0x0, NotSerialized) {
      If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x8)
      }
      Return(0xF)
    }
  } // END Device(GPIO)
} // END Scope(\_SB.PC00)

