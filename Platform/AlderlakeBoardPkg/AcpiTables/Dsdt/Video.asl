/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(DIDX)
External(DIDY)
External(\_SB.PC00.GFX0.DD2F, DeviceObj)

// Brightness Notification:
//    Generate a brightness related notification
//      to the LFP if its populated.
//
//  Arguments:
//    Arg0: Notification value.
//
//  Return Value:
//    None
Method(BRTN,1,Serialized)
{
    If(LEqual(And(DIDX,0x0F0F),0x400))
    {
        Notify(\_SB.PC00.GFX0.DD1F,Arg0)
    }
    If(LEqual(And(DIDY,0x0F0F),0x401))
    {
        Notify(\_SB.PC00.GFX0.DD2F,Arg0)
    }
}
