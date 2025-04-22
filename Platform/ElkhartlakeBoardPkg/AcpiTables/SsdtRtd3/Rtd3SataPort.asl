/** @file
  ACPI RTD3 SSDT table for SATA ports

  Copyright (c) 2011 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Include in the SATA port scope(PRTx) to implement RTD3 functionality
//

/// @details
/// Code in this file uses following variables:
/// PBAR: SATA command port base address
/// Below objects should be defined according to the format described in PinDriverLib.asl
/// PWRG: power control pin - mandatory
/// @defgroup SATA port Scope **/

/// Define _PR0, _PR3 PowerResource Package
// command port = ABAR + PBAR

//
// DSDT imports
//
//External(IR3D, MethodObj)
//External(_ADR, IntObj)
//
// AcpiPinDriverLib imports(platform DSDT)
//
External(\PIN.ON, MethodObj)
External(\PIN.OFF, MethodObj)
External(\PIN.STA, MethodObj)

Method(_PR0) {
  Return(Package(){SPPR})
}

Method(_PR3) {
  Return(Package(){SPPR})
}

Method(_S0W) {
  Return(4)
}

PowerResource(SPPR, 0, 0) {
  Method(_STA) {
    Return(SPSA())
  }

  Method(_ON) {
    //
    // If RAID support has been added check if RTD3 isn't blocked - IR3D method
    // If RTD3 is blocked return.
    //
    //If (IR3D()) {
    //  Return()
    //}
    SPON()            // _ON Method
    Sleep(16)       // Delay for power ramp.
  }

  Method(_OFF) {
    //
    // If RAID support has been added check if RTD3 isn't blocked - IR3D method
    // If RTD3 is blocked return.
    //
    //If (IR3D()) {
    //  Return()
    //}
    SPOF()
  }
}

Name(OFTM, Zero) /// OFTM: Time returned by Timer() when the Power resource was turned OFF

Method(SPSA) {
  Return (\PIN.STA(PWRG))
}

Method(SPON, 0) {
  \PIN.ON(PWRG)
}

Method(SPOF, 0, Serialized) {

  // drive power pin "OFF"
  \PIN.OFF(PWRG)
  Store(Timer(), ^OFTM) /// Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
}
