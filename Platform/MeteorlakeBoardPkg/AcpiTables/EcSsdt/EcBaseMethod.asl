/** @file
  EC SSDT Base method for read, write and send commands.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.SGOV, MethodObj)
External (\_SB.GGIV, MethodObj)
External (\_SB.ASRT, MethodObj)

Name (ECTK, One)    // ECDT (Embedded Controller Boot Resources Table) Check to correct ECAV flag in the beginning

Mutex (ECMT, 0)

// ECRD (Embedded Read Method)
//
// Read the the EC RAM data
//
//  Arguments: (1)
//    Arg0 - Object to Read
//  Return Value:
//    Read Value
//    0xFFFFFFFF to indicate EC is not available
//
Method (ECRD, 1, Serialized, 0, IntObj, FieldUnitObj)
{
  //
  // Check for ECDT support, set ECAV to One if ECDT is supported by OS
  // Only check once at beginning since ECAV might be clear later in certain conditions
  //
  If (ECTK) {
    If (LGreaterEqual (_REV, 2)) {
      Store (One, ECAV)
    }
    Store (Zero, ECTK)   // Clear flag for checking once only
  }

  Store (Acquire (ECMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
  If (LEqual (Local0, Zero))  // check for Mutex acquired
  {
    If (ECAV) {
      Store (DerefOf (Arg0), Local1) // Execute Read from EC
      Release (ECMT)
      Return (Local1)
    }
    Else
    {
      Release (ECMT)
    } // If (ECAV)
  } // If EC Acquired
  Return (0xFFFFFFFF)
}

// ECWT (Embedded Write Method)
//
// Update the data to EC RAM
//
//  Arguments: (2)
//    Arg0 - Value to Write
//    Arg1 - Object to Write to
//  Return Value:
//    0x0         - Update EC Ram successfully
//    0xFFFFFFFF  - Fail to update the EC RAM
//
Method (ECWT, 2, Serialized,,, {IntObj, FieldUnitObj})
{
  //
  // Check for ECDT support, set ECAV to One if ECDT is supported by OS
  // Only check once at beginning since ECAV might be clear later in certain conditions
  //
  If (ECTK) {
    If (LGreaterEqual (_REV, 2)) {
      Store (One, ECAV)
    }
    Store (Zero, ECTK)   // Clear flag for checking once only
  }

  Store (Acquire (ECMT, 1000), Local0)  // save Acquire result so we can check for Mutex acquired
  If (LEqual (Local0, Zero))  // check for Mutex acquired
  {
    If (ECAV) {
      Store (Arg0, Arg1) // Execute Write to EC
    } // If (ECAV)
    Else {
      Return (0xFFFFFFFF)
    }
    Release (ECMT)
    Return (0x0)
  } // If EC Acquired
  Return (0xFFFFFFFF)
}

//
// ECMD (Embedded Controller Command)
//
// Handle all commands sent to EC by BIOS
//
//  Arguments: (1)
//    Arg0 - EC command
//  Return Value:
//    0x00        = Success
//    0xFFFFFFFF  = Failure
//
Method (ECMD, 1, Serialized)
{
  //
  // Check for ECDT support, set ECAV to One if ECDT is supported by OS
  // Only check once at beginning since ECAV might be clear later in certain conditions
  //
  If (ECTK) {
    If (LGreaterEqual (_REV, 2)) {
      Store (One, ECAV)
    }
    Store (Zero, ECTK)   // Clear flag for checking once only
  }

  If (ECAV) {
    Store (ECRD (RefOf (CMDR)), Local0)
    Store (0, Local1)
    //
    // Time out value is 10 second
    //
    While (LAnd (Local0, LLess (Local1, 25000))) {
      Stall (20)
      Store (ECRD (RefOf (CMDR)), Local0)
      Increment (Local1)
    }

    If (LGreaterEqual (Local1, 25000)) {
      If (CondRefOf (\_SB.ASRT)) {
        \_SB.ASRT (0x9FEC, Arg0)
      }
      Return (0xFFFFFFFF)
    }

    ECWT (Arg0, RefOf (CMDR))
    Return (0x00)
  }
  Return (0xFFFFFFFF)
}
