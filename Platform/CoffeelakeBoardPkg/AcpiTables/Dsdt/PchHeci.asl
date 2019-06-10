/**@file
  ACPI DSDT table for MEI1

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
External(HIDW, MethodObj)
External(HIWC, MethodObj)

Scope(\_SB.PCI0) {
  //
  // Management Engine Interface 1 - Device 22, Function 0
  //
  Device(HECI) {
    Name(_ADR, 0x00160000)

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
  } // Device(HECI)
}

