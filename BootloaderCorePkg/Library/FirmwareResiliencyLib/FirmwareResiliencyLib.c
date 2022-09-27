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
  Retrieve the current overall FW update state, if it exists

  @param[out] State The current overall FW update state
**/
VOID
EFIAPI
GetUpdateState (
  OUT UINT8* State
  )
{
  FW_UPDATE_STATUS    *FwUpdStatus;
  EFI_STATUS          Status;
  UINT32              RsvdBase;
  UINT32              RsvdSize;
  if (State != NULL) {
    Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
    if (EFI_ERROR (Status)) {
      *State = FW_UPDATE_SM_INIT;
    } else {
      FwUpdStatus = (FW_UPDATE_STATUS *)(UINTN)RsvdBase;
      *State = FwUpdStatus->StateMachine;
    }
  }
}

/**
  Retrieve the current in-flight FW update state, if it exists

  @param[out] State The current in-flight FW update state
**/
VOID
EFIAPI
GetInFlightUpdateState (
  OUT UINT8* State
  )
{
  FW_UPDATE_COMP_STATUS   *FwUpdCompStatus;
  EFI_STATUS              Status;
  UINT32                  RsvdBase;
  UINT32                  RsvdSize;
  UINT8                   Cnt;

  if (State != NULL) {
    Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
    if (EFI_ERROR (Status)) {
      *State = FW_UPDATE_IMAGE_UPDATE_NONE;
    } else {
      // Find the first in-flight update, if it exists
      FwUpdCompStatus = (FW_UPDATE_COMP_STATUS *)(UINTN)(RsvdBase + sizeof(FW_UPDATE_STATUS));
      Cnt = 0;
      while (!(FwUpdCompStatus->UpdatePending & FW_UPDATE_IN_PROGRESS_FLAGS) && Cnt < MAX_FW_COMPONENTS) {
        ++FwUpdCompStatus;
        ++Cnt;
      }

      // Return status of first in-flight update, otherwise return none status
      if(Cnt == MAX_FW_COMPONENTS) {
        *State = FW_UPDATE_IMAGE_UPDATE_DONE;
      } else {
        *State = FwUpdCompStatus->UpdatePending;
      }
    }
  }
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
  UINT8 InFlightUpdateState;

  GetInFlightUpdateState (&InFlightUpdateState);
  switch (InFlightUpdateState) {
    case FW_UPDATE_IMAGE_UPDATE_PART_A:
      if (GetCurrentBootPartition () == PrimaryPartition) {
        DEBUG((DEBUG_INFO, "Partition to be updated is same as current boot partition (primary)\n"));
        SetRecoveryTrigger ();
      }
      break;

    case FW_UPDATE_IMAGE_UPDATE_PART_B:
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
  if (WasPreviousTcoTimeout ()) {
    ClearTcoStatus ();
    IncrementFailedBootCount ();
    FailedBootCount = GetFailedBootCount ();
    DEBUG ((DEBUG_INFO, "Boot failure occurred! Failed boot count: %d\n", FailedBootCount));
    if (FailedBootCount >= BootFailureThreshold) {
      ClearFailedBootCount ();
      SetRecoveryTrigger ();
      NewPartition = GetCurrentBootPartition () == PrimaryPartition ? BackupPartition : PrimaryPartition;
      DEBUG ((DEBUG_INFO, "Boot failure threshold reached! Switching to partition: %d\n", NewPartition));
      Status = SetBootPartition (NewPartition);
      if (EFI_ERROR (Status)) {
        CpuHalt("Unable to recover corrupted partition!\n");
      }
      ResetSystem (EfiResetCold);
    }
  }
}
