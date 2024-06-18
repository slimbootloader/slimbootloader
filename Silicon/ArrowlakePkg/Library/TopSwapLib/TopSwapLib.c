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
#include <Library/P2SbSocLib.h>
#include <Include/P2SbSidebandAccessLib.h>
#include <Register/PchRegsPcr.h>

#define R_RTC_PCR_BUC                   0x3414

/**
  Get the P2SB controller and build the P2SB sideband access.

  @param P2SbAccess
  @return EFI_STATUS
**/
EFI_STATUS
EFIAPI
GetP2SbAccess (
  OUT P2SB_CONTROLLER               *P2SbController,
  OUT P2SB_SIDEBAND_REGISTER_ACCESS *P2SbAccess
  )
{
  EFI_STATUS                    Status;

#if FixedPcdGetBool(PcdMtlSSupport)
  Status = MtlPchGetP2SbController(P2SbController);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = BuildP2SbSidebandAccess(
    P2SbController,
    MTL_PCH_PID_RTC,
    0,
    P2SbPrivateConfig,
    P2SbMsgAccess,
    FALSE,
    P2SbAccess
    );
#else
  Status = MtlSocGetP2SbController(P2SbController);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = BuildP2SbSidebandAccess(
    P2SbController,
    PID_RTC_HOST,
    0,
    P2SbMemory,
    P2SbMmioAccess,
    TRUE,
    P2SbAccess
    );
#endif
  return Status;
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
  EFI_STATUS                    Status;
  P2SB_CONTROLLER               P2SbController;
  P2SB_SIDEBAND_REGISTER_ACCESS P2SbAccess;

  Status = GetP2SbAccess (&P2SbController, &P2SbAccess);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Partition == BackupPartition) {
    DEBUG((DEBUG_INFO, "Setting Backup Partition\n"));
    P2SbAccess.Access.Or32(&P2SbAccess.Access, R_RTC_PCR_BUC, BIT0);
  } else {
    DEBUG((DEBUG_INFO, "Setting Primary Partition\n"));
    P2SbAccess.Access.And32(&P2SbAccess.Access, R_RTC_PCR_BUC, (UINT32)~BIT0);
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
  P2SB_CONTROLLER               P2SbController;
  P2SB_SIDEBAND_REGISTER_ACCESS P2SbAccess;

  if (Partition == NULL) {
    DEBUG ((DEBUG_ERROR, "Partition not initialized!\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = GetP2SbAccess (&P2SbController, &P2SbAccess);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (P2SbAccess.Access.Read32(&P2SbAccess.Access, R_RTC_PCR_BUC) & BIT0) {
    *Partition = BackupPartition;
    DEBUG((DEBUG_INFO, "Current Boot Partition is Backup\n"));
  } else {
    *Partition = PrimaryPartition;
    DEBUG((DEBUG_INFO, "Current Boot Partition is Primary\n"));
  }

  return EFI_SUCCESS;
}

