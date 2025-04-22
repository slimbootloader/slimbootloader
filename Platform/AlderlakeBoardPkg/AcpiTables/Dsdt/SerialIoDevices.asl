/** @file
  ACPI DSDT table

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Since asl compiler update to 2018 version
// Empty template will casuse warrning
// using this Macro to replace empty ResourceTemplate with End Tag
//
#ifndef EMPTY_RESOURCE_TEMPLATE
#define EMPTY_RESOURCE_TEMPLATE Buffer(){0x79, 0x00}
#endif

//
//  Define each SerialIO devices these item value must sync SetupVariable.h
//
#define SERIAL_IO_I2C_TOUCHPAD      0x1
#define SERIAL_IO_I2C_TOUCHPANEL    0x2
#define SERIAL_IO_I2C_UCMC          0x4
#define SERIAL_IO_I2C_PD02          0x8
#define SERIAL_IO_SPI_FINGERPRINT   0x1
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
#define SERIAL_IO_I2C6              0xC
#define SERIAL_IO_I2C7              0xD

Scope(\_SB.PC00) {

  Name (HIDG, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE")) //GUID for HID-over-I2C
  Name (TP7G, ToUUID("EF87EB82-F951-46DA-84EC-14871AC6F84B")) //GUID for Touch device support in Windows7

  Method(HIDD, 5, Serialized) {
    // Common _DSM method for HID I2C devices
    // Arg0..3 - original arguments to _DSM method, as passed by OS
    // Arg4 - HID descriptor address to be returned from Function1
    If(LEqual(Arg0, HIDG)) {
      If(LEqual(Arg2, Zero)) { // Function 0 : Query Function
        If(LEqual(Arg1, One)) { // Revision 1
          Return (Buffer(One) { 0x03 })
        }
      }
      If(LEqual(Arg2, One)) { // Function 1 : HID Function
        Return (Arg4) // HID Descriptor Address (IHV Specific)
      }
    }
    Return (Buffer(1){0})
  }

  Method(TP7D, 6, Serialized) {
    // Common _DSM method for touch devices support in Win7
    // Arg0..3 - original arguments to _DSM method, as passed by OS
    // Arg4..5 - buffers containing resources (such as I2c bus, interrupt) to be returned from Function1
    If (LEqual(Arg0, TP7G)) {
      If (LEqual(Arg2, Zero)) { // Function 0 : Query Function
        If (LEqual(Arg1, One)) { // Revision 1
          Return(Buffer(One) { 0x03 })
        }
      }
      If (LEqual(Arg2, One)) { // Function 1 : Report resources
        Return (ConcatenateResTemplate(Arg4, Arg5))
      }
    }
    Return (Buffer(1){0})
  }

  //
  // Create I2C Bus Resource Descriptor for _CRS Usage
  // Arg0 - I2C Controller Number (Resource Source)
  // Arg1 - I2C Bus Address of the Connection (Peripheral Address)
  // Arg2 - I2C Bus Speed of the Connection (Connection Speed)
  // Returns Buffer with 'I2cSerialBus' Resource Descriptor
  //
  Method (I2CM, 3, Serialized) {
    Switch (ToInteger(Arg0)) {
      Case(SERIAL_IO_I2C0) { Name (IIC0 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C0", 0x00, ResourceConsumer, DEV0,) })
        CreateWordField (IIC0, DEV0._ADR, DAD0)
        CreateDWordField(IIC0,DEV0._SPE,DSP0)
        Store (Arg1, DAD0)
        Store (Arg2, DSP0)
        Return (IIC0) }
      Case(SERIAL_IO_I2C1) { Name (IIC1 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C1", 0x00, ResourceConsumer, DEV1,) })
        CreateWordField (IIC1, DEV1._ADR, DAD1)
        CreateDWordField(IIC1,DEV1._SPE,DSP1)
        Store (Arg1, DAD1)
        Store (Arg2, DSP1)
        Return (IIC1) }
      Case(SERIAL_IO_I2C2) { Name (IIC2 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C2", 0x00, ResourceConsumer, DEV2,) })
        CreateWordField (IIC2, DEV2._ADR, DAD2)
        CreateDWordField(IIC2,DEV2._SPE,DSP2)
        Store (Arg1, DAD2)
        Store (Arg2, DSP2)
        Return (IIC2) }
      Case(SERIAL_IO_I2C3) { Name (IIC3 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C3", 0x00, ResourceConsumer, DEV3,) })
        CreateWordField (IIC3, DEV3._ADR, DAD3)
        CreateDWordField(IIC3,DEV3._SPE,DSP3)
        Store (Arg1, DAD3)
        Store (Arg2, DSP3)
        Return (IIC3) }
      Case(SERIAL_IO_I2C4) { Name (IIC4 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C4", 0x00, ResourceConsumer, DEV4,) })
        CreateWordField (IIC4, DEV4._ADR, DAD4)
        CreateDWordField(IIC4,DEV4._SPE,DSP4)
        Store (Arg1, DAD4)
        Store (Arg2, DSP4)
        Return (IIC4) }
      Case(SERIAL_IO_I2C5) { Name (IIC5 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C5", 0x00, ResourceConsumer, DEV5,) })
        CreateWordField (IIC5, DEV5._ADR, DAD5)
        CreateDWordField(IIC5,DEV5._SPE,DSP5)
        Store (Arg1, DAD5)
        Store (Arg2, DSP5)
        Return (IIC5) }
      Case(SERIAL_IO_I2C6) { Name (IIC6 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C6", 0x00, ResourceConsumer, DEV6,) })
        CreateWordField (IIC6, DEV6._ADR, DAD6)
        CreateDWordField(IIC6,DEV6._SPE,DSP6)
        Store (Arg1, DAD6)
        Store (Arg2, DSP6)
        Return (IIC6) }
      Case(SERIAL_IO_I2C7) { Name (IIC7 , ResourceTemplate () { I2cSerialBus (0, ControllerInitiated, 400000, AddressingMode7Bit,"\\_SB.PC00.I2C7", 0x00, ResourceConsumer, DEV7,) })
        CreateWordField (IIC7, DEV7._ADR, DAD7)
        CreateDWordField(IIC7,DEV7._SPE,DSP7)
        Store (Arg1, DAD7)
        Store (Arg2, DSP7)
        Return (IIC7) }
      Default {Return (0)}
    }
  } // Method I2CM

  //
  // Create SPI Bus Resource Descriptor for _CRS Usage
  // Arg0 - SPI Controller Number (Resource Source)
  // Arg1 - SPI Clock Phase Keyword of the Connection (Clock Phase)
  // Arg2 - SPI Bus Speed of the Connection (Connection Speed)
  // Returns Buffer with 'SPISerialBus' Resource Descriptor
  //
  Method (SPIM, 3, Serialized) {
    Switch (ToInteger(Arg0)) {
      Case(SERIAL_IO_SPI1) { Name (IIC7 , ResourceTemplate () { SPISerialBus(0,PolarityLow,FourWireMode,8,ControllerInitiated,10000000,ClockPolarityLow,ClockPhaseFirst,"\\_SB.PC00.SPI1",,,DEV7) })
        CreateByteField (IIC7, DEV7._PHA, DPH7)
        CreateDWordField(IIC7,DEV7._SPE,DSP7)
        Store (Arg1, DPH7)
        Store (Arg2, DSP7)
        Return (IIC7) }
      Case(SERIAL_IO_SPI2) { Name (IIC8 , ResourceTemplate () { SPISerialBus(0,PolarityLow,FourWireMode,8,ControllerInitiated,10000000,ClockPolarityLow,ClockPhaseFirst,"\\_SB.PC00.SPI2",,,DEV8) })
        CreateByteField (IIC8, DEV8._PHA, DPH8)
        CreateDWordField(IIC8,DEV8._SPE,DSP8)
        Store (Arg1, DPH8)
        Store (Arg2, DSP8)
        Return (IIC8) }
      Default {Return (EMPTY_RESOURCE_TEMPLATE)}
    }
  } // Method SPIM

  //
  // Create I2C Bus Resource of UCMC descriptor for _CRS usage
  // Arg0 - I2C controller number (Resource Source)
  // Returns buffer with 'I2cSerialBus' resource descriptor
  //
  Method (UCMM, 1, Serialized) {
    Switch (ToInteger(Arg0)) {
      Case(SERIAL_IO_I2C0) {
        Name (UCM0 , ResourceTemplate () {
        I2CSerialBus (0x38, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C0", 0, ResourceConsumer,,)
        I2CSerialBus (0x3F, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C0", 0, ResourceConsumer,,)
        I2CSerialBus (0x20, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C0", 0, ResourceConsumer,,)
        I2CSerialBus (0x27, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C0", 0, ResourceConsumer,,)
        })
        Return (UCM0) }
      Case(SERIAL_IO_I2C1) {
        Name (UCM1 , ResourceTemplate () {
        I2CSerialBus (0x38, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C1", 0, ResourceConsumer,,)
        I2CSerialBus (0x3F, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C1", 0, ResourceConsumer,,)
        I2CSerialBus (0x20, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C1", 0, ResourceConsumer,,)
        I2CSerialBus (0x27, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C1", 0, ResourceConsumer,,)
        })
        Return (UCM1) }
      Case(SERIAL_IO_I2C2) {
        Name (UCM2 , ResourceTemplate () {
        I2CSerialBus (0x21, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C2", 0, ResourceConsumer,,)
        I2CSerialBus (0x25, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C2", 0, ResourceConsumer,,)
        })
        Return (UCM2) }
      Case(SERIAL_IO_I2C3) {
        Name (UCM3 , ResourceTemplate () {
        I2CSerialBus (0x38, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C3", 0, ResourceConsumer,,)
        I2CSerialBus (0x3F, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C3", 0, ResourceConsumer,,)
        I2CSerialBus (0x20, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C3", 0, ResourceConsumer,,)
        I2CSerialBus (0x27, ControllerInitiated, 400000, AddressingMode7Bit, "\\_SB.PC00.I2C3", 0, ResourceConsumer,,)
        })
        Return (UCM3) }
      Default {Return (0)}
    }
  } // Method MCMM

  //
  // Check the I2CN supported UCMC or PD02
  // Arg0 - I2C SerialIo Devices for controller
  // Return: 1 Supported, 0 Not supported
  //
  Method (I2CS, 1) {
    Return (Or (And (Arg0, SERIAL_IO_I2C_UCMC), And (Arg0, SERIAL_IO_I2C_PD02)))
  } // Method I2CS

} // Scope SB.PC00


//-----------------------------
//  Serial IO I2C0 Controller
//-----------------------------
Scope(\_SB.PC00.I2C0) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Name(I2CI, 0)
  Method(_INI) {
    Store (SDS0, I2CN)
    Store (SERIAL_IO_I2C0, I2CX)
  }
}

//-----------------------------
//  Serial IO I2C1 Controller
//-----------------------------
Scope(\_SB.PC00.I2C1) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Name(I2CI, 1)
  Method(_INI){
    Store (SDS1, I2CN)
    Store (SERIAL_IO_I2C1, I2CX)
  }
}

//-----------------------------
//  Serial IO I2C2 Controller
//-----------------------------
Scope(\_SB.PC00.I2C2) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Name(I2CI, 2)
  Method(_INI) {
    Store (SDS2, I2CN)
    Store (SERIAL_IO_I2C2, I2CX)
  }
}

//-----------------------------
//  Serial IO I2C3 Controller
//-----------------------------
Scope(\_SB.PC00.I2C3) {
  Name(I2CN, 0)
  Name(I2CX, 0)
  Name(I2CI, 3)
  Method(_INI) {
    Store (SDS3, I2CN)
    Store (SERIAL_IO_I2C3, I2CX)
  }
}

//-----------------------------
//  Serial IO SPI1 Controller
//-----------------------------
Scope(\_SB.PC00.SPI1)
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
Scope(\_SB.PC00.SPI2)
{
  Name(SPIP, 0)
  Name(SPIX, 0)
  Method(_INI) {
    Store (SDS8, SPIP)
    Store (SERIAL_IO_SPI2, SPIX)
  }
}

