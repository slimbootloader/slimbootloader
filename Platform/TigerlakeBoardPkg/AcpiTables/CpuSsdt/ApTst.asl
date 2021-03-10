/** @file
  Intel Processor AP TST ACPI Code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(INDEX) \
  External(\_SB.PR##INDEX, DeviceObj) \
  Scope(\_SB.PR##INDEX) \
  { \
    Method(_TPC, 0) \
    { \
      Return(\_SB.PR00._TPC) \
    } \
    Method(_PTC, 0) \
    { \
      Return(\_SB.PR00._PTC) \
    } \
    Method(_TSS, 0) \
    { \
      Return(\_SB.PR00._TSS) \
    } \
    Method(_TSD, 0) \
    { \
      Return(\_SB.PR00.GTSD(INDEX)) \
    } \
  }


DefinitionBlock(
  "APTST.aml",
  "SSDT",
  0x02,
  "PmRef",
  "ApTst",
  0x3000
  )
{
  External(\_SB.PR00._TPC, MethodObj)
  External(\_SB.PR00._PTC, MethodObj)
  External(\_SB.PR00._TSS, MethodObj)
  External(\_SB.PR00.GTSD, MethodObj)

  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(1)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(2)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(3)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(4)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(5)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(6)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(7)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(8)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(9)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(10)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(11)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(12)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(13)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(14)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(15)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(16)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(17)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(18)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(19)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(20)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(21)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(22)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(23)
} // End of Definition Block

#undef AP_SCOPE_TPC_PTC_TSS_TSD_METHOD
