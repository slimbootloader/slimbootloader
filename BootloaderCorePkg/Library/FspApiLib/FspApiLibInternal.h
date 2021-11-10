/** @file

  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _FSP_API_LIB_INTERNAL_H_
#define _FSP_API_LIB_INTERNAL_H_

#include <PiPei.h>
#include <Guid/FspHeaderFile.h>
#include <FspEas/FspApi.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ThunkLib.h>

/**
  Switch to new stack and then call specified function with arguments.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Context1     The first parameter to pass to 32bit code.
  @param[in] Context2     The second parameter to pass to 32bit code.
  @param[in] NewStack     The new stack top to use.

  @return    EFI_STATUS returned from the 32bit code.
**/
EFI_STATUS
EFIAPI
FspmSwitchStack (
  IN VOID        *EntryPoint,
  IN VOID        *Context1,   OPTIONAL
  IN VOID        *Context2,   OPTIONAL
  IN VOID        *NewStack
  );

#endif
