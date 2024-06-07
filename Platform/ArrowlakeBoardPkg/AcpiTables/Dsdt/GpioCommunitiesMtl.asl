/**@file
  ACPI definition for GPIO controller for ADL

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "GpioAcpiDefinesMtl.h"

Scope (\_SB) {
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
      If (LEqual (PCHS, PCH_S)) {
        Return (GPIO_HID_MTL_SOC_S)
      } Else {
        Return (GPIO_VER6_SOC_M_ACPI_HID)
      }
    }

    Name (LINK,"\\_SB.GPI0")

    Method (_CRS, 0x0, NotSerialized) {
      //
      //  For MTL-SOC-S
      //
      Name (RBPS,ResourceTemplate () {
        Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS0)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS1)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS3)
      })
      CreateDWordField (RBPS,IRQH._INT,INTH)
      Store (SGIR,INTH)
      CreateDWordField (RBPS,RBS0._BAS,CMH0)
      Store (Add (SBRG,PCH_GPIO_COM0), CMH0)
      CreateDWordField (RBPS,RBS1._BAS,CMH1)
      Store (Add (SBRG,PCH_GPIO_COM1), CMH1)
      CreateDWordField (RBPS,RBS3._BAS,CMH3)
      Store (Add (SBRG,PCH_GPIO_COM3), CMH3)

      If (LNotEqual (PCHS, PCH_S)) {
        //
        //  For MTL-SOC-M/P
        //
        Name (RBFS,ResourceTemplate () {
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS4)
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBS5)
        })
        CreateDWordField (RBFS,RBS4._BAS,CMH4)
        Store (Add (SBRG,PCH_GPIO_COM4), CMH4)
        CreateDWordField (RBFS,RBS5._BAS,CMH5)
        Store (Add (SBRG,PCH_GPIO_COM5), CMH5)
        ConcatenateResTemplate (RBPS, RBFS, Local0)
        Return (Local0)
      }
      Return (RBPS)
    }

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x8)
      }
      Return (0xF)
    }
  } // END Device (GPIO)
} // END Scope (\_SB.PC00)
