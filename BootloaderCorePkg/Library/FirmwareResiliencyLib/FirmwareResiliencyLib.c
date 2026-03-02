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
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Register/HeciRegs.h>
#include <BootloaderCoreGlobal.h>
#include <FirmwareUpdateStatus.h>
#include <PlatformBase.h>

// OC_WDT register for FW update trigger (persists across reset)
#define R_ACPI_IO_OC_WDT_CTL   0x54
#define B_FW_UPDATE_TRIGGER    BIT16

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

/**
  Check for ME firmware code corruption.

  Reads HFSTS1/HFSTS2 registers and detects ME code corruption based on:
  - CurrentState == ME_STATE_RECOVERY (0x02)
  - FtBupLdFlr == 1 (Fault-Tolerant Bringup Load Failure)
  - FwUpdIpu == 1 (FW Update IPU Needed)

  If corruption detected, stores info in LdrGlobal and sets BOOT_ON_FLASH_UPDATE mode.

  @param[in,out] LdrGlobal   Pointer to loader global data

  @retval EFI_SUCCESS        ME is healthy
  @retval EFI_DEVICE_ERROR   ME corruption detected, recovery triggered
**/
VOID
EFIAPI
CheckForMeCodeFailures (
  IN OUT LOADER_GLOBAL_DATA  *LdrGlobal
  )
{
  HECI_FWS_REGISTER       MeFwSts;
  HECI_GS_SHDW_REGISTER   MeFwSts2;
  UINT32                  HeciBase;
  BOOLEAN                 Corrupted;

  DEBUG ((DEBUG_INFO, "[FW Resiliency] Checking ME firmware code health...\n"));

  // Read HECI registers (PCI 0:22:0)
  HeciBase = PCI_LIB_ADDRESS (0, HECI_DEV, HECI_FUN, 0);

  MeFwSts.ul  = PciRead32 (HeciBase + R_ME_HFS);    // HFSTS1 at offset 0x40
  MeFwSts2.ul = PciRead32 (HeciBase + 0x48);        // HFSTS2 at offset 0x48

  DEBUG ((DEBUG_INFO, "  HFSTS1 = 0x%08X\n", MeFwSts.ul));
  DEBUG ((DEBUG_INFO, "  HFSTS2 = 0x%08X\n", MeFwSts2.ul));

  // Check corruption indicators (BIOS-aligned logic)
  Corrupted = FALSE;

  if (MeFwSts.r.CurrentState == ME_STATE_RECOVERY) {
    DEBUG ((DEBUG_ERROR, "  ME in Recovery State (CurrentState = 0x%X)\n", MeFwSts.r.CurrentState));
    Corrupted = TRUE;
  }

  if (MeFwSts.r.FtBupLdFlr == 1) {
    DEBUG ((DEBUG_ERROR, "  FtBupLdFlr set - ME code load failure!\n"));
    Corrupted = TRUE;
  }

  if (MeFwSts2.r.FwUpdIpu == 1) {
    DEBUG ((DEBUG_ERROR, "  FwUpdIpu set - ME update needed!\n"));
    Corrupted = TRUE;
  }

  if (Corrupted) {
    UINT32  OcWdtValue;

    DEBUG ((DEBUG_ERROR, "\n"));
    DEBUG ((DEBUG_ERROR, "*** ME FIRMWARE CODE CORRUPTION DETECTED! ***\n"));
    DEBUG ((DEBUG_ERROR, "\n"));

    // Store recovery info in LdrGlobal for payload to access
    LdrGlobal->MeRecoveryInfo.Signature = ME_RECOVERY_SIGNATURE;
    LdrGlobal->MeRecoveryInfo.CorruptionType = ME_CORRUPTION_CODE;
    LdrGlobal->MeRecoveryInfo.Hfsts1 = MeFwSts.ul;
    LdrGlobal->MeRecoveryInfo.Hfsts2 = MeFwSts2.ul;

    // Check if recovery was already attempted using OC_WDT trigger bit
    OcWdtValue = IoRead32 (ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL);
    if (OcWdtValue & B_FW_UPDATE_TRIGGER) {
      DEBUG ((DEBUG_WARN, "Recovery already attempted (OC_WDT trigger set), booting degraded\n"));
      return;
    }

    // Set OC_WDT trigger to request firmware update
    OcWdtValue |= B_FW_UPDATE_TRIGGER;
    IoWrite32 (ACPI_BASE_ADDRESS + R_ACPI_IO_OC_WDT_CTL, OcWdtValue);
    DEBUG ((DEBUG_INFO, "OC_WDT trigger set for ME recovery\n"));
    return;
  }

  DEBUG ((DEBUG_INFO, "[FW Resiliency] ME Code Health: PASSED\n"));
}
