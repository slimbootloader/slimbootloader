/**@file
  ACPI DSDT table for ISH

  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00) {
  //
  // Integrated Sensor Hub (PCI Mode)
  //
#ifdef PCH_ICL
if(LNotEqual(PCHS, PCHN)) {
#endif
  Device(ISHD) {
    Name(_ADR, 0x00120000)
    Method(_DSM, 0x4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }

      //-------------------------------------------
      //  HID Wake up Event solution
      //-------------------------------------------
      If(CondRefOf(HIWC)) {
        If(HIWC(Arg0)) {
          If(CondRefOf(HIDW)) {
            Return (HIDW(Arg0, Arg1, Arg2, Arg3))
          }
        }
      }

      Return(Buffer() {0})
    } // End _DSM
  } // Device(ISHD)
#ifdef PCH_ICL
  } // PCH_ICL
#endif
}
