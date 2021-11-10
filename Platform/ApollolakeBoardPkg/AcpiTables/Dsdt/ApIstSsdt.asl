/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "APIST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApIst",
  0x3000
  )
{
  External(\_PR.CPU0._PSS, MethodObj)
  External(\_PR.CPU0._PCT, MethodObj)
  External(\_PR.CPU0._PPC, IntObj)
  External(\_PR.CPU0._PSD, MethodObj)
  External(\_PR.CPU1, DeviceObj)
  External(\_PR.CPU2, DeviceObj)
  External(\_PR.CPU3, DeviceObj)
  External(\_PR.CFGD, IntObj)
  External (PDC0)

  include ("ApIst.asl")
} // End of Definition Block
