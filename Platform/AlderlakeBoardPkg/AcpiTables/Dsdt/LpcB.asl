/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


// LPC Bridge - Device 31, Function 0
scope (\_SB.PC00.LPCB) {
      Include ("EC.ASL")
      Include ("LpcDev.asl")
      Include ("H8S2113Sio.asl")
}
