/** @file
  Library for top swap.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TopSwapLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/P2SbLib.h>
#include <Register/PchRegsPcr.h>
#include <Library/PciLib.h>

#define R_RTC_PCR_BUC                   0x3414

#if FixedPcdGetBool(PcdMtlSSupport)
#define  P2SB_BUS                       0x80
#define  P2SB_DEVICE                    31
#define  P2SB_FUNCTION                  1
#define  P2SB_PID                       MTL_PCH_PID_RTC
#else
#define  P2SB_BUS                       0
#define  P2SB_DEVICE                    31
#define  P2SB_FUNCTION                  1
#define  P2SB_PID                       PID_RTC_HOST
#endif


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

  P2SbAndThenOr32 (PCI_LIB_ADDRESS(P2SB_BUS, P2SB_DEVICE, P2SB_FUNCTION, 0), MTL_PCH_PID_RTC, 0, R_RTC_PCR_BUC, AndData, OrData);

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

  RegValue = P2sbRead32 (PCI_LIB_ADDRESS(P2SB_BUS, P2SB_DEVICE, P2SB_FUNCTION, 0), MTL_PCH_PID_RTC, 0, R_RTC_PCR_BUC);
  *Partition = (BOOT_PARTITION)(RegValue & BIT0);

  return EFI_SUCCESS;
}

