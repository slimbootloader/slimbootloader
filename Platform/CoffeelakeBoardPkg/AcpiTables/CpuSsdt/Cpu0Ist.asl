/** @file
  Intel Processor Power Management ACPI Code.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

DefinitionBlock (
  "CPU0IST.aml",
  "SSDT",
  0x02,
  "PmRef",
  "Cpu0Ist",
  0x3000
  )
{
  External(\_SB.OSCP, IntObj)
  External(\TCNT, FieldUnitObj)
  External(\_SB.CPPC, FieldUnitObj)
  External(\_SB.PR00, DeviceObj)

  Scope(\_SB.PR00)
  {
    Method(_PPC, 0)
    {
      Return(\_SB.CPPC) // return max P-state available
    }

    //
    // NOTE:  For CMP systems; this table is not loaded unless
    //        the required driver support is present.
    //        So, we do not check for those cases here.
    //
    // CFGD[0]  = EIST Capable/Enabled
    // PDCx[0]  = OS Capable of Hardware P-State control
    //
    Name(_PCT, Package ()  // Native Mode
    {
        ResourceTemplate(){Register(FfixedHW, 0, 0, 0)},
        ResourceTemplate(){Register(FfixedHW, 0, 0, 0)}
    })

    //
    // Returns a list of supported processor performance states.
    //
    // \_SB.OSCP[10] = Platform-Wide OS Capable for no limit 16 P-states
    //
    Method(_PSS,0)
    {
      If(And(\_SB.OSCP,0x0400))
      {
        Return (TPSS)
      }
      Else
      {
        Return (LPSS)
      }
    }

    //
    // LPSS is for OS to support limit 16 P-states
    //
    Name(LPSS,Package()
    {
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}
    })

    //
    // TPSS is for ACPI 6.0 and beyond support 255 states for no limit 16 P-states.
    // @TODO : Here has limitation that namespace size only could up to 99, over 99 will cause CpuSsdt load fail and P-state no cycling issue.
    //
    Name(TPSS,Package()
    {
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}
    })
  } // End of Scope(\_SB.PR00)
} // End of Definition Block

