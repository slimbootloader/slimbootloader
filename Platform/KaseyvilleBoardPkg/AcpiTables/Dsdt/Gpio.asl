/**@file
 ACPI definition for GPIO controller

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope(\_SB) {
  //----------------------------
  //  GPIO Controller
  //----------------------------
  Device (GPI0) {
    Method (_HID) {
      Return(GPIO_IBL_ACPI_HID)
    }

    Method (_CID) {
      Return(GPIO_IBL_ACPI_CID)
    }

    Method (_CRS, 0x0, Serialized) {
      //
      // In this resource descriptor we only return GPIO memory ranges that are
      // going to be used by GPIO OS driver. All communities that
      // do not contain GPIO pads that can be used in GPIO mode
      // are defined as motherboard restricted resource to prevent
      // OS from accessing them.
      //
      Name (RBUF, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBR0)
        Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQ) { 14 } //Interrupt IRQ_EN
      })

      CreateDWordField(RBUF,RBR0._BAS,COM0)
      CreateDWordField(RBUF,IRQ._INT,IRQN)
      Store( Add(SBRG,IBL_GPIO_COM), COM0)
      Store(SGIR,IRQN)

      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized) {
      If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x0)
      }
      Return(0xF)
    }
  } // END Device(GPIO)
} // END Scope(\_SB.PCI0)
