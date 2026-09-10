/** @file
  This file contains the TXT SSDT Table ASL code.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "TxtSsdt.aml",
  "SSDT",
  2,
  "INTEL ",
  "TxtSsdt",
  0x1000
  )
{
  External (CPID)

  Scope(\_SB) {
    //
    // TXT ACM Device for SINIT ACM loading
    //
    Device (ACM) {
      Name (_HID, "INTC1025" )  // _HID: Hardware ID
      //
      // Update _CID (Compatibility ID)
      //   _CID will be updated based on CPU ID
      //
      Method (_CID) {
        //
        // Mask CPU Family
        //
        And (CPID, 0xFFFFFFF0, Local0)
        If (LOr (LOr(LEqual (Local0, 0x000C06D0), LEqual (Local0, 0x000C06D2)), LOr(LEqual (Local0, 0x000C06C0), LEqual (Local0, 0x000C06C2)))) {
          Return ("INT_PTL_SINIT")
        }
        //
        // PTL and WCL share driver. Therefore use the same descriptor string
        //
        If (LEqual (Local0, 0x000D0650)) {
          Return ("INT_PTL_SINIT")
        }
        Return ("INT_UNK_SINIT")
      } // End of Method (_CID)
    } // End of Device (ACM)
  } // End of Scope(\_SB) for TxtAcm
}  // End of definition block
