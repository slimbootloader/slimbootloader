/** @file
  Intel Processor Power Management ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/*#include "CpuPowerMgmt.h"*/

Scope(\_PR.CPU0)
{
        Method (_CST, 0)
        {
            If (LEqual(MWEN, 0))
            {
                    Return( Package()
                    {
                        3,
                        Package()
                        {   // C1, LVT
                            ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
                            1,
                            1,
                            1000
                        },
                        Package()
                        {
                            // C6L, LVT
                            ResourceTemplate(){Register(SystemIO, 8, 0, 0x415)},
                            2,
                            50,
                            10
                        },
                        Package()
                        {
                            // C10, LVT
                            ResourceTemplate(){Register(SystemIO, 8, 0, 0x419)},
                            3,
                            150,   //Pre-silicon setting, 11 ms for worst-case exit latency
                            10
                        }
                    })
            }
                    Return( Package()
                    {
                        3,
                        Package()
                        {   // C1, MWAIT
                            ResourceTemplate(){Register(FFixedHW, 1, 2, 0x01, 1)},
                            1,
                            1,
                            1000
                        },
                        Package()
                        {
                            // C6L, MWAIT Extension
                            ResourceTemplate(){Register(FFixedHW, 1, 2, 0x21, 1)},
                            2,
                            50,
                            10
                        },
                        Package()
                        {
                            // C10, MWAIT Extension
                            ResourceTemplate(){Register(FFixedHW, 1, 2, 0x60, 1)},
                            3,
                            150,   //Pre-silicon setting, 11 ms for worst-case exit latency
                            10
                        }
                    })
        }
}
