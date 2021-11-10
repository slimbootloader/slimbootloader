/** @file
  ACPI Lid Switch Control Method Support

 Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
External(\_SB.PC00.GFX0.CLID)                 // LID STATUS
Scope(\_SB)
{
  // Define a Lid Switch.
  Device(LID0)
  {
    Name(_HID,EISAID("PNP0C0D"))
    Method(_STA)
    {
      Return(0x0F)
    }

    Method(_LID,0)
    {
    // 0 = Closed, 1 = open.
    //
    // There is no LID support in EHL. Default it to open to avoid blank display in OS.
    //
      Store(0x80000003,\_SB.PC00.GFX0.CLID)
      Store(0x1,LIDS)
      Return(1)
    }

    //
    // Update Lid Status and notify Lid and Gfx device when ISH Notifies for Lid change
    //
    Method(UNLS,0,Serialized)
    {

    } // End of ULDS
  }
} // End of Scope(\_SB)
