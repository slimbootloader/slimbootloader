/** @file
  ACPI DSDT table


  Copyright (c) 2011 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(UPBT, MethodObj)

  Method (HIDW, 4, Serialized)
  {
    //--------------------------------------------
    //  Intel Proprietary Wake up Event solution
    //
    //  Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
    //
    //--------------------------------------------
    // DSM UUID for Intel Proprietary Wake up Event solution. Do Not change.
    If (LEqual(Arg0, ToUUID("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37")))
    {
      // Function 0 : Query Function
      If (LEqual(Arg2, Zero))
      {
        // Revision 0
        If (LEqual(Arg1, Zero)) // The current revision is 0
        {
          Return(Buffer(One) { 0x03 }) // There are 1 function defined other than Query.
        }
        Else
        {
          Return(0) // Revision mismatch
        }
      }
      // Function 1 :
      If(LEqual(Arg2, One))
      {
        Switch(ToInteger(DeRefOf(Index(Arg3, 0))))
        {
          Case (0) // Send HID Event/Wake Notification
          {
            If (CondRefOf(\_SB.HIDD.HPEM))
            {
              \_SB.HIDD.HPEM(27)
              //P8XH(0,0x5C)
            }
          }
          Case (1) // Send Sleep button Notification. Put the system to Sleep
          {
            If (CondRefOf(\_SB.SLPB))
            {
              Notify(\_SB.SLPB, 0x80)
              //P8XH(0,0x5D)
            }
          }
          Case (2) // Send Dock Event
          {
          }
          Case (3) // Send Undock Event
          {
          }
        }
        Return(0)
      }
      Else
      {
        Return(0) // Function number mismatch but normal return.
      }
    }
    Else
    {
      Return(Buffer(One) { 0x00 }) // Guid mismatch
    }
  } // End Method (HIDW)

  Method (HIWC, 1) // HID WAKE CHECK
  {
    If (LEqual(Arg0, ToUUID("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37")))
    {
      Return (1)
    }
    Return (0)
  } // End Method (HIWC)
