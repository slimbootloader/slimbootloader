/** @file
  ACPI EC SSDT table for LID switch.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define FlavorMobile                        0x1
#define FlavorMobileWorkstation             0x6

//
// Define a Lid Switch.
//
Device (LID0)
{
  External (\PFLV, IntObj)
  External (LSWP, IntObj)
  External (IUBE, IntObj)
  External (\GPRW, MethodObj)
  External (\GGPE, MethodObj)

  Name (_HID, EISAID ("PNP0C0D"))
  Method (_STA)
  {
    If (LOr(LEqual (\PFLV, FlavorMobile),LEqual(\PFLV,FlavorMobileWorkstation))){
      Return (0x0F)
    }
    Return (0x00)
  }
  Method (_LID, 0)
  {
    // 0 = Closed, 1 = Open.
    If ((ECRD (RefOf (LSTE))) == LIDS) {
      Return (LIDS)
    } Else {
      Store(LIDS, Local0)
      Store(ECRD (RefOf (LSTE)), LIDS)
      Return (Local0)
    }
  }
  //
  // Enable SCI wake for LID SWITCH
  //
  If (LNotEqual (LSWP, 0)) {
    Method (_PRW, 0) {
      Return (GPRW (GGPE (LSWP), 3))
    }
  }
}
