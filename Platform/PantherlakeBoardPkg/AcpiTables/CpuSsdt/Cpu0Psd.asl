/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "CPU0PSD.aml",
  "SSDT",
  0x02,
  "PmRef",
  "Cpu0Psd",
  0x3000
  )
{
  External (\PF00, IntObj)
  External (\TCNT, FieldUnitObj)
  External (\_SB.CFGD, FieldUnitObj)
  External (\_SB.PR00, DeviceObj)
  External (HTRO)
  External (MDID)
  External (SCM0)
  External (SCM1)
  External (BCNT)
  External (DI00)
  External (CM00)


  Scope(\_SB.PR00)
  {
    //
    // Get _PSD
    // Arg0 = CPU Index
    // Arg1 = ModuleID
    // Arg2 = CoreCountInModule
    //
    Method (GPSD, 3)
    {
      Name (HPSD,Package () // HW_ALL
      {
        Package () { 5,     // NumEntries. Current Value is 5.
                    0,      // Revision. Current Value is 0.
                    0,      // Domain.
                    0xFE,   // Coordination type 0xFE = HW_ALL
                    0x80    // Number of processors.
        }
      })

      Name (SPSD,Package () // SW_ALL
      {
        Package () { 5,     // NumEntries. Current Value is 5.
                    0,      // Revision. Current Value is 0.
                    0,      // Domain.
                    0xFC,   // Coordination type 0xFC = SW_ALL
                    0x80    // Number of processors.
        }
      })

      If (LAnd (And (\_SB.CFGD, PPM_TURBO_BOOST_MAX), And (\_SB.CFGD, PPM_HWP)))
      {
        Store (Arg0, Index (DerefOf (Index (HPSD, 0)),2)) // Domain
        Store (1, Index (DerefOf (Index (HPSD, 0)),4))    // Number of processors belonging to the domain.
      } Else {
        //
        // For Hetero Cores, Each module have to be in its seperate domain
        //
        If (HTRO) {
          //
          // Number of processors belonging to the domain.
          //
          Store (Arg2, Index (DerefOf (Index (HPSD, 0)), 4))
          Store (Arg2, Index (DerefOf (Index (SPSD, 0)), 4))

          Store (Arg1, Index (DerefOf (Index (HPSD, 0)), 2)) // Domain
          Store (Arg1, Index (DerefOf (Index (SPSD, 0)), 2)) // Domain
        } Else {
          Store (TCNT, Index (DerefOf (Index (HPSD, 0)), 4))
          Store (TCNT, Index (DerefOf (Index (SPSD, 0)), 4))
        }
      }
      If (And (PF00,0x0800))
      {
        Return (HPSD)
      }
      Return (SPSD)
    }

    Method (_PSD,0)
    {
      Return (GPSD (0, DI00, CM00))
    }
  }
} // End of Definition Block

