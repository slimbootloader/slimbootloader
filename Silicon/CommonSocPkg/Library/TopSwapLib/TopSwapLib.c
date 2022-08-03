/** @file
  Library for top swap.

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TopSwapLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>

#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PID_RTC_HOST                    0xC3
#define R_RTC_PCR_BUC                   0x3414

/**
  Get the address of P2sb register.

  This function get the address of the P2sb register.

  @retval  UINTN  The base address of the P2sb register.
**/
UINTN
GetP2sbBase (
  VOID
  )
{
  return MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_LPC, 1, 0);
}

/**
  Get the address of the top swap register.

  This function get the address of the top swap register and reports
  if P2sbBar was unhidden in the process.

  @param[out] TopSwapBase   The base address of the top swap register.
  @param[out] P2sbWasHidden If P2sbBar needed to be unhidden to get
                            the address.
  @retval  EFI_SUCCESS      Address successfully found.
  @retval  others           Error occurred.
**/
EFI_STATUS
GetTopSwapBase (
  OUT UINT32    *TopSwapBase,
  OUT BOOLEAN   *P2sbWasHidden
  )
{
  UINTN   P2sbBase;
  UINT32  P2sbBar;
  BOOLEAN P2sbIsHidden;

  if (TopSwapBase == NULL || P2sbWasHidden == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  P2sbBase = GetP2sbBase ();
  P2sbIsHidden = FALSE;
  if (MmioRead16 (P2sbBase) == 0xFFFF) {
    MmioWrite8 (P2sbBase + 0xE1, 0);
    DEBUG ((DEBUG_INFO, "P2sbBar is hidden, unhide it\n"));
    P2sbIsHidden = TRUE;
  }


  P2sbBar = MmioRead32 (P2sbBase + 0x10);
  P2sbBar  &= 0xFFFFFFF0;
  if (P2sbBar == 0xFFFFFFF0) {
    DEBUG ((DEBUG_ERROR, "P2sbBar could not be unhidden!\n"));
    return EFI_ACCESS_DENIED;
  }

  *TopSwapBase    = P2sbBar | ((PID_RTC_HOST) << 16) | (UINT16)(R_RTC_PCR_BUC);
  *P2sbWasHidden  = P2sbIsHidden;

  return EFI_SUCCESS;
}

/**
  Hide P2sbBar.
**/
VOID
HideP2sbBar (
  VOID
  )
{
  MmioWrite8 (GetP2sbBase () + 0xE1, BIT0);
}

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
  EFI_STATUS  Status;
  UINT32      TopSwapBase;
  BOOLEAN     P2sbIsHidden;

  Status = GetTopSwapBase (&TopSwapBase, &P2sbIsHidden);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Partition == BackupPartition) {
    MmioOr32 (TopSwapBase, BIT0);
  } else {
    MmioAnd32 (TopSwapBase, (UINT32)~BIT0);
  }

  if (P2sbIsHidden) {
    HideP2sbBar ();
  }

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
  EFI_STATUS  Status;
  UINT32      TopSwapBase;
  BOOLEAN     P2sbIsHidden;

  if (Partition == NULL) {
    DEBUG ((DEBUG_ERROR, "Partition not initialized!\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = GetTopSwapBase (&TopSwapBase, &P2sbIsHidden);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (MmioRead32 (TopSwapBase) & BIT0) {
    *Partition = BackupPartition;
  } else {
    *Partition = PrimaryPartition;
  }

  if (P2sbIsHidden) {
    HideP2sbBar ();
  }

  return EFI_SUCCESS;
}
