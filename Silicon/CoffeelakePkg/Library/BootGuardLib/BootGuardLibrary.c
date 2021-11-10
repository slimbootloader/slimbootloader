/** @file
  BootGuardLibrary implementation.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

@par Specification Reference:
**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BootGuardLib.h>
#include <RegAccess.h>
#include <MkhiMsgs.h>
#include <MeChipset.h>

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  )
{
  UINT64          BootGuardCapability;

  BootGuardCapability = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_CAPABILITY;


  if (BootGuardCapability != 0) {
    DEBUG ((DEBUG_INFO, "Processor supports Boot Guard.\n"));
    return TRUE;
  } else {
    DEBUG ((DEBUG_INFO, "Processor does not support Boot Guard.\n"));
    return FALSE;
  }
}

/**
  Report platform specific  Boot Guard information.

  @param[out] *BootGuardInfo - Pointer to BootGuardInfo.
**/
VOID
GetBootGuardInfo (
  OUT BOOT_GUARD_INFO *BootGuardInfo
  )
{
  UINT32                  MsrValue;
  UINT32                  MeFwSts2;
  UINT32                  MeFwSts4;
  UINT32                  MeFwSts5;
  UINT32                  MeFwSts6;
  UINTN                   HeciBaseAddress;
  UINT32                  BootGuardAcmStatus;
  UINT32                  BootGuardBootStatus;

  ///
  /// Check if System Supports Boot Guard
  ///
  if (IsBootGuardSupported()) {
    BootGuardInfo->BootGuardCapability = TRUE;

    BootGuardAcmStatus  = *(UINT32 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
    DEBUG ((DEBUG_INFO, "Boot Guard ACM Status = %x\n", BootGuardAcmStatus));

    BootGuardBootStatus  = *(UINT32 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
    DEBUG ((DEBUG_INFO, "Boot Guard Boot Status = %x\n", BootGuardBootStatus));

    ///
    /// Read ME FWS Registers
    ///
    HeciBaseAddress = MmPciBase (
                      ME_BUS,
                      ME_DEVICE_NUMBER,
                      HECI_FUNCTION_NUMBER
                      );
    MeFwSts2 = MmioRead32 (HeciBaseAddress + R_ME_GS_SHDW);
    DEBUG ((DEBUG_INFO, "ME FW STS 2 = %x\n", MeFwSts2));

    MeFwSts4 = MmioRead32 (HeciBaseAddress + R_ME_HFS_4);
    DEBUG ((DEBUG_INFO, "ME FW STS 4 = %x\n", MeFwSts4));

    MeFwSts5 = MmioRead32 (HeciBaseAddress + R_ME_HFS_5);
    DEBUG ((DEBUG_INFO, "ME FW STS 5 = %x\n", MeFwSts5));

    MeFwSts6 = MmioRead32 (HeciBaseAddress + R_ME_HFS_6);
    DEBUG ((DEBUG_INFO, "ME FW STS 6 = %x\n", MeFwSts6));

    ///
    /// Check Bit 12 in ME FWSTS4 to check if TPM_DISCONNECT_ALL bit is set
    /// or ENF Shutdown path is taken by ME FW.
    /// Also Identify any failures in ACM
    ///
    if (((MeFwSts4 & (B_TPM_DISCONNECT | B_BOOT_GUARD_ENF_MASK)) != 0) ||
        ((BootGuardAcmStatus & B_BOOT_GUARD_ACM_ERRORCODE_MASK) != 0)) {
      DEBUG ((DEBUG_INFO, "All TPM's on Platform are Disconnected\n"));
      BootGuardInfo->DisconnectAllTpms = TRUE;
    }
    BootGuardInfo->BypassTpmInit = FALSE;
    BootGuardInfo->MeasuredBoot = FALSE;
    BootGuardInfo->VerifiedBoot = FALSE;

    if ((MeFwSts4 & BIT10) != 0) {
      DEBUG ((DEBUG_INFO, "Sx Resume Type Identified - TPM Event Log not required for ACM Measurements\n"));
      BootGuardInfo->ByPassTpmEventLog = TRUE;
    }
    ///
    /// Check bit 0 of BOOT_GUARD_SACM_INFO MSR if system is in Boot Guard boot mode
    ///
    MsrValue = (UINT32) AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO);
    DEBUG ((DEBUG_INFO, "MSR_BOOT_GUARD_SACM_INFO MSR = %x\n", MsrValue));
    if ((MsrValue & B_BOOT_GUARD_SACM_INFO_NEM_ENABLED) == 0) {
      DEBUG ((DEBUG_INFO, "NEM is not initiated by Boot Guard ACM\n"));
    }
    if ((MsrValue & B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT) != 0) {
      BootGuardInfo->VerifiedBoot = TRUE;
    }
    if ((MsrValue & B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT) != 0) {
      BootGuardInfo->MeasuredBoot = TRUE;
      ///
      /// if measured bit is set, BIOS needs to bypass startup command
      ///
      if ((MsrValue & B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS) != 0) {
        BootGuardInfo->BypassTpmInit = TRUE;
      }
      ///
      /// Read present TPM type
      ///
      BootGuardInfo->TpmType = (TPM_TYPE) ( (MsrValue & V_TPM_PRESENT_MASK) >> 1 );
      DEBUG ((DEBUG_INFO, "TPM Type is %x\n", BootGuardInfo->TpmType));
    }
  } else {
    BootGuardInfo->BootGuardCapability = FALSE;
  }

  DEBUG ((DEBUG_INFO, "Boot Guard Support status: %x\n", BootGuardInfo->BootGuardCapability));

  return;

}

