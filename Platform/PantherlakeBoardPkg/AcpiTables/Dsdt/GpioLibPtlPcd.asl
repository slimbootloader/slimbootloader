/** @file
  GPIO ASL library

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Include ("GpioCommonV2.asl")

//
// GPIO Access Library
//
Scope (\_SB)
{
  //
  // Get GPIO group information
  // This is an internal library function
  //
  Method (GDSC, 0, NotSerialized) {
    Return (GPCS)
  }

  //
  // Get GpioIo Resource Descriptor
  // For possible arguments refer to AcpiGpio.h
  //
  // Arg0 - GPIO pad
  // Arg1 - Shared
  // Arg2 - PinConfig
  // Arg3 - DebounceTimeout
  // Arg4 - DriveStrength
  // Arg5 - IORestriction
  //
  Method (G_IO, 0x6, Serialized)
  {
    //
    // Get Community Index
    //
    Store (GCOM (Arg0), Local0)

    Switch (ToInteger (Local0)) {
      Case (0) {
        Name (GP00, ResourceTemplate (){  GpioIo (Shared, PullDefault, 0, 0, IoRestrictionNone, GPIO_RESOURCE_SOURCE_GPI0,,, G000){0}})
        CreateBitField (GP00, G000._SHR, SHR0)
        Store (Arg1, SHR0)
        CreateByteField (GP00, G000._PPI, PPI0)
        Store (Arg2, PPI0)
        CreateWordField (GP00, G000._DBT, DBT0)
        Store (Arg3, DBT0)
        CreateWordField (GP00, G000._DRS, DRS0)
        Store (Arg4, DRS0)
        CreateField (GP00, G000._IOR, 2, IOR0)
        Store (Arg5, IOR0)
        CreateWordField (GP00, G000._PIN, PIN0)
        Store (GNUM (Arg0), PIN0)
        Return (GP00)
      }
      Case (1) {
        Name (GP01, ResourceTemplate (){  GpioIo (Shared, PullDefault, 0, 0, IoRestrictionNone, GPIO_RESOURCE_SOURCE_GPI1,,, G001){0}})
        CreateBitField (GP01, G001._SHR, SHR1)
        Store (Arg1, SHR1)
        CreateByteField (GP01, G001._PPI, PPI1)
        Store (Arg2, PPI1)
        CreateWordField (GP01, G001._DBT, DBT1)
        Store (Arg3, DBT1)
        CreateWordField (GP01, G001._DRS, DRS1)
        Store (Arg4, DRS1)
        CreateField (GP01, G001._IOR, 2, IOR1)
        Store (Arg5, IOR1)
        CreateWordField (GP01, G001._PIN, PIN1)
        Store (GNUM (Arg0), PIN1)
        Return (GP01)
      }
      Case (2) {
        Name (GP02, ResourceTemplate (){  GpioIo (Shared, PullDefault, 0, 0, IoRestrictionNone, GPIO_RESOURCE_SOURCE_GPI3,,, G002){0}})
        CreateBitField (GP02, G002._SHR, SHR2)
        Store (Arg1, SHR2)
        CreateByteField (GP02, G002._PPI, PPI2)
        Store (Arg2, PPI2)
        CreateWordField (GP02, G002._DBT, DBT2)
        Store (Arg3, DBT2)
        CreateWordField (GP02, G002._DRS, DRS2)
        Store (Arg4, DRS2)
        CreateField (GP02, G002._IOR, 2, IOR2)
        Store (Arg5, IOR2)
        CreateWordField (GP02, G002._PIN, PIN2)
        Store (GNUM (Arg0), PIN2)
        Return (GP02)
      }
      Case (3) {
        Name (GP03, ResourceTemplate (){  GpioIo (Shared, PullDefault, 0, 0, IoRestrictionNone, GPIO_RESOURCE_SOURCE_GPI4,,, G003){0}})
        CreateBitField (GP03, G003._SHR, SHR3)
        Store (Arg1, SHR3)
        CreateByteField (GP03, G003._PPI, PPI3)
        Store (Arg2, PPI3)
        CreateWordField (GP03, G003._DBT, DBT3)
        Store (Arg3, DBT3)
        CreateWordField (GP03, G003._DRS, DRS3)
        Store (Arg4, DRS3)
        CreateField (GP03, G003._IOR, 2, IOR3)
        Store (Arg5, IOR3)
        CreateWordField (GP03, G003._PIN, PIN3)
        Store (GNUM (Arg0), PIN3)
        Return (GP03)
      }
      Case (4) {
        Name (GP04, ResourceTemplate (){  GpioIo (Shared, PullDefault, 0, 0, IoRestrictionNone, GPIO_RESOURCE_SOURCE_GPI5,,, G004){0}})
        CreateBitField (GP04, G004._SHR, SHR4)
        Store (Arg1, SHR4)
        CreateByteField (GP04, G004._PPI, PPI4)
        Store (Arg2, PPI4)
        CreateWordField (GP04, G004._DBT, DBT4)
        Store (Arg3, DBT4)
        CreateWordField (GP04, G004._DRS, DRS4)
        Store (Arg4, DRS4)
        CreateField (GP04, G004._IOR, 2, IOR4)
        Store (Arg5, IOR4)
        CreateWordField (GP04, G004._PIN, PIN4)
        Store (GNUM (Arg0), PIN4)
        Return (GP04)
      }
      Default {Return (0)}
    }

  }

  //
  // Get GpioInt Resource Descriptor
  //
  // Arg0 - GPIO pad
  // Arg1 - Interrupt Mode
  // Arg2 - Interrupt Polarity
  // Arg3 - Interrupt Sharing (_SHR) and Wake Capability
  // Arg4 - PinConfig
  // Arg5 - DebounceTimeout
  //
  Method (G_IN, 0x6, Serialized)
  {
    //
    // Get Community Index
    //
    Store (GCOM (Arg0), Local0)

    Switch (ToInteger (Local0)) {
      Case (0) {
        Name (GP00, ResourceTemplate (){  GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0, GPIO_RESOURCE_SOURCE_GPI0, 0, ResourceConsumer, G000){0}})
        CreateBitField (GP00, G000._MOD, MOD0)
        Store (Arg1, MOD0)
        CreateField (GP00, G000._POL, 2, POL0)
        Store (Arg2, POL0)
        CreateField (GP00, G000._SHR, 2, SHR0)
        Store (Arg3, SHR0)
        CreateByteField (GP00, G000._PPI, PPI0)
        Store (Arg4, PPI0)
        CreateWordField (GP00, G000._DBT, DBT0)
        Store (Arg5, DBT0)
        CreateWordField (GP00, G000._PIN, PIN0)
        Store (GNUM (Arg0), PIN0)
        Return (GP00)
      }
      Case (1) {
        Name (GP01, ResourceTemplate (){  GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0, GPIO_RESOURCE_SOURCE_GPI1, 0, ResourceConsumer, G001){0}})
        CreateBitField (GP01, G001._MOD, MOD1)
        Store (Arg1, MOD1)
        CreateField (GP01, G001._POL, 2, POL1)
        Store (Arg2, POL1)
        CreateField (GP01, G001._SHR, 2, SHR1)
        Store (Arg3, SHR1)
        CreateByteField (GP01, G001._PPI, PPI1)
        Store (Arg4, PPI1)
        CreateWordField (GP01, G001._DBT, DBT1)
        Store (Arg5, DBT1)
        CreateWordField (GP01, G001._PIN, PIN1)
        Store (GNUM (Arg0), PIN1)
        Return (GP01)
      }
      Case (2) {
        Name (GP02, ResourceTemplate (){  GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0, GPIO_RESOURCE_SOURCE_GPI3, 0, ResourceConsumer, G002){0}})
        CreateBitField (GP02, G002._MOD, MOD2)
        Store (Arg1, MOD2)
        CreateField (GP02, G002._POL, 2, POL2)
        Store (Arg2, POL2)
        CreateField (GP02, G002._SHR, 2, SHR2)
        Store (Arg3, SHR2)
        CreateByteField (GP02, G002._PPI, PPI2)
        Store (Arg4, PPI2)
        CreateWordField (GP02, G002._DBT, DBT2)
        Store (Arg5, DBT2)
        CreateWordField (GP02, G002._PIN, PIN2)
        Store (GNUM (Arg0), PIN2)
        Return (GP02)
      }
      Case (3) {
        Name (GP03, ResourceTemplate (){  GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0, GPIO_RESOURCE_SOURCE_GPI4, 0, ResourceConsumer, G003){0}})
        CreateBitField (GP03, G003._MOD, MOD3)
        Store (Arg1, MOD3)
        CreateField (GP03, G003._POL, 2, POL3)
        Store (Arg2, POL3)
        CreateField (GP03, G003._SHR, 2, SHR3)
        Store (Arg3, SHR3)
        CreateByteField (GP03, G003._PPI, PPI3)
        Store (Arg4, PPI3)
        CreateWordField (GP03, G003._DBT, DBT3)
        Store (Arg5, DBT3)
        CreateWordField (GP03, G003._PIN, PIN3)
        Store (GNUM (Arg0), PIN3)
        Return (GP03)
      }
      Case (4) {
        Name (GP04, ResourceTemplate (){  GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullDefault, 0, GPIO_RESOURCE_SOURCE_GPI5, 0, ResourceConsumer, G004){0}})
        CreateBitField (GP04, G004._MOD, MOD4)
        Store (Arg1, MOD4)
        CreateField (GP04, G004._POL, 2, POL4)
        Store (Arg2, POL4)
        CreateField (GP04, G004._SHR, 2, SHR4)
        Store (Arg3, SHR4)
        CreateByteField (GP04, G004._PPI, PPI4)
        Store (Arg4, PPI4)
        CreateWordField (GP04, G004._DBT, DBT4)
        Store (Arg5, DBT4)
        CreateWordField (GP04, G004._PIN, PIN4)
        Store (GNUM (Arg0), PIN4)
        Return (GP04)
      }
      Default {Return (0)}
    }
  }
  Include ("GpioLibCommonV2.asl")
} // \_SB Scope
