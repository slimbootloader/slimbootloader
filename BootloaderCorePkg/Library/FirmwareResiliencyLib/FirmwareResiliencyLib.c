/** @file

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/TcoTimerLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/DebugLib.h>
#include <Library/FirmwareResiliencyLib.h>
#include <Library/TopSwapLib.h>
#include <Library/PcdLib.h>
#include <Library/WatchDogTimerLib.h>
#include <FirmwareUpdateStatus.h>

/**
  Retrieve FW update state from the reserved region

  @retval StateMachine The current FWU state machine
**/
UINT8
EFIAPI
GetFwuStateMachine (
  VOID
  )
{
  FW_UPDATE_STATUS    *FwUpdStatus;
  EFI_STATUS          Status;
  UINT32              RsvdBase;
  UINT32              RsvdSize;

  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
  if (!EFI_ERROR (Status)) {
    FwUpdStatus = (FW_UPDATE_STATUS *)(UINTN)RsvdBase;
    return FwUpdStatus->StateMachine;
  }
  return FW_UPDATE_SM_INIT;
}

/**
  Check if ACM detected corruption in IBB
**/
VOID
EFIAPI
CheckForAcmFailures (
  VOID
  )
{
  UINT8 StateMachine;

  // If already marked in recovery path, no need to check for recovery path.
  if (IsRecoveryTriggered ()) {
    return;
  }

  StateMachine = GetFwuStateMachine ();
  switch (StateMachine) {
    case FW_UPDATE_SM_PART_A:
      if (GetCurrentBootPartition () == PrimaryPartition) {
        DEBUG((DEBUG_INFO, "Partition to be updated is same as current boot partition (primary)\n"));
        SetRecoveryTrigger ();
      }
      break;

    case FW_UPDATE_SM_PART_B:
      if (GetCurrentBootPartition () == BackupPartition) {
        DEBUG((DEBUG_INFO, "Partition to be updated is same as current boot partition (backup)\n"));
        SetRecoveryTrigger ();
      }
      break;

    default:
      if (GetCurrentBootPartition () == BackupPartition) {
        DEBUG((DEBUG_INFO, "Booting from backup partition outside of update\n"));
        SetRecoveryTrigger ();
      }
      break;
  }
}

/**
  Check if TCO timer detected corruption in OBB or a dead loop/crash in IBB/OBB

    @param[in] BootFailureThreshold The number of boots to attempt before recovery
**/
VOID
EFIAPI
CheckForTcoTimerFailures (
  IN UINT8 BootFailureThreshold
  )
{
  EFI_STATUS          Status;
  BOOT_PARTITION      NewPartition;
  UINT32              FailedBootCount;

  // If unable to boot all the way up to PLD, recovery is necessary.
  if (WasBootCausedByTcoTimeout ()) {
    ClearTcoStatus ();
    IncrementFailedBootCount ();
    FailedBootCount = GetFailedBootCount ();
    DEBUG ((DEBUG_INFO, "Boot failure occurred! Failed boot count: %d\n", FailedBootCount));
    if (FailedBootCount >= BootFailureThreshold) {
      if (IsRecoveryTriggered ()) {
        CpuHalt ("Unable to recover partition, both partitions are failing!\n");
      }
      ClearFailedBootCount ();
      SetRecoveryTrigger ();
      NewPartition = GetCurrentBootPartition () == PrimaryPartition ? BackupPartition : PrimaryPartition;
      DEBUG ((DEBUG_INFO, "Boot failure threshold reached! Switching to partition: %d\n", NewPartition));
      Status = SetBootPartition (NewPartition);
      if (EFI_ERROR (Status)) {
        CpuHalt ("Unable to recover partition, failed to switch boot partition!\n");
      }
      ResetSystem (EfiResetCold);
    }
  }
}
