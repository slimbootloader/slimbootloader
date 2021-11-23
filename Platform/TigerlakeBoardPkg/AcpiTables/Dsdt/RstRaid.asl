/** @file
  RST RAID support in ACPI

@copyright

  Copyright 2015 - 2019 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Indicate to the RST driver whether it is required to support D3 on the SATA controller.
// This method is called via a _DSM method in SAT0 scope.
// Method GUID: {9CD9CDDD-8845-4AFD-8392-31C4EB87CABD}
// Argument:
// Arg0 - Revision id passed from _DSM : IntObj
// Arg1 - Function index : IntObj
// Arg2 - Package of arguments : PkgObj
//
Method(D3AS, 3, Serialized,,, {IntObj, IntObj, PkgObj}) {
  Switch(ToInteger(Arg1)) {
    Case(0) {
      // This method supports function index 0 and 1
      Return(Buffer() {0x3})
    }
    Case(1) {
      // For now D3 support for adapter is required only if PS_ON is enabled.
      If(LEqual(PSON, 1)) {
        Return(Buffer() {1})
      } Else {
        Return(Buffer() {0})
      }
    }
  }
  Return(Buffer() {0})
}
