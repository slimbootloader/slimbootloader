/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "CPU0CST.aml",
  "SSDT",
  2,
  "PmRef",
  "Cpu0Cst",
  0x3001
  )
{
External(\_SB.PR00, DeviceObj)
External(PWRS)
External(CFGD)
External(PF00)
External(FMBL)
External(FEMD)
External(PFLV)
External(C6MW)       // Mwait Hint value for C6
External(C7MW)       // Mwait Hint value for C7
External(CDMW)       // Mwait Hint value for C8/C9/C10
External(C6LT)       // Latency Value for C6
External(C7LT)       // Latency Value for C7
External(CDLT)       // Latency Value for C8/C9/C10
External(CDLV)       // IO Level value for C8/C9/C10
External(CDPW)       // Power value for  C8/C9/C10

Scope(\_SB.PR00)
{
  //
  // Create Temp packages for each C-state and Initialize them to default IO_LVL
  //
  // C1  Temp Package (C1 - HLT)
  //
  Name ( C1TM, Package()
  {
    ResourceTemplate () {Register(FFixedHW, 0, 0, 0)},
    1,
    C1_LATENCY,
    C1_POWER
  })

  //
  // C6 Temp Package
  //
  Name ( C6TM, Package()
  {
     ResourceTemplate () {Register(SystemIO, 8, 0, PCH_ACPI_LV3)},
     2,
     0,
     C6_POWER
   })

  //
  // C7 Temp Package
  //
  Name ( C7TM, Package()
  {
     ResourceTemplate () {Register(SystemIO, 8, 0, PCH_ACPI_LV4)},
     2,
     0,
     C7_POWER
   })

  //
  // CD Temp Package - Deep C-states - covers C8/C9/C10
  //
  Name ( CDTM, Package()
  {
     ResourceTemplate () {Register(SystemIO, 8, 0, PCH_ACPI_LV4)},
     3,
     0,
     0
   })
  //
  // ResourceTemplate for MWait Extentions Supported.
  //
  Name ( MWES, ResourceTemplate(){Register(FFixedHW, 1, 2, 0x00, 1)})
  //
  // Valid/Invalid Flags for ACPI C2 and C3
  //
  Name (AC2V, 0)
  Name (AC3V, 0)
  //
  // Package for reporting 3 C-states
  //
  Name ( C3ST, Package()
  {
    3,
    Package() {},
    Package() {},
    Package() {}
  })
  //
  // Package for reporting 2 C-states
  //
  Name ( C2ST, Package()
  {
    2,
    Package() {},
    Package() {}
  })
  //
  // Package for reporting 1 C-state
  //
  Name ( C1ST, Package()
  {
    1,
    Package() {}
  })
  //
  // C-state initialization flag
  //
  Name(CSTF, 0)
  //
  // Returns the C-state table based on platform configuration.
  // This method is serialized since it uses various global packages and updates them in run time to return the current C-state table.
  //
  Method (_CST, 0, Serialized)
  {
    If(LNot(CSTF))
    {
     //
     // First call to _CST.
     // Update Latency Values for C6/C7/CD based on the Latency values passed through PPM NVS
     //
     Store (C6LT, Index(C6TM, 2))
     Store (C7LT, Index(C7TM, 2))
     Store (CDLT, Index(CDTM, 2))
     //
     // Update the IO_LVL and Power values in CD temp package
     //
     Store (CDPW, Index(CDTM, 3))
     Store (CDLV, Index (DerefOf (Index (CDTM, 0)),7))
     //
     //   CFGD[11] = 1 - MWAIT extensions supported
     //   PDCx[9]  = 1 - OS supports MWAIT extensions
     //   PDCx[8]  = 1 - OS supports MWAIT for C1 (Inferred from PDCx[9] = 1.)
     //

     If(LAnd(And(CFGD, PPM_MWAIT_EXT), And(PF00,0x200)))
     {
      //
      // Processor MWAIT extensions supported and OS supports MWAIT extensions
      // 1. Replace the IO LVL ResourceTemplate of C1TM, C6TM, C7TM, CDTM with MWAIT EXT ResourceTemplate (FFixedHW)
      // 2. Update the Mwait Hint Values for C6/C7/CD based on the Latency values passed through PPM NVS
      //

      Store (MWES, Index (C1TM, 0))
      Store (MWES, Index (C6TM, 0))
      Store (MWES, Index (C7TM, 0))
      Store (MWES, Index (CDTM, 0))

      Store (C6MW, Index (DerefOf (Index (C6TM, 0)),7))
      Store (C7MW, Index (DerefOf (Index (C7TM, 0)),7))
      Store (CDMW, Index (DerefOf (Index (CDTM, 0)),7))
     }
     ElseIf (LAnd(And(CFGD, PPM_MWAIT_EXT), And(PF00,0x100)))
     {
      //
      // Update C1 temp package ResourceTemplate if OS supports Mwait for C1
      //
      Store (MWES, Index (C1TM, 0))
     }

     Store (Ones, CSTF)
    }
    //
    // Initialize the ACPI C2, C3 Valid/Invalid flags to Invalid (0)
    //
    Store(Zero, AC2V)
    Store(Zero, AC3V)
    //
    // Create C state Package with Acpi C1= C1,ACPI C2=MaxSupported(C6,C3,C7),ACPI C3=MaxSupported(C8,C9,C10).
    // It is safe to assume C1 always supported if we enable C-states.
    //
    Store (C1TM, Index (C3ST,1))

    If(And(CFGD,PPM_C7))
    {
      Store (C7TM, Index (C3ST,2))
      Store (Ones, AC2V)
    }ElseIf(And(CFGD,PPM_C6))
    {
      Store (C6TM, Index (C3ST,2))
      Store (Ones, AC2V)
    }
    If(And(CFGD,PPM_CD)) {
      Store (CDTM, Index (C3ST,3))
      Store (Ones, AC3V)
    }

    //
    // Filter and return the final C-state package
    //
    If(LAnd(AC2V, AC3V))
    {
    Return (C3ST)
    }
    ElseIf(AC2V)
    {
     Store (DerefOf (Index (C3ST,1)), Index (C2ST,1))
     Store (DerefOf (Index (C3ST,2)), Index (C2ST,2))
     Return (C2ST)
    }
    ElseIf(AC3V)
    {
    Store (DerefOf (Index (C3ST,1)), Index (C2ST,1))
    Store (DerefOf (Index (C3ST,3)), Index (C2ST,2))
    Store (2, Index (DerefOf (Index (C2ST, 2)),1))
    Return (C2ST)
    }
    Else
    {
    Store (DerefOf (Index (C3ST,1)), Index (C1ST,1))
    Return (C1ST)
    }
  }
}
}
