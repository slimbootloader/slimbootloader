/** @file
  Library for top swap.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TopSwapLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/P2sbLib.h>
#include <PchReservedResources.h>
#include <Library/PciLib.h>

#define R_RTC_PCR_BUC                   0x3414
#define  P2SB_PID                       0x6C
#define  P2SB_BASE                      PCH_PCR_BASE_ADDRESS

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
  )
{
  UINT32             AndData;
  UINT32             OrData;

  AndData = (UINT32)~BIT0;
  if (Partition == BackupPartition) {
    OrData = BIT0;
  } else {
    OrData = 0;
  }

  P2SbAndThenOr32 (P2SB_BASE, P2SB_PID, 0, R_RTC_PCR_BUC, AndData, OrData);
  return EFI_SUCCESS;
}

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
  )
{
  UINT32             RegValue;

  RegValue = P2sbRead32 (P2SB_BASE, P2SB_PID, 0, R_RTC_PCR_BUC);
  *Partition = (BOOT_PARTITION)(RegValue & BIT0);


  return EFI_SUCCESS;
}
