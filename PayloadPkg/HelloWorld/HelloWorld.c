/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "HelloWorld.h"

/**
  Payload main entry.

  @param  Param           parameter passed from SwitchStack().
  @param  PldBase         payload base passed from SwitchStack().

**/
VOID
EFIAPI
PayloadMain (
  IN  VOID             *Param,
  IN  VOID             *PldBase
  )
{
  UINT8      Key;

  DEBUG ((DEBUG_INFO, "\n\n==================== Hello World ====================\n\n"));

  Key = 0;
  while (Key != 0x1B) {
    if (ConsolePoll ()) {
      if (ConsoleRead (&Key, 1) > 0) {
        if ((Key >= 0x20) && (Key < 0x7F)) {
          ConsolePrint("Key '%c' pressed !\n", Key);
        }
      }
    }
  }

  DEBUG ((DEBUG_INFO, "\nExit\n"));
  CpuDeadLoop ();
}
