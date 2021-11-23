/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB)
{
  Device(PR00){               // Unique name for Processor 0.
    Name(_HID,"ACPI0007")
    Name(_UID,0)              // Unique ID for Processor 0.
  }

  Device(PR01){               // Unique name for Processor 1.
    Name(_HID,"ACPI0007")
    Name(_UID,1)              // Unique ID for Processor 1.
  }

  Device(PR02){               // Unique name for Processor 2.
    Name(_HID,"ACPI0007")
    Name(_UID,2)              // Unique ID for Processor 2.
  }

  Device(PR03){               // Unique name for Processor 3.
    Name(_HID,"ACPI0007")
    Name(_UID,3)              // Unique ID for Processor 3.
  }

  Device(PR04){              // Unique name for Processor 4.
    Name(_HID,"ACPI0007")
    Name(_UID,4)             // Unique ID for Processor 4.
  }

  Device(PR05){              // Unique name for Processor 5.
    Name(_HID,"ACPI0007")
    Name(_UID,5)             // Unique ID for Processor 5.
  }

  Device(PR06){              // Unique name for Processor 6.
    Name(_HID,"ACPI0007")
    Name(_UID,6)             // Unique ID for Processor 6.
  }

  Device(PR07){              // Unique name for Processor 7.
    Name(_HID,"ACPI0007")
    Name(_UID,7)             // Unique ID for Processor 7.
  }

  Device(PR08){              // Unique name for Processor 8.
    Name(_HID,"ACPI0007")
    Name(_UID,8)             // Unique ID for Processor 8.
  }

  Device(PR09){              // Unique name for Processor 9.
    Name(_HID,"ACPI0007")
    Name(_UID,9)             // Unique ID for Processor 9.
  }

  Device(PR10){              // Unique name for Processor 10.
    Name(_HID,"ACPI0007")
    Name(_UID,10)             // Unique ID for Processor 10.
  }

  Device(PR11){              // Unique name for Processor 11.
    Name(_HID,"ACPI0007")
    Name(_UID,11)             // Unique ID for Processor 11.
  }

  Device(PR12){              // Unique name for Processor 12.
    Name(_HID,"ACPI0007")
    Name(_UID,12)             // Unique ID for Processor 12.
  }

  Device(PR13){              // Unique name for Processor 13.
    Name(_HID,"ACPI0007")
    Name(_UID,13)             // Unique ID for Processor 13.
  }

  Device(PR14){              // Unique name for Processor 14.
    Name(_HID,"ACPI0007")
    Name(_UID,14)             // Unique ID for Processor 14.
  }

  Device(PR15){              // Unique name for Processor 15.
    Name(_HID,"ACPI0007")
    Name(_UID,15)             // Unique ID for Processor 15.
  }

  Device(PR16){              // Unique name for Processor 16.
    Name(_HID,"ACPI0007")
    Name(_UID,16)             // Unique ID for Processor 16.
  }

  Device(PR17){              // Unique name for Processor 17.
    Name(_HID,"ACPI0007")
    Name(_UID,17)             // Unique ID for Processor 17.
  }

  Device(PR18){              // Unique name for Processor 18.
    Name(_HID,"ACPI0007")
    Name(_UID,18)             // Unique ID for Processor 18.
  }

  Device(PR19){              // Unique name for Processor 19.
    Name(_HID,"ACPI0007")
    Name(_UID,19)             // Unique ID for Processor 19.
  }

  Device(PR20){              // Unique name for Processor 20.
    Name(_HID,"ACPI0007")
    Name(_UID,20)             // Unique ID for Processor 20.
  }

  Device(PR21){              // Unique name for Processor 21.
    Name(_HID,"ACPI0007")
    Name(_UID,21)             // Unique ID for Processor 21.
  }

  Device(PR22){              // Unique name for Processor 22.
    Name(_HID,"ACPI0007")
    Name(_UID,22)             // Unique ID for Processor 22.
  }

  Device(PR23){              // Unique name for Processor 23.
    Name(_HID,"ACPI0007")
    Name(_UID,23)             // Unique ID for Processor 23.
  }

} // End Scope(\_SB)

//
// _CPC (Continuous Performance Control) Package declaration
//  Package
//  {
//    NumEntries,                           // Integer
//    Revision,                             // Integer
//    HighestPerformance,                   // Generic Register Descriptor
//    NominalPerformance,                   // Generic Register Descriptor
//    LowestNonlinearPerformance,           // Generic Register Descriptor
//    LowestPerformance,                    // Generic Register Descriptor
//    GuaranteedPerformanceRegister,        // Generic Register Descriptor
//    DesiredPerformanceRegister,           // Generic Register Descriptor
//    MinimumPerformanceRegister,           // Generic Register Descriptor
//    MaximumPerformanceRegister,           // Generic Register Descriptor
//    PerformanceReductionToleranceRegister,// Generic Register Descriptor
//    TimeWindowRegister,                   // Generic Register Descriptor
//    CounterWraparoundTime,                // Generic Register Descriptor
//    NominalCounterRegister,               // Generic Register Descriptor
//    DeliveredCounterRegister,             // Generic Register Descriptor
//    PerformanceLimitedRegister,           // Generic Register Descriptor
//    EnableRegister                        // Generic Register Descriptor
//  }
//
Scope(\_SB.PR00)
{
  Name(CPC2, Package()
  {
    21, // Number of entries
    02, // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate() {Register(FFixedHW, 8, 0, 0x771, 4)}, // HighestPerformance
    ResourceTemplate() {Register(FFixedHW, 8, 8, 0xCE, 4)},  // Nominal Performance - Maximum Non Turbo Ratio
    ResourceTemplate() {Register(FFixedHW, 8, 16, 0x771, 4)},//Lowest nonlinear Performance
    ResourceTemplate() {Register(FFixedHW, 8, 24, 0x771, 4)}, // LowestPerformance
    ResourceTemplate() {Register(FFixedHW, 8, 8, 0x0771, 4)}, // Guaranteed Performance
    ResourceTemplate() {Register(FFixedHW, 8, 16, 0x0774, 4)}, // Desired PerformanceRegister
    ResourceTemplate() {Register(FFixedHW, 8, 0, 0x774, 4)}, // Minimum PerformanceRegister
    ResourceTemplate() {Register(FFixedHW, 8, 8, 0x774, 4)}, // Maximum PerformanceRegister
    ResourceTemplate() {Register(SystemMemory, 0, 0, 0, 0)}, // Performance ReductionToleranceRegister (Null)
    ResourceTemplate() {Register(SystemMemory, 0, 0, 0, 0)}, // Time window  register(Null)
    ResourceTemplate() {Register(SystemMemory, 0, 0, 0, 0)}, // Counter wrap around time(Null)
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // Reference counter register (PPERF)
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // Delivered counter register (APERF)
    ResourceTemplate() {Register(FFixedHW, 2, 1, 0x777, 4)}, // Performance limited register
    ResourceTemplate() {Register(FFixedHW, 1, 0, 0x770, 4)}, // Enable register
    1, // Autonomous selection enable register (Exclusively autonomous)
    ResourceTemplate() {Register(FFixedHW, 10, 32, 0x774, 4)}, // Autonomous activity window register
    ResourceTemplate() {Register(FFixedHW, 8, 24, 0x774, 4)}, // Autonomous energy performance preference register
    0 // Reference performance (not supported)
  })

  Name(CPOC, Package()
  {
    21, // Number of entries
    02, // Revision
    //
    // Describe processor capabilities
    //
    255, // HighestPerformance
    ResourceTemplate() {Register(FFixedHW, 8, 8, 0xCE, 4)},  // Nominal Performance - Maximum Non Turbo Ratio
    ResourceTemplate() {Register(FFixedHW, 8, 16, 0x771, 4)},//Lowest nonlinear Performance
    ResourceTemplate() {Register(FFixedHW, 8, 24, 0x771, 4)}, // LowestPerformance
    ResourceTemplate() {Register(FFixedHW, 8, 8, 0x0771, 4)}, // Guaranteed Performance
    ResourceTemplate() {Register(FFixedHW, 8, 16, 0x0774, 4)}, // Desired PerformanceRegister
    ResourceTemplate() {Register(FFixedHW, 8, 0, 0x774, 4)}, // Minimum PerformanceRegister
    ResourceTemplate() {Register(FFixedHW, 8, 8, 0x774, 4)}, // Maximum PerformanceRegister
    ResourceTemplate() {Register(SystemMemory, 0, 0, 0, 0)}, // Performance ReductionToleranceRegister (Null)
    ResourceTemplate() {Register(SystemMemory, 0, 0, 0, 0)}, // Time window  register(Null)
    ResourceTemplate() {Register(SystemMemory, 0, 0, 0, 0)}, // Counter wrap around time(Null)
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE7, 4)}, // Reference counter register (PPERF)
    ResourceTemplate() {Register(FFixedHW, 64, 0, 0xE8, 4)}, // Delivered counter register (APERF)
    ResourceTemplate() {Register(FFixedHW, 2, 1, 0x777, 4)}, // Performance limited register
    ResourceTemplate() {Register(FFixedHW, 1, 0, 0x770, 4)}, // Enable register
    1, // Autonomous selection enable register (Exclusively autonomous)
    ResourceTemplate() {Register(FFixedHW, 10, 32, 0x774, 4)}, // Autonomous activity window register
    ResourceTemplate() {Register(FFixedHW, 8, 24, 0x774, 4)}, // Autonomous energy performance preference register
    0 // Reference performance (not supported)
  })

}// end Scope(\_SB.PR00)

#ifndef SPS_SUPPORT // SPS is using Processor Aggregator Device different way
Scope(\_SB)
{
  // The Processor Aggregator Device provides a control point that enables the platform to perform
  // specific processor configuration and control that applies to all processors in the platform.
  Device (PAGD)
  {
    Name (_HID, "ACPI000C") // Processor Aggregator Device

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If(\_OSI("Processor Aggregator Device")){
        Return (0x0F) // Processor Aggregator Device is supported by this OS.
      } Else {
        Return (0)    // No support in this OS.
      }
    }


    // _PUR (Processor Utilization Request)
    //
    // The _PUR object is an optional object that may be declared under the Processor Aggregator Device
    // and provides a means for the platform to indicate to OSPM the number of logical processors
    // to be idled. OSPM evaluates the _PUR object as a result of the processing of a Notify event
    // on the Processor Aggregator device object of type 0x80.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package
    //
    Name (_PUR, Package() // Requests a number of logical processors to be placed in an idle state.
    {
      1,                           // RevisionID, Integer: Current value is 1
      0                            // NumProcessors, Integer
    })

  } // end Device(PAGD)
}// end Scope(\_SB)
#endif // ndef SPS_SUPPORT

