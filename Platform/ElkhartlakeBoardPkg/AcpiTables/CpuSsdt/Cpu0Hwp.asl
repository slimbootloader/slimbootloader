/** @file
  This file contains the  HWP SSDT Table ASL code.

  Copyright (c) 2012 - 2019, Intel Corporation. All rights reserved.<BR>
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
External(\_SB.PR00.CPOC, PkgObj)
External(\_SB.CFGD, IntObj)
External(\_SB.LMPS, IntObj)
External(\_SB.ITBM, IntObj)
External(\_SB.OSCP, IntObj)

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
  Method(_CPC,0)
  {
    //
    // Must return different table if overclocking is enabled CFGD[24] - Overclocking Fully unlocked
    // and OS cannot support notifications (0x85) for frequency re-read
    //
    If(LAnd(And(\_SB.CFGD, PPM_OC_UNLOCKED),LNot(And(\_SB.OSCP,0x1000)))) {
      Return(CPOC) // Return CP0C table if version 2 is enabled with overclocking and no OS frequency re-read support.
    } Else {
      If(LAnd(And(\_SB.CFGD, PPM_TURBO_BOOST_MAX), LEqual(\_SB.ITBM, 0))) {
        //
        // Intel Turbo Boost Max Technology 3.0 is available and reference code policy is disabled
        // Here the _CPC object will report the highest performance of the slowest core.
        //
        If(LNotEqual(\_SB.LMPS, 0)) {
        Store(\_SB.LMPS, DeRefOf(Index(CPC2,2)))
      }
      }
      Return(CPC2)  // Return CPC2 table if version 2 is enabled in all other cases.
    }

  }
}// end Scope(\_SB.PR00)

}// end of definition block
