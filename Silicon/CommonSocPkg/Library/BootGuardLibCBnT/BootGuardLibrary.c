/** @file
  BootGuardLibrary implementation.

  Copyright (c) 2012 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


@par Specification Reference:
**/

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BootGuardLib.h>
#include <BootGuardRegister.h>
#include <Library/BootloaderCommonLib.h>

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
EFIAPI
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
  Report platform specific Boot Guard information.

  @param[out] *BootGuardInfo - Pointer to BootGuardInfo.
**/
VOID
EFIAPI
GetBootGuardInfo (
  IN  UINTN            HeciBaseAddress,
  OUT BOOT_GUARD_INFO *BootGuardInfo
  )
{
  UINT32                  MsrValue;
  UINT32                  MeFwSts4;
  UINT32                  BootGuardAcmStatus;
  UINT64                  BootGuardBootStatus;

  ///
  /// Check if System Supports Boot Guard
  ///
  if (IsBootGuardSupported ()) {
    BootGuardInfo->BootGuardCapability = TRUE;

    BootGuardAcmStatus  = *(UINT32 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
    DEBUG ((DEBUG_INFO, "Boot Guard ACM Status = %x\n", BootGuardAcmStatus));

    BootGuardBootStatus  = *(UINT64 *) (UINTN) (TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
    DEBUG ((DEBUG_INFO, "Boot Guard Boot Status = %llx\n", BootGuardBootStatus));

    ///
    /// Read ME FWS Registers
    ///
    MeFwSts4 = MmioRead32 (HeciBaseAddress + R_ME_HFS_4);
    DEBUG ((DEBUG_INFO, "ME FW STS 4 = %x\n", MeFwSts4));

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

    ///
    /// Check Bit 46 in BootGuardBootStatus to handle Tpm2Startup(Restore) errors.
    ///
    if ((BootGuardBootStatus & B_CPU_BOOT_GUARD_BOOTSTATUS_S3_TPM_STARTUP_FAILED) != 0) {
      BootGuardInfo->TpmStartupFailureOnS3 = TRUE;
      DEBUG ((DEBUG_INFO, "ACM Tpm2Startup (State) failure detected during S3 flow.\n"));
    }
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
