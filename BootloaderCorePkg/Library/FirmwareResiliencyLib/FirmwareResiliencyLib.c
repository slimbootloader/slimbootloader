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
#include <Library/SpiFlashLib.h>
#include <Library/VariableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Pi/PiBootMode.h>
#include <FirmwareUpdateStatus.h>
#include <RecoveryStatus.h>

STATIC CONST CHAR16 *mRecoveryStatusVariableName = RECOVERY_STATUS_VARIABLE_NAME;

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
  FW_UPDATE_STATUS    FwUpdStatus;
  EFI_STATUS          Status;
  UINT32              RsvdBase;
  UINT32              RsvdSize;

  Status = GetComponentInfoByPartition (FLASH_MAP_SIG_BLRESERVED, FALSE, &RsvdBase, &RsvdSize);
  if (!EFI_ERROR (Status) && RsvdBase >= PcdGet32(PcdFlashBaseAddress)) {
    RsvdBase -= PcdGet32(PcdFlashBaseAddress);
    Status = SpiFlashRead (FlashRegionBios, RsvdBase, sizeof (FwUpdStatus), (UINT8 *)&FwUpdStatus);
    if (!EFI_ERROR (Status)) {
      return FwUpdStatus.StateMachine;
    }
  }
  return FW_UPDATE_SM_INIT;
}

/**
  Initialize a RECOVERY_STATUS structure to its default (no recovery) state.

  @param[out] Status  The structure to initialize.
**/
STATIC
VOID
InitDefaultRecoveryStatus (
  OUT RECOVERY_STATUS  *Status
  )
{
  ZeroMem (Status, sizeof (RECOVERY_STATUS));
  Status->Revision        = RECOVERY_STATUS_REVISION;
  Status->Reason          = RECOVERY_REASON_NONE;
  Status->AttemptCount    = 0;
  Status->LastResult      = RECOVERY_RESULT_PENDING;
  Status->FailedBootCount = 0;
}

/**
  Persist the RECOVERY_STATUS variable to the SBL variable region.

  @param[in] Status  The structure to write.

  @retval EFI_SUCCESS  The variable was written successfully.
  @retval others       The write failed.
**/
STATIC
EFI_STATUS
SaveRecoveryStatus (
  IN RECOVERY_STATUS  *Status
  )
{
  return SetVariable (
           (CHAR16 *)mRecoveryStatusVariableName,
           &gRecoveryStatusVariableGuid,
           EFI_VARIABLE_NON_VOLATILE,
           sizeof (RECOVERY_STATUS),
           Status
           );
}

/**
  Reset the RECOVERY_STATUS variable to its default (no recovery) state..

  @retval EFI_SUCCESS  The variable was reset successfully.
  @retval others       The write failed.
**/
STATIC
EFI_STATUS
ResetRecoveryStatus (
  VOID
  )
{
  RECOVERY_STATUS  Status;

  InitDefaultRecoveryStatus (&Status);
  return SaveRecoveryStatus (&Status);
}

/**
  Detect whether CSME triggered a Top Swap due to a boot failure outside SBL.

  On IBB corruption, CSME forces a Top Swap (via WDT expiry) so the next boot
  comes from the backup partition. The only reliable signature is booting from
  the backup partition with no firmware update in progress.

  @retval TRUE   A CSME-WDT-triggered Top Swap (recovery) was detected.
  @retval FALSE  No CSME-WDT failure detected.
**/
STATIC
BOOLEAN
DetectCsmeWdtFailure (
  VOID
  )
{
  UINT8 StateMachine;

  // Already in recovery path; no need to re-detect.
  if (IsRecoveryTriggered ()) {
    return FALSE;
  }

  StateMachine = GetFwuStateMachine ();

  // An in-progress update (PART_A/PART_B) boots by design; not a trigger.
  if (StateMachine == FW_UPDATE_SM_PART_A) {
    if (GetCurrentBootPartition () == PrimaryPartition) {
      DEBUG ((DEBUG_INFO, "Partition to be updated is same as current boot partition (primary)\n"));
    }
    return FALSE;
  }
  if (StateMachine == FW_UPDATE_SM_PART_B) {
    if (GetCurrentBootPartition () == BackupPartition) {
      DEBUG ((DEBUG_INFO, "Partition to be updated is same as current boot partition (backup)\n"));
    }
    return FALSE;
  }

  // Outside an update the expected partition is primary; backup means CSME
  // performed a Top Swap. This is the only valid recovery trigger here.
  if (GetCurrentBootPartition () == BackupPartition) {
    DEBUG ((DEBUG_INFO, "Booting from backup partition outside of update\n"));
    return TRUE;
  }

  return FALSE;
}

/**
  Unified resiliency check point.

  Consolidates ACM and TCO checks and maintains persistent recovery state.
  Must be called after BoardInit(PostConfigInit) and before FSP-M.

  @param[in] BootFailureThreshold  Consecutive TCO timeouts before recovery.
  @param[in] MaxRecoveryAttempts   Recovery attempts before CpuHalt().
**/
VOID
EFIAPI
UnifiedResiliencyCheck (
  IN UINT8  BootFailureThreshold,
  IN UINT8  MaxRecoveryAttempts
  )
{
  RECOVERY_STATUS   Status;
  UINTN             Size;
  EFI_STATUS        EfiStatus;
  BOOLEAN           VarExists;
  BOOLEAN           NeedPartitionSwitch;
  UINT8             NewReason;
  BOOT_PARTITION    NewPartition;
  // Skip explicit capsule-update boots; still run for recovery-triggered boots.
  if ((GetBootMode () == BOOT_ON_FLASH_UPDATE) && !IsRecoveryTriggered ()) {
    return;
  }
  //
  // Read existing "RecoveryStatus" variable (may not exist).
  //
  ZeroMem (&Status, sizeof (Status));
  Size      = sizeof (RECOVERY_STATUS);
  EfiStatus = GetVariable (
                (CHAR16 *)mRecoveryStatusVariableName,
                &gRecoveryStatusVariableGuid,
                NULL,
                &Size,
                &Status
                );

  // Accept the variable only when it matches the current size and revision.
  // Any malformed or wrong-revision entry from an earlier build is removed.
  VarExists = (!EFI_ERROR (EfiStatus) &&
               (Size == sizeof (RECOVERY_STATUS)) &&
               (Status.Revision == RECOVERY_STATUS_REVISION));
  if (!EFI_ERROR (EfiStatus) && !VarExists) {
    DEBUG ((DEBUG_WARN, "Resiliency: discarding invalid RecoveryStatus entry\n"));
    ResetRecoveryStatus ();
  }

  // If variable exists, handle post-recovery and stale-counter cleanup.
  if (VarExists) {
    // Clear state after successful recovery.
    if (Status.LastResult == RECOVERY_RESULT_SUCCESS) {
      DEBUG ((DEBUG_INFO, "Resiliency: recovery succeeded, clearing state\n"));
      ResetRecoveryStatus ();
      ClearRecoveryTrigger ();
      VarExists = FALSE;
    }

    // Drop stale non-consecutive TCO count.
    if ((Status.Reason == RECOVERY_REASON_NONE) &&
        (Status.FailedBootCount > 0) &&
        !WasBootCausedByTcoTimeout ()) {
      DEBUG ((DEBUG_INFO, "Resiliency: clearing stale TCO boot counter\n"));
      ResetRecoveryStatus ();
      VarExists = FALSE;
    }
  }

  //
  // Detect failures.
  //
  NewReason           = RECOVERY_REASON_NONE;
  NeedPartitionSwitch = FALSE;

  //
  // CSME-WDT triggered Top Swap detection (failure detected outside SBL).
  //
  if (DetectCsmeWdtFailure ()) {
    NewReason           |= RECOVERY_REASON_CSME_WDT;
  }

  //
  // TCO timer failure detection.
  //
  if (WasBootCausedByTcoTimeout ()) {
    ClearTcoStatus ();

    if (!VarExists) {
      InitDefaultRecoveryStatus (&Status);
      VarExists = TRUE;
    }

    Status.FailedBootCount++;
    DEBUG ((DEBUG_INFO, "Boot failure occurred! Failed boot count: %d\n", Status.FailedBootCount));

    if (Status.FailedBootCount >= BootFailureThreshold) {
      NewReason          |= RECOVERY_REASON_SBL;
      NeedPartitionSwitch = TRUE;
    } else {
      // Below threshold: persist TCO counter.
      EfiStatus = SaveRecoveryStatus (&Status);
      if (EFI_ERROR (EfiStatus)) {
        CpuHalt ("Resiliency: failed to persist TCO counter state\n");
      }
      if ((NewReason == RECOVERY_REASON_NONE) && (Status.Reason == RECOVERY_REASON_NONE)) {
        return;
      }
    }
  }

  //
  // No new failure detected this boot.
  //
  if (NewReason == RECOVERY_REASON_NONE) {
    if (VarExists && (Status.Reason != RECOVERY_REASON_NONE)) {
      // Recovery already pending from a prior boot.
      if (Status.LastResult != RECOVERY_RESULT_PENDING) {
        // Previous attempt failed; count retry.
        Status.AttemptCount++;
        Status.LastResult = RECOVERY_RESULT_PENDING;
        if (Status.AttemptCount > MaxRecoveryAttempts) {
          CpuHalt ("Recovery failed after maximum attempts\n");
        }
        EfiStatus = SaveRecoveryStatus (&Status);
        if (EFI_ERROR (EfiStatus)) {
          CpuHalt ("Resiliency: failed to persist retry state\n");
        }
      }
      //
      // Keep trigger set so FW update payload runs recovery.
      //
      SetRecoveryTrigger ();
    }
    //
    // Otherwise no recovery is needed - continue normal boot.
    //
    return;
  }

  //
  // Recovery needed - update or create the variable.
  //
  if (VarExists) {
    Status.Reason     |= NewReason;
    Status.AttemptCount += 1;
    Status.LastResult   = RECOVERY_RESULT_PENDING;
  } else {
    InitDefaultRecoveryStatus (&Status);
    Status.Reason       = NewReason;
    Status.AttemptCount = 1;
    Status.LastResult   = RECOVERY_RESULT_PENDING;
  }

  //
  // Anti-loop protection.
  //
  if (Status.AttemptCount > MaxRecoveryAttempts) {
    CpuHalt ("Recovery failed after maximum attempts\n");
  }

  //
  // Write updated variable.
  //
  EfiStatus = SaveRecoveryStatus (&Status);
  if (EFI_ERROR (EfiStatus)) {
    CpuHalt ("Resiliency: failed to persist recovery state\n");
  }

  //
  // Ensure the WDT recovery trigger (BIT20) is set.
  //
  SetRecoveryTrigger ();

  //
  // Switch partition and cold reset when required.
  //
  if ((Status.Reason & RECOVERY_REASON_SBL) && NeedPartitionSwitch) {
    NewPartition = (GetCurrentBootPartition () == PrimaryPartition) ? BackupPartition : PrimaryPartition;
    DEBUG ((DEBUG_INFO, "Boot failure threshold reached! Switching to partition: %d\n", NewPartition));
    EfiStatus = SetBootPartition (NewPartition);
    if (EFI_ERROR (EfiStatus)) {
      CpuHalt ("Unable to recover partition, failed to switch boot partition!\n");
    }
    ResetSystem (EfiResetCold);
  }

  //
  // No partition switch: recovery continues in this boot, so override the
  // boot mode to BOOT_ON_FLASH_UPDATE so Stage2 runs the FW update payload.
  //
  SetBootMode (BOOT_ON_FLASH_UPDATE);
}
