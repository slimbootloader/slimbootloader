/**@file
 Trace Hub debug library for ASL code.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PchReservedResources.h>
External(PTHM, FieldUnitObj)
External(\_SB.PC00.PTHT.THEN, MethodObj, IntObj)
External(\_SB.PTHH.THEN, MethodObj, IntObj)

Name (THMN, 0x48)  // Host number should align with gSiPkgTokenSpaceGuid.PcdTraceHubDebugLibMaster
Name (THCN, 0x0E)  // Channel number could be different from gSiPkgTokenSpaceGuid.PcdTraceHubDebugLibChannel, 0~80h are available
Name (CNPM, 0x80)  // Channel number per Host
Name (FMSN, 0x10)  // Firmware Host start number

Scope(\) {
  // Trace Hub debug address
  // This is internal helper runtine of THDS
  // Arg0 : Host number
  // Arg1 : Channel number
  Method (THDA, 2, Serialized) {
    // Local0 = PCH_TRACE_HUB_FW_BASE_ADDRESS + 0x40 * (V_TRACE_HUB_MEM_MTB_CHLCNT * (Master - V_TRACE_HUB_MEM_MTB_FTHMSTR)) + 0x40 * Channel;
    Store (PCH_TRACE_HUB_FW_BASE_ADDRESS, Local0)
    Add (Local0, Multiply (0x40, Multiply (CNPM, Subtract (Arg0, FMSN))), Local0)
    Add (Local0, Multiply (0x40, Arg1), Local0)
    Return (Local0)
  }

  // String to raw data
  // This is internal helper runtine of THDS
  // Arg0 : string
  // Arg1 : Index
  // Arg2 : size
  Method (STRD, 3, Serialized) {
    If (LGreater (Add (Arg1, Arg2), SizeOf (Arg0))) {
      Return (0)
    }
    // Local0 is return value
    // Lccal1 is loop index
    // Local2 is char of string
    // Local3 is buffer of string
    ToBuffer (Arg0, Local3)
    Store (0, Local0)
    Store (0, Local1)
    While (LLess (Local1, Arg2)) {
      Store (DeRefOf (Index (Local3, Add (Arg1, Local1))), Local2)
      Add (Local0, ShiftLeft (Local2, Multiply (8, Local1)), Local0)
      Increment (Local1)
    }
    Return (Local0)
  }

  // Trace Hub debug string
  // Arg0 : debug string
  Method (THDS, 1, Serialized) {
   //
   // Check if BIOS trace is enabled.
   //
   If (LEqual (PTHM, 0)) {
     Return
   } ElseIf (LEqual (PTHM, 1)) {
     If (LNot (\_SB.PC00.PTHT.THEN())) {
       Return
     }
   } ElseIf (LEqual (PTHM, 2)) {
     If (LNot (\_SB.PTHH.THEN())) {
       Return
     }
   } Else {
     Return
   }
    // Adding newline at the end of the string
    Concatenate (Arg0, "\n", local2)
    // Local0 is the length of string
    // Local1 is the debug base address
    // Local2 is the debug string with "\n"
    Store (Sizeof (local2), Local0)
    Store (THDA (THMN, THCN), Local1)
    OperationRegion (THBA, SystemMemory, local1, 0x40)
    Field (THBA, QWordAcc, NoLock, Preserve) {
      Offset (0x00),
      QO00, 64,
    }
    Field (THBA, DWordAcc, NoLock, Preserve) {
      Offset (0x00),
      DO00, 32,
      Offset (0x10),
      DO10, 32,
      offset (0x30),
      DO30, 32,
    }
    Field (THBA, WordAcc, NoLock, Preserve) {
      Offset (0x00),
      WO00, 16,
    }
    Field (THBA, ByteAcc, NoLock, Preserve) {
      Offset (0x00),
      BO00, 8,
    }

    // time stamp
    Store (0x01000242, DO10)
    // length of string
    Store (Local0, WO00)
    // string
    Store (0, Local6)
    Store (Local0, Local7)
    while (LGreaterEqual(Local7, 8)) {
      Store (STRD (local2, Local6, 8), QO00)
      Add (Local6, 8, Local6)
      Subtract (Local7, 8, Local7)
    }
    If (LGreaterEqual(Local7, 4)) {
      Store (STRD (local2, Local6, 4), DO00)
      Add (Local6, 4, Local6)
      Subtract (Local7, 4, Local7)
    }
    If (LGreaterEqual(Local7, 2)) {
      Store (STRD (local2, Local6, 2), WO00)
      Add (Local6, 2, Local6)
      Subtract (Local7, 2, Local7)
    }
    If (LGreaterEqual(Local7, 1)) {
      Store (STRD (local2, Local6, 1), BO00)
      Add (Local6, 1, Local6)
      Subtract (Local7, 1, Local7)
    }
    // flag
    Store (0, DO30)
  }

  // Trace Hub debug Hex string
  // Arg0 : Integer, buffer
  Method (THDH, 1, Serialized) {
    THDS (ToHexString (Arg0))
  }

  // Trace Hub debug decimal string
  // Arg0 : Integer, buffer
  Method (THDD, 1, Serialized) {
    THDS (ToDecimalString (Arg0))
  }
}
