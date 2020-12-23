/**@file
 ACPI definition for GPIO controller

 Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
Include("GpioAcpiDefinesVer3.h")

Scope(\_SB) {
  //----------------------------
  //  GPIO Controller
  //----------------------------
  Device (GPI0)
  {
    Method (_HID) {
      // Return HID based on PCH Product
      Return(GPIO_VER3_ACPI_HID)
    }

    Name (LINK,"\\_SB.GPI0")

    Method (_CRS, 0x0, Serialized) {
        Name(RBFL,ResourceTemplate(){
          Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQL) { 14 } //Interrupt IRQ_EN
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL0)
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL1)
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL2)
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL3)
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL4)
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL5)
        })

        CreateDWordField(RBFL,IRQL._INT,INTL)
        Store(SGIR,INTL)
        CreateDWordField(RBFL,RBL0._BAS,CML0)
        Store( Add(SBRG,PCH_GPIO_COM0), CML0)
        CreateDWordField(RBFL,RBL1._BAS,CML1)
        Store( Add(SBRG,PCH_GPIO_COM1), CML1)
        CreateDWordField(RBFL,RBL2._BAS,CML2)
        Store( Add(SBRG,PCH_GPIO_COM2), CML2)
        CreateDWordField(RBFL,RBL3._BAS,CML3)
        Store( Add(SBRG,PCH_GPIO_COM3), CML3)
        CreateDWordField(RBFL,RBL4._BAS,CML4)
        Store( Add(SBRG,PCH_GPIO_COM4), CML4)
        CreateDWordField(RBFL,RBL5._BAS,CML5)
        Store( Add(SBRG,PCH_GPIO_COM5), CML5)

        Return(RBFL)
      }

    Method (_STA, 0x0, NotSerialized) {
      If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x3)
      }
      Return(0xF)
    }
  } // END Device(GPIO)
} // END Scope(\_SB)

