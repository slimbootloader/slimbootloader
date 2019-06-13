/** @file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOOT_GUARD_LIB_H_
#define _BOOT_GUARD_LIB_H_

///
/// The TPM category, TPM 1.2, TPM 2.0 and PTT are defined.
///
typedef enum {
  TpmNone = 0, ///< 0: No TPM device present on system
  dTpm12,      ///< 1: TPM 1.2 device present on system
  dTpm20,      ///< 2: TPM 2.0 device present on system
  Ptt,         ///< 3: PTT present on system
  TpmTypeMax   ///< 4: Unknown device
} TPM_TYPE;

///
/// Information related to Boot Guard Configuration.
///
typedef struct {
  BOOLEAN  VerifiedBoot;        ///< Report VerifiedBoot Boot setting in Boot Guard profile. 0: Disable; 1: Enable.
  BOOLEAN  MeasuredBoot;        ///< Report Measured Boot setting in Boot Guard profile. 0: Disable; 1: Enable.
  /**
  ByPassTpmInit is set to 1 if Boot Guard ACM does TPM initialization successfully.
  - 0: No TPM initialization happen in Boot Guard ACM.
  - 1: TPM initialization is done by Boot Guard ACM.
  **/
  BOOLEAN  BypassTpmInit;
  TPM_TYPE TpmType;             ///< Report what TPM device is available on system.
  BOOLEAN  BootGuardCapability; ///< Value is set to 1 if chipset is Boot Guard capable.
  /**
  Value is set to 1 if microcode failed to load Boot Guard ACM or ENF Shutdown path is taken by ME FW.
  - 0: BIOS TPM code continue with TPM initization based on MeasuredBoot.
  - 1: BIOS TPM code is not to do any futher TPM initization and extends.
  **/
  BOOLEAN  DisconnectAllTpms;
  /**
  It is indicated BIOS TPM code is not to create DetailPCR or AuthorityPCR event log if Sx resume type is S3,
  Deep-S3, or iFFS Resume.
  - 0: Create TPM event log if not Sx Resume Type.
  - 1: Bypass TPM Event Log if Sx Resume Type is identified.
  **/
  BOOLEAN  ByPassTpmEventLog;
} BOOT_GUARD_INFO;

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  );

/**
  Report platform specific  Boot Guard information.

  @param[out] *BootGuardInfo - Pointer to BootGuardInfo.
**/
VOID
GetBootGuardInfo (
  OUT BOOT_GUARD_INFO *BootGuardInfo
  );
#endif
