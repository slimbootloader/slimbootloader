/**@file
  CNVi Common ACPI definitions

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PcieRegs.h>
#include <Register/CnviRegs.h>

Scope (\_SB.PC00) {
  //
  // Set BT_EN
  //
  Method (SBTE, 0x1, Serialized) {
    //
    // Arg0 - Value to BT_EN
    //
    If (LNotEqual (GBTP, 0)) {
      Store (GBTP, Local0)
      \_SB.SGOV (Local0, Arg0)

      // S0i2/3 dynamic switching for CNVi BT
      \_SB.S023 (3, Arg0)
    }
  }

  //
  // Get BT_EN value
  //
  Method (GBTE, 0) {
    If (LNotEqual (GBTP, 0)) {
      Store (GBTP, Local0)
      Return (\_SB.GGOV (Local0))
    } Else {
      Return (0)
    }
  }

  //
  // Update BT Audio Offload setting
  //
  Method (AOLX) {
    // BT Audio Offload data structure
    Name (AODS, Package () {
      0,    // Revision
      0x12, // DomainType, 0x12:Bluetooth Core
      0     // Audio Offload selection, 0: Disabled, 1:Enabled
      })

    If (LEqual (CBTA, 1)) {
      Store (1, Index (AODS, 2))
    }
    Return (AODS)
  }
}

//
// AOLD (Audio Offload)
//   Arguments:
//     None
//   Return Value:
//     Revision
//     DomainType
//     Audio Offload selection, default value is Disabled
//
If (LAnd (LEqual (CRFP, 1), LEqual (CBTC, 1))) {
  If (LEqual (CBTI, 1)) {
    If (LEqual (PU2C, 10)) {
      Scope (\_SB.PC00.XHCI.RHUB.HS10) {
        Method (AOLD) {
          Return (AOLX ())
        }
      }
    } ElseIf (LEqual (PU2C, 14)) {
      Scope (\_SB.PC00.XHCI.RHUB.HS14) {
        Method (AOLD) {
          Return (AOLX ())
        }
      }
    } ElseIf (LEqual (PU2C, 8)) {
      Scope (\_SB.PC00.XHCI.RHUB.HS08) {
        Method (AOLD) {
          Return (AOLX ())
        }
      }
    }
  }
}
