/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "Cpu0Hwp.aml",
  "SSDT",
  2,
  "PmRef",
  "Cpu0Hwp",
  0x3000
  )
{
External(\_SB.PR00, DeviceObj)
External(\TCNT, FieldUnitObj)
External(\_SB.HWPV, IntObj)
External(\_SB.CFGD, IntObj)
External(\_SB.OSCP, IntObj)
External(\_SB.HWPE, IntObj)
External (CORE)
External (RPEF)
External (MXP1)
External (BSFQ)
External (P100)
External (IS00)

// _CPC (Continuous Performance Control)
//  _CPC is a per-processor ACPI object that declares an interface for OSPM to
//  transition the processor into a performance state based on a continuous range
//  of allowable values.  Each CPPC register is described in a Generic Register
//  Descriptor format and maps to an unique PCC shared memory
//  location.  For a complete description of _CPC object, refer to ACPI 5.0
//  specification, section 8.4.5.1.
// Arguments: (0)
//  None
// Return Value:
//  A Package of elements in the following format
//
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
Scope(\_SB.PR00)
{
  Name (CPC3, Package ()
  {
    23, // Number of entries
    03, // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate () {Register (FFixedHW, 8, 0, 0x771, 4)},   // HighestPerformance
    ResourceTemplate () {Register (FFixedHW, 8, 8, 0xCE, 4)},    // Nominal Performance = P1Ratio[i] * IpcScaling[i] / 100
    ResourceTemplate () {Register (FFixedHW, 8, 16, 0x771, 4)},  // Lowest nonlinear Performance
    ResourceTemplate () {Register (FFixedHW, 8, 24, 0x771, 4)},  // LowestPerformance
    ResourceTemplate () {Register (FFixedHW, 8, 8, 0x0771, 4)},  // Guaranteed Performance
    ResourceTemplate () {Register (FFixedHW, 8, 16, 0x0774, 4)}, // Desired PerformanceRegister
    ResourceTemplate () {Register (FFixedHW, 8, 0, 0x774, 4)},   // Minimum PerformanceRegister
    ResourceTemplate () {Register (FFixedHW, 8, 8, 0x774, 4)},   // Maximum PerformanceRegister
    ResourceTemplate () {Register (SystemMemory, 0, 0, 0, 0)},   // Performance ReductionToleranceRegister (Null)
    ResourceTemplate () {Register (SystemMemory, 0, 0, 0, 0)},   // Time window  register (Null)
    ResourceTemplate () {Register (SystemMemory, 0, 0, 0, 0)},   // Counter wrap around time (Null)
    ResourceTemplate () {Register (FFixedHW, 64, 0, 0xE7, 4)},   // Reference counter register (PPERF)
    ResourceTemplate () {Register (FFixedHW, 64, 0, 0xE8, 4)},   // Delivered counter register (APERF)
    ResourceTemplate () {Register (FFixedHW, 2, 1, 0x777, 4)},   // Performance limited register
    ResourceTemplate () {Register (FFixedHW, 1, 0, 0x770, 4)},   // Enable register
    1, // Autonomous selection enable register (Exclusively autonomous)
    ResourceTemplate () {Register (FFixedHW, 10, 32, 0x774, 4)}, // Autonomous activity window register
    ResourceTemplate () {Register (FFixedHW, 8, 24, 0x774, 4)},  // Autonomous energy performance preference register
    0, // Reference Performance = MaxNonTurboRatio * IpcScaling[i] / 100
    0, // Lowest Frequency (not supported)
    0, // Nominal Frequency = P1Ratio[i] * BusFrequency
  })

  Name (CPC4, Package ()
  {
    25, // Number of entries
    04, // Revision
    //
    // Describe processor capabilities
    //
    ResourceTemplate () {Register (FFixedHW, 8, 0, 0x771, 4)},   // HighestPerformance
    ResourceTemplate () {Register (FFixedHW, 8, 8, 0xCE, 4)},    // Nominal Performance = P1Ratio[i] * IpcScaling[i] / 100
    ResourceTemplate () {Register (FFixedHW, 8, 16, 0x771, 4)},  // Lowest nonlinear Performance
    ResourceTemplate () {Register (FFixedHW, 8, 24, 0x771, 4)},  // LowestPerformance
    ResourceTemplate () {Register (FFixedHW, 8, 8, 0x0771, 4)},  // Guaranteed Performance
    ResourceTemplate () {Register (FFixedHW, 8, 16, 0x0774, 4)}, // Desired PerformanceRegister
    ResourceTemplate () {Register (FFixedHW, 8, 0, 0x774, 4)},   // Minimum PerformanceRegister
    ResourceTemplate () {Register (FFixedHW, 8, 8, 0x774, 4)},   // Maximum PerformanceRegister
    ResourceTemplate () {Register (SystemMemory, 0, 0, 0, 0)},   // Performance ReductionToleranceRegister (Null)
    ResourceTemplate () {Register (SystemMemory, 0, 0, 0, 0)},   // Time window  register (Null)
    ResourceTemplate () {Register (SystemMemory, 0, 0, 0, 0)},   // Counter wrap around time (Null)
    ResourceTemplate () {Register (FFixedHW, 64, 0, 0xE7, 4)},   // Reference counter register (PPERF)
    ResourceTemplate () {Register (FFixedHW, 64, 0, 0xE8, 4)},   // Delivered counter register (APERF)
    ResourceTemplate () {Register (FFixedHW, 2, 1, 0x777, 4)},   // Performance limited register
    ResourceTemplate () {Register (FFixedHW, 1, 0, 0x770, 4)},   // Enable register
    1, // Autonomous selection enable register (Exclusively autonomous)
    ResourceTemplate () {Register (FFixedHW, 10, 32, 0x774, 4)}, // Autonomous activity window register
    ResourceTemplate () {Register (FFixedHW, 8, 24, 0x774, 4)},  // Autonomous energy performance preference register
    0, // Reference Performance = MaxNonTurboRatio * IpcScaling[i] / 100
    0, // Lowest Frequency (not supported)
    0, // Nominal Frequency = P1Ratio[i] * BusFrequency
    0, // OSPMNominalPerformanceRegister
    //
    // Resource Priority Feature
    //
    Package () {
      Package() {
        Package(0x3) {0x00000003, 0x00000004, 0x00000005},//ControlledResources
        1, //Enable Value
        ResourceTemplate () {Register (FFixedHW, 0x1, 0x0, 0xC88, 0x4)}, //Enable Register
        4, //Priority Count
        ResourceTemplate () {Register (FFixedHW, 0x04, 0x20, 0xC8F, 0x4)}// Priority Register
      }
    }
  })

  //
  // GCPC (Index, P1Ratio, IpcScaling)
  //
  Method (GCPC,3)
  {
    //
    // ReferencePerformance = MaxNonTurboRatio * IpcScaling[i] / 100
    //
    Local0 = MXP1 * Arg2
    Local0 /= 100
    //ADBG(Concatenate("  _CPC.ReferencePerformance (Max P1 * Scaling / 100) = ", ToDecimalString (Local0)))
    Store (Local0, Index (CPC3, 20))
    Store (Local0, Index (CPC4, 20))

    //
    // NominalFrequency = P1Ratio[i] * BusFrequency
    //
    Local0 = Arg1 * BSFQ
    //ADBG(Concatenate("  _CPC.NominalFrequency (P1 * BusFrequency) =", ToDecimalString(Local0)))
    Store (Local0, Index (CPC3, 22))
    Store (Local0, Index (CPC4, 22))

    //
    // NominalPerformance = PerCoreP1Ratio[i] * IpcScaling[i] / 100
    //
    Local0 = Arg1 * Arg2
    Local0 /= 100
    //ADBG(Concatenate("  _CPC.NominalPerformance (P1 * Scaling / 100) = ", ToDecimalString(Local0)))
    Store (Local0, Index (CPC3, 3))
    Store (Local0, Index (CPC4, 3))

    //
    // Must return different values for HighestPerformance with the conditions as below:
    //  ==============================================================================================
    // | OC Enabled:                                  | OC Disabled:                                  |
    // |   - ITBM3 Supported                          |  - ITBM3 Supported                            |
    // |     - _CPC Native ITBM3 : MSR 0x771          |    - _CPC Native ITBM3 : MSR 0x771            |
    // |   - ITBM3 Not supported : (Max Perf = 255)   |  - ITBM3 Not supported : MSR 0x771            |
    //  ==============================================================================================
    //
    If (And (\_SB.CFGD, PPM_OC_UNLOCKED)) {  // OC enabled
      If (LEqual (And (\_SB.CFGD, PPM_TURBO_BOOST_MAX), 0)) { // ITBM3 unsupported
        Store (0xFF, Index (CPC3, 2))
        Store (0xFF, Index (CPC4, 2))
      }
    }

    If (RPEF) {
      Return (CPC4)
    } Else {
      Return (CPC3)
    }
  }

  Method (_CPC,0)
  {
    Return (GCPC (0, P100, IS00))
  }
}// end Scope (\_SB.PR00)

}// end of definition block
