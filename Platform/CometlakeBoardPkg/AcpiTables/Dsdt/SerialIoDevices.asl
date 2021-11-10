/** @file
  ACPI DSDT table

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(\PMST.PMC4, FieldUnitObj)
External(\_SB.PCI0.I2C1.INC1, MethodObj)

//
//  Define each SerialIO devices these item value must sync SetupVariable.h
//
#define SERIAL_IO_I2C0              0x0
#define SERIAL_IO_I2C1              0x1
#define SERIAL_IO_I2C2              0x2
#define SERIAL_IO_I2C3              0x3
#define SERIAL_IO_I2C4              0x4
#define SERIAL_IO_I2C5              0x5
#define SERIAL_IO_SPI0              0x6
#define SERIAL_IO_SPI1              0x7
#define SERIAL_IO_SPI2              0x8
#define SERIAL_IO_UART0             0x9
#define SERIAL_IO_UART1             0xA
#define SERIAL_IO_UART2             0xB

Scope(\_SB.PCI0) {
  //
  // Create I2C Bus Resource Descriptor for _CRS Usage
  // Arg0 - I2C Controller Number (Resource Source)
  // Arg1 - I2C Bus Address of the Connection (Slave Address)
  // Arg2 - I2C Bus Speed of the Connection (Connection Speed)
  // Returns Buffer with 'I2cSerialBus' Resource Descriptor
  //
  Method (I2CM, 3, Serialized) {
    Switch (ToInteger(Arg0)) {
      Case(SERIAL_IO_I2C0) { Name (IIC0 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PCI0.I2C0", 0x00, ResourceConsumer, DEV0,) })
        CreateWordField (IIC0, DEV0._ADR, DAD0)
        CreateDWordField(IIC0,DEV0._SPE,DSP0)
        Store (Arg1, DAD0)
        Store (Arg2, DSP0)
        Return (IIC0) }
      Case(SERIAL_IO_I2C1) { Name (IIC1 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PCI0.I2C1", 0x00, ResourceConsumer, DEV1,) })
        CreateWordField (IIC1, DEV1._ADR, DAD1)
        CreateDWordField(IIC1,DEV1._SPE,DSP1)
        Store (Arg1, DAD1)
        Store (Arg2, DSP1)
        Return (IIC1) }
      Case(SERIAL_IO_I2C2) { Name (IIC2 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PCI0.I2C2", 0x00, ResourceConsumer, DEV2,) })
        CreateWordField (IIC2, DEV2._ADR, DAD2)
        CreateDWordField(IIC2,DEV2._SPE,DSP2)
        Store (Arg1, DAD2)
        Store (Arg2, DSP2)
        Return (IIC2) }
      Case(SERIAL_IO_I2C3) { Name (IIC3 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PCI0.I2C3", 0x00, ResourceConsumer, DEV3,) })
        CreateWordField (IIC3, DEV3._ADR, DAD3)
        CreateDWordField(IIC3,DEV3._SPE,DSP3)
        Store (Arg1, DAD3)
        Store (Arg2, DSP3)
        Return (IIC3) }
      Case(SERIAL_IO_I2C4) { Name (IIC4 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PCI0.I2C4", 0x00, ResourceConsumer, DEV4,) })
        CreateWordField (IIC4, DEV4._ADR, DAD4)
        CreateDWordField(IIC4,DEV4._SPE,DSP4)
        Store (Arg1, DAD4)
        Store (Arg2, DSP4)
        Return (IIC4) }
      Case(SERIAL_IO_I2C5) { Name (IIC5 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PCI0.I2C5", 0x00, ResourceConsumer, DEV5,) })
        CreateWordField (IIC5, DEV5._ADR, DAD5)
        CreateDWordField(IIC5,DEV5._SPE,DSP5)
        Store (Arg1, DAD5)
        Store (Arg2, DSP5)
        Return (IIC5) }
      Default {Return (0)}
    }
  } // Method I2CM
} // Scope SB.PCI0

//-----------------------------
//  Serial IO I2C0 Controller
//-----------------------------
Scope(\_SB.PCI0.I2C0) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Method(_INI) {
    Store (SDS0, I2CN)
    Store (SERIAL_IO_I2C0, I2CX)
  }
}

//-----------------------------
//  Serial IO I2C1 Controller
//-----------------------------
Scope(\_SB.PCI0.I2C1) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Method(_INI){
    Store (SDS1, I2CN)
    Store (SERIAL_IO_I2C1, I2CX)
    If (CondRefOf(\_SB.PCI0.I2C1.INC1)) {
      INC1()
    }
  }
}

//-----------------------------
//  Serial IO I2C2 Controller
//-----------------------------
Scope(\_SB.PCI0.I2C2) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Method(_INI) {
    Store (SDS2, I2CN)
    Store (SERIAL_IO_I2C2, I2CX)
  }
}

//-----------------------------
//  Serial IO I2C3 Controller
//-----------------------------
Scope(\_SB.PCI0.I2C3) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Method(_INI) {
    Store (SDS3, I2CN)
    Store (SERIAL_IO_I2C3, I2CX)
  }
}

//-----------------------------
//  Serial IO I2C4 Controller
//-----------------------------
Scope(\_SB.PCI0.I2C4) {
  //
  // PAC193x Microchip for Whiskeylake boards
  //
  if (LEqual(PLID, 0x14) || LEqual(PLID, 0x15)) {
    Include ("PAC193x.asl")
  }
}

//-----------------------------
//  Serial IO SPI1 Controller
//-----------------------------
Scope(\_SB.PCI0.SPI1)
{
  Name(SPIP, 0)
  Name(SPIX, 0)
  Method(_INI) {
    Store (SDS7, SPIP)
    Store (SERIAL_IO_SPI1, SPIX)
  }
}

//-----------------------------
//  Serial IO SPI2 Controller
//-----------------------------
Scope(\_SB.PCI0.SPI2)
{
  Name(SPIP, 0)
  Name(SPIX, 0)
  Method(_INI) {
    Store (SDS8, SPIP)
    Store (SERIAL_IO_SPI2, SPIX)
  }
}
