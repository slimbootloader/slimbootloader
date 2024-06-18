/**@file
 ACPI definition for GPIO controller for MTL PCH

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "GpioAcpiDefinesMtlPchS.h"

Scope (\_SB) {

  Device (PGPI)
  {
    Method (_HID) {
      // Return motherboard reserved resources HID when GPIO is hidden
      If (LEqual (GPHD, 1)) {
        Return ("PNP0C02")
      }
      Return (GPIOV2_MTL_PCH_S_HID)
    }

    Name (SBRG, 0)
    Store (\_SB.PC02.SBRG, SBRG)

    Name (GEI0, 0)
    Store (\_SB.PC02.GEI0, GEI0)
    Name (GEI1, 0)
    Store (\_SB.PC02.GEI1, GEI1)
    Name (GEI2, 0)
    Store (\_SB.PC02.GEI2, GEI2)
    Name (GED0, 0)
    Store (\_SB.PC02.GED0, GED0)
    Name (GED1, 0)
    Store (\_SB.PC02.GED1, GED1)
    Name (GED2, 0)
    Store (\_SB.PC02.GED2, GED2)

    Method (_CRS, 0x0, NotSerialized) {
      Name (RBFS,ResourceTemplate () {
        Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , IRQH) { 14 } //Interrupt IRQ_EN
        QWordMemory (
          ResourceConsumer, ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,                // AddressGranularity
          0x0000000000000000, // AddressMinimum _MIN
          0x000000000000FFFF, // AddressMaximum _MAX
          0x0,                // AddressTranslation
          0x10000,            // RangeLength _LEN
          , ,
          RBS0,
          AddressRangeMemory,
          )
        QWordMemory (
          ResourceConsumer, ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,                // AddressGranularity
          0x0000000000000000, // AddressMinimum _MIN
          0x000000000000FFFF, // AddressMaximum _MAX
          0x0,                // AddressTranslation
          0x10000,            // RangeLength _LEN
          , ,
          RBS1,
          AddressRangeMemory,
          )
        QWordMemory (
          ResourceConsumer, ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,                // AddressGranularity
          0x0000000000000000, // AddressMinimum _MIN
          0x000000000000FFFF, // AddressMaximum _MAX
          0x0,                // AddressTranslation
          0x10000,            // RangeLength _LEN
          , ,
          RBS3,
          AddressRangeMemory,
          )
        QWordMemory (
          ResourceConsumer, ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,                // AddressGranularity
          0x0000000000000000, // AddressMinimum _MIN
          0x000000000000FFFF, // AddressMaximum _MAX
          0x0,                // AddressTranslation
          0x10000,            // RangeLength _LEN
          , ,
          RBS4,
          AddressRangeMemory,
          )
        QWordMemory (
          ResourceConsumer, ,
          MinFixed,
          MaxFixed,
          NonCacheable,
          ReadWrite,
          0x0,                // AddressGranularity
          0x0000000000000000, // AddressMinimum _MIN
          0x000000000000FFFF, // AddressMaximum _MAX
          0x0,                // AddressTranslation
          0x10000,            // RangeLength _LEN
          , , RBS5,
          AddressRangeMemory,
          )
      })
      CreateDWordField (RBFS,IRQH._INT,INTH)
      Store (SGIR,INTH)
      CreateQWordField (RBFS, RBS0._MIN, CMN0)
      CreateQWordField (RBFS, RBS0._MAX, CMX0)
      Store (Add (SBRG, MTL_PCH_GPIO_COM0), CMN0)
      Store (Add (CMN0, 0x0000FFFF), CMX0)

      CreateQWordField (RBFS, RBS1._MIN, CMN1)
      CreateQWordField (RBFS, RBS1._MAX, CMX1)
      Store (Add (SBRG, MTL_PCH_GPIO_COM1), CMN1)
      Store (Add (CMN1, 0x0000FFFF), CMX1)

      CreateQWordField (RBFS, RBS3._MIN, CMN3)
      CreateQWordField (RBFS, RBS3._MAX, CMX3)
      Store (Add (SBRG, MTL_PCH_GPIO_COM3), CMN3)
      Store (Add (CMN3, 0x0000FFFF), CMX3)

      CreateQWordField (RBFS, RBS4._MIN, CMN4)
      CreateQWordField (RBFS, RBS4._MAX, CMX4)
      Store (Add (SBRG, MTL_PCH_GPIO_COM4), CMN4)
      Store (Add (CMN4, 0x0000FFFF), CMX4)

      CreateQWordField (RBFS, RBS5._MIN, CMN5)
      CreateQWordField (RBFS, RBS5._MAX, CMX5)
      Store (Add (SBRG, MTL_PCH_GPIO_COM5), CMN5)
      Store (Add (CMN5, 0x0000FFFF), CMX5)

      Return (RBFS)
    }

    Method (_STA, 0x0, NotSerialized) {
      If (LEqual (GPHD, 1)) { // Hide GPIO ACPI device
        Return(0x8)
      }
      Return (0xF)
    }

    Include ("GpioGroupsMtlPch.asl")

    Method (GDSC, 0, NotSerialized) {
      Return (GPCS)
    }

    //
    // Get GPIO maximal group
    // This is internal library function
    //
    Method (GMXG, 0x0, NotSerialized)
    {
      Return (GPIOV2_MTL_PCH_S_GROUP_MAX)
    }

    Include ("GpioLibCommonV2.asl")
  }
}