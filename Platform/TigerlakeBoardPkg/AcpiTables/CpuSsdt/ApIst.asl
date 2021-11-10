/** @file
  Intel Processor Power Management IST ACPI Code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "CpuPowerMgmt.h"

#define AP_SCOPE_PPC_PCT_PSS_METHOD(INDEX) \
  External(\_SB.PR##INDEX, DeviceObj) \
  Scope(\_SB.PR##INDEX) \
  { \
    Method(_PPC,0) \
    { \
      Return(\_SB.PR00._PPC) \
    } \
    Method(_PCT,0) \
    { \
      Return(\_SB.PR00._PCT) \
    } \
    Method(_PSS,0) \
    { \
      Return(\_SB.PR00._PSS) \
    } \
  }


DefinitionBlock (
  "APIST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApIst",
  0x3000
  )
{
  External(\_SB.PR00._PPC, MethodObj)
  External(\_SB.PR00._PCT, MethodObj)
  External(\_SB.PR00._PSS, MethodObj)

  AP_SCOPE_PPC_PCT_PSS_METHOD(01)
  AP_SCOPE_PPC_PCT_PSS_METHOD(02)
  AP_SCOPE_PPC_PCT_PSS_METHOD(03)
  AP_SCOPE_PPC_PCT_PSS_METHOD(04)
  AP_SCOPE_PPC_PCT_PSS_METHOD(05)
  AP_SCOPE_PPC_PCT_PSS_METHOD(06)
  AP_SCOPE_PPC_PCT_PSS_METHOD(07)
  AP_SCOPE_PPC_PCT_PSS_METHOD(08)
  AP_SCOPE_PPC_PCT_PSS_METHOD(09)
  AP_SCOPE_PPC_PCT_PSS_METHOD(10)
  AP_SCOPE_PPC_PCT_PSS_METHOD(11)
  AP_SCOPE_PPC_PCT_PSS_METHOD(12)
  AP_SCOPE_PPC_PCT_PSS_METHOD(13)
  AP_SCOPE_PPC_PCT_PSS_METHOD(14)
  AP_SCOPE_PPC_PCT_PSS_METHOD(15)
  AP_SCOPE_PPC_PCT_PSS_METHOD(16)
  AP_SCOPE_PPC_PCT_PSS_METHOD(17)
  AP_SCOPE_PPC_PCT_PSS_METHOD(18)
  AP_SCOPE_PPC_PCT_PSS_METHOD(19)
  AP_SCOPE_PPC_PCT_PSS_METHOD(20)
  AP_SCOPE_PPC_PCT_PSS_METHOD(21)
  AP_SCOPE_PPC_PCT_PSS_METHOD(22)
  AP_SCOPE_PPC_PCT_PSS_METHOD(23)
} // End of Definition Block

#undef AP_SCOPE_PPC_PCT_PSS_METHOD
