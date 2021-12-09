/**@file
 ACPI definition for GPIO VER3 controller

  Copyright (c) 2011 - 2021, Intel Corporation. All rights reserved.<BR>
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
      // Return motherboard reserved resources HID when GPIO is hidden
      if(LEqual(GPHD, 1)) {
        Return("PNP0C02")
      }
      // Return HID based on PCH Product
      Return(GPIO_VER3_ACPI_HID)
    }

    If(LEqual(\NGPS,1)) {
      Name (_UID, 0)
    }
    Name (LINK,"\\_SB.GPI0")

    Method (_CRS, 0x0, Serialized) {
      Name(IRQB, ResourceTemplate() {
        Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQL) { 14 } //Interrupt IRQ_EN
      })
      CreateDWordField(IRQB,IRQL._INT,INTL)
      Store(SGIR,INTL)

      Name(OGPB,ResourceTemplate(){
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL0)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL1)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL2)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL3)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL4)
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBL5)
      })
      CreateDWordField(OGPB,RBL0._BAS,CML0)
      Store( Add(SBRG,PCH_GPIO_COM0), CML0)
      CreateDWordField(OGPB,RBL1._BAS,CML1)
      Store( Add(SBRG,PCH_GPIO_COM1), CML1)
      CreateDWordField(OGPB,RBL2._BAS,CML2)
      Store( Add(SBRG,PCH_GPIO_COM2), CML2)
      CreateDWordField(OGPB,RBL3._BAS,CML3)
      Store( Add(SBRG,PCH_GPIO_COM3), CML3)
      CreateDWordField(OGPB,RBL4._BAS,CML4)
      Store( Add(SBRG,PCH_GPIO_COM4), CML4)
      CreateDWordField(OGPB,RBL5._BAS,CML5)
      Store( Add(SBRG,PCH_GPIO_COM5), CML5)

      Name(NGPB,ResourceTemplate(){
        Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBMM)
      })
      CreateDWordField(NGPB,RBMM._BAS,CMH0)
      Store( Add(SBRG,PCH_GPIO_COM0), CMH0)

      If(LEqual(\NGPS,1)) {
        Return(ConcatenateResTemplate(IRQB, NGPB))
      } Else {
        Return(ConcatenateResTemplate(IRQB, OGPB))
      }
    }

    Method (_STA, 0x0, NotSerialized) {
      If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x3)
      }
      Return(0xF)
    }
  } // END Device(GPIO)

  If(LEqual(\NGPS,1)){
    Device (GPI1)
    {
      Method (_HID) {
        // Return HID based on PCH Product
        Return(GPIO_VER3_ACPI_HID)
      }

      Name (_UID, 1)

      Name (LINK,"\\_SB.GPI1")

      Method (_CRS, 0x0, NotSerialized) {
         Name(RBUF,ResourceTemplate(){
          Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBMM)
        })

        CreateDWordField(RBUF,IRQH._INT,INTH)
        Store(SGIR,INTH)
        CreateDWordField(RBUF,RBMM._BAS, CMH1)
        Store( Add(SBRG,PCH_GPIO_COM1),  CMH1)

        Return(RBUF)
       }

      Method (_STA, 0x0, NotSerialized) {
        If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
          Return(0x3)
        }
        Return(0xF)
      }
    } // END Device(GPI1)

    Device (GPI2)
    {
      Method (_HID) {
        // Return HID based on PCH Product
        Return(GPIO_VER3_ACPI_HID)
      }

      Name (_UID, 2)

      Name (LINK,"\\_SB.GPI2")

      Method (_CRS, 0x0, NotSerialized) {
         Name(RBUF,ResourceTemplate(){
          Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBMM)
        })

        CreateDWordField(RBUF,IRQH._INT,INTH)
        Store(SGIR,INTH)
        CreateDWordField(RBUF,RBMM._BAS, CMH2)
        Store( Add(SBRG,PCH_GPIO_COM2),  CMH2)

        Return(RBUF)
      }

      Method (_STA, 0x0, NotSerialized) {
        // This community is powered by DSW
        // and should not be visible in the UI
        Return(0x0)
      }

    } // END Device(GPI2)

    Device (GPI3)
    {
      Method (_HID) {
        // Return HID based on PCH Product
        Return(GPIO_VER3_ACPI_HID)
      }

      Name (_UID, 3)

      Name (LINK,"\\_SB.GPI3")

      Method (_CRS, 0x0, NotSerialized) {
        Name(RBUF,ResourceTemplate(){
          Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBMM)
        })

        CreateDWordField(RBUF,IRQH._INT,INTH)
        Store(SGIR,INTH)
        CreateDWordField(RBUF,RBMM._BAS, CMH3)
        Store( Add(SBRG,PCH_GPIO_COM3),  CMH3)

        Return(RBUF)
      }

      Method (_STA, 0x0, NotSerialized) {
        If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
          Return(0x3)
        }
        Return(0xF)
      }

    } // END Device(GPI3)

    Device (GPI4)
    {
      Method (_HID) {
        // Return HID based on PCH Product
        Return(GPIO_VER3_ACPI_HID)
      }

      Name (_UID, 4)

      Name (LINK,"\\_SB.GPI4")

      Method (_CRS, 0x0, NotSerialized) {
        Name(RBUF,ResourceTemplate(){
          Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBMM)
        })

        CreateDWordField(RBUF,IRQH._INT,INTH)
        Store(SGIR,INTH)
        CreateDWordField(RBUF,RBMM._BAS, CMH4)
        Store( Add(SBRG,PCH_GPIO_COM4),  CMH4)

        Return(RBUF)
      }

      Method (_STA, 0x0, NotSerialized) {
        If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
          Return(0x3)
        }
        Return(0xF)
      }
    } // END Device(GPI4)

    Device (GPI5)
    {
      Method (_HID) {
        // Return HID based on PCH Product
        Return(GPIO_VER3_ACPI_HID)
      }

      Name (_UID, 5)

      Name (LINK,"\\_SB.GPI5")

      Method (_CRS, 0x0, NotSerialized) {
        Name(RBUF,ResourceTemplate(){
          Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
          Memory32Fixed (ReadWrite, 0x00000000, 0x00010000, RBMM)
        })

        CreateDWordField(RBUF,IRQH._INT,INTH)
        Store(SGIR,INTH)
        CreateDWordField(RBUF,RBMM._BAS, CMH5)
        Store( Add(SBRG,PCH_GPIO_COM5),  CMH5)

        Return(RBUF)
      }

      Method (_STA, 0x0, NotSerialized) {
        If(LEqual(GPHD, 1)) { // Hide GPIO ACPI device
          Return(0x3)
        }
        Return(0xF)
      }
    } // END Device(GPI5)
  }
} // END Scope(\_SB)

