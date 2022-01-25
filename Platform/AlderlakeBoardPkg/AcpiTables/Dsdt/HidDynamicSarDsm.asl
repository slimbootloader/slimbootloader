/** @file
  ACPI DSDT table

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
  Method (ISAR, 4)
  {
    Name (BUF1, Buffer(2) {0,0})
    //--------------------------------------------
    //  Intel Proprietary Dynamic SAR solution
    //
    //  Method(_DSM, 0x4, Serialized, 0, {IntObj, BuffObj}, {BuffObj, IntObj, IntObj, PkgObj})
    //
    //--------------------------------------------
    If (LEqual(Arg0, ToUUID("1730E71D-E5DD-4A34-BE57-4D76B6A2FE37")))
    {
        Return(0) // Function number mismatch but normal return.
    }
    Else
    {
      Return(Buffer(One) { 0x00 }) // Guid mismatch
    }
  } // End Method (ISAR)
