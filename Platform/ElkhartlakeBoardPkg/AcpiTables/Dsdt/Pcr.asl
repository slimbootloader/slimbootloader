/**@file
  Intel ACPI Reference Code for PCR

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\)
{
  //
  // PCR Register Access Methods
  //
  // PCR Dword Read
  // arg0: PID
  // arg1: Offset
  //
  Method (PCRR, 2, Serialized) {
    Add (ShiftLeft (arg0, 16), arg1, Local0)
    Add (SBRG, Local0, Local0)
    OperationRegion (PCR0, SystemMemory, Local0, 0x4)
    Field(PCR0,DWordAcc,Lock,Preserve) {
      Offset(0x00),
      DAT0, 32
    } // End Field PCR0
    Return (DAT0)
  } // End Method PCRR

  //
  // PCR Dword Write
  // arg0: PID
  // arg1: Offset
  // arg2: write data
  //
  Method (PCRW, 3, Serialized) {
    Add (ShiftLeft (arg0, 16), arg1, Local0)
    Add (SBRG, Local0, Local0)
    OperationRegion (PCR0, SystemMemory, Local0, 0x4)
    Field(PCR0,DWordAcc,Lock,Preserve) {
      Offset(0x00),
      DAT0, 32
    } // End Field PCR0
    Store (arg2, DAT0)
  } // End Method PCRW

  //
  // PCR Dword Or
  // arg0: PID
  // arg1: Offset
  // arg2: Or data
  //
  Method (PCRO, 3, Serialized) {
    Store(PCRR(arg0,arg1),Local0)    // Store PCR Read data in Local0
    Store(Or(Local0,arg2),Local1)    // Or data
    PCRW(arg0,arg1,Local1)           // Write data back
  }

  //
  // PCR Dword And
  // arg0: PID
  // arg1: Offset
  // arg2: And data
  //
  Method (PCRA, 3, Serialized) {
    Store(PCRR(arg0,arg1),Local0)    // Store PCR Read data in Local0
    Store(And(Local0,arg2),Local1)   // And data
    PCRW(arg0,arg1,Local1)           // Write data back
  }

  //
  // PCR Dword AndThenOr
  // arg0: PID
  // arg1: Offset
  // arg2: And data
  // arg3: Or data
  //
  Method (PCAO, 4, Serialized) {
    Store(PCRR(arg0,arg1),Local0)           // Store PCR Read data in Local0
    Store(Or(And(Local0,arg2),arg3),Local1) // AndThenOr
    PCRW(arg0,arg1,Local1)                  // Write data back
  }
}
