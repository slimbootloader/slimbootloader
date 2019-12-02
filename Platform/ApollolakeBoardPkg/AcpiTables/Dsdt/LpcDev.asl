/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

scope (\_SB) {

  Device(RTC)  // RTC
  {
    Name(_HID,EISAID("PNP0B00"))

    Name(_CRS,ResourceTemplate()
    {
      IO(Decode16,0x70,0x70,0x01,0x08)
    })
  }

  Device(HPET)  // High Performance Event Timer
  {
    Name (_HID, EisaId ("PNP0103"))
    Name (_UID, 0x00)
    Method (_STA, 0, NotSerialized)
    {
      Return (0x0F)
    }

    Method (_CRS, 0, Serialized)
    {
      Name (RBUF, ResourceTemplate ()
      {
        Memory32Fixed (ReadWrite,
                       0xFED00000,  // Address Base
                       0x00000400,  // Address Length
                       )
        Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, , , )
        {
          0x00000008,   //0xB HPET-2
        }
     })
     Return (RBUF)
   }
 }
}

Device(IPIC) // 8259 PIC
{
  Name(_HID,EISAID("PNP0000"))

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x20,0x20,0x01,0x02)
    IO(Decode16,0x24,0x24,0x01,0x02)
    IO(Decode16,0x28,0x28,0x01,0x02)
    IO(Decode16,0x2C,0x2C,0x01,0x02)
    IO(Decode16,0x30,0x30,0x01,0x02)
    IO(Decode16,0x34,0x34,0x01,0x02)
    IO(Decode16,0x38,0x38,0x01,0x02)
    IO(Decode16,0x3C,0x3C,0x01,0x02)
    IO(Decode16,0xA0,0xA0,0x01,0x02)
    IO(Decode16,0xA4,0xA4,0x01,0x02)
    IO(Decode16,0xA8,0xA8,0x01,0x02)
    IO(Decode16,0xAC,0xAC,0x01,0x02)
    IO(Decode16,0xB0,0xB0,0x01,0x02)
    IO(Decode16,0xB4,0xB4,0x01,0x02)
    IO(Decode16,0xB8,0xB8,0x01,0x02)
    IO(Decode16,0xBC,0xBC,0x01,0x02)
    IO(Decode16,0x4D0,0x4D0,0x01,0x02)
    IRQNoFlags() {2}
  })
}

Device(LDRC) // LPC Device Resource Consumption
{
  Name(_HID,EISAID("PNP0C02"))

  Name(_UID,2)

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x2E,0x2E,0x1,0x02)      // WPCN381U SIO Config Index + Data.
    IO(Decode16,0x4E,0x4E,0x1,0x02)      // LPC Slot Access.
    IO(Decode16,0x61,0x61,0x1,0x1)       // NMI Status.
    IO(Decode16,0x63,0x63,0x1,0x1)       // Processor I/F.
    IO(Decode16,0x65,0x65,0x1,0x1)       // Processor I/F.
    IO(Decode16,0x67,0x67,0x1,0x1)       // Processor I/F.
    IO(Decode16,0x70,0x70,0x1,0x1)       // NMI Enable.
    IO(Decode16,0x80,0x80,0x1,0x10)      // Postcode.
    IO(Decode16,0x92,0x92,0x1,0x1)       // Processor I/F.
    IO(Decode16,0xB2,0xB2,0x01,0x02)     // Software SMI.
    IO(Decode16,0x680,0x680,0x1,0x20)    // 32 Byte I/O.
    IO(Decode16,0x400,0x400,0x1,0x80)    // ACPI Base.
    IO(Decode16,0x500,0x500,0x1,0xFF)    // GPIO Base.
    IO(Decode16,0x600,0x600,0x1,0x20)    // WPCN381U SIO SWC + ACPI Base.
    IO(Decode16,0x164e,0x164e,0x1,0x02)  // WPCN381U SIO Config Index1 + Data1.
  })
}
Device(TIMR) // 8254 Timer
{
  Name(_HID,EISAID("PNP0100"))

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x40,0x40,0x01,0x04)
    IO(Decode16,0x50,0x50,0x10,0x04)
    IRQNoFlags() {0}
  })
}
