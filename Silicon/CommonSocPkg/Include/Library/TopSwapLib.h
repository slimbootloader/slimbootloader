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
  Get the address of the top swap register.

  This function gets the address of the top swap register and reports
  if P2SB was unhidden in the process.

  @param[out] TopSwapBase   The base address of the top swap register.
  @param[out] P2sbWasHidden If P2SB BAR needed to be unhidden to get
                            the address.
  @retval  EFI_SUCCESS      Address successfully found.
  @retval  others           Error occurred.
**/
EFI_STATUS
GetTopSwapBase (
  OUT UINT32    *TopSwapBase,
  OUT BOOLEAN   *P2sbWasHidden
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
