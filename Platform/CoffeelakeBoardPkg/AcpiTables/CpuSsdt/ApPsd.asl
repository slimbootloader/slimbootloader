/** @file
  Intel Processor Power Management PSD ACPI Code.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "APPSD.aml",
  "SSDT",
  2,
  "PmRef",
  "ApPsd",
  0x3000
  )
{
  External(\PC00, IntObj)
  External(\TCNT, FieldUnitObj)
  External(\_SB.PR00, DeviceObj)
  External(\_SB.PR01, DeviceObj)
  External(\_SB.PR02, DeviceObj)
  External(\_SB.PR03, DeviceObj)
  External(\_SB.PR04, DeviceObj)
  External(\_SB.PR05, DeviceObj)
  External(\_SB.PR06, DeviceObj)
  External(\_SB.PR07, DeviceObj)
  External(\_SB.PR08, DeviceObj)
  External(\_SB.PR09, DeviceObj)
  External(\_SB.PR10, DeviceObj)
  External(\_SB.PR11, DeviceObj)
  External(\_SB.PR12, DeviceObj)
  External(\_SB.PR13, DeviceObj)
  External(\_SB.PR14, DeviceObj)
  External(\_SB.PR15, DeviceObj)
  External(\_SB.PR00._PPC, MethodObj)
  External(\_SB.PR00._PCT, MethodObj)
  External(\_SB.PR00._PSS, MethodObj)
  External(\_SB.CFGD, FieldUnitObj)

  Scope(\_SB.PR01)
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
        Store (1, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR01)

  Scope(\_SB.PR02)
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
        Store (2, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR02)


  Scope(\_SB.PR03)
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
        Store (3, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR03)

  Scope(\_SB.PR04)
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
        Store (4, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
       } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR04)

  Scope(\_SB.PR05)
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
        Store (5, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR05)

  Scope(\_SB.PR06)
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
        Store (6, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR06)

  Scope(\_SB.PR07)
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
        Store (7, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR07)

  Scope(\_SB.PR08)
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
        Store (8, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR08)

  Scope(\_SB.PR09)
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
        Store (9, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR09)

  Scope(\_SB.PR10)
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
        Store (10, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR10)

  Scope(\_SB.PR11)
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
        Store (11, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR11)

  Scope(\_SB.PR12)
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
        Store (12, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR12)

  Scope(\_SB.PR13)
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
        Store (13, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR13)

  Scope(\_SB.PR14)
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
        Store (14, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR14)

  Scope(\_SB.PR15)
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
        Store (15, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        Store (TCNT, Index(DerefOf(Index(HPSD, 0)),4))
        Store (TCNT, Index(DerefOf(Index(SPSD, 0)),4))
      }
      If(And(PC00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }
  } // End of Scope(\_SB.PR15)

} // End of Definition Block
