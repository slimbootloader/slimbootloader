/** @file
  Intel Processor AP TST ACPI Code.

  Copyright (c) 1999 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(INDEX, DOMAIN) \
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
      Return(\_SB.PR00.GTSD(DOMAIN)) \
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

  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(01, 1)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(02, 2)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(03, 3)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(04, 4)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(05, 5)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(06, 6)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(07, 7)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(08, 8)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(09, 9)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(10, 10)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(11, 11)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(12, 12)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(13, 13)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(14, 14)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(15, 15)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(16, 16)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(17, 17)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(18, 18)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(19, 19)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(20, 20)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(21, 21)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(22, 22)
  AP_SCOPE_TPC_PTC_TSS_TSD_METHOD(23, 23)
} // End of Definition Block

#undef AP_SCOPE_TPC_PTC_TSS_TSD_METHOD
