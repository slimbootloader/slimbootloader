/** @file
  Header file for top swap library implementation.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TOP_SWAP_LIB_H_
#define _TOP_SWAP_LIB_H_

#include <Library/BootloaderCommonLib.h>

/**
  Switch between the boot partitions.

  This function will set the top swap register bit with the given partition.

  @param[in] Partition        Partition to be indicated in register.

  @retval  EFI_SUCCESS        Parition successfully set.
  @retval  others             Error occurred.
**/
EFI_STATUS
EFIAPI
SetBootPartition (
  IN BOOT_PARTITION  Partition
  );

/**
  Get the current boot partition.

  This function will read the top swap register bit for the current partition.

  @param[in] Partition        Partition indicated by register.

  @retval  EFI_SUCCESS        Partition successfully retrieved.
  @retval  others             Error occurred.
**/
EFI_STATUS
EFIAPI
GetBootPartition (
  OUT BOOT_PARTITION *Partition
  );

#endif
