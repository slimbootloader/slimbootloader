/** @file
  ACPI DSDT table

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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

Scope (\_SB.PC00) {
  //
  // Create SPI Bus Resource Descriptor for _CRS Usage
  // Arg0 - SPI Controller Number (Resource Source)
  // Arg1 - SPI Clock Phase Keyword of the Connection (Clock Phase)
  // Arg2 - SPI Bus Speed of the Connection (Connection Speed)
  // Returns Buffer with 'SPISerialBus' Resource Descriptor
  //
  Method (SPIM, 3, Serialized) {
    Switch (ToInteger (Arg0)) {
      Case (SERIAL_IO_SPI1) { Name (IIC7 , ResourceTemplate () { SPISerialBus (0,PolarityLow,FourWireMode,8,ControllerInitiated,10000000,ClockPolarityLow,ClockPhaseFirst,"\\_SB.PC00.SPI1",,,DEV7) })
        CreateByteField (IIC7, DEV7._PHA, DPH7)
        CreateDWordField (IIC7,DEV7._SPE,DSP7)
        Store (Arg1, DPH7)
        Store (Arg2, DSP7)
        Return (IIC7) }
      Case (SERIAL_IO_SPI2) { Name (IIC8 , ResourceTemplate () { SPISerialBus (0,PolarityLow,FourWireMode,8,ControllerInitiated,10000000,ClockPolarityLow,ClockPhaseFirst,"\\_SB.PC00.SPI2",,,DEV8) })
        CreateByteField (IIC8, DEV8._PHA, DPH8)
        CreateDWordField (IIC8,DEV8._SPE,DSP8)
        Store (Arg1, DPH8)
        Store (Arg2, DSP8)
        Return (IIC8) }
      Default {Return (EMPTY_RESOURCE_TEMPLATE)}
    }
  } // Method SPIM
} // Scope SB.PC00

//-----------------------------
//  Serial IO SPI1 Controller
//-----------------------------
Scope (\_SB.PC00.SPI1)
{
  Name (SPIP, 0)
  Name (SPIX, 0)
  Method (_INI) {
    Store (SDS7, SPIP)
    Store (SERIAL_IO_SPI1, SPIX)
  }
}

//-----------------------------
//  Serial IO SPI2 Controller
//-----------------------------
Scope (\_SB.PC00.SPI2)
{
  Name (SPIP, 0)
  Name (SPIX, 0)
  Method (_INI) {
    Store (SDS8, SPIP)
    Store (SERIAL_IO_SPI2, SPIX)
  }
}

