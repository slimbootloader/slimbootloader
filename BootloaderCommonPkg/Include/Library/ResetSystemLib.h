/** @file
  System reset Library Services.  This library class defines a set of
  methods that reset the whole system.

Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __RESET_SYSTEM_LIB_H__
#define __RESET_SYSTEM_LIB_H__
#include <PiPei.h>

/**
  Resets the entire platform.

  @param[in] ResetType          The type of reset to perform.

**/
VOID
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE   ResetType
  );

#endif
