/** @file
  ACPI RTD3 SSDT table for SATA ports

  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
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
External(IR3D, MethodObj)
External(_ADR, IntObj)
//
// AcpiPinDriverLib imports(platform DSDT)
//
External(\PIN.ON, MethodObj)
External(\PIN.OFF, MethodObj)
External(\PIN.STA, MethodObj)

Method(STNC, Zero, Serialized) // Check SATA config
{
  If (LEqual (\_SB.PC00.SAT0.MBR6, 0xFFFFFFFF)) {
    Return(0x01)             // no SATA ABAR base address present
  }
  Add(\_SB.PC00.SAT0.MBR6, PBAR, Local0)
  OperationRegion(ASTS, SystemMemory,Local0, 0x04)
  Field(ASTS, DWordAcc, NoLock, Preserve)
  {
    AMST, 32,
  }
  If (LEqual (AMST, 0xFFFFFFFF)) {
    Return(0x01)             // no SATA present
  }
  Return(0x00)
}

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
    If (STNC()) {
      Return(0x00)
    }
    Return(SPSA())
  }

  Method(_ON) {
    If (STNC()) {
      Return()
    }
    //
    // If RAID support has been added check if RTD3 isn't blocked - IR3D method
    // If RTD3 is blocked return.
    //
    If (IR3D()) {
      Return()
    }
    SPON()            // _ON Method
    Sleep(16)       // Delay for power ramp.
  }

  Method(_OFF) {
    If (STNC()) {
      Return()
    }
    //
    // If RAID support has been added check if RTD3 isn't blocked - IR3D method
    // If RTD3 is blocked return.
    //
    If (IR3D()) {
      Return()
    }
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
  Add(\_SB.PC00.SAT0.MBR6, PBAR, Local0)
  /// if S0Ix enabled
  If(LEqual(S0ID, 1)) {
    OperationRegion(PSTS, SystemMemory, Local0, 0x18)
    Field(PSTS, DWordAcc, NoLock, Preserve)
    {
      Offset(0x0),
      CMST, 1,  //PxCMD.ST
      CSUD, 1,  //PxCMD.SUD
      , 2,
      CFRE, 1,  //PxCMD.FRE
      Offset(0x10),
      SDET, 4,  //PxSSTS.DET
      Offset(0x14),
      CDET, 4   //PxSCTL.DET
    }

    // WA for RST driver creating volume failed.
    // In order to fix the error caused by reigster change during creating volume of RST driver, skip to disable phy in RAID mode.
    If(LAnd(LEqual(\_SB.PC00.SAT0.SUBC, 0x06),LOr(LEqual(SDET, 1), LEqual(SDET, 3))))  { /// Execute offline flow only if Device detected and Phy not offline
      ///- Clear ST (PxCMD.ST)
      Store(0, CMST)                   // PBAR[0]
      ///- Clear FRE
      Store(0, CFRE)                   // PBAR[4]
      ///- Clear SUD (PxCMD.SUD)
      Store(0, CSUD)                   // PBAR[1]
      ///- Set DET to 4 (PxSCTL.DET)
      Store(4, CDET)                   // PBAR+0x14[3:0]
      Sleep(16)
      ///- Wait until PxSSTS.DET == 4
      While(LNotEqual(SDET, 4)){
        Sleep(16)
      }
    }
  } // if S0Ix enabled
  // drive power pin "OFF"
  \PIN.OFF(PWRG)
  Store(Timer(), ^OFTM) /// Store time when Disk turned OFF(non-zero OFTM indicate minimum 50ms requirement does apply when _ON called next time)
}
