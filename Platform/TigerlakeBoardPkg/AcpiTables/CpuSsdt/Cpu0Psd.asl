/** @file
  Intel Processor Power Management ACPI Code.

 @copyright
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
  External(\_SB.CFGD, FieldUnitObj)
  External(\_SB.PR00, DeviceObj)
  External (HTRO)
  External (CORE)
  External (MDID)
  External (SCNT)
  External (BCNT)

  Scope(\_SB.PR00)
  {
    //
    // Get _PSD
    // Arg0 = CPU Domain ID
    //
    Method(GPSD, 1)
    {
      Name(HPSD,Package() // HW_ALL
      {
        Package() { 5,    // NumEntries. Current Value is 5.
                    0,    // Revision. Current Value is 0.
                    0,    // Domain.
                    0xFE, // Coordination type 0xFE = HW_ALL
                    0x80  // Number of processors.
        }
      })

      Name(SPSD,Package() // SW_ALL
      {
        Package() { 5,    // NumEntries. Current Value is 5.
                    0,    // Revision. Current Value is 0.
                    0,    // Domain.
                    0xFC, // Coordination type 0xFC = SW_ALL
                    0x80  // Number of processors.
        }
      })

      If (LAnd(And(\_SB.CFGD, PPM_TURBO_BOOST_MAX), And(\_SB.CFGD, PPM_HWP)))
      {
        Store (Arg0, Index(DerefOf(Index(HPSD, 0)),2)) // Domain
        Store (1, Index(DerefOf(Index(HPSD, 0)),4)) // Number of processors belonging to the domain.
      } Else {
        //
        // For Hetero Cores, All big cores will be in Single domain
        // and Each Small Core module have to be in a seperate domain
        //
        If (HTRO) {
          If (And(ShiftRight(CORE, Arg0), BIT0)) {
            // Small Core
            Store (SCNT, Index (DerefOf (Index (HPSD, 0)), 4))
            Store (SCNT, Index (DerefOf (Index (SPSD, 0)), 4))
            If (LAnd(MDID, BIT0)) {
              Store (0xFF, Index (DerefOf (Index(HPSD, 0)), 2)) // Domain
            } Else {
              If (LAnd(MDID, BIT1)) {
                Store (0xFE, Index (DerefOf (Index(HPSD, 0)), 2)) // Domain
              }
            }
          } Else {
            Store (BCNT, Index (DerefOf (Index (HPSD, 0)), 4))
            Store (BCNT, Index (DerefOf (Index (SPSD, 0)), 4))
          }
        } Else {
          Store (TCNT, Index (DerefOf (Index (HPSD, 0)), 4))
          Store (TCNT, Index (DerefOf (Index (SPSD, 0)), 4))
        }
      }
      If(And(PF00,0x0800))
      {
        Return(HPSD)
      }
      Return(SPSD)
    }

    Method(_PSD,0)
    {
      Return(GPSD(0))
    }
  }
} // End of Definition Block

