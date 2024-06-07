/** @file
  ACPI DSDT table

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//----------------------------------------------------------------------------------------
//  Intel Proprietary IMGCLK Enabling DSM Method
//
//  Method (_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
//  Arguments:
// Arg0: UUID Unique function identifier
// Arg1: Integer Revision Level
// Arg2: Integer Function Index
// Arg3: Package Parameters -
//       Index 0 - Clock source select (0: IMGCLKOUT_0, 1: IMGCLKOUT_1, 2: IMGCLKOUT_2, 3: IMGCLKOUT_3, 4: IMGCLKOUT_4)
//       Index 1 - Clock Enable / Disable (0: Disable, 1: Enable)
//       Index 2 - Select 24MHz / 19.2 MHz (0: 24MHz, 1: 19.2MHz)
//-----------------------------------------------------------------------------------------
// DSM UUID for IMGCLK Enabling. Do Not change.
If (LEqual (Arg0, ToUUID ("82C0D13A-78C5-4244-9BB1-EB8B539A8D11")))
{
  // Function 0 : Query Function
  If (LEqual (Arg2, Zero))
  {
    // Revision 0
    If (LEqual (Arg1, Zero)) // The current revision is 0
    {
      Return (Buffer (One) { 0x03 }) // There are 1 function defined other than Query.
    }
    Else
    {
      Return (0) // Revision mismatch
    }
  }
  // Function 1 :
  If (LEqual (Arg2, One))
  {
    \_SB.ICLK.CLKC (ToInteger (DeRefOf (Index (Arg3, 0))), ToInteger (DeRefOf (Index (Arg3, 1))))
    \_SB.ICLK.CLKF (ToInteger (DeRefOf (Index (Arg3, 0))), ToInteger (DeRefOf (Index (Arg3, 2))))
  }
  Else
  {
    Return (0) // Function number mismatch but normal return.
  }
}
