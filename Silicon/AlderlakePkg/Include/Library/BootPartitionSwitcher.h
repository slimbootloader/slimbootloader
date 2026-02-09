/** @file
  Boot partition switcher interface.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _BOOT_PARTITION_SWITCHER_H_
#define _BOOT_PARTITION_SWITCHER_H_

#include <Library/BootloaderCommonLib.h>

VOID
EFIAPI
BootPartitionSwitcher (
  IN BOOT_PARTITION Partition
  );

#endif
