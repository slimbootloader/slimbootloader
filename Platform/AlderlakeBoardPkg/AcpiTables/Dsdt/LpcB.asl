/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


// LPC Bridge - Device 31, Function 0
scope (\_SB.PC00.LPCB) {

#if FixedPcdGetBool(PcdAdlNSupport) == 0
      Include ("EC.ASL")
#endif
      Include ("LpcDev.asl")
      Include ("H8S2113Sio.asl")
}
