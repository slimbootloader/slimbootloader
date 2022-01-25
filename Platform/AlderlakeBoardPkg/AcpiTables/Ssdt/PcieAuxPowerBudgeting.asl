/**@file
  Methods for PCIe auxilary power budgeting support.
  Include it in the PCIe root port scope that supports aux. power budgeting

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name(CURP, 0) // Currently used auxilary power on this PCIe slot.

//
// To use this code define in the platform specific file following names
// RAPC - Platform wide power budgeting. Located in the "\" scope.
// PSPL - PCIe slot auxilary power limit. Located in the PCIe root port scope.
// BCPR - Block core power removal on the slot. Located in the PCIe root port scope.
//

//
// Auxilary power allocation method.
// Arg0 - New power to be allocated
//
// @return 0 - power allocation failed, 1 - power allocation succeded
//
Method(PALC, 1, Serialized) {
  If(LGreater(Arg0, PSPL)) {
    Return(0)
  } Else {
     If(LGreater(Arg0, CURP)) {
      //
      // If the power request is greater then what we are using right now
      // then determine how much more power we have to reserve and try to resrve it.
      //
      Subtract(Arg0, CURP, Local0)
      If(RAPC(1, Local0)) {
        //
        // If the power request has been accepted then update currently used power
        // and return success to the driver.
        //
        Store(Arg0, CURP)
        Return(1)
      } Else {
        Return(0)
      }
    } Else {
      //
      // If the power request is lower or equal then what we are using right now
      // simply obtain the difference and release the power to the platform.
      //
      Subtract(CURP, Arg0, Local0)
      RAPC(0, Local0) // We don't care about the status since it is release
      Return(1)
    }
  }
}

//
// PCIe auxilary power budget initialization.
// This method will be called from reference code during _INI
// method execution for the root port.
//
Method(PINI, 0, Serialized) {
  //
  // At init time allocate PCIe CEM form factor defined auxilary power for the slot
  // which is 1238mW.
  //
  If(PALC(1238)) {
    BCPR(0)//allow for core power removal
  } Else {
    BCPR(1)//block core power removal
  }
}

//
// PCIe power budget allocation.
// This method will be called from reference code
// during PCIe _DSM function 10 evaluation.
// On each call to this method the current power used by the slot
// will be updated to reflect last successfully negotiated value.
//
// Arg0 - New requested auxilary power from the driver in mW.
//
// @return Alocation status as defined by PCIe _DSM function index 11
//      0h - Denied
//      1h - Granted
//      2h - No main power removal
//
Method(PPBA, 1, Serialized) {
  //
  // Value of 0x80000000 signifies that bus driver wants to block core power
  // removal for entire PCI hierarchy connected to this root port.
  //
  If(LEqual(Arg0, 0x80000000)) {
    BCPR(1)//block core power removal
    Return(2)
  } Else {
    If(PALC(Arg0)) {
      BCPR(0)//allow for core power removal
      Return(1)
    } Else {
      BCPR(1)//block core power removal
      Return(0)
    }
  }
}

