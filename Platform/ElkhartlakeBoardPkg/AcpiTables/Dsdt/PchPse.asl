/**@file
  ACPI DSDT table for PSE

 Copyright (c) 2019 - 2023 Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(IPCS, MethodObj)  // IPC Command Send (Read/Write)

Scope(\_SB.PC00) {
  //
  // Programmable Service Engine (PCI Mode)
  //
  Device(PSED) {
    Name(_ADR, 0x001d0000)
    OperationRegion(IPCC,PCI_Config,0x00,0x100)
    Field(IPCC,AnyAcc,NoLock,Preserve)
    {
      Offset(0), VDID, 32,
      Offset(0x10), TADL,  32,
      Offset(0x14), TADH,  32,
      Offset(0x84),
          , 15,
      PMES, 1
    }
    Method(_DSM, 0x4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }

      //-------------------------------------------
      //  HID Wake up Event solution
      //-------------------------------------------
      If(CondRefOf(HIWC)) {
        If(HIWC(Arg0)) {
          If(CondRefOf(HIDW)) {
            Return (HIDW(Arg0, Arg1, Arg2, Arg3))
          }
        }
      }

      Return(Buffer() {0})
    } // End _DSM

    Name (_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package (2) {"firmware-name","pseFw.bin"}
        // This is called by Kernel provide the name of shimloader
      }
    })

    Method(_PRW, 0) {
      Return(GPRW(0x68, 5)) // can wakeup from S5 state
    }

    Method (_S0W, 0, NotSerialized) {
      Return (0x03)  // _S0W: S0 Device Wake State
    }

    //
    // PME status from PMCSR for the PSE IPC device
    //
    Method(IPME,0,Serialized) {
      If(LAnd(LNotEqual(VDID,0xFFFFFFFF), LEqual(PMES,1))) {
        Return(0x01)
      }
      Return(0x00)
    }
  } // Device(PSED)

  //
  // TSN0
  //
  Device(OTN0) {
    Name(_ADR, 0x001D0001) // Device 29, Function 1
    OperationRegion(TSRT,PCI_Config,0x00,0x100)
    Field(TSRT,AnyAcc,NoLock,Preserve)
    {
      Offset(0), VDID, 32,
      Offset(0x10), TADL,  32,
      Offset(0x14), TADH,  32,
      Offset(0x84),
          , 15,
      PMES, 1
    }
    Method(_PRW, 0) {
      Return(GPRW(0x68, 5)) // can wakeup from S5 state
    }

    //
    // PME status from PMCSR for the PSE TSN0 device
    //
    Method(IPME,0,Serialized) {
      If(LAnd(LNotEqual(VDID,0xFFFFFFFF), LEqual(PMES,1))) {
        Return(0x01)
      }
      Return(0x00)
    }

    Method (_IPC, 0x7,Serialized) {
      Return(IPCS(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6))
    }

    Method(_STX)
    {
      Return(0x0F)
    }
  }

  //
  // TSN1
  //
  Device(OTN1) {
    Name(_ADR, 0x001D0002) // Device 29, Function 2
    OperationRegion(TSRT,PCI_Config,0x00,0x100)
    Field(TSRT,AnyAcc,NoLock,Preserve)
    {
      Offset(0), VDID, 32,
      Offset(0x10), TADL,  32,
      Offset(0x14), TADH,  32,
      Offset(0x84),
          , 15,
      PMES, 1
    }
    Method(_PRW, 0) {
      Return(GPRW(0x68, 5)) // can wakeup from S5 state
    }

    //
    // PME status from PMCSR for the PSE TSN1 device
    //
    Method(IPME,0,Serialized) {
      If(LAnd(LNotEqual(VDID,0xFFFFFFFF), LEqual(PMES,1))) {
        Return(0x01)
      }
      Return(0x00)
    }

    Method (_IPC, 0x7,Serialized) {
       Return(IPCS(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6))
    }

    Method(_STX)
    {
      Return(0x0F)
    }
  }

  //-------------------------------------------
  // PSE DMA0
  //-------------------------------------------
  Device(DMA0) {
    Name (_ADR, 0x001D0003) // Device 29, Function 3
    Name (_HID, "80864BB4")
    Name (_UID, Zero)
    Method (_STA, 0, NotSerialized)
    {
      Return(0x00)
    }
  } // Device(DMA0)

  //-------------------------------------------
  // PSE DMA1
  //-------------------------------------------
  Device(DMA1) {
    Name (_ADR, 0x001D0004) // Device 29, Function 4
    Name (_HID, "80864BB5")
    Name (_UID, Zero)
    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, PD1R)
      Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {35}  // DMA #1 IRQ
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^PD1R._BAS, D1BA)
      CreateDwordField(^RBUF, ^PD1R._LEN, D1LN)
      Store(D10A, D1BA)
      Store(D10L, D1LN)
      Return (RBUF)
    }
    Method (_STA, 0, NotSerialized)
    {
      If(LNotEqual(PD1E, 0)) {
        Return(0x0F)
      }
      Return(0x00)
    }
  } // Device(DMA1)

  //-------------------------------------------
  // PSE DMA2
  //-------------------------------------------
  Device(DMA2) {
    Name (_ADR, 0x001D0005) // Device 29, Function 5
    Name (_HID, "80864BB6")
    Name (_UID, Zero)
    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, PD2R)
      Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {36}  // DMA #2 IRQ
    })
    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^PD2R._BAS, D2BA)
      CreateDwordField(^RBUF, ^PD2R._LEN, D2LN)
      Store(D20A, D2BA)
      Store(D20L, D2LN)
      Return (RBUF)
    }
    Method (_STA, 0, NotSerialized)
    {
      If(LNotEqual(PD2E, 0)) {
        Return(0x0F)
      }
      Return(0x00)
    }
  } // Device(DMA2)

  //-------------------------------------------
  // PSE PWM
  //-------------------------------------------
  Device(OPWM) {
    Name (_ADR, 0x001D0006) // Device 29, Function 6
  } // Device(OPWM)

  //-------------------------------------------
  // PSE OADC
  //-------------------------------------------
  Device(OADC) {
    Name (_ADR, 0x001D0007) // Device 29, Function 7
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x80, 0x0, Width32Bit, )
        FixedDMA(0x81, 0x1, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OADC)

  //-------------------------------------------
  // PSE SPI-0
  //-------------------------------------------
  Device(OSP0) {
    Name (_ADR, 0x00130000) // Device 19, Function 0
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x92, 0x0, Width32Bit, )
        FixedDMA(0x93, 0x1, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OSP0)

  //-------------------------------------------
  // PSE SPI-1
  //-------------------------------------------
  Device(OSP1) {
    Name (_ADR, 0x00130001) // Device 19, Function 1
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x94, 0x2, Width32Bit, )
        FixedDMA(0x95, 0x3, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OSP1)

  //-------------------------------------------
  // PSE SPI-2
  //-------------------------------------------
  Device(OSP2) {
    Name (_ADR, 0x00130002) // Device 19, Function 2
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x96, 0x4, Width32Bit, )
        FixedDMA(0x97, 0x5, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OSP2)

  //-------------------------------------------
  // PSE SPI-3
  //-------------------------------------------
  Device(OSP3) {
    Name (_ADR, 0x00130003) // Device 19, Function 3
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x98, 0x6, Width32Bit, )
        FixedDMA(0x99, 0x7, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OSP3)

  //-------------------------------------------
  // PSE I2C-0
  //-------------------------------------------
  Device(OIC0) {
    Name (_ADR, 0x001B0000) // Device 27, Function 0
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x82, 0x0, Width32Bit, )
        FixedDMA(0x83, 0x1, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC0)

  //-------------------------------------------
  // PSE I2C-1
  //-------------------------------------------
  Device(OIC1) {
    Name (_ADR, 0x001B0001) // Device 27, Function 1
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x84, 0x2, Width32Bit, )
        FixedDMA(0x85, 0x3, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC1)

  //-------------------------------------------
  // PSE I2C-2
  //-------------------------------------------
  Device(OIC2) {
    Name (_ADR, 0x001B0002) // Device 27, Function 2
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x86, 0x4, Width32Bit, )
        FixedDMA(0x87, 0x5, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC2)

  //-------------------------------------------
  // PSE I2C-3
  //-------------------------------------------
  Device(OIC3) {
    Name (_ADR, 0x001B0003) // Device 27, Function 3
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x88, 0x6, Width32Bit, )
        FixedDMA(0x89, 0x7, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC3)

  //-------------------------------------------
  // PSE I2C-4
  //-------------------------------------------
  Device(OIC4) {
    Name (_ADR, 0x001B0004) // Device 27, Function 4
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x8A, 0x0, Width32Bit, )
        FixedDMA(0x8B, 0x1, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC4)

  //-------------------------------------------
  // PSE I2C-5
  //-------------------------------------------
  Device(OIC5) {
    Name (_ADR, 0x001B0005) // Device 29, Function 5
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x8C, 0x2, Width32Bit, )
        FixedDMA(0x8D, 0x3, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC5)

  //-------------------------------------------
  // PSE I2C-6
  //-------------------------------------------
  Device(OIC6) {
    Name (_ADR, 0x001B0006) // Device 27, Function 6
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x8E, 0x4, Width32Bit, )
        FixedDMA(0x8F, 0x5, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC6)

  //-------------------------------------------
  // PSE I2C-7
  //-------------------------------------------
  Device(OIC7) {
    Name (_ADR, 0x00180000) // Device 24, Function 0
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x90, 0x6, Width32Bit, )
        FixedDMA(0x91, 0x7, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIC7)

  //-------------------------------------------
  // PSE CAN-0
  //-------------------------------------------
  Device(CAN0) {
    Name (_ADR, 0x00180001) // Device 24, Function 1

    Method (_STA, 0, Serialized) {
      If (LEqual (\PC0E, 0)) {
        //
        // This will prevent OSPM from running
        // methods like _PSx for this device.
        //
        Return (0)
      } Else {
        //
        // Report as fully functional and to be shown in UI.
        //
        Return (0xF)
      }
    }
    Method(_DSD, 0) {
      Return(
          Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"bosch,mram-cfg", Package () {0x800, 0x80, 0x40, 0x40, 0, 0x40, 0x10, 0x10}}
            }
          }
        )
    }
  } // Device(CAN0)

  //-------------------------------------------
  // PSE CAN-1
  //-------------------------------------------
  Device(CAN1) {
    Name (_ADR, 0x00180002) // Device 24, Function 2
    Method (_STA, 0, Serialized) {
      If (LEqual (\PC1E, 0)) {
        //
        // This will prevent OSPM from running
        // methods like _PSx for this device.
        //
        Return (0)
      } Else {
        //
        // Report as fully functional and to be shown in UI.
        //
        Return (0xF)
      }
    }
    Method(_DSD, 0) {
      Return(
          Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"bosch,mram-cfg", Package () {0x800, 0x80, 0x40, 0x40, 0, 0x40, 0x10, 0x10}}
            }
          }
        )
    }
  } // Device(CAN1)

  //-------------------------------------------
  // PSE QEP-0
  //-------------------------------------------
  Device(QEP0) {
    Name (_ADR, 0x00180003) // Device 24, Function 3
  } // Device(QEP0)

  //-------------------------------------------
  // PSE QEP-1
  //-------------------------------------------
  Device(QEP1) {
    Name (_ADR, 0x00180004) // Device 24, Function 4
  }  // Device(QEP1)

  //-------------------------------------------
  // PSE QEP-2
  //-------------------------------------------
  Device(QEP2) {
    Name (_ADR, 0x00180005) // Device 24, Function 5
  } // Device(QEP2)

  //-------------------------------------------
  // PSE QEP-3
  //-------------------------------------------
  Device(QEP3) {
    Name (_ADR, 0x00180006) // Device 24, Function 6
  } // Device(QEP3)

  //-------------------------------------------
  // PSE GPIO-0
  //-------------------------------------------
  Device(GP00) {
    Name (_ADR, 0x00130004) // Device 19, Function 4
  } // Device(GP00)

  //-------------------------------------------
  // PSE GPIO-1
  //-------------------------------------------
  Device(GP01) {
    Name (_ADR, 0x00130005) // Device 19, Function 5
  } // Device(GP01)

  //-------------------------------------------
  // PSE UART-0
  //-------------------------------------------
  Device(OUA0) {
    Name (_ADR, 0x00110000) // Device 17, Function 0
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x40, 0x0, Width32Bit, )
        FixedDMA(0x41, 0x1, Width32Bit, )
      })
      Return(RBUF)
    }
    Method(_DSD, 0) {
      If(LNotEqual(PUA0, 0)) {
        Return(
          Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"snps,rs485-interface-en", 1},
                Package () {"snps,de-active-high", 1},
            }
          }
        )
      }
    }
  } // Device(OUA0)

  //-------------------------------------------
  // PSE UART-1
  //-------------------------------------------
  Device(OUA1) {
    Name (_ADR, 0x00110001) // Device 17, Function 1
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x42, 0x2, Width32Bit, )
        FixedDMA(0x43, 0x3, Width32Bit, )
      })
      Return(RBUF)
    }
    Method(_DSD, 0) {
      If(LNotEqual(PUA1, 0)) {
        Return(
          Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"snps,rs485-interface-en", 1},
                Package () {"snps,de-active-high", 1},
            }
          }
        )
      }
    }
  } // Device(OUA1)

  //-------------------------------------------
  // PSE UART-2
  //-------------------------------------------
  Device(OUA2) {
    Name (_ADR, 0x00110002) // Device 17, Function 2
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x44, 0x4, Width32Bit, )
        FixedDMA(0x45, 0x5, Width32Bit, )
      })
      Return(RBUF)
    }
    Method(_DSD, 0) {
      If(LNotEqual(PUA2, 0)) {
        Return(
          Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"snps,rs485-interface-en", 1},
                Package () {"snps,de-active-high", 1},
            }
          }
        )
      }
    }
  } // Device(OUA2)

  //-------------------------------------------
  // PSE UART-3
  //-------------------------------------------
  Device(OUA3) {
    Name (_ADR, 0x00110003) // Device 17, Function 3
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x46, 0x6, Width32Bit, )
        FixedDMA(0x47, 0x7, Width32Bit, )
      })
      Return(RBUF)
    }
    Method(_DSD, 0) {
      If(LNotEqual(PUA3, 0)) {
        Return(
          Package () {
            ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
            Package () {
                Package () {"snps,rs485-interface-en", 1},
                Package () {"snps,de-active-high", 1},
            }
          }
        )
      }
    }
  } // Device(OUA3)

  //-------------------------------------------
  // PSE UART-4
  //-------------------------------------------
  Device(OUA4) {
    Name (_ADR, 0x00110004) // Device 17, Function 4
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x48, 0x0, Width32Bit, )
        FixedDMA(0x49, 0x1, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OUA4)

  //-------------------------------------------
  // PSE UART-5
  //-------------------------------------------
  Device(OUA5) {
    Name (_ADR, 0x00110005) // Device 17, Function 5
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x4A, 0x2, Width32Bit, )
        FixedDMA(0x4B, 0x3, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OUA5)

  //-------------------------------------------
  // PSE I2S-0
  //-------------------------------------------
  Device(OIS0) {
    Name (_ADR, 0x00110006) // Device 17, Function 6
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x4C, 0x4, Width32Bit, )
        FixedDMA(0x4D, 0x5, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIS0)

  //-------------------------------------------
  // PSE I2S-1
  //-------------------------------------------
  Device(OIS1) {
    Name (_ADR, 0x00110007) // Device 17, Function 7
    Method(_CRS, 0x0, Serialized)
    {
      Name(RBUF, ResourceTemplate ()
      {
        FixedDMA(0x4E, 0x6, Width32Bit, )
        FixedDMA(0x4F, 0x7, Width32Bit, )
      })
      Return(RBUF)
    }
  } // Device(OIS1)

}
