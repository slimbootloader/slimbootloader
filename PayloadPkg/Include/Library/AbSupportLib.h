/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Guid/OsBootOptionGuid.h>

#ifndef __AB_BOOT_SUPPORT_LIB_H__
#define __AB_BOOT_SUPPORT_LIB_H__

/**
  Check which slot to boot.

  @param[in]  BootOption      Image information where to load image.
  @param[in]  HwPartHandle    The hardware partition handle

  @retval  0                  Select first slot to boot
  @retval  1                  Select second slot to boot
**/
INT32
EFIAPI
GetBootSlot (
  IN OS_BOOT_OPTION          *OsBootOption,
  IN EFI_HANDLE              HwPartHandle
  );

#endif
