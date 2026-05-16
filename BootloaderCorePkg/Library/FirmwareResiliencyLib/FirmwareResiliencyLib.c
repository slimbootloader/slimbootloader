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
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/OsBootOptionGuid.h>
#include <FirmwareUpdateStatus.h>
#include <Register/HeciRegs.h>
#include <IndustryStandard/Pci22.h>
#include <PlatformBase.h>

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

/** Check HFSTS1/2 via HECI PCI config space in Stage1B.
    Triggers CSME capsule FWU (BIT16 + cold reset) on first corruption detection,
    or boots degraded if FWU already completed. **/
VOID
EFIAPI
CheckForMeCodeFailures (
  VOID
  )
{
  if (!IsMeCorrupt ()) {
    return;
  }

  // FWU already triggered on a prior boot — let the FWU payload proceed.
  if (WdtGetScratchpad (BIT16) != 0) {
    DEBUG ((DEBUG_INFO, "ME corrupt but CSME FWU already triggered - letting FWU payload proceed\n"));
    return;
  }

  // FWU completed but ME still corrupt — boot degraded, avoid infinite loop.
  if (GetFwuStateMachine () != FW_UPDATE_SM_INIT) {
    DEBUG ((DEBUG_WARN, "ME still corrupt after CSME FWU (SM=0x%02X) - booting degraded\n",
            GetFwuStateMachine ()));
    return;
  }

  // First detection — trigger CSME capsule FWU.
  DEBUG ((DEBUG_WARN, "ME corruption detected - triggering CSME capsule FWU and resetting\n"));
  WdtSetScratchpad (BIT16);
  ResetSystem (EfiResetCold);
}

/** Reads HFSTS1 and HFSTS2 from HECI PCI config space into caller-supplied pointers. Both are zeroed if HECI device is not present. **/
VOID
EFIAPI
GetMeHfsts (
  OUT UINT32  *Hfsts1,
  OUT UINT32  *Hfsts2
  )
{
  UINTN         HeciBase;
  UINT32        MeDeviceAddr;
  PLT_PCI_DEVICE MeDev;

  *Hfsts1 = 0;
  *Hfsts2 = 0;

  MeDeviceAddr = GetDeviceAddr (PlatformDeviceMe, 0);
  if (MeDeviceAddr == 0) {
    MeDev.PciBusNumber    = 0;
    MeDev.PciDeviceNumber = HECI_DEV;
  } else {
    CopyMem (&MeDev, &MeDeviceAddr, sizeof (UINT32));
  }

  HeciBase = PCI_LIB_ADDRESS (MeDev.PciBusNumber, MeDev.PciDeviceNumber, HECI_FUN, 0);
  if (PciRead16 (HeciBase + PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    return;  // HECI device not present
  }

  *Hfsts1 = PciRead32 (HeciBase + R_ME_HFS);
  *Hfsts2 = PciRead32 (HeciBase + R_ME_HFS_2);
}

/** Returns TRUE if HFSTS1/2 indicate ME recovery/FtBupLdFlr/FwUpdIpu; FALSE otherwise**/
BOOLEAN
EFIAPI
IsMeCorrupt (
  VOID
  )
{
  HECI_FWS_REGISTER      MeFwSts;
  HECI_GS_SHDW_REGISTER  MeFwSts2;

  GetMeHfsts (&MeFwSts.ul, &MeFwSts2.ul);

  if ((MeFwSts.ul == 0) && (MeFwSts2.ul == 0)) {
    return FALSE;  // HECI not present
  }

  if ((MeFwSts.r.CurrentState == ME_STATE_RECOVERY) ||
      (MeFwSts.r.FtBupLdFlr   == 1) ||
      (MeFwSts2.r.FwUpdIpu    == 1)) {
    DEBUG ((DEBUG_INFO, "ME corruption detected (HFSTS1=0x%08X HFSTS2=0x%08X)\n", MeFwSts.ul, MeFwSts2.ul));
    return TRUE;
  }

  return FALSE;
}
