/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "CPU0CST.aml",
  "SSDT",
  2,
  "PmRef",
  "Cpu0Cst",
  0x3001
  )
{
  External(\_PR.CPU0, DeviceObj)
  External(PWRS)
  External(\_PR.CFGD, IntObj)
  External(PDC0)
  External(FMBL)
  External(FEMD)
  External(PFLV)
  External(C3MW)       // Mwait Hint value for C3
  External(C6MW)       // Mwait Hint value for C6
  External(C7MW)       // Mwait Hint value for C7
  External(CDMW)       // Mwait Hint value for C8/C9/C10
  External(C3LT)       // Latency value for C3
  External(C6LT)       // Latency Value for C6
  External(C7LT)       // Latency Value for C7
  External(CDLT)       // Latency Value for C8/C9/C10
  External(CDLV)       // IO Level value for C8/C9/C10
  External(CDPW)       // Power value for  C8/C9/C10
  External(MWEN)

  include ("Cpu0Cst.asl")
}
