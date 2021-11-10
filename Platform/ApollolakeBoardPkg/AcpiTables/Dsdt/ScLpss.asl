/** @file
  ACPI DSDT table

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

scope (\_SB.PCI0) {
  //
  // LPIO1 PWM
  //
  Device(PWM) {
    Name (_ADR, 0x001A0000)
    Name (_DDN, "Intel(R) PWM Controller")
    Name (_UID, 1)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  }

  //
  // LPIO1 HS-UART #1
  //
  Device(URT1) {
    Name (_ADR, 0x00180000)
    Name (_DDN, "Intel(R) HS-UART Controller #1")
    Name (_UID, 1)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }


  } //  Device (URT1)

  //
  // LPIO1 HS-UART #2
  //
  Device(URT2) {
    Name (_ADR, 0x00180001)
    Name (_DDN, "Intel(R) HS-UART Controller #2")
    Name (_UID, 2)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }

  } // Device (URT2)

  //
  // LPIO1 HS-UART #3
  //
  Device(URT3) {
    Name (_ADR, 0x00180002)
    Name (_DDN, "Intel(R) HS-UART Controller #3")
    Name (_UID, 3)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device (URT3)

  //
  // LPIO1 HS-UART #4
  //
  Device(URT4) {
    Name (_ADR, 0x00180003)
    Name (_DDN, "Intel(R) HS-UART Controller #4")
    Name (_UID, 4)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device (URT4)

  //
  // LPIO1 SPI
  //
  Device(SPI1) {
    Name (_ADR, 0x00190000)
    Name (_DDN, "Intel(R) SPI Controller #1")
    Name (_UID, 1)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device (SPI1)

  //
  // LPIO1 SPI #2
  //
  Device(SPI2) {
    Name (_ADR, 0x00190001)
    Name (_DDN, "Intel(R) SPI Controller #2")
    Name (_UID, 2)

    Name (RBUF, ResourceTemplate ()
    {
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
   } // Device (SPI2)

  //
  // LPIO1 SPI #3
  //
  Device(SPI3) {
    Name (_ADR, 0x00190002)
    Name (_DDN, "Intel(R) SPI Controller #3")
    Name (_UID, 3)

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x8, 0x8, Width32Bit, )
      //FixedDMA(0x9, 0x9, Width32Bit, )
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device (SPI3)


  //
  // LPIO2 I2C #0
  // Connect to AUDIO CODEC, MCSI CAM
  //
  Device(I2C0) {
    Name (_ADR, 0x00160000)
    Name (_DDN, "Intel(R) I2C Controller #0")
    Name (_UID, 1)
    Name (LINK,"\\_SB.PCI0.I2C0") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x10, 0x0, Width32Bit, )
      //FixedDMA(0x11, 0x1, Width32Bit, )
    })
    Name (IC0S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC0S}
      }
    })
    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device(I2C0)

  //
  // LPIO2 I2C #1
  // Connect to NFC
  //
  Device(I2C1) {
    Name (_ADR, 0x00160001)
    Name (_DDN, "Intel(R) I2C Controller #1")
    Name (_UID, 2)
    Name (LINK,"\\_SB.PCI0.I2C1") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
    })
    Name (IC1S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC1S}
      }
    })
    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }

    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device (I2C1)

  //
  // LPIO2 I2C #2
  // Connect to MCSI CAMERA
  //
  Device(I2C2) {
    Name (_ADR, 0x00160002)
    Name (_DDN, "Intel(R) I2C Controller #2")
    Name (_UID, 3)
    Name (LINK,"\\_SB.PCI0.I2C2") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x14, 0x4, Width32Bit, )
      //FixedDMA(0x15, 0x5, Width32Bit, )
    })
    Name (IC2S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC2S}
      }
    })
    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device(I2C2)

  //
  // LPIO2 I2C #3
  // Connect to TOUCH PANEL
  //
  Device(I2C3) {
    Name (_ADR, 0x00160003)
    Name (_DDN, "Intel(R) I2C Controller #3")
    Name (_UID, 4)
    Name (LINK,"\\_SB.PCI0.I2C3") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x16, 0x6, Width32Bit, )
      //FixedDMA(0x17, 0x7, Width32Bit, )
    })
    Name (IC3S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC3S}
      }
    })

    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  }  // Device(I2C3)

  //
  // LPIO2 I2C #4
  // Connect to TOUCH PAD
  //
  Device(I2C4) {
    Name (_ADR, 0x00170000)
    Name (_DDN, "Intel(R) I2C Controller #4")
    Name (_UID, 1)
    Name (LINK,"\\_SB.PCI0.I2C4") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x18, 0x0, Width32Bit, )
      //FixedDMA(0x19, 0x1, Width32Bit, )
    })
    Name (IC4S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC4S}
      }
    })

    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device(I2C4)


  //
  // LPIO2 I2C #5
  // Connect to  SENSOR AIC
  //
  Device(I2C5) {
  Name (_ADR, 0x00170001)
    Name (_DDN, "Intel(R) I2C Controller #5")
    Name (_UID, 2)
    Name (LINK,"\\_SB.PCI0.I2C5") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x1A, 0x02, Width32Bit, )
      //FixedDMA(0x1B, 0x03, Width32Bit, )
    })
    Name (IC5S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC5S}
      }
    })
    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device(I2C5)


  //
  // LPIO2 I2C #6
  // Connect to SENSOR AIC
  //
  Device(I2C6) {
  Name (_ADR, 0x00170002)
    Name (_DDN, "Intel(R) I2C Controller #6")
    Name (_UID, 3)
    Name (LINK,"\\_SB.PCI0.I2C6") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x1C, 0x4, Width32Bit, )
      //FixedDMA(0x1D, 0x5, Width32Bit, )
    })
    Name (IC6S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC6S}
      }
    })
    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device(I2C6)

  //
  // LPIO2 I2C #7
  // Connect to PSS RFID / INA DEVICESx16
  //
  Device(I2C7) {
  Name (_ADR, 0x00170003)
    Name (_DDN, "Intel(R) I2C Controller #7")
    Name (_UID, 4)
    Name (LINK,"\\_SB.PCI0.I2C7") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      //FixedDMA(0x1C, 0x4, Width32Bit, )
      //FixedDMA(0x1D, 0x5, Width32Bit, )
    })
    Name (IC7S,400000)

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"clock-frequency", IC7S}
      }
    })
    Method (FMCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 100, 214, 28 })
      Return (PKG)
    }
    Method (FPCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 38, 80, 12 })
      Return (PKG)
    }
    Method (HSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 5, 24, 12 })
      Return (PKG)
    }
    Method (SSCN, 0x0, Serialized)
    {
      Name (PKG, Package(3) { 580,730, 28})
      Return (PKG)
    }

    Method (_CRS, 0x0, NotSerialized)
    {
      Return (RBUF)
    }
  } // Device(I2C7)
}// PCI0

