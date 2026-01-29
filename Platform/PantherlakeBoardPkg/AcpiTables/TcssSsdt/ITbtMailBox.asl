/** @file
 Integrated Thunderbolt MailBox ACPI methods

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include <TbtMailBoxCmdDefinition.h>

Field (DPME, DWordAcc, NoLock, Preserve)
{
  Offset (R_USB4_CFG_TBT_DMA_CFG_VS_CAP_9), // 0xC8
  ,     31,
  INFR,  1, // ITBT NVM FW Ready
  Offset (0xEC), // 0xEC
  TB2P, 32, // TBT to PCIe
  P2TB, 32  // PCIe to TBT
}

//
// ITBT MailBox Command Method
// Arg0 : MailBox Cmd ID
//
Method (ITMB, 1, Serialized) {
  ADBG (ToHexString (_DDN))
  ADBG (Concatenate ("MailBox Cmd ", ToHexString (ShiftRight (Arg0, 1))))
  Or (Arg0, PCIE2TBT_VLD_B, Local0)
  Store (Local0, P2TB)
  ADBG ("Command is sent")
}

//
// Wait For Command Completed
// Arg0    TimeOut value (unit is 1 millisecond)
//
Method (WFCC, 1, Serialized) {
  WTBS (Arg0)
  Store (0, P2TB)
  WTBC (Arg0)
}

//
// Wait For Command Set
// Arg0    TimeOut value
// Return : 0 - Timeout, non-zero - Command done bit is set within timeout value
//
Method (WTBS, 1, Serialized) {
  Store (Arg0, Local0)
  ADBG (Concatenate ("Wait TB2P to be set, DUID ", ToHexString (DUID)))
  While (LGreater (Local0, 0)) {
    //
    // Wait for Bit to Set
    //
    If (And (TB2P, TBT2PCIE_DON_R)) {
      ADBG (Concatenate ("TB2P is set, DUID ", ToHexString (DUID)))
      ADBG (Concatenate ("Wait count = ", ToHexString (Subtract (Arg0, Local0))))
      Break
    }
    Decrement (Local0)
    Sleep (1)
  }

  If (LEqual (Local0, 0)) {
    ADBG (Concatenate ("Command Timeout, DUID ", ToHexString (DUID)))
    ADBG (Concatenate ("Timeout Value ", ToHexString (Arg0)))
  }

  Return (Local0)
}

//
// Wait For Command Clear
// Arg0    TimeOut value
//
Method (WTBC, 1, Serialized) {
  Store (Arg0, Local0)
  ADBG (Concatenate ("Wait TB2P to be clear, DUID ", ToHexString (DUID)))
  While (LGreater (Local0, 0)) {
    //
    // Wait for Bit to Clear
    //
    If (LNotEqual (And (TB2P, TBT2PCIE_DON_R), TBT2PCIE_DON_R)) {
      ADBG (Concatenate ("TB2P is clear, DUID ", ToHexString (DUID)))
      Break
    }
    Decrement (Local0)
    Sleep (1)
  }

  If (LEqual (Local0, 0)) {
    ADBG (Concatenate ("Command Timeout, DUID ", ToHexString (DUID)))
    ADBG (Concatenate ("Timeout Value ", ToHexString (Arg0)))
  }
}

//
// TCSS ITBT CONNECT_TOPOLOGY MailBox Command Method
//
Method (CNTP, 0, Serialized) {
  ADBG (Concatenate ("TCSS Send ITBT CONNECT TOPOLOGY Cmd Start, DUID -", ToHexString (DUID)))
  Store (0, Local0)
  //
  // Set Force Power if it is not set.
  //
  If (LEqual (DFPE, 0)) {
    Store (0x22, DMAD)
    Store (1, DFPE)
    //
    // Poll the ITBT NVM FW Ready bit with timeout(default is 500ms) before send the TBT MailBox command
    //
    While (LAnd (LEqual (INFR, 0), LLess (Local0, ITFP))) {
      Sleep (1)
      Increment (Local0)
    }
  }
  If (LNotEqual (Local0, ITFP)) {
    ITMB (PCIE2TBT_CONNECT_TOPOLOGY_COMMAND)
  }
  Else {
    ADBG ("Force Power Timeout occurred")
  }
  //
  // Unset Force Power.
  //
  If (LNotEqual (DFPE, 0)) {
    ADBG ("Unset Force Power")
    Store (0, DFPE)
  }
  ADBG (Concatenate ("TCSS Send ITBT CONNECT TOPOLOGY Cmd End, DUID -", ToHexString (DUID)))
}

//
// Name: CMMB
// Description: Send set CM mode mailbox command
// Input: Arg0 - CM mode value (0: FW CM, 1: SW CM)
// Return: 0 = Success, 0xFF = Failure
// Local0: Wait for command done status. 0 = Timeout, non-zero = Command done.
// Local1: Set CM mode mail box command status.
//
Method (CMMB, 1, Serialized)
{
  //
  // From TBT FW's view, two modes are provided for CM implementation.
  // - Firmware CM mode  : TBT FW implements CM functions.
  // - Pass Through mode : TBT FW provides USB4 host interface for CM implementation.
  // To implement SW CM, TBT FW need to be switched to Pass Through mode.
  //
  If (LEqual (Arg0, 0))
  {
    ADBG ("Set FW CM mode")
    //
    // Set FW CM mode and connect topology
    //
    ITMB (PCIE2TBT_FIRMWARE_CM_MODE)
  }
  ElseIf (LEqual (Arg0, 1))
  {
    ADBG ("Set SW CM mode")
    //
    // Set Pass Through mode for SW CM implementation
    //
    ITMB (PCIE2TBT_PASS_THROUGH_MODE)
  }

  //
  // Wait for set CM mode command done.
  //
  Store (WTBS (SET_CM_TIMEOUT_IN_MS), Local0)
  If (LNotEqual (Local0, 0))
  {
    ADBG ("Set CM mode done bit is set")
    Store (0, Local1)
  }
  Else {
    ADBG ("Set CM mode command timeout")
    Store (0xFF, Local1)
  }
  Store (0, P2TB)

  Return (Local1)
}

//
// Name: CHCM
// Description: Change CM mode
// Input: Arg0 - CM mode value (0: FW CM, 1: SW CM)
// Return: 0 = Success. 0xFF = Failure
// Local0: Index for polling set force power
// Local1: Change CM mode status
// Local2: Wait for command done status. 0 = Timeout, non-zero = Command done.
//
Method (CHCM, 1, Serialized)
{
  ADBG (Concatenate ("iTBT CHCM - Switch CM mode to ", Arg0))
  If (LGreater (Arg0, 1))
  {
    ADBG ("Invalid CM mode value")
    Return (0xFF)
  }
  Store (0, Local0)
  //
  // Set Force Power if it is not set.
  //
  If (LEqual (DFPE, 0)) {
    ADBG ("Set Force Power")
    Store (0x22, DMAD)
    Store (1, DFPE)
    //
    // Poll the ITBT NVM FW Ready bit with timeout(default is 500ms) before send the TBT MailBox command
    //
    While (LAnd (LEqual (INFR, 0), LLess (Local0, ITFP))) {
      Sleep (1)
      Increment (Local0)
    }
  } Else {
    ADBG ("Force Power has been set")
  }

  If (LNotEqual (Local0, ITFP)) {
    //
    // Send CM mode mailbox command
    //
    Store (CMMB (Arg0), Local1)

    //
    // Send Connect Topology command if CM mode is set to FW CM successfully
    //
    If (LAnd (LEqual (Arg0, 0), LEqual (Local1, 0)))
    {
      //
      // Send Connect Topology command
      //
      ADBG ("Send Connect Topology command")
      ITMB (PCIE2TBT_CONNECT_TOPOLOGY_COMMAND)
      //
      // Wait for command done bit set
      //
      Store (WTBS (ITCT), Local2)
      If (LNotEqual (Local2, 0))
      {
        ADBG ("Connect topology command done bit is set")
        Store (0, Local1)
      }
      Else {
        ADBG ("Connect topology command timeout!")
        Store (0xFF, Local1)
      }
      Store (0, P2TB)
    }
  }
  Else {
    ADBG ("Force Power Timeout occurred")
    Store (0xFF, Local1)
  }

  //
  // Unset Force Power.
  //
  If (LNotEqual (DFPE, 0)) {
    ADBG ("Unset Force Power")
    Store (0, DFPE)
  }

  ADBG (Concatenate ("iTBT change CM mode status = ", Local1))
  Return (Local1)
}