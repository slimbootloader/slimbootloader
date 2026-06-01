/** @file
  Input Handler header - keyboard input and scan code translation.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

#include "Include/UiCommon.h"

/**
  Initialize input handler.

  @retval EFI_SUCCESS   Input initialized.
**/
EFI_STATUS
InputInit (
  VOID
  );

/**
  Blocking read of a single key, returning a UI_KEY_xxx code.

  @return Key code (UI_KEY_UP, UI_KEY_ENTER, etc.)
**/
UINT16
InputGetKey (
  VOID
  );

/**
  Non-blocking poll for key availability.

  @return TRUE if a key is available.
**/
BOOLEAN
InputKeyReady (
  VOID
  );

#endif // _INPUT_HANDLER_H_
