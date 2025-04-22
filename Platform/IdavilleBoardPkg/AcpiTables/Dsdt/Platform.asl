/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Port from client
//

//
// Original file line: 91
//


  External (\_SB.OSNC, MethodObj)


//
// Original file line: 163
//

Method(ADBG,1,Serialized)
{
  Return(0)
}

//
// Original file line: 1460
//
Scope (\)
{
  //
  // Global Name, returns current Interrupt controller mode;
  // updated from _PIC control method
  //

  //
  // Procedure: GPRW
  //
  // Description: Generic Wake up Control Method ("Big brother")
  //              to detect the Max Sleep State available in ASL Name scope
  //              and Return the Package compatible with _PRW format.
  // Input: Arg0 =  bit offset within GPE register space device event will be triggered to.
  //        Arg1 =  Max Sleep state, device can resume the System from.
  //                If Arg1 = 0, Update Arg1 with Max _Sx state enabled in the System.
  // Output:  _PRW package
  //
  Name(PRWP, Package(){Zero, Zero})   // _PRW Package

  Method(GPRW, 2)
  {
    Store(Arg0, Index(PRWP, 0))             // copy GPE#
    //
    // SS1-SS4 - enabled in BIOS Setup Sleep states
    //
    Store(ShiftLeft(SS1,1),Local0)          // S1 ?
    Or(Local0,ShiftLeft(SS2,2),Local0)      // S2 ?
    Or(Local0,ShiftLeft(SS3,3),Local0)      // S3 ?
    Or(Local0,ShiftLeft(SS4,4),Local0)      // S4 ?
    //
    // Local0 has a bit mask of enabled Sx(1 based)
    // bit mask of enabled in BIOS Setup Sleep states(1 based)
    //
    If(And(ShiftLeft(1, Arg1), Local0))
    {
      //
      // Requested wake up value (Arg1) is present in Sx list of available Sleep states
      //
      Store(Arg1, Index(PRWP, 1))           // copy Sx#
    }
    Else
    {
      //
      // Not available -> match Wake up value to the higher Sx state
      //
      ShiftRight(Local0, 1, Local0)
      // If(LOr(LEqual(OSFL, 1), LEqual(OSFL, 2))) {  // ??? Win9x
      // FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Max Sx
      // } Else {           // ??? Win2k / XP
     FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Min Sx
      // }
    }

    Return(PRWP)
  }
}

//
// Access BIOSSCRATCHPAD3_CFG (UBOX Dev 0 Func 2 PCI_Config 0xAC)
//
OperationRegion (SPD3, SystemMemory, 0x8FE020AC, 0x4)
Field(SPD3, DWordAcc, NoLock, Preserve)
{
  FWSC,  8,
}

//
// Platform specific FWU trigger method
//
Method(FWUC, 2)
{
  If(LEqual(Arg0, Zero)) {
    // Read
    And(FWSC, 0x00FF, Local0)
  } Else {
    // Write
    And(ToInteger(Arg1), 0xFF, Local0)
    And(FWSC, 0x00, Local1)
    Or(Local0, Local1, FWSC)
  }
  Return (Local0)
}
