/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "CPU0IST.aml",
  "SSDT",
  0x02,
  "PmRef",
  "Cpu0Ist",
  0x3000
  )
{
  External (\_PR.CPU0, DeviceObj)
  //External (\_PR.CPPC)
  External (PDC0)
  External(\_PR.CFGD, IntObj)
  External (TCNT)
  External (NLPC)

  include ("Cpu0Ist.asl")
} // End of Definition Block
