/** @file
Intel ACPI sample code for Connectivity devices

 Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/PchInfoLib.h>

External(\_SB.PC00.XHCI.RHUB.HS08, DeviceObj)
External(\_SB.PC00.XHCI.RHUB.HS10, DeviceObj)
External(\_SB.PC00.CNIP, MethodObj)
External(\_SB.PC00.SBTE, MethodObj)
External(\_SB.PC00.GBTE, MethodObj)
External(PCHS)
External(CNVI)
External(CBTC)

//
// Report BT power resource if CNVi is present and BT interface is USB.
//
If (AND (\_SB.PC00.CNIP (), LEqual (CBTC, 1))) {
  If (LEqual (PCHS, PCH_LP)) {
    Scope(\_SB.PC00.XHCI.RHUB.HS10)
    {
      //
      // Power resource object for CNVi BT
      //
      PowerResource(BTPR, 0, 0)
      {
        Method(_STA, 0)
        {
          If(LEqual (\_SB.PC00.GBTE (), 1)) {
            Return (0x01)
          } Else {
            Return (0x00)
          }
        }
        Method(_ON, 0, Serialized)
        {
          \_SB.PC00.SBTE (0x01) // Turn on BT
        }
        Method(_OFF, 0, Serialized)
        {
          \_SB.PC00.SBTE (0x00) // Turn off BT
        }
      }
    } // Scope(\_SB.PC00.XHCI.RHUB.HS10)
  }

} // If (AND (\_SB.PC00.CNIP (), LEqual (CBTC, 1)))
