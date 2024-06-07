/** @file
  ACPI DSDT table

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Comms Hub module definition.
//
scope (\)
{
  Device (CHUB)
  {
    Name (_HID, EISAID ("INT339B"))

    Method (_STA)
    {
      If (LEqual (CHEN, 1))
      {
        Return (0x0F) // Comms Hub enabled, show it
      }
      Return (0x0)
    }

    //--------------------
    //  Comms Hub Event solution
    //--------------------
    //
    // _DSM (Device Specific Method)
    //
    // This optional object is a control method that enables devices to provide device specific control
    // functions that are consumed by the device driver.
    //
    // Arguments: (4)
    //   Arg0 - A Buffer containing a UUID
    //   Arg1 - An Integer containing the Revision ID
    //   Arg2 - An Integer containing the Function Index
    //   Arg3 - A Package that contains function-specific arguments
    // Return Value:
    //   If Function Index = 0, a Buffer containing a function index bit-field.
    //   Otherwise, the return value and type depends on the UUID and revision ID.
    //
    Method (_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
    {
      //
      // verify UUID -- "1730E71D-E5DD-4A34-BE57-4D76B6A2FE37"
      //
      If (LEqual (Arg0, ToUUID ("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37")))
      {
        If (LGreaterEqual (ToInteger (Arg1),0))        // Revision 0.
        {
          Switch (ToInteger (Arg2))            // Switch to Function Index.
          {
            //
            // Function 0, Query Function - return supported functions BitIndex.
            //
            Case (0)
            {
              Return (Buffer () {0x03}) // There is 1 function defined other than Query.
            }
            //
            // Function 1,  -
            //
            Case (1)
            {
              Switch (ToInteger (DeRefOf (Index (Arg3, 0))))
              {
                Case (0) // Send HID Event/Wake Notification
                {
                  \_SB.HIDD.HPEM (0x1C)
                }
              }
            }
          } // End Function Index
          Return (0)
        }  // End Revision check
        Return (0)
      }  // End UUID check
      Return (Buffer () {0})
    }  // End _DSM Method
  }
}