/** @file
  ACPI SSDT Library for eDP features on DG device.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(\_SB.GGOV, MethodObj)
External(\_SB.SGOV, MethodObj)
External(EDMX)
External(DBRL)
External(DNID)
External(DGD1)
External(DGD2)
External(DGD3)
External(DGD4)
External(DGD5)
External(DGD6)
External(DGD7)
External(DGD8)
External(DGD9)
External(DGDA)
External(DGDB)
External(DGDC)
External(DGDD)
External(DGDE)
External(DGDF)
External(DGDX)
External(DGDS)
External(\_SB.PC00.RP12.DIDL)
External(\_SB.PC00.RP12.DDL2)
External(\_SB.PC00.RP12.DDL3)
External(\_SB.PC00.RP12.DDL4)
External(\_SB.PC00.RP12.DDL5)
External(\_SB.PC00.RP12.DDL6)
External(\_SB.PC00.RP12.DDL7)
External(\_SB.PC00.RP12.DDL8)
External(\_SB.PC00.RP12.DDL9)
External(\_SB.PC00.RP12.DD10)
External(\_SB.PC00.RP12.DD11)
External(\_SB.PC00.RP12.DD12)
External(\_SB.PC00.RP12.DD13)
External(\_SB.PC00.RP12.DD14)
External(\_SB.PC00.RP12.DD15)
External(BCLP)
External(ASLC)
External(\_SB.PC00.LPCB.H_EC.LID0, DeviceObj)
External(\_SB.PC00.LPCB.H_EC.LSTE)
External(\_SB.PC00.LPCB.H_EC.ECWT, MethodObj)
External(\_SB.PC00.LPCB.H_EC.ECRD, MethodObj)
External (\LIDS, FieldUnitObj)
External(\_SB.PC00.RP12.CLID)

//
// DG device has two internal bridges within.
// PEGP is the bridge which is directly connected to RP.
// PEGB is the bridge connected with the first bridge.
// PEGE is the endpoint connected to second bridge.
// DEDP is the eDP device on DG endpoint.
//

  Device (PEGB) {
    Name(_ADR, 0x10000)
    //
    // Declare a device scope for DG endpoint.
    //
    Device (PEGE) {
      Name(_ADR, 0x00000000)
      //
      // _DSM Device Specific Method
      //
      // Arg0: UUID Unique function identifier
      // Arg1: Integer Revision Level
      // Arg2: Integer Function Index (0 = Return Supported Functions)
      // Arg3: Package Parameters
      Method(_DSM, 4, Serialized) {
        //
        // Switch based on which unique function identifier was passed in
        //
        If (LEqual(Arg0, ToUUID ("EDBF573D-E352-4CE7-BF23-37CE9F8402B8"))) {
          //
          // _DSM Definition for DG functions
          // Arguments:
          // Arg0: UUID: EDBF573D-E352-4CE7-BF23-37CE9F8402B8
          // Arg1: Revision ID: 1
          // Arg2: Function Index: 21
          // Arg3: Input argument : 0/1/2
          //
          // Return:
          // Success or Failed as Status, Current Mux setting
          //

          //
          // Switch by function index
          //
          Switch(ToInteger(Arg2)) {
            //
            // Function Index: 0
            // Get bitmask of supported functions
            //
            // Return: A bitmask of functions supported
            //
            Case (0)
              {
                If (LEqual(Arg1, 1)) { // test Arg1 for Revision ID: 1
                  Store("DG Supported Functions Bitmap ", Debug)
                  ADBG("DG _DSM Fun0")
                  Store(0x1, Local0)
                  If (LNotEqual(EDMX, 0)) {
                    ADBG ("Display Mux Supported")
                    Or (Local0, 0x200000, Local0)
                  }
                  // Set bit 22 to indicate Lid Switch simulation support
                  Or (Local0, 0x400000, Local0)
                  ADBG (Concatenate ("IGFX DSM Function Support = ", ToHexString (Local0)))
                  Return(Local0)
                }
              }

            //
            // Function Index: 21
            // Get and Set Display Mux GPIO control for dGfx
            // Arg3 : 0 to get mux status
            //        1 to set mux control to iGfx
            //        2 to set mux control to dGfx
            // Return: Package {Status, 0}
            //         Status : 0 Success
            //                : 1 fail
            //
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
              ADBG("DG eDP _DSM (21)")
              If (LEqual (And(DerefOf (Index (Arg3,0)), 0x1), 0))
              {
                ADBG("Get Current Disp Mux Status")
                \_SB.GGOV(EDMX)
                Store(0, Local0)
              }
              Else
              {
                ADBG("Set Disp Mux Status")
                If (LEqual (And(DerefOf (Index (Arg3,0)), 0x2), 0))
                {
                  ADBG("Set Disp Mux to iGfx")
                  \_SB.SGOV(EDMX, 0)
                  Store(0, Local0)
                }
                Else
                {
                  ADBG("Set Disp Mux to dGfx")
                  \_SB.SGOV(EDMX, 1)
                  Store(0, Local0)
                }
              }
              Store(ShiftLeft(\_SB.GGOV(EDMX),1), DGDF)
              Store(Local0, DGSF)
              ADBG(Concatenate("ExitResult :: ", ToHexString(DGSF)))
              ADBG(Concatenate("Current Mux :: ", ToHexString(DGDF)))
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
                If (CondRefOf(\_SB.PC00.LPCB.H_EC.LSTE)){
                  \_SB.PC00.LPCB.H_EC.ECWT(0, RefOf(\_SB.PC00.LPCB.H_EC.LSTE))
                  Store(0x0, LIDS)
                  Store(0, \_SB.PC00.RP12.CLID)
                  ADBG("Notify LID0")
                  Notify(\_SB.PC00.LPCB.H_EC.LID0, 0x80)
                  ADBG ("OS Notified")
                  Return(0)
                } Else {
                  ADBG ("Lid close failed")
                }
              }

              Case(1)
              {
                If (CondRefOf(\_SB.PC00.LPCB.H_EC.LSTE)) {
                  \_SB.PC00.LPCB.H_EC.ECWT(1, RefOf(\_SB.PC00.LPCB.H_EC.LSTE))
                  Store(0x1, LIDS)
                  Store(0x03, \_SB.PC00.RP12.CLID)
                  ADBG("Notify LID0")
                  Notify(\_SB.PC00.LPCB.H_EC.LID0, 0x80)
                  ADBG ("OS Notified")
                  Return(0)
                } Else {
                  ADBG ("Lid open failed")
                }
              }

              Default
              {
                Return(1)
              }
            }
          }
        } // End of switch(Arg2)
      } // End of if (ToUUID("EDBF573D-E352-4CE7-BF23-37CE9F8402B8"))
      Return (Buffer () {0x00})
    } // End of _DSM


      // Enable/Disable Output Switching.  In WIN2K/WINXP, _DOS = 0 will
      // get called during initialization to prepare for an ACPI Display
      // Switch Event.  During an ACPI Display Switch, the OS will call
      // _DOS = 2 immediately after a Notify=0x80 to temporarily disable
      // all Display Switching.  After ACPI Display Switching is complete,
      // the OS will call _DOS = 0 to re-enable ACPI Display Switching.
      Method(_DOS,1)
      {
        //
        // Store Display Switching and LCD brightness BIOS control bit
        //
        Store(And(Arg0,7),DGDS)
      }
      //
      // Enumerate the Display Environment.  This method will return
      // valid addresses for all display device encoders present in the
      // system.  The Miniport Driver will reject the addresses for every
      // encoder that does not have an attached display device.  After
      // enumeration is complete, the OS will call the _DGS methods
      // during a display switch only for the addresses accepted by the
      // Miniport Driver.  For hot-insertion and removal of display
      // devices, a re-enumeration notification will be required so the
      // address of the newly present display device will be accepted by
      // the Miniport Driver.
      //
      Method(_DOD,0)
      {
        //
        // One LFP device is supported by default
        //
        Store(1, DNID)
        If(LNotEqual(DIDL, Zero))
        {
          Store(DGDL(DIDL),DGD1)
        }
        If(LNotEqual(DDL2, Zero))
        {
          Store(DGDL(DDL2),DGD2)
        }
        If(LNotEqual(DDL3, Zero))
        {
          Store(DGDL(DDL3),DGD3)
        }
        If(LNotEqual(DDL4, Zero))
        {
          Store(DGDL(DDL4),DGD4)
        }
        If(LNotEqual(DDL5, Zero))
        {
          Store(DGDL(DDL5),DGD5)
        }
        If(LNotEqual(DDL6, Zero))
        {
          Store(DGDL(DDL6),DGD6)
        }
        If(LNotEqual(DDL7, Zero))
        {
          Store(DGDL(DDL7),DGD7)
        }
        If(LNotEqual(DDL8, Zero))
        {
          Store(DGDL(DDL8),DGD8)
        }
        If(LNotEqual(DDL9, Zero))
        {
          Store(DGDL(DDL9),DGD9)
        }
        If(LNotEqual(DD10, Zero))
        {
          Store(DGDL(DD10),DGDA)
        }
        If(LNotEqual(DD11, Zero))
        {
          Store(DGDL(DD11),DGDB)
        }
        If(LNotEqual(DD12, Zero))
        {
          Store(DGDL(DD12),DGDC)
        }
        If(LNotEqual(DD13, Zero))
        {
          Store(DGDL(DD13),DGDD)
        }
        If(LNotEqual(DD14, Zero))
        {
          Store(DGDL(DD14),DGDE)
        }
        If(LNotEqual(DD15, Zero))
        {
          Store(DGDL(DD15),DGDF)
        }

        //
        // Enumerate the encoders. Note that for
        // current silicon, the maximum number of encoders
        // possible is 15.
        //
        If(LEqual(DNID,1))
        {
          Name(TMP1,Package() {
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP1,0))
          Return(TMP1)
        }

        If(LEqual(DNID,2))
        {
          Name(TMP2,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP2,0))
          Store(Or(0x10000,DGD2),Index(TMP2,1))
          Return(TMP2)
        }

        If(LEqual(DNID,3))
        {
          Name(TMP3,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP3,0))
          Store(Or(0x10000,DGD2),Index(TMP3,1))
          Store(Or(0x10000,DGD3),Index(TMP3,2))
          Return(TMP3)
        }

        If(LEqual(DNID,4))
        {
          Name(TMP4,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP4,0))
          Store(Or(0x10000,DGD2),Index(TMP4,1))
          Store(Or(0x10000,DGD3),Index(TMP4,2))
          Store(Or(0x10000,DGD4),Index(TMP4,3))
          Return(TMP4)
        }

        If(LEqual(DNID,5))
        {
          Name(TMP5,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP5,0))
          Store(Or(0x10000,DGD2),Index(TMP5,1))
          Store(Or(0x10000,DGD3),Index(TMP5,2))
          Store(Or(0x10000,DGD4),Index(TMP5,3))
          Store(Or(0x10000,DGD5),Index(TMP5,4))
          Return(TMP5)
        }

        If(LEqual(DNID,6))
        {
          Name(TMP6,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP6,0))
          Store(Or(0x10000,DGD2),Index(TMP6,1))
          Store(Or(0x10000,DGD3),Index(TMP6,2))
          Store(Or(0x10000,DGD4),Index(TMP6,3))
          Store(Or(0x10000,DGD5),Index(TMP6,4))
          Store(Or(0x10000,DGD6),Index(TMP6,5))
          Return(TMP6)
        }

        If(LEqual(DNID,7))
        {
          Name(TMP7,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP7,0))
          Store(Or(0x10000,DGD2),Index(TMP7,1))
          Store(Or(0x10000,DGD3),Index(TMP7,2))
          Store(Or(0x10000,DGD4),Index(TMP7,3))
          Store(Or(0x10000,DGD5),Index(TMP7,4))
          Store(Or(0x10000,DGD6),Index(TMP7,5))
          Store(Or(0x10000,DGD7),Index(TMP7,6))
          Return(TMP7)
        }

        If(LEqual(DNID,8))
        {
          Name(TMP8,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP8,0))
          Store(Or(0x10000,DGD2),Index(TMP8,1))
          Store(Or(0x10000,DGD3),Index(TMP8,2))
          Store(Or(0x10000,DGD4),Index(TMP8,3))
          Store(Or(0x10000,DGD5),Index(TMP8,4))
          Store(Or(0x10000,DGD6),Index(TMP8,5))
          Store(Or(0x10000,DGD7),Index(TMP8,6))
          Store(Or(0x10000,DGD8),Index(TMP8,7))
          Return(TMP8)
        }

        If(LEqual(DNID,9))
        {
          Name(TMP9,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMP9,0))
          Store(Or(0x10000,DGD2),Index(TMP9,1))
          Store(Or(0x10000,DGD3),Index(TMP9,2))
          Store(Or(0x10000,DGD4),Index(TMP9,3))
          Store(Or(0x10000,DGD5),Index(TMP9,4))
          Store(Or(0x10000,DGD6),Index(TMP9,5))
          Store(Or(0x10000,DGD7),Index(TMP9,6))
          Store(Or(0x10000,DGD8),Index(TMP9,7))
          Store(Or(0x10000,DGD9),Index(TMP9,8))
          Return(TMP9)
        }

        If(LEqual(DNID,0x0A))
        {
          Name(TMPA,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPA,0))
          Store(Or(0x10000,DGD2),Index(TMPA,1))
          Store(Or(0x10000,DGD3),Index(TMPA,2))
          Store(Or(0x10000,DGD4),Index(TMPA,3))
          Store(Or(0x10000,DGD5),Index(TMPA,4))
          Store(Or(0x10000,DGD6),Index(TMPA,5))
          Store(Or(0x10000,DGD7),Index(TMPA,6))
          Store(Or(0x10000,DGD8),Index(TMPA,7))
          Store(Or(0x10000,DGD9),Index(TMPA,8))
          Store(Or(0x10000,DGDA),Index(TMPA,9))
          Return(TMPA)
        }

        If(LEqual(DNID,0x0B))
        {
          Name(TMPB,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPB,0))
          Store(Or(0x10000,DGD2),Index(TMPB,1))
          Store(Or(0x10000,DGD3),Index(TMPB,2))
          Store(Or(0x10000,DGD4),Index(TMPB,3))
          Store(Or(0x10000,DGD5),Index(TMPB,4))
          Store(Or(0x10000,DGD6),Index(TMPB,5))
          Store(Or(0x10000,DGD7),Index(TMPB,6))
          Store(Or(0x10000,DGD8),Index(TMPB,7))
          Store(Or(0x10000,DGD9),Index(TMPB,8))
          Store(Or(0x10000,DGDA),Index(TMPB,9))
          Store(Or(0x10000,DGDB),Index(TMPB,10))
          Return(TMPB)
        }

        If(LEqual(DNID,0x0C))
        {
          Name(TMPC,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPC,0))
          Store(Or(0x10000,DGD2),Index(TMPC,1))
          Store(Or(0x10000,DGD3),Index(TMPC,2))
          Store(Or(0x10000,DGD4),Index(TMPC,3))
          Store(Or(0x10000,DGD5),Index(TMPC,4))
          Store(Or(0x10000,DGD6),Index(TMPC,5))
          Store(Or(0x10000,DGD7),Index(TMPC,6))
          Store(Or(0x10000,DGD8),Index(TMPC,7))
          Store(Or(0x10000,DGD9),Index(TMPC,8))
          Store(Or(0x10000,DGDA),Index(TMPC,9))
          Store(Or(0x10000,DGDB),Index(TMPC,10))
          Store(Or(0x10000,DGDC),Index(TMPC,11))
          Return(TMPC)
        }

        If(LEqual(DNID,0x0D))
        {
          Name(TMPD,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPD,0))
          Store(Or(0x10000,DGD2),Index(TMPD,1))
          Store(Or(0x10000,DGD3),Index(TMPD,2))
          Store(Or(0x10000,DGD4),Index(TMPD,3))
          Store(Or(0x10000,DGD5),Index(TMPD,4))
          Store(Or(0x10000,DGD6),Index(TMPD,5))
          Store(Or(0x10000,DGD7),Index(TMPD,6))
          Store(Or(0x10000,DGD8),Index(TMPD,7))
          Store(Or(0x10000,DGD9),Index(TMPD,8))
          Store(Or(0x10000,DGDA),Index(TMPD,9))
          Store(Or(0x10000,DGDB),Index(TMPD,10))
          Store(Or(0x10000,DGDC),Index(TMPD,11))
          Store(Or(0x10000,DGDD),Index(TMPD,12))
          Return(TMPD)
        }

        If(LEqual(DNID,0x0E))
        {
          Name(TMPE,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPE,0))
          Store(Or(0x10000,DGD2),Index(TMPE,1))
          Store(Or(0x10000,DGD3),Index(TMPE,2))
          Store(Or(0x10000,DGD4),Index(TMPE,3))
          Store(Or(0x10000,DGD5),Index(TMPE,4))
          Store(Or(0x10000,DGD6),Index(TMPE,5))
          Store(Or(0x10000,DGD7),Index(TMPE,6))
          Store(Or(0x10000,DGD8),Index(TMPE,7))
          Store(Or(0x10000,DGD9),Index(TMPE,8))
          Store(Or(0x10000,DGDA),Index(TMPE,9))
          Store(Or(0x10000,DGDB),Index(TMPE,10))
          Store(Or(0x10000,DGDC),Index(TMPE,11))
          Store(Or(0x10000,DGDD),Index(TMPE,12))
          Store(Or(0x10000,DGDE),Index(TMPE,13))
          Return(TMPE)
        }

        If(LEqual(DNID,0x0F))
        {
          Name(TMPF,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPF,0))
          Store(Or(0x10000,DGD2),Index(TMPF,1))
          Store(Or(0x10000,DGD3),Index(TMPF,2))
          Store(Or(0x10000,DGD4),Index(TMPF,3))
          Store(Or(0x10000,DGD5),Index(TMPF,4))
          Store(Or(0x10000,DGD6),Index(TMPF,5))
          Store(Or(0x10000,DGD7),Index(TMPF,6))
          Store(Or(0x10000,DGD8),Index(TMPF,7))
          Store(Or(0x10000,DGD9),Index(TMPF,8))
          Store(Or(0x10000,DGDA),Index(TMPF,9))
          Store(Or(0x10000,DGDB),Index(TMPF,10))
          Store(Or(0x10000,DGDC),Index(TMPF,11))
          Store(Or(0x10000,DGDD),Index(TMPF,12))
          Store(Or(0x10000,DGDE),Index(TMPF,13))
          Store(Or(0x10000,DGDF),Index(TMPF,14))
          Return(TMPF)
        }

        If(LEqual(DNID,0x10))
        {
          Name(TMPG,Package() {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF})
          Store(Or(0x10000,DGD1),Index(TMPG,0))
          Store(Or(0x10000,DGD2),Index(TMPG,1))
          Store(Or(0x10000,DGD3),Index(TMPG,2))
          Store(Or(0x10000,DGD4),Index(TMPG,3))
          Store(Or(0x10000,DGD5),Index(TMPG,4))
          Store(Or(0x10000,DGD6),Index(TMPG,5))
          Store(Or(0x10000,DGD7),Index(TMPG,6))
          Store(Or(0x10000,DGD8),Index(TMPG,7))
          Store(Or(0x10000,DGD9),Index(TMPG,8))
          Store(Or(0x10000,DGDA),Index(TMPG,9))
          Store(Or(0x10000,DGDB),Index(TMPG,10))
          Store(Or(0x10000,DGDC),Index(TMPG,11))
          Store(Or(0x10000,DGDD),Index(TMPG,12))
          Store(Or(0x10000,DGDE),Index(TMPG,13))
          Store(Or(0x10000,DGDF),Index(TMPG,14))
          Return(TMPG)
        }

        //
        // If nothing else, return Unknown LFP.
        // (Prevents compiler warning.)
        //
        Return(Package() {0x00000400})
      } //end of _DOD

      //
      //Device for eDP
      //
      Device(DEDP)
      {
        Method(_ADR,0,Serialized)
        {
          If(LEqual(And(0x0F00,DGD1),0x400))
          {
            Store(DGD1, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD2),0x400))
          {
            Store(DGD2, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD3),0x400))
          {
            Store(DGD3, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD4),0x400))
          {
            Store(DGD4, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD5),0x400))
          {
            Store(DGD5, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD6),0x400))
          {
            Store(DGD6, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD7),0x400))
          {
            Store(DGD7, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD8),0x400))
          {
            Store(DGD8, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGD9),0x400))
          {
            Store(DGD9, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGDA),0x400))
          {
            Store(DGDA, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGDB),0x400))
          {
            Store(DGDB, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGDC),0x400))
          {
            Store(DGDC, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGDD),0x400))
          {
            Store(DGDD, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGDE),0x400))
          {
            Store(DGDE, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          ElseIf(LEqual(And(0x0F00,DGDF),0x400))
          {
            Store(DGDF, DGDX)
            Return(And(0xFFFF,DGDX))
          }
          Else
          {
            ADBG("DG: eDP is not present")
            Return(0x1F)
          }
        }

        //
        // Query List of Brightness Control Levels Supported.
        //
        Method(_BCL,0)
        {
          //
          // List of supported brightness levels in the following sequence.
          // Level when machine has full power.
          // Level when machine is on batteries.
          // Other supported levels.
          //
          Return(Package(){80, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100})
        }

        //
        // Set the Brightness Level.
        //
        Method (_BCM,1)
        {
          //
          // Set the requested level if it is between 0 and 100%.
          //
          If(LAnd(LGreaterEqual(Arg0,0),LLessEqual(Arg0,100)))
          {
            Store(Divide(Multiply(Arg0, 255), 100), BCLP) // Convert from percent to 0-255.
            Or(BCLP, 0x80000000, BCLP) // Set the valid bit.
            Store(2, ASLC) // Store "Backlight control event" to ASLC[31:1]
            Store(Arg0,DBRL)  // Store Brightness Level.
          }
        }

        //
        // Brightness Query Current level.
        //
        Method (_BQC,0)
        {
          Return(DBRL)
        }
      }

      Method(DGDL,1)
      {
        Increment(DNID)
        Store(And(Arg0,0xF0F),Local0)
        Or(0x80000000,Local0, Local1)
        If(LEqual(DIDL,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL2,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL3,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL4,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL5,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL6,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL7,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL8,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DDL9,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DD10,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DD11,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DD12,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DD13,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DD14,Local0))
        {
          Return(Local1)
        }
        If(LEqual(DD15,Local0))
        {
          Return(Local1)
        }
        Return(0)
      }
    }
  }