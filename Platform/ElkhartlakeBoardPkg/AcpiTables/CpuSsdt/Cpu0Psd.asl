/** @file
  Intel Processor Power Management ACPI Code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
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
  External(\PF00, IntObj)
  External(\TCNT, FieldUnitObj)
  External(\_SB.CFGD, IntObj)
  External(\_SB.PR00, DeviceObj)

  Scope(\_SB.PR00)
  {
    Name(HPSD,Package() // HW_ALL
    {
      Package() {5,    // NumEntries. Current Value is 5.
                 0,    // Revision. Current Value is 0.
                 0,    // Domain.
                 0xFE, // Coordination type 0xFE = HW_ALL
                 0x80  // Number of processors.
      }
    })

    Name(SPSD,Package() // SW_ALL
    {
      Package() {5,    // NumEntries. Current Value is 5.
                 0,    // Revision. Current Value is 0.
                 0,    // Domain.
                 0xFC, // Coordination type 0xFC = SW_ALL
                 0x80  // Number of processors.
      }
    })

    //
    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      If (And(\_SB.CFGD, PPM_TURBO_BOOST_MAX)) // Intel Turbo Boost Max 3.0
      {
        Store (0, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PF00,0x0800)) // If Hardware co-ordination of P states
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR00)
} // End of Definition Block

