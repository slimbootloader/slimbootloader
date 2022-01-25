/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
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
External(\_SB.PR00.CPC2, PkgObj)
External(\_SB.CFGD, IntObj)
External(\_SB.LMPS, IntObj)
External(\_SB.ITBM, IntObj)
External(\_SB.OSCP, IntObj)
External(\_SB.HWPE, IntObj)
External (CORE)
External (SFBC)
External (SFSC)
External (NMFQ)
External (INFS)

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

  Name(CPOC, Package()
  {
    21, // Number of entries
    02, // Revision
    //
    // Describe processor capabilities
    //
    255, // HighestPerformance fix ratio in integer for OC (255) or ITBM disable with Turbo Boost Max 3.0 supported
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

  Name(CPC3, Package()
  {
    23, // Number of entries
    03, // Revision
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
    0, // Reference performance (not supported)
    0, // Lowest Frequency (not supported)
    0, // Nominal Frequency
  })

  Method(GCPC,1)
  {
    //
    // Must return different table if overclocking is enabled CFGD[24] - Overclocking Fully unlocked
    //
    If(And(\_SB.CFGD, PPM_OC_UNLOCKED)) {
      Return(CPOC) // Return CP0C table if version 2 is enabled with overclocking.
    } Else {
      If(LAnd(And(\_SB.CFGD, PPM_TURBO_BOOST_MAX), LEqual(\_SB.ITBM, 0))) {
        //
        // Intel Turbo Boost Max Technology 3.0 is available and when policy is disabled
        // Here the _CPC object will report the highest performance of the slowest core.
        //
        If(LNotEqual(\_SB.LMPS, 0)) {
          Return(CPOC)
        }
      }
      If (INFS) {
        Store (NMFQ, Index(CPC3, 22))
        If (And(ShiftRight(CORE, Arg0), BIT0)) {
          Store (SFBC, Index(CPC3, 3))
        } Else {
          Store (SFSC, Index(CPC3, 3))
        }
      }
      Return(CPC3)
    }
  }

  Method(_CPC,0)
  {
    Return(GCPC(0))
  }
}// end Scope(\_SB.PR00)

}// end of definition block
