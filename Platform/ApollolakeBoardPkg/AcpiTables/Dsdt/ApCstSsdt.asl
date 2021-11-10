/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "APCST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApCst",
  0x3000
  )
{
  External(\_PR.CPU1, DeviceObj)
  External(\_PR.CPU2, DeviceObj)
  External(\_PR.CPU3, DeviceObj)
  External(\_PR.CPU0._CST)

  include ("ApCst.asl")
}
