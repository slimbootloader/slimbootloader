/**@file
  isCLK ASL code.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#if FixedPcdGetBool (PcdWclSupport) == 1
#define IETM_PARTICIPANT_HID           "INTC10FC"
#else
#define IETM_PARTICIPANT_HID           "INTC10D4"
#define LEDGER_ISLAND_PARTICIPANT_HID  "INTC10D5"
#endif


Scope (\_SB) {
  Device (IETM) {
    //
    // Intel Platform Framework Device
    //
    Name (_UID, "IETM")
    Name (_HID, IETM_PARTICIPANT_HID)

    Method (_STA) {
      If (LEqual (IPFE,1)) {
        Return (0x0F)
      } Else {
        Return (0x00)
      }
    }
  }
}