/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "CpuPowerMgmt.h"

#define AP_SCOPE_PSD_METHOD(INDEX, MODULE, DOMAIN) \
  External(\_SB.PR##INDEX, DeviceObj) \
  If (CondRefOf (\_SB.PR##INDEX)) { \
    Scope(\_SB.PR##INDEX) \
    { \
      Method(_PSD,0) \
      { \
        Return(\_SB.PR00.GPSD(DOMAIN, MODULE)) \
      } \
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
  External (\PF00, IntObj)
  External (\TCNT, FieldUnitObj)
  External (\_SB.CFGD, FieldUnitObj)
  External (\_SB.PR00.GPSD, MethodObj)
  External (CM01)
  External (CM02)
  External (CM03)
  External (CM04)
  External (CM05)
  External (CM06)
  External (CM07)
  External (CM08)
  External (CM09)
  External (CM10)
  External (CM11)
  External (CM12)
  External (CM13)
  External (CM14)
  External (CM15)
  External (CM16)
  External (CM17)
  External (CM18)
  External (CM19)
  External (CM20)
  External (CM21)
  External (CM22)
  External (CM23)
  External (CM24)
  External (CM25)
  External (CM26)
  External (CM27)
  External (CM28)
  External (CM29)
  External (CM30)
  External (CM31)
  External (CM32)
  External (CM33)
  External (CM34)
  External (CM35)
  External (CM36)
  External (CM37)
  External (CM38)
  External (CM39)
  External (CM40)
  External (CM41)
  External (CM42)
  External (CM43)
  External (CM44)
  External (CM45)
  External (CM46)
  External (CM47)
  External (CM48)
  External (CM49)
  External (CM50)
  External (CM51)
  External (CM52)
  External (CM53)
  External (CM54)
  External (CM55)
  External (CM56)
  External (CM57)
  External (CM58)
  External (CM59)
  External (CM60)
  External (CM61)
  External (CM62)
  External (CM63)
  //
  // The _PSD object provides information to the OSPM related
  // to P-State coordination between processors in a multi-processor
  // configurations.
  //
  AP_SCOPE_PSD_METHOD(01, CM01, 1)
  AP_SCOPE_PSD_METHOD(02, CM02, 2)
  AP_SCOPE_PSD_METHOD(03, CM03, 3)
  AP_SCOPE_PSD_METHOD(04, CM04, 4)
  AP_SCOPE_PSD_METHOD(05, CM05, 5)
  AP_SCOPE_PSD_METHOD(06, CM06, 6)
  AP_SCOPE_PSD_METHOD(07, CM07, 7)
  AP_SCOPE_PSD_METHOD(08, CM08, 8)
  AP_SCOPE_PSD_METHOD(09, CM09, 9)
  AP_SCOPE_PSD_METHOD(10, CM10, 10)
  AP_SCOPE_PSD_METHOD(11, CM11, 11)
  AP_SCOPE_PSD_METHOD(12, CM12, 12)
  AP_SCOPE_PSD_METHOD(13, CM13, 13)
  AP_SCOPE_PSD_METHOD(14, CM14, 14)
  AP_SCOPE_PSD_METHOD(15, CM15, 15)
  AP_SCOPE_PSD_METHOD(16, CM16, 16)
  AP_SCOPE_PSD_METHOD(17, CM17, 17)
  AP_SCOPE_PSD_METHOD(18, CM18, 18)
  AP_SCOPE_PSD_METHOD(19, CM19, 19)
  AP_SCOPE_PSD_METHOD(20, CM20, 20)
  AP_SCOPE_PSD_METHOD(21, CM21, 21)
  AP_SCOPE_PSD_METHOD(22, CM22, 22)
  AP_SCOPE_PSD_METHOD(23, CM23, 23)

  AP_SCOPE_PSD_METHOD(24, CM24, 24)
  AP_SCOPE_PSD_METHOD(25, CM25, 25)
  AP_SCOPE_PSD_METHOD(26, CM26, 26)
  AP_SCOPE_PSD_METHOD(27, CM27, 27)
  AP_SCOPE_PSD_METHOD(28, CM28, 28)
  AP_SCOPE_PSD_METHOD(29, CM29, 29)
  AP_SCOPE_PSD_METHOD(30, CM30, 30)
  AP_SCOPE_PSD_METHOD(31, CM31, 31)
  AP_SCOPE_PSD_METHOD(32, CM32, 32)
  AP_SCOPE_PSD_METHOD(33, CM33, 33)
  AP_SCOPE_PSD_METHOD(34, CM34, 34)
  AP_SCOPE_PSD_METHOD(35, CM35, 35)
  AP_SCOPE_PSD_METHOD(36, CM36, 36)
  AP_SCOPE_PSD_METHOD(37, CM37, 37)
  AP_SCOPE_PSD_METHOD(38, CM38, 38)
  AP_SCOPE_PSD_METHOD(39, CM39, 39)
  AP_SCOPE_PSD_METHOD(40, CM40, 40)
  AP_SCOPE_PSD_METHOD(41, CM41, 41)
  AP_SCOPE_PSD_METHOD(42, CM42, 42)
  AP_SCOPE_PSD_METHOD(43, CM43, 43)
  AP_SCOPE_PSD_METHOD(44, CM44, 44)
  AP_SCOPE_PSD_METHOD(45, CM45, 45)
  AP_SCOPE_PSD_METHOD(46, CM46, 46)
  AP_SCOPE_PSD_METHOD(47, CM47, 47)
  AP_SCOPE_PSD_METHOD(48, CM48, 48)
  AP_SCOPE_PSD_METHOD(49, CM49, 49)
  AP_SCOPE_PSD_METHOD(50, CM50, 50)
  AP_SCOPE_PSD_METHOD(51, CM51, 51)
  AP_SCOPE_PSD_METHOD(52, CM52, 52)
  AP_SCOPE_PSD_METHOD(53, CM53, 53)
  AP_SCOPE_PSD_METHOD(54, CM54, 54)
  AP_SCOPE_PSD_METHOD(55, CM55, 55)
  AP_SCOPE_PSD_METHOD(56, CM56, 56)
  AP_SCOPE_PSD_METHOD(57, CM57, 57)
  AP_SCOPE_PSD_METHOD(58, CM58, 58)
  AP_SCOPE_PSD_METHOD(59, CM59, 59)
  AP_SCOPE_PSD_METHOD(60, CM60, 60)
  AP_SCOPE_PSD_METHOD(61, CM61, 61)
  AP_SCOPE_PSD_METHOD(62, CM62, 62)
  AP_SCOPE_PSD_METHOD(63, CM63, 63)

} // End of Definition Block

#undef AP_SCOPE_PSD_METHOD
