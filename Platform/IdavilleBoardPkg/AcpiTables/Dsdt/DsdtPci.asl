/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// @NOTE: This should be read from hardware to reflect
// real PCI exress base. Currently we do not have such
// capability so hardcode is used instead.
//
#define PCI_EXPRESS_BASE 0x80000000

Scope(\_SB.PC00) {

  //
  // PCI-specific method's GUID
  //
  Name(PCIG, ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))
  //
  // PCI's _DSM - an attempt at modular _DSM implementation
  // When writing your own _DSM function that needs to include PCI-specific methods, do this:
  //
  // Method(_YOUR_DSM,4){
  //   if(Lequal(Arg0,PCIG)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
  //   ...continue your _DSM by checking different GUIDs...
  //   else { return(0) }
  // }
  //
  Method(PCID, 4, Serialized) {
    If(LEqual(Arg0, PCIG)) {         // PCIE capabilities UUID
      If(LGreaterEqual(Arg1,3)) {                                              // revision at least 3
        If(LEqual(Arg2,0)) { Return (Buffer(2){0x01,0x03}) }                   // function 0: list of supported functions
        If(LEqual(Arg2,8)) { Return (1) }                                      // function 8: Avoiding Power-On Reset Delay Duplication on Sx Resume
        If(LEqual(Arg2,9)) { Return (Package(5){50000,Ones,Ones,50000,Ones}) } // function 9: Specifying Device Readiness Durations
      }
    }
    return (Buffer(1){0})
  }

  Method(PCIC,1,Serialized) {
    return (0)
  }

  //
  // Get the PCI express base address.
  // Currently hardcoded.
  //
  Method (GPCB) {
    Return (PCI_EXPRESS_BASE)
  }
}

