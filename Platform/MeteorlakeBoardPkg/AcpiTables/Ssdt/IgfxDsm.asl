/** @file
  IGD OpRegion/_DSM Reference Code.
  This file contains Get BIOS Data and Callback functions for
  the Integrated Graphics Device (IGD) OpRegion/DSM mechanism

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(\_SB.GGOV, MethodObj)
External(\_SB.SGOV, MethodObj)
External(EDMX)
External(\_SB.PC00.LPCB.H_EC.LID0, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.LSTE)
External(\_SB.PC00.LPCB.H_EC.ECWT, MethodObj)
External(\_SB.PC00.LPCB.H_EC.ECRD, MethodObj)
//
// _DSM Device Specific Method
//
// Arg0: UUID Unique function identifier
// Arg1: Integer Revision Level
// Arg2: Integer Function Index (1 = Return Supported Functions)
// Arg3: Additional Inputs/Package Parameters Bits [31:0] input as {Byte0, Byte1, Byte2, Byte3} to BIOS which is passed as 32 bit DWORD by Driver
//
Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {

  If (LEqual(Arg0, ToUUID ("3E5B41C6-EB1D-4260-9D15-C71FBADAE414"))) {
    //
    // _DSM Definition for Igd functions
    // Arguments:
    // Arg0: UUID: 3E5B41C6-EB1D-4260-9D15-C71FBADAE414
    // Arg1: Revision ID: 1
    // Arg2: Function Index: 16
    // Arg3: Additional Inputs Bits[31:0] Arg3 {Byte0, Byte1, Byte2, Byte3}
    //
    // Return:
    // Success for simple notification, Opregion update for some routines and a Package for AKSV
    //

    //
    // Switch by function index
    //
    Switch(ToInteger(Arg2)) {
      //
      // Function Index: 0
      // Standard query - A bitmask of functions supported
      //
      // Return: A bitmask of functions supported
      //
      Case (0)
      {
        If (LEqual(Arg1, 1)) { // test Arg1 for Revision ID: 1
          Store("iGfx Supported Functions Bitmap ", Debug)

          Store(0x1, Local0)
          If (LNotEqual(EDMX, 0)) {
            Or (Local0, 0x200000, Local0)
          }
          // Set bit 22 to indicate Lid Switch simulation support
          Or (Local0, 0x400000, Local0)
          Return(Local0)
        }
      }

      //
      // Function Index: 21
      // Get and Set Display Mux GPIO control for iGfx
      // Arg3 : 0 to get mux status
      //        1 to set mux control to iGfx
      //        2 to set mux control to dGfx
      // Return: Package {Status, 0}
      //         Status : 0 Success
      //                : 1 fail
      Case(21) {
        //
        // DGDB Data Buffer is data for return DSM fun#21
        // with below buffer format
        // Byte[0-3] is Data field.
        // Byte[4] is Status field.
        //
        Name(DGDB, Buffer(5){0,0,0,0,0})
        CreateDwordField(DGDB, 0x00, DGDF) // Current Display Mux Setting Data Field
        CreateByteField(DGDB, 0x04, DGSF)  // Status Field
                                           //    Success - 0
                                           //    Fail    - 1

        Store(1, Local0)
        If (LEqual (And(DerefOf (Index (Arg3,0)), 0x1), 0))
        {
          \_SB.GGOV(EDMX)
          Store(0, Local0)
        }
        Else
        {
          If (LEqual (And(DerefOf (Index (Arg3,0)), 0x2), 0))
          {
            \_SB.SGOV(EDMX, 0)
            Store(0, Local0)
          }
          Else
          {
            \_SB.SGOV(EDMX, 1)
            Store(0, Local0)
          }
        }
        Store(ShiftLeft(\_SB.GGOV(EDMX),1), DGDF)
        Store(Local0, DGSF)
        Return(DGDB)
      }

      //
      // Function Index: 22
      // Simulate Lid switch On/Off
      // Arg3 : 0 to close Lid
      //        1 to open Lid
      // Return: Package {Status, 0}
      //         Status : 0 Success
      //                : 1 fail
      //
      Case(22) {
        Switch (ToInteger(Arg3)) {
          Case(0)
            {
              If (CondRefOf(\_SB.PC00.LPCB.H_EC.LSTE)) {
                \_SB.PC00.LPCB.H_EC.ECWT(0, RefOf(\_SB.PC00.LPCB.H_EC.LSTE))
                Store(0x0, LIDS)
                Store(0, \_SB.PC00.GFX0.CLID)
                Notify(\_SB.PC00.LPCB.H_EC.LID0, 0x80)
                Return(0)
              }
            }

          Case(1)
            {
              If (CondRefOf(\_SB.PC00.LPCB.H_EC.LSTE)) {
                \_SB.PC00.LPCB.H_EC.ECWT(1, RefOf(\_SB.PC00.LPCB.H_EC.LSTE))
                Store(0x1, LIDS)
                Store(0x03, \_SB.PC00.GFX0.CLID)
                Notify(\_SB.PC00.LPCB.H_EC.LID0, 0x80)
                Return(0)
              }
            }

          Default
          {
            Return(1)
          }
        }
      }
    } // End of switch(Arg2)
  } // End of if (ToUUID("3E5B41C6-EB1D-4260-9D15-C71FBADAE414D"))

  Return (Buffer () {0x00})
} // End of _DSM
