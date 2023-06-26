/** @file
  ACPI EC SSDT table for Button Indicator.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// Hardware Button Array support
//
Device (BIND)  // Button Indicators.
{
  External (\OSYS)
  External (\PB1E, IntObj)
  External (\IUBE, IntObj)

  Name (_HID, "INT33D2")
  Name (_CID, "PNP0C40")

  Method (_STA, 0, Serialized)
  {
    If (LAnd (And (IUBE, 1), LGreaterEqual (\OSYS, 2013)))
    {
      If (And (\PB1E, 1)) // 10Sec Power Button is enabled?
      {
        Return (0x000B) // Not visible in UI
      }
      Else
      {
        Return (0x000F)
      }
    }
    Return (0x00)
  }
  //
  // _DSM : Device Specific Method for the Windows Compatible Button Array.
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index
  // Arg3: Package Parameters
  //
  Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
  {
    // Compare passed in UUID to supported UUID.

    If (LEqual (Arg0, ToUUID ("DFBCF3C5-E7A5-44E6-9C1F-29C76F6E059C")))
    {
      If (LEqual (0, ToInteger (Arg1)))        // Revision 0.
      {
        Switch (ToInteger (Arg2))            // Switch to Function Index.
        {
          //
          // Function 0, Query of supported functions.
          //

          Case (0)
          {
            Return (Buffer () {0x03})
          }

          //
          // Function 1, Windows Compatible Button Array Power Button Properties.
          //

          Case (1)
          {
            // Only return support if platform enabled via setup.

            If (LEqual (And (\PB1E, 1), 1))
            {
              Return (0x07)
            }

            Return (0x00)

          }
        } // End Switch statement
      }  // End Revision check
    }  // End UUID check

    // If the code falls through to this point, just return a buffer of 0.

    Return (Buffer () {0x00})

  }  // End _DSM Method
}
