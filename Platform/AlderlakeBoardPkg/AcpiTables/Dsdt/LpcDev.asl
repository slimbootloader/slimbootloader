/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/PmcRegs.h>
#include <Register/TcoRegs.h>

Device(HPET)  // High Performance Event Timer
{
  Name(_HID,EISAID("PNP0103"))
  Name(_UID, 0)

  Name(BUF0,ResourceTemplate()
  {
    Memory32Fixed(ReadWrite,0xFED00000,0x400,FED0)
  })

  Method(_STA,0)
  {
    // Show this Device only if the OS is WINXP or beyond.
    If(HPTE)
    {
      Return(0x000F)  // Enabled, do Display.
    }

    Return(0x0000)      // Return Nothing.
  }

  Method(_CRS,0,Serialized)
  {
    If(HPTE)
    {
      // Check if HPETimer Base should be modified.
      CreateDwordField(BUF0,^FED0._BAS,HPT0)
      Store(HPTB,HPT0)
    }

    Return(BUF0)
  }
  OperationRegion(ETDI,SystemMemory,0xFED00000,0x1FF)
  Field(ETDI, AnyAcc, NoLock, Preserve)
  {
    Offset(0),
    GEID, 64,
    Offset(0x10),
    GECF, 64,
    Offset(0x20),
    GEST, 64,
    Offset(0xF0),
    MAIN, 64,
    Offset(0x100),
    TCN0, 8,
    Offset(0x108),
    TCM0, 8,
    Offset(0x120),
    TCN1, 8,
    Offset(0x128),
    TCM1, 8,
  }
}

Device(IPIC)  // 8259 PIC
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


Device(MATH)  // Math Co-Processor
{
  Name(_HID,EISAID("PNP0C04"))

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0xF0,0xF0,0x01,0x01)
    IRQNoFlags() {13}
  })

  //
  // Report device present for LPT-H.
  //
  Method (_STA, 0x0, NotSerialized)
  {
    If(LEqual(PCHS, PCHH)) {
      Return(0x1F)
    } else {
      Return(0x0)
    }
  }
}


Device(LDRC)  // LPC Device Resource Consumption
{
  Name(_HID,EISAID("PNP0C02"))

  Name(_UID,2)

  Name(_CRS,ResourceTemplate()                      // This is for Cougar Point
  {
    IO(Decode16,0x2E,0x2E,0x1,0x02)             // SIO Access.
    IO(Decode16,0x4E,0x4E,0x1,0x02)             // LPC Slot Access.
    IO(Decode16,0x61,0x61,0x1,0x1)               // NMI Status.
    IO(Decode16,0x63,0x63,0x1,0x1)               // Processor I/F.
    IO(Decode16,0x65,0x65,0x1,0x1)               // Processor I/F.
    IO(Decode16,0x67,0x67,0x1,0x1)               // Processor I/F.
    IO(Decode16,0x70,0x70,0x1,0x1)               // NMI Enable.
    IO(Decode16,0x80,0x80,0x1,0x1)               // Port 80h.
    IO(Decode16,0x92,0x92,0x1,0x1)               // Processor I/F.
    IO(Decode16,0xB2,0xB2,0x01,0x02)           // Software SMI.
    IO(Decode16,0x680,0x680,0x1,0x20)        // 32 Byte I/O.
    IO(Decode16,0x164e,0x164e,0x1,0x02)    // 16 Byte I/O.
  })
}

Device(RTC) // RTC
{
  Name(_HID,EISAID("PNP0B00"))

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x70,0x70,0x01,0x08)
    IRQNoFlags() {8}
  })

  Method(_STA) {
    If (LEqual(\STAS,1)){
      Return(0x0F)
    } Else {
      Return(0x00)
    }
  }
}

Device(TIMR)  // 8254 Timer
{
  Name(_HID,EISAID("PNP0100"))

  Name(_CRS,ResourceTemplate()
  {
    IO(Decode16,0x40,0x40,0x01,0x04)
    IO(Decode16,0x50,0x50,0x10,0x04)
    IRQNoFlags() {0}
  })
}

Device(CWDT)
{
  Name(_HID,EISAID("INT3F0D"))
  Name(_CID,EISAID("PNP0C02"))

  Method(_STA,0,Serialized)
  {
    Return(0x0F)
  }

  Method (_CRS, 0x0, NotSerialized) {
    Name (RBUF, ResourceTemplate () {
      IO(Decode16, 0, 0, 0x4, 0x4,OCWD)  // ACPIBASE + OcWdtCtl offset
    })

    CreateWordField(RBUF,OCWD._MIN,OMIN)
    CreateWordField(RBUF,OCWD._MAX,OMAX)

    Add(PMBS,R_ACPI_IO_OC_WDT_CTL,OMIN)
    Add(PMBS,R_ACPI_IO_OC_WDT_CTL,OMAX)

    Return (RBUF)
  }
}
