/** @file
  This file provids APi to call into Multiboot entry.

  Copyright (c) 2014 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/MultibootLib.h>
#include <Library/ThunkLib.h>

/**
  ASM code to jump into MultiBoot image entry point.

  @param[in]     State  Pointer to boot state buffer defined by MultiBoot spec.

**/
VOID
EFIAPI
AsmJumpToMultibootOs (
  IN IA32_BOOT_STATE  *State
);


/**
  Jump into 32bit MultiBoot image entry point by thunking.

  @param[in]     State  Pointer to boot state buffer defined by MultiBoot spec.

**/
VOID
EFIAPI
JumpToMultibootOs (
  IN IA32_BOOT_STATE    *State
  )
{
  Execute32BitCode ((UINT64)&AsmJumpToMultibootOs, (UINT64)State, (UINT64)NULL, FALSE);
}
