/** @file
  Intel Processor Power Management PSD ACPI Code.

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "CpuPowerMgmt.h"

#define AP_SCOPE_PSD_METHOD(INDEX) \
  External(\_SB.PR##INDEX, DeviceObj) \
  Scope(\_SB.PR##INDEX) \
  { \
    Method(_PSD,0) \
    { \
      Return(\_SB.PR00.GPSD(INDEX)) \
    } \
  }


DefinitionBlock (
  "APPSD.aml",
  "SSDT",
  2,
  "PmRef",
  "ApPsd",
  0x3000
  )
{
  External(\PF00, IntObj)
  External(\TCNT, FieldUnitObj)
  External(\_SB.CFGD, FieldUnitObj)
  External(\_SB.PR00.GPSD, MethodObj)

  //
  // The _PSD object provides information to the OSPM related
  // to P-State coordination between processors in a multi-processor
  // configurations.
  //
  AP_SCOPE_PSD_METHOD(1)
  AP_SCOPE_PSD_METHOD(2)
  AP_SCOPE_PSD_METHOD(3)
  AP_SCOPE_PSD_METHOD(4)
  AP_SCOPE_PSD_METHOD(5)
  AP_SCOPE_PSD_METHOD(6)
  AP_SCOPE_PSD_METHOD(7)
  AP_SCOPE_PSD_METHOD(8)
  AP_SCOPE_PSD_METHOD(9)
  AP_SCOPE_PSD_METHOD(10)
  AP_SCOPE_PSD_METHOD(11)
  AP_SCOPE_PSD_METHOD(12)
  AP_SCOPE_PSD_METHOD(13)
  AP_SCOPE_PSD_METHOD(14)
  AP_SCOPE_PSD_METHOD(15)
  AP_SCOPE_PSD_METHOD(16)
  AP_SCOPE_PSD_METHOD(17)
  AP_SCOPE_PSD_METHOD(18)
  AP_SCOPE_PSD_METHOD(19)
  AP_SCOPE_PSD_METHOD(20)
  AP_SCOPE_PSD_METHOD(21)
  AP_SCOPE_PSD_METHOD(22)
  AP_SCOPE_PSD_METHOD(23)
} // End of Definition Block

#undef AP_SCOPE_PSD_METHOD
