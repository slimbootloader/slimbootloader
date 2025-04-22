/** @file
  This file contains the APs HWP SSDT Table ASL code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define AP_SCOPE_CPC_METHOD(INDEX) \
  External(\_SB.PR##INDEX, DeviceObj) \
  Scope(\_SB.PR##INDEX) \
  { \
    Method(_CPC,0) \
    { \
      Return(\_SB.PR00._CPC) \
    } \
  }


DefinitionBlock (
  "ApHwp.aml",
  "SSDT",
  2,
  "PmRef",
  "ApHwp",
  0x3000
  )
{
  External(\_SB.PR00._CPC, MethodObj)

  AP_SCOPE_CPC_METHOD(01)
  AP_SCOPE_CPC_METHOD(02)
  AP_SCOPE_CPC_METHOD(03)
  AP_SCOPE_CPC_METHOD(04)
  AP_SCOPE_CPC_METHOD(05)
  AP_SCOPE_CPC_METHOD(06)
  AP_SCOPE_CPC_METHOD(07)
  AP_SCOPE_CPC_METHOD(08)
  AP_SCOPE_CPC_METHOD(09)
  AP_SCOPE_CPC_METHOD(10)
  AP_SCOPE_CPC_METHOD(11)
  AP_SCOPE_CPC_METHOD(12)
  AP_SCOPE_CPC_METHOD(13)
  AP_SCOPE_CPC_METHOD(14)
  AP_SCOPE_CPC_METHOD(15)
  AP_SCOPE_CPC_METHOD(16)
  AP_SCOPE_CPC_METHOD(17)
  AP_SCOPE_CPC_METHOD(18)
  AP_SCOPE_CPC_METHOD(19)
  AP_SCOPE_CPC_METHOD(20)
  AP_SCOPE_CPC_METHOD(21)
  AP_SCOPE_CPC_METHOD(22)
  AP_SCOPE_CPC_METHOD(23)
}// end of definition block

#undef AP_SCOPE_CPC_METHOD
