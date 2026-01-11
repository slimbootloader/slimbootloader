/** @file
  Intel ACPI Sample Code for CNV in DSDT

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External (\_SB.GGOV, MethodObj)
External (\_SB.SGOV, MethodObj)
External (GBTK)
//
// WiFi/BT Mutex
//
Mutex (CNMT, 0)
Name (RDLY, 160)
Name (BRMT, 0)

Scope (\_SB)
{
  //
  // Set M.2 BT RF-Kill (W_DISABLE2#) pin
  //
  Method (BTRK, 0x1, Serialized)
  {
    //
    // Arg0 - Value to W_DISABLE2#
    //
    If (CondRefOf (GBTK)) {
      If (LNotEqual (GBTK, 0)) {
        If (CondRefOf (\_SB.SGOV)) {
          \_SB.SGOV (GBTK, Arg0)
        }
      }
    } Else {
      ADBG ("Set M.2 BT RF-Kill pin failied")
    }
  }

  //
  // Get value of M.2 BT RF-Kill (W_DISABLE2#) pin
  //
  Method (GBTR, 0)
  {
    If (CondRefOf (\_SB.GGOV)) {
      If (CondRefOf (GBTK)) {
        Return (\_SB.GGOV (GBTK))
      } Else {
        ADBG ("Get M.2 BT RF-Kill pin failied")
        Return (0)
      }
    } Else {
      ADBG ("Get M.2 BT RF-Kill pin status failied")
      Return (0)
    }
  }
}