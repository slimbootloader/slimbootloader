/** @file
  Intel Processor Power Management CST ACPI Code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define AP_SCOPE_CST_METHOD(INDEX) \
  External(\_SB.PR##INDEX, DeviceObj) \
  Scope(\_SB.PR##INDEX) \
  { \
    Method(_CST,0) \
    { \
      Return(\_SB.PR00._CST) \
    } \
  }


DefinitionBlock (
  "APCST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApCst",
  0x3000
  )
{
  External(\_SB.PR00._CST)

  AP_SCOPE_CST_METHOD(01)
  AP_SCOPE_CST_METHOD(02)
  AP_SCOPE_CST_METHOD(03)
  AP_SCOPE_CST_METHOD(04)
  AP_SCOPE_CST_METHOD(05)
  AP_SCOPE_CST_METHOD(06)
  AP_SCOPE_CST_METHOD(07)
  AP_SCOPE_CST_METHOD(08)
  AP_SCOPE_CST_METHOD(09)
  AP_SCOPE_CST_METHOD(10)
  AP_SCOPE_CST_METHOD(11)
  AP_SCOPE_CST_METHOD(12)
  AP_SCOPE_CST_METHOD(13)
  AP_SCOPE_CST_METHOD(14)
  AP_SCOPE_CST_METHOD(15)
  AP_SCOPE_CST_METHOD(16)
  AP_SCOPE_CST_METHOD(17)
  AP_SCOPE_CST_METHOD(18)
  AP_SCOPE_CST_METHOD(19)
  AP_SCOPE_CST_METHOD(20)
  AP_SCOPE_CST_METHOD(21)
  AP_SCOPE_CST_METHOD(22)
  AP_SCOPE_CST_METHOD(23)
}  // End of Definition Block

#undef AP_SCOPE_CST_METHOD
